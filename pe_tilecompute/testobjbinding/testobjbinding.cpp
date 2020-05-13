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

void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  Local<Value> arg = args[0];
  String::Utf8Value value(isolate, arg);
  printf("V8Logged: %s\n", *value ) ;

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
vector<short> g_tempdata(1024*1024*5) ;
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
		return ;
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
	v8::Local<v8::String> sourceForEachPixel = v8::String::NewFromUtf8(GetIsolate(), "var forEachPixelFunction=function( pixelFunction ){log('inside forEachPixelFunction');var nband = 5 ;var width = 1024 ;var height = 1024 ;var asize = width*height ;var pxvalues = new Int16Array(nband) ;var outarr = new Int16Array(width*height) ;for(var i = 0 ; i<asize ; ++i  ){for(var ib = 0 ; ib <nband ; ++ib ){pxvalues[ib] = this.tiledata[i*nband+ib] ;}outarr[i] = pixelFunction( pxvalues , i );}return outarr ;}").ToLocalChecked();
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
  cout<< *res <<endl ;

  return true;
}




int main()
{
	std::string js_source =  R"(
		log("hello"); 
		PixelEngine.log("this is pixelengine.");
		var ds1 = PixelEngine.Dataset("fy3d");
		var sum = 0 ;
		var pixelfunction = function(pxval,index){var t2=0; if(pxval[0]>255){ t2=(pxval[0]/Math.cos(0.5) -500)/(pxval[0]/Math.cos(0.5)+500) ;}else{ t2=(pxval[0]/Math.cos(0.6) -500)/(pxval[0]/Math.cos(0.6)+500) ;}   return t2; }
		var pixelfunction2 = function(pxval,index){var t2=0; if(pxval[0]>255){ t2=(pxval[0]/Math.cos(0.5) -500)/(pxval[0]/Math.cos(0.5)+500) ;}else{ t2=(pxval[0]/Math.cos(0.6) -500)/(pxval[0]/Math.cos(0.6)+500) ;}   return t2+pxval[0]; }
		var tdata = ds1.tiledata ;
		var outdata0 = new Int16Array(1024*1024);
		var pxvals = new Int16Array(5);
		for(var i = 0 ; i<1024*1024;++i )
		{
			for( var ib = 0 ; ib<5 ; ++ ib )
			{
				pxvals[ib] = tdata[i*5+ib];
			}
			outdata0[i] = pixelfunction2( pxvals ,i) ;
		}
		log('outdata0:' + outdata0[1]) ;
		 )" ;

	string js_source2 = R"(
		log("hello"); 
		PixelEngine.log("this is pixelengine. test2");
		var ds1 = PixelEngine.Dataset("fy3d");
		var sum = 0 ;
		var pixelfunction = function(pxval,index){var t2=0; if(pxval[0]>255){ t2=(pxval[0]/Math.cos(0.5) -500)/(pxval[0]/Math.cos(0.5)+500) ;}else{ t2=(pxval[0]/Math.cos(0.6) -500)/(pxval[0]/Math.cos(0.6)+500) ;}   return t2; }
		var pixelfunction2 = function(pxval,index){var t2=0; if(pxval[0]>255){ t2=(pxval[0]/Math.cos(0.5) -500)/(pxval[0]/Math.cos(0.5)+500) ;}else{ t2=(pxval[0]/Math.cos(0.6) -500)/(pxval[0]/Math.cos(0.6)+500) ;}   return t2+pxval[0]; }
		var sum2 = 0 ;
		var outdata2 = ds1.forEachPixel( pixelfunction2 ) ;
		log('outdata2:' + outdata2[1]) ;
		 )" ;

	for(int i = 0 ; i< g_tempdata.size() ; ++ i )
	{
		g_tempdata[i] = i % 1024 ;
	}

		//function pixelfunction(pxval){var t2=0; if(pxval>255){ t2=(pxval/Math.cos(0.5) -500)/(pxval/Math.cos(0.5)+500) ;}else{ t2=(pxval/Math.cos(0.6) -500)/(pxval/Math.cos(0.6)+500) ;}   return t2; }
  
	

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

	
	//String::New(js_source.c_str(), js_source.size());
	Local<String> source = String::NewFromUtf8( isolate, js_source.c_str()).ToLocalChecked();

	PixelEngine pe(isolate) ;
	pe.Initialize() ;

	for(int i = 0 ; i<5 ; ++ i )
	{
		unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		pe.ExecuteScript( js_source2) ;
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();			
		cout<<"dura2 : "<<now1-now<<endl ;
	}

	for(int i = 0 ; i<5 ; ++ i )
	{
		unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		pe.ExecuteScript( js_source) ;
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();			
		cout<<"dura : "<<now1-now<<endl ;
	}


	

	 
	return 0;
}

