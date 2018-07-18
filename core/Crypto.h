#ifndef CRYPTO_H
#define CRYPTO_H

#include <vector>
#include "EncryptionKey.h"
#include "EncryptionKeyAes.h"

namespace crypto
{
  class Encryption
  {
    public:
      Encryption() = delete;
      ~Encryption() = delete;

      static std::vector<uint8_t> encrypt(const EncryptionKey* kek, const uint8_t* data, size_t size);

  };

  class Decryption
  {
    public:
      Decryption() = delete;
      ~Decryption() = delete;

      static std::vector<uint8_t> decrypt(const EncryptionKey* kek, const uint8_t* data, size_t size);
  };

  class CryptoMetaData
  {
    public:
      CryptoMetaData(const EncryptionKeyAes key = EncryptionKeyAes(),
                     const uint64_t encryptedDataSize = 0,
                     const uint64_t decryptedDataSize = 0);

      ~CryptoMetaData();

      size_t serialize(const uint8_t* data) const;

      static CryptoMetaData deserialize(const uint8_t* data, size_t size);

      size_t getMetaDataSize() const;

      size_t getCipherSize() const;

      uint64_t getKeySize() const;

      uint64_t getDecryptedDataSize() const;

      uint64_t getEncryptedDataSize() const;

      uint64_t getIvSize() const;

      const EncryptionKeyAes& getKey() const;

    private:
      EncryptionKeyAes m_key;
      uint64_t m_keySize;
      uint64_t m_decryptedDataSize;
      uint64_t m_encryptedDataSize;
      uint64_t m_ivSize;
  };

} // crypto


#endif // CRYPTO_H
