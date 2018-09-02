cmake_minimum_required(VERSION 3.10)

if(APPLE)
  set(CMAKE_FIND_FRAMEWORK LAST)
endif()

project(OrthancAzureBlobStoragePlugin)

# enable c++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "d")

option(USE_VCPGK "using vcpg environment" OFF)

set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR "0")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR "2")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH "2")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION "${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR}.${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR}.${AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH}" )

# Add custom cmake modules to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}" )

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
endif()

if(NOT USE_VCPGK)
  if(APPLE AND NOT OPENSSL_ROOT_DIR)
    # Prefer a homebrew version of OpenSSL over the one in /usr/lib
    file(GLOB OPENSSL_ROOT_DIR /usr/local/Cellar/openssl/*)

    # Prefer the latest (make the latest one first)
    list(REVERSE OPENSSL_ROOT_DIR)

    # Find required packages
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
  endif()
endif()

find_package(Orthanc REQUIRED)  
find_package(JsonCpp REQUIRED)
find_package(CryptoPP REQUIRED)
find_package(cpprestsdk REQUIRED)

if(USE_VCPKG)
  find_package(AzureStorage REQUIRED)
  set(AZURESTORAGE_LIBRARIES Azure::Storage)
else()
  find_package(AzureStorage QUIET)
  if(AzureStorage_FOUND)
    set(AZURESTORAGE_LIBRARIES Azure::Storage)
  else()
    include(FetchAzureStorageCpp)
    set(AZURESTORAGE_LIBRARIES azurestorage)
  endif()
endif()

include_directories(core)
include_directories(plugin)

set(SOURCES_CORE  core/Crypto.h
                  core/Crypto.cpp
                  core/CryptoMetaData.h
                  core/CryptoMetaData.cpp
                  core/Cipher.h
                  core/Cipher.cpp
                  core/azure/AuthEndpoint.h
                  core/azure/AuthEndpoint.cpp
                  core/azure/JwtToken.h
                  core/azure/JwtToken.cpp
                  core/azure/AzureKeyVaultException.h
                  core/azure/AzureKeyVaultClient.h
                  core/azure/AzureKeyVaultClient.cpp
                  core/azure/AzureKeyVaultEncryptionKey.h
                  core/azure/AzureKeyVaultEncryptionKey.cpp
                  core/EncryptionKey.h
                  core/AesEncryptionKey.h
                  core/AesEncryptionKey.cpp)

set(SOURCES_PLUGIN  plugin/AzureBlobStorageException.h
                    plugin/Configuration.h
                    plugin/Configuration.cpp
                    plugin/AzureBlobStorageConnection.h
                    plugin/AzureBlobStorageConnection.cpp
                    plugin/AzureBlobStorageArea.h
                    plugin/AzureBlobStorageArea.cpp
                    plugin/Plugin.cpp)

add_library(OrthancAzureBlobStorage SHARED ${SOURCES_CORE} ${SOURCES_PLUGIN})

target_link_libraries(OrthancAzureBlobStorage
                      Orthanc::Plugins
                      ${AZURESTORAGE_LIBRARIES}
                      CryptoPP::CryptoPP
                      JsonCpp::JsonCpp)

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

install(FILES plugin/resources/azure-blob-storage.json 
        DESTINATION "plugin")

add_subdirectory("tests")
