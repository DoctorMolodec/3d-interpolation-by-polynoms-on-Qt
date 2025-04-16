#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpolation2.h"
#include "method2_2.h"
#include "method2_1.h"

//интерпол€ци€ в трехмерном постранстве
static double f0(double x, double y)
{
    return x * 0 + y * 0 + 1;
}
static double f1(double x, double y)
{
    return x + y * 0;
}
static double f2(double x, double y)
{
    return x * 0 + y;
}
static double f3(double x, double y)
{
    return x + y;
}
static double f4(double x, double y)
{
    return sqrt(x * x + y * y);
}
static double f5(double x, double y)
{
    return x * x + y * y;
}
static double f6(double x, double y)
{
    return exp(x * x - y * y);
}
static double f7(double x, double y)
{
    return 1 / (25 * x * x + 25 * y * y + 1);
}

interpolation2_ctx interpolation2_create(int method, int n_x, int n_y, int k, double x_a, double x_b, double y_a, double y_b)
{
    //проверка на вмен€емость полученных данных
    interpolation2_ctx res_ptr;
    if (n_x < 3 || n_y < 3 || x_a >= x_b || y_a >= y_b || method < 1 || method > 2)
    {
        return NULL;
    }

    res_ptr = (interpolation2_ctx)malloc(sizeof(*res_ptr));
    if (res_ptr == NULL)
    {
        return NULL;
    }
    res_ptr->method = method;
    res_ptr->k = k;
    res_ptr->n_x = n_x;
    res_ptr->x_a = x_a;
    res_ptr->x_b = x_b;
    res_ptr->n_y = n_y;
    res_ptr->y_a = y_a;
    res_ptr->y_b = y_b;
    res_ptr->h_x = (res_ptr->x_b - res_ptr->x_a) / (res_ptr->n_x - 1);
    res_ptr->h_y = (res_ptr->y_b - res_ptr->y_a) / (res_ptr->n_y - 1);
    res_ptr->G = (double*)malloc((16 * n_x * n_y) * sizeof(double));
    res_ptr->Ax = (double*)malloc(16 * sizeof(double));
    res_ptr->Ay = (double*)malloc(16 * sizeof(double));
    res_ptr->P = (double*)malloc(3 * fmax(n_x, n_y) * sizeof(double));
    res_ptr->Fij = (double*)malloc(16 * sizeof(double));
    if (res_ptr->G == NULL || res_ptr->Ax == NULL || res_ptr->Ay == NULL || res_ptr->Fij == NULL || res_ptr->P == NULL)// || res_ptr->F == NULL || res_ptr->Fx == NULL || res_ptr->Fy == NULL || res_ptr->Fxy == NULL)
    {
        free(res_ptr->Fij);
        free(res_ptr->G);
        free(res_ptr->P);
        free(res_ptr->Ax);
        free(res_ptr->Ay);
        free(res_ptr);
        return NULL;
    }
    switch (k)
    {
    case INTERPOLATION2_F_1:
        res_ptr->f = f0;
        break;
    case INTERPOLATION2_F_X:
        res_ptr->f = f1;
        break;
    case INTERPOLATION2_F_Y:
        res_ptr->f = f2;
        break;
    case INTERPOLATION2_F_XY:
        res_ptr->f = f3;
        break;
    case INTERPOLATION2_F_SQRX2Y2:
        res_ptr->f = f4;
        break;
    case INTERPOLATION2_F_X2Y2:
        res_ptr->f = f5;
        break;
    case INTERPOLATION2_F_EX2Y2:
        res_ptr->f = f6;
        break;
    case INTERPOLATION2_F_1DIVX2Y2:
        res_ptr->f = f7;
        break;
    default:
        free(res_ptr->G);
        free(res_ptr->P);
        free(res_ptr->Ax);
        free(res_ptr->Ay);
        free(res_ptr->Fij);
        free(res_ptr);
        return NULL;
    }
    //матрица јх и ју необходима€ дл€ вычисленний 
    res_ptr->Ax[0] = 1;
    res_ptr->Ax[1] = 0;
    res_ptr->Ax[2] = 0;
    res_ptr->Ax[3] = 0;
    res_ptr->Ax[4] = 0;
    res_ptr->Ax[5] = 1;
    res_ptr->Ax[6] = 0;
    res_ptr->Ax[7] = 0;
    res_ptr->Ax[8] = -3 / (res_ptr->h_x * res_ptr->h_x);
    res_ptr->Ax[9] = -2 / res_ptr->h_x;
    res_ptr->Ax[10] = 3 / (res_ptr->h_x * res_ptr->h_x);
    res_ptr->Ax[11] = -1 / res_ptr->h_x;
    res_ptr->Ax[12] = 2 / (res_ptr->h_x * res_ptr->h_x * res_ptr->h_x);
    res_ptr->Ax[13] = 1 / (res_ptr->h_x * res_ptr->h_x);
    res_ptr->Ax[14] = -2 / (res_ptr->h_x * res_ptr->h_x * res_ptr->h_x);
    res_ptr->Ax[15] = 1 / (res_ptr->h_x * res_ptr->h_x);

    res_ptr->Ay[0] = 1;
    res_ptr->Ay[4] = 0;
    res_ptr->Ay[8] = 0;
    res_ptr->Ay[12] = 0;
    res_ptr->Ay[1] = 0;
    res_ptr->Ay[5] = 1;
    res_ptr->Ay[9] = 0;
    res_ptr->Ay[13] = 0;
    res_ptr->Ay[2] = -3 / (res_ptr->h_y * res_ptr->h_y);
    res_ptr->Ay[6] = -2 / res_ptr->h_y;
    res_ptr->Ay[10] = 3 / (res_ptr->h_y * res_ptr->h_y);
    res_ptr->Ay[14] = -1 / res_ptr->h_y;
    res_ptr->Ay[3] = 2 / (res_ptr->h_y * res_ptr->h_y * res_ptr->h_y);
    res_ptr->Ay[7] = 1 / (res_ptr->h_y * res_ptr->h_y);
    res_ptr->Ay[11] = -2 / (res_ptr->h_y * res_ptr->h_y * res_ptr->h_y);
    res_ptr->Ay[15] = 1 / (res_ptr->h_y * res_ptr->h_y);


    if (method == 1)
    {
        interpolation2_method_1(res_ptr);
    }
    if (method == 2)
    {
        interpolation2_method_2(res_ptr);
    }

    return res_ptr;
}


//здесь вычисл€ем к какому многочлену относитс€ точка и вычисл€ем ее значение
double interpolation2_calculate(interpolation2_ctx ctx, double x, double y)
{
    int i, j;
    double x_i, y_j, delta_x = 0, delta_y = 0, res = 0, tmp = 0;
    int number = 0;
    i = (int)((x - ctx->x_a) / ctx->h_x);
    j = (int)((y - ctx->y_a) / ctx->h_y);
    if (i < 0)
    {
        i = 0;
    }
    if (i > ctx->n_x - 2)
    {
        i = ctx->n_x - 2;
    }
    if (j < 0)
    {
        j = 0;
    }
    if (j > ctx->n_y - 2)
    {
        j = ctx->n_y - 2;
    }

    number = 16 * (i * (ctx->n_y - 1) + j);
    x_i = ctx->x_a + i * ctx->h_x;
    y_j = ctx->y_a + j * ctx->h_y;
    delta_x = x - x_i;
    delta_y = y - y_j;
    for (int i1 = 0; i1 < 4; i1++)
    {
        for (int l = 0; l < 4; l++)
        {
            tmp = tmp * delta_y;
            tmp = tmp + ctx->G[number + (3 - i1) * 4 + 3 - l];
        }

        res = delta_x * res;
        res = res + tmp;
        tmp = 0;
    }


    return res;
}

void interpolation2_destroy(interpolation2_ctx ctx)
{
    free(ctx->G);
    free(ctx->P);
    free(ctx->Ax);
    free(ctx->Ay);
    free(ctx->Fij);
    free(ctx);
}
