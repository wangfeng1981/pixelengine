
#ifndef W_JSON_TOOL_H
#define W_JSON_TOOL_H

#include "ajson5.h"
#include <string>

using std::string;

struct WJsonTool{
   
    static bool getKeyStrValue(ArduinoJson::JsonObject& jsonObject,string key,string& retStrVal) ;
    
} ;



#endif