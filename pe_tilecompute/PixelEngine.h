//PixelEngine.h

#ifndef PIXEL_ENGINE_H

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

struct PixelEngine
{
	static vector<int> ColorRainbow  ;//1
	static vector<int> ColorBlues  ;//2
	static vector<int> ColorReds  ;//3
	static vector<int> ColorGreens  ;//4
	static vector<int> ColorGrays  ;//0 and others

	static vector<int> GetColorRamp(int colorid,int inverse=0) ;
	static void Value2Color(int valx,float K,int nodata,int* nodataColor,int vmin,int interpol,vector<int>& colorRamp,int ncolor,unsigned char& rr,unsigned char& rg,unsigned char& rb,unsigned char& ra );
	static void ColorReverse(vector<int>& colors) ;

	//PixelEngine
	static void GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//from java
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
	static bool initTemplate(PixelEngine& thePE,Isolate* isolate, Local<Context>& context );// not static


	//private method
	static Local<Object> CPP_NewDataset(Isolate* isolate,Local<Context>& context
		,const int datatype 
		,const int width 
		,const int height
		,const int nband );

	//not static
	v8::Isolate* isolate ;
	v8::Isolate::CreateParams create_params;
	Global<Context> m_context ;//need Reset


	Global<Value> GlobalFunc_ForEachPixelCallBack ;//not static, need Reset
	PixelEngine() ;//one
	~PixelEngine() ;//three
	bool RunScriptForTile( string& jsSource,int dt,int z,int y,int x, vector<unsigned char>& retbinary) ;//two

} ;





#endif