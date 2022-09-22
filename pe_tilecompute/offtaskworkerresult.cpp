#include "offtaskworkerresult.h"

OfftaskWorkerResult::OfftaskWorkerResult()
{
    this->ofid = 0 ;
    this->state = 8 ;
    this->status=0;
}

string OfftaskWorkerResult::toJson()
{
    stringstream ss ;
    ss<<"{\"ofid\":"<<this->ofid<<",\"state\":"<<this->state<<",\"status\":"<<this->status<<",\"resultRelFilepath\":\""<<this->resultRelFilepath<<"\"}" ;
    return ss.str() ;
}

