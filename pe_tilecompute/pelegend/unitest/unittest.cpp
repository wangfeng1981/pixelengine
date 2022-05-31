//unittest.cpp

#include <iostream>
#include <string>
#include "pelegend.h"

using namespace std;


int main(int argc, char* argv[] ) {

	cout<<"usage:pelegend /some/dir/some.json dpi"<<endl ;
	cout<<"output will be in /some/dir/some.json.png"<<endl; 
	PeLegend pel ;
	cout<<"pelegend version:"<<pel.version()<<endl ;

	if( argc!=3 ){
		cout<<"argc not 3."<<endl ;
		return 11 ;
	}

	string jfile = argv[1] ;
	string outfile = jfile + ".png" ;
	int dpi = atof(argv[2]) ;
	string error ;
	int state = pel.makePngByJsonFile(jfile, outfile,dpi, error ) ;
	if( state==0 ){
		cout<<"outfile "<<outfile<<" , ok"<<endl ;
	}else{
		cout<<"failed to make outfile :"<<error<<endl ;
	}

	return 0 ;
}