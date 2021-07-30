#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"
float fun(float a, float b, float c){

    return 136 + 53*a + 143*b + 161*c + 132*a*b + 149*a*c + 3*b*c;

}

int main(void)
{
	
	srand(1234567890);

    int numSamples = 5;

    int numFactors = 3;
	
	int numPower = pow(2,numFactors);

    matrix_t* x = make_matrix(numFactors, numSamples);

    for(int i = 0; i < numSamples; i++){

        for(int j = 0; j < numFactors; j++){

            set_matrix(x, j, i,  (rand() % 1000) / 500.0f - 1.0f);

        }

    }
	printf("x\n");
    print_matrix(x);
	
	matrix_t* X = make_matrix(numPower, numSamples);
	for(int i = 0; i < numSamples; i++){
		//printf("I%d\n",i);
        for(int j = 0; j < numPower; j++){
			//printf("J%d\n",j);
			double prod = 1;
			for(int k = 0; k < numFactors; k++){
				//printf("K%d\n",k);
				if((j>>k)&1){
					//printf("MULTPRODBY%f\n",get_matrix(x,k,i));
					prod *= get_matrix(x,k,i);
				}
				//printf("PROD%f\n",prod);
			}
            set_matrix(X, j, i, prod);

        }

    }
	printf("X\n");
	print_matrix(X);
	
    matrix_t* Y = make_matrix(1,numSamples);

    for(int i = 0; i< numSamples; i++){

        Y->data[i] = fun(X->data[i*X->col + 0],X->data[i*X->col + 1],X->data[i*X->col + 2]);

        // set_matrix(Y, 0, i,  (rand() % 1000) / 500.0f - 1.0f);

    }
	printf("Y\n");
    print_matrix(Y);

   

    matrix_t* XT = transpose_matrix(X);
	printf("XT\n");
    print_matrix(XT);

    matrix_t* temp = mult_matrix(XT,X);
	printf("temp\n");
    print_matrix(temp);

    matrix_t* Xi = inverse_matrix(temp);
	printf("Xi\n");
    print_matrix(Xi);

    matrix_t* temp2 = mult_matrix(Y,XT);
	printf("temp2\n");
    print_matrix(temp2);

    matrix_t* B = mult_matrix(Xi,temp2);
	printf("B\n");
    print_matrix(B);
	
	/*
	matrix_t *Test = make_matrix(5, 5);
	for (long i = 0; i < Test->row; i++)
	{
		for (long j = 0; j < Test->col; j++)
		{
			set_matrix(Test, j, i, (((double)(rand() % 1000) / 500.0) - 1.0));
		}
	}

	printf("Test:\n");
	print_matrix(Test);

	matrix_t *Inv = inverse_matrix(Test);
	printf("Inv:\n");
	print_matrix(Inv);

	matrix_t *Ident = mult_matrix(Inv, Test);
	printf("Ident:\n");
	print_matrix(Ident);

	destroy_matrix(Test);
	destroy_matrix(Inv);
	destroy_matrix(Ident);
	*/
	return 0;
}
