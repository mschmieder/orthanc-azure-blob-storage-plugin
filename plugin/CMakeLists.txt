  set(SOURCES AzureBlobStorageException.h
            Configuration.h
            Configuration.cpp
            AzureBlobStorageConnection.h
            AzureBlobStorageConnection.cpp
            AzureBlobStorageArea.h
            AzureBlobStorageArea.cpp
            Plugin.cpp)

add_library(OrthancAzureBlobStorage SHARED ${SOURCES})

target_link_libraries(OrthancAzureBlobStorage 
                      Orthanc::Plugins
                      Azure::Storage
                      JsonCpp::JsonCppStatic)

include(GenerateExportHeader)
generate_export_header(OrthancAzureBlobStorage)