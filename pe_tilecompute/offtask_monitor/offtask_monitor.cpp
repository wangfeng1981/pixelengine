//离线任务监控程序，每个一定时间扫描数据库
// tbofftaskzonalstat 找到最老的一条状态为0的任务，
// 改写任务状态为1，然后生成json开始执行spark任务，
// 任务结束后将结果写回数据库，等待下一次运行。



#include <vector>
#include <string>
#include "ajson5.h"
#include "zonalstat_monitor.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

using namespace std;



int main(int argc, char* argv[] ){
	cout<<"A program to monitor offtask. 2021-3-29 by wf"<<endl ;
	cout<<"usage:offtask_monitor config.json"<<endl ;
	cout<<"v1.0 2021-3-29"<<endl ;
    cout<<"v1.1 2021-4-1"<<endl ;
    cout<<"v2.0 use new monitor config.json, support multi-offtask. 2021-4-6"<<endl ;
    cout<<"v2.1 support data composite. 2021-4-7"<<endl ;

    if( argc!=2 ){
        cout<<"params not equals 2. exit."<<endl ;
        return 11 ;
    }

    string mconfigfile = argv[1] ;
    //global config
    MonitorConfig monitorConfig ;
    bool mconfigok = monitorConfig.loadFromJson( mconfigfile) ;
    if( mconfigok==false ){
        cout<<"failed to load json from :"<<mconfigfile<<endl ;
        return 12 ; 
    }
    monitorConfig.print() ;
    
    auto dailylogger = spdlog::daily_logger_mt("l", monitorConfig.logfile.c_str() , 2, 0) ;//2:00 am
    spdlog::flush_every(std::chrono::seconds(5) ) ;
    spdlog::set_default_logger(dailylogger );


    cout<<"start runZonalStatMonitor ( all offtask monitor )... "<<endl ;
    //名称虽然是zonalstat但是离线任务的事情统一给管理了2021-4-6
    runZonalStatMonitor(monitorConfig) ;




	return 0 ;

}




