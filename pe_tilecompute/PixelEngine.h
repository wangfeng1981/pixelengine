//PixelEngine.h
//add Custom ColorRamp 2020-6-20
//version 2.0
// return ds , if ds has 3 bands use rgb, else use first band render out[0,255];
//update 2022-3-22 15:45

#ifndef PIXEL_ENGINE_H

#define PIXEL_ENGINE_H

//must put this before v8.h
#define V8_COMPRESS_POINTERS

#include <v8.h>
#include <libplatform/libplatform.h>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <ctime>
#include <chrono>
#include  <algorithm>
#include <fstream>
#include <sstream>

#include "lodepng.h"

#include "PeStyle.h"
#include "PeTileData.h"
#include "wstringutil.h"
#include "wstringutils.h" //2022-3-6
#include "wTextfilereader.h"
#include "wDatasetDatetime.h"
#include "wAST.h"
#include "pemultipolygon.h"//2020-10-15
#include "peroi.h"
#include "ajson5.h"
#include "esprimacpptool.h"//2022-2-12
#include "whsegtlvobject.h"//2022-3-6
#include "wstatisticdata.h"//2022-3-26

//2022-3-26
#include "wcomputelonlatarea.h"
#define W_COMPUTE_LON_LAT_AREA_IMPLEMENT
#include "wcomputelonlatarea.h"

//2022-4-1
#include "datetimecollection.h"
//2022-4-2
#include "template_methods.h"


using namespace v8;
using namespace std;
using namespace ArduinoJson;

using pe::wAST;

struct PixelEngineColorRamp;
using pe::PeStyle;
using pe::PeTileData;

//PixelEngineHelperInterface
//v1
//v2 增加获取hseg.tlv数据接口
struct PixelEngineHelperInterface {
	//
	//virtual bool getTileDataByPid(string& tbname,string& fami,long dt,int pid,vector<int> bandindices,int z,int y,int x,vector<unsigned char> retTileData,string& errorText);

	//
	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData,
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText)=0;

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
		string& errorText)=0;

	//
	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText)=0;

	//get render style by id from system
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText) =0;

	//PixelEngineHelper 2020-9-24
	//virtual bool writeTileData(string& tb,string& fami,int64_t col,int pid,int z,int y,int x, PeTileData& tileData) = 0;

	//2022-3-6 从外部读取roi hseg.tlv数据 isUserRoi=1为用户roi，isUserRoi=0为系统ROI，rid为关系数据库中主键
	virtual bool getRoiHsegTlv(int isUserRoi,int rid,vector<unsigned char>& retTlvData)=0 ;



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
		string& errorText)=0;
    //2022-3-31 从外部获取日期时间集合对象
    virtual bool buildDatetimeCollections(
        string dsName, //2022-4-3
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
    ) = 0 ;

    //2022-7-3
    //isBefore = 1 , get before nearest., others (0)  get after nearest.
    //deprecated , use getNearestDatetime2
    virtual bool getNearestDatetime(string dsname,int64_t currdt, int isBefore,int64_t& retDt, string& retDisplay)=0 ;

    //2022-7-8
    //isBefore = 1 , get before nearest., others (0)  get after nearest.
    virtual bool getNearestDatetime2(string dsname,int64_t currdt, int isBefore,
        int64_t& retDt,
        int64_t& retDt0, int64_t& retDt1,
        string& retDisplay)=0 ;

    inline static string version(){ return "v4.1" ; }//2022-7-8
};






//deprecated , use PixelEngine_GetDataFromExternal2_FunctionPointer
// typedef bool (*PixelEngine_GetDataFromExternal_FunctionPointer)(
// 		void* pePtr,
// 		string ,//name
// 		string ,//datetime
// 		vector<int>& ,//bands [0,1,2] , not used actually
// 		vector<unsigned char>&,//return binary
// 		int& dt,//return datatype
// 		int& wid,//return width
// 		int& hei,//return height
// 		int& nbands );//return nbands

//get data from external by z,y,x
typedef bool (*PixelEngine_GetDataFromExternal2_FunctionPointer)(
		void* pePtr,
		string ,//name
		string ,//datetime
		vector<int>& ,//bands [0,1,2] , not used actually
		int tilez,
		int tiley,
		int tilex,
		vector<unsigned char>&,//return binary
		int& dataType,//return datatype
		int& wid,//return width
		int& hei,//return height
		int& nbands);//return nbands

//get data array from external by z,y,x
typedef bool (*PixelEngine_GetDataFromExternal2Arr_FunctionPointer)(
		void* pePtr,
		string ,//name
		string ,//from datetime
		string ,//to datetime
		vector<int>& ,//bands [0,1,2] , not used actually
		int tilez,
		int tiley,
		int tilex,
		int filterMonth , //-1 ignored , 1-12
		int filterDay ,   //-1 ignored , 1-31
		int filterHour,  //-1 ignored , 0-23
		int filterMinu , //-1 ignored , 0-59
		int filterSec ,  //-1 ignored, 0-59
		vector<vector<unsigned char> >&,//return binary
		vector<long>& ,//return time array
		int& dataType,//return datatype
		int& wid,//return width
		int& hei,//return height
		int& nbands,//return nbands
		int& numds );//return number of dataset.



//get ColorRamp from external by StringID
typedef PixelEngineColorRamp (*PixelEngine_GetColorRampFromExternal_FunctionPointer)(
		void* pePtr, string );


//map reduce start ////////////////////////////////////////////////////////////////////
//not used yet,20200722
struct PixelEngineMapReduce
{
	string tabName ;
	long datetime ;
	int zlevel ;
	string mapTile2ObjFunction ;
	string reduceObj2ObjFunction ;
	bool isSame(PixelEngineMapReduce& mr) ;
} ;


struct PixelEngineMapReduceContainer
{
	vector<PixelEngineMapReduce> mapreduceVector ;
	void add(PixelEngineMapReduce& mr) ;
} ;



//map reduce end.////////////////////////////////////////////////////////////////////



struct PixelEngineTileInfo
{
	int z,y,x ;
} ;


struct PixelEngineColorRamp
{
	inline PixelEngineColorRamp():numColors(0){Nodata=0;NodataColor[0]=NodataColor[1]=NodataColor[2]=NodataColor[3]=0;} ;
	static const int MAXNUM_COLORS = 50 ;
	bool useInteger  ;
	int numColors ;
	int ivalues[MAXNUM_COLORS] ;
	float fvalues[MAXNUM_COLORS] ;
	unsigned char r[MAXNUM_COLORS] ;
	unsigned char g[MAXNUM_COLORS] ;
	unsigned char b[MAXNUM_COLORS] ;
	unsigned char a[MAXNUM_COLORS] ;
	std::string labels[MAXNUM_COLORS] ;
	double Nodata;
	unsigned char NodataColor[4] ;
	std::string NodataLabel ;
	bool unwrap( Isolate* isolate , Local<v8::Value> obj) ;//no labels
	bool unwrapWithLabels( Isolate* isolate , Local<v8::Value> obj) ;//unwrap labels.
	void copy2v8(Isolate* isolate , Local<v8::Value> obj) ;
	int upper_bound(int val) ;
	int binary_equal(int val) ;
} ;

//使用PE前，需要调用静态方法 PixelEngine::initV8()
//然后传入获取外部数据的接口指针 pe.helperPointer = &helperObj ;//helperObj 需要实现 PixelEngineHelperInterface接口并实例化。
struct PixelEngine
{
	static string pejs_version ;//pixelengine.js version.
	static vector<int> ColorRainbow  ;//1
	static vector<int> ColorBlues  ;//2
	static vector<int> ColorReds  ;//3
	static vector<int> ColorGreens  ;//4
	static vector<int> ColorGrays  ;//0 and others

	static vector<int> GetColorRamp(int colorid,int inverse=0) ;
	static void Value2Color(int valx,float K,int nodata,int* nodataColor,int vmin,int vmax,int interpol,vector<int>& colorRamp,int ncolor,unsigned char& rr,unsigned char& rg,unsigned char& rb,unsigned char& ra );
	static void Value2Color(int valx,PixelEngineColorRamp& cr,int interpol,unsigned char& rr,unsigned char& rg,unsigned char& rb,unsigned char& ra );
	static void ColorReverse(vector<int>& colors) ;
	static long RelativeDatetimeConvert(long curr,long relative) ;
	static bool IsMaybeLocalOK(MaybeLocal<Value>& val) ;

	//private
	void log(string& str) ;



	//PixelEngine
	static void GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl
    static void GlobalFunc_DatafileCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_DatasetArrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//load a datetime range dataset.
	static void GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl

	static void GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	/// let dataset=pe.NewDataset(datatype,width,height,nband);
	static void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	//2020-9-13
	static void GlobalFunc_GetStyleCallBack(const v8::FunctionCallbackInfo<v8::Value>& args);
	//pixelengine.roi()
	static void GlobalFunc_RoiCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;


	//Dataset methods:

	static void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_ColorRampCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	//dataset.clip()
	static void GlobalFunc_ClipCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

	/// 2022-3-6
	/// 对应javascript中 dataset.clip2函数
	/// js中调用示例： dataset.clip2("sys:1",0); dataset.clip2("user:1",0);
	static void GlobalFunc_Clip2CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

    //2020-12-01 convert dataset into other datatype,
    //in js var newds = dataset.convertToDataType(pe.DataTypeFloat32);
    static void GlobalFunc_ConvertToDataType(const v8::FunctionCallbackInfo<v8::Value>& args);




	//global methods:
	static void GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void Dataset2Png( Isolate* isolate, Local<Context>& context, Local<Value> dsValue
	, vector<unsigned char>& retpngbinary ) ;
	static bool initTemplate(PixelEngine* thePE,Isolate* isolate, Local<Context>& context );// not static
	static void initV8() ;
	static std::unique_ptr<v8::Platform> v8Platform ;

	//tools methods
	static bool V8ObjectGetIntValue(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,int& retvalue );
	static bool V8ObjectGetNumberValue(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,double& retvalue );
	static bool V8ObjectGetBoolValue(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,bool& retvalue );
	static bool V8ObjectGetUint8Array(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,int shouldElementNumber , void* copyToPtr );
	static bool V8ObjectGetInt32Array(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,int shouldElementNumber , void* copyToPtr );
	static bool V8ObjectGetInt16Array(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,int shouldElementNumber , void* copyToPtr );
	static bool V8ObjectGetFloat32Array(Isolate* isolate,Local<Object>&obj,Local<Context>&context,string key,int shouldElementNumber , void* copyToPtr );

	//2020-9-14
	static bool V8ObjectGetUint16Array(Isolate* isolate, Local<Object>& obj, Local<Context>& context, string key, int shouldElementNumber, void* copyToPtr);
	static bool V8ObjectGetUint32Array(Isolate* isolate, Local<Object>& obj, Local<Context>& context, string key, int shouldElementNumber, void* copyToPtr);
	static bool V8ObjectGetFloat64Array(Isolate* isolate, Local<Object>& obj, Local<Context>& context, string key, int shouldElementNumber, void* copyToPtr);
    static void* V8ObjectGetTypedArrayBackPtr(Isolate* isolate, Local<Object>& obj, Local<Context>& context, string key);
    //获取数组指针，同时获取字节长度
    static void* V8ObjectGetTypedArrayBackPtr2(Isolate* isolate, Local<Object>& obj, Local<Context>& context, string key,size_t& byteLen);



	//private method
	static Local<Object> CPP_NewDataset(Isolate* isolate,Local<Context>& context
		,const int datatype
		,const int width
		,const int height
		,const int nband );
	//2022-4-2
    static Local<Object> CPP_NewDataset(Isolate* isolate,Local<Context>& context
		,const int datatype
		,const int width
		,const int height
		,const int nband
		,string dsname
		,int64_t datetime
		,const int z
		,const int y
		,const int x );

	static Local<Object> CPP_NewDatasetArray(Isolate* isolate,Local<Context>& context
		,const int datatype
		,const int width
		,const int height
		,const int nband
		,const int numds );
	static string long2str(long val) ;



	//not static
	v8::Isolate* isolate ;
	v8::Isolate::CreateParams create_params;
	Global<Context> m_context ;//need Reset
	PixelEngineTileInfo tileInfo ;
	int64_t currentDateTime ;
	void* extraPointer ;//do not release.
	string pe_logs ;//max length 1k bytes.
	PixelEngineMapReduceContainer mapredContainer ;//not used yet,20200722
	vector<PeRoi> roiVector ;//不再维护，后续使用roi2.hseg.tlv 2022-3-22

public:
    //2022-3-22 add
	vector<string> m_dsnameDtVec ;//2022-3-22 记录脚本中访问的数据集名称与时间，名称与时间使用逗号分割，数据对使用分号分割
	vector<string> m_roi2Vec ;//2022-3-22 记录脚本中访问的ROI2的ID，类似sys:1,user:2
	inline vector<string> GetDsnameDtVec() { return m_dsnameDtVec; }
	inline vector<string> GetRoi2Vec() { return m_roi2Vec; }
	inline string GetPeLogs() { return pe_logs ; }
    //2022-3-22 end

	Global<Value> GlobalFunc_ForEachPixelCallBack ;//not static, need Reset
	Global<Value> GlobalFunc_GetPixelCallBack ;//not static , need reset
	Global<Value> GlobalFunc_ForEachDataCallBack ;//not static , need reset, let newdscoll=dscoll.forEachData(func); 2022-9-6
	PixelEngine() ;//one
	~PixelEngine() ;//three


	////////////////////////////////////////////////////////////////////////////
	/// below methods would be called by outsider

	/// 第一版瓦片计算接口，为了保持so的兼容性保留该接口，后面不再使用 2021-1-21
	//deprecated 2022-7-24 , use RunScriptForTileWithoutRenderWithExtra
	bool RunScriptForTile(void* extra,string& jsSource,long dt,int z,int y,int x, vector<unsigned char>& retbinary) ;

	/// 计算一次的瓦片接口，返回json，用于数据拉伸计算直方图等，很少使用 2021-1-21
	bool RunScriptForComputeOnce(void* extra, string& jsSource,long currentdt
                                            ,int z,int y,int x, string& retJsonStr ) ;

	/// 检查脚本语法错误
	string CheckScriptOk(string& scriptSource) ;
	inline string GetVersion() { return PixelEngine::pejs_version + " helper:"+PixelEngineHelperInterface::version() ; }
	//2020-9-13 get style from script 运行脚本以获得渲染方案
	bool RunToGetStyleFromScript(string& scriptContent, PeStyle& retstyle, string& retLogText);
	//2020-9-13
	// 运行脚本不渲染
	//deprecated 2022-7-24 , use RunScriptForTileWithoutRenderWithExtra
	bool RunScriptForTileWithoutRender(void* extra, string& scriptContent, int64_t currentDatetime,
		int z, int y, int x, PeTileData& tileData , string& logStr);
	// 运行脚本并渲染
	//deprecated 2022-7-24 , use RunScriptForTileWithoutRenderWithExtra
	bool RunScriptForTileWithRender(void* extra, string& scriptContent, PeStyle& inStyle, int64_t currentDatetime,
		int z, int y, int x, vector<unsigned char>& retPngBinary, int& pngwid,int& pnghei, string& logStr);//

    //2021-1-21
	// 运行脚本不渲染,增加对extraJsonText支持，因此不需要currentDatetime了
	bool RunScriptForTileWithoutRenderWithExtra(void* extra, string& scriptContent,
        string& extraJsonText,
		int z, int y, int x,
        PeTileData& tileData , string& logStr);
	//2021-1-21
    // 运行脚本并渲染,增加对extraJsonText支持，因此不需要currentDatetime了
	bool RunScriptForTileWithRenderWithExtra(void* extra, string& scriptContent, PeStyle& inStyle,
        string& extraJsonText,
        int z, int y, int x,
        vector<unsigned char>& retPngBinary, int& pngwid,int& pnghei, string& logStr);//

	// 运行脚本获取语法树 2020-9-19
	bool RunScriptForAST(void* extra, string& scriptContent, string& retJsonStr, string& errorText);
	//解析Dataset-Datetime 数据集时间日期对
	bool RunScriptForDatasetDatetimePairs(void* extra,
		string& scriptContent,vector<wDatasetDatetime>& retDsDtVec,string& errorText);

    //2022-3-22 获取脚本运行后输出的有用信息，比如dsname，dt，roi2，log等信息


    //获取脚本中全部数据集名称 2022-2-12
    //这个函数没有使用v8,不依赖其他初始化代码，直接使用即可，使用esprima.cpp实现AST解析
    //数据集名称包括 pe.Dataset(...) pe.DatasetArray(...) pe.DataFile(...)
    //2022-7-26 add pe.DatasetCollection and pe.DatasetCollections
    bool GetDatasetNameArray(void* extra,
		string& scriptContent,
        vector<string>& retDsNameArr,
        string& errorText) ;


    /**
     * @brief 通用执行js脚本程序，通过关键字获取该变量的值，这个值必须是字符串格式的，一般使用json的文本。
     * @param extra 保留关键字,jni pointer
     * @param scriptContent 脚本文本
     * @param dt 当前传入日期时间
     * @param z 瓦片z
     * @param y 瓦片y
     * @param x 瓦片x
     * @param variableName 需要获取变量的名字
     * @param retJsonText 返回的变量结果，需要是字符串格式
     * @param retError 返回错误信息，如果有错误
     * @return 返回true、false
     */
    bool RunScriptAndGetVariableJsonText(void* extra,
                string& scriptContent,
                int64_t dt,int z,int y,int x,
                string variableName,
                string& retJsonText,
                string& retError) ;

    //2022-7-17
	// 运行脚本中main函数，必须返回null或者字符串格式结果，注意返回"null"字符串与null是一样的
	// resultStr 的结果包括 "", "null", "not_string" , "........"
	bool RunScriptForTextResultWithExtra(
        void* extra,
        string& scriptContent,
        string& extraJsonText,
        string& resultStr,
        string& logStr);



    //mapreduce框架 获取目标zlevel
	bool MapRedRunScriptForZlevel(void* extra,
		string& scriptContent,int& retZlevel, string& error);

    //mapreduce框架 获取目标extent
	bool MapRedRunScriptForExtent(void* extra,
		string& scriptContent,double& left,double& right,double& up,double& down, string& error);

    //mapreduce框架 获取目标sharedobject json text
	bool MapRedRunScriptForSharedObj(void* extra,
		string& scriptContent,string& retJsonText, string& error);

    //mapreduce框架 运行map function
	bool MapRedRunScriptForMapFunc(void* extra,
		string& scriptContent,int64_t dt,int z,int y,int x,
        string& sharedObjJsonText, string& retJsonText, string& error);

    //mapreduce框架 运行reduce function
	bool MapRedRunScriptForReduceFunc(void* extra,
		string& scriptContent,string& sharedObjJsonText,
        string& keystr, string& obj1JsonText
        , string& obj2JsonText,
        string& retJsonText, string& error);

    //mapreduce框架 运行main function
	bool MapRedRunScriptForMainFunc(void* extra,
		string& scriptContent,string& sharedObjJsonText,
        string& objCollectionJsonText ,
        string& retJsonText, string& error);





	///
	////////////////////////////////////////////////////////////////////////////////



	//static PixelEngine_GetDataFromExternal_FunctionPointer GetExternalDatasetCallBack;//will deprecated
	static PixelEngine_GetDataFromExternal2_FunctionPointer GetExternalTileDataCallBack;
	static PixelEngine_GetDataFromExternal2Arr_FunctionPointer GetExternalTileDataArrCallBack ;
	static PixelEngine_GetColorRampFromExternal_FunctionPointer GetExternalColorRampCallBack ;


	//2020-9-12
	PixelEngineHelperInterface* helperPointer;


	////////////////////////// STATIC tools methods.
	//2020-9-13
	static PixelEngine* getPixelEnginePointer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static string convertV8LocalValue2CppString(Isolate* isolate, Local<Value>& v8value);
	static Local<Value> warpCppStyle2V8Object(Isolate* isolate, PeStyle& style);

	//private static tool methods
	private:
		static bool unwarpMultiPolygon(Isolate* isolate,Local<Value>& jsMulPoly,
			PeMultiPolygon& retMPoly );
		static bool convertV8MaybeLocalValue2Double(MaybeLocal<Value>& maybeVal,
			double& retval);
		static bool convertV8LocalValue2Int(Local<Value>& v8Val,int& retval) ;
        static bool convertV8LocalValue2IntArray(Local<Context>&context,Local<Value>& v8Val,vector<int>& retvec) ;
        static bool convertV8LocalValue2DoubleArray(Local<Context>&context,Local<Value>& v8Val,vector<double>& retvec) ;






	public:
	//2020-9-19
	static bool quietMode;

	private:
		//2020-9-14
		bool innerRenderTileDataByPeStyle(PeTileData& tileData, PeStyle& style, vector<unsigned char>& retPngBinary, string& error);

		//for exact,discrete,linear
		template<typename T>
		bool innerData2RGBAByPeStyle( T* dataPtr, int width, int height, int nbands, PeStyle& style, vector<unsigned char>& retRGBAData,string& retLogStr);
		//for gray,rgb,rgba
		template<typename T>
		bool innerData2RGBAByPeStyle2(T* dataPtr, int width, int height, int nbands, PeStyle& style, vector<unsigned char>& retRGBAData,string& retLogStr);

		bool innerRenderTileDataWithoutStyle(PeTileData& tileData, vector<unsigned char>& retPngBinary, string& error);
		template<typename T>
		void innerData2RGBAWithoutStyle(T* data, int width, int height, int nbands, vector<unsigned char>& retPngBinary );

		bool innerRGBAData2Png(vector<unsigned char>& rgbaData, int width, int height, vector<unsigned char>& retPngBinary);
		bool innerV8Dataset2TileData(Isolate* isolate, Local<Context>& context, Local<Value>& v8dsValue, PeTileData& retTileData, string& error);
		template<typename T>
		static unsigned char clamp255(T val);

		static bool innerTileOutsideRoi(PeRoi& roi,int tilez,int tiley,int tilex,
			int wid,int hei);

		template<typename T>
		static bool innerCopyRoiData(T* source,T* target,PeRoi& roi,int fillval,
			int tilez,int tiley,int tilex,int wid,int hei,int nbands) ;



public:
        //拷贝瓦片数据，使用tlv进行roi裁剪 2022-3-6
        //use for public method 2022-3-23
        template<typename T>
		static bool innerCopyRoiData2(T* source,T* target,WHsegTlvObject& roi,int fillval,
			int tilez,int tiley,int tilex,int wid,int hei,int nbands) ;

private:
        //2020-12-01 copy array data from source to target
        static bool innerCopyArrayData(void* srcDataPtr,int srcType,size_t srcElementCount, void* tarDataPtr,int tarType) ;
        template<typename T, typename U>
        static void innerCopyArrayData(T* srcDataPtr,size_t srcElementCount, U* tarDataPtr) ;


        //2022-3-26
public:
//        /// 使用 hseg.tlv 数据对瓦片数据进行区域统计 2022-3-26
//        /// 统计过程中首先判断filldata，如果是filldata那么无论是否在统计区间内都不参与统计
//        /// @retval 统计过程没有异常返回true，反之返回false，注意即使一个像素没有落在roi中只要没有异常错误仍然返回true，统计结果都是0.
//        template<typename T>
//		static bool computeStatistic(
//            T* source,            //瓦片数据二进制格式强制转换制定类型
//            WHsegTlvObject& roi , //hsegtlv格式roi对象
//            double fillval ,      //数据的填充值
//			int tilez,int tiley,int tilex, //瓦片坐标
//			int wid, int hei , int nbands, //瓦片宽，高，和波段数
//			double valMinInc, //统计区间最小值，包含
//			double valMaxInc, //统计区间最大值，包含
//			vector<WStatisticData>& retBandStatDataVec //返回统计结果，要求是个空的vec
//			) ;

    //使用 hseg.tlv 数据对瓦片数据进行区域统计 2022-3-26
    /// @retval 统计过程没有异常返回true，反之返回false，注意即使一个像素没有落在roi中只要没有异常错误仍然返回true，统计结果都是0.
    template<typename T>
    static bool computeStatistic(
        T* source,            //瓦片数据二进制格式强制转换制定类型
        WHsegTlvObject& roi , //hsegtlv格式roi对象
        double fillval ,      //数据的填充值
        int tilez,int tiley,int tilex, //瓦片坐标
        int wid, int hei , int nbands, //瓦片宽，高，和波段数
        double valMinInc, //统计区间最小值，包含
        double valMaxInc, //统计区间最大值，包含
        vector<WStatisticData>& retBandStatDataVec //返回统计结果，要求是个空的vec
        )
    {
        retBandStatDataVec.resize(nbands) ;
        for(int ib = 0;ib<nbands;++ib)
        {
            retBandStatDataVec[ib].validMin = valMaxInc;
            retBandStatDataVec[ib].validMax = valMinInc;
        }

        int bandsize = wid*hei;
        int tilefullx0 = tilex * wid;
        int tilefullx1 = tilefullx0 + wid;

        int tilefully0 = tiley * hei  ;    //inclusive
        int tilefully1 = tilefully0 + hei ;//exclusive

        if( roi.allLevelHsegs.size() == 0 ) return false ;//bad roi.hseg.tlv
        int iroilevel = 0 ;
        double hseg_scale_to_tilez = 1.0 ;
        if( tilez < roi.allLevelHsegs.size() ){
            iroilevel = tilez ;
        }else{
            iroilevel = roi.allLevelHsegs.size() - 1 ;
            for(int izzz = iroilevel ; izzz < tilez ; ++ izzz ){
                hseg_scale_to_tilez *= 2.0 ;
            }
        }

        wLevelHseg& useRoilevel = roi.allLevelHsegs[iroilevel] ;

        if( useRoilevel.hsegs.size()>0 ){
            int nseg = useRoilevel.hsegs.size();
            int roi_top_y_in_full = useRoilevel.hsegs[0].y     * hseg_scale_to_tilez;
            int roi_btm_y_in_full = useRoilevel.hsegs.back().y * hseg_scale_to_tilez;
            if( roi_top_y_in_full >= tilefully1 ){
                //outside
            }else if( roi_btm_y_in_full < tilefully0 ){
                //outside
            }
            else
            {
                WComputeLonLatArea wclla ;
                for(int iseg=0 ; iseg<nseg ; ++iseg ){
                    int seg_y0_in_full = useRoilevel.hsegs[iseg].y * hseg_scale_to_tilez ;// include
                    int seg_y1_in_full = seg_y0_in_full            + hseg_scale_to_tilez ;//not include
                    if( tilefully0 >= seg_y1_in_full ){
                        //go next seg
                    }else if( tilefully1 <= seg_y0_in_full ){
                        //go out of this tile, just go out of seg loop
                        break ;
                    }else{
                        //seg inside tile
                        for(int curr_y_in_full = seg_y0_in_full ; curr_y_in_full < seg_y1_in_full ; ++ curr_y_in_full )
                        {
                            int y_in_tile = curr_y_in_full - tilefully0 ;
                            if( y_in_tile>=0 && y_in_tile<hei )
                            {
                                //y inside
                                int seg_x0_in_tile = std::max(0  ,  (int)( useRoilevel.hsegs[iseg].x0    * hseg_scale_to_tilez - tilefullx0) ) ;// 260,261, -10 , -5
                                int seg_x1_in_tile = std::min(wid,  (int)((useRoilevel.hsegs[iseg].x1+1) * hseg_scale_to_tilez - tilefullx0) ) ;//not include
                                for(int x_in_tile = seg_x0_in_tile ; x_in_tile < seg_x1_in_tile ; ++ x_in_tile )
                                {
                                    //
                                    int it_inside_tileband = y_in_tile*wid + x_in_tile ;
                                    //x inside
                                    for(int ib=0;ib<nbands;++ib){
                                        int insideTileit=ib*bandsize + it_inside_tileband ;
                                        double val = source[insideTileit];
                                        ++ retBandStatDataVec[ib].allCnt;
                                        if( val==fillval ){
                                            ++ retBandStatDataVec[ib].fillCnt;
                                        }else if( val >= valMinInc && val<= valMaxInc ) {
                                            ++ retBandStatDataVec[ib].validCnt;
                                            retBandStatDataVec[ib].sum+=val ;
                                            retBandStatDataVec[ib].sq_sum+=val*val ;
                                            retBandStatDataVec[ib].validMin=std::min(val,retBandStatDataVec[ib].validMin);
                                            retBandStatDataVec[ib].validMax=std::max(val,retBandStatDataVec[ib].validMax);
                                            retBandStatDataVec[ib].areakm2+=wclla.computeArea(tilez,tiley,y_in_tile) ;//bugfixed
                                        }

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return true ;
    }

    protected:
        ///2022-4-1 数据类型换算字节数量
        static int datatype2bytelen(const int datatype) ;


    //2022-3-31 DatasetCollection APIs
    protected:
        /// 2022-3-31
        /// 对应js中pe.DatasetCollection("dsname",[20010101030405,...]) 的C++ 方法
        ///       pe.DatasetCollection("dsname",datetimeCollection)
        /// @retval DatasetCollection对象
        /// DatasetCollection对象的定义为
        ///
        /// PixelEngine.DatasetCollection = {
        ///   "dsname":"can be empty",
        ///   "key":"2010",//可以为空
        ///   //注意dtArr的元素数量与dataArr元素数量是一致的
        ///   "dtArr":[
        ///     20010101090909,20010102090909,...,20010131090909],
        ///   "dataArr":[
        ///     ByteArray/Int16Array/.../DoubleArray,
        ///     ByteArray/Int16Array/.../DoubleArray,
        ///     ...
        ///     ByteArray/Int16Array/.../DoubleArray
        ///   ],
        ///   "x":0 , "y":0 , "z":0 ,
        ///   "width":256, "height":256,
        ///   "nband":1,
        ///   "dataType":1/2/3/../7
        /// }
        static void GlobalFunc_DatasetCollectionCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
        /// 2022-3-31
        /// 对应js中pe.DatasetCollections("dsname",datetimeCollArray) 的C++ 方法
        /// @retval DatasetCollection Array
        static void GlobalFunc_DatasetCollectionsCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
        /// 2022-4-1 使用获取的数据够建一个DatasetCollection对象
        static Local<Object> CPP_NewDatasetCollection(
            Isolate* isolate,Local<Context>& context
            ,string dsname
            ,string key
            ,vector<int64_t>& dtArr
            ,vector< vector<unsigned char> >& dataArr
            ,const int datatype
            ,const int width
            ,const int height
            ,const int nband
            ,const int tilez,const int tiley,const int tilex
            );

        /// 2022-4-1 通过远端构建 DatetimeCollection 数组
        /// 该方法会在cpp中生成JDtCollectionBuilder类对象，然后转为Json
        /// 这个Json作为参数调用 helper接口，从外部服务进行 DatetimeCollection构建
        /// 用于构建DtCollections的辅助类
        /// public class JDtCollectionBuilder {
        /// 	//总体时间区间
        ///     public JDtPair wholePeriod ;
        ///     //重复类型，为空表示不重复，m逐月重复，y逐年重复
        ///     public String  repeatType ;
        ///     //重复时间区间
        ///     public JDtPair repeatPeriod;
        /// }
        /// public class JDtPair {
        ///     long startDt ;//开始时间
        ///     long stopDt ;//结束时间
        ///     boolean startInclusive = true ;//是否包含开始时间
        ///     boolean stopInclusive = true ;//是否包含结束时间
        ///     int stopInNextYear=0 ;//结束时间是否在下一年
        /// }
        ///
        /// 返回的 DatetimeCollection[] 与 DatetimeCollection 定义示例如下
        /// [
        /// 	{
        /// 		"key":"",
        /// 		"datetimes":[20200101235959,20200102235959,...]
        /// 	},
        /// 	...
        /// ]
        ///
        /// js调用示例
        /// const dtcollections=pe.RemoteBuildDtCollections(
        ///   dsName , //string 2022-4-3
        ///   whole_start,    //int64
        ///   whole_start_inc,//0 or 1
        ///   whole_stop,
        ///   whole_stop_inc,
        ///   repeatType,     // '' 'm' 'y'
        ///   repeat_start,   //int64
        ///   repeat_start_inc, //0 or 1
        ///   repeat_stop,      //int64
        ///   repeat_stop_inc,  //0 or 1
        ///   repeat_stop_nextyear  //0 or 1
        ///   ) ;
        static void GlobalFunc_RemoteBuildDtCollectionsCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-1 本地构造一个连续几天的 DatetimeCollection 对象
        /// 比较基础常用的一个dtcoll接口是给定基础时间，构造前N天的时间序列 js调用示例如下
        /// const dtcollection=pe.LocalBuildDtCollectionByStopDt(
        ///   2021,//stopyyyy
        ///   3,   //stopmm
        ///   31,  //stopdd
        ///   5    //n days before, e.g. this will make 3-26~3-31 total 6days
        /// );
        static void GlobalFunc_LocalBuildDtCollectionByStopDtCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-1 单个DatasetCollection合成 返回结果是一个Dataset
        /// let ds1 = PixelEngine.CompositeDsCollection(
        /// dsCollection
        /// ,method    //参考下面
        /// ,validMin
        /// ,validMax
        /// ,filldata
        /// [,outDataType]  //默认与输入数据一致
        /// ) ;
        static void GlobalFunc_CompositeDsCollectionCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;


        const static int s_CompositeMethodMin;//1
        const static int s_CompositeMethodMax;//2
        const static int s_CompositeMethodAve;//3
        const static int s_CompositeMethodSum;//4

        /// 2022-4-1 DatasetCollection[] 合成 返回结果是一个Dataset
        /// let ds2 = PixelEngine.CompositeDsCollections(
        /// dsCollectionArr
        /// ,method //pe.CompositeMethodMin 1
        ///         //pe.CompositeMethodMax 2
        ///         //pe.CompositeMethodAve 3
        ///         //pe.CompositeMethodSum 4
        /// ,validMin
        /// ,validMax
        /// ,filldata
        /// [,outDataType]  //默认与输入数据一致
        /// );
        static void GlobalFunc_CompositeDsCollectionsCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-1  Dataset 差值(A-B)合成 返回结果是一个Dataset 波段和数据类型必须一致
        /// let dsResult = datasetA.subtract( datasetB, validmin, validmax, filldata )
        static void GlobalFunc_SubtractCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-2 Dataset 提取单个波段 返回结果是一个单波段Dataset
        /// let onebandDataset = dataset.extract(iband) ;// iband is zero based.
        static void GlobalFunc_ExtractCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-2 整合多个 Dataset 反回结果是包含全部波段 Dataset 数据类型必须一致
        /// let multiDs = pe.StackDatasets( [ds0,ds1,...,dsN] ) ;//如果有一个ds为空，构造失败
        static void GlobalFunc_StackDatasetsCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

        /// 2022-4-2 两个数组求差值，结果写入 dataA，  dataA=dataA-dataB
        /// A与B的值都在valid范围内才计算，反之填充filldata
        template<typename T> static bool innerDataASubtractB(
            T* dataA,T* dataB,
            int dataElementSize,
            double validMin,double validMax,double fillval
        ){
            for(int it = 0 ; it < dataElementSize; ++ it )
            {
                if( dataA[it]>=validMin && dataA[it]<=validMax
                 && dataB[it]>=validMin && dataB[it]<=validMax )
                 {
                    dataA[it] =dataA[it] - dataB[it] ;
                 }else{
                    dataA[it] =fillval ;
                 }
            }
            return true ;
        }

public:
    //object function
    inline string getPeLog(){return pe_logs;} //2022-7-3

protected:
    //2022-7-3
    //从服务器获取指定产品的指定最近的之前日期，包括当前日期，如果有当前日期那么返回当前日期，否则找距离当前日期最近的前面日期
    // 比如数据库有 20200000... ，20210000... 如果输入 20201231... 那么返回 {'dt':20200000... ,'display':'2020年'}
    //注意返回的个对象 包括 dt:Int64 和 display:String 两个属性
    // 如果失败返回空对象null 失败包括没有对应产品，没有最近的之前日期或者输入参数错误
    //in js:
    // let dt1 = pe.NearestDatetimeBefore('mod/ndvi', pe.Datetime(2022,1,1) );
    // let dt2 = pe.NearestDatetimeBefore('mod/ndvi', 20220101000000 ) ;
    static void GlobalFunc_NearestDatetimeBeforeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //2022-7-3 与GlobalFunc_NearestDatetimeBeforeCallBack类似，但是查找的日期是指定日期之后的最近日期，注意该接口不查找当前日期
    static void GlobalFunc_NearestDatetimeAfterCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

    //2022-9-6
    /// 数据集掩摸函数
    /// datasetCollection.mask( masktiledata, filldata );
    //对 DatasetCollection 每一个dataArr中的数据数组进行掩摸，所谓掩摸就是masktiledata为1的值保留，反之使用填充值替换。
    //掩摸的结果保存在当前这个datasetCollection对象中。
    static void GlobalFunc_DsCollectionMaskCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //2022-9-6
    /// 数据集合成函数
    /// let dataset=datasetCollection.compose(method,vmininc,vmaxinc,filldata,outType);
    //对dataArr中的数据进行合成，合成结果返回一个dataset对象，默认返回类型与datasetCollection一致，除非指定类型。
    //合成方法 最大，最小，平均，求和
    /// ,method //pe.CompositeMethodMin 1
    ///         //pe.CompositeMethodMax 2
    ///         //pe.CompositeMethodAve 3
    ///         //pe.CompositeMethodSum 4
    //逻辑是先判断输入数据是否是filldata，如果不是再判断是否在[vminInc,vmaxInc]之内，如果是参与合成。
    // 对于平均合成，内部使用double进行计算，然后最后的结果仍然是当前类型或者指定类型。
    static void GlobalFunc_DsCollectionComposeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //2022-9-6
    /// 数据映射
    /// dataset.map(oldval,newval);
    /// 数据映射1 就是把特定值转换为指定值，只在替换当前对象数据，不返回新对象，对于非指定值不做操作。不变换数据类型。
    ///
    ///dataset.map2(vminInc,vmaxInc,newval);
    /// 数据映射2 就是把特定范围值转换为指定值，只在替换当前对象数据，不返回新对象，对于范围外不做操作。不变换数据类型。
    static void GlobalFunc_DsMapCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    static void GlobalFunc_DsMap2CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //2022-9-6
    /// 数据生成1,0掩摸值
    /// let maskds = dataset.buildmask( maskval );
    /// 构造掩摸数据 返回一个新建的Byte类型Dataset，原数据等于maskval返回1，否则返回0。
    ///
    /// let maskds = dataset.buildmask2(vminInc,vmaxInc);
    /// 构造掩摸数据 返回一个新建的Byte类型Dataset，且在特定范围[vmin,vmax]以内返回1，否则返回0。
    static void GlobalFunc_DsBuildMaskCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    static void GlobalFunc_DsBuildMask2CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

    //2022-9-6
    /// javascript callback, create a new empty DatasetCollection
    /// create an empty DatasetCollection.
    /// this function will call pure c++ function to create empty datasetCollection
    /// let dscoll = pe.NewDatasetCollection(datatype,width,height,nband,numdt);
    static void GlobalFunc_NewDatasetCollectionCallBack(const v8::FunctionCallbackInfo<v8::Value>& args);

    //2022-9-8
    /// 数据掩摸函数
    /// dataset.mask( masktiledata, filldata );
    //对 dataset 进行掩摸，所谓掩摸就是masktiledata为1的值保留，反之使用填充值替换。
    //掩摸的结果保存在当前这个dataset对象中。
    static void GlobalFunc_DsMaskCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

public:
    //2022-9-6
	//运行脚本scriptContent中指定函数caller() ，注意不带参数，所有参数通过pe.extraData传入。
	//运行过程中有异常返回false，反之返回true，结果保存在tileData中，logStr为日志.
	// 关于组装scriptContent的说明 >>>组装代码<<<
	// >>>pe.extraData={...};<<<  最上面插入pe.extraData
	// var sdui={...};
	// >>>sdui={...}<<<  sdui定以后插入sdui更新值
	// ...other script codes...
	// >>>var __PE__the_caller_result=caller();<<<  最下插入caller函数调用
	bool RunScriptFunctionForTileResult(
        string& scriptContent,
        string caller,
        int z, int y, int x,
        PeTileData& tileData);

    //调用脚本制定函数，可以返回字符串结果，也可以没有任何返回，如果没有任何返回resultText为空字符串.
    bool RunScriptFunctionForTextResultOrNothing(
        string& scriptContent,
        string caller,
        int z, int y, int x,  //x y z 可以给定，如果没有用给0即可。
        string& resultText);

protected:
    //1. const isok=pe.write_file(filename, textcontent);
    static void GlobalFunc_PE_Write_File_CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //2. const textOrNull=pe.read_file(filename);
    static void GlobalFunc_PE_Read_File_CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //3. const retcodeOrN9999 = pe.call_bash("some_command param1 param2...");
    static void GlobalFunc_PE_Call_Bash_CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
    //4. const isok=pe.file_exist(filename); 2022-12-11
    static void GlobalFunc_PE_File_Exist_CallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

public :
    ///2022-9-27 不依赖v8和js，直接二进制数据渲染RGBA四波段结果,如果PeStyle无效直接按0-255绘图
	/// 注意这里与之前内部渲染不同，这里的结果rgba是BSQ顺序，而老的程序是BIP顺序
    bool RenderData2RgbaByPeStyle(
        unsigned char* dataPtr,//BSQ
        int datatype,
        int wid,
        int hei,
        int nbands,
        PeStyle& style,vector<unsigned char>& rgbaData,//BSQ RGBA four bands.
        string& error) ;
    /// public method to convert four bands RGBA byte data into png binary,
    /// this is rewrap of private function innerRGBAData2Png, nothing more. 2022-9-27
    bool rgbaData2Png(
    	vector<unsigned char>& rgbaData, //BSQ
    	int width, int height,
    	vector<unsigned char>& retPngBinary);


} ;






#endif
