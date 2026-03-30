add_library(timing STATIC timing.cpp)

target_include_directories(timing
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
