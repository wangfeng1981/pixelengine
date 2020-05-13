#include "WTileDataBuffer.h"

WTileDataBuffer::WTileDataBuffer()
{
	this->dataPointer = 0;
	this->dataSize = 0;
	this->dataType = 0 ;//byte-1 , short-3
}


WTileDataBuffer::~WTileDataBuffer()
{
	this->destroy();
}


void WTileDataBuffer::destroy()
{
	if (this->dataPointer) {
		delete[] this->dataPointer;
		this->dataPointer = 0;
		this->dataSize = 0;
	}
}
bool WTileDataBuffer::create(const int sz)
{
	this->destroy();
	this->dataPointer = new unsigned char[sz];
	if (this->dataPointer) {
		this->dataSize = sz;
		return true;
	}
	else {
		this->dataSize = 0;
		return false;
	}
}
bool WTileDataBuffer::create( const int bytesz , const int dtype ) ;
{
	this->destroy();
	this->dataPointer = new unsigned char[sz];
	if (this->dataPointer) {
		this->dataSize = sz;
		this->dataType = dtype ;
		return true;
	}
	else {
		this->dataSize = 0;
		return false;
	}
}