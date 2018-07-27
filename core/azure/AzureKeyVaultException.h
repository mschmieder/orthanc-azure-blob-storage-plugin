#ifndef AZURE_KEY_VAULT_EXCEPTION_H
# define AZURE_KEY_VAULT_EXCEPTION_H

#include <cpprest/http_msg.h>

#include <stdexcept>
#include <string>

namespace az
{
  class AzureKeyVaultException : public std::runtime_error
  {
    public:
      AzureKeyVaultException(const std::string& message, web::http::http_response response)
        : std::runtime_error("Error in AzureBlobStorage: " + message),
        m_response(response)

        {

        }

        virtual const web::http::http_response& http_response() const {
          return m_response;
        }

        virtual size_t status_code() const {
          return m_response.status_code();
        }

    private:
      web::http::http_response m_response;
  };


  class AzureKeyVaultUnauthorizedException : public AzureKeyVaultException {
    public:
      AzureKeyVaultUnauthorizedException(const std::string& message, web::http::http_response response)
      : AzureKeyVaultException(message, response)
      {

      }
  };

}


#endif // AZURE_KEY_VAULT_EXCEPTION_H
