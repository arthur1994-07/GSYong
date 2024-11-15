// * History
// 2002.05.30 jgkim Create
// 2003.02.04 jgkim Error 처리 추가

#include "../pch.h"
#include <stdarg.h>     // WIN32 and ANSI 
// #include <varargs.h> // UNIX
#include <stdio.h>
#include "s_CClientLogSystem.h"

#include "../../SigmaCore/DebugInclude.h"

CLogSystem::CLogSystem()
{
	dLogInterval = 3600; // 60sec * 60min = 1hour
	StartLog();

	memset(log_tmpbuf, 0, sizeof(char) * (100));
	memset(log_filename, 0, sizeof(char) * (100));
}

CLogSystem::~CLogSystem()
{
}

void CLogSystem::StartLog(void)
{
	/*
	FILE *LogFile;	
    struct tm *today;
	int nRetCode = 0;

	time(&ltime);
	today = localtime(&ltime);

	strftime(log_filename, 100, "Log%Y%m%d%H.log", today);

	if ((LogFile = fopen(log_filename, "w")) != NULL)
	{
		fprintf(LogFile, "%s Program Log Started \n", GetTime());
		fclose(LogFile);		
	}
	*/
}

void CLogSystem::Write(char *string, ...)
{
	/*
	char	buf[C_BUFFER_SIZE];
	int		nRetCode = 0;
	double	elapsed_time = 0;
	time_t	ctime;
	
	if (string == NULL) return;

	ctime = time(&ctime);
	elapsed_time = difftime(ltime, ctime);
	
	if (elapsed_time >= dLogInterval)
	{
		StartLog();
	}

	va_list ap;
	va_start(ap, string);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	FILE *LogFile;

	if ((LogFile = fopen(log_filename, "a")) == NULL)
	{
		// 파일오픈에 실패하면 log_start 를 시켜보고
		// 다시 파일을 오픈한다.
		// 그래두 에러가 발생하면 디스크가 꽉찼거나 기타 에러이다.
		// 대처방법 없음...
		StartLog();		
		LogFile = fopen(log_filename, "a");
		if (LogFile == NULL) return; // 에러 발생시 리턴

	}
	fprintf(LogFile, "%s %s\n", GetTime(), buf);	
	fclose(LogFile);
	*/
}

char* CLogSystem::GetTime(void)
{	
	time_t ltime;    
    struct tm *today;

	// Use time structure to build a customized time string. 
	time(&ltime);
	today = localtime(&ltime);

    // Use strftime to build a customized time string. 
	strftime(log_tmpbuf, 100,"%Y %m %d %H:%M:%S", today);
	return log_tmpbuf;
}