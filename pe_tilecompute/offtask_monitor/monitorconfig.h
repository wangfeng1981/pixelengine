//监控程序配置参数json解析

#ifndef MONITOR_CONFIG_H
#define MONITOR_CONFIG_H

#include <string>
#include "ajson5.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::string ;
using std::ifstream ;
using namespace ArduinoJson;

struct MonitorConfig{
    string host, port, db, user, pwd ;
    string offtaskjsondir ;
    string zonalstatcmd ;
    string logfile ;
    int sleep_sec ;
    bool loadFromJson( string jsonfilename ) ;
    void print() ;
    
} ;



#endif 