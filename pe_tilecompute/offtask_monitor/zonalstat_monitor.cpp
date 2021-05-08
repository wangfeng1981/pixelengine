#include "zonalstat_monitor.h"

#include <chrono>
#include <thread>
#include "wmysql.h"
#include "wdirtools.h"
#include "wdatetimeUtil.h"
#include "wstringutils.h"
#include "composite_input.h"
#include "export_input.h"

const int MYSQL_RECONNECT_SECONDS = 10 ;
const int MAX_FAILED_COUNT=100 ;

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



//所有离线任务都在这里管理起来，不仅仅是区域统计的任务
void runZonalStatMonitor( const MonitorConfig& config ) 
{
    spdlog::info("runZonalStatMonitor start") ;
    int sleepmillseconds = config.sleep_sec*1000 ;
    int runsecondsForLog = 0 ;
    int secondsToLog = 600 ;//每十分中记录一下心跳
    int failedCount = 0 ;
    
    if( config.modearray.size() ==0 ){
        spdlog::warn("config.modearray is empty. exit.") ;
        return ;
    }
    
    int modeindex_cursor = 0 ;
    
    while( true ){
        
        const MonitorMode& monitorMode = config.modearray[modeindex_cursor] ;
        spdlog::info("monitor offtask mode:{}" , monitorMode.mode) ;
        ++ modeindex_cursor ;
        if( modeindex_cursor ==config.modearray.size() ){
            modeindex_cursor = 0 ;
        }
        
        //查询数据库
        vector<vector<string> > selResults ;
        {
            string modestr = wStringUtils::int2str( monitorMode.mode) ;
            string selsql = string("select tid,mode,content,uid from tbofftaskzonalstat where mode=")+ modestr
                +" AND status=0 Order By tid ASC Limit 1";
            wMysql wmysql ;
            string connError ;
            bool connok = wmysql.connect(config.host,config.user,config.pwd,config.db,connError) ;
            if( connok==false ){
                spdlog::warn("wmysql connect failed.") ;
                std::this_thread::sleep_for(std::chrono::milliseconds(MYSQL_RECONNECT_SECONDS*1000));
                ++ failedCount ;
                if( failedCount>MAX_FAILED_COUNT ){
                    spdlog::warn("failed count exceed:{}" ,MAX_FAILED_COUNT ) ;
                    break ;
                }
                continue ;
            }
            int selcode = wmysql.selectsql(selsql , selResults) ;
            if( selcode!=0 ){
                spdlog::warn("wmysql select failed:"+selsql) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(MYSQL_RECONNECT_SECONDS*1000));
                ++ failedCount ;
                if( failedCount>MAX_FAILED_COUNT ){
                    spdlog::warn("failed count exceed:{}" ,MAX_FAILED_COUNT ) ;
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
            string offtaskcontent = selResults[0][2] ;//json内容
            string taskuserid = selResults[0][3] ;
            int taskuseridval = atof( taskuserid.c_str() ) ;
            
            spdlog::info("find offtask id {}" , offtaskid) ;
            //更新任务状态
            writeTaskStatus(config , taskid , "1" , "" , "" ) ;
            
            //生成输入输出json文件的路径
            string currdatetime = wDatetime::currentDateTimeStrTight() ;
            string yearstr = currdatetime.substr(0,4) ;
            string ymdstr = currdatetime.substr(0,8) ;
            vector<string> subdirs ;
            subdirs.push_back(yearstr) ;
            subdirs.push_back(ymdstr) ;
            string dirError ;
            wDirTools dirTools ;
            //没有必要生产子目录
            //string productDir = dirTools.makeProductDir( config.offtaskjsondir,subdirs, dirError) ;
            string inputjsonfilename  = "";  
            string outputfilenamedb = "" ;
            string outputfilename = "";  //这个值不在c++中生产了，改为在java生产并记录在mysql中。
            
            //临时变量 for 数据合成
            CompositeContentFromMysql compositeInputParams ;
            
            //临时变量 用于 数据导出记录入库的文件名称
            ExportContentFromMysql tempExportContentFromMysql ;
            
            //生成输入json文件
            if( monitorMode.mode==0 || monitorMode.mode==1 || monitorMode.mode==2 ){
                //区域统计任务，实况序列，历史序列 
                
                ZSContentFromMysql zscontent ;
                zscontent.fromJsonText( offtaskcontent) ;
                
                inputjsonfilename  = zscontent.outfilename + ".in.json" ;
                outputfilename =     zscontent.outfilename ;
                outputfilenamedb   = zscontent.outfilenamedb ;
                
                ZSSparkInput sparkinputjson ;
                sparkinputjson.fromZSContentFromMysql( zscontent, imode ) ;
                
                spdlog::info("write input json {}" , inputjsonfilename) ;
                bool inputjsonok = sparkinputjson.writeToJsonFile(inputjsonfilename) ;
                if( inputjsonok==false ){
                    spdlog::warn("write input failed: {}", inputjsonfilename) ;
                    //failed
                    writeTaskStatus(config , taskid , "3" , "write input json failed." , "" ) ;
                    ++ failedCount ;
                    if( failedCount>MAX_FAILED_COUNT ){
                        spdlog::warn("failed count exceed:{}" ,MAX_FAILED_COUNT ) ;
                        break ;
                    }
                    continue ; 
                }
                
            }else if( monitorMode.mode==4 ){
                //数据合成任务
                
                CompositeContentFromMysql coMysqlContent(offtaskcontent) ;
                compositeInputParams = coMysqlContent ;
                
                inputjsonfilename  = coMysqlContent.outfilename + ".in.json" ;
                outputfilename =     coMysqlContent.outfilename ;
                outputfilenamedb   = coMysqlContent.outfilenamedb ;
                
                CompositeSparkInput sparkinputjson(offtaskid , coMysqlContent) ;
                spdlog::info("write input json {}" , inputjsonfilename) ;
                bool inputjsonok = sparkinputjson.writeToJsonFile(inputjsonfilename) ;
                if( inputjsonok==false ){
                    spdlog::warn("write input failed: {}", inputjsonfilename) ;
                    //failed
                    writeTaskStatus(config , taskid , "3" , "write input json failed." , "" ) ;
                    ++ failedCount ;
                    if( failedCount>MAX_FAILED_COUNT ){
                        spdlog::warn("failed count exceed:{}" ,MAX_FAILED_COUNT ) ;
                        break ;
                    }
                    continue ; 
                }                
            }else if( monitorMode.mode==5 ){
                //数据导出任务

                ExportContentFromMysql mysqlcontent ;
                spdlog::info("begin parse mysql content for export tid:{}" ,offtaskid ) ;
                bool mysqlcontentParseOk = mysqlcontent.loadFromJson(offtaskcontent) ;
                if( mysqlcontentParseOk==false )
                {
                    ++ failedCount ;
                    spdlog::warn("parse mysql content failed for export tid:{}" ,offtaskid ) ;
                    writeTaskStatus(config , taskid , "3" , "parse json from mysql failed." , "" ) ;
                    continue ;
                }
                else{
                    inputjsonfilename  = mysqlcontent.outfilename + ".in.json" ;
                    outputfilename =     mysqlcontent.outfilename ;
                    outputfilenamedb   = mysqlcontent.outfilenamedb ;
                    
                    tempExportContentFromMysql = mysqlcontent ;
                    
                    ExportTaskInput taskinput( mysqlcontent) ;
                    spdlog::info("begin writing task input json {}" , inputjsonfilename) ;
                    bool writeok = taskinput.writeToJsonFile( inputjsonfilename) ;
                    if( writeok==false ){
                        ++ failedCount ;
                        spdlog::warn("write input failed: {}", inputjsonfilename) ;
                        writeTaskStatus(config , taskid , "3" , "write input json failed "+inputjsonfilename , "" ) ;
                        if( failedCount>MAX_FAILED_COUNT ){
                            spdlog::warn("failed count exceed:{}" ,MAX_FAILED_COUNT ) ;
                            break ;
                        }
                        continue ;
                    }else{
                        spdlog::info("write task input json {} ok." , inputjsonfilename) ;
                    }
                }
            }
            
            if( inputjsonfilename=="" || outputfilename==""  ||outputfilenamedb=="" ){
                spdlog::warn("inputjsonfilename or outputfilename is empty." ) ;
                writeTaskStatus(config , taskid , "3" , "inputjsonfilename or outputfilename is empty.", "" ) ;
                continue ;
            }
            
            
            //调用spark或者其他离线命令行
            string thecommand = monitorMode.cmdtem ;
            thecommand = wStringUtils::replaceString( thecommand,"{{{IN}}}" , inputjsonfilename) ;
            thecommand = wStringUtils::replaceString( thecommand,"{{{OUT}}}" , outputfilename) ;
            spdlog::info("begin call:{}" , thecommand) ;
            int retval = system( thecommand.c_str() ) ;
            spdlog::info("{} result:{}" ,thecommand, retval) ;
            
            //结果写回数据库
            if( wDirTools::isFileExist( outputfilename ) ){
                
                //数据库记录相对路径
                writeTaskStatus(config , taskid , "2" , "" , outputfilenamedb ) ;    
                
                //下面后续操作针对数据合成，其他任务没有这个要求
                if( monitorMode.mode==4 ){
                    //数据合成，更新数据库，添加产品id，波段信息等操作
                    CompositeSparkOutput coOutput ;
                    bool cook = coOutput.loadFromJson(outputfilename) ;
                    if( cook==false ){
                        spdlog::error("failed to load from json for composite result {}",outputfilename) ;
                    }else{
                        //添加产品id，波段信息等操作
                        bool dbok = coOutput.doDbWork(config,compositeInputParams,taskuseridval) ;
                        if( dbok == true ){
                            spdlog::info("db work for composite result OK.") ;
                        }else{
                            spdlog::error("db work for composite result failed.") ;
                        }
                    }
                }
            }
            else{
                //failed
                writeTaskStatus(config , taskid , "3" , "offtask command failed, no result file." , "" ) ;
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