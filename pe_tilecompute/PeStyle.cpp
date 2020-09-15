#include "PeStyle.h"
#include <exception>
#include <iostream>
using std::cout;
using std::endl;




namespace pe {

	string PeColorElement::toJson() {
		char buff[256];
		sprintf(buff, "{\"val\":%f,\"r\":%d,\"g\":%d, \"b\":%d, \"a\":%d , \"lbl\":\"%s\" }",
			val, r, g, b, a, lbl.c_str());
		return string(buff);
	}

	string PeVRangeElement::toJson() {
		stringstream ss;
		ss << "{\"minval\":" << minval << ",\"maxval\":" << maxval << "}";
		return ss.str() ;
	}

	PeStyle PeStyle::emptyStyle() {
		return PeStyle();
	}

	bool PeStyle::loadFromJson(string jsonText) {
		vranges.clear();
		colors.clear();
		bands.clear();

		try {
			ArduinoJson::DynamicJsonBuffer jsonBuffer;
			ArduinoJson::JsonObject& root = jsonBuffer.parseObject(jsonText);
			if (root.success() == false) return false;

			if (root.containsKey("type")) type = root["type"].as<char*>();
			else return false;
			if (root.containsKey("nodatacolor")) {
				JsonObject& nodata = root["nodatacolor"].as<JsonObject>();
				nodatacolor.val = nodata["val"].as<double>();
				nodatacolor.r = nodata["r"].as<int>();
				nodatacolor.g = nodata["g"].as<int>();
				nodatacolor.b = nodata["b"].as<int>();
				nodatacolor.a = nodata["a"].as<int>();
				nodatacolor.lbl = nodata["lbl"].as<char*>();
			}
			else {
				nodatacolor.val = 0;
				nodatacolor.r = 0;
				nodatacolor.g =0;
				nodatacolor.b =0;
				nodatacolor.a = 0;
				nodatacolor.lbl ="";
			}
			
			if (root.containsKey("colors")) {
				JsonArray& colorsArr = root["colors"].as<JsonArray>();
				for (int ic = 0; ic < colorsArr.size(); ++ic)
				{
					JsonObject& obj1 = colorsArr[ic];
					PeColorElement c1;
					c1.val = obj1["val"].as<double>();
					c1.r = obj1["r"].as<int>();
					c1.g = obj1["g"].as<int>();
					c1.b = obj1["b"].as<int>();
					c1.a = obj1["a"].as<int>();
					c1.lbl = obj1["lbl"].as<char*>();
					colors.push_back(c1);
				}
			}

			if (root.containsKey("vranges")) {
				JsonArray& arr2 = root["vranges"].as<JsonArray>();
				for (int ic = 0; ic < arr2.size(); ++ic)
				{
					JsonObject& obj1 = arr2[ic];
					PeVRangeElement c1;
					c1.minval = obj1["minval"].as<double>();
					c1.maxval = obj1["maxval"].as<double>();
					vranges.push_back(c1);
				}
			}
			if (root.containsKey("bands")) {
				JsonArray& arr2 = root["bands"].as<JsonArray>();
				for (int ic = 0; ic < arr2.size(); ++ic)
				{
					int bindex = arr2[ic].as<int>() ;
					bands.push_back(bindex);
				}
			}
			
			return true;
		}
		catch (std::exception ex) {
			cout << "parse json exception:" << ex.what() << endl;
			return false;
		}
		catch (...) {
			cout << "parse json unknow exception" << endl;
			return false;
		}
		return false;
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
		ss << "],\"vranges\":[";
		for (int iv = 0; iv < vranges.size(); ++iv) {
			if (iv > 0) {
				ss << ",";
			}
			ss << vranges[iv].toJson();
		}
		ss << "],\"bands\":[";
		for (int ib = 0; ib < bands.size(); ++ib) {
			if (ib > 0) {
				ss << ",";
			}
			ss << bands[ib];
		}
		ss << "]}";
		return ss.str();
	}

	//获取bands数组中第index个波段索引值
	int PeStyle::getBand(int index) {
		if (index < 0 || index >= bands.size()) {
			cout << "Warning : bad index in getBand(), use 0 as default." << endl;
			return 0;
		}
		else {
			return bands[index];
		}
	}
 
 


	



}