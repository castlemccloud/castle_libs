#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"

int main(void)
{
	
	srand(1234567890);
	
	long numFactors = 2;
	long numPower = 2;
	long numSamples = 2*pow(numPower,numFactors);
	
	matrix_t* b = make_matrix(1,pow(numPower,numFactors));
	for(long i = 0; i < b->row; i++){
		set_matrix(b, 0, i,  (rand() % 1000) / 50.0f - 10.0f);
	}
	printf("b\n");
	print_matrix(b);
	
	matrix_t* x = make_matrix(numFactors, numSamples);
	for(long i = 0; i < numSamples; i++){
		for(long j = 0; j < numFactors; j++){
			set_matrix(x, j, i,  (rand() % 1000) / 50.0f - 10.0f);
		}
	}
	//printf("x\n");
	//print_matrix(x);
	
	matrix_t* Y = make_matrix(1,numSamples);
	for(long i = 0; i< numSamples; i++){
		set_matrix(Y, 0, i,  evaluate_matrix(numPower, super_matrix(x, 0, i, numFactors, 1), b));
	}
	//printf("Y\n");
	//print_matrix(Y);
	
	matrix_t* X = design_matrix(numPower, x);
	//printf("X\n");
	//print_matrix(X);
	matrix_t* XT = transpose_matrix(X);
	//printf("XT\n");
	//print_matrix(XT);
	matrix_t* temp = mult_matrix(X,XT);
	//printf("temp\n");
	//print_matrix(temp);
	matrix_t* Xi = inverse_matrix(temp);
	//printf("Xi\n");
	//print_matrix(Xi);
	matrix_t* temp2 = mult_matrix(Y,XT);
	//printf("temp2\n");
	//print_matrix(temp2);
	matrix_t* B = mult_matrix(temp2, Xi);
	printf("B\n");
	print_matrix(B);
	
	double TotalErr = 0;
	for(long i = 0; i < numSamples; i++){
		double val = get_matrix(Y, 0, i) - evaluate_matrix(numPower, super_matrix(x, 0, i, numFactors, 1), B);
		TotalErr += val*val;
		
	}
	printf("Err %f\n",pow(TotalErr,0.5));
	
	return 0;
}
