#ifndef WV8ENGINE_H
#define WV8ENGINE_H

#include <vector>
#include "v8.h"
#include "libplatform/libplatform.h"
#include <string>
#include "WTileDataBuffer.h"

class WV8Engine
{
public:
	WV8Engine();
	inline bool init(v8::Isolate* iso){this->isolate = iso ;} ;
	~WV8Engine();

protected :
	v8::Isolate* isolate;
	v8::Local<v8::UnboundScript> unboundScript ;

public:
	bool computeOneOutputBandTile(WTileDataBuffer& fy4TileData
		, std::string scriptText 
		, WTileDataBuffer& outDataBuffer
		, std::string& errorText
		, int& dura_ms);
	bool computeRGBOutputBandTile(WTileDataBuffer& fy4TileData
		, std::string scriptText 
		, WTileDataBuffer& outDataBuffer
		, std::string& errorText
		, int& dura_ms);
	bool computeRGBAOutputBandTile(WTileDataBuffer& fy4TileData
		, std::string scriptText 
		, WTileDataBuffer& outDataBuffer
		, std::string& errorText
		, int& dura_ms);
	bool checkSciptOK( 
	 std::string scriptText
	, std::string& errorText
	 ) ;

	bool computeOneOutputBandTileV2(WTileDataBuffer& fy4TileData
		, std::string scriptText
		, WTileDataBuffer& outDataBuffer
		, std::string& errorText
		, int& dura_ms ) ;

	bool computeOneOutputJson(WTileDataBuffer& fy4TileData
		, std::string scriptText
		, std::string extra
		, std::string& outputString
		, std::string& errorText
		, int& dura_ms ) ;

	bool computeJson2Json(std::string injsonStr , 
			std::string scriptStr ,
			std::string extraStr ,
			std::string& outputText , 
			std::string& errorTextInComputing , 
			int& duration_ms ) ;
 
	bool computeTile2Tile(WTileDataBuffer& fy4tiledata , 
			std::string scriptStr ,
			std::string extraStr ,
			WTileDataBuffer& outDataBuffer , 
			std::string& errorTextInComputing , 
			int& duration_ms ) ;

	bool pe_computeTile2Tile( vector<WTileDataBuffer>& tiledataVec , 
			std::string scriptStr ,
			std::string extraStr ,
			WTileDataBuffer& outDataBuffer , 
			std::string& errorTextInComputing , 
			int& duration_ms ) ;

	//last one nine tiles 2 tile
};

#endif
