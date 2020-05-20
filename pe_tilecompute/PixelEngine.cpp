//PixelEngine.cpp
#include "PixelEngine.h"


void PixelEngine::ColorReverse(vector<int>& colors) 
{
	int nc = colors.size()/3 ;
	int hnc = nc/2 ;
	int r ,g , b ;
	for(int i = 0 ; i<hnc ; ++ i )
	{
		r = colors[i*3] ;
		g = colors[i*3+1] ;
		b = colors[i*3+2] ;
		colors[i*3] = colors[(nc-1-i)*3] ;
		colors[i*3+1] = colors[(nc-1-i)*3+1] ;
		colors[i*3+2] = colors[(nc-1-i)*3+2] ;
		colors[(nc-1-i)*3]=r ;
		colors[(nc-1-i)*3+1]=g;
		colors[(nc-1-i)*3+2]=b ;
	}
}



vector<int> PixelEngine::GetColorRamp(int colorid,int inverse) 
{
	if( colorid==1 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorRainbow ;
		}else
		{
			vector<int> v = PixelEngine::ColorRainbow ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==2 )
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorBlues ;
		}else
		{
			vector<int> v = PixelEngine::ColorBlues ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==3 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorReds ;
		}else
		{
			vector<int> v = PixelEngine::ColorReds ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else if( colorid==4 ){
		if( inverse==0 )
		{
			return PixelEngine::ColorGreens ;
		}else
		{
			vector<int> v = PixelEngine::ColorGreens ;
    		PixelEngine::ColorReverse(v);
    		return v ;
		}
	}else
	{
		if( inverse==0 )
		{
			return PixelEngine::ColorGrays ;
		}else
		{
			vector<int> v = PixelEngine::ColorGrays ;
			PixelEngine::ColorReverse(v);
			return v ;
		}
	}
}

vector<int> PixelEngine::ColorRainbow{
	215,25,28
	,232,91,58
	,249,158,89
	,254,201,128
	,255,237,170
	,237,248,185
	,199,233,173
	,157,211,167
	,100,171,176
	,43,131,186
} ;

vector<int> PixelEngine::ColorBlues
{
	 247,251,255
	,226,238,249
	,205,224,242
	,176,210,232
	,137,191,221
	,96,166,210
	,62,142,196
	,33,114,182
	,10,84,158
	,8,48,107
} ;

vector<int> PixelEngine::ColorReds{
	255, 245, 240
	,254, 227, 214
	,253, 198, 175
	,252, 164, 134
	,252,146,114
	,251,106,74
	,239,59,44
	,203,24,29
	,165,15,21
	,103,0,13
} ;
vector<int> PixelEngine::ColorGreens{
	 247,252,245
	,232,246,227
	,208,237,202
	,178,224,171
	,142,209,140
	,102,189,111
	,61,167,90
	,35,140,69
	,3,112,46
	,0,68,27
} ;
vector<int> PixelEngine::ColorGrays{
	250,250,250
	,223,223,223
	,196,196,196
	,168,168,168
	,141,141,141
	,114,114,114
	,87,87,87
	,59,59,59
	,32,32,32
	,5,5,5

} ;