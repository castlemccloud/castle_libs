#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"

int main(void)
{
	
	srand(1234567890);
	
	long numFactors = 4;
	long numPower = 4;
	long numSamples = 2*pow(numPower,numFactors);
	mpc_t T; mpc_init2(T, PRECISION);
	matrix_t* b = make_matrix(1,pow(numPower,numFactors));
	for(long i = 0; i < b->row; i++){
		mpc_set_d(T, (((double)(rand() % 1000) / 500.0) - 1.0), MPC_RNDDD);
		set_matrix(b, 0, i,  T);
	}
	printf("b\n");
	print_matrix(b);
	
	matrix_t* x = make_matrix(numFactors, numSamples);
	for(long i = 0; i < numSamples; i++){
		for(long j = 0; j < numFactors; j++){
			mpc_set_d(T, (((double)(rand() % 1000) / 500.0) - 1.0), MPC_RNDDD);
			set_matrix(x, j, i,  T);
		}
	}
	printf("x\n");
	print_matrix(x);
	
	matrix_t* Y = make_matrix(1,numSamples);
	for(long i = 0; i< numSamples; i++){
		evaluate_matrix(numPower, super_matrix(x, 0, i, numFactors, 1), b, T);
		set_matrix(Y, 0, i, T);
	}
	printf("Y\n");
	print_matrix(Y);
	
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
	
	mpc_t TotalErr; mpc_init2(TotalErr, PRECISION);
	mpc_set_si(TotalErr, 0, MPC_RNDDD);
	mpc_t val; mpc_init2(val, PRECISION);
	for(long i = 0; i < numSamples; i++){
		evaluate_matrix(numPower, super_matrix(x, 0, i, numFactors, 1), B, T);
		get_matrix(Y, 0, i, val);
		mpc_sub(val,val,T,MPC_RNDDD);
		mpc_mul(val,val,val,MPC_RNDDD);
		mpc_add(TotalErr,TotalErr,val,MPC_RNDDD);
		
	}
	mpc_pow_d(TotalErr,TotalErr,0.5,MPC_RNDDD);
	char * str = mpc_get_str(10, 6, TotalErr, MPC_RNDDD);
	printf("Err %14s\n",str);
	mpc_free_str(str);
	mpc_clear(T);
	mpc_clear(TotalErr);
	mpc_clear(val);
	
	return 0;
}
