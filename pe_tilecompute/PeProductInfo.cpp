
#include "PeProductInfo.h"
#include <sstream>
#include <fstream>
using std::ifstream;
using std::stringstream;

namespace pe {

	bool PeProductInfo::loadFromJson(string jsonText) {
		ArduinoJson::DynamicJsonBuffer jsonBuffer;
		ArduinoJson::JsonVariant root = jsonBuffer.parseObject(jsonText);

		productName = root["productName"].as<char*>() ;
		proj = root["proj"].as<char*>();
		minZoom = root["minZoom"].as<int>() ;
		maxZoom = root["maxZoom"].as<int>() ;
		dataType = root["dataType"].as<int>();
		bandNum = root["bandNum"].as<int>() ;

		hTableName = root["hTableName"].as<char*>();
		hFamily = root["hFamily"].as<char*>();
		hPidByteNum = root["hPidByteNum"].as<int>();
		hPid = root["hPid"].as<int>();
		hYXByteNum = root["hYXByteNum"].as<int>();
		noData =  root["noData"].as<double>();
		tileWid = root["tileWid"].as<int>() ;
		tileHei = root["tileHei"].as<int>();
		compress = root["compress"].as<char*>();

		JsonArray& bandnamesJArr = root["bandNames"].as<JsonArray>();
		JsonArray& bandPidsJArr = root["bandPids"].as<JsonArray>();
		JsonArray& bandBandIndicesJArr = root["bandBandIndices"].as<JsonArray>();
		JsonArray& bandBandNumsArr = root["bandBandNums"].as<JsonArray>();

		JsonArray& scalesJArr = root["scales"].as<JsonArray>();
		JsonArray& offsetsJArr = root["offsets"].as<JsonArray>();


		for (int ib = 0; ib < bandnamesJArr.size(); ++ib)
		{
			string bname1 = bandnamesJArr[ib];
			int    pid1 = bandPidsJArr[ib].as<int>();
			int    bindex1 = bandBandIndicesJArr[ib].as<int>();
			double scale1 = scalesJArr[ib].as<double>();
			double offset1 = offsetsJArr[ib].as<double>();

			int bbnums = bandBandNumsArr[ib].as<int>();

			bandNames.push_back(bname1);
			bandPids.push_back(pid1);
			bandBandIndices.push_back(bindex1);
			bandBandNums.push_back(bbnums);

			scales.push_back(scale1);
			offsets.push_back(offset1);

		}		

		dataTypeByteLen = 0;
		switch (dataType)
		{
		case 1: dataTypeByteLen = 1; break;
		case 2: dataTypeByteLen = 2; break;
		case 3:dataTypeByteLen = 2; break;
		case 4: dataTypeByteLen = 4; break;
		case 5:dataTypeByteLen = 4; break;
		case 6:dataTypeByteLen = 4; break;//float
		default:
			break;
		}


		return true;
	}

	bool PeProductInfo::loadFromJsonFile(string filename) {
		ifstream ifs(filename.c_str());
		ArduinoJson::DynamicJsonBuffer jsonBuffer;
		ArduinoJson::JsonVariant root = jsonBuffer.parseObject(ifs);

		productName = root["productName"].as<char*>();
		proj = root["proj"].as<char*>();
		minZoom = root["minZoom"].as<int>();
		maxZoom = root["maxZoom"].as<int>();
		dataType = root["dataType"].as<int>();
		bandNum = root["bandNum"].as<int>();

		hTableName = root["hTableName"].as<char*>();
		hFamily = root["hFamily"].as<char*>();
		hPidByteNum = root["hPidByteNum"].as<int>();
		hPid = root["hPid"].as<int>();
		hYXByteNum = root["hYXByteNum"].as<int>();
		noData = root["noData"].as<double>();
		tileWid = root["tileWid"].as<int>();
		tileHei = root["tileHei"].as<int>();
		compress = root["compress"].as<char*>();

		JsonArray& bandnamesJArr = root["bandNames"].as<JsonArray>();
		JsonArray& bandPidsJArr = root["bandPids"].as<JsonArray>();
		JsonArray& bandBandIndicesJArr = root["bandBandIndices"].as<JsonArray>();
		JsonArray& bandBandNumsArr = root["bandBandNums"].as<JsonArray>();

		JsonArray& scalesJArr = root["scales"].as<JsonArray>();
		JsonArray& offsetsJArr = root["offsets"].as<JsonArray>();


		for (int ib = 0; ib < bandnamesJArr.size(); ++ib)
		{
			string bname1 = bandnamesJArr[ib];
			int    pid1 = bandPidsJArr[ib].as<int>();
			int    bindex1 = bandBandIndicesJArr[ib].as<int>();
			double scale1 = scalesJArr[ib].as<double>();
			double offset1 = offsetsJArr[ib].as<double>();

			int bbnums = bandBandNumsArr[ib].as<int>();

			bandNames.push_back(bname1);
			bandPids.push_back(pid1);
			bandBandIndices.push_back(bindex1);
			bandBandNums.push_back(bbnums);

			scales.push_back(scale1);
			offsets.push_back(offset1);

		}

		dataTypeByteLen = 0;
		switch (dataType)
		{
		case 1: dataTypeByteLen = 1; break;
		case 2: dataTypeByteLen = 2; break;
		case 3:dataTypeByteLen = 2; break;
		case 4: dataTypeByteLen = 4; break;
		case 5:dataTypeByteLen = 4; break;
		case 6:dataTypeByteLen = 4; break;//float
		default:
			break;
		}


		return true;
	}

	string PeProductInfo::toJson() {
		ArduinoJson::DynamicJsonBuffer jsonBuffer;
		ArduinoJson::JsonObject& root = jsonBuffer.createObject();
		root["productName"] = this->productName;
		root["proj"] = this->proj;
		root["minZoom"] = this->minZoom;
		root["maxZoom"] = this->maxZoom;
		root["dataType"] = this->dataType;
		root["bandNum"] = this->bandNum;

		ArduinoJson::JsonArray& bandNamesArr = root.createNestedArray("bandNames");
		ArduinoJson::JsonArray& bandPidsArr = root.createNestedArray("bandPids");
		ArduinoJson::JsonArray& bandBandIndicesArr = root.createNestedArray("bandBandIndices");
		ArduinoJson::JsonArray& bandBandNumsArr = root.createNestedArray("bandBandNums");
		ArduinoJson::JsonArray& scalesArr = root.createNestedArray("scales");
		ArduinoJson::JsonArray& offsetsArr = root.createNestedArray("offsets");

		for (int ib = 0; ib < bandNames.size(); ++ib) {
			bandNamesArr.add(bandNames[ib]);
			bandPidsArr.add(bandPids[ib]);
			bandBandIndicesArr.add(bandBandIndices[ib]);
			bandBandNumsArr.add(bandBandNums[ib]);
			scalesArr.add(scales[ib]);
			offsetsArr.add(offsets[ib]);
		}

 
		root["hTableName"] = this->hTableName;
		root["hFamily"] = this->hFamily;

		root["hPidByteNum"] = this->hPidByteNum;
		root["hPid"] = this->hPid;
		root["hYXByteNum"] = this->hYXByteNum;

		root["noData"] = this->noData;
		root["tileWid"] = this->tileWid;
		root["tileHei"] = this->tileHei;

		root["compress"] = this->compress;

		string outJsonText;
		root.printTo(outJsonText);

		return outJsonText;
	}
}
