#include "monitorconfig.h"


bool MonitorConfig::loadFromJson( string jsonfilename ) 
{
    ifstream ifs( jsonfilename.c_str() ) ;
    
    DynamicJsonBuffer jsonbuffer ;
    JsonObject& root = jsonbuffer.parseObject(ifs) ;
    
    this->host = root["host"].as<char*>() ;
    this->port = root["port"].as<char*>() ;
    this->db = root["db"].as<char*>() ;
    this->user = root["user"].as<char*>() ;
    this->pwd = root["pwd"].as<char*>() ;
    this->offtaskjsondir = root["offtaskjsondir"].as<char*>() ;
    this->sleep_sec = root["sleep_sec"].as<int>() ;
    this->logfile = root["logfile"].as<char*>() ;
    
    //load modearray
    JsonArray& marr = root["modearray"].as<JsonArray>() ;
    for(size_t im =0 ; im < marr.size() ; ++ im ){
        JsonObject& mo = marr[im].as<JsonObject>() ;
        MonitorMode mm  ;
        mm.mode = mo["mode"].as<int>() ;
        mm.cmdtem = mo["cmdtem"].as<char*>() ;
        this->modearray.push_back(mm) ;
        cout<<"mode:"<<mm.mode<<endl; ;
        cout<<"cmdtem:"<<mm.cmdtem<<endl ;
    }
    
    
    return true ;
}


void MonitorConfig::print( )
{
    cout<<"host:"<<this->host <<endl ;
    cout<<"port:"<<this->port <<endl ;
    cout<<"db:"<<this->db <<endl ;
    cout<<"user:"<<this->user <<endl ;
    cout<<"pwd:"<<this->pwd <<endl ;
    cout<<"offtaskjsondir:"<<this->offtaskjsondir <<endl ;
    cout<<"sleep_sec:"<<this->sleep_sec <<endl ;
    cout<<"logfile:"<<this->logfile <<endl ;
    for(size_t im = 0 ;im < modearray.size();++im){
        cout<<"  mode:"<<modearray[im].mode<<endl; ;
        cout<<"  cmdtem:"<<modearray[im].cmdtem<<endl ;
    }
} 