#ifndef FIGURA
#define FIGURA
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "ponto.hpp"
#include "list.hpp"

using namespace std;

typedef struct figura* Figura;

Figura newEmptyFigura();

Figura newFigura(List);

void addPonto(Figura, Ponto);

void addPontos(Figura, Figura);

Figura fileToFigura(const char*);

vector<float> getPontos(Figura f);

vector<float> getNormais(Figura f);

void deleteFigura(void*);


#endif // FIGURA