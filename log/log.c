#include "log.h"

static int init = 0;
static FILE *fp = NULL;

static int close_file()
{
	if(fp)
		return fclose(fp);
	
	return -1;
}

static int open_file()
{
	fp = fopen("./"LOGFILENAME, "a");
	
	if(fp == NULL){
		fprintf(stderr, "open log failed\n");
		return -1;
	}
	
	return 0;
}
static int reopen_file()
{
	close_file();
	return open_file();
}
static int write_file(char *str)
{
	if(fprintf(fp, "%s", str) < 0){
		if(reopen_file() < 0)
			return -1;
		else
			return fprintf(fp, "%s", str);
	}

	return 0;

}

int init_log()
{
	int ret = 0;
	
	if(init == 0){
		ret = open_file();
		init = 1;
	}
	
	return ret;
}
int close_log()
{
	return close_file();
}
int log_write(const char *fmt, ...)
{
	int ret = 0;
	char buffer[1000];
	va_list ap;
	time_t cur_time;
	struct tm tm_now;
	char timestamp[20];


/*make sure open the file*/
	if(init == 0){
		ret = init_log();
		if(ret < 0)
			return -1; 	
	}	

/*the log formate should like "yy-mm-dd hh-mm-ss" */
	memset(buffer, 0, sizeof(buffer));

	memset(timestamp, 0, sizeof(timestamp));
	time(&cur_time);
        localtime_r(&cur_time, &tm_now);
	strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", &tm_now);
	sprintf(buffer, "%s:\t", timestamp);

	ret = strlen(buffer);
	va_start(ap, fmt);
	vsprintf(buffer + ret, fmt, ap);
	va_end(ap);

	return write_file(buffer);
	
}
int _log_write(char *file, int line, const char *fmt,...)
{
	int ret = 0;
	char buffer[1000];
	va_list ap;
	time_t cur_time;
	struct tm tm_now;
	char timestamp[20];


/*make sure open the file*/
	if(init == 0){
		ret = init_log();
		if(ret < 0)
			return -1; 	
	}	

/*the log formate should like "yy-mm-dd hh-mm-ss" */
	memset(buffer, 0, sizeof(buffer));

	memset(timestamp, 0, sizeof(timestamp));
	time(&cur_time);
        localtime_r(&cur_time, &tm_now);
	strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", &tm_now);
	sprintf(buffer, "%s:: ", timestamp);
	ret = strlen(buffer);
	sprintf(buffer + ret, "%s:%d ", file, line);

	ret = strlen(buffer);
	va_start(ap, fmt);
	vsprintf(buffer + ret, fmt, ap);
	va_end(ap);

	return write_file(buffer);
}
