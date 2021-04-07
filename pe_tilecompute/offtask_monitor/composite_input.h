#ifndef COMPOSITE_INPUT_H
#define COMPOSITE_INPUT_H

#include <string>
#include "ajson5.h"
#include <fstream>
#include <iostream>

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
    
    CompositeSparkInput(int oftid1 , CompositeContentFromMysql& co) ;
    bool writeToJsonFile(string outfilename) ;
} ;


#endif





