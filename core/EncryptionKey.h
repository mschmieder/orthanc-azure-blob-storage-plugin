#ifndef ENCRYPTION_KEY_H
#define ENCRYPTION_KEY_H

#include <string>

namespace crypto
{
  class EncryptionKey
  {
    public:
      EncryptionKey() {}
      virtual ~EncryptionKey() {}

      /**
       * @brief      wraps the data into an encrypted array
       *
       * @param[in]  unencryptedData  The unencrypted data
       * @param[in]  size             The size
       *
       * @return     encrypted data
       */
      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const = 0;

      /**
       * @brief      unwraps the encrypted data
       *
       * @param[in]  data  The data
       * @param[in]  size  The size
       *
       * @return     decrypted data
       */
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const = 0;

      /**
       * @brief      unwraps the encrypted data
       *
       * @param[in]  data      The data
       * @param[in]  size      The size
       * @param      dest      The destination
       * @param[in]  destSize  The destination size
       */
      virtual void unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const = 0;

      /**
       * @brief      returns the meta data as json string
       *
       * @return     json string
       */
      virtual std::string metaData() const = 0;

  };

} // crypto


#endif // ENCRYPTION_KEY_H
