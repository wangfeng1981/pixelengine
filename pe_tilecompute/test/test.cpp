// program to test PixelEngine.h .cpp is good
#include <vector>
#include <iostream>
#include <string>

#include "../PixelEngine.h"


using namespace std;

bool testExternalFunc(
		void* pePtr,//PixelEngine
		string name,//name
		string datetime,//datetime
		vector<int>& vec,//bands [0,1,2]
		vector<unsigned char>& ret,//return binary
		int& dt,//return datatype
		int& wid,//return width
		int& hei,//return height 
		int& nbands )//return nbands
{
	wid = 256 ;
	hei = 256 ;
	nbands = 3 ;
	dt = 3 ;
	ret.resize(wid*hei*nbands*2) ;
	short* data = (short*)ret.data() ;
	for(int it = 0 ; it<wid*hei*nbands;++it )
	{
		data[it] = 1 ;
	}
	return true ;
}


bool testExteranlTileDataFunc(
		void* pePtr,//PixelEngine
		string name,//name
		string datetime,//datetime
		vector<int>& vec,//bands [0,1,2]
		int z, 
		int y, 
		int x,
		vector<unsigned char>& ret,//return binary
		int& dtype ,
		int& wid,
		int& hei,
		int& nband
		) 
{
	cout<<"testExteranlTileDataFunc zyx "<<z<<" "<<y<<" "<<x<<endl ;
	wid = 256 ;
	hei = 256 ;
	nband = 3 ;
	dtype = 3 ;
	ret.resize(wid*hei*nband*2) ;
	short* data = (short*)ret.data() ;
	for(int it = 0 ; it<wid*hei;++it )
	{
		int ix = it%wid ;
		int iy = it/wid ;
		data[it] = x*2+y*2;
	}
	return true ;
}



int main()
{
	//test 
	string source = 
		"function main(){"
		"  function convfunc(r,n,th){for(var t=n%256,e=parseInt(n/256),f=0,o=-10;o<=10;++o)for(var a=-10;a<=10;++a)f+=th.getPixel(0,t+o,e+a,-1);return f/441;};"
		"  function pxfunc(r,n){return r[0];};"
		"  var dsf = PixelEngine.Dataset(\"fy3d\",\"20190601\",[0,1,2]);"
		"  var ds2 = dsf.forEachPixel(convfunc) ;"
		"  return ds2.renderPsuedColor(0,-2,2,-99,[0,0,0,0],1,1,0) ;"
		"}" ;

	vector<unsigned char> retbinary ;
	PixelEngine::initV8() ;
	PixelEngine::GetExternalDatasetCallBack=testExternalFunc;
	PixelEngine::GetExternalTileDataCallBack=testExteranlTileDataFunc ;

	PixelEngine pe ;
	pe.RunScriptForTile( nullptr, source, 0,0,0,0, retbinary) ;

	if( retbinary.size() > 1 )
	{
		FILE* pf = fopen("test.png" , "wb") ;
		fwrite(retbinary.data() , 1 , retbinary.size() , pf) ;
		fclose(pf) ;
	}else
	{
		cout<<"Error: retbinary empty"<<endl;
	}

	return 0 ;	
}

//iband,vmin,vmax,nodata,nodataColor
// function main(){
//  var ds=PixelEngine.Dataset("fy3dtable",20190601,[0,1,2]);
//  return ds.renderGray(0,0,7000,-9999,[0,0,0,0]);
//   };