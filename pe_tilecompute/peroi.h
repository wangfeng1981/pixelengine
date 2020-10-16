//peroi.h

#ifndef PE_ROI_H
#define PE_ROI_H

#include <vector>
#include <string>
#include "pemultipolygon.h"
#include <iostream>
#include <algorithm>


using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::max;
using std::min;

struct PeRoi
{
    struct HSeg {int x0,y,x1;} ;
    
	int zlevel;
	string proj;
	vector<HSeg> hsegs ;//[x0,y,x1, ... , x0,y,x1, ....]
	
	PeRoi();
	bool buildRoiByMulPolys( PeMultiPolygon& mpoly,int tilesize,int z,int y,int x) ;
	void computeLevelInfo(int z,int tilesize,double& reso,int& fullwid,int& fullhei);
    
    static bool compareFunc( HSeg& s1, HSeg& s2);

private:
	bool horiLineInterLineSeg(const double horiY ,
			const double xprev,
			const double yprev,
			const double x0,//line in polygon
			const double y0,
			const double x1,
			const double y1, 
			double & resX );

	void polyMinMaxY( vector<PeMultiPolygonPoint>& poly,double& ymin,double& ymax) ;
};



#endif