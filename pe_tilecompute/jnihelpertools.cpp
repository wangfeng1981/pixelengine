//jnihelpertools.cpp
#include "jnihelpertools.h"


bool JniHelperTools::copyJbyteArray2VectorData(JNIEnv* env,jbyteArray jbyteArr, vector<unsigned char>& retData)
{
	jbyte* barr = (jbyte*)env->GetByteArrayElements(jbyteArr,0) ;
	jsize  blen = env->GetArrayLength(jbyteArr) ;
	if( blen==0 ){
		return false;
	}
	retData.resize(blen);
	memcpy( retData.data() , barr , blen ) ;
	env->ReleaseByteArrayElements(jbyteArr,barr,0) ;
	return true;
}

/// 失败返回 NULL
jbyteArray JNICALL JniHelperTools::createJbyteArrayByVectorData(JNIEnv* env,vector<unsigned char>& indata )
{
	if( indata.size()==0 ) return NULL ;
	jbyteArray jarr = env->NewByteArray(indata.size());
	const signed char* sptr = (signed char*)indata.data();
	env->SetByteArrayRegion(jarr ,0, indata.size() , sptr );
	return jarr ;
}