#ifndef AZURE_BLOB_STORAGE_CONNECTION_H
#define AZURE_BLOB_STORAGE_CONNECTION_H

#include "orthancazureblobstorage_export.h"
#include <stdint.h>
#include <string>
#include <was/storage_account.h>
#include <was/blob.h>
#include "azure/AzureKeyVaultClient.h"

namespace OrthancPlugins
{
  class ORTHANCAZUREBLOBSTORAGE_EXPORT AzureBlobStorageConnection
  {
    public:
      AzureBlobStorageConnection(const std::string& storage_connection_string = "",
                                 const std::string& container_name = "");
      
      AzureBlobStorageConnection(const AzureBlobStorageConnection& other) = delete;
      ~AzureBlobStorageConnection();

      void establishConnection();
      void setConnectionString(const std::string& storage_connection_string);
      void setContainerName(const std::string& container_name);
      void enableEncryption(bool enable);
      bool encryptionEnabled() const;
      void setKeyVaultBaseUrl(const std::string& baseUrl);
      void setKeyVaultClientId(const std::string& clientId);
      void setKeyVaultClientSecret(const std::string& clientSecret);
      void setKeyVaultKeyId(const std::string& kid);

      azure::storage::cloud_blob_container* getContainer() { return &m_container; }
      const azure::storage::cloud_blob_container* getContainer() const { return &m_container; }

      const crypto::EncryptionKey* getKeyEncryptionKey() const {return &m_kek;}
      

    private:
      std::string m_storage_connection_string;
      std::string m_container_name;

      azure::storage::cloud_storage_account   m_storage_account;
      azure::storage::cloud_blob_client       m_blob_client;
      azure::storage::cloud_blob_container    m_container;

      bool m_encryptionEnabled;
      std::string m_keyVaultBaseUrl;
      std::string m_keyVaultClientId;
      std::string m_keyVaultClientSecret;
      std::string m_keyVaultKeyId;
    
      std::unique_ptr<az::AzureKeyVaultClient> m_kvc; 
      az::AzureKeyVaultEncryptionKey m_kek; 
  };
}

#endif // AZURE_BLOB_STORAGE_CONNECTION_H
