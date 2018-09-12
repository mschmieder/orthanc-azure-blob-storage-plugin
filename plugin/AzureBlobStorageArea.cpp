#include "AzureBlobStorageArea.h"
#include "Configuration.h"
#include <Crypto.h>

#include <sstream>
#include <cpprest/rawptrstream.h>

using namespace OrthancPlugins;


AzureBlobStorageArea::AzureBlobStorageArea(AzureBlobStorageConnection* connection)
  : m_connection(connection)
{
}

AzureBlobStorageArea::~AzureBlobStorageArea()
{
}

void AzureBlobStorageArea::DeleteBlob(const std::string& uuid)
{
  // get container reference
  auto pContainer = m_connection->getContainer();

  // Retrieve reference to a blob
  azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(uuid);

  if (blockBlob.exists()) {
      blockBlob.delete_blob();
  }
  else {
    std::stringstream sstr;
    sstr<<"Warning in AzureBlobStorageArea::DeleteBlob: Request blob '"<< uuid << "' does not exist. Skipping";
    OrthancPluginLogWarning(m_connection->pluginContext(), sstr.str().c_str());
  }
}


pplx::task<void> AzureBlobStorageArea::UploadData(const std::string& blobName, const uint8_t* blob, size_t blobSize)
{
  // get container reference
  auto pContainer = m_connection->getContainer();

  // Create or overwrite the blob with contents from a local file.
  azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(blobName);
  concurrency::streams::istream input_stream = concurrency::streams::rawptr_stream<uint8_t>::open_istream(blob, blobSize);
  blockBlob.upload_from_stream(input_stream);
  return input_stream.close();
}

std::vector<uint8_t> AzureBlobStorageArea::DownloadData(const std::string& blobName)
{
  // get container reference
  auto pContainer = m_connection->getContainer();

  azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(blobName);

  if (!blockBlob.exists())
  {
    throw AzureBlobStorageException("Requested blob does not exist: " + blobName);
  }

  blockBlob.download_attributes();

  // create enough memory to store the blob
  size_t size = blockBlob.properties().size();
  std::vector<uint8_t> vecData(size);

  // Save blob contents to a file.
  concurrency::streams::ostream output_stream = concurrency::streams::rawptr_stream<uint8_t>::open_ostream(vecData.data(), size);
  blockBlob.download_to_stream(output_stream);

  return vecData;
}

size_t AzureBlobStorageArea::DownloadData(const std::string& blobName, void*& data)
{
  // get container reference
  auto pContainer = m_connection->getContainer();

  azure::storage::cloud_block_blob blockBlob = pContainer->get_block_blob_reference(blobName);

  if (!blockBlob.exists())
  {
    throw AzureBlobStorageException("Requested blob does not exist: " + blobName);
  }

  blockBlob.download_attributes();

  // create enough memory to store the blob
  size_t size = blockBlob.properties().size();
  if (0 == size)
  {
    data = nullptr;
  }
  else
  {
    data = malloc(size);
  }

  // Save blob contents to a file.
  concurrency::streams::ostream output_stream = concurrency::streams::rawptr_stream<uint8_t>::open_ostream((uint8_t*)data, size);
  blockBlob.download_to_stream(output_stream);

  return size;
}


std::string AzureBlobStorageArea::CreateBlobName(const std::string& uuid, OrthancPluginContentType type)
{
  // create blob name out of uuid and type
  std::stringstream block_name_stream;
  block_name_stream << uuid << "-" << type;

  return block_name_stream.str();
}

void AzureBlobStorageArea::Create(const std::string& uuid,
                                   const void* content,
                                   size_t size,
                                   OrthancPluginContentType type)
{
  // Retrieve reference to a blob named "my-blob-1".
  std::string blobId = CreateBlobName(uuid, type);
  std::string metaId = blobId + ".info";

  const uint8_t* data_ptr = (const uint8_t*)content;
  size_t dataSize = size;
  crypto::Cipher cipher;

  pplx::task<void> metaUploadTask;
  if (m_connection->encryptionEnabled())
  {
    cipher = crypto::Encryption::encrypt(m_connection->getKeyEncryptionKey(), (const uint8_t*)content, size);
    data_ptr = cipher.data().data();
    dataSize = cipher.data().size();

    // upload meta data
    const std::string& meta = cipher.metaData();
    metaUploadTask = this->UploadData(metaId, (const uint8_t*)meta.c_str(), meta.size());
  }

  // Create or overwrite the blob with contents from a local file.
  pplx::task<void> dataUploadTask = this->UploadData(blobId, data_ptr, dataSize);

  metaUploadTask.wait();
  dataUploadTask.wait();
}

void AzureBlobStorageArea::Read(void*& content,
                                 size_t& size,
                                 const std::string& uuid,
                                 OrthancPluginContentType type)
{
  // Retrieve reference to a blob
  std::string blobId = CreateBlobName(uuid, type);
  std::string metaId = blobId + ".info";
  size = 0;

  if (m_connection->encryptionEnabled())
  {
    std::vector<uint8_t> metaDataJson = this->DownloadData(metaId);
    crypto::CryptoMetaData metaData = crypto::CryptoMetaData::fromJson(std::string(metaDataJson.begin(), metaDataJson.end()));

    std::vector<uint8_t> encryptedBlob = this->DownloadData(blobId);

    size_t decryptedDataSize = metaData.getDecryptedDataSize();

    // malloc is required, new creates issues with free on orthanc's side
    content = malloc(decryptedDataSize);
    size = decryptedDataSize;

    utility::string_t kek_id = metaData.asJson().at(U("kek")).at(U("kid")).as_string();
    az::AzureKeyVaultEncryptionKey kek = m_connection->getAzureKeyEncryptionKey(kek_id);
    crypto::Decryption::decrypt(&kek,
                                metaData,
                                encryptedBlob.data(), encryptedBlob.size(),
                                (uint8_t*)content,
                                decryptedDataSize);

  }
  else
  {
    size = this->DownloadData(blobId, content);
  }
}

void  AzureBlobStorageArea::Remove(const std::string& uuid,
                                   OrthancPluginContentType type)
{
  std::string blobId = CreateBlobName(uuid, type);
  std::string metaId = blobId + ".info";
  this->DeleteBlob(blobId);
  this->DeleteBlob(metaId);
}

