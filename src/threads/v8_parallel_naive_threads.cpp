/**
 * @file v8_parallel_naive_threads.cpp
 * @brief Thread-scaling benchmark for the naive parallel Cholesky version.
 * @details Runs a thread sweep on a fixed matrix size and writes three timings
 * per thread count to CSV.
 */

#include "cholesky/v8_parallel_naive.hpp"

#include <omp.h>  // OpenMP runtime API
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Computes a smooth correlation-style value for matrix generation.
 * @param x Row coordinate.
 * @param y Column coordinate.
 * @param s Scaling factor based on matrix size.
 * @return Correlation value used as a matrix entry.
 */
double corr(double x, double y, double s) {
    return 0.99 * exp(-0.5 * 16.0 * (x - y) * (x - y) / (s * s));
}

/**
 * @brief Builds a symmetric positive-definite test matrix.
 * @param n Dimension of the square matrix.
 * @return Flat row-major matrix data.
 */
std::vector<double> make_test_matrix(int n) {
    std::vector<double> matrix(n * n, 0.0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[n * i + j] = corr(i, j, n);
        }
        matrix[n * i + i] = 1.0;
    }

    return matrix;
}

/**
 * @brief Runs the thread-sweep benchmark for v8 and saves results to CSV.
 * @return 0 on success, 1 if file output or factorisation fails.
 */
int main() {
    const std::string output_dir = "output";
    const std::string output_file = output_dir + "/v8_parallel_naive_threads_runs.csv";
    // fixed matrix and block size
    const int n = 4096;
    const int block_size = 64;

    std::filesystem::create_directories(output_dir);

    std::ofstream csv(output_file);
    if (!csv.is_open()) {
        std::cerr << "Error: could not open output file: " << output_file << "\n";
        return 1;
    }

    csv << "num_threads,run_1,run_2,run_3\n";
    csv << std::fixed << std::setprecision(6);

    // vary thread count from 1 to 70 and run three times per count
    for (int NUM = 1; NUM <= 70; ++NUM) {
        csv << NUM;

        for (int run = 0; run < 3; ++run) {
            std::vector<double> matrix = make_test_matrix(n);
            // openmp sweep runtime thread counts, naive (vanilla) parallel version to measure initial improvements
            omp_set_num_threads(NUM);
            const double elapsed_time = cholesky_parallel_naive(matrix.data(), n, block_size);
            if (elapsed_time < 0.0) {
                std::cerr << "Error: cholesky_parallel_naive failed for n=" << n
                          << ", block_size=" << block_size << "\n";
                return 1;
            }
            csv << "," << elapsed_time;
        }

        csv << "\n";
    }

    std::cout << "Saved results to " << output_file
              << " using matrix_size=" << n
              << " and block_size=" << block_size << "\n";
    return 0;
}
