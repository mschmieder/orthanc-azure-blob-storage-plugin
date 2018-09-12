include(FetchContent)

FetchContent_Declare(
  azurestoragecpp
  GIT_REPOSITORY   https://github.com/Azure/azure-storage-cpp.git
  GIT_TAG          v5.0.0
  #PATCH_COMMAND    git apply ${PROJECT_SOURCE_DIR}/3rdparty/azure-storage-cpp/cpprestsdk-2.10-compatibility.patch
)

FetchContent_GetProperties(azurestoragecpp)
if(NOT azurestoragecpp_POPULATED)
  FetchContent_Populate(azurestoragecpp)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${azurestoragecpp_SOURCE_DIR}/Microsoft.WindowsAzure.Storage/cmake/Modules/")
  add_subdirectory(${azurestoragecpp_SOURCE_DIR}/Microsoft.WindowsAzure.Storage ${azurestoragecpp_BINARY_DIR})
  include_directories(${azurestoragecpp_SOURCE_DIR}/Microsoft.WindowsAzure.Storage/includes)
endif()