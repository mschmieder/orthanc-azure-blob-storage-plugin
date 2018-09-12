#ifndef AZURE_BLOB_STORAGE_CONNECTION_H
#define AZURE_BLOB_STORAGE_CONNECTION_H

#include "orthancazureblobstorage_export.h"
#include <stdint.h>
#include <string>
#include <was/storage_account.h>
#include <was/blob.h>
#include "azure/AzureKeyVaultClient.h"

#include <orthanc/OrthancCPlugin.h>

namespace OrthancPlugins
{
  class ORTHANCAZUREBLOBSTORAGE_EXPORT AzureBlobStorageConnection
  {
    public:
      /**
       * @brief Construct a new Azure Blob Storage Connection object
       *
       * @param storage_connection_string
       * @param container_name
       */
      AzureBlobStorageConnection(OrthancPluginContext* context = nullptr,
                                 const std::string& storage_connection_string = "",
                                 const std::string& container_name = "");

      AzureBlobStorageConnection(const AzureBlobStorageConnection& other) = delete;

      /**
       * @brief Destroy the Azure Blob Storage Connection object
       *
       */
      ~AzureBlobStorageConnection();

      /**
       * @brief establishes the connection to the blob storage
       *
       */
      void establishConnection();

      /**
       * @brief Set the Connection String object
       *
       * @param storage_connection_string
       */
      void setConnectionString(const std::string& storage_connection_string);

      /**
       * @brief Set the Container Name object
       *
       * @param container_name
       */
      void setContainerName(const std::string& container_name);

      /**
       * @brief
       *
       * @param enable
       */
      void enableEncryption(bool enable);

      /**
       * @brief
       *
       * @return true
       * @return false
       */
      bool encryptionEnabled() const;

      /**
       * @brief Set the Key Vault Base Url object
       *
       * @param baseUrl
       */
      void setKeyVaultBaseUrl(const std::string& baseUrl);

      /**
       * @brief Set the Key Vault Client Id object
       *
       * @param clientId
       */
      void setKeyVaultClientId(const std::string& clientId);

      /**
       * @brief Set the Key Vault Client Secret object
       *
       * @param clientSecret
       */
      void setKeyVaultClientSecret(const std::string& clientSecret);

      /**
       * @brief Set the Key Vault Key Id object
       *
       * @param kid
       */
      void setKeyVaultKeyId(const std::string& kid);

      /**
       * @brief Get the Container object
       *
       * @return azure::storage::cloud_blob_container*
       */
      azure::storage::cloud_blob_container* getContainer() { return &m_container; }

      /**
       * @brief Get the Container object
       *
       * @return const azure::storage::cloud_blob_container*
       */
      const azure::storage::cloud_blob_container* getContainer() const { return &m_container; }

      /**
       * @brief Get the Key Encryption Key object
       *
       * @return const crypto::EncryptionKey*
       */
      const crypto::EncryptionKey* getKeyEncryptionKey() const {return &m_kek;}

      az::AzureKeyVaultEncryptionKey getAzureKeyEncryptionKey(const std::string& keyId);

      /**
       * @brief
       *
       * @return const OrthancPluginContext*
       */
      OrthancPluginContext* pluginContext() { return m_pluginContext; }

    private:
      OrthancPluginContext* m_pluginContext;

      std::string m_storage_connection_string;
      std::string m_container_name;

      azure::storage::cloud_storage_account m_storage_account;
      azure::storage::cloud_blob_client m_blob_client;
      azure::storage::cloud_blob_container m_container;

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
