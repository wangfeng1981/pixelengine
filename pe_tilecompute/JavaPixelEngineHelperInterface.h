#ifndef JAVAPIXELENGINEHELPERINTERFACE_H
#define JAVAPIXELENGINEHELPERINTERFACE_H
#include <jni.h>
#include <string>
#include <vector>
#include <iostream>
#include "PeStyle.h"
#include "PixelEngine.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;

struct JavaPixelEngineHelperInterface : PixelEngineHelperInterface
{
public:
 	JNIEnv * env;
 	inline JavaPixelEngineHelperInterface():env(0){}

	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData, 
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText);

 
	virtual bool getTileDataArray( 
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
		string& errorText);

 
	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , string& errorText);

 
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText) ;


private:
	bool getJavaObjectIntField(jobject obj,const char* fieldname,int& retval) ;
	bool getJavaObjectLongArrField(jobject obj,const char* fieldname,vector<int64_t>& retvec) ;
	bool getJavaObjectByteArrOfArrField(jobject obj,const char* fieldname,vector<vector<unsigned char> >& retvecOfVec) ;
	
	bool getJavaObjectByteField(jobject obj,const char* fieldname,unsigned char& retval) ;
	bool getJavaObjectDoubleField(jobject obj,const char* fieldname,double& retval) ;
	bool getJavaObjectStringField(jobject obj,const char* fieldname,string& retval) ;
	bool getJavaObjectIntArrField(jobject obj,const char* fieldname,vector<int>& retval) ;
	bool getJavaObjectByteArrField(jobject obj,const char* fieldname,vector<unsigned char>& retval) ;
	bool getJavaObjectStrArrField(jobject obj,const char* fieldname,vector<string>& retval) ;
	bool unwrapColorElement(jobject obj,pe::PeColorElement& retval) ;
	bool unwrapRangeElement(jobject obj,pe::PeVRangeElement& retval) ;

	bool unwrapJavaTileData(jobject obj,
		vector<int64_t>& dtvec,
		vector<vector<unsigned char> >& datavec,
		int& width,
		int& height,
		int& nband ,
		int& numds ,
		int& dataType,
		int& x,
		int& y,
		int& z );

	bool unwrapJavaColorRamp(jobject obj,
		PixelEngineColorRamp& cr );

	bool unwrapJavaStyle(jobject obj,
		pe::PeStyle& style ) ;


//toolbox
public :
	static std::string jstring2cstring(JNIEnv *env,jstring jStr);
	static jstring cstring2jstring(JNIEnv *env,	const char* str);
} ;




#endif