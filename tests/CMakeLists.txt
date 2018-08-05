find_package(GTest)

if(NOT GTEST_FOUND)
    include(FetchGoogletest)
    set(GTEST_BOTH_LIBRARIES gtest gtest_main)
endif()

include(GoogleTest)

enable_testing()

include_directories("${PROJECT_SOURCE_DIR}/plugin")

add_executable(AzureStorageTests StorageTest.cpp)
target_link_libraries(AzureStorageTests OrthancAzureBlobStorage
                                        ${AZURESTORAGE_LIBRARIES}
                                        ${GTEST_BOTH_LIBRARIES}
                                        )

#gtest_add_tests(TARGET AzureStorageTests TEST_PREFIX azure:)


add_executable(CryptoTests CryptoTest.cpp)
target_link_libraries(CryptoTests OrthancAzureBlobStorage
                                  ${AZURESTORAGE_LIBRARIES}
                                  ${GTEST_BOTH_LIBRARIES}
                                  )

#gtest_add_tests(TARGET CryptoTests TEST_PREFIX crypto:)
