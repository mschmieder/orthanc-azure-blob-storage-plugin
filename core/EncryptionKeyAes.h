#ifndef ENCRYPTION_KEY_AES_H
#define ENCRYPTION_KEY_AES_H

#include "EncryptionKey.h"
#include <osrng.h>
#include <modes.h>
#include <hex.h>
#include <files.h>
#include <filters.h>

namespace crypto
{
  class EncryptionKeyAes : public EncryptionKey
  {
    public:
      EncryptionKeyAes();
      EncryptionKeyAes(const CryptoPP::SecByteBlock& key, const std::vector<uint8_t>& iv);
      EncryptionKeyAes(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv );

      virtual ~EncryptionKeyAes();

      void generate(size_t keyLenth = CryptoPP::AES::MAX_KEYLENGTH);
      
      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const;
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const;
      const std::vector<uint8_t>& iv() const;

      const uint8_t* data() const;

      size_t size() const;

      const CryptoPP::SecByteBlock& key() const;

    private:
      CryptoPP::SecByteBlock m_key;
      std::vector<uint8_t> m_iv;
  };

} // crypto


#endif // ENCRYPTION_KEY_AES_H
