#ifndef AZURE_KEY_VAULT_CLIENT_H
#define AZURE_KEY_VAULT_CLIENT_H

#include <cpprest/http_client.h>

namespace az
{
    class AccessToken {
    public:
        AccessToken(const utility::string_t& tokenType = "", const utility::string_t& token = "")
            : m_tokenType(tokenType), m_token(token){}

        ~AccessToken(){}

        utility::string_t value() const {
            return m_token;
        }
        utility::string_t type() const {
            return m_tokenType;
        }

    private:
        utility::string_t m_tokenType;
        utility::string_t m_token;
    };

  class AzureKeyVaultClient
  {
    public:
      AzureKeyVaultClient(const utility::string_t& baseUrl,
                          const utility::string_t& apiVersion = "2016-10-01");

      utility::string_t wrapKey(const utility::string_t& data,
                                            const utility::string_t& keyName,
                                            const utility::string_t& keyVersion,
                                            bool cvtToBase64 = true,
                                            const utility::string_t& alg = U("RSA-OAEP-256"));

      utility::string_t unwrapKey(const utility::string_t& data,
                                              const utility::string_t& keyName,
                                              const utility::string_t& keyVersion,
                                              const utility::string_t& alg = U("RSA-OAEP-256"));

      utility::string_t encrypt(const utility::string_t& data,
                                            const utility::string_t& keyName,
                                            const utility::string_t& keyVersion,
                                            bool cvtToBase64 = true,
                                            const utility::string_t& alg = U("RSA-OAEP-256"));

      utility::string_t decrypt(const utility::string_t& data,
                                              const utility::string_t& keyName,
                                              const utility::string_t& keyVersion,
                                              const utility::string_t& alg = U("RSA-OAEP-256"));

      pplx::task<void> Authenticate( const utility::string_t& clientId,
                                     const utility::string_t& clientSecret,
                                     const utility::string_t& keyVaultName );

      bool GetSecretValue( utility::string_t secretName, web::json::value& secret );


      utility::string_t getBaseUrl() const {
          return m_baseUrl;
      }

      utility::string_t getApiVersion() const {
          return m_apiVersion;
      }

      void setAccessToken(const AccessToken& token){
          m_accessToken = token;
      }

      const AccessToken& getAccessToken(){
          return m_accessToken;
      }

  private:
      utility::string_t get_https_url( utility::string_t headerValue );
      pplx::task<void> GetLoginUrl();
      pplx::task<void> get_secret( utility::string_t secretName );

      utility::string_t read_response_body( web::http::http_response response );


      utility::string_t NewGuid();

      utility::string_t tokenType;
      utility::string_t accessToken;
      utility::string_t keyVaultUrl;
      utility::string_t loginUrl;
      utility::string_t resourceUrl;
      utility::string_t keyVaultName;
      utility::string_t keyVaultRegion;

      utility::string_t m_baseUrl;
      utility::string_t m_apiVersion;

    private:
      AccessToken m_accessToken;
      int status_code;
      web::json::value secret;
  };
} // az

#endif // AZURE_KEY_VAULT_CLIENT_H


