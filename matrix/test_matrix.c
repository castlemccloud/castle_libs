#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "matrix.h"


int main(void) {
	
	srand(1234567890);
	
	/*
	// X-axis
	double pitch = (((rand() % 2000) / 1000.0) - 1.0) * M_PI;
	
	matrix_t * Rx = make_matrix(4, 4);
	mpc_set_d(get_matrix(Rx, 0, 0), 1.0, MPC_RNDDD);
	mpc_set_d(get_matrix(Rx, 1, 1), cos(pitch), MPC_RNDDD);
	mpc_set_d(get_matrix(Rx, 2, 1), -sin(pitch), MPC_RNDDD);
	mpc_set_d(get_matrix(Rx, 1, 2), sin(pitch), MPC_RNDDD);
	mpc_set_d(get_matrix(Rx, 2, 2), cos(pitch), MPC_RNDDD);
	mpc_set_d(get_matrix(Rx, 3, 3), 1.0, MPC_RNDDD);
	
	// Y-axis
	double yaw = (((rand() % 2000) / 1000.0) - 1.0) * M_PI;
	
	matrix_t * Ry = make_matrix(4, 4);
	mpc_set_d(get_matrix(Ry, 0, 0), cos(yaw), MPC_RNDDD);
	mpc_set_d(get_matrix(Ry, 2, 0), sin(yaw), MPC_RNDDD);
	mpc_set_d(get_matrix(Ry, 1, 1), 1.0, MPC_RNDDD);
	mpc_set_d(get_matrix(Ry, 0, 2), -sin(yaw), MPC_RNDDD);
	mpc_set_d(get_matrix(Ry, 2, 2), cos(yaw), MPC_RNDDD);
	mpc_set_d(get_matrix(Ry, 3, 3), 1.0, MPC_RNDDD);
	
	// Z-axis
	double roll = (((rand() % 2000) / 1000.0) - 1.0) * M_PI;
	
	matrix_t * Rz = make_matrix(4, 4);
	mpc_set_d(get_matrix(Rz, 0, 0), cos(roll), MPC_RNDDD);
	mpc_set_d(get_matrix(Rz, 1, 0), -sin(roll), MPC_RNDDD);
	mpc_set_d(get_matrix(Rz, 0, 1), sin(roll), MPC_RNDDD);
	mpc_set_d(get_matrix(Rz, 1, 1), cos(roll), MPC_RNDDD);
	mpc_set_d(get_matrix(Rz, 2, 2), 1.0, MPC_RNDDD);
	mpc_set_d(get_matrix(Rz, 3, 3), 1.0, MPC_RNDDD);


	matrix_t * tmp = mult_matrix(Rz, Ry);
	matrix_t * Rotation = mult_matrix(tmp, Rx);

	print_matrix(Rotation);

	clock_t start = clock();
	matrix_t * inverse_Rotation = inverse_matrix(Rotation);
	clock_t end = clock();
	printf("Inverse time: %ld\n", end - start);


	matrix_t * ident = mult_matrix(Rotation, inverse_Rotation);
	
	printf("Ident:\n");
	print_matrix(ident);
	
	
	destroy_matrix(ident);
	destroy_matrix(inverse_Rotation);
	destroy_matrix(Rotation);
	destroy_matrix(tmp);
	destroy_matrix(Rz);
	destroy_matrix(Ry);
	destroy_matrix(Rx);
	*/



	for(long l = 3; l <= 64; l++) {
		matrix_t * temp = make_matrix(l, l);
		for(long i = 0; i < l; i++) {
			for(long j = 0; j < l; j++) {
				mpc_set_d(get_matrix(temp, j, i), (rand() % 2000 / 1000.0 - 1.0), MPC_RNDDD);
			}
		}

		clock_t start = clock();
		matrix_t * pmet = inverse_matrix(temp);
		clock_t end = clock();
		printf("Inverse time for %ld,%ld matrix: %ld\n", l, l, end - start);

		matrix_t * ident = mult_matrix(temp, pmet);

		//printf("Temp: \n");
		//print_matrix(temp);

		//printf("pmeT: \n");
		//print_matrix(pmet);

		//printf("Ident: \n");
		//print_matrix(ident);

		destroy_matrix(temp);
		destroy_matrix(pmet);
		destroy_matrix(ident);
	}




	/*
	matrix_t * A = make_matrix(3, 3);
	mpc_set_d(get_matrix(A, 0, 0), 2.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 1, 0), 3.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 2, 0), 5.0, MPC_RNDDD);

	mpc_set_d(get_matrix(A, 0, 1), 7.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 1, 1), 11.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 2, 1), 13.0, MPC_RNDDD);

	mpc_set_d(get_matrix(A, 0, 2), 17.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 1, 2), 19.0, MPC_RNDDD);
	mpc_set_d(get_matrix(A, 2, 2), 23.0, MPC_RNDDD);


	matrix_t * iA = inverse_matrix(A);

	matrix_t * ident = mult_matrix(A, iA);


	printf("A:\n");
	print_matrix(A);

	printf("iA:\n");
	print_matrix(iA);

	printf("ident:\n");
	print_matrix(ident);


	destroy_matrix(A);
	destroy_matrix(iA);
	destroy_matrix(ident);
	*/



	return 0;
}
