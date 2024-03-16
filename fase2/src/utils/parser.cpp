#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

struct WindowSettings {
    int width = 0;
    int height = 0;
};

struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct LookAt {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Up {
    float x = 0.0f;
    float y = 1.0f; // Default up direction
    float z = 0.0f;
};

struct Projection {
    float fov = 60.0f; // Default field of view
    float near = 1.0f; // Default near plane
    float far = 1000.0f; // Default far plane
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

struct Transform {
    float translateX = 0.0f;
    float translateY = 0.0f;
    float translateZ = 0.0f;
    float rotateAngle = 0.0f;
    float rotateX = 0.0f;
    float rotateY = 1.0f;
    float rotateZ = 0.0f;
};

struct GroupNode {
    Transform transform;
    std::vector<ModelFile> modelFiles;
    std::vector<GroupNode> children;
};

struct ParserSettings {
    WindowSettings window;
    CameraSettings camera;
    GroupNode rootNode;
};

void parseWindowSettings(tinyxml2::XMLElement* windowElement, WindowSettings& window) {
    if (windowElement) {
        windowElement->QueryIntAttribute("width", &window.width);
        windowElement->QueryIntAttribute("height", &window.height);
    }
    else {
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

        positionElement->QueryFloatAttribute("x", &camera.position.x);
        positionElement->QueryFloatAttribute("y", &camera.position.y);
        positionElement->QueryFloatAttribute("z", &camera.position.z);

        lookAtElement->QueryFloatAttribute("x", &camera.lookAt.x);
        lookAtElement->QueryFloatAttribute("y", &camera.lookAt.y);
        lookAtElement->QueryFloatAttribute("z", &camera.lookAt.z);

        if (upElement) {
            upElement->QueryFloatAttribute("x", &camera.up.x);
            upElement->QueryFloatAttribute("y", &camera.up.y);
            upElement->QueryFloatAttribute("z", &camera.up.z);
        }

        if (projectionElement) {
            projectionElement->QueryFloatAttribute("fov", &camera.projection.fov);
            projectionElement->QueryFloatAttribute("near", &camera.projection.near);
            projectionElement->QueryFloatAttribute("far", &camera.projection.far);
        }
    }
    else {
        std::cerr << "Camera settings not found." << std::endl;
        exit(1);
    }
}

void parseTransform(tinyxml2::XMLElement* transformElement, Transform& transform) {
    if (transformElement) {
        transformElement->QueryFloatAttribute("translateX", &transform.translateX);
        transformElement->QueryFloatAttribute("translateY", &transform.translateY);
        transformElement->QueryFloatAttribute("translateZ", &transform.translateZ);
        transformElement->QueryFloatAttribute("rotateAngle", &transform.rotateAngle);
        transformElement->QueryFloatAttribute("rotateX", &transform.rotateX);
        transformElement->QueryFloatAttribute("rotateY", &transform.rotateY);
        transformElement->QueryFloatAttribute("rotateZ", &transform.rotateZ);
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
    }
    else {
        std::cerr << "Model files not found." << std::endl;
        exit(1);
    }
}

void parseGroupNode(tinyxml2::XMLElement* groupElement, GroupNode& groupNode) {
    if (groupElement) {
        auto transformElement = groupElement->FirstChildElement("transform");
        if (transformElement) {
            parseTransform(transformElement, groupNode.transform);
        }

        auto modelsElement = groupElement->FirstChildElement("models");
        if (modelsElement) {
            parseModelFiles(modelsElement, groupNode.modelFiles);
        }

        auto childElement = groupElement->FirstChildElement("group");
        while (childElement) {
            GroupNode childNode;
            parseGroupNode(childElement, childNode);
            groupNode.children.push_back(childNode);
            childElement = childElement->NextSiblingElement("group");
        }
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
    if (!groupElement) {
        std::cerr << "Group element not found." << std::endl;
        exit(1);
    }

    parseGroupNode(groupElement, settings->rootNode);

    return settings;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
		printf("Usage: %s <path to xml file>\n", argv[0]);
		return 1;
	}
	std::string filePath = argv[1];
	printf("File path: %s\n", filePath.c_str());
	ParserSettings* settings = ParserSettingsConstructor(filePath);
	std::cout << "Window width: " << settings->window.width << std::endl;
	std::cout << "Window height: " << settings->window.height << std::endl;
	std::cout << "Camera position: (" << settings->camera.position.x << ", " << settings->camera.position.y << ", " << settings->camera.position.z << ")" << std::endl;
	std::cout << "Camera lookAt: (" << settings->camera.lookAt.x << ", " << settings->camera.lookAt.y << ", " << settings->camera.lookAt.z << ")" << std::endl;
	std::cout << "Camera up: (" << settings->camera.up.x << ", " << settings->camera.up.y << ", " << settings->camera.up.z << ")" << std::endl;
	std::cout << "Camera projection: fov=" << settings->camera.projection.fov << ", near=" << settings->camera.projection.near << ", far=" << settings->camera.projection.far << std::endl;
	std::cout << "Root node has " << settings->rootNode.modelFiles.size() << " model files." << std::endl;
	std::cout << "Root node has " << settings->rootNode.children.size() << " children." << std::endl;
	return 0;
}       