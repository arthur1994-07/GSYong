#ifndef S_LOG_SYSTEM_H_
#define S_LOG_SYSTEM_H_

#include <time.h>

#pragma once

/**
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 * \version 1.0
 * first version
 * \date 2002-05-30
 * \author jgkim
 */
class CLogSystem 
{
public:
	CLogSystem();
	~CLogSystem();

protected:
	time_t ltime;
	char log_tmpbuf[100];
	char log_filename[100];
	double dLogInterval;
public:
	char* GetTime(void);
	void Write(char *string, ...);
	void StartLog(void);
protected:

};

#endif // S_LOG_SYSTEM_H_