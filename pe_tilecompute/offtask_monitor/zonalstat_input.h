#ifndef ZONAL_STAT_INPUT_H
#define ZONAL_STAT_INPUT_H

#include <string>
#include "ajson5.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::string ;
using std::ifstream ;
using namespace ArduinoJson;

struct ZSContentFromMysql{
    double vmin ;
    double vmax ;
    int64_t fromdt ;
    int64_t todt ;
    string regfile ;
    int zlevel ;
    string hTableName;
    int hPid ;
    int bsqIndex ;
    int dataType ;
    string hFamily ;
    int hpidblen ;
    int yxblen ;
    string method ;//min,max,ave
    string offsetdt ;
    double bandValidMin ;
    double bandValidMax ;
    double bandNodata ;
    string outfilename ;
    string outfilenamedb ;
    
    inline ZSContentFromMysql():vmin(0),vmax(0),fromdt(0),todt(0),zlevel(0),hPid(0),bsqIndex(0),dataType(0),hpidblen(0),yxblen(0),bandValidMax(0),bandValidMin(0),bandNodata(0){}
    
    bool fromJsonText(string text) ;
    string int2str(int val) ;
    string double2str(double val) ;
    string long2str(int64_t val) ;
    
} ;


struct ZSSparkInput{
    string mode ;//sk,ls
    string htable;
    string hpid ;
    string hfami ;
    string hpidblen ;
    string yxblen ;
    string zlevel ;
    string regionfile ;
    string fromdt ;
    string todt ;
    string validMin ;
    string validMax ;
    string filldata ;
    string iband ;
    string dataType ;
    //ls
    string offsetdt ;
    string method ;//min,max,ave
    string combMin ;
    string combMax ;
    
    //imode =0 区域统计，1实况序列分析，2历史序列分析
    //imode=0,1 使用sk分析，=2使用ls分析
    void fromZSContentFromMysql(  ZSContentFromMysql& zsc , int imode) ;
    bool writeToJsonFile(string outfilename) ;
    
} ;










#endif