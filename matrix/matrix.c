#include "matrix.h"


long modd(long A, long B) {
	if (B == 0) return -1;
	long k = A/B;
	long rtn = A - k*B;
	if (rtn < 0) rtn += B;
	return rtn;
}



matrix_t * make_matrix(long col, long row) {
	matrix_t * rtn = (matrix_t *) malloc(sizeof(matrix_t));
	*rtn = (matrix_t){col, row, (mpc_t *)malloc(sizeof(mpc_t)*col*row)};
	
	for(long i = 0; i < row*col; i++) {
		mpc_init2(rtn->data[i], PRECISION);
		//rtn->data[i] = 0.0f;
	}
	return rtn;
}

void destroy_matrix(matrix_t * M) {
	if (M) {
		for(long i = 0; i < M->row*M->col; i++) {
			mpc_clear(M->data[i]);
		}
		free(M->data);
		free(M);
	}
}

void set_matrix(matrix_t * M, long col, long row, mpc_t value) {
	if (M) {
		mpc_set(M->data[modd(col, M->col) + modd(row, M->row) * M->col], value, MPC_RNDDD);
	}
}
void get_matrix(matrix_t * M, long col, long row, mpc_t rtn) {
	if (M) {
		mpc_set(rtn, M->data[modd(col, M->col) + modd(row, M->row) * M->col], MPC_RNDDD);
	}
}

matrix_t * super_matrix(matrix_t * M, long c_off, long r_off, long col, long row) {
	if (M) {
		matrix_t * rtn = make_matrix(col, row);
		
		mpc_t temp; mpc_init2(temp, PRECISION);
		
		for(long i = 0; i < row; i++) {
			for(long j = 0; j < col; j++) {
				
				get_matrix(M, c_off + j, r_off + i, temp);
				set_matrix(rtn, j, i, temp);
				
			}
		}
		
		mpc_clear(temp);
		
		return rtn;
	}
	return NULL;
}

void get_col_matrix(matrix_t * M, long col, mpc_t  * data) {
	if (M) {
		for(long i = 0; i < M->row; i++) {
			get_matrix(M, col, i, data[i]);
		}
	}
}
void get_row_matrix(matrix_t * M, long row, mpc_t  * data) {
	if (M) {
		for(long j = 0; j < M->col; j++) {
			get_matrix(M, j, row, data[j]);
		}
	}
}

void set_col_matrix(matrix_t * M, long col, mpc_t  * data) {
	if (M) {
		for(long i = 0; i < M->row; i++) {
			set_matrix(M, col, i, data[i]);
		}
	}
}

void set_row_matrix(matrix_t * M, long row, mpc_t  * data) {
	if (M) {
		for(long j = 0; j < M->col; j++) {
			set_matrix(M, j, row, data[j]);
		}
	}
}

void determinate(matrix_t * M, mpc_t rtn) {
	
	if (M && M->col == M->row) {
		long l = M->col;
		long m = l-1;
		
		if (l < 2) {
			
			get_matrix(M, 0, 0, rtn);
			
		} else if (l == 2) {
			
			//return (get_matrix(M, 0, 0) * get_matrix(M, 1, 1)) - (get_matrix(M, 0, 1) * get_matrix(M, 1, 0));
			
			mpc_t a, b;
			
			mpc_init2(a, PRECISION);
			mpc_init2(b, PRECISION);
			
			get_matrix(M, 0, 0, rtn);
			get_matrix(M, 1, 1, b);
			mpc_mul(rtn, rtn, b, MPC_RNDDD);
			
			get_matrix(M, 0, 1, a);
			get_matrix(M, 1, 0, b);
			mpc_mul(a, a, b, MPC_RNDDD);
			
			mpc_sub(rtn, rtn, a, MPC_RNDDD);
			mpc_clear(a);
			mpc_clear(b);
			
		} else {
			
			matrix_t * temp = super_matrix(M,0,0,l,l);
			
			mpc_t * F = (mpc_t *) malloc(sizeof(mpc_t) * l);
			mpc_t * Q = (mpc_t *) malloc(sizeof(mpc_t) * l);
			
			for(long i = 0; i < l; i++) {
				mpc_init2(F[i], PRECISION);
				mpc_init2(Q[i], PRECISION);
			}
			
			mpc_t A; mpc_init2(A, PRECISION);
			mpc_t B; mpc_init2(B, PRECISION);
			mpc_t S; mpc_init2(S, PRECISION);
			mpc_t T; mpc_init2(T, PRECISION);
			
			for(long i = 0; i < m; i++) {
					
				get_row_matrix(temp, i, F);
				
				mpc_set(A, F[i], MPC_RNDDD);
				
				// A Zero Fix
				//if (A == 0.0) {
				if (!mpc_cmp_si(A, 0)){
					long a;
					for(a = 0; a < l; a++) {
						get_matrix(temp, i, a, T);
						if (i != a && mpc_cmp_si(T, 0)) {
							break;
						}
					}
					
					if (a < l) {
						get_row_matrix(temp, a, Q);
						for(long b = 0; b < l; b++) {
							mpc_add(F[i], F[i], Q[i], MPC_RNDDD);
						}
						set_row_matrix(temp, i, F);
						
					} else {
						printf("No Solution!?!?\n");
						destroy_matrix(temp);
						return;
					}
					
				}

				
				for(long j = i+1; j < l; j++) {
					
					get_row_matrix(temp, j, Q);
					
					mpc_set(B, Q[i], MPC_RNDDD);
					
					if(!mpc_cmp_si(B,0)) continue;
					
					mpc_div(S, B, A, MPC_RNDDD);
					
					for(long k = 0; k < l; k++) {
						
						mpc_mul(T, S, F[k], MPC_RNDDD);
						mpc_sub(Q[k], Q[k], T, MPC_RNDDD);
						
					}
					
					set_row_matrix(temp, j, Q);
				}
				
			}
			
			
			// Return Values
			mpc_set_d(rtn, 1.0, MPC_RNDDD);
			
			for(long i = 0; i < l; i ++) {
				get_matrix(temp, i, i, T);
				mpc_mul(rtn, rtn, T, MPC_RNDDD);
			}
			
			// Cleanup
			mpc_clear(T);
			mpc_clear(S);
			mpc_clear(B);
			mpc_clear(A);
			for(long i = 0; i < l; i++) {
				mpc_clear(F[i]);
				mpc_clear(Q[i]);
			}
			free(F);
			free(Q);
			destroy_matrix(temp);
		}
		
	}
	
}

matrix_t * transpose_matrix(matrix_t * M) {
	if (M) {
		
		mpc_t T; mpc_init2(T, PRECISION);
		
		matrix_t * rtn = make_matrix(M->row, M->col);
		for(long i = 0; i < rtn->row; i++) {
			for(long j = 0; j < rtn->col; j++) {
				get_matrix(M, i, j, T);
				set_matrix(rtn, j, i, T);
				
			}
		}
		mpc_clear(T);
		
		return rtn;
	}
	
	return NULL;
}

matrix_t * mult_matrix(matrix_t * A, matrix_t * B) {
	
	if (A && B && A->row == B->col) {
		
		long l = A->row;
		
		matrix_t * rtn = make_matrix(A->col, B->row);
		
		mpc_t sum; mpc_init2(sum, PRECISION);
		mpc_t T; mpc_init2(T, PRECISION);
		
		mpc_t * c = (mpc_t *) malloc(sizeof(mpc_t) * l);
		mpc_t * r = (mpc_t *) malloc(sizeof(mpc_t) * l);
		for(long i = 0; i < l; i++) {
			mpc_init2(c[i], PRECISION);
			mpc_init2(r[i], PRECISION);
		}
		
		for(long i = 0; i < rtn->row; i++) {
			for(long j = 0; j < rtn->col; j++) {
				
				get_col_matrix(A, j, c);
				get_row_matrix(B, i, r);
				
				mpc_set_d(sum, 0.0, MPC_RNDDD);
				
				for(long k = 0; k < l; k++) {
					mpc_set(T, c[k], MPC_RNDDD);
					mpc_mul(T, T, r[k], MPC_RNDDD);
					mpc_add(sum, sum, T, MPC_RNDDD);
				}
				
				set_matrix(rtn, j, i, sum);
			}
		}
		
		
		for(long i = 0; i < l; i++) {
			mpc_clear(r[i]);
			mpc_clear(c[i]);
		}
		free(r);
		free(c);
		mpc_clear(T);
		mpc_clear(sum);
		
		return rtn;
	}
	
	return NULL;
}

matrix_t * design_matrix(long numPower, matrix_t * x) {
	long numFactors = x->col;
	long numSamples = x->row;
	long XSize = pow(numPower,numFactors);
	matrix_t* X = make_matrix(XSize, numSamples);
	mpc_t prod; mpc_init2(prod, PRECISION); 
	mpc_t T; mpc_init2(T, PRECISION);
	
	for(long i = 0; i < numSamples; i++){
		for(long j = 0; j < XSize; j++){
			mpc_set_d(prod, 1.0,MPC_RNDDD);
			for(long k = 0; k < numFactors; k++){
				get_matrix(x,k,i,T);
				long power = (j / (long)pow(numPower,k))%numPower;
				mpc_pow_si(T,T,power,MPC_RNDDD);
				mpc_mul(prod, prod, T, MPC_RNDDD);
			}
			set_matrix(X, j, i, prod);
		}
	}
	mpc_clear(prod);
	mpc_clear(T);
	return X;
}

void evaluate_matrix(long numPower, matrix_t * variables, matrix_t * design, mpc_t rtn) {
	mpc_t prod; mpc_init2(prod, PRECISION); 
	mpc_t T; mpc_init2(T, PRECISION);
	mpc_set_si(rtn,0,MPC_RNDDD);
	for(long j = 0; j < design->row; j++){
		get_matrix(design,0,j, prod);
		for(long k = 0; k < variables->col; k++){
			get_matrix(variables,k,0,T);
			long power = (j / (long)pow(numPower,k))%numPower;
			mpc_pow_si(T,T,power ,MPC_RNDDD);
			mpc_mul(prod, prod, T, MPC_RNDDD);
		}
		mpc_add(rtn, rtn, prod, MPC_RNDDD);
	}
}



matrix_t * inverse_matrix(matrix_t * M) {
	
	if (M && M->col == M->row) {
		
		long l = M->col;
		long m = l-1;
		
		mpc_t deter; mpc_init2(deter, MPC_RNDDD);
		
		matrix_t * CoFactor = make_matrix(l, l);
		for(long i = 0; i < l; i++) {
			for(long j = 0; j < l; j++) {
				
				matrix_t * temp = super_matrix(M, j+1, i+1, m, m);
				matrix_t * temp2 = super_matrix(temp, -j, -i, m, m);
				
				determinate(temp2, deter);
				
				if ((i+j)%2) mpc_neg(deter, deter, MPC_RNDDD);
				
				set_matrix(CoFactor, j, i, deter);
				
				destroy_matrix(temp2);
				destroy_matrix(temp);
			}
		}
		
		mpc_t * r = (mpc_t *) malloc(sizeof(mpc_t) * l);
		mpc_t * c = (mpc_t *) malloc(sizeof(mpc_t) * l);
		for(long i = 0; i < l; i++) {
			mpc_init2(r[i], PRECISION);
			mpc_init2(c[i], PRECISION);
		}
		
		get_row_matrix(M, 0, r);
		get_row_matrix(CoFactor, 0, c);
		
		mpc_t T; mpc_init2(T, MPC_RNDDD);
		
		mpc_set_si(deter, 0, MPC_RNDDD);
		for(long i = 0; i < l; i++) {
			mpc_mul(T, r[i], c[i], MPC_RNDDD);
			mpc_add(deter, deter, T, MPC_RNDDD);
		}
		
		if (!mpc_cmp_si(deter, 0)) {
			printf("Singularity!\n");
			
			mpc_clear(T);
			mpc_clear(deter);
			
			for(long i = 0; i < l; i++) {
				mpc_clear(r[i]);
				mpc_clear(c[i]);
			}
			free(r);
			free(c);
			
			destroy_matrix(CoFactor);
			return NULL;
		}
		
		
		
		
		matrix_t * rtn = make_matrix(l, l);
		for(long i = 0; i < l; i++) {
			for(long j = 0; j < l; j++) {
				
				get_matrix(CoFactor, i, j, T);
				mpc_div(T, T, deter, MPC_RNDDD);
				set_matrix(rtn, j, i, T);
				
			}
		}
		
		destroy_matrix(CoFactor);
		
		mpc_clear(T);
		mpc_clear(deter);
		
		for(long i = 0; i < l; i++) {
			mpc_clear(r[i]);
			mpc_clear(c[i]);
		}
		free(r);
		free(c);
		
		return rtn;
	}
	
	return NULL;
}





void print_matrix(matrix_t * M) {
	if (M) {
		
		mpc_t T; mpc_init2(T, PRECISION);
		
		for(long i = 0; i < M->row; i++) {
			for(long j = 0; j < M->col; j++) {
				
				get_matrix(M, j, i, T);
				
				char * str = mpc_get_str(10, 4, T, MPC_RNDDD);
				
				printf("%15s", str);
				
				mpc_free_str(str);
				
				printf("%s", j < M->col - 1 ? ", " : "\n");
				
			}
		}
		
		mpc_clear(T);
		
	}
}

