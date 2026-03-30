/**
 * @file v11_parallel_threads.cpp
 * @brief Benchmarking tool to measure OpenMP scaling for Cholesky v11.
 * @details This script performs a thread sweep (1 to 70 threads) to evaluate
 * how the parallel implementation scales on a fixed matrix size of 4096.
 * Results are output into a CSV for easy plotting later.
 */

#include "cholesky/v11_parallel.hpp"

#include <omp.h> // OpenMP runtime API
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Computes a correlation value based on exponential decay.
 * @param x Row coordinate.
 * @param y Column coordinate.
 * @param s Scaling factor.
 * @return Computed correlation value.
 */
double corr(double x, double y, double s)
{
    return 0.99 * exp(-0.5 * 16.0 * (x - y) * (x - y) / (s * s));
}

/**
 * @brief Utility to generate a large test matrix in memory.
 * @param n Dimension of the square matrix.
 * @return A vector containing the full matrix data.
 */
std::vector<double> make_test_matrix(int n)
{
    std::vector<double> matrix(n * n, 0.0);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            matrix[n * i + j] = corr(i, j, n);
        }
        matrix[n * i + i] = 1.0;
    }

    return matrix;
}

int main()
{
    const std::string output_dir = "output";
    const std::string output_file = output_dir + "/v11_parallel_threads_runs.csv";
    // fix matrix size and block size
    const int n = 4096;
    const int block_size = 64;

    std::filesystem::create_directories(output_dir);

    // Ensure the output directory exists
    std::ofstream csv(output_file);
    if (!csv.is_open())
    {
        std::cerr << "Error: could not open output file: " << output_file << "\n";
        return 1;
    }
    // Three runs per thread count to account for variance
    csv << "num_threads,run_1,run_2,run_3\n";
    csv << std::fixed << std::setprecision(6);

    // vary thread count
    for (int NUM = 1; NUM <= 70; ++NUM)
    {
        csv << NUM;

        for (int run = 0; run < 3; ++run)
        {
            // Regenerate the matrix
            std::vector<double> matrix = make_test_matrix(n);
            // Set the number of threads
            omp_set_num_threads(NUM);
            const double elapsed_time = cholesky_parallel(matrix.data(), n, block_size);
            if (elapsed_time < 0.0)
            {
                std::cerr << "Error: cholesky_parallel failed for n=" << n
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
