/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	//int blockSz;
	int rowB;
	int colB;
	//int r;
	//int c;
	//int diag = 0;
	int temp0, temp1, temp2, temp3, temp4, temp7, temp5, temp6;// temp7;
//	int temp1, temp7;
	// total 11 int typy variables now
	int k;
	int l;

/*
 *	For 64 x 64 case
 *	block size 8
 *	temp7 used to store block size temporarily
 */

	if (M == 64 && N == 64){
/*		temp7 = 8;
		for (k = 0; k < N; k += temp7){
			for (l = 0; l < M; l += temp7){
			// load A[i][] A[i+1][] A[i+2][]
			temp0 = A[k][l];
			temp1 = A[k + 1][l];
			temp2 = A[k + 2][l];
			temp3 = A[k + 2][l + 1];
			temp4 = A[k + 2][l + 2];

			// load B[j+3][]
			B[l + 3][k]     = A[k][l + 3];
			B[l + 3][k + 1] = A[k + 1][l + 3];
			B[l + 3][k + 2] = A[k + 2][l + 3];

			// load B[j+2][]
			// evict A[i+2][?]
			B[l + 2][k]     = A[k][l + 2];
			B[l + 2][k + 1] = A[k + 1][l + 2];
			B[l + 2][k + 2] = temp4;

			temp4 = A[k + 1][l + 1];

			// B[j+1][]
			// evict A[i+1][]?
			B[l + 1][k]     = A[k][l + 1];
			B[l + 1][k + 1] = temp4;
			B[l + 1][k + 2] = temp3;

			// load B[j][]
			// evict A[i][]?
			B[l][k]     = temp0;
			B[l][k + 1] = temp1;
			B[l][k + 2] = temp2;

			// A[i+3][]
			// evict B[j+3][]?
			B[l][k + 3]     = A[k + 3][l];
			B[l + 1][k + 3] = A[k + 3][l + 1];
			B[l + 2][k + 3] = A[k + 3][l + 2];

			temp0 = A[k + 3][l + 3];

			// B[j+3][]
			// evict A[i+3][]?
			B[l + 3][k + 3] = temp0;
		}
	}
*/
		for (colB = 0; colB < 64; colB+=8){
			temp7 = 64;
			if (colB < temp7){
				for (k = 0; k < 4; ++k){
					for (l = 0; l < 8; ++l){
						temp0 = 0;
						temp1 = colB + 8;
						// to avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 8;
						}
						B[temp0 + k][temp1 + l] = A[colB + k][colB + l];
				}	}
				for (k = 0; k < 4; ++k){
					for (l = 0; l < 8; ++l){
						temp0 = 0;
						temp1 = colB + 16;
						// avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 16;
						}
						B[temp0 + k][temp1 + l] = A[colB + k + 4][colB + l];
				}	}
				for (k = 0; k < 4; ++k){
					for (l = 0; l < 4; ++l){
						temp0 = 0;
						temp1 = colB + 8;
						// avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 8;
						}
						B[colB + l][colB + k] = B[temp0 + k][temp1 + l];
				}	}
				for (k = 0; k < 4; ++k){
					for (l = 0; l < 4; ++l){
						temp0 = 0;
						temp1 = colB + 16;
						// avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 16;
						}
						B[colB + l][colB + k + 4] = B[temp0 + k][temp1 + l];
				}	}
				for (k = 0; k < 4; ++k){
					for (l = 4; l < 8; ++l){
						temp0 = 0;
						temp1 = colB + 8;
						// avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 8;
						}
						B[colB + l][colB + k] = B[temp0 + k][temp1 + l];
				}	}
				for (k = 0; k < 4; ++k){
					for (l = 4; l < 8; ++l){
						temp0 = 0;
						temp1 = colB + 16;
						// avoid evictions
						if (temp1 >= N){
							temp0 = 4;
							temp1 = 16;
						}
						B[colB + l][colB + k + 4] = B[temp0 + k][temp1 + l];
				}	}
			} else {
			// other
				for (k = 0; k < 8; ++k){
					for (l = 0; l < 8; ++l)
						B[colB + l][colB + k] = A[colB + k][colB + l];
				}
			}
		}
		for (rowB = 0; rowB < N; rowB += 8){
			for (colB = 0; colB < M; colB += 8){
				if (rowB != colB){
					// A[0-1][4-7]
					for (k = colB; k < colB + 4; k++){
						for(l = rowB; l < rowB + 2; l++){
							B[k][l] = A[l][k];}
					}
					// temp storage
					temp0 = A[rowB][colB + 4];
					temp1 = A[rowB][colB + 5];
					temp2 = A[rowB][colB + 6];
					temp3 = A[rowB][colB + 7];
					temp4 = A[rowB + 1][colB + 4];
					temp5 = A[rowB + 1][colB + 5];
					temp6 = A[rowB + 1][colB + 6];
					temp7 = A[rowB + 1][colB + 7];
					
					// A[4-7][]
					for (k = colB; (k < colB + 4) && (k < M); k++){
						for(l = rowB + 2; (l < rowB + 4) && (l < N); l++){
							B[k][l] = A[l][k];}
					}
					for (k = colB + 4; (k < colB + 8) && (k < M); k++){
						for(l = rowB + 2; (l < rowB + 4) && (l < N); l++){
							B[l - rowB + colB][k - colB + rowB] = A[l][k];}
					}

					for (l = rowB + 4; (l < rowB + 8) && (l < N); l++){
						for(k = colB; (k < colB + 2) && (k < M); k++){
							B[k][l] = A[l][k];}
					}

					// other
					for (l = rowB + 2; (l < rowB + 4) && (l < N); l++){
						for(k = colB + 4; (k < colB + 6) && (k < M); k++){
							B[k][l] = B[l - rowB + colB][k - colB + rowB];}
					}

					B[colB + 4][rowB] = temp0;
					B[colB + 5][rowB] = temp1;
					B[colB + 4][rowB + 1] = temp4;
					B[colB + 5][rowB + 1] = temp5;
	
					temp0 = B[colB + 2][rowB + 6];
					temp1 = B[colB + 2][rowB + 7];
					temp4 = B[colB + 3][rowB + 6];
					temp5 = B[colB + 3][rowB + 7];


					for (l = rowB + 4; (l < rowB + 8) && (l < N); l++){
						for(k = colB + 2; (k < colB + 4) && (k < M); k++){
							B[k][l] = A[l][k];}
					}

					B[colB + 6][rowB] = temp2;
					B[colB + 6][rowB + 1] = temp6;
					B[colB + 6][rowB + 2] = temp0;
					B[colB + 6][rowB + 3] = temp4;
					B[colB + 7][rowB] = temp3;
					B[colB + 7][rowB + 1] = temp7;
					B[colB + 7][rowB + 2] = temp1;
					B[colB + 7][rowB + 3] = temp5;

					// finishing A[4-7][]
					for (k = colB + 4; (k < colB + 6) && (k < M); k++){
						for(l = rowB + 4; (l < rowB + 6) && (l < N); l++){
							B[k + 2][l]     = A[l][k + 2];
							B[k][l]         = A[l][k];
							B[k + 2][l + 2] = A[l + 2][k + 2];
							B[k][l + 2]     = A[l + 2][k];
					}	}
				}
			}
		}
	} else {
/*
 *	For 32 x 32 case
 *	block size also 8
 *	temp7 used to store block size temporarily
 */
		if (M == 32 && N == 32){
			temp7 = 8;}
/*
 * 	For 61 x 64 case
 * 	block size used is 16
 *	temp7 used to store block size temporarily
 */
		else if (M == 61 && N == 67){		// maybe IF not necessary, should probably be a general else case
			temp7 = 16;}
		else {					// catch all case?? not necessary for lab but good to have
		
			temp7 = 8;}
		
		for (rowB = 0; rowB < M; rowB += temp7){
			for (colB = 0; colB < N; colB += temp7){
				// non-eviction cases
				if (colB != rowB){
					for (l = colB; l < colB + temp7; ++l){

						if (l >= N){	break;}
						for (k = rowB; k < rowB + temp7; ++k){

							if (k >= M){	break;}
							B[k][l] = A[l][k];
						}
					}
				} else {
					for (l = colB; l < colB + temp7; ++l){
					
						if (l >= N){	break;}
						for (k = rowB; k < l; ++k){
							if (k >= M){	break;}
							B[k][l] = A[l][k];
						}
						// save because will be evicted otherwise
						temp1 = A[l][l];
						
						for (k = l + 1; k < rowB + temp7; ++k){	
							if (k >= M){	break;}
							B[k][l] = A[l][k];
						}
						// restore
						B[l][l] = temp1;
					}
				}
		}	}
	}

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
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

