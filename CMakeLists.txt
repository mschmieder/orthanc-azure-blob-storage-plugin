cmake_minimum_required(VERSION 3.10)

project(OrthancAzureBlobStoragePlugin)

# enable c++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "d")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR "0")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR "1")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH "0")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION "${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR}.${AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR}.${AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH}" )

# Add custom cmake modules to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}" )

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
endif()

# Find required packages
find_package(Orthanc REQUIRED)  
find_package(JsonCpp REQUIRED)
find_package(cryptopp REQUIRED)
find_package(AzureStorage REQUIRED)

add_subdirectory("core")
add_subdirectory("plugin")
add_subdirectory("tests")
