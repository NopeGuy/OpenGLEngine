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
#include "parser.cpp"
#include "parser.hpp"

using namespace std;

// Defines de cores
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

int mode = GL_LINE;

Parser parser = NULL;
List figuras = NULL;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// cálculo do aspect ratio 
	float ratio = w * 1.0 / h;

	// Define a matriz de projeção como atual
	glMatrixMode(GL_PROJECTION);
	// Carrega a Matriz Identidade
	glLoadIdentity();
	
	// Define a área de visualização como a janela inteira
    glViewport(0, 0, w, h);

	// Define a perspetiva
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// Retorna ao modo de matriz de visualização de modelo
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

	// Definir câmara
	glLoadIdentity();
	gluLookAt(radius*cos(beta_)*sin(alpha), radius*sin(beta_), radius*cos(beta_)*cos(alpha),
		      lookAtx,lookAty,lookAtz,
			  upx,upy,upz);

	// Eixos (Depois esconder se preciso)
	glBegin(GL_LINES);
		// Eixo X
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Eixo Y
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f,-100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Eixo Z
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f,-100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	// figuras
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glBegin(GL_TRIANGLES);
	drawFiguras(figuras);
    glEnd();
	
	glutSwapBuffers();
}


void specKeyProc(int key_code, int x, int y) {
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

void keyProc(unsigned char key, int x, int y) {
	switch (key)
	{
		case 'a': { // esquerda
			alpha -= 0.1f;
			break;
		}

		case 'd': { // direita
			alpha += 0.1f;
			break;
		}

		case 'w': { // cima 
			beta_ += beta_ <= 1.48f ? 0.1f : 0.0f;
			break;
		}

		case 's': { // baixo
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

	Parser parser = newParser();
	// Carregar os dados das figuras
	parser = xmlToConfig(argv[1]); 
	List models   = getModels(parser);
	figuras 	  = newEmptyList();
	for(unsigned int i = 0; i < getListLength(models); i++){
		addValueList(figuras, fileToFigura((char*)getListElemAt(models,i)));
	}
	// Carregamento dos dados da câmara
	camx    = getXPosCam(configuration);
	camy    = getYPosCam(configuration);
	camz    = getZPosCam(configuration);
	radius  = sqrt(camx*camx + camy*camy + camz*camz);
	lookAtx = getXLookAt(configuration);
	lookAty = getYLookAt(configuration);
	lookAtz = getZLookAt(configuration);
	upx 	= getXUp(configuration);
	upy 	= getYUp(configuration);
	upz 	= getZUp(configuration);
	alpha = acos(camz/sqrt(camx*camx + camz*camz));
	beta_ = asin(camy/radius);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1200,980);
	glutCreateWindow("3D Engine");
		

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
	// Depois tirar
	glutKeyboardFunc(keyProc);
	glutSpecialFunc(specKeyProc);


	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// GLUT's main cycle
	glutMainLoop();
	
	deepDeleteList(figuras,deleteFigura);
	return 1;
}