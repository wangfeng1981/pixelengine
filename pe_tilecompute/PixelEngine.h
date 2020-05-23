//PixelEngine.h

#ifndef PIXEL_ENGINE_H

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

using namespace v8;
using namespace std;

//deprecated , use PixelEngine_GetDataFromExternal2_FunctionPointer
typedef bool (*PixelEngine_GetDataFromExternal_FunctionPointer)(
		void* pePtr,
		string ,//name
		string ,//datetime
		vector<int>& ,//bands [0,1,2] , not used actually
		vector<unsigned char>&,//return binary
		int& dt,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands );//return nbands

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
		vector<vector<unsigned char> >&,//return binary
		vector<int>& ,//return time array
		int& dataType,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands,//return nbands
		int& numds );//return number of dataset.



struct PixelEngineTileInfo
{
	int z,y,x ;
} ;

struct PixelEngine
{
	static vector<int> ColorRainbow  ;//1
	static vector<int> ColorBlues  ;//2
	static vector<int> ColorReds  ;//3
	static vector<int> ColorGreens  ;//4
	static vector<int> ColorGrays  ;//0 and others

	static vector<int> GetColorRamp(int colorid,int inverse=0) ;
	static void Value2Color(int valx,float K,int nodata,int* nodataColor,int vmin,int vmax,int interpol,vector<int>& colorRamp,int ncolor,unsigned char& rr,unsigned char& rg,unsigned char& rb,unsigned char& ra );
	static void ColorReverse(vector<int>& colors) ;

	//PixelEngine
	static void GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl
	static void GlobalFunc_DatasetArrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//load a datetime range dataset.
	static void GlobalFunc_GetTileDataCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from exteranl
	
	static void GlobalFunc_LocalDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ; 
	static void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;


	//Dataset methods:

	static void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

	//global methods:
	static void GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) ;
	static void Dataset2Png( Isolate* isolate, Local<Context>& context, Local<Value> dsValue
	, vector<unsigned char>& retpngbinary ) ;
	static bool initTemplate(PixelEngine* thePE,Isolate* isolate, Local<Context>& context );// not static
	static void initV8() ;
	static std::unique_ptr<v8::Platform> v8Platform ;

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



	//not static
	v8::Isolate* isolate ;
	v8::Isolate::CreateParams create_params;
	Global<Context> m_context ;//need Reset
	PixelEngineTileInfo tileInfo ;
	void* extraPointer ;//do not release.


	Global<Value> GlobalFunc_ForEachPixelCallBack ;//not static, need Reset
	Global<Value> GlobalFunc_GetPixelCallBack ;//not static , need reset
	PixelEngine() ;//one
	~PixelEngine() ;//three
	bool RunScriptForTile(void* extra,string& jsSource,int dt,int z,int y,int x, vector<unsigned char>& retbinary) ;//two


	static PixelEngine_GetDataFromExternal_FunctionPointer GetExternalDatasetCallBack;//will deprecated
	static PixelEngine_GetDataFromExternal2_FunctionPointer GetExternalTileDataCallBack;//will deprecated
	static PixelEngine_GetDataFromExternal2Arr_FunctionPointer GetExternalTileDataArrCallBack ;

} ;





#endif