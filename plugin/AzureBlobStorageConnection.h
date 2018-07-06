#ifndef AZURE_BLOB_STORAGE_CONNECTION_H
#define AZURE_BLOB_STORAGE_CONNECTION_H

#include "orthancazureblobstorage_export.h"
#include <stdint.h>
#include <string>
#include <was/storage_account.h>
#include <was/blob.h>

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

      azure::storage::cloud_blob_container* getContainer() { return &m_container; }
      const azure::storage::cloud_blob_container* getContainer() const { return &m_container; }

    private:
      std::string m_storage_connection_string;
      std::string m_container_name;

      azure::storage::cloud_storage_account   m_storage_account;
      azure::storage::cloud_blob_client       m_blob_client;
      azure::storage::cloud_blob_container    m_container;
  };
}

#endif // AZURE_BLOB_STORAGE_CONNECTION_H
