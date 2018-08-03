include(GoogleTest)
find_package(GTest REQUIRED)
enable_testing()

include_directories("${PROJECT_SOURCE_DIR}/plugin")

add_executable(AzureStorageTests StorageTest.cpp)
target_link_libraries(AzureStorageTests OrthancAzureBlobStorage
                                        ${AZURESTORAGE_LIBRARIES}
                                        GTest::GTest
                                        Boost::system)

#gtest_add_tests(TARGET AzureStorageTests TEST_PREFIX azure:)


add_executable(CryptoTests CryptoTest.cpp)
target_link_libraries(CryptoTests OrthancAzureBlobStorageCore
                                  GTest::GTest
                                  Boost::system)

#gtest_add_tests(TARGET CryptoTests TEST_PREFIX crypto:)
