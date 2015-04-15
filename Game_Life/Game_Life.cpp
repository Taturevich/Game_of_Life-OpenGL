// OpenGlExample.cpp : Defines the entry point for the console application.
//#pragma comment(lib, "glut32.lib")
//#include <stdafx.cpp>
#include "stdafx.h"
#include < glut.h > 
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>

// ���������� ���������� � ���������� �������� �������������
DWORD WINAPI Opengl();

struct    cell          // ��������� ������
{
	float	lbx; //����� ������ ���� ������ (���������� �� �)
	float lby; //����� ������ ���� ������(���������� �� y)
	int hline = 0; //�����-��������� ������������� �����
	int  condition = 0; //��������� ������  (������� ���� ������; �� ������� ��� ������ ������� �����) 
	bool lock = false; //������������ ������ �� ������� ���� (�� ������� ��� ������ ��������)
};
cell cells[10][10];                // ������� ����� �������� ���� (����, ������� ��������)
cell prevgen[10][10];              // ���� ����������� ���������

float mx = 0.0, my = 0.0;		//���������� ��� ��������� ����
bool start = false;         // ����������, ����������� �������� ��������� �������� 
int i, j;					//���������� ��� ������ ������
float down = false;             //������� �� ������
bool painter = false;
void Armageddon();
void next_generation();





//����������� ��������� OpenGL
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0)              // �������������� ������� �� ����
	{
		height = 1;
	}
	glViewport(0, 0, width, height);  // ����� ������� ������� ���������
	glMatrixMode(GL_PROJECTION);   // ����� ������� ��������������
	glLoadIdentity();              // ����� ������� ��������������
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f); // �������� �����. ���� 640x480 (0,0 � ���� ����)
	glMatrixMode(GL_MODELVIEW);    // ����� ������� ��������� ����
	glLoadIdentity();              // ����� ������� ��������� ����
}

// ��� ��������� ��� OpenGL �������� �����
int InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH);    // ��������� ������� �����������
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // ������ ���
	glClearDepth(1.0f);         // ��������� ������ �������
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // ����������� �����
	glEnable(GL_BLEND);         // ��������� ����������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ��� ����������
	return 0;                // ������������� �������� �������
}

//������� ������������ ������
void drawcell(int i, int j)
{
	glBegin(GL_QUADS);
	glVertex2d(20 + (i * 60), 70 + (j * 60));
	glVertex2d(80 + (i * 60), 70 + (j * 60));
	glVertex2d(80 + (i * 60), 130 + (j * 60));
	glVertex2d(20 + (i * 60), 130 + (j * 60));
	glEnd();
}

//������� ��������� �����
void drawline(int i, int j)
{
	if (j < 10 && i<10)
	{
		glLineWidth(3.0f);               // ������ ����� 3.0f  
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);           // ������ ��������� ��������������� ������� ������
		glVertex2d(20 + (i * 60), 70 + (j * 60)); // ����� ������� �������������� �����
		glVertex2d(80 + (i * 60), 70 + (j * 60)); // ������ ������� �������������� �����
		glVertex2d(20 + (i * 60), 70 + ((j + 1) * 60)); // ������ �����
		glVertex2d(80 + (i * 60), 70 + ((j + 1) * 60)); // 
		glEnd();                     // ����� ��������� ��������������� ������� ������
	}
	if (i < 10 && j<10)
	{
		glLineWidth(3.0f);               // ������ ����� 3.0f
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);           // ������ ��������� ������������� ������� ������
		glVertex2d(20 + (i * 60), 70 + (j * 60));  // ������� ������� ������������ �����
		glVertex2d(20 + (i * 60), 130 + (j * 60)); // ������ ������� ������������ �����
		glVertex2d(20 + ((i + 1) * 60), 70 + (j * 60));  // ������� ������� ������������ �����
		glVertex2d(20 + ((i + 1) * 60), 130 + (j * 60)); // ������ ������� ������������ �����
		glEnd();                     // ����� ��������� ������������� ������� ������*/
	}
}

//������� ����������� ����� ������ ����� ������, � ��������� i, j
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
	//������������ ������
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



//������� �������� �������� ��������� � ����� ����
void checkMouse(int mx, int my)
{
	for (i = 0; i < 11; i++)   // ���� ����� �������
	{
		for (j = 0; j<11; j++) // ���� ������ ����
		{

			//������ ���������� ����� �����
			if (i < 10 && j < 10)
			{
				prevgen[i][j].lbx = 20 + (i * 60);
				prevgen[i][j].lby = 50 + (j * 45);
			}

			//�������� �� ��������� ����� ������� � ���������� ������

			//�������� �������� ������ ���������
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

			//������ ������ ���������
			if (mx>prevgen[i][j].lbx  && mx<prevgen[i][j].lbx + 60 && my>prevgen[i][j].lby  && my < prevgen[i][j].lby + 45 && prevgen[i][j].lock != true
				&& painter == true)
			{
				prevgen[i][j].condition = 1; //������ ��������� (����) ������
				cells[i][j].condition = 1;
			}


		}
	}
}


//�������, �������������� �������� � ����� ����
void MousePressed(int button, int state, int ax, int ay)
{
	down = button == GLUT_LEFT_BUTTON && state == GLUT_LEFT; //���� ������ (� �� ������) ����� ������� ����
	if (down)
	{
		mx = ax; //��������� ���������� ���������� ���� �� �
		my = ay; //��������� ���������� ���������� ���� �� �
		checkMouse(mx, my);
	}
}

//��������� ������ ���������
void next_generation()
{
	for (i = 0; i < 11; i++)   // ���� ����� �������
	{
		for (j = 0; j < 11; j++) // ���� ������ ����
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

//������� ���������� �������� ����� ��� ���������� ��������
void new_generation()
{
	for (i = 0; i < 11; i++)   // ���� ����� �������
	{
		for (j = 0; j < 11; j++) // ���� ������ ����
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

//������� ������� ����
void Armageddon()
{
	for (i = 0; i < 11; i++)   // ���� ����� �������
	{
		for (j = 0; j < 11; j++) // ���� ������ ����
		{
			cells[i][j].condition = 0;
			prevgen[i][j].condition = 0;
		}
	}
}


//�������� ������� ����������
void display(void)
{
	for (i = 0; i < 11; i++)   // ���� ����� �������
	{
		for (j = 0; j < 11; j++) // ���� ������ ����
		{
			//����������� ������ ������
			if (i < 10 && j < 10) //�� ������ �� ��������� ����
			{
				glColor3f(0.0f, 0.0f, 0.0f); //������ ����
				if (cells[i][j].condition == 1)
				{
					glColor3f(1.0f, 0.0f, 0.0f);   //���� ����� �������
					drawcell(i, j);
				}
				else
					drawcell(i, j);
			}

			//������ �����
			if (i < 10 && j < 10)
				drawline(i, j);
		}
	}
	//���� �������� ��������, ���������� ����� ���������
	if (start == true && painter == false)
	{
		next_generation();
		//������ ����� ��������� �� ����� ������� � ���������� ��������
		new_generation();
	}

	//���������� ��� �������
	if (painter == false)
	{
		glColor3f(1.0f, 0.0f, 0.0f);   //���� ����� �������
		drawcell(14, 1);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 1.0f);   //���� ����� �����
		drawcell(14, 1);
	}

	glColor3f(0.0f, 1.0f, 0.0f);   //���� ����� �������
	drawcell(14, 3);


	glutSwapBuffers();
}


//��������� ���� OpenGL
void init(void)
{
	/* ��������� ������ ��� */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1000, 1000, 0, -1, 1);
}

//�������, �������� ��������
void Timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(250, Timer, 0);
}

//����� ������� ��������� ����
DWORD WINAPI Opengl()
{
	glutTimerFunc(10, Timer, 1);
	glutMouseFunc(MousePressed);
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0; /* ISO C requires main to return int. */
}


//��������� ��������� ���� , display mode
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
	Opengl(); // ������� ���������
	return 0;
}




