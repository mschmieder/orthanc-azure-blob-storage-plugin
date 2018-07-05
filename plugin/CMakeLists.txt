set(SOURCES Configuration.h
            Configuration.cpp
            AzureBlobStorageConnection.h
            AzureBlobStorageConnection.cpp
            AzureBlobStorageArea.h
            AzureBlobStorageArea.cpp
            Plugin.cpp)

add_library(azure-blob-storage-plugin SHARED ${SOURCES})

target_link_libraries(azure-blob-storage-plugin 
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
                      Orthanc::Plugins
                      OpenSSL::SSL
                      OpenSSL::Crypto
                      cpprestsdk::cpprest
                      ${LIBLZMA_LIBRARIES}
                      Iconv::Iconv
                      ${LIBXML2_LIBRARY}
                      Azure::Storage
                      JsonCpp::JsonCppStatic)
