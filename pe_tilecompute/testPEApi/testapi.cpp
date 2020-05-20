//Complete example of binding a C++ class with Google V8.

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

//PixelEngine
extern void GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ; 
extern void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

//Dataset methods:
Global<Value> GlobalFunc_ForEachPixelCallBack ;
extern void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
extern void GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
extern void GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
extern void GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;

//global methods:
extern void GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) ;
//extern void GlobalFunc_ForEachPixelCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//in JavaScript


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
} ;

void PixelEngine::ColorReverse(vector<int>& colors) 
{
	int nc = colors.size()/3 ;
	int hnc = nc/2 ;
	int r ,g , b ;
	for(int i = 0 ; i<hnc ; ++ i )
	{
		r = colors[i*3] ;
		g = colors[i*3+1] ;
		b = colors[i*3+2] ;
		colors[i*3] = colors[(nc-1-i)*3] ;
		colors[i*3+1] = colors[(nc-1-i)*3+1] ;
		colors[i*3+2] = colors[(nc-1-i)*3+2] ;
		colors[(nc-1-i)*3]=r ;
		colors[(nc-1-i)*3+1]=g;
		colors[(nc-1-i)*3+2]=b ;
	}
}

vector<int> PixelEngine::GetColorRamp(int colorid,int inverse) 
{
	if( colorid==1 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorRainbow ;
		}else
		{
			vector<int> v = PixelEngine::ColorRainbow ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==2 )
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorBlues ;
		}else
		{
			vector<int> v = PixelEngine::ColorBlues ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==3 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorReds ;
		}else
		{
			vector<int> v = PixelEngine::ColorReds ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==4 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorGreens ;
		}else
		{
			vector<int> v = PixelEngine::ColorGreens ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorGrays ;
		}else
		{
			vector<int> v = PixelEngine::ColorGrays ;
			PixelEngine::ColorReverse(v);
			return v ;
		}
	}
}

vector<int> PixelEngine::ColorRainbow{
	215,25,28
	,232,91,58
	,249,158,89
	,254,201,128
	,255,237,170
	,237,248,185
	,199,233,173
	,157,211,167
	,100,171,176
	,43,131,186
} ;

vector<int> PixelEngine::ColorBlues
{
	 247,251,255
	,226,238,249
	,205,224,242
	,176,210,232
	,137,191,221
	,96,166,210
	,62,142,196
	,33,114,182
	,10,84,158
	,8,48,107
} ;

vector<int> PixelEngine::ColorReds{
	255, 245, 240
	,254, 227, 214
	,253, 198, 175
	,252, 164, 134
	,252,146,114
	,251,106,74
	,239,59,44
	,203,24,29
	,165,15,21
	,103,0,13
} ;
vector<int> PixelEngine::ColorGreens{
	 247,252,245
	,232,246,227
	,208,237,202
	,178,224,171
	,142,209,140
	,102,189,111
	,61,167,90
	,35,140,69
	,3,112,46
	,0,68,27
} ;
vector<int> PixelEngine::ColorGrays{
	250,250,250
	,223,223,223
	,196,196,196
	,168,168,168
	,141,141,141
	,114,114,114
	,87,87,87
	,59,59,59
	,32,32,32
	,5,5,5

} ;


// global->Set(String::NewFromUtf8Literal(GetIsolate(), "log"),
//             FunctionTemplate::New(GetIsolate(), LogCallback));

void GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if (args.Length() < 1) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  Local<Value> arg = args[0];
  String::Utf8Value value(isolate, arg);
  cout<<"log: "<< *value <<endl;;
}


Local<Object> CPP_NewDataset(Isolate* isolate,Local<Context>& context
	,const int datatype 
	,const int width 
	,const int height
	,const int nband )
{
	cout<<"inside CPP_NewDataset"<<endl; 
	v8::EscapableHandleScope handle_scope(isolate);

	Local<Object> ds = Object::New(isolate) ;
	Local<Value> forEachFunc = GlobalFunc_ForEachPixelCallBack.Get(isolate) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFunc );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderGray").ToLocalChecked(),
            FunctionTemplate::New(isolate, GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderPsuedColor").ToLocalChecked(),
            FunctionTemplate::New(isolate, GlobalFunc_RenderPsuedColorCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "fillRange").ToLocalChecked(),
            FunctionTemplate::New(isolate, GlobalFunc_FillRangeCallBack)->GetFunction(context).ToLocalChecked() );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderRGB").ToLocalChecked(),
            FunctionTemplate::New(isolate, GlobalFunc_RenderRGBCallBack)->GetFunction(context).ToLocalChecked() );
	

	ds->Set(context
		,String::NewFromUtf8(isolate, "width").ToLocalChecked()
		,Integer::New(isolate,width) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "height").ToLocalChecked()
		,Integer::New(isolate,height) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "nband").ToLocalChecked()
		,Integer::New(isolate,nband) ) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "dataType").ToLocalChecked()
		,Integer::New(isolate,datatype) ) ;

	if( datatype == 3 )
	{//short
		int bsize = width*height*nband*2 ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Int16Array> i16array = Int16Array::New(arrbuff,0,bsize/2) ;
		ds->Set(context
			,String::NewFromUtf8(isolate, "tiledata").ToLocalChecked()
			,i16array ) ;
	}else
	{//byte
		int bsize = width*height*nband*2 ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate,bsize) ;
		Local<Uint8Array> u8array = Uint8Array::New(arrbuff,0,bsize) ;
		ds->Set(context
			,String::NewFromUtf8(isolate, "tiledata").ToLocalChecked()
			,u8array ) ;
	}


	return handle_scope.Escape(ds);
}

//iband,vmin,vmax,nodata,nodataColor
void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderGrayCallBack"<<endl; 
	if (args.Length() != 5 ){
		cout<<"Error: args.Length != 5 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;
	Local<Value> v8_nodata = args[3] ;
	Local<Value> v8_nodatacolor = args[4] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	int nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Object> nodataColorObj = v8_nodatacolor->ToObject(context).ToLocalChecked() ;
	int nodataColor[] = {0,0,0,0} ;
	nodataColor[0] = nodataColorObj->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[1] = nodataColorObj->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[2] = nodataColorObj->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[3] = nodataColorObj->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;


	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;


	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = 255.f/(vmax-vmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		
		for(int it = 0 ; it < asize ; ++ it )
		{
			if( backDataOffset[it] == nodata ){
				outbackData[it] = nodataColor[0] ;
				outbackData[asize+it] = nodataColor[1] ;
				outbackData[asize*2+it] = nodataColor[2] ;
				outbackData[asize*3+it] = nodataColor[3] ;
			}else
			{
				int gray = (backDataOffset[it]-vmin) * theK ;
				if( gray < 0 ) gray = 0 ;
				else if( gray > 255 ) gray = 255 ;
				outbackData[it] = gray ;
				outbackData[asize+it] = gray;
				outbackData[asize*2+it] = gray ;
				outbackData[asize*3+it] = 255;
			}
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
	
		for(int it = 0 ; it < asize ; ++ it )
		{
			if( backDataOffset[it] == nodata ){
				outbackData[it] = nodataColor[0] ;
				outbackData[asize+it] = nodataColor[1] ;
				outbackData[asize*2+it] = nodataColor[2] ;
				outbackData[asize*3+it] = nodataColor[3] ;
			}else
			{
				int gray = (backDataOffset[it]-vmin) * theK ;
				if( gray < 0 ) gray = 0 ;
				else if( gray > 255 ) gray = 255 ;
				outbackData[it] = gray ;
				outbackData[asize+it] = gray;
				outbackData[asize*2+it] = gray ;
				outbackData[asize*3+it] = 255;
			}
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}


//ri,gi,bi,rmin,rmax,gmin,gmax,bmin,bmax
void GlobalFunc_RenderRGBCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderRGBCallBack"<<endl; 
	if (args.Length() != 9 ){
		cout<<"Error: args.Length != 9 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	int ri = args[0]->ToInteger(context).ToLocalChecked()->Value();
	int gi = args[1]->ToInteger(context).ToLocalChecked()->Value();
	int bi = args[2]->ToInteger(context).ToLocalChecked()->Value();

	int rmin = args[3]->ToInteger(context).ToLocalChecked()->Value();
	int rmax = args[4]->ToInteger(context).ToLocalChecked()->Value();

	int gmin = args[5]->ToInteger(context).ToLocalChecked()->Value();
	int gmax = args[6]->ToInteger(context).ToLocalChecked()->Value();

	int bmin = args[7]->ToInteger(context).ToLocalChecked()->Value();
	int bmax = args[8]->ToInteger(context).ToLocalChecked()->Value();

	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;

	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float rK = 255.f/(rmax-rmin) ;
	float gK = 255.f/(gmax-gmin) ;
	float bK = 255.f/(bmax-bmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* roffset = backData + ri * asize;
		short* goffset = backData + gi * asize;
		short* boffset = backData + bi * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			outbackData[it] = min( max( (roffset[it]-rmin) * rK,0.f) ,255.f)  ;
			outbackData[asize+it] = min( max( (goffset[it]-gmin) * gK,0.f) ,255.f) ;  
			outbackData[asize*2+it] = min( max( (boffset[it]-bmin) * bK,0.f) ,255.f) ; 
			outbackData[asize*3+it] = 255 ;
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* roffset = backData + ri * asize;
		unsigned char* goffset = backData + gi * asize;
		unsigned char* boffset = backData + bi * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			outbackData[it] = min( max( (roffset[it]-rmin) * rK,0.f) ,255.f)  ;
			outbackData[asize+it] = min( max( (goffset[it]-gmin) * gK,0.f) ,255.f) ;  
			outbackData[asize*2+it] = min( max( (boffset[it]-bmin) * bK,0.f) ,255.f) ; 
			outbackData[asize*3+it] = 255 ;
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}


//fill value in the same dataset , not create a new one , no return.
//iband,vmin,vmax
void GlobalFunc_FillRangeCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_FillRangeCallBack"<<endl; 
	if (args.Length() != 3 ){
		cout<<"Error: args.Length !=2 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	
	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = (vmax-vmin)*1.f/asize ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			backDataOffset[it] = it*theK + vmin ;
		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			backDataOffset[it] = it*theK + vmin ;
		}
	}
	//info.GetReturnValue().Set(i16arr);
	//args.GetReturnValue().Set(outds) ;
}

void PixelEngine::Value2Color(int valx,float K
	,int nodata,int* nodataColor
	,int vmin , int interpol
	,vector<int>& colorRamp , int ncolor
	,unsigned char& rr 
	,unsigned char& rg 
	,unsigned char& rb 
	,unsigned char& ra ){
	if( valx == nodata ){
		rr = nodataColor[0] ;
		rg = nodataColor[1] ;
		rb = nodataColor[2] ;
		ra = nodataColor[3] ;
	}else
	{
		int dn = (valx-vmin) * K ;
		float weightHigh = (valx-vmin) * K - dn ;
		if( dn < 0 )
		{
			rr = colorRamp[0] ;
			rg =  colorRamp[1] ;
			rb =  colorRamp[2]  ;
			ra = 255;
		}
		else if( dn >= ncolor-1 ) 
		{
			int off = (ncolor-1)*3;
			rr =  colorRamp[off] ; 
			rg =  colorRamp[off+1] ;
			rb =  colorRamp[off+2] ; 
			ra = 255;

		}else
		{
			int off0 = dn*3 ;
			int off1 = off0+3 ;
			if( interpol ==1 )
			{//interpol
				float wl = 1.f-weightHigh ;
				rr = colorRamp[off0]*wl+colorRamp[off1]*weightHigh ;
				rg = colorRamp[off0+1]*wl+colorRamp[off1+1]*weightHigh ;
				rb = colorRamp[off0+2]*wl+colorRamp[off1+2]*weightHigh ; 
				ra = 255;
			}else
			{//discrete
				rr = colorRamp[off0] ;
				rg = colorRamp[off0+1];
				rb = colorRamp[off0+2] ;
				ra = 255;
			}
		}
	}
}


//iband,vmin,vmax,nodata,nodataColor,colorid,noraml/inverse(0/1),discrete/interpol(0/1)
void GlobalFunc_RenderPsuedColorCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderPsuedColorCallBack"<<endl; 
	if (args.Length() != 8 ){
		cout<<"Error: args.Length != 8 "<<endl ;
		return;
	}
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8_iband = args[0];
	Local<Value> v8_vmin = args[1] ;
	Local<Value> v8_vmax = args[2] ;
	Local<Value> v8_nodata = args[3] ;
	Local<Value> v8_nodatacolor = args[4] ;
	Local<Value> v8_colorid = args[5] ;
	Local<Value> v8_inverse = args[6] ;
	Local<Value> v8_interpol = args[7] ;

	int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
	int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
	int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
	int nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Object> nodataColorObj = v8_nodatacolor->ToObject(context).ToLocalChecked() ;
	int nodataColor[] = {0,0,0,0} ;
	nodataColor[0] = nodataColorObj->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[1] = nodataColorObj->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[2] = nodataColorObj->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	nodataColor[3] = nodataColorObj->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

	int colorid = v8_colorid->ToInteger(context).ToLocalChecked()->Value() ;
	int inverse = v8_inverse->ToInteger(context).ToLocalChecked()->Value() ;
	int interpol = v8_interpol->ToInteger(context).ToLocalChecked()->Value() ;

	vector<int> colorRamp = PixelEngine::GetColorRamp(colorid,inverse) ;
	int ncolor = colorRamp.size()/3 ;

	Local<Object> thisobj =  args.This() ;
	int thisDataType = thisobj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int width = thisobj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int height = thisobj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	int nband = thisobj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	//cout<<"thisDataType "<<thisDataType<<endl;

	//output
	Local<Object> outds = CPP_NewDataset(isolate,context
		,1
		,width
		,height
		,4 );
	Local<Value> outDataValue = outds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Uint8Array* outU8Array = Uint8Array::Cast(*outDataValue) ;
	unsigned char* outbackData = (unsigned char*) outU8Array->Buffer()->GetBackingStore()->Data() ;


	Local<Value> tiledataValue = thisobj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	int asize = width * height ;
	float theK = (ncolor-1.f)/(vmax-vmin) ;
	if( thisDataType==3 )
	{//short
		
		Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
		short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
		short* backDataOffset = backData + iband * asize;
		int tcolor[3] ;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it],theK,nodata,nodataColor
				,vmin,interpol,colorRamp,ncolor,outbackData[it],outbackData[it+asize]
				,outbackData[it+asize*2],outbackData[it+asize*3]) ;

		}
	}else
	{//byte
		Uint8Array* u8Array = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* backData = (unsigned char*) u8Array->Buffer()->GetBackingStore()->Data() ;
		unsigned char* backDataOffset = backData + iband * asize;
		for(int it = 0 ; it < asize ; ++ it )
		{
			PixelEngine::Value2Color(backDataOffset[it],theK,nodata,nodataColor
				,vmin,interpol,colorRamp,ncolor,outbackData[it],outbackData[it+asize]
				,outbackData[it+asize*2],outbackData[it+asize*3]) ;
		}
	}
	
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(outds) ;
}

//create an empty Dataset.
void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_NewDatasetCallBack"<<endl; 
	if (args.Length() != 4 ){
		cout<<"Error: args.Length != 4 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> dt0 = args[0];
	Local<Value> w0 = args[1] ;
	Local<Value> h0 = args[2] ;
	Local<Value> nb0 = args[3] ;

	int dt = dt0->ToInteger(context).ToLocalChecked()->Value() ;
	int wid = w0->ToInteger(context).ToLocalChecked()->Value() ;
	int hei = h0->ToInteger(context).ToLocalChecked()->Value() ;
	int nband = nb0->ToInteger(context).ToLocalChecked()->Value() ;

	Local<Object> ds = CPP_NewDataset( isolate
		,context
		,dt
		,wid
		,hei
		,nband );

	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}


//create a dataset from name, datetime, bands
void GlobalFunc_DatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_DatasetCallBack"<<endl; 
	if (args.Length() != 3 ){
		cout<<"Error: args.Length != 3 "<<endl ;
		return;
	}

	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context(isolate->GetCurrentContext()) ;

	Local<Value> v8name = args[0];
	Local<Value> v8datetime = args[1] ;
	Local<Value> v8bands = args[2] ;

	String::Utf8Value nameutf8( isolate , v8name) ;
	string name( *nameutf8 ) ;

	String::Utf8Value dtutf8( isolate , v8datetime) ;
	string datetime( *dtutf8 ) ;

	cout<<name<<","<<datetime<<endl ;

	if( v8bands->IsArray() )
	{
		cout<<"v8bands is array"<<endl ;
	}
	if( v8bands->IsObject() )
	{
		cout<<"v8bands is object"<<endl ;
	}
	Array* i32array = Array::Cast(*v8bands) ;
	int nband = i32array->Length() ;

	cout<<"nband "<<nband<<endl ;

	int dt = 3;
	int wid = 512 ;
	int hei = 512 ;
	int nb0 = 6 ;
	vector<unsigned char> imgdata(wid*hei*nb0*2) ;
	{
		FILE* pf = fopen("/home/hadoop/tempdata/fy3d512bsp","rb") ;
		fread( imgdata.data() , 1 , wid*hei*nb0*2 , pf) ;
		fclose(pf) ;
	}

	Local<Object> ds = CPP_NewDataset( isolate
		,context
		,dt
		,wid
		,hei
		,nband );
	Local<Value> tiledataValue = ds->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;
	Int16Array* i16Array = Int16Array::Cast(*tiledataValue) ;
	short* backData = (short*) i16Array->Buffer()->GetBackingStore()->Data() ;
	int asize = wid * hei ;
	short* indata = (short*) imgdata.data() ;
	for(int ib = 0 ; ib<nband ; ++ ib )
	{
		int ib0 = i32array->Get(context,ib).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
		short* backDataOffset = backData + ib * asize;
		cout<<"ib,ib0 "<<ib<<","<<ib0<<endl ;
		for(int it = 0 ;it<asize ; ++ it )
		{
			backDataOffset[it] = indata[ib0*asize+it] ;
		}
	}
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;
}

 

void Dataset2Png( Isolate* isolate, Local<Context>& context, Local<Value> dsValue
	, vector<unsigned char>& retpngbinary )
{
	unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	Object* dsObj = Object::Cast(*dsValue) ;
	int dt = dsObj->Get(context,
		String::NewFromUtf8(isolate,"dataType").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int width = dsObj->Get(context,
		String::NewFromUtf8(isolate,"width").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int height = dsObj->Get(context,
		String::NewFromUtf8(isolate,"height").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	int nband = dsObj->Get(context,
		String::NewFromUtf8(isolate,"nband").ToLocalChecked())
		.ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
	Local<Value> tiledataValue = dsObj->Get(context,
		String::NewFromUtf8(isolate,"tiledata").ToLocalChecked())
		.ToLocalChecked() ;

	if( dt == 1 )
	{
		Uint8Array* u8arr = Uint8Array::Cast(*tiledataValue) ;
		unsigned char* dataptr = (unsigned char*) u8arr->Buffer()->GetBackingStore()->Data() ;
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("get dataptr:%d ms \n", now1 - now);//1024*1024 use 340millisec

		const int imgsize = width * height;
		vector<unsigned char> rgbadata(imgsize * 4, 0);
		if( nband==4 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it+imgsize];
				rgbadata[it * 4 + 2] = dataptr[it+2*imgsize];
				rgbadata[it * 4 + 3] = dataptr[it+3*imgsize];
			}
		}else if(nband==3 )
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it+imgsize];
				rgbadata[it * 4 + 2] = dataptr[it+2*imgsize];
				rgbadata[it * 4 + 3] = 255;
			}
		} else
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it];
				rgbadata[it * 4 + 2] = dataptr[it];
				rgbadata[it * 4 + 3] = 255;
			}
		}
		unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("forloop png:%d ms \n", now2 - now1);//1024*1024 use 340millisec

		retpngbinary.clear();
		retpngbinary.reserve(1024*1024*4) ;
		lodepng::State state; //optionally customize this one
		state.encoder.filter_palette_zero = 0; //
		state.encoder.add_id = false; //Don't add LodePNG version chunk to save more bytes
		state.encoder.text_compression = 1; //
		state.encoder.zlibsettings.nicematch = 258; //
		state.encoder.zlibsettings.lazymatching = 1; //
		state.encoder.zlibsettings.windowsize = 512; //32768
		state.encoder.filter_strategy = LFS_ZERO;//{ LFS_ZERO, LFS_MINSUM, LFS_ENTROPY, LFS_BRUTE_FORCE };
		state.encoder.zlibsettings.minmatch = 3;
		state.encoder.zlibsettings.btype = 2;
		state.encoder.auto_convert = 0;
		unsigned error = lodepng::encode(retpngbinary, rgbadata, width, height, state);
		
		unsigned long now3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  	printf("encode png:%d ms \n", now3 - now2);//1024*1024 use 340millisec

	}
}

void initTemplate( Isolate* isolate, Local<Context>& context )
{
	v8::HandleScope handle_scope(isolate);

	Local<Object> global = context->Global() ;
	//var PixelEngine = {} ;
	Local<Object> pe = Object::New(isolate) ;

	Maybe<bool> okpe = global->Set( context
		,String::NewFromUtf8(isolate, "PixelEngine").ToLocalChecked()
		,pe ) ;// bind PixelEngine
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampRainbow").ToLocalChecked(),
	   Integer::New(isolate,1));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampBlues").ToLocalChecked(),
	   Integer::New(isolate,2));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampReds").ToLocalChecked(),
	   Integer::New(isolate,3));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGreens").ToLocalChecked(),
	   Integer::New(isolate,4));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampGrays").ToLocalChecked(),
	   Integer::New(isolate,0));
	pe->Set(context
		,String::NewFromUtf8(isolate, "NewDataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );

	pe->Set(context
		,String::NewFromUtf8(isolate, "Dataset").ToLocalChecked(),
           FunctionTemplate::New(isolate, GlobalFunc_DatasetCallBack)->GetFunction(context).ToLocalChecked() );

	//normal/inverse
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampNormal").ToLocalChecked(),
	Integer::New(isolate,0));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInverse").ToLocalChecked(),
	Integer::New(isolate,1));
	//discrete/interpol
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampDiscrete").ToLocalChecked(),
	Integer::New(isolate,0));
	pe->Set(context,String::NewFromUtf8(isolate, "ColorRampInterpolate").ToLocalChecked(),
	Integer::New(isolate,1));


	//global function log
	global->Set(context
		,String::NewFromUtf8(isolate, "log").ToLocalChecked(),
           FunctionTemplate::New(isolate, GlobalFunc_Log)->GetFunction(context).ToLocalChecked() );



	//set globalFunc_forEachPixelCallBack in javascript
	string sourceforEachPixelFunction = R"(
		var globalFunc_forEachPixelCallBack = function(pxfunc){
			log("inside globalFunc_forEachPixelCallBack") ;
			var outds = null ; 
			var outtiledata = null ;

			var width = this.width ;
			var height = this.height ;
			var asize = width*height ;
			var nband = this.nband ;
			var pxvals = new Int16Array(nband) ;
			var ib = 0 ;
			var intiledata = this.tiledata ;
			var outband = 0 ;
			for(var it = 0 ; it<asize ; ++ it )
			{
				for(ib=0;ib<nband;++ib)
				{
					pxvals[ib]=intiledata[ib*asize+it] ;
				}
				var res = pxfunc(pxvals,it) ;
				if( outtiledata==null )
				{
					outband = res.length ;
					outds = globalFunc_newDatasetCallBack(3,width,height,outband) ;
					outtiledata = outds.tiledata ;
				}
				for(ib=0;ib<outband;++ib)
				{
					outtiledata[ib*asize+it]=res[ib] ;
				}
			}
			return outds ;
		} ;
	)" ;
	v8::Local<v8::Script> scriptForEach =
          v8::Script::Compile(context
          	, String::NewFromUtf8(isolate,sourceforEachPixelFunction.c_str()).ToLocalChecked()
          	).ToLocalChecked();
    v8::Local<v8::Value> resultForEach = scriptForEach->Run(context).ToLocalChecked();
    Local<Value> forEachFuncInJs = global->Get(context 
    	,String::NewFromUtf8(isolate, "globalFunc_forEachPixelCallBack").ToLocalChecked() ).ToLocalChecked() ;
    GlobalFunc_ForEachPixelCallBack.Reset(isolate , forEachFuncInJs) ;




    //set globalFunc_newDatasetCallBack
    global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_newDatasetCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, GlobalFunc_NewDatasetCallBack)->GetFunction(context).ToLocalChecked() );





	//global function globalFunc_renderGrayCallBack
	global->Set(context
		,String::NewFromUtf8(isolate, "globalFunc_renderGrayCallBack").ToLocalChecked(),
           FunctionTemplate::New(isolate, GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );


}

 

int main(int argc, char* argv[]) {
  // Initialize V8.
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
  
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate );
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);// enter scope
    
    initTemplate(isolate,context) ;

  
     
    {
      // Create a string containing the JavaScript source code.

    	string source = R"(
    		var ds3 = PixelEngine.Dataset("fy3d","20190601" ,[0,1,2]) ;
    		ds3.renderRGB(2,1,0,0,5000,0,5000,0,5000) ;
    	)";
       

      // Compile the source code.
      v8::Local<v8::Script> script =
          v8::Script::Compile(context
          	, String::NewFromUtf8(isolate,source.c_str()).ToLocalChecked()
          	).ToLocalChecked();

	  unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
      unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  printf("dura1:%d ms \n", now1 - now);//1024*1024 use 340millisec

	  //tiledata to png
	  vector<unsigned char> retpngbinary ;
	  Dataset2Png( isolate, context, result
			, retpngbinary );


	  unsigned long now2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  printf("dura1+png:%d ms \n", now2 - now);//1024*1024 use 340millisec

	  if( retpngbinary.size() > 1 )
	  {
	  	FILE* pf = fopen("test.png" , "wb") ;
	  	fwrite(retpngbinary.data() , 1 , retpngbinary.size() , pf) ;
	  	fclose(pf) ;
	  }else
	  {
	  	cout<<"Error: retpngbinary empty"<<endl;
	  }


      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(isolate, result);
      printf("%s\n", *utf8);
    }
 	
 	GlobalFunc_ForEachPixelCallBack.Reset() ;
 
  }


  cout<<"out isolate scope."<<endl; 

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  cout<<"out isolate scope.1"<<endl; 
  v8::V8::Dispose();
  cout<<"out isolate scope.2"<<endl; 
  v8::V8::ShutdownPlatform();
  cout<<"out isolate scope.3"<<endl; 
  delete create_params.array_buffer_allocator;
  cout<<"out isolate scope.4"<<endl; 
  return 0;
}