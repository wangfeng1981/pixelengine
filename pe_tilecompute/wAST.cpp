#include "wAST.h"

namespace pe {

	void wAST::parse(string jsonText) {
		
		rootNode = buffer.parse(jsonText);
		if (rootNode.success() == false) {
			cout << "Error : wAST::parse failed." << endl;
		}
		else {
			cout << "Info : wAST::parse ok." << endl;
		}
	}

 

	

	bool wAST::findDatasets(string astJsonText, vector<string>& retDsName, string& error) {
		ArduinoJson::DynamicJsonBuffer buffer;
		JsonObject& root = buffer.parse(astJsonText);
		if (root["type"] == "Program") {


		}


		return true;
	}

	bool wAST::findNodeByName(JsonObject& node, const string& name0, string& retType) {
		if (node.containsKey("id")) {
			JsonObject& idobj = node["id"];
			if (idobj.containsKey("name")) {
				string namestr = idobj["name"].as<char*>();
				if (namestr.compare(name0) == 0 ) {
					retType = idobj["type"].as<char*>();
					return true;
				}
			}
		}
		if (node.containsKey("body")) {
			if (node["body"].is<JsonArray>()) {
				JsonArray& arr = node["body"].as<JsonArray>();
				for (int ii = 0; ii < arr.size(); ++ii) {
					bool has1 = this->findNodeByName(arr[ii], name0, retType);
					if (has1 == true) {
						return true;
					}
				}
			}
			else if (node["body"].is<JsonObject>()) {
				JsonObject& bodyobj = node["body"];
				bool has1 = this->findNodeByName(bodyobj, name0, retType);
				if (has1 == true) {
					return true;
				}
			}
		}
		return false;
	}

	bool wAST::hasReturnableMainFunction(JsonObject& node) {
		if (node.containsKey("type") && node.containsKey("id") && node.containsKey("body")  ) {
			JsonObject& idobj = node["id"];
			JsonObject& bodyobj = node["body"];
			if (idobj.containsKey("name")) {
				string namestr = idobj["name"].as<char*>();
				string typestr = node["type"].as<char*>();
				if (typestr.compare("FunctionDeclaration") == 0 && namestr.compare("main")==0 ) {
					bool hasreturn = this->hasNodeWithType(bodyobj, "ReturnStatement");
					if(hasreturn==true )
						return true;
				}
			}
		}
		if (node.containsKey("body")) {
			if (node["body"].is<JsonArray>()) {
				JsonArray& arr = node["body"].as<JsonArray>();
				for (int ii = 0; ii < arr.size(); ++ii) {
					bool has1 = this->hasReturnableMainFunction(arr[ii]);
					if (has1 == true) {
						return true;
					}
				}
			}
			else if (node["body"].is<JsonObject>()) {
				JsonObject& bodyobj = node["body"];
				bool has1 = this->hasReturnableMainFunction(bodyobj );
				if (has1 == true) {
					return true;
				}
			}
		}
		return false;
	}

	bool wAST::hasNodeWithType(JsonObject& node, const char* typeName) {
		if (node.containsKey("type")) {
			string typestr = node["type"].as<char*>();
			if (typestr.compare(typeName) == 0) {
				return true;
			}
		}
		if (node.containsKey("body")) {
			if (node["body"].is<JsonArray>()) {
				JsonArray& arr = node["body"].as<JsonArray>();
				for (int ii = 0; ii < arr.size(); ++ii) {
					bool has1 = this->hasNodeWithType(arr[ii], typeName);
					if (has1 == true) {
						return true;
					}
				}
			}
			else if (node["body"].is<JsonObject>()) {
				JsonObject& bodyobj = node["body"];
				bool has1 = this->hasNodeWithType(bodyobj, typeName);
				if (has1 == true) {
					return true;
				}
			}
		}
		return false;
	}

	void wAST::findObjectPropertyCallStatement(JsonObject& node, const char* objName, const char* propName, vector<string>& firstArgumentsVec) {
		for (ArduinoJson::Internals::ListConstIterator<ArduinoJson::JsonPair> iter = node.begin(); iter != node.end(); ++iter) {
			if (strcmp(iter->key, "callee") == 0) {
				if (iter->value.as<JsonObject>().containsKey("object") && iter->value.as<JsonObject>().containsKey("property")) {
					JsonObject& object1 = iter->value.as<JsonObject>()["object"];
					JsonObject& property1 = iter->value.as<JsonObject>()["property"];
					if (object1.containsKey("name") && property1.containsKey("name")) {
						string object1name = object1["name"].as<char*>();
						string ppt1name = property1["name"].as<char*>();
						if ((object1name.compare(objName) == 0 )
							&&
							(ppt1name.compare(propName) == 0 )
							) {
							//find pe.Dataset or pe.DatasetArray
							if (node.containsKey("arguments")) {
								//callee:{...} , arguments:[]
								JsonArray& arr1 = node["arguments"].as<JsonArray>();
								JsonObject& arg0 = arr1[0];
								if (arg0.containsKey("type") && arg0.containsKey("value")) {
									string arg0type = arg0["type"].as<char*>();
									string arg0value = arg0["value"].as<char*>();
									if (arg0type.compare("Literal") == 0) {
										firstArgumentsVec.push_back(arg0value);
									}
								}
							}

						}
					}
				}
			}//if iter callee

			if (iter->value.is<JsonObject>())
			{
				JsonObject& tempObj1 = iter->value.as<JsonObject>();
				this->findObjectPropertyCallStatement(tempObj1, objName, propName, firstArgumentsVec);
			}
			else if (iter->value.is<JsonArray>()) {
				JsonArray& arr2 = iter->value.as<JsonArray>();
				for (int it2 = 0; it2 < arr2.size(); ++it2) {
					if (arr2[it2].is<JsonObject>()) {
						JsonObject& tempObj1 = arr2[it2].as<JsonObject>();
						this->findObjectPropertyCallStatement(tempObj1, objName, propName, firstArgumentsVec);
					}
				}
			}
		}
	}//end


	void wAST::findObjectPropertyCallStatement(JsonObject& node, const char* objName, const char* propName, 
		vector<wDatasetDatetime>& dsdtvec) {
		for (ArduinoJson::Internals::ListConstIterator<ArduinoJson::JsonPair> iter = node.begin(); iter != node.end(); ++iter) {
			if (strcmp(iter->key, "callee") == 0) {
				if (iter->value.as<JsonObject>().containsKey("object") && iter->value.as<JsonObject>().containsKey("property")) {
					JsonObject& object1 = iter->value.as<JsonObject>()["object"];
					JsonObject& property1 = iter->value.as<JsonObject>()["property"];
					if (object1.containsKey("name") && property1.containsKey("name")) {
						string object1name = object1["name"].as<char*>();
						string ppt1name = property1["name"].as<char*>();
						if ((object1name.compare(objName) == 0)
							&&
							(ppt1name.compare(propName) == 0)
							) {
							//find pe.Dataset or pe.DatasetArray
							if (node.containsKey("arguments")) {
								//callee:{...} , arguments:[]
								JsonArray& arr1 = node["arguments"].as<JsonArray>();
								if (arr1.size() > 2) {
									JsonObject& arg0 = arr1[0];
									JsonObject& arg1 = arr1[1];
									if (arg0.containsKey("type") && arg0.containsKey("value")
										&& arg1.containsKey("type") && arg1.containsKey("value")) {
										string arg0type = arg0["type"].as<char*>();
										string arg0value = arg0["value"].as<char*>();

										string arg1type = arg1["type"].as<char*>();
										string arg1value = arg1["value"].as<char*>();

										if (arg0type.compare("Literal") == 0 && arg1type.compare("Literal") == 0) {
											wDatasetDatetime dtds;
											dtds.ds = arg0value;
											dtds.dt0 = atof(arg1value.c_str());
											dsdtvec.push_back(dtds);
										}
									}
								}
								
							}

						}
					}
				}
			}//if iter callee

			if (iter->value.is<JsonObject>())
			{
				JsonObject& tempObj1 = iter->value.as<JsonObject>();
				this->findObjectPropertyCallStatement(tempObj1, objName, propName, dsdtvec);
			}
			else if (iter->value.is<JsonArray>()) {
				JsonArray& arr2 = iter->value.as<JsonArray>();
				for (int it2 = 0; it2 < arr2.size(); ++it2) {
					if (arr2[it2].is<JsonObject>()) {
						JsonObject& tempObj1 = arr2[it2].as<JsonObject>();
						this->findObjectPropertyCallStatement(tempObj1, objName, propName, dsdtvec);
					}
				}
			}
		}
	}//end
}
