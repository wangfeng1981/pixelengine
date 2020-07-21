

#include <string>
#include <vector>
#include <iostream>
#include "lodepng.h"
#include "com_pixelengine_V8Helper.h"
#include "PixelEngine.h"
#include <random>
using namespace std;
 

// debug function , it should be remove after debug
extern void Dataset2Png(unsigned char* dataptr,int width,int height,vector<unsigned char>& retpngbinary);
//bool g_javaStaffInited = false ;
// jclass JavaV8HelperClass ;
// jmethodID methodidTile ;
// jobject javaV8Helper ;

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

jstring cstring2jstring(JNIEnv *env, 
	const char* str) {
	return env->NewStringUTF(str) ;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reserved){
	cout<<"jni onload 2"<<endl; 
	PixelEngine::initV8() ;
    return JNI_VERSION_1_4;
}

string int2cstr(int ival)
{
	char buff[32] ;
	sprintf(buff , "%d" , ival ) ;
	return string(buff) ;
}

//deprecated , use GetTileDataFromJava
//get external data 
bool GetDataFromJava(
		void* pePtr,//PixelEngine instance pointer
		string name,//name
		string datetime,//datetime
		vector<int>& bands,//bands [0,1,2]
		vector<unsigned char>& retbinary,//return binary
		int& dt,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands )//return nbands
{
	cout<<"TileComputeV8Helper GetDataFromJava"<<endl; 

	
	PixelEngine* pixelEnginePointer = (PixelEngine*)pePtr ;
		int tilex = pixelEnginePointer->tileInfo.x ;
		int tiley = pixelEnginePointer->tileInfo.y ;
		int tilez = pixelEnginePointer->tileInfo.z ;


	JNIEnv* env = (JNIEnv*)pixelEnginePointer->extraPointer ;//java environment
	jclass	JavaV8HelperClass = (env)->FindClass("com/pixelengine/V8Helper");
	jmethodID	methodidTile = (env)->GetMethodID(JavaV8HelperClass,"GetTileData"
    	,"(Ljava/lang/String;Ljava/lang/String;III)[B");
	jobject	javaV8Helper = env->AllocObject(JavaV8HelperClass);
	
    cout<<"jni 4"<<endl; 
    jbyteArray tileByteArray = (jbyteArray) env->CallObjectMethod(
    	javaV8Helper,methodidTile
    	,cstring2jstring(env,name.c_str())
    	,cstring2jstring(env,datetime.c_str()) 
    	,tilez 
    	,tiley
    	,tilex
    	) ;
    if( tileByteArray== NULL ){
    	cout<<"in c++ tilebytearray is null, return null."<<endl ;
    	return false ;
    }
    cout<<"jni 5"<<endl; 
    size_t length = (size_t) env->GetArrayLength(tileByteArray);
    cout<<"len "<<length<<endl ;
    jbyte* pBytes = env->GetByteArrayElements(tileByteArray, NULL);

    dt = 3 ;
    wid = 256 ;
    hei = 256 ;
    nbands = 6 ;

    retbinary.resize(length) ;
    memcpy( retbinary.data(), (unsigned char*)pBytes , length ) ;

    env->ReleaseByteArrayElements(tileByteArray, pBytes, JNI_ABORT);
	return true ;
}


//get external data
bool GetTileDataFromJava(
		void* pePtr,//PixelEngine instance pointer
		string name,//name
		string datetime,//datetime
		vector<int>& bands,//bands [0,1,2] 
		int tilez,
		int tiley,
		int tilex,
		vector<unsigned char>& retbinary,//return binary
		int& dt,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands )//return nbands
{
	cout<<"TileComputeV8Helper GetTileDataFromJava"<<endl; 

	
	PixelEngine* pixelEnginePointer = (PixelEngine*)pePtr ;

	JNIEnv* env = (JNIEnv*)pixelEnginePointer->extraPointer ;//java environment
	jclass	JavaV8HelperClass = (env)->FindClass("com/pixelengine/V8Helper");
	jmethodID	methodidTile = (env)->GetMethodID(JavaV8HelperClass,"GetTileData"
    	,"(Ljava/lang/String;Ljava/lang/String;[IIII)Lcom/pixelengine/TileResult;");
	jobject	javaV8Helper = env->AllocObject(JavaV8HelperClass);
	
	jintArray jbandarr = env->NewIntArray(bands.size()) ;
	env->SetIntArrayRegion(jbandarr,0,bands.size(), (int*) bands.data() ) ;
    
    jobject resultobj = (jobject) env->CallObjectMethod(
    	javaV8Helper,methodidTile
    	,cstring2jstring(env,name.c_str())
    	,cstring2jstring(env,datetime.c_str()) 
    	,jbandarr
    	,tilez 
    	,tiley
    	,tilex
    	) ;
    if( resultobj== NULL ){
    	cout<<"in c++ resultobj is null, return null."<<endl ;
    	return false ;
    }
    cout<<"jni 5"<<endl; 

    jclass resultClass = env->FindClass("com/pixelengine/TileResult") ;

    jfieldID dtypefid = env->GetFieldID(resultClass,"dataType","I");
    jfieldID nbandfid = env->GetFieldID(resultClass,"nband","I");
    jfieldID numdsfid = env->GetFieldID(resultClass,"numds","I");
    jfieldID dtarrayfid = env->GetFieldID(resultClass,"datetimeArray","[J") ;//long for J
    jfieldID dataarrayfid = env->GetFieldID(resultClass,"tiledataArray","[[B") ;
    jfieldID computeOncedataId = env->GetFieldID(resultClass,"computeOnceData","Ljava/lang/String;") ;

    dt = env->GetIntField(resultobj, dtypefid) ;
    nbands = env->GetIntField(resultobj, nbandfid) ;
    //numds = env->GetIntField(resultobj, numdsfid) ;
    wid = 256 ;
    hei = 256 ;
	printf("dt %d, nband %d, numds %d\n" , dt , nbands , 1 ) ;

    jobject dtarrayobj = env->GetObjectField(resultobj, dtarrayfid) ;
    jobject dataarrayobj = env->GetObjectField(resultobj, dataarrayfid) ;

    jstring codataJStr =  (jstring) env->GetObjectField(resultobj, computeOncedataId);
    string codataCStr = jstring2string(env , codataJStr) ;
    cout<<"in cpp get computeOnceData:"<<codataCStr<<endl ;

    jlongArray* dtarrayPtr = (jlongArray*)(&dtarrayobj) ;
    jobjectArray* dataarrayPtr = (jobjectArray*)(&dataarrayobj) ;

    //int* dtarrPtr = env->GetIntArrayElements(*dtarrayPtr,NULL);
    //dtArr.resize(numds) ;
    //retbinaryArr.resize(numds) ;

	jobject jobj1 = env->GetObjectArrayElement(*dataarrayPtr, 0) ;
	jbyteArray* jbarrPtr = (jbyteArray*)(&jobj1) ;
	size_t length1 = env->GetArrayLength(*jbarrPtr) ;
	jbyte* pbytes = env->GetByteArrayElements(*jbarrPtr,NULL) ;
	retbinary.resize(length1) ;
	memcpy( retbinary.data() , (unsigned char*)pbytes , length1 ) ;
	env->ReleaseByteArrayElements(*jbarrPtr, pbytes, JNI_ABORT);

	return true ;
}





//get external data array
bool GetTileDataArrayFromJava(
		void* pePtr,//PixelEngine instance pointer
		string name,//name
		string from,//datetime
		string to , 
		vector<int>& bands,//bands [0,1,2] 
		int tilez,
		int tiley,
		int tilex,
		int filtermon , 
		int filterday , 
		int filterhour , 
		int filterminu , 
		int filtersec , 
		vector<vector<unsigned char> >& retbinaryArr,//return binary
		vector<long>& dtArr,
		int& dt,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands,
		int& numds )//return nbands
{
	cout<<"TileComputeV8Helper GetTileDataArrayFromJava"<<endl; 
	PixelEngine* pixelEnginePointer = (PixelEngine*)pePtr ;

	JNIEnv* env = (JNIEnv*)pixelEnginePointer->extraPointer ;//java environment
	jclass	JavaV8HelperClass = (env)->FindClass("com/pixelengine/V8Helper");
	jmethodID	methodidTile = (env)->GetMethodID(JavaV8HelperClass,"GetTileDataArray"
    	,"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[IIIIIIIII)Lcom/pixelengine/TileResult;");
	jobject	javaV8Helper = env->AllocObject(JavaV8HelperClass);

	jintArray jbandarr = env->NewIntArray(bands.size()) ;
	env->SetIntArrayRegion(jbandarr,0,bands.size(), (int*) bands.data() ) ;

	jobject resultobj = (jobject) env->CallObjectMethod(javaV8Helper,methodidTile
		,cstring2jstring(env,name.c_str())
    	,cstring2jstring(env,from.c_str()) 
    	,cstring2jstring(env,to.c_str()) 
    	,jbandarr
    	,tilez,tiley,tilex
    	,filtermon,filterday,filterhour,filterminu,filtersec
		) ;
	
    if( resultobj== NULL ){
    	cout<<"in c++ GetTileDataArrayFromJava is null, return null."<<endl ;
    	return false ;
    }
    cout<<"jni 51"<<endl; 

    jclass resultClass = env->FindClass("com/pixelengine/TileResult") ;

    jfieldID dtypefid = env->GetFieldID(resultClass,"dataType","I");
    jfieldID nbandfid = env->GetFieldID(resultClass,"nband","I");
    jfieldID numdsfid = env->GetFieldID(resultClass,"numds","I");
    jfieldID dtarrayfid = env->GetFieldID(resultClass,"datetimeArray","[J") ;
    jfieldID dataarrayfid = env->GetFieldID(resultClass,"tiledataArray","[[B") ;

    dt = env->GetIntField(resultobj, dtypefid) ;
    nbands = env->GetIntField(resultobj, nbandfid) ;
    numds = env->GetIntField(resultobj, numdsfid) ;
    wid = 256 ;
    hei = 256 ;

    printf("dt %d, nband %d, numds %d\n" , dt , nbands , numds ) ;

    jobject dtarrayobj = env->GetObjectField(resultobj, dtarrayfid) ;
    jobject dataarrayobj = env->GetObjectField(resultobj, dataarrayfid) ;

    jlongArray* dtarrayPtr = (jlongArray*)(&dtarrayobj) ;
    jobjectArray* dataarrayPtr = (jobjectArray*)(&dataarrayobj) ;

    long* dtarrPtr = env->GetLongArrayElements(*dtarrayPtr,NULL);
    dtArr.resize(numds) ;
    retbinaryArr.resize(numds) ;

	for(int ids = 0 ; ids < numds ; ++ ids )
	{
		dtArr[ids] = dtarrPtr[ids] ;
		jobject jobj1 = env->GetObjectArrayElement(*dataarrayPtr, ids) ;
		jbyteArray* jbarrPtr = (jbyteArray*)(&jobj1) ;
		size_t length1 = env->GetArrayLength(*jbarrPtr) ;
		jbyte* pbytes = env->GetByteArrayElements(*jbarrPtr,NULL) ;
		retbinaryArr[ids].resize(length1) ;
		memcpy( retbinaryArr[ids].data() , (unsigned char*)pbytes , length1 ) ;
		env->ReleaseByteArrayElements(*jbarrPtr, pbytes, JNI_ABORT);
	}
    env->ReleaseLongArrayElements(*dtarrayPtr, dtarrPtr, JNI_ABORT);
    env->DeleteLocalRef(jbandarr) ;
	return true ;
}




//get external color ramp
PixelEngineColorRamp GetColorRampFromJava(void* pePtr,string colorid )
{
	cout<<"in c++ TileComputeV8Helper GetColorRampFromJava"<<endl; 

	PixelEngine* pixelEnginePointer = (PixelEngine*)pePtr ;
	JNIEnv* env = (JNIEnv*)pixelEnginePointer->extraPointer ;//java environment
	jclass	JavaV8HelperClass = (env)->FindClass("com/pixelengine/V8Helper");
	jmethodID	methodidColorRamp = (env)->GetMethodID(JavaV8HelperClass,"GetColorRamp"
    	,"(Ljava/lang/String;)Lcom/pixelengine/ColorRamp;");
	jobject	javaV8Helper = env->AllocObject(JavaV8HelperClass);

	PixelEngineColorRamp cr; 
    
    jobject resultobj = (jobject) env->CallObjectMethod(
    	javaV8Helper,methodidColorRamp
    	,cstring2jstring(env,colorid.c_str())
    	) ;
    if( resultobj== NULL ){
    	cout<<"Error: in c++ returned colorRamp from java is null, return empty cr."<<endl ;
    	
    	return cr ;
    }

    jclass resultClass = env->FindClass("com/pixelengine/ColorRamp") ;

    jfieldID ivaluesid = env->GetFieldID(resultClass,"ivalues","[I") ;//long for J
    jfieldID rid = env->GetFieldID(resultClass,"r","[B") ;
    jfieldID gid = env->GetFieldID(resultClass,"g","[B") ;
    jfieldID bid = env->GetFieldID(resultClass,"b","[B") ;
    jfieldID aid = env->GetFieldID(resultClass,"a","[B") ;

    jfieldID nodataid = env->GetFieldID(resultClass,"Nodata","D") ;
    jfieldID nodatacolorid = env->GetFieldID(resultClass,"NodataColor","[B") ;

    cr.Nodata = env->GetDoubleField(resultobj, nodataid) ;
    
    jobject ivalobj = env->GetObjectField(resultobj, ivaluesid) ;
    jobject robj = env->GetObjectField(resultobj, rid) ;
    jobject gobj = env->GetObjectField(resultobj, gid) ;
    jobject bobj = env->GetObjectField(resultobj, bid) ;
    jobject aobj = env->GetObjectField(resultobj, aid) ;
    jobject ndcolorobj = env->GetObjectField(resultobj, nodatacolorid) ;

    jintArray* ivalptr = (jintArray*)(&ivalobj) ;
    jbyteArray* rptr = (jbyteArray*)(&robj) ;
    jbyteArray* gptr = (jbyteArray*)(&gobj) ;
    jbyteArray* bptr = (jbyteArray*)(&bobj) ;
    jbyteArray* aptr = (jbyteArray*)(&aobj) ;
    jbyteArray* ndcolorptr = (jbyteArray*)(&ndcolorobj) ;

    cr.numColors = env->GetArrayLength(*ivalptr) ;

    int* ivalptr2 = env->GetIntArrayElements(*ivalptr,NULL);
    jbyte* rptr2 = env->GetByteArrayElements(*rptr,NULL) ;
    jbyte* gptr2 = env->GetByteArrayElements(*gptr,NULL) ;
    jbyte* bptr2 = env->GetByteArrayElements(*bptr,NULL) ;
    jbyte* aptr2 = env->GetByteArrayElements(*aptr,NULL) ;
    jbyte* ndcolorptr2 = env->GetByteArrayElements(*ndcolorptr,NULL) ;

    for(int ic = 0 ; ic < cr.numColors ; ++ ic )
    {
    	cr.ivalues[ic] = ivalptr2[ic] ;
    	cr.r[ic] = (unsigned char)(rptr2[ic]) ;
    	cr.g[ic] = (unsigned char)(gptr2[ic]) ;
    	cr.b[ic] = (unsigned char)(bptr2[ic]) ;
    	cr.a[ic] = (unsigned char)(aptr2[ic]) ;
    }

    cr.NodataColor[0]=(unsigned char)(ndcolorptr2[0]) ;
    cr.NodataColor[1]=(unsigned char)(ndcolorptr2[1]) ;
    cr.NodataColor[2]=(unsigned char)(ndcolorptr2[2]) ;
    cr.NodataColor[3]=(unsigned char)(ndcolorptr2[3]) ;

    env->ReleaseIntArrayElements(*ivalptr, ivalptr2, JNI_ABORT);
    env->ReleaseByteArrayElements(*rptr, rptr2, JNI_ABORT);
    env->ReleaseByteArrayElements(*gptr, gptr2, JNI_ABORT);
    env->ReleaseByteArrayElements(*bptr, bptr2, JNI_ABORT);
    env->ReleaseByteArrayElements(*aptr, aptr2, JNI_ABORT);
    env->ReleaseByteArrayElements(*ndcolorptr, ndcolorptr2, JNI_ABORT);

	return cr ;
}


 
JNIEXPORT jbyteArray JNICALL Java_com_pixelengine_V8Helper_CallTileCompute
  (JNIEnv *env, jobject obj, jstring script, jlong current, jint z, jint y, jint x)
{
 
	string jsSource = jstring2string(env,script) ;
	if( PixelEngine::GetExternalTileDataArrCallBack ==nullptr )
	{
		//PixelEngine::GetExternalDatasetCallBack = GetDataFromJava ;//deprecated
		PixelEngine::GetExternalTileDataCallBack = GetTileDataFromJava ;//will be deprecated 
		PixelEngine::GetExternalTileDataArrCallBack = GetTileDataArrayFromJava ;
		PixelEngine::GetExternalColorRampCallBack = GetColorRampFromJava ;
	}
	vector<unsigned char> retbinary ;
	PixelEngine pe ;
	bool runok = pe.RunScriptForTile(env,jsSource,current,z,y,x,retbinary) ;
	if( runok )
	{
		jbyteArray retval = env->NewByteArray(retbinary.size());
		const signed char* sptr = (signed char*)retbinary.data();
		env->SetByteArrayRegion(retval,0, retbinary.size() , sptr );
		return retval ;
	}else
	{
		return nullptr ;
	}
	return nullptr ;
}


//output com.pixelengine.V8HelperResult
// package com.pixelengine;
// public class V8HelperResult {
//     String logs ;//logs
//     byte[] data ;//png binary
// }
JNIEXPORT jobject JNICALL Java_com_pixelengine_V8Helper_CallTileComputeV2
  (JNIEnv *env, jobject obj, jstring script, jlong current, jint z, jint y, jint x)
{
	printf("debug using CallTileComputeV2\n") ;
	jclass	javaV8HelperResultClass = (env)->FindClass("com/pixelengine/V8HelperResult");
	jobject	javaV8HelperResult = env->AllocObject(javaV8HelperResultClass);
	jfieldID logsid = env->GetFieldID(javaV8HelperResultClass,"logs","Ljava/lang/String;") ;
    jfieldID dataid = env->GetFieldID(javaV8HelperResultClass,"data","[B") ;
 
	string jsSource = jstring2string(env,script) ;
	if( PixelEngine::GetExternalTileDataArrCallBack ==nullptr )
	{
		PixelEngine::GetExternalTileDataCallBack = GetTileDataFromJava ;//will be deprecated 
		PixelEngine::GetExternalTileDataArrCallBack = GetTileDataArrayFromJava ;
		PixelEngine::GetExternalColorRampCallBack = GetColorRampFromJava ;
	}
	vector<unsigned char> retbinary ;
	PixelEngine pe ;
	bool runok = pe.RunScriptForTile(env,jsSource,current,z,y,x,retbinary) ;
	if( runok )
	{
		jbyteArray retval = env->NewByteArray(retbinary.size());
		const signed char* sptr = (signed char*)retbinary.data();
		env->SetByteArrayRegion(retval,0, retbinary.size() , sptr );

		env->SetObjectField( javaV8HelperResult, logsid, cstring2jstring(env,pe.pe_logs.c_str()) ) ;
		env->SetObjectField( javaV8HelperResult, dataid, retval ) ;
		return javaV8HelperResult ;
	}else
	{
		env->SetObjectField( javaV8HelperResult, logsid, cstring2jstring(env,pe.pe_logs.c_str()) ) ;
		return javaV8HelperResult ;
	}
}



JNIEXPORT jstring JNICALL Java_com_pixelengine_V8Helper_Version
  (JNIEnv *env, jobject obj)
{
	return cstring2jstring( env , "2.1" ) ;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	cout<<"jni unload"<<endl ;
    return;
}


JNIEXPORT jstring JNICALL Java_com_pixelengine_V8Helper_CheckScriptOK
  (JNIEnv *env, jobject obj, jstring source)
{
	string jsSource = jstring2string(env,source) ;
	if( jsSource.length() < 1 ){
		return cstring2jstring( env , "Error: source is empty." ) ;
	}else
	{
		PixelEngine pe ;
		string errorinfo = pe.CheckScriptOk(jsSource) ;
		return cstring2jstring( env , errorinfo.c_str() ) ;
	}
	
}

JNIEXPORT jstring JNICALL Java_com_pixelengine_V8Helper_ComputeOnce
  (JNIEnv * env, jobject obj, jstring source)
{
	string nothing = "{\"computeonce\":\"nothing runs.\"}" ;
	return cstring2jstring( env , nothing.c_str() ) ;
}


// debug function , it should be remove after debug
void Dataset2Png(unsigned char* dataptr,int width,int height,vector<unsigned char>& retpngbinary)
{
	const int imgsize = width * height;
	short* sptr = (short*)dataptr ;
	vector<unsigned char> rgbadata(imgsize * 4, 0);
	for (int it = 0; it < imgsize; ++it)
	{
		rgbadata[it * 4 + 0] = sptr[it]*255.f/10000 ;
		rgbadata[it * 4 + 1] = sptr[it]*255.f/10000 ;
		rgbadata[it * 4 + 2] = sptr[it]*255.f/10000 ;
		rgbadata[it * 4 + 3] = 255;
	}
	retpngbinary.clear();
	retpngbinary.reserve(512*512*4) ;
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
}

//test
int main()
{

}


