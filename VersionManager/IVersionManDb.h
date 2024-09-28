#ifndef _VERSION_MAN_IVERSION_MAN_DB_H_
#define _VERSION_MAN_IVERSION_MAN_DB_H_

#pragma once

#include <vector>
#include "DatabaseTable.h"

class IVersionManagerDb
{
public:
    IVersionManagerDb();
    virtual ~IVersionManagerDb();

protected:
    std::string m_DbUserID;
    std::string m_DbUserPassword;
    std::string m_DbName;	

public:
    virtual int	OpenVerDB(
        const std::string& DbSource,
        const char* szOdbcName,
        const char* szUsrID,
        const char* szUsrPasswd,
        const char* szDbName) = 0;
    virtual void CloseVerDB() = 0;
    virtual int GetAllFileList(std::vector<FullFileList>& v) = 0;
    virtual int	GetFileList(int cd_idx, std::vector<FileList>& v) = 0;
	virtual int GetFileIndex(const CString& strFileName, int nDir) = 0;
    virtual CString	GetFolderName(int nDirNum) = 0;
    virtual int DeleteFile(int nFileNum) = 0;
    virtual int GetFolderList(std::vector<FolderList>& v) = 0;
    virtual int CreateFolder(const CString& strDir) = 0;
    virtual int	RemoveFolder(int nDirNum) = 0;
    virtual int GetNotUploadedFileList(std::vector<FullFileList>& v) = 0;
    virtual int UpdateFileStateTrue(int nIdx) = 0;
    virtual int UpdateFileStateFalse(const CString& strFileName) = 0;
    virtual BOOL IsFolderExist(const CString& strFolderName) = 0;
    virtual int GetFolderNum(const CString& strName) = 0;
    virtual BOOL IsFileExist(const CString& strFileName, int nDir) = 0;
    virtual int GetFileFolderNum(const CString& strFileName) = 0;
    virtual int UpdateFile(const CString& strFileName, const CString& strMD5, int nVerIncreaseCount, int nDir) = 0;
	virtual int UpdateIsPreDown(const CString& strFileName, const int& nIsPreDown) = 0;
    virtual int InsertFile(int nDirNum, const CString& strFileName, const CString& strMD5) = 0;
};

#endif // _VERSION_MAN_IVERSION_MAN_DB_H_