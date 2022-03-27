//用于计算等经纬度瓦片像素的面积，
//2022-3-26 created
//2022-3-27 bugfixed for iyvec use double for area rather than int.

#ifndef W_COMPUTE_LON_LAT_AREA_H
#define W_COMPUTE_LON_LAT_AREA_H

#include <GeographicLib/PolygonArea.hpp>
#include <GeographicLib/DMS.hpp>
#include <GeographicLib/Utility.hpp>
#include <GeographicLib/GeoCoords.hpp>
#include <GeographicLib/Ellipsoid.hpp>
#include <cmath>
#include <vector>
using std::vector;
using namespace GeographicLib;

struct WComputeLonLatArea
{
public:
    /// @param itilez 等级 0-12，最大不错过25级
    /// @param itiley 瓦片y索引值
    /// @param iy     瓦片内部y坐标 0-255
    /// @retval 面积，单位平方公里
    double computeArea(int itilez,int itiley,int iy) ;

    WComputeLonLatArea();
private:
    int cached_itilez,cached_itiley;
    vector<double>cached_iyvec;//bugfixed 2022-3-27


} ;


#endif



