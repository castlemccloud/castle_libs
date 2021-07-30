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
	*rtn = (matrix_t){col, row, (double *)malloc(sizeof(double)*col*row)};
	for(long i = 0; i < row*col; i++) {
		rtn->data[i] = 0.0f;
	}
	return rtn;
}

void destroy_matrix(matrix_t * M) {
	if (M) {
		free(M->data);
		free(M);
	}
}

void set_matrix(matrix_t * M, long col, long row, double value) {
	if (M) {
		col = modd(col, M->col);
		row = modd(row, M->row);
		M->data[col + row * M->col] = value;
	}
}
double get_matrix(matrix_t * M, long col, long row) {
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
				set_matrix(rtn, j, i, get_matrix(M, c_off + j, r_off + i));
			}
		}
		
		return rtn;
	}
	return NULL;
}

void get_col_matrix(matrix_t * M, long col, double * data) {
	if (M) {
		for(long i = 0; i < M->row; i++) {
			data[i] = get_matrix(M, col, i);
		}
	}
}
void get_row_matrix(matrix_t * M, long row, double * data) {
	if (M) {
		for(long j = 0; j < M->col; j++) {
			data[j] = get_matrix(M, j, row);
		}
	}
}

double determinate(matrix_t * M) {
	
	if (M && M->col == M->row) {
		long l = M->col;
		
		if (l < 2) {
			
			return 0.0;
			
		} else if (l == 2) {
			
			return (get_matrix(M, 0, 0) * get_matrix(M, 1, 1)) - (get_matrix(M, 0, 1) * get_matrix(M, 1, 0));
			
			
		} else {
			
			matrix_t * temp = make_matrix(l, l);
			for(long i = 0; i < l*l; i++) temp->data[i] = M->data[i];
			
			double F[l];
			double Q[l];
			
			for(long i = 0; i < m; i++) {
			    
			    get_row_matrix(temp, i, F);
			    double A = F[i];
			    
			    for(long j = i+1; j < l; j++) {
			        
			        get_row_matrix(temp, j, Q);
			        
			        double B = Q[i];
			        double S = -(B / A);
			        
			        for(long k = 0; k < l; k++) {
			            
			            Q[k] = Q[k] + S*F[k];
			            
			        }
			        
			        set_row_matrix(temp, j, Q);
			        
			    }
			    
			}
			
			double product = 1.0;
			
			for(long i = 0; i < l; i++) {
				
				product *= get_matrix(temp, i, i);
				
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
				
				set_matrix(rtn, j, i, get_matrix(M, i, j));
				
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
				printf("% 6.3f%c", get_matrix(M, j, i), (j == l ? ']' : '\t'));
			}
			printf("\n");
		}
		
	}
}
