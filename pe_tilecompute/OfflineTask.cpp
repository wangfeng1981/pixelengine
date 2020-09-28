#include "OfflineTask.h"



using std::ifstream;
using namespace ArduinoJson;

namespace pe {
	bool OfflineTask::loadFromJsonFile(string filename) {

		ifstream ifs(filename.c_str());
		DynamicJsonBuffer jsonbuffer;
		JsonObject& root = jsonbuffer.parse(ifs);
		if (root.success() == false ) {
			return false;
		}
		oftid = root["oftid"].as<int>() ;
		//wStringUtil util;
		//scriptContent = util.unescape( root["scriptContent"].as<char*>() );
		scriptContent = root["scriptContent"].as<char*>();
		extent= root["extent"].as<char*>() ;

		zmin= root["zmin"].as<int>() ;
		zmax= root["zmax"].as<int>() ;
		outProductId=root["outProductId"].as<int>() ;

		outDatetime= root["outDatetime"].as<int64_t>();
		startTime=root["startTime"].as<int64_t>()  ;
		endTime=root["endTime"].as<int64_t>() ;

		uid=root["uid"].as<int>() ;
		storage = root["storage"].as<int>();

		return true;
	}

	string OfflineTask::toJson() {
		DynamicJsonBuffer jsonbuffer;
		JsonObject& root = jsonbuffer.createObject();
		root["oftid"] = oftid;
		root["scriptContent"] = scriptContent;
		root["extent"] = extent;

		root["zmin"] = zmin;
		root["zmax"] = zmax;
		root["outProductId"] = outProductId;

		root["outDatetime"] = outDatetime;
		root["startTime"] = startTime;
		root["endTime"] = endTime;

		root["uid"] = uid;
		root["storage"] = storage;
		string str;
		
		root.printTo(str);
		return str;
	}
}