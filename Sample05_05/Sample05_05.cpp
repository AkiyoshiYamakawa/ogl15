#include <gl/glut.h>

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);//�J���[�E�o�b�t�@��������
	
	glColor3f(1.0f, 0.0f, 0.0f); //�ԐF
	glLineWidth(3.0f);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(-0.8f, +0.0f);

		glVertex2f(-0.0f, -0.8f);

		glVertex2f(+0.4f, -0.5f);

		glVertex2f(+0.5f, +0.3f);

		glVertex2f(-0.1f, +0.6f);

	glEnd();

	glFlush();//���߂̎��s
}

int main(int argc, char *argv[])
{
	glutInit(&argc,argv);

	glutInitWindowSize(640,480);
	glutInitDisplayMode(GLUT_RGBA); //�f�B�X�v���C�E���[�h
	glutCreateWindow("Color Buffer: 90-yamakawa");
		
	glutDisplayFunc(display);//�R�[���o�b�N�֐��o�^

	/* �J���[�E�o�b�t�@�̏����l*/
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glutMainLoop();

	return 0;
}