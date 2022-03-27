//存储区域统计的数据
// 2022-3-26


#ifndef W_STATISTIC_DATA_H

struct WStatisticData
{
    double sum ;     //valid value sum
    double sq_sum ;  //valid value square sum
    double validCnt ;//valid pixel count in roi
    double validMin ;//min valid value in roi
    double validMax ;//max valid value in roi
    double areakm2 ; //valid pixels area in km2
    double allCnt ;  //all pixel count in roi
    double fillCnt ; //filldata count in roi

    inline WStatisticData():
        sum(0),sq_sum(0),
        validCnt(0),validMin(0),validMax(0),
        areakm2(0),allCnt(0),fillCnt(0){}
} ;



#endif

