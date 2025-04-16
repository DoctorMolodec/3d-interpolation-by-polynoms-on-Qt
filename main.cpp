#include "scene3D.h"
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>
#include <QAction>
#include <iostream>


//initially, a request is entered from the console in the form 1 100 100 4 -1 1 -10 10 where
//1 is the method, 100 and 100 are the number of interpolation points in x and y respectively
//4 is the interpolated function from the list, -1 1 is the interval for x, -10 10 is the interval for y
int main(int argc, char** argv)
{ 
    //below, checks are performed for data correctness
    //both by type and by value
    double x_a, x_b, y_a, y_b;
    int method, n_x, n_y, k;

    if (argc < 9)
    {
        std::cout << "Not enough arguments" << std::endl;
        return -1;
    }
    else
        if (argc > 9)
        {
            std::cout << "Exceeding the number of arguments" << std::endl;
            return -1;
        }

    if (sscanf(argv[1], "%d", &method) != 1)
    {
        std::cout << "Error in the data type in the method " << std::endl;
        return -1;
    }
    else
        if (method != 1 && method != 2)
        {
            std::cout << "Unknown method" << std::endl;
            return -1;
        }

    if (sscanf(argv[2], "%d", &n_x) != 1)
    {
        std::cout << "Error in the data type in the amount of points" << std::endl;
        return -1;
    }
    else
        if (n_x < 5)
        {
            std::cout << "Not enough amount of points" << std::endl;
            return -1;
        }
    if (sscanf(argv[3], "%d", &n_y) != 1)
    {
        std::cout << "Error in the data type in the amount of points" << std::endl;
        return -1;
    }
    else
        if (n_y < 5)
        {
            std::cout << "Not enough amount of points" << std::endl;
            return -1;
        }

    if (sscanf(argv[4], "%d", &k) != 1)
    {
        std::cout << "Error in the data type in the function" << std::endl;
        return -1;
    }
    else
        if (k < 0 || k > 7)
        {
            std::cout << "Invalid function" << std::endl;
            return -1;
        }

    if (sscanf(argv[5], "%lf", &x_a) != 1 || sscanf(argv[6], "%lf", &x_b) != 1)
    {
        std::cout << "Error in the data type in the segment" << std::endl;
        return -1;
    }
    else
        if (x_a > x_b || x_b - x_a > 1e9 || x_b - x_a < 1e-6)
        {
            std::cout << "Incorrect segment" << std::endl;
            return -1;
        }
    if (sscanf(argv[7], "%lf", &y_a) != 1 || sscanf(argv[8], "%lf", &y_b) != 1)
    {
        std::cout << "Error in the data type in the segment" << std::endl;
        return -1;
    }
    else
        if (y_a > y_b || y_b - y_a > 1e9 || y_b - y_a < 1e-6)
        {
            std::cout << "Incorrect segment" << std::endl;
            return -1;
        }

    QApplication app(argc, argv); 

    Scene3D scene1(NULL, method, n_x, n_y, k, x_a, x_b, y_a, y_b); 
    scene1.resize(1000, 600); 
    scene1.show(); 


    return app.exec();
}
