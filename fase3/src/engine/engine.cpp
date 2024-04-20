#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/glew.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include "math.h"
#include "../utils/figura.hpp"
#include "../utils/ponto.hpp"
#include "../utils/list.hpp"
#include "../tinyXML/tinyxml2.h"
#include "../utils/parser.hpp"
#include <vector>

using namespace std;

#define NOW ((1.0f*glutGet(GLUT_ELAPSED_TIME))/1000.0f)
float init_time = 0.0f;


GLuint verticeCount;
GLuint vertices = 0;

// VBO's
GLuint* buffers = NULL; // temos um buffer para cada figura
int info[100]; // aqui guardamos o tamanho de cada buffer de cada figura
unsigned int figCount = 0; // total de figuras existentes no ficheiro de configuração.
GLuint bufferId[100]; // buffer id

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

Parser* settings = new Parser();
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

// fill lists

void fillList(const Group* group)
{
	if (group != nullptr)
	{
		int modelLength = group->modelFiles.size();

		for (int i = 0; i < modelLength; i++) {
			const char* fileChar = group->modelFiles.at(i).fileName.c_str();
			printf("\n boda: %s \n" , fileChar);
			addValueList(figuras, fileToFigura(fileChar));
			printf("Model file: %s\n", fileChar);
		}
		for (const auto& child : group->children)
			fillList(&child);
	}
}


void printModelsFromSettings(const Parser* settings) {
	for (const auto& model : settings->rootNode.modelFiles) {
		printf("Model file: %s\n", model.fileName.c_str());
	}
}

void importFiguras(List figs) {
	figCount = getListLength(figs);
	printf("Figuras: %d\n", figCount);


	// Iterate through the list of figures and create buffers
	for (unsigned long i = 0; i < getListLength(figs); i++) {
		printf("Figura %lu\n", i);
		glGenBuffers(i+1, &bufferId[i]);
		Figura fig = (Figura)getListElemAt(figs, i);
		List figPontos = getPontos(fig);
		vector<float> vVertices;

		// Iterate through the list of points in the figure and populate vVertices
		for (unsigned long j = 0; j < getListLength(figPontos); j++) {
			Ponto point = (Ponto)getListElemAt(figPontos, j);
			vVertices.push_back(getX(point));
			vVertices.push_back(getY(point));
			vVertices.push_back(getZ(point));
		}
		// Calculate the number of vertices
		info[i] = (vVertices.size() / 3);
		printf("Vertices: %d\n", info[i]);

		// Bind the buffer and fill it with data
		glBindBuffer(GL_ARRAY_BUFFER, bufferId[i]);
		glBufferData(GL_ARRAY_BUFFER, vVertices.size() * sizeof(float), vVertices.data(), GL_STATIC_DRAW);
	}
}


void draw() {
	for (unsigned long i = 0; i < figCount; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferId[i]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, info[i]);
	}
}

// Function to execute transformations
void executeTransformations(const Transform& transform) {
	float x = transform.x;
	float y = transform.y;
	float z = transform.z;
	char tr_type = transform.type;
	if (tr_type == 'r') { // Rotation
		float r_angle = transform.angle;
		float r_time = transform.time;
		if (r_time > 0.0f) {
			r_angle = ((NOW - init_time) * 360.0f) / r_time;
		}
		glRotatef(r_angle, x, y, z);
	}
	else if (tr_type == 't') { // Translation
		float t_time = transform.time;
		if (t_time > 0.0f) {
			// Handle translation animation
		}
		else {
			glTranslatef(x, y, z);
		}
	}
	else if (tr_type == 's') { // Scale
		glScalef(x, y, z); // Fix: Pass x, y, z as arguments
	}
}


// Desenha os eixos, caso a flag esteja ativa.
void drawEixos() {
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(RED);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(GREEN);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(BLUE);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glColor3f(WHITE);
	glEnd();
}

void drawTeapot() {
	glPushMatrix();
	glColor3f(WHITE);
	glTranslatef(-5, 0, 0);
	glutWireTeapot(1);
	glPopMatrix();
}

void drawGroups(const Group* group) {
	if (group != nullptr) {
		glPushMatrix(); // Push the current matrix onto the stack
		for (const auto& transform : group->transforms) {
			executeTransformations(transform);
		}


		draw(); // Draw the figure(s

		// Render child groups
		for (const auto& child : group->children) {
			drawGroups(&child);
		}

		glPopMatrix(); // Restore the previous matrix
	}
}

void renderScene(void) {
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera
	glLoadIdentity();
	gluLookAt(camx, camy, camz, lookAtx, lookAty, lookAtz, upx, upy, upz);

	glColor3f(1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	// Draw eixos
	drawEixos();
	drawGroups(&settings->rootNode);
	// Apply transformations and draw groups
	//drawGroups(&settings->rootNode);

	// End of frame
	glutSwapBuffers();
}

void specKeyProc(int key_code, int x, int y) {
	switch (key_code){
		case GLUT_KEY_UP:{
			radius -= 0.5f;
			break;
		}
		
		case GLUT_KEY_DOWN:{
			radius += 0.5f;
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
	camx = radius * sin(alpha) * cos(beta_);
	camy = radius * sin(beta_);
	camz = radius * cos(alpha) * cos(beta_);
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
	settings = ParserSettingsConstructor(filePath);
	camx    = settings->camera.position.x;	
	camy    = settings->camera.position.y;
	camz    = settings->camera.position.z;
	lookAtx = settings->camera.lookAt.x;
	lookAty = settings->camera.lookAt.y;
	lookAtz = settings->camera.lookAt.z;
	upx		= settings->camera.up.x;
	upz		= settings->camera.up.z;
	upy		= settings->camera.up.y;
	fov		= settings->camera.projection.fov;
	near	= settings->camera.projection.near;
	far		= settings->camera.projection.far;
	width	= settings->window.width;
	height	= settings->window.height;
	radius	= sqrt(camx * camx + camy * camy + camz * camz);
	alpha   = acos(camz/sqrt(camx*camx + camz*camz));
	beta_   = asin(camy/radius); 

	fillList(&settings->rootNode);

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Engine");
	glewInit();
	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	// put here the registration of the keyboard callbacks (por enquanto só mexem na camara como forma de debug)
	glutKeyboardFunc(keyProc);
	glutSpecialFunc(specKeyProc);

	glEnableClientState(GL_VERTEX_ARRAY);

	importFiguras(figuras);

//	drawGroups(&settings->rootNode);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// enter GLUT's main cycle
	glutMainLoop();
	
	deepDeleteList(figuras, deleteFigura);

	return 1;
} 

/* Debugging function to print the parsed values

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <xml_file_path>" << std::endl;
		return 1;
	}

	const std::string filePath = argv[1];

	// Create a Parser object and parse the XML file
	Parser* settings = ParserSettingsConstructor(filePath);
	if (!settings) {
		std::cerr << "Failed to parse XML file." << std::endl;
		return 1;
	}

	// Print the parsed values
	print(*settings);

	// Clean up memory
	delete settings;

	return 0;
}
*/