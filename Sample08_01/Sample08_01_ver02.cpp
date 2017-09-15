#define  _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <gl/glut.h>

/*	�O���ϐ�	*/
int status = 0;//�{�^���̏��
float pos_x = 0;
float pos_y = 0;
int  window_width = 640;
int  window_height = 480;

/* �e��R�[���o�b�N�֐�*/
//�f�B�X�v���C
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(1.0f, 0.0f, 0.0f); //�ԐF
	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glVertex2f( 0.0f,  0.0f);
		glVertex2f( pos_x  ,  pos_y );
	glEnd();

	glFlush();
}

//�}�E�X(�N���b�N)
void mouse(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("���{�^���������܂���@ (%d,%d)\n",x,y);
		status = 1;
		pos_x =  ( (float)x - window_width / 2.0f) *2.0f / window_width ;
		pos_y =  (-(float)y + window_height/ 2.0f) *2.0f / window_height ;

	}else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		printf("���{�^���𗣂��܂���@ (%d,%d)\n",x,y);
		status = 0;
	}
}

//�h���b�O
void dragMotion(int x, int y)
{
	if(status == 1){
		printf("DragMotion @ (%d,%d)\n",x,y);
	}
}

//�p�b�V�u
void passiveMotion(int x, int y)
{
	printf("PassiveMotion @ (%d,%d)\n",x,y);
}

/*	���C���֐�	*/
int main(int argc, char *argv[])
{
	glutInit(&argc,argv);
	glutInitWindowSize(window_width,window_height);
	glutCreateWindow("Mouse Callback");

	//�e��R�[���o�b�N�֐��̓o�^
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(dragMotion);
	glutPassiveMotionFunc(passiveMotion);

	glClearColor(1.0, 1.0, 1.0,1.0);

	glutMainLoop();

	return 0;
}
