#include "export_input.h"


bool ExportContentFromMysql::loadFromJson(string text) 
{
    DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(text);
	bool parseOk = root.success() ; 
    if( parseOk==false ){
        return false ;
    }
    this->inpid =   root["inpid"].as<int>() ;
    this->dt =      root["dt"].as<int64_t>() ;
    this->htable =  root["htable"].as<char*>() ;
    this->hfami =   root["hfami"].as<char*>() ;
    
    this->hpid =    root["hpid"].as<int>() ;
    this->hpidblen= root["hpidblen"].as<int>() ;
    this->yxblen =  root["yxblen"].as<int>() ;
    this->left =    root["left"].as<double>() ;
    
    this->right =   root["right"].as<double>() ;
    this->top =     root["top"].as<double>() ;
    this->bottom =  root["bottom"].as<double>() ;
    this->level =   root["level"].as<int>() ;
    
    this->filldata =        root["filldata"].as<int>() ;
    this->outfilename =     root["outfilename"].as<char*>() ;
    this->outfilenamedb =   root["outfilenamedb"].as<char*>() ;
    
    this->zookeeper =       root["zookeeper"].as<char*>() ;
    this->datatype =        root["datatype"].as<int>() ;
    return true ;
}

ExportContentFromMysql::ExportContentFromMysql() 
{
    this->inpid = 0 ;
    this->bottom = 0 ;
    this->dt = 0 ;
    this->filldata = 0 ;
    this->hpid = 0 ;
    this->hpidblen = 0 ;
    this->left = 0 ;
    this->level = 0 ;
    this->right = 0 ;
    this->top = 0 ;
    this->yxblen = 0 ;
    this->datatype = 0 ;
}


////////////////////////////////////////////

ExportTaskInput::ExportTaskInput( ExportContentFromMysql& ex) 
{
    zookeeper = ex.zookeeper ;
    htable = ex.htable ;
    hpid = ex.hpid ;
    hfam = ex.hfami ;
    hcol = ex.dt ;
    level = ex.level ;
    datatype = ex.datatype ;
    hpidblen = ex.hpidblen ;
    yxblen = ex.yxblen ;
    left = ex.left ;
    right = ex.right;
    top = ex.top ;
    bottom = ex.bottom ;
}
bool ExportTaskInput::writeToJsonFile(string outfilename) 
{
    DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
    root["zookeeper"] = zookeeper ;
    root["htable"] = htable ;
    root["hpid"] = hpid ;
    root["hfam"] = hfam ;
    root["hcol"] = hcol ;
    root["level"] = level ;
    root["datatype"] = datatype ;
    root["hpidblen"] = hpidblen ;
    root["yxblen"] = yxblen ;
    root["left"] = left ;
    root["right"] = right ;
    root["top"] = top ;
    root["bottom"] = bottom ;
    
    string outtext ;
    root.printTo(outtext);
    std::ofstream ofs(outfilename.c_str());
    if( ofs.good() == true )
    {
        ofs<<outtext ;
        ofs.close() ;
        return true ;
    }else{
        return false ;
    }
    
}