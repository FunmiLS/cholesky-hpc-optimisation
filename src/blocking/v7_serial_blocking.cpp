/**
 * @file v7_serial_blocking.cpp
 * @brief Block-size benchmark for the serial optimised Cholesky version.
 * @details Uses a fixed matrix size and measures runtime over several block
 * sizes, logging three runs for each configuration.
 */

#include "cholesky/v7_serial.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

// matrix generation
/**
 * @brief Computes a smooth correlation-style value for matrix generation.
 * @param x Row coordinate.
 * @param y Column coordinate.
 * @param s Scaling factor based on matrix size.
 * @return Correlation value used as a matrix entry.
 */
double corr(double x, double y, double s) {
    return 0.99 * std::exp(-0.5 * 16.0 * (x - y) * (x - y) / (s * s));
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

}  // namespace

// Vary block size experiment
/**
 * @brief Runs the block-size sweep for v7 and saves results to CSV.
 * @return 0 on success, 1 if file output or factorisation fails.
 */

int main() {
    const std::string output_dir = "output";
    const std::string output_file = output_dir + "/v7_serial_block_size_runs.csv";
    const int matrix_size = 1024; // fixed matrix size for all runs
    const std::vector<int> block_sizes = {8, 16, 32, 64, 128, 256, 512};

    std::filesystem::create_directories(output_dir);

    std::ofstream csv(output_file);
    if (!csv.is_open()) {
        std::cerr << "Error: could not open output file: " << output_file << "\n";
        return 1;
    }

    csv << "block_size,run_1,run_2,run_3\n";
    csv << std::fixed << std::setprecision(6);

    for (int block_size : block_sizes) {
        csv << block_size;

        for (int run = 0; run < 3; ++run) {
            std::vector<double> matrix = make_test_matrix(matrix_size);
            const double elapsed_time =
                cholesky_optimized_blocked(matrix.data(), matrix_size, block_size);
            if (elapsed_time < 0.0) {
                std::cerr << "Error: cholesky_optimized_blocked failed for n=" << matrix_size
                          << ", block_size=" << block_size << "\n";
                return 1;
            }
            csv << "," << elapsed_time;
        }

        csv << "\n";
    }

    std::cout << "Saved results to " << output_file
              << " using matrix_size=" << matrix_size << "\n";
    return 0;
}
