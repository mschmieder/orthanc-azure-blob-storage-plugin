cmake_minimum_required(VERSION 3.10)

project(OrthancAzureBlobStoragePlugin)

# enable c++11
set(CMAKE_CXX_STANDARD 14)

# Add custom cmake modules to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}" )

# locate AzureStorage
find_package(AzureStorage)

# locate Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)

# locate Orthanc
find_package(Orthanc REQUIRED)

# locate JsonCpp
find_package(JsonCpp REQUIRED)

add_subdirectory("plugin")
add_subdirectory("tests")
