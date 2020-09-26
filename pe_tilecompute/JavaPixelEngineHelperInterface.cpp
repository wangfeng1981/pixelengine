#include "JavaPixelEngineHelperInterface.h"


bool JavaPixelEngineHelperInterface::getTileData(int64_t dt, string& dsName, vector<int> bandindices,
	int z, int y, int x, vector<unsigned char>& retTileData, 
	int& dataType,
	int& wid,
	int& hei,
	int& nbands,
	string& errorText)
{
	cout<<"JavaPixelEngineHelperInterface::getTileData"<<endl;
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::getTileData env is null"<<endl ;
		return false;
	}

	jclass	javaHelperClass =  env->FindClass("com/pixelengine/HBasePixelEngineHelper");
	jobject	javaHelperObject = env->AllocObject(javaHelperClass);
	
	//Signature: (Ljava/lang/String;JIII)Lcom/pixelengine/TileComputeResult;
	//TileData getTileData( long dt, String dsName,int[] bandindices,int z,int y,int x)
	jmethodID getTileDataMethodId = env->GetMethodID(javaHelperClass, 
		"getTileData", 
		"(JLjava/lang/String;[IIII)Lcom/pixelengine/TileData;");
    
	jintArray jbands = env->NewIntArray(bandindices.size()) ;
	for(int ib = 0 ; ib < bandindices.size() ; ++ ib ){
		int bandindex = bandindices[ib];
		env->SetIntArrayRegion(
			jbands, ib, 1, &bandindex );
	}

    //TileData getTileData( long dt, String dsName,int[] bandindices,int z,int y,int x)
	jobject tileDataJavaObj = (jobject) env->CallObjectMethod(javaHelperObject,
		getTileDataMethodId,
		dt,
		env->NewStringUTF(dsName.c_str()) , 
		jbands , 
		z,y,x
		);

	if( tileDataJavaObj== nullptr ){
		cout<<"Error : tileDataJavaObj is null at z,y,x "<<
			z<<","<<y<<","<<x<<endl ;
		return false ;
	}
	int numds = 0;
	vector<int64_t> dtVec ;
	vector<vector<unsigned char> > dataVecOfVec ;
	bool isok = this->unwrapJavaTileData(tileDataJavaObj , 
		dtVec , 
		dataVecOfVec , 
		wid,
		hei,
		nbands,
		numds , 
		dataType , 
		x,y,z
		) ;
	if( isok ==false ){
		cout<<"Error : unwrap tileDataJavaObj failed."<<endl ;
		return false ;
	}

	cout<<"Info : unwrap ok."<<endl ;

	cout<<"w,h,nb,nds,dt:"<<wid<<","<<hei<<","<<nbands<<","<<numds<<","<<dataType<<endl;
	cout<<"z,y,x:"<<z<<","<<y<<","<<x<<endl ;
	cout<<"data[1]:"<<(int)dataVecOfVec[0][1]<<endl ;

	env->DeleteLocalRef(javaHelperObject);
    env->DeleteLocalRef(javaHelperClass);
 
	return true;
}


bool JavaPixelEngineHelperInterface::getTileDataArray( 
	int64_t fromdtInclusive, int64_t todtInclusive,
	string& dsName, vector<int> bandindices, int z, int y, int x,
	int filterMonth,int filterDay,int filterHour,int filterMinu,
	int filterSec ,
	vector<vector<unsigned char>>& retTileDataArr,
	vector<int64_t>& dtArr ,
	int& dataType,
	int& wid,
	int& hei,
	int& nbands,
	string& errorText)
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::getTileDataArray env is null"<<endl ;
		return false;
	}
	jclass	javaHelperClass =  env->FindClass("com/pixelengine/HBasePixelEngineHelper");
	jobject	javaHelperObject = env->AllocObject(javaHelperClass);
	
	jmethodID methodId = env->GetMethodID(javaHelperClass, 
		"getTileDataArray", 
		"(JJLjava/lang/String;[IIIIIIIII)Lcom/pixelengine/TileData;");
    
	jintArray jbands = env->NewIntArray(bandindices.size()) ;
	for(int ib = 0 ; ib < bandindices.size() ; ++ ib ){
		int bandindex = bandindices[ib];
		env->SetIntArrayRegion(
			jbands, ib, 1, &bandindex );
	}

	jobject tileDataJavaObj = (jobject) env->CallObjectMethod(javaHelperObject,
		methodId,
		fromdtInclusive,
		todtInclusive,
		env->NewStringUTF(dsName.c_str()) , 
		jbands , 
		z,y,x,
		filterMonth,
		filterDay,
		filterHour,
		filterMinu,
		filterSec
		);

	if( tileDataJavaObj== nullptr ){
		cout<<"Error : tileDataJavaObj is null at z,y,x "<<
			z<<","<<y<<","<<x<<endl ;
		return false ;
	}
	int numds = 0;
 
	bool isok = this->unwrapJavaTileData(tileDataJavaObj , 
		dtArr , 
		retTileDataArr , 
		wid,
		hei,
		nbands,
		numds , 
		dataType , 
		x,y,z
		) ;
	if( isok ==false ){
		cout<<"Error : unwrap tileDataJavaObj failed."<<endl ;
		return false ;
	}

	cout<<"Info : unwrap ok."<<endl ;

	cout<<"w,h,nb,nds,dt:"<<wid<<","<<hei<<","<<nbands<<","<<numds<<","<<dataType<<endl;
	cout<<"z,y,x:"<<z<<","<<y<<","<<x<<endl ;
	for(int ids=0;ids<numds;++ids){
		cout<<"data["<<ids<<"][1]:"<<(int)retTileDataArr[ids][1]<<endl ;
	}

	env->DeleteLocalRef(javaHelperObject);
    env->DeleteLocalRef(javaHelperClass);
	return true;
}


bool JavaPixelEngineHelperInterface::getColorRamp(string& crid ,
	PixelEngineColorRamp& crobj ,
	string& errorText)
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::getColorRamp env is null"<<endl ;
		return false;
	}

	jclass	javaHelperClass =  env->FindClass("com/pixelengine/HBasePixelEngineHelper");
	jobject	javaHelperObject = env->AllocObject(javaHelperClass);
	
	//Signature: (Ljava/lang/String;)Lcom/pixelengine/ColorRamp;
	// public ColorRamp getColorRamp( String crid )
	jmethodID methodID = env->GetMethodID(javaHelperClass, 
		"getColorRamp", 
		"(Ljava/lang/String;)Lcom/pixelengine/ColorRamp;");

	jobject jo = (jobject) env->CallObjectMethod(javaHelperObject,
		methodID,
		env->NewStringUTF(crid.c_str())
		);

	if( jo== nullptr ){
		cout<<"Error : crobj is null for ColorRamp id "<<crid<<endl ;
		return false ;
	}

	bool isok = this->unwrapJavaColorRamp(jo ,crobj) ;
	if( isok ==false ){
		cout<<"Error : unwrap ColorRamp failed."<<endl ;
		return false ;
	}
	cout<<"Info : unwrap ColorRamp ok."<<endl ;
	env->DeleteLocalRef(javaHelperObject);
    env->DeleteLocalRef(javaHelperClass);
 
	return true;
}


bool JavaPixelEngineHelperInterface::getStyle(string& styleid, PeStyle& retStyle, string& errorText) 
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::getStyle env is null"<<endl ;
		return false;
	}
	jclass	javaHelperClass =  env->FindClass("com/pixelengine/HBasePixelEngineHelper");
	jobject	javaHelperObject = env->AllocObject(javaHelperClass);
	
	jmethodID methodID = env->GetMethodID(javaHelperClass, 
		"getStyle", 
		"(Ljava/lang/String;)Lcom/pixelengine/JPeStyle;");

	jobject jo = (jobject) env->CallObjectMethod(javaHelperObject,
		methodID,
		env->NewStringUTF(styleid.c_str())
		);
	if( jo == NULL ){
		cout<<"Error : jo is null for JPeStyle id "<<styleid<<endl ;
		return false ;
	}

	bool isok = this->unwrapJavaStyle(jo ,retStyle) ;
	if( isok ==false ){
		cout<<"Error : unwrap JPeStyle failed."<<endl ;
		return false ;
	}
	cout<<"Info : unwrap JPeStyle ok."<<endl ;
	env->DeleteLocalRef(javaHelperObject);
    env->DeleteLocalRef(javaHelperClass);
	return true;
}



bool JavaPixelEngineHelperInterface::unwrapJavaColorRamp(jobject obj,
	PixelEngineColorRamp& cr )
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::unwrapJavaColorRamp env is null"<<endl ;
		return false;
	}
	vector<string> lv ;
	vector<int> vv ;
	vector<unsigned char> rv,gv,bv,av,nc;
	bool ok1 = this->getJavaObjectIntArrField(obj,"ivalues",vv) ;
	bool ok2 = this->getJavaObjectByteArrField(obj,"r" , rv) ;
	ok2 = this->getJavaObjectByteArrField(obj,"g" , gv) ;
	ok2 = this->getJavaObjectByteArrField(obj,"b" , bv) ;
	ok2 = this->getJavaObjectByteArrField(obj,"a" , av) ;
	ok2 = this->getJavaObjectStrArrField(obj,"labels" , lv) ;
	ok2 = this->getJavaObjectDoubleField(obj,"Nodata", cr.Nodata) ;
	ok2 = this->getJavaObjectByteArrField(obj,"NodataColor", nc) ;
	ok2 = this->getJavaObjectStringField(obj,"NodataLabel" , cr.NodataLabel ) ;
	for(int ic=0;ic<rv.size();++ic){
		cr.ivalues[ic] = vv[ic] ;
		cr.r[ic] = rv[ic] ;
		cr.g[ic] = gv[ic] ;
		cr.b[ic] = bv[ic] ;
		cr.a[ic] = av[ic] ;
		cr.labels[ic] = lv[ic] ;
	}
	cr.numColors = rv.size() ;
	for(int i=0;i<4;++i ){
		cr.NodataColor[i] = nc[i] ;
	}
	return true;
}

bool JavaPixelEngineHelperInterface::unwrapJavaStyle(jobject obj,
	pe::PeStyle& style ) 
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::unwrapJavaStyle env is null"<<endl ;
		return false;
	}
	jclass someClass = env->GetObjectClass(obj);

	bool ok1 = this->getJavaObjectIntArrField(obj,"bands",style.bands) ;
	ok1 = this->getJavaObjectStringField(obj,"type" , style.type ) ;

	jfieldID colorsid = env->GetFieldID( someClass, "colors", 
		"[Lcom/pixelengine/JPeColorElement;");
	jfieldID nodatacolorid = env->GetFieldID( someClass, "nodatacolor", 
		"Lcom/pixelengine/JPeColorElement;");
	jfieldID vrangesid = env->GetFieldID( someClass, "vranges", 
		"[Lcom/pixelengine/JPeVRangeElement;");

	if (NULL == colorsid || NULL== nodatacolorid || NULL==vrangesid) {
			cout<<"Error : failed to get field colorsid/nodatacolorid/vrangesid."<<endl;
			return false;
	}

	jobjectArray jo_colors =(jobjectArray) env->GetObjectField( obj, colorsid);
	jobject jo_nodatacolor = env->GetObjectField( obj, nodatacolorid);
	jobjectArray jo_vranges =(jobjectArray) env->GetObjectField( obj, vrangesid);

	ok1 = this->unwrapColorElement(jo_nodatacolor, style.nodatacolor) ;
	
	int colorslen = (int) env->GetArrayLength( jo_colors);
	int vrangeslen = (int) env->GetArrayLength( jo_vranges);

	style.colors.resize(colorslen) ;
	style.vranges.resize(vrangeslen) ;

	for(int i = 0 ; i<colorslen ; ++ i ){
		jobject obj1 = env->GetObjectArrayElement(jo_colors,i) ;
		ok1 = this->unwrapColorElement(obj1, style.colors[i] ) ;
	}
	for(int i = 0 ; i<vrangeslen;++ i ){
		jobject obj1 = env->GetObjectArrayElement(jo_vranges,i) ;
		ok1 = this->unwrapRangeElement(obj1, style.vranges[i] ) ;
	}
	return true;
}


bool JavaPixelEngineHelperInterface::unwrapJavaTileData(
		jobject obj,
		vector<int64_t>& dtvec,
		vector<vector<unsigned char> >& datavec,
		int& width,
		int& height,
		int& nband ,
		int& numds ,
		int& dataType,
		int& x,
		int& y,
		int& z )
{
	JNIEnv *env = this->env	 ;
	if( env==0 ){
		cout<<"Error : JavaPixelEngineHelperInterface::unwrapJavaTileData env is null"<<endl ;
		return false;
	}
	jclass tileDataJavaClass =  env->GetObjectClass(obj);

	bool ok1 = this->getJavaObjectIntField(obj,"width" , width) ;
	bool ok2 = this->getJavaObjectIntField(obj,"height" , height) ;
	bool ok3 = this->getJavaObjectIntField(obj,"nband" , nband) ;
	bool ok4 = this->getJavaObjectIntField(obj,"numds" , numds) ;
	bool ok5 = this->getJavaObjectIntField(obj,"dataType" , dataType) ;
	bool ok6 = this->getJavaObjectIntField(obj,"x" , x) ;
	bool ok7 = this->getJavaObjectIntField(obj,"y" , y) ;
	bool ok8 = this->getJavaObjectIntField(obj,"z" , z) ; 

	bool ok9 = this->getJavaObjectLongArrField(obj,"datetimeArray" , dtvec ) ;
	bool ok10 = this->getJavaObjectByteArrOfArrField(obj,"tiledataArray" , datavec) ;

	if( ok1 && ok2 && ok3 && ok4 && ok5 && ok6 && ok7 && ok8 && ok9 && ok10){
		return true;
	}else{
		return false;
	}
}

bool JavaPixelEngineHelperInterface::getJavaObjectIntField(jobject obj,const char* fieldname,int& retval) {
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fidNumber = env->GetFieldID( someClass, fieldname, "I");
	if (NULL == fidNumber) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}
	jint number = env->GetIntField( obj, fidNumber);
	retval = number ;
	return true;
}

bool JavaPixelEngineHelperInterface::getJavaObjectLongArrField(jobject obj,const char* fieldname,
	vector<int64_t>& retvec) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "[J");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}

	// Get the object field, returns JObject (because Array is instance of Object)
	jobject jo = env->GetObjectField( obj, fid);
	// Cast it to a  
	jlongArray * arr = reinterpret_cast<jlongArray*>(&jo) ;
	// Get the elements (you probably have to fetch the length of the array as well  
	int64_t* data = env->GetLongArrayElements(*arr, NULL);
	jint len = env->GetArrayLength( *arr);
	int leni = len ;
	retvec.resize(leni) ;
	for(int idt = 0 ; idt < leni; ++ idt ){
		retvec[idt] = data[idt] ;
	}
	// Don't forget to release it 
	env->ReleaseLongArrayElements(*arr, data, 0);
	return true;
}

bool JavaPixelEngineHelperInterface::getJavaObjectByteArrOfArrField(jobject obj,const char* fieldname,
	vector<vector<unsigned char> >& retvecOfVec) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "[[B");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}

	// Get the object field, returns JObject (because Array is instance of Object)
	jobject jo = env->GetObjectField( obj, fid);
	// Cast it to a 
	jobjectArray* arr = reinterpret_cast<jobjectArray*>(&jo) ;
	int len1 = env->GetArrayLength(  *arr ) ;
	retvecOfVec.resize(len1) ;
	for(int i1 = 0 ; i1 < len1 ; ++ i1 )
	{
		jobject jo2 = env->GetObjectArrayElement(*arr,i1);
		jbyteArray* arr2 = reinterpret_cast<jbyteArray*>(&jo2) ;
		unsigned char* data2 = (unsigned char*)env->GetByteArrayElements(*arr2,NULL) ;
		int len2 = env->GetArrayLength(  *arr2 ) ;
		retvecOfVec[i1].resize(len2) ;
		memcpy( retvecOfVec[i1].data() ,  data2 , len2 ) ;
		env->ReleaseByteArrayElements(*arr2, (jbyte*)data2, 0);
	}
	return true;
}


std::string JavaPixelEngineHelperInterface::jstring2cstring(JNIEnv *env,
	jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    if( length == 0 ){
    	return "" ;
    }else{
    	std::string ret = std::string((char *)pBytes, length);
	    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

	    env->DeleteLocalRef(stringJbytes);
	    env->DeleteLocalRef(stringClass);
	    return ret;
    }
}

jstring JavaPixelEngineHelperInterface::cstring2jstring(JNIEnv *env,
	const char* str){
	return env->NewStringUTF(str) ;
}



bool JavaPixelEngineHelperInterface::getJavaObjectDoubleField(jobject obj,const char* fieldname,double& retval) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fidNumber = env->GetFieldID( someClass, fieldname, "D");
	if (NULL == fidNumber) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}
	jdouble number = env->GetDoubleField( obj, fidNumber);
	retval = number ;
	return true;
}

bool JavaPixelEngineHelperInterface::getJavaObjectByteField(jobject obj,const char* fieldname,unsigned char& retval) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fidNumber = env->GetFieldID( someClass, fieldname, "B");
	if (NULL == fidNumber) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}
	jbyte number = env->GetByteField( obj, fidNumber);
	retval = (unsigned char) number ;
	return true;
}

bool JavaPixelEngineHelperInterface::getJavaObjectStringField(jobject obj,const char* fieldname,string& retval) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "Ljava/lang/String;");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}
	jstring fvalue =(jstring) env->GetObjectField( obj, fid);
	retval = JavaPixelEngineHelperInterface::jstring2cstring(env,fvalue) ;
	return true;
}
bool JavaPixelEngineHelperInterface::getJavaObjectIntArrField(jobject obj,const char* fieldname,vector<int>& retval) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "[I");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}

	// Get the object field, returns JObject (because Array is instance of Object)
	jobject jo = env->GetObjectField( obj, fid);
	// Cast it to a  
	jintArray * arr = reinterpret_cast<jintArray*>(&jo) ;
	// Get the elements (you probably have to fetch the length of the array as well  
	int* data = env->GetIntArrayElements(*arr, NULL);
	jint len = env->GetArrayLength( *arr);
	int leni = len ;
	retval.resize(leni) ;
	for(int idt = 0 ; idt < leni; ++ idt ){
		retval[idt] = data[idt] ;
	}
	// Don't forget to release it 
	env->ReleaseIntArrayElements(*arr, data, 0);
	return true;
}
bool JavaPixelEngineHelperInterface::getJavaObjectByteArrField(jobject obj,const char* fieldname,vector<unsigned char>& retval) 
{
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "[B");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}

	// Get the object field, returns JObject (because Array is instance of Object)
	jobject jo = env->GetObjectField( obj, fid);
	// Cast it to a  
	jbyteArray * arr = reinterpret_cast<jbyteArray*>(&jo) ;
	// Get the elements (you probably have to fetch the length of the array as well  
	unsigned char* data =(unsigned char*) env->GetByteArrayElements(*arr, NULL);
	jint len = env->GetArrayLength( *arr);
	int leni = len ;
	retval.resize(leni) ;
	for(int idt = 0 ; idt < leni; ++ idt ){
		retval[idt] = data[idt] ;
	}
	// Don't forget to release it 
	env->ReleaseByteArrayElements(*arr, (jbyte*)data, 0);
	return true;
}
bool JavaPixelEngineHelperInterface::getJavaObjectStrArrField(jobject obj,const char* fieldname,vector<string>& retval) 
{
	cout<<"debug in cpp getJavaObjectStrArrField "<<endl;
	JNIEnv *env = this->env	 ;
	jclass someClass = env->GetObjectClass(obj);
	jfieldID fid = env->GetFieldID( someClass, fieldname, "[Ljava/lang/String;");
	if (NULL == fid) {
			cout<<"Error : failed to get field "<<fieldname<< " from java object."<<endl;
			return false;
	}

	// Get the object field, returns JObject (because Array is instance of Object)
	jobjectArray jo = (jobjectArray) env->GetObjectField( obj, fid);
	jsize len = env->GetArrayLength(jo);
	int leni = len ;
	retval.resize(leni) ;
	cout<<"debug in cpp getJavaObjectStrArrField leni "<<leni<<endl;
	for(int idt = 0 ; idt < leni; ++ idt ){
		jstring str1 = (jstring) env->GetObjectArrayElement(jo, idt);
		retval[idt] = JavaPixelEngineHelperInterface::jstring2cstring(env,str1) ;
	}
	return true;
}
bool JavaPixelEngineHelperInterface::unwrapColorElement(jobject obj,pe::PeColorElement& retval) 
{
	this->getJavaObjectDoubleField(obj,"val",retval.val) ;
	this->getJavaObjectByteField(obj,"r",retval.r) ;
	this->getJavaObjectByteField(obj,"g",retval.g) ;
	this->getJavaObjectByteField(obj,"b",retval.b) ;
	this->getJavaObjectByteField(obj,"a",retval.a) ;
	this->getJavaObjectStringField(obj,"lbl" , retval.lbl) ;
	return true;
}
bool JavaPixelEngineHelperInterface::unwrapRangeElement(jobject obj,pe::PeVRangeElement& retval) 
{
	this->getJavaObjectDoubleField(obj,"minval",retval.minval) ;
	this->getJavaObjectDoubleField(obj,"maxval",retval.maxval) ;
	return true;
}