///////////////////////////////////////////////////////////////////////////////
// CGmCfg.cpp
// 
// class CGmCfg
//
// * History
// 2003.09.09 jgkim Create
//
// Copyright 2002-2004 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// Config file load class
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <stdio.h>
#include <string.h>
#include <time.h>
#include "CGmCfg.h"
#include "c_CBit.h" // NetServerLib 
#include "../../EngineLib/Common/IniLoader.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/XML/2.5.3/ticpp.h"
#include "../../SigmaCore/Log/LogMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGmCfg* CGmCfg::SelfInstance = NULL;

CGmCfg::CGmCfg()
{
	SetDefault();
}

CGmCfg* CGmCfg::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CGmCfg();

	return SelfInstance;
}

void CGmCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CGmCfg::SetDefault(void)
{
	strcpy(m_szDBType, "ADO");
	m_dwSGNum = 0;
	m_nDBType = gmce::ADO;
}

CGmCfg::~CGmCfg()
{
}

CString	CGmCfg::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

gmce::IDBManager *CGmCfg::GetDBMan()
{
	if (m_nDBType == gmce::ADO)
		return CADOManager::GetInstance();
	else
		return CGMOdbcManager::GetInstance();
}

int CGmCfg::LoadODBCInfo()
{
	// Data type problem
	CString strFileName = GetAppPath() + "\\GMChaEdit.cfg";
	CFile cfgFile;
	CFileException fileException;
	if (!cfgFile.Open(strFileName.GetString(), CFile::modeRead, &fileException))
	{
		TRACE( "Can't open file %s, error = %u\n", strFileName, fileException.m_cause );
		return FALSE;
	}
	else
	{
		GM_UTIL::CBit cBit;

		cfgFile.Read(m_szOdbcName, GM_MAX_NAME_LENGTH);
		cfgFile.Read(m_szOdbcUserName, GM_MAX_NAME_LENGTH);
		cfgFile.Read(m_szOdbcPassword, GM_MAX_NAME_LENGTH);

		cBit.buf_decode(m_szOdbcName,     GM_MAX_NAME_LENGTH, 1, 7);
		cBit.buf_decode(m_szOdbcUserName, GM_MAX_NAME_LENGTH, 1, 7);
		cBit.buf_decode(m_szOdbcPassword, GM_MAX_NAME_LENGTH, 1, 7);


		m_nDBType = gmce::ODBC;

		return TRUE;
	}
}

int CGmCfg::SaveODBCInfo()
{
	CString strFileName = GetAppPath() + "\\GMChaEdit.cfg";
	CFile cfgFile;
	CFileException fileException;

	if (!cfgFile.Open( strFileName.GetString(), CFile::modeCreate | CFile::modeReadWrite, &fileException))
	{
		TRACE( "Can't open file %s, error = %u\n", strFileName, fileException.m_cause );
		return FALSE;
	}
	else
	{
		GM_UTIL::CBit cBit;

		cBit.buf_encode(m_szOdbcName,     GM_MAX_NAME_LENGTH, 1, 7);
		cBit.buf_encode(m_szOdbcUserName, GM_MAX_NAME_LENGTH, 1, 7);
		cBit.buf_encode(m_szOdbcPassword, GM_MAX_NAME_LENGTH, 1, 7);

        cfgFile.Write(m_szOdbcName,		sizeof(m_szOdbcName));
		cfgFile.Write(m_szOdbcUserName, sizeof(m_szOdbcName));
		cfgFile.Write(m_szOdbcPassword, sizeof(m_szOdbcName));
		cfgFile.Close();

		return TRUE;
	}
}

int CGmCfg::Process(char* sLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = " ,\t\n";
	char *token;

	// Establish string and get the first token
	token = ::strtok(sLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (::strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		
		else if (::strcmp(token, "odbc_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_db_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcDbName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcUserName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcPassword, token);
			else
				return 0;
		}		
		else
		{
			
		}
		// Get next token
		token = ::strtok( NULL, seps );
	}
	return 0;
}

char* CGmCfg::GetOdbcName()
{
	return m_szOdbcName;
}

char* CGmCfg::GetUserName()
{
	return m_szOdbcUserName;
}

char* CGmCfg::GetUserPass()
{
	return m_szOdbcPassword;
}

void  CGmCfg::SetOdbcName(CString strOdbcName)
{
	::StringCchCopy(m_szOdbcName, GM_MAX_NAME_LENGTH, strOdbcName.GetString());
}

void  CGmCfg::SetUserName(CString strUserName)
{
	::StringCchCopy(m_szOdbcUserName, GM_MAX_NAME_LENGTH, strUserName.GetString());
}
void  CGmCfg::SetUserPass(CString strUserPass)
{
	::StringCchCopy(m_szOdbcPassword, GM_MAX_NAME_LENGTH, strUserPass.GetString());
}


#ifdef TIXML_USE_TICPP
//
//LoadADOInfo() load db information from .xml file includes encoded string.
//
BOOL CGmCfg::LoadADOInfo(bool bToolMode)
{
	CString csFileName = GetAppPath() + "\\" + CONFIG_FILENAME_XML;
	
	gmce::A_GROUP agroup;

	try
	{
		ticpp::Document doc;

		doc.LoadFile(csFileName.GetBuffer());

		//ticpp::Iterator<ticpp::Element> pHead;

		ticpp::Element* pHead	= NULL;
		ticpp::Element* pDB		= NULL;
		ticpp::Element* pElem	= NULL;

		
		pHead = doc.FirstChildElement("groups");

		pHead = pHead->FirstChildElement("group");
		
		
		do
		{	
			{		
				pElem = pHead->FirstChildElement( "title" );
				agroup.title = pElem->GetText();
				
				pDB = pHead->FirstChildElement( "userdb" );
				{
					pElem = pDB->FirstChildElement("name");
					agroup.userDB.name = pElem->GetText();
					Decrypt(agroup.userDB.name,	agroup.userDB.name);

					pElem = pDB->FirstChildElement( "ip" );
					agroup.userDB.ip = pElem->GetText();
					Decrypt(agroup.userDB.ip,	agroup.userDB.ip);

					pElem = pDB->FirstChildElement( "id" );
					agroup.userDB.id = pElem->GetText();
					Decrypt(agroup.userDB.id,	agroup.userDB.id);

					pElem = pDB->FirstChildElement( "pw" );
					agroup.userDB.pw = pElem->GetText();
					Decrypt(agroup.userDB.pw,	agroup.userDB.pw);
				}
			

				pDB = pHead->FirstChildElement( "gamedb" );
				{
					pElem = pDB->FirstChildElement("name");
					agroup.gameDB.name = pElem->GetText();
					Decrypt(agroup.gameDB.name,	agroup.gameDB.name);

					pElem = pDB->FirstChildElement( "ip" );
					agroup.gameDB.ip = pElem->GetText();
					Decrypt(agroup.gameDB.ip,	agroup.gameDB.ip);

					pElem = pDB->FirstChildElement( "id" );
					agroup.gameDB.id = pElem->GetText();
					Decrypt(agroup.gameDB.id,	agroup.gameDB.id);

					pElem = pDB->FirstChildElement( "pw" );
					agroup.gameDB.pw = pElem->GetText();
					Decrypt(agroup.gameDB.pw,	agroup.gameDB.pw);
				}
			
				pDB = pHead->FirstChildElement( "logdb" );
				{
					pElem = pDB->FirstChildElement("name");
					agroup.logDB.name = pElem->GetText();
					Decrypt(agroup.logDB.name,	agroup.logDB.name);

					pElem = pDB->FirstChildElement( "ip" );
					agroup.logDB.ip = pElem->GetText();
					Decrypt(agroup.logDB.ip,	agroup.logDB.ip);

					pElem = pDB->FirstChildElement( "id" );
					agroup.logDB.id = pElem->GetText();
					Decrypt(agroup.logDB.id,	agroup.logDB.id);

					pElem = pDB->FirstChildElement( "pw" );
					agroup.logDB.pw = pElem->GetText();
					Decrypt(agroup.logDB.pw,	agroup.logDB.pw);
				}
			}

			m_vGROUPS.push_back(agroup);
		}
		while ( pHead = pHead->NextSiblingElement(false) );	

	}
	catch ( ticpp::Exception& ex )
	{
		sc::writeLogError(std::string("CGmCfg::LoadADOInfo - ") + ex.m_details);
		
        size_t t = m_vGROUPS.size();

		if( t <= 0)			
			return FALSE;
	}	
	
	return TRUE;
}

#else

//
//LoadADOInfo() load db information from .bin file encoded by RanEditor.
//
BOOL CGmCfg::LoadADOInfo(bool bToolMode)
{
	CString csFileName = GetAppPath() + "\\" + CONFIG_FILENAME_BIN;
	
	gmce::DB_INFO DBINFO;
	
	CIniLoader cFILE(",", bToolMode);

	if( !cFILE.open( csFileName.GetBuffer(), true) )
	{
		return FALSE;
	}


	cFILE.SetUseErrorMsgBox( FALSE );

	{
		cFILE.getflag_sz( "DATABASE", "TYPE", 0, 1, m_szDBType, 8 );
		cFILE.getflag ( "DATABASE", "SGNum", 0, 1, m_dwSGNum );


		if ( strcmp(m_szDBType, "ADO") || m_dwSGNum <= 0 )
		{
			m_nDBType = gmce::ODBC;
			//ODBC
			strcpy(m_szDBType, "ODBC");

			return FALSE;	// use ODBC load routine.
		}
		else
		{
			m_nDBType = gmce::ADO;
		}
	}

	if (m_nDBType == gmce::ADO)
	{
		CString csSG;
		for (int i=0; i<m_dwSGNum; i++)
		{
			ZeroMemory(&DBINFO, sizeof(DBINFO));

			csSG.Format("ADO_SG_%d", i);		

			cFILE.getflag_sz( csSG.GetBuffer(), "UserDB_DB", 0, 1, DBINFO.UserDB.DB, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "UserDB_IP", 0, 1, DBINFO.UserDB.IP, 16 );
			cFILE.getflag_sz( csSG.GetBuffer(), "UserDB_ID", 0, 1, DBINFO.UserDB.ID, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "UserDB_PW", 0, 1, DBINFO.UserDB.PW, 32 );

			cFILE.getflag_sz( csSG.GetBuffer(), "GameDB_DB", 0, 1, DBINFO.GameDB.DB, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "GameDB_IP", 0, 1, DBINFO.GameDB.IP, 16 );
			cFILE.getflag_sz( csSG.GetBuffer(), "GameDB_ID", 0, 1, DBINFO.GameDB.ID, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "GameDB_PW", 0, 1, DBINFO.GameDB.PW, 32 );

			cFILE.getflag_sz( csSG.GetBuffer(), "LogDB_DB", 0, 1, DBINFO.LogDB.DB, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "LogDB_IP", 0, 1, DBINFO.LogDB.IP, 16 );
			cFILE.getflag_sz( csSG.GetBuffer(), "LogDB_ID", 0, 1, DBINFO.LogDB.ID, 32 );
			cFILE.getflag_sz( csSG.GetBuffer(), "LogDB_PW", 0, 1, DBINFO.LogDB.PW, 32 );

			m_vDBINFO.push_back(DBINFO);
		}
	}

	return TRUE;
}

#endif


void CGmCfg::Encrypt(std::string &strPlain, std::string &strEncrypted)
{
    sc::DbEncrypt Encrypt;
    Encrypt.encrypt(strPlain, strEncrypted);
}


void CGmCfg::Encrypt(CString &csPlain, CString &csEncrypted)
{
	std::string strEncrypted;

	Encrypt(std::string(csPlain.GetBuffer()), strEncrypted);

	csEncrypted.SetString(strEncrypted.c_str());
}


void CGmCfg::Decrypt(std::string &strEncrypted, std::string &strPlain)
{	
    sc::DbEncrypt Decrypt;
    Decrypt.decrypt(strEncrypted, strPlain);
}


void CGmCfg::Decrypt(CString &csEncrypted, CString &csPlain)
{
	std::string strPlain;

	Decrypt(std::string(csEncrypted.GetBuffer()), strPlain);

	csPlain.SetString(strPlain.c_str());
}

