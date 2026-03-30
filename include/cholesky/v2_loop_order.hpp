#ifndef MPHIL_DIS_CHOLESKY_V2_LOOP_ORDER_HPP
#define MPHIL_DIS_CHOLESKY_V2_LOOP_ORDER_HPP

#include <cmath>

#include "timing.h"

inline double cholesky_v2_loop_order_impl(double *c, int n) {
    if (n <= 0 || n > 100000) {
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

        // loop order change from v1 baseline: outer loop is i, inner loop is j (was j then i)
        for (int i = p + 1; i < n; ++i) { // update submatrix below-right
            for (int j = p + 1; j < n; ++j) { // of diagonal element
                c[n * i + j] -= c[n * i + p] * c[n * p + j];
            }
        }
    }

    return timing::get_split() * 1e-3; // return time in seconds
}

#endif
