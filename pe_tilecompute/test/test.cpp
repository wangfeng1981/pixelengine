// program to test PixelEngine.h .cpp is good
#include <vector>
#include <iostream>
#include <string>

#include "../PixelEngine.h"


using namespace std;



int main()
{
	//test 
	string source = 
		"function main(){"
		"  var ds3 = PixelEngine.LocalDataset(\"/home/hadoop/tempdata/fy3d512bsp\",3,512,512,6) ;"
		"  return ds3.renderRGB(2,1,0,0,5000,0,5000,0,5000) ;"
		"}" ;

	vector<unsigned char> retbinary ;

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