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

struct PEDataset
{
	int dataType;//1-byte , 3-short
	int width,height,nband,byteLength;
	unsigned char* tiledata ;
	PEDataset(int dt,int w,int h,int nb);
	//v8 will not release internal field data.
	inline ~PEDataset(){ printf("~PEDataset()\n"); if(tiledata)delete[] tiledata;tiledata=0; } ;
private:
	inline PEDataset(PEDataset& right){} ;
};

PEDataset::PEDataset(int dt,int w,int h,int nb)
{

	width=w;height=h;dataType=dt;nband=nb;
	if( dataType==3 )
	{
		byteLength = width*height*nband*2 ;
	}else
	{
		byteLength = width*height*nband  ;
	}
	tiledata = new unsigned char[byteLength] ;
}

void PEDataset_GetCallBack(v8::Local<v8::String> property,
              const v8::PropertyCallbackInfo<Value>& info) 
{
	cout<<"inside get call back"<<endl ;
	Isolate* isolate = info.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);

	String::Utf8Value u8value ( isolate, property ) ;
	string pname( *u8value ) ;
	v8::Local<v8::Object> self = info.Holder();
	Local<Value> internalValue = self->GetInternalField(0) ;
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	if( ptr == 0 )
	{
		cout<<"internal ptr is null"<<endl ;
		return ;
	}
	PEDataset* dsPtr = static_cast<PEDataset*>(ptr) ;
	if( pname == "width" )
	{
		info.GetReturnValue().Set(dsPtr->width);
	}else 	if( pname == "height" )
	{
		info.GetReturnValue().Set(dsPtr->height);
	}else 	if( pname == "nband" )
	{
		info.GetReturnValue().Set(dsPtr->nband);
	}else 	if( pname == "dataType" )
	{
		info.GetReturnValue().Set(dsPtr->dataType);
	} else 	if( pname == "tiledata" )
	{
		int i16size = dsPtr->width*dsPtr->height*dsPtr->nband ;
		Local<ArrayBuffer> arrbuff = ArrayBuffer::New( isolate,dsPtr->tiledata,i16size*2 ) ;
		Local<Int16Array> i16arr = Int16Array::New( arrbuff , 0 , i16size) ;
		info.GetReturnValue().Set(i16arr);
	} 
}

string wft_int2str(int i)
{
	char temp[16] ;
	sprintf( temp, "%d" , i) ;
	return string(temp) ;
}

void PEDataset_ForEachPixelCallBack(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if( args.Length() !=1  ) return ;
	cout<<"inside foreachpixel call back"<<endl ;
	Isolate* isolate = args.GetIsolate() ;
	v8::HandleScope handle_scope(isolate);
	Local<Context> context( isolate->GetCurrentContext() ) ;

	v8::Local<v8::Object> self = args.Holder();
	Local<Value> internalValue = self->GetInternalField(0) ;
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	PEDataset* dsPtr = static_cast<PEDataset*>(ptr) ;
	int asize = dsPtr->width * dsPtr->height ;
	int nband = dsPtr->nband ;
	short* pxvals = new short[nband] ;
	short* tiledata = (short*) dsPtr->tiledata ;

	//create a compiled function
	Local<String> code = String::NewFromUtf8(isolate,
	  R"(function(v,i) {
	     return (v[3]-v[2])/(v[3]+v[2])*10000;
	    })").ToLocalChecked();

	//unwarp pixelfunction
	Function* pxfunc = Function::Cast(*args[0]);
	// String::Utf8Value pfname ( isolate, pxfunc->GetName() ) ;
	// cout<<"pxfunc name:"<< *pfname<<endl;

	//Local<Value> cfunc = Script::Compile(context,code).ToLocalChecked()->Run(context).ToLocalChecked();
	//Function* cfunc2 = Function::Cast(*cfunc);

	

	PEDataset* outds = new PEDataset(3 , dsPtr->width , dsPtr->height , 1 ) ;
 
	short* outdata = (short*)outds->tiledata ;

	Local<ArrayBuffer> arrbuff = ArrayBuffer::New( isolate , pxvals ,  nband*2 ) ;//BackStore
	Local<Int16Array> i16arr = Int16Array::New( arrbuff , 0 , nband ) ;
	Local<Integer> index = Integer::New(isolate,0) ;
	Local<Value> pxargs[2] = { i16arr , index } ;
	for(int i = 0 ; i<asize ; ++ i )
	{
		// for(int ib = 0 ; ib<nband ; ++ ib )
		// {
		// 	pxvals[ib] = tiledata[ib*asize + i] ;
		// }
		pxfunc->Call( context, context->Global() , 2 ,  pxargs ) ;
		//outdata[i] = (pxvals[3]-pxvals[2])*1.f/(pxvals[3]+pxvals[2])*10000 ;

	}
 
	delete  outds ;
	delete[] pxvals ;
}

  // global->Set(String::NewFromUtf8Literal(GetIsolate(), "log"),
  //             FunctionTemplate::New(GetIsolate(), LogCallback));


void initTemplate( Isolate* isolate, Local<Context>& context )
{
	v8::HandleScope handle_scope(isolate);

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
}


void testCppForEachPixel( PEDataset* dsPtr )
{
	int nband = dsPtr->nband ;
	short* pxvals = new short[nband] ;
	short* tiledata = (short*) dsPtr->tiledata ;
	int asize = dsPtr->width*dsPtr->height ;

	PEDataset* outds = new PEDataset(3 , dsPtr->width , dsPtr->height , 1) ;
	short* outdata = (short*) outds->tiledata ;
	for(int i = 0 ; i<asize ; ++ i )
	{
		for(int ib = 0 ; ib<nband ; ++ ib )
		{
			pxvals[ib] = tiledata[ib*asize + i] ;
		} 

		outdata[i] = (pxvals[3]-pxvals[2])*1.f/(pxvals[3]+pxvals[2])*10000 ;

	}
	delete outds ;
	delete[] pxvals;
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
    v8::Context::Scope context_scope(context);
    initTemplate(isolate,context) ;

    PEDataset* ds1ptr = new PEDataset(3,1024,1024,6) ;
     
    {
    	v8::HandleScope handle_scope(isolate);
    	Local<ObjectTemplate> templ =
      		Local<ObjectTemplate>::New( isolate , PEDatasetTemplate );

    	int icnt = templ->InternalFieldCount () ;
    	cout<<"icnt "<<icnt<<endl ;
    	
    	
    	if( ds1ptr==0 )
    	{
    		cout<<"bad ds1ptr "<<endl;
    	}
	    Local<Object> ds1obj = templ->NewInstance(context).ToLocalChecked() ;
	    cout<<"newinstance ok."<<endl; 

	    ds1obj->SetInternalField(0 , External::New(isolate,ds1ptr) ) ;
	    //ds1obj->SetAlignedPointerInInternalField(0 , ds1ptr) ;

	    cout<<"set internal ptr."<<endl ;
	    
	    //void* tempptr2 = ds1obj->GetAlignedPointerFromInternalField(0) ;
		ds1obj->GetInternalField(0) ;
	    cout<<"get ptr "<<endl ;
	    // /ds1obj->GetInternalField(0) ;

	    Local<Object> contextGlobal = context->Global() ;
	    contextGlobal->Set( context , String::NewFromUtf8(isolate,"ds1").ToLocalChecked(), ds1obj ) ;

    }

     
     
    {
      // Create a string containing the JavaScript source code.
      v8::Local<v8::String> source =
          v8::String::NewFromUtf8(isolate, "var func=function(vals,index){return vals[0];}; ds1.forEachPixel( func); ").ToLocalChecked() ;

      v8::Local<v8::String> source2 =
          v8::String::NewFromUtf8(isolate, "var func=function(vals,index){return vals[0];};var vals=new Int16Array(6); for(var i=0;i<1024*1024;++i){for(var ib=0;ib<6;++ib){vals[ib]=ib;} func(vals,i); }; ").ToLocalChecked() ;


      // Compile the source code.
      v8::Local<v8::Script> script =
          v8::Script::Compile(context, source).ToLocalChecked();

	  unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
      unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	  printf("dura:%d ms \n", now1 - now);//1024*1024 use 340millisec

      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(isolate, result);
      printf("%s\n", *utf8);
    }

    {
		unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		testCppForEachPixel(ds1ptr) ;
		unsigned long now1= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		printf("testcppdura:%d ms \n", now1 - now);//1024*1024 use 340millisec
    }
	
    PEDatasetForEachPixelFunction.Reset() ;
    PEDatasetTemplate.Reset() ;// global handle must reset before v8 out.
    delete ds1ptr;
 
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