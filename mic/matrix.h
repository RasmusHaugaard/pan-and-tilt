#ifndef _MATRIX_H
#define _MATRIX_H
#include "emp_type.h"

typedef FP32 MAT_DAT_TYPE;

typedef struct{
    INT8U nrow;
    INT8U ncol;
    INT16U size;
    MAT_DAT_TYPE* data;
} matrix;

typedef matrix* matrix_ptr;

matrix_ptr new_matrix(INT8U nrow, INT8U ncol);
matrix_ptr new_matrix_zero(INT8U nrow, INT8U ncol);
matrix_ptr new_matrix_ident(INT8U nrow, INT8U ncol);

void matrix_cpy(matrix_ptr dest, matrix_ptr src);

FP32 get_matrix(matrix_ptr m, INT8U row, INT8U col);
void set_matrix(matrix_ptr m, INT8U row, INT8U col, MAT_DAT_TYPE val); // (row, col) = val

void set_matrix_all(matrix_ptr m, MAT_DAT_TYPE val);
void ident_matrix(matrix_ptr m);

void add_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b); // dest = a + b
void sub_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b); // dest = a - b
void mul_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b); //dest = a * b
void mul_add_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b); // dest += a * b
void mul_sub_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b); // dest -= a * b
void mul_const_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c); // dest = src * c
void mul_const_add_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c); // dest += src * c
void mul_const_sub_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c); // dest -= src * c
void transp_matrix(matrix_ptr dest, matrix_ptr src);


#endif
