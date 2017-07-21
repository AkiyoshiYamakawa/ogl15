#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <direct.h>
#include <time.h>
#include <GL/glut.h>
//#include <GL/gl_screenshot.h> 
using namespace std;

int WindowPositionX = 200;  //��������E�B���h�E�ʒu��X���W
int WindowPositionY = 200;  //��������E�B���h�E�ʒu��Y���W
int WindowWidth = 500;    //��������E�B���h�E�̕�
int WindowHeight = 500;    //��������E�B���h�E�̍���
char WindowTitle[] = "���E�̎n�܂�";  //�E�B���h�E�̃^�C�g��

static GLfloat floor_planar[4];
static GLfloat floor_s = 50.0f;
static GLfloat pM[16];
static GLfloat lightpos[4] = { -30, -100, 50, 1 };

typedef struct _QUADS_VERTEX{
  GLfloat v0[3];
  GLfloat v1[3];
  GLfloat v2[3];
  GLfloat v3[3];
}QUADS_VERTEX;

static QUADS_VERTEX floor_v = {
  {  floor_s,  floor_s, -1.0f },
  { -floor_s,  floor_s, -1.0f },
  { -floor_s, -floor_s, -1.0f },
  {  floor_s, -floor_s, -1.0f },
};

#define _BITMAP 0
int tn = 0;
double dt = 0.05;
//gl_screenshot gs; //bmp�t�@�C���̏o��

struct {
  double x, y, z;
  double vx, vy, vz;
}p[100];
int pn = 0;
double ax = 0.0 , ay = 0.0 , az = -4.0;
double vx = 5.0 , vy = 5.0 , vz = 20.0; 
double hanpatu = 0.9;
//----------------------------------------------------
// ���������̒�`
//----------------------------------------------------
struct MaterialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess;
};
//jade(�Ő�)
MaterialStruct ms_jade = {
  {0.135,     0.2225,   0.1575,   1.0},
  {0.54,      0.89,     0.63,     1.0},
  {0.316228,  0.316228, 0.316228, 1.0},
  12.8};
//ruby(���r�[)
MaterialStruct ms_ruby  = {
  {0.1745,   0.01175,  0.01175,   1.0},
  {0.61424,  0.04136,  0.04136,   1.0},
  {0.727811, 0.626959, 0.626959,  1.0},
  76.8};
//----------------------------------------------------
// �֐��v���g�^�C�v�i��ɌĂяo���֐����ƈ����̐錾�j
//----------------------------------------------------
void Initialize(void);
void Display(void);
void Idle();
void Keyboard(unsigned char key, int x, int y);
void Ground(void);  //��n�̕`��

void findPlane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3]); 
void shadowMatrix(GLfloat *m, GLfloat plane[4],  GLfloat light[4]);
void DrawFloor(bool bTexture);
void DrawShadow(void);
void DrawStructure(bool);
//----------------------------------------------------
// ���C���֐�
//----------------------------------------------------
int main(int argc, char *argv[]){
  srand((unsigned)time(NULL));
#if _BITMAP
  _mkdir("bitmap"); //bmp�t�@�C���ۑ��p�̃t�H���_�̍쐬
#endif
  glutInit(&argc, argv);//���̏�����
  glutInitWindowPosition(WindowPositionX, WindowPositionY);//�E�B���h�E�̈ʒu�̎w��
  glutInitWindowSize(WindowWidth, WindowHeight); //�E�B���h�E�T�C�Y�̎w��
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//�f�B�X�v���C���[�h�̎w��
  glutCreateWindow(WindowTitle);  //�E�B���h�E�̍쐬
  glutDisplayFunc(Display); //�`�掞�ɌĂяo�����֐����w�肷��i�֐����FDisplay�j
  glutKeyboardFunc(Keyboard);//�L�[�{�[�h���͎��ɌĂяo�����֐����w�肷��i�֐����FKeyboard�j
  glutIdleFunc(Idle);       //�v���O�����A�C�h����Ԏ��ɌĂяo�����֐�
  Initialize(); //�����ݒ�̊֐����Ăяo��
  glutMainLoop();
  return 0;
}
//----------------------------------------------------
// �����ݒ�̊֐�
//----------------------------------------------------
void Initialize(void){
  glClearColor(1.0, 1.0, 1.0, 1.0); //�w�i�F
  glEnable( GL_DEPTH_TEST ); //�f�v�X�o�b�t�@���g�p�FglutInitDisplayMode() �� GLUT_DEPTH ���w�肷��
  glDepthFunc( GL_LEQUAL );
  glClearDepth( 1.0 );

  findPlane( floor_planar,
        floor_v.v0,
        floor_v.v1,
        floor_v.v2 );

  //�����ϊ��s��̐ݒ�------------------------------
  glMatrixMode(GL_PROJECTION);//�s�񃂁[�h�̐ݒ�iGL_PROJECTION : �����ϊ��s��̐ݒ�AGL_MODELVIEW�F���f���r���[�ϊ��s��j
  glLoadIdentity();//�s��̏�����
  gluPerspective(30.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0); //�������e�@�̎��̐�gluPerspactive(th, w/h, near, far);

}
//----------------------------------------------------
// �`��̊֐�
//----------------------------------------------------
void Display(void) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  //���_�̐ݒ�------------------------------
  gluLookAt(
       0.0, -160.0, 40, // ���_�̈ʒux,y,z;
       0.0,  0.0,  0.0,   // ���E�̒��S�ʒu�̎Q�Ɠ_���Wx,y,z
       0.0,  0.0,  1.0 ) ;  //���E�̏�����̃x�N�g��x,y,z
  //----------------------------------------

  //���f���r���[�ϊ��s��̐ݒ�--------------------------
  glMatrixMode(GL_MODELVIEW);//�s�񃂁[�h�̐ݒ�iGL_PROJECTION : �����ϊ��s��̐ݒ�AGL_MODELVIEW�F���f���r���[�ϊ��s��j
  glLoadIdentity();//�s��̏�����
  glViewport(0, 0, WindowWidth, WindowHeight);
  //----------------------------------------------

  //�X�e���V���o�b�t�@�N���A�l�̐ݒ�--------------------------
  glClearStencil( 0 );
  glCullFace( GL_BACK );
  glEnable( GL_CULL_FACE );
  glEnable( GL_AUTO_NORMAL );
  glEnable( GL_NORMALIZE );
  //----------------------------------------

  // ���ʎˉe�s��̎Z�o--------------------------
  shadowMatrix(pM,floor_planar,lightpos);
  //--------------------------

  // ����ON-----------------------------
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glLightfv( GL_LIGHT0,GL_POSITION,lightpos );
  //-----------------------------------

  glPushMatrix();
    DrawStructure(false);
    DrawShadow();
  glPopMatrix();

  glDisable(GL_AUTO_NORMAL);
  glDisable(GL_NORMALIZE);

  //�A�eOFF-----------------------------
  glDisable(GL_LIGHTING);
  //-----------------------------------

  Ground();

#if _BITMAP
  ostringstream fname;
  int tt = tn +10000;
  fname  << "bitmap/" << tt << ".bmp" ;//�o�̓t�@�C����
  string name = fname.str();
  gs.screenshot(name.c_str(), 24);
  tn++ ;
#endif

  glutSwapBuffers(); //glutInitDisplayMode(GLUT_DOUBLE)�Ń_�u���o�b�t�@�����O�𗘗p��
}
//----------------------------------------------------
// ���̂̕`��
//----------------------------------------------------
void DrawStructure(bool flag){
  for(int i=1; i<=pn; i++){
    p[i].vx += ax * dt;
    p[i].vy += ay * dt;
    p[i].vz += az * dt;
    p[i].x += p[i].vx * dt;
    p[i].y += p[i].vy * dt;
    p[i].z += p[i].vz * dt;
    if(p[i].z < 4 && abs(p[i].x) < floor_s  && abs(p[i].y) < floor_s ){
      p[i].z = 4.0;
      p[i].vz = -hanpatu * p[i].vz;
    }
    if(!flag || p[i].z >0){
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
      glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
      glTranslated(p[i].x , p[i].y , p[i].z );//���s�ړ��l�̐ݒ�
      glutSolidSphere(4.0, 20, 20);//�����F(���a, Z���܂��̕�����, Z���ɉ�����������)
    glPopMatrix();
    }
  }
}

//----------------------------------------------------
// ��n�̕`��
//----------------------------------------------------
void Ground(void) {
    double ground_max_x = 300.0;
    double ground_max_y = 300.0;
    glColor3d(0.8, 0.8, 0.8);  // ��n�̐F
    glBegin(GL_LINES);
    for(double ly = -ground_max_y ;ly <= ground_max_y; ly+=10.0){
      glVertex3d(-ground_max_x, ly, -1.1);
      glVertex3d(ground_max_x, ly , -1.1);
    }
    for(double lx = -ground_max_x ;lx <= ground_max_x; lx+=10.0){
      glVertex3d(lx, ground_max_y , -1.1);
      glVertex3d(lx, -ground_max_y, -1.1);
    }
    glEnd();
}

//----------------------------------------------------
// �A�C�h�����ɌĂяo�����֐�
//----------------------------------------------------
void Idle(){
  glutPostRedisplay(); //glutDisplayFunc()���P����s����
}
//----------------------------------------------------
// �L�[�{�[�h���͎��ɌĂяo�����֐�
//----------------------------------------------------
void Keyboard(unsigned char key, int x, int y){
  switch ( key )
  {
  case 'a':
    pn++;
    p[pn].x = -0.0;
    p[pn].y = -10.0;
    p[pn].z = 5.0;
    p[pn].vx = 0.1 * vx * ( (double)rand()/(double)RAND_MAX - (double)rand()/(double)RAND_MAX );
    p[pn].vy = 0.1 * vy * ( (double)rand()/(double)RAND_MAX - (double)rand()/(double)RAND_MAX );
    p[pn].vz = 0.1 * vz * ( (double)rand()/(double)RAND_MAX );
    break;
  case 'q':
    exit(0);
    break;

  default:
    break;
  }
}
//----------------------------------------------------
// �����ʂ̕������ƍs��̌v�Z
//----------------------------------------------------
void findPlane(
  GLfloat plane[4],  // �쐬���镽�ʕ������̌W��
  GLfloat v0[3],    // ���_�P
  GLfloat v1[3],    // ���_�Q
  GLfloat v2[3])    // ���_�R
{
  GLfloat vec0[3], vec1[3];

  // Need 2 vectors to find cross product.
  vec0[0] = v1[0] - v0[0];
  vec0[1] = v1[1] - v0[1];
  vec0[2] = v1[2] - v0[2];

  vec1[0] = v2[0] - v0[0];
  vec1[1] = v2[1] - v0[1];
  vec1[2] = v2[2] - v0[2];

  // find cross product to get A, B, and C of plane equation
  plane[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
  plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
  plane[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];

  plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}
void shadowMatrix(
  GLfloat *m,      // �쐬����s��̃|�C���^
  GLfloat plane[4],  // �ˉe����\�ʂ̕��ʕ������̌W��
  GLfloat light[4])  // �����̓������W�l
{
  GLfloat dot;

  // Find dot product between light position vector and ground plane normal.
  dot = plane[0] * light[0] +
      plane[1] * light[1] +
      plane[2] * light[2] +
      plane[3] * light[3];

  m[0]  = dot - light[0] * plane[0];
  m[4]  = 0.f - light[0] * plane[1];
  m[8]  = 0.f - light[0] * plane[2];
  m[12] = 0.f - light[0] * plane[3];

  m[1]  = 0.f - light[1] * plane[0];
  m[5]  = dot - light[1] * plane[1];
  m[9]  = 0.f - light[1] * plane[2];
  m[13] = 0.f - light[1] * plane[3];

  m[2]  = 0.f - light[2] * plane[0];
  m[6]  = 0.f - light[2] * plane[1];
  m[10] = dot - light[2] * plane[2];
  m[14] = 0.f - light[2] * plane[3];

  m[3]  = 0.f - light[3] * plane[0];
  m[7]  = 0.f - light[3] * plane[1];
  m[11] = 0.f - light[3] * plane[2];
  m[15] = dot - light[3] * plane[3];
}

//----------------------------------------------------
// ���̕`��Ɖe�̕`��
//----------------------------------------------------
void DrawFloor(bool bTexture){
  if( bTexture ){
    // ���Ƀe�N�X�`�����g�����̓R�R�Őݒ肷��
  //  glBindTexture( GL_TEXTURE_2D, );

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
  //    glTexCoord2f( , );
      glVertex3fv( floor_v.v0 );
  //    glTexCoord2f( , );
      glVertex3fv( floor_v.v1 );
  //    glTexCoord2f( , );
      glVertex3fv( floor_v.v2 );
  //    glTexCoord2f( , );
      glVertex3fv( floor_v.v3 );
    glEnd();
    glEnable(GL_LIGHTING);
  }else{
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
      glVertex3fv( floor_v.v0 );
      glVertex3fv( floor_v.v1 );
      glVertex3fv( floor_v.v2 );
      glVertex3fv( floor_v.v3 );
    glEnd();
    glEnable(GL_LIGHTING);
  }
}
void DrawShadow(void){
  /////////////////////////////////////////////
  //���̃X�e���V����t����
  glEnable(GL_STENCIL_TEST);
  glStencilFunc( GL_ALWAYS, 1, ~0);
  //���ꂩ��`�悷����̂̃X�e���V���l�ɂ��ׂĂP�^�O������
  glStencilOp(GL_KEEP,GL_KEEP ,GL_REPLACE);
  glColor4f(0.7f, 0.4f, 0.0f, 1.0f);
  DrawFloor( true );//���̕`��

  /////////////////////////////////////////////
  //�J���[�E�f�v�X�o�b�t�@�}�X�N���Z�b�g����
  //����ňȉ��̓��e�̃s�N�Z���̐F�̒l�́A�������܂�Ȃ��B
  glColorMask(0,0,0,0);
  glDepthMask(0);
  /////////////////////////////////////////////
  //���ɃI�u�W�F�N�g�̉e�̃X�e���V����t����
  glEnable(GL_STENCIL_TEST);
  glStencilFunc( GL_EQUAL, 1, ~0);
  //���ꂩ��`�悷����̂̃X�e���V���l�ɂ��ׂĂP�^�O������
  glStencilOp(GL_KEEP,GL_KEEP ,GL_INCR);
  glDisable(GL_DEPTH_TEST);
  glPushMatrix();
    glMultMatrixf(pM);
    DrawStructure(true);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);

  /////////////////////////////////////////////
  //�r�b�g�}�X�N������
  glColorMask(1,1,1,1);
  glDepthMask(1);

  /////////////////////////////////////////////
  //�e������
  glStencilFunc( GL_EQUAL, 2, ~0 );
  glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glDisable(GL_DEPTH_TEST);
      DrawFloor( false );//���̕`��
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
}

/*
typedef struct _QUADS_VERTEX{
  GLfloat v0[3];
  GLfloat v1[3];
  GLfloat v2[3];
  GLfloat v3[3];
}QUADS_VERTEX;
*/

/*
static QUADS_VERTEX floor_v = {
  {  floor_s,  floor_s, -1.0f },
  { -floor_s,  floor_s, -1.0f },
  { -floor_s, -floor_s, -1.0f },
  {  floor_s, -floor_s, -1.0f },
};
*/

//----------------------------------------------------
// ���̂̕`��
//----------------------------------------------------
/*
void DrawStructure(bool flag){
  for(int i=1; i<=pn; i++){
    p[i].vx += ax * dt;
    p[i].vy += ay * dt;
    p[i].vz += az * dt;
    p[i].x += p[i].vx * dt;
    p[i].y += p[i].vy * dt;
    p[i].z += p[i].vz * dt;
    if(p[i].z < 4 && abs(p[i].x) < floor_s  && abs(p[i].y) < floor_s ){
      p[i].z = 4.0;
      p[i].vz = -hanpatu * p[i].vz;
    }
    if(!flag || p[i].z >0){
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
      glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
      glTranslated(p[i].x , p[i].y , p[i].z );//���s�ړ��l�̐ݒ�
      glutSolidSphere(4.0, 20, 20);//�����F(���a, Z���܂��̕�����, Z���ɉ�����������)
    glPopMatrix();
    }
  }
}
*/
