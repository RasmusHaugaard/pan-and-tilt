#include "matrix.h"
#include <stdlib.h>

matrix_ptr new_matrix(INT8U nrow, INT8U ncol)
{
    INT16U size = nrow * ncol;
    matrix_ptr m = malloc(sizeof(matrix));
    MAT_DAT_TYPE* data = malloc(sizeof(MAT_DAT_TYPE) * size);
    m->nrow = nrow;
    m->ncol = ncol;
    m->size = size;
    m->data = data;
    return m;
}

matrix_ptr new_matrix_zero(INT8U nrow, INT8U ncol)
{
    matrix_ptr m = new_matrix(nrow, ncol);
    set_matrix_all(m, 0);
    return m;
}

matrix_ptr new_matrix_ident(INT8U nrow, INT8U ncol)
{
    matrix_ptr m = new_matrix(nrow, ncol);
    ident_matrix(m);
    return m;
}

void matrix_cpy(matrix_ptr dest, matrix_ptr src)
{
    for (INT16U i = 0; i < dest->size; i++)
        dest->data[i] = src->data[i];
}

FP32 get_matrix(matrix_ptr m, INT8U r, INT8U c)
{
    return m->data[(r * m->ncol) + c];
}

void set_matrix(matrix_ptr m, INT8U r, INT8U c, MAT_DAT_TYPE val)
{
    m->data[(r * m->ncol) + c] = val;
}

void set_matrix_all(matrix_ptr m, MAT_DAT_TYPE val)
{
    for (INT16U i = 0; i < m->size; i++)
        m->data[i] = val;
}

void ident_matrix(matrix_ptr m)
{
    INT16U s = 0;
    for (INT8U r = 0; r < m->nrow; r++)
        for (INT8U c = 0; c < m->ncol; c++, s++)
            m->data[s] = r == c;
}

void add_matrix(matrix_ptr d, matrix_ptr a, matrix_ptr b)
{
    for (INT16U i = 0; i < a->size; i++)
        d->data[i] = a->data[i] + b->data[i];
}

void sub_matrix(matrix_ptr d, matrix_ptr a, matrix_ptr b)
{
    for (INT16U i = 0; i < a->size; i++)
        d->data[i] = a->data[i] - b->data[i];
}

void mul_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b)
{
    for (INT8U dr = 0; dr < dest->nrow; dr++){
        for (INT8U dc = 0; dc < dest->ncol; dc++)
        {
            FP32 sum = 0;
            for (INT8U ac = 0; ac < a->ncol; ac++)
                sum += a->data[dr * a->ncol + ac] * b->data[ac * b->ncol + dc];
            dest->data[dr * dest->ncol + dc] = sum;
        }
    }
}

void mul_add_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b)
{
    for (INT8U dr = 0; dr < dest->nrow; dr++){
        for (INT8U dc = 0; dc < dest->ncol; dc++)
        {
            FP32 sum = 0;
            for (INT8U ac = 0; ac < a->ncol; ac++)
                sum += a->data[dr * a->ncol + ac] * b->data[ac * b->ncol + dc];
            dest->data[dr * dest->ncol + dc] += sum;
        }
    }
}

void mul_sub_matrix(matrix_ptr dest, matrix_ptr a, matrix_ptr b)
{
    for (INT8U dr = 0; dr < dest->nrow; dr++){
        for (INT8U dc = 0; dc < dest->ncol; dc++)
        {
            FP32 sum = 0;
            for (INT8U ac = 0; ac < a->ncol; ac++)
                sum += a->data[dr * a->ncol + ac] * b->data[ac * b->ncol + dc];
            dest->data[dr * dest->ncol + dc] -= sum;
        }
    }
}

void mul_const_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c)
{
    for (INT16U i = 0; i < src->size; i++)
        dest->data[i] = src->data[i] * c;
}

void mul_const_add_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c)
{
    for (INT16U i = 0; i < src->size; i++)
        dest->data[i] += src->data[i] * c;
}

void mul_const_sub_matrix(matrix_ptr dest, matrix_ptr src, MAT_DAT_TYPE c)
{
    for (INT16U i = 0; i < src->size; i++)
        dest->data[i] -= src->data[i] * c;
}

void transp_matrix(matrix_ptr dest, matrix_ptr src)
{
    for (INT8U r = 0; r < src->nrow; r++)
        for (INT8U c = 0; c < src->ncol; c++)
            dest->data[c*dest->ncol + r] = src->data[r*src->ncol + c];
}
