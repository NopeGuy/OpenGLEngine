#ifndef CONFIG
#define CONFIG
#include "../tinyXML/tinyxml.h"
#include "../utils/list.hpp"
#include <stdlib.h>

typedef struct config* Config;

Config newConfig();

Config xmlToConfig(const char*);

List getModels(Config);

void setCamPosition(Config, float, float, float);

float getXPosCam(Config);

float getYPosCam(Config);

float getZPosCam(Config);

float getXLookAt(Config conf);

float getYLookAt(Config conf);

float getZLookAt(Config conf);

float getXUp(Config conf);

float getYUp(Config conf);

float getZUp(Config conf);

void deleteConfig(Config);

#endif // CONFIG