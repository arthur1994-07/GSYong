#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../Pet/GLPetField.h"
#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::CreatePet(
	int nChaNum,
	const TCHAR* szPetName,
	int nPetType,
	int nPetMID,
	int nPetSID,
	int nPetStyle,
	int nPetColor,
	int nPetCardMID,
	int nPetCardSID)
{
    sc::writeLogError("COdbcManager::CreatePet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nChaNum <=0 || !szPetName)
	{
		Print("COdbcManager::CreatePet");
		return sc::db::DB_ERROR;
	}
	
    std::string Query(
        sc::string::format(
            "{call sp_InsertPet('%s',%d,%d,%d,%d,%d,%d,%d,%d,?)}",
		    szPetName,
		    nChaNum,
		    nPetType,
		    nPetMID,
		    nPetSID,
		    nPetCardMID,
		    nPetCardSID,
		    nPetStyle,
		    nPetColor));	
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName)
{
    sc::writeLogError("COdbcManager::RenamePet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (!szPetName)
	{
		Print("COdbcManager::RenamePet szPetName==NULL");
		return sc::db::DB_ERROR;
	}

	TCHAR szPetNameTemp[PETNAMESIZE+1] = {0};
	::StringCchCopy(szPetNameTemp, PETNAMESIZE+1, szPetName);	

    std::string Query(
        sc::string::format(
            "{call sp_RenamePet(%d,%d,'%s',?)}", nChaNum, nPetNum, szPetNameTemp));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::ExchangePet(int nChaNum, int nPetNum)
{
    sc::writeLogError("COdbcManager::ExchangePet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nChaNum <= 0 || nPetNum <=0)
	{
		Print("COdbcManager::ExchangePet");
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetChaNum(%d,%d,?)}", nChaNum, nPetNum));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetPetColor(int nChaNum, int nPetNum, int nPetColor)
{
    sc::writeLogError("COdbcManager::SetPetColor. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0)
	{
		Print("COdbcManager::SetPetColor");
		return sc::db::DB_ERROR;
	}
	
    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetColor(%d,%d,%d,?)}",
            nChaNum,
            nPetNum,
            nPetColor));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetPetStyle(int nChaNum, int nPetNum, int nPetStyle)
{
    sc::writeLogError("COdbcManager::SetPetStyle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0)
	{
		Print("COdbcManager::SetPetStyle");
		return sc::db::DB_ERROR;
	}
	
    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetStyle(%d,%d,%d,?)}",
            nChaNum,
            nPetNum,
            nPetStyle));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetPetFull(int nChaNum, int nPetNum, int nPetFull)
{
    sc::writeLogError("COdbcManager::SetPetFull. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0 || nPetFull < 0)
	{
		Print("COdbcManager::SetPetFull");
		return sc::db::DB_ERROR;
	}
	
    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetFull(%d,%d,%d,?)}", nChaNum, nPetNum, nPetFull));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DeletePet(int nChaNum, int nPetNum)
{
    sc::writeLogError("COdbcManager::DeletePet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0)
	{
		Print("COdbcManager::DeletePet");
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_DeletePet(%d,%d,?)}", nChaNum, nPetNum));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetPetInven(
	int nChaNum,
	int nPetNum,
	se::ByteStream &ByteStream )
{
    sc::writeLogError("COdbcManager::SetPetInven. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0)
	{
		Print(_T("COdbcManager::SetPetInven"));
		return sc::db::DB_ERROR;
	}

	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	int nResult = 0;
	ByteStream.GetBuffer( pBuffer, dwSize );
	if (pBuffer != NULL)
	{
		std::strstream strPetInven;
		strPetInven << _T("UPDATE PetInfo SET PetInfo.PetPutOnItems=? WHERE (PetNum=" <<  nPetNum << "And PetChaNum=" << nChaNum << ")");
		strPetInven << std::ends;
		nResult = m_pGameDB->WriteImage( strPetInven, (DWORD) nPetNum, (BYTE *) pBuffer, dwSize );

		strPetInven.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		pBuffer = NULL;
		return nResult;
	}
	else
	{
		return sc::db::DB_ERROR;
	}
*/
}

int COdbcManager::SetPetSkill(
    int nChaNum,
	int nPetNum,
	int nPetInvenType,
	int nPetInvenMID,
	int nPetInvenSID,
	int nPetInvenCMID,
	int nPetInvenCSID,
	int nPetInvenAvailable)
{
    sc::writeLogError("COdbcManager::SetPetSkill. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nPetNum <= 0)
	{
		Print(_T("COdbcManager::SetPetSkill"));
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
	        "{call sp_UpdatePetInven(%d,%d,%d,%d,%d,%d,%d,%d,?)}", 
		    nChaNum,
		    nPetNum,
		    nPetInvenType,
		    nPetInvenMID,
		    nPetInvenSID,
		    nPetInvenCMID,
		    nPetInvenCSID,
		    nPetInvenAvailable));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum)
{
    sc::writeLogError("COdbcManager::SetPetSkillSlotNum. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (wSkillSlotNum == 0)
	{
		Print(_T("COdbcManager::SetPetSkillSlotNum"));
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetSkillSlotNum(%d,%d,%u,?)}", nChaNum, nPetNum, wSkillSlotNum));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID)
{
    sc::writeLogError("COdbcManager::GetPet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (pPet == NULL)
	{
		Print(_T("COdbcManager::GetPet pPet==NULL"));
		return sc::db::DB_ERROR;
	}
	
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	// Pet 의 기본정보를 가져온다.
    std::string Query(
        sc::string::format(
            "SELECT PetName, PetChaNum, PetType, PetMID, PetSID, PetCardMID, PetCardSID, PetStyle, PetColor, PetFull, PetSkinMID, PetSkinSID, PetSkinScale, PetSkinTime, PetSkinStartDate, PetSkillSlotNum FROM dbo.PetInfo WHERE PetNum=%u And PetChaNum=%u",
            dwPetNum,
            dwClientID));

	SQLRETURN sReturn = 0;
	int		  nRowCount = 0;
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
        return sc::db::DB_ERROR;
	}

	SQLCHAR szPetName[PETNAMESIZE+1] = {0}; SQLINTEGER cbPetName = SQL_NTS;
	SQLINTEGER nOwnerNum = 0, cbOwnerNum = SQL_NTS;
	SQLINTEGER nType = 0, cbType = SQL_NTS;
	SQLINTEGER nMainID = 0, cbMainID = SQL_NTS;
	SQLINTEGER nSubID = 0, cbSubID = SQL_NTS;
	SQLINTEGER nPetCardMID = 0, cbPetCardMID = SQL_NTS;
	SQLINTEGER nPetCardSID = 0, cbPetCardSID = SQL_NTS;
	SQLINTEGER nStyle = 0, cbStyle = SQL_NTS;
	SQLINTEGER nColor = 0, cbColor = SQL_NTS;
	SQLINTEGER nFull = 0, cbFull = SQL_NTS;
	// Pet 스킨팩 추가
	SQLINTEGER nPetSkinMID = 0, cbPetSkinMID = SQL_NTS;
	SQLINTEGER nPetSkinSID = 0, cbPetSkinSID = SQL_NTS;
	SQLINTEGER nPetSkinScale = 0, cbPetSkinScale = SQL_NTS;
	SQLINTEGER nPetSkinTime = 0, cbPetSkinTime = SQL_NTS;
	TIMESTAMP_STRUCT sPetSkinStartDate; SQLINTEGER cbPetSkinStartDate   = SQL_NTS;

	// 스킬 슬롯 갯수 추가
	SQLSMALLINT nPetSkillSlotNum = 0; SQLINTEGER cbPetSkillSlotNum = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szPetName, PETNAMESIZE+1, &cbPetName);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nOwnerNum, 0, &cbOwnerNum);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nType, 0, &cbType);	
			::SQLGetData(spConn->hStmt, 4, SQL_C_LONG, &nMainID, 0, &cbMainID);
			::SQLGetData(spConn->hStmt, 5, SQL_C_LONG, &nSubID, 0, &cbSubID);
			::SQLGetData(spConn->hStmt, 6, SQL_C_LONG, &nPetCardMID, 0, &cbPetCardMID);
			::SQLGetData(spConn->hStmt, 7, SQL_C_LONG, &nPetCardSID, 0, &cbPetCardSID);
			::SQLGetData(spConn->hStmt, 8, SQL_C_LONG, &nStyle, 0, &cbStyle);
			::SQLGetData(spConn->hStmt, 9, SQL_C_LONG, &nColor, 0, &cbColor);
			::SQLGetData(spConn->hStmt, 10, SQL_C_LONG, &nFull, 0, &cbFull);
			::SQLGetData(spConn->hStmt, 11, SQL_C_LONG, &nPetSkinMID, 0, &cbPetSkinMID);
			::SQLGetData(spConn->hStmt, 12, SQL_C_LONG, &nPetSkinSID, 0, &cbPetSkinSID);
			::SQLGetData(spConn->hStmt, 13, SQL_C_LONG, &nPetSkinScale, 0, &cbPetSkinScale);
			::SQLGetData(spConn->hStmt, 14, SQL_C_LONG, &nPetSkinTime, 0, &cbPetSkinTime);
			::SQLGetData(spConn->hStmt, 15, SQL_C_TYPE_TIMESTAMP, &sPetSkinStartDate, 0, &cbPetSkinStartDate);
			::SQLGetData(spConn->hStmt, 16, SQL_SMALLINT, &nPetSkillSlotNum, 0, &cbPetSkillSlotNum);

			if (cbPetName != 0 && cbPetName != -1)
				pPet->SetName((const char*) szPetName);
			pPet->m_dwOwner								= (DWORD) nOwnerNum;
			pPet->m_emTYPE								= int(nType);
			pPet->m_sPetID.wMainID						= (WORD) nMainID;
			pPet->m_sPetID.wSubID						= (WORD) nSubID;
			pPet->m_sPetCardID.wMainID					= (WORD) nPetCardMID;
			pPet->m_sPetCardID.wSubID					= (WORD) nPetCardSID;
			pPet->m_wStyle								= (WORD) nStyle;
			pPet->m_wColor								= (WORD) nColor;
			pPet->m_nFull								= nFull;
			pPet->m_sPetSkinPackData.dwPetSkinTime		= (DWORD) nPetSkinTime;
			pPet->m_sPetSkinPackData.sMobID.wMainID		= (WORD) nPetSkinMID;
			pPet->m_sPetSkinPackData.sMobID.wSubID		= (WORD) nPetSkinSID;
			pPet->m_sPetSkinPackData.fScale				= (float)nPetSkinScale / 100.0f;
			pPet->m_wMaxSkillSlot						= (WORD) nPetSkillSlotNum;
						
			// 프리미엄 만료기간
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sPetSkinStartDate.year <= 1970 || sPetSkinStartDate.year >= 2999)
			{
				sPetSkinStartDate.year = 1970;
				sPetSkinStartDate.month = 2;
				sPetSkinStartDate.day = 1;
				sPetSkinStartDate.hour = 1;
				sPetSkinStartDate.minute = 1;
				sPetSkinStartDate.second = 1;
			}
            CTime cTemp(sPetSkinStartDate.year, sPetSkinStartDate.month,  sPetSkinStartDate.day, 
                        sPetSkinStartDate.hour, sPetSkinStartDate.minute, sPetSkinStartDate.second);
            pPet->m_sPetSkinPackData.startTime   = cTemp.GetTime();

			if( pPet->m_sPetSkinPackData.dwPetSkinTime == 0 )
			{
				pPet->m_sPetSkinPackData.Init();
			}
            else
            {
				CTime currentTime = CTime::GetCurrentTime();
				CTimeSpan timeSpan = currentTime - pPet->m_sPetSkinPackData.startTime;
				if( timeSpan.GetTotalSeconds() >= pPet->m_sPetSkinPackData.dwPetSkinTime )
				{
					pPet->m_sPetSkinPackData.Init();
				}
                else
                {
					pPet->m_sPetSkinPackData.bUsePetSkinPack = TRUE;
				}
			}

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		sc::writeLogError(sc::string::format("DB:GetPet RowCount = 0, PetID = %d, CharID = %d",
			dwPetNum, dwClientID ));

		
		return sc::db::DB_ROWCNT_ERROR;
	}

	// Pet 의 인벤토리를 가져온다.
	if ( GetPetInven( dwClientID, pPet, dwPetNum ) == sc::db::DB_ERROR) return sc::db::DB_ERROR;

	// Pet 의 스킬을 가져온다.
	if ( GetPetSkill( dwClientID, pPet, dwPetNum ) == sc::db::DB_ERROR) return sc::db::DB_ERROR;		

	return sc::db::DB_OK;
*/
}

int COdbcManager::GetPetInven( 
    int nChaNum,
	GLPET* pPet,
	DWORD dwPetNum )
{
    sc::writeLogError("COdbcManager::GetPetInven. Do not call this function");
    return sc::db::DB_ERROR;
/*
	if (pPet == NULL)
	{
		Print(_T("COdbcManager::GetPetInven pPet==NULL"));
		return sc::db::DB_ERROR;
	}

	se::ByteStream ByteStream;
	int nReturn = 0;

	// Pet PutOnItem
	//std::strstream strPutOnItems;
	//strPutOnItems << "SELECT PetInfo.PetPutOnItems FROM PetInfo WHERE (PetNum=" <<  dwPetNum << ")";
	//strPutOnItems << std::ends;
	nReturn = m_pGameDB->ReadImagePet("PetInfo.PetPutOnItems", nChaNum, dwPetNum, ByteStream);
	//strPutOnItems.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nReturn == sc::db::DB_ERROR)
	{
		return sc::db::DB_ERROR;
	}
	else 
	{
		SETPUTONITEMS_BYBUF( pPet->m_PutOnItems, ByteStream );
		return sc::db::DB_OK;
	}
*/
}

int COdbcManager::GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum)
{
    sc::writeLogError("COdbcManager::GetPetSkill. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (!pPet) 
	{
		Print(_T("COdbcManager::GetPetSkill pPet==NULL"));
		return sc::db::DB_ERROR;
	}

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;	

    std::string Query(
        sc::string::format(
            "SELECT PetInvenMID, PetInvenSID, PetInvenAvailable FROM dbo.PetInven WHERE PetNum=%u AND PetInvenType=%d AND PetChaNum=%d ORDER BY PetInvenNum",
            dwPetNum,
            COdbcManager::SKILL,
            nChaNum));

    SQLRETURN sReturn = 0;
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
        return sc::db::DB_ERROR;
	}

	SQLINTEGER nAvailable = 0, cbAvailable = SQL_NTS;
	SQLINTEGER nMainID = 0, cbMainID = SQL_NTS;
	SQLINTEGER nSubID = 0, cbSubID = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_SHORT, &nMainID, 0, &cbMainID);
			::SQLGetData(spConn->hStmt, 2, SQL_C_SHORT, &nSubID, 0, &cbSubID);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG,  &nAvailable, 0, &cbAvailable);
            
			PETSKILL PetSkill;
			PetSkill.sNativeID.wMainID = (WORD) nMainID;
			PetSkill.sNativeID.wSubID  = (WORD) nSubID;			
			
			// pPet->m_ExpSkills.insert ( PETSKILL_PAIR ( PetSkill.sNativeID.dwID, PetSkill ) );
			// pPet->m_ExpSkills[PetSkill.sNativeID.dwID] = PetSkill;
			pPet->m_ExpSkills.insert( std::make_pair( PetSkill.sNativeID.dwID, PetSkill ) );

			// Note : 활성화된 스킬
			if ( nAvailable )
			{
				for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
				{
					if ( nAvailable & (1<<i) )
					{
						pPet->m_sActiveSkillID[i] = PetSkill.sNativeID;
					}
				}
			}
		}
		else
		{
			break;
		}		
	}
	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
*/
}

int COdbcManager::GetPetFull(int nChaNum, DWORD dwPetNum)
{
    sc::writeLogError("COdbcManager::GetPetFull. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_GetPetFull(%d,%u,?)}", nChaNum, dwPetNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::GetRestorePetList(
	int nChaNum,
	std::vector<PET_RELIST>& vecPetReList)
{
    sc::writeLogError("COdbcManager::GetRestorePetList. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	// 부활할 PET의 리스트를 가져온다.
	// PetNum, PetName, PetType의 정보를 DB Table에서 가져온다.	
    std::string Query(
        sc::string::format(
            "SELECT PetNum, PetName, PetType, PetCardMID, PetCardSID From dbo.PetInfo Where PetChaNum=%d And PetDeleted=1",
            nChaNum));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);	
		return sc::db::DB_ERROR;
	}

	SQLCHAR    szPetName[PETNAMESIZE+1] = {0}; SQLINTEGER cbPetName = SQL_NTS;
	SQLINTEGER nPetNum = 0, cbPetNum=SQL_NTS;
	SQLINTEGER nPetType = 0,	cbPetType=SQL_NTS;
	SQLINTEGER nPetCardMID = 0, cbPetCardMID=SQL_NTS;
	SQLINTEGER nPetCardSID = 0, cbPetCardSID=SQL_NTS;

	
	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			PET_RELIST sPetReList;

			// Bind data			
			::SQLGetData(spConn->hStmt,  1, SQL_C_LONG,	&nPetNum,                0, &cbPetNum);
			::SQLGetData(spConn->hStmt,  2, SQL_C_CHAR,  szPetName, PETNAMESIZE+1, &cbPetName);
			::SQLGetData(spConn->hStmt,  3, SQL_C_LONG,	&nPetType,				 0, &cbPetType);
			::SQLGetData(spConn->hStmt,  4, SQL_C_LONG,	&nPetCardMID,            0, &cbPetCardMID);
			::SQLGetData(spConn->hStmt,  5, SQL_C_LONG,	&nPetCardSID,            0, &cbPetCardSID);

			 if (cbPetName != 0 && cbPetName != -1)
				::StringCchCopy(sPetReList.szPetName, PETNAMESIZE+1, (const TCHAR*) szPetName);

			 sPetReList.nPetNum = nPetNum;
			 sPetReList.emType = (int) nPetType;
			 sPetReList.sPetCardID.wMainID = (WORD) nPetCardMID;
			 sPetReList.sPetCardID.wSubID = (WORD) nPetCardSID;

			 vecPetReList.push_back( sPetReList );
		}
		else
		{
			break;
		}		
	}	  
	//m_pGameDB->FreeConnection(spConn);

	return sc::db::DB_OK;
*/
}

int COdbcManager::RestorePet(DWORD dwPetNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::RestorePet. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (dwPetNum<=0)
	{
		Print(_T("COdbcManager::RestorePet"));
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_RestorePet(%u,%d,?)}", dwPetNum, nChaNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::UpdatePetSkin(
    int nChaNum,
    DWORD dwPetNum,
    int nPetSkinMID,
    int nPetSkinSID,
    int nPetSkinScale,
    int nPetSkinTime)
{
    sc::writeLogError("COdbcManager::UpdatePetSkin. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (dwPetNum <= 0 || nChaNum <= 0)
	{
		Print(_T("COdbcManager::UpdatePetSkin") );
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_UpdatePetSkin(%d,%u,%d,%d,%d,%d,?)}",
		    nChaNum,
		    dwPetNum,
		    nPetSkinMID,
		    nPetSkinSID,
		    nPetSkinScale,
		    nPetSkinTime));

	return m_pGameDB->ExecuteSpInt(Query);													
*/
}
