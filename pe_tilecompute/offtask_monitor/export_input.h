#ifndef EXPORT_INPUT_H
#define EXPORT_INPUT_H
//数据导出任务数据结构

#include <string>
#include <vector>
#include "ajson5.h"
#include <fstream>
#include <iostream>
#include "wmysql.h"
#include "monitorconfig.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "wstringutils.h"

using std::vector ;
using std::cout;
using std::endl;
using std::string ;
using std::ifstream ;
using std::ofstream;
using namespace ArduinoJson;

//从mysql获取的json字符串解析成数据结构
struct ExportContentFromMysql{
    int inpid;
    int64_t dt ;
    string htable, hfami ;
    int hpid ;
    int hpidblen ;
    int yxblen ;
    double left,right,top,bottom ;
    int level ;
    int filldata ;
    string outfilename ;
    string outfilenamedb ;
    string zookeeper ; 
    int datatype;       //1-byte,2-u16,3-i16,4-u32,5-i32,6-f32,7-double

    bool loadFromJson(string text) ;
    ExportContentFromMysql() ;
 
} ;

//用于c++调用命令行时的输入json文件
struct ExportTaskInput{
    string zookeeper, htable ; 
    int hpid ;
    string hfam ;
    int64_t hcol ;
    int level ;
    int datatype ;
    int hpidblen ;
    int yxblen ;
    double left , right , bottom , top ;
    
    inline ExportTaskInput() {}
    ExportTaskInput( ExportContentFromMysql& ex) ;
    bool writeToJsonFile(string outfilename) ;
} ;





#endif



