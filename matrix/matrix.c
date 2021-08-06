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
	*rtn = (matrix_t){col, row, (mpfr_t *)malloc(sizeof(mpfr_t)*col*row)};
	for(long i = 0; i < row*col; i++) {
		mpfr_init2 (rtn->data[i], MPFR_PRECISION);
		mpfr_set_d (rtn->data[i], 0.0, MPFR_RNDD);
		//rtn->data[i] = 0.0f;
	}
	return rtn;
}

void destroy_matrix(matrix_t * M) {
	if (M) {
		for(long i = 0; i < row*col; i++) {
			mpfr_clear (M->data[i]);
		}
		free(M->data);
		free(M);
	}
}

void set_matrix(matrix_t * M, long col, long row, mpfr_t value) {
	if (M) {
		col = modd(col, M->col);
		row = modd(row, M->row);
		mpfr_set (M->data[col + row * M->col], value, MPFR_RNDD);
		//M->data[col + row * M->col] = value;
	}
}
mpfr_t get_matrix(matrix_t * M, long col, long row) {
	if (M) {
		col = modd(col, M->col);
		row = modd(row, M->row);
		return M->data[col + row * M->col];
	}
	return 0.0;
}

matrix_t * super_matrix(matrix_t * M, long c_off, long r_off, long col, long row) {
	if (M) {
		matrix_t * rtn = make_matrix(col, row);
		
		for(long i = 0; i < row; i++) {
			for(long j = 0; j < col; j++) {
				
				//rtn->data[j + i*col] = M->data[modd(j + c_off, M->col) + modd(i + r_off, M->row)*M->col];
				set_matrix(rtn, j, i, get_matrix(M, c_off + j, r_off + i));
			}
		}
		
		return rtn;
	}
	return NULL;
}

void get_col_matrix(matrix_t * M, long col, mpfr_t  * data) {
	if (M) {
		col = modd(col, M->col);
		
		for(long i = 0, j = 0; i < M->row; i++, j += M->col) {
			
			//data[i] = M->data[col + j];
			//data[i] = get_matrix(M, col, i);
			mpfr_set (data[i], get_matrix(M, col, i), MPFR_RNDD);
		}
	}
}
void get_row_matrix(matrix_t * M, long row, mpfr_t  * data) {
	if (M) {
		row = modd(row, M->row);
		row *= M->col;
		
		for(long j = 0; j < M->col; j++) {
			
			//data[j] = M->data[j + row];
			//data[j] = get_matrix(M, j, row);
			mpfr_set (data[i], get_matrix(M, j, row), MPFR_RNDD);
		}
	}
}

void set_col_matrix(matrix_t * M, long col, mpfr_t  * data) {
	if (M) {
		col = modd(col, M->col);
		
		for(long i = 0; i < M->row; i++) {
			
			//M->data[col + i*M->col] = data[i];
			set_matrix(M, col, i, data[i]);
			
		}
	}
}

void set_row_matrix(matrix_t * M, long row, mpfr_t  * data) {
	if (M) {
		row = modd(row, M->row);
		row *= M->row;
		
		for(long j = 0; j < M->col; j++) {
			
			//M->data[j + row] = data[j];
			set_matrix(M, j, row, data[j]);
		}
	}
}

mpfr_t determinate(matrix_t * M) {
	
	if (M && M->col == M->row) {
		long l = M->col;
		long m = l-1;
		
		if (l < 2) {
			
			return get_matrix(M, 0, 0);
			
		} else if (l == 2) {
			
			//return (get_matrix(M, 0, 0) * get_matrix(M, 1, 1)) - (get_matrix(M, 0, 1) * get_matrix(M, 1, 0));
			
			mpfr_t a, b;
			mpfr_inits (a, b);
			mpfr_set (a, get_matrix(M, 0, 0), MPFR_RNDD);
			mpfr_mul (a, a, get_matrix(M, 1, 1), MPFR_RNDU);
			
			mpfr_set (b, get_matrix(M, 0, 1), MPFR_RNDD);
			mpfr_mul (b, b, get_matrix(M, 1, 0), MPFR_RNDU);
			
			mpfr_sub (a, a, b, MPFR_RNDU);
			mpfr_clear (b);
			
			return a;
			
		} else {
			
			//double sum = 0;
			mpfr_t sum;
			mpfr_init_set_ui (sum, 0,MPFR_RNDD);
			for(long i = 0; i < M->row*M->col; i += M->col) {
				//sum += M->data[i];
				mpfr_add (sum, sum, M->data[i], MPFR_RNDD);
			}
			//if (sum == 0.0) return 0.0;
			if (! mpfr_cmp_ui(sum,0)) return sum;
			mpfr_clear (sum);

			//matrix_t * temp = make_matrix(l, l);
			//for(long i = 0; i < l*l; i++) temp->data[i] = M->data[i];
			
			matrix_t * temp = super_matrix(M,0,0,l,l);
			
			mpfr_t F[l];
			mpfr_t Q[l];
			
			for(long i = 0; i < m; i++) {
					
				get_row_matrix(temp, i, F);
				//double A = F[i];
				mpfr_t A;
				mpfr_init_set (A, F[i],MPFR_RNDD);
				
				// A Zero Fix
				//if (A == 0.0) {
				if (! mpfr_cmp_ui(A,0)){
					long a;
					for(a = 0; a < l; a++) {
						//if (i != a && get_matrix(temp, i, a) != 0.0) {
						if (i != a && mpfr_cmp_ui(get_matrix(temp, i, a),0) ) {
							break;
						}
					}
					
					if (a < l) {
						get_row_matrix(temp, a, Q);
						for(long b = 0; b < l; b++) {
							//F[i] += Q[i];
							mpfr_add (F[i], F[i], Q[i], MPFR_RNDD);
						}
						set_row_matrix(temp, i, F);
						
					} else {
						printf("No Solution!?!?\n");
						destroy_matrix(temp);
						return A;
					}
					
				}
/*
*--------------------------------------------------------------------------------STOPPED_HERE----------------------------
*/
				for(long j = i+1; j < l; j++) {
					//long b = j*temp->col;
					
					//for(long a = 0; a < l; a++) {
						//Q[a] = temp->data[a + b];
					//}
					get_row_matrix(temp, j, Q);
					
					mpfr_t B = Q[i];
					
					//if (B == 0.0) continue;
					if(! mpfr_cmp_ui(Q[i],0))continue
					
					double S = (B / A);
					mpfr_t S;
					mpfr_init(S);
					mpfr_div(S, B, A, MPFR_RNDD);
					
					for(long k = 0; k < l; k++) {
						Q[k] = Q[k] - S*F[k];
					}
					
					for(long a = 0; a < l; a++) {
						temp->data[a + b] = Q[a];
					}
				}
			}
			
			long double product = 1.0;
			for(long i = 0; i < temp->row*temp->col; i += temp->col+1) {
				
				product *= temp->data[i];
				
			}
			
			destroy_matrix(temp);
			
			
			return product;
			
		}
		
	}
	return 0.0;
}

matrix_t * transpose_matrix(matrix_t * M) {
	if (M) {
		
		matrix_t * rtn = make_matrix(M->row, M->col);
		for(long i = 0; i < rtn->row; i++) {
			for(long j = 0; j < rtn->col; j++) {
				
				rtn->data[j + i*rtn->col] = M->data[i + j * M->col];
				
				//set_matrix(rtn, j, i, get_matrix(M, i, j));
				
			}
		}
		return rtn;
	}
	
	return NULL;
}

matrix_t * mult_matrix(matrix_t * A, matrix_t * B) {
	
	if (A && B && A->row == B->col) {
		
		long l = A->row;
		
		matrix_t * rtn = make_matrix(A->col, B->row);
		
		for(long i = 0; i < rtn->row; i++) {
			for(long j = 0; j < rtn->col; j++) {
				
				double c[l]; get_col_matrix(A, j, c);
				double r[l]; get_row_matrix(B, i, r);
				
				double sum = 0.0;
				for(long k = 0; k < l; k++) {
					sum += c[k]*r[k];
				}
				
				set_matrix(rtn, j, i, sum);
			}
		}
		
		return rtn;
	}
	
	return NULL;
}

matrix_t * design_matrix(long numPower, matrix_t * x) {
	long numFactors = x->col;
	long numSamples = x->row;
	long XSize = pow(numPower,numFactors);
	matrix_t* X = make_matrix(XSize, numSamples);
	
	for(long i = 0; i < numSamples; i++){
		for(long j = 0; j < XSize; j++){
			double prod = 1;
			for(long k = 0; k < numFactors; k++){
				prod *= pow(get_matrix(x,k,i), (j / (long)pow(numPower,k))%numPower);
			}
			set_matrix(X, j, i, prod);
		}
	}
	return X;
}

double evaluate_matrix(long numPower, matrix_t * variables, matrix_t * design) {
	double sum = 0;
	for(long j = 0; j < design->row; j++){
		double prod = get_matrix(design,0,j);
		for(long k = 0; k < variables->col; k++){
			prod *= pow(get_matrix(variables,k,0), (j / (long)pow(numPower,k))%numPower);
		}
		sum += prod;
	}
	
	return sum;
}

matrix_t * inverse_matrix(matrix_t * M) {
	
	if (M && M->col == M->row) {
		
		long l = M->col;
		long m = l-1;
		
		matrix_t * CoFactor = make_matrix(l, l);
		for(long i = 0; i < l; i++) {
			for(long j = 0; j < l; j++) {
				
				matrix_t * temp = super_matrix(M, j+1, i+1, m, m);
				matrix_t * temp2 = super_matrix(temp, -j, -i, m, m);
				
				set_matrix(CoFactor, j, i, ((i+j)%2 ? -1.0 : 1.0) * determinate(temp2));
				
				destroy_matrix(temp2);
				destroy_matrix(temp);
			}
		}
		
		double deter = 0.0;
		double r[l]; get_row_matrix(M, 0, r);
		double c[l]; get_row_matrix(CoFactor, 0, c);
		for(long i = 0; i < l; i++) {
			deter += r[i]*c[i];
		}
		if (deter == 0.0) {
			printf("Singularity!\n");
			destroy_matrix(CoFactor);
			return NULL;
		}
		
		double ideter = 1.0 / deter;
		
		matrix_t * rtn = make_matrix(l, l);
		for(long i = 0; i < l; i++) {
			for(long j = 0; j < l; j++) {
				
				set_matrix(rtn, j, i, get_matrix(CoFactor, i, j) * ideter);
				
			}
		}
		
		destroy_matrix(CoFactor);
		
		
		return rtn;
	}
	
	return NULL;
}

void print_matrix(matrix_t * M) {
	if (M) {
		
		for(long i = 0; i < M->row; i++) {
			long l = M->col - 1;
			printf("[");
			for(long j = 0; j < M->col; j++) {
				printf("%f%c", get_matrix(M, j, i), (j == l ? ']' : '\t'));
			}
			printf("\n");
		}
		
	}
}
