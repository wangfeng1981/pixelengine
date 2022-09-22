#include <iostream>
#include <string>
#include "pelegend.h"
#include "pelegendrenderproperty.h"


using namespace std;


int main(int argc, char* argv[] ) {
    cout<<"v2.0.0.0"<<endl;
	cout<<"usage:pelegend /.../renderproperty.json /.../style.json dpi"<<endl ;
	cout<<"output will be in /some/dir/some.json.png"<<endl;

	PeLegend pel ;
	cout<<"pelegend version:"<<pel.version()<<endl ;

	if( argc!=4 ){
		cout<<"argc not 4."<<endl ;
		cout<<"**** Rendder Property Json Sample: ****"<<endl ;
		PeLegendRenderProperty ppt ;
		ppt.print();
		return 11 ;
	}
    string pptfile = argv[1];
	string jfile =   argv[2] ;
	string outfile = jfile + ".png" ;
	int dpi = atof(argv[3]) ;
	string error ;
	int state = pel.makePngByJsonFile(jfile, outfile,dpi, error ) ;
	if( state==0 ){
		cout<<"outfile "<<outfile<<" , ok"<<endl ;
	}else{
		cout<<"failed to make outfile :"<<error<<endl ;
	}
	string outfile2 = jfile + "-2.png";
	int state2 = pel.makePngByJsonFile2(jfile,pptfile,outfile2,dpi,error);
	cout<<"state2 "<<state2<<" error "<<error<<endl ;
	return 0 ;
}
