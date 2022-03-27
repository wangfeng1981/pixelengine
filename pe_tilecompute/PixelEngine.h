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
#include "lodepng.h"
#include  <algorithm>
#include "PeStyle.h"
#include "PeTileData.h"
#include "wstringutil.h"
#include "wstringutils.h" //2022-3-6
#include "wTextfilereader.h"
#include "wDatasetDatetime.h"
#include "wAST.h"
#include "pemultipolygon.h"//2020-10-15
#include "peroi.h"
#include <memory>
#include "ajson5.h"
#include "esprimacpptool.h"//2022-2-12
#include "whsegtlvobject.h"//2022-3-6
#include "wstatisticdata.h"//2022-3-26

//2022-3-26
#include "wcomputelonlatarea.h"
#define W_COMPUTE_LON_LAT_AREA_IMPLEMENT
#include "wcomputelonlatarea.h"


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
	//Í¨¹ýpid»ñÈ¡ÍßÆ¬Êý¾Ý
	//virtual bool getTileDataByPid(string& tbname,string& fami,long dt,int pid,vector<int> bandindices,int z,int y,int x,vector<unsigned char> retTileData,string& errorText);

	//Í¨¹ýpdtname»ñÈ¡ÍßÆ¬Êý¾Ý
	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData,
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText)=0;

	//»ñÈ¡Ê±¼ä¶ÎÍßÆ¬Êý¾Ý
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

	//»ñµÃÑÕÉ«ÁÐ±í
	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText)=0;

	//get render style by id from system
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText) =0;

	//±£´æÍßÆ¬Êý¾Ýµ½´æ´¢Éè±¸(Õâ¸ö½Ó¿Ú²»Ó¦¸Ã·ÅÔÚPixelEngineHelperÀïÃæ 2020-9-24)
	//virtual bool writeTileData(string& tb,string& fami,int64_t col,int pid,int z,int y,int x, PeTileData& tileData) = 0;

	//2022-3-6 从外部读取roi hseg.tlv数据 isUserRoi=1为用户roi，isUserRoi=0为系统ROI，rid为关系数据库中主键
	virtual bool getRoiHsegTlv(int isUserRoi,int rid,vector<unsigned char>& retTlvData)=0 ;

	inline static string version(){ return "v2" ; }

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
    static void GlobalFunc_DatafileCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//2020-10-30
	static void GlobalFunc_DatasetArrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//load a datetime range dataset.
	static void GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl

	static void GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
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
	PixelEngine() ;//one
	~PixelEngine() ;//three


	////////////////////////////////////////////////////////////////////////////
	/// below methods would be called by outsider

	/// 第一版瓦片计算接口，为了保持so的兼容性保留该接口，后面不再使用 2021-1-21
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
	bool RunScriptForTileWithoutRender(void* extra, string& scriptContent, int64_t currentDatetime,
		int z, int y, int x, PeTileData& tileData , string& logStr);
	// 运行脚本并渲染
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



} ;






#endif
