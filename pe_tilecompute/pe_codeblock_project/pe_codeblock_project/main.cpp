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

void unit_test_foreachpixel() ;//forEachPixel 2022-7-24

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
void unit_test_js_RemoteDtCollections_DsCollections();//2022-4-3
void unit_test_pe_datetime();//2022-7-3
void unit_test_run_main_text_result();//2022-7-17
void unit_test_get_datasetname();//2022-7-26
void unit_test_dscoll_forEachData();//2022-9-4

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

    unit_test_js_RemoteDtCollections_DsCollections();

    //2022-7-3
    unit_test_pe_datetime();

    //2022-7-17
    unit_test_run_main_text_result() ;

    //2022-7-24
    unit_test_foreachpixel() ;

    unit_test_get_datasetname();//2022-7-26


    unit_test_dscoll_forEachData();//2022-9-4


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
                "pe.log(dsc.dataArr[0][0]);"
                "pe.log(dsc.dataArr[1][0]);"
                "pe.log(dsc.dataArr[2][0]);"
                "let ds1 = pe.CompositeDsCollection(dsc,pe.CompositeMethodMin,1,235,255) ;"
                "let ds2 = pe.CompositeDsCollection(dsc,pe.CompositeMethodMax,1,240,255) ;"
                "let ds3 = pe.CompositeDsCollection(dsc,pe.CompositeMethodAve,1,240,255,6) ;"
                "let ds4 = pe.CompositeDsCollection(dsc,pe.CompositeMethodSum,1,250,255,6) ;"
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

//2022-4-3
void unit_test_js_RemoteDtCollections_DsCollections()
{
    cout<<"-----------------unit_test_js_RemoteDtCollections ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let dtcs=pe.RemoteBuildDtCollections('test',20010000000000,1,20020000000000,1,'y',19000101000000,1,19000228000000,1,0);"
                "pe.log( JSON.stringify(dtcs) );"
                "let dscs = pe.DatasetCollections('test',dtcs);"
                "let ds1=pe.CompositeDsCollections(dscs,pe.CompositeMethodMin,1,210,255);"
                "let ds2=pe.CompositeDsCollections(dscs,pe.CompositeMethodMax,1,220,255);"
                "let ds3=pe.CompositeDsCollections(dscs,pe.CompositeMethodAve,1,220,255,6);"
                "let ds4=pe.CompositeDsCollections(dscs,pe.CompositeMethodSum,1,220,255,6);"
                "pe.log(ds1.tiledata[1]);"
                "pe.log(ds2.tiledata[1]);"
                "pe.log(ds2.tiledata[65536+1]);"
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




//2022-7-3
void unit_test_pe_datetime()
{
    cout<<"-----------------unit_test_pe_datetime ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let dt=pe.Datetime(2011,1,2,12,30,59);"
                "pe.log(dt);"
                "dt=pe.Datetime('abc');"
                "pe.log(dt);"
                "dt=pe.Datetime(2011,'22');"
                "pe.log(dt);"
                "let dt1=pe.NearestDatetimeAfter('dsname', 20110901000000);"
                "let dt2=pe.NearestDatetimeBefore('dsname', 20110901000000);"
                "let dt3=pe.NearestDatetimeBefore('dsname', 1);"
                "pe.log( JSON.stringify(dt1) );pe.log( JSON.stringify(dt2) );pe.log( JSON.stringify(dt3) );"
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
    cout<<"logstr:"<< endl <<pe.getPeLog()<<endl ;
}



void unit_test_run_main_text_result()
{
    cout<<"-----------------unit_test_run_main_text_result ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "return null;"
                "}";
    string script2 =
                "function main(){"
                "let a = 1;"
                "}";
    string script3 =
            "function main(){"
            "let a={};a.url='http://localhost';a.params={};a.params.lyr='layer_name';"
            "return a;"
            "}";
    string script4 =
            "function main(){"
            "let a=b*c;"
            "return b;"
            "}";
    string script5=
        "function main(){"
        "let a={};a.url='http://localhost';a.params={};a.params.lyr='layer_name';"
        "return JSON.stringify(a);"
        "}";

    string res1,res2,res3,res4,res5 ;
    string extrastr = "" ;
    string logstr ;
    bool ok1 = pe.RunScriptForTextResultWithExtra(
        0,
        script1 ,
        extrastr ,
        res1,
        logstr
    ) ;
    cout<<"run script1:"<<ok1<<":"<<res1<<endl ;

    bool ok2 = pe.RunScriptForTextResultWithExtra(
        0,
        script2 ,
        extrastr ,
        res2,
        logstr
    ) ;
    cout<<"run script2:"<<ok2<<":"<<res2<<endl ;

    bool ok3 = pe.RunScriptForTextResultWithExtra(
        0,
        script3 ,
        extrastr ,
        res3,
        logstr
    ) ;
    cout<<"run script3:"<<ok3<<":"<<res3<<endl ;

    bool ok4 = pe.RunScriptForTextResultWithExtra(
        0,
        script4 ,
        extrastr ,
        res4,
        logstr
    ) ;
    cout<<"run script4:"<<ok4<<":"<<res4<<endl ;

    bool ok5 = pe.RunScriptForTextResultWithExtra(
        0,
        script5 ,
        extrastr ,
        res5,
        logstr
    ) ;
    cout<<"run script5:"<<ok5<<":"<<res5<<endl ;
}

void unit_test_foreachpixel()
{
cout<<"-----------------unit_test_foreachpixel ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let ds0=pe.NewDataset(1,256,256,1);"
                "let ds=pe.Dataset('test/3bands',20220101000000,[0,1,2]);"
                "pe.log(ds.tiledata[0]); pe.log(ds.tiledata[1]);"
                "pe.log(ds.tiledata[65535]); pe.log(ds.tiledata[65536+1]);"
                "pe.log(ds.tiledata[65536+2]); pe.log(ds.tiledata[65536*2+1]);"
                "pe.log(ds.tiledata[65536*2+2]); pe.log(ds.tiledata[65536*2+3]);"
                "pe.log(pe.pejs_version);"
                "ds0.tiledata[1]=99;"
                "pe.log(ds0.tiledata[0]);"
                "pe.log(ds0.tiledata[1]);"
                "return ds;"
                "}";

    PeTileData res1 ;
    string extrastr = "" ;
    string logstr ;
    bool ok1 = pe.RunScriptForTileWithoutRenderWithExtra(
        0,
        script1 ,
        extrastr,
        0,0,0,
        res1,
        logstr
    ) ;
    cout<<"run script1:"<<ok1<<endl ;
    cout<<"log:"<<logstr<<endl ;

}

void unit_test_get_datasetname()
{
    cout<<"-----------------unit_test_get_datasetname ---------------"<<endl;
    PixelEngine pe ;


    string script1 =
                "function main(){"
                "let ds0=pe.NewDataset(1,256,256,1);"
                "let ds=pe.Dataset('test/name1',20220101000000,[0,1,2]);"
                "let ds1=pe.DatasetCollection('test/name2',[20220701000000]);"
                "let ds2=pe.DatasetCollections('test/name3',[20220701000000]);"
                "return ds;"
                "}";
    vector<string> names ;
    string error;
    bool ok1 = pe.GetDatasetNameArray(0,script1,names,error) ;
    cout<<"ok1:"<<ok1<<endl ;
    for(int i = 0 ; i<names.size();++i){
        cout<<"Get DsName "<<i<<":"<<names[i]<<endl ;
    }
}



//2022-9-4
void unit_test_dscoll_forEachData() {
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let dscoll1 = pe.DatasetCollection('test/name2',[1,2,3,4,5,6,7,8,9,10]);"
                "let dscoll2 = dscoll1.forEachData(function(data1){let data2=new Int32Array(65536);for(let i=0;i<65536;++i)data2[i]=data1[i]+1;return data2;});"
                "let ds1=pe.CompositeDsCollection(dscoll1,pe.CompositeMethodMin,0,210,255);"
                "let ds2=pe.CompositeDsCollection(dscoll2,pe.CompositeMethodMin,0,210,255);"
                "pe.log(ds1.tiledata[0]);"
                "pe.log(ds2.tiledata[0]);"
                "return ds2;"
                "}";

    PeTileData res1 ;
    string extrastr = "" ;
    string logstr ;
    bool ok1 = pe.RunScriptForTileWithoutRenderWithExtra(
        0,
        script1 ,
        extrastr,
        0,0,0,
        res1,
        logstr
    ) ;
    cout<<"run script1:"<<ok1<<endl ;
    cout<<"log:"<<logstr<<endl ;


}





