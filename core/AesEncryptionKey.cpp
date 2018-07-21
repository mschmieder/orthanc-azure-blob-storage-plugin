#include "AesEncryptionKey.h"

using namespace crypto;
using namespace CryptoPP;

AesEncryptionKey::AesEncryptionKey()
{
}

AesEncryptionKey::AesEncryptionKey(const SecByteBlock& key, const std::vector<uint8_t>& iv )
  : m_key(key), m_iv(iv)
{
}

AesEncryptionKey::AesEncryptionKey(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv )
  : m_iv(iv)
{
  m_key = SecByteBlock(key.size());
  m_key.Assign((const uint8_t*)key.data(), key.size());
}

AesEncryptionKey::~AesEncryptionKey()
{
}

void AesEncryptionKey::generate(size_t keyLenth)
{
  AutoSeededRandomPool prng;

  m_iv.resize(AES::BLOCKSIZE);
  m_key = SecByteBlock(keyLenth);

  prng.GenerateBlock( m_key, m_key.size() );
  prng.GenerateBlock( m_iv.data(), m_iv.size() );
}

std::vector<uint8_t> AesEncryptionKey::AesEncryptionKey::wrap(const uint8_t* unencryptedData, size_t size) const
{
  std::vector<uint8_t> cipher;
  cipher.resize(size + AES::BLOCKSIZE);

  CBC_Mode< AES >::Encryption enc;
  enc.SetKeyWithIV( m_key, m_key.size(), m_iv.data(), m_iv.size() );

  ArraySink cs(&cipher[0], cipher.size());
  ArraySource(unencryptedData, size, true,
              new StreamTransformationFilter(enc, new Redirector(cs)));

  // Set cipher text length now that its known
  cipher.resize(cs.TotalPutLength());

  return cipher;
}

std::vector<uint8_t> AesEncryptionKey::unwrap(const uint8_t* data, size_t size) const
{
  std::vector<uint8_t> recover;
  recover.resize(size);

  CBC_Mode< AES >::Decryption d;
  d.SetKeyWithIV( m_key, m_key.size(), m_iv.data(), m_iv.size() );

  ArraySink rs(&recover[0], recover.size());

  ArraySource(data, size, true,
              new StreamTransformationFilter(d, new Redirector(rs)));

  // Set recovered text length now that its known
  recover.resize(rs.TotalPutLength());

  return recover;
}

const std::vector<uint8_t>& AesEncryptionKey::iv() const
{
  return m_iv;
}

const uint8_t* AesEncryptionKey::data() const
{
  return m_key.data();
}

size_t AesEncryptionKey::size() const
{
  return m_key.size();
}

const SecByteBlock& AesEncryptionKey::key() const
{
  return m_key;
}