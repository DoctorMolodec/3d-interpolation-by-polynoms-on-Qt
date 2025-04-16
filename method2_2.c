#include "method2_2.h"


void create_Gamma(interpolation2_ctx ctx, double h_x, double h_y);
void multMatrix1(interpolation2_ctx ctx, int curr_i, int curr_j);
double F_xx(int k, double x, double y);
double F_yy(int k, double x, double y);


double F_xx(int k, double x, double y)
{
    double res = 0;
    if (k == 0)
    {
        res = 0;
    }
    if (k == 1)
    {
        res = 0;
    }
    if (k == 2)
    {
        res = 0;
    }
    if (k == 3)
    {
        res = 0;
    }
    if (k == 4)
    {
        if ((fabs(x) < 1e-15) && (fabs(y) < 1e-15))
        {
            res = 0;
        }
        else
        {
            res = y * y * sqrt(y * y + x * x) / (pow(y, 4) + 2 * pow(y, 2) * pow(x, 2) + pow(x, 4));
        }
    }
    if (k == 5)
    {
        res = 2;
    }
    if (k == 6)
    {
        res = (4 * x * x + 2) * exp(x * x - y * y);
    }
    if (k == 7)
    {
        res = -50 * (25 * x * x + 25 * y * y + 1 - 100 * x * x) / pow(25 * x * x + 25 * y * y + 1, 3);
    }
    return res;
}



double F_yy(int k, double x, double y)
{
    double res = 0;
    if (k == 0)
    {
        res = 0;
    }
    if (k == 1)
    {
        res = 0;
    }
    if (k == 2)
    {
        res = 0;
    }
    if (k == 3)
    {
        res = 0;
    }
    if (k == 4)
    {
        if ((fabs(x) < 1e-15) && (fabs(y) < 1e-15))
        {
            res = 0;
        }
        else
        {
            res = x * x * sqrt(y * y + x * x) / (pow(y, 4) + 2 * pow(y, 2) * pow(x, 2) + pow(x, 4));
        }
    }
    if (k == 5)
    {
        res = 2;
    }
    if (k == 6)
    {
        res = (4 * y * y - 2) * exp(x * x - y * y);
    }
    if (k == 7)
    {
        res = -50 * (25 * x * x + 25 * y * y + 1 - 100 * y * y) / pow(25 * x * x + 25 * y * y + 1, 3);
    }
    return res;
}




void create_Gamma(interpolation2_ctx ctx, double h_x, double h_y)
{
    for (int i = 0; i < ctx->n_x; i++)
    {
        for (int j = 0; j < ctx->n_y; j++)
        {
            ctx->G[16 * (i * ctx->n_y + j)] = ctx->f(ctx->x_a + h_x * i, ctx->y_a + h_y * j);
        }
    }
    //Fx

    for (int j = 0; j < ctx->n_y; j++)
    {
        ctx->P[3 * 0 + 0] = 0;
        ctx->P[3 * 0 + 1] = 2;
        ctx->P[3 * 0 + 2] = 1;

        ctx->G[j * 16 + 4] =
            3 * (ctx->G[1 * 16 * ctx->n_y + j * 16 + 0 * 4 + 0] - ctx->G[0 * 16 * ctx->n_y + j * 16 + 0 * 4 + 0]) / h_x
            - h_x * F_xx(ctx->k, ctx->x_a, ctx->y_a + j * h_y) / 2;

        for (int i = 1; i < ctx->n_x - 1; i++)
        {
            ctx->P[3 * i + 0] = h_x;
            ctx->P[3 * i + 1] = 4 * h_x;
            ctx->P[3 * i + 2] = h_x;
            ctx->G[16 * (i * ctx->n_y + j) + 4] = 3 * (ctx->G[(i + 1) * 16 * ctx->n_y + j * 16 + 0 * 4 + 0]
                - ctx->G[(i - 1) * 16 * ctx->n_y + j * 16 + 0 * 4 + 0]);
        }
        ctx->P[3 * (ctx->n_x - 1) + 0] = 1;
        ctx->P[3 * (ctx->n_x - 1) + 1] = 2;
        ctx->P[3 * (ctx->n_x - 1) + 2] = 0;

        ctx->G[((ctx->n_x - 1) * ctx->n_y + j) * 16 + 4] =
            3 * (ctx->G[(ctx->n_x - 1) * 16 * ctx->n_y + j * 16 + 0 * 4 + 0] - ctx->G[(ctx->n_x - 2) * 16 * ctx->n_y + j * 16 + 0 * 4 + 0]) / h_x
            + h_x * F_xx(ctx->k, ctx->x_b, ctx->y_a + j * h_y) / 2;

        for (int i = 0; i < ctx->n_x - 1; i++)
        {
            ctx->P[3 * (i + 1) + 1] = ctx->P[3 * (i + 1) + 1] - ctx->P[3 * i + 2] * ctx->P[3 * (i + 1) + 0] / ctx->P[3 * i + 1];
            ctx->G[16 * ((i + 1) * ctx->n_y + j) + 4] = ctx->G[16 * ((i + 1) * ctx->n_y + j) + 4]
                - ctx->P[3 * (i + 1) + 0] / ctx->P[3 * i + 1] * ctx->G[16 * (i * ctx->n_y + j) + 4];
            ctx->P[3 * (i + 1)] = 0;
        }


        ctx->G[16 * ((ctx->n_x - 1) * ctx->n_y + j) + 4] = ctx->G[16 * ((ctx->n_x - 1) * ctx->n_y + j) + 4] / ctx->P[3 * (ctx->n_x - 1) + 1];
        ctx->P[3 * (ctx->n_x - 1) + 1] = 1;
        for (int i = ctx->n_x - 1; i > 0; i--)
        {
            ctx->G[16 * ((i - 1) * ctx->n_y + j) + 4] = ctx->G[16 * ((i - 1) * ctx->n_y + j) + 4] - ctx->P[3 * (i - 1) + 2] * ctx->G[16 * (i * ctx->n_y + j) + 4];
            ctx->G[16 * ((i - 1) * ctx->n_y + j) + 4] = ctx->G[16 * ((i - 1) * ctx->n_y + j) + 4] / ctx->P[3 * (i - 1) + 1];
            ctx->P[3 * (i - 1) + 1] = 1;
        }
        ctx->G[16 * (0 * ctx->n_y + j) + 4] = ctx->G[16 * (0 * ctx->n_y + j) + 4] / ctx->P[3 * 0 + 1];
    }

    //F_y

    for (int j = 0; j < ctx->n_x; j++)
    {
        ctx->P[3 * 0 + 0] = 0;
        ctx->P[3 * 0 + 1] = 2;
        ctx->P[3 * 0 + 2] = 1;

        ctx->G[j * 16 * ctx->n_y + 1] =
            3 * (ctx->G[j * 16 * ctx->n_y + 1 * 16 + 0 * 4 + 0] - ctx->G[j * 16 * ctx->n_y + 0 * 16 + 0 * 4 + 0]) / h_y
            - h_y * F_yy(ctx->k, ctx->x_a + j * h_x, ctx->y_a) / 2;

        ctx->G[j * 16 * ctx->n_y + 5] =
            3 * (ctx->G[j * 16 * ctx->n_y + 1 * 16 + 1 * 4 + 0] - ctx->G[j * 16 * ctx->n_y + 0 * 16 + 1 * 4 + 0]) / h_y
            - h_y * F_yy(ctx->k, ctx->x_a + j * h_x, ctx->y_a) / 2;

        for (int i = 1; i < ctx->n_y - 1; i++)
        {
            ctx->P[3 * i + 0] = h_y;
            ctx->P[3 * i + 1] = 4 * h_y;
            ctx->P[3 * i + 2] = h_y;
            ctx->G[16 * (j * ctx->n_y + i) + 1] = 3 * (ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 0 * 4 + 0]
                - ctx->G[j * 16 * ctx->n_y + (i - 1) * 16 + 0 * 4 + 0]);

            ctx->G[16 * (j * ctx->n_y + i) + 5] = 3 * (ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 1 * 4 + 0]
                - ctx->G[j * 16 * ctx->n_y + (i - 1) * 16 + 1 * 4 + 0]);
        }
        ctx->P[3 * (ctx->n_y - 1) + 0] = 1;
        ctx->P[3 * (ctx->n_y - 1) + 1] = 2;
        ctx->P[3 * (ctx->n_y - 1) + 2] = 0;

        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 1] =
            3 * (ctx->G[j * 16 * ctx->n_y + (ctx->n_y - 1) * 16 + 0 * 4 + 0] - ctx->G[j * 16 * ctx->n_y + (ctx->n_y - 2) * 16 + 0 * 4 + 0]) / h_y
            + h_y * F_yy(ctx->k, ctx->x_a + j * h_x, ctx->y_b) / 2;

        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 5] =
            3 * (ctx->G[j * 16 * ctx->n_y + (ctx->n_y - 1) * 16 + 1 * 4 + 0] - ctx->G[j * 16 * ctx->n_y + (ctx->n_y - 2) * 16 + 1 * 4 + 0]) / h_y
            + h_y * F_yy(ctx->k, ctx->x_a + j * h_x, ctx->y_b) / 2;

        for (int i = 0; i < ctx->n_y - 1; i++)
        {
            ctx->P[3 * (i + 1) + 1] = ctx->P[3 * (i + 1) + 1] - ctx->P[3 * i + 2] * ctx->P[3 * (i + 1) + 0] / ctx->P[3 * i + 1];
            ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 0 * 4 + 1] = ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 0 * 4 + 1]
                - ctx->P[3 * (i + 1) + 0] / ctx->P[3 * i + 1] * ctx->G[j * 16 * ctx->n_y + i * 16 + 0 * 4 + 1];

            ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 1 * 4 + 1] = ctx->G[j * 16 * ctx->n_y + (i + 1) * 16 + 1 * 4 + 1]
                - ctx->P[3 * (i + 1) + 0] / ctx->P[3 * i + 1] * ctx->G[j * 16 * ctx->n_y + i * 16 + 1 * 4 + 1];

            ctx->P[3 * (i + 1)] = 0;
        }


        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 1] = ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 1] / ctx->P[3 * (ctx->n_y - 1) + 1];

        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 5] = ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 5] / ctx->P[3 * (ctx->n_y - 1) + 1];
        ctx->P[3 * (ctx->n_y - 1) + 1] = 1;
        for (int i = ctx->n_y - 1; i > 0; i--)
        {
            ctx->G[16 * (j * ctx->n_y + (i - 1)) + 1] = ctx->G[16 * (j * ctx->n_y + (i - 1)) + 1] - ctx->P[3 * (i - 1) + 2] * ctx->G[16 * (j * ctx->n_y + i) + 1];
            ctx->G[16 * (j * ctx->n_y + (i - 1)) + 1] = ctx->G[16 * (j * ctx->n_y + (i - 1)) + 1] / ctx->P[3 * (i - 1) + 1];

            ctx->G[16 * (j * ctx->n_y + (i - 1)) + 5] = ctx->G[16 * (j * ctx->n_y + (i - 1)) + 5] - ctx->P[3 * (i - 1) + 2] * ctx->G[16 * (j * ctx->n_y + i) + 5];
            ctx->G[16 * (j * ctx->n_y + (i - 1)) + 5] = ctx->G[16 * (j * ctx->n_y + (i - 1)) + 5] / ctx->P[3 * (i - 1) + 1];
            ctx->P[3 * (i - 1) + 1] = 1;
        }
        ctx->G[16 * (j * ctx->n_y + 0) + 1] = ctx->G[16 * (j * ctx->n_y + 0) + 1] / ctx->P[3 * 0 + 1];

        ctx->G[16 * (j * ctx->n_y + 0) + 5] = ctx->G[16 * (j * ctx->n_y + 0) + 5] / ctx->P[3 * 0 + 1];

    }

}


void multMatrix1(interpolation2_ctx ctx, int curr_i, int curr_j)
{
    double c[16];
    int tmp;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            c[i * 4 + j] = 0;
            if (i < 2)
            {
                c[i * 4 + j] = ctx->Fij[i * 4 + j];
                continue;
            }
            for (int k = 0; k < 4; k++)
            {
                c[i * 4 + j] = ctx->Fij[k * 4 + j] * ctx->Ax[i * 4 + k] + c[i * 4 + j];
            }
        }
    }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            ctx->Fij[i * 4 + j] = c[i * 4 + j];
        }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            c[i * 4 + j] = 0;
            if (j < 2)
            {
                c[i * 4 + j] = ctx->Fij[i * 4 + j];
                continue;
            }
            for (int k = 0; k < 4; k++)
            {
                c[i * 4 + j] = ctx->Ay[k * 4 + j] * ctx->Fij[i * 4 + k] + c[i * 4 + j];
            }
        }
    }
    //ctx->G[curr_i * 16 * ctx->n_y + curr_j ] = c[0];
    tmp = 16 * (curr_i * (ctx->n_y - 1) + (curr_j));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            ctx->G[tmp + i * 4 + j] = c[i * 4 + j];
        }
}

void interpolation2_method_2(interpolation2_ctx ctx)
{
    int tmp;
    create_Gamma(ctx, ctx->h_x, ctx->h_y);

    for (int i = 0; i < ctx->n_x - 1; i++)
    {
        for (int j = 0; j < ctx->n_y - 1; j++)
        {
            tmp = 16 * (i * ctx->n_y + j);
            ctx->Fij[0] = ctx->G[tmp];
            ctx->Fij[1] = ctx->G[tmp + 1];
            ctx->Fij[2] = ctx->G[tmp + 16];
            ctx->Fij[3] = ctx->G[tmp + 17];
            ctx->Fij[4] = ctx->G[tmp + 4];
            ctx->Fij[5] = ctx->G[tmp + 5];
            ctx->Fij[6] = ctx->G[tmp + 20];
            ctx->Fij[7] = ctx->G[tmp + 21];
            ctx->Fij[8] = ctx->G[tmp + 16 * ctx->n_y];
            ctx->Fij[9] = ctx->G[tmp + 16 * ctx->n_y + 1];
            ctx->Fij[10] = ctx->G[tmp + 16 * ctx->n_y + 16];
            ctx->Fij[11] = ctx->G[tmp + 16 * ctx->n_y + 17];
            ctx->Fij[12] = ctx->G[tmp + 16 * ctx->n_y + 4];
            ctx->Fij[13] = ctx->G[tmp + 16 * ctx->n_y + 5];
            ctx->Fij[14] = ctx->G[tmp + 16 * ctx->n_y + 20];
            ctx->Fij[15] = ctx->G[tmp + 16 * ctx->n_y + 21];
            multMatrix1(ctx, i, j);
        }
    }
}
