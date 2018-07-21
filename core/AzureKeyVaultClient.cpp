#include "AzureKeyVaultClient.h"

// For UUID generation
extern "C"
{
#ifdef WIN32
#include <rpc.h>
#else
#include <uuid/uuid.h>
#endif
}

using namespace az;

AzureKeyVaultClient::AzureKeyVaultClient(const utility::string_t& baseUrl,
    const utility::string_t& apiVersion)
    : m_baseUrl(baseUrl),
      m_apiVersion(apiVersion)
{

}

//////////////////////////////////////////////////////////////////////////////
//
utility::string_t AzureKeyVaultClient::read_response_body( web::http::http_response response )
{
  auto bodyStream = response.body();
  concurrency::streams::stringstreambuf sb;
  auto& target = sb.collection();
  bodyStream.read_to_end(sb).get();
#ifdef _WIN32 // Windows uses UNICODE but result is in UTF8, so we need to convert it
  utility::string_t wtarget;
  wtarget.assign( target.begin(), target.end() );
  return wtarget;
#else
  return target;
#endif
}
//////////////////////////////////////////////////////////////////////////////
// Call Azure KeyVault REST API to retrieve a secret
bool AzureKeyVaultClient::GetSecretValue( utility::string_t secretName, web::json::value& secret )
{
  get_secret( secretName ).wait();
  secret = this->secret;
  return this->status_code == 200;
}

pplx::task<void> AzureKeyVaultClient::get_secret( utility::string_t secretName )
{
  auto impl = this;
  // create the url path to query the keyvault secret
  utility::string_t url = _XPLATSTR("https://") + impl->keyVaultName + _XPLATSTR(".vault.azure.net/secrets/") + secretName + _XPLATSTR("?api-version=2015-06-01");
  web::http::client::http_client client( url );
  web::http::http_request request( web::http::methods::GET );
  request.headers().add(_XPLATSTR("Accept"), _XPLATSTR("application/json"));
  request.headers().add(_XPLATSTR("client-request-id"), NewGuid() );
  // add access token we got from authentication step
  request.headers().add(_XPLATSTR("Authorization"), impl->tokenType + _XPLATSTR(" ") + impl->accessToken );
  // Azure HTTP REST API call
  return client.request(request).then([impl](web::http::http_response response)
  {
    std::error_code err;
    impl->status_code = response.status_code();
    if ( impl->status_code == 200 )
    {
      utility::string_t target = impl->read_response_body( response );
      impl->secret = web::json::value::parse( target.c_str(), err );
    }
    else
    {
      impl->secret = web::json::value::parse( _XPLATSTR("{\"id\":\"\",\"value\":\"\"}"), err );
    }
  });
}

//////////////////////////////////////////////////////////////////////////////
// helper to parse out https url in double quotes
utility::string_t AzureKeyVaultClient::get_https_url( utility::string_t headerValue )
{
  size_t pos1 = headerValue.find(_XPLATSTR("https://"));
  if ( pos1 != utility::string_t::npos )
  {
    size_t pos2 = headerValue.find(_XPLATSTR("\""), pos1 + 1);
    if ( pos2 > pos1 )
    {
      utility::string_t url = headerValue.substr(pos1, pos2 - pos1);
      headerValue = url;
    }
    else
    {
      utility::string_t url = headerValue.substr(pos1);
      headerValue = url;
    }
  }
  return headerValue;
}
//////////////////////////////////////////////////////////////////////////////
// Make a HTTP POST to oauth2 IDP source to get JWT Token containing
// access token & token type
pplx::task<void> AzureKeyVaultClient::Authenticate(const utility::string_t& clientId, const utility::string_t& clientSecret, const utility::string_t& keyVaultName )
{
  auto impl = this;
  impl->keyVaultName = keyVaultName;

  // make a un-auth'd request to KeyVault to get a response that contains url to IDP
  impl->GetLoginUrl().wait();

  // create the oauth2 authentication request and pass the clientId/Secret as app identifiers
  utility::string_t url = impl->loginUrl + _XPLATSTR("/oauth2/token");
  web::http::client::http_client client( url );
  utility::string_t postData = _XPLATSTR("resource=") + web::uri::encode_uri( impl->resourceUrl ) + _XPLATSTR("&client_id=") + clientId
                               + _XPLATSTR("&client_secret=") + clientSecret + _XPLATSTR("&grant_type=client_credentials");
  web::http::http_request request( web::http::methods::POST );
  request.headers().add(_XPLATSTR("Content-Type"), _XPLATSTR("application/x-www-form-urlencoded"));
  request.headers().add(_XPLATSTR("Accept"), _XPLATSTR("application/json"));
  request.headers().add(_XPLATSTR("return-client-request-id"), _XPLATSTR("true"));
  request.headers().add(_XPLATSTR("client-request-id"), NewGuid() );
  request.set_body( postData );

  // response from IDP is a JWT Token that contains the token type and access token we need for
  // Azure HTTP REST API calls
  return client.request(request).then([impl]( web::http::http_response response )
  {
    impl->status_code = response.status_code();
    if ( impl->status_code == 200 )
    {
      utility::string_t target = impl->read_response_body( response );
      std::error_code err;
      web::json::value jwtToken = web::json::value::parse( target.c_str(), err );
      if ( err.value() == 0 )
      {
        impl->setAccessToken(AccessToken(jwtToken[_XPLATSTR("token_type")].as_string(),
                             jwtToken[_XPLATSTR("access_token")].as_string()));

        impl->tokenType = jwtToken[_XPLATSTR("token_type")].as_string();
        impl->accessToken = jwtToken[_XPLATSTR("access_token")].as_string();
      }
    }
  });
}
//////////////////////////////////////////////////////////////////////////////
// Make a HTTP Get to Azure KeyVault unauthorized which gets us a response
// where the header contains the url of IDP to be used
pplx::task<void> AzureKeyVaultClient::GetLoginUrl()
{
  auto impl = this;
  utility::string_t url = _XPLATSTR("https://") + impl->keyVaultName + _XPLATSTR(".vault.azure.net/secrets/secretname?api-version=2015-06-01");
  web::http::client::http_client client( url );
  return client.request( web::http::methods::GET ).then([impl]( web::http::http_response response )
  {
    impl->status_code = response.status_code();
    if ( impl->status_code == 401 )
    {
      web::http::http_headers& headers = response.headers();
      impl->keyVaultRegion = headers[_XPLATSTR("x-ms-keyvault-region")];
      const utility::string_t& wwwAuth = headers[_XPLATSTR("WWW-Authenticate")];
      // parse WWW-Authenticate header into url links. Format:
      // Bearer authenticate="url", resource="url"
      utility::string_t delimiter = _XPLATSTR(" ");
      size_t count = 0, start = 0, end = wwwAuth.find(delimiter);
      while (end != utility::string_t::npos)
      {
        utility::string_t part = wwwAuth.substr(start, end - start);
        if ( count == 1 )
        {
          impl->loginUrl = impl->get_https_url( part );
        }
        start = end + delimiter.length();
        end = wwwAuth.find(delimiter, start);
        count++;
      }
      utility::string_t part = wwwAuth.substr(start, end - start);
      impl->resourceUrl = impl->get_https_url( part );
    }
  });
}

utility::string_t AzureKeyVaultClient::NewGuid()
{
#ifdef WIN32
  UUID uuid;
  UuidCreate ( &uuid );

  unsigned char * str;
  UuidToStringA ( &uuid, &str );

  std::string s( ( char* ) str );

  RpcStringFreeA ( &str );
#else
  uuid_t uuid;
  uuid_generate_random ( uuid );
  char s[37];
  uuid_unparse ( uuid, s );
#endif
  return s;
}

pplx::task<web::http::http_response> key_operation(const utility::string_t& op,
                                                   const web::http::method method,
                                                   const utility::string_t& baseUrl,
                                                   const web::json::value& body,
                                                   const utility::string_t& keyName,
                                                   const utility::string_t& keyVersion,
                                                   const AccessToken& accessToken,
                                                   const utility::string_t& apiVersion)
{
    // create the url path to query the keyvault secret
    utility::string_t url = baseUrl + _XPLATSTR("/keys/") +
                            keyName + _XPLATSTR("/") +
                            keyVersion + _XPLATSTR("/") +
                            op + _XPLATSTR("?api-version=")+apiVersion;


    std::cout<<url<<std::endl;

    web::http::client::http_client client( url );
    web::http::http_request request( method );
    request.headers().add(_XPLATSTR("Content-Type"), _XPLATSTR("application/json"));
    // add access token we got from authentication step
    request.headers().add(_XPLATSTR("Authorization"), accessToken.type() + _XPLATSTR(" ") + accessToken.value() );

    request.set_body(body);

    // Azure HTTP REST API call
    return client.request(request);
}

utility::string_t AzureKeyVaultClient::wrapKey(const utility::string_t& data,
    const utility::string_t& keyName,
    const utility::string_t& keyVersion,
    bool cvtToBase64,
    const utility::string_t& alg)
{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);

  if(cvtToBase64){
      std::vector<uint8_t> vecData(data.begin(), data.end());
      request_body[U("value")] = web::json::value::string(utility::conversions::to_base64(vecData));
  }
  else{
      request_body[U("value")] = web::json::value::string(data);
  }

  web::http::http_response response = key_operation("wrapkey",
                                                    web::http::methods::POST,
                                                    this->getBaseUrl(),
                                                    request_body, keyName, keyVersion,
                                                    this->getAccessToken(),
                                                    this->getApiVersion() ).get();

    auto status_code = response.status_code();
    auto json_response = response.extract_json().get();
    if(status_code != 200){
        // TODO throw exception
        std::stringstream stream;
        json_response.serialize(stream);

        return stream.str();
    }


   return json_response[U("value")].as_string();

}

utility::string_t AzureKeyVaultClient::unwrapKey(const utility::string_t& data,
    const utility::string_t& keyName,
    const utility::string_t& keyVersion,
    const utility::string_t& alg)

{
    web::json::value request_body;
    request_body[U("alg")] = web::json::value::string(alg);
    std::vector<uint8_t> vecData(data.begin(), data.end());
    request_body[U("value")] = web::json::value::string(data);


    web::http::http_response response = key_operation("unwrapkey",
                                                      web::http::methods::POST,
                                                      this->getBaseUrl(),
                                                      request_body, keyName, keyVersion,
                                                      this->getAccessToken(),
                                                      this->getApiVersion() ).get();

      auto status_code = response.status_code();
      auto json_response = response.extract_json().get();

      if(status_code != 200){
          // TODO throw exception
          std::stringstream stream;
          json_response.serialize(stream);

          return stream.str();
      }

     auto vec = utility::conversions::from_base64(json_response[U("value")].as_string());

     return utility::string_t(vec.begin(), vec.end());
}

utility::string_t AzureKeyVaultClient::encrypt(const utility::string_t& data,
    const utility::string_t& keyName,
    const utility::string_t& keyVersion,
    bool cvtToBase64,
    const utility::string_t& alg)
{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);

  if(cvtToBase64){
      std::vector<uint8_t> vecData(data.begin(), data.end());
      request_body[U("value")] = web::json::value::string(utility::conversions::to_base64(vecData));
  }
  else{
      request_body[U("value")] = web::json::value::string(data);
  }

  web::http::http_response response = key_operation("encrypt",
                                                    web::http::methods::POST,
                                                    this->getBaseUrl(),
                                                    request_body, keyName, keyVersion,
                                                    this->getAccessToken(),
                                                    this->getApiVersion() ).get();

    auto status_code = response.status_code();
    auto json_response = response.extract_json().get();
    if(status_code != 200){
        // TODO throw exception
        std::stringstream stream;
        json_response.serialize(stream);

        return stream.str();
    }


   return json_response[U("value")].as_string();

}

utility::string_t AzureKeyVaultClient::decrypt(const utility::string_t& data,
    const utility::string_t& keyName,
    const utility::string_t& keyVersion,
    const utility::string_t& alg)

{
    web::json::value request_body;
    request_body[U("alg")] = web::json::value::string(alg);
    std::vector<uint8_t> vecData(data.begin(), data.end());
    request_body[U("value")] = web::json::value::string(data);


    web::http::http_response response = key_operation("decrypt",
                                                      web::http::methods::POST,
                                                      this->getBaseUrl(),
                                                      request_body, keyName, keyVersion,
                                                      this->getAccessToken(),
                                                      this->getApiVersion() ).get();

      auto status_code = response.status_code();
      auto json_response = response.extract_json().get();

      if(status_code != 200){
          // TODO throw exception
          std::stringstream stream;
          json_response.serialize(stream);

          return stream.str();
      }

     auto vec = utility::conversions::from_base64(json_response[U("value")].as_string());

     return utility::string_t(vec.begin(), vec.end());
}
