include(FindPackageHandleStandardArgs)

find_path(AzureStorage_INCLUDE_DIR
  NAMES
  was/storage_account.h
  PATH_SUFFIXES include
  HINTS ${AZURESTORAGE_ROOT} ${AZURESTORAGE_ROOT_DIR})

find_library(AzureStorage_LIBRARY
  NAMES
  azurestorage
  PATHS lib
  PATH_SUFFIXES
  HINTS ${AZURESTORAGE_ROOT} ${AZURESTORAGE_ROOT_DIR})

find_package_handle_standard_args(AzureStorage
  DEFAULT_MSG
  AzureStorage_INCLUDE_DIR
  AzureStorage_LIBRARY)

if(AZURESTORAGE_FOUND)
  add_library(Azure::Storage UNKNOWN IMPORTED)
  set_target_properties(Azure::Storage PROPERTIES
                        IMPORTED_LOCATION "${AzureStorage_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${AzureStorage_INCLUDE_DIR}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")

  set(AZURESTORAGE_LIBRARY "${AzureStorage_LIBRARY}")
  set(AZURESTORAGE_INCLUDE_DIRS "${AzureStorage_INCLUDE_DIR}")
endif()