#include "AzureBlobStorageArea.h"
#include "Configuration.h"
#include <sstream>
#include <cpprest/rawptrstream.h>
namespace OrthancPlugins
{
  AzureBlobStorageArea::AzureBlobStorageArea(AzureBlobStorageConnection* connection)
  : m_connection(connection)
  {
  }

  AzureBlobStorageArea::~AzureBlobStorageArea()
  {
  }

  std::string AzureBlobStorageArea::CreateBlobName(const std::string& uuid, OrthancPluginContentType type)
  {
    // create blob name out of uuid and type
    std::stringstream block_name_stream;
    block_name_stream << uuid << "-" << type;

    return block_name_stream.str();
  }

  void  AzureBlobStorageArea::Create(const std::string& uuid,
   const void* content,
   size_t size,
   OrthancPluginContentType type)
  {
    // get container reference
    auto pContainer = m_connection->getContainer();

    // Retrieve reference to a blob named "my-blob-1".
    azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(CreateBlobName(uuid, type));

    // Create or overwrite the blob with contents from a local file.
    concurrency::streams::istream input_stream = concurrency::streams::rawptr_stream<uint8_t>::open_istream((uint8_t*)content, size);
    blockBlob.upload_from_stream(input_stream);
    input_stream.close().wait();
  }

  void  AzureBlobStorageArea::Read(void*& content,
   size_t& size,
   const std::string& uuid,
   OrthancPluginContentType type)
  {
    // get container reference
    auto pContainer = m_connection->getContainer();

    // Retrieve reference to a blob
    azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(CreateBlobName(uuid, type));

    if (!blockBlob.exists())
    {
      throw AzureBlobStorageException("Requested blob does not exist");
    }

    blockBlob.download_attributes();

    // create enough memory to store the blob
    size = blockBlob.properties().size();
    if (0 == size)
    {
      content = NULL;
    } 
    else
    {
      content = malloc(size);
    }
    
    // Save blob contents to a file.
    concurrency::streams::ostream output_stream = concurrency::streams::rawptr_stream<uint8_t>::open_ostream((uint8_t*)content, size);
    blockBlob.download_to_stream(output_stream);
  }

  void  AzureBlobStorageArea::Remove(const std::string& uuid,
   OrthancPluginContentType type)
  {
    // get container reference
    auto pContainer = m_connection->getContainer();

    // Retrieve reference to a blob
    azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(CreateBlobName(uuid, type));

    if (!blockBlob.exists())
    {
      throw AzureBlobStorageException("Requested blob does not exist");
    }

    blockBlob.delete_blob();
  }

}
