#include "wjsontool.h"


bool WJsonTool::getKeyStrValue(
    ArduinoJson::JsonObject& jsonObject,
    string key,string& retStrVal) 
{
    const char* ptr = jsonObject[key.c_str()].as<char*>() ;
    if( ptr==nullptr ){
        retStrVal = "" ;
        return false ;
    }else{
        retStrVal = string(ptr) ;
        return true ;
    }
}