#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "tinyxml/tinyxml2.h"

struct WindowSettings {
    int width, height;
};

struct CameraSettings {
    float positionX, positionY, positionZ;
    float lookAtX, lookAtY, lookAtZ;
    float upX, upY, upZ;
    float fov, near, far;
};

struct ModelFile {
    std::string fileName;
};

void parseWindowSettings(tinyxml2::XMLElement* windowElement, WindowSettings& window);
void parseCameraSettings(tinyxml2::XMLElement* cameraElement, CameraSettings& camera);
void parseModelFiles(tinyxml2::XMLElement* modelsElement, std::vector<ModelFile>& modelFiles);
bool loadXML(const std::string& filePath, tinyxml2::XMLDocument& doc);

#endif // PARSER_HPP
