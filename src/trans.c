/*
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */
#include "cachelab.h"
#include <stdio.h>
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] =
    "Transpose submission"; // 请不要修改“Transpose_submission”

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {

    //                          请在此处添加代码
    //*************************************Begin********************************************************
    int i, j, k;
    int a1, a2, a3, a4, a5, a6, a7, a8;

    if (M == 32) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 8; k++) {
                    a1 = A[i * 8 + k][j * 8];
                    a2 = A[i * 8 + k][j * 8 + 1];
                    a3 = A[i * 8 + k][j * 8 + 2];
                    a4 = A[i * 8 + k][j * 8 + 3];
                    a5 = A[i * 8 + k][j * 8 + 4];
                    a6 = A[i * 8 + k][j * 8 + 5];
                    a7 = A[i * 8 + k][j * 8 + 6];
                    a8 = A[i * 8 + k][j * 8 + 7];
                    B[j * 8][i * 8 + k] = a1;
                    B[j * 8 + 1][i * 8 + k] = a2;
                    B[j * 8 + 2][i * 8 + k] = a3;
                    B[j * 8 + 3][i * 8 + k] = a4;
                    B[j * 8 + 4][i * 8 + k] = a5;
                    B[j * 8 + 5][i * 8 + k] = a6;
                    B[j * 8 + 6][i * 8 + k] = a7;
                    B[j * 8 + 7][i * 8 + k] = a8;
                }
            }
        }
    } else if (M == 64) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                // A₁₁ᵀ → B₁₁，A₁₂ᵀ → B₁₂（临时位置）
                for (k = 0; k < 4; k++) {
                    a1 = A[i * 8 + k][j * 8];
                    a2 = A[i * 8 + k][j * 8 + 1];
                    a3 = A[i * 8 + k][j * 8 + 2];
                    a4 = A[i * 8 + k][j * 8 + 3];
                    a5 = A[i * 8 + k][j * 8 + 4];
                    a6 = A[i * 8 + k][j * 8 + 5];
                    a7 = A[i * 8 + k][j * 8 + 6];
                    a8 = A[i * 8 + k][j * 8 + 7];

                    B[j * 8][i * 8 + k] = a1;
                    B[j * 8 + 1][i * 8 + k] = a2;
                    B[j * 8 + 2][i * 8 + k] = a3;
                    B[j * 8 + 3][i * 8 + k] = a4;
                    B[j * 8][i * 8 + k + 4] = a5;
                    B[j * 8 + 1][i * 8 + k + 4] = a6;
                    B[j * 8 + 2][i * 8 + k + 4] = a7;
                    B[j * 8 + 3][i * 8 + k + 4] = a8;
                }

                // 将B₁₂中的A₁₂ᵀ搬到B₂₁，同时将A₂₁ᵀ写入B₁₂
                for (k = 0; k < 4; k++) {
                    /* 读出B₁₂中暂存的A₁₂ᵀ（B行已在Cache中，命中） */
                    a1 = B[j * 8 + k][i * 8 + 4];
                    a2 = B[j * 8 + k][i * 8 + 5];
                    a3 = B[j * 8 + k][i * 8 + 6];
                    a4 = B[j * 8 + k][i * 8 + 7];

                    /* 读A₂₁的一列 */
                    a5 = B[i * 8 + 4][j * 8 + k];
                    a6 = B[i * 8 + 5][j * 8 + k];
                    a7 = B[i * 8 + 6][j * 8 + k];
                    a8 = B[i * 8 + 7][j * 8 + k];

                    /* A₂₁ᵀ → B₁₂ */
                    B[j * 8 + k][i * 8 + 4] = a5;
                    B[j * 8 + k][i * 8 + 5] = a6;
                    B[j * 8 + k][i * 8 + 6] = a7;
                    B[j * 8 + k][i * 8 + 7] = a8;

                    /* A₁₂ᵀ → B₂₁ */
                    B[j * 8 + 4 + k][i * 8] = a1;
                    B[j * 8 + 4 + k][i * 8 + 1] = a2;
                    B[j * 8 + 4 + k][i * 8 + 2] = a3;
                    B[j * 8 + 4 + k][i * 8 + 3] = a4;
                }

                // A₂₂ → B₂₂
                for (k = 0; k < 4; k++) {
                    a5 = A[i * 8 + 4 + k][j * 8 + 4];
                    a6 = A[i * 8 + 4 + k][j * 8 + 5];
                    a7 = A[i * 8 + 4 + k][j * 8 + 6];
                    a8 = A[i * 8 + 4 + k][j * 8 + 7];

                    B[j * 8 + 4][i * 8 + 4 + k] = a5;
                    B[j * 8 + 5][i * 8 + 4 + k] = a6;
                    B[j * 8 + 6][i * 8 + 4 + k] = a7;
                    B[j * 8 + 7][i * 8 + 4 + k] = a8;
                }
            }
        }
    } else {
        for (i = 0; i < N; i += 8) {
            /* 主块区：完整的8列宽块，可以用a1~a8展开 */
            for (j = 0; j + 7 < M; j += 8) {
                for (k = i; k < i + 8 && k < N; k++) {
                    a1 = A[k][j];
                    a2 = A[k][j + 1];
                    a3 = A[k][j + 2];
                    a4 = A[k][j + 3];
                    a5 = A[k][j + 4];
                    a6 = A[k][j + 5];
                    a7 = A[k][j + 6];
                    a8 = A[k][j + 7];
                    B[j][k] = a1;
                    B[j + 1][k] = a2;
                    B[j + 2][k] = a3;
                    B[j + 3][k] = a4;
                    B[j + 4][k] = a5;
                    B[j + 5][k] = a6;
                    B[j + 6][k] = a7;
                    B[j + 7][k] = a8;
                }
            }
            /* 边界列：最后不足8列的残余部分逐元素处理 */
            for (k = i; k < i + 8 && k < N; k++) {
                for (j = (M / 8) * 8; j < M; j++) {
                    a1 = A[k][j];
                    B[j][k] = a1;
                }
            }
        }
    }

    //**************************************End**********************************************************
}

/*
 * 我们在下面定义了一个简单的方法来帮助您开始，您可以根据下面的例子把上面值置补充完整。
 */

/*
 * 简单的基线转置功能，未针对缓存进行优化。
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions-此函数向驱动程序注册转置函数。
 *在运行时，驱动程序将评估每个注册的函数并总结它们的性能。这是一种试验不同转置策略的简便方法。
 */
void registerFunctions() {
    /* 注册解决方案函数  */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* 注册任何附加转置函数 */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose -
 * 函数检查B是否是A的转置。在从转置函数返回之前，可以通过调用它来检查转置的正确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
