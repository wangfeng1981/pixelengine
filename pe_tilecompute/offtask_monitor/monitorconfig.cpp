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
    
    this->zonalstatcmd = root["zonalstatcmd"].as<char*>() ;
    this->logfile = root["logfile"].as<char*>() ;
    
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
    cout<<"zonalstatcmd:"<<this->zonalstatcmd <<endl ;
    cout<<"logfile:"<<this->logfile <<endl ;
} 