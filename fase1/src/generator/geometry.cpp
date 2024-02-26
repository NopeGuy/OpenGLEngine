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

namespace fs = std::filesystem;

void generateSphere(float radius, int slices, int stacks, const std::string& filename) {
    fs::path outputPath = fs::current_path().parent_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita: " << filePath << std::endl;
        return;
    }

    // ângulo de cada fatia e pilha
    float deltaPhi = M_PI / stacks;
    float deltaTheta = 2 * M_PI / slices;

    for (int i = 0; i <= stacks; ++i) {
        float phi = i * deltaPhi;
        for (int j = 0; j < slices; ++j) {
            float theta = j * deltaTheta;

            // Coordenadas esféricas para coordenadas cartesianas
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * sinf(phi) * sinf(theta);
            float z = radius * cosf(phi);

            // Escreve as coordenadas no arquivo
            file << x << " " << y << " " << z << std::endl;
        }
    }

    file.close();
    std::cout << "Arquivo '" << filePath << "' criado com sucesso." << std::endl;
}


void generateBox(float size, int divisions, const std::string& filename){
    fs::path outputPath = fs::current_path().parent_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    float step = size / divisions; // Distância entre cada divisão
    float half = size / 2; // Metade do tamanho do cubo para centralizá-lo na origem

    // Itera por cada face do cubo
    for (int i = 0; i < 6; i++) {
        // Determina a orientação da face
        float nx = 0, ny = 0, nz = 0;
        switch (i) {
            case 0: ny = 1; break; // Topo
            case 1: ny = -1; break; // Base
            case 2: nz = 1; break; // Frente
            case 3: nz = -1; break; // Atrás
            case 4: nx = 1; break; // Direita
            case 5: nx = -1; break; // Esquerda
        }

        // Itera por cada divisão da face
        for (int y = 0; y < divisions; y++) {
            for (int x = 0; x < divisions; x++) {
                // Calcula as coordenadas dos quatro vértices do quadrado atual
                for (int corner = 0; corner < 4; corner++) {
                    float dx = (corner == 0 || corner == 3) ? 0 : step;
                    float dy = (corner < 2) ? 0 : step;
                    float vx = nx * half + (nx == 0 ? (x * step - half + dx) : 0);
                    float vy = ny * half + (ny == 0 ? (y * step - half + dy) : 0);
                    float vz = nz * half + (nz == 0 ? (x * step - half + dx) : 0);

                    // Escreve as coordenadas do vértice no arquivo
                    file << "v " << vx << " " << vy << " " << vz << std::endl;
                }
            }
        }
    }

    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}

void generatePlane(float size, int divisions, const std::string& filename){
    fs::path outputPath = fs::current_path().parent_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    float step = size / divisions; // Distância entre cada divisão
    float half = size / 2;         // Metade do tamanho do plano para centralizá-lo na origem

    // Geração de vértices para cada divisão do plano
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            // Coordenadas do vértice inferior esquerdo de um quadrado do grid
            float x = (i * step) - half;
            float z = (j * step) - half;

            // Primeiro triângulo do quadrado
            file << x << " 0 " << z << "\n";                  // Inferior esquerdo
            file << x + step << " 0 " << z << "\n";          // Inferior direito
            file << x << " 0 " << z + step << "\n";          // Superior esquerdo

            // Segundo triângulo do quadrado
            file << x + step << " 0 " << z << "\n";          // Inferior direito
            file << x + step << " 0 " << z + step << "\n";   // Superior direito
            file << x << " 0 " << z + step << "\n";          // Superior esquerdo
        }
    }

    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}


void generateCone(float radius, float height, int slices, int stacks, const std::string& filename){
    fs::path outputPath = fs::current_path().parent_path().parent_path() / "output";
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    // Ângulo entre cada slice
    float deltaAngle = 2 * M_PI / slices;
    // Diferença de altura entre cada stack
    float deltaHeight = height / stacks;

    // Gerar a base do cone
    for (int i = 0; i < slices; ++i) {
        float angle = i * deltaAngle;
        float nextAngle = (i + 1) * deltaAngle;

        // Vértices da base (0, 0, 0) e dois pontos na borda
        file << "0 0 0\n"; // Centro da base
        file << radius * cos(nextAngle) << " " << radius * sin(nextAngle) << " 0\n"; // Próximo ponto na borda
        file << radius * cos(angle) << " " << radius * sin(angle) << " 0\n"; // Ponto atual na borda
    }

    // Gerar os lados do cone
    for (int i = 0; i < slices; ++i) {
        for (int j = 0; j < stacks; ++j) {
            float angle = i * deltaAngle;
            float nextAngle = (i + 1) * deltaAngle;
            
            // Calcula os raios e alturas para os vértices
            float lowerRadius = radius * (stacks - j) / stacks;
            float upperRadius = radius * (stacks - j - 1) / stacks;
            float lowerHeight = j * deltaHeight;
            float upperHeight = (j + 1) * deltaHeight;

            // Quatro vértices do trapézio (dois inferiores, dois superiores)
            file << lowerRadius * cos(angle) << " " << lowerRadius * sin(angle) << " " << lowerHeight << "\n"; // Inferior atual
            file << upperRadius * cos(angle) << " " << upperRadius * sin(angle) << " " << upperHeight << "\n"; // Superior atual
            file << upperRadius * cos(nextAngle) << " " << upperRadius * sin(nextAngle) << " " << upperHeight << "\n"; // Superior próximo

            file << upperRadius * cos(nextAngle) << " " << upperRadius * sin(nextAngle) << " " << upperHeight << "\n"; // Superior próximo
            file << lowerRadius * cos(nextAngle) << " " << lowerRadius * sin(nextAngle) << " " << lowerHeight << "\n"; // Inferior próximo
            file << lowerRadius * cos(angle) << " " << lowerRadius * sin(angle) << " " << lowerHeight << "\n"; // Inferior atual
        }
    }

    file.close();
    std::cout << "Arquivo '" << filename << "' criado com sucesso." << std::endl;
}