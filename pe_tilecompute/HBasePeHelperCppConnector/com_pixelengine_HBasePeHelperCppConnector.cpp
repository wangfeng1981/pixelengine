/* DO NOT EDIT THIS FILE - it is machine generated */
#include "com_pixelengine_HBasePeHelperCppConnector.h"

#include <string>
#include <vector>
#include <iostream>
#include "lodepng.h"
#include "PixelEngine.h"
#include <random>
#include "../JavaPixelEngineHelperInterface.h"
#include "../wDatasetDatetime.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "../ajson5.h"
#include "PeStyle.h"

using namespace std;
using namespace ArduinoJson;

/// 这个文件下的全局函数是对应Java中CppConnector类的成员函数
/// Java主动调用的。




jstring cstring2jstring(JNIEnv *env, 
	const char* str) {
	return env->NewStringUTF(str) ;
}




/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    GetVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_GetVersion
  (JNIEnv * env, jobject object)
{
	PixelEngine pe;
	string ver  = string("connector_version:0.1.1")+";core_version:" + pe.GetVersion() ;
	return JavaPixelEngineHelperInterface::cstring2jstring(env, ver.c_str()) ;
}

/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    RunToGetStyleFromScript
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 * return json string of Style
 */
JNIEXPORT jstring JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_RunToGetStyleFromScript
  (JNIEnv * env, jobject object, jstring javaPEHelperClassName, jstring scriptContent)
{
	printf("in Java_com_pixelengine_HBasePeHelperCppConnector_RunToGetStyleFromScript\n");
	string helperclassname = JavaPixelEngineHelperInterface::jstring2cstring(env,javaPEHelperClassName) ;
	JavaPixelEngineHelperInterface helper(env, helperclassname) ;  

	PixelEngine::initV8() ;
	PixelEngine pe ;
	pe.helperPointer = &helper ;

	string cscriptContent = JavaPixelEngineHelperInterface::jstring2cstring(env,scriptContent) ;

	string logtext ;
	pe::PeStyle retStyle;

	bool isok = pe.RunToGetStyleFromScript(cscriptContent, retStyle ,logtext);
	if( isok ){
		string stylejson = retStyle.toJson() ;
		return JavaPixelEngineHelperInterface::cstring2jstring(env,stylejson.c_str() );
	}else {
		printf("Error : failed to get style from script. %s\n" , logtext.c_str() ) ;
		return JavaPixelEngineHelperInterface::cstring2jstring(env, "" ) ;
	}
}



/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    ParseScriptForDsDt
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
{
	"dsdtarr":[] ,
	"error":"",
	"status":0   //0 for good, 1 for error	
}

 */
JNIEXPORT jstring JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_ParseScriptForDsDt
  (JNIEnv * env, jobject object , 
  	jstring javaPEHelperClassName, 
  	jstring scriptContent ) 
{
	printf("in Java_com_pixelengine_HBasePeHelperCppConnector_ParseScriptForDsDt()\n") ;

	string helperclassname = JavaPixelEngineHelperInterface::jstring2cstring(env,javaPEHelperClassName) ;
	JavaPixelEngineHelperInterface helper(env, helperclassname) ; 

	PixelEngine::initV8() ;
	PixelEngine pe ;
	pe.helperPointer = &helper ;

	string cscriptContent = JavaPixelEngineHelperInterface::jstring2cstring(env,scriptContent) ;

	vector<wDatasetDatetime> retdsdtvec;
	string errorText;
	bool ok = pe.RunScriptForDatasetDatetimePairs( nullptr ,
		cscriptContent,
		retdsdtvec ,
		errorText) ;
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	if( ok ){
		root["status"] = 0 ;//ok
		root["error"] = "" ;
		JsonArray& dsdtarr = root.createNestedArray("dsdtarr") ;
		for(int ids = 0 ; ids < retdsdtvec.size(); ++ ids ){
			JsonObject& dobj = dsdtarr.createNestedObject() ;
			dobj["ds"] = retdsdtvec[ids].ds ;
			dobj["dt0"] = retdsdtvec[ids].dt0 ;
			dobj["dt1"] = retdsdtvec[ids].dt1 ;
		}
	}else{
		root["status"] = 1 ; 
		root["error"] = errorText ;
		//root["dsdtarr"]
		root.createNestedArray("dsdtarr") ;
	}

	string jsonstr ;
	root.printTo(jsonstr) ;
	jstring jstr = JavaPixelEngineHelperInterface::cstring2jstring(env, jsonstr.c_str() ) ;
	return jstr ;
}

/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    RunScriptForTileWithoutRender
 * Signature: (Ljava/lang/String;Ljava/lang/String;JIII)Lcom/pixelengine/TileComputeResult;

 */
JNIEXPORT jobject JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_RunScriptForTileWithoutRender
  (JNIEnv * env, jobject object,
  	jstring javaPEHelperClassName, 
  	jstring scriptContent, 
  	jlong datetime, 
  	jint z, jint y, jint x)
{

	printf("in Java_com_pixelengine_HBasePeHelperCppConnector_RunScriptForTileWithoutRender()\n") ;
	jclass	javaTileComputeResultClass = (env)->FindClass("com/pixelengine/TileComputeResult");
	if( javaTileComputeResultClass == NULL )
	{
		printf("Error : not find class of com/pixelengine/TileComputeResult.");
		return NULL ;
	}
	jobject	javaResult = env->AllocObject(javaTileComputeResultClass);

	string helperclassname = JavaPixelEngineHelperInterface::jstring2cstring(env,javaPEHelperClassName) ;
	JavaPixelEngineHelperInterface helper(env, helperclassname) ; 

	PixelEngine::initV8() ;
	PixelEngine pe ;
	pe.helperPointer = &helper ;

	string cscript = JavaPixelEngineHelperInterface::jstring2cstring(env,scriptContent) ;
	PeTileData retTileData;
	string logStr;

	bool runok = pe.RunScriptForTileWithoutRender(nullptr,
		cscript , 
		datetime , 
		z , y , x , 
		retTileData ,
		logStr ) ;

	if( runok == false ){
		cout<<"run script failed : "<<logStr<<endl;

		helper.setJavaObjectIntField(javaResult,"status",1) ;//status=1 bad.
		helper.setJavaObjectStringField(javaResult,"log",logStr.c_str()) ;

	}else{
		cout<<"run script ok."<<endl ;
		helper.setJavaObjectIntField(javaResult,"status",0) ;//status=0 ok.
		helper.setJavaObjectIntField(javaResult,"outType", 0) ;//0-dataset, 1-png
		helper.setJavaObjectIntField(javaResult,"dataType", retTileData.dataType ) ;
		helper.setJavaObjectIntField(javaResult,"width",  retTileData.width ) ;
		helper.setJavaObjectIntField(javaResult,"height", retTileData.height ) ;
		helper.setJavaObjectIntField(javaResult,"nbands", retTileData.nbands ) ;
		helper.setJavaObjectByteArrField(javaResult,"binaryData", retTileData.tiledata ) ;
		helper.setJavaObjectIntField(javaResult,"z", z) ;
		helper.setJavaObjectIntField(javaResult,"y", y) ;
		helper.setJavaObjectIntField(javaResult,"x", x) ;
	}
	return javaResult ;	
}


/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    RunScriptForTileWithRender
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JIII)Lcom/pixelengine/TileComputeResult;
 */
JNIEXPORT jobject JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_RunScriptForTileWithRender
  (JNIEnv * env , jobject object , 
  	jstring javaPEHelperClassName, 
  	jstring scriptContent, 
  	jstring styleJson, 
  	jlong datetime , 
  	jint z, 
  	jint y, 
  	jint x
  	)
{
	printf("in Java_com_pixelengine_HBasePeHelperCppConnector_RunScriptForTileWithRender()\n") ;
	jclass	javaTileComputeResultClass = (env)->FindClass("com/pixelengine/TileComputeResult");
	if( javaTileComputeResultClass == NULL )
	{
		printf("Error : not find class of com/pixelengine/TileComputeResult.");
		return NULL ;
	}

	jobject	javaResult = env->AllocObject(javaTileComputeResultClass);

	string helperclassname = JavaPixelEngineHelperInterface::jstring2cstring(env,javaPEHelperClassName) ;
	JavaPixelEngineHelperInterface helper(env, helperclassname) ; 
	
	PixelEngine::initV8() ;
	PixelEngine pe ;
	pe.helperPointer = &helper ;
	string cscript = JavaPixelEngineHelperInterface::jstring2cstring(env,scriptContent) ;
	string cstyle = JavaPixelEngineHelperInterface::jstring2cstring(env,styleJson) ;

	pe::PeStyle tstyle ;
	bool styleok = tstyle.loadFromJson(cstyle) ;
	if( styleok ){

	}else{
		printf("Warning : bad style from json.\n") ;
	}
	vector<unsigned char> retPngBinary ;
	string logStr;
	int pngwid=0;
	int pnghei=0;

	bool runok = pe.RunScriptForTileWithRender(nullptr,
			cscript , 
			tstyle,
			(int64_t)datetime , 
			z , y , x , 
			retPngBinary ,
			pngwid,
			pnghei,
			logStr ) ;

	if( runok ){
		cout<<"run script ok."<<endl ;
		helper.setJavaObjectIntField(javaResult,"status",0) ;//status=0 ok.
		helper.setJavaObjectIntField(javaResult,"outType", 1) ;//0-dataset, 1-png
		helper.setJavaObjectIntField(javaResult,"dataType", 1 ) ;//useful ?
		helper.setJavaObjectIntField(javaResult,"width",  pngwid ) ;
		helper.setJavaObjectIntField(javaResult,"height", pnghei ) ;
		helper.setJavaObjectIntField(javaResult,"nbands", 4 ) ;//useful ?
		helper.setJavaObjectByteArrField(javaResult,"binaryData", retPngBinary) ;
		helper.setJavaObjectIntField(javaResult,"z", z) ;
		helper.setJavaObjectIntField(javaResult,"y", y) ;
		helper.setJavaObjectIntField(javaResult,"x", x) ;
		return javaResult;
	}else{
		printf("Error : failed to pe.RunScriptForTileWithRender.\n") ;
		helper.setJavaObjectIntField(javaResult,"status",1) ;//status=1 bad.
		helper.setJavaObjectStringField(javaResult,"log",logStr.c_str()) ;
		return javaResult ;
	}
}

/*
 * Class:     com_pixelengine_HBasePeHelperCppConnector
 * Method:    CheckScriptOk
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_pixelengine_HBasePeHelperCppConnector_CheckScriptOk
  (JNIEnv * env , jobject object , jstring javaPEHelperClassName, jstring scriptContent )
{
	string jsSource = JavaPixelEngineHelperInterface::jstring2cstring(env,scriptContent) ;
	if( jsSource.length() < 1 ){
		return JavaPixelEngineHelperInterface::cstring2jstring( env , "Error: source is empty." ) ;
	}else
	{
		PixelEngine::initV8() ;
		PixelEngine pe ;
		string errorinfo = pe.CheckScriptOk(jsSource) ;
		return JavaPixelEngineHelperInterface::cstring2jstring( env , errorinfo.c_str() ) ;
	}
}



// /// check script syntax ok
// string CheckScriptOk(string& scriptSource) ;
// /// get core version
// inline string GetVersion() { return PixelEngine::pejs_version; }
// //2020-9-13 get style from script
// bool RunToGetStyleFromScript(string& scriptContent, PeStyle& retstyle, string& retLogText);
// //2020-9-13
// //run tile compute without render, return dataset
// bool RunScriptForTileWithoutRender(void* extra, string& scriptContent, int64_t currentDatetime,
// 	int z, int y, int x, PeTileData& tileData , string& logStr);
// //run tile compute with render, return png
// bool RunScriptForTileWithRender(void* extra, string& scriptContent, PeStyle& inStyle, int64_t currentDatetime,
// 	int z, int y, int x, vector<unsigned char>& retPngBinary, string& logStr);//


// //解析Dataset-Datetime 数据集时间日期对
// bool RunScriptForDatasetDatetimePairs(void* extra,
// 	string& scriptContent,vector<wDatasetDatetime>& retDsDtVec,string& errorText);





    // const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    // size_t length = (size_t) env->GetArrayLength(stringJbytes);
    // jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    // std::string ret = std::string((char *)pBytes, length);
    // env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    // env->DeleteLocalRef(javaHelperObject);
    // env->DeleteLocalRef(javaHelperClass);
