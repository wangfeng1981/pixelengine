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


Local<Object> CPP_NewDataset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	cout<<"inside CPP_NewDataset"<<endl; 
	Isolate* isolate = args.GetIsolate() ;
	v8::EscapableHandleScope handle_scope(isolate);
	Local<Context> context( isolate->GetCurrentContext() ) ;

	Local<Object> ds = Object::New(isolate) ;

	Local<Value> forEachFunc = GlobalFunc_ForEachPixelCallBack.Get(isolate) ;
	ds->Set(context
		,String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked(),
            forEachFunc );
	ds->Set(context
		,String::NewFromUtf8(isolate, "renderGray").ToLocalChecked(),
            FunctionTemplate::New(isolate, GlobalFunc_RenderGrayCallBack)->GetFunction(context).ToLocalChecked() );
	return handle_scope.Escape(ds);
}

void GlobalFunc_RenderGrayCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_RenderGrayCallBack"<<endl; 
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);

	Local<Object> ds = CPP_NewDataset( args );
	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;

}


void GlobalFunc_NewDatasetCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	cout<<"inside GlobalFunc_NewDatasetCallBack"<<endl; 
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	 
	Local<Object> ds = CPP_NewDataset( args );

	//info.GetReturnValue().Set(i16arr);
	args.GetReturnValue().Set(ds) ;

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
			var emptyDs = globalFunc_newDatasetCallBack() ;
			return emptyDs ;
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


	/*

	Local<ObjectTemplate> raw_template = v8::ObjectTemplate::New(isolate);
	raw_template->SetInternalFieldCount(1);
	raw_template->SetAccessor(v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(),PEDataset_GetCallBack );
	raw_template->SetAccessor(v8::String::NewFromUtf8(isolate, "height").ToLocalChecked(),PEDataset_GetCallBack );
	raw_template->SetAccessor(v8::String::NewFromUtf8(isolate, "nband").ToLocalChecked(),PEDataset_GetCallBack );
	raw_template->SetAccessor(v8::String::NewFromUtf8(isolate, "dataType").ToLocalChecked(),PEDataset_GetCallBack );
	//raw_template->Set(v8::String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked()
	//	,FunctionTemplate::New(isolate, PEDataset_ForEachPixelCallBack) );
	Local<Value> forEachFunc ;
	if( PEDatasetForEachPixelFunction.IsEmpty() )
	{
		string loopCode = "var PixelEngine_Dataset_ForEachPixel="
			"function(pxfunc){"
				"var asize=this.width*this.height;var ib=0;"
				"var vals=new Int16Array(this.nband);"
				"var tdata=this.tiledata;"
				"for(var it=0;it<asize;++it){"
				  "for(ib=0;ib<this.nband;++ib){"
				    "vals[ib]=tdata[ib*asize+it];"
				  "}"
				  "pxfunc(vals,it);"
				"}"
			"};" ;
		v8::Local<v8::String> source =
          v8::String::NewFromUtf8(isolate,loopCode.c_str()).ToLocalChecked() ;
    	v8::Local<v8::Script> script =
          v8::Script::Compile(context,source).ToLocalChecked();
    	v8::Local<v8::Value> resultForEachPixelFunc = script->Run(context).ToLocalChecked();
    	String::Utf8Value res(isolate,resultForEachPixelFunc) ;
    	cout<<"result foreachpixel:"<<*res<<endl;
    	v8::Local<v8::Value> forPixelFuncObj = 
    		context->Global()->Get(context , v8::String::NewFromUtf8(isolate, "PixelEngine_Dataset_ForEachPixel").ToLocalChecked()).ToLocalChecked();
    	PEDatasetForEachPixelFunction.Reset(isolate , forPixelFuncObj) ;
    	forEachFunc = Local<Value>::New( isolate ,forPixelFuncObj ) ;
	}else
	{
		forEachFunc = Local<Value>::New( isolate ,PEDatasetForEachPixelFunction ) ; 
	}
	
	cout<<"set : "<<endl ;
    raw_template->Set(v8::String::NewFromUtf8(isolate, "forEachPixel").ToLocalChecked()
		,forEachFunc );

	PEDatasetTemplate.Reset( isolate ,raw_template ) ;
	*/
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
    		var ds0 = globalFunc_newDatasetCallBack() ;
    		var ds1 = ds0.forEachPixel(null) ;
    		ds1.forEachPixel().renderGray().forEachPixel().renderGray() ;
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
	  printf("dura:%d ms \n", now1 - now);//1024*1024 use 340millisec

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