//20201-3-2 update

#ifndef W_DATETIME_H

#define W_DATETIME_H
#include <ctime>
#include <string>
#include <chrono>
using std::string;

struct wDatetime{

    //2020-01-01 00:00:00
	static std::string currentDateTimeStr();
    //2020-01-01 00:00:00 CST
    static std::string currentDateTimeStrCST();
    
    
	//yyyyMMddhhmmss
	static std::string currentDateTimeStrTight() ;
    
    //return current milliseconds
    static int64_t currentMilliSeconds() ;
    
    //yyyyMMdd -> yyyy-MM-dd
    static string slimDate2FatDate( string date1 );
    //yyyyMMdd -> yyyy年MM月dd日
    static string slimDate2FatDateCN( string date1 ) ;
    
    //hhmmss -> hh:mm:ss
    static string slimTime2FatTime( string time1 ) ;
    
    //utm convert into beijing time
    static bool convertUtmTimeStr2BeijingTimeStr( string utmdate, string utmtime, string& bjdate, string& bjtime ) ;
    
    //date time string to seconds
    static bool convertDatetimeStr2Timestamp(string datestr, string timestr , time_t & retSeconds ) ;
    
};


//yyyyMMdd -> yyyy-MM-dd
string wDatetime::slimDate2FatDate( string date1 )
{
    if( date1.length() == 8 ){
        string newtime ;
        newtime = date1.substr(0,4) + "-" + date1.substr(4,2) + "-" + date1.substr(6,2) ;
        return newtime ;
    }else{
        return date1 ;
    }
        
}

//hhmmss -> hh:mm:ss
string wDatetime::slimTime2FatTime( string time1 ) {
    if( time1.length() == 6 ){
        string newtime ;
        newtime = time1.substr(0,2) + ":" + time1.substr(2,2) + ":" + time1.substr(4,2) ;
        return newtime ;
    }else{
        return time1 ;
    }
    
}

//yyyyMMdd -> yyyy-MM-dd
string wDatetime::slimDate2FatDateCN( string date1 )
{
    if( date1.length() == 8 ){
        string newtime ;
        newtime = date1.substr(0,4) + "年" + date1.substr(4,2) + "月" + date1.substr(6,2)+"日";
        return newtime ;
    }else{
        return date1 ;
    }
        
}


std::string wDatetime::currentDateTimeStr()
{
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1;
	int year = aTime->tm_year + 1900;

	int hour = aTime->tm_hour;
	int minu = aTime->tm_min;
	int sec = aTime->tm_sec;

	char buff[30];
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day , hour ,minu , sec );
	return std::string(buff);
}

std::string wDatetime::currentDateTimeStrCST()
{
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1;
	int year = aTime->tm_year + 1900;

	int hour = aTime->tm_hour;
	int minu = aTime->tm_min;
	int sec = aTime->tm_sec;

	char buff[30];
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d CST", year, month, day , hour ,minu , sec );
	return std::string(buff);
}

std::string wDatetime::currentDateTimeStrTight()
{
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1;
	int year = aTime->tm_year + 1900;

	int hour = aTime->tm_hour;
	int minu = aTime->tm_min;
	int sec = aTime->tm_sec;

	char buff[30];
	sprintf(buff, "%04d%02d%02d%02d%02d%02d", year, month, day , hour ,minu , sec );
	return std::string(buff);
}


//return current milliseconds
int64_t wDatetime::currentMilliSeconds() 
{
    unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return now ;
}


bool wDatetime::convertDatetimeStr2Timestamp(string datestr, string timestr , time_t & retSeconds ) 
{
    try{
        int ymd = std::stoi( datestr ) ;
        int hms = std::stoi( timestr ) ;
        
        tm tm1 ;
        tm1.tm_year = ymd/10000 - 1900 ;
        tm1.tm_mon =  (ymd%10000)/100 - 1 ;
        tm1.tm_mday = ymd%100 ;
        tm1.tm_hour=  hms / 10000 ;
        tm1.tm_min = (hms%10000)/100 ;
        tm1.tm_sec = hms%100 ;
        tm1.tm_isdst = 0 ;
        cout<<"debug "<<tm1.tm_hour<<endl ;
        
        time_t tt = mktime( &tm1 ) ;
        retSeconds = tt ;
        return true;
    }catch(std::invalid_argument &e )
    {
        return false ;
    }catch(std::out_of_range & e ){
        return false ;
    }
}


//utm convert into beijing time input should be YYYYMMDD , HHmmss
bool wDatetime::convertUtmTimeStr2BeijingTimeStr( string utmdate, string utmtime, string& bjdate, string& bjtime ) 
{
    time_t utmtt ;
    bool ok1 = convertDatetimeStr2Timestamp( utmdate , utmtime , utmtt) ;
    if( ok1 == false ){
        return false ;
    }
    //add 8 hours
    time_t bjtt = utmtt + 8 * 3600 ;
    
    struct tm *aTime = localtime(&bjtt);
	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1;
	int year = aTime->tm_year + 1900;

	int hour = aTime->tm_hour;
	int minu = aTime->tm_min;
	int sec = aTime->tm_sec;

	char buff[32];
	sprintf(buff, "%04d%02d%02d", year, month, day );
    bjdate = string(buff) ;
    
    sprintf(buff, "%02d%02d%02d", hour, minu, sec );
    bjtime = string(buff) ;
    
    return true ;
    
}
    
    
#endif