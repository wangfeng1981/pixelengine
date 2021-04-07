//监控程序配置参数json解析

#ifndef MONITOR_CONFIG_H
#define MONITOR_CONFIG_H

#include <string>
#include "ajson5.h"
#include <fstream>
#include <iostream>
#include <vector>

using std::vector ;
using std::cout;
using std::endl;
using std::string ;
using std::ifstream ;
using namespace ArduinoJson;

struct MonitorMode{
   int mode ;//0-区域统计(zs) , 1-实况序列分析(sk) , 2-历史序列分析(ls) , 4-数据合成(co) 
   string cmdtem ;//命令模板
} ;

struct MonitorConfig{
    string host, port, db, user, pwd ;
    string offtaskjsondir ;
    vector<MonitorMode> modearray ;
    string logfile ;
    int sleep_sec ;
    bool loadFromJson( string jsonfilename ) ;
    void print() ;
    
} ;



#endif 