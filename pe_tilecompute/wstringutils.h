#ifndef W_STRING_UTILS_H
#define W_STRING_UTILS_H
#include <string>
#include <vector>
#include <iostream>

using std::cout ;
using std::endl ;
using std::string;

struct wStringUtils{

	static std::vector<std::string> splitString(const string wholeString, const string sep) ;
	static std::string getFileNameFromFilePath(string filepath);
	static bool stringContains(string wholeStr , string subStr) ;
	static string replaceString(std::string str, const std::string& from, const std::string& to);
	static string int2str( int val ) ;
    static string double2str(double val) ;
    static string long2str( int64_t val) ;
} ;


#endif