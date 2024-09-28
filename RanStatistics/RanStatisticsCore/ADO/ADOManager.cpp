//
//mjeon.ado
// 2010.12.29
//

#include "stdAfx.h"
#include "odbc/DBBase.h"
#include "ADOManager.h"
#include "ConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CADOManager* CADOManager::SelfInstance = NULL;

CADOManager::CADOManager(void)
{
	m_pConsole = CConsoleMessage::GetInstance();

	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		m_pConsole->PrintMsg(_T("::CoInitialized() Failure."));
	}
	else
	{
		m_pConsole->PrintMsg(_T("::CoInitialized() Succeed."));		
	}
}

CADOManager::~CADOManager(void)
{	
}


CADOManager* CADOManager::GetInstance()
{
	//
	//mjeon
	//Singleton using heap. In case of DbExcuter, it's singleton uses static instead of heap.
	//Compare both solutions and select better one.
	//
	if (SelfInstance == NULL)
		SelfInstance = new CADOManager();
	return SelfInstance;
}

void CADOManager::ReleaseInstance()
{
	::CoUninitialize();
	
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}
