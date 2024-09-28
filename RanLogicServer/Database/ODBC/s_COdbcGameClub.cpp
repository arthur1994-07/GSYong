#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../RanLogic/Club/GLClub.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"
#include "../DBAction/DbAction.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country)
{
    sc::writeLogError("COdbcManager::CreateClub. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_create_guild(%1%,'%2%', %3%, ?)}",
            ChaDbNum,
            ClubName.c_str(),
            static_cast<int> (Country)));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DeleteClub(DWORD dwClub, DWORD dwChaNum)
{
    sc::writeLogError("COdbcManager::DeleteClub. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_delete_guild(%u,%u,?)}", dwClub, dwChaNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetClubRank(DWORD dwClub, DWORD dwRank)
{
    sc::writeLogError("COdbcManager::SetClubRank. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_update_guild_rank(%u,%u,?)}", dwClub, dwRank));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss)
{    
    CTime cTemp(tDiss);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");
    
    std::string Query(
        sc::string::format(
            "UPDATE dbo.GuildInfo SET GuExpireTime='%s' WHERE GuNum=%u",
            strTime.GetString(),
            dwClub));

	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::SetClubAllianceSec(DWORD dwClub, __time64_t tSec)
{
	CTime cTemp(tSec);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    std::string Query(
        sc::string::format(
            "UPDATE dbo.GuildInfo SET GuAllianceSec='%s' WHERE GuNum=%u",
            strTime.GetString(),
            dwClub));
	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::SetClubAllianceDis(DWORD dwClub, __time64_t tDis)
{
	CTime cTemp(tDis);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    std::string Query(
        sc::string::format(
            "UPDATE dbo.GuildInfo SET GuAllianceDis='%s' WHERE GuNum=%u",
            strTime.GetString(),
            dwClub));

	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority)
{
	CTime cTemp(tAuthority);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

	std::string Query(
	    sc::string::format(
            "UPDATE dbo.GuildInfo SET GuAuthorityTime='%s' WHERE GuNum=%u",
            strTime.GetString(),
            dwClub));

	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::AddClubMember(DWORD dwClub, DWORD dwChaNum)
{
    sc::writeLogError("COdbcManager::AddClubMember. Use ADO");
    return sc::db::DB_ERROR;
/*
	std::string Query(
	    sc::string::format(
            "{call sp_add_guild_member(%u,%u,?)}", dwClub, dwChaNum));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DeleteClubMember(DWORD dwChaNum)
{
    sc::writeLogError("COdbcManager::DeleteClubMember. Use ADO");
    return sc::db::DB_ERROR;
/*
	std::string Query(
	    sc::string::format(
            "{call sp_delete_guild_member(%u,?)}", dwChaNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember)
{
    MIN_STATIC_ASSERT(GLCLUBMEMBER::VERSION <= 0x0004);

	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
		
	std::string Query(
	    sc::string::format(
            "EXEC sp_GetGuildMember %1%", dwClub));	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaNum     = 0, cbChaNum     = SQL_NTS;
	SQLINTEGER nGuPosition = 0, cbGuPosition = SQL_NTS;
    SQLINTEGER nChaLevel   = 0, cbChaLevel   = SQL_NTS;
    SQLINTEGER nChaClass   = 0, cbChaClass   = SQL_NTS;
	TIMESTAMP_STRUCT sLastPlayTime; SQLINTEGER cbLastPlayTime = SQL_NTS;

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
			GLCLUBMEMBER ClubMember;            

			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nChaNum, 0, &cbChaNum);
			::SQLGetData(spConn->hStmt, 2, SQL_C_CHAR, szChaName, CHR_ID_LENGTH+1, &cbChaName);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nGuPosition, 0, &cbGuPosition);
            ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG, &nChaLevel,   0, &cbChaLevel);
            ::SQLGetData(spConn->hStmt, 5, SQL_C_TYPE_TIMESTAMP, &sLastPlayTime, 0, &cbLastPlayTime); // ChaLastPlayDate
            ::SQLGetData(spConn->hStmt, 6, SQL_C_LONG, &nChaClass,   0, &cbChaClass);

			if (cbChaName != 0 && cbChaName != -1)
				ClubMember.SetName((const char*) szChaName);
			ClubMember.m_ChaDbNum = static_cast<DWORD> (nChaNum);
			ClubMember.m_Grade    = static_cast<DWORD> (nGuPosition);
            ClubMember.m_ChaLevel = static_cast<int>   (nChaLevel);
            ClubMember.m_ChaClass = static_cast<DWORD> (nChaClass);
            sc::time::AdjustTime(sLastPlayTime);
            ClubMember.SetLastPlayTime(sc::time::GetTime(sLastPlayTime));

			vMember.push_back(ClubMember);
		}
		else
		{
			break;
		}
	}
	return sc::db::DB_OK;
}

int COdbcManager::GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth)
{
    sc::writeLogError(std::string("GetClubAuth. do not call this function. use ado."));
    return sc::db::DB_ERROR;
}

int COdbcManager::ResetClubPosition(DWORD ClubDbNum)
{
    sc::writeLogError(std::string("ResetClubPosition. do not call this function. use ado."));
    return sc::db::DB_ERROR;
}

int COdbcManager::GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub)
{
    MIN_STATIC_ASSERT(GLClub::VERSION <= 0x0007);

    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    // 클럽 배틀 시스템 추가로 인해서 GuildInfo Table 컬럼 추가
    // GuildInfo Table 컬럼 추가, viewGuildInfo 수정 필요
	SQLINTEGER nGuBattleWin    = 0, cbGuBattleWin=SQL_NTS;
    SQLINTEGER nGuBattleDraw   = 0, cbGuBattleDraw=SQL_NTS;
	SQLINTEGER nGuBattleLose   = 0, cbGuBattleLose=SQL_NTS;

	SQLINTEGER nGuAllianceBattleWin    = 0, cbGuAllianceBattleWin=SQL_NTS;
    SQLINTEGER nGuAllianceBattleDraw   = 0, cbGuAllianceBattleDraw=SQL_NTS;
	SQLINTEGER nGuAllianceBattleLose   = 0, cbGuAllianceBattleLose=SQL_NTS;

	TIMESTAMP_STRUCT sBattleLastTime;    SQLINTEGER cbBattleLastTime=SQL_NTS;

    std::string Query("EXEC sp_GetGuildInfoAll");        
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	SQLCHAR szChaName[CHR_ID_LENGTH+1] = {0};         SQLINTEGER cbChaName  = SQL_NTS;
	SQLCHAR szGuName[CHR_ID_LENGTH+1] = {0};          SQLINTEGER cbGuName   = SQL_NTS;
	SQLCHAR szGuNotice[EMCLUB_NOTICE_LEN]= {0};       SQLINTEGER cbGuNotice = SQL_NTS;
    SQLCHAR GuNoticeChaName[CHR_ID_LENGTH+1] = {0};   SQLINTEGER cbNoticeChaName = SQL_NTS;    
    SQLCHAR szGuNoticeNewbie[EMCLUB_NOTICE_LEN]= {0}; SQLINTEGER cbGuNoticeNewbie = SQL_NTS;
    
	SQLINTEGER nGuNum    = 0, cbGuNum=SQL_NTS;
    SQLINTEGER nChaNum   = 0, cbChaNum=SQL_NTS;
	SQLINTEGER nGuDeputy = 0, cbGuDeputy=SQL_NTS;
    SQLINTEGER nGuRank   = 0, cbGuRank=SQL_NTS;
    SQLINTEGER nGuMarkVer= 0, cbGuMarkVer=SQL_NTS;
    SQLINTEGER nGuCountry = 0, cbGuCountry=SQL_NTS; // World battle 2012-02-13 jgkim
    SQLINTEGER nGuPublicType = 0, cbGuPublicType=SQL_NTS;

    SQLSMALLINT nGuAutoKickUse   = 0; SQLINTEGER cbGuAutoKickUse   = SQL_NTS;
    SQLSMALLINT nGuAutoKickGrade = 0; SQLINTEGER cbGuAutoKickGrade = SQL_NTS;
    SQLSMALLINT nGuAutoKickDay   = 0; SQLINTEGER cbGuAutoKickDay   = SQL_NTS;

    TIMESTAMP_STRUCT sMakeTime;       SQLINTEGER cbMakeTime       = SQL_NTS;
    TIMESTAMP_STRUCT sExpireTime;     SQLINTEGER cbExpireTime     = SQL_NTS;
    TIMESTAMP_STRUCT sAuthorityTime;  SQLINTEGER cbAuthorityTime  = SQL_NTS;
	TIMESTAMP_STRUCT sAllianceSec;    SQLINTEGER cbAllianceSec    = SQL_NTS;
	TIMESTAMP_STRUCT sAllianceDis;    SQLINTEGER cbAllianceDis    = SQL_NTS;
    TIMESTAMP_STRUCT GuNoticeDate;    SQLINTEGER cbNoticeDate     = SQL_NTS;

    LONGLONG   llGuildMoney = 0;       SQLINTEGER cbGuildMoney = SQL_NTS;
	LONGLONG   llGuildIncomeMoney = 0; SQLINTEGER cbGuildIncomeMoney = SQL_NTS;

    DWORD dwSize = (DWORD) (sizeof(DWORD) * EMCLUB_MARK_SX*EMCLUB_MARK_SY);
    se::ByteStream ByteStream;
	
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
            MIN_STATIC_ASSERT(GLClub::VERSION <= 0x0007);

			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nGuNum, 0, &cbGuNum);
            ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nChaNum, 0, &cbChaNum);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nGuDeputy, 0, &cbGuDeputy);
			::SQLGetData(spConn->hStmt, 4, SQL_C_CHAR, szChaName, CHR_ID_LENGTH, &cbChaName);
            ::SQLGetData(spConn->hStmt, 5, SQL_C_CHAR, szGuName, CHR_ID_LENGTH, &cbGuName);

            ::SQLGetData(spConn->hStmt, 6, SQL_C_LONG, &nGuRank, 0, &cbGuRank);
            ::SQLGetData(spConn->hStmt, 7, SQL_C_SBIGINT, &llGuildMoney, 0, &cbGuildMoney);
			::SQLGetData(spConn->hStmt, 8, SQL_C_SBIGINT, &llGuildIncomeMoney, 0, &cbGuildIncomeMoney);
            ::SQLGetData(spConn->hStmt, 9, SQL_C_LONG, &nGuMarkVer, 0, &cbGuMarkVer);
			// 클럽 배틀 추가
			::SQLGetData(spConn->hStmt, 10, SQL_C_LONG, &nGuBattleWin, 0, &cbGuBattleWin);

			::SQLGetData(spConn->hStmt, 11, SQL_C_LONG, &nGuBattleDraw, 0, &cbGuBattleDraw);
			::SQLGetData(spConn->hStmt, 12, SQL_C_LONG, &nGuBattleLose, 0, &cbGuBattleLose);
			::SQLGetData(spConn->hStmt, 13, SQL_C_TYPE_TIMESTAMP, &sBattleLastTime, 0, &cbBattleLastTime);
			::SQLGetData(spConn->hStmt, 14, SQL_C_LONG, &nGuAllianceBattleWin, 0, &cbGuAllianceBattleWin);
			::SQLGetData(spConn->hStmt, 15, SQL_C_LONG, &nGuAllianceBattleDraw, 0, &cbGuAllianceBattleDraw);

			::SQLGetData(spConn->hStmt, 16, SQL_C_LONG, &nGuAllianceBattleLose, 0, &cbGuAllianceBattleLose);
            ::SQLGetData(spConn->hStmt, 17, SQL_C_TYPE_TIMESTAMP, &sMakeTime, 0, &cbMakeTime);
            ::SQLGetData(spConn->hStmt, 18, SQL_C_TYPE_TIMESTAMP, &sExpireTime, 0, &cbExpireTime);
            ::SQLGetData(spConn->hStmt, 19, SQL_C_TYPE_TIMESTAMP, &sAuthorityTime, 0, &cbAuthorityTime);
			::SQLGetData(spConn->hStmt, 20, SQL_C_TYPE_TIMESTAMP, &sAllianceSec, 0, &cbAllianceSec);

			::SQLGetData(spConn->hStmt, 21, SQL_C_TYPE_TIMESTAMP, &sAllianceDis, 0, &cbAllianceDis);
			::SQLGetData(spConn->hStmt, 22, SQL_C_CHAR, szGuNotice, EMCLUB_NOTICE_LEN, &cbGuNotice);
            ::SQLGetData(spConn->hStmt, 23, SQL_C_LONG, &nGuCountry, 0, &cbGuCountry); // World battle 2012-02-13 jgkim
            
            ::SQLGetData(spConn->hStmt, 24, SQL_C_CHAR, GuNoticeChaName, CHR_ID_LENGTH, &cbNoticeChaName);
            ::SQLGetData(spConn->hStmt, 25, SQL_C_TYPE_TIMESTAMP, &GuNoticeDate, 0, &cbNoticeDate);
            
            ::SQLGetData(spConn->hStmt, 26, SQL_C_LONG, &nGuPublicType, 0, &cbGuPublicType); // GuPublicType

            ::SQLGetData(spConn->hStmt, 27, SQL_C_SHORT, &nGuAutoKickUse,   0, &cbGuAutoKickUse); // A.GuAutoKickUse
            ::SQLGetData(spConn->hStmt, 28, SQL_C_SHORT, &nGuAutoKickGrade, 0, &cbGuAutoKickGrade); // A.GuAutoKickGrade
            ::SQLGetData(spConn->hStmt, 29, SQL_C_SHORT, &nGuAutoKickDay,   0, &cbGuAutoKickDay); // A.GuAutoKickDay
            ::SQLGetData(spConn->hStmt, 30, SQL_C_CHAR, szGuNoticeNewbie, EMCLUB_NOTICE_LEN, &cbGuNoticeNewbie);

            std::tr1::shared_ptr<GLClub> spClub = std::tr1::shared_ptr<GLClub> (new GLClub);

            spClub->m_Country = static_cast<wb::EM_COUNTRY> (nGuCountry);
            spClub->m_DbNum = static_cast<DWORD> (nGuNum);
            spClub->m_MasterCharDbNum = static_cast<DWORD> (nChaNum);
			spClub->m_dwCDCertifior = static_cast<DWORD> (nGuDeputy);
            spClub->PublicSet(static_cast<DWORD> (nGuPublicType));

            club::AUTO_KICK_OUT AutoKickOut(nGuAutoKickUse, nGuAutoKickGrade, nGuAutoKickDay);
            spClub->AutoKickOutSet(AutoKickOut);
            
            if (cbGuName != 0 && cbGuName != -1)
			{
				CString strGuName(szGuName);
				strGuName.Trim(_T(" ")); // 앞뒤 공백제거
                ::StringCchCopy(
					spClub->m_szName,
					CHR_ID_LENGTH,
					(const TCHAR*) strGuName.GetString() );
			}
            
            if (cbChaName != 0 && cbChaName != -1)
			{
				CString strChaName(szChaName);
				strChaName.Trim(_T(" ")); // 앞뒤 공백제거
			    ::StringCchCopy(
					spClub->m_szMasterName,
					CHR_ID_LENGTH,
					(const TCHAR*) strChaName.GetString() );
			}
			
            if (cbGuNotice != 0 && cbGuNotice != -1)
			{
				CString strGuNotice(szGuNotice);
				strGuNotice.Trim(_T(" ")); // 앞뒤 공백제거
				strGuNotice.Replace("''", "'"); // '' -> '
                spClub->SetNotice(strGuNotice.GetString());
			}

            if (cbGuNoticeNewbie != 0 && cbGuNoticeNewbie != -1)
            {
                CString strGuNoticeNewbie(szGuNoticeNewbie);
                strGuNoticeNewbie.Trim(_T(" ")); // 앞뒤 공백제거
                strGuNoticeNewbie.Replace("''", "'"); // '' -> '
                spClub->SetNoticeNewbie(strGuNoticeNewbie.GetString());
            }
	        
            spClub->m_dwRank = (DWORD) nGuRank;
            spClub->SetStorageMoney(llGuildMoney);
			spClub->SetIncomeMoney(llGuildIncomeMoney);
	        spClub->m_dwMarkVER = (DWORD) nGuMarkVer;

			// 클럽 배틀 추가
			spClub->m_dwBattleWin = (DWORD) nGuBattleWin;
			spClub->m_dwBattleDraw = (DWORD) nGuBattleDraw;
			spClub->m_dwBattleLose = (DWORD) nGuBattleLose;

			spClub->m_dwAllianceBattleWin = (DWORD) nGuAllianceBattleWin;
			spClub->m_dwAllianceBattleDraw = (DWORD) nGuAllianceBattleDraw;
			spClub->m_dwAllianceBattleLose = (DWORD) nGuAllianceBattleLose;

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sBattleLastTime.year < 1970 || sBattleLastTime.year >= 2999)
			{                
                Print(
                    sc::string::format(
                        "sCLUB.sBattleLastTime year %d", sBattleLastTime.year));

				sBattleLastTime.year = 1970;
				sBattleLastTime.month = 2;
				sBattleLastTime.day = 1;
				sBattleLastTime.hour = 1;
				sBattleLastTime.minute = 1;
				sBattleLastTime.second = 1;
			}            
            spClub->m_tLastBattle = sc::time::GetTime(sBattleLastTime);

             
            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sMakeTime.year < 1970 || sMakeTime.year >= 2999)
			{       
                Print(
                    sc::string::format(
                        "sCLUB.sMakeTime year %d", sMakeTime.year));

				sMakeTime.year = 1970;
				sMakeTime.month = 2;
				sMakeTime.day = 1;
				sMakeTime.hour = 1;
				sMakeTime.minute = 1;
				sMakeTime.second = 1;
			}            
            spClub->m_tOrganize = sc::time::GetTime(sMakeTime);
            
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sExpireTime.year < 1970 || sExpireTime.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "sCLUB.m_tDissolution year %d", sExpireTime.year));

				sExpireTime.year = 1970;
				sExpireTime.month = 2;
				sExpireTime.day = 1;
				sExpireTime.hour = 1;
				sExpireTime.minute = 1;
				sExpireTime.second = 1;
			}            
            spClub->m_tDissolution = sc::time::GetTime(sExpireTime);

            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAllianceSec.year < 1970 || sAllianceSec.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "sCLUB.m_tAllianceSec year %d", sAllianceSec.year));

				sAllianceSec.year = 1970;
				sAllianceSec.month = 2;
				sAllianceSec.day = 1;
				sAllianceSec.hour = 1;
				sAllianceSec.minute = 1;
				sAllianceSec.second = 1;
			}			
			spClub->m_tAllianceSec = sc::time::GetTime(sAllianceSec);

            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAllianceDis.year < 1970 || sAllianceDis.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "sCLUB.m_tAllianceDis year %d", sAllianceDis.year));

				sAllianceDis.year = 1970;
				sAllianceDis.month = 2;
				sAllianceDis.day = 1;
				sAllianceDis.hour = 1;
				sAllianceDis.minute = 1;
				sAllianceDis.second = 1;
			}			
			spClub->m_tAllianceDis = sc::time::GetTime(sAllianceDis);

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAuthorityTime.year < 1970 || sAuthorityTime.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "sCLUB.m_tAuthority year %d", sAuthorityTime.year));

				sAuthorityTime.year = 1970;
				sAuthorityTime.month = 2;
				sAuthorityTime.day = 1;
				sAuthorityTime.hour = 1;
				sAuthorityTime.minute = 1;
				sAuthorityTime.second = 1;
			}            
            spClub->m_tAuthority = sc::time::GetTime(sAuthorityTime);

            if (GuNoticeDate.year < 1970 || GuNoticeDate.year >= 2999)
            {
                Print(
                    sc::string::format(
                        "Club Notice Date %1%", GuNoticeDate.year));
                GuNoticeDate.year   = 1970;
                GuNoticeDate.month  = 2;
                GuNoticeDate.day    = 1;
                GuNoticeDate.hour   = 1;
                GuNoticeDate.minute = 1;
                GuNoticeDate.second = 1;
            }            
            spClub->m_NoticeDate = sc::time::GetTime(GuNoticeDate);

            if (cbNoticeChaName != 0 && cbNoticeChaName != -1)
            {
                CString strNoticeChaName(GuNoticeChaName);
                strNoticeChaName.Trim(_T(" ")); // 앞뒤 공백제거
                ::StringCchCopy(
                    spClub->m_szNoticeChaName,
                    CHR_ID_LENGTH,
                    (const TCHAR*) strNoticeChaName.GetString() );
            }
            
			ByteStream.ClearBuffer();
	        //SQLRETURN sReturn2 = ReadClubMarkImage(sCLUB.m_dwID, ByteStream);
			int nSize = ReadClubMarkImage(spClub->m_DbNum, ByteStream);

	        if (nSize <= 0)
            {
            }
            else
            {
				LPBYTE pBuff(NULL);
                ByteStream.GetBuffer(pBuff, dwSize);				
				memcpy(spClub->m_aryMark, pBuff, dwSize);
				ByteStream.ClearBuffer();
			}
            
            // Master Character Infomation
            std::string MasterChaName;
            int MasterChaLevel = 0;
            if (sc::db::DB_OK != GetChaNameLevel(spClub->m_MasterCharDbNum, MasterChaName, MasterChaLevel))
            {
                sc::writeLogError(
                    sc::string::format(
                        "Can't find club master. ClubDbNum %1% %2% MasterCharDbNum %3%",
                        spClub->m_DbNum,
                        spClub->m_szName,
                        spClub->m_MasterCharDbNum));
            }
            else
            {
                spClub->SetMasterName(MasterChaName);
                spClub->SetMasterCharLevel(MasterChaLevel);
            }
            vClub.push_back(spClub);
		}
		else
		{
			break;
		}
	}
	return sc::db::DB_OK;
}

int COdbcManager::ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream)
{
    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	ByteStream.ClearBuffer();			

	std::string Query(
	    sc::string::format(
            "SELECT GuildInfo.GuMarkImage FROM dbo.GuildInfo where (GuNum=%u)", dwClub));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
			ByteStream.ClearBuffer ();		
	}
	//m_pGameDB->FreeConnection(spConn);
	return lTotalSize;
}

int COdbcManager::SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice)
{
    sc::writeLogError("COdbcManager::SetClubNotice. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string TempClubNotice(ClubNotice);
    sc::string::SqlInjectionCheck(TempClubNotice);
    std::string Query(
        sc::string::format(
            "{call dbo.GuildInfoNoticeUpdate(%1%, '%2%', '%3%')}", dwClub, ChaName, TempClubNotice));
	return m_pGameDB->ExecuteSp(Query);
*/
}

int	COdbcManager::WriteClubMarkImage(
	DWORD dwClub,
	DWORD dwMarkVer,
	BYTE* pData, 
	int nSize )
{    
    if ((pData == NULL) || (nSize < 0) || (nSize > EMCLUB_MARK_SIZE))
        return sc::db::DB_ERROR;
 
	std::string Query(
	    sc::string::format(
            "UPDATE dbo.GuildInfo SET GuMarkVer=%u WHERE GuNum=%u", dwMarkVer, dwClub));

    m_pGameDB->ExecuteSQL(Query);

    // Update Image
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	Query = sc::string::format("UPDATE dbo.GuildInfo SET GuMarkImage=? WHERE (GuNum=%u)", dwClub);

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(spConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
	return sc::db::DB_OK;
}

int COdbcManager::GetChaGuildNum(DWORD dwChaNum)
{
    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
		
	std::string Query(
	    sc::string::format("SELECT GuNum FROM dbo.ChaInfo WHERE ChaNum=%u", dwChaNum));
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	SQLINTEGER nGuNum  = 0, cbGuNum=SQL_NTS;
    int nReturn = 0;
    
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
			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nGuNum, 0, &cbGuNum);
            nReturn = nGuNum;
		}
		else
		{
			break;
		}		
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	//m_pGameDB->FreeConnection(spConn);
	return nReturn;
}

int COdbcManager::SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney)
{
	if (llMoney < 0)
    {
		Print("SetClubIncomeMoney llMoney Must be large than zero");
        return sc::db::DB_ERROR;
    }

	std::string Query(
	    sc::string::format(
            "Update dbo.GuildInfo Set GuIncomeMoney=%1% WHERE GuNum=%2%", llMoney, dwClub));

	return m_pGameDB->ExecuteSQL(Query);
}

LONGLONG COdbcManager::GetClubIncomeMoney(DWORD dwClub)
{
	if (dwClub < 1)
    {
		Print(_T("GetClubIncomeMoney dwClub is must large than zero"));
        return sc::db::DB_ERROR;
    }

    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
		
	std::string Query(
	    sc::string::format(
            "SELECT GuIncomeMoney From dbo.GuildInfo WHERE GuNum=%u", dwClub));
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	LONGLONG   llGuMoney = 0;
    SQLINTEGER cbGuMoney = SQL_NTS;
	
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
			// Bind data
            ::SQLGetData(spConn->hStmt, 1, SQL_C_SBIGINT, &llGuMoney, 0, &cbGuMoney);
		}
		else
		{
			break;
		}
	}
	//m_pGameDB->FreeConnection(spConn);
	return llGuMoney;
}

int COdbcManager::SetClubMoney(DWORD dwClub, LONGLONG llMoney)
{
	if (llMoney < 0)
    {
		Print("SetClubMoney llMoney Must be large than zero");
        return sc::db::DB_ERROR;
    }

	std::string Query(
	    sc::string::format(
            "Update dbo.GuildInfo Set GuMoney=%1% WHERE GuNum=%2%", llMoney, dwClub));
    
	return m_pGameDB->ExecuteSQL(Query);
}

LONGLONG COdbcManager::GetClubMoney(DWORD dwClub)
{
	if (dwClub < 1)
    {
		Print(_T("GetClubMoney dwClub is must large than zero"));
        return sc::db::DB_ERROR;
    }

    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
		
	std::string Query(
	    sc::string::format(
            "SELECT GuMoney From dbo.GuildInfo WHERE GuNum=%u", dwClub));
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	LONGLONG   llGuMoney = 0;
    SQLINTEGER cbGuMoney = SQL_NTS;
	
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
			// Bind data
            ::SQLGetData(spConn->hStmt, 1, SQL_C_SBIGINT, &llGuMoney, 0, &cbGuMoney);
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);
	return llGuMoney;
}

int COdbcManager::ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream)
{
	if (dwClub < 1)
	{
		Print(_T("ReadClubStorage dwClub must be large than zero"));
		return sc::db::DB_ERROR;
	}

    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	ByteStream.ClearBuffer();
		
	std::string Query(
	    sc::string::format(
            "SELECT GuildInfo.GuStorage FROM dbo.GuildInfo where (GuNum=%u)", dwClub));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
			ByteStream.ClearBuffer ();		
	}
	//m_pGameDB->FreeConnection(spConn);
	return lTotalSize;
}

int COdbcManager::GetGuildMigrationState( DWORD dwClub )
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;

	std::string Query(
		sc::string::format("SELECT MigrationState FROM dbo.GuildInfo where (GuNum=%u)", dwClub));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	SQLINTEGER nState  = 0, cbState=SQL_NTS;
	int nReturn = 1;

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
			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nState, 0, &cbState);
			nReturn = nState;
		}
		else
		{
			break;
		}		
	}
	return nReturn;
}

int COdbcManager::WriteClubStorage(
	DWORD dwClub,
	BYTE* pData,
	int nSize )
{
    if ((pData == NULL) || (nSize < 0) || (dwClub < 1))
	{
		Print(_T("WriteClubStorage (pData, nSize, dwClub) invalid argument"));
        return sc::db::DB_ERROR;
	}   

    // Update Image
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	std::string Query(
	    sc::string::format(
            "UPDATE dbo.GuildInfo SET MigrationState=1, GuStorage=? WHERE (GuNum=%u)", dwClub));

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(spConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
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
	return sc::db::DB_OK;
}

int COdbcManager::SetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags)
{
	if (dwClub == 0 || dwChaNum <= 0)
	{
		return sc::db::DB_ERROR;
	}

	std::string Query(
	    sc::string::format(
            "UPDATE ChaInfo SET GuPosition=%u WHERE GuNum=%u AND ChaNum=%u",
            dwSubMasterFlags, dwClub, dwChaNum));
    
	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::SetClubDeputy(DWORD dwClub, DWORD dwDeputy)
{	
	std::string Query(
	    sc::string::format(
            "Update dbo.GuildInfo WITH (UPDLOCK) Set GuDeputy=%u WHERE GuNum=%u", dwDeputy, dwClub));
    
	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::SetClubAuthority (DWORD dwClub, DWORD dwMasterID)
{
	std::string Query(
	    sc::string::format(
            "Update dbo.GuildInfo WITH (UPDLOCK) Set ChaNum=%u WHERE GuNum=%u", dwMasterID, dwClub));    
	return m_pGameDB->ExecuteSQL(Query);
}

int COdbcManager::GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB)
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	std::string Query("SELECT RegionID, GuNum, RegionTax FROM dbo.GuildRegion WITH (NOLOCK) ORDER BY RegionID");
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
		
	SQLINTEGER   nRegionID  = 0; SQLINTEGER cbRegionID  = SQL_NTS;
	SQLINTEGER   nGuNum     = 0; SQLINTEGER cbGuNum     = SQL_NTS;
	SQLFLOAT     fRegionTax = 0; SQLINTEGER cbRegionTax = SQL_NTS;
		
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
			GLGUID_DB sGUID_DB;

			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,	&nRegionID,  0, &cbRegionID);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,	&nGuNum,     0, &cbGuNum);
			::SQLGetData(spConn->hStmt, 3, SQL_C_DOUBLE,	&fRegionTax, 0, &cbRegionTax);			
			
			sGUID_DB.m_dwID           = (DWORD) nRegionID;
			sGUID_DB.m_dwCLUBID       = (DWORD) nGuNum;
			sGUID_DB.m_fCommissionPer = (float) fRegionTax;

			vGUID_DB.push_back(sGUID_DB);
		}
		else
		{
			break;
		}		
	}	  
	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
}
    
int COdbcManager::SetClubRegion(DWORD dwRegionID, DWORD dwClub, float fTax)
{
    sc::writeLogError("COdbcManager::SetClubRegion. Use ADO");
    return sc::db::DB_ERROR;
/*
	std::string Query(
	    sc::string::format(
            "{call sp_add_guild_region(%u,%u,%f,?)}", dwRegionID, dwClub, fTax));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DelClubRegion(DWORD dwRegionID, DWORD dwClub)
{
    sc::writeLogError("COdbcManager::DelClubRegion. Use ADO");
    return sc::db::DB_ERROR;
/*
	std::string Query(
	    sc::string::format(
            "{call sp_delete_guild_region(%u,%u,?)}", dwRegionID, dwClub));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int	COdbcManager::SetClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
    sc::writeLogError("COdbcManager::SetClubAlliance. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (dwClubP == 0 || dwClubS == 0)
	{
		Print(_T("SetClubAlliance:Check Club Number"));
		return sc::db::DB_ERROR;
	} // if (dwClubP == 0 || dwClubS == 0)
	
	std::string Query(
	    sc::string::format(
            "{call InsertGuildAlliance(%u,%u,?)}", dwClubP, dwClubS));

	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DelClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
	if (dwClubP == 0 || dwClubS == 0)
	{
		Print(_T("DelClubAlliance:Check Club Number"));
		return sc::db::DB_ERROR;
	}
	
	std::string Query(
	    sc::string::format(
            "{call DeleteGuildAlliance(%u,%u,?)}", dwClubP, dwClubS));

	return m_pGameDB->ExecuteSpInt(Query);
}

int COdbcManager::GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance )
{
	vecAlliance.clear();

	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;	
	
	std::string Query("SELECT GuNumP, GuNumS FROM dbo.GuildAlliance WITH (NOLOCK) ORDER BY GuNumP");
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
		
	SQLINTEGER   nGuNumP = 0; SQLINTEGER cbGuNumP = SQL_NTS;
	SQLINTEGER   nGuNumS = 0; SQLINTEGER cbGuNumS = SQL_NTS;	
		
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
			// Bind data			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,	&nGuNumP, 0, &cbGuNumP);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,	&nGuNumS, 0, &cbGuNumS);

			vecAlliance.push_back ( std::make_pair(nGuNumP,nGuNumS) );
		}
		else
		{
			break;
		}		
	}	  

	//m_pGameDB->FreeConnection(spConn);

	return sc::db::DB_OK;
}

int COdbcManager::SetClubBattle( DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, int nAlliance )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ODBCManager SetClubBattle"));
		return sc::db::DB_ERROR;
	}

    std::string Query(
	    sc::string::format(
            "{call sp_RequestGuBattle(%u,%u,%u,%u,?)}", dwClubP, dwClubS, dwEndTime, nAlliance));
	return m_pGameDB->ExecuteSpInt(Query);
}

int COdbcManager::EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ODBCManager EndClubBattle"));
		return sc::db::DB_ERROR;
	}

	int nReturn;

	if ( bAlliance ) 
	{
		std::string Query(
		    sc::string::format(
                "{call sp_EndGuAllianceBattle(%u,%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuFlag, nGuKillNum, nGuDeathNum));
		nReturn = m_pGameDB->ExecuteSpInt(Query);
	}
	else
	{
		std::string Query(
		    sc::string::format(
                "{call sp_EndGuBattle(%u,%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuFlag, nGuKillNum, nGuDeathNum));
		nReturn = m_pGameDB->ExecuteSpInt(Query);
	}

	return nReturn;
}

int COdbcManager::SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ODBCManager SaveClubBattle"));
		return sc::db::DB_ERROR;
	}

	std::string Query(
	    sc::string::format(
            "{call sp_SaveGuBattle(%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuKillNum, nGuDeathNum));
	return m_pGameDB->ExecuteSpInt(Query);
}

int COdbcManager::ReSetAllianceBattle( DWORD dwClub )
{
	if (dwClub <= 0 )
	{
		Print(_T("ODBCManager ReSetAllianceBattle"));
		return sc::db::DB_ERROR;
	}

	std::string Query(
	    sc::string::format(
            "{call sp_ResetAllianceBattle(%u,?)}", dwClub));
	return m_pGameDB->ExecuteSpInt(Query);
}

int COdbcManager::GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo )
{
	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "SELECT GuSNum, GuFlag, GuKillNum, GuDeathNum, GuBattleStartDate, GuBattleEndDate, GuAlliance FROM dbo.GuildBattle WHERE GuPNum=%u and GuFlag=%d ORDER BY GuBattleNum",
            dwClub,
            GLCLUBBATTLE::CLUB_BATTLE_NOW));
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);		
		return sc::db::DB_ERROR;
	}
	
	SQLINTEGER nGuSNum		= 0, cbGuSNum		= SQL_NTS;
	SQLINTEGER nGuFlag		= 0, cbGuFlag		= SQL_NTS;
	SQLINTEGER nGuKillNum	= 0, cbGuKillNum	= SQL_NTS;
	SQLINTEGER nGuDeathNum	= 0, cbGuDeathNum	= SQL_NTS;
	SQLINTEGER nGuAlliance	= 0, cbGuAlliance	= SQL_NTS;
	TIMESTAMP_STRUCT sStartDate; SQLINTEGER cbStartDate	= SQL_NTS;
	TIMESTAMP_STRUCT sEndDate; SQLINTEGER cbEndDate	= SQL_NTS;
	
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
			GLCLUBBATTLE sBattle;

			// Bind data
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,	  &nGuSNum,             0, &cbGuSNum);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,	  &nGuFlag,             0, &cbGuFlag);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG,	  &nGuKillNum,          0, &cbGuKillNum);
			::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,	  &nGuDeathNum,         0, &cbGuDeathNum);
			::SQLGetData(spConn->hStmt, 5, SQL_C_TYPE_TIMESTAMP, &sStartDate,    0, &cbStartDate);
			::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sEndDate,		0, &cbEndDate);
			::SQLGetData(spConn->hStmt, 7, SQL_C_LONG,	  &nGuAlliance,         0, &cbGuAlliance);
			
			sBattle.m_dwCLUBID		= (DWORD) nGuSNum;
			sBattle.m_nBattleFlag	= (GLCLUBBATTLE::CLUB_BATTLE_ENUM)nGuFlag;
			sBattle.m_wKillPoint	= (WORD)nGuKillNum;
			sBattle.m_wDeathPoint	= (WORD)nGuDeathNum;
			sBattle.m_bAlliance		= ( nGuAlliance > 0 ) ? true : false;

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sStartDate.year < 1970 || sStartDate.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "vBattleInfo.m_tGuBattleStartTime year %d", sStartDate.year));

				sStartDate.year = 1970;
				sStartDate.month = 2;
				sStartDate.day = 1;
				sStartDate.hour = 1;
				sStartDate.minute = 1;
				sStartDate.second = 1;
			}
            CTime cTempStart(sStartDate.year, sStartDate.month,  sStartDate.day, 
                         sStartDate.hour, sStartDate.minute, sStartDate.second);
            sBattle.m_tStartTime = cTempStart.GetTime();

			if (sEndDate.year < 1970 || sEndDate.year >= 2999)
			{                
				Print(
                    sc::string::format(
                        "vBattleInfo.m_tEndTime year %d", sEndDate.year));

				sEndDate.year = 1970;
				sEndDate.month = 2;
				sEndDate.day = 1;
				sEndDate.hour = 1;
				sEndDate.minute = 1;
				sEndDate.second = 1;
			}
            CTime cTempEnd(sEndDate.year, sEndDate.month,  sEndDate.day, 
                         sEndDate.hour, sEndDate.minute, sEndDate.second);
            sBattle.m_tEndTime = cTempEnd.GetTime();

			vBattleInfo.push_back(sBattle);
		}
		else
		{
			break;
		}		
	}
	
	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;	
}

int COdbcManager::InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName)
{
    sc::writeLogError("Do not call this function. use ADO InsertClubAuth.");
    return sc::db::DB_ERROR;
}

int COdbcManager::UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName)
{
    sc::writeLogError("Do not call this function. use ADO UpdateClubAuth.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB)
{    
    sc::writeLogError("Do not call this function. use ADO ChangeClubAuthIndex.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChangeClubMemberGrade(DWORD ChaDbNum, int Grade)
{
    sc::writeLogError("Do not call this function. use ADO ChangeClubMemberGrade.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName)
{
    sc::writeLogError("Do not call this function. use ADO ClubNickUpdate.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag)
{
    sc::writeLogError("Do not call this function. use ADO ClubPublicFlagUpdate.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut)
{
    sc::writeLogError("Do not call this function. use ADO ClubAutoKickUpdate.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice)
{
    sc::writeLogError("Do not call this function. use ADO ClubNewbieNoticeUpdate.");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaClubSecedeTimeReset(DWORD ChaDbNum)
{
    sc::writeLogError("Do not call this function. use ADO ChaClubSecedeTimeReset.");
    return sc::db::DB_ERROR;
}