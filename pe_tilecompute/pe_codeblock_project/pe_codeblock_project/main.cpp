#include <iostream>
//用Codeblock写代码，所以还要用到这个main函数

#include "../../PixelEngine.h"
#include "../../JavaPixelEngineHelperInterface.h"
#include "../../../../sharedcodes/zonalstat/whsegtlvobject.h"

using namespace std;

void unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;

void unit_test_isTileOverlay() ;


int main()
{
    cout << "Hello PixelEngine!" << endl;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    cout<<"PE Version:"<<pe.GetVersion()<<endl ;

    cout<<"-----------------unit test------------------"<<endl;
    unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv() ;

    unit_test_isTileOverlay();

    return 0;
}










void unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv()
{
    cout<<"--- unit_test_JavaPixelEngineHelperInterface_getRoiHsegTlv ---"<<endl ;



}


void unit_test_isTileOverlay()
{
    int tilez = 7 ;
    int tiley = 21 ;
    int tilex = 106 ;
    WHsegTlvObject roi ;
    string error;
    bool isok = roi.readFromFile( "taihu.hseg.tlv" , error);
    if( isok==false){
        cout<<"roi.readFromFile failed."<<endl ;
        return ;
    }
    bool isover = roi.isTileOverlay(tilez,tiley,tilex);
    cout<<"isover:"<<isover<<endl ;
}
