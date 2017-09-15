#define  _USE_MATH_DEFINES
#include <math.h>
#include <gl/glut.h>

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);//�J���[�E�o�b�t�@��������
	int N = 20;
	glColor3f(0.f, 0.f, 0.f); // ����x��y����`��
	glBegin(GL_LINES);
		glVertex2d(0,-1.0f);
		glVertex2d(0,+1.0f);
		glVertex2d(-1.0f,0.0f);
		glVertex2d(+1.0f,0.0f);
	glEnd();

	glColor3f(1.f, 0.f, 0.f); //��
	glBegin(GL_LINE_STRIP);
		double x=0;
		for( int i = -N; i<=N; i++){
			x = (double)i/(double)N ;
			glVertex2f( x, 5*(x*x*x-0.5*x) );
		}
	glEnd();

	glFlush();//���߂̎��s
}

int main(int argc, char *argv[])
{
	glutInit(&argc,argv);

	glutInitWindowSize(640,480);
	glutInitDisplayMode(GLUT_RGBA); //�f�B�X�v���C�E���[�h
	glutCreateWindow("3���Ȑ�:By 90-yamakawa");
		
	glutDisplayFunc(display);//�R�[���o�b�N�֐��o�^

	/* �J���[�E�o�b�t�@�̏����l*/
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glutMainLoop();

	return 0;
}
