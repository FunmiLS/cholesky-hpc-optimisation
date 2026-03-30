// timing files courtesy of Prof J Fergusson
/**
 * @file timing.cpp
 * @brief Small timing helper used by the Cholesky implementations.
 * @details Stores a global split point and exposes start/split methods based
 * on the C++ high-resolution clock.
 */

#include "timing.h"
#include <chrono>

/**
 * @brief Global timestamp used as the current split reference.
 */
std::chrono::high_resolution_clock::time_point time_point;

/**
 * @brief Starts or resets the internal timing clock.
 */
void timing::start_clock() {
    time_point = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Returns elapsed time since the previous split in milliseconds.
 * @return Milliseconds between now and the last stored time point.
 */
double timing::get_split() {
    std::chrono::high_resolution_clock::time_point time_split = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = time_split - time_point;
    time_point = time_split;
    return duration.count();
}
