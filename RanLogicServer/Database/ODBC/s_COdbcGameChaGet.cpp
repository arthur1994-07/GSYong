#include "../../pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::GetCharacterInfo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2)
{
    sc::writeLogError("COdbcManager::GetCharacterInfo. Use ADO");
    return sc::db::DB_ERROR;
}

int	COdbcManager::GetChaBInfo(int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci)
{
	if (nUserNum <= 0 || nChaNum <= 0)
	{
		return sc::db::DB_ERROR;
	}
    
	SQLRETURN sReturn = 0;
	int nRowCount = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	// 2003-11-27 Jgkim:Add
	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	//std::strstream strTemp;
	//strTemp << "SELECT ChaName, ChaClass, ChaSchool, ChaDex, ChaIntel, ChaPower,";
	//strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaLevel, ChaHair,";	
	//strTemp << "ChaFace, ChaBright, ChaSex, ChaHairColor, ChaExp,";
	//strTemp << "ChaSaveMap, ChaHP FROM ChaInfo ";
	//strTemp << "WHERE ChaNum=" << nChaNum;
	//strTemp << " AND UserNum=" << nUserNum;
	//strTemp << std::ends;

	TCHAR szTemp[512] = {0};
	_snprintf( szTemp, 512, "SELECT ChaName, ChaClass, ChaSchool, ChaDex, ChaIntel, ChaPower,"
							"ChaStrong, ChaSpirit, ChaStrength, ChaLevel, ChaHair,"
							"ChaFace, ChaBright, ChaSex, ChaHairColor, ChaExp,"
							"ChaSaveMap, ChaHP FROM dbo.ChaInfo "
							"WHERE ChaNum=%d AND UserNum=%d",
							nChaNum, nUserNum );
	
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

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nChaSchool = 0, cbChaSchool=SQL_NTS;
	SQLINTEGER nChaDex = 0, cbChaDex=SQL_NTS;
	SQLINTEGER nChaIntel = 0, cbChaIntel=SQL_NTS;
	SQLINTEGER nChaPower = 0, cbChaPower=SQL_NTS; 
	
	SQLINTEGER nChaStrong = 0, cbChaStrong=SQL_NTS;   
	SQLINTEGER nChaSpirit = 0, cbChaSpirit=SQL_NTS;
	SQLINTEGER nChaStrength = 0, cbChaStrength=SQL_NTS; 
	SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;
	SQLINTEGER nChaHair = 0, cbChaHair = SQL_NTS;
	
	SQLINTEGER nChaFace = 0, cbChaFace = SQL_NTS;	
	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;
	
	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	SQLSMALLINT nChaSex = 0; SQLINTEGER cbChaSex = SQL_NTS;
	SQLINTEGER nChaHairColor =0, cbChaHairColor = SQL_NTS;

	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;
	SQLINTEGER nChaHP = 0, cbChaHP=SQL_NTS;

	LONGLONG   llChaExp = 0; SQLINTEGER cbChaExp=SQL_NTS;

	while (true)
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
			// Bind data
			sci->m_dwCharID = nChaNum;
			::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR, szChaName, CHR_ID_LENGTH+1, &cbChaName);
			::SQLGetData(spConn->hStmt,  2, SQL_C_LONG, &nChaClass, 0, &cbChaClass);
			::SQLGetData(spConn->hStmt,  3, SQL_C_LONG, &nChaSchool, 0, &cbChaSchool);
			::SQLGetData(spConn->hStmt,  4, SQL_C_LONG, &nChaDex,   0, &cbChaDex);
			::SQLGetData(spConn->hStmt,  5, SQL_C_LONG, &nChaIntel, 0, &cbChaIntel);
			::SQLGetData(spConn->hStmt,  6, SQL_C_LONG, &nChaPower, 0, &cbChaPower);

			::SQLGetData(spConn->hStmt,  7, SQL_C_LONG, &nChaStrong, 0, &cbChaStrong);
			::SQLGetData(spConn->hStmt,  8, SQL_C_LONG, &nChaSpirit, 0, &cbChaSpirit);
			::SQLGetData(spConn->hStmt,  9, SQL_C_LONG, &nChaStrength, 0, &cbChaStrength);
			::SQLGetData(spConn->hStmt, 10, SQL_C_LONG, &nChaLevel,    0, &cbChaLevel);
			::SQLGetData(spConn->hStmt, 11, SQL_C_LONG, &nChaHair,	  0, &cbChaHair);

			::SQLGetData(spConn->hStmt, 12, SQL_C_LONG,	&nChaFace,     0, &cbChaFace);
			::SQLGetData(spConn->hStmt, 13, SQL_C_LONG,  &nChaBright,   0, &cbChaBright);

			// 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			::SQLGetData(spConn->hStmt, 14, SQL_SMALLINT, &nChaSex,       0, &cbChaSex);
			::SQLGetData(spConn->hStmt, 15, SQL_C_LONG,	 &nChaHairColor, 0, &cbChaHairColor);			

			::SQLGetData(spConn->hStmt, 16, SQL_C_SBIGINT,&llChaExp,    0, &cbChaExp);

			::SQLGetData(spConn->hStmt, 17, SQL_C_LONG, &nChaSaveMap,    0, &cbChaSaveMap);
			::SQLGetData(spConn->hStmt, 18, SQL_C_LONG, &nChaHP,			0, &cbChaHP);

			 if (cbChaName != 0 && cbChaName != -1) 				 
				sci->m_ChaName = (const char*) szChaName;

			 sci->m_emClass = EMCHARCLASS(nChaClass);
			 sci->m_wSchool = (WORD) nChaSchool;
			 sci->m_sStats.wDex = (WORD) nChaDex;
			 sci->m_sStats.wInt = (WORD) nChaIntel;
			 sci->m_sStats.wPow = (WORD) nChaPower;

			 sci->m_sStats.wStr = (WORD) nChaStrong;
			 sci->m_sStats.wSpi = (WORD) nChaSpirit;
			 sci->m_sStats.wSta = (WORD) nChaStrength;
			 sci->m_wLevel      = (WORD) nChaLevel;
			 sci->m_wHair		= (WORD) nChaHair;

			 sci->m_wFace       = (WORD) nChaFace;
			 sci->m_nBright     = nChaBright;

			 // 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			 sci->m_wSex        = (WORD) nChaSex;
			 sci->m_wHairColor  = (WORD) nChaHairColor;

			 sci->m_sHP.wNow	= (WORD) nChaHP;			 
			 sci->m_sSaveMapID  = (DWORD) nChaSaveMap;

			 sci->m_sExperience.lnNow  = llChaExp;

			 nRowCount++;
		}
		else
		{
			break;
		}
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	//m_pGameDB->FreeConnection(spConn);

	// 반드시 nRowCount 가 1 이어야 한다.
	if (nRowCount != 1)
	{
		return sc::db::DB_ERROR;
	}

	// 착용아이템 정보
	se::ByteStream ByteStream;	
	//std::strstream strPutOnItems;
	//strPutOnItems << "SELECT ChaInfo.ChaPutOnItems FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	//strPutOnItems << std::ends;
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
	//strPutOnItems.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn == sc::db::DB_ERROR)
		return sc::db::DB_ERROR;
	else
		SETPUTONITEMS_BYBUF(sci->m_PutOnItems, ByteStream);

	return sc::db::DB_OK;
}

int COdbcManager::GetChaBInfoAll(int UserDbNum, int ServerGroup, std::vector<SCHARINFO_LOBBY>& vCharInfo)
{
    MIN_ASSERT(0 && "Do not call this function. this function only can call from Cache Server.");
    return sc::db::DB_ERROR;
}

int COdbcManager::GetChaBInfoAllForMigration( int UserDbNum, int ServerGroup, std::vector< SCHARINFO_LOBBY >& vCharInfo )
{
	MIN_ASSERT(0 && "Do not call this function. this function only can call from Cache Server.");
	return sc::db::DB_ERROR;
}

int COdbcManager::GetChaPhoneNumber(
	int nChaNum,
	TCHAR* szPhoneNumber )
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	// 캐릭터 정보를 가져온다.
	//std::strstream strTemp;
	//strTemp << "SELECT ChaPhone ";
	//strTemp << " FROM ChaSmsInfo WHERE ChaNum=" << nChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT ChaPhone FROM dbo.ChaSmsInfo WHERE ChaNum=%d", nChaNum );

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

	SQLCHAR szChaPhNum[SMS_RECEIVER] = {0}; SQLINTEGER cbChaPhNum = SQL_NTS;

	while (true)
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
			// Bind data			
			::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR, szChaPhNum, SMS_RECEIVER, &cbChaPhNum);			

			if (cbChaPhNum != 0 && cbChaPhNum != -1) 				 
				::StringCchCopy( szPhoneNumber, SMS_RECEIVER, (const char*) szChaPhNum );			
		}
		else
		{
			break;
		}
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
}


// 베트남 탐직방지 시스템의 추가 정보 불러오는 부분 추가
int COdbcManager::GetVTCharInfo(int nChaNum, SCHARDATA2* pChaData2)
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT saveMoney, saveExp, itemCount FROM dbo.ChaInfo WHERE ChaNum=%d", nChaNum );

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

	LONGLONG   llChaSaveExp = 0, llChaSaveMoney = 0;	
	SQLINTEGER cbChaSaveExp = SQL_NTS, cbChaSaveMoney = SQL_NTS; 
	SQLINTEGER nChaitemCount = 0, cbChaitemCount = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data			
			::SQLGetData(spConn->hStmt,1, SQL_C_SBIGINT, &llChaSaveMoney,  0, &cbChaSaveMoney);
			::SQLGetData(spConn->hStmt,2, SQL_C_SBIGINT, &llChaSaveExp,    0, &cbChaSaveExp);
			::SQLGetData(spConn->hStmt,3, SQL_C_LONG, &nChaitemCount,    0, &cbChaitemCount);

			pChaData2->m_lVNGainSysMoney        = llChaSaveMoney;
			pChaData2->m_lVNGainSysExp			= llChaSaveExp;
			pChaData2->m_dwVietnamInvenCount	= nChaitemCount;
		}
		else
		{
			break;
		}
	}

	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
}

// 대만 해킹 문제 관련해서 최근 로그아웃 했을때의 정보를 DB에서 읽어옴.
int COdbcManager::GetLastCharInfo(int nUserNum, int nChaNum, SCHARDATA2* pChaData2)
{
	SQLRETURN sReturn = 0;
	{		
		std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
		if (!spConn)
            return sc::db::DB_ERROR;
	
		TCHAR szTemp[128] = {0};
		_snprintf( szTemp, 128, "SELECT ChaMoney,ChaLevel FROM dbo.ChaLastInfo WHERE ChaNum=%d", nChaNum );
	
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
	
		// ChaLastInfo Table의 금액 컬럼의 암호화를 위해서 ChaMoney 컬럼의 DataType을 int -> varchar(100)으로 수정한다.
		//	LONGLONG   llChaSaveMoney = 0;	
		//	SQLINTEGER cbChaSaveMoney = SQL_NTS; 
	
		// 캐릭터 로그아웃 할때의 ChaMoney
		SQLCHAR    szTempMoney[CHAR_TEMPMONEY] = {0}; SQLINTEGER cbTempMoney = SQL_NTS;
		SQLINTEGER nChaLevel = 0, cbChaLevel = SQL_NTS;
	
		while (true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);
	
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				// Bind data			
				//			::SQLGetData(pConn->hStmt,1, SQL_C_SBIGINT, &llChaSaveMoney,  0, &cbChaSaveMoney);
				::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szTempMoney, CHAR_TEMPMONEY, &cbTempMoney);
				::SQLGetData(spConn->hStmt,2, SQL_C_LONG, &nChaLevel,    0, &cbChaLevel);
	
				//			pChaData2->m_lnTempMoney    = llChaSaveMoney;			
				if (cbTempMoney != 0 && cbTempMoney != -1) // TempMoney 값 복사
				{
					pChaData2->SetTempInvenMoney( DECRYPTMONEY((const char*) szTempMoney) );
				}
				pChaData2->m_wTempLevel		= (WORD)nChaLevel;
			}
			else
			{
				break;
			}
		}
	
		//m_pGameDB->FreeConnection(spConn);
	}

	{	
		sReturn = 0;
		std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
		if (!spConn)
            return sc::db::DB_ERROR;

		TCHAR szTemp[128] = {0};
		_snprintf( szTemp, 128, "SELECT UserMoney FROM dbo.UserLastInfo WHERE UserNum=%d", nUserNum );

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

		while (true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);

			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				// Bind data
				//			::SQLGetData(pConn->hStmt,1, SQL_C_SBIGINT, &llUserSaveMoney,  0, &cbUserSaveMoney);
				::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szTempStorageMoney, CHAR_TEMPMONEY, &cbTempStorageMoney);

				//			pChaData2->m_lnTempStorageMoney  = llUserSaveMoney;
				if (cbTempStorageMoney != 0 && cbTempStorageMoney != -1) // TempStorageMoney 값 복사
				{
					pChaData2->SetTempStorageMoney( DECRYPTMONEY((const char*) szTempStorageMoney) );
				}			
			}
			else
			{
				break;
			}
		}

		//m_pGameDB->FreeConnection(spConn);
	}

	return sc::db::DB_OK;
}

//! 캐릭터의 이름과 레벨을 가져온다.
int COdbcManager::GetChaNameLevel(DWORD ChaDbNum, std::string& ChaName, int& ChaLevel)
{
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
    if (!spConn)
        return sc::db::DB_ERROR;
    
    std::string Query(
        sc::string::format(
            "EXEC sp_GetChaNameLevel %1%", ChaDbNum));
    SQLRETURN sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
    if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
    {
        Print(Query);
        Print(GetErrorString(spConn->hStmt));
        return sc::db::DB_ERROR;
    }

    SQLCHAR szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;
    
    while (true)
    {
        sReturn = ::SQLFetch(spConn->hStmt);
        if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
            Print(GetErrorString(spConn->hStmt));
            return sc::db::DB_ERROR;
        }

        if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            // Bind data
            ::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szChaName, CHR_ID_LENGTH+1, &cbChaName);            
            ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nChaLevel, 0, &cbChaLevel);

            if (cbChaName != 0 && cbChaName != -1)
                ChaName = (const char*) szChaName;
            ChaLevel = nChaLevel;
        }
		else
		{
			break;
		}
    }
    return sc::db::DB_OK;
}
