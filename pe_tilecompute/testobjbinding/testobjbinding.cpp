//Complete example of binding a C++ class with Google V8.
 


#include <v8.h>
#include <libplatform/libplatform.h>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

using namespace v8;
using namespace std;


void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  Local<Value> arg = args[0];
  String::Utf8Value value(isolate, arg);
  printf("V8Logged: %s\n", *value ) ;
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

	v8::Local<v8::Context> context = Context::New(GetIsolate(), NULL, global);//make local context and pass in global object
	m_context.Reset(GetIsolate(), context);//persisit local context
	 
	Context::Scope context_scope(context);//switch to this context do something.

	return true ;
}

bool PixelEngine::ExecuteScript( string cstring ) {
  HandleScope handle_scope(GetIsolate());
  TryCatch try_catch(GetIsolate());
  //Local<Context> context = Context::New( GetIsolate() );
  Local<Context> context = Local<Context>::New( GetIsolate(), m_context);
  Context::Scope context_scope(context);	
 
  string ss2 = "log(\"hello\");" ;
  Local<String> script =  String::NewFromUtf8( GetIsolate() , ss2.c_str(),
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
	std::string js_source = R"(
		log(\"start....\") ;

	)";
	

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
	pe.ExecuteScript( js_source) ;
	 
	return 0;
}

