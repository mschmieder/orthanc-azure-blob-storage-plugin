#ifndef AZURE_KEY_VAULT_ENCRYPTION_KEY_H
#define AZURE_KEY_VAULT_ENCRYPTION_KEY_H

#include "EncryptionKey.h"
#include <string>
#include <cpprest/details/basic_types.h>
#include <cpprest/json.h>

// forward declaration
namespace az {
    class AzureKeyVaultClient;
}

namespace az
{

  class AzureKeyVaultEncryptionKey : public crypto::EncryptionKey
  {
    public:
      AzureKeyVaultEncryptionKey(const az::AzureKeyVaultClient* client = nullptr,
                                 const utility::string_t& kid = "",
                                 const web::json::value& attributes = web::json::value());

      virtual ~AzureKeyVaultEncryptionKey();

      const web::json::value& attributes() const {
          return m_attributes;
      }

      const web::json::value& attributes(const utility::string_t& attribute) const {
          return m_attributes.at(attribute);
      }

      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const;
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const;
      virtual void unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const;

      virtual std::string metaData() const;

    private:
      utility::string_t m_kid;
      web::json::value m_attributes;

      const az::AzureKeyVaultClient* m_keyVaultClient;
  };

} // crypto

#endif // AZURE_KEY_VAULT_ENCRYPTION_KEY_H
