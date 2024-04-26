#include <iostream>
#include <vector>
#include <string>
#include "../tinyXML/tinyxml2.h"

struct Window {
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
    float y = 1.0f; 
    float z = 0.0f;
};

struct Projection {
    float fov = 60.0f; 
    float near = 1.0f; 
    float far = 1000.0f; 
};

struct Camera {
    Position position;
    LookAt lookAt;
    Up up;
    Projection projection;
};

struct Light{
    std::string type;
    float cutoff = 0.0f;
    Position position;
    Position direction;
};

struct Transform {
    char type;
    float x;
    float y;
    float z;
    float angle;
    float time;
    bool align;
    std::vector<Position> points;
};

struct ModelFile {
    std::string fileName;
};


struct Color {
    std::string type;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float shininess = 0.0f;
};

struct Texture {
    std::string file;
};

struct Model{
    ModelFile modelFile;
    Texture texture;
    std::vector<Color> color;
};

struct Group {
    std::vector<Transform> transforms;
    std::vector<Model> models;
    std::vector<Group> children;
};

struct Parser {
    Window window;
    Camera camera;
    //std::vector<Light> lights;
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

void parseLights(tinyxml2::XMLElement* lightsElement, std::vector<Light>& lights) {
    if (lightsElement) {
        auto lightElement = lightsElement->FirstChildElement("light");
        while (lightElement) {
            Light light;
            const char* type = lightElement->Attribute("type");
            if (type) {
                std::string typeStr = type;
                if (typeStr == "point") {
                    light.type = typeStr;
                    light.position.x = lightElement->FloatAttribute("posx");
                    light.position.y = lightElement->FloatAttribute("posy");
                    light.position.z = lightElement->FloatAttribute("posz");
                } else if (typeStr == "directional") {
                    light.type = typeStr;
                    light.direction.x = lightElement->FloatAttribute("dirx");
                    light.direction.y = lightElement->FloatAttribute("diry");
                    light.direction.z = lightElement->FloatAttribute("dirz");
                } else if (typeStr == "spot") {
                    light.type = typeStr;
                    light.position.x = lightElement->FloatAttribute("posx");
                    light.position.y = lightElement->FloatAttribute("posy");
                    light.position.z = lightElement->FloatAttribute("posz");
                    light.direction.x = lightElement->FloatAttribute("dirx");
                    light.direction.y = lightElement->FloatAttribute("diry");
                    light.direction.z = lightElement->FloatAttribute("dirz");
                    light.cutoff = lightElement->FloatAttribute("cutoff");
                } else {
                    // Handle unrecognized type
                }
            }
            lights.push_back(light);
            lightElement = lightElement->NextSiblingElement("light");
        }
    }
}



void parseTransform(tinyxml2::XMLElement* transformElement, std::vector<Transform>& transforms) {
    if (transformElement) {
        for (tinyxml2::XMLElement* t = transformElement->FirstChildElement(); t; t = t->NextSiblingElement()) {
            Transform transform;
            transform.type = t->Value()[0];
            if (transform.type == 't') {
                const char* timeAttr = t->Attribute("time");
                if (timeAttr) {
                    transform.time = atof(timeAttr);
                    transform.align = t->BoolAttribute("align", false);
                    tinyxml2::XMLElement* pointElement = t->FirstChildElement("point");
                    while (pointElement) {
                        Position point;
                        pointElement->QueryFloatAttribute("x", &point.x);
                        pointElement->QueryFloatAttribute("y", &point.y);
                        pointElement->QueryFloatAttribute("z", &point.z);
                        transform.points.push_back(point);
                        pointElement = pointElement->NextSiblingElement("point");
                    }
                }
                else {
                    transform.time = 0.0f;
                    transform.x = atof(t->Attribute("x"));
                    transform.y = atof(t->Attribute("y"));
                    transform.z = atof(t->Attribute("z"));
                }
            }
            else if (transform.type == 'r') {
                const char* timeAttr = t->Attribute("time");
                if (timeAttr) {
                    transform.time = atof(timeAttr);
                    transform.x = atof(t->Attribute("x"));
                    transform.y = atof(t->Attribute("y"));
                    transform.z = atof(t->Attribute("z"));
                }
                else {
                    transform.time = 0.0f;
                    transform.angle = atof(t->Attribute("angle"));
                    transform.x = atof(t->Attribute("x"));
                    transform.y = atof(t->Attribute("y"));
                    transform.z = atof(t->Attribute("z"));
                }
            }
            else if (transform.type == 's') {
                transform.time = 0.0f;
                transform.x = atof(t->Attribute("x"));
                transform.y = atof(t->Attribute("y"));
                transform.z = atof(t->Attribute("z"));
            }
            else {
                transform.angle = 0.0f;
            }
            transforms.push_back(transform);
        }
    }
}

void parseModelFiles(tinyxml2::XMLElement* modelsElement, std::vector<Model>& modelVector) {
    if (modelsElement) {
        tinyxml2::XMLElement* modelElement = modelsElement->FirstChildElement("model");
        while (modelElement) {
            Model model;
            const char* fileName = modelElement->Attribute("file");
            if (fileName)
                model.modelFile.fileName = fileName;
            
            tinyxml2::XMLElement* textureElement = modelElement->FirstChildElement("texture");
            while (textureElement) {
                const char* textureFile = textureElement->Attribute("file");
                if (textureFile)
                    model.texture.file = textureFile;
                textureElement = textureElement->NextSiblingElement("texture");
            }
            
            tinyxml2::XMLElement* colorElement = modelElement->FirstChildElement("color");
            if (colorElement) {
                tinyxml2::XMLElement* diffuseElement = colorElement->FirstChildElement("diffuse");
                if (diffuseElement) {
                    Color diffuseColor;
                    diffuseColor.type = "diffuse";
                    diffuseElement->QueryFloatAttribute("R", &diffuseColor.r);
                    diffuseElement->QueryFloatAttribute("G", &diffuseColor.g);
                    diffuseElement->QueryFloatAttribute("B", &diffuseColor.b);
                    model.color.push_back(diffuseColor);
                }

                tinyxml2::XMLElement* ambientElement = colorElement->FirstChildElement("ambient");
                if (ambientElement) {
                    Color ambientColor;
                    ambientColor.type = "ambient";
                    ambientElement->QueryFloatAttribute("R", &ambientColor.r);
                    ambientElement->QueryFloatAttribute("G", &ambientColor.g);
                    ambientElement->QueryFloatAttribute("B", &ambientColor.b);
                    model.color.push_back(ambientColor);
                }

                tinyxml2::XMLElement* specularElement = colorElement->FirstChildElement("specular");
                if (specularElement) {
                    Color specularColor;
                    specularColor.type = "specular";
                    specularElement->QueryFloatAttribute("R", &specularColor.r);
                    specularElement->QueryFloatAttribute("G", &specularColor.g);
                    specularElement->QueryFloatAttribute("B", &specularColor.b);
                    model.color.push_back(specularColor);
                }

                tinyxml2::XMLElement* emissiveElement = colorElement->FirstChildElement("emissive");
                if (emissiveElement) {
                    Color emissive;
                    emissive.type = "emissive";
                    emissiveElement->QueryFloatAttribute("R", &emissive.r);
                    emissiveElement->QueryFloatAttribute("G", &emissive.g);
                    emissiveElement->QueryFloatAttribute("B", &emissive.b);
                    model.color.push_back(emissive);
                }

                tinyxml2::XMLElement* shininessElement = colorElement->FirstChildElement("shininess");
                if (shininessElement) {
                    Color shininess;
                    shininess.type = "shininess";
                    shininessElement->QueryFloatAttribute("value", &shininess.shininess);
                    model.color.push_back(shininess);
                }
            }

            modelVector.push_back(model);
            modelElement = modelElement->NextSiblingElement("model");
        }
    }
}

void parseGroupNode(tinyxml2::XMLElement* groupElement, Group& groupNode) {
    if (groupElement) {
        auto transformElement = groupElement->FirstChildElement("transform");
        if (transformElement) {
            parseTransform(transformElement, groupNode.transforms);
        }

        auto modelsElement = groupElement->FirstChildElement("models");
        if (modelsElement) {
            parseModelFiles(modelsElement, groupNode.models);
        }

        auto childElement = groupElement->FirstChildElement("group");
        while (childElement) {
            Group childNode;
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

    tinyxml2::XMLElement* lightElement = worldElement->FirstChildElement("lights");
    //parseLights(lightElement, settings->lights);

    tinyxml2::XMLElement* groupElement = worldElement->FirstChildElement("group");
    if (!groupElement) {
        std::cerr << "Group element not found." << std::endl;
        exit(1);
    }

    parseGroupNode(groupElement, settings->rootNode);
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
                std::cout << "Translation: (" << transform.x << ", " << transform.y << ", " << transform.z << ")" << std::endl;
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
            std::cout << "Rotation: (" << transform.x << ", " << transform.y << ", " << transform.z << ")" << std::endl;
        }
        else if (transform.type == 's') {
            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << "Scale: (" << transform.x << ", " << transform.y << ", " << transform.z << ")" << std::endl;
        }
        else {
            for (int i = 0; i < depth; ++i)
                std::cout << "  ";
            std::cout << "Unknown transform type: " << transform.type << std::endl;
        }
    }

    for (const auto& model : group.models) {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
        std::cout << "File Name: " << model.modelFile.fileName << std::endl;
        if(!model.texture.file.empty())
            std::cout << "Texture File: " << model.texture.file << std::endl;
        std::cout << "\n";
        
        if(model.color.size() > 0)
            std::cout << "Color:\n";
        std::cout << "\n";
        bool hasColor = false;
        for (const auto& color : model.color) {
            std::cout << color.type << ", " << color.r << ", " << color.g << ", " << color.b << std::endl;
        }
        std::cout << "\n";

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
    std::cout << "FOV: " << parser.camera.projection.fov << std::endl;
    std::cout << "Near: " << parser.camera.projection.near << std::endl;
    std::cout << "Far: " << parser.camera.projection.far << std::endl;

    /*
    for (const auto& light : parser.lights.lights) {
        if(!parser.lights.lights.empty()){
        std::cout << "\nLights:" << std::endl;
        std::cout << "Type: " << light.type << std::endl;
        std::string typeStr = light.type;
        if (typeStr == "point") {
            std::cout << "Position: (" << light.pointLight.position.x << ", " << light.pointLight.position.y << ", " << light.pointLight.position.z << ")" << std::endl;
        } else if (typeStr == "directional") {
            std::cout << "Direction: (" << light.directionalLight.direction.x << ", " << light.directionalLight.direction.y << ", " << light.directionalLight.direction.z << ")" << std::endl;
        } else if (typeStr == "spot") {
            std::cout << "Position: (" << light.spotLight.position.x << ", " << light.spotLight.position.y << ", " << light.spotLight.position.z << ")" << std::endl;
            std::cout << "Direction: (" << light.spotLight.direction.x << ", " << light.spotLight.direction.y << ", " << light.spotLight.direction.z << ")" << std::endl;
            std::cout << "Cutoff: " << light.spotLight.cutoff << std::endl;
        }
    }
    }
    */
    std::cout << "\nTransforms:" << std::endl;
    printGroup(parser.rootNode);

    std::cout << std::endl;
}
