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

      virtual std::vector<uint8_t> wrap(const uint8_t* unencryptedData, size_t size) const = 0;  
      virtual std::vector<uint8_t> unwrap(const uint8_t* data, size_t size) const = 0;
  };

} // crypto


#endif // ENCRYPTION_KEY_H
