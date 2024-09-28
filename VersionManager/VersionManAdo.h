#ifndef _VERSION_MAN_ADO_H_
#define _VERSION_MAN_ADO_H_

#pragma once

#include "../SigmaCore/Database/Ado/AdoClass.h"
#include "IVersionManDb.h"

class VersionManAdo : public IVersionManagerDb
{
public:
    VersionManAdo();
    virtual ~VersionManAdo();

protected:
    sc::db::CjADO m_Ado;
    std::string m_ConnectionString;

public:
    virtual int	OpenVerDB(
        const std::string& DbSource,
        const char* szOdbcName,
        const char* szUsrID,
        const char* szUsrPasswd,
        const char* szDbName) override;
    virtual void CloseVerDB() override;
    virtual int GetAllFileList(std::vector<FullFileList>& v) override;
    virtual int	GetFileList(int cd_idx, std::vector<FileList>& v) override;
	virtual int GetFileIndex(const CString& strFileName, int nDir) override;
    virtual CString	GetFolderName(int nDirNum) override;
    virtual int DeleteFile(int nFileNum) override;
    virtual int GetFolderList(std::vector<FolderList>& v) override;
    virtual int CreateFolder(const CString& strDir) override;
    virtual int	RemoveFolder(int nDirNum) override;
    virtual int GetNotUploadedFileList(std::vector<FullFileList>& v) override;
    virtual int UpdateFileStateTrue(int nIdx) override;
    virtual int UpdateFileStateFalse(const CString& strFileName) override;
    virtual BOOL IsFolderExist(const CString& strFolderName) override;
    virtual int GetFolderNum(const CString& strName) override;
    virtual BOOL IsFileExist(const CString& strFileName, int nDir) override;
    virtual int GetFileFolderNum(const CString& strFileName) override;
    virtual int UpdateFile(const CString& strFileName, const CString& strMD5, int nVerIncreaseCount, int nDir) override;
	virtual int UpdateIsPreDown(const CString& strFileName, const int& nIsPreDown) override;
    virtual int InsertFile(int nDirNum, const CString& strFileName, const CString& strMD5) override;
};

#endif // _VERSION_MAN_ADO_H_