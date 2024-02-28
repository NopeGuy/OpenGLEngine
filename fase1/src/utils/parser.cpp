#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

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

void printWindowSettings(const WindowSettings& window) {
    std::cout << "Window Settings:" << std::endl;
    std::cout << "Width: " << window.width << std::endl;
    std::cout << "Height: " << window.height << std::endl;
}

void printCameraSettings(const CameraSettings& camera) {
    std::cout << "Camera Settings:" << std::endl;
    std::cout << "Position: (" << camera.positionX << ", " << camera.positionY << ", " << camera.positionZ << ")" << std::endl;
    std::cout << "LookAt: (" << camera.lookAtX << ", " << camera.lookAtY << ", " << camera.lookAtZ << ")" << std::endl;
    std::cout << "Up: (" << camera.upX << ", " << camera.upY << ", " << camera.upZ << ")" << std::endl;
    std::cout << "FOV: " << camera.fov << std::endl;
    std::cout << "Near Plane: " << camera.near << std::endl;
    std::cout << "Far Plane: " << camera.far << std::endl;
}

void printModelFiles(const std::vector<ModelFile>& modelFiles) {
    std::cout << "Model Files:" << std::endl;
    for (const auto& model : modelFiles) {
        std::cout << model.fileName << std::endl;
    }
}

void parseWindowSettings(tinyxml2::XMLElement* windowElement, WindowSettings& window) {
    if (windowElement) {
        windowElement->QueryIntAttribute("width", &window.width);
        windowElement->QueryIntAttribute("height", &window.height);
    } else {
        std::cerr << "Window settings not found." << std::endl;
        exit(1);
    }
}

void parseCameraSettings(tinyxml2::XMLElement* cameraElement, CameraSettings& camera) {
    if (cameraElement) {
        auto positionElement = cameraElement->FirstChildElement("position");
        auto lookAtElement = cameraElement->FirstChildElement("lookAt");
        auto upElement = cameraElement->FirstChildElement("up");
        auto projectionElement = cameraElement->FirstChildElement("projection");

        positionElement->QueryFloatAttribute("x", &camera.positionX);
        positionElement->QueryFloatAttribute("y", &camera.positionY);
        positionElement->QueryFloatAttribute("z", &camera.positionZ);

        lookAtElement->QueryFloatAttribute("x", &camera.lookAtX);
        lookAtElement->QueryFloatAttribute("y", &camera.lookAtY);
        lookAtElement->QueryFloatAttribute("z", &camera.lookAtZ);

        upElement->QueryFloatAttribute("x", &camera.upX);
        upElement->QueryFloatAttribute("y", &camera.upY);
        upElement->QueryFloatAttribute("z", &camera.upZ);

        projectionElement->QueryFloatAttribute("fov", &camera.fov);
        projectionElement->QueryFloatAttribute("near", &camera.near);
        projectionElement->QueryFloatAttribute("far", &camera.far);
    } else {
        std::cerr << "Camera settings not found." << std::endl;
        exit(1);
    }
}

void parseModelFiles(tinyxml2::XMLElement* modelsElement, std::vector<ModelFile>& modelFiles) {
    if (modelsElement) {
        tinyxml2::XMLElement* modelElement = modelsElement->FirstChildElement("model");
        while (modelElement) {
            ModelFile model;
            const char* fileName = modelElement->Attribute("file");
            if (fileName)
                model.fileName = fileName;
            modelFiles.push_back(model);
            modelElement = modelElement->NextSiblingElement("model");
        }
    } else {
        std::cerr << "Model files not found." << std::endl;
        exit(1);
    }
}

bool loadXML(const std::string& filePath, tinyxml2::XMLDocument& doc) {
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file." << std::endl;
        return false;
    }
    return true;
}

int main() {
    tinyxml2::XMLDocument doc;

    // Insert the full path to the XML file
    char filePath[] = "/Example/Path/To/Your/File.xml";
    if (!loadXML(filePath, doc)) {
        return 1;
    }

    WindowSettings window;
    tinyxml2::XMLElement* windowElement = doc.FirstChildElement("world")->FirstChildElement("window");
    parseWindowSettings(windowElement, window);

    CameraSettings camera;
    tinyxml2::XMLElement* cameraElement = doc.FirstChildElement("world")->FirstChildElement("camera");
    parseCameraSettings(cameraElement, camera);

    std::vector<ModelFile> modelFiles;
    tinyxml2::XMLElement* modelsElement = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("models");
    parseModelFiles(modelsElement, modelFiles);

    printWindowSettings(window);
    printCameraSettings(camera);
    printModelFiles(modelFiles);

    return 0;
}