#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/hex.h>

#include "AesEncryptionKey.h"

#include <cpprest/json.h>

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

void AesEncryptionKey::unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const
{
  CBC_Mode< AES >::Decryption d;
  d.SetKeyWithIV( m_key, m_key.size(), m_iv.data(), m_iv.size() );

  ArraySink rs(dest, destSize);

  ArraySource(data, size, true,
              new StreamTransformationFilter(d, new Redirector(rs)));
}

std::vector<uint8_t> AesEncryptionKey::unwrap(const uint8_t* data, size_t size) const
{
    std::vector<uint8_t> recover(size);

    CBC_Mode< AES >::Decryption d;
    d.SetKeyWithIV( m_key, m_key.size(), m_iv.data(), m_iv.size() );

    ArraySink rs(recover.data(), size);

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

std::string AesEncryptionKey::hexData() const
{
  std::string keyHex;
  StringSource ss(m_key.data(), m_key.size(), true, new HexEncoder(new StringSink(keyHex)));

  return keyHex;
}

std::string AesEncryptionKey::metaData() const
{
    web::json::value meta;
    web::json::value key;
    key[U("value")] = web::json::value::string(this->hexData());

    std::vector<web::json::value> jsonIv;
    for(auto it = m_iv.cbegin(); it != m_iv.cend(); ++it){
        jsonIv.push_back(web::json::value::number(*it));
    }

    key[U("iv")] = web::json::value::array(jsonIv);
    meta[U("key")] = key;

    std::stringstream stream;
    meta.serialize(stream);

    return stream.str();
}

AesEncryptionKey AesEncryptionKey::fromJson(const std::string &meta)
{
    web::json::value metaJson = web::json::value::parse(meta);

    std::string keyHex = metaJson.at(U("key")).at(U("value")).as_string();
    std::string keyByte;
    StringSource ss(keyHex, true, new HexDecoder(new StringSink(keyByte)));

    std::vector<uint8_t> iv;
    web::json::array ivArray = metaJson.at(U("key")).at(U("iv")).as_array();

    for(auto it = ivArray.cbegin(); it != ivArray.cend(); ++it){
        iv.push_back(static_cast<uint8_t>(it->as_number().to_uint32()));
    }

    return AesEncryptionKey(std::vector<uint8_t>(keyByte.begin(),keyByte.end()),
                            iv);
}
