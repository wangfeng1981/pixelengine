#pragma once
#include <string>
#include <vector>
#include <sstream>

using std::stringstream;

using std::vector;
using std::string;

#include "../../sharedcodes2/ajson5.h"
using namespace ArduinoJson;


namespace pe {
	struct PeColorElement {
		double val;
		unsigned char r, g, b, a;
		string lbl;
		string toJson();
	};

	struct PeStyle
	{
		string type;
		vector<PeColorElement> colors;
		PeColorElement nodatacolor;

		bool loadFromJson(string jsonText);
		string toJson();
	};
}



