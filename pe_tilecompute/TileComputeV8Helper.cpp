

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


 
JNIEXPORT jbyteArray JNICALL Java_com_pixelengine_V8Helper_CallTileCompute
  (JNIEnv *env, jobject obj, jstring script, jint current, jint z, jint y, jint x)
{
 
	string jsSource = jstring2string(env,script) ;
	if( PixelEngine::GetExternalDatasetCallBack ==nullptr )
	{
		PixelEngine::GetExternalDatasetCallBack = GetDataFromJava ;
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

	


 //    vector<unsigned char> retbinary ;
 //    Dataset2Png( data.data() , 256 , 256 , retbinary) ;

	// jbyteArray retval = env->NewByteArray(retbinary.size());
	// const signed char* sptr = (signed char*)retbinary.data();
	// env->SetByteArrayRegion(retval,0, retbinary.size() , sptr );

	return nullptr ;
}


JNIEXPORT jstring JNICALL Java_com_pixelengine_V8Helper_Version
  (JNIEnv *env, jobject obj)
{
	return cstring2jstring( env , "1.0" ) ;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	cout<<"jni unload"<<endl ;
    return;
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


