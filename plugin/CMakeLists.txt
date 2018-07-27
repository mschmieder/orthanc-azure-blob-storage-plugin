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
                      OrthancAzureBlobStorageCore
                      Orthanc::Plugins
                      Azure::Storage
                      JsonCpp::JsonCppStatic)

include(GenerateExportHeader)
generate_export_header(OrthancAzureBlobStorage)

target_include_directories(OrthancAzureBlobStorage PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_compile_definitions(OrthancAzureBlobStorage
                           PRIVATE "-DAZURE_BLOB_STORAGE_PLUGIN_VERSION=\"${AZURE_BLOB_STORAGE_PLUGIN_VERSION}\"")

set_target_properties(OrthancAzureBlobStorage PROPERTIES VERSION   ${AZURE_BLOB_STORAGE_PLUGIN_VERSION}
                                                         SOVERSION ${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR} )

install(TARGETS OrthancAzureBlobStorage
        LIBRARY DESTINATION plugin
        ARCHIVE DESTINATION plugin)

install(FILES resources/azure-blob-storage.json 
        DESTINATION "plugin")
