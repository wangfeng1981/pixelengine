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



bool CompositeSparkOutput::loadFromJson(string jsonfilename) 
{
    ifstream ifs(jsonfilename.c_str());
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(ifs); 
    if( root.success()==false ){
        spdlog::error("failed to parse json {}" , jsonfilename) ;
        return false ;
    }
    
    this->proj = root["proj"].as<char*>() ;
    this->minZoom = root["minZoom"].as<int>() ;
    this->maxZoom = root["maxZoom"].as<int>() ;
    this->dataType = root["dataType"].as<int>() ;
    this->tileWid = root["tileWid"].as<int>() ;
    this->tileHei = root["tileHei"].as<int>() ;
    
    JsonArray& barr = root["bandList"].as<JsonArray>() ;
    for(size_t ib = 0 ; ib < barr.size() ; ++ ib )
    {
        JsonObject& bobj = barr[ib].as<JsonObject>() ;
        CompositeProductBand band ;
        band.hPid = bobj["hPid"].as<int>() ;
        band.bsqIndex = bobj["bsqIndex"].as<int>() ;
        band.bName = bobj["bName"].as<char*>() ;
        band.scale = bobj["scale"].as<double>() ;
        band.offset = bobj["offset"].as<double>() ;
        band.validMin = bobj["validMin"].as<double>() ;
        band.validMax = bobj["validMax"].as<double>() ;
        band.noData = bobj["noData"].as<double>() ;
        this->bandList.push_back(band) ;
    }
    ifs.close() ;
    
    return true ;
}

bool CompositeSparkOutput::doDbWork(const MonitorConfig& config,CompositeContentFromMysql coParams,int userid) 
{
    bool updateok = CompositeSparkOutput::updateProductInDb(config,
        coParams.outpid, this->proj , 
        this->minZoom , this->maxZoom , 
        this->dataType , 0 , 
        coParams.outhtable , this->tileWid , this->tileHei , 
        "deflate" , 0 , 
        userid ) ;
    if( updateok==false ){
        return false ;
    }
    
    for(size_t ib = 0 ; ib < this->bandList.size(); ++ ib )
    {
        CompositeProductBand band = bandList[ib] ;
        bool bandok = CompositeSparkOutput::insertProductBandInDb(config,
            coParams.outpid, ib, band.hPid, band.bsqIndex,
            band.bName , band.scale , band.offset ,
            coParams.vmin , coParams.vmax ,
            band.noData ) ;
        if( bandok==false ){
            return false ;
        }
    }
    
    bool dataitemok = CompositeSparkOutput::insertProductDataItem(config,
        coParams.outpid,coParams.outhcol,
        coParams.left,coParams.right,coParams.top,coParams.bottom) ;
    if( dataitemok==false ){
        return false ;
    }
    
    return true ;
}


//更新产品记录
bool CompositeSparkOutput::updateProductInDb(const MonitorConfig& config,
        const int pid,string proj,
        int minZoom,int maxZoom,
        int dataType, int timeType,
        string htablename,int tilewid,int tilehei,
        string compress,int styleid,
        int userid) 
{
    //content json
    DynamicJsonBuffer jsonbuffer ;
    JsonObject& root = jsonbuffer.createObject() ;
    root["proj"] = proj ;
    root["minZoom"] = minZoom ;
    root["maxZoom"] = maxZoom ;
    root["dataType"] = dataType ;
    root["timeType"] = timeType ;
    root["hTableName"] = htablename ;
    root["tileWid"] = tilewid ;
    root["tileHei"] = tilehei ;
    root["compress"] = compress ;
    root["styleid"] = styleid ;
    string productjsoncontent ;
    root.printTo(productjsoncontent) ;
    spdlog::info("product json content : {}" , productjsoncontent) ;
    
    string upsql = string("UPDATE tbproduct SET info='")
        + productjsoncontent + 
        "' WHERE pid=" + wStringUtils::int2str(pid) ;
    spdlog::info("update product sql : {}" , upsql) ;

    wMysql wmysql ;
    string connError ;
    bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
    if( connok==false ){
        spdlog::warn("wmysql connect failed when try to update product.") ;
        return false ;
    }
    int retval = wmysql.runsql(upsql) ;
    if( retval == 0 ){
        spdlog::info("wmysql runsql good {}.", retval) ;
        return true ;
    }else{
        spdlog::warn("wmysql runsql failed {}.", retval) ;
        return false ;
    }
}



bool CompositeSparkOutput::insertProductBandInDb(const MonitorConfig& config,
    int pid,int bindex,int hpid,int bsqindex,
    string bname,double scale,double offset,
    double validmin,double validmax,double nodata) 
{
    //content json
    DynamicJsonBuffer jsonbuffer ;
    JsonObject& root = jsonbuffer.createObject() ;
    root["hPid"] = hpid ;
    root["bsqIndex"] = bsqindex ;
    root["bName"] = bname ;
    root["scale"] = scale ;
    root["offset"] = offset ;
    root["validMin"] = validmin ;
    root["validMax"] = validmax ;
    root["noData"] = nodata ;
    string bandinfo ;
    root.printTo(bandinfo) ;
    spdlog::info("band info json content : {}" , bandinfo) ;
    
    string insql = string("INSERT INTO tbproductband (pid,bindex,info) VALUES (")
        + wStringUtils::int2str(pid) + ","
        + wStringUtils::int2str(bindex) + ","
        + "'" + bandinfo + "'" 
        + ")" ;
    spdlog::info("insert band sql : {}" , insql) ;

    wMysql wmysql ;
    string connError ;
    bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
    if( connok==false ){
        spdlog::warn("wmysql connect failed when try to insert product band.") ;
        return false ;
    }
    int retval = wmysql.runsql(insql) ;
    if( retval == 0 ){
        spdlog::info("wmysql runsql good {}.", retval) ;
        return true ;
    }else{
        spdlog::warn("wmysql runsql failed {}.", retval) ;
        return false ;
    }
}


bool CompositeSparkOutput::insertProductDataItem(const MonitorConfig& config,
    int pid,int64_t hcol,double left,double right,
    double top,double bottom) 
{
    std::string currdt = wmysql_current_datetimestr() ;
    
    
    string insql = string("INSERT INTO tbproductdataitem (pid,hcol,`left`,`right`,`top`,`bottom`,createtime,updatetime) VALUES (")
        + wStringUtils::int2str(pid) + ","
        + wStringUtils::long2str(hcol) + ","
        + wStringUtils::double2str(left) + ","
        + wStringUtils::double2str(right) + ","
        + wStringUtils::double2str(top) + ","
        + wStringUtils::double2str(bottom) + ","
        + "'" + currdt + "'" + ","
        + "'" + currdt + "'"
        + ")" ;
    spdlog::info("insert data item sql : {}" , insql) ;

    wMysql wmysql ;
    string connError ;
    bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
    if( connok==false ){
        spdlog::warn("wmysql connect failed when try to insert data item.") ;
        return false ;
    }
    int retval = wmysql.runsql(insql) ;
    if( retval == 0 ){
        spdlog::info("wmysql runsql good {}.", retval) ;
        return true ;
    }else{
        spdlog::warn("wmysql runsql failed {}.", retval) ;
        return false ;
    }
}