///2018-1-10 add function update some record.


#ifndef W_MYSQL_H
#define W_MYSQL_H

#include "/usr/include/mysql/mysql.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std ;
using std::cout;


//获取当前日期，格式YYYYMMDD HH:mm:ss 2017-11-14
std::string wmysql_current_datetimestr();





class wMysql
{
public:

	inline wMysql():conn(0){} ;

	bool connect( string host , 
		string user , 
		string pwd , 
		string db , string& error ) ;

  
    //获取select结果
    int selectsql(string sqlstr, vector<vector<string> >& resultRows ) ;
	int runsql(string sqlstr) ;


	inline ~wMysql(){ mysql_close(conn); conn=0; } ;

private:
	MYSQL* conn ;


} ;


#endif
