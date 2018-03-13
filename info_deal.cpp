#include <stdio.h>
#include <mysql.h>
using namespace std;
#include <iostream>
typedef std::string string_type;
#include <string>
#include "curlHttp.cpp"
#include "writelog.cpp"

// 调用info_deal
extern "C" char *info_deal(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	if (!(args->args && args->args[0] && args->args[1] && args->args[2] && args->args[3] && args->args[4] && args->args[5])) {
		*is_null = 1;
		*length = 8;
		snprintf(result, 8, "is null");
		return result;
	}
	string responseError = "";
	if (getResponse(args->args[0], args->args[1], args->args[2], args->args[3], args->args[4], args->args[5], responseError)) {
		*length = 2;
		snprintf(result, 2, "0");
		return result;
	}
	else {
		//*error = 1;
		*length = responseError.length() + 1;
		snprintf(result, *length, "%s", responseError.c_str());

		/*log_file();
		write_log("responseError=%s\n", responseError.c_str());
		write_log("length=%d\n", responseError.length());
		write_log("result=%s\n", result);
		close_file();*/

		return result;
	}
}

/*资源分配*/
extern "C" my_bool info_deal_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	//6个参数
    if (6 != args->arg_count  || args->arg_type[0] != STRING_RESULT  || args->arg_type[1] != STRING_RESULT  || args->arg_type[2] != STRING_RESULT || args->arg_type[3] != STRING_RESULT || args->arg_type[4] != STRING_RESULT || args->arg_type[5] != STRING_RESULT){
        strncpy(message, "please input 6 args and must be string;", MYSQL_ERRMSG_SIZE);
        return -1;
    }
    args->arg_type[0] = STRING_RESULT;
    args->arg_type[1] = STRING_RESULT;
    args->arg_type[2] = STRING_RESULT;
	args->arg_type[3] = STRING_RESULT;
	args->arg_type[4] = STRING_RESULT;
	args->arg_type[5] = STRING_RESULT;

    initid->ptr       = NULL;
    return 0;
}

/*
// 测试
//g++ -shared -fPIC -I /usr/local/mysql/include/ -l curl -o info_deal.so info_deal.cpp
//cp info_deal.so /usr/local/mysql/lib/plugin/

//DROP FUNCTION IF EXISTS `info_deal`; create function info_deal returns string soname 'info_deal.so';

//CREATE TRIGGER `tri_info_deal` AFTER INSERT ON `nfe_tcp_receive_msg` FOR EACH ROW begin
//	DECLARE c int;
//	set c = (select info_deal(CAST(new.eqid AS CHAR),CAST(new.els_type AS CHAR),CAST(new.s_status AS CHAR),new.s_feedback,new.or_data,CAST(new.have_time AS CHAR)));
//end;
*/