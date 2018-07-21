#ifndef AZURE_KEY_VAULT_ENCRYPTION_KEY_H
#define AZURE_KEY_VAULT_ENCRYPTION_KEY_H

#include "EncryptionKey.h"
#include <string>

namespace crypto
{

  class AzureKeyVaultEncryptionKey : public EncryptionKey
  {
    public:
      AzureKeyVaultEncryptionKey(const std::string& vaultBaseUrl,
                                 const std::string& keyName,
                                 const std::string& keyVersion,
                                 const std::string& apiVersion = "2016-10-01");

      ~AzureKeyVaultEncryptionKey();


    private:
      std::string m_vaultBaseUrl;
      std::string m_keyName;
      std::string m_keyVersion;
      std::string m_apiVersion;
  };

} // crypto

#endif // AZURE_KEY_VAULT_ENCRYPTION_KEY_H
