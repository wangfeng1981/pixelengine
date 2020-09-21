#pragma once

#ifndef PE_PRODUCT_INFO_H
#define PE_PRODUCT_INFO_H

#include <string>
#include <vector>
#include "../../sharedcodes2/ajson5.h"
#include "compress.h"
using std::string;
using std::vector;
using namespace ArduinoJson;

namespace pe {


	struct PeProductInfo
	{
		inline PeProductInfo():minZoom(0),maxZoom(0),dataType(0),bandNum(0),hPidByteNum(0),hPid(0),hYXByteNum(0),noData(0),tileWid(0),tileHei(0){ dataTypeByteLen = 0; }

		string productName;
		string proj;
		int minZoom;
		int maxZoom;
		int dataType;
		int bandNum;

		vector<string> bandNames;
		vector<int> bandPids;
		vector<int> bandBandIndices;
		vector<int> bandBandNums;
		vector<double> scales;
		vector<double> offsets;

		string hTableName;
		string hFamily;
		int hPidByteNum;
		int hPid;
		int hYXByteNum;
		double noData;
		int tileWid;
		int tileHei;
		string compress;

		int dataTypeByteLen ;

		bool loadFromJson(string jsonText);
		bool loadFromJsonFile(string filename);
		void updateDataTypeByteLen();
		string toJson();
	};



}
//
//{
//    "productName": "fy3d",
//        "proj" : "EPSG:4326",
//        "minZoom" : "0",
//        "maxZoom" : "9",
//        "dataType" : "3",
//        "bandNum" : "6",
//        "bandNames" : [
//            "B01", "B02", "B03", "B04", "B05", "B06"
//        ] ,
//        "bandPids" : [
//            1, 1, 1, 1, 1, 1
//        ] ,
//        "bandBandIndices" : [
//                    0, 1, 2, 3, 4, 5
//                ] ,
//        "scales" : [
//                    1, 1, 1, 1, 1, 1
//                ] ,
//        "offsets" : [
//                            0, 0, 0, 0, 0, 0
//                        ] ,
//        "hTableName" : "tiledata",
//          "hFamily" : "fami",
//         "hPidByteNum" : "2",
//          "hPid" : "1",
//           "hYXByteNum" : "2",
//          "noData" : "0",
//          "tileWid" : "256",
//          "tileHei" : "256",
//        "compress" : "deflate"
//}

#endif