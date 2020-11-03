//unit-test.cpp
// program to test PixelEngine.h .cpp is good 
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include "../PixelEngine.h"
#include "../ajson5.h"


using namespace std;
using namespace ArduinoJson;

struct UnitTestHelper:public PixelEngineHelperInterface {
	virtual bool getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData, 
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText) ;

	virtual bool getTileDataArray( 
		int64_t fromdtInclusive, int64_t todtInclusive,
		string& dsName, vector<int> bandindices, int z, int y, int x,
		int filterMonth,int filterDay,int filterHour,int filterMinu,
		int filterSec ,
		vector<vector<unsigned char>>& retTileDataArr,
		vector<int64_t>& dtArr ,
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText) ;

	virtual bool getColorRamp(string& crid , PixelEngineColorRamp& crobj , 
			string& errorText);

	//get render style by id from system
	virtual bool getStyle(string& styleid, PeStyle& retStyle, string& errorText);
};

bool UnitTestHelper::getTileData(int64_t dt, string& dsName, vector<int> bandindices,
		int z, int y, int x, vector<unsigned char>& retTileData, 
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText)
{
	dataType=1;
	wid=256;
	hei=256;
	nbands=1;
	retTileData.resize(256*256) ;
	for(int it=0;it < 256*256; ++ it ){
		retTileData[it] = (int) dt ;
	}

	return true;
}

bool UnitTestHelper::getTileDataArray( 
		int64_t fromdtInclusive, int64_t todtInclusive,
		string& dsName, vector<int> bandindices, int z, int y, int x,
		int filterMonth,int filterDay,int filterHour,int filterMinu,
		int filterSec ,
		vector<vector<unsigned char>>& retTileDataArr,
		vector<int64_t>& dtArr ,
		int& dataType,
		int& wid,
		int& hei,
		int& nbands,
		string& errorText) 
{
	return false;
}

bool UnitTestHelper::getColorRamp(string& crid , PixelEngineColorRamp& crobj , 
			string& errorText)
{
	return false;
}

bool UnitTestHelper::getStyle(string& styleid, PeStyle& retStyle, string& errorText)
{
	return false;
}


int main()
{
	cout<<"unit test map reduce procedure. 2020-11-03."<<endl;

	string source = string("function zlevelFunc() { return 2; }")
	+ "function extentFunc() { return [100,110,20,-20]; }"
	+ "function sharedObjectFunc() { return {v0:0,v1:10}; }"
	+ "function mapFunc( sh ) {var sum=0.0; var ds = pe.Dataset('test',pe.DatetimeCurrent,[0]);var ds1=ds.forEachPixel(function(val){var pixelval = val[0];if(pixelval>=sh.v0 && pixelval <=sh.v1){sum += pixelval ;}return 0;});return {key:\"sum\", val:sum} ; }"
	+ "function reduceFunc( sharedObj, key, obj1, obj2 ) { var obj3=obj1+obj2; return obj3; }"
	+ "function main(sharedObj, objCollection ) {return objCollection; }" ;

	UnitTestHelper helper ;

	PixelEngine::initV8() ;
	PixelEngine pe ;
	pe.helperPointer = & helper ; 
	
    int zlevel=0;
    string error ;
	bool ok1 = pe.MapRedRunScriptForZlevel(nullptr,source,zlevel,error);
    cout<<"ok1:"<<ok1<<endl;
    cout<<"zlevel:"<<zlevel<<endl;


    double ll0,ll1,ll2,ll3;
	bool ok2 = pe.MapRedRunScriptForExtent(nullptr,source,ll0,ll1,ll2,ll3,error) ;
    cout<<"ok2:"<<ok2<<endl;
    cout<<"extent:"<<ll0<<","<<ll1<<","<<ll2<<","<<ll3<<endl;
    
    string sharedtext;
    pe.MapRedRunScriptForSharedObj(nullptr,source,sharedtext,error);
    
    cout<<"mapping ... ... "<<endl;
    DynamicJsonBuffer jsonbuffer ;
    
    int numxtile = pow(2,zlevel) ;
    vector<string> mapResults;
    int64_t currdatetime=2;
    for(int tiley=0; tiley < numxtile/2; ++ tiley )
    {
        for(int tilex=0; tilex < numxtile; ++ tilex )
        {
            string maptext;
            bool mapok = pe.MapRedRunScriptForMapFunc(nullptr,
                source,currdatetime
                ,zlevel,tiley,tilex,
                sharedtext,maptext,error) ;
            if( mapok==false )
            {
                cout<<"map failed !"<<endl ;
            }else{
                cout<<"map:"<<maptext<<endl;
                //unwarp only use val
                JsonObject& obj1 = jsonbuffer.parseObject(maptext) ;
                if( obj1.containsKey("key") && obj1.containsKey("val") )
                {
                    string temp_valtext ;
                    obj1["val"].printTo(temp_valtext) ;
                    cout<<"val:"<<temp_valtext<<endl;
                    mapResults.push_back(temp_valtext) ;
                }else{
                    cout<<"nokey."<<maptext<<endl;
                    mapResults.push_back(maptext) ;
                }                
            }
        }
    }
    
    cout<<"reducing ... ... of total "<<mapResults.size()<<endl;
    string keystr = "sum";
    string map0 ;
    if( mapResults.size()>0 )
    {
        map0 = mapResults[0];
        for(int i = 1; i < mapResults.size(); ++ i )
        {
            string newRes;
            bool redok = pe.MapRedRunScriptForReduceFunc(nullptr,source,sharedtext,keystr,
                map0, mapResults[i],newRes , error ) ;
            if( redok ){
                cout<<"reduce ok "<<newRes<<endl;
                map0 = newRes;
            }else{
                cout<<"reduce bad "<<error<<endl;
            }
        }
    }
    
    
    cout<<"finally main()...."<<endl;
    string mainRes;
    bool mainok = pe.MapRedRunScriptForMainFunc(nullptr,source,sharedtext,map0,mainRes,error) ;
    if( mainok )
    {
        cout<<"main ok "<<mainRes<<endl;
    }else{
        cout<<"main bad "<<error<<endl;
    }
    
    
    

	return 0;
}














// bool testExternalFunc(
// 		void* pePtr,//PixelEngine
// 		string name,//name
// 		string datetime,//datetime
// 		vector<int>& vec,//bands [0,1,2]
// 		vector<unsigned char>& ret,//return binary
// 		int& dt,//return datatype
// 		int& wid,//return width
// 		int& hei,//return height 
// 		int& nbands )//return nbands
// {
// 	wid = 256 ;
// 	hei = 256 ;
// 	nbands = 3 ;
// 	dt = 3 ;
// 	ret.resize(wid*hei*nbands*2) ;
// 	short* data = (short*)ret.data() ;
// 	for(int it = 0 ; it<wid*hei*nbands;++it )
// 	{
// 		data[it] = 1 ;
// 	}
// 	return true ;
// }



// bool testExteranlTileDataFunc(
// 		void* pePtr,//PixelEngine
// 		string name,//name
// 		string datetime,//datetime
// 		vector<int>& vec,//bands [0,1,2]
// 		int z, 
// 		int y, 
// 		int x,
// 		vector<unsigned char>& ret,//return binary
// 		int& dtype ,
// 		int& wid,
// 		int& hei,
// 		int& nband
// 		) 
// {
// 	cout<<"testExteranlTileDataFunc zyx "<<z<<" "<<y<<" "<<x<<endl ;
// 	wid = 256 ;
// 	hei = 256 ;
// 	nband = 3 ;
// 	dtype = 3 ;
// 	ret.resize(wid*hei*nband*2) ;
// 	short* data = (short*)ret.data() ;
// 	for(int it = 0 ; it<wid*hei;++it )
// 	{
// 		data[it] = it%256;
// 	}
// 	return true ;
// }


// bool testExteranlTileDataArrFunc(
// 		void* pePtr,//PixelEngine
// 		string name,//name
// 		string fromdatetime,//datetime
// 		string todatetime,
// 		vector<int>& vec,//bands [0,1,2]
// 		int z, 
// 		int y, 
// 		int x,
// 		int filterMonth , //-1 ignored , 1-12
// 		int filterDay ,   //-1 ignored , 1-31
// 		int filterHour,  //-1 ignored , 0-23
// 		int filterMinu , //-1 ignored , 0-59
// 		int filterSec ,  //-1 ignored, 0-59
// 		vector<vector<unsigned char> >& retdatavec,//return binary
// 		vector<long>& rettimevec,
// 		int& dtype ,
// 		int& wid,
// 		int& hei,
// 		int& nband,
// 		int& numds
// 		) 
// {
// 	cout<<"testExteranlTileDataArrFunc zyx "<<z<<" "<<y<<" "<<x<<endl ;
// 	wid = 256 ;
// 	hei = 256 ;
// 	nband = 3 ;
// 	dtype = 3 ;
// 	numds = 10 ;

// 	retdatavec.resize(numds) ;
// 	rettimevec.resize(numds) ;
// 	for(int ids = 0 ; ids < numds ; ++ ids )
// 	{
// 		vector<unsigned char>& ret = retdatavec[ids]  ;
// 		ret.resize(wid*hei*nband*2) ;
// 		short* data = (short*)ret.data() ;
// 		for(int it = 0 ; it<wid*hei;++it )
// 		{
// 			data[it] = it%256;
// 		}
// 		rettimevec[ids] = ids ;
// 	}
// 	return true ;
// }


// PixelEngineColorRamp testExteranlColorRampFunc(
// 	void* pePtr,//PixelEngine
// 	string colorid) 
// {
// 	cout<<"testExteranlColorRampFunc colorid "<<colorid<<endl ;

// 	PixelEngineColorRamp cr ;

// 	for(int ic = 0 ; ic<10 ; ++ ic )
// 	{
// 		cr.ivalues[ic] = ic*25 ;
// 		cr.r[ic] = 0 ;
// 		cr.g[ic] = ic*25 ;
// 		cr.b[ic] = ic*25 ;
// 		cr.a[ic] = 255 ;
// 		cr.numColors++ ;
// 	}
// 	cr.Nodata = -1 ;
// 	cr.NodataColor[0] = 0 ;
// 	cr.NodataColor[1] = 0 ;
// 	cr.NodataColor[2] = 0 ;
// 	cr.NodataColor[3] = 0 ;
// 	return cr ;
// }
// int main()
// {
// 	string source = 
// 	"function main(){"
// 	"  function convfunc(r,n,th){for(var t=n%256,e=parseInt(n/256),f=0,o=-10;o<=10;++o)for(var a=-10;a<=10;++a)f+=th.getPixel(0,t+o,e+a,-1);return f/441;};"
// 	"  function pxfunc2(n,r,f,u){return n[0];};"
// 	"  var dsarr = PixelEngine.DatasetArray(\"fy3d\",\"20190601\", \"20190610\",[0,1,2]);"
// 	"  var ds2 = dsarr.forEachPixel(pxfunc2) ;"
// 	"  var cr = PixelEngine.ColorRamp() ;"
// 	"  cr.add(0,32,255,32,255,'') ;"
//     "  cr.add(128,128,128,128,255,'') ;"
// 	"  cr.add(256,255,255,255,255,'') ; cr.Nodata=-99;"
// 	"  var cr2=PixelEngine.ColorRamp('some_color_id') ;"
// 	"  return ds2.renderPsuedColor(0,cr2,0) ;"
// 	"}" ;

// 	vector<unsigned char> retbinary ;
// 	PixelEngine::initV8() ;

// 	PixelEngine::GetExternalTileDataCallBack=testExteranlTileDataFunc ;
// 	PixelEngine::GetExternalTileDataArrCallBack=testExteranlTileDataArrFunc ;
// 	PixelEngine::GetExternalColorRampCallBack=testExteranlColorRampFunc;

// 	PixelEngine pe ;
// 	pe.RunScriptForTile( nullptr, source, 0,0,0,0, retbinary) ;

// 	if( retbinary.size() > 1 )
// 	{
// 		FILE* pf = fopen("test-clrid.png" , "wb") ;
// 		fwrite(retbinary.data() , 1 , retbinary.size() , pf) ;
// 		fclose(pf) ;
// 	}else
// 	{
// 		cout<<"Error: retbinary empty"<<endl;
// 	}

// 	return 0 ;	
// }

//iband,vmin,vmax,nodata,nodataColor
// function main(){
//  var ds=PixelEngine.Dataset("fy3dtable",20190601,[0,1,2]);
//  return ds.renderGray(0,0,7000,-9999,[0,0,0,0]);
//   };