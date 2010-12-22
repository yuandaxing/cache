#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define LOGFILENAME	"log.txt"	/*should be a string*/
int init_log();
/* if we used the log, we should close the log*/
int close_log();
int log_write(const char *fmt, ...);
int _log_write(char *file, int line, const char *fmt,...);
#define LOG_WRITE(...) _log_write(__FILE__, __LINE__, __VA_ARGS__)
