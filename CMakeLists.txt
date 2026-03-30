cmake_minimum_required(VERSION 3.20)

project(mphil_dis_cholesky LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_BENCHMARKS "Build benchmark executables in src/matrix and src/threads" ON)
option(BUILD_EXAMPLE "Build example executable in example/" ON)
option(ENABLE_OPENMP "Enable OpenMP for parallel targets when available" ON)

set(FILESYSTEM_LINK_LIB "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        set(FILESYSTEM_LINK_LIB "stdc++fs")
    endif()
endif()

include(CTest)

if(ENABLE_OPENMP)
    find_package(OpenMP REQUIRED COMPONENTS CXX)
endif()

add_subdirectory(src)

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
