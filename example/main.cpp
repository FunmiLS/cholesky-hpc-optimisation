/**
 * @file main.cpp
 * @brief Usage example of the `mphil_dis_cholesky` API.
 *
 * The input matrix is overwritten in place. After factorisation, the lower
 * triangle contains the Cholesky factor L.
 */


#include <cmath>
#include <cstdio>
#include <vector>

#include "mphil_dis_cholesky.h"

namespace {

/**
 * @brief Computes log-determinant from a lower-triangular Cholesky factor.
 * @param factor Factor matrix in row-major layout.
 * @param n Matrix dimension.
 * @return The value of `log(|A|)` reconstructed from `L`.
 */
double log_determinant_from_factor(const std::vector<double> &factor, int n) {
    double logdet = 0.0;
    for (int i = 0; i < n; ++i) {
        // Access diagonal L(i,i) in row-major storage.
        logdet += std::log(factor[static_cast<size_t>(n) * static_cast<size_t>(i) +
                                   static_cast<size_t>(i)]);
    }
    return 2.0 * logdet;
}

/**
 * @brief Prints a square matrix to stdout with a simple label.
 * @param matrix Matrix data in row-major layout.
 * @param n Matrix dimension.
 * @param label Text printed before the matrix.
 */
void print_matrix(const std::vector<double> &matrix, int n, const char *label) {
    std::printf("%s\n", label);
    for (int i = 0; i < n; ++i) {
        std::printf("  ");
        for (int j = 0; j < n; ++j) {
            std::printf("%8.4f ", matrix[static_cast<size_t>(n) * static_cast<size_t>(i) +
                                         static_cast<size_t>(j)]);
        }
        std::printf("\n");
    }
    std::printf("\n");
}

}  

/**
 * @brief Demonstrates calling the public API on a tiny SPD matrix.
 * @return `0` on success, `1` if factorisation fails.
 */
int main() {
    const int n = 2;
    std::vector<double> matrix = {
        4.0, 2.0,
        2.0, 26.0,
    };

    std::printf("Cholesky example: in-place factorisation of a symmetric positive-definite matrix\n\n");
    print_matrix(matrix, n, "Input matrix A:");

    const double elapsed = mphil_dis_cholesky(matrix.data(), n);
    if (elapsed < 0.0) {
        std::fprintf(stderr, "API factorisation failed.\n");
        return 1;
    }

    print_matrix(matrix, n, "Matrix after factorisation:");
    std::printf("log|A| = %.8f\n", log_determinant_from_factor(matrix, n));
    std::printf("elapsed time = %.6e seconds\n\n", elapsed);

    std::printf("Expected matrix:\n");
    std::printf("    [ 2 1 ]\n");
    std::printf("A = [ 1 5 ]\n");

    return 0;
}
