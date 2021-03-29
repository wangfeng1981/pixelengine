#ifndef W_STRING_UTILS_H
#define W_STRING_UTILS_H
#include <string>
#include <vector>

using std::string;

struct wStringUtils{

	static std::vector<std::string> splitString(const string wholeString, const string sep) ;
	static std::string getFileNameFromFilePath(string filepath);
	static bool stringContains(string wholeStr , string subStr) ;
	static string replaceString(std::string str, const std::string& from, const std::string& to);
	static string int2str( int val ) ;
    static string double2str(double val) ;
} ;

string wStringUtils::int2str( int val ) 
{
	char buff[1024] ;
	sprintf(buff , "%d" , val ) ;
	return string(buff) ;
}

string wStringUtils::double2str(double val) 
{
    char buff[256] ;
    sprintf(buff,"%.3f" , val) ;
    return string(buff) ;
}

std::vector<std::string> wStringUtils::splitString(const string wholeString, const string sep) 
{
	std::vector<std::string> result;

	int pos0 = 0;
	while (pos0 < wholeString.length())
	{
		int pos1 = wholeString.find(sep,pos0);
		if (pos1 == std::string::npos)
		{
			std::string substr = wholeString.substr(pos0);
			if (substr.length() > 0) result.push_back(substr);
			break;
		}
		else {
			int len1 = pos1 - pos0;
			std::string substr = wholeString.substr(pos0 , len1);
			if (substr.length() > 0) result.push_back(substr);
			pos0 = pos1 + 1;
		}
	}
	return result;
}

std::string wStringUtils::getFileNameFromFilePath(string filepath)
{
	return filepath.substr(filepath.find_last_of("/\\") + 1);
}

bool wStringUtils::stringContains(string wholeStr , string subStr) {
	if (wholeStr.find(subStr) != std::string::npos) {
	    return true;
	}else{
		return false;
	}
}

//替换字符串
std::string wStringUtils::replaceString(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}


#endif