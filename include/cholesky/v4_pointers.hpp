#ifndef MPHIL_DIS_CHOLESKY_V4_POINTERS_HPP
#define MPHIL_DIS_CHOLESKY_V4_POINTERS_HPP

#include <cmath>
#include <cstddef>
#include <cstdio>

#include "timing.h"

inline double cholesky_v4_pointers_impl(double *c, int n) {
    if (n <= 0 || n > 100000) {
        std::fprintf(stderr, "cholesky: invalid matrix size n=%d (must be 1 <= n <= 100000)\n", n);
        return -1.0;
    }

    timing::start_clock();

    for (int p = 0; p < n; ++p) { // move along the diagonal of the matrix
        double *row_p = c + static_cast<size_t>(p) * static_cast<size_t>(n);
        // pointer access row_p points to row p

        const double diag = std::sqrt(row_p[p]);
        row_p[p] = diag; // update diagonal element

        for (int j = p + 1; j < n; ++j) { // update row to right of diagonal element
            row_p[j] /= diag;
        }
        for (int i = p + 1; i < n; ++i) { // update column below diagonal element
            c[static_cast<size_t>(i) * static_cast<size_t>(n) + static_cast<size_t>(p)] /= diag;
        }

        for (int j = p + 1; j < n; ++j) { // update submatrix below-right
            for (int i = p + 1; i < n; ++i) { // of diagonal element
                double *row_i = c + static_cast<size_t>(i) * static_cast<size_t>(n);
                // pointer access row_i points to row i
                row_i[j] -= row_i[p] * row_p[j];
            }
        }
    }

    return timing::get_split() * 1e-3; // return time in seconds
}

#endif
