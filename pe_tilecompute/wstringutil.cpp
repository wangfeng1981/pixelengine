#include "wstringutil.h"

 namespace pe {
	std::string wStringUtil::replace(std::string origstr, std::string fromstr, std::string tostr)
	{
		size_t start_pos = 0;
		while ((start_pos = origstr.find(fromstr, start_pos)) != std::string::npos) {
			origstr.replace(start_pos, fromstr.length(), tostr);
			start_pos += tostr.length(); // Handles case where 'to' is a substring of 'from'
		}
		return origstr;
	}

	std::string wStringUtil::tolower(std::string origstr)
	{
		std::string newstr = origstr;
		for (int ic = 0; ic < origstr.size(); ++ic)
		{
			if (origstr[ic] >= 'A' && origstr[ic] <= 'Z') {
				newstr[ic] = std::tolower(origstr[ic]);
			}

		}
		return newstr;
	}

	bool wStringUtil::isValidNumber(std::string str, double& value) {
		return((std::istringstream(str) >> value >> std::ws).eof());
	}

	std::string wStringUtil::unescape(std::string  instr) {
		
		instr = replace(instr, "\\a", "\a");
		instr = replace(instr, "\\b", "\b");
		instr = replace(instr, "\\f", "\f");
		instr = replace(instr, "\\n", "\n");
		instr = replace(instr, "\\r", "\r");
		instr = replace(instr, "\\t", "\t");
		instr = replace(instr, "\\v", "\v");
		instr = replace(instr, "\\\\", "\\");
		instr = replace(instr, "\\\'", "\'");
		instr = replace(instr, "\\\"", "\"");
		instr = replace(instr, "\\\?", "\?");
		return instr;
	}

	std::string wStringUtil::escape(std::string  instr) {

		std::stringstream ss;
		for (int i = 0; i < instr.length(); ++i) {
			switch (instr[i]) {
				case '\a': ss << '\\' << 'a'; break;
				case '\b': ss << '\\' << 'b'; break;
				case '\f': ss << '\\' << 'f'; break;
				case '\n': ss << '\\' << 'n'; break;
				case '\r': ss << '\\' << 'r'; break;
				case '\t': ss << '\\' << 't'; break;
				case '\v': ss << '\\' << 'v'; break;
				case '\\': ss << '\\'  ; break;
				case '\'': ss << '\\' << '\''; break;
				case '\"': ss << '\\' << '\"'; break;
				case '\?': ss << '\\' << '?'; break;
				default:
					ss << instr[i];
			}
		}

		return ss.str() ;
	}
}