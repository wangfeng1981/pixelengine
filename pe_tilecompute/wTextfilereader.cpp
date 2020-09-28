#include "wTextfilereader.h"

namespace pe {
 

	std::string wTextfilereader::readfile(std::string  filename)
	{
		std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		if (ifs.is_open() == false)
		{
			return "";
		}

		std::ifstream::pos_type fileSize = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		ifs.read(bytes.data(), fileSize);
		return std::string(bytes.data(), fileSize);
	}
}