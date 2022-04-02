#ifndef TEMPLATE_METHODS_H
#define TEMPLATE_METHODS_H

/// 2022-4-2
/// 模版函数放在 pxielengine.h 中显得太臃肿了， 单独把模版函数放到这个头文件里。


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
        if( inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
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
        if( inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
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
        if( inDataPtr[it] >= validmin && inDataPtr[it] <= validmax )
        {
            if( outDataPtr[it] == fillval ){
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
    vector<short> cntdata(elementSize,0);
    //first
    for(int it= 0 ; it < elementSize;++it )
    {
        outDataPtr[it] = fillval ;
    }

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
            TemplateMethods_CompositeSum( tempInData,outDataPtr, cntdata, elementSize, validmin,validmax,fillval) ;
        }
    }

    if( method==CM_AVE)
    {
        for(int it = 0 ; it < elementSize; ++ it )
        {
            if( cntdata[it] > 0 ){
                outDataPtr[it] = outDataPtr[it] / cntdata[it] ;
            }
        }
    }

}


/// 2022-4-2  根据数据类型调用模版函数
void CallTemplateMethods_ForCollectionComposite(vector<void*> indataptrvec,void* outptr,
int indatatype,int outdatatype,
int elementSize,double validmin,double validmax,double filldata, TemplateMethods_CompositeMethod method) ;



#endif // TEMPLATE_METHODS_H
