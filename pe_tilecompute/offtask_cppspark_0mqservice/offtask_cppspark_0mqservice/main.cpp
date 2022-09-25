/// 2022-4-5 遥感大数据离线区域统计、数据合成、序列分析，这些调用spark的zeromq服务程序


#include <iostream>
#include "ajson5.h"
#include <fstream>
#include <string>
#include <list>
#include <zmq.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include  <mutex>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "wstringutils.h"
#include "offtaskworkerresult.h"


using namespace std;
using namespace ArduinoJson;



struct OrderMsg {
int ofid;
int mode;//0-zonalstat , 1-shikuang serial, 2-lishi serial, 4-tc2hbase , 5-export (not in this program)
         //6-gots
string orderRelFilepath;
inline OrderMsg():ofid(0),mode(0){}
void fromJsonString(char* jsonstr) ;
} ;

void OrderMsg::fromJsonString(char* jsonstr)
{
    spdlog::info("order parsing");
    DynamicJsonBuffer jbuffer ;
    JsonObject& root = jbuffer.parseObject(jsonstr) ;
    ofid = root["ofid"].as<int>() ;
    mode = root["mode"].as<int>() ;
    orderRelFilepath = root["orderRelFilepath"].as<char*>() ;
    spdlog::info("order parsing done for:{} , {}",ofid,mode);
}



list<OrderMsg> global_orderMsgList ;
std::mutex g_list_mutex;  //
string g_offtask_sparkv8tilecompute_jar ;
string g_sparkmaster ;
string g_pedir ;
string g_task17configfile ;
string g_sparksubmit ;
string g_sendResultSocket ;
string g_gotsworker ;//2022-9-12

bool loadTask17Config(string task17configfile,
    string& orderReceiverSocket,
    string& resultSenderSocket,
    string& jarfile,
    string& sparkmaster ,
    string& pedir,
    string& sparksubmit,
    string& gotsworker )
{
    ifstream ifs( task17configfile ) ;
    if( ifs.good() )
    {
        DynamicJsonBuffer jbuffer ;
        JsonObject& root = jbuffer.parseObject(ifs) ;
        bool parseok = root.success() ;
        if( parseok==false ){
            cout<<"failed to parse task17 config json"<<endl ;
            return false ;
        }
        cout<<"reading json.offtask_cppspark_order_recv_socket"<<endl ;
        orderReceiverSocket = root["offtask_cppspark_order_recv_socket"].as<char*>() ;
        cout<<"reading json.offtask_result_collector"<<endl ;
        resultSenderSocket = root["offtask_result_collector"].as<char*>() ;
        cout<<"reading json.offtask_sparkv8tilecompute_jar"<<endl ;
        jarfile = root["offtask_sparkv8tilecompute_jar"].as<char*>() ;
        cout<<"reading json.sparkmaster"<<endl ;
        sparkmaster = root["sparkmaster"].as<char*>() ;
        cout<<"reading json.pedir"<<endl ;
        pedir = root["pedir"].as<char*>() ;
        cout<<"reading json.sparksubmit"<<endl ;
        sparksubmit = root["sparksubmit"].as<char*>() ;
        cout<<"reading json.offtask_pe_gots_worker"<<endl;
        gotsworker =  root["offtask_pe_gots_worker"].as<char*>() ;

        return true ;
    }else
    {
        cout<<"bad task17configfile "<<task17configfile<<endl ;
        return false ;
    }
}


//send result back to task17
void sendResultMsgBackToTask17( OfftaskWorkerResult& offresult )
{
    spdlog::info("prepare send result msg") ;
    string jsonstr = offresult.toJson() ;
    void* context2 = zmq_ctx_new() ;
    void* socket2 = zmq_socket( context2 , ZMQ_PUSH ) ;
    const int timeout_ms=2000;
    int timeout_r1=zmq_setsockopt(socket2,
        ZMQ_CONNECT_TIMEOUT,
        &timeout_ms,
        sizeof(timeout_ms)
        );
    int timeout_r2=zmq_setsockopt(socket2,
        ZMQ_SNDTIMEO,
        &timeout_ms,
        sizeof(timeout_ms)
        );
    int   sendConnectResult = zmq_connect( socket2, g_sendResultSocket.c_str() ) ;
    spdlog::info("sendConnectResult:{}",sendConnectResult) ;
    int sendbytecnt = zmq_send(socket2 ,  jsonstr.c_str() , jsonstr.length() , 0 ) ;
    spdlog::info("send byte cnt:{}",sendbytecnt) ;
}


//return status code of spark program.
void processOrderThreadFunction()
{
    cout<<"inside processOrderThreadFunction"<<endl ;
    int heartbeat = 0 ;//every 60 seconds
    while(true)
    {
        OrderMsg msg ;

        {
            std::lock_guard<std::mutex> guard(g_list_mutex) ;
            if( global_orderMsgList.empty() != true )
            {
                msg = global_orderMsgList.front() ;
                global_orderMsgList.pop_front();
                spdlog::info("global_orderMsgList pop the front order for process, curr size:{}",global_orderMsgList.size() ) ;

            }
        }

        if( msg.ofid!= 0 )
        {
            //need to process
            spdlog::info("begin process {},{},{}", msg.ofid,msg.mode,msg.orderRelFilepath) ;
            {
                OfftaskWorkerResult toffresult;
                toffresult.ofid = msg.ofid ;
                toffresult.state = 0 ;
                toffresult.resultRelFilepath = "" ;
                toffresult.status = 1 ;
                sendResultMsgBackToTask17(toffresult) ;
            }

            if( msg.mode>=0 && msg.mode<=5  )
            {
                string relResultjsonfile = wStringUtils::replaceString(
                                            msg.orderRelFilepath,".json","-result.json") ;

                string sparkcmd = "" ;
                if( msg.mode==0 )
                {
                    //zonal-stat
                    /* spark-submit --master spark://vb1:7077 Task16SparkV8TileComputing.jar
                         task17config.json stat task-stat-order.json output-stat.json */

                    sparkcmd = g_sparksubmit + " --master "+g_sparkmaster
                        + " " + g_offtask_sparkv8tilecompute_jar
                        + " " + g_task17configfile
                        + " stat "
                        + " " + g_pedir + msg.orderRelFilepath
                        + " " + g_pedir + relResultjsonfile;


                }else if( msg.mode==1 || msg.mode==2 )
                {
                    //serial
                    /* spark-submit --master spark://vb1:7077 Task16SparkV8TileComputing.jar
                         task17config.json serial task-serial-order.json output-serial.json */

                    sparkcmd = g_sparksubmit + " --master "+g_sparkmaster
                        + " " + g_offtask_sparkv8tilecompute_jar
                        + " " + g_task17configfile
                        + " serial "
                        + " " + g_pedir + msg.orderRelFilepath
                        + " " + g_pedir + relResultjsonfile;
                }else if( msg.mode==4 )
                {
                    //composite
                    //spark-submit --master spark://vb1:7077 Task16SparkV8TileComputing.jar
                    //  task17config.json jshbase task-js2hb-order.json output-jshbase.json
                    sparkcmd = g_sparksubmit + " --master "+g_sparkmaster
                        + " " + g_offtask_sparkv8tilecompute_jar
                        + " " + g_task17configfile
                        + " jshbase "
                        + " " + g_pedir + msg.orderRelFilepath
                        + " " + g_pedir + relResultjsonfile;

                }

                if( sparkcmd.length() > 0 )
                {
                    spdlog::info("will call:{}" , sparkcmd) ;
                    int spark_rc = system(sparkcmd.c_str()) ;
                    spdlog::info("spark_rc:{}" , spark_rc) ;
                    OfftaskWorkerResult offresult;
                    offresult.ofid = msg.ofid ;
                    offresult.state = spark_rc ;
                    offresult.resultRelFilepath = relResultjsonfile ;
                    sendResultMsgBackToTask17(offresult) ;
                }else{
                    spdlog::info("empty sparkcmd") ;
                }

            }else if( msg.mode==6 )
            {
                string relResultjsonfile = wStringUtils::replaceString(
                                            msg.orderRelFilepath,".js","-result.json") ;
                //gots
                string gotsworkercmd = g_gotsworker + " " + g_pedir + msg.orderRelFilepath;
                spdlog::info("will call:{}" , gotsworkercmd) ;
                int gotsworkercmd_rc = system(gotsworkercmd.c_str()) ;
                spdlog::info("gotsworkercmd_rc:{}" , gotsworkercmd_rc) ;
                OfftaskWorkerResult offresult;
                offresult.ofid = msg.ofid ;
                offresult.state = gotsworkercmd_rc ;
                offresult.resultRelFilepath = relResultjsonfile ;
                sendResultMsgBackToTask17(offresult) ;
            }
        }else
        {//空队列等待1秒钟，如果刚刚处理了一个说明很可能队列不空，所以立即处理
            std::this_thread::sleep_for (std::chrono::seconds(1));
            ++ heartbeat ;
            if( heartbeat> 600 ){
                heartbeat = 0 ;
                spdlog::info("process thread heart beat!") ;
            }
        }

    }

}




int main(int argc , char* argv[])
{
    string version = "" ;
    cout << "A program to run spark task by using zeromq service. 2022-4-5" << endl;
    cout<<(version="v1.0.1 created 2022-4-5")<<endl ;
    cout<<(version="v1.1.4 created 2022-9-13")<<endl ;//add gots
    cout<<"usage: offtask_cppspark_0mqservice task17config.json"<<endl ;
    if( argc != 2 )
    {
        cout<<"argc not 2"<<endl ;
        return 11 ;
    }

    auto dailylogger = spdlog::daily_logger_mt("l", "logs/daily.txt" , 0, 0) ;//00:00 am
    spdlog::flush_every(std::chrono::seconds(1) ) ;
    spdlog::set_default_logger(dailylogger );
    spdlog::info("--------program start---------") ;
    spdlog::info(version) ;

    string task17configfile = argv[1] ;
    g_task17configfile = task17configfile ;
    cout<<"task17configfile:"<<task17configfile<<endl ;

    string orderRecvSocket ;

    bool configok = loadTask17Config(
        task17configfile
        ,orderRecvSocket
        ,g_sendResultSocket
        ,g_offtask_sparkv8tilecompute_jar
        ,g_sparkmaster
        ,g_pedir
        ,g_sparksubmit
        ,g_gotsworker ) ;

    if( configok==false ){
        cout<<"load config failed."<<endl ;
        return 12 ;
    }

    cout<<"orderRecvSocket:"<<orderRecvSocket<<endl ;
    cout<<"sendResultSocket:"<<g_sendResultSocket<<endl ;
    cout<<"offtask_sparkv8tilecompute_jar:"<<g_offtask_sparkv8tilecompute_jar<<endl ;
    cout<<"sparkmaster:"<<g_sparkmaster<<endl ;
    cout<<"pedir:"<<g_pedir<<endl ;
    cout<<"sparksubmit:"<<g_sparksubmit<<endl ;
    cout<<"gotsworker:"<<g_gotsworker<<endl ;

    cout<<"start order receiving socket ... "<<endl ;
    void* recvContext = zmq_ctx_new() ;
    void* recvSocket = zmq_socket(recvContext , ZMQ_PULL) ;
    int   recvConnectResult = zmq_connect( recvSocket, orderRecvSocket.c_str() ) ;
    cout<<"recvConnectResult:"<<recvConnectResult<<endl ;


    cout<<"start order processing thread"<<endl ;
    std::thread processingThread(processOrderThreadFunction);


    cout<<"order receiving socket running..."<<endl ;
    while(true)
    {
        char buffer[2048] ;
        int nbytes = zmq_recv( recvSocket , buffer , 2048 , 0) ;
        buffer[nbytes] = '\0' ;
        spdlog::info("recv nbytes {}" , nbytes) ;

        OrderMsg msg ;
        msg.fromJsonString(buffer) ;
        {
            std::lock_guard<std::mutex> guard(g_list_mutex) ;
            if( global_orderMsgList.size() < 1000 )
            {
                global_orderMsgList.push_back( msg ) ;
                spdlog::info("global_orderMsgList add new order, curr size:{}",global_orderMsgList.size() ) ;
            }else{
                spdlog::warn("global_orderMsgList exceed 1000 items, no one will be added.") ;
            }
        }
    }

    spdlog::info("clean zeromq.") ;
    zmq_close (recvSocket);
    zmq_ctx_destroy (recvContext);
    spdlog::info("program out.") ;
    cout<<"program out"<<endl ;
    return 0;
}
