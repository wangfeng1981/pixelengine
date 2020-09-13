
#include "PeProductInfo.h"


namespace pe {

	bool PeProductInfo::loadFromJson(string jsonText) {
		ArduinoJson::DynamicJsonBuffer jsonBuffer;
		ArduinoJson::JsonVariant root = jsonBuffer.parseObject(jsonText);

		productName = root["productName"].as<char*>() ;
		proj = root["proj"].as<char*>();
		minZoom = atof(root["minZoom"].as<char*>());
		maxZoom = atof(root["maxZoom"].as<char*>());
		dataType = atof(root["dataType"].as<char*>());
		bandNum = atof(root["bandNum"].as<char*>() );

		hTableName = root["hTableName"].as<char*>();
		hFamily = root["hFamily"].as<char*>();
		hPidByteNum = atof(root["hPidByteNum"].as<char*>());
		hPid = atof(root["hPid"].as<char*>());
		hYXByteNum = atof(root["hYXByteNum"].as<char*>());
		noData = atof(root["noData"].as<char*>());
		tileWid = atof(root["tileWid"].as<char*>() );
		tileHei = atof(root["tileHei"].as<char*>());
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
}
