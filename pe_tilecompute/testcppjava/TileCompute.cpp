
#include "Demo.h"
#include <string>
#include <vector>
#include <iostream>
#include "lodepng.h"
#include "com_pixelengine_V8Helper.h"
using namespace std;

// debug function , it should be remove after debug
extern void Dataset2Png(unsigned char* dataptr,int width,int height,vector<unsigned char>& retpngbinary);
//bool g_javaStaffInited = false ;
// jclass JavaV8HelperClass ;
// jmethodID methodidTile ;
// jobject javaV8Helper ;


jstring cstring2jstring(JNIEnv *env, 
	char* str) {
	return env->NewStringUTF(str) ;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reserved){
	cout<<"jni onload 2"<<endl; 
    return JNI_VERSION_1_4;
}

 
JNIEXPORT jbyteArray JNICALL Java_com_pixelengine_V8Helper_CallTileCompute
  (JNIEnv *env, jobject obj, jstring script, jint current, jint z, jint y, jint x)
{

	jclass	JavaV8HelperClass = (env)->FindClass("com/pixelengine/V8Helper");
	jmethodID	methodidTile = (env)->GetMethodID(JavaV8HelperClass,"GetTileData"
    	,"(Ljava/lang/String;Ljava/lang/String;III)[B");
	jobject	javaV8Helper = env->AllocObject(JavaV8HelperClass);
	
    cout<<"jni 4"<<endl; 
    jbyteArray tileByteArray = (jbyteArray) env->CallObjectMethod(javaV8Helper,methodidTile
    	,cstring2jstring(env,"fy3dtable")
    	,cstring2jstring(env,"20190601"),z,y,x) ;
    if( tileByteArray== NULL ){
    	cout<<"in c++ tilebytearray is null, return null."<<endl ;
    	return NULL ;
    }
    cout<<"jni 5"<<endl; 
    size_t length = (size_t) env->GetArrayLength(tileByteArray);
    cout<<"len "<<length<<endl ;
    jbyte* pBytes = env->GetByteArrayElements(tileByteArray, NULL);

    //int length = 256*256 ;
    vector<unsigned char> data( length) ;
    for(int it = 0 ; it < length ; ++ it ) 
    	data[it] = (unsigned char)pBytes[it]  ;

    //give it to v8 here

    env->ReleaseByteArrayElements(tileByteArray, pBytes, JNI_ABORT);


    vector<unsigned char> retbinary ;
    Dataset2Png( data.data() , 256 , 256 , retbinary) ;

	jbyteArray retval = env->NewByteArray(retbinary.size());
	const signed char* sptr = (signed char*)retbinary.data();
	env->SetByteArrayRegion(retval,0, retbinary.size() , sptr );

	return retval ;
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


