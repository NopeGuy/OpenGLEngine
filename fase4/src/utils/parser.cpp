#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

struct Window {
    int width = 0;
    int height = 0;
};

struct Point {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Camera {
    Point position;
    Point lookAt;
    Point up;
    Point projection;
};

struct Transform {
    char type;
    Point point;
    float angle = 0.0f;
    float time = 0.0f;
    bool align;
    std::vector<Point> points;
};

struct Model {
    std::string fileName;
};

struct Group {
    std::vector<Transform> transforms;
    std::vector<Model> modelFiles;
    std::vector<Group> children;
};

struct Parser {
    Window window;
    Camera camera;
    Group rootNode;
};

void parseWindowSettings(tinyxml2::XMLElement* windowElement, Window& window) {
    if (windowElement) {
        windowElement->QueryIntAttribute("width", &window.width);
        windowElement->QueryIntAttribute("height", &window.height);
    }
    else {
        std::cerr << "Window settings not found." << std::endl;
        exit(1);
    }
}

void parseCameraSettings(tinyxml2::XMLElement* cameraElement, Camera& camera) {
    if (cameraElement) {
        auto positionElement = cameraElement->FirstChildElement("position");
        auto lookAtElement = cameraElement->FirstChildElement("lookAt");
        auto upElement = cameraElement->FirstChildElement("up");
        auto projectionElement = cameraElement->FirstChildElement("projection");

        if (positionElement && lookAtElement) {
            positionElement->QueryFloatAttribute("x", &camera.position.x);
            positionElement->QueryFloatAttribute("y", &camera.position.y);
            positionElement->QueryFloatAttribute("z", &camera.position.z);

            lookAtElement->QueryFloatAttribute("x", &camera.lookAt.x);
            lookAtElement->QueryFloatAttribute("y", &camera.lookAt.y);
            lookAtElement->QueryFloatAttribute("z", &camera.lookAt.z);
        }

        if (upElement) {
            upElement->QueryFloatAttribute("x", &camera.up.x);
            upElement->QueryFloatAttribute("y", &camera.up.y);
            upElement->QueryFloatAttribute("z", &camera.up.z);
        }

        if (projectionElement) {
            projectionElement->QueryFloatAttribute("fov", &camera.projection.x);
            projectionElement->QueryFloatAttribute("near", &camera.projection.y);
            projectionElement->QueryFloatAttribute("far", &camera.projection.z);
        }
    }
    else {
        std::cerr << "Camera settings not found." << std::endl;
        exit(1);
    }
}

void parseTransform(tinyxml2::XMLElement* transformElement, std::vector<Transform>& transforms) {
    if (!transformElement)
        return;

    for (tinyxml2::XMLElement* t = transformElement->FirstChildElement(); t; t = t->NextSiblingElement()) {
        Transform transform;
        transform.type = t->Value()[0];

        const char* timeAttr = t->Attribute("time");
        transform.time = timeAttr ? atof(timeAttr) : 0.0f;

        switch (transform.type) {
        case 't':
            transform.align = t->BoolAttribute("align", false);
            for (tinyxml2::XMLElement* pointElement = t->FirstChildElement("point"); pointElement; pointElement = pointElement->NextSiblingElement("point")) {
                Point point;
                pointElement->QueryFloatAttribute("x", &point.x);
                pointElement->QueryFloatAttribute("y", &point.y);
                pointElement->QueryFloatAttribute("z", &point.z);
                transform.points.push_back(point);
            }
            if (transform.time == 0.0f) {
                transform.point.x = atof(t->Attribute("x"));
                transform.point.y = atof(t->Attribute("y"));
                transform.point.z = atof(t->Attribute("z"));
            }
            break;

        case 'r':
            transform.angle = atof(t->Attribute("angle"));
            transform.point.x = atof(t->Attribute("x"));
            transform.point.y = atof(t->Attribute("y"));
            transform.point.z = atof(t->Attribute("z"));
            break;

        case 's':
            transform.point.x = atof(t->Attribute("x"));
            transform.point.y = atof(t->Attribute("y"));
            transform.point.z = atof(t->Attribute("z"));
            break;

        default:
            break;
        }

        transforms.push_back(transform);
    }
}

void parseModelFiles(tinyxml2::XMLElement* modelsElement, std::vector<Model>& modelFiles) {
    if (!modelsElement)
        return;

    for (tinyxml2::XMLElement* modelElement = modelsElement->FirstChildElement("model"); modelElement; modelElement = modelElement->NextSiblingElement("model")) {
        Model model;
        const char* fileName = modelElement->Attribute("file");
        if (fileName)
            model.fileName = fileName;
        modelFiles.push_back(std::move(model)); // Move semantics for efficient copying
    }
}

void parseGroupNode(tinyxml2::XMLElement* groupElement, Group& groupNode) {
    if (!groupElement)
        return;

    auto transformElement = groupElement->FirstChildElement("transform");
    if (transformElement)
        parseTransform(transformElement, groupNode.transforms);

    auto modelsElement = groupElement->FirstChildElement("models");
    if (modelsElement)
        parseModelFiles(modelsElement, groupNode.modelFiles);

    for (tinyxml2::XMLElement* childElement = groupElement->FirstChildElement("group"); childElement; childElement = childElement->NextSiblingElement("group")) {
        Group childNode;
        parseGroupNode(childElement, childNode);
        groupNode.children.push_back(std::move(childNode)); // Move semantics for efficient copying
    }
}



bool loadXML(const std::string& filePath, tinyxml2::XMLDocument& doc) {
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file." << std::endl;
        return false;
    }
    return true;
}

Parser* ParserSettingsConstructor(const std::string& filePath) {
    Parser* settings = new Parser;

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

    // print all models in settings
    for (const auto& model : settings->rootNode.modelFiles) {
		std::cout << "File Name: " << model.fileName << std::endl;
	}
    return settings;
}


void printGroup(const Group& group, int depth = 0) {
    for (const auto& transform : group.transforms) {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
        std::cout << "Type: " << transform.type << std::endl;
        if (transform.type == 't') {
            if (transform.time != 0.0f) {
                for (int i = 0; i < depth; ++i)
                    std::cout << "  ";
                std::cout << "Time: " << transform.time << std::endl;
                for (int i = 0; i < depth; ++i)
                    std::cout << "  ";
                std::cout << "Align: " << (transform.align ? "true" : "false") << std::endl;
                for (const auto& point : transform.points) {
                    for (int i = 0; i < depth; ++i)
                        std::cout << "  ";
                    std::cout << "Translation Point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
                }
            }
            else {
                for (int i = 0; i < depth; ++i)
                    std::cout << "  ";
                std::cout << "Translation: (" << transform.point.x<< ", " << transform.point.y << ", " << transform.point.z << ")" << std::endl;
            }
        }
        else if (transform.type == 'r') {
            if (transform.time != 0.0f) {
                for (int i = 0; i < depth; ++i)
                    std::cout << "  ";
                std::cout << "Time: " << transform.time << std::endl;
            }
            else {
                for (int i = 0; i < depth; ++i)
                    std::cout << "  ";
                std::cout << "Angle: " << transform.angle << std::endl;
            }
            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << "Rotation: (" << transform.point.x << ", " << transform.point.y << ", " << transform.point.z << ")" << std::endl;
        }
        else if (transform.type == 's') {
            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << "Scale: (" << transform.point.x << ", " << transform.point.y << ", " << transform.point.z << ")" << std::endl;
        }
        else {
            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << "Unknown transform type: " << transform.type << std::endl;
        }
    }

    for (const auto& model : group.modelFiles) {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
        std::cout << "File Name: " << model.fileName << std::endl;
    }

    for (const auto& child : group.children) {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
        std::cout << "Child Group:" << std::endl;
        printGroup(child, depth + 1);
    }
}

void print(const Parser& parser) {
    std::cout << "Window settings:" << std::endl;
    std::cout << "Width: " << parser.window.width << std::endl;
    std::cout << "Height: " << parser.window.height << std::endl;

    std::cout << "\nCamera settings:" << std::endl;
    std::cout << "Position: (" << parser.camera.position.x << ", " << parser.camera.position.y << ", " << parser.camera.position.z << ")" << std::endl;
    std::cout << "LookAt: (" << parser.camera.lookAt.x << ", " << parser.camera.lookAt.y << ", " << parser.camera.lookAt.z << ")" << std::endl;
    std::cout << "Up: (" << parser.camera.up.x << ", " << parser.camera.up.y << ", " << parser.camera.up.z << ")" << std::endl;
    std::cout << "Projection:" << std::endl;
    std::cout << "FOV: " << parser.camera.projection.x << std::endl;
    std::cout << "Near: " << parser.camera.projection.y << std::endl;
    std::cout << "Far: " << parser.camera.projection.z << std::endl;

    std::cout << "\nTransforms:" << std::endl;
    printGroup(parser.rootNode);

    std::cout << std::endl;
}