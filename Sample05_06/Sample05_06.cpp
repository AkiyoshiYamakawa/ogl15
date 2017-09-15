#define  _USE_MATH_DEFINES
#include <math.h>
#include <gl/glut.h>

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);//カラー・バッファを初期化
	int N = 20;
	glColor3f(0.f, 0.f, 0.f); // 黒でx軸y軸を描く
	glBegin(GL_LINES);
		glVertex2d(0,-1.0f);
		glVertex2d(0,+1.0f);
		glVertex2d(-1.0f,0.0f);
		glVertex2d(+1.0f,0.0f);
	glEnd();

	glColor3f(1.f, 0.f, 0.f); //赤
	glBegin(GL_LINE_STRIP);
		double x=0;
		for( int i = -N; i<=N; i++){
			x = (double)i/(double)N ;
			glVertex2f( x, 5*(x*x*x-0.5*x) );
		}
	glEnd();

	glFlush();//命令の実行
}

int main(int argc, char *argv[])
{
	glutInit(&argc,argv);

	glutInitWindowSize(640,480);
	glutInitDisplayMode(GLUT_RGBA); //ディスプレイ・モード
	glutCreateWindow("3次曲線:By 90-yamakawa");
		
	glutDisplayFunc(display);//コールバック関数登録

	/* カラー・バッファの初期値*/
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glutMainLoop();

	return 0;
}
