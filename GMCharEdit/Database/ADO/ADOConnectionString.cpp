#include "stdafx.h"
#include "./ADOConnectionString.h"

 
CjConnectionStrings* CjConnectionStrings::m_pInstance = NULL;


CjConnectionStrings::CjConnectionStrings()
{
}

CjConnectionStrings::~CjConnectionStrings()
{
}

CjConnectionStrings* CjConnectionStrings::GetInstance()
{
	static sc::DefaultLock instanceLock;

	if (m_pInstance == NULL)
	{
		instanceLock.lockOn();

		if (m_pInstance == NULL)
		{
			m_pInstance = new CjConnectionStrings();		
		}

		instanceLock.lockOff();
	}

	return m_pInstance;	
}

void CjConnectionStrings::ReleaseInstance()
{
	static sc::DefaultLock instanceLock;

	instanceLock.lockOn();

	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}

	instanceLock.lockOff();
}

BOOL CjConnectionStrings::AddConnString(int nKey, std::string &strIP, std::string &strDBName, std::string &strUserID, std::string &strUserPass)
{
	return AddConnString(nKey, CString(strIP.c_str()), CString(strDBName.c_str()), CString(strUserID.c_str()), CString(strUserPass.c_str()));
}

BOOL CjConnectionStrings::AddConnString(int nKey, CString &csIP, CString &csDBName, CString &csUserID, CString &csUserPass)
{
	CString csConnString;

	csConnString.Format("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s"
						,csUserPass.GetBuffer()
						,csUserID.GetBuffer()
						,csDBName.GetBuffer()
						,csIP.GetBuffer()
						);

	lockOn();

	if ( m_mConnStrings.find(nKey) != m_mConnStrings.end() )
	{
		//
		// the Key already exists in the map
		//

		return FALSE;
	}

	m_mConnStrings[nKey] = csConnString;

	lockOff();

	return TRUE;
}

BOOL CjConnectionStrings::GetConnString(int nKey, CString &csConnString)
{
	lockOn();

	if ( m_mConnStrings.find(nKey) == m_mConnStrings.end() )
	{
		//
		// could not find
		//
		return FALSE;
	}

	csConnString = m_mConnStrings[nKey];	

	lockOff();

	return TRUE;
}

