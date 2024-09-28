//
//mjeon.ado
// 2010.12.29
//

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <stdio.h>

#include <vector>
#include <sstream>
#include <hash_map>

#include "odbc/DBBase.h"
#include "CLock.h"
#include "Requirement/ConsoleMessage.h"



class CADOManager : public CDBManager, public CLock
{
public:	
	static CADOManager* GetInstance();
	static void ReleaseInstance();


private:
	CADOManager(void);
	~CADOManager(void);
	static CADOManager* SelfInstance;

public:

	CConsoleMessage *m_pConsole;	


	int SQLPage01_NewUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_NewChar( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_MonthlyUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_MonthlyShopIncome( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	
	int SQLPage02_AnaysisGameTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_ConcurrentUsers( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_ChaLevel( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_IP2Nation( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );

	int SQLPage03_CC_Num( CTime &ctFrom, CTime &ctTo, BOOL bExceptKOR, CListBox *plbCCNum, CListBox *plbCCDetail, CListBox *plbUID );

	int SQLPage04_Daily_PlayTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult, CListBox *plbDetail );

};

