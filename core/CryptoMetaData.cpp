#include "CryptoMetaData.h"
#include <cpprest/json.h>

using namespace crypto;


CryptoMetaData::CryptoMetaData(const std::string& metaKek,
                               const std::string& metaCek,
                               const uint64_t encryptedDataSize,
                               const uint64_t decryptedDataSize)
  : m_metaKek(metaKek),
    m_metaCek(metaCek),
    m_decryptedDataSize(decryptedDataSize),
    m_encryptedDataSize(encryptedDataSize)
{
}

CryptoMetaData::~CryptoMetaData()
{
}

CryptoMetaData CryptoMetaData::create(const EncryptionKey* kek,
                                      const EncryptionKey* cek,
                                      const uint64_t encryptedDataSize,
                                      const uint64_t decryptedDataSize)
{
  return CryptoMetaData(kek->metaData(),
                        cek->metaData(),
                        encryptedDataSize,
                        decryptedDataSize);
}


// void CryptoMetaData::serialize(std::ostream& stream) const
// {
//   stream << this->asJson();
// }

// CryptoMetaData CryptoMetaData::deserialize(std::istream& stream)
// {
//   web::json::value meta = web::json::value::parse(stream);
//   std::stringstream sstr;
//   meta.serialize(sstr);

//   return CryptoMetaData::fromJson(sstr.str());
// }


uint64_t CryptoMetaData::getDecryptedDataSize() const
{
  return m_decryptedDataSize;
}

uint64_t CryptoMetaData::getEncryptedDataSize() const
{
  return m_encryptedDataSize;
}

std::string CryptoMetaData::asJson() const
{
  web::json::value meta;

  web::json::value cipher;
  cipher[U("size_decrypted")] = web::json::value::number(m_decryptedDataSize);
  cipher[U("size_encrypted")] = web::json::value::number(m_encryptedDataSize);

  meta[U("kek")] = web::json::value::parse(m_metaKek);
  meta[U("cek")] = web::json::value::parse(m_metaCek);
  meta[U("cipher")] = cipher;

  std::stringstream sstr;
  meta.serialize(sstr);

  return sstr.str();
}

CryptoMetaData CryptoMetaData::fromJson(const std::string& meta_json)
{
  web::json::value meta = web::json::value::parse(meta_json);

  std::stringstream kek_stream;
  meta.at(U("kek")).serialize(kek_stream);

  std::stringstream cek_stream;
  meta.at(U("cek")).serialize(cek_stream);

  return CryptoMetaData(kek_stream.str(),
                        cek_stream.str(),
                        meta.at(U("cipher")).at(U("size_decrypted")).as_number().to_uint64(),
                        meta.at(U("cipher")).at(U("size_encrypted")).as_number().to_uint64());
}
