///////////////////////////////////////////////////////////////////////////////
// s_CGmCfg.h
//
// class CGmCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CGmCfg_H_
#define CGmCfg_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "../Database/DbBase.h"

#define GM_MAX_NAME_LENGTH 100

#define CONFIG_FILENAME_XML		"GMCharEdit.xml"
#define CONFIG_FILENAME_BIN		"GMCharEdit.bin"

class CGmCfg
{
protected:
	
public:
	static CGmCfg* GetInstance();
	static void	ReleaseInstance();

	void		SetDefault(void);
	int			Process(char* sLine);
	int			LoadODBCInfo();	
	int			SaveODBCInfo();
	CString		GetAppPath();

	gmce::IDBManager *GetDBMan();


	gmce::ENUM_DB_TYPE m_nDBType;

	TCHAR		m_szDBType[8];	
	DWORD		m_dwSGNum;	

/*
	typedef struct
	{
		TCHAR IP[16];
		TCHAR ID[32];
		TCHAR PW[32];
	} AUTH;

	typedef struct {
		AUTH UserDB;
		AUTH GameDB;
		AUTH LogDB;
	} DB_INFO;
*/



	//
	//mjeon.config
	//
#ifdef TIXML_USE_TICPP	
	//
	//XML
	//
	std::vector<gmce::A_GROUP>						m_vGROUPS;
	typedef std::vector<gmce::A_GROUP>::iterator	iterGROUPS;
#else
	//
	//BIN
	//
	std::vector<gmce::DB_INFO>						m_vDBINFO;
	typedef std::vector<gmce::DB_INFO>::iterator	iterDBINFO;
#endif

	


private:
	CGmCfg();
	virtual ~CGmCfg();

	static CGmCfg* SelfInstance;

protected:
	char m_szOdbcName[GM_MAX_NAME_LENGTH];
	char m_szOdbcDbName[GM_MAX_NAME_LENGTH];
	char m_szOdbcUserName[GM_MAX_NAME_LENGTH];
	char m_szOdbcPassword[GM_MAX_NAME_LENGTH];
	char m_szFtpIP[GM_MAX_NAME_LENGTH];
	int  m_nFtpPort;
	char m_szFtpUserName[GM_MAX_NAME_LENGTH];
	char m_szFtpPassword[GM_MAX_NAME_LENGTH];
	char m_szTargetPath[256];	

public:
	char* GetOdbcName();
	char* GetUserName();
	char* GetUserPass();
	void  SetOdbcName(CString strOdbcName);
	void  SetUserName(CString strUserName);
	void  SetUserPass(CString strUserPass);

	BOOL  LoadADOInfo(bool bToolMode);

	void Encrypt(std::string &strPlain, std::string &strEncrypted);
	void Encrypt(CString &csPlain, CString &csEncrypted);

	void Decrypt(std::string &strEncrypted, std::string &Plain);
	void Decrypt(CString &csEncrypted, CString &csPlain);

};

#endif // CGmCfg_H_