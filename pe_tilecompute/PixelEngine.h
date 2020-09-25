//PixelEngine.h
//add Custom ColorRamp 2020-6-20
//version 2.0
// return ds , if ds has 3 bands use rgb, else use first band render out[0,255];

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
#include "lodepng.h"
#include  <algorithm>
#include "PeStyle.h"
#include "PeTileData.h"
#include "./sqlite_interface/webservice/wstringutil.h"
#include "./sqlite_interface/webservice/wTextfilereader.h"

using namespace v8;
using namespace std;


struct PixelEngineColorRamp;
using pe::PeStyle;
using pe::PeTileData;

//PixelEngineHelperInterface
struct PixelEngineHelperInterface {
	//通过pid获取瓦片数据
	//virtual bool getTileDataByPid(string& tbname,string& fami,long dt,int pid,vector<int> bandindices,int z,int y,int x,vector<unsigned char> retTileData,string& errorText);

	//通过pdtname获取瓦片数据
	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData, 
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText)=0;

	//获取时间段瓦片数据
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

	//获得颜色列表
	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText)=0;

	//get render style by id from system
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText) =0;

	//保存瓦片数据到存储设备(这个接口不应该放在PixelEngineHelper里面 2020-9-24)
	//virtual bool writeTileData(string& tb,string& fami,int64_t col,int pid,int z,int y,int x, PeTileData& tileData) = 0;

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
	static void GlobalFunc_DatasetArrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//load a datetime range dataset.
	static void GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl
	
	static void GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ; 
	static void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	//2020-9-13
	static void GlobalFunc_GetStyleCallBack(const v8::FunctionCallbackInfo<v8::Value>& args);


	//Dataset methods:

	static void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_ColorRampCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

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
 


	//private method
	static Local<Object> CPP_NewDataset(Isolate* isolate,Local<Context>& context
		,const int datatype 
		,const int width 
		,const int height
		,const int nband );
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


	Global<Value> GlobalFunc_ForEachPixelCallBack ;//not static, need Reset
	Global<Value> GlobalFunc_GetPixelCallBack ;//not static , need reset
	PixelEngine() ;//one
	~PixelEngine() ;//three

	/// 老版本瓦片计算，为了保证以前业务可用保留，后续开发不再调用
	bool RunScriptForTile(void* extra,string& jsSource,long dt,int z,int y,int x, vector<unsigned char>& retbinary) ;

	/// 老版本瓦片计算，为了保证以前业务可用保留，后续开发不再调用
	bool RunScriptForComputeOnce(void* extra, string& jsSource,long currentdt
                                            ,int z,int y,int x, string& retJsonStr ) ;

	/// 检查脚本是否有语法错误
	string CheckScriptOk(string& scriptSource) ;
	//2020-9-13 get style from script 从脚本获取PeStyle对象
	bool RunToGetStyleFromScript(string& scriptContent, PeStyle& retstyle, string& retLogText);
	//2020-9-13
	//运行脚本保留数据，不渲染
	bool RunScriptForTileWithoutRender(void* extra, string& scriptContent, int64_t currentDatetime,
		int z, int y, int x, PeTileData& tileData , string& logStr);
	//运行脚本并渲染png图片，PeStyle从外部传入
	bool RunScriptForTileWithRender(void* extra, string& scriptContent, PeStyle& inStyle, int64_t currentDatetime,
		int z, int y, int x, vector<unsigned char>& retPngBinary, string& logStr);//
	//使用esprima解析脚本生成AST json对象 2020-9-19
	bool RunScriptForAST(void* extra, string& scriptContent, string& retJsonStr, string& errorText);



	//static PixelEngine_GetDataFromExternal_FunctionPointer GetExternalDatasetCallBack;//will deprecated
	static PixelEngine_GetDataFromExternal2_FunctionPointer GetExternalTileDataCallBack; 
	static PixelEngine_GetDataFromExternal2Arr_FunctionPointer GetExternalTileDataArrCallBack ;
	static PixelEngine_GetColorRampFromExternal_FunctionPointer GetExternalColorRampCallBack ;


	//2020-9-12
	PixelEngineHelperInterface* helperPointer;

	//2020-9-13
	static PixelEngine* getPixelEnginePointer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static string convertV8LocalValue2CppString(Isolate* isolate, Local<Value>& v8value);
	static Local<Value> warpCppStyle2V8Object(Isolate* isolate, PeStyle& style);

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
} ;






#endif