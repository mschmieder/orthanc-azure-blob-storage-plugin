#include "AzureBlobStorageArea.h"
#include "Configuration.h"

namespace OrthancPlugins
{  
  AzureBlobStorageArea::AzureBlobStorageArea(AzureBlobStorageConnection* connection)
  : m_connection(connection)
  {
  }

  AzureBlobStorageArea::~AzureBlobStorageArea()
  {
  }

  void  AzureBlobStorageArea::Create(const std::string& uuid,
                                      const void* content,
                                      size_t size,
                                      OrthancPluginContentType type)
  {
  }

  void  AzureBlobStorageArea::Read(void*& content,
                                    size_t& size,
                                    const std::string& uuid,
                                    OrthancPluginContentType type) 
  {
  }

  void  AzureBlobStorageArea::Remove(const std::string& uuid,
                                      OrthancPluginContentType type)
  {
  }

}
