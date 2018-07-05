set(SOURCES Configuration.h
            Configuration.cpp
            AzureBlobStorageConnection.h
            AzureBlobStorageConnection.cpp
            AzureBlobStorageArea.h
            AzureBlobStorageArea.cpp
            Plugin.cpp)

add_library(azure-blob-storage-plugin ${SOURCES})

target_link_libraries(azure-blob-storage-plugin 
                      Orthanc::Plugins
                      Azure::Storage
                      JsonCpp::JsonCppStatic)