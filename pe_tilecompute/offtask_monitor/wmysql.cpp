#include "wmysql.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"


//获取当前日期，格式YYYYMMDD HH:mm:ss 2017-11-14
std::string wmysql_current_datetimestr()
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



bool wMysql::connect(string host , 
		string user , 
		string pwd , 
		string db, string& error ) 
{
	if( conn == 0 )
	{
		conn = mysql_init(0) ;
	    if( mysql_real_connect(conn , 
	                            host.c_str() , 
	                            user.c_str() , 
	                            pwd.c_str() , 
	                            db.c_str() , 
	                            0 , 0 , CLIENT_MULTI_STATEMENTS )
	                    == NULL )
	    { 
            error = mysql_error(conn) ;
            spdlog::critical("Error: mysql failed to connect:{}" , error ) ;
            return false ;
	    }else{
            spdlog::info("connect mysql success.") ;
            return true ;
        }
	}
    else{
        spdlog::info("mysql already connected.") ;
        return true ;//already connected
    }
}

  
int wMysql::runsql(string sqlstr)
{
	if( conn == 0 )
	{
        spdlog::critical("Error: mysql no connection" ) ;
		return 1 ;
	}

	if( mysql_query(conn , sqlstr.c_str() ) )
	{
        spdlog::critical("Error : run sql query failed:{}" , sqlstr ) ;
		return 2 ;
	} else
	{
		int naffected = mysql_affected_rows(conn) ;
        spdlog::info("affected rows : {}" , naffected ) ;
        while( mysql_next_result(conn)==0 ){} ;
        return 0 ;
	}


	// cout<<"update data_id "<<rid<<":"
	// <<pid<<","<<maskid<<","<<value<<","<<ymdstr<<endl;
	// //修改这里 如果已有记录那么插入
	// string sql = string("UPDATE tb_xulie_data SET value='") + value + "' "  ;
	// sql += " WHERE data_id='" + rid + "'; "  ;
	// cout<<sql<<endl ;
	// //UPDATE table_name SET col_name1="",col_name2=""  WHERE id=interid
		
}

     
int wMysql::selectsql(string sqlstr, vector<vector<string> >& resultRows ) 
{   
    if( conn == 0 )
	{
		spdlog::critical("Error: mysql no connection" ) ;
		return 1;
	}
	
	if( mysql_query(conn , sqlstr.c_str() )!=0 )
	{
        spdlog::critical("Error : run sql query failed:{} , mysql_erro:{}" , sqlstr  , mysql_error(conn) ) ;
	    return 2;
	} 
	MYSQL_RES * res = mysql_store_result(conn) ;
	if (res == NULL)
	{
	 	mysql_close( conn);
        spdlog::critical("Error : mysql_store_result failed.") ;
	 	return 3 ;
	}

	int num_fields = mysql_num_fields(res);

	MYSQL_ROW row;
	while( (row = mysql_fetch_row(res) ) )
	{
		vector<string> rowone ;
		for(int ifield = 0; ifield < num_fields; ifield++)
		{
			if( row[ifield] == 0 ){
				rowone.push_back("") ;
			}else{
				rowone.push_back(row[ifield]) ;
			}
		}
		resultRows.push_back(rowone) ;
	} 
	mysql_free_result(res) ;
	res = NULL ;
	while( mysql_next_result(conn)==0 ){} ;
	return 0 ; 
}
