/**
 * @file v11_parallel.cpp
 * @brief Benchmark runner for the final parallel Cholesky implementation.
 * @details Evaluates runtime over matrix sizes with fixed OpenMP threads and
 * block size, saving three runs per size to CSV.
 */

#include "cholesky/v11_parallel.hpp"

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
 * @brief Runs the matrix-size benchmark for v11 and saves results to CSV.
 * @return 0 on success, 1 if file output or factorisation fails.
 */
int main() {
    const std::string output_dir = "output";
    const std::string output_file = output_dir + "/v11_parallel_matrix_size_runs.csv";
    const int block_size = 64;
    const int NUM = 16;

    // openmp fixed thread count
    omp_set_num_threads(NUM);

    std::filesystem::create_directories(output_dir);

    std::ofstream csv(output_file);
    if (!csv.is_open()) {
        std::cerr << "Error: could not open output file: " << output_file << "\n";
        return 1;
    }

    csv << "matrix_size,run_1,run_2,run_3\n";
    csv << std::fixed << std::setprecision(6);

    // vary matrix size
    const std::vector<int> sizes = {128, 256, 512, 768, 1024, 2048, 4096};
    for (int n : sizes) {
        csv << n;

        for (int run = 0; run < 3; ++run) {
            std::vector<double> matrix = make_test_matrix(n);
            const double elapsed_time = cholesky_parallel(matrix.data(), n, block_size);
            if (elapsed_time < 0.0) {
                std::cerr << "Error: cholesky_parallel failed for n=" << n
                          << ", block_size=" << block_size << "\n";
                return 1;
            }
            csv << "," << elapsed_time;
        }

        csv << "\n";
    }

    std::cout << "Saved results to " << output_file
              << " using num_threads=" << NUM
              << " using block_size=" << block_size << "\n";
    return 0;
}
