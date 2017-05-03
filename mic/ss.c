#include "ss.h"
#include <stdlib.h>

ss_ptr new_ss(matrix_ptr A, matrix_ptr B, matrix_ptr C, matrix_ptr K, matrix_ptr Ke, FP32 Nbar, FP32 ts)
{
    ss_ptr s = malloc(sizeof(ss));
    //constant
    s->A = A;
    s->B = B;
    s->C = C;
    s->K = K;
    s->Ke = Ke;
    s->Nbar = Nbar;
    s->ts = ts;
    //varying
    s->x = new_matrix_zero(A->ncol, 1);
    s->u = new_matrix_zero(B->ncol, 1);
    s->r = new_matrix_zero(A->ncol, 1);
    s->y = new_matrix_zero(Ke->ncol, 1);
    //temps
    s->Atemp = new_matrix_zero(A->nrow, A->ncol);
    s->xtemp = new_matrix_zero(s->x->nrow, s->x->ncol);
    return s;
}

void ss_next(ss_ptr s)
{
    //update observed state
    //dx/dt = (A-Ke*C)*x + B*u + Ke*y
    mul_matrix(s->Atemp, s->Ke, s->C);
    sub_matrix(s->Atemp, s->A, s->Atemp);
    mul_matrix(s->xtemp, s->Atemp, s->x);
    mul_add_matrix(s->xtemp, s->B, s->u);
    mul_add_matrix(s->xtemp, s->Ke, s->y);
    // x += dx/dt * ts;
    mul_const_add_matrix(s->x, s->xtemp, s->ts);

    //calculate process variable based on observed state
    //u = r*Nbar -K*x
    mul_const_matrix(s->u, s->r, s->Nbar);
    mul_sub_matrix(s->u, s->K, s->x);
}
