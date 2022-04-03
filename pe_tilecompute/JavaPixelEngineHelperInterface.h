#ifndef JAVAPIXELENGINEHELPERINTERFACE_H
#define JAVAPIXELENGINEHELPERINTERFACE_H
//2022-3-23
//2022-3-26
//2022-4-3

#include <jni.h>
#include <string>
#include <vector>
#include <iostream>
#include "PeStyle.h"
#include "PixelEngine.h"

/// 这个类用于实例化一个对象与java中的PixelEngineHelperClassName相互对应，
/// 用于从java请求遥感瓦片数据和原信息。
/// 这个类是用在C++里，c++主动调用Java中对应的Helper对象。

using std::string;
using std::cout;
using std::endl;
using std::vector;

struct JavaPixelEngineHelperInterface : PixelEngineHelperInterface
{
public:
 	JNIEnv * env;
 	string javaPixelEngineHelperClassName ;//should not be empty.

 	JavaPixelEngineHelperInterface(JNIEnv* env0,string javaHelperClassName);

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

	//2022-3-6 从外部读取roi hseg.tlv数据 isUserRoi=1为用户roi，isUserRoi=0为系统ROI，rid为关系数据库中主键, retTlvData返回的二进制结果
	virtual bool getRoiHsegTlv(int isUserRoi,int rid,vector<unsigned char>& retTlvData) ;



	//2022-3-31 从外部获取 DatasetCollection 数据
    virtual bool getTileDataCollection(
        string& dsName,   //输入数据集名称
		vector<int64_t> datetimes, //输入日期时间数组，注意实际返回的数据不一定全部都有，具体要看retdtArr
		int z, int y, int x, //瓦片坐标
		vector<vector<unsigned char>>& retTileDataArr, //返回二进制数据，一个datetime对应一个vector<unsigned char>
		vector<int64_t>& retdtArr ,//返回成功获取的 datetime数组，数量与retTileDataArr一致
		int& retdataType,//返回数据类型 1 byte，2 u16,3 i16, 4 u32, 5 i32, 6 f32, 7 f64
		int& retwid,     //返回瓦片宽度
		int& rethei,     //返回瓦片高度
		int& retnbands,  //返回瓦片波段数量
		string& errorText);
    //2022-3-31 从外部获取日期时间集合对象
    virtual bool buildDatetimeCollections(
        string dsName,
        int64_t whole_start ,
        int whole_start_inc , //0 or 1
        int64_t whole_stop ,
        int whole_stop_inc ,
        string repeat_type , // '' 'm' 'y'
        int64_t repeat_start,
        int repeat_start_inc,
        int64_t repeat_stop,
        int repeat_stop_inc,
        int repeat_stop_nextyear, //0 or 1
        vector<DatetimeCollection>& retdtcollarray
    );


public:
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

	bool setJavaObjectIntField(jobject obj,const char* fieldname,int val);
	bool setJavaObjectStringField(jobject obj,const char* fieldname,const char* val);
	bool setJavaObjectByteArrField(jobject obj,const char* fieldname,vector<unsigned char>& val);
	bool setJavaObjectDoubleField(jobject obj,const char* fieldname,double val);//2022-3-26

} ;




#endif
