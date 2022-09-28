//jnihelpertools.h

#ifndef JNI_HELPER_TOOLS_H
#define JNI_HELPER_TOOLS_H
//2022-3-23
//2022-3-26
//2022-4-3

#include <jni.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>

using std::string;
using std::vector;
using std::cout;
using std::endl;

struct JniHelperTools {

public:

	bool copyJbyteArray2VectorData(JNIEnv* env,jbyteArray jbyteArr, vector<unsigned char>& retData) ;
	
	/// 失败返回 NULL
	jbyteArray JNICALL createJbyteArrayByVectorData(JNIEnv* env,vector<unsigned char>& indata ) ;

} ;




#endif
