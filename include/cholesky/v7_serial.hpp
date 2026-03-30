#ifndef MPHIL_DIS_CHOLESKY_V7_SERIAL_HPP
#define MPHIL_DIS_CHOLESKY_V7_SERIAL_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>

#include "timing.h"

inline double cholesky_optimized_blocked(double *c, int n, int block_size_hint) {
    if (n <= 0 || n > 100000 || block_size_hint <= 0) {
        std::fprintf(stderr, "cholesky: invalid matrix size\n");
        return -1.0;
    }

    const int B = block_size_hint;
    timing::start_clock();

    // cache blocking process matrix in B by B tiles
    for (int k = 0; k < n; k += B) {
        const int block_size = std::min(B, n - k);

        for (int p = k; p < k + block_size; ++p) { // move along the diagonal of the matrix
            double *row_p = c + static_cast<size_t>(p) * n;
            // pointer access row_p points to row p

            const double diag = std::sqrt(row_p[p]);
            row_p[p] = diag; // update diagonal element
            const double inv_diag = 1.0 / diag;
            // scalar hoisting in inner loops

            for (int j = p + 1; j < k + block_size; ++j) { // update row to right of diagonal element
                row_p[j] *= inv_diag;
            }

            for (int i = p + 1; i < k + block_size; ++i) { // update column below diagonal element
                double *row_i = c + static_cast<size_t>(i) * n;
                // pointer access row_i points to row i
                const double factor = row_i[p] * inv_diag;
                // scalar hoisting factor reused across j updates
                row_i[p] = factor;

                for (int j = p + 1; j < k + block_size; ++j) { // update submatrix below-right
                    row_i[j] -= factor * row_p[j];              // of diagonal element
                }
            }
        }

        // blocked panel to solve for rows beneath the current diagonal block
        for (int i = k + block_size; i < n; i += B) {
            const int cur_B_i = std::min(B, n - i);
            for (int p = k; p < k + block_size; ++p) {
                const double diag = c[static_cast<size_t>(p) * n + p];
                for (int r = i; r < i + cur_B_i; ++r) {
                    double acc = c[static_cast<size_t>(r) * n + p];
                    for (int t = k; t < p; ++t) {
                        acc -= c[static_cast<size_t>(r) * n + t] * c[static_cast<size_t>(p) * n + t];
                    }
                    const double solved = acc / diag;
                    c[static_cast<size_t>(r) * n + p] = solved;
                    c[static_cast<size_t>(p) * n + r] = solved;
                }
            }
        }

        // blocked trailing submatrix update
        for (int i = k + block_size; i < n; i += B) {
            const int cur_B_i = std::min(B, n - i);
            for (int j = k + block_size; j < n; j += B) {
                const int cur_B_j = std::min(B, n - j);

                for (int pi = i; pi < i + cur_B_i; ++pi) {
                    double *row_pi = c + static_cast<size_t>(pi) * n;
                    // pointer access row_pi points to row pi
                    for (int pk = k; pk < k + block_size; ++pk) {
                        double *row_pk = c + static_cast<size_t>(pk) * n;
                        // pointer access row_pk points to row pk
                        const double val_ik = row_pi[pk];
                        // scalar hoisting val_ik reused across pj updates

                        for (int pj = j; pj < j + cur_B_j; ++pj) {
                            row_pi[pj] -= val_ik * row_pk[pj];
                        }
                    }
                }
            }
        }
    }

    return timing::get_split() * 1e-3; // return time in seconds
}

#endif
