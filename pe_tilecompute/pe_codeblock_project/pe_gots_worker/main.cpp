#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


#include "../../PixelEngine.h"
#include "../../../../sharedcodes/zonalstat/whsegtlvobject.h"
#include "../../wcomputelonlatarea.h"
#include "debugpixelenginehelperinterface.h"

int main(int argc,char* argv[])
{
    cout << "A program for GOTS v8 worker." << endl;
    cout << "V1.0.0.0 created." << endl;
    cout << "usage: pe_gots_worker some.js"<<endl ;

    if( argc!=2 ){
        cout<<"argc not 2"<<endl ;
        return 11 ;
    }
    string jsfile = argv[1] ;
    cout<<"reading "<<jsfile<<endl ;
    ifstream stream1(jsfile.c_str()) ;
    if( stream1.good()==false ){
        cout<<"read failed."<<endl ;
        return 12 ;
    }
    stringstream ssbuffer ;
    ssbuffer<<stream1.rdbuf();
    string scriptText = ssbuffer.str() ;
    stream1.close();

    DebugPixelEngineHelperInterface debugHelper ;
    PixelEngine::initV8() ;
    PixelEngine pe ;
    pe.helperPointer = &debugHelper ;
    cout<<"run main() ..."<<endl ;
    string res1 ;
    bool ok1 = pe.RunScriptFunctionForTextResultOrNothing(scriptText,"main",0,0,0,res1);

    cout<<"run main done: "<<ok1<<endl ;
    cout<<"return: "<<res1<<endl ;
    cout<<"pe.log(): "<<endl ;
    cout<<pe.getPeLog()<<endl ;

    return 0;
}
