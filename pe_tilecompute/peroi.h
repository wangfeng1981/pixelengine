//peroi.h

#ifndef PE_ROI_H
#define PE_ROI_H

#include <vector>
#include <string>
#include "pemultipolygon.h"
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct PeRoi
{
	int zlevel;
	string proj;
	vector<int> hsegs ;//[x0,y,x1, ... , x0,y,x1, ....]
	
	PeRoi();
	bool buildRoiByMulPolys( PeMultiPolygon& mpoly,int z) ;
	void computeLevelInfo(int z,int tilesize,double& reso,int& fullwid,int& fullhei);

private:
	bool horiLineInterLineSeg(const double horiY ,
			const double xprev,
			const double yprev,
			const double x0,//line in polygon
			const double y0,
			const double x1,
			const double y1, 
			double & resX );

	void polyMinMaxY( vector<PeMultiPolygonPoint>& poly, double& ymin,double& ymax) ;
};



#endif