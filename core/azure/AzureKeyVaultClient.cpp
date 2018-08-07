#include "AzureKeyVaultClient.h"
#include "AzureKeyVaultException.h"

#include <string>
#include <vector>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace az;

/*
Base64 translates 24 bits into 4 ASCII characters at a time. First,
3 8-bit bytes are treated as 4 6-bit groups. Those 4 groups are
translated into ASCII characters. That is, each 6-bit number is treated
as an index into the ASCII character array.
If the final set of bits is less 8 or 16 instead of 24, traditional base64
would add a padding character. However, if the length of the data is
known, then padding can be eliminated.
One difference between the "standard" Base64 is two characters are different.
See RFC 4648 for details.
This is how we end up with the Base64 URL encoding.
*/

const char base64_url_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
};

std::string base64_encode(const std::string& in) {
  std::string out;
  int val =0, valb=-6;
  size_t len = in.length();
  unsigned int i = 0;
  for (i = 0; i < len; i++) {
    unsigned char c = in[i];
    val = (val<<8) + c;
    valb += 8;
    while (valb >= 0) {
      out.push_back(base64_url_alphabet[(val>>valb)&0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    out.push_back(base64_url_alphabet[((val<<8)>>(valb+8))&0x3F]);
  }
  return out;
}

std::string base64_decode(const std::string& in) {
  std::string out;
  std::vector<int> T(256, -1);
  unsigned int i;
  for (i =0; i < 64; i++) T[base64_url_alphabet[i]] = i;

  int val = 0, valb = -8;
  for (i = 0; i < in.length(); i++) {
    unsigned char c = in[i];
    if (T[c] == -1) break;
    val = (val<<6) + T[c];
    valb += 6;
    if (valb >= 0) {
      out.push_back(char((val>>valb)&0xFF));
      valb -= 8;
    }
  }
  return out;
}

AzureKeyVaultClient::AzureKeyVaultClient(const utility::string_t& baseUrl,
    const utility::string_t& apiVersion)
  : m_baseUrl(baseUrl),
    m_clientId(_XPLATSTR("")),
    m_clientSecret(_XPLATSTR("")),
    m_apiVersion(apiVersion),
    m_is_authenticated(false)
{

}

AzureKeyVaultClient::AzureKeyVaultClient(const utility::string_t& baseUrl,
                                         const utility::string_t& clientId,
                                         const utility::string_t& clientSecret,
                                         const utility::string_t& apiVersion)
  : m_baseUrl(baseUrl),
    m_clientId(clientId),
    m_clientSecret(clientSecret),
    m_apiVersion(apiVersion),
    m_is_authenticated(false)
{

}

utility::string_t AzureKeyVaultClient::readResponseBody( web::http::http_response response )
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
// helper to parse out https url in double quotes
utility::string_t AzureKeyVaultClient::extractUri( utility::string_t headerValue )
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
  return web::uri::encode_uri(headerValue);
}

bool AzureKeyVaultClient::authenticate()
{
    // make a un-auth'd request to KeyVault to get a response that contains url to IDP
    AuthEndpoint authEndpoint = this->getAuthEndpoint();

    // create the oauth2 authentication request and pass the clientId/Secret as app identifiers
    utility::string_t url = authEndpoint.getLoginUri() + _XPLATSTR("/oauth2/token");
    web::http::client::http_client client( url );
    utility::string_t postData = _XPLATSTR("resource=") +  authEndpoint.getAuthUri() + _XPLATSTR("&client_id=") + m_clientId
                                 + _XPLATSTR("&client_secret=") + m_clientSecret + _XPLATSTR("&grant_type=client_credentials");
    web::http::http_request request( web::http::methods::POST );
    request.headers().add(_XPLATSTR("Content-Type"), _XPLATSTR("application/x-www-form-urlencoded"));
    request.headers().add(_XPLATSTR("Accept"), _XPLATSTR("application/json"));
    request.headers().add(_XPLATSTR("return-client-request-id"), _XPLATSTR("true"));
    request.headers().add(_XPLATSTR("client-request-id"), createGuid() );
    request.set_body( postData );

    // response from IDP is a JWT Token that contains the token type and access token we need for
    // Azure HTTP REST API calls
    web::http::http_response response = this->handleRequest(url, request);

    auto status_code = response.status_code();
    if ( status_code == 200 )
    {
      utility::string_t target = readResponseBody( response );

      std::error_code err;
      web::json::value jwtToken = web::json::value::parse( target.c_str(), err );
      if ( err.value() == 0 )
      {
        this->setAccessToken(JwtToken(jwtToken[_XPLATSTR("token_type")].as_string(),
                                         jwtToken[_XPLATSTR("access_token")].as_string()));
      }
    }

    m_is_authenticated = (status_code == 200);
    return m_is_authenticated;
}

//////////////////////////////////////////////////////////////////////////////
// Make a HTTP POST to oauth2 IDP source to get JWT Token containing
// access token & token type
bool AzureKeyVaultClient::authenticate(const utility::string_t& clientId, const utility::string_t& clientSecret)
{ 
  m_clientId = clientId;
  m_clientSecret = clientSecret ;

  return this->authenticate();
}
//////////////////////////////////////////////////////////////////////////////
// Make a HTTP Get to Azure KeyVault unauthorized which gets us a response
// where the header contains the url of IDP to be used
AuthEndpoint AzureKeyVaultClient::getAuthEndpoint()
{
  utility::string_t url = this->getBaseUrl() + _XPLATSTR("/secrets/secretname?api-version=") + this->getApiVersion();
  web::http::client::http_client client( url );

  return client.request( web::http::methods::GET ).then([]( web::http::http_response response )
  {
    AuthEndpoint endpoint;

    auto status_code = response.status_code();
    if ( status_code == 401 ) // unothorized
    {
      utility::string_t authUri;
      utility::string_t loginUri;

      web::http::http_headers& headers = response.headers();
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
          loginUri = AzureKeyVaultClient::extractUri( part );
        }
        start = end + delimiter.length();
        end = wwwAuth.find(delimiter, start);
        count++;
      }
      utility::string_t part = wwwAuth.substr(start, end - start);
      authUri = web::uri::encode_uri(AzureKeyVaultClient::extractUri( part ));
      endpoint = AuthEndpoint(loginUri, authUri);
    }
    return endpoint;
  }).get();
}

utility::string_t AzureKeyVaultClient::createGuid()
{
    boost::uuids::random_generator generator;
    return boost::uuids::to_string(generator());
}

web::http::http_response AzureKeyVaultClient::executeKeyOperation(const utility::string_t& op,
    const web::http::method method,
    const web::json::value& body,
    const utility::string_t& kid,
    const JwtToken& accessToken,
    const utility::string_t& apiVersion)
{
  // create the url path to query the keyvault secret
  utility::string_t url = kid + _XPLATSTR("/") + op + _XPLATSTR("?api-version=") + apiVersion;
  web::http::http_request request( method );
  request.headers().add(_XPLATSTR("Content-Type"), _XPLATSTR("application/json"));
  // add access token we got from authentication step
  request.headers().add(_XPLATSTR("Authorization"), accessToken.type() + _XPLATSTR(" ") + accessToken.value() );

  request.set_body(body);

  // Azure HTTP REST API call
  return this->handleRequest(url, request);
}

utility::string_t AzureKeyVaultClient::wrapKey(const utility::string_t& data,
    const utility::string_t& kid,
    bool cvtToBase64,
    const utility::string_t& alg)
{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);

  if (cvtToBase64)
  {
    request_body[U("value")] = web::json::value::string(base64_encode(data));
  }
  else
  {
    request_body[U("value")] = web::json::value::string(data);
  }

  web::http::http_response response = this->executeKeyOperation("wrapkey",
                                      web::http::methods::POST,
                                      request_body, kid,
                                      this->getAccessToken(),
                                      this->getApiVersion() );

  handleHttpResponse(response);

  auto status_code = response.status_code();
  auto json_response = response.extract_json().get();
  if (status_code != 200)
  {
    // TODO throw exception
    std::stringstream stream;
    json_response.serialize(stream);

    return stream.str();
  }

  return json_response[U("value")].as_string();
}

utility::string_t AzureKeyVaultClient::unwrapKey(const utility::string_t& data,
    const utility::string_t& kid,
    const utility::string_t& alg)

{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);
  std::vector<uint8_t> vecData(data.begin(), data.end());
  request_body[U("value")] = web::json::value::string(data);

  web::http::http_response response = this->executeKeyOperation("unwrapkey",
                                      web::http::methods::POST,
                                      request_body, kid,
                                      this->getAccessToken(),
                                      this->getApiVersion() );

  handleHttpResponse(response);

  auto status_code = response.status_code();
  auto json_response = response.extract_json().get();

  if (status_code != 200)
  {
    // TODO throw exception
    std::stringstream stream;
    json_response.serialize(stream);

    return stream.str();
  }

  return base64_decode(json_response[U("value")].as_string());
}

utility::string_t AzureKeyVaultClient::encrypt(const utility::string_t& data,
    const utility::string_t& kid,
    bool cvtToBase64,
    const utility::string_t& alg)
{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);

  if (cvtToBase64)
  {
    request_body[U("value")] = web::json::value::string(base64_encode(data));
  }
  else
  {
    request_body[U("value")] = web::json::value::string(data);
  }

  web::http::http_response response = this->executeKeyOperation("encrypt",
                                      web::http::methods::POST,
                                      request_body, kid,
                                      this->getAccessToken(),
                                      this->getApiVersion() );

  handleHttpResponse(response);

  auto status_code = response.status_code();
  auto json_response = response.extract_json().get();
  if (status_code != 200)
  {
    // TODO throw exception
    std::stringstream stream;
    json_response.serialize(stream);

    return stream.str();
  }

  return json_response[U("value")].as_string();
}

utility::string_t AzureKeyVaultClient::decrypt(const utility::string_t& data,
    const utility::string_t& kid,
    const utility::string_t& alg)

{
  web::json::value request_body;
  request_body[U("alg")] = web::json::value::string(alg);
  std::vector<uint8_t> vecData(data.begin(), data.end());
  request_body[U("value")] = web::json::value::string(data);


  web::http::http_response response = this->executeKeyOperation("decrypt",
                                      web::http::methods::POST,
                                      request_body, kid,
                                      this->getAccessToken(),
                                      this->getApiVersion() );

  handleHttpResponse(response);

  auto status_code = response.status_code();
  auto json_response = response.extract_json().get();

  if (status_code != 200)
  {
    // TODO throw exception
    std::stringstream stream;
    json_response.serialize(stream);

    return stream.str();
  }

  return base64_decode(json_response[U("value")].as_string());
}

std::vector<AzureKeyVaultEncryptionKey> AzureKeyVaultClient::getKeys()
{

    utility::string_t url = this->getBaseUrl() + _XPLATSTR("/keys") +
                            _XPLATSTR("?api-version=") + this->getApiVersion();

    web::http::client::http_client client( url );
    web::http::http_request request( web::http::methods::GET );

    // add access token we got from authentication step
    request.headers().add(_XPLATSTR("Authorization"), this->getAccessToken().type() + _XPLATSTR(" ") + this->getAccessToken().value() );

    // Azure HTTP REST API call
    web::http::http_response response = this->handleRequest(url, request);
    std::vector<AzureKeyVaultEncryptionKey> keys;
    handleHttpResponse(response);
    
    if(response.status_code() == 200){
        web::json::value json_response = response.extract_json().get();

        web::json::array key_array = json_response[U("value")].as_array();
        for(auto it = key_array.cbegin(); it != key_array.cend(); ++it)
        {
           utility::string_t kid = it->at(U("kid")).as_string();

           keys.push_back(this->getKey(kid));
        }
    }

    return keys;
}

AzureKeyVaultEncryptionKey AzureKeyVaultClient::getKey(const utility::string_t& kid)
{
    AzureKeyVaultEncryptionKey key;

    utility::string_t url = kid + _XPLATSTR("?api-version=") + this->getApiVersion();

    web::http::client::http_client client( url );
    web::http::http_request request( web::http::methods::GET );

    // add access token we got from authentication step
    request.headers().add(_XPLATSTR("Authorization"), this->getAccessToken().type() + _XPLATSTR(" ") + this->getAccessToken().value() );

    // Azure HTTP REST API call
    web::http::http_response response = this->handleRequest(url, request);

    // will throw exception if something goes wrong
    handleHttpResponse(response);
    
    if(response.status_code() == 200){
        web::json::value attributes = response.extract_json().get();
        key = AzureKeyVaultEncryptionKey(this,
                                         attributes[U("key")][U("kid")].as_string(),
                                         attributes);
    }

    return key;
}

 AzureKeyVaultEncryptionKey AzureKeyVaultClient::getKeyByName(const utility::string_t& keyName, const utility::string_t& keyVersion)
 {
    utility::string_t kid = this->getBaseUrl() +
                            _XPLATSTR("/keys/") + keyName;

    if(keyVersion.size() > 0){
        kid += _XPLATSTR("/") + keyVersion;
    }

    return this->getKey(kid);
 }

pplx::task<web::http::http_response> postRequest(const utility::string_t& url, web::http::http_request request) 
{
    web::http::client::http_client client( url );
    return client.request(request);  
}

web::http::http_response AzureKeyVaultClient::handleRequest(const utility::string_t& url, web::http::http_request request)
{
  auto response = postRequest(url, request).get();

  if(401 == response.status_code()){
    // if unauthorized, try to authenticate again
    if (! this->authenticate() ){
      throw AzureKeyVaultUnauthorizedException(response.extract_string().get(), response);
    }
    request.headers().remove("Authorization");
    request.headers().add(_XPLATSTR("Authorization"), this->getAccessToken().type() + _XPLATSTR(" ") + this->getAccessToken().value() );
    response = postRequest(url, request).get();
  }

  return response;
}

void AzureKeyVaultClient::handleHttpResponse(web::http::http_response response)
{
  switch (response.status_code())
  {
      case 200: // ok
        break;
      case 401: {
        throw AzureKeyVaultUnauthorizedException(response.extract_string().get(), response);
        break;
      }
      default:
          throw AzureKeyVaultException(response.extract_string().get(), response);
  }
}
