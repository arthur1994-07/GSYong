#ifndef _CODBCENV_H_
#define _CODBCENV_H_

#pragma once

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")

#include <sqlext.h> // ODBC library
#include <odbcinst.h>

#include <Windows.h>
#include <strstream>
#include <stdio.h>

//#include "../../../RanLogic/s_NetGlobal.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"

#include "../../../SigmaCore/Database/DbDefine.h"

class COdbcEnv
{
public:
	static COdbcEnv* GetInstance();
	static void ReleaseInstance();	
	
	int	 CreateHandle();
	void CloseHandle();
	SQLHENV GetHandle();

	SQLHENV m_hEnv;  // Handle of sql (void*)

private:
	COdbcEnv(void);
	~COdbcEnv(void);

	static COdbcEnv* SelfInstance;
};

#endif // _CODBCENV_H_
