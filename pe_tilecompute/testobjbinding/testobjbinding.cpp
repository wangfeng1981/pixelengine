//Complete example of binding a C++ class with Google V8.
 


#include <v8.h>
#include <libplatform/libplatform.h>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <ctime>
#include <chrono>


using namespace v8;
using namespace std;

const int PIXELENGINE_DATETIME_CURRENT = -1 ;
vector<short> g_tempdata ;


void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  Local<Value> arg = args[0];
  String::Utf8Value value(isolate, arg);
  printf("V8Logged: %s\n", *value ) ;
}


// iband , vmin , vmax , nodata , nodataColor[0,0,0,0]
void Dataset_RenderGray_Callback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() != 5 ) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  Local<Context> context( isolate->GetCurrentContext() ) ;

  Local<Value> v8_iband = args[0];
  Local<Value> v8_vmin = args[1] ;
  Local<Value> v8_vmax = args[2] ;
  Local<Value> v8_nodata = args[3] ;
  Local<Value> v8_nodatacolor = args[4] ;

  int iband = v8_iband->ToInteger(context).ToLocalChecked()->Value() ;
  int vmin = v8_vmin->ToInteger(context).ToLocalChecked()->Value() ;
  int vmax = v8_vmax->ToInteger(context).ToLocalChecked()->Value() ;
  int nodata = v8_nodata->ToInteger(context).ToLocalChecked()->Value() ;

  int nodataColor[] = {0,0,0,0} ;
  nodataColor[0] = v8_nodatacolor->ToObject(context).ToLocalChecked()->Get(context,0).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
  nodataColor[1] = v8_nodatacolor->ToObject(context).ToLocalChecked()->Get(context,1).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
  nodataColor[2] = v8_nodatacolor->ToObject(context).ToLocalChecked()->Get(context,2).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;
  nodataColor[3] = v8_nodatacolor->ToObject(context).ToLocalChecked()->Get(context,3).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value() ;

  cout<<"iband "<<iband<<endl ;
  cout<<"vmin "<<vmin<<endl; 
  cout<<"vmax "<<vmax<<endl ;
  cout<<"nodata "<<nodata<<endl ;
  cout<<"ndcolor "<<nodataColor[0] << " "<<nodataColor[1]<<" "<<nodataColor[2]<<" "<<nodataColor[3]<<endl ;

}

struct PEDataset
{
	int dataType ;//1 - byte , 3 - short
	int width ;
	int height ;
	vector<unsigned char> tiledata ;


} ;

// PixelEngine.Dataset( strTableName, intDatetime, bandindices ) ;
// e.g. PixelEngine.Dataset("fy3d" , 20190601 , [0,1,2,3] );
// e.g. PixelEngine.Dataset("fy3d" , PixelEngine.Current() , [0,1,2,3] );
// PixelEngine.Dataset( strTableName ) ;// get current datetime with all bands
void DatasetCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() == 1 || args.Length() == 3 )
	{//ok
		Isolate* isolate = args.GetIsolate();
		HandleScope scope(isolate);
		Local<Context> context( isolate->GetCurrentContext() );
		//v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr);
		//Context::Scope context_scope(context);

		Local<Value> arg = args[0];
		String::Utf8Value value(isolate, arg);
		//printf("V8Logged: PixelEngine.Dataset(%s) is called. \n", *value ) ;
		//cout<<"tempdata ok"<<endl; 

		//attach tiledata to Dataset
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New(isolate, g_tempdata.data() , g_tempdata.size() * sizeof(short));
		Local<Int16Array>  shtarr  = Int16Array::New(arrbuff, 0, g_tempdata.size());
		Local<Object> dsobj = Object::New( isolate ) ;
		dsobj->CreateDataProperty( isolate->GetCurrentContext() ,
			String::NewFromUtf8(isolate, "tiledata").ToLocalChecked() ,
            shtarr);

		//attach forEachPixel function object to Dataset
		v8::Local<v8::Value> forPixelFuncObj = context->Global()->Get(context ,
			 v8::String::NewFromUtf8( isolate, "forEachPixelFunction").ToLocalChecked()).ToLocalChecked();
		Maybe<bool> attachPixelFuncOk = dsobj->Set( isolate->GetCurrentContext() ,
			String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
			forPixelFuncObj  ) ;
		if( attachPixelFuncOk.IsNothing() )
		{
			cout<<"failed to attach forEachPixel."<<endl;
		}

		//printf("set ok.\n") ;
  		args.GetReturnValue().Set(dsobj);

	}else
	{
		Isolate* isolate = args.GetIsolate();
		HandleScope scope(isolate);
		Local<Context> context( isolate->GetCurrentContext() );

		//attach tiledata
		Local<Object> dsobj = Object::New( isolate ) ;
		dsobj->CreateDataProperty( isolate->GetCurrentContext() ,
			String::NewFromUtf8(isolate, "tiledata").ToLocalChecked() ,
            Null(isolate) );

		//attach forEachPixel function object to Dataset
		v8::Local<v8::Value> forPixelFuncObj = context->Global()->Get(context ,
			 v8::String::NewFromUtf8( isolate, "forEachPixelFunction").ToLocalChecked()).ToLocalChecked();
		Maybe<bool> attachPixelFuncOk = dsobj->Set( isolate->GetCurrentContext() ,
			String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
			forPixelFuncObj  ) ;
		if( attachPixelFuncOk.IsNothing() )
		{
			cout<<"failed to attach forEachPixel."<<endl;
		}

		//attach RenderGray function
  		MaybeLocal<Function> renderGrayFunc = Function::New( context ,Dataset_RenderGray_Callback ) ;
  		Maybe<bool> renderGrayOk = dsobj->Set( isolate->GetCurrentContext(),
  				String::NewFromUtf8( isolate, "RenderGray").ToLocalChecked() , 
  				renderGrayFunc.ToLocalChecked() );//  
		if( renderGrayOk.IsNothing() )
		{
			cout<<"failed to attach RenderGray."<<endl;
		}

		//printf("set ok.\n") ;
  		args.GetReturnValue().Set(dsobj);
	}
}


struct PixelEngine 
{
	inline PixelEngine(Isolate* isolate): m_isolate(isolate) {} 
    inline Isolate* GetIsolate() { return m_isolate; }
	Isolate* m_isolate;
	inline ~PixelEngine(){ this->m_context.Reset(); };
	Global<Context> m_context ;
	bool Initialize();
	bool ExecuteScript(string cscript) ;
} ;

bool PixelEngine::Initialize()
{
	HandleScope handle_scope(GetIsolate());//make handle scope
	Local<ObjectTemplate> global = ObjectTemplate::New(GetIsolate());//make a global object in js
  	global->Set(String::NewFromUtf8(GetIsolate(), "log").ToLocalChecked() ,
              FunctionTemplate::New(GetIsolate(), LogCallback));// global object bind cpp function to js 'log' function.


  	Local<ObjectTemplate> pixelengineObj = ObjectTemplate::New( GetIsolate() ) ;
  	pixelengineObj->Set(String::NewFromUtf8(GetIsolate(), "log").ToLocalChecked() ,
              FunctionTemplate::New(GetIsolate(), LogCallback));
  	pixelengineObj->Set(String::NewFromUtf8(GetIsolate(), "Dataset").ToLocalChecked() ,
              FunctionTemplate::New(GetIsolate(), DatasetCallback));
  	// set ( name , value )
	global->Set(String::NewFromUtf8(GetIsolate(), "PixelEngine").ToLocalChecked() ,
              pixelengineObj );


	v8::Local<v8::Context> context = Context::New(GetIsolate(), NULL, global);//make local context and pass in global object
	//m_context.Reset(GetIsolate(), context);//persisit local context
	 
	Context::Scope context_scope(context);//switch to this context do something.

	//forEachPixel function
	v8::Local<v8::String> sourceForEachPixel = v8::String::NewFromUtf8(GetIsolate(), "var forEachPixelFunction=function( pixelFunction ){log('inside forEachPixelFunction');var nband = 5 ;var width = 1024 ;var height = 1024 ;var asize = width*height ;var pxvalues = new Int16Array(nband) ;var outarr = new Int16Array(width*height) ;for(var i = 0 ; i<asize ; ++i  ){for(var ib = 0 ; ib <nband ; ++ib ){pxvalues[ib] = this.tiledata[i*nband+ib] ;}outarr[i] = pixelFunction( pxvalues , i );} var dsout = PixelEngine.Dataset() ; dsout.tiledata = outarr ; return dsout ;}").ToLocalChecked();
    v8::Local<v8::Script> scriptForEachPixel = v8::Script::Compile(context, sourceForEachPixel).ToLocalChecked();
    v8::TryCatch tryCatch(GetIsolate());
    v8::MaybeLocal<v8::Value> resultForEachPixel = scriptForEachPixel->Run(context);
    if (resultForEachPixel.IsEmpty()) {
        v8::String::Utf8Value e(GetIsolate(), tryCatch.Exception());
        std::cerr << "Exception: " << *e << std::endl;
        return false ;
    } else {
        v8::String::Utf8Value r(GetIsolate(), resultForEachPixel.ToLocalChecked());
        std::cout << *r << std::endl;
    }
    v8::Local<v8::Value> forPixelFuncObj = context->Global()->Get(context , v8::String::NewFromUtf8(GetIsolate(), "forEachPixelFunction").ToLocalChecked()).ToLocalChecked();
    m_context.Reset(GetIsolate(), context);//persisit local context

	return true ;
}

bool PixelEngine::ExecuteScript( string cscript ) {
  HandleScope handle_scope(GetIsolate());
  TryCatch try_catch(GetIsolate());
  //Local<Context> context = Context::New( GetIsolate() );
  Local<Context> context = Local<Context>::New( GetIsolate(), m_context);
  Context::Scope context_scope(context);	
 //var ds1 = PixelEngine.Dataset(\"fy3d\"); log(ds1.tiledata[999]); 
  //string ss2 = "log(\"hello\"); PixelEngine.log(\"this is pixelengine.\");var ds1 = PixelEngine.Dataset(\"fy3d\");  log(ds1.tiledata[999]); " ;
  Local<String> script =  String::NewFromUtf8( GetIsolate() , cscript.c_str(),
					 NewStringType::kNormal).ToLocalChecked();
  Local<Script> compiled_script;
  // Compile the script and check for errors.
  if (!Script::Compile(context, script).ToLocal(&compiled_script)) {
    String::Utf8Value error(GetIsolate(), try_catch.Exception());
    cout<< *error<<endl ;
    // The script failed to compile; bail out.
    return false;
  }

  // Run the script!
  Local<Value> result;
  if (!compiled_script->Run(context).ToLocal(&result)) {
    // The TryCatch above is still in effect and will have caught the error.
    String::Utf8Value error(GetIsolate(), try_catch.Exception());
    //Log(*error);
    cout<< *error<<endl ;
    // Running the script failed; bail out.
    return false;
  }
  String::Utf8Value res(GetIsolate(), result);
  cout<< "script result: " << *res <<endl ;

  return true;
}




int main()
{
	const int testDataShortSize = 512*512*6 ;
	const int testDataByteSize = testDataShortSize*2 ;
	
	//load test data fy3d 512x512x6 short , BIP order
	{
		vector<short> testTileData( testDataShortSize ) ;
		FILE* pf = fopen("/home/hadoop/tempdata/fy3d512bip","rb") ;
		if( pf==0)
		{
			cout<<"error : failed to open test file."<<endl ;
			return 1;
		}
		fread( testTileData.data() , 1 , testDataByteSize , pf ) ;
		fclose(pf) ;
		g_tempdata.resize( testDataShortSize ) ;
		for(int i = 0 ; i<testDataShortSize ; ++ i )
		{
			g_tempdata[i] = testTileData[i] ;
		}
		cout<<"load data ok."<<endl ;
	}

	std::string js_ndviscript =  R"(
		function pemain() {
			var ndvifunction = function(pxvalues , index )
			{
				var ndvi = 0.0 ;
				if( pxvalues[0] > 0 )
				{
					ndvi = (pxvalues[3]-pxvalues[2])/(pxvalues[3]+pxvalues[2])*10000 ;
				}else
				{
					ndvi = -9999 ;
				}
				return ndvi ;
			};

			var fy3ds = PixelEngine.Dataset("fy3d") ;
			var ndvids = fy3ds.forEachPixel( ndvifunction ) ;
			return ndvids.RenderGray(0 , 0 , 10000 , -9999 , [0,0,0,255]) ;
		}
		pemain() ;
		)" ;

	v8::V8::InitializeICUDefaultLocation(".");
	v8::V8::InitializeExternalStartupData(".");
	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	Isolate::CreateParams create_params;
	create_params.array_buffer_allocator =
	  v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	Isolate* isolate = Isolate::New(create_params);
	Isolate::Scope isolate_scope(isolate);
	HandleScope scope(isolate);

	PixelEngine pe(isolate) ;
	pe.Initialize() ;

	for(int i = 0 ; i<1 ; ++ i )
	{
		unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		pe.ExecuteScript( js_ndviscript) ;
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();			
		cout<<"dura2 : "<<now1-now<<endl ;
	}

	 
	return 0;
}

