#ifndef MPHIL_DIS_CHOLESKY_V1_BASELINE_HPP
#define MPHIL_DIS_CHOLESKY_V1_BASELINE_HPP

#include <cmath>
#include <cstdio>

#include "timing.h"

inline double cholesky_v1_baseline_impl(double *c, int n) {
    if (n <= 0 || n > 100000) { // adding 100,000 matrix limit
        std::fprintf(stderr, "cholesky: invalid matrix size n=%d (must be 1 <= n <= 100000)\n", n);
        return -1.0;
    }

    timing::start_clock();

    for (int p = 0; p < n; ++p) { // move along the diagonal of the matrix
        const double diag = std::sqrt(c[p * n + p]);
        c[n * p + p] = diag; // update diagonal element

        for (int j = p + 1; j < n; ++j) { // update row to right of diagonal element
            c[n * p + j] /= diag;
        }

        for (int i = p + 1; i < n; ++i) { // update column below diagonal element
            c[n * i + p] /= diag;
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
