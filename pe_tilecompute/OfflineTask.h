#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include "ajson5.h"
#include "wstringutil.h"

using std::string;

namespace pe {
	struct OfflineTask
	{
		int oftid, zmin, zmax, outProductId, uid , storage ;
		int64_t outDatetime, startTime, endTime;
		string scriptContent, extent;

		inline OfflineTask() :oftid(0), zmin(0), zmax(0), outProductId(0), uid(0), outDatetime(0), startTime(0), endTime(0),storage(0) {};

		bool loadFromJsonFile(string filename);
		string toJson();
	};

}

