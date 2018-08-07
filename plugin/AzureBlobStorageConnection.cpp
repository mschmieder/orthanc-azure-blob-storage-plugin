#include "AzureBlobStorageConnection.h"
#include "AzureBlobStorageException.h"

#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>

namespace OrthancPlugins
{

  AzureBlobStorageConnection::AzureBlobStorageConnection(const std::string& storage_connection_string,
      const std::string& container_name )
    : m_storage_connection_string(storage_connection_string),
      m_container_name(container_name)
  {
  }

  AzureBlobStorageConnection::~AzureBlobStorageConnection()
  {
  }

  void AzureBlobStorageConnection::setConnectionString(const std::string& storage_connection_string)
  {
    m_storage_connection_string = storage_connection_string;
  }

  void AzureBlobStorageConnection::setContainerName(const std::string& container_name)
  {
    m_container_name = container_name;
  }

  void AzureBlobStorageConnection::establishConnection()
  {
    // Retrieve storage account from connection string.
    m_storage_account = azure::storage::cloud_storage_account::parse(m_storage_connection_string);

    // Create the blob client.
    m_blob_client = m_storage_account.create_cloud_blob_client();

    // Retrieve a reference to a previously created container.
    m_container = m_blob_client.get_container_reference(m_container_name);

    // Create the container if it doesn't already exist.
    m_container.create_if_not_exists();

    if (m_encryptionEnabled)
    {
      m_kvc = std::make_unique<az::AzureKeyVaultClient>(az::AzureKeyVaultClient(m_keyVaultBaseUrl,
              m_keyVaultClientId,
              m_keyVaultClientSecret));

      m_kek = m_kvc->getKey(m_keyVaultKeyId);
    }
  }

  void AzureBlobStorageConnection::enableEncryption(bool enable)
  {
    m_encryptionEnabled = enable;
  }

  bool AzureBlobStorageConnection::encryptionEnabled() const
  {
    return m_encryptionEnabled;
  }

  void AzureBlobStorageConnection::setKeyVaultBaseUrl(const std::string& baseUrl)
  {
    m_keyVaultBaseUrl = baseUrl;
  }

  void AzureBlobStorageConnection::setKeyVaultClientId(const std::string& clientId)
  {
    m_keyVaultClientId = clientId;
  }

  void AzureBlobStorageConnection::setKeyVaultClientSecret(const std::string& clientSecret)
  {
    m_keyVaultClientSecret = clientSecret;
  }

  void AzureBlobStorageConnection::setKeyVaultKeyId(const std::string& kid)
  {
    m_keyVaultKeyId = kid;
  }

} // OrthancPlugins
