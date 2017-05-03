#ifndef _SS_H
#define _SS_H
#include "emp_type.h"
#include "matrix.h"

typedef struct{
    //constant
    matrix_ptr A;
    matrix_ptr B;
    matrix_ptr C;
    matrix_ptr K;
    matrix_ptr Ke;
    FP32 Nbar;
    FP32 ts;
    //varying
    matrix_ptr r; //reference
    matrix_ptr x; //state
    matrix_ptr y; //output
    matrix_ptr u; //control variables
    //temps
    matrix_ptr Atemp;
    matrix_ptr xtemp;
} ss;

typedef ss* ss_ptr;

ss_ptr new_ss(matrix_ptr A, matrix_ptr B, matrix_ptr C, matrix_ptr K, matrix_ptr Ke, FP32 Nbar, FP32 ts);
void ss_next(ss_ptr s);

#endif
