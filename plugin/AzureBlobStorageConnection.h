#ifndef AZURE_BLOB_STORAGE_CONNECTION_H
#define AZURE_BLOB_STORAGE_CONNECTION_H

#include <stdint.h>
#include <string>
#include <was/storage_account.h>
#include <was/blob.h>

namespace OrthancPlugins
{
  class AzureBlobStorageConnection
  {
    public:
      AzureBlobStorageConnection(const utility::string_t& storage_connection_string = U(""),
                                 const utility::string_t& container_name = U(""));
      
      AzureBlobStorageConnection(const AzureBlobStorageConnection& other) = delete;
      ~AzureBlobStorageConnection();

      void establishConnection();
      void setConnectionString(const utility::string_t& storage_connection_string);
      void setContainerName(const utility::string_t& container_name);

      azure::storage::cloud_blob_container* getContainer() { return &m_container; }
      const azure::storage::cloud_blob_container* getContainer() const { return &m_container; }

    private:
      utility::string_t m_storage_connection_string;
      utility::string_t m_container_name;

      azure::storage::cloud_storage_account   m_storage_account;
      azure::storage::cloud_blob_client       m_blob_client;
      azure::storage::cloud_blob_container    m_container;
  };
}

#endif // AZURE_BLOB_STORAGE_CONNECTION_H
