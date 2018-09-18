#ifndef AZURE_BLOB_STORAGE_AREA_H
# define AZURE_BLOB_STORAGE_AREA_H

#include "orthancazureblobstorage_export.h"
#include <orthanc/OrthancCPlugin.h>
#include <memory>
#include "AzureBlobStorageConnection.h"
#include "AzureBlobStorageException.h"

namespace OrthancPlugins
{
  class AzureBlobStorageConnection;

  class ORTHANCAZUREBLOBSTORAGE_EXPORT AzureBlobStorageArea
  {
    public:
      AzureBlobStorageArea(AzureBlobStorageConnection* connection);
      ~AzureBlobStorageArea();

      /**
       * @brief      creates a data entity in the storage
       *
       * @param[in]  uuid     The uuid
       * @param[in]  content  The content
       * @param[in]  size     The size
       * @param[in]  type     The type
       */
      void Create(const std::string& uuid,
                  const void* content,
                  size_t size,
                  OrthancPluginContentType type);

      /**
       * @brief      reads data from the storage
       *
       * @param      content  The content
       * @param      size     The size
       * @param[in]  uuid     The uuid
       * @param[in]  type     The type
       */
      void Read(void*& content,
                size_t& size,
                const std::string& uuid,
                OrthancPluginContentType type);


      /**
       * @brief      removes data for mthe storage
       *
       * @param[in]  uuid  The uuid
       * @param[in]  type  The type
       */
      void Remove(const std::string& uuid,
                  OrthancPluginContentType type);

    private:
      /**
       * @brief      Creates a blob name.
       *
       * @param[in]  uuid  The uuid
       * @param[in]  type  The type
       *
       * @return     name of the blob
       */
      static std::string CreateBlobName(const std::string& uuid, OrthancPluginContentType type);

      void DeleteBlob(const std::string& uuid);
      pplx::task<void> UploadData(const std::string& blobName, const uint8_t* blob, size_t blobSize);
      std::vector<uint8_t> DownloadData(const std::string& blobName);
      size_t DownloadData(const std::string& blobName, void*& data);

      std::unique_ptr<AzureBlobStorageConnection>  m_connection;
  };
}

#endif // AZURE_BLOB_STORAGE_AREA_H

