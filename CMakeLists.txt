cmake_minimum_required(VERSION 3.10)

project(OrthancAzureBlobStoragePlugin)

# enable c++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "d")

set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MAJOR "0")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_MINOR "1")
set(AZURE_BLOB_STORAGE_PLUGIN_VERSION_PATCH "0")

# Add custom cmake modules to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")

# Find required packages
find_package(Orthanc REQUIRED)  
find_package(JsonCpp REQUIRED)
find_package(AzureStorage REQUIRED)

add_subdirectory("plugin")
add_subdirectory("tests")
