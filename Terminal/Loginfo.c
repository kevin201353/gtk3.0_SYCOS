/***********
//filename: Loginfo.c
//author: zhaosenhua
//version: 1.0
//funciton list:LogInfo()
//description: 写日志， 记录程序Debug信息
//Date: 2016/6/23
**************/
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SZIE   1024
#define FILE_DEBUG_LOG_PATH    "DebugInfo.log"   //日志文件名

void LogInfo(const char* ms, ... )
{
	char wzLog[MAX_BUFFER_SZIE] = {0};
	char buffer[MAX_BUFFER_SZIE] = {0};
	va_list args;
	va_start(args, ms);
	vsprintf( wzLog ,ms,args);
	va_end(args);

	time_t now;
	time(&now);
	struct tm *local;
	local = localtime(&now);
	printf("%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon,
			local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
			wzLog);
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon,
				local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
				wzLog);
	FILE* file = fopen(FILE_DEBUG_LOG_PATH,"a+");
	fwrite(buffer,1,strlen(buffer),file);
	fclose(file);
	return ;
}
