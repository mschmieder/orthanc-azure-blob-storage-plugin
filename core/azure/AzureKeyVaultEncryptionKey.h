#ifndef AZURE_KEY_VAULT_ENCRYPTION_KEY_H
#define AZURE_KEY_VAULT_ENCRYPTION_KEY_H

#include "EncryptionKey.h"
#include <string>
#include <cpprest/details/basic_types.h>
#include <cpprest/json.h>


namespace az
{
  // forward declaration
  class AzureKeyVaultClient;

  /**
   * @brief      Class for azure key vault encryption key.
   */
  class AzureKeyVaultEncryptionKey : public crypto::EncryptionKey
  {
    public:
      /**
       * @brief      { function_description }
       *
       * @param[in]  client      The client
       * @param[in]  kid         The kid
       * @param[in]  attributes  The attributes
       */
      AzureKeyVaultEncryptionKey(const az::AzureKeyVaultClient* client = nullptr,
                                 const utility::string_t& kid = "",
                                 const web::json::value& attributes = web::json::value());

      /**
       * @brief      Destroys the object.
       */
      virtual ~AzureKeyVaultEncryptionKey();

      /**
       * @brief      key attributes
       *
       * @return     attributes
       */
      const web::json::value& attributes() const
      {
        return m_attributes;
      }

      /**
       * @brief      attribute
       *
       * @param[in]  attribute  The attribute
       *
       * @return     json value
       */
      const web::json::value& attribute(const utility::string_t& attributeName) const
      {
        return m_attributes.at(attributeName);
      }


      /**
      * @brief      wraps the data into an encrypted array
      *
      * @param[in]  unencryptedData  The unencrypted data
      * @param[in]  size             The size
      *
      * @return     encrypted data
      */
      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const;

      /**
       * @brief      unwraps the encrypted data
       *
       * @param[in]  data  The data
       * @param[in]  size  The size
       *
       * @return     decrypted data
       */
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const;

      /**
       * @brief      unwraps the encrypted data
       *
       * @param[in]  data      The data
       * @param[in]  size      The size
       * @param      dest      The destination
       * @param[in]  destSize  The destination size
       */
      virtual void unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const;

      /**
       * @brief      returns the meta data as json string
       *
       * @return     json string
       */
      virtual std::string metaData() const;

    private:
      utility::string_t m_kid;
      web::json::value m_attributes;

      const az::AzureKeyVaultClient* m_keyVaultClient;
  };

} // crypto

#endif // AZURE_KEY_VAULT_ENCRYPTION_KEY_H
