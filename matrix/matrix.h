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
 * The get and set functions get and set values into a matrix
 * at the given column and row.
 * 
 * Indexies will wrap around the edges of the matrix, 
 * so all values, including negative values are valid.
 */
void set_matrix(matrix_t * M, long col, long row, double value);
double get_matrix(matrix_t * M, long col, long row);

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
 * column or row and place them into the array data.
 * 
 * User is responsible for ensuring data has enough space.
 */
void get_col_matrix(matrix_t * M, long col, double * data);
void get_row_matrix(matrix_t * M, long row, double * data);


/**
 * Calculates the determinate of a matrix.
 * Size does not matter. Go big.
 */
double determinate(matrix_t * M);

/**
 * Transposes the given matrix into a new matrix.
 */
matrix_t * transpose_matrix(matrix_t * M);

/**
 * Multiplies two matrixies into a new matrix
 */
matrix_t * mult_matrix(matrix_t * A, matrix_t * B);

/**
 * Calculates the inverse of a given matrix.
 */
matrix_t * inverse_matrix(matrix_t * M);


/**
 * Prints out the data in a matrix.
 */
void print_matrix(matrix_t * M);



/* End of cplusplus */
#ifdef __cplusplus
}
#endif

/* End of __MATRIX_H */
#endif