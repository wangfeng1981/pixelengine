#pragma once
//用于解析javascript脚本
#include "ajson5.h"
#include <string>
#include <vector>
#include <iostream>
#include "wDatasetDatetime.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using ArduinoJson::JsonArray;
using ArduinoJson::JsonObject;

namespace pe {
	struct wAST
	{
		ArduinoJson::DynamicJsonBuffer buffer;
		ArduinoJson::JsonVariant rootNode;
		//必须首先调用这个函数，然后才能使用别的函数
		void parse(string jsonText);

		bool findDatasets(string astJsonText, vector<string>& retDsName, string& error);

		bool findNodeByName(JsonObject& node, const string& name0, string& retType );

		bool hasNodeWithType(JsonObject& node, const char* typeName);

		bool hasReturnableMainFunction(JsonObject& node);

		void findObjectPropertyCallStatement(JsonObject& node, const char* objName, const char* propName, vector<string>& firstArgumentsVec);
		void findObjectPropertyCallStatement(JsonObject& node, const char* objName, const char* propName, 
			vector<wDatasetDatetime>& dsdtvec);
	};
}


