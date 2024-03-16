#ifndef PARSER_SETTINGS_HPP
#define PARSER_SETTINGS_HPP

#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

struct WindowSettings {
    int width;
    int height;
};

struct Position {
    float x;
    float y;
    float z;
};

struct LookAt {
    float x;
    float y;
    float z;
};

struct Up {
    float x;
    float y;
    float z;
};

struct Projection {
    float fov;
    float near;
    float far;
};

struct CameraSettings {
    Position position;
    LookAt lookAt;
    Up up;
    Projection projection;
};

struct ModelFile {
    std::string fileName;
};

struct ParserSettings {
    WindowSettings window;
    CameraSettings camera;
    std::vector<ModelFile> modelFiles;
};

void parseWindowSettings(tinyxml2::XMLElement* windowElement, WindowSettings& window);
void parseCameraSettings(tinyxml2::XMLElement* cameraElement, CameraSettings& camera);
void parseModelFiles(tinyxml2::XMLElement* modelsElement, std::vector<ModelFile>& modelFiles);
bool loadXML(const std::string& filePath, tinyxml2::XMLDocument& doc);
ParserSettings* ParserSettingsConstructor(const std::string& filePath);

#endif // PARSER_SETTINGS_HPP
