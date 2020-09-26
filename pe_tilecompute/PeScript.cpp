#include "PeScript.h"

namespace pe {

	string PeScript::toJson() {
		ArduinoJson::DynamicJsonBuffer buffer;
		ArduinoJson::JsonObject& root = buffer.createObject();
		root["sid"] = sid;
		root["title"] = title;
		root["scriptContent"] = scriptContent;
		root["updateTime"] = updateTime;
		root["uid"] = uid;
		string jsonstr;
		root.printTo(jsonstr);
		return jsonstr;

	}
}