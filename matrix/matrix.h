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
#include <stdint.h>
#include <complex.h>

#include <mpc.h>

#define PRECISION 64

typedef struct {
	
	long col;
	long row;
	mpc_t * data;
	
} matrix_t;



/**
 * Constructor and destructor for matrix_t * objects
 * 
 * The parameters col and row determine the matrix's
 * Columns and Rows respectively.
 * 
 * Be sure to destroy every matrix to prevent memory leaks.
 */
matrix_t * make_matrix(long col, long row);
void destroy_matrix(matrix_t * M);



/**
 * The get and set functions get and set values longo a matrix
 * at the given column and row.
 * 
 * Indexies will wrap around the edges of the matrix, 
 * so all values, including negative values are valid.
 */
#define get_matrix(M, c, r) M->data[c + r*M->col]

//void set_matrix(matrix_t * M, long col, long row, mpc_t value);
//void get_matrix(matrix_t * M, long col, long row, mpc_t rtn);

/**
 * Super matrix is to get a subset of a matrix.
 * It will also loop around the edges of the matrix.
 * So it will repeat the inital matrix if the desired one is larger.
 * 
 * c_off is the offset to the columns for the start of the new matrix.
 * r_off is the offset to the rows for the start of the new matrix.
 * 
 * row and col describe the new matrix's size.
 */
matrix_t * super_matrix(matrix_t * M, long c_off, long r_off, long col, long row);

/**
 * The get col, and row functions will pull values from the specified
 * column or row and place them longo the array data.
 * 
 * User is responsible for ensuring data has enough space.
 */
void get_col_matrix(matrix_t * M, long col, mpc_t * data);
void get_row_matrix(matrix_t * M, long row, mpc_t * data);

void set_col_matrix(matrix_t * M, long col, mpc_t * data);
void set_row_matrix(matrix_t * M, long row, mpc_t * data);


/**
 * Calculates the determinate of a matrix.
 * Size does not matter. Go big.
 */
void determinate(matrix_t * M, mpc_t rtn);

/**
 * Transposes the given matrix longo a new matrix.
 */
matrix_t * transpose_matrix(matrix_t * M);

/**
 * Multiplies two matrixies longo a new matrix
 */
matrix_t * mult_matrix(matrix_t * A, matrix_t * B);
	
/**
 * Expands a matrix for use of linear regression models
 */
matrix_t *design_matrix(long numPower, matrix_t *x);
void evaluate_matrix(long numPower, matrix_t * variables, matrix_t * design, mpc_t rtn);

/**
 * Calculates the inverse of a given matrix.
 */
matrix_t * inverse_matrix(matrix_t * M);


/**
 * prints out the data in a matrix.
 */
void print_matrix(matrix_t * M);



/* End of cplusplus */
#ifdef __cplusplus
}
#endif

/* End of __MATRIX_H */
#endif
