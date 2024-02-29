#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include "math.h"
#include "../utils/figura.hpp"
#include "../utils/ponto.hpp"
#include "../utils/list.hpp"
#include "../tinyXML/tinyxml2.h"
#include "../utils/parser.hpp"

using namespace std;

// Códigos de cores
#define RED 1.0f,0.0f,0.0f
#define GREEN 0.0f,1.0f,0.0f
#define BLUE 0.0f,0.0f,1.0f
#define YELLOW 1.0f, 1.0f, 0.0f
#define CYAN 0.0f, 1.0f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f

// Variáveis da câmara
float alpha = M_PI / 4;
float beta_ = M_PI / 4;
float radius = 5.0f;
float camx = 5.0f;
float camy = 5.0f;
float camz = 5.0f;
float lookAtx = 0.0f;
float lookAty = 0.0f;
float lookAtz = 0.0f;
float upx = 0.0f;
float upy = 0.0f;
float upz = 0.0f;
float fov = 0.0f;
float near = 0.0f;
float far = 0.0f;


// Window Settings
int width = 0;
int height = 0;

// File Settings
std::string fileName = "";

int mode = GL_LINE;

List figuras = NULL;



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawFiguras(List figs){
	for(unsigned long i = 0; i < getListLength(figs); i++){
		Figura fig = (Figura)getListElemAt(figs,i);
		List figPontos = getPontos(fig);
		for(unsigned long j = 0; j < getListLength(figPontos); j++){
			Ponto point = (Ponto)getListElemAt(figPontos,j);
			glVertex3f(getX(point), getY(point), getZ(point));
		}
	}
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(radius*cos(beta_)*sin(alpha), radius*sin(beta_), radius*cos(beta_)*cos(alpha),
		      lookAtx,lookAty,lookAtz,
			  upx,upy,upz);

	// put drawing instructions here
	// linhas dos eixos
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f,-100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f,-100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	// put the geometric transformations here
	// ...
	//

	// figuras
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glBegin(GL_TRIANGLES);
	drawFiguras(figuras);
    glEnd();
	
	// End of frame
	glutSwapBuffers();
}

// write function to process keyboard events

// Só altera a posição da camera, para debug.
void specKeyProc(int key_code, int x, int y) {
	x = y; y=x; // Para não aparecerem os warnings.
	switch (key_code){
		case GLUT_KEY_UP:{
			radius -= 0.1f;
			break;
		}
		
		case GLUT_KEY_DOWN:{
			radius += 0.1f;
			break;
		}

		default:
			break;
	}
	glutPostRedisplay();
}

// Só altera a posição da camera, para debug, e altera os modes para GL_FILL, GL_LINES, GL_POINT
void keyProc(unsigned char key, int x, int y) {
	x = y; y=x; // Para não aparecerem os warnings.
	switch (key)
	{
		case 'a': { // left
			alpha -= 0.1f;
			break;
		}

		case 'd': { // right
			alpha += 0.1f;
			break;
		}

		case 'w': { // up 
			beta_ += beta_ <= 1.48f ? 0.1f : 0.0f;
			break;
		}

		case 's': { // down
			beta_ -= beta_ >= -1.48f ? 0.1f : 0.0f;
			break;
		}

		case('f'):
			mode = GL_FILL;
			break;

		case('l'):
			mode = GL_LINE;
			break;

		case('p'):
			mode = GL_POINT;
			break;

		default:
			break;
	}
	glutPostRedisplay();
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <path to xml file>\n", argv[0]);
		return 1;
	}
	std::string filePath = argv[1];	
	printf("File path: %s\n", filePath.c_str());
	figuras = newEmptyList();
	ParserSettings* settings = ParserSettingsConstructor(filePath);
	camx    = settings->camera.positionX;
	camy    = settings->camera.positionY;
	camz    = settings->camera.positionZ;
	lookAtx = settings->camera.lookAtX;
	lookAty = settings->camera.lookAtY;
	lookAtz = settings->camera.lookAtZ;
	upx 	= settings->camera.upX;
	upy 	= settings->camera.upY;
	upz 	= settings->camera.upZ;
	fov     = settings->camera.fov;
	near    = settings->camera.near;
	far     = settings->camera.far;
	width	= settings->window.width;
	height	= settings->window.height;
	fileName = settings->modelFiles[0].fileName;
	radius	= sqrt(camx * camx + camy * camy + camz * camz);
	alpha   = acos(camz/sqrt(camx*camx + camz*camz));
	beta_   = asin(camy/radius); 

	const char* fileChar = fileName.c_str();
	addValueList(figuras, fileToFigura(fileChar));




	printf("camx: %f\n", camx);
	printf("camy: %f\n", camy);
	printf("camz: %f\n", camz);
	printf("lookAtx: %f\n", lookAtx);
	printf("lookAty: %f\n", lookAty);
	printf("lookAtz: %f\n", lookAtz);
	printf("upx: %f\n", upx);
	printf("upy: %f\n", upy);
	printf("upz: %f\n", upz);
	printf("fov: %f\n", fov);
	printf("near: %f\n", near);
	printf("far: %f\n", far);
	printf("width: %d\n", width);
	printf("height: %d\n", height);
	printf("fileName: %s\n", fileName.c_str());

	
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Engine");
		
	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
	// put here the registration of the keyboard callbacks (por enquanto só mexem na camara como forma de debug)
	glutKeyboardFunc(keyProc);
	glutSpecialFunc(specKeyProc);


	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// enter GLUT's main cycle
	glutMainLoop();
	
	deepDeleteList(figuras,deleteFigura);
	return 1;
}