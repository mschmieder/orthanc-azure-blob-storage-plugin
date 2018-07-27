#ifndef AZURE_KEY_VAULT_CLIENT_H
#define AZURE_KEY_VAULT_CLIENT_H

#include <vector>
#include "AzureKeyVaultEncryptionKey.h"
#include "azure/AuthEndpoint.h"
#include "azure/JwtToken.h"

#include <cpprest/http_client.h>

namespace az
{
  class AzureKeyVaultClient
  {
    public:
      /**
       * @brief      Constructs the object.
       *
       * @param[in]  baseUrl     The base url
       * @param[in]  apiVersion  The api version
       */
      AzureKeyVaultClient(const utility::string_t& baseUrl,
                          const utility::string_t& apiVersion = "2016-10-01");

      /**
       * @brief      Constructs the object.
       *
       * @param[in]  baseUrl       The base url
       * @param[in]  clientId      The client identifier
       * @param[in]  clientSecret  The client secret
       * @param[in]  apiVersion    The api version
       */
      AzureKeyVaultClient(const utility::string_t& baseUrl,
                          const utility::string_t& clientId,
                          const utility::string_t& clientSecret,
                          const utility::string_t& apiVersion = "2016-10-01");

      /**
       * @brief      encrypt a key
       *
       * @param[in]  data         The data
       * @param[in]  k            { parameter_description }
       * @param[in]  cvtToBase64  The cvt to base 64
       * @param[in]  alg          The alg
       *
       * @return     encrypted key
       */
      utility::string_t wrapKey(const utility::string_t& data,
                                const utility::string_t& kíd,
                                bool cvtToBase64 = true,
                                const utility::string_t& alg = U("RSA-OAEP-256")) const;

      /**
       * @brief      decrypt a key
       *
       * @param[in]  data  The data
       * @param[in]  kid   The kid
       * @param[in]  alg   The alg
       *
       * @return     decrypted key
       */
      utility::string_t unwrapKey(const utility::string_t& data,
                                  const utility::string_t& kid,
                                  const utility::string_t& alg = U("RSA-OAEP-256")) const;

      /**
       * @brief      encrypt data
       *
       * @param[in]  data         The data
       * @param[in]  kid          The kid
       * @param[in]  cvtToBase64  The cvt to base 64
       * @param[in]  alg          The alg
       *
       * @return    encrypted data
       */
      utility::string_t encrypt(const utility::string_t& data,
                                const utility::string_t& kid,
                                bool cvtToBase64 = true,
                                const utility::string_t& alg = U("RSA-OAEP-256")) const;


      /**
       * @brief      decrypt data
       *
       * @param[in]  data  The data
       * @param[in]  kid   The kid
       * @param[in]  alg   The alg
       *
       * @return     decrypted data
       */
      utility::string_t decrypt(const utility::string_t& data,
                                const utility::string_t& kid,
                                const utility::string_t& alg = U("RSA-OAEP-256")) const;


      /**
       * @brief      authenticate on key vault
       *
       * @param[in]  clientId      The client identifier
       * @param[in]  clientSecret  The client secret
       *
       * @return     success
       */
      bool authenticate( const utility::string_t& clientId,
                         const utility::string_t& clientSecret);

      /**
       * @brief      authenticate on key vault
       *
       * @return     success
       */
      bool authenticate();

      /**
       * @brief      retrieve encryption keys
       *
       * @return     vector containing keys
       */
      std::vector<AzureKeyVaultEncryptionKey> keys() const;

      /**
       * @brief      Gets the key.
       *
       * @param[in]  kid      The kid
       * @param[in]  version  The version
       *
       * @return     The key.
       */
      AzureKeyVaultEncryptionKey getKey(const utility::string_t& kid) const;
      
      /**
       * @brief      Gets the key by name.
       *
       * @param[in]  keyName     The key name
       * @param[in]  keyVersion  The key version
       *
       * @return     The key by name.
       */
      AzureKeyVaultEncryptionKey getKeyByName(const utility::string_t& keyName, const utility::string_t& keyVersion = U("")) const;

      /**
       * @brief      Determines if authenticated.
       *
       * @return     True if authenticated, False otherwise.
       */
      bool isAuthenticated() const
      {
        return m_is_authenticated;
      }

      /**
       * @brief      Gets the base url.
       *
       * @return     The base url.
       */
      utility::string_t getBaseUrl() const
      {
        return m_baseUrl;
      }

      /**
       * @brief      Gets the api version.
       *
       * @return     The api version.
       */
      utility::string_t getApiVersion() const
      {
        return m_apiVersion;
      }

      /**
       * @brief      Sets the access token.
       *
       * @param[in]  token  The token
       */
      void setAccessToken(const JwtToken& token)
      {
        m_accessToken = token;
      }

      /**
       * @brief      Gets the access token.
       *
       * @return     The access token.
       */
      const JwtToken& getAccessToken() const
      {
        return m_accessToken;
      }

    private:
      /**
       * @brief      extracts the URI from a given header value
       *
       * @param[in]  headerValue  The header value
       *
       * @return     URI
       */
      static utility::string_t extractUri( utility::string_t headerValue );

      /**
       * @brief      Reads a response body.
       *
       * @param[in]  response  The response
       *
       * @return     returns the response body as string
       */
      static utility::string_t readResponseBody( web::http::http_response response );

      /**
       * @brief      Creates an unique identifier.
       *
       * @return     unique id
       */
      static utility::string_t createGuid();

      /**
       * @brief      Gets the auth endpoint.
       *
       * @return     The auth endpoint.
       */
      AuthEndpoint getAuthEndpoint();


      /**
       * @brief      handling of different return codes. 
       *             Will throw execptions if return code is other than 200
       *
       * @param[in]  response  The response
       */
      static void handleHttpResponse(web::http::http_response response);


      utility::string_t m_baseUrl;
      utility::string_t m_clientId;
      utility::string_t m_clientSecret;
      utility::string_t m_apiVersion;

      JwtToken m_accessToken;
      bool m_is_authenticated;
  };
} // az

#endif // AZURE_KEY_VAULT_CLIENT_H


