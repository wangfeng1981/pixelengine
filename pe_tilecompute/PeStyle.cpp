#include "PeStyle.h"

namespace pe {

	string PeColorElement::toJson() {
		char buff[256];
		sprintf(buff, "{\"val\":%f,\"r\":%d,\"g\":%d, \"b\":%d, \"a\":%d , \"lbl\":\"%s\" }",
			val, r, g, b, a, lbl.c_str());
		return string(buff);
	}

	bool PeStyle::loadFromJson(string jsonText) {

		ArduinoJson::DynamicJsonBuffer jsonBuffer;
		ArduinoJson::JsonVariant root = jsonBuffer.parseObject(jsonText);

		type = root["type"].as<char*>();
		JsonObject& nodata = root["nodatacolor"].as<JsonObject>();
		nodatacolor.val = nodata["val"].as<double>();
		nodatacolor.r = nodata["r"].as<int>();
		nodatacolor.g = nodata["g"].as<int>();
		nodatacolor.b = nodata["b"].as<int>();
		nodatacolor.lbl = nodata["lbl"].as<char*>();

		JsonArray& colorsArr = root["colors"].as<JsonArray>();
		for (int ic = 0; ic < colorsArr.size(); ++ic)
		{
			JsonObject& obj1 = colorsArr[ic];
			PeColorElement c1;
			c1.val = obj1["val"].as<double>();
			c1.r = obj1["r"].as<int>();
			c1.g = obj1["g"].as<int>();
			c1.b = obj1["b"].as<int>();
			c1.lbl = obj1["lbl"].as<char*>();
			colors.push_back(c1);
		}

		return true;

	}

	string PeStyle::toJson() {
		stringstream ss;
		ss << "{ \"type\":\"" << type <<"\"," ;
		ss << "\"nodatacolor\":" << nodatacolor.toJson() <<",";
		ss << "\"colors\":[";
		for (int ic = 0; ic < colors.size(); ++ic) {
			if (ic > 0) {
				ss << ",";
			}
			ss << colors[ic].toJson();
		}
		ss << "]}";
		return ss.str();
	}
}