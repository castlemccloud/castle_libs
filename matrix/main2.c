#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"
float fun(float a, float b, float c){
	
	return 136 + 53*a + 143*b +  132*a*b + 161*c + 149*a*c + 3*b*c + 0 *a*b*c;
}

int main(void)
{
	
	srand(1234567890);
	
	int numFactors = 3;
	
	int numPower = 3;
	
	int numSamples = 2 * pow(numPower,numFactors);
	
	matrix_t* x = make_matrix(numFactors, numSamples);
	
	for(int i = 0; i < numSamples; i++){
		for(int j = 0; j < numFactors; j++){
			set_matrix(x, j, i,  (rand() % 1000) / 500.0f - 1.0f);
		}
	}
	printf("x\n");
	print_matrix(x);
	
	matrix_t* Y = make_matrix(1,numSamples);
	
	for(int i = 0; i< numSamples; i++){
		Y->data[i] = fun(x->data[i*x->col + 0],x->data[i*x->col + 1],x->data[i*x->col + 2]);
	}
	printf("Y\n");
	print_matrix(Y);
	matrix_t* X = design_matrix(numPower, x);
	matrix_t* XT = transpose_matrix(X);
	matrix_t* temp = mult_matrix(X,XT);
	matrix_t* Xi = inverse_matrix(temp);
	matrix_t* temp2 = mult_matrix(Y,XT);
	matrix_t* B = mult_matrix(temp2, Xi);
	printf("B\n");
	print_matrix(B);
	
	return 0;
}
