#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "mphil_dis_cholesky.h"
#include "cholesky/v10_parallel_dynamic.hpp"
#include "cholesky/v11_parallel.hpp"
#include "cholesky/v1_baseline.hpp"
#include "cholesky/v2_loop_order.hpp"
#include "cholesky/v3_scalar_hoisting.hpp"
#include "cholesky/v4_pointers.hpp"
#include "cholesky/v5_merge_loop.hpp"
#include "cholesky/v6_cache_blocking.hpp"
#include "cholesky/v7_serial.hpp"
#include "cholesky/v8_parallel_naive.hpp"
#include "cholesky/v9_parallel_static.hpp"

/**
 * @file cholesky_test.cpp
 * @brief Unit tests for various Cholesky decomposition implementations.
 * This file compares custom implementations (v1 through v11) against
 * the LAPACK dpotrf routine.
 */

extern "C"
{
    /**
     * @brief LAPACK Cholesky factorisation routine used as the correctness reference.
     */
    void dpotrf_(const char *uplo, const int *n, double *a, const int *lda, int *info);
}

namespace
{
    /**
     * @brief Computes a correlation value for generating test matrices.
     * @param x Row index.
     * @param y Column index.
     * @param s Scaling factor.
     * @return A double representing the correlation between x and y.
     */

    double corr(double x, double y, double s)
    {
        return 0.99 * std::exp(-0.5 * 16.0 * (x - y) * (x - y) / (s * s));
    }
    /**
     * @brief Generates a symmetric positive-definite matrix for testing.
     * @param n The dimension of the square matrix.
     * @return A flat vector containing the n*n matrix elements.
     */

    std::vector<double> make_test_matrix(int n)
    {
        std::vector<double> matrix(static_cast<size_t>(n) * static_cast<size_t>(n), 0.0);
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                matrix[static_cast<size_t>(i) * static_cast<size_t>(n) + static_cast<size_t>(j)] =
                    corr(i, j, n);
            }
            matrix[static_cast<size_t>(i) * static_cast<size_t>(n) + static_cast<size_t>(i)] = 1.0;
        }
        return matrix;
    }

    /**
     * @brief Calculates the log-determinant of a matrix from its Cholesky factor.
     * @details Uses the property that the determinant is the square of the product
     * of diagonal elements of the L factor.
     * @param matrix The factorised matrix.
     * @param n The dimension of the matrix.
     * @return The log-determinant as a double.
     */
    double log_det_from_factor(const std::vector<double> &matrix, int n)
    {
        double sum_logs = 0.0;
        for (int p = 0; p < n; ++p)
        {
            const double diag =
                matrix[static_cast<size_t>(p) * static_cast<size_t>(n) + static_cast<size_t>(p)];
            sum_logs += std::log(diag);
        }
        return 2.0 * sum_logs;
    }

    /**
     * @brief Computes the log-determinant using the standard LAPACK dpotrf.
     * @param matrix The input matrix to factorise.
     * @param n The dimension of the matrix.
     * @return The reference log-determinant, or NaN if factorisation fails.
     */
    double reference_log_det_lapack(std::vector<double> matrix, int n)
    {
        const char uplo = 'L';
        const int lda = n;
        int info = 0;
        dpotrf_(&uplo, &n, matrix.data(), &lda, &info);
        if (info != 0)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return log_det_from_factor(matrix, n);
    }

    /**
     * @brief Helper to execute a specific version of the Cholesky implementation.
     * @param version The version number (1-11).
     * @param matrix The input matrix to be processed.
     * @param n The dimension of the matrix.
     * @return The resulting log-determinant, or NaN on failure.
     */
    double run_version_and_get_log_det(int version, std::vector<double> matrix, int n)
    {
        constexpr int block_size = 4;
        double elapsed_time = -1.0;

        switch (version)
        {
        case 1:
            elapsed_time = cholesky_v1_baseline_impl(matrix.data(), n);
            break;
        case 2:
            elapsed_time = cholesky_v2_loop_order_impl(matrix.data(), n);
            break;
        case 3:
            elapsed_time = cholesky_v3_scalar_hoisting_impl(matrix.data(), n);
            break;
        case 4:
            elapsed_time = cholesky_v4_pointers_impl(matrix.data(), n);
            break;
        case 5:
            elapsed_time = cholesky_v5_merge_loop_impl(matrix.data(), n);
            break;
        case 6:
            elapsed_time = cholesky_v6_cache_blocking_impl(matrix.data(), n, block_size);
            break;
        case 7:
            elapsed_time = cholesky_optimized_blocked(matrix.data(), n, block_size);
            break;
        case 8:
            elapsed_time = cholesky_parallel_naive(matrix.data(), n, block_size);
            break;
        case 9:
            elapsed_time = cholesky_parallel_static(matrix.data(), n, block_size);
            break;
        case 10:
            elapsed_time = cholesky_parallel_dynamic(matrix.data(), n, block_size);
            break;
        case 11:
            elapsed_time = cholesky_parallel(matrix.data(), n, block_size);
            break;
        default:
            return std::numeric_limits<double>::quiet_NaN();
        }

        if (elapsed_time < 0.0)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return log_det_from_factor(matrix, n);
    }

    /**
     * @test Verifies that all implementation versions (v1-v11) match the LAPACK reference.
     */
    TEST(Correctness, LogDetMatchesReferenceForV1ToV11)
    {
        const std::vector<int> sizes = {2, 4, 10};
        constexpr double tolerance = 1e-8;

        for (int n : sizes)
        {
            const std::vector<double> input = make_test_matrix(n);
            const double reference = reference_log_det_lapack(input, n);
            ASSERT_TRUE(std::isfinite(reference)) << "Reference factorization failed for n=" << n;

            for (int version = 1; version <= 11; ++version)
            {
                SCOPED_TRACE("n=" + std::to_string(n) + ", version=v" + std::to_string(version));
                const double test_value = run_version_and_get_log_det(version, input, n);
                ASSERT_TRUE(std::isfinite(test_value));
                EXPECT_NEAR(test_value, reference, tolerance);
            }
        }
    }

    /**
     * @test Verifies the primary public API function against the LAPACK reference.
     */
    TEST(Correctness, PublicApiMatchesReferenceLogDet)
    {
        const std::vector<int> sizes = {2, 4, 10};
        constexpr double tolerance = 1e-8;

        for (int n : sizes)
        {
            std::vector<double> matrix = make_test_matrix(n);
            const double reference = reference_log_det_lapack(matrix, n);
            ASSERT_TRUE(std::isfinite(reference)) << "Reference factorization failed for n=" << n;

            const double elapsed_time = mphil_dis_cholesky(matrix.data(), n);
            ASSERT_GE(elapsed_time, 0.0);
            EXPECT_NEAR(log_det_from_factor(matrix, n), reference, tolerance);
        }
    }

    /**
     * @test Stress tests the factorisation with a larger 64x64 matrix.
     */
    TEST(Correctness, LargerMatrixFactorisationMatchesReference)
    {
        const int n = 64;
        constexpr double tolerance = 1e-7;

        std::vector<double> matrix = make_test_matrix(n);
        const double reference = reference_log_det_lapack(matrix, n);
        ASSERT_TRUE(std::isfinite(reference)) << "Reference factorisation failed for n=" << n;

        const double elapsed_time = mphil_dis_cholesky(matrix.data(), n);
        ASSERT_GE(elapsed_time, 0.0);
        EXPECT_NEAR(log_det_from_factor(matrix, n), reference, tolerance);
    }

}
