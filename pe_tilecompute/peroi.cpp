//peroi.cpp


#include "peroi.h"

PeRoi::PeRoi()
{
	zlevel = 0;
}

bool PeRoi::horiLineInterLineSeg(const double horiY ,
	const double xprev,
	const double yprev,
	const double x0,//line in polygon
	const double y0,
	const double x1,
	const double y1, 
	double & resX ) // cross point to pixel x in image //bugfixed 2020-6-18// update 2020-10-15
{
	if( x0==x1 && y0==y1 ){
		return false;
	}else
	{
		if( y0 == horiY )
		{
			if( y0 > yprev && y0 > y1 ){
				return false ;//outside up corner
			}else if( y0 < yprev && y0 < y1 )
			{
				return false ;//outside down corner
			}else{
				resX = x0 ;
				return true;
			}
		}else{
			if( (horiY-y0)*(horiY-y1) < 0 )
			{//inside
				if( x0==x1 ){
					resX = x0;
					return true ;
				}else
				{
					double k = (y0 - y1) / (x0 - x1);//y0 must not equal with y1
					double b = y0 - k * x0;
					resX = (horiY - b) / k;
					return true;
				}
			}else
			{
				return false;
			}
		}
	}
}



bool PeRoi::buildRoiByMulPolys( PeMultiPolygon& mpoly,int tilesize,int z,int y,int x)
{
	cout<<"debug in buildRoiByMulPolys"<<endl;
	this->hsegs.clear() ;
	this->hsegs.reserve(1024*3) ;//guess 1k segs

	double reso = 1;
	int fullwid=0;
	int fullhei=0;
	this->zlevel = z;
	this->computeLevelInfo(z ,tilesize , reso,fullwid,fullhei );
	cout<<"debug reso,fullwid,fullhei:"<<reso<<","<<fullwid<<","<<fullhei<<endl;

	double topY = 90.0 - reso/2.0 ;
	double resoY = -reso ;
	double leftX = -180.0 ;

	double tileTopLat = topY - y*tilesize*reso;
	double tileBottomLat = tileTopLat - tilesize*reso;

	vector<double> crossPointXVector ;

	for(int ipoly = 0 ; ipoly < mpoly.polys.size() ; ++ ipoly )
	{//each poly

		crossPointXVector.clear();
		vector<PeMultiPolygonPoint>& polypoints = mpoly.polys[ipoly] ;

		cout<<"debug ipoly-"<<ipoly<<" points:"<<polypoints.size()<<endl ;

		if( polypoints.size() > 3 )
		{// at least a triangle, a triangle have four points.

			double polyminlat = 0;
			double polymaxlat = 0;
			this->polyMinMaxY( polypoints, polyminlat , polymaxlat );

			cout<<"debug ipoly-"<<ipoly<<" minlat:"<<polyminlat<<" maxlat:"<<polymaxlat<<endl ;
			cout<<"debug tile minlat:"<<tileBottomLat<<" maxlat:"<<tileTopLat<<endl ;

			double useminlat = max( tileBottomLat, polyminlat) ;
			double usemaxlat = min( tileTopLat, polymaxlat);

			cout<<"debug used minlat:"<<useminlat<<" maxlat:"<<usemaxlat<<endl ;

			int starterY = (90 - usemaxlat)/reso ;
			int stopperY = (90 - useminlat)/reso + 1 ;
			if( starterY<0 ) starterY=0;

			for(int curY = starterY ; curY < stopperY; ++ curY )
			{
				crossPointXVector.clear();

				double horiLat = 90.0 - curY * reso ;
				double xprev = (polypoints.end()-2)->v0;
				double yprev = (polypoints.end()-2)->v1;
				for(int ivert =0 ; ivert < polypoints.size()-1 ; ++ ivert )
				{//for each vert, except last vert
					double v0x = polypoints[ivert].v0 ;
					double v0y = polypoints[ivert].v1 ;
					double resX = 0;
					if( this->horiLineInterLineSeg(horiLat , 
						xprev,yprev,
						v0x,v0y,
						polypoints[ivert+1].v0,polypoints[ivert+1].v1,
						resX )  )
					{//cross point
						crossPointXVector.push_back(resX) ;
					}
					xprev = v0x ;
					yprev = v0y ;
				}
				if( crossPointXVector.size() > 0 && crossPointXVector.size()%2==0 )
				{
					//sort x 
					std::sort(crossPointXVector.begin(), crossPointXVector.end());

					//append hsegs
					for (int ix = 0; ix < crossPointXVector.size(); ix += 2) {
						int tx0 = (crossPointXVector[ix] - leftX) / reso + 0.5;
						int tx1 = (crossPointXVector[ix + 1] - leftX) / reso + 0.5;
						if (tx0 >= fullwid || tx1 < 0) {
							//outside
						}
						else {
                            HSeg tempSeg;
                            tempSeg.x0 = tx0;
                            tempSeg.y = curY ;
                            tempSeg.x1 = tx1 ;
							hsegs.push_back(tempSeg);
						}
					}
				}
			}//end one hori line
		}//end if
		cout<<"debug hsegs.size:"<<hsegs.size()<<endl ;

		//debug
		{
			for(int ih = 0 ; ih <hsegs.size(); ++ ih )
			{
				cout<<"debug ih-"<<ih<<" x0,y,x1:"<<hsegs[ih].x0<<","<<hsegs[ih].y
					<<","<<hsegs[ih].x1<<endl ;
			}
		}
		//enddebug
	}//end each poly

	//need sort all by y   
	std::sort(hsegs.begin() , hsegs.end(), PeRoi::compareFunc);

	return true;
}

void PeRoi::computeLevelInfo(int z,int tilesize,double& reso,int& fullwid,int& fullhei)
{
	fullwid = tilesize;
	for(int i = 1 ; i<=z; ++ i )
	{
		fullwid *= 2 ;
	}
	reso = 360.0/fullwid;
	fullhei = fullwid/2;
}

void PeRoi::polyMinMaxY( vector<PeMultiPolygonPoint>& poly, double& ymin,double& ymax) 
{
	if( poly.size() > 3 ){
		ymin = poly[0].v1;
		ymax = ymin;
		for(int i = 1 ; i< poly.size() ; ++ i )
		{
			double ty = poly[i].v1 ;
			if( ty > ymax ){
				ymax = ty;
			}else if( ty < ymin ){
				ymin = ty ;
			}

		}
	}

}


bool PeRoi::compareFunc( HSeg& s1, HSeg& s2)
{
    if( s1.y < s2.y ) return true ;
    else return false;
}