include(FindPackageHandleStandardArgs)

find_package(Threads REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(LibLZMA REQUIRED)
find_package(Iconv REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(UUID REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup random system thread locale regex filesystem chrono date_time)

find_path(AzureStorage_INCLUDE_DIR
  NAMES
  was/storage_account.h
  PATH_SUFFIXES include
  HINTS ${AZURESTORAGE_ROOT} ${AZURESTORAGE_ROOT_DIR})

find_library(AzureStorage_LIBRARY
  NAMES
  azurestorage
  PATH_SUFFIXES lib
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

  set_property(TARGET Azure::Storage
               PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
               ${UUID_LIBRARIES}
               Boost::thread
               Boost::log
               Boost::log_setup
               Boost::random
               Boost::system
               Boost::thread
               Boost::locale
               Boost::regex
               Boost::filesystem
               Boost::chrono
               Boost::date_time
               Threads::Threads
               OpenSSL::SSL
               OpenSSL::Crypto
               cpprestsdk::cpprest
               ${LIBLZMA_LIBRARIES}
               Iconv::Iconv
               ${LIBXML2_LIBRARY})

  set(AZURESTORAGE_LIBRARY "${AzureStorage_LIBRARY}")
  set(AZURESTORAGE_INCLUDE_DIRS "${AzureStorage_INCLUDE_DIR}")
endif()