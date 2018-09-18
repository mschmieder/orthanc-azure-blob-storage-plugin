#include "CryptoMetaData.h"
#include <cpprest/json.h>

using namespace crypto;

CryptoMetaData::CryptoMetaData(const std::string &metaKek,
                               const std::string &metaCek,
                               const uint64_t encryptedDataSize,
                               const uint64_t decryptedDataSize)
    : m_decryptedDataSize(decryptedDataSize),
      m_encryptedDataSize(encryptedDataSize)
{
  Json::CharReaderBuilder builder;
  Json::CharReader* reader = builder.newCharReader();

  std::string errors;
  if (!reader->parse(metaKek.c_str(), metaKek.c_str() + metaKek.size(), &m_metaKek, &errors))
  {
    delete reader;
    throw std::runtime_error(errors);
  }
  if (!reader->parse(metaCek.c_str(), metaCek.c_str() + metaCek.size(), &m_metaCek, &errors))
  {
    delete reader;
    throw std::runtime_error(errors);
  }

  delete reader;
}

CryptoMetaData::CryptoMetaData(const Json::Value& meta)
{
  if (!meta.isMember("cipher")){
    throw std::runtime_error("missing parameter 'cipher'");
  }
  if (!meta.isMember("kek")){
    throw std::runtime_error("missing parameter 'cek'");
  }
  if (!meta.isMember("cek")){
    throw std::runtime_error("missing parameter 'kek'");
  }


  m_decryptedDataSize = meta["cipher"]["size_decrypted"].asUInt64();
  m_encryptedDataSize = meta["cipher"]["size_encrypted"].asUInt64();
  m_metaCek = meta["cek"];
  m_metaKek = meta["kek"];
}

CryptoMetaData::~CryptoMetaData()
{
}

CryptoMetaData CryptoMetaData::create(const EncryptionKey *kek,
                                      const EncryptionKey *cek,
                                      const uint64_t encryptedDataSize,
                                      const uint64_t decryptedDataSize)
{
  return CryptoMetaData(kek->metaData(),
                        cek->metaData(),
                        encryptedDataSize,
                        decryptedDataSize);
}

uint64_t CryptoMetaData::getDecryptedDataSize() const
{
  return m_decryptedDataSize;
}

uint64_t CryptoMetaData::getEncryptedDataSize() const
{
  return m_encryptedDataSize;
}

std::string CryptoMetaData::asJsonString() const
{
  Json::Value meta = this->asJson();

  Json::StreamWriterBuilder builder;
  return Json::writeString(builder, meta);
}

std::string CryptoMetaData::getKekMetaDataAsString() const
{
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, m_metaKek);
}


std::string CryptoMetaData::getCekMetaDataAsString() const
{
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, m_metaCek);
}

Json::Value CryptoMetaData::asJson() const
{
  Json::Value meta(Json::objectValue);

  Json::Value cipher(Json::objectValue);
  cipher["size_decrypted"] = Json::Value::UInt64(m_decryptedDataSize);
  cipher["size_encrypted"] = Json::Value::UInt64(m_encryptedDataSize);

  meta["kek"] = m_metaKek;
  meta["cek"] = m_metaCek;
  meta["cipher"] = cipher;

  return meta;
}

CryptoMetaData CryptoMetaData::fromJson(const std::string &meta_json)
{
  Json::CharReaderBuilder builder;
  Json::CharReader* reader = builder.newCharReader();

  std::string errors;

  Json::Value meta;
  if (!reader->parse(meta_json.c_str(), meta_json.c_str() + meta_json.size(), &meta, &errors)){
    delete reader;
    throw std::runtime_error(errors);
  }
  delete reader;
  return CryptoMetaData(meta);
}
