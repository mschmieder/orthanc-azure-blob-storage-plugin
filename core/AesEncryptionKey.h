#ifndef ENCRYPTION_KEY_AES_H
#define ENCRYPTION_KEY_AES_H

#include "EncryptionKey.h"

#include <cryptopp/secblock.h>
#include <cryptopp/aes.h>
#include <vector>


namespace crypto
{
  class AesEncryptionKey : public crypto::EncryptionKey
  {
    public:
      AesEncryptionKey();
      virtual ~AesEncryptionKey();

      AesEncryptionKey(const CryptoPP::SecByteBlock& key, const std::vector<uint8_t>& iv);
      AesEncryptionKey(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv );

      void generate(size_t keyLenth = CryptoPP::AES::MAX_KEYLENGTH);

      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const;
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const;
      virtual void unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const;

      virtual std::string metaData() const;
      static AesEncryptionKey fromJson(const std::string& meta);

      const std::vector<uint8_t>& iv() const;

      const uint8_t* data() const;

      std::string hexData() const;

      size_t size() const;

      const CryptoPP::SecByteBlock& key() const;

    private:
      CryptoPP::SecByteBlock m_key;
      std::vector<uint8_t> m_iv;
  };

} // crypto


#endif // ENCRYPTION_KEY_AES_H
