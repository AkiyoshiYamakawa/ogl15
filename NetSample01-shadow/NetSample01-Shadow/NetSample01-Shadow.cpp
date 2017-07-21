#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <direct.h>
#include <time.h>
#include <GL/glut.h>
//#include <GL/gl_screenshot.h> 
using namespace std;

int WindowPositionX = 200;  //生成するウィンドウ位置のX座標
int WindowPositionY = 200;  //生成するウィンドウ位置のY座標
int WindowWidth = 500;    //生成するウィンドウの幅
int WindowHeight = 500;    //生成するウィンドウの高さ
char WindowTitle[] = "世界の始まり";  //ウィンドウのタイトル

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
//gl_screenshot gs; //bmpファイルの出力

struct {
  double x, y, z;
  double vx, vy, vz;
}p[100];
int pn = 0;
double ax = 0.0 , ay = 0.0 , az = -4.0;
double vx = 5.0 , vy = 5.0 , vz = 20.0; 
double hanpatu = 0.9;
//----------------------------------------------------
// 物質質感の定義
//----------------------------------------------------
struct MaterialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess;
};
//jade(翡翠)
MaterialStruct ms_jade = {
  {0.135,     0.2225,   0.1575,   1.0},
  {0.54,      0.89,     0.63,     1.0},
  {0.316228,  0.316228, 0.316228, 1.0},
  12.8};
//ruby(ルビー)
MaterialStruct ms_ruby  = {
  {0.1745,   0.01175,  0.01175,   1.0},
  {0.61424,  0.04136,  0.04136,   1.0},
  {0.727811, 0.626959, 0.626959,  1.0},
  76.8};
//----------------------------------------------------
// 関数プロトタイプ（後に呼び出す関数名と引数の宣言）
//----------------------------------------------------
void Initialize(void);
void Display(void);
void Idle();
void Keyboard(unsigned char key, int x, int y);
void Ground(void);  //大地の描画

void findPlane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3]); 
void shadowMatrix(GLfloat *m, GLfloat plane[4],  GLfloat light[4]);
void DrawFloor(bool bTexture);
void DrawShadow(void);
void DrawStructure(bool);
//----------------------------------------------------
// メイン関数
//----------------------------------------------------
int main(int argc, char *argv[]){
  srand((unsigned)time(NULL));
#if _BITMAP
  _mkdir("bitmap"); //bmpファイル保存用のフォルダの作成
#endif
  glutInit(&argc, argv);//環境の初期化
  glutInitWindowPosition(WindowPositionX, WindowPositionY);//ウィンドウの位置の指定
  glutInitWindowSize(WindowWidth, WindowHeight); //ウィンドウサイズの指定
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//ディスプレイモードの指定
  glutCreateWindow(WindowTitle);  //ウィンドウの作成
  glutDisplayFunc(Display); //描画時に呼び出される関数を指定する（関数名：Display）
  glutKeyboardFunc(Keyboard);//キーボード入力時に呼び出される関数を指定する（関数名：Keyboard）
  glutIdleFunc(Idle);       //プログラムアイドル状態時に呼び出される関数
  Initialize(); //初期設定の関数を呼び出す
  glutMainLoop();
  return 0;
}
//----------------------------------------------------
// 初期設定の関数
//----------------------------------------------------
void Initialize(void){
  glClearColor(1.0, 1.0, 1.0, 1.0); //背景色
  glEnable( GL_DEPTH_TEST ); //デプスバッファを使用：glutInitDisplayMode() で GLUT_DEPTH を指定する
  glDepthFunc( GL_LEQUAL );
  glClearDepth( 1.0 );

  findPlane( floor_planar,
        floor_v.v0,
        floor_v.v1,
        floor_v.v2 );

  //透視変換行列の設定------------------------------
  glMatrixMode(GL_PROJECTION);//行列モードの設定（GL_PROJECTION : 透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
  glLoadIdentity();//行列の初期化
  gluPerspective(30.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0); //透視投影法の視体積gluPerspactive(th, w/h, near, far);

}
//----------------------------------------------------
// 描画の関数
//----------------------------------------------------
void Display(void) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  //視点の設定------------------------------
  gluLookAt(
       0.0, -160.0, 40, // 視点の位置x,y,z;
       0.0,  0.0,  0.0,   // 視界の中心位置の参照点座標x,y,z
       0.0,  0.0,  1.0 ) ;  //視界の上方向のベクトルx,y,z
  //----------------------------------------

  //モデルビュー変換行列の設定--------------------------
  glMatrixMode(GL_MODELVIEW);//行列モードの設定（GL_PROJECTION : 透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
  glLoadIdentity();//行列の初期化
  glViewport(0, 0, WindowWidth, WindowHeight);
  //----------------------------------------------

  //ステンシルバッファクリア値の設定--------------------------
  glClearStencil( 0 );
  glCullFace( GL_BACK );
  glEnable( GL_CULL_FACE );
  glEnable( GL_AUTO_NORMAL );
  glEnable( GL_NORMALIZE );
  //----------------------------------------

  // 平面射影行列の算出--------------------------
  shadowMatrix(pM,floor_planar,lightpos);
  //--------------------------

  // 光源ON-----------------------------
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

  //陰影OFF-----------------------------
  glDisable(GL_LIGHTING);
  //-----------------------------------

  Ground();

#if _BITMAP
  ostringstream fname;
  int tt = tn +10000;
  fname  << "bitmap/" << tt << ".bmp" ;//出力ファイル名
  string name = fname.str();
  gs.screenshot(name.c_str(), 24);
  tn++ ;
#endif

  glutSwapBuffers(); //glutInitDisplayMode(GLUT_DOUBLE)でダブルバッファリングを利用可
}
//----------------------------------------------------
// 物体の描画
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
      glTranslated(p[i].x , p[i].y , p[i].z );//平行移動値の設定
      glutSolidSphere(4.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();
    }
  }
}

//----------------------------------------------------
// 大地の描画
//----------------------------------------------------
void Ground(void) {
    double ground_max_x = 300.0;
    double ground_max_y = 300.0;
    glColor3d(0.8, 0.8, 0.8);  // 大地の色
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
// アイドル時に呼び出される関数
//----------------------------------------------------
void Idle(){
  glutPostRedisplay(); //glutDisplayFunc()を１回実行する
}
//----------------------------------------------------
// キーボード入力時に呼び出される関数
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
// 床平面の方程式と行列の計算
//----------------------------------------------------
void findPlane(
  GLfloat plane[4],  // 作成する平面方程式の係数
  GLfloat v0[3],    // 頂点１
  GLfloat v1[3],    // 頂点２
  GLfloat v2[3])    // 頂点３
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
  GLfloat *m,      // 作成する行列のポインタ
  GLfloat plane[4],  // 射影する表面の平面方程式の係数
  GLfloat light[4])  // 光源の同時座標値
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
// 床の描画と影の描画
//----------------------------------------------------
void DrawFloor(bool bTexture){
  if( bTexture ){
    // 床にテクスチャを使う時はココで設定する
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
  //床のステンシルを付ける
  glEnable(GL_STENCIL_TEST);
  glStencilFunc( GL_ALWAYS, 1, ~0);
  //これから描画するもののステンシル値にすべて１タグをつける
  glStencilOp(GL_KEEP,GL_KEEP ,GL_REPLACE);
  glColor4f(0.7f, 0.4f, 0.0f, 1.0f);
  DrawFloor( true );//床の描画

  /////////////////////////////////////////////
  //カラー・デプスバッファマスクをセットする
  //これで以下の内容のピクセルの色の値は、書き込まれない。
  glColorMask(0,0,0,0);
  glDepthMask(0);
  /////////////////////////////////////////////
  //床にオブジェクトの影のステンシルを付ける
  glEnable(GL_STENCIL_TEST);
  glStencilFunc( GL_EQUAL, 1, ~0);
  //これから描画するもののステンシル値にすべて１タグをつける
  glStencilOp(GL_KEEP,GL_KEEP ,GL_INCR);
  glDisable(GL_DEPTH_TEST);
  glPushMatrix();
    glMultMatrixf(pM);
    DrawStructure(true);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);

  /////////////////////////////////////////////
  //ビットマスクを解除
  glColorMask(1,1,1,1);
  glDepthMask(1);

  /////////////////////////////////////////////
  //影をつける
  glStencilFunc( GL_EQUAL, 2, ~0 );
  glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glDisable(GL_DEPTH_TEST);
      DrawFloor( false );//床の描画
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
// 物体の描画
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
      glTranslated(p[i].x , p[i].y , p[i].z );//平行移動値の設定
      glutSolidSphere(4.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();
    }
  }
}
*/
