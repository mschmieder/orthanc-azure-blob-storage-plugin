#ifndef AZURE_BLOB_STORAGE_AREA_H
# define AZURE_BLOB_STORAGE_AREA_H

#include <orthanc/OrthancCPlugin.h>
#include <memory>
#include <thread>
#include "AzureBlobStorageConnection.h"

namespace OrthancPlugins
{
  class AzureBlobStorageArea
  {  
    public:
      AzureBlobStorageArea(AzureBlobStorageConnection* connection);
      ~AzureBlobStorageArea();

      void Create(const std::string& uuid,
                  const void* content,
                  size_t size,
                  OrthancPluginContentType type);

      void Read(void*& content,
                size_t& size,
                const std::string& uuid,
                OrthancPluginContentType type);

      void Remove(const std::string& uuid,
                  OrthancPluginContentType type);

    private:
      std::unique_ptr<AzureBlobStorageConnection>  m_connection;
      std::mutex m_mutex;

  };
}

#endif // AZURE_BLOB_STORAGE_AREA_H

