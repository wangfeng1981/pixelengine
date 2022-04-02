
#include "wcomputelonlatarea.h"

const Geodesic WComputeLonLatArea_geod( Constants::WGS84_a() , Constants::WGS84_f() );
const int WComputeLonLatArea_numresos = 26 ;
// 0-25 , total 26
const double WComputeLonLatArea_resos[]={
1.406250000000000000,
0.703125000000000000,
0.351562500000000000,
0.175781250000000000,
0.087890625000000000,
0.043945312500000000,
0.021972656250000000,
0.010986328125000000,
0.005493164062500000,
0.002746582031250000,
0.001373291015625000,
0.000686645507812500,
0.000343322753906250,
0.000171661376953125,
0.000085830688476563,
0.000042915344238281,
0.000021457672119141,
0.000010728836059570,
0.000005364418029785,
0.000002682209014893,
0.000001341104507446,
0.000000670552253723,
0.000000335276126862,
0.000000167638063431,
0.000000083819031715,
0.000000041909515858
} ;

WComputeLonLatArea::WComputeLonLatArea()
{
    cached_itilez=-1;
    cached_itiley=-1;
    cached_iyvec = vector<double>(256,-1);//bugfixed 2022-3-27
}

/// @param itilez 等级 0-12，最大不错过25级
/// @param itiley 瓦片y索引值
/// @param iy     瓦片内部y坐标 0-255
/// @retval 面积，单位平方公里
double WComputeLonLatArea::computeArea(int itilez,int itiley,int iy)
{
    if( itilez< 0 || itilez >= WComputeLonLatArea_numresos ) return 0.0 ;
    if( itilez == cached_itilez &&
        itiley == cached_itiley &&
        cached_iyvec[iy]>0
    ){
        return cached_iyvec[iy] ;
    }
    cached_itilez=itilez ;
    cached_itiley=itiley ;
    double latTop = 90.0 - (itiley * 256 + iy)*WComputeLonLatArea_resos[itilez] ;
    double latBottom = latTop - WComputeLonLatArea_resos[itilez] ;
    PolygonArea poly(WComputeLonLatArea_geod , false);//polyline use false for compute area
    poly.AddPoint( latTop , WComputeLonLatArea_resos[itilez] );
    poly.AddPoint( latTop ,    0.0 );
    poly.AddPoint( latBottom , 0.0 );
    poly.AddPoint( latBottom , WComputeLonLatArea_resos[itilez] );
    double perimeter, area;
    poly.Compute(false, true , perimeter, area);//reverse(false),sign(false)
    area=area/1000.0/1000.0 ;//bugfixed
    cached_iyvec[iy] = area ;
    return area ;//meter to km
}

