#ifndef COMPOSITE_INPUT_H
#define COMPOSITE_INPUT_H

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



//{
//    "inpid": 1,
//    "bandindex": 0,
//    "fromdt": 20180100000000,
//    "todt": 20180200000000,
//    "vmin": 0,
//    "vmax": 255,
//    "filldata": 0,
//    "method": "min",
//    "scriptfilename": "/home/hadoop/tempdata/pe-tmpdir//2021/20210406/co-20210406124258.js",
//    "outhtable": "sparkv8out",
//    "outhfami": "tiles",
//    "outpid": 8,
//    "outhpid": 8,
//    "outhpidblen": 4,
//    "outyxblen": 2,
//    "outhcol": 1,
//    "userbound": 0,
//    "left": 0,
//    "right": 0,
//    "top": 0,
//    "bottom": 0,
//    "zmin": 0,
//    "zmax": 12
//}

struct CompositeContentFromMysql{
     int inpid, bandindex;
     int64_t fromdt , todt ;
     double vmin, vmax ;
     double filldata ;
     string method ;
     string scriptfilename , outhtable, outhfami ;
     int outpid , outhpid , outhpidblen , outyxblen;
     int outhcol ;
     int userbound ;//0-not use, 1-use 
     double left , right , top , bottom ;
     int zmin,zmax ;
    
    CompositeContentFromMysql(string text) ;
    inline CompositeContentFromMysql() {} 
 
} ;



//{
//    "oftid":40,
//    "scriptfilename":"/home/hadoop/somedir/temp-test.js",
//    "outhtable":"sparkv8out",
//    "outhfami":"tiles",
//    "outhpid":"7",
//    "outhpidblen":4,
//    "outyxblen":2,
//    "outhcol":1,
//    "usebound":0,
//    "left":0,
//    "right":0,
//    "top":0,
//    "bottom":0,
//    "zmin":0,
//    "zmax":2
//}

struct CompositeSparkInput{
    
     int oftid ;
     string scriptfilename , outhtable, outhfami ;
     int outhpid , outhpidblen, outyxblen ;
     int outhcol , userbound ;
     double left,right,top,bottom;
     int zmin,zmax ;
    
    inline CompositeSparkInput() {}
    CompositeSparkInput(int oftid1 , CompositeContentFromMysql& co) ;
    bool writeToJsonFile(string outfilename) ;
} ;

struct CompositeProductBand{
    int hPid ;
    int bsqIndex;
    string bName ;
    double scale ;
    double offset ;
    double validMin ;
    double validMax ;
    double noData ;
} ;

struct CompositeSparkOutput{
    string proj ;
    int minZoom,maxZoom,dataType;
    int tileWid,tileHei ;
    vector<CompositeProductBand> bandList ;
    
    bool loadFromJson(string jsonfilename) ;
    bool doDbWork(const MonitorConfig& config,CompositeContentFromMysql coParams,int userid) ;
    
    //更新产品信息
    static bool updateProductInDb(const MonitorConfig& config,
        const int pid,string proj,
        int minZoom,int maxZoom,
        int dataType, int timeType,
        string htablename,int tilewid,int tilehei,
        string compress,int styleid,
        int userid) ;
        
    //添加产品波段信息
    static bool insertProductBandInDb(const MonitorConfig& config,
        const int pid,int bindex,int hpid,int bsqindex,
        string bname,double scale,double offset,
        double validmin,double validmax,double nodata) ;
        
    //添加产品期次记录
    static bool insertProductDataItem(const MonitorConfig& config,
        const int pid,int64_t hcol,double left,double right,
        double top,double bottom) ;
};


#endif





