
#include <string>
#include <iostream>
#include "WV8Engine.h"

#include <ctime>
#include <chrono>
#include "wftools_linux.h"

using  std::string ;

WV8Engine::WV8Engine():isolate(0)
{
}


WV8Engine::~WV8Engine()
{
	this->isolate = 0;
}



bool WV8Engine::checkSciptOK( 
	 std::string scriptText
	, std::string& errorText
	 ) 
{
	v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
	v8::Context::Scope context_scope(tempContext);
	v8::TryCatch try_catch(this->isolate);
	v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, scriptText.c_str() ,
				v8::NewStringType::kNormal) ;
	if (source.IsEmpty()) {
		//compile error
		v8::String::Utf8Value error( this->isolate, try_catch.Exception());
		errorText = std::string(*error);
		std::cout << "build source error:" << errorText << std::endl;
		return false;
	}
	v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
	if (compiledScript.IsEmpty()) {
		//compile error
		v8::String::Utf8Value error( this->isolate, try_catch.Exception());
		errorText = std::string(*error);
		std::cout << "compile error:" << errorText << std::endl;
		return false;
	}

	return true ;
}


bool WV8Engine::computeOneOutputBandTile(WTileDataBuffer& fy4TileData
	, std::string scriptText
	, WTileDataBuffer& outDataBuffer
	, std::string& errorText
	, int& dura_ms )
{
	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		//tempContext->Global()->Set(v8::String::NewFromUtf8(this->isolate,"inArr"), sArray);//7.x api
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api
			

		//create output data buffer and pass into js vm.
		outDataBuffer.create(256 * 256 * 2);//short[256*256]
		v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
			this->isolate,
			outDataBuffer.getDataPointer() ,
			outDataBuffer.getDataSize() );
		v8::Local<v8::Int16Array> outSArray = v8::Int16Array::New(outbuf, 0, outDataBuffer.getDataSize()/2 );// 
		tempContext->Global()->Set(tempContext , 
			v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
			outSArray);//8.x api
 
			
		//std::string debugScript = "var sum=0; for(var i = 0 ; i<arr.length ; ++ i ){ if(i<256*256){outArr[i]=arr[i]+1;} sum+=1;} ;";//very slow
		//std::string debugScript = "var sum=0; outArr.forEach(function(val,index){ outArr[index]=index;}) ;";//fast
		//std::string debugScript = "var sum=0;";
		std::string processScript =
			scriptText + 
			"outArr.forEach( function(val,index) {"
			"let pixeloffset=index*18;"
			"let b1=inArr[pixeloffset+0] ;"
			"let b2=inArr[pixeloffset+1] ;"
			"let b3=inArr[pixeloffset+2] ;"
			"let b4=inArr[pixeloffset+3] ;"
			"let b5=inArr[pixeloffset+4] ;"
			"let b6=inArr[pixeloffset+5] ;"
			"let b7=inArr[pixeloffset+6] ;"
			"let b8=inArr[pixeloffset+7] ;"
			"let b9=inArr[pixeloffset+8] ;"
			"let b10=inArr[pixeloffset+9] ;"
			"let b11=inArr[pixeloffset+10] ;"
			"let b12=inArr[pixeloffset+11] ;"
			"let b13=inArr[pixeloffset+12] ;"
			"let b14=inArr[pixeloffset+13] ;"
			"let t0=inArr[pixeloffset+14] ;"
			"let t1=inArr[pixeloffset+15] ;"
			"let p0=inArr[pixeloffset+16] ;"
			"let p1=inArr[pixeloffset+17] ;"
			"outArr[index]=computeSingleShortOuput(b1,b2,b3,b4,b5,b6,b7,b8,b9, "
			"	b10,b11,b12,b13,b14,t0,t1,p0,p1);"
			"}) ;";

		v8::TryCatch try_catch(this->isolate);
		// Create a string containing the JavaScript source code.
		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}
		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		//debug flag for unbound script.
		bool useUnboundScript = false ;
		
		if( useUnboundScript ) {
			//use unbound script
			bool isUSEmpty= this->unboundScript.IsEmpty() ;
			printf("unboundScript isUSEmpty:%d\n" , isUSEmpty) ;
			if(   isUSEmpty   ) {
				v8::ScriptCompiler::Source compilerSource( source.ToLocalChecked() , nullptr ) ;//1.make source
				v8::ScriptCompiler::CompileUnboundScript(this->isolate , 
				  &compilerSource ).ToLocal( & this->unboundScript ) ;//2.make unbound script.
			}

			v8::Local<v8::Script> bindedCompiledScript = (*this->unboundScript)->BindToCurrentContext() ;//3.bind context, every times it runs.
			if (bindedCompiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "bindedCompiledScript bind error:" << errorText << std::endl;
				return false;
			}
	 
			v8::Local<v8::Value> runResultb = bindedCompiledScript->Run(tempContext).ToLocalChecked();//4.run 
			if (runResultb.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "unbound run error:" << errorText << std::endl;
				return false;
			}

		}else {
			//compile the script every time.
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//

			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
		}
	 
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


		if( useUnboundScript ) {
			printf("unboundscript dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}else{
			printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}
		
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;

}



bool WV8Engine::computeRGBOutputBandTile(WTileDataBuffer& fy4TileData
	, std::string scriptText
	, WTileDataBuffer& outDataBuffer
	, std::string& errorText
	, int& dura_ms )
{
	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeRGBOutputBandTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		//tempContext->Global()->Set(v8::String::NewFromUtf8(this->isolate,"inArr"), sArray);//7.x api
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api

		//create output data buffer and pass into js vm.
		outDataBuffer.create(256 * 256 * 3);//rgb
		v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
			this->isolate,
			outDataBuffer.getDataPointer() ,
			outDataBuffer.getDataSize() );
		v8::Local<v8::Uint8Array> outSArray = v8::Uint8Array::New(outbuf, 0, outDataBuffer.getDataSize() );// 
		tempContext->Global()->Set(tempContext , 
			v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
			outSArray);//8.x api
			
		//std::string debugScript = "var sum=0; for(var i = 0 ; i<arr.length ; ++ i ){ if(i<256*256){outArr[i]=arr[i]+1;} sum+=1;} ;";//very slow
		//std::string debugScript = "var sum=0; outArr.forEach(function(val,index){ outArr[index]=index;}) ;";//fast
		//std::string debugScript = "var sum=0;";
		std::string processScript =
			scriptText + 
			"for(let index=0;index<65536;++index){"
			"let pixeloffset=index*18;"
			"let b1=inArr[pixeloffset+0] ;"
			"let b2=inArr[pixeloffset+1] ;"
			"let b3=inArr[pixeloffset+2] ;"
			"let b4=inArr[pixeloffset+3] ;"
			"let b5=inArr[pixeloffset+4] ;"
			"let b6=inArr[pixeloffset+5] ;"
			"let b7=inArr[pixeloffset+6] ;"
			"let b8=inArr[pixeloffset+7] ;"
			"let b9=inArr[pixeloffset+8] ;"
			"let b10=inArr[pixeloffset+9] ;"
			"let b11=inArr[pixeloffset+10] ;"
			"let b12=inArr[pixeloffset+11] ;"
			"let b13=inArr[pixeloffset+12] ;"
			"let b14=inArr[pixeloffset+13] ;"
			"let t0=inArr[pixeloffset+14] ;" 
			"let t1=inArr[pixeloffset+15] ;"
			"let p0=inArr[pixeloffset+16] ;"
			"let p1=inArr[pixeloffset+17] ;"
			"let temp3=computeRGBOuput(b1,b2,b3,b4,b5,b6,b7,b8,b9, "
			"	b10,b11,b12,b13,b14,t0,t1,p0,p1);"
			"let outoffset=index*3;"
			"outArr[outoffset]=temp3[0];outArr[outoffset+1]=temp3[1];outArr[outoffset+2]=temp3[2];" 
			"} ;";//output use bip

		v8::TryCatch try_catch(this->isolate);

		// Create a string containing the JavaScript source code.
		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal)  ;// .ToLocalChecked();
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}

		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		//debug flag for unbound script.
		bool useUnboundScript = false ;
		
		if( useUnboundScript ) {
			//use unbound script
			bool isUSEmpty= this->unboundScript.IsEmpty() ;
			printf("unboundScript isUSEmpty:%d\n" , isUSEmpty) ;
			if(   isUSEmpty   ) {
				v8::Local<v8::String> localsource ;
				source.ToLocal(&localsource) ;
				v8::ScriptCompiler::Source compilerSource( localsource, nullptr ) ;//1.make source
				v8::ScriptCompiler::CompileUnboundScript(this->isolate , 
				  &compilerSource ).ToLocal( & this->unboundScript ) ;//2.make unbound script.
			}
			v8::Local<v8::Script> bindedCompiledScript = (*this->unboundScript)->BindToCurrentContext() ;//3.bind context, every times it runs.
			if (bindedCompiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "bindedCompiledScript bind error:" << errorText << std::endl;
				return false;
			}
			v8::Local<v8::Value> runResultb = bindedCompiledScript->Run(tempContext).ToLocalChecked();//4.run 
			if (runResultb.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "unbound run error:" << errorText << std::endl;
				return false;
			}

		}else {
			//compile the script every time.
			v8::Local<v8::String> localsource ;
				source.ToLocal(&localsource) ;
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, localsource) ;
 
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
 
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
 
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
		}
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		if( useUnboundScript ) {
			//printf("unboundscript dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}else{
			//printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}
		
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeRGBOutputBandTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeRGBOutputBandTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;

}



bool WV8Engine::computeRGBAOutputBandTile(WTileDataBuffer& fy4TileData
	, std::string scriptText
	, WTileDataBuffer& outDataBuffer
	, std::string& errorText
	, int& dura_ms )
{
	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeRGBAOutputBandTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		//tempContext->Global()->Set(v8::String::NewFromUtf8(this->isolate,"inArr"), sArray);//7.x api
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api

		//create output data buffer and pass into js vm.
		outDataBuffer.create(256 * 256 * 4);//rgba
		v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
			this->isolate,
			outDataBuffer.getDataPointer() ,
			outDataBuffer.getDataSize() );
		v8::Local<v8::Uint8Array> outSArray = v8::Uint8Array::New(outbuf, 0, outDataBuffer.getDataSize());// 
		tempContext->Global()->Set(tempContext , 
			v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
			outSArray);//8.x api
			
		//std::string debugScript = "var sum=0; for(var i = 0 ; i<arr.length ; ++ i ){ if(i<256*256){outArr[i]=arr[i]+1;} sum+=1;} ;";//very slow
		//std::string debugScript = "var sum=0; outArr.forEach(function(val,index){ outArr[index]=index;}) ;";//fast
		//std::string debugScript = "var sum=0;";
		std::string processScript =
			scriptText + 
			"for(let index=0;index<65536;++index){"
			"let pixeloffset=index*18;"
			"let b1=inArr[pixeloffset+0] ;"
			"let b2=inArr[pixeloffset+1] ;"
			"let b3=inArr[pixeloffset+2] ;"
			"let b4=inArr[pixeloffset+3] ;"
			"let b5=inArr[pixeloffset+4] ;"
			"let b6=inArr[pixeloffset+5] ;"
			"let b7=inArr[pixeloffset+6] ;"
			"let b8=inArr[pixeloffset+7] ;"
			"let b9=inArr[pixeloffset+8] ;"
			"let b10=inArr[pixeloffset+9] ;"
			"let b11=inArr[pixeloffset+10] ;"
			"let b12=inArr[pixeloffset+11] ;"
			"let b13=inArr[pixeloffset+12] ;"
			"let b14=inArr[pixeloffset+13] ;"
			"let t0=inArr[pixeloffset+14] ;"
			"let t1=inArr[pixeloffset+15] ;"
			"let p0=inArr[pixeloffset+16] ;"
			"let p1=inArr[pixeloffset+17] ;"
			"let temp4=computeRGBAOuput(b1,b2,b3,b4,b5,b6,b7,b8,b9, "
			"	b10,b11,b12,b13,b14,t0,t1,p0,p1);"
			"let outoffset=index*4;"
			"outArr[outoffset]=temp4[0];outArr[outoffset+1]=temp4[1];outArr[outoffset+2]=temp4[2];outArr[outoffset+3]=temp4[3];" 
			"};";//output use bip

		v8::TryCatch try_catch(this->isolate);

		// Create a string containing the JavaScript source code.
		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}

		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		//debug flag for unbound script.
		bool useUnboundScript = false ;
		
		if( useUnboundScript ) {
			//use unbound script
			bool isUSEmpty= this->unboundScript.IsEmpty() ;
			printf("unboundScript isUSEmpty:%d\n" , isUSEmpty) ;
			if(   isUSEmpty   ) {
				v8::ScriptCompiler::Source compilerSource( source.ToLocalChecked() , nullptr ) ;//1.make source
				v8::ScriptCompiler::CompileUnboundScript(this->isolate , 
				  &compilerSource ).ToLocal( & this->unboundScript ) ;//2.make unbound script.
			}

			v8::Local<v8::Script> bindedCompiledScript = (*this->unboundScript)->BindToCurrentContext() ;//3.bind context, every times it runs.
			if (bindedCompiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "bindedCompiledScript bind error:" << errorText << std::endl;
				return false;
			}

			v8::Local<v8::Value> runResultb = bindedCompiledScript->Run(tempContext).ToLocalChecked();//4.run 
			if (runResultb.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "unbound run error:" << errorText << std::endl;
				return false;
			}

		}else {
			//compile the script every time.

			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;

			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext) ;//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
		}
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


		if( useUnboundScript ) {
			printf("unboundscript dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}else{
			printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		}
		
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeRGBAOutputBandTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeRGBAOutputBandTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;

}



///output band maybe byte/short/int/float
/// "function bandmathByte(b1,b2,b3,b4,b5,b6,b7,b8,b9, "
/// "	b10,b11,b12,b13,b14,t0,t1,p0,p1){  "
/// "  // all b1-b13 and t0-p1 are refl,bt and degrees."
/// "  return b1*100;" 
/// "} ";
///bandmathByte
///bandmathShort
///bandmathInt
///bandmathFloat
bool WV8Engine::computeOneOutputBandTileV2(WTileDataBuffer& fy4TileData
	, std::string scriptText
	, WTileDataBuffer& outDataBuffer
	, std::string& errorText
	, int& dura_ms )
{
	//check script function ok
	const int RETTYPE_BYTE  = 0 ;//byte
	const int RETTYPE_SHORT = 1 ;//short
	const int RETTYPE_INT   = 2 ;//int
	const int RETTYPE_FLOAT = 3 ;//float
	const int IMAGE_SIZE=256*256;
	int returnType = RETTYPE_BYTE ;

	string scriptText2 = scriptText ;

	if ( scriptText.find("bandmathByte") != std::string::npos) {
		returnType = RETTYPE_BYTE ;
		scriptText2 = wft_replaceString(scriptText,"bandmathByte","bandmathFloat") ;


	}else if ( scriptText.find("bandmathShort") != std::string::npos) {
		returnType = RETTYPE_SHORT ;
		scriptText2 = wft_replaceString(scriptText,"bandmathShort","bandmathFloat") ;


	}else if ( scriptText.find("bandmathInt") != std::string::npos) {
		returnType = RETTYPE_INT ;
		scriptText2 = wft_replaceString(scriptText,"bandmathInt","bandmathFloat") ;


	}else if ( scriptText.find("bandmathFloat") != std::string::npos){
		//float
		returnType = RETTYPE_FLOAT ;

	}else{
		errorText = "ERRORID-3001 in WV8Engine::computeOneTile: no bandmath function." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}



	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		//tempContext->Global()->Set(v8::String::NewFromUtf8(this->isolate,"inArr"), sArray);//7.x api
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api
			

		//create output data buffer and pass into js vm.
		std::unique_ptr<float[]> fdatabuffer(new float[IMAGE_SIZE]) ;

		v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
			this->isolate,
			fdatabuffer.get() ,
			IMAGE_SIZE* 4 );
		v8::Local<v8::Float32Array> outFloatArray = v8::Float32Array::New(outbuf, 0, IMAGE_SIZE );// 
		tempContext->Global()->Set(tempContext , 
			v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
			outFloatArray);//8.x api
 
			
		//std::string debugScript = "var sum=0; for(var i = 0 ; i<arr.length ; ++ i ){ if(i<256*256){outArr[i]=arr[i]+1;} sum+=1;} ;";//very slow
		//std::string debugScript = "var sum=0; outArr.forEach(function(val,index){ outArr[index]=index;}) ;";//fast
		//std::string debugScript = "var sum=0;";
		std::string processScript =
			scriptText2 + 
			"outArr.forEach( function(val,index) {"
			"let pixeloffset=index*18;"
			"let b1=inArr[pixeloffset+0]/10000 ;"
			"let b2=inArr[pixeloffset+1]/10000 ;"
			"let b3=inArr[pixeloffset+2]/10000 ;"
			"let b4=inArr[pixeloffset+3]/10000 ;"
			"let b5=inArr[pixeloffset+4]/10000 ;"
			"let b6=inArr[pixeloffset+5]/10000 ;"
			"let b7=inArr[pixeloffset+6]/100+100 ;"
			"let b8=inArr[pixeloffset+7]/100+100 ;"
			"let b9=inArr[pixeloffset+8]/100+100 ;"
			"let b10=inArr[pixeloffset+9]/100+100 ;"
			"let b11=inArr[pixeloffset+10]/100+100 ;"
			"let b12=inArr[pixeloffset+11]/100+100 ;"
			"let b13=inArr[pixeloffset+12]/100+100 ;"
			"let b14=inArr[pixeloffset+13]/100+100 ;"
			"let t0=inArr[pixeloffset+14]/100 ;"
			"let t1=inArr[pixeloffset+15]/100 ;"
			"let p0=inArr[pixeloffset+16]/10 ;"
			"let p1=inArr[pixeloffset+17]/10 ;"
			"outArr[index]=bandmathFloat(b1,b2,b3,b4,b5,b6,b7,b8,b9, "
			"	b10,b11,b12,b13,b14,t0,t1,p0,p1);"
			"}) ;";

		std::cout<<"debug processScript:"<<processScript<<std::endl ;

		v8::TryCatch try_catch(this->isolate);
		// Create a string containing the JavaScript source code.
		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}
		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		{
			//compile the script every time.
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//

			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
		}

		//correct out type
		if( returnType == RETTYPE_BYTE ){
			outDataBuffer.create(IMAGE_SIZE);
			for(int it = 0 ; it < IMAGE_SIZE ; ++ it ){
				outDataBuffer.getDataPointer()[it] = (unsigned char) fdatabuffer.get()[it] ;
			}
		}else if( returnType == RETTYPE_SHORT ){
			outDataBuffer.create(IMAGE_SIZE*2);
			short* tptr = (short*)outDataBuffer.getDataPointer() ;
			for(int it = 0 ; it < IMAGE_SIZE ; ++ it ){
				tptr[it] = (short) fdatabuffer.get()[it] ;
			}
		}else if( returnType == RETTYPE_INT ){
			outDataBuffer.create(IMAGE_SIZE*4);
			int* tptr = (int*) outDataBuffer.getDataPointer() ;
			for(int it = 0 ; it < IMAGE_SIZE ; ++ it ){
				tptr[it] = (int) fdatabuffer.get()[it] ;
			}
		}else{
			outDataBuffer.create(IMAGE_SIZE*4);
			memcpy( outDataBuffer.getDataPointer() ,  fdatabuffer.get() ,
				outDataBuffer.getDataSize() ) ;
		} 
	 
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;

}





///compute a tile output a string, generally a json text. 
///the javascript must use template:
///function tile2json( inArr, extraobj ){
///    ... ...
///	   let jsonstr = JSON.stringify(value);
///    return jsonstr;//must in json format string.	
///}
///@param scriptText input javascript code, must use template above.
///@param extra should be json string
///@param outputString output string.
///@param errorText error info.
///@param duration in ms.
bool WV8Engine::computeOneOutputJson(WTileDataBuffer& fy4TileData
		, std::string scriptText
		, std::string extra
		, std::string& outputString
		, std::string& errorText
		, int& dura_ms ) 
{

	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);//
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		//tempContext->Global()->Set(v8::String::NewFromUtf8(this->isolate,"inArr"), sArray);//7.x api
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api
		
		if( extra.length()==0 ) extra="{}" ;

		v8::TryCatch try_catch(this->isolate);
		// Create a string containing the JavaScript source code.
		std::string processScript =
			"let extra=JSON.parse('"+extra+"');" + // parse json
			scriptText + 
			"retstr=tile2json(inArr,extra);" ;

		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}
		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


		{
			//compile the script every time.
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//

			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
			v8::String::Utf8Value retstr(this->isolate , runResult.ToLocalChecked() ) ;
			outputString = std::string( *retstr ) ;
			std::cout<<"debug outputString:" << outputString<<std::endl ;
		}
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;

}



///compute a json array output a json 
///the javascript must use template:
///function jsonarray2json( jsonarray, extraobj ){
///    ... ... //jsonarray should be a root array object.[{...},{...}...]
///	   let jsonstr = JSON.stringify(value);
///    return jsonstr;//must in json format string.	
///}
///@param jsonarray input javascript code, must use template above.
///@param scriptText input javascript code, must use template above.
///@param extra should be json string
///@param outputString output string.
///@param errorText error info.
///@param duration in ms.
bool WV8Engine::computeJson2Json(
		std::string injsonStr , 
		std::string scriptText ,
		std::string extra ,
		std::string& outputString , 
		std::string& errorText , 
		int& dura_ms ) 
{
	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}

	try {
		 
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);//
		v8::Context::Scope context_scope(tempContext);
				
		if( extra.length()==0 ) extra="{}" ;

		v8::TryCatch try_catch(this->isolate);
		// Create a string containing the JavaScript source code.
		std::string processScript =
			"let jsonarr=JSON.parse('"+injsonStr+"');" + // parse array
			"let extra=JSON.parse('"+extra+"');" + // parse json
			scriptText + 
			"retstr=jsonarray2json(jsonarr,extra);" ;

		std::cout<<"debug computeJson2Json:"<<processScript<<std::endl;

		v8::MaybeLocal<v8::String> source =
			v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
				v8::NewStringType::kNormal) ;
		if (source.IsEmpty()) {
			//compile error
			v8::String::Utf8Value error( this->isolate, try_catch.Exception());
			errorText = std::string(*error);
			std::cout << "build source error:" << errorText << std::endl;
			return false;
		}
		unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


		{
			//compile the script every time.
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//

			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}
			v8::String::Utf8Value retstr(this->isolate , runResult.ToLocalChecked() ) ;
			outputString = std::string( *retstr ) ;
			std::cout<<"debug outputString:" << outputString<<std::endl ;
		}
		unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		printf("dura:%ld ms \n", now1 - now);//1024*1024 use 340millisec
		dura_ms = now1 - now ;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}
	return true;
}


///@param scriptStr 
///  function tile2tileb1(){...} because inArr outArr are global varibles, so no need params.
bool WV8Engine::computeTile2Tile(
	WTileDataBuffer& fy4TileData , 
	std::string scriptText ,
	std::string extra ,
	WTileDataBuffer& outDataBuffer , 
	std::string& errorText , 
	int& dura_ms ) 
{

	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}
	const int IMAGE_SIZE=256*256;
	unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api

		v8::TryCatch try_catch(this->isolate);

		std::string processScript = "let extra=JSON.parse('"+extra+"');" + scriptText ;
		if ( scriptText.find("tile2tileb1") != std::string::npos) {
			processScript =  processScript + "tile2tileb1();" ;
			
			outDataBuffer.create( IMAGE_SIZE ) ;//alloc memories
			
			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Uint8Array> outJsArray = v8::Uint8Array::New(outbuf, 0, IMAGE_SIZE );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}



		}else if ( scriptText.find("tile2tileb3") != std::string::npos) {
			processScript = processScript + "tile2tileb3();" ;
			outDataBuffer.create( IMAGE_SIZE*3 ) ;
			
			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Uint8Array> outJsArray = v8::Uint8Array::New(outbuf, 0, IMAGE_SIZE*3 );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}

		}else if ( scriptText.find("tile2tileb4") != std::string::npos) {
			processScript = processScript + "tile2tileb4();" ;
			outDataBuffer.create( IMAGE_SIZE*4 ) ;

			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Uint8Array> outJsArray = v8::Uint8Array::New(outbuf, 0, IMAGE_SIZE*4 );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}

		}else if ( scriptText.find("tile2tiles1") != std::string::npos) {
			processScript = processScript + "tile2tiles1();" ;
			outDataBuffer.create( IMAGE_SIZE*2 ) ;

			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Int16Array> outJsArray = v8::Int16Array::New(outbuf, 0, IMAGE_SIZE );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}

		}else if ( scriptText.find("tile2tilei1") != std::string::npos) {
			processScript = processScript + "tile2tilei1();" ;
			outDataBuffer.create( IMAGE_SIZE*4 ) ;

			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Int32Array> outJsArray = v8::Int32Array::New(outbuf, 0, IMAGE_SIZE );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}

		}else if ( scriptText.find("tile2tilef1") != std::string::npos) {
			processScript = processScript + "tile2tilef1();" ;
			outDataBuffer.create( IMAGE_SIZE*4 ) ;

			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Float32Array> outJsArray = v8::Float32Array::New(outbuf, 0, IMAGE_SIZE );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}

		} else {
			errorText = "ERRORID-1001 in WV8Engine::computeOneTile: script function not supported." ;
			std::cout<<errorText<<std::endl ;
			return false ;
		}
		
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}

	unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	dura_ms = now1 - now ;
	printf("debug tile2tile dura:%ld ms \n",dura_ms);//1024*1024 use 340millisec
	return true;
}


// this method is for pixel engine 2020-5-12
//
bool WV8Engine::pe_computeTile2Tile( vector<WTileDataBuffer>& tiledataVec , 
		std::string scriptStr ,
		std::string extraStr ,
		WTileDataBuffer& outDataBuffer , 
		std::string& errorTextInComputing , 
		int& duration_ms ) 
{

	if( this->isolate == 0 ){
		errorText = "ERRORID-1001 in WV8Engine::computeOneTile: WV8Engine has a null isolate." ;
		std::cout<<errorText<<std::endl ;
		return false ;
	}
	const int IMAGE_SIZE=256*256;
	unsigned long now = std::chrono::duration_cast
				<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	try {
		int inDataByteSize = fy4TileData.getDataSize() ;
		int inDataShortSize = inDataByteSize / 2;
		// Create a new context.
		v8::Local<v8::Context> tempContext = v8::Context::New(this->isolate);// 一个独立的js运行环境
		v8::Context::Scope context_scope(tempContext);

		// put input fy4data into js vm.
		v8::Local<v8::ArrayBuffer> arrbuf = v8::ArrayBuffer::New(
			this->isolate ,
			fy4TileData.getDataPointer() ,
			inDataByteSize ) ;
		v8::Local<v8::Int16Array> sArray = v8::Int16Array::New(arrbuf, 0, inDataShortSize);
		tempContext->Global()->Set(tempContext,
			v8::String::NewFromUtf8(this->isolate,"inArr").ToLocalChecked(), 
			sArray);//8.x api

		v8::TryCatch try_catch(this->isolate);

		std::string processScript = "let extra=JSON.parse('"+extra+"');" + scriptText ;
		if ( scriptText.find("tile2tileb1") != std::string::npos) {
			processScript =  processScript + "tile2tileb1();" ;
			
			outDataBuffer.create( IMAGE_SIZE ) ;//alloc memories
			
			//put outbuffer into v8 vm.
			v8::Local<v8::ArrayBuffer> outbuf = v8::ArrayBuffer::New(
				this->isolate,
				outDataBuffer.getDataPointer() ,
				outDataBuffer.getDataSize() );
			v8::Local<v8::Uint8Array> outJsArray = v8::Uint8Array::New(outbuf, 0, IMAGE_SIZE );// 
			tempContext->Global()->Set(tempContext , 
				v8::String::NewFromUtf8(this->isolate, "outArr").ToLocalChecked(), 
				outJsArray);//binding

			
			// Create a string containing the JavaScript source code.
			v8::MaybeLocal<v8::String> source =
				v8::String::NewFromUtf8( this->isolate, processScript.c_str() ,
					v8::NewStringType::kNormal) ;
			if (source.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "build source error:" << errorText << std::endl;
				return false;
			}
			// Compile the source code.
			v8::MaybeLocal<v8::Script> compiledScript =
				v8::Script::Compile(tempContext, source.ToLocalChecked() ) ;
			if (compiledScript.IsEmpty()) {
				//compile error
				v8::String::Utf8Value error( this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "compile error:" << errorText << std::endl;
				return false;
			}
			v8::MaybeLocal<v8::Value> runResult = compiledScript.ToLocalChecked()->Run(tempContext);//
			if (runResult.IsEmpty()) {
				//run error
				v8::String::Utf8Value error(this->isolate, try_catch.Exception());
				errorText = std::string(*error);
				std::cout << "run error:" << errorText << std::endl;
				return false;
			}



		} else {
			errorText = "ERRORID-1001 in WV8Engine::computeOneTile: script function not supported." ;
			std::cout<<errorText<<std::endl ;
			return false ;
		}
		
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		errorText = std::string("ERRORID-1000 in WV8Engine::computeOneTile: ") + ex.what();
		std::cerr << errorText << std::endl;
		return false;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		errorText = "ERRORID-2000 in WV8Engine::computeOneTile: Unknown failure occurred. Possible memory corruption";
		std::cerr << errorText << std::endl;
		return false;
	}

	unsigned long now1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	dura_ms = now1 - now ;
	printf("debug tile2tile dura:%ld ms \n",dura_ms);//1024*1024 use 340millisec
	return true;


	return true ;
}