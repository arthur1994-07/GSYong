#include "../../pch.h"
#include "../../../EngineLib/Common/profile.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../Server/ServerCfg.h"
#include "../../Server/NetServer.h"
#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime)
{
    sc::writeLogError("COdbcManager::SetChaStorageDate. Use ADO");
    return sc::db::DB_ERROR;

    /*
	if( nStorageNum <= 0 || nStorageNum >= EMSTORAGE_CHANNEL - 1 )
		return sc::db::DB_ERROR;

    CTime cTemp(tTime);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szTemp[128] = {0};
	
    //std::strstream strTemp;
    //strTemp << "Update UserInven Set ";

    switch (nStorageNum)
    {
    case 1:
		_snprintf( szTemp, 128, "Update UserInven Set ChaStorage2='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage2='" << strTime.GetString() << "'";
        break;
    case 2:
		_snprintf( szTemp, 128, "Update UserInven Set ChaStorage3='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage3='" << strTime.GetString() << "'";
        break;
    case 3:
		_snprintf( szTemp, 128, "Update UserInven Set ChaStorage4='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage4='" << strTime.GetString() << "'";
        break;
    default:
        return sc::db::DB_ERROR;
        break;
    }

    //strTemp << " WHERE UserNum=" << nUserNum;
    //strTemp << std::ends;

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
    */
}

int COdbcManager::SetChaInvenNum(int nChaNum, WORD wInvenLine)
{
    sc::writeLogError("COdbcManager::SetChaInvenNum. Use ADO");
    return sc::db::DB_ERROR;
/*
	if( nChaNum <= 0 )
		return sc::db::DB_ERROR;

	if( wInvenLine <= 0 || wInvenLine > EInvenExtendLine )
		return sc::db::DB_ERROR;

    //std::strstream strTemp;
    //strTemp << "Update ChaInfo Set ChaInvenLine=";
    //strTemp << wInvenLine;
    //strTemp << " WHERE ChaNum=" << nChaNum;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "Update ChaInfo Set ChaInvenLine=%u  WHERE ChaNum=%d", wInvenLine, nChaNum );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
*/
}

///////////////////////////////////////////////////////////////////////////
// 유저 인벤토리를 읽는다
int COdbcManager::ReadUserInven(SCHARDATA2* pChaData2)
{    
	bool bInven = CheckInven(m_pServer->ServerGroup(), pChaData2->GetUserID());
	if (!bInven)
	{
		MakeUserInven(m_pServer->ServerGroup(), pChaData2->GetUserID());
	}

	// 유저인벤토리, money
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLRETURN sReturn = 0;
	int nUserNum = 0;
	
	//std::strstream strTemp;
	//strTemp << "SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 FROM UserInven WHERE UserNum=" << pChaData2->m_dwUserID; // << " AND ";
	//// strTemp << "SGNum=" << m_pConfigOld->GetServerGroup() ;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 FROM dbo.UserInven WHERE UserNum=%u", pChaData2->GetUserID() );
	
	sReturn = ::SQLExecDirect(spConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        Print(szTemp);
		Print(GetErrorString(spConn->hStmt));
        //m_pGameDB->FreeConnection(spConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return sc::db::DB_ERROR;
    }

	LONGLONG   llUserMoney = 0;
    SQLINTEGER cbUserMoney = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate2; SQLINTEGER cbStorageDate2 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate3; SQLINTEGER cbStorageDate3 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate4; SQLINTEGER cbStorageDate4 = SQL_NTS;

	while(true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);
		    Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return sc::db::DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
            ::SQLGetData(spConn->hStmt, 1, SQL_C_SBIGINT,        &llUserMoney,   0, &cbUserMoney);
            ::SQLGetData(spConn->hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sStorageDate2, 0, &cbStorageDate2);
            ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sStorageDate3, 0, &cbStorageDate3);
            ::SQLGetData(spConn->hStmt, 4, SQL_C_TYPE_TIMESTAMP, &sStorageDate4, 0, &cbStorageDate4);

			pChaData2->SetStorageMoney( llUserMoney );

            // 인벤토리 사용만료기간
			
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sStorageDate2.year <= 1970 || sStorageDate2.year >= 2999)
			{
				sStorageDate2.year = 1970;
				sStorageDate2.month = 2;
				sStorageDate2.day = 1;
				sStorageDate2.hour = 1;
				sStorageDate2.minute = 1;
				sStorageDate2.second = 1;
			}
            CTime cTemp2(sStorageDate2.year, sStorageDate2.month,  sStorageDate2.day, 
                         sStorageDate2.hour, sStorageDate2.minute, sStorageDate2.second);
            pChaData2->m_tSTORAGE[0] = cTemp2.GetTime();

            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sStorageDate3.year <= 1970 || sStorageDate3.year >= 2999)
			{
				sStorageDate3.year = 1970;
				sStorageDate3.month = 2;
				sStorageDate3.day = 1;
				sStorageDate3.hour = 1;
				sStorageDate3.minute = 1;
				sStorageDate3.second = 1;
			}
            CTime cTemp3(sStorageDate3.year, sStorageDate3.month,  sStorageDate3.day, 
                         sStorageDate3.hour, sStorageDate3.minute, sStorageDate3.second);
            pChaData2->m_tSTORAGE[1] = cTemp3.GetTime();

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sStorageDate4.year <= 1970 || sStorageDate4.year >= 2999)
			{
				sStorageDate4.year = 1970;
				sStorageDate4.month = 2;
				sStorageDate4.day = 1;
				sStorageDate4.hour = 1;
				sStorageDate4.minute = 1;
				sStorageDate4.second = 1;
			}
            CTime cTemp4(sStorageDate4.year, sStorageDate4.month,  sStorageDate4.day, 
                         sStorageDate4.hour, sStorageDate4.minute, sStorageDate4.second);
            pChaData2->m_tSTORAGE[2] = cTemp4.GetTime();
		}
		else
		{
			break;		
		}		
    }
	//m_pGameDB->FreeConnection(spConn);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	// image
	se::ByteStream ByteStream;
	sReturn = ReadUserInven(m_pServer->ServerGroup(), pChaData2->GetUserID(), ByteStream);
	if (sReturn == sc::db::DB_ERROR)
		return sc::db::DB_ERROR;

	pChaData2->SETSTORAGE_BYBUF(ByteStream);
	return sc::db::DB_OK;
}

///////////////////////////////////////////////////////////////////////////
// 사용자 인벤토리에서 데이타를 읽어온다.	
int	COdbcManager::ReadUserInven(int SGNum, DWORD dwUserNum, se::ByteStream &ByteStream)
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	ByteStream.ClearBuffer();
			
	//std::strstream strTemp;
	//strTemp << "SELECT UserInven.UserInven FROM UserInven where (UserNum=" <<  dwUserNum << ")";
	//strTemp << std::ends;


	//
	//mjeon.ToDo
	//SQL parameter function,SQLBindParameter, is faster than 
	//filling a buffer with SQL-stmt and variables using sprintf().
	//In case of SQLBindParameter, it does not cast any variables to string and vice versa.
	//That's why SQL parameter is faster.
	//
	//Let's replace all sprintf() with SQL Parameter.
	//

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT UserInven.UserInven FROM dbo.UserInven where (UserNum=%u)", dwUserNum );

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
        Print(szTemp);
		Print(GetErrorString(spConn->hStmt));		
		//m_pGameDB->FreeConnection(spConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}	
	
	// Retrieve and display each row of data.
	BYTE pBuffer[sc::db::DB_IMAGE_BUF_SIZE] = {0};
	SQLINTEGER  lSize=0, lTotalSize=0;

	while ((sReturn = ::SQLFetch(spConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::ZeroMemory(pBuffer, sc::db::DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(spConn->hStmt, 1, SQL_C_BINARY, pBuffer, sc::db::DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBuffer, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;			
		}
		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer();		
	}
	//m_pGameDB->FreeConnection(spConn);
	return lTotalSize;
}

///////////////////////////////////////////////////////////////////////////
// 유저인벤토리가 있는지 체크한다.	
bool COdbcManager::CheckInven(int SGNum, DWORD dwUserNum)
{
	//std::strstream strTemp;
	//strTemp << "{call GetInvenCount(";	
	//strTemp << dwUserNum;
	//strTemp << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call GetInvenCount(%u,?)}", dwUserNum );

	if (m_pGameDB->ExecuteSpInt(szTemp) > 0)
	{
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return true;
	}
	else
	{
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return false;
	}
}

int COdbcManager::MakeUserInven(int SGNum, DWORD dwUserNum)
{	
	//std::strstream strTemp;
	//strTemp << "{call MakeUserInven(";
	//strTemp << SGNum << ",";
	//strTemp << dwUserNum << ",";
	//strTemp << "?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call MakeUserInven(%d,%u,?)}", SGNum, dwUserNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 사용자 인벤토리에 데이타를 저장한다.
int	COdbcManager::WriteUserInven(
	int SGNum, 
	LONGLONG llMoney /* LONGLONG __int64 */, 
	DWORD dwUserNum, 
	BYTE* pData, 
	int nSize)
{	
    sc::writeLogError("COdbcManager::WriteUserInven. Use ADO");
    return sc::db::DB_ERROR;
/*
	// Update Money
	//std::strstream strMoney;
	//strMoney << "{call UpdateUserMoney(";
	//strMoney << dwUserNum;
	//strMoney << ",";
	//strMoney << llMoney;
	//strMoney << ")}";
	//strMoney << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateUserMoney(%u,%I64d)}", dwUserNum, llMoney );

	if( llMoney < 0 )
		CDebugSet::ToHackingFile( "$$$$$ Unusual money DB tried save.... $$$$$, %d, %I64d", dwUserNum, llMoney );


	m_pGameDB->ExecuteSp( szTemp );
	//strMoney.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	// Update Image
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "UPDATE UserInven SET UserInven = ? WHERE (UserNum=" <<  dwUserNum << ") ";
	//strTemp << std::ends;

	_snprintf( szTemp, 128, "UPDATE UserInven SET UserInven = ? WHERE (UserNum=%u)", dwUserNum );

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(spConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
        Print(szTemp);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}	
	
	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(spConn->hStmt, 
								1, 
								SQL_PARAM_INPUT,
								SQL_C_BINARY, 
								SQL_LONGVARBINARY,
								nSize, 
								0, 
								(SQLPOINTER) 2, 
								0, 
								&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
        Print(szTemp);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(spConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
        Print(szTemp);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return sc::db::DB_ERROR;
	}
	
	SQLPOINTER pToken = NULL;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(spConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(spConn->hStmt, pData, (SQLINTEGER) nSize);
		}		
	}
	//m_pGameDB->FreeConnection(spConn);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return sc::db::DB_OK;
*/
}

int COdbcManager::UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney )
{
	if ( lnUserMoney <= 0 )	return sc::db::DB_ERROR;

	TCHAR szTemp[512] = {0};

	switch ( m_ServiceProvider )
	{
	case SP_TAIWAN:
		_snprintf( szTemp, 512, "{call sp_UpdateUserMoneyAdd_3B2C75D1D26646AB9B3E561A3F6CD095(%u,%I64d,?)}", dwUserID, lnUserMoney );
		break;
	default:
		_snprintf( szTemp, 512, "{call sp_UpdateUserMoneyAdd(%u,%I64d,?)}", dwUserID, lnUserMoney );
		break;
	}

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}

int COdbcManager::UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney )
{
	if ( lnUserMoney <= 0 )	return sc::db::DB_ERROR;

	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT UserMoney FROM dbo.UserLastInfo WHERE UserNum=%u", dwUserID );

	sReturn = ::SQLExecDirect(spConn->hStmt,
		(SQLCHAR*)szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// UserLastInfo Table의 금액 컬럼의 암호화를 위해서 ChaMoney 컬럼의 DataType을 int -> varchar(100)으로 수정한다.
	//	LONGLONG   llUserSaveMoney = 0;
	//	SQLINTEGER cbUserSaveMoney = SQL_NTS;	
	SQLCHAR    szTempStorageMoney[CHAR_TEMPMONEY] = {0}; 
	SQLINTEGER cbTempStorageMoney = SQL_NTS;

	LONGLONG lnTempMoney = 0;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szTempStorageMoney, CHAR_TEMPMONEY, &cbTempStorageMoney);

			if (cbTempStorageMoney != 0 && cbTempStorageMoney != -1) // TempStorageMoney 값 복사
			{
				lnTempMoney = 0;
				
				char szTempStorageMoney1[CHAR_TEMPMONEY], szTemp[2];				

				::StringCchCopy(szTempStorageMoney1, CHAR_TEMPMONEY, (const char*) szTempStorageMoney);

				size_t i;
				for( i = 0; i < strlen(szTempStorageMoney1); i++ )
				{
					szTempStorageMoney1[i] -= 20;
					sprintf( szTemp, "%c", szTempStorageMoney1[i] );
					lnTempMoney = ( lnTempMoney * 10 ) + atoi(szTemp);
				}

				lnTempMoney = lnTempMoney >> 0x04;
			}
		}
		else
		{
			break;
		}		
	}

	char szTempMoney[CHAR_TEMPMONEY];

	lnTempMoney += lnUserMoney;

	lnTempMoney <<= 0x04;

	sprintf( szTempMoney, "%I64d", lnTempMoney );
	for( size_t i = 0; i < strlen(szTempMoney); i++ )
	{
		szTempMoney[i] += 20;
	}

	TCHAR szTemp2[512] = {0};
	_snprintf( szTemp2, 512, "Exec sp_InsertUserLastInfo %u, '%s'",
		dwUserID,szTempMoney);

	if (m_pGameDB->ExecuteSQL(szTemp2) != sc::db::DB_OK)
	{
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	//m_pGameDB->FreeConnection(spConn);	
	return sc::db::DB_OK;
}
