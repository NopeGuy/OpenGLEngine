#include "geometry.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <filesystem>
#include <cmath>
#include <corecrt_math_defines.h>

#include "../utils/ponto.hpp"
#include <fstream>

namespace fs = std::filesystem;

void generateSphere(float radius, int slices, int stacks, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita: " << filePath << std::endl;
        return;
    }

    int totalVertices = 2 * 3 * slices * stacks; // Cada quadrado na esfera é dividido em 2 triângulos, 3 vértices por triângulo
    file << totalVertices << std::endl;

    float deltaPhi = M_PI / stacks; // Intervalo de variação para phi (de 0 a PI)
    float deltaTheta = 2 * M_PI / slices; // Intervalo de variação para theta (de 0 a 2*PI)

    for (int i = 0; i < stacks; ++i) {
        float phi = i * deltaPhi;
        float nextPhi = (i + 1) * deltaPhi;

        for (int j = 0; j < slices; ++j) {
            float theta = j * deltaTheta;
            float nextTheta = (j + 1) * deltaTheta;

            // Coordenadas dos vértices do quadrado atual
            float x1 = radius * sinf(phi) * cosf(theta);
            float y1 = radius * cosf(phi);
            float z1 = radius * sinf(phi) * sinf(theta);

            float x2 = radius * sinf(nextPhi) * cosf(theta);
            float y2 = radius * cosf(nextPhi);
            float z2 = radius * sinf(nextPhi) * sinf(theta);

            float x3 = radius * sinf(phi) * cosf(nextTheta);
            float y3 = radius * cosf(phi);
            float z3 = radius * sinf(phi) * sinf(nextTheta);

            float x4 = radius * sinf(nextPhi) * cosf(nextTheta);
            float y4 = radius * cosf(nextPhi);
            float z4 = radius * sinf(nextPhi) * sinf(nextTheta);

            // Primeiro triângulo
            file << x1 << "," << y1 << "," << z1 << "\n";
            file << x2 << "," << y2 << "," << z2 << "\n";
            file << x4 << "," << y4 << "," << z4 << "\n";

            // Segundo triângulo
            file << x4 << "," << y4 << "," << z4 << "\n";
            file << x3 << "," << y3 << "," << z3 << "\n";
            file << x1 << "," << y1 << "," << z1 << "\n";
        }
    }

    file.close();
    std::cout << "Arquivo '" << filePath << "' criado com sucesso." << std::endl;
}

void generateBox(float size, int divisions, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    float step = size / divisions;
    float half = size / 2;
    int totalVertices = 6 * 2 * divisions * divisions * 3;
    file << totalVertices << std::endl;

    // Função de vértices (para ser mais legivel)
    auto writeVertex = [&](float x, float y, float z) {
        file << x << "," << y << "," << z << std::endl;
        };

    // Faces do cubo
    for (int i = 0; i < divisions; ++i) {
        float y = -half + i * step;
        float yNext = y + step;
        for (int j = 0; j < divisions; ++j) {
            float x = -half + j * step;
            float xNext = x + step;

            // Topo (y = half)
            writeVertex(x, half, y); writeVertex(x, half, yNext); writeVertex(xNext, half, y);
            writeVertex(xNext, half, y); writeVertex(x, half, yNext); writeVertex(xNext, half, yNext);

            // Base (y = -half)
            writeVertex(x, -half, y); writeVertex(xNext, -half, y); writeVertex(x, -half, yNext);
            writeVertex(xNext, -half, y); writeVertex(xNext, -half, yNext); writeVertex(x, -half, yNext);

            // Frente (z = half)
            writeVertex(x, y, half); writeVertex(xNext, y, half); writeVertex(x, yNext, half);
            writeVertex(xNext, y, half); writeVertex(xNext, yNext, half); writeVertex(x, yNext, half);

            // Trás (z = -half)
            writeVertex(x, y, -half); writeVertex(x, yNext, -half); writeVertex(xNext, y, -half);
            writeVertex(xNext, y, -half); writeVertex(x, yNext, -half); writeVertex(xNext, yNext, -half);

            // Direita (x = half)
            writeVertex(half, y, x); writeVertex(half, yNext, x); writeVertex(half, y, xNext);
            writeVertex(half, y, xNext); writeVertex(half, yNext, x); writeVertex(half, yNext, xNext);

            // Esquerda (x = -half)
            writeVertex(-half, y, x); writeVertex(-half, y, xNext); writeVertex(-half, yNext, x);
            writeVertex(-half, y, xNext); writeVertex(-half, yNext, xNext); writeVertex(-half, yNext, x);
        }
    }
    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}

void generatePlane(float size, int divisions, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    float step = size / divisions;
    float half = size / 2.0f;
    int totalVertices = divisions * divisions * 6;  // Cada quadrado é dividido em 2 triângulos, 3 vértices por triângulo

    // Escrevendo o número total de vértices no arquivo
    file << totalVertices << std::endl;

    // Geração de vértices para cada divisão do plano e escrita direta no arquivo
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            float x = (i * step) - half;
            float z = (j * step) - half;

            // Ajustando a ordem dos vértices para que as normais apontem para cima
            // Primeiro triângulo do quadrado
            file << x << ", 0, " << z + step << std::endl;          // Superior esquerdo
            file << x + step << ", 0, " << z << std::endl;          // Inferior direito
            file << x << ", 0, " << z << std::endl;                 // Inferior esquerdo

            // Segundo triângulo do quadrado
            file << x << ", 0, " << z + step << std::endl;          // Superior esquerdo
            file << x + step << ", 0, " << z + step << std::endl;   // Superior direito
            file << x + step << ", 0, " << z << std::endl;          // Inferior direito
        }
    }

    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}


void generateCone(float radius, float height, int slices, int stacks, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    // Cada slice da base gera 3 vértices e cada stack de cada slice gera 6 vértices
    int totalVertices = slices * 3 + slices * stacks * 6;
    file << totalVertices << std::endl;

    // Ângulo entre cada slice
    float deltaAngle = 2 * M_PI / slices;
    // Diferença de altura entre cada stack
    float deltaHeight = height / stacks;

    // Gerar a base do cone corrigida para ser visível de baixo
    for (int i = 0; i < slices; ++i) {
        float angle = i * deltaAngle;
        float nextAngle = (i + 1) * deltaAngle;

        // Inverte a ordem dos vértices da borda para a normal apontar para baixo
        file << "0, 0, 0\n"; // Centro da base
        file << radius * cos(angle) << ", 0, " << radius * sin(angle) << "\n"; // Ponto atual na borda
        file << radius * cos(nextAngle) << ", 0, " << radius * sin(nextAngle) << "\n"; // Próximo ponto na borda
    }

    // Gerar os lados do cone
    for (int i = 0; i < slices; ++i) {
        float angle = i * deltaAngle;
        float nextAngle = (i + 1) * deltaAngle;

        // Topo do cone
        float tipX = 0, tipY = height, tipZ = 0; // Vértice do topo do cone

        // Base do cone
        float baseX1 = radius * cos(angle), baseZ1 = radius * sin(angle); // Ponto atual na borda da base
        float baseX2 = radius * cos(nextAngle), baseZ2 = radius * sin(nextAngle); // Próximo ponto na borda da base

        // Triângulo que liga a base ao topo
        file << tipX << ", " << tipY << ", " << tipZ << "\n"; // Topo
        file << baseX2 << ", 0, " << baseZ2 << "\n"; // Próximo ponto na borda da base
        file << baseX1 << ", 0, " << baseZ1 << "\n"; // Ponto atual na borda da base
    }

    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}

void generateRing(float ir, float er, int slices, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }
    if (slices <= 0)
    {
        std::cerr << "Não pode ter um slices <= 0" << std::endl;
        return;
    }
    // Ângulo entre cada slice
    float deltaAngle = 2 * M_PI / slices;

    // Vertices 2faces * 2 triangulos * 3pontos
    int totalVertices = 2 * 2 * 3 * slices;
    file << totalVertices << std::endl;

    // Função de vértices (para ser mais legivel)
    auto writeVertex = [&](float angle, float radius) {
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        file << x << "," << 0.0 << "," << z << std::endl;
        };

    float pos = 0;
    for (int i = 0; i < slices; i++, pos += deltaAngle) {
        float angle = i * deltaAngle;
        float nextAngle = (i + 1) * deltaAngle;

        writeVertex(angle, ir);
        writeVertex(angle, er);
        writeVertex(nextAngle, ir);

        writeVertex(nextAngle, ir);
        writeVertex(angle, er);
        writeVertex(nextAngle, er);

        writeVertex(nextAngle, ir);
        writeVertex(angle, er);
        writeVertex(angle, ir);

        writeVertex(angle, er);
        writeVertex(nextAngle, ir);
        writeVertex(nextAngle, er);
    }
}

//Bezier//

Ponto formulae(float t, Ponto point1, Ponto point2, Ponto point3, Ponto point4) {
    float aux = 1.0 - t;
    float pt1 = aux * aux * aux;
    float pt2 = 3 * (aux * aux) * t;
    float pt3 = 3 * aux * (t * t);
    float pt4 = t * t * t;

    float x = (pt1 * getX(point1)) + (pt2 * getX(point2)) + (pt3 * getX(point3)) + (pt4 * getX(point4));
    float y = (pt1 * getY(point1)) + (pt2 * getY(point2)) + (pt3 * getY(point3)) + (pt4 * getY(point4));
    float z = (pt1 * getZ(point1)) + (pt2 * getZ(point2)) + (pt3 * getZ(point3)) + (pt4 * getZ(point4));

    return newPonto(x, y, z);
}

Ponto bezier(float u, float v, std::vector<Ponto>& controlPoints, std::vector<int>& patch) {
    Ponto res[4];
    for (int i = 0; i < 4; ++i) {
        res[i] = formulae(u, controlPoints[patch[4 * i]], controlPoints[patch[4 * i + 1]],
            controlPoints[patch[4 * i + 2]], controlPoints[patch[4 * i + 3]]);
    }
    return formulae(v, res[0], res[1], res[2], res[3]);
}

void generateBezierSurface(const std::string& patchFilePath, const std::string& outputFile) {
    std::ifstream patchFile(patchFilePath);
    std::ofstream outFile(outputFile);
    std::string line;
    int numberOfPatches;

    if (!patchFile.is_open() || !outFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    // Ler o número de patches
    std::getline(patchFile, line);
    numberOfPatches = std::stoi(line);

    std::vector<std::vector<int>> patchIndices(numberOfPatches);
    for (int i = 0; i < numberOfPatches; ++i) {
        std::getline(patchFile, line);
        std::stringstream ss(line);
        for (int j = 0; j < 16; ++j) {
            int index;
            ss >> index;
            patchIndices[i].push_back(index);
        }
    }

    // Ler os pontos de comando
    std::getline(patchFile, line);
    int numberOfControlPoints = std::stoi(line);
    std::vector<Ponto> controlPoints(numberOfControlPoints);
    for (int i = 0; i < numberOfControlPoints; ++i) {
        std::getline(patchFile, line);
        std::stringstream ss(line);
        float x, y, z;
        ss >> x >> y >> z;
        controlPoints[i] = newPonto(x, y, z);
    }

    // Configurações de tesselação e geração de pontos de Bezier
    int tessellation = 10;
    float step = 1.0 / tessellation;
    for (auto& patch : patchIndices) {
        for (float u = 0; u < 1; u += step) {
            for (float v = 0; v < 1; v += step) {
                Ponto point = bezier(u, v, controlPoints, patch);
                outFile << getX(point) << " " << getY(point) << " " << getZ(point) << std::endl;
                deletePonto(point);
            }
        }
    }

    // Limpeza e fechamento dos arquivos
    for (Ponto p : controlPoints) {
        deletePonto(p);
    }
    patchFile.close();
    outFile.close();
}
