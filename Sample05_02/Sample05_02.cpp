#include <gl/glut.h>

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);//カラー・バッファを初期化
	
	glColor3f(1.0f, 0.0f, 0.0f); //赤色
	glLineWidth(3.0f);
	glBegin(GL_LINE_STRIP);
		glVertex2f(-0.8f, -0.8f);

		glVertex2f(+0.8f, +0.8f);

		glVertex2f(+0.8f, -0.8f);

		glVertex2f(-0.2f, +0.7f);

		glVertex2f(-0.9f, +0.2f);
	glEnd();


	glFlush();//命令の実行
}

int main(int argc, char *argv[])
{
	glutInit(&argc,argv);

	glutInitWindowSize(640,480);
	glutInitDisplayMode(GLUT_RGBA); //ディスプレイ・モード
	glutCreateWindow("Color Buffer: 90-yamakawa");
		
	glutDisplayFunc(display);//コールバック関数登録

	/* カラー・バッファの初期値*/
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glutMainLoop();

	return 0;
}
