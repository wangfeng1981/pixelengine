//pemultipolygon.h
//Multi-Polygon

#ifndef PE_MULTIPOLYGON_H
#define PE_MULTIPOLYGON_H
#include <vector>

using std::vector;

struct PeMultiPolygonPoint
{
	double v0,v1;//long,lat
}

struct PeMultiPolygon
{
	vector< vector<PeMultiPolygonPoint> > polys ;
};



#endif