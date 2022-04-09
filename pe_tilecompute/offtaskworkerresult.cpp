#include "offtaskworkerresult.h"

OfftaskWorkerResult::OfftaskWorkerResult()
{
    this->ofid = 0 ;
    this->state = 8 ;
}

string OfftaskWorkerResult::toJson()
{
    stringstream ss ;
    ss<<"{\"ofid\":"<<this->ofid<<",\"state\":"<<this->state<<",\"resultRelFilepath\":\""<<this->resultRelFilepath<<"\"}" ;
    return ss.str() ;
}

