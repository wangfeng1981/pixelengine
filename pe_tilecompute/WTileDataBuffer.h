#ifndef WTILEDATABUFFER_H
#define WTILEDATABUFFER_H
class WTileDataBuffer
{
public:
	WTileDataBuffer();
	~WTileDataBuffer();
	inline unsigned char* getDataPointer() {
		return this->dataPointer;
	}
	inline int getDataSize() {
		return dataSize;
	}
	int dataType ;// 1-byte 3-short
	void destroy();
	bool create(const int sz);//deprecated 2020-5-12, use bool create( const int bytesz , const int dtype ) ;
	bool create( const int bytesz , const int dtype ) ;
private:
	inline WTileDataBuffer(WTileDataBuffer& right){} ;
	unsigned char* dataPointer;
	int   dataSize;
};



#endif