#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace pe {

	struct PeTileData {
		vector<unsigned char> tiledata;
		int width, height, nbands, dataType;
	};

}