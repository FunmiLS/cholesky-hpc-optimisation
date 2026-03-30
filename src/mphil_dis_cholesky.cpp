/**
 * @file mphil_dis_cholesky.cpp
 * @brief Implementation of the `mphil_dis_cholesky` API.
 *
 * This file implements the mphil_dis_cholesky function, which performs
 * Cholesky factorisation on a given matrix using a parallel algorithm.
 */

#include "mphil_dis_cholesky.h"

#include "cholesky/v11_parallel.hpp"

namespace
{
    /**
     * Default block size for the parallel Cholesky factorisation.
     */
    constexpr int kDefaultBlockSize = 64;

}

/**
 * @brief Runs the public API entry point for Cholesky factorisation.
 * @param c Pointer to a row-major square matrix, overwritten in place.
 * @param n Matrix dimension.
 * @return Elapsed runtime in seconds, or a negative value on failure.
 */
double mphil_dis_cholesky(double *c, int n)
{
    // Call the parallel Cholesky factorisation with the default block size.
    // The input matrix c is overwritten in place with the Cholesky factor.
    return cholesky_parallel(c, n, kDefaultBlockSize);
}
