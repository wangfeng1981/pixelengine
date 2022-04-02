#include "datetimecollection.h"

//2022-4-1
bool DatetimeCollection::makeDateList(int stopyear,int stopmon,int stopday,int ndaysbefore,vector<int64_t>& retdatearr)
{
    if( stopyear < 1900 ) return false ;
    if( stopyear > 9999) return false ;
    if( stopmon < 1 ) return false ;
    if( stopmon > 12 ) return false ;
    if( stopday < 1 ) return false;
    if( stopday > 31 ) return false ;
    if( ndaysbefore< 0 ) return false ;

    struct tm date = { 0, 0, 12 }  ;
    date.tm_year = stopyear - 1900 ;
    date.tm_mon =  stopmon - 1 ;  // note: zero indexed
    date.tm_mday = stopday ;       // note: not zero indexed
    retdatearr.resize( ndaysbefore +1 ) ;
    const time_t ONE_DAY_Seconds = 24* 3600 ;
    time_t stopseconds = mktime(&date) ;
    for(int i = 0 ; i < ndaysbefore ; ++ i )
    {
        time_t theseconds = stopseconds - (i+1) * ONE_DAY_Seconds ;
        struct tm *newdate = localtime(&theseconds) ;
        int64_t newymd = ( (1900+newdate->tm_year)*10000L + (newdate->tm_mon+1) *100 + newdate->tm_mday )*1000000L ;
        retdatearr[ndaysbefore-1-i] = newymd ;
    }
    retdatearr[ndaysbefore] = (stopyear*10000L + stopmon*100 + stopday)*1000000L ;
    return true ;
}
