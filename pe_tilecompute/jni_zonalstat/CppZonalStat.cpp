
#include "com_pixelengine_CppZonalStat.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

using std::vector;
using std::max;
using std::min;
using std::cout;
using std::endl;

const int TILE_SIZE=256;

const double PI = 3.14159265;

const double A = 6378.164 ;
const double C = 6356.779 ;
const double D = 111.13 ;
const double PI_2_a_c_vs_360 = 2 * PI * A * C / 360.0 ;
const double PI_vs_180 = PI/180.0 ;
const double A_A = A*A ;
const double C_C = C*C ;




struct StatResult{
	long count ;
	double sum ;
	double vmax ;
	double vmin ;
	double areakm2 ;
	long totcount ;
	long fillcount ;
	StatResult() ;
};

StatResult::StatResult(){
	count=0;
	sum=0;
	vmax=0;
	vmin=0;
	areakm2=0;
	totcount=0;
	fillcount=0;
}



///@param rx longitude resolution in degree
///@param ry latitude resolution in degree
///@param lat pixel latitude in degree
///@return area in km2
double calcArea(double rx,double ry,double lat)
{
    double tanval = tan(lat*PI_vs_180) ;
    double dx = rx * PI_2_a_c_vs_360 * sqrt(1.0/(C_C+A_A*tanval*tanval))  ;
    double dy = ry * D ;
    return dx*dy ;
}



template<typename T>
void zonalstat(vector<int>& segs, T* dataPtr,int tiley,int tilex, 
	double reso , double valid0,double valid1,
	int iband, int filldata,StatResult& result
	)
{
    int fully0 = tiley * TILE_SIZE ;
    int fully1 = fully0 + TILE_SIZE ;
    int fullx0 = tilex * TILE_SIZE ;
    int fullx1 = fullx0 + TILE_SIZE ;
    int nseg = segs.size()/3 ;//每三个整数组成一个水平线段

	//瓦片左上角经纬度坐标	
	double tilelonleft = fullx0 * reso - 180 ;
    double tilelattop =  fully0 * (-reso) + 90 ;

    int bandoffset = iband * TILE_SIZE * TILE_SIZE ;//波段偏移值，单位像素
    for(int iseg = 0 ; iseg < nseg ; ++ iseg )
    {
        int segx0 = segs[iseg*3] ;
        int segy  = segs[iseg*3+1] ;
        int segx1 = segs[iseg*3+2] ;
        if( segy >= fully0 && segy < fully1 )
        {
            if( segx0 >= fullx1 ){
                continue ;//线段水平方向超出瓦片
            }else if( segx1 < fullx0 ){
                continue ;//线段水平方向超出瓦片
            }else
            {
                int segyInTile = segy - fully0 ;//线段y坐标在瓦片内部相对坐标
                int segx0InTile = max( 0           ,segx0 - fullx0) ;
                int segx1InTile = min( TILE_SIZE-1 ,segx1 - fullx0 ) ;
                int rowoffset = bandoffset + TILE_SIZE * segyInTile ;
                for(int ixInTile = segx0InTile ; ixInTile <= segx1InTile ; ++ ixInTile )
                {
                    T pxval = dataPtr[rowoffset+ixInTile] ;
                    ++ result.totcount ;

                    if( pxval >= valid0 && pxval <= valid1 )
                    {
                        double theArea = calcArea(reso,reso, tilelattop - reso*segyInTile) ;
                        if( result.count == 0 )
                        {
                            result.count = 1 ;
                            result.sum = pxval ;
                            result.vmax = pxval ;
                            result.vmin = pxval ;
                            result.areakm2 = theArea ;
                        }else{
                            ++ result.count ;
                            result.areakm2 += theArea ;
                            result.sum += pxval ;
                            if( result.vmax < pxval )
                            {
                                result.vmax = pxval ;
                            }
                            if( result.vmin > pxval )
                            {
                                result.vmin =pxval ;
                            }
                        }
                    }else if( pxval == (T)filldata ){
                        ++ result.fillcount ;
                    }
                }
            }
        }else if( segy >= fully1 )
        {//水平线段已经超出瓦片下边y值，跳出，后面不用统计了。
            break;
        }
    }
}

JNIEXPORT jobject JNICALL Java_com_pixelengine_CppZonalStat_doit
  (JNIEnv * env, jobject jobj , jintArray segs, jbyteArray data, 
  	jint tiley, jint tilex, jdouble reso, jdouble valid0, 
  	jdouble valid1, jint iband, jint datatype, jint filldata)
{
    cout<<"v1.0"<<endl ;

    int itiley = (int)tiley;
    int itilex = (int)tilex;
    double dreso = (double)reso ;
    double dvalid0 = (double)valid0;
    double dvalid1 = (double)valid1;
    int iiband = (int)iband ;
    int idtype = (int)datatype;
    int ifilldata = (int)filldata ;

    vector<int> isegs ;
    int segsSize = env->GetArrayLength(segs);
    int *segsbody = env->GetIntArrayElements(segs, 0);
    if( segsbody != 0 ){
        isegs = vector<int>( segsbody , segsbody + segsSize );
    }else{
        cout<<"Error: failed to get segsbody."<<endl ;
        return NULL ;
    }
    env->ReleaseIntArrayElements(segs, segsbody, 0);


    int dataByteSize = env->GetArrayLength( data);
    char* dataBody = (char*) env->GetByteArrayElements( data, 0);
    vector<char> idataVec;
    if( dataBody!=0 ){
        idataVec = vector<char>( dataBody , dataBody + dataByteSize );
    }else{
        cout<<"Error: failed to get dataBody."<<endl ;
        return NULL;
    }
    env->ReleaseByteArrayElements(data, (jbyte*)dataBody, 0);
	
    //com.pixelengine.ZonalStatResult
    jclass classCppZonalStat = env->FindClass("com/pixelengine/ZonalStatResult");
    if (NULL == classCppZonalStat){
        cout<<"Error: failed to find class of com/pixelengine/ZonalStatResult."<<endl ;
        return NULL ;
    }
    jobject resultObject = env->AllocObject( classCppZonalStat);
    //public void setValues(long cnt,double tsum,double vmin,double vmax,double area,long totcnt,long fcnt ) 
    jmethodID methodId = env->GetMethodID( classCppZonalStat, "setValues", "(JDDDDJJ)V");
    if (NULL == methodId){
        cout<<"Error: failed to get method id of setValues."<<endl ;
        return NULL ;
    }

    //call algorithm
    StatResult cresult ;
    
    if( idtype== 1){
        zonalstat<unsigned char>( isegs, (unsigned char*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==2 ){
        zonalstat<unsigned short>( isegs, (unsigned short*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==3 ){
        zonalstat<short>( isegs, (short*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==4 ){
        zonalstat<unsigned int>( isegs, (unsigned int*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==5 ){
        zonalstat<int>( isegs, (int*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==6 ){
        zonalstat<float>( isegs, (float*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }else if( idtype==7 ){
        zonalstat<double>( isegs, (double*)idataVec.data(), itiley,
            itilex, dreso, dvalid0, dvalid1 , iiband, ifilldata, cresult ) ;
    }


    env->CallVoidMethod(resultObject, methodId, 
        cresult.count,
        cresult.sum,
        cresult.vmin,
        cresult.vmax,
        cresult.areakm2, 
        cresult.totcount , 
        cresult.fillcount 
        );
    return resultObject ;
}



//将输入byte数组的数据写入目标byte数组，有效值在validpxcnt中记录，无效记录0
template<typename T>
void assignDatas( T* sourcedata, T* targetdata, short* validpxcntdata, 
    const int elementsize, const double vmin,const double vmax)
{
    for(int it = 0 ; it < elementsize; ++ it ){
        targetdata[it] = sourcedata[it] ;
        if( sourcedata[it] >= vmin && sourcedata[it] <= vmax ){
            validpxcntdata[it] = 1 ;
        }else{
            validpxcntdata[it] = 0 ;
        }
    }
}

//将输入byte数组的数据合成到目标byte数组，有效值在validpxcnt中+1记录，无效记录跳过
//method -1 最小合成， +1最大合成 ， 0 平均合成
template<typename T>
void combineDatas( T* sourcedata, T* targetdata, short* validpxcntdata, 
    const int elementsize, const double vmin,const double vmax,const int method)
{
    for(int it = 0 ; it < elementsize; ++ it ){
        if( sourcedata[it] >= vmin && sourcedata[it] <= vmax ){
            if( validpxcntdata[it]==0 ){
                targetdata[it] = sourcedata[it] ;
            }else{
                if( method==-1 ){
                    targetdata[it] = min( targetdata[it] , sourcedata[it]) ;
                }else if( method==1 ){
                    targetdata[it] = max( targetdata[it] , sourcedata[it]) ;
                }else{
                    double tsum = targetdata[it]*1.0*validpxcntdata[it] + sourcedata[it] ;
                    targetdata[it] = tsum/(validpxcntdata[it]+1) ;
                }
            }
            validpxcntdata[it] += 1 ;//
        }
    }
}




/* 用java历史序列分析中 数据合成
 * Class:     com_pixelengine_CppZonalStat
 * Method:    assignDataBytes
 * Signature: ([BDDI[B[S)V
 */
JNIEXPORT void JNICALL Java_com_pixelengine_CppZonalStat_assignDataBytes
  (JNIEnv *env, jobject jobj, jbyteArray data, jdouble vmin, jdouble vmax,
    jint datatype, jbyteArray targetdata, jshortArray validpxcnt)
{
    cout<<"v1.0"<<endl ;

    double dvalid0 = (double)vmin;
    double dvalid1 = (double)vmax;
    int idtype = (int)datatype;

    //有效像素累加个数
    int imgsize = env->GetArrayLength(validpxcnt);
    short *validpxcntDataPtr = env->GetShortArrayElements(validpxcnt, 0);
    if( validpxcntDataPtr == 0 ){
        cout<<"Error: failed to get validpxcnt."<<endl ;
        return ;
    }
    //使用指针，所以先不释放



    //目标像素值
    int targetdatabytesize = env->GetArrayLength(targetdata);
    char *targetdatabytePtr = (char*) env->GetByteArrayElements(targetdata, 0);
    if( targetdatabytePtr == 0 ){
        cout<<"Error: failed to get targetdata."<<endl ;
        return ;
    }
    //使用指针，所以先不释放



    //输入byte数组
    int indatabytesize = env->GetArrayLength( data);
    char* indataBody = (char*) env->GetByteArrayElements( data, 0);
    vector<char> inbytedataVec ;
    if( indataBody!=0 ){
        inbytedataVec = vector<char>( indataBody , indataBody + indatabytesize );
    }else{
        cout<<"Error: failed to get source data."<<endl ;
        return ;
    }
    env->ReleaseByteArrayElements(data, (jbyte*)indataBody, 0);// 拷贝到vector以后立即释放

    if( idtype== 1){
        assignDatas<unsigned char>( 
            (unsigned char*)inbytedataVec.data(), 
            (unsigned char*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;

    }else if( idtype==2 ){
        assignDatas<unsigned short>( 
            (unsigned short*)inbytedataVec.data(), 
            (unsigned short*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;

    }else if( idtype==3 ){
        assignDatas< short>( 
            ( short*)inbytedataVec.data(), 
            ( short*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;
    }else if( idtype==4 ){
        assignDatas<unsigned int>( 
            (unsigned int*)inbytedataVec.data(), 
            (unsigned int*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;
    }else if( idtype==5 ){
        assignDatas<int>( 
            (int*)inbytedataVec.data(), 
            (int*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;
    }else if( idtype==6 ){
        assignDatas<float>( 
            (float*)inbytedataVec.data(), 
            (float*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;
    }else if( idtype==7 ){
        assignDatas<double>( 
            (double*)inbytedataVec.data(), 
            (double*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1
        ) ;
    }

    env->ReleaseByteArrayElements(targetdata, (jbyte*)targetdatabytePtr , 0);
    env->ReleaseShortArrayElements(validpxcnt, validpxcntDataPtr, 0);
}

/* 用java历史序列分析中 数据合成
 * Class:     com_pixelengine_CppZonalStat
 * Method:    combineDataBytes
 * Signature: ([BDDII[B[S)V
 */
JNIEXPORT void JNICALL Java_com_pixelengine_CppZonalStat_combineDataBytes
  (JNIEnv *env, jobject jobj, jbyteArray data, jdouble vmin, jdouble vmax, 
    jint datatype, jint method, jbyteArray targetdata, jshortArray validpxcnt)
{
    cout<<"v1.0"<<endl ;

    double dvalid0 = (double)vmin;
    double dvalid1 = (double)vmax;
    int idtype = (int)datatype;
    int imethod = (int)method ;

    //有效像素累加个数
    int imgsize = env->GetArrayLength(validpxcnt);
    short *validpxcntDataPtr = env->GetShortArrayElements(validpxcnt, 0);
    if( validpxcntDataPtr == 0 ){
        cout<<"Error: failed to get validpxcnt."<<endl ;
        return ;
    }
    //使用指针，所以先不释放


    //目标像素值
    int targetdatabytesize = env->GetArrayLength(targetdata);
    char *targetdatabytePtr =(char*) env->GetByteArrayElements(targetdata, 0);
    if( targetdatabytePtr == 0 ){
        cout<<"Error: failed to get targetdata."<<endl ;
        return ;
    }
    //使用指针，所以先不释放



    //输入byte数组
    int indatabytesize = env->GetArrayLength( data);
    char* indataBody = (char*) env->GetByteArrayElements( data, 0);
    vector<char> inbytedataVec ;
    if( indataBody!=0 ){
        inbytedataVec = vector<char>( indataBody , indataBody + indatabytesize );
    }else{
        cout<<"Error: failed to get source data."<<endl ;
        return ;
    }
    env->ReleaseByteArrayElements(data, (jbyte*)indataBody, 0);// 拷贝到vector以后立即释放

    if( idtype== 1){
        combineDatas<unsigned char>( 
            (unsigned char*)inbytedataVec.data(), 
            (unsigned char*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;

    }else if( idtype==2 ){
        combineDatas<unsigned short>( 
            (unsigned short*)inbytedataVec.data(), 
            (unsigned short*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;

    }else if( idtype==3 ){
        combineDatas< short>( 
            ( short*)inbytedataVec.data(), 
            ( short*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;
    }else if( idtype==4 ){
        combineDatas<unsigned int>( 
            (unsigned int*)inbytedataVec.data(), 
            (unsigned int*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;
    }else if( idtype==5 ){
        combineDatas<int>( 
            (int*)inbytedataVec.data(), 
            (int*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;
    }else if( idtype==6 ){
        combineDatas<float>( 
            (float*)inbytedataVec.data(), 
            (float*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;
    }else if( idtype==7 ){
        combineDatas<double>( 
            (double*)inbytedataVec.data(), 
            (double*)targetdatabytePtr,
            validpxcntDataPtr , 
            imgsize , 
            dvalid0,
            dvalid1,imethod
        ) ;
    }

    env->ReleaseByteArrayElements(targetdata, (jbyte*)targetdatabytePtr , 0);
    env->ReleaseShortArrayElements(validpxcnt, validpxcntDataPtr, 0);
}
