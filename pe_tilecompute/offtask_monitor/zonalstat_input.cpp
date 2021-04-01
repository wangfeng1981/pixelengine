#include "zonalstat_input.h"


bool ZSContentFromMysql::fromJsonText(string text) 
{
	ArduinoJson::DynamicJsonBuffer jsonBuffer;
	ArduinoJson::JsonObject& root = jsonBuffer.parseObject(text);
    
    {
        this->bandNodata = root["bandNodata"].as<double>() ;
        this->bandValidMax = root["bandValidMax"].as<double>() ;
        this->bandValidMin = root["bandValidMin"].as<double>() ;
        this->bsqIndex = root["bsqIndex"].as<int>() ;
        this->dataType = root["dataType"].as<int>() ;
        this->fromdt = root["fromdt"].as<int64_t>() ;
        this->todt = root["todt"].as<int64_t>() ;
        this->hFamily = root["hFamily"].as<char*>() ;
        this->hPid = root["hPid"].as<int>() ;
        this->hpidblen = root["hpidblen"].as<int>() ;
        this->hTableName = root["hTableName"].as<char*>() ;
        this->regfile = root["regfile"].as<char*>() ;
        this->vmax = root["vmax"].as<double>() ;
        this->vmin = root["vmin"].as<double>() ;
        this->yxblen = root["yxblen"].as<int>() ;
        this->zlevel = root["zlevel"].as<int>() ;
        this->method = root["method"].as<char*>() ;//min,max,ave
        this->offsetdt = root["offsetdt"].as<char*>() ;
    }
    return true ;
}
string ZSContentFromMysql::int2str(int val) 
{
    char buff[64] ;
    sprintf(buff,"%d" , val) ;
    return string(buff) ;
}
string ZSContentFromMysql::double2str(double val) 
{
    char buff[64] ;
    sprintf(buff,"%f" , val) ;
    return string(buff) ;
}
string ZSContentFromMysql::long2str(int64_t val) 
{
    char buff[64] ;
    sprintf(buff,"%ld" , val) ;
    return string(buff) ;
}

void ZSSparkInput::fromZSContentFromMysql(  ZSContentFromMysql& zsc ,int imode) 
{
    this->combMax = zsc.double2str(zsc.bandValidMax) ;
    this->combMin = zsc.double2str(zsc.bandValidMin) ;
    this->dataType = zsc.int2str(zsc.dataType) ;
    this->filldata = zsc.int2str( (int)zsc.bandNodata);
    this->fromdt = zsc.long2str( zsc.fromdt ) ;
    this->todt = zsc.long2str( zsc.todt ) ;
    
    this->hfami = zsc.hFamily;
    this->hpid = zsc.int2str(zsc.hPid) ;
    this->hpidblen = zsc.int2str(zsc.hpidblen) ;
    this->htable = zsc.hTableName ;
    this->iband = zsc.int2str( zsc.bsqIndex ) ;
    this->method = zsc.method  ;
    if( imode ==0 || imode==1 ){
        this->mode = "sk" ;
    }else{
        this->mode = "ls" ;
    }
    this->offsetdt = zsc.offsetdt ;
    this->regionfile = zsc.regfile ;
    
    this->validMax = zsc.double2str(zsc.vmax) ;
    this->validMin = zsc.double2str(zsc.vmin) ;
    this->yxblen = zsc.int2str(zsc.yxblen) ;
    this->zlevel = zsc.int2str(zsc.zlevel) ;
}



bool ZSSparkInput::writeToJsonFile(string outfilename) 
{
    FILE* pf = fopen(outfilename.c_str(),"w") ;
    if( pf!= 0 ){
        fprintf(pf,"{") ;
        
        fprintf(pf,"  \"combMax\":\"%s\"," , this->combMax.c_str() ) ;
        fprintf(pf,"  \"combMin\":\"%s\"," , this->combMin.c_str() ) ;
        fprintf(pf,"  \"dataType\":\"%s\"," , this->dataType.c_str() ) ;
        fprintf(pf,"  \"filldata\":\"%s\"," , this->filldata.c_str() ) ;
        
        fprintf(pf,"  \"fromdt\":\"%s\"," , this->fromdt.c_str() ) ;
        fprintf(pf,"  \"hfami\":\"%s\"," , this->hfami.c_str() ) ;
        fprintf(pf,"  \"hpid\":\"%s\"," , this->hpid.c_str() ) ;
        fprintf(pf,"  \"hpidblen\":\"%s\"," , this->hpidblen.c_str() ) ;
        
        fprintf(pf,"  \"htable\":\"%s\"," , this->htable.c_str() ) ;
        fprintf(pf,"  \"iband\":\"%s\"," , this->iband.c_str() ) ;
        fprintf(pf,"  \"method\":\"%s\"," , this->method.c_str() ) ;
        fprintf(pf,"  \"mode\":\"%s\"," , this->mode.c_str() ) ;
        
        fprintf(pf,"  \"offsetdt\":\"%s\"," , this->offsetdt.c_str() ) ;
        fprintf(pf,"  \"regionfile\":\"%s\"," , this->regionfile.c_str() ) ;
        fprintf(pf,"  \"todt\":\"%s\"," , this->todt.c_str() ) ;
        fprintf(pf,"  \"validMax\":\"%s\"," , this->validMax.c_str() ) ;
        
        fprintf(pf,"  \"validMin\":\"%s\"," , this->validMin.c_str() ) ;
        fprintf(pf,"  \"yxblen\":\"%s\"," , this->yxblen.c_str() ) ;
        fprintf(pf,"  \"zlevel\":\"%s\"    " , this->zlevel.c_str() ) ;
        
        fprintf(pf,"}") ;
        fclose(pf) ;pf = 0 ;
        return true ;
    }else{
        return false ;
    }
}