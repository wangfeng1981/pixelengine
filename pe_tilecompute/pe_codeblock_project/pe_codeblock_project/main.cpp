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
void unit_test_script_caller() ;//2022-9-6
void unit_test_filereadwrite_bash() ;//2022-9-9
void unit_test_data2rgba2png() ;//2022-9-28
void unit_test_file_exist();//2022-12-11

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


    cout<<"1"<<endl ;
    unit_test_datasetcollection() ;

    cout<<"2"<<endl ;
    unit_test_datasetcollection_array() ;

    cout<<"3"<<endl ;
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

    unit_test_script_caller() ;//2022-9-6

    unit_test_filereadwrite_bash();//2022-9-9

    unit_test_data2rgba2png() ;//2022-9-28

    unit_test_file_exist();//2022-12-11

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
    cout<<"-----------------unit test for DatasetCollection, unit_test_datasetcollection ---------------"<<endl;

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
    cout<<"logstr:"<<logstr<<endl ;
    cout<<"done."<<endl ;
}



void unit_test_datasetcollection_array()
{
    cout<<"-----------------unit test for DatasetCollections() unit_test_datasetcollection_array ---------------"<<endl;
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
    cout<<"-----------------unit_test_dscoll_forEachData ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    //1. let newDatasetCollection = datasetCollection.forEachData( func );
    //2. datasetCollection.mask( masktiledata , filldata );
    //3. let newDataset = datasetCollection.compose(method,vminInc,vmaxInc,filldata,outType);
    //4. dataset.map(oldval,newval);
    //5. dataset.map2(vminInc,vmaxInc,newval);
    //6. let maskds = dataset.buildmask( maskval );
    //7. let maskds = dataset.buildmask2(vminInc,vmaxInc);
    //8. 增加外部调用接口 RunScriptFunctionForTileResult( fullscriptWithExtraDataAndSDUI );
    //9. let dscoll=pe.NewDatasetCollection(datatype,w,h,nb,numdt);
    //10. dataset.mask(masktiledata,filldata);

    string script1 =
                "function main(){"
                "let dscoll1 = pe.DatasetCollection('test/name2',[1,2,3,4,5,6,7,8,9,10]);"
                "pe.log('test 1................');"
                "pe.log(dscoll1.dataArr[0][0]);"
                "pe.log(dscoll1.dataArr[0][256*256]);"
                "pe.log(dscoll1.dataArr[0][256*256*2]);"
                "pe.log(dscoll1.dataArr[1][0]);"
                "pe.log(dscoll1.dataArr[1][256*256]);"
                "pe.log(dscoll1.dataArr[1][256*256*2]);"
                "let dscoll2 = dscoll1.forEachData(function(data1){let data2=new Int32Array(65536);for(let i=0;i<65536;++i)data2[i]=data1[i]+10;return data2;});"
                "pe.log(dscoll2.dataArr[0][0]);"
                "pe.log(dscoll2.dataArr[1][0]);"
                "pe.log(dscoll2.dataArr[2][0]);"
                "pe.log('test 2................');"
                "let maskdata=new Uint8Array(65536);maskdata[0]=1;maskdata[1]=0;maskdata[2]=1;maskdata[3]=0;"
                "dscoll1.mask(maskdata,255);"
                "pe.log(dscoll1.dataArr[0][0]);"
                "pe.log(dscoll1.dataArr[0][1]);"
                "pe.log(dscoll1.dataArr[0][2]);"
                "pe.log(dscoll1.dataArr[0][3]);"
                "pe.log(dscoll1.dataArr[1][0]);"
                "pe.log(dscoll1.dataArr[1][1]);"
                "pe.log(dscoll1.dataArr[1][2]);"
                "pe.log(dscoll1.dataArr[1][3]);"
                "pe.log('test 3................');"
                "let ds1=pe.CompositeDsCollection(dscoll1,pe.CompositeMethodMax,0,210,255);"
                "let ds2=dscoll1.compose(pe.CompositeMethodSum,0,210,255,3);"
                "let ds3=dscoll2.compose(pe.CompositeMethodAve,0,210,255);"
                "pe.log(ds1.tiledata[0]);"
                "pe.log(ds2.tiledata[0]);"
                "pe.log(ds3.tiledata[0]);"
                "pe.log('test 4................');"
                "ds2.tiledata[0]=55;ds2.tiledata[1]=33;ds2.tiledata[2]=11;"
                "ds2.map(55,44);"
                "ds2.map(33,44);"
                "pe.log(ds2.tiledata[0]);"
                "pe.log(ds2.tiledata[1]);"
                "pe.log(ds2.tiledata[2]);"
                "pe.log('ds2.dataType:'+ds2.dataType);"
                "pe.log('test 5................');"
                "ds2.tiledata[0]=10;ds2.tiledata[1]=11;ds2.tiledata[2]=12;"
                "ds2.map2(10,11,99);"
                "pe.log(ds2.tiledata[0]);"
                "pe.log(ds2.tiledata[1]);"
                "pe.log(ds2.tiledata[2]);"
                "pe.log('test 6................');"
                "let mask1=ds2.buildmask(99);"
                "pe.log(mask1.tiledata[0]);"
                "pe.log(mask1.tiledata[1]);"
                "pe.log(mask1.tiledata[2]);"
                "pe.log('mask1.dataType:'+mask1.dataType);"
                "pe.log('test 7................');"
                "ds2.tiledata[0]=10;ds2.tiledata[1]=11;ds2.tiledata[2]=12;"
                "let mask2=ds2.buildmask2(10,11);"
                "pe.log(mask2.tiledata[0]);"
                "pe.log(mask2.tiledata[1]);"
                "pe.log(mask2.tiledata[2]);"
                "pe.log('mask2.dataType:'+mask2.dataType);"
                "pe.log('test 9................');"
                "let dscoll3=pe.NewDatasetCollection(3,256,256,1,3);"
                "dscoll3.dataArr[0][0]=1;dscoll3.dataArr[0][1]=2;dscoll3.dataArr[0][2]=3;"
                "dscoll3.dataArr[1][0]=4;dscoll3.dataArr[1][1]=5;dscoll3.dataArr[1][2]=6;"
                "dscoll3.dataArr[2][0]=7;dscoll3.dataArr[2][1]=8;dscoll3.dataArr[2][2]=9;"
                "dscoll3.mask( mask2.tiledata, 0);"
                "let ds4=dscoll3.compose(4,1,254,0,3);"
                "pe.log(ds4.tiledata[0]);"
                "pe.log(ds4.tiledata[1]);"
                "pe.log(ds4.tiledata[2]);"
                "pe.log('test10 ...............');"
                "let mask10=new Uint8Array(65536);"
                "mask10[0]=0;mask10[1]=0;mask10[2]=1;"
                "ds4.mask(mask10,99);"
                "pe.log(ds4.tiledata[0]);"
                "pe.log(ds4.tiledata[1]);"
                "pe.log(ds4.tiledata[2]);"
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
    cout<<"::::log::::"<<endl<<logstr<<endl ;
}


//2022-9-6
void unit_test_script_caller() {
    cout<<"-----------------unit_test_script_caller ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "pe.extraData={datetime:20010909000000,dtArr:[1,2,3,4]};"
                "function main(){"
                "let ds=pe.Dataset('test/name',pe.extraData.datetime);"
                "return ds;"
                "}"
                "function callerOne(){"
                "let dscoll=pe.DatasetCollection('test/name2',pe.extraData.dtArr);"
                "if(typeof dscoll ==='undefined') return null;"
                "let ds=dscoll.compose(4,1,255,0);"
                "pe.log('dtype:'+ds.dataType);"
                "return ds;"
                "}"
                ;
    PeTileData res1 ;
    bool ok1 = pe.RunScriptFunctionForTileResult(script1,"callerOne",0,0,0,res1) ;
    cout<<"run caller:"<<ok1<<endl ;
    cout<<"::::log::::"<<endl<<pe.getPeLog()<<endl ;
    cout<<(int)res1.tiledata[0]<<endl ;
    cout<<(int)res1.tiledata[65536]<<endl ;
    cout<<(int)res1.tiledata[65536*2]<<endl ;
}

//2022-9-9
void unit_test_filereadwrite_bash()
{
cout<<"-----------------unit_test_filereadwrite_bash ---------------"<<endl;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;

    string script1 =
                "function main(){"
                "let ok1=pe.write_file('~/test1.json',null);"
                "let ok2=pe.write_file('~/test2.json',JSON.stringify({name:'hehe',age:11,sex:'male'}) ) ;"
                "let read1=pe.read_file('~/test1.json');"
                "let read2=pe.read_file('~/test2.json');"
                "let code1=pe.call_bash('do nothing.');"
                "let code2=pe.call_bash('ls ~');"
                "pe.log('ok1 '+ok1);"
                "pe.log('ok2 '+ok2);"
                "pe.log('read1 '+read1);"
                "pe.log('read2 '+read2);"
                "pe.log('bash1 '+code1);"
                "pe.log('bash2 '+code2);"
                "}"
                "function func2(){"
                "let ok1=pe.write_file('/home/hadoop/test1.json',null);"
                "let ok2=pe.write_file('/home/hadoop/test2.json',JSON.stringify({name:'hehe',age:11,sex:'male'}) ) ;"
                "let read1=pe.read_file('/home/hadoop/test1.json');"
                "let read2=pe.read_file('/home/hadoop/test2.json');"
                "let code1=pe.call_bash('do nothing.');"
                "let code2=pe.call_bash('ls ~');"
                "pe.log('ok1 '+ok1);"
                "pe.log('ok2 '+ok2);"
                "pe.log('read1 '+read1);"
                "pe.log('read2 '+read2);"
                "pe.log('bash1 '+code1);"
                "pe.log('bash2 '+code2);"
                "return read2;"
                "}"
                ;
    string res1,res2;
    bool ok1 = pe.RunScriptFunctionForTextResultOrNothing(script1,"main",0,0,0,res1);
    bool ok2 = pe.RunScriptFunctionForTextResultOrNothing(script1,"func2",0,0,0,res2);
    cout<<"run caller:"<<ok1<<endl ;
    cout<<"::::log::::"<<endl<<pe.getPeLog()<<endl ;
    cout<<"ok1 "<<ok1<<" res1 "<<res1<<endl ;
    cout<<"ok2 "<<ok2<<" res2 "<<res2<<endl ;

}

//2022-9-28
void unit_test_data2rgba2png()
{
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine pe ;
    //no v8 , no js, no helper
    vector<short> tempFourBandData(256*256*4) ;
    int ival=30000 ;
    for(int ib=0;ib<4;++ib){
        for(int iy =0;iy<256;++iy){
            for(int ix =0;ix<256;++ix )
            {
                tempFourBandData[ib*65536+iy*256+ix] = ival ;
                ival += 5 ;
                if( ival>=65000 ) ival=1 ;
            }
        }
    }

    pe::PeStyle style = pe::PeStyle::emptyStyle();
    style.type="linear";
    pe::PeColorElement ce0 , ce1 ;
    ce0.r=255;ce0.g=0;ce0.b=0;ce0.a=255;ce0.val=10000;
    ce1.r=0;ce1.g=0;ce1.b=255;ce1.a=255;ce1.val=30000;
    style.colors.push_back(ce0);
    style.colors.push_back(ce1);


    vector<unsigned char> rgba1 , rgba2, png1, png2;
    string error1,error2;

    bool ok1 = pe.RenderData2RgbaByPeStyle(
        (unsigned char*)tempFourBandData.data() ,
        3,
        256,256,4,
        style,
        rgba1,
        error1
    ) ;
    cout<<"1 "<<ok1<<","<<error1<<endl ;

    pe::PeStyle noStyle = pe::PeStyle::emptyStyle() ;
    bool ok2 = pe.RenderData2RgbaByPeStyle(
        (unsigned char*)tempFourBandData.data() ,
        3,
        256,256,4,
        noStyle,
        rgba2,
        error2
    ) ;
    cout<<"2 "<<ok2<<","<<error2<<endl ;

    bool ok11 = pe.rgbaData2Png(rgba1,256,256,png1);
    bool ok22 = pe.rgbaData2Png(rgba2,256,256,png2);
    cout<<"11 "<<ok11<<endl;
    cout<<"22 "<<ok22<<endl;

    if( ok11 ){
        FILE* pf = fopen("unit_test_data2rgba2png-1.png","wb");
        fwrite(png1.data(),1,png1.size(),pf);
        fclose(pf);
    }
    if( ok11 ){
        FILE* pf = fopen("unit_test_data2rgba2png-2.png","wb");
        fwrite(png2.data(),1,png2.size(),pf);
        fclose(pf);
    }

}

void unit_test_file_exist()
{
cout<<"-----------------unit_test_file_exist ---------------"<<endl;
    string ss = "function main(){"
                "let ok1=pe.file_exist('/home/hadoop/test.qgs');"
                "let ok2=pe.file_exist('/home/hadoop/test22222.qgs');"
                "pe.log(ok1);pe.log(ok2);"
                "}" ;
    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine pe ;
    string res1;
    bool ok1 = pe.RunScriptFunctionForTextResultOrNothing(ss,"main",0,0,0,res1);
}

