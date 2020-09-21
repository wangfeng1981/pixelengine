#pragma once
#include <string>
struct wDatasetDatetime
{
	std::string ds;
	int64_t dt0,dt1;
	inline wDatasetDatetime() :dt0(0),dt1(0) {};
};

