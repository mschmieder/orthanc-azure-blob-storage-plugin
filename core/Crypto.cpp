#include "Crypto.h"
#include "AesEncryptionKey.h"
#include <cpprest/json.h>

using namespace crypto;

Cipher Encryption::encrypt(const EncryptionKey* kek, const uint8_t* data, size_t size)
{
  // generate one time content encryption key
  AesEncryptionKey cek;
  cek.generate();

  // encrypt one time content encryption key (cek) with key encryption key (kek)
  AesEncryptionKey cek_encrypted(kek->wrap(cek.data(), cek.size()), cek.iv());

  // encrypt the data
  std::vector<uint8_t> encryted_data = cek.wrap(data, size);

  // create the meta data that is stored along the encrypted data
  CryptoMetaData meta = CryptoMetaData::create(kek, &cek_encrypted, size, encryted_data.size());

  return Cipher(encryted_data, meta.asJson());
}


std::vector<uint8_t> Decryption::decrypt(const EncryptionKey* kek,
                                         const CryptoMetaData& meta,
                                         const uint8_t* data, size_t size)
{
  // retrieve the encrypted content encryption key
  AesEncryptionKey cek_encrypted = AesEncryptionKey::fromJson(meta.getCekMetaData());

  // decrypt the content encryption key using the key encryption key and
  // add the stored initialization vector
  AesEncryptionKey cek(kek->unwrap(cek_encrypted.data(), cek_encrypted.size()),
                       cek_encrypted.iv());

  // decrypt the encrypted data
  std::vector<uint8_t> decrypted_data = cek.unwrap(data, size);

  return decrypted_data;
}

void Decryption::decrypt(const EncryptionKey* kek,
                    const CryptoMetaData& meta,
                    const uint8_t* data, size_t size,
                    uint8_t* decryptedData,
                    size_t decryptedDataSize)
{
    // retrieve the encrypted content encryption key
    AesEncryptionKey cek_encrypted = AesEncryptionKey::fromJson(meta.getCekMetaData());

    // decrypt the content encryption key using the key encryption key and
    // add the stored initialization vector
    AesEncryptionKey cek(kek->unwrap(cek_encrypted.data(), cek_encrypted.size()),
                         cek_encrypted.iv());

    // decrypt the encrypted data
    cek.unwrap(data, size, decryptedData, decryptedDataSize);
}
