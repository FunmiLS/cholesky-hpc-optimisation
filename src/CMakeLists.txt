function(add_cholesky_executable target_name source_file)
    add_executable(${target_name} ${source_file})

    target_include_directories(${target_name}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(${target_name}
        PRIVATE
            timing
            m
    )

    if(DEFINED FILESYSTEM_LINK_LIB AND NOT FILESYSTEM_LINK_LIB STREQUAL "")
        target_link_libraries(${target_name} PRIVATE ${FILESYSTEM_LINK_LIB})
    endif()
endfunction()

function(enable_openmp_if_available target_name)
    if(NOT ENABLE_OPENMP)
        message(FATAL_ERROR "${target_name} requires ENABLE_OPENMP=ON")
    endif()

    if(NOT OpenMP_CXX_FOUND)
        message(FATAL_ERROR "${target_name} requires OpenMP")
    endif()

    target_link_libraries(${target_name} PRIVATE OpenMP::OpenMP_CXX)
endfunction()

add_subdirectory(timing)

add_library(mphil_dis_cholesky STATIC mphil_dis_cholesky.cpp)

target_include_directories(mphil_dis_cholesky
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mphil_dis_cholesky
    PUBLIC
        timing
        m
)

if(DEFINED FILESYSTEM_LINK_LIB AND NOT FILESYSTEM_LINK_LIB STREQUAL "")
    target_link_libraries(mphil_dis_cholesky PUBLIC ${FILESYSTEM_LINK_LIB})
endif()

if(ENABLE_OPENMP)
    target_link_libraries(mphil_dis_cholesky PUBLIC OpenMP::OpenMP_CXX)
endif()

if(BUILD_BENCHMARKS)
    add_subdirectory(blocking)
    add_subdirectory(matrix)
    add_subdirectory(threads)
endif()
