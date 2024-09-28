#include "stdafx.h"
#include "../SigmaCore/Database/DbDefine.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"
#include "./VersionManAdo.h"

#include "../SigmaCore/DebugInclude.h"

VersionManAdo::VersionManAdo()
{
}

VersionManAdo::~VersionManAdo()
{
    CloseVerDB();
}

int	VersionManAdo::OpenVerDB(
    const std::string& DbSource,
    const char* szOdbcName,
    const char* szUsrID,
    const char* szUsrPasswd,
    const char* szDbName)
{
    if (!szUsrID || !szUsrPasswd || !szDbName)
        return sc::db::DB_ERROR;

    m_ConnectionString = sc::string::format(
            "Provider=SQLOLEDB.1;Password=%1%;Persist Security Info=True;User ID=%2%;Initial Catalog=%3%;Data Source=%4%",
            szUsrPasswd,
            szUsrID,
            szDbName,
            DbSource);
    if (!m_Ado.Open(m_ConnectionString))
        return sc::db::DB_ERROR;
    else
        return sc::db::DB_OK;
}

void VersionManAdo::CloseVerDB()
{
    if (m_Ado.IsOpened())
        m_Ado.Close();
}

int VersionManAdo::GetAllFileList(std::vector<FullFileList>& v)
{
    std::string Query("SELECT Path, cf_name, cf_version, cf_idx, cf_isPreDown FROM ViewClientFile");
    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    FullFileList FileInfo;

    std::string FolderName;    
    m_Ado.GetCollect("Path", FolderName);    
    if (!FolderName.empty())
    {
        sc::string::lowerString(FolderName);
        FileInfo.strFolder = FolderName.c_str();
    }

    std::string FileName;
    m_Ado.GetCollect("cf_name", FileName);
    if (!FileName.empty())
    {
        sc::string::lowerString(FileName);
        FileInfo.strFileName = FileName.c_str();

        m_Ado.GetCollect("cf_version", FileInfo.nVersion);
        m_Ado.GetCollect("cf_idx", FileInfo.nIdx);
		m_Ado.GetCollect("cf_isPreDown", FileInfo.nIsPreDown);
    }

    v.push_back(FileInfo);

    END_GETCOLLECT(m_Ado);

    return sc::db::DB_OK;
}

int	VersionManAdo::GetFileList(int cd_idx, std::vector<FileList>& v)
{
    std::string Query(
        sc::string::format(
            "SELECT cf_idx, cf_version, cf_name, CONVERT(varchar, cf_date, 20) AS Expr1, cf_desc, cf_upload, cf_isPreDown FROM ClientFile WHERE cd_idx=%1% ORDER BY cf_name",
            cd_idx));
    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    FileList Data;

    std::string strTemp;

    m_Ado.GetCollect("cf_idx", Data.nIdx);
    m_Ado.GetCollect("cf_version", Data.nVer);
    m_Ado.GetCollect("cf_name", strTemp);
    Data.strFile = strTemp.c_str();
    m_Ado.GetCollect("Expr1", strTemp);
    Data.strDate = strTemp.c_str();
    m_Ado.GetCollect("cf_desc", strTemp);
    Data.strDesc = strTemp.c_str();
    m_Ado.GetCollect("cf_upload", Data.nFtp);
	m_Ado.GetCollect("cf_isPreDown", Data.nIsPreDown);

    v.push_back(Data);

    END_GETCOLLECT(m_Ado);

    return sc::db::DB_OK;
}

CString	VersionManAdo::GetFolderName(int nDirNum)
{
    std::string Query(
        sc::string::format(
            "SELECT Path FROM ClientDir WHERE cd_idx=%1%",
            nDirNum));

    CString ReturnStr;
    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    std::string Path;
    m_Ado.GetCollect("Path", Path);
    ReturnStr = Path.c_str();

    END_GETCOLLECT(m_Ado);

    return ReturnStr;
}

int VersionManAdo::DeleteFile(int nFileNum)
{
    std::string Query(
        sc::string::format(
            "DELETE ClientFile WHERE cf_idx=%1%",
            nFileNum));
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int VersionManAdo::GetFolderList(std::vector<FolderList>& v)
{
    std::string Query("SELECT cd_idx, Path FROM ClientDir ORDER BY Path");
    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    FolderList Data;
    std::string FolderName;
    m_Ado.GetCollect("cd_idx", Data.nIdx);
    m_Ado.GetCollect("Path", FolderName);
    Data.strName = FolderName.c_str();

    v.push_back(Data);

    END_GETCOLLECT(m_Ado);
    return sc::db::DB_OK;
}

int VersionManAdo::CreateFolder(const CString& strDir)
{
    std::string Query(
        sc::string::format(
            "INSERT INTO ClientDir (Path) VALUES ('%1%')",
            strDir.GetString()));
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int	VersionManAdo::RemoveFolder(int nDirNum)
{
    std::string Query1(
        sc::string::format(
            "DELETE FROM ClientFile WHERE cd_idx=%1%",
            nDirNum));
    if (m_Ado.Execute4Cmd(Query1))
    {
        std::string Query2(
            sc::string::format(
                "DELETE FROM ClientDir WHERE cd_idx=%1%",
                nDirNum));
        if (m_Ado.Execute4Cmd(Query2))
            return sc::db::DB_OK;
        else
            return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_ERROR;
    }
}

int VersionManAdo::GetNotUploadedFileList(std::vector<FullFileList>& v)
{
    std::string Query("SELECT Path, cf_name, cf_version, cf_idx, cf_isPreDown FROM ViewClientFile WHERE cf_upload=0");
    
    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    FullFileList Data;    
    
    std::string Path;
    m_Ado.GetCollect("Path", Path);
    if (!Path.empty())
    {
        sc::string::lowerString(Path);
        Data.strFolder = Path.c_str();
    }

    std::string FileName;
    m_Ado.GetCollect("cf_name", FileName);
    if (!FileName.empty())
    {
        sc::string::lowerString(FileName);
        Data.strFileName = FileName.c_str();
        m_Ado.GetCollect("cf_version", Data.nVersion);
        m_Ado.GetCollect("cf_idx", Data.nIdx);
		m_Ado.GetCollect("cf_isPreDown", Data.nIsPreDown);
    }

    v.push_back(Data);

    END_GETCOLLECT(m_Ado);

    return sc::db::DB_OK;
}

int VersionManAdo::UpdateFileStateTrue(int nIdx)
{
    std::string Query(
        sc::string::format(
            "UPDATE ClientFile SET cf_upload=1 WHERE cf_idx=%1%",
            nIdx));
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int VersionManAdo::UpdateFileStateFalse(const CString& strFileName)
{
    std::string Query(
        sc::string::format(
            "UPDATE ClientFile SET cf_upload=0 WHERE cf_name='%1%'",
            strFileName.GetString()));
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

BOOL VersionManAdo::IsFolderExist(const CString& strFolderName)
{
    BOOL bReturn = FALSE;
    std::string Query(
        sc::string::format(
            "SELECT Path FROM ClientDir WHERE Path='%1%'",
            strFolderName.GetString()));
    sc::string::replace_all(Query, std::string("/"), std::string("\\"));

    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);
    
    std::string Path;
    m_Ado.GetCollect("Path", Path);
    bReturn = TRUE;

    END_GETCOLLECT(m_Ado);

    return bReturn;
}

int VersionManAdo::GetFolderNum(const CString& strName)
{
    std::string Query(
        sc::string::format(
            "SELECT cd_idx FROM ClientDir WHERE Path='%1%'",
            strName.GetString()));
    sc::string::replace_all(Query, std::string("/"), std::string("\\"));

    m_Ado.Execute4Cmd(Query);

    int nIdx = 0;
    BEGIN_GETCOLLECT(m_Ado);

    m_Ado.GetCollect("cd_idx", nIdx);

    END_GETCOLLECT(m_Ado);

    return nIdx;
}

BOOL VersionManAdo::IsFileExist(const CString& strFileName, int nDir)
{
    std::string Query(
        sc::string::format(
            //"SELECT cf_name FROM ClientFile WHERE cf_name='%1%' AND cd_idx = %2%",
            //strFileName.GetString(), nDir));
			// 중복 파일명 사용가능하도록 수정했으나 게임내 텍스쳐는 경로를 구분하지 않고 이름으로 구분하기 때문에 적용할 수 없음
			"SELECT cf_name FROM ClientFile WHERE cf_name='%1%'", strFileName.GetString()));
    BOOL bReturn = FALSE;

    m_Ado.Execute4Cmd(Query);

    BEGIN_GETCOLLECT(m_Ado);

    std::string FileName;
    m_Ado.GetCollect("cf_name", FileName);
    bReturn = TRUE;

    END_GETCOLLECT(m_Ado);

    return bReturn;
}

int VersionManAdo::GetFileFolderNum(const CString& strFileName)
{
    std::string Query(
        sc::string::format(
            "SELECT cd_idx FROM ClientFile WHERE cf_name='%1%'",
            strFileName.GetString()));

    m_Ado.Execute4Cmd(Query);

    int nIdx = 0;
    BEGIN_GETCOLLECT(m_Ado);

    m_Ado.GetCollect("cd_idx", nIdx);
    
    END_GETCOLLECT(m_Ado);

    return nIdx;
}

int VersionManAdo::UpdateFile(const CString& strFileName, const CString& strMD5, int nVerIncreaseCount, int nDir)
{
    std::string Query(
        sc::string::format(
			// 중복 파일명 사용가능하도록 수정했으나 게임내 텍스쳐는 경로를 구분하지 않고 이름으로 구분하기 때문에 적용할 수 없음
            //"UPDATE ClientFile SET cf_version = cf_version + %1%, cf_date=getdate(), cf_md5='%2%' WHERE cf_name='%3%' AND cd_idx = %4%",
			"UPDATE ClientFile SET cf_version = cf_version + %1%, cf_date=getdate(), cf_md5='%2%' WHERE cf_name='%3%'",
			nVerIncreaseCount,
            strMD5.GetString(),
            strFileName.GetString()));
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int VersionManAdo::UpdateIsPreDown(const CString& strFileName, const int& nIsPreDown)
{
	std::string Query(
		sc::string::format(
		"UPDATE ClientFile SET cf_isPreDown = '%1%' WHERE cf_name='%2%'",
		nIsPreDown,
		strFileName.GetString()));
	if (m_Ado.Execute4Cmd(Query))
		return sc::db::DB_OK;
	else
		return sc::db::DB_ERROR;
}

int VersionManAdo::InsertFile(int nDirNum, const CString& strFileName, const CString& strMD5)
{
	// 2014. 11. 24
	// cf_upload가 0이여야지 FTP 전송하는데 NULL로 되어서 FTP 전송 못함
	// cf_size, cf_upload, cf_FileSize 를 추가하는 부분이 없어서 강제로 0으로 셋팅
	// 기존에는 테이블 기본값으로 설정이 되어있어서 문제가 없었는데 변경하면서 문제가 생김. 차후 다른 DB로 이동할때도 같은 문제가 발생할 수 있으므로 default 값 강제로 추가
//#ifdef PREDOWNLOAD
    std::string Query(
        sc::string::format(
            "INSERT INTO ClientFile (cd_idx, cf_name, cf_md5, cf_version, cf_size, cf_upload, cf_FileSize, cf_date) VALUES (%1%, '%2%', '%3%', 1, 0, 0, 0, GETDATE())",
            nDirNum,
            strFileName.GetString(),
            strMD5.GetString()));
//#else
//    std::string Query(
//        sc::string::format(
//            "INSERT INTO ClientFile (cd_idx, cf_name, cf_md5, cf_version, cf_size, cf_upload, cf_FileSize, cf_md5, cf_date) VALUES (%1%, '%2%', '%3%', 1, 0, 0, 0, '', GETDATE())",
//
//            nDirNum,
//            strFileName.GetString(),
//            strMD5.GetString()));
//#endif
    if (m_Ado.Execute4Cmd(Query))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int VersionManAdo::GetFileIndex(const CString& strFileName, int nDir)
{
	std::string Query(
		sc::string::format(
		// 중복 파일명 사용가능하도록 수정했으나 게임내 텍스쳐는 경로를 구분하지 않고 이름으로 구분하기 때문에 적용할 수 없음
		//"SELECT cf_idx FROM ViewClientFile WHERE cf_name='%1%' AND cd_idx = %2%",
		"SELECT cf_idx FROM ViewClientFile WHERE cf_name='%1%'",
		strFileName.GetString()));

	m_Ado.Execute4Cmd(Query);

	int nIdx = -1;
	BEGIN_GETCOLLECT(m_Ado);

	m_Ado.GetCollect("cf_idx", nIdx);

	END_GETCOLLECT(m_Ado);

	return nIdx;
}