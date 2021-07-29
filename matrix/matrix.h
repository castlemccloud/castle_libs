/**
 * TODO: Make propper description
 */


#ifndef __MATRIX_H
#define __MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
	
	long col;
	long row;
	double * data;
	
} matrix_t;

matrix_t * make_matrix(long col, long row);
void destroy_matrix(matrix_t * M);

void set_matrix(matrix_t * M, long col, long row, double value);
double get_matrix(matrix_t * M, long col, long row);

matrix_t * super_matrix(matrix_t * M, long c_off, long r_off, long col, long row);

void get_col_matrix(matrix_t * M, long col, double * data);
void get_row_matrix(matrix_t * M, long row, double * data);

double determinate(matrix_t * M);

matrix_t * transpose_matrix(matrix_t * M);
matrix_t * mult_matrix(matrix_t * A, matrix_t * B);
matrix_t * inverse_matrix(matrix_t * M);

void print_matrix(matrix_t * M);



/* End of cplusplus */
#ifdef __cplusplus
}
#endif

/* End of __MATRIX_H */
#endif