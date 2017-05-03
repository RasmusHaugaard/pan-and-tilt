#include "ss_conf.h"

INT8U abe[1001];
INT8U a[1];

ss_ptr ss_tilt_setup()
{
    FP32* f;
    *f = .5;

    matrix_ptr A = new_matrix(3, 3);
    set_matrix(A, 0, 0, -1367.64705882353);
    set_matrix(A, 0, 1, 0);
    set_matrix(A, 0, 2, -149.705882352941);

    set_matrix(A, 1, 0, 0);
    set_matrix(A, 1, 1, 0);
    set_matrix(A, 1, 2, 1);

    set_matrix(A, 2, 0, 52.8190937391906);
    set_matrix(A, 2, 1, 0);
    set_matrix(A, 2, 2, -1.25907990314770);

    matrix_ptr B = new_matrix(3, 1);
    set_matrix(B, 0, 0, 294.117647058824);
    set_matrix(B, 1, 0, 0);
    set_matrix(B, 2, 0, 0);

    matrix_ptr C = new_matrix(1, 3);
    set_matrix(C, 0, 0, 0);
    set_matrix(C, 0, 1, 1);
    set_matrix(C, 0, 2, 0);

    matrix_ptr K = new_matrix(1, 3);
    set_matrix(K, 0, 0, 0.0326625865381244);
    set_matrix(K, 0, 1, 10);
    set_matrix(K, 0, 2, 0.848704060307682);

    matrix_ptr Ke = new_matrix(1, 3);
    set_matrix(Ke, 0, 0, 0);
    set_matrix(Ke, 0, 1, 10);
    set_matrix(Ke, 0, 2, 0);

    FP32 Nbar = 10;

    FP32 ts = 0.01;

    return new_ss(A, B, C, K, Ke, Nbar, ts);
}
