set(EXAMPLE_PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/include/mphil_dis_cholesky.h")

if(NOT EXISTS "${EXAMPLE_PUBLIC_HEADER}")
    message(WARNING
        "Skipping example target: ${EXAMPLE_PUBLIC_HEADER} was not found."
    )
    return()
endif()

add_executable(example main.cpp)

target_link_libraries(example
    PRIVATE
        mphil_dis_cholesky
)
