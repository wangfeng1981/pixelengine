#include <jni.h>
#include "Demo.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

jstring cstring2jstring(JNIEnv *env, 
	char* str) {
	return env->NewStringUTF(str) ;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reserved){
	cout<<"jni onload"<<endl; 
    return JNI_VERSION_1_4;
}


JNIEXPORT jbyteArray JNICALL Java_Demo_CallTileCompute
  (JNIEnv *env , jobject obj)
{
	

	jclass DemoClass = (env)->FindClass("Demo");
    jmethodID methodid = (env)->GetMethodID(DemoClass,"GetInt","()I");
    jmethodID methodidTile = (env)->GetMethodID(DemoClass,"GetTileData"
    	,"(Ljava/lang/String;Ljava/lang/String;III)[B");
    jobject demoobj = env->AllocObject(DemoClass);
    //jint ivalue =  env->CallIntMethod( demoobj, methodid);//int ivalue2 = (int)ivalue ;
    jbyteArray tileByteArray = (jbyteArray) env->CallObjectMethod(demoobj,methodidTile,cstring2jstring(env,"fy3d")
    	,cstring2jstring(env,"20190601"),5,4,3) ;

    size_t length = (size_t) env->GetArrayLength(tileByteArray);
    jbyte* pBytes = env->GetByteArrayElements(tileByteArray, NULL);
    vector<unsigned char> data( length) ;
    for(int it = 0 ; it < 256*256 ; ++ it ) data[it] = (unsigned char)pBytes[it] + 1 ;

    env->ReleaseByteArrayElements(tileByteArray, pBytes, JNI_ABORT);

    (env)->DeleteLocalRef((jobject)tileByteArray);
    (env)->DeleteLocalRef((jobject)demoobj);
    (env)->DeleteLocalRef((jobject)DemoClass);
    (env)->DeleteLocalRef((jobject)methodid);
    (env)->DeleteLocalRef((jobject)methodidTile);


    

	jbyteArray retval = env->NewByteArray(256*256);
	const signed char* sptr = (signed char*)data.data();
	env->SetByteArrayRegion(retval,0, data.size() , sptr );


	

 


	return retval ;
}


JNIEXPORT jstring JNICALL Java_Demo_Version
  (JNIEnv *env, jobject obj)
{
	return cstring2jstring( env , "1.0" ) ;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	cout<<"jni unload"<<endl ;
    return;
}


//test
int main()
{

}


