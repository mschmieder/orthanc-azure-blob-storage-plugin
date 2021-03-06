#include "gtest/gtest.h"
#include "Configuration.h"
#include "AzureBlobStorageArea.h"
#include <orthanc/OrthancCPlugin.h>

const utility::string_t storage_connection_string(U("DefaultEndpointsProtocol=https;AccountName=XXXXXX;AccountKey=XXXXXXX;EndpointSuffix=core.windows.net"));
const utility::string_t container_name(U("orthanc-blob-container"));

class AzureBlobStorageTest : public ::testing::Test {
 protected:
    std::unique_ptr<OrthancPlugins::AzureBlobStorageArea> storage_;

  virtual void SetUp()
  {
      OrthancPlugins::AzureBlobStorageConnection conn;

//    std::unique_ptr<OrthancPlugins::AzureBlobStorageConnection> connection = std::make_unique<OrthancPlugins::AzureBlobStorageConnection>();
//    connection->setConnectionString(storage_connection_string);
//    connection->setContainerName(container_name);
//    connection->establishConnection();

//    storage_ = std::make_unique<OrthancPlugins::AzureBlobStorageArea>(connection.release());
  }

  virtual void TearDown()
  {
  }


};

const static std::string input_data(1024*1024, 'A');
const static std::string filename = OrthancPlugins::GenerateUuid();
const static OrthancPluginContentType type = OrthancPluginContentType_Unknown;

TEST_F(AzureBlobStorageTest, StoreFiles)
{
    ASSERT_NO_THROW(storage_->Create(filename, input_data.c_str(), input_data.length(), type));

    void* content;
    size_t size;
    storage_->Read(content, size, filename, type);
    //convert dtaa to string of the specified size
    char* d = static_cast<char*>(content);
    std::string res(d, d + size);

    ASSERT_EQ(input_data.length(), size);
    ASSERT_EQ(input_data, res);

    // free allocated by the Read method memory
    free(content);

    storage_->Remove(filename, type);

    ASSERT_ANY_THROW(storage_->Read(content, size, filename, type));
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
