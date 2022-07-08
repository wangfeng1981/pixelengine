#ifndef DEBUGPIXELENGINEHELPERINTERFACE_H
#define DEBUGPIXELENGINEHELPERINTERFACE_H

#include <PixelEngine.h>


class DebugPixelEngineHelperInterface : public PixelEngineHelperInterface
{
    public:
        DebugPixelEngineHelperInterface();
        virtual ~DebugPixelEngineHelperInterface();

        //
	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData,
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText);

	//
	virtual bool getTileDataArray(
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
		string& errorText);

	//
	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText);

	//get render style by id from system
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText) ;

	//PixelEngineHelper 2020-9-24
	//virtual bool writeTileData(string& tb,string& fami,int64_t col,int pid,int z,int y,int x, PeTileData& tileData) = 0;

	//2022-3-6 从外部读取roi hseg.tlv数据 isUserRoi=1为用户roi，isUserRoi=0为系统ROI，rid为关系数据库中主键
	virtual bool getRoiHsegTlv(int isUserRoi,int rid,vector<unsigned char>& retTlvData) ;

	//2022-3-31 从外部获取 DatasetCollection 数据
    virtual bool getTileDataCollection(
        string& dsName,   //输入数据集名称
		vector<int64_t> datetimes, //输入日期时间数组，注意实际返回的数据不一定全部都有，具体要看retdtArr
		int z, int y, int x, //瓦片坐标
		vector<vector<unsigned char>>& retTileDataArr, //返回二进制数据，一个datetime对应一个vector<unsigned char>
		vector<int64_t>& retdtArr ,//返回成功获取的 datetime数组，数量与retTileDataArr一致
		int& retdataType,//返回数据类型 1 byte，2 u16,3 i16, 4 u32, 5 i32, 6 f32, 7 f64
		int& retwid,     //返回瓦片宽度
		int& rethei,     //返回瓦片高度
		int& retnbands,  //返回瓦片波段数量
		string& errorText);

	//2022-4-1
    virtual bool buildDatetimeCollections(
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
    ) ;

    //2022-7-3
    virtual bool getNearestDatetime(string dsname,int64_t currdt,
    int isBefore,//1 or 0
    int64_t& retDt, string& retDisplay);


    //2022-7-8
    virtual bool getNearestDatetime2(string dsname,int64_t currdt,
        int isBefore,//1 or 0
        int64_t& retDt,
        int64_t& retDt0,int64_t& retDt1,
        string& retDisplay);


    protected:

    private:
};

#endif // DEBUGPIXELENGINEHELPERINTERFACE_H
