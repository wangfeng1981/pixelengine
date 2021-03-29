#ifndef W_DIR_TOOLS_H
#define W_DIR_TOOLS_H

#ifdef __linux__
#include <sys/stat.h>

#endif

#include <string>
#include <iostream>
#include <cstdio>

using std::cout;
using std::endl;
using std::string;

//
class wDirTools
{
public:
	bool isDirExist( std::string& dirname ) ;
	bool mkDir( std::string& dirname ) ;
	string makeProductDir(string  rootdir,vector<string> subdirs, string& errorText) ;

	static string extractRelativePath( string rootdir , string fullPath ) ;
	static bool isFileExist( std::string filename ) ;
	static bool removeFile( std::string filename ) ;
};

//
bool wDirTools::isDirExist( std::string& dirname ) 
{
	#ifdef __linux__
	struct stat buffer;
	return (stat (dirname.c_str(),&buffer)==0 );
	#endif
	return false;
}

//
bool wDirTools::mkDir( std::string& dirname ) 
{
	#ifdef __linux__
	if( mkdir(dirname.c_str(),0777) == -1 )
	{
		cerr <<"Error : failed to make dir :"<<dirname<<endl;
		return false;
	}else
	{
		cout<<"Info : successfully make dir:"<<dirname<<endl;
		return true;
	}
	#endif
	return false;
}

//
string wDirTools::makeProductDir(string  rootdir,vector<string> subdirs, string& errorText)
{
	string outdir = rootdir;
	cout<<outdir<<endl;
	if( isDirExist(outdir) )
	{
		for(int idir = 0 ; idir < subdirs.size() ; ++ idir )
		{
			if( subdirs[idir].length()== 0 )
			{
				errorText = "subdirs[idir] is empty string." ;
				return "" ;
			}
			//concat output dir 
			outdir += "/" + subdirs[idir] ;
			cout<<outdir<<endl ;

			//test if output dir is ok.
			if( isDirExist(outdir) == false )
			{
				//not exist, then make it.
				if( mkDir( outdir ) ==false ) {
					//make failed , return nullstr.
					errorText = string("failed make dir:" )+ outdir ;
					return "" ;
				}
			}
		}
		return outdir ;
	}else
	{
		cout<<"Error : root dir is not exist:"<<outdir<<endl;
		errorText = string("root dir is not exist:" )+ rootdir ;
		return "";
	}
	return "";
}


//use relative path
string wDirTools::extractRelativePath( string rootdir , string fullPath ) 
{
	if( fullPath.length() > rootdir.length() )
	{
		return fullPath.substr( rootdir.length() , fullPath.length() - rootdir.length() ) ;
	}else
	{
		return fullPath ;
	}
}


bool wDirTools::isFileExist( std::string filename ) {
	FILE* pf = fopen( filename.c_str() ,"rb" ) ;
	if( pf== 0 ){
		cout<<"Info : file not exist "<<filename<<endl ;
		return false ;
	}
	fclose(pf) ;
	return true ;
}
bool wDirTools::removeFile( std::string filename ) {
	if( remove( filename.c_str() ) != 0 ){
		cout<<"Error : failed to delete file:"<<filename <<endl ;
		return false ;
	}
	else{
		cout<<"Info : File successfully deleted:"<<filename <<endl ;
		return true ;
	}
}


#endif
