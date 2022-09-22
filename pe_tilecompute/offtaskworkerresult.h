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
        int status;//0-not start; 1-running; 2-done; 3-failed.
        string resultRelFilepath ;// relative file path under pedir
        string toJson() ;
};

#endif // OFFTASKWORDERRESULT_H
