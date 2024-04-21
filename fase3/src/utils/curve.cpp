#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/glew.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "math.h"
#include "../utils/figura.hpp"
#include "../utils/ponto.hpp"
#include "../utils/list.hpp"
#include "../tinyXML/tinyxml2.h"
#include "../utils/parser.hpp"
#include <vector>
#include "../utils/curve.hpp"
// Variável que define o nível de tesselação da curva
float tesselation = 0.01;
// Variável que define a posição do objeto
float Pos[3] = { 0,0,0 };
// Variável que define a derivada do objeto
float Deriv[3] = { 0,0,0 };

// Função que multiplica uma matriz por um vetor
void multMatrixVector(float* m, float* v, float* res) {
	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}


void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {
	// Matriz Catmull-Rom
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
					 { 1.0f, -2.5f,  2.0f, -0.5f},
					 {-0.5f,  0.0f,  0.5f,  0.0f},
					 { 0.0f,  1.0f,  0.0f,  0.0f} };

	// 
	float T[4] = { t * t * t, t * t, t, 1 };
	float T_deriv[4] = { 3 * t * t,2 * t, 1, 0 };


	for (int i = 0; i < 3; i++) { // x, y, z
		float p[4] = { p0[i], p1[i], p2[i], p3[i] };
		float A[4];

		//Compute vector A = M * P 
		multMatrixVector((float*)m, p, A);

		//Compute pos[i] = T * A
		pos[i] = T[0] * A[0] + T[1] * A[1] + T[2] * A[2] + T[3] * A[3];

		//compute deriv[i] = T' * A
		deriv[i] = T_deriv[0] * A[0] + T_deriv[1] * A[1] + T_deriv[2] * A[2] + T_deriv[3] * A[3];
	}
}
void getGlobalCatmullRomPoint(float gt, vector<float> px, vector<float> py, vector<float> pz, float* pos, float* deriv) {
	int POINT_COUNT = (int)px.size();

	// Tempo local (real)
	float t = gt * POINT_COUNT;
	int index = floor(t);
	t = t - index;

	// Determina os indices dos pontos de controlo
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;

	// Determina os pontos de controlo a serem utilizados
	float p[4][3];
	p[0][0] = px[indices[0]];
	p[0][1] = py[indices[0]];
	p[0][2] = pz[indices[0]];

	p[1][0] = px[indices[1]];
	p[1][1] = py[indices[1]];
	p[1][2] = pz[indices[1]];

	p[2][0] = px[indices[2]];
	p[2][1] = py[indices[2]];
	p[2][2] = pz[indices[2]];

	p[3][0] = px[indices[3]];
	p[3][1] = py[indices[3]];
	p[3][2] = pz[indices[3]];

	getCatmullRomPoint(t, p[0], p[1], p[2], p[3], pos, deriv);
}
void renderCatmullRomCurve(vector<float> px, vector<float> py, vector<float> pz) {
	// draw curve using line segments with GL_LINE_LOOP
	glBegin(GL_LINE_LOOP);
	for (float i = 0; i < 1; i += tesselation) {
		getGlobalCatmullRomPoint(i, px, py, pz, Pos, Deriv);
		glVertex3f(Pos[0], Pos[1], Pos[2]);
	}
	glEnd();
}