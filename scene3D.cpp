#include <QtGui> // подключаем модуль QtGui
#include "scene3D.h"
#include <math.h> // подключаем математическую библиотеку

#define N 1000

const static float pi = 3.141593, k = pi / 180; // глобальная переменная



Scene3D::Scene3D(QWidget* parent, int method, int n_x, int n_y, int function_number, double x_a, double x_b, double y_a, double y_b)
    : QGLWidget(parent), xRot(-90), yRot(0), zRot(90), zTra(0), nSca(0.8),
    x_a(x_a), x_b(x_b), y_a(y_a), y_b(y_b), n_x(n_x), n_y(n_y),
    graph_number(method), function_number(function_number),
    ctx1(interpolation2_create(1, n_x, n_y, function_number, x_a, x_b, y_a, y_b)),
    ctx2(interpolation2_create(2, n_x, n_y, function_number, x_a, x_b, y_a, y_b))
{
    x_a1 = x_a;
    x_b1 = x_b;
    y_a1 = y_a;
    y_b1 = y_b;
    functions[0] = &Scene3D::function;
    functions[1] = &Scene3D::method1;
    functions[2] = &Scene3D::method2;
    functions[3] = &Scene3D::error1;
    functions[4] = &Scene3D::error2;



    draw_f();
    updateGL();
}

void Scene3D::change_function()
{
    function_number = (function_number + 1) % 8;
    draw_f();
}

double Scene3D::function(double x, double y)
{
    return ctx1->f(x, y);
}

double Scene3D::method1(double x, double y)
{
    double res;
    res = interpolation2_calculate(ctx1, x, y);
    if (fabs(res) < 1e-15)
    {
        res = 0;
    }
    return res;
}

double Scene3D::method2(double x, double y)
{
    double res;
    res = interpolation2_calculate(ctx2, x, y);
    if (fabs(res) < 1e-15)
    {
        res = 0;
    }
    return res;
}

double Scene3D::error1(double x, double y)
{
    double res;
    res = ctx1->f(x, y) - interpolation2_calculate(ctx1, x, y);
    if (fabs(res) < 1e-15)
    {
        res = 0;
    }
    return res;
}

double Scene3D::error2(double x, double y)
{
    double res;
    res = ctx1->f(x, y) - interpolation2_calculate(ctx2, x, y);
    if (fabs(res) < 1e-15)
    {
        res = 0;
    }
    return res;
}

void Scene3D::draw_f()
{
    scale = 1;
    new_interpolation2(n_x, n_y, function_number, x_a, x_b, y_a, y_b);
    x_a1 = x_a;
    x_b1 = x_b;
    y_a1 = y_a;
    y_b1 = y_b;
    last_inf = 0;
    last_sup = 0;
    switch (function_number)
    {
    case 0:
        function_name = "f(x,y) = 1";
        break;
    case 1:
        function_name = "f(x,y) = x";
        break;
    case 2:
        function_name = "f(x,y) = y";
        break;
    case 3:
        function_name = "f(x,y) = x + y";
        break;
    case 4:
        function_name = "f(x,y) = sqrt(x * x + y * y)";
        break;
    case 5:
        function_name = "f(x,y) = x * x + y * y";
        break;
    case 6:
        function_name = "f(x,y) = exp(x * x - y * y)";
        break;
    case 7:
        function_name = "f(x,y) = 1/(25*x*x+25*y*y+1)";
        break;
    }

    updateGL();
}

void Scene3D::initializeGL() // инициализация
{
    // просто фон окна
    qglClearColor(Qt::white); // цвет для очистки буфера изображения - здесь
    glEnable(GL_DEPTH_TEST); // устанавливает режим проверки глубины пикселей
    glShadeModel(GL_FLAT); // отключает режим сглаживания цветов
    glEnable(GL_CULL_FACE); // устанавливается режим, когда строятся только
    // внешние поверхности
}

/*virtual*/ void Scene3D::resizeGL(int nWidth, int nHeight) // окно виджета
{
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity(); // присваивает проекционной матрице единичную матрицу
    GLfloat ratio =
        (GLfloat)nHeight /
        (GLfloat)nWidth; // отношение высоты окна виджета к его ширине

    // мировое окно
    if (nWidth >= nHeight)
        glOrtho(-1.0 / ratio, 1.0 / ratio, -1.0, 1.0, -10.0, 1.0); // параметры видимости ортогональной проекции
    else
        glOrtho(-1.0, 1.0, -1.0 * ratio, 1.0 * ratio, -10.0, 1.0); // параметры видимости ортогональной проекции
    // плоскости отсечения (левая, правая, верхняя, нижняя, передняя, задняя)

    // поле просмотра
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

/*virtual*/ void Scene3D::paintGL() // рисование
{
    // glClear(GL_COLOR_BUFFER_BIT); // окно виджета очищается текущим цветом
    // очистки
    glClear(GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT); // очистка буфера изображения и глубины

    glMatrixMode(GL_MODELVIEW); // устанавливает положение и ориентацию матрице
    // моделирования
    glLoadIdentity(); // загружает единичную матрицу моделирования

    // последовательные преобразования
    glScalef(nSca, nSca, nSca); // масштабирование
    glTranslatef(0.0f, zTra, 0.0f); // трансляция
    glRotatef(xRot, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X
    glRotatef(yRot, 0.0f, 1.0f, 0.0f); // поворот вокруг оси Y
    glRotatef(zRot, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z

    //drawAxis(); // рисование осей координат
    drawFigure(); // нарисовать фигуру


}

void Scene3D::drawAxis() // построить оси координат
{
    glLineWidth(2.0f); // устанавливаю ширину линии приближённо в пикселях
    // до вызова команды ширина равна 1 пикселю по умолчанию

    glColor4f(1.00f, 0.00f, 0.00f,
        1.0f); // устанавливается цвет последующих примитивов
    // ось x
    glBegin(GL_LINES); // построение линии
    glVertex3f(x_a, 0.0f, 0.0f); // первая точка
    glVertex3f(x_b, 0.0f, 0.0f); // вторая точка
    // ось y
    glVertex3f(0.0f, y_a, 0.0f);
    glVertex3f(0.0f, y_b, 0.0f);
    // ось z
    glVertex3f(0.0f, 0.0f, 10.0f);
    glVertex3f(0.0f, 0.0f, -10.0f);
    glEnd();
}

void Scene3D::drawFigure() // построить фигуру
{
    double x1 = x_a1, y1 = y_a1, z1;
    double s_x = (x_b1 - x_a1) / N;
    double s_y = (y_b1 - y_a1) / N;
    double abs_max = 1;
    sup = 0;
    inf = 0;
    for (int i = 0; i < N + 1; i++)
    {
        for (int j = 0; j < N + 1; j++)
        {

            z1 = (this->*functions[graph_number])(x1, y1);

            if (z1 >= sup)
            {
                sup = z1;
            }
            if (z1 <= inf)
            {
                inf = z1;
            }
            y1 = y1 + s_y;
            //if(y1-y_b1>1e-15)
                //break;
        }
        x1 = x1 + s_x;
        y1 = y_a1;
        //if(x1-x_b1>1e-15)
            //break;

    }
    abs_max = fmax(fabs(sup), fabs(inf));
    if (abs_max < 1e-15)
    {
        abs_max = 10;
    }

    if (fmax(fabs(last_sup), fabs(last_inf)) < 1e-15)
    {
        error_drop = 0;
    }
    else
    {
        error_drop = abs_max / fmax(fabs(last_sup), fabs(last_inf));
        if ((error_drop < 1) && (error_drop > 1e-15))
        {
            error_drop = 1 / error_drop;
        }
    }

    glScalef(nSca / (x_b1 - x_a1), nSca / (y_b1 - y_a1), nSca / abs_max);
    glTranslatef(-(x_a1 + x_b1) / 2, -(y_a1 + y_b1) / 2, -abs_max / 2);



    glLineWidth(2.0f); // устанавливаю ширину линии приближённо в пикселях
    // до вызова команды ширина равна 1 пикселю по умолчанию

    glColor4f(1.00f, 0.00f, 0.00f, 1.0f); // устанавливается цвет последующих примитивов
    // ось x
    glBegin(GL_LINES); // построение линии
    if (x_a1 * x_b1 > 0)
    {
        if (x_a1 > 0)
        {
            glVertex3f(0, 0.0f, 0.0f); // первая точка
            glVertex3f(2 * x_b1, 0.0f, 0.0f); // вторая точка
        }
        else
        {
            glVertex3f(2 * x_a1, 0.0f, 0.0f); // первая точка
            glVertex3f(0, 0.0f, 0.0f); // вторая точка
        }
    }
    else
    {
        glVertex3f(2 * x_a1, 0.0f, 0.0f); // первая точка
        glVertex3f(2 * x_b1, 0.0f, 0.0f); // вторая точка
    }
    // ось y
    if (y_a1 * y_b1 > 0)
    {
        if (y_a1 > 0)
        {
            glVertex3f(0, 0.0f, 0.0f); // первая точка
            glVertex3f(0, 2 * y_b1, 0.0f); // вторая точка
        }
        else
        {
            glVertex3f(0, 2 * y_a1, 0.0f); // первая точка
            glVertex3f(0, 0.0f, 0.0f); // вторая точка
        }
    }
    else
    {
        glVertex3f(0, 2 * y_a1, 0.0f); // первая точка
        glVertex3f(0, 2 * y_b1, 0.0f); // вторая точка
    }
    // ось z
    glVertex3f(0.0f, 0.0f, abs_max);
    glVertex3f(0.0f, 0.0f, -abs_max);
    glEnd();


    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2f, 3.0f, 0.3f, 0.7);
    glBegin(GL_TRIANGLES);
    x1 = x_a1 + s_x;
    y1 = y_a1 + s_y;
    for (int i = 1; i < N + 1; i++)
    {
        for (int j = 1; j < N + 1; j++)
        {

            glColor4f((x_a1 + x1) / (x_b1 - x_a1) / 2 + 0.5, (y_a1 + y1) / (y_b1 - y_a1) / 2 + 0.5, 0.5f, 0.7);
            glVertex3f(x1 - s_x, y1, (this->*functions[graph_number])(x1 - s_x, y1));
            glVertex3f(x1 - s_x, y1 - s_y, (this->*functions[graph_number])(x1 - s_x, y1 - s_y));
            glVertex3f(x1, y1, (this->*functions[graph_number])(x1, y1));
            glVertex3f(x1, y1 - s_y, (this->*functions[graph_number])(x1, y1 - s_y));
            glVertex3f(x1, y1, (this->*functions[graph_number])(x1, y1));
            glVertex3f(x1 - s_x, y1 - s_y, (this->*functions[graph_number])(x1 - s_x, y1 - s_y));
            y1 = y1 + s_y;

        }
        x1 = x1 + s_x;
        y1 = y_a1 + s_y;
    }

    glEnd();
    qglColor("Black");
    renderText(0, 20, "Next graph: 1");
    renderText(90, 20, "Increase scale: 2");
    renderText(200, 20, "Decrease scale: 3");
    renderText(310, 20, "Double points: 4");
    renderText(415, 20, "Half points: 5");
    renderText(500, 20, "Rotate -15: 8");
    renderText(600, 20, "Rotate +15: 9");
    renderText(700, 20, "Next function: 0");
    renderText(800, 20, "Exit: Esc");

    qglColor("Blue");
    renderText(0, 40, function_name);
    renderText(0, 60, "graph = " + QString::number(graph_number));
    renderText(0, 80, "n_x = " + QString::number(n_x));
    renderText(0, 100, "n_y = " + QString::number(n_y));
    renderText(0, 120, "scale = " + QString::number(scale));
    renderText(0, 140, "angle = " + QString::number(zRot - 360 * floor(zRot / 360)) + " degree");
    qglColor("Purple");
    renderText(0, 160, "max = " + QString::number(sup));
    renderText(0, 180, "min = " + QString::number(inf));
    renderText(0, 200, "x: (" + QString::number(x_a1) + ", " + QString::number(x_b1) + ")");
    renderText(0, 220, "y: (" + QString::number(y_a1) + ", " + QString::number(y_b1) + ")");
    if ((graph_number == 3) || (graph_number == 4))
    {
        renderText(0, 240, "Error drop: " + QString::number(error_drop));
    }
}

void Scene3D::zoom_in()
{
    scale *= 2;
    double x2_a, x2_b, y2_a, y2_b;
    x2_a = (x_a1 + x_b1) / 2 - (x_b1 - x_a1) / 4;
    x2_b = (x_a1 + x_b1) / 2 + (x_b1 - x_a1) / 4;
    x_a1 = x2_a;
    x_b1 = x2_b;

    y2_a = (y_a1 + y_b1) / 2 - (y_b1 - y_a1) / 4;
    y2_b = (y_a1 + y_b1) / 2 + (y_b1 - y_a1) / 4;
    y_a1 = y2_a;
    y_b1 = y2_b;

    last_inf = 0;
    last_sup = 0;

    updateGL();
}

void Scene3D::zoom_out()
{
    scale /= 2;
    double x2_a, x2_b, y2_a, y2_b;
    x2_a = (x_a1 + x_b1) / 2 - (x_b1 - x_a1);
    x2_b = (x_a1 + x_b1) / 2 + (x_b1 - x_a1);
    x_a1 = x2_a;
    x_b1 = x2_b;

    y2_a = (y_a1 + y_b1) / 2 - (y_b1 - y_a1);
    y2_b = (y_a1 + y_b1) / 2 + (y_b1 - y_a1);
    y_a1 = y2_a;
    y_b1 = y2_b;

    last_inf = 0;
    last_sup = 0;

    updateGL();
}

void Scene3D::double_n()
{
    n_x *= 2;
    n_y *= 2;
    new_interpolation2(n_x, n_y, function_number, x_a, x_b, y_a, y_b);
    last_inf = inf;
    last_sup = sup;
    updateGL();
}

void Scene3D::half_n()
{
    n_x /= 2;
    n_x = fmax(n_x, 5);

    n_y /= 2;
    n_y = fmax(n_y, 5);
    new_interpolation2(n_x, n_y, function_number, x_a, x_b, y_a, y_b);
    last_inf = inf;
    last_sup = sup;
    updateGL();
}

void Scene3D::rotate_z(int factor) {
    zRot += factor * 15;
}


void Scene3D::rotate_up() // повернуть сцену вверх
{
    xRot += 15.0;
}

void Scene3D::rotate_down() // повернуть сцену вниз
{
    xRot -= 15.0;
}

void Scene3D::rotate_left() // повернуть сцену влево
{
    zRot += 1.0;
}

void Scene3D::rotate_right() // повернуть сцену вправо
{
    zRot -= 1.0;
}

void Scene3D::keyPressEvent(QKeyEvent* pe) // нажатие определенной клавиши
{
    switch (pe->key())
    {
    case Qt::Key_Up:
        rotate_up(); // повернуть сцену вверх
        break;

    case Qt::Key_Down:
        rotate_down(); // повернуть сцену вниз
        break;

    case Qt::Key_Left:
        rotate_left(); // повернуть сцену влево
        break;

    case Qt::Key_Right:
        rotate_right(); // повернуть сцену вправо
        break;

    case Qt::Key_Escape: // клавиша "эскейп"
        this->close(); // завершает приложение
        break;

    case Qt::Key_0:
        this->change_function();
        break;

    case Qt::Key_1:
        this->change_graph();
        break;

    case Qt::Key_2:
        this->zoom_in();
        break;

    case Qt::Key_3:
        this->zoom_out();
        break;

    case Qt::Key_4:
        this->double_n();
        break;

    case Qt::Key_5:
        this->half_n();
        break;

    case Qt::Key_8:
        this->rotate_z(1);
        break;

    case Qt::Key_9:
        this->rotate_z(-1);
        break;
    }


    updateGL(); // обновление изображения
}

void Scene3D::change_graph()
{
    graph_number = (graph_number + 1) % 5;
    last_inf = 0;
    last_sup = 0;
    updateGL();
}

void Scene3D::new_interpolation2(int n_x, int n_y, int function_number, double x_a, double x_b, double y_a, double y_b)
{
    interpolation2_ctx t = interpolation2_create(1, n_x, n_y, function_number, x_a, x_b, y_a, y_b);
    interpolation2_destroy(ctx1);
    ctx1 = t;

    t = interpolation2_create(2, n_x, n_y, function_number, x_a, x_b, y_a, y_b);
    interpolation2_destroy(ctx2);
    ctx2 = t;
}

Scene3D::~Scene3D()
{
    interpolation2_destroy(ctx1);
    interpolation2_destroy(ctx2);
}
