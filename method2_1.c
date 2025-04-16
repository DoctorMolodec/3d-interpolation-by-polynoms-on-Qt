#include "method2_1.h"

double F_x(int k, double x, double y);
double F_y(int k, double x, double y);
double F_x_y(int k, double x, double y);
void multMatrix(interpolation2_ctx ctx, int curr_i, int curr_j, double* c);
void create_Gamma_1(interpolation2_ctx ctx, double h_x, double h_y);


void create_Gamma_1(interpolation2_ctx ctx, double h_x, double h_y)
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


        ctx->G[j * 16 + 4] = (ctx->G[16 * (ctx->n_y + j)] - ctx->f(ctx->x_a - h_x, ctx->y_a + h_y * j)) / (2 * h_x);

        for (int i = 1; i < ctx->n_x - 1; i++)
        {
            ctx->G[16 * (i * ctx->n_y + j) + 4] = F_x(ctx->k, ctx->x_a + h_x * i, ctx->y_a + h_y * j);

        }


        ctx->G[((ctx->n_x - 1) * ctx->n_y + j) * 16 + 4] = (ctx->f(ctx->x_b + h_x, ctx->y_a + h_y * j) - ctx->G[16 * ((ctx->n_x - 2) * ctx->n_y + j)]) / (2 * h_x);
    }


    //Fy, Fxy


    for (int j = 0; j < ctx->n_x; j++)
    {


        ctx->G[j * 16 * ctx->n_y + 1] = (ctx->G[j * 16 * ctx->n_y + 16] - ctx->f(ctx->x_a + j * h_x, ctx->y_a - h_y)) / (2 * h_y);

        ctx->G[j * 16 * ctx->n_y + 5] = (ctx->G[j * 16 * ctx->n_y + 20] - ctx->f(ctx->x_a + j * h_x, ctx->y_a - h_y)) / (2 * h_y);

        for (int i = 1; i < ctx->n_y - 1; i++)
        {

            ctx->G[16 * (j * ctx->n_y + i) + 1] = F_y(ctx->k, ctx->x_a + j * h_x, ctx->y_a + i * h_y);
            ctx->G[16 * (j * ctx->n_y + i) + 5] = F_x_y(ctx->k, ctx->x_a + j * h_x, ctx->y_a + i * h_y);

        }


        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 1] = (ctx->f(ctx->x_a + j * h_x, ctx->y_b + h_y) - ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 2))]) / (2 * h_y);

        ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 1)) + 5] = (ctx->f(ctx->x_a + j * h_x, ctx->y_b + h_y) - ctx->G[16 * (j * ctx->n_y + (ctx->n_y - 2)) + 4]) / (2 * h_y);
    }

}

void multMatrix(interpolation2_ctx ctx, int curr_i, int curr_j, double* c)
{
    int tmp;
    double d;
    tmp = 16 * (curr_i * ctx->n_y + (curr_j - curr_i));
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
            if (j < 2)
            {
                ctx->G[tmp + i * 4 + j] = c[i * 4 + j];
                continue;
            }
            d = 0;
            for (int k = 0; k < 4; k++)
            {
                d = ctx->Ay[k * 4 + j] * ctx->Fij[i * 4 + k] + d;
            }
            ctx->G[tmp + i * 4 + j] = d;
        }
    }

}


void interpolation2_method_1(interpolation2_ctx ctx)
{
    int tmp;
    double c[16];
    create_Gamma_1(ctx, ctx->h_x, ctx->h_y);

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



            multMatrix(ctx, i, j, c);
        }

    }
}

double F_x(int k, double x, double y)
{
    double res = 0;
    if (k == 0)
    {
        res = 0;
    }
    if (k == 1)
    {
        res = 1;
    }
    if (k == 2)
    {
        res = 0;
    }
    if (k == 3)
    {
        res = 1;
    }
    if (k == 4)
    {
        if (fabs(y) < 1e-15)
        {
            if (x > 0)
            {
                res = 1;
            }
            else
            {
                res = -1;
            }
            if (fabs(x) < 1e-15)
            {
                res = 0;
            }
        }
        else
        {
            res = x / sqrt(x * x + y * y);
        }
    }
    if (k == 5)
    {
        res = 2 * x;
    }
    if (k == 6)
    {
        res = 2 * x * exp(x * x - y * y);
    }
    if (k == 7)
    {
        res = -50 * x / pow(25 * x * x + 25 * y * y + 1, 2);
    }
    return res;
}

double F_y(int k, double x, double y)
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
        res = 1;
    }
    if (k == 3)
    {
        res = 1;
    }
    if (k == 4)
    {
        if (fabs(x) < 1e-15)
        {
            if (y > 0)
            {
                res = 1;
            }
            else
            {
                res = -1;
            }
            if (fabs(y) < 1e-15)
            {
                res = 0;
            }
        }
        else
        {
            res = y / sqrt(x * x + y * y);
        }
    }
    if (k == 5)
    {
        res = 2 * y;
    }
    if (k == 6)
    {
        res = -2 * y * exp(x * x - y * y);
    }
    if (k == 7)
    {
        res = -50 * y / pow(25 * x * x + 25 * y * y + 1, 2);
    }
    return res;
}

double F_x_y(int k, double x, double y)
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
            res = -y * x / pow(sqrt(x * x + y * y), 3);
        }
    }
    if (k == 5)
    {
        res = 0;
    }
    if (k == 6)
    {
        res = -4 * x * y * exp(x * x - y * y);
    }
    if (k == 7)
    {
        res = 5000 * x * y / pow(25 * x * x + 25 * y * y + 1, 3);
    }
    return res;
}
