/// tile computing for pixel engine .

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "libplatform/libplatform.h"
#include "v8.h"

#include "WV8Engine.h"

using std::cout;
using std::endl ;

//std::unique_ptr<v8::Platform> v8Platform = nullptr;
//ShellArrayBufferAllocator array_buffer_allocator;


struct WTilesComputing {
	std::unique_ptr<v8::Platform> v8Platform ;
	v8::Isolate::CreateParams     m_create_params;
	v8::Isolate*                  m_isolate = 0 ;

	WTilesComputing()  ;
	~WTilesComputing() ;

} ;
 

WTilesComputing::WTilesComputing()  {
	v8::V8::InitializeICU();
    v8Platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(v8Platform.get());
    v8::V8::Initialize();
	
	m_create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	m_isolate = v8::Isolate::New(m_create_params);

}


WTilesComputing::~WTilesComputing() {
	//cout<<"release WTilesComputing"<<endl ;
	//m_isolate->Exit() ;
	m_isolate->Dispose();
	//delete m_create_params.array_buffer_allocator;
}

 


 
std::string g_theVersion = "0.1 2020-5-12" ; 

  

jstring cstring2jstring(JNIEnv *env, std::string& cppstr ) {
	return env->NewStringUTF(cppstr.c_str()) ;
}


JNIEXPORT jstring JNICALL Java_htht_mongo_spark_jni_TilesCompute_version(JNIEnv * env, jobject obj) {
	return cstring2jstring(env , g_theVersion) ;
}

 


std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}



///One tile produce one tile output.
///@param env jni things
///@param obj jni things
///@param type c/nc/c_header/nc_header
///@param script javascript function should be one of tile2tileb1 tile2tiles1 tile2tilei1 tile2tilef1 tile2tileb3 tile2tileb4.
///@param data input fy4 tile data.
///@param extra extra json string
JNIEXPORT jbyteArray JNICALL Java_htht_mongo_spark_jni_TilesCompute_MapComputingTile2Tile
  (JNIEnv * env, jobject obj, 
  	jstring script,  
  	jbyteArray data , 
  	jstring extra ) {  

	WTileDataBuffer outTileData ;
  	WTilesComputing wv8 ;
	{
		v8::Isolate::Scope isolate_scope(wv8.m_isolate);
		v8::HandleScope scope(wv8.m_isolate);

		WV8Engine v8engine  ;
		v8engine.init(wv8.m_isolate ) ;

		jbyte *inbytes = (jbyte *)env->GetByteArrayElements(data, NULL);
		size_t inlength = (size_t) env->GetArrayLength(data);
		WTileDataBuffer inDataBuffer ;
		inDataBuffer.create( inlength ) ;
		memcpy( inDataBuffer.getDataPointer() , inbytes , inlength ) ;
		env->ReleaseByteArrayElements(data, inbytes, 0 );

		WTileDataBuffer inTileData ;
		DecompressTileData(inDataBuffer, inTileData);

	  	std::string errorTextInComputing ;

	  	int duration_ms = 0 ;
		std::string extraStr = jstring2string(env , extra ) ;
		std::string scriptStr = jstring2string(env , script ) ;

		bool bOkComputing = v8engine.computeTile2Tile( inTileData , 
				scriptStr ,
				extraStr,
				outTileData , 
				errorTextInComputing , 
				duration_ms
				) ;
		if( bOkComputing==false ){
			std::cout<<"Error at computeTile2Tile:"<<errorTextInComputing<<std::endl;
		}else {
			std::cout<<"debug success at computeTile2Tile"<<std::endl;
		}
	}
	return GenerateOutputJByteArray(env , obj ,type,script,outTileData ) ;
  	///@return tile data. support one band(byte,short,int,float), three band(byte), four band(byte).
}



