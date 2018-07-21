#include "AzureKeyVaultEncryptionKey.h"


using namespace crypto;

AzureKeyVaultEncryptionKey::AzureKeyVaultEncryptionKey(const std::string& vaultBaseUrl,
    const std::string& keyName,
    const std::string& keyVersion,
    const std::string& apiVersion)
  : m_vaultBaseUrl(vaultBaseUrl),
    m_keyName(keyName),
    m_keyVersion(keyVersion),
    m_apiVersion(apiVersion)
{

}

AzureKeyVaultEncryptionKey::~AzureKeyVaultEncryptionKey()
{

}
