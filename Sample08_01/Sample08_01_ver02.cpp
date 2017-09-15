#define  _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <gl/glut.h>

/*	外部変数	*/
int status = 0;//ボタンの状態
float pos_x = 0;
float pos_y = 0;
int  window_width = 640;
int  window_height = 480;

/* 各種コールバック関数*/
//ディスプレイ
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(1.0f, 0.0f, 0.0f); //赤色
	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glVertex2f( 0.0f,  0.0f);
		glVertex2f( pos_x  ,  pos_y );
	glEnd();

	glFlush();
}

//マウス(クリック)
void mouse(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("左ボタンを押しました@ (%d,%d)\n",x,y);
		status = 1;
		pos_x =  ( (float)x - window_width / 2.0f) *2.0f / window_width ;
		pos_y =  (-(float)y + window_height/ 2.0f) *2.0f / window_height ;

	}else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		printf("左ボタンを離しました@ (%d,%d)\n",x,y);
		status = 0;
	}
}

//ドラッグ
void dragMotion(int x, int y)
{
	if(status == 1){
		printf("DragMotion @ (%d,%d)\n",x,y);
	}
}

//パッシブ
void passiveMotion(int x, int y)
{
	printf("PassiveMotion @ (%d,%d)\n",x,y);
}

/*	メイン関数	*/
int main(int argc, char *argv[])
{
	glutInit(&argc,argv);
	glutInitWindowSize(window_width,window_height);
	glutCreateWindow("Mouse Callback");

	//各種コールバック関数の登録
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(dragMotion);
	glutPassiveMotionFunc(passiveMotion);

	glClearColor(1.0, 1.0, 1.0,1.0);

	glutMainLoop();

	return 0;
}
