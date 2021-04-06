#pragma once
#include <string>
#include "ajson5.h"

 //
using std::string;

namespace pe {
	struct PeScript
	{
		int sid, uid;
		int64_t updateTime;
		string title;
		string scriptContent;
		string toJson();
		inline PeScript():sid(0),uid(0),updateTime(0) {}
	};
}


