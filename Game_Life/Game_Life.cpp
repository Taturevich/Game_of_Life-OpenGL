// OpenGlExample.cpp : Defines the entry point for the console application.
//#pragma comment(lib, "glut32.lib")
//#include <stdafx.cpp>
#include "stdafx.h"
#include < glut.h > 
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>

// глобальная переменная – количество активных пользователей
DWORD WINAPI Opengl();

struct    cell          // Структура ячейки
{
	float	lbx; //левый нижний угол ячейки (координата по х)
	float lby; //левый нижний угол ячейки(координата по y)
	int hline = 0; //линии-подсветки потенциальных ходов
	int  condition = 0; //состояние ячейки  (текущий цвет ячейки; по дефолту все клетки черного цвета) 
	bool lock = false; //блокирование ячейки от нажатия мыши (по дефолту все клетки свободны)
};
cell cells[10][10];                // Матрица всего игрового поля (поле, которое рисуется)
cell prevgen[10][10];              // Поле предыдущего поколения

float mx = 0.0, my = 0.0;		//переменные для координат мыши
bool start = false;         // Переменная, позволяющая задавать начальные значения 
int i, j;					//переменные для обхода циклов
float down = false;             //клавиша не нажата
bool painter = false;
void Armageddon();
void next_generation();





//расширенные настройки OpenGL
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0)              // Предотвращение деления на ноль
	{
		height = 1;
	}
	glViewport(0, 0, width, height);  // Сброс текущей области просмотра
	glMatrixMode(GL_PROJECTION);   // Выбор матрицы проектирования
	glLoadIdentity();              // Сброс матрицы проектирования
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f); // Создание ортог. вида 640x480 (0,0 – верх лево)
	glMatrixMode(GL_MODELVIEW);    // Выбор матрицы просмотра вида
	glLoadIdentity();              // Сброс матрицы просмотра вида
}

// Все настройки для OpenGL делаются здесь
int InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH);    // Разрешить плавное сглаживание
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // Черный фон
	glClearDepth(1.0f);         // Настройка буфера глубины
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Сглаживание линий
	glEnable(GL_BLEND);         // Разрешить смешивание
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Тип смешивания
	return 0;                // Инициализация окончена успешно
}

//ФУНКЦИЯ ЗАКРАШИВАНИЯ ЯЧЕЙКИ
void drawcell(int i, int j)
{
	glBegin(GL_QUADS);
	glVertex2d(20 + (i * 60), 70 + (j * 60));
	glVertex2d(80 + (i * 60), 70 + (j * 60));
	glVertex2d(80 + (i * 60), 130 + (j * 60));
	glVertex2d(20 + (i * 60), 130 + (j * 60));
	glEnd();
}

//функция рисования линии
void drawline(int i, int j)
{
	if (j < 10 && i<10)
	{
		glLineWidth(3.0f);               // Ширина линий 3.0f  
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);           // Начало рисования горизонтального бордюра ячейки
		glVertex2d(20 + (i * 60), 70 + (j * 60)); // Левая сторона горизонтальной линии
		glVertex2d(80 + (i * 60), 70 + (j * 60)); // Правая сторона горизонтальной линии
		glVertex2d(20 + (i * 60), 70 + ((j + 1) * 60)); // Нижняя грань
		glVertex2d(80 + (i * 60), 70 + ((j + 1) * 60)); // 
		glEnd();                     // Конец рисования горизонтального бордюра ячейки
	}
	if (i < 10 && j<10)
	{
		glLineWidth(3.0f);               // Ширина линий 3.0f
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);           // Начало рисования вертикального бордюра ячейки
		glVertex2d(20 + (i * 60), 70 + (j * 60));  // Верхняя сторона вертикальной линии
		glVertex2d(20 + (i * 60), 130 + (j * 60)); // Нижняя сторона вертикальной линии
		glVertex2d(20 + ((i + 1) * 60), 70 + (j * 60));  // Верхняя сторона вертикальной линии
		glVertex2d(20 + ((i + 1) * 60), 130 + (j * 60)); // Нижняя сторона вертикальной линии
		glEnd();                     // Конец рисования вертикального бордюра ячейки*/
	}
}

//функция вычисляющая живые клетки около клетки, с индексами i, j
int checkneighbors(int i, int j)
{
	int life = 0;
	if (prevgen[i - 1][j].condition == 1 && i > 0)
	{
		life++;
	}
	if (prevgen[i + 1][j].condition == 1 && i<10)
	{
		life++;
	}
	if (prevgen[i][j - 1].condition == 1 && j>0)
	{
		life++;
	}
	if (prevgen[i][j + 1].condition == 1 && j > 0)
	{
		life++;
	}
	//диагональные соседи
	if (prevgen[i - 1][j - 1].condition == 1 && i > 0 && j > 0)
	{
		life++;
	}
	if (prevgen[i + 1][j + 1].condition == 1 && i < 10 && j<10)
	{
		life++;
	}
	if (prevgen[i + 1][j - 1].condition == 1 && i<10 && j>0)
	{
		life++;
	}
	if (prevgen[i - 1][j + 1].condition == 1 && i>0 && j<10)
	{
		life++;
	}
	return life;
}



//функция проверки значения координат с клика мыши
void checkMouse(int mx, int my)
{
	for (i = 0; i < 11; i++)   // Цикл слева направо
	{
		for (j = 0; j<11; j++) // Цикл сверху вниз
		{

			//задаем координаты узлов сетки
			if (i < 10 && j < 10)
			{
				prevgen[i][j].lbx = 20 + (i * 60);
				prevgen[i][j].lby = 50 + (j * 45);
			}

			//ПРОВЕРКИ НА ПОПАДАНИЕ КЛИКА КУРСОРА В ПОДХОДЯЩУЮ КЛЕТКУ

			//начинаем рисовать первое поколение
			if (mx>860 && mx<920 && my>95 && my<140 && painter == false)
			{
				painter = true;
				start = false;
				mx = 0; my = 0;
			}
			else
			{
				if (mx>860 && mx<920 && my>95 && my<140 && painter == true)
					Armageddon();
			}

			if (mx>860 && mx<920 && my>185 && my<240 && painter == true)
			{
				painter = false;
				start = true;
				next_generation();
			}

			//рисуем первое поколение
			if (mx>prevgen[i][j].lbx  && mx<prevgen[i][j].lbx + 60 && my>prevgen[i][j].lby  && my < prevgen[i][j].lby + 45 && prevgen[i][j].lock != true
				&& painter == true)
			{
				prevgen[i][j].condition = 1; //первое состояние (цвет) клетки
				cells[i][j].condition = 1;
			}


		}
	}
}


//функция, обрабатывающая значения с клика мыши
void MousePressed(int button, int state, int ax, int ay)
{
	down = button == GLUT_LEFT_BUTTON && state == GLUT_LEFT; //ЕСЛИ НАЖАТА (И НЕ ЗАЖАТА) ЛЕВАЯ КЛАВИША МЫШИ
	if (down)
	{
		mx = ax; //назначаем переменной координату мыши по х
		my = ay; //назначаем переменной координату мыши по у
		checkMouse(mx, my);
	}
}

//генерация нового поколения
void next_generation()
{
	for (i = 0; i < 11; i++)   // Цикл слева направо
	{
		for (j = 0; j < 11; j++) // Цикл сверху вниз
		{
			int life = checkneighbors(i, j);
			if (cells[i][j].condition == 0)
			{
				if (life == 3)
					cells[i][j].condition = 1;
			}
			else
			{
				if (life<2 || life>3)
					cells[i][j].condition = 0;
			}
		}
	}
}

//функция перезаписи значений ячеек для дальнейшей эволюции
void new_generation()
{
	for (i = 0; i < 11; i++)   // Цикл слева направо
	{
		for (j = 0; j < 11; j++) // Цикл сверху вниз
		{
			if (cells[i][j].condition == 1)
			{
				prevgen[i][j].condition = 1;
			}
			else
			{
				prevgen[i][j].condition = 0;
			}
		}
	}
}

//функция очистки поля
void Armageddon()
{
	for (i = 0; i < 11; i++)   // Цикл слева направо
	{
		for (j = 0; j < 11; j++) // Цикл сверху вниз
		{
			cells[i][j].condition = 0;
			prevgen[i][j].condition = 0;
		}
	}
}


//ОСНОВНАЯ ФУНКЦИЯ ПРОРИСОВКИ
void display(void)
{
	for (i = 0; i < 11; i++)   // Цикл слева направо
	{
		for (j = 0; j < 11; j++) // Цикл сверху вниз
		{
			//ЗАКРАШИВАЕМ НУЖНЫЕ КЛЕТКИ
			if (i < 10 && j < 10) //не рисуем за пределами поля
			{
				glColor3f(0.0f, 0.0f, 0.0f); //ЧЕРНЫЙ ЦВЕТ
				if (cells[i][j].condition == 1)
				{
					glColor3f(1.0f, 0.0f, 0.0f);   //Цвет линии красный
					drawcell(i, j);
				}
				else
					drawcell(i, j);
			}

			//рисуем сетку
			if (i < 10 && j < 10)
				drawline(i, j);
		}
	}
	//если запущена эволюция, генерируем новое поколение
	if (start == true && painter == false)
	{
		next_generation();
		//ставим новое поколение на место старого и продолжаем эфолюцию
		new_generation();
	}

	//ИНФОРМАЦИЯ ДЛЯ ИГРОКОВ
	if (painter == false)
	{
		glColor3f(1.0f, 0.0f, 0.0f);   //Цвет линии красный
		drawcell(14, 1);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 1.0f);   //Цвет линии синий
		drawcell(14, 1);
	}

	glColor3f(0.0f, 1.0f, 0.0f);   //Цвет линии зеленый
	drawcell(14, 3);


	glutSwapBuffers();
}


//параметры окна OpenGL
void init(void)
{
	/* установим черный фон */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1000, 1000, 0, -1, 1);
}

//функция, задающая анимацию
void Timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(250, Timer, 0);
}

//вызов функций рисования поля
DWORD WINAPI Opengl()
{
	glutTimerFunc(10, Timer, 1);
	glutMouseFunc(MousePressed);
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0; /* ISO C requires main to return int. */
}


//Определим параметры окна , display mode
int main(int argc, char** argv)
{
	HWND hwnd = GetForegroundWindow();
	if (hwnd != NULL)
	{
		ShowWindow(hwnd, SW_HIDE);
		PlaySound(L"02_scorpions_the_game_of_life_myzuka.org.wav", NULL, SND_ASYNC | SND_LOOP);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(100, 200);
	glutCreateWindow("GAME OF LIFE");
	Opengl(); // ФУНКЦИЯ РИСОВАНИЯ
	return 0;
}




