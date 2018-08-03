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
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR "1")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH "0")
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
  endif()
endif()

# Find required packages
find_package(Orthanc REQUIRED)  
find_package(JsonCpp REQUIRED)
find_package(CryptoPP REQUIRED)
find_package(cpprestsdk REQUIRED)

if(USE_VCPKG)
  find_package(AzureStorage REQUIRED)
  set(AZURESTORAGE_LIBRARIES Azure::Storage)
else()
  if(USE_SYSTEM_AZURESTORAGECPP)
    find_package(AzureStorage REQUIRED)
    set(AZURESTORAGE_LIBRARIES Azure::Storage)
  else()
    include(FetchAzureStorageCpp)
    set(AZURESTORAGE_LIBRARIES azurestorage)
  endif()
endif()

add_subdirectory("core")
add_subdirectory("plugin")
add_subdirectory("tests")
