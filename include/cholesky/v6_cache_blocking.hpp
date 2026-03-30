#ifndef MPHIL_DIS_CHOLESKY_V6_CACHE_BLOCKING_HPP
#define MPHIL_DIS_CHOLESKY_V6_CACHE_BLOCKING_HPP

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "timing.h"

inline double cholesky_v6_cache_blocking_impl(double *c, int n, int block_size) {
    if (n <= 0 || n > 100000 || block_size <= 0) {
        std::fprintf(stderr, "cholesky: invalid matrix size n=%d or block_size=%d\n", n, block_size);
        return -1.0;
    }

    timing::start_clock();

    const int BS = block_size;

    for (int p = 0; p < n; ++p) { // move along the diagonal of the matrix
        const double diag = std::sqrt(c[p * n + p]);
        c[n * p + p] = diag; // update diagonal element

        for (int j = p + 1; j < n; ++j) { // update row to right of diagonal element
            c[n * p + j] /= diag;
        }

        for (int i = p + 1; i < n; ++i) { // update column below diagonal element
            c[n * i + p] /= diag;
        }

        // update submatrix below right of diagonal element
        // cache blocking, block i/j loops to improve locality
        for (int jb = p + 1; jb < n; jb += BS) {
            const int j_end = std::min(n, jb + BS);
            for (int ib = p + 1; ib < n; ib += BS) {
                const int i_end = std::min(n, ib + BS);
                for (int j = jb; j < j_end; ++j) {
                    for (int i = ib; i < i_end; ++i) {
                        c[n * i + j] -= c[n * i + p] * c[n * p + j];
                    }
                }
            }
        }
    }

    return timing::get_split() * 1e-3; // return time in seconds
}

#endif
