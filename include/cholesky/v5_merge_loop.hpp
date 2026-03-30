#ifndef MPHIL_DIS_CHOLESKY_V5_MERGE_LOOP_HPP
#define MPHIL_DIS_CHOLESKY_V5_MERGE_LOOP_HPP

#include <cmath>
#include <cstdio>

#include "timing.h"

inline double cholesky_v5_merge_loop_impl(double *c, int n) {
    if (n <= 0 || n > 100000) {
        std::fprintf(stderr, "cholesky: invalid matrix size n=%d (must be 1 <= n <= 100000)\n", n);
        return -1.0;
    }

    timing::start_clock();

    for (int p = 0; p < n; ++p) { // move along the diagonal of the matrix
        const double diag = std::sqrt(c[p * n + p]);
        c[n * p + p] = diag; // update diagonal element

        // merge loop updates row to right and column below in one loop
        for (int k = p + 1; k < n; ++k) {
            c[n * p + k] /= diag;
            c[n * k + p] /= diag;
        }

        for (int j = p + 1; j < n; ++j) { // update submatrix below-right
            for (int i = p + 1; i < n; ++i) { // of diagonal element
                c[n * i + j] -= c[n * i + p] * c[n * p + j];
            }
        }
    }

    return timing::get_split() * 1e-3; // return time in seconds
}

#endif
