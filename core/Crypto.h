#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include "EncryptionKey.h"
#include "Cipher.h"
#include "CryptoMetaData.h"

#include <sstream>
#include <vector>

namespace crypto
{
  class Encryption
  {
    public:
      Encryption() = delete;
      ~Encryption() = delete;

      /**
       * @brief      encrypt data using a key encryption key
       *
       * @param[in]  kek   The kek
       * @param[in]  data  The data
       * @param[in]  size  The size
       *
       * @return     cipher holding encrypted data
       */
      static Cipher encrypt(const EncryptionKey* kek, const uint8_t* data, size_t size);

  };

  class Decryption
  {
    public:
      Decryption() = delete;
      ~Decryption() = delete;

      /**
       * @brief      decrypt data using key encryption key
       *
       * @param[in]  kek   The kek
       * @param[in]  meta  The meta
       * @param[in]  data  The data
       * @param[in]  size  The size
       *
       * @return     decrypted data
       */
      static std::vector<uint8_t> decrypt(const EncryptionKey* kek,
                                          const CryptoMetaData& meta,
                                          const uint8_t* data, size_t size);

      /**
       * @brief      decrypt data using key encryption key
       *
       * @param[in]  kek                The kek
       * @param[in]  meta               The meta
       * @param[in]  data               The data
       * @param[in]  size               The size
       * @param      decryptedData      The decrypted data
       * @param[in]  decryptedDataSize  The decrypted data size
       */
      static void decrypt(const EncryptionKey* kek,
                          const CryptoMetaData& meta,
                          const uint8_t* data, size_t size,
                          uint8_t* decryptedData,
                          size_t decryptedDataSize);
  };



} // crypto


#endif // CRYPTO_H
