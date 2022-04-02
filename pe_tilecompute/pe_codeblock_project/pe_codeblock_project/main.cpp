#include <iostream>
//用Codeblock写代码，所以还要用到这个main函数

#include "../../PixelEngine.h"
#include "../../JavaPixelEngineHelperInterface.h"
#include "../../../../sharedcodes/zonalstat/whsegtlvobject.h"
#include "../../wcomputelonlatarea.h"
#include "wdatetime.h"
#include "debugpixelenginehelperinterface.h"

using namespace std;

void unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;

void unit_test_isTileOverlay() ;
void unit_test_tlv_statistic() ;
void unit_test_datasetcollection() ;//2022-3-31
void unit_test_datasetcollection_array();//2022-4-1
void unit_test_ndays_before() ;//2022-4-2
void unit_test_js_LocalBuildDtCollectionByStopDt() ;//2022-4-2
void unit_test_js_ds_extract_method();//2022-4-2
void unit_test_js_ds_subtract_method() ;
void unit_test_js_pe_stackdatasets_method() ;
void unit_test_js_compositedatasetcollection() ;

int main()
{
    cout << "Hello PixelEngine!" << endl;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    cout<<"PE Version:"<<pe.GetVersion()<<endl ;

//    cout<<"-----------------unit test 测试等经纬面积计算效率------------------"<<endl;
//    int64_t startdt =wDatetime::currentMilliSeconds() ;
//    WComputeLonLatArea clla ;
//    for(int i = 0 ; i<256*256; ++i ){
//        int iy = i % 128 ;
//        clla.computeArea(0 , 0 , iy) ;
//    }
//    int64_t stopdt =wDatetime::currentMilliSeconds() ;
//    cout<<256*256<<" dura "<<(stopdt-startdt)<<" ms"<<endl ;

    //cout<<"-----------------unit test 测试tlv区域统计------------------"<<endl;
    //unit_test_tlv_statistic() ;


    //cout<<"-----------------unit test------------------"<<endl;
    //unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;
    //unit_test_isTileOverlay();


    cout<<"-----------------unit test for DatasetCollection ---------------"<<endl;
    unit_test_datasetcollection() ;

    cout<<"-----------------unit test for DatasetCollections() ---------------"<<endl;
    unit_test_datasetcollection_array() ;

    unit_test_js_LocalBuildDtCollectionByStopDt() ;

    unit_test_js_ds_extract_method() ;

    unit_test_js_ds_subtract_method() ;

    unit_test_js_pe_stackdatasets_method() ;

    unit_test_js_compositedatasetcollection() ;

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



void unit_test_datasetcollection()
{
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;
    string script1 ="pe.log('test datasetcollection.');"
                    "function main(){"
                    "let dtc={key:'testkey',datetimes:[20190109235959,1,2,3,4]};"
                    "let dsc=pe.DatasetCollection('test/collection',dtc);"
                    "pe.log(dsc.dtArr.length);"
                    "pe.log(dsc.nband);"
                    "pe.log(dsc.dataArr[0][0]);"
                    "pe.log(dsc.dataArr[1][1]);"
                    "return null;"
                    "}"
     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        0,0,0,
        tiledata,
        logstr
    ) ;
}



void unit_test_datasetcollection_array()
{
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 ="pe.log('test datasetcollections.');"
                "function main(){"
                "let dtc1={key:'2019',datetimes:[20190109235959,20190129235959]};"
                "let dtc2={key:'2020',datetimes:[20200101235959,20200102235959,20200103235959]};"
                "let dtcoll_arr=[dtc1,dtc2];"
                "let dsc_arr=pe.DatasetCollections('test/collection',dtcoll_arr);"
                "pe.log(dsc_arr.length);"
                "let dsc=dsc_arr[1];"
                "pe.log(dsc.nband);"
                "pe.log(dsc.dataArr[0][0]);"
                "pe.log(dsc.dataArr[1][1]);"
                "let test_dtcArr=pe.RemoteBuildDtCollections(20190109235959,2,3,4,'y',6,7,8,9,10);"
                "pe.log( JSON.stringify(test_dtcArr) );"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;
}




void unit_test_ndays_before() //2022-4-2
{
    cout<<"-----------------unit test for datecollection ndays before ---------------"<<endl;
    vector<int64_t> dtarr ;
    DatetimeCollection::makeDateList(2021,1,1,5,dtarr);
    for(int i=0;i<dtarr.size();++i) cout<<dtarr[i]<<endl;
}


//2022-4-2
void unit_test_js_LocalBuildDtCollectionByStopDt()
{
    cout<<"-----------------unit_test_js_LocalBuildDtCollectionByStopDt ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let dtc1=pe.LocalBuildDtCollectionByStopDt(2019,2,3,10);"
                "pe.log( dtc1.datetimes[1] );"
                "pe.log( dtc1.datetimes[2] );"
                "let dsc=pe.DatasetCollection('test',dtc1);"
                "pe.log(dsc.dataArr.length);"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;

}


//2022-4-2
void unit_test_js_ds_extract_method()
{
    cout<<"-----------------unit_test_js_ds_extract_method ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let ds1=pe.Dataset('test',20110202112030,[0,1,2]);"
                "let ds2=ds1.extract(2);"
                "pe.log( ds2.tiledata[1]  );"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;

}

//2022-4-2
void unit_test_js_ds_subtract_method()
{
    cout<<"-----------------unit_test_js_ds_subtract_method ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let ds1=pe.Dataset('test',20110202112030,[0,1,2]);"
                "let ds2=ds1.extract(2);"
                "let ds0=ds1.extract(0);"
                "let dsout=ds0.subtract(ds2,0,100,-9999);"
                "pe.log( dsout.tiledata[1]  );"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;

}



//2022-4-2
void unit_test_js_pe_stackdatasets_method()
{
    cout<<"-----------------unit_test_js_pe_stackdatasets_method ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let ds1=pe.Dataset('test',20110202112030,[0,1,2]);"
                "let ds2=ds1.extract(2);"
                "let ds0=ds1.extract(0);"
                "let dsout=pe.StackDatasets( [ds2,ds0] );"
                "pe.log( dsout.tiledata[1]  );"
                "pe.log( dsout.tiledata[65536 + 1]  );"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;

}


/// 2022-4-1 单个DatasetCollection合成 返回结果是一个Dataset
        /// let ds1 = PixelEngine.CompositeDsCollection(
        /// dsCollection
        /// ,method    //参考下面
        /// ,validMin
        /// ,validMax
        /// ,filldata
        /// [,outDataType]  //默认与输入数据一致
        /// ) ;

//2022-4-2
void unit_test_js_compositedatasetcollection()
{
    cout<<"-----------------unit_test_js_compositedatasetcollection ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let dtc1=pe.LocalBuildDtCollectionByStopDt(2019,2,3,10);"
                "pe.log( dtc1.datetimes[1] );"
                "pe.log( dtc1.datetimes[2] );"
                "let dsc=pe.DatasetCollection('test',dtc1);"
                "pe.log(dsc.dataArr.length);"
                "let ds1 = pe.CompositeDsCollection(dsc,pe.CompositeMethodMin,1,10,-9999) ;"
                "let ds2 = pe.CompositeDsCollection(dsc,pe.CompositeMethodMax,1,10,-9999) ;"
                "let ds3 = pe.CompositeDsCollection(dsc,pe.CompositeMethodAve,1,10,-9999) ;"
                "let ds4 = pe.CompositeDsCollection(dsc,pe.CompositeMethodSum,1,10,-9999) ;"
                "pe.log(ds1.tiledata[1]);"
                "pe.log(ds2.tiledata[1]);"
                "pe.log(ds3.tiledata[1]);"
                "pe.log(ds4.tiledata[1]);"
                "return null;"
                "}"

     ;
    PeTileData tiledata ;
    string extrastr = "" ;
    string logstr ;
    pe.RunScriptForTileWithoutRenderWithExtra(0,
        script1 ,
        extrastr ,
        1,2,3,
        tiledata,
        logstr
    ) ;

}







