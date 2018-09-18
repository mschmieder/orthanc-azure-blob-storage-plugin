#include "AzureKeyVaultEncryptionKey.h"
#include "AzureKeyVaultClient.h"

using namespace az;
using namespace crypto;

AzureKeyVaultEncryptionKey::AzureKeyVaultEncryptionKey(az::AzureKeyVaultClient* client,
    const std::string& kid,
    const std::string& attributes_as_json)
  : m_kid(kid),
    m_keyVaultClient(client)
{
    if(attributes_as_json.size() > 0){
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        std::string errors;
        if (!reader->parse(attributes_as_json.c_str(), attributes_as_json.c_str() + attributes_as_json.size(), &m_attributes, &errors)){
          delete reader;
          throw std::runtime_error(errors);
        }
        delete reader;
    }
}

AzureKeyVaultEncryptionKey::~AzureKeyVaultEncryptionKey()
{
}


std::vector<uint8_t> AzureKeyVaultEncryptionKey::wrap(const uint8_t* unencryptedData, size_t size) const
{
   utility::string_t enc_data = m_keyVaultClient->wrapKey(utility::string_t(reinterpret_cast<const char*>(unencryptedData),size),
                                                          m_kid,
                                                          true, "RSA-OAEP-256");

   return std::vector<uint8_t>(enc_data.begin(), enc_data.end());
}

std::vector<uint8_t> AzureKeyVaultEncryptionKey::unwrap(const uint8_t* data, size_t size) const
{
    utility::string_t decrypted_data = m_keyVaultClient->unwrapKey(utility::string_t(reinterpret_cast<const char*>(data),size),
                                                                   m_kid,
                                                                   "RSA-OAEP-256");

    return std::vector<uint8_t>(decrypted_data.begin(), decrypted_data.end());
}

void AzureKeyVaultEncryptionKey::unwrap(const uint8_t* data, size_t size, uint8_t* dest, size_t destSize) const
{
    utility::string_t decrypted_data = m_keyVaultClient->unwrapKey(utility::string_t(reinterpret_cast<const char*>(data),size),
                                                                   m_kid,
                                                                   "RSA-OAEP-256");
    memcpy(dest, decrypted_data.c_str(), decrypted_data.size());
}

std::string AzureKeyVaultEncryptionKey::metaData() const
{
    Json::Value meta(Json::objectValue);
    Json::Value key(Json::objectValue);

    key["kid"] = Json::Value(m_kid);
    meta["key"] = key;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, meta);
}

