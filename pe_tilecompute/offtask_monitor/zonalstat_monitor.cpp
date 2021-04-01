#include "zonalstat_monitor.h"

#include <chrono>
#include <thread>
#include "wmysql.h"
#include "wdirtools.h"
#include "wdatetimeUtil.h"
#include "wstringutils.h"

const int MYSQL_RECONNECT_SECONDS = 10 ;
const int MAX_MYSQL_FAILED_COUNT=100 ;

bool writeTaskStatus( const MonitorConfig& config,
    string taskid, string newstatus, string message, string resultfilename ){
    
    string updatesql = string("UPDATE tbofftaskzonalstat SET ")
        + " status='"+ newstatus + "', "
        + " message='" + message+"', "
        + " result='" + resultfilename+"' "
        + " WHERE tid=" + taskid  ;
    spdlog::info("run sql:{}" , updatesql) ;
    wMysql wmysql ;
    string connError;
    bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
    if( connok==false ){
        spdlog::warn("writeTaskStatus mysql conn failed.") ;
        return false ;
    }
    
    int retcode = wmysql.runsql( updatesql) ;
    spdlog::warn("writeTaskStatus runsql return code {}." , retcode) ;
    
    if( retcode==0){
        return true ;
    }else{
        return false ;
    }    
}




void runZonalStatMonitor( const MonitorConfig& config ) 
{
    spdlog::info("runZonalStatMonitor start") ;
    int sleepmillseconds = config.sleep_sec*1000 ;
    int runsecondsForLog = 0 ;
    int secondsToLog = 60 ;
    int mysqlFailedCount = 0 ;
    
    while( true ){
        
        //查询数据库
        vector<vector<string> > selResults ;
        {
            string selsql = "select tid,mode,content from tbofftaskzonalstat where status=0 Order By tid ASC Limit 1";
            wMysql wmysql ;
            string connError ;
            bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
            if( connok==false ){
                spdlog::warn("wmysql connect failed.") ;
                std::this_thread::sleep_for(std::chrono::milliseconds(MYSQL_RECONNECT_SECONDS*1000));
                ++ mysqlFailedCount ;
                if( mysqlFailedCount>MAX_MYSQL_FAILED_COUNT ){
                    spdlog::warn("wmysql failed exceed:{}" ,MAX_MYSQL_FAILED_COUNT ) ;
                    break ;
                }
                continue ;
            }
            int selcode = wmysql.selectsql(selsql , selResults) ;
            if( selcode!=0 ){
                spdlog::warn("wmysql select failed:"+selsql) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(MYSQL_RECONNECT_SECONDS*1000));
                ++ mysqlFailedCount ;
                if( mysqlFailedCount>MAX_MYSQL_FAILED_COUNT ){
                    spdlog::warn("wmysql failed exceed:{}" ,MAX_MYSQL_FAILED_COUNT ) ;
                    break ;
                }
                continue ; 
            }
        }
        
        //找到了一条任务
        if( selResults.size() > 0 )
        {
            string taskid =  selResults[0][0] ;
            int offtaskid = atof( taskid.c_str() ) ;
            string modestr = selResults[0][1] ;
            int imode = atof( modestr.c_str() ); 
            string offtaskcontent = selResults[0][2] ;
            spdlog::info("find offtask id {}" , offtaskid) ;
            //更新任务状态
            writeTaskStatus(config , taskid , "1" , "" , "" ) ;
            
            //生成输入json
            ZSContentFromMysql zscontent ;
            zscontent.fromJsonText( offtaskcontent) ;
            ZSSparkInput sparkinputjson ;
            sparkinputjson.fromZSContentFromMysql( zscontent, imode ) ;
            string currdatetime = wDatetime::currentDateTimeStrTight() ;
            string yearstr = currdatetime.substr(0,4) ;
            string ymdstr = currdatetime.substr(0,8) ;
            vector<string> subdirs ;
            subdirs.push_back(yearstr) ;
            subdirs.push_back(ymdstr) ;
            string dirError ;
            wDirTools dirTools ;
            string productDir = dirTools.makeProductDir( config.offtaskjsondir,subdirs, dirError) ;
            string inputjsonfilename = productDir + "/zs_in_" + currdatetime + ".json" ;
            string outputjsonfilename = productDir + "/zs_out_" + currdatetime + ".json" ;
            spdlog::info("write input json {}" , inputjsonfilename) ;
            bool inputjsonok = sparkinputjson.writeToJsonFile(inputjsonfilename) ;
            if( inputjsonok==false ){
                spdlog::warn("write input failed: {}", inputjsonfilename) ;
                //failed
                writeTaskStatus(config , taskid , "3" , "write input json failed." , "" ) ;
                continue ;
            }
            
            //调用spark
            string thecommand = config.zonalstatcmd ;
            thecommand = wStringUtils::replaceString( thecommand,"{{{IN}}}" , inputjsonfilename) ;
            thecommand = wStringUtils::replaceString( thecommand,"{{{OUT}}}" , outputjsonfilename) ;
            spdlog::info("begin call:{}" , thecommand) ;
            cout<<"begin call:"<<thecommand<<endl ;
            int retval = system( thecommand.c_str() ) ;
            spdlog::info("{} result:{}" ,thecommand, retval) ;
            
            //结果写回数据库
            if( wDirTools::isFileExist( outputjsonfilename ) ){
                //success
                writeTaskStatus(config , taskid , "2" , "" , outputjsonfilename ) ;
            }
            else{
                //failed
                writeTaskStatus(config , taskid , "3" , "spark failed." , "" ) ;
            }
        }
        
        //wait for next
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepmillseconds));
        runsecondsForLog += config.sleep_sec ;
        if( runsecondsForLog >= secondsToLog ){
            runsecondsForLog = 0 ;
            spdlog::info("runZonalStatMonitor running...") ;
        }
    }
    spdlog::info("runZonalStatMonitor out") ;
}