#ifndef TEMPLATE_METHODS_H
#define TEMPLATE_METHODS_H

/// 2022-4-2
/// 模版函数放在 pxielengine.h 中显得太臃肿了， 单独把模版函数放到这个头文件里。
/// 2022-9-6 更新对有效值的判断，首先判断是否是filldata，然后在判断validrange，这样可以避免无效值在有效范围内的这种情况。


#include <vector>
#include <algorithm>

using std::vector ;
using std::max;
using std::min;


enum TemplateMethods_CompositeMethod {
    CM_MIN, CM_MAX, CM_AVE, CM_SUM
};

//2022-4-2 indata与outdata 最小合成，结果写回到 outdata中
template<typename T, typename U>
void TemplateMethods_CompositeMin( T* inDataPtr, U* outDataPtr, int elementSize, double validmin,double validmax,double fillval)
{
    for(int it = 0 ; it < elementSize;++ it )
    {
        if( inDataPtr[it]!=fillval && inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
        {
            if( outDataPtr[it] == fillval ) outDataPtr[it] = inDataPtr[it] ;
            else
            {
                outDataPtr[it] = min( outDataPtr[it] , (U)inDataPtr[it] ) ;
            }
        }
    }
}

//2022-4-2 indata与outdata 最大合成，结果写回到 outdata中
template<typename T, typename U>
void TemplateMethods_CompositeMax( T* inDataPtr, U* outDataPtr, int elementSize, double validmin,double validmax,double fillval)
{
    for(int it = 0 ; it < elementSize;++ it )
    {
        if( inDataPtr[it]!=fillval && inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
        {
            if( outDataPtr[it] == fillval ) outDataPtr[it] = inDataPtr[it] ;
            else
            {
                outDataPtr[it] = max( outDataPtr[it] , (U)inDataPtr[it] ) ;
            }
        }
    }
}

//2022-4-2 indata与outdata 求和合成，结果写回到 outdata中， 计数增加到 cntdata中
template<typename T, typename U>
void TemplateMethods_CompositeSum( T* inDataPtr, U* outDataPtr, vector<short>& cntdata, int elementSize, double validmin,double validmax,double fillval)
{
    for(int it = 0 ; it < elementSize;++ it )
    {
        if( inDataPtr[it]!=fillval && inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
        {
            if( cntdata[it] == 0 ){
                outDataPtr[it] = inDataPtr[it] ;
                cntdata[it] = 1;
            }
            else
            {
                outDataPtr[it] += inDataPtr[it] ;
                cntdata[it] +=1 ;
            }
        }
    }
}

///2022-4-2
/// 一组数据合成
template<typename T, typename U>
void TemplateMethods_Composite( vector<T*> inDataPtrVec, U* outDataPtr, int elementSize, double validmin,double validmax,double fillval
    ,TemplateMethods_CompositeMethod method  // min,max,ave,sum
)
{
    if( inDataPtrVec.size()==0 ) return ;
    vector<double> sumdata(elementSize,fillval) ;
    vector<short>  cntdata(elementSize,0);

    //first
    for(int it= 0 ; it < elementSize;++it ) outDataPtr[it] = fillval;

    for(int idata = 0 ; idata < inDataPtrVec.size() ; ++ idata )
    {
        T* tempInData = inDataPtrVec[idata] ;
        if( method == CM_MIN )
        {
            TemplateMethods_CompositeMin(tempInData, outDataPtr, elementSize, validmin,validmax,fillval) ;
        }else if( method == CM_MAX )
        {
            TemplateMethods_CompositeMax(tempInData, outDataPtr, elementSize, validmin,validmax,fillval) ;
        }else
        {
            //sum or ave
            double* tempOutPtr = (double*)sumdata.data() ;
            TemplateMethods_CompositeSum( tempInData, tempOutPtr , cntdata, elementSize, validmin,validmax,fillval) ;
        }
    }

    if( method==CM_AVE)
    {
        for(int it = 0 ; it < elementSize; ++ it )
        {
            if( cntdata[it] > 0 ){
                outDataPtr[it] = sumdata[it] / cntdata[it] ;
            }
        }
    }else if( method==CM_SUM ){
        for(int it = 0 ; it < elementSize; ++ it )
        {
            if( cntdata[it] > 0 ){
                outDataPtr[it] = sumdata[it] ;
            }
        }
    }

}


/// 2022-4-2  根据数据类型调用模版函数
void CallTemplateMethods_ForCollectionComposite(vector<void*> indataptrvec,void* outptr,
int indatatype,int outdatatype,
int elementSize,double validmin,double validmax,double filldata, TemplateMethods_CompositeMethod method) ;



//2022-9-6 生成掩摸1，0值，等于maskval的为1，反之为0
template<typename T>
void TemplateMethods_BuildMask( T* inDataPtr, unsigned char* outDataPtr, int elementSize, double maskval)
{
    for(int it = 0 ; it < elementSize;++ it )
    {
        outDataPtr[it]=0;
        if( inDataPtr[it] == maskval )
        {
            outDataPtr[it]=1;
        }
    }
}

//2022-9-6 生成掩摸1，0值，介于valminInc和valmaxInc之间的为1，反之为0
template<typename T>
void TemplateMethods_BuildMask2( T* inDataPtr, unsigned char* outDataPtr, int elementSize, double vminInc,double vmaxInc)
{
    for(int it = 0 ; it < elementSize;++ it )
    {
        outDataPtr[it]=0;
        if( inDataPtr[it] >= vminInc && inDataPtr[it]<=vmaxInc )
        {
            outDataPtr[it]=1;
        }
    }
}
void CallTemplateMethods_BuildMask( void* indataptr,int indatatype,unsigned char* outptr,int elementSize,double maskval) ;
void CallTemplateMethods_BuildMask2( void* indataptr,int indatatype,unsigned char* outptr,int elementSize,double vminInc,double vmaxInc) ;


//2022-9-6 值重映射，在原对象内存中操作，不新建对象
template<typename T>
void TemplateMethods_DsMap( T* dataPtr,int nband, int elementSize, double oldval,double newval)
{
    for(int ib = 0 ; ib<nband;++ib ){
        int offset = ib * elementSize ;
        for(int it = 0 ; it < elementSize;++ it )
        {
            if( dataPtr[offset+it] == oldval )
            {
                dataPtr[offset+it]=newval;
            }
        }
    }
}


//2022-9-6 值重映射，在原对象内存中操作，不新建对象
template<typename T>
void TemplateMethods_DsMap2( T* dataPtr,int nband,int elementSize, double vminInc,double vmaxInc,double newval)
{
    for(int ib = 0 ; ib<nband;++ib ){
        int offset = ib * elementSize ;
        for(int it = 0 ; it < elementSize;++ it )
        {
            if( dataPtr[offset+it] >= vminInc && dataPtr[offset+it]<=vmaxInc )
            {
                dataPtr[offset+it]=newval;
            }
        }
    }
}
void CallTemplateMethods_DsMap( void* dataPtr,int dataType,int nband,int elementSize,double oldval,double newval) ;
void CallTemplateMethods_DsMap2( void* dataPtr,int dataType,int nband,int elementSize,double vminInc,double vmaxInc,double newval) ;


//2022-9-6 used in dscollection.mask(masktiledata, filldata);
template<typename T>
void TemplateMethods_TileDataMask( T* dataPtr,int nband,int elementSize, unsigned char* maskdata,double filldata)
{
    for(int ib = 0 ; ib<nband;++ib ){
        int offset = ib * elementSize ;
        for(int it = 0 ; it < elementSize;++ it )
        {
            if( maskdata[it]!=1 )
            {
                dataPtr[offset+it]=filldata;
            }
        }
    }
}
void CallTemplateMethods_TileDataMask( void* dataPtr,int dataType,int nband,int elementSize, unsigned char* maskdata,double filldata) ;


#endif // TEMPLATE_METHODS_H
