#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"


int main(void) {
	
	srand(1234567890);
	
	
	matrix_t * Test = make_matrix(64, 64);
	
	mpc_t T; mpc_init2(T, PRECISION);
	
	for(long i = 0; i < Test->row; i++) {
		for(long j = 0; j < Test->col; j++) {
			
			mpc_set_d(T, (((double)(rand() % 1000) / 500.0) - 1.0), MPC_RNDDD);
			
			set_matrix(Test, j, i, T);
		}
	}
	
	mpc_clear(T);
	
	
	
	printf("Test:\n");
	print_matrix(Test);
	
	matrix_t * Inv = inverse_matrix(Test);
	printf("Inv:\n");
	print_matrix(Inv);
	
	matrix_t * Ident = mult_matrix(Inv, Test);
	printf("Ident:\n");
	print_matrix(Ident);
	
	destroy_matrix(Test);
	destroy_matrix(Inv);
	destroy_matrix(Ident);
	
	
	return 0;
}