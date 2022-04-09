#ifndef OFFTASKWORKERRESULT_H
#define OFFTASKWORKERRESULT_H

#include <string>
#include <sstream>

using std::stringstream ;
using std::string ;

class OfftaskWorkerResult
{
    public:
        OfftaskWorkerResult();
        int ofid ;
        int state;
        string resultRelFilepath ;// relative file path under pedir
        string toJson() ;
};

#endif // OFFTASKWORDERRESULT_H
