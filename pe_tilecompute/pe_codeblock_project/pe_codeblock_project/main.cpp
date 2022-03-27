#include <iostream>
//用Codeblock写代码，所以还要用到这个main函数

#include "../../PixelEngine.h"
#include "../../JavaPixelEngineHelperInterface.h"
#include "../../../../sharedcodes/zonalstat/whsegtlvobject.h"


#include "../../wcomputelonlatarea.h"



#include "wdatetime.h"


using namespace std;

void unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;

void unit_test_isTileOverlay() ;
void unit_test_tlv_statistic() ;

int main()
{
    cout << "Hello PixelEngine!" << endl;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    cout<<"PE Version:"<<pe.GetVersion()<<endl ;

    cout<<"-----------------unit test 测试等经纬面积计算效率------------------"<<endl;
//    int64_t startdt =wDatetime::currentMilliSeconds() ;
//    WComputeLonLatArea clla ;
//    for(int i = 0 ; i<256*256; ++i ){
//        int iy = i % 128 ;
//        clla.computeArea(0 , 0 , iy) ;
//    }
//    int64_t stopdt =wDatetime::currentMilliSeconds() ;
//    cout<<256*256<<" dura "<<(stopdt-startdt)<<" ms"<<endl ;

    cout<<"-----------------unit test 测试tlv区域统计------------------"<<endl;
    unit_test_tlv_statistic() ;


    cout<<"-----------------unit test------------------"<<endl;
    unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;

    unit_test_isTileOverlay();

    return 0;
}


void unit_test_tlv_statistic()
{
    WHsegTlvObject roi ;
    string error;
    bool isok = roi.readFromFile( "/var/www/html/pe/roi/test-1100.geojson.hseg.tlv" , error);
    if( isok==false){
        cout<<"roi.readFromFile failed."<<endl ;
        return ;
    }
    vector<WStatisticData> statdatavec ;
    vector<short> tiledata(256*256*3,3) ;
    PixelEngine::computeStatistic( (short*)tiledata.data(), roi, 0 ,
        5,4,26,
        256,256, 3, 1, 255, statdatavec
    ) ;//beijing inside tile 5,4,26, baike has 16410km2, our compute 16836km2 in level5.
    cout<<statdatavec[0].allCnt<<endl ;
    cout<<statdatavec[0].areakm2<<endl ;
    cout<<statdatavec[0].fillCnt<<endl ;
    cout<<statdatavec[0].sq_sum<<endl ;
    cout<<statdatavec[0].sum<<endl ;
    cout<<statdatavec[0].validCnt<<endl ;
    cout<<statdatavec[0].validMax<<endl ;
    cout<<statdatavec[0].validMin<<endl ;

        cout<<statdatavec[1].allCnt<<endl ;
    cout<<statdatavec[1].areakm2<<endl ;
    cout<<statdatavec[1].fillCnt<<endl ;
    cout<<statdatavec[1].sq_sum<<endl ;
    cout<<statdatavec[1].sum<<endl ;
    cout<<statdatavec[1].validCnt<<endl ;
    cout<<statdatavec[1].validMax<<endl ;
    cout<<statdatavec[1].validMin<<endl ;
}







void unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv()
{
    cout<<"--- unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv ---"<<endl ;



}


void unit_test_isTileOverlay()
{
    int tilez = 7 ;
    int tiley = 21 ;
    int tilex = 106 ;
    WHsegTlvObject roi ;
    string error;
    bool isok = roi.readFromFile( "taihu.hseg.tlv" , error);
    if( isok==false){
        cout<<"roi.readFromFile failed."<<endl ;
        return ;
    }
    bool isover = roi.isTileOverlay(tilez,tiley,tilex);
    cout<<"isover:"<<isover<<endl ;
}
