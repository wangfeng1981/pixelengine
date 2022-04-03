#include "debugpixelenginehelperinterface.h"

DebugPixelEngineHelperInterface::DebugPixelEngineHelperInterface()
{
    //ctor
}

DebugPixelEngineHelperInterface::~DebugPixelEngineHelperInterface()
{
    //dtor
}


bool DebugPixelEngineHelperInterface::getTileData(
    int64_t dt, string& dsName, vector<int> bandindices,
    int z, int y, int x, vector<unsigned char>& retTileData,
    int& dataType,
    int& wid,
    int& hei,
    int& nbands,
    string& errorText)
{
    cout<<"in DebugPixelEngineHelperInterface::getTileData"<<endl ;
    cout<<"dt "<<dt<<endl ;
    cout<<"dsName "<<dsName<<endl ;
    cout<<"z "<<z<<endl ;
    cout<<"y "<<y<<endl ;
    cout<<"x "<<x<<endl ;
    dataType = 3 ;//int16
    wid = 256 ;
    hei = 256 ;
    nbands = bandindices.size() ;
    if( nbands==0 ) nbands = 1 ;
    retTileData.resize( nbands * 256*256 * 2 , 0) ;
    for(int ib=0;ib<nbands;++ib )
    {
        short* dataPtr = (short*)retTileData.data() ;
        short* bandDataPtr = dataPtr + ib*256*256 ;
        for(int it = 0 ; it < 256*256 ; ++ it)
        {
            bandDataPtr[it] = ib + 1 ;
        }
    }
    return true ;
}

//
bool DebugPixelEngineHelperInterface::getTileDataArray(
    int64_t fromdtInclusive, int64_t todtInclusive,
    string& dsName, vector<int> bandindices, int z, int y, int x,
    int filterMonth,int filterDay,int filterHour,int filterMinu,
    int filterSec ,
    vector<vector<unsigned char>>& retTileDataArr,
    vector<int64_t>& dtArr ,
    int& dataType,
    int& wid,
    int& hei,
    int& nbands,
    string& errorText)
{
    cout<<"in DebugPixelEngineHelperInterface::getTileDataArray, this is deprecated."<<endl ;
    errorText = "deprecated method." ;
    return false ;
}

//
bool DebugPixelEngineHelperInterface::getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText)
{
    errorText = "not implement" ;
    return false ;
}

//get render style by id from system
bool DebugPixelEngineHelperInterface::getStyle(string& styleid, PeStyle& retStyle, string& errorText)
{
    errorText = "not implement" ;
    return false ;
}



//2022-3-6 从外部读取roi hseg.tlv数据 isUserRoi=1为用户roi，isUserRoi=0为系统ROI，rid为关系数据库中主键
bool DebugPixelEngineHelperInterface::getRoiHsegTlv(int isUserRoi,int rid,vector<unsigned char>& retTlvData)
{
    return false;
}

//2022-3-31 从外部获取 DatasetCollection 数据
bool DebugPixelEngineHelperInterface::getTileDataCollection(
    string& dsName,   //输入数据集名称
    vector<int64_t> datetimes, //输入日期时间数组，注意实际返回的数据不一定全部都有，具体要看retdtArr
    int z, int y, int x, //瓦片坐标
    vector<vector<unsigned char>>& retTileDataArr, //返回二进制数据，一个datetime对应一个vector<unsigned char>
    vector<int64_t>& retdtArr ,//返回成功获取的 datetime数组，数量与retTileDataArr一致
    int& retdataType,//返回数据类型 1 byte，2 u16,3 i16, 4 u32, 5 i32, 6 f32, 7 f64
    int& retwid,     //返回瓦片宽度
    int& rethei,     //返回瓦片高度
    int& retnbands,  //返回瓦片波段数量
    string& errorText)
{
    //use three bands data 1,2,3
    cout<<"in DebugPixelEngineHelperInterface::getTileDataCollection"<<endl ;
    cout<<"dsName "<<dsName<<endl ;
    for(int idt = 0 ; idt < datetimes.size() ; ++ idt ) cout<<"idt-"<<idt<<" "<<datetimes[idt]<<endl ;
    cout<<"z "<<z<<endl ;
    cout<<"y "<<y<<endl ;
    cout<<"x "<<x<<endl ;

    retdataType = 1 ;//
    retdtArr = datetimes ;
    retwid = 256 ;
    rethei = 256 ;
    retnbands = 3 ;

    retTileDataArr.resize( datetimes.size() ) ;
    int dataval=200 ;
    for(int idt = 0 ; idt < datetimes.size() ; ++ idt )
    {
        retTileDataArr[idt].resize( retnbands*256*256*1 ) ;
        for(int ib = 0 ; ib < retnbands;++ib )
        {
            dataval ++ ;
            unsigned char* dataPtr = (unsigned char*)retTileDataArr[idt].data() ;
            unsigned char* bandDataPtr = dataPtr + ib*256*256 ;
            for(int it = 0 ; it < 256*256 ; ++ it)
            {
                bandDataPtr[it] = dataval ;
            }
        }
    }
    cout<<"out DebugPixelEngineHelperInterface::getTileDataCollection"<<endl ;
    return true ;
}


//2022-4-1
bool DebugPixelEngineHelperInterface::buildDatetimeCollections(
    string dsName,
    int64_t whole_start ,
    int whole_start_inc , //0 or 1
    int64_t whole_stop ,
    int whole_stop_inc ,
    string repeat_type , // '' 'm' 'y'
    int64_t repeat_start,
    int repeat_start_inc,
    int64_t repeat_stop,
    int repeat_stop_inc,
    int repeat_stop_nextyear, //0 or 1
    vector<DatetimeCollection>& dtcollarray
)
{
    cout<<"whole:"<<whole_start<<endl ;
    cout<<whole_start_inc<<endl ;
    cout<<whole_stop<<endl ;
    cout<<whole_stop_inc<<endl ;
    cout<<repeat_type<<endl ;
    cout<<repeat_start<<endl ;
    cout<<repeat_start_inc<<endl ;
    cout<<repeat_stop<<endl ;
    cout<<repeat_stop_inc<<endl ;
    cout<<repeat_stop_nextyear<<endl;

    DatetimeCollection dtc1 , dtc2 ;
    dtc1.key="2001" ;
    dtc1.datetimes.push_back(20010101235959L) ;
    dtc1.datetimes.push_back(20010102203030L) ;

    dtc2.key = "2002" ;
    dtc2.datetimes.push_back(20020101235959L) ;
    dtc2.datetimes.push_back(20020102203030L) ;
    dtc2.datetimes.push_back(20020103203030L) ;
    dtc2.datetimes.push_back(20020104203030L) ;
    dtc2.datetimes.push_back(20020105203030L) ;

    dtcollarray.push_back(dtc1) ;
    dtcollarray.push_back(dtc2) ;
    return true ;
}
