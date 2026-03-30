set(TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/test_correctness.cpp")
if(NOT EXISTS "${TEST_SOURCE}")
    message(STATUS "No tests found at test/test_correctness.cpp")
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(test_correctness ${TEST_SOURCE})

target_link_libraries(test_correctness
    PRIVATE
        mphil_dis_cholesky
        GTest::gtest_main
)

if(APPLE)
    target_link_libraries(test_correctness PRIVATE "-framework Accelerate")
else()
    find_package(LAPACK REQUIRED)
    target_link_libraries(test_correctness PRIVATE LAPACK::LAPACK)
endif()

include(GoogleTest)
gtest_discover_tests(test_correctness)
