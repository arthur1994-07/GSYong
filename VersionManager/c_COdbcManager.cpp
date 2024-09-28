#include "StdAfx.h"
#include <strstream>
#include "c_COdbcManager.h"
#include <strsafe.h> // Safe string function

VersionManOdbc::VersionManOdbc()
{
	hEnvVer = NULL;  // Handle of sql (void*)	
	hOdbcVer = NULL;; // Handle of ODBC (void*)	
}

VersionManOdbc::~VersionManOdbc(void)
{
	if (hOdbcVer != NULL)
	{
		::SQLDisconnect(hOdbcVer);
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcVer);
		hOdbcVer = NULL;
	}

	if (hEnvVer != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
	}
}

void VersionManOdbc::SetVerOdbcName(const char* szName)
{
    if (szName)
	    m_OdbcName = szName;
}

void VersionManOdbc::SetVerUserID(const char* szUsrID)
{
    if (szUsrID)
	    m_DbUserID = szUsrID;
}

void VersionManOdbc::SetVerUserPass(const char* szUsrPasswd)
{
    if (szUsrPasswd)
	    m_DbUserPassword = szUsrPasswd;
}

void VersionManOdbc::SetVerDBName(const char* szDBName)
{
    if (szDBName)
	    m_DbName = szDBName;
}

void VersionManOdbc::SetVerDBServer(
	const char* szName,
	const char* szUsrID,
	const char* szUsrPass,
	const char* szDBName)
{
	SetVerOdbcName	(szName);
	SetVerUserID	(szUsrID);
	SetVerUserPass	(szUsrPass);
	SetVerDBName	(szDBName);	
}

int	VersionManOdbc::OpenVerDB()
{
	SQLRETURN sReturn; // SQL return value (short)

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnvVer);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		return DB_ERROR;
	}

	// ODBC set environment
	// version
	sReturn = ::SQLSetEnvAttr(hEnvVer, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
		return DB_ERROR;
	}
    
	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_DBC, hEnvVer, &hOdbcVer);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcVer);
		hOdbcVer = NULL;
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
		return DB_ERROR;
	}

	// ODBC connect
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(hOdbcVer, 5, (void*) SQL_LOGIN_TIMEOUT, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{	
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
		return DB_ERROR;
	}

	sReturn = ::SQLConnect(hOdbcVer, 
						(SQLCHAR*) m_OdbcName.c_str(),
						SQL_NTS,
						(SQLCHAR*) m_DbUserID.c_str(),
						SQL_NTS,
						(SQLCHAR*) m_DbUserPassword.c_str(),
						SQL_NTS);

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
		return DB_ERROR;
	}
}

int	VersionManOdbc::OpenVerDB(
    const std::string& DbSource,
	const char* szOdbcName, 
	const char* szUsrID, 
	const char* szUsrPasswd,
	const char* szDbName )
{
	SetVerDB(szOdbcName, szUsrID, szUsrPasswd, szDbName);
	return OpenVerDB();
}

void VersionManOdbc::CloseVerDB()
{
	if (hOdbcVer != NULL)
	{
		::SQLDisconnect(hOdbcVer);
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcVer);
		hOdbcVer = NULL;
	}

	if (hEnvVer != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvVer);
		hEnvVer = NULL;
	}
}

void VersionManOdbc::SetVerDB(
	const char* szOdbcName,
	const char* szUsrID,
	const char* szUsrPass,
	const char* szDBName )
{
	SetVerOdbcName(szOdbcName);
	SetVerUserID(szUsrID);
	SetVerUserPass(szUsrPass);
}

//int	VersionManOdbc::WriteImage(const char* objName, int nChaNum, char* pData, int nSize)
//{
//	SQLRETURN	sReturn; // SQL return value (short)	
//	SQLHSTMT	hStmt; // Handle of statement (void*)
//	SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
//	SQLINTEGER  lSize=0, lTotalSize=0;
//
//	// Prepare statement handle
//	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcGame, &hStmt);
//	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
//	{
//		
//	}
//	else
//	{
//		return DB_ERROR;
//	}
//
//	std::strstream strTemp;
//	strTemp << "UPDATE ChaInfo SET " << objName << " = ? WHERE (ChaNum=" <<  nChaNum << ")";
//	strTemp << '\0';
//
//	// Prepare parameter
//	sReturn = ::SQLPrepare(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
//	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO) 
//	{
//
//	}
//	else
//	{
//		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
//		return DB_ERROR;
//	}
//
//	// Bind parameter
//	SQLINTEGER cbBinaryParam;
//	sReturn = ::SQLBindParameter(hStmt, 
//								1, 
//								SQL_PARAM_INPUT,
//								SQL_C_BINARY, 
//								SQL_LONGVARBINARY,
//								0, 
//								0, 
//								(SQLPOINTER) 2, 
//								0, 
//								&cbBinaryParam);	
//	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO) 
//	{
//
//	}
//	else
//	{
//		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
//		return DB_ERROR;
//	}
//
//	// Execute SQL
//	sReturn = ::SQLExecute(hStmt);
//	
//	SQLPOINTER pToken;
//	SQLINTEGER cbData;
//	while (sReturn == SQL_NEED_DATA) 
//	{
//		sReturn = ::SQLParamData(hStmt, &pToken);
//		if (sReturn == SQL_NEED_DATA) 
//		{				
//			::SQLPutData(hStmt, pData, (SQLINTEGER) nSize);
//		}
//	}
//	return DB_OK;
//}
//
//int	VersionManOdbc::ReadImage	(const char* objName, int nChaNum, se::ByteStream &ByteStream)
//{	
//	SQLRETURN	sReturn; // SQL return value (short)	
//	SQLHSTMT	hStmt; // Handle of statement (void*)
//	SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
//	SQLINTEGER  lSize=0, lTotalSize=0;
//	
//	// Prepare statement handle
//	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcGame, &hStmt);
//	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
//	{
//		
//	}
//	else
//	{
//		return DB_ERROR;
//	}		
//
//	std::strstream strTemp;
//	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
//	strTemp << '\0';
//
//	// Create a result
//	sReturn = ::SQLExecDirect(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
//	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
//	{
//		
//	}
//	else
//	{
//		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
//		return DB_ERROR;
//	}
//	
//	// Retrieve and display each row of data.
//	while ((sReturn = ::SQLFetch(hStmt)) != SQL_NO_DATA) 
//	{			
//		while (1) 
//		{
//			sReturn = ::SQLGetData(hStmt, 1, SQL_C_BINARY, pBinary, 1024, &lSize);
//			if (sReturn == SQL_NO_DATA || lSize == 0)
//				break;			
//			lTotalSize += lSize;
//			ByteStream.WriteBuffer((LPBYTE) pBinary, lSize);			
//		}
//
//		if ( lTotalSize < 12 )		
//			ByteStream.ClearBuffer ();
//	}
//
//	// Close the cursor.
//	::SQLCloseCursor(hStmt);
//	return DB_OK;
//}

int	VersionManOdbc::GetFolderList(
	std::vector<FolderList> &v )
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	std::strstream strTemp;
	strTemp << "SELECT cd_idx, Path FROM ClientDir ORDER BY Path";
	strTemp << '\0';

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	SQLINTEGER nIdx;
	SQLCHAR szFolderName[256];
	SQLINTEGER cbID = SQL_NTS, cbName = SQL_NTS, cbDESC = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//
			//mjeon.ToDo
			//SQLGetData can be replaced with SQLBindCol().
			//In case of SQLBindCol(), it does not need to be called for every loop.
			//

			::SQLGetData(hStmt, 1, SQL_C_LONG, &nIdx,  0, &cbID); 
			::SQLGetData(hStmt, 2, SQL_C_CHAR, szFolderName,  256, &cbName); 
			FolderList sTemp;
			sTemp.nIdx = nIdx;
			sTemp.strName.Format(CString(szFolderName)); 
			v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}

	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return DB_OK;
}

int VersionManOdbc::GetNotUploadedFileList(
	std::vector<FullFileList> &v )
{
 	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	std::strstream strTemp;
	strTemp << "SELECT Path, cf_name, cf_version, cf_idx FROM ViewClientFile WHERE cf_upload=0";
	strTemp << '\0';

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	
	SQLCHAR szPath[256];
	SQLCHAR szFileName[256];
	
	SQLINTEGER cbPath = SQL_NTS;
	SQLINTEGER cbFileName = SQL_NTS;
    SQLINTEGER cbVersion = SQL_NTS;
    SQLINTEGER cbIdx = SQL_NTS;
    SQLINTEGER nVersion, nIdx;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_CHAR, szPath,  256, &cbPath); 
			::SQLGetData(hStmt, 2, SQL_C_CHAR, szFileName,  256, &cbFileName); 
            ::SQLGetData(hStmt, 3, SQL_C_LONG, &nVersion,  0, &cbVersion);
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nIdx,  0, &cbIdx);
            
			FullFileList sTemp;			
			if (cbPath != 0 && cbPath != -1)            
			{
				sTemp.strFolder.Format(CString(szPath));
				sTemp.strFolder = sTemp.strFolder.MakeLower();
			}
			if (cbFileName != 0 && cbFileName != -1)
			{	
				sTemp.strFileName.Format(CString(szFileName));
				sTemp.strFileName = sTemp.strFileName.MakeLower();
                sTemp.nVersion = nVersion;
                sTemp.nIdx = nIdx;
			}
			
			v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}

	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return DB_OK;  
}

int VersionManOdbc::GetAllFileList(
	std::vector<FullFileList> &v )
{
 	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

#ifdef CHINAPARAM
	std::strstream strTemp;
	strTemp << "SELECT Path, cf_name, cf_version, cf_idx, cf_md5 FROM ViewClientFile";
	strTemp << '\0';
#else
	std::strstream strTemp;
	strTemp << "SELECT Path, cf_name, cf_version, cf_idx FROM ViewClientFile";
	strTemp << '\0';
#endif

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	
	SQLCHAR szPath[FILENAME_SIZE] = {0}; SQLINTEGER cbPath = SQL_NTS;
	SQLCHAR szFileName[SUBPATH_SIZE] = {0}; SQLINTEGER cbFileName = SQL_NTS;
#ifdef CHINAPARAM
	SQLCHAR szMD5[MD5_SIZE] = {0}; SQLINTEGER cbMD5 = SQL_NTS;
#endif

    SQLINTEGER cbVersion = SQL_NTS;
    SQLINTEGER cbIdx = SQL_NTS;
    SQLINTEGER nVersion, nIdx;
    
	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData( hStmt, 1, SQL_C_CHAR, szPath,  SUBPATH_SIZE, &cbPath );
			::SQLGetData( hStmt, 2, SQL_C_CHAR, szFileName,  FILENAME_SIZE, &cbFileName );
            ::SQLGetData( hStmt, 3, SQL_C_LONG, &nVersion,  0, &cbVersion );
            ::SQLGetData( hStmt, 4, SQL_C_LONG, &nIdx,  0, &cbIdx );
#ifdef CHINAPARAM
			::SQLGetData( hStmt, 5, SQL_C_CHAR, szMD5, MD5_SIZE, &cbMD5 );
#endif

			FullFileList sTemp;
			if (cbPath != 0 && cbPath != -1)
			{
				sTemp.strFolder.Format(CString(szPath));
				sTemp.strFolder = sTemp.strFolder.MakeLower();
			}
			if (cbFileName != 0 && cbFileName != -1)
			{
				sTemp.strFileName.Format( CString(szFileName) );
				sTemp.strFileName = sTemp.strFileName.MakeLower();
                sTemp.nVersion = nVersion;
                sTemp.nIdx = nIdx;
			}
#ifdef CHINAPARAM
			if ( cbMD5 != 0 && cbMD5 != -1 )
			{
				sTemp.strMD5.Format( CString(szMD5) );
			}
#endif
			v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}

	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return DB_OK;   
}

int	VersionManOdbc::GetFileList(
	int cd_idx,
	std::vector<FileList> &v )
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

#ifdef CHINAPARAM
	std::strstream strTemp;
	strTemp << "SELECT cf_idx, cf_version, cf_name, CONVERT(varchar, cf_date, 20) AS Expr1, cf_desc, cf_upload, cf_md5 FROM ClientFile ";
	strTemp << " WHERE cd_idx=" << cd_idx;
	strTemp << " ORDER BY cf_name";
	strTemp << '\0';
#else
	std::strstream strTemp;
	strTemp << "SELECT cf_idx, cf_version, cf_name,  CONVERT(varchar, cf_date, 20) AS Expr1, cf_desc, cf_upload FROM ClientFile ";
	strTemp << " WHERE cd_idx=" << cd_idx;
	strTemp << " ORDER BY cf_name";
	strTemp << '\0';
#endif
	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	
	SQLCHAR szFile[emFileList(FILENAME_SIZE)] = {0}; SQLINTEGER cbName = SQL_NTS;
	SQLCHAR szDate[emFileList(DATE_SIZE)] = {0}; SQLINTEGER cbDate = SQL_NTS;
	SQLCHAR szDesc[emFileList(DESC_SIZE)] = {0}; SQLINTEGER cbDesc = SQL_NTS;
	SQLCHAR szMD5[emFileList(MD5_SIZE)] = {0}; SQLINTEGER cbMD5 = SQL_NTS;

	SQLINTEGER nIdx;
	SQLINTEGER nVer;
    SQLINTEGER nUpload;

	SQLINTEGER cbID   = SQL_NTS;	
    SQLINTEGER cbUpload = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData( hStmt, 1, SQL_C_LONG, &nIdx,  0, &cbID );
			::SQLGetData( hStmt, 2, SQL_C_LONG, &nVer,  0, &cbID );
			::SQLGetData( hStmt, 3, SQL_C_CHAR, szFile,  emFileList(FILENAME_SIZE), &cbName );
			::SQLGetData( hStmt, 4, SQL_C_CHAR, szDate,  emFileList(DATE_SIZE), &cbDate );
			::SQLGetData( hStmt, 5, SQL_C_CHAR, szDesc,  emFileList(DESC_SIZE), &cbDesc );
            ::SQLGetData( hStmt, 6, SQL_C_LONG, &nUpload,  0, &cbUpload);
#ifdef CHINAPARAM
			::SQLGetData( hStmt, 7, SQL_C_CHAR, szMD5, emFileList(MD5_SIZE), &cbMD5 );
#endif
			FileList sTemp;
			sTemp.nIdx = nIdx;
			sTemp.nVer = nVer;
            sTemp.nFtp = nUpload;
			if (cbName != 0 && cbName != -1) sTemp.strFile.Format(CString(szFile));
			if (cbDate != 0 && cbDate != -1) sTemp.strDate.Format(CString(szDate));
			if (cbDesc != 0 && cbDesc != -1) sTemp.strDesc.Format(CString(szDesc));
#ifdef CHINAPARAM
			if (cbMD5 != 0 && cbMD5 != -1) sTemp.strMD5.Format( CString(szMD5) );
#endif
			v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}

	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return DB_OK;
}

CString VersionManOdbc::GetFolderName(
	int nDirNum )
{
    SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	CString strReturn;
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return strReturn;
	}

	std::strstream strTemp;
	strTemp << "SELECT Path FROM ClientDir ";
	strTemp << " WHERE cd_idx=" << nDirNum;
	strTemp << '\0';

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return strReturn;
	}

	SQLCHAR szPath[256];	
	SQLINTEGER cbPath = SQL_NTS;	

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_CHAR, szPath, 256, &cbPath);

			if (cbPath != 0 && cbPath != -1) 
				strReturn.Format("%s", szPath);
		}
		else
		{
			break;
		}
	}

	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return strReturn;
}

int VersionManOdbc::RemoveFolder(
	int nDirNum )
{
	CString strTemp;
	strTemp.Format("DELETE FROM ClientFile WHERE cd_idx=%d", nDirNum);
	if (Delete(strTemp) == DB_OK)
	{
		strTemp.Format("DELETE FROM ClientDir WHERE cd_idx=%d", nDirNum);
		return Delete(strTemp);
	}
	else
	{
		return DB_ERROR;
	}
}

int VersionManOdbc::CreateFolder(
	const CString& strDir )
{
	CString strTemp;
	strTemp = "INSERT INTO ClientDir (Path) VALUES ('" + strDir + "')";
	// 해당 디렉토리 만들기
	return Insert( strTemp );
}

int VersionManOdbc::Insert(
	const CString& strSQL )
{
	return ExecuteSQL(strSQL);
}

int	VersionManOdbc::Update(
	const CString& strSQL )
{
	return ExecuteSQL(strSQL);
}

int VersionManOdbc::Delete(
	const CString& strSQL )
{
	return ExecuteSQL(strSQL);
}

int	VersionManOdbc::ExecuteSQL(
	const CString& strSQL )
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}
	
	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strSQL.GetString(), 
							SQL_NTS);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);	
		return DB_ERROR;
	}
}

void VersionManOdbc::msgBox(
	const CString& strMsg )
{
	::MessageBox( NULL, strMsg, "INFO", MB_OK );
}

int VersionManOdbc::InsertFile(
	int nDirNum,
	const CString& strFileName,
	const CString& strMD5 )
{
#ifdef CHINAPARAM
	std::strstream strTemp;
	strTemp << "INSERT INTO ClientFile (cd_idx, cf_name, cf_md5, cf_version) VALUES (" << nDirNum << ",'" << strFileName << "','" << strMD5 << "',1)";
	strTemp << '\0';
#else
	std::strstream strTemp;
	strTemp << "INSERT INTO ClientFile (cd_idx, cf_name, cf_version) VALUES (" << nDirNum << ",'" << strFileName << "',1)";
	strTemp << '\0';
#endif
	TRACE0( strTemp );
	int nReturn = Insert( CString(strTemp.str()) );
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}		

// 파일이 변경되었을때 변경된 파일의
// 버전정보와 md5 값을 갱신한다.
int	VersionManOdbc::UpdateFile(
	const CString& strFileName,
	const CString& strMD5,
	int nVerIncreaseCount,
	int nDir)
{
	CString strTemp;
#ifdef CHINAPARAM
	strTemp = "UPDATE ClientFile SET cf_version = cf_version+1, cf_date=getdate(), cf_md5='" + strMD5 + "' WHERE cf_name='" + strFileName + "'";
#else
	strTemp = "UPDATE ClientFile SET cf_version = cf_version+1, cf_date=getdate() WHERE cf_name='" + strFileName + "'";
#endif
	TRACE0( strTemp );
	return Update( strTemp );
}

BOOL VersionManOdbc::IsFileExist( const CString& strFileName, int nDir )
{	
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	CString strTemp;
	strTemp = "SELECT cf_name FROM ClientFile WHERE cf_name='" + strFileName + "'";

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.GetString(), 
							SQL_NTS);

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return FALSE;
	}
	
	SQLCHAR szFile[256];	
	SQLINTEGER cbFile = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_CHAR, szFile,  256, &cbFile);
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return TRUE;
		}
		else
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return FALSE;			
		}
	}
}

BOOL VersionManOdbc::IsFolderExist(
	const CString& strFolderName )
{
    SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	CString strTemp;
	strTemp = "SELECT Path FROM ClientDir WHERE Path='" + strFolderName + "'";
    strTemp.Replace('/', '\\');

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.GetString(), 
							SQL_NTS);

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return FALSE;
	}
	
	SQLCHAR szFile[256];	
	SQLINTEGER cbFile = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_CHAR, szFile,  256, &cbFile);
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return TRUE;
		}
		else
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return FALSE;			
		}
	}
}

int VersionManOdbc::GetFolderNum(
	const CString& strName )
{
    SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	CString strTemp;
	strTemp = "SELECT cd_idx FROM ClientDir WHERE Path='" + strName + "'";
    strTemp.Replace('/', '\\');

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.GetString(), 
							SQL_NTS);

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

    SQLINTEGER nIdx;	
	SQLINTEGER cbID   = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nIdx,  0, &cbID); 
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return nIdx;
		}
		else
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return DB_ERROR;			
		}
	}
}

int VersionManOdbc::GetFileFolderNum(
	const CString& strFileName )
{
    SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcVer, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

	CString strTemp;
	strTemp = "SELECT cd_idx FROM ClientFile WHERE cf_name='" + strFileName + "'";   

	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strTemp.GetString(), 
							SQL_NTS);

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}

    SQLINTEGER nIdx;	
	SQLINTEGER cbID   = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			printf("SQLFetch Failed \n");
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nIdx,  0, &cbID); 
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return nIdx;
		}
		else
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return DB_ERROR;			
		}
	}

}

int VersionManOdbc::DeleteFile(
	int nFileNum )
{
    std::strstream strTemp;
	strTemp << "DELETE ClientFile WHERE cf_idx=" << nFileNum;
	strTemp << '\0';

	int nReturn = Delete( CString( strTemp.str() ) );
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

int VersionManOdbc::UpdateFileStateTrue(
	int nIdx )
{
    std::strstream strTemp;
	strTemp << "UPDATE ClientFile SET cf_upload=1 WHERE cf_idx=" << nIdx;
	strTemp << '\0';
	
	int nReturn = Update(CString(strTemp.str()));
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

int VersionManOdbc::UpdateFileStateFalse(
	const CString& strFileName )
{
    std::string strTemp;
	strTemp = "UPDATE ClientFile SET cf_upload=0 WHERE cf_name='";
	strTemp += strFileName.GetString();
	strTemp += "'";
	
	return Update(CString(strTemp.c_str()));
}