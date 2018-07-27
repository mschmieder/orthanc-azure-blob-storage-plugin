set(SOURCES Crypto.h
            Crypto.cpp
            CryptoMetaData.h
            CryptoMetaData.cpp
            Cipher.h
            Cipher.cpp
            azure/AuthEndpoint.h
            azure/AuthEndpoint.cpp
            azure/JwtToken.h
            azure/JwtToken.cpp
            azure/AzureKeyVaultException.h
            azure/AzureKeyVaultClient.h
            azure/AzureKeyVaultClient.cpp
            azure/AzureKeyVaultEncryptionKey.h
            azure/AzureKeyVaultEncryptionKey.cpp
            EncryptionKey.h
            AesEncryptionKey.h
            AesEncryptionKey.cpp)

add_library(OrthancAzureBlobStorageCore STATIC ${SOURCES})

target_include_directories(OrthancAzureBlobStorageCore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                                                              $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_link_libraries(OrthancAzureBlobStorageCore cryptopp-static
                                                  Azure::Storage
                                                  cpprestsdk::cpprest)


set_target_properties(OrthancAzureBlobStorageCore PROPERTIES VERSION   ${AZURE_BLOB_STORAGE_PLUGIN_VERSION}
                                                         SOVERSION ${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR} )
