// program to test PixelEngine.h .cpp is good
#include <vector>
#include <iostream>
#include <string>

#include "../PixelEngine.h"


using namespace std;

bool testExternalFunc(
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
		data[it] = it%10000 ;
	}
	return true ;
}


int main()
{
	//test 
	string source = 
		"function main(){"
		"  var ds3 = PixelEngine.LocalDataset(\"/home/hadoop/tempdata/fy3d512bsp\",3,512,512,6) ;"
		"  var dsf = PixelEngine.Dataset(\"fy3d\",\"20190601\",[0,1,2]);"
		"  return dsf.renderRGB(2,1,0,0,5000,0,5000,0,5000) ;"
		"}" ;

	vector<unsigned char> retbinary ;

	PixelEngine::GetExternalDatasetCallBack=testExternalFunc;

	PixelEngine pe ;
	pe.RunScriptForTile( source, 0,0,0,0, retbinary) ;

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