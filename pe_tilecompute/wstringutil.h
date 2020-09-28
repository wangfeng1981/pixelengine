#pragma once
#include <string>
#include <cctype>
#include <sstream>



namespace pe {
	struct wStringUtil {
		std::string replace(std::string  origstr, std::string fromstr, std::string tostr);
		std::string tolower(std::string origstr);

		bool isValidNumber(std::string str, double& value);
		std::string unescape(std::string  instr);
		std::string escape(std::string  instr);
	};
	
}
