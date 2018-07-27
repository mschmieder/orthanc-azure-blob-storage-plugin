#include "AzureKeyVaultEncryptionKey.h"
#include "AzureKeyVaultClient.h"

using namespace az;
using namespace crypto;

AzureKeyVaultEncryptionKey::AzureKeyVaultEncryptionKey(const az::AzureKeyVaultClient* client,
    const utility::string_t& kid,
    const web::json::value& attributes)
  : m_kid(kid),
    m_keyVaultClient(client),
    m_attributes(attributes)
{
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
    web::json::value meta;
    web::json::value key;
    key[U("kid")] = web::json::value::string(m_kid);
    meta[U("key")] = key;

    std::stringstream stream;
    meta.serialize(stream);

    return stream.str();
}

