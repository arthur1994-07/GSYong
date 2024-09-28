#pragma once

#ifndef _C_COdbcManager_H_
#define _C_COdbcManager_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Auto Link Library Files
// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <Windows.h>
#include <stdio.h>
#include <vector>

#include "IVersionManDb.h"
#include "DatabaseTable.h"

// Seconds of database response time
#define DB_RESPONSE_TIME	30
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2

// string data size
#define USR_ID_LENGTH				20
#define CHR_ID_LENGTH				33
#define USR_PASS_LENGTH				20

#define CHAT_MSG_SIZE				100
#define GLOBAL_CHAT_MSG_SIZE		981
#define DB_SVR_NAME_LENGTH			50
#define DB_NAME_LENGTH				50
#define SERVER_NAME_LENGTH			50
#define MAP_NAME_LENGTH				50

// #ifndef MAX_SERVERCHAR_NUM
//	#define MAX_SERVERCHAR_NUM		2 // 전체 서버에서 만들수 있는 캐릭터 갯수
// #endif

#define MAX_ONESERVERCHAR_NUM		30 // 한 서버에서 만들수 있는 캐릭터 갯수

class VersionManOdbc : public IVersionManagerDb
{
public:
	VersionManOdbc();
	virtual ~VersionManOdbc();

protected:
   	SQLHENV hEnvVer;  // Handle of sql (void*)	
	SQLHDBC hOdbcVer; // Handle of ODBC (void*)	

    std::string m_OdbcName;    
	int	m_nVerDBTimeOut;

public:	
	void SetVerDB(
			const char* szOdbcName,
			const char* szUsrID,
			const char* szUsrPass,
			const char* szDBName );

	void SetVerOdbcName(const char* szName);

	void SetVerUserID(const char* szUsrID);

	void SetVerUserPass(const char* szUsrPasswd);
	void SetVerDBName(const char* szDBName);
	void SetVerDBServer(const char* szName, const char* szUsrID, const char* szUsrPass, const char* szDBName);
	int OpenVerDB();

	virtual int	OpenVerDB(
        const std::string& DbSource,
        const char* szOdbcName,
        const char* szUsrID,
        const char* szUsrPasswd,
        const char* szDbName) override;

	virtual void CloseVerDB() override;

	// Get Folder List
	virtual int GetFolderList(std::vector<FolderList>& v) override;

	// Get File List
	virtual int	GetFileList(int cd_idx, std::vector<FileList>& v) override;

    virtual int GetAllFileList(std::vector<FullFileList>& v) override;

    virtual int GetNotUploadedFileList(std::vector<FullFileList>& v) override;

    virtual int UpdateFileStateTrue(int nIdx) override;

    virtual int UpdateFileStateFalse(const CString& strFileName) override;

	virtual int CreateFolder(const CString& strDir) override;

	virtual int	RemoveFolder(int nDirNum) override;
	
    virtual CString	GetFolderName(int nDirNum) override;

    virtual int GetFolderNum(const CString& strName) override;

    virtual int GetFileFolderNum(const CString& strFileName) override;

	virtual int InsertFile(int nDirNum, const CString& strFileName, const CString& strMD5) override;

	// 파일이 변경되었을때 변경된 파일의
	// 버전정보와 md5 값을 갱신한다.
	virtual int UpdateFile(const CString& strFileName, const CString& strMD5, int nVerIncreaseCount, int nDir) override;

    virtual int DeleteFile(int nFileNum) override;

    virtual BOOL IsFileExist(const CString& strFileName, int nDir) override;

    virtual BOOL IsFolderExist(const CString& strFolderName) override;

	int ExecuteSQL(const CString& strSQL);

	int Insert(const CString& strSQL);
	
	int Update(const CString& strSQL);

	int	Delete(const CString& strSQL);

	void msgBox(const CString& strMsg);
};

#endif _C_COdbcManager_H_