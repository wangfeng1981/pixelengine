#ifndef DATETIMECOLLECTION_H
#define DATETIMECOLLECTION_H
//2022-4-1

#include <vector>
#include <string>
#include <ctime>
using std::vector;
using std::string;

struct DatetimeCollection
{
    public:

        string key;
        vector<int64_t> datetimes ;

        static bool makeDateList(int stopyear,int stopmon,int stopday,int ndaysbefore,vector<int64_t>& retdatearr);

    protected:

    private:
};

#endif // DATETIMECOLLECTION_H
