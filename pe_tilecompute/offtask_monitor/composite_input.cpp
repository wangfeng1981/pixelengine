#include "composite_input.h"


CompositeContentFromMysql::CompositeContentFromMysql(string text) 
{
    DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(text); 
    
    inpid = root["inpid"].as<int>() ;
    bandindex = root["bandindex"].as<int>() ;
    fromdt = root["fromdt"].as<int64_t>() ;
    todt = root["todt"].as<int64_t>() ;
    vmin = root["vmin"].as<double>() ;
    vmax = root["vmax"].as<double>() ;
    filldata = root["filldata"].as<double>() ;
    
    method = root["method"].as<char*>() ;
    scriptfilename = root["scriptfilename"].as<char*>() ;
    outhtable = root["outhtable"].as<char*>() ;
    outhfami = root["outhfami"].as<char*>() ;
    
    outpid = root["outpid"].as<int>() ;
    outhpid = root["outhpid"].as<int>() ;
    outhpidblen = root["outhpidblen"].as<int>() ;
    outyxblen = root["outyxblen"].as<int>() ;
    outhcol = root["outhcol"].as<int>() ;
    
    userbound = root["userbound"].as<int>() ;
    left = root["left"].as<double>() ;
    right = root["right"].as<double>() ;
    top = root["top"].as<double>() ;
    bottom = root["bottom"].as<double>() ;
    
    zmin = root["zmin"].as<int>() ;
    zmax = root["zmax"].as<int>() ;
}



CompositeSparkInput::CompositeSparkInput(int oftid1 ,CompositeContentFromMysql& co ) 
{
    oftid = oftid1 ;
    scriptfilename = co.scriptfilename ;
    outhtable = co.outhtable ;
    outhfami = co.outhfami ;
    outhpid = co.outhpid ;
    outhpidblen = co.outhpidblen ;
    outyxblen = co.outyxblen ;
    outhcol = co.outhcol ;
    userbound = co.userbound ;
    left = co.left ;
    right = co.right ;
    top = co.top ;
    bottom = co.bottom ;
    zmin = co.zmin ;
    zmax = co.zmax ;
}

bool CompositeSparkInput::writeToJsonFile(string outfilename) 
{
    DynamicJsonBuffer jsonbuffer ;
    JsonObject& root = jsonbuffer.createObject() ;
    root["oftid"] = oftid ;
    root["scriptfilename"] = scriptfilename ;
    root["outhtable"] = outhtable ;
    root["outhfami"] = outhfami ;
    
    root["outhpid"] = outhpid ;
    root["outhpidblen"] = outhpidblen ;
    root["outyxblen"] = outyxblen ;
    root["outhcol"] = outhcol ;
    
    root["userbound"] = userbound ;
    root["left"] = left ;
    root["right"] = right ;
    root["top"] = top ;
    
    root["bottom"] = bottom ;
    root["zmin"] = zmin ;
    root["zmax"] = zmax ;
    
    string outJsonText ;
    root.printTo(outJsonText);
    std::ofstream ofs(outfilename.c_str());
    if( ofs.good()==true ){
        ofs<<outJsonText ;
        ofs.close() ;
        return true ; 
    }else{
        return false ;
    }

}