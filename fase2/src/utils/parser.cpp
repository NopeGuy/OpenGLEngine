#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

struct WindowSettings {
    int width = 0;
    int height = 0;
};

struct CameraSettings {
    float positionX = 0.0f;
    float positionY = 0.0f;
    float positionZ = 0.0f;
    float lookAtX = 0.0f;
    float lookAtY = 0.0f;
    float lookAtZ = 0.0f;
    float upX = 0.0f;
    float upY = 0.0f;
    float upZ = 0.0f;
    float fov = 0.0f;
    float near = 0.0f;
    float far = 0.0f;
};

struct ModelFile {
    std::string fileName;
};

struct TransformSettings {
    float translateX = 0.0f;
    float translateY = 0.0f;
    float translateZ = 0.0f;
};

struct GroupSettings {
    TransformSettings transform;
    std::vector<ModelFile> modelFiles;
    std::vector<GroupSettings> nestedGroups; 
};

struct ParserSettings {
    WindowSettings window;
    CameraSettings camera;
    std::vector<GroupSettings> groups;
};

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
    }
    else {
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

void parseTransformSettings(tinyxml2::XMLElement* transformElement, TransformSettings& transform) {
    if (transformElement) {
        auto translateElement = transformElement->FirstChildElement("translate");
        translateElement->QueryFloatAttribute("x", &transform.translateX);
        translateElement->QueryFloatAttribute("y", &transform.translateY);
        translateElement->QueryFloatAttribute("z", &transform.translateZ);
    } else {
        std::cerr << "Transform settings not found." << std::endl;
        exit(1);
    }
}

void parseGroupSettings(tinyxml2::XMLElement* groupElement, std::vector<GroupSettings>& groups) {
    while (groupElement) {
        GroupSettings group;
        tinyxml2::XMLElement* transformElement = groupElement->FirstChildElement("transform");
        parseTransformSettings(transformElement, group.transform);

        tinyxml2::XMLElement* modelsElement = groupElement->FirstChildElement("models");
        parseModelFiles(modelsElement, group.modelFiles);

        tinyxml2::XMLElement* nestedGroupElement = groupElement->FirstChildElement("group");
        if (nestedGroupElement) {
            parseGroupSettings(nestedGroupElement, group.nestedGroups); 
        }

        groups.push_back(group);
        groupElement = groupElement->NextSiblingElement("group");
    }
}

bool loadXML(const std::string& filePath, tinyxml2::XMLDocument& doc) {
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file." << std::endl;
        return false;
    }
    return true;
}

ParserSettings* ParserSettingsConstructor(const std::string& filePath) {
    ParserSettings* settings = new ParserSettings;

    tinyxml2::XMLDocument doc;
    if (!loadXML(filePath, doc)) {
        return nullptr;
    }

    tinyxml2::XMLElement* worldElement = doc.FirstChildElement("world");
    if (!worldElement) {
        std::cerr << "World element not found." << std::endl;
        exit(1);
    }

    tinyxml2::XMLElement* windowElement = worldElement->FirstChildElement("window");
    parseWindowSettings(windowElement, settings->window);

    tinyxml2::XMLElement* cameraElement = worldElement->FirstChildElement("camera");
    parseCameraSettings(cameraElement, settings->camera);

    tinyxml2::XMLElement* groupElement = worldElement->FirstChildElement("group");
    parseGroupSettings(groupElement, settings->groups);

    return settings;
}

void printGroupSettings(const std::vector<GroupSettings>& groups, int level = 0) {
    for (const auto& group : groups) {
        std::string indent(level * 2, ' '); // Indentation based on the level of the group

        std::cout << indent << "Transform: (" << group.transform.translateX << ", "
                  << group.transform.translateY << ", " << group.transform.translateZ << ")\n";

        for (const auto& model : group.modelFiles) {
            std::cout << indent << "Model File: " << model.fileName << "\n";
        }

        if (!group.nestedGroups.empty()) {
            std::cout << indent << "Nested Groups:\n";
            printGroupSettings(group.nestedGroups, level + 1); // Recursive call for nested groups
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <xml_file_path>" << std::endl;
        return 1;
    }

    const std::string filePath = argv[1];

    // Parse XML file
    ParserSettings* settings = ParserSettingsConstructor(filePath);
    if (!settings) {
        std::cerr << "Failed to parse XML file." << std::endl;
        return 1;
    }

    // Print window settings
    std::cout << "Window Settings:" << std::endl;
    std::cout << "Width: " << settings->window.width << std::endl;
    std::cout << "Height: " << settings->window.height << std::endl;

    // Print camera settings
    std::cout << "\nCamera Settings:" << std::endl;
    std::cout << "Position: (" << settings->camera.positionX << ", "
              << settings->camera.positionY << ", " << settings->camera.positionZ << ")" << std::endl;
    std::cout << "LookAt: (" << settings->camera.lookAtX << ", "
              << settings->camera.lookAtY << ", " << settings->camera.lookAtZ << ")" << std::endl;
    std::cout << "Up: (" << settings->camera.upX << ", "
              << settings->camera.upY << ", " << settings->camera.upZ << ")" << std::endl;
    std::cout << "FOV: " << settings->camera.fov << std::endl;
    std::cout << "Near: " << settings->camera.near << std::endl;
    std::cout << "Far: " << settings->camera.far << std::endl;

    // Print group settings
    std::cout << "\nGroup Settings:" << std::endl;
    printGroupSettings(settings->groups);

    // Clean up
    delete settings;

    return 0;
}
