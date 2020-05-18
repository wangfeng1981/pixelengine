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

using namespace v8;
using namespace std;


 
Global<ObjectTemplate> PEDatasetTemplate ;
Global<Value> PEDatasetForEachPixelFunction;

Global<Value> GlobalFunc_ForEachPixelCallBack ;
extern void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;
extern void GlobalFunc_Log(const v8::FunctionCallbackInfo<v8::Value>& args) ;
//extern void GlobalFunc_ForEachPixelCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) ;//in JavaScript
 

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

 

void Dataset2Png( Isolate* isolate, Local<Context>& context, Local<Value> dsValue
	, vector<unsigned char>& retpngbinary )
{
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
		}else
		{
			for (int it = 0; it < imgsize; ++it)
			{
				rgbadata[it * 4 + 0] = dataptr[it];
				rgbadata[it * 4 + 1] = dataptr[it];
				rgbadata[it * 4 + 2] = dataptr[it];
				rgbadata[it * 4 + 3] = 255;
			}
		}
		retpngbinary.clear();
		lodepng::State state; //optionally customize this one
		state.encoder.filter_palette_zero = 0; //
		state.encoder.add_id = false; //Don't add LodePNG version chunk to save more bytes
		state.encoder.text_compression = 1; //
		state.encoder.zlibsettings.nicematch = 258; //
		state.encoder.zlibsettings.lazymatching = 1; //
		state.encoder.zlibsettings.windowsize = 4096; //32768
		state.encoder.filter_strategy = LFS_ZERO;//{ LFS_ZERO, LFS_MINSUM, LFS_ENTROPY, LFS_BRUTE_FORCE };
		state.encoder.zlibsettings.minmatch = 3;
		state.encoder.zlibsettings.btype = 2;
		state.encoder.auto_convert = 0;
		unsigned error = lodepng::encode(retpngbinary, rgbadata, width, height, state);
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
    		var ds0 = globalFunc_newDatasetCallBack(3,256,256,6) ;
    		var ds1 = ds0.forEachPixel(function(pxvals,index){return [pxvals[0]+50,pxvals[1]+100,pxvals[2]+150];}) ;
    		ds1.renderGray(2,0,255,-1,[0,0,0,0]) ;
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