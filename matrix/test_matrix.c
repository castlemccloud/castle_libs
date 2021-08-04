#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"


int main(void) {
	
	srand(time(NULL));
	
	
	matrix_t * Test = make_matrix(64, 64);
	for(long i = 0; i < Test->row; i++) {
		for(long j = 0; j < Test->col; j++) {
			set_matrix(Test, j, i, (((double)(rand() % 1000) / 500.0) - 1.0));
		}
	}
	
	
	
	
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