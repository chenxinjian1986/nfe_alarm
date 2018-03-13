/*日志记录*/
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <unistd.h>  
#include <assert.h>  
#include <string.h>  
#include <string>  
#include <fcntl.h>  
#include <stdarg.h>  

//test
enum switch_mode
{
	mode_minute,
	mode_hour,
	mode_day,
	mode_month
};
int file_fd = -1;
int log_file(switch_mode mode = mode_day)
{
	char file_path[512] = { 0 };
	char filetime[32] = { 0 };
	struct tm tm_time;
	time_t t_log;
	std::string log_time = "";

	assert(getcwd(file_path, 512) != NULL);    //当前目录  
	if (file_path[strlen(file_path) - 1] != '/') {
		file_path[strlen(file_path)] = '/';
	}
	if (access(file_path, F_OK) != 0) {     //目录不存在  
		std::string build_path = "mkdir -p ";
		build_path += file_path;
		assert(system(build_path.c_str()) != 0);
	}

	t_log = time(NULL);
	localtime_r(&t_log, &tm_time);
	strftime(filetime, sizeof(filetime), "%Y%m%d%H%M%S", &tm_time); //日志的时间  
	switch (mode) {  //日志存储模式  
	case mode_minute:
		log_time.assign(filetime, 0, 12);
		break;
	case mode_hour:
		log_time.assign(filetime, 0, 10);
		break;
	case mode_day:
		log_time.assign(filetime, 0, 8);
		break;
	case mode_month:
		log_time.assign(filetime, 0, 6);
		break;
	default:
		log_time.assign(filetime, 0, 8);
	}
	strcat(file_path, "log_");
	strcat(file_path, log_time.c_str());
	strcat(file_path, ".log");

	file_fd = open(file_path, O_RDWR | O_CREAT | O_APPEND, 0666);
	assert(file_fd != -1);
	return 0;
}
void write_cmd(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
int write_log(const char *msg, ...)
{
	char final[2048] = { 0 };   //当前时间记录  
	va_list vl_list;
	va_start(vl_list, msg);
	char content[1024] = { 0 };
	vsprintf(content, msg, vl_list);   //格式化处理msg到字符串  
	va_end(vl_list);

	time_t  time_write;
	struct tm tm_Log;
	time_write = time(NULL);        //日志存储时间  
	localtime_r(&time_write, &tm_Log);
	strftime(final, sizeof(final), "[%Y-%m-%d %H:%M:%S] ", &tm_Log);

	strncat(final, content, strlen(content));
	assert(msg != NULL && file_fd != -1);
	assert(write(file_fd, final, strlen(final)) == strlen(final));
	return 0;
}
void close_file()
{
	close(file_fd);
}