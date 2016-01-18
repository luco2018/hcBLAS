#include "sgemm_array_kernels.h"

// Sgemm Wrapper routine that invokes the appropriate kernel routines depending on the input dimension M N and K
hcblasStatus gemm_HC(hc::accelerator_view &accl_view,
                     const int order, char TransA, char TransB,
                     const int M, const int N, const int K,
                     const float alpha, hc::array<float> &A_mat,
                     long aOffset, long lda,
                     hc::array<float> &B_mat,
                     long bOffset, long ldb, const float beta,
                     hc::array<float> &C_mat,
                     long cOffset, long ldc,
                     long A_batchOffset = 0, long B_batchOffset = 0, long C_batchOffset = 0, int batchSize = 0) {
  hcblasStatus status = HCBLAS_SUCCEEDS;
  // Start the operations

  if (order) {
    if(batchSize > 0) {
      if (TransB == 'n') {
        if (TransA == 'n') {
          status = gemm_NoTransAB(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
        } else {
          status = gemm_NoTransB(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
        }
      } else if (TransA == 'n') {
        status = gemm_NoTransA(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
      } else {
        status = gemm_TransAB(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
      }
    } else {
      if (TransB == 'n') {
        if (TransA == 'n') {
          status = gemm_NoTransAB(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
        } else {
          status = gemm_NoTransB(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
        }
      } else if (TransA == 'n') {
        status = gemm_NoTransA(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
      } else {
        status = gemm_TransAB(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
      }
    }
  } else {
    if(batchSize > 0) {
      if (TransB == 'n') {
        if (TransA == 'n') {
          status = gemm_NoTransAB_rMajor(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
        } else {
          status = gemm_NoTransB_rMajor(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
        }
      } else if (TransA == 'n') {
        status = gemm_NoTransA_rMajor(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
      } else {
        status = gemm_TransAB_rMajor(accl_view, A_mat, aOffset, A_batchOffset, B_mat, bOffset, B_batchOffset, C_mat, cOffset, C_batchOffset, M, N, K, lda, ldb, ldc, alpha, beta, batchSize);
      }
    } else {
      if (TransB == 'n') {
        if (TransA == 'n') {
          status = gemm_NoTransAB_rMajor(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
        } else {
          status = gemm_NoTransB_rMajor(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
        }
      } else if (TransA == 'n') {
        status = gemm_NoTransA_rMajor(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
      } else {
        status = gemm_TransAB_rMajor(accl_view, A_mat, aOffset, B_mat, bOffset, C_mat, cOffset, M, N, K, lda, ldb, ldc, alpha, beta);
      }
    }
  }

  return status;
}

// Sgemm Call Type I: Inputs and outputs are C++ HC float array containers
hcblasStatus  Hcblaslibrary :: hcblas_sgemm(hc::accelerator_view &accl_view,
					    hcblasOrder order,
					    hcblasTranspose typeA,
					    hcblasTranspose typeB, const int M,
					    const int N, const int K, const float &alpha,
					    hc::array<float> &A, const long lda,
					    hc::array<float> &B, const long ldb,
					    const float &beta,
					    hc::array<float> &C, const long ldc,
					    const long aOffset, const long bOffset, const long cOffset) {
  int i, j;
  float temp;
  hcblasStatus status = HCBLAS_SUCCEEDS;

  // Quick return if possible
  if (!M || !N || !K) {
    return HCBLAS_INVALID;
  }

  // For alpha = 0
  if (alpha == 0) {
    std::vector<float> HostC(M * N);
    hc::copy(C, begin(HostC));
    if (beta == 0) {
      for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
          HostC[cOffset + i + j * ldc] = 0;
        }
      }
    } else {
      for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
          temp = HostC[cOffset + i + j * ldc];
          HostC[cOffset + i + j * ldc] = temp * (beta);
        }
      }
    }
    hc::copy(begin(HostC), end(HostC), C);
    return status;
  }

  status = gemm_HC(accl_view, order, typeA, typeB, M, N, K, alpha, A,
                                aOffset, lda, B, bOffset, ldb, beta, C,
                                cOffset, ldc);
  return status;
}

/* SGEMM- Overloaded function with arguments related to batch processing */
hcblasStatus Hcblaslibrary :: hcblas_sgemm(hc::accelerator_view &accl_view,
					   hcblasOrder order,
					   hcblasTranspose typeA,
					   hcblasTranspose typeB, const int M,
					   const int N, const int K, const float &alpha,
					   hc::array<float> &A, const long lda, const long A_batchOffset,
					   hc::array<float> &B, const long ldb, const long B_batchOffset,
					   const float &beta,
					   hc::array<float> &C, const long ldc, const long C_batchOffset,
					   const long aOffset, const long bOffset, const long cOffset, const int batchSize) {
  int i, j, k;
  float temp;
  hcblasStatus status = HCBLAS_SUCCEEDS;

  // Quick return if possible
  if (!M || !N || !K) {
    return HCBLAS_INVALID;
  }

  // For alpha = 0
  if (alpha == 0) {
    std::vector<float> HostC(M * N * batchSize);
    hc::copy(C, begin(HostC));
    if (beta == 0) {
     for ( k = 0; k < batchSize; ++k) {
      for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
          HostC[cOffset + C_batchOffset * k + i + j * ldc] = 0;
        }
      }
     }
    } else {
     for (k = 0; k < batchSize; ++k) {
      for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
          temp = HostC[cOffset + C_batchOffset * k + i + j * ldc];
          HostC[cOffset + C_batchOffset * k + i + j * ldc] = temp * (beta);
        }
      }
     }
    }
    hc::copy(begin(HostC), end(HostC), C);
    return status;
  }

  status = gemm_HC(accl_view, order, typeA, typeB, M, N, K, alpha, A, aOffset, lda, B,
          bOffset, ldb, beta, C, cOffset, ldc, A_batchOffset, B_batchOffset, C_batchOffset, batchSize);
  return status;
}

