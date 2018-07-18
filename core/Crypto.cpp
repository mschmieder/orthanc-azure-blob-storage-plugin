#include "Crypto.h"
#include "EncryptionKeyAes.h"
using namespace crypto;
using namespace CryptoPP;

std::vector<uint8_t> Encryption::encrypt(const EncryptionKey* kek, const uint8_t* data, size_t size)
{
  // generate one time content encryption key
  EncryptionKeyAes cek;
  cek.generate();

  // encrypt one time content encryption key (cek) with key encryption key (kek)
  EncryptionKeyAes cek_encrypted(kek->wrap(cek.data(), cek.size()), cek.iv());

  // encrypt the data
  std::vector<uint8_t> encryted_data = cek.wrap(data, size);

  // create the meta data that is stored along the encrypted data
  CryptoMetaData meta(cek_encrypted, size, encryted_data.size());

  // create the buffer containing meta data and enrypted data (cipher)
  std::vector<uint8_t> cipher;
  cipher.resize(meta.getCipherSize());

  // serialize the meta data into the cipher
  size_t numBytesMeta = meta.serialize(cipher.data());

  // compute start location of the encrypted data
  const uint8_t* enc_data_ptr = cipher.data() + numBytesMeta;

  // copy encrypted data into the buffer
  memcpy((void*)enc_data_ptr, encryted_data.data(), encryted_data.size());

  return cipher;
}


std::vector<uint8_t> Decryption::decrypt(const EncryptionKey* kek, const uint8_t* data, size_t size)
{
  // Deserialize the meta data
  CryptoMetaData meta = CryptoMetaData::deserialize(data, size);

  // retrieve the encrypted content encryption key
  EncryptionKeyAes cek_encrypted = meta.getKey();

  // decrypt the content encryption key using the key encryption key and
  // add the stored initialization vector
  EncryptionKeyAes cek(kek->unwrap(cek_encrypted.data(), cek_encrypted.size()),
                       cek_encrypted.iv());

  // compute the encrypted data location
  const uint8_t* encrypted_data_ptr = data + meta.getMetaDataSize();

  // decrypt the encrypted data
  std::vector<uint8_t> decrypted_data = cek.unwrap(encrypted_data_ptr, meta.getEncryptedDataSize());

  return decrypted_data;
}


CryptoMetaData::CryptoMetaData(const EncryptionKeyAes key,
                               const uint64_t encryptedDataSize,
                               const uint64_t decryptedDataSize)
  : m_key(key),
    m_keySize(0),
    m_decryptedDataSize(decryptedDataSize),
    m_encryptedDataSize(encryptedDataSize),
    m_ivSize(0)
{
    m_keySize = key.size();
    m_ivSize = key.iv().size(); 
}

CryptoMetaData::~CryptoMetaData()
{
}

size_t CryptoMetaData::serialize(const uint8_t* data) const
{
  const uint8_t* ptr = data;
  size_t numBytes = 0;
  memcpy((void*)ptr, &m_keySize, sizeof(m_keySize));
  ptr += sizeof(m_keySize);
  numBytes += sizeof(m_keySize);

  memcpy((void*)ptr, &m_ivSize, sizeof(m_ivSize));
  ptr += sizeof(m_ivSize);
  numBytes += sizeof(m_ivSize);

  memcpy((void*)ptr, &m_decryptedDataSize, sizeof(m_decryptedDataSize));
  ptr += sizeof(m_decryptedDataSize);
  numBytes += sizeof(m_decryptedDataSize);

  memcpy((void*)ptr, &m_encryptedDataSize, sizeof(m_encryptedDataSize));
  ptr += sizeof(m_encryptedDataSize);
  numBytes += sizeof(m_encryptedDataSize);

  memcpy((void*)ptr, m_key.data(), m_keySize);
  ptr += m_keySize;
  numBytes += m_keySize;

  memcpy((void*)ptr, m_key.iv().data(), m_ivSize);
  ptr += m_ivSize;
  numBytes += m_ivSize;

  return numBytes;
}

CryptoMetaData CryptoMetaData::deserialize(const uint8_t* data, size_t size)
{
  CryptoMetaData meta;

  const uint8_t* ptr = data;
  memcpy(&meta.m_keySize, ptr, sizeof(meta.m_keySize));
  ptr += sizeof(m_keySize);

  memcpy(&meta.m_ivSize, ptr, sizeof(meta.m_ivSize));
  ptr += sizeof(m_ivSize);

  memcpy(&meta.m_decryptedDataSize, ptr, sizeof(meta.m_decryptedDataSize));
  ptr += sizeof(meta.m_decryptedDataSize);

  memcpy(&meta.m_encryptedDataSize, ptr, sizeof(meta.m_encryptedDataSize));
  ptr += sizeof(meta.m_encryptedDataSize);

  CryptoPP::SecByteBlock secKey(meta.m_keySize);
  memcpy((void*)secKey.data(), ptr, meta.m_keySize);
  ptr += meta.m_keySize;

  std::vector<uint8_t> iv;
  iv.resize(meta.m_ivSize);
  memcpy((void*)iv.data(), ptr, meta.m_ivSize);
  ptr += meta.m_ivSize;

  meta.m_key = EncryptionKeyAes(secKey, iv);

  return meta;
}


size_t CryptoMetaData::getMetaDataSize() const
{
  return sizeof(m_keySize) +
         sizeof(m_decryptedDataSize) +
         sizeof(m_encryptedDataSize) +
         sizeof(m_ivSize) +
         m_keySize +
         m_ivSize;
}

size_t CryptoMetaData::getCipherSize() const
{
  return this->getMetaDataSize() + m_encryptedDataSize;
}

uint64_t CryptoMetaData::getKeySize() const
{
  return m_keySize;
}

uint64_t CryptoMetaData::getDecryptedDataSize() const
{
  return m_decryptedDataSize;
}

uint64_t CryptoMetaData::getEncryptedDataSize() const
{
  return m_decryptedDataSize;
}

uint64_t CryptoMetaData::getIvSize() const
{
  return m_ivSize;
}

const EncryptionKeyAes& CryptoMetaData::getKey() const
{
  return m_key;
}
