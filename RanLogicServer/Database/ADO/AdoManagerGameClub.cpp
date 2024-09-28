#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Math/SeqUniqueGuid.h"
#include "../../../RanLogic/Club/GLClub.h"
#include "../../Club/GLClubAgent.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

//! 새로운 클럽을 생성한다.
//! \param strClubName 클럽이름
//! \param dwChaNum    클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)	
int AdoManager::CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@ChaNum",    ChaDbNum);
    Ado.AppendIParamVarchar("@GuName",    ClubName.c_str(), ClubName.length());
    Ado.AppendIParamInteger    ("@GuCountry", static_cast<int> (Country));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_create_guild", "@nReturn");
}

//! 클럽을 삭제한다.
//! dwClub : 클럽번호
//! dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
int AdoManager::DeleteClub(DWORD dwClub, DWORD dwChaNum)    
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamInteger("@GuNum",  static_cast<int> (dwClub));
    Ado.AppendIParamInteger("@ChaNum", static_cast<int> (dwChaNum));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_delete_guild", "@nReturn");
}

//! 클럽랭크를 세팅한다.
//! dwClub : 클럽번호
//! dwRank : 랭킹
int AdoManager::SetClubRank(DWORD dwClub, DWORD dwRank)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamInteger("@nGuNum", static_cast<int> (dwClub));
    Ado.AppendIParamInteger("@nRank",  static_cast<int> (dwRank));
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_update_guild_rank", "@nReturn");
}

int AdoManager::ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream)
{
    sc::writeLogError("Do not call this function. ReadClubMarkImage");
    return sc::db::DB_ERROR;
}

int AdoManager::WriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize)
{
    sc::writeLogError("Do not call this function. WriteClubMarkImage");
    return sc::db::DB_ERROR;
}

//! 클럽 해체시간을 설정한다.
//! \param dwClub 클럽번호
//! \param tDiss 해체시간
int AdoManager::SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss)
{
    sc::writeLogError("Do not call this function. SetClubDissolutionTime");
    return sc::db::DB_ERROR;
}

//! 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
//! \param dwClub 클럽번호
//! \param tSec 시간
int AdoManager::SetClubAllianceSec(DWORD dwClub, __time64_t tSec)
{
    sc::writeLogError("Do not call this function. SetClubAllianceSec");
    return sc::db::DB_ERROR;
}

//! 클럽의 동맹해산 시간을 정한다.
//! \param dwClub 클럽번호
//! \param tDis 해산시간
int AdoManager::SetClubAllianceDis(DWORD dwClub, __time64_t tDis)
{
    sc::writeLogError("Do not call this function. SetClubAllianceDis");
    return sc::db::DB_ERROR;
}

int AdoManager::SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority)
{
    sc::writeLogError("Do not call this function. SetClubAuthorityTime");
    return sc::db::DB_ERROR;
}

int AdoManager::GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub)
{
    MIN_STATIC_ASSERT(GLClub::VERSION <= 0x0007);
    sc::writeLogError("Do not call this function. GetClubInfo");
    return sc::db::DB_ERROR;
}

int AdoManager::GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember)
{
    sc::writeLogError("Do not call this function. GetClubMember");
    return sc::db::DB_ERROR;
}

//! 클럽의 권한 설정을 가져온다.
int AdoManager::GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth)
{    
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@GuNum", ClubDbNum);

    if (!Ado.Execute4Cmd("dbo.GetGuildAuth", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())        
        return sc::db::DB_ERROR;

    int ItemMid = 0;
    int ItemSid = 0;
    int ItemPrice  = 0;
    do
    {
        // AuthIndex, AuthFlag, AuthName
        int AuthIndex = 0;
        DWORD AuthFlag = 0;
        std::string AuthName;

        Ado.GetCollect(0, AuthIndex);
        Ado.GetCollect(1, AuthFlag);
        Ado.GetCollect(2, AuthName);
        if (!Ado.IsCollectedAll())
        {
            sc::writeLogError(
                std::string("GetClubAuth IsCollectedAll"));
            return sc::db::DB_ERROR;
        }

        club::AUTH_GRADE_DB AuthGrade(AuthIndex, AuthName, AuthFlag);
        vAuth.push_back(AuthGrade);
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::ResetClubPosition(DWORD ClubDbNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);
    APPEND_IPARAM_INT(Ado, "@GuNum", ClubDbNum);
    return ExecuteStoredProcedure("dbo.ResetGuildPosition", Ado);
}

int AdoManager::AddClubMember(DWORD dwClub, DWORD dwChaNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@GuNum",  static_cast<int> (dwClub));
    Ado.AppendIParamInteger("@ChaNum", static_cast<int> (dwChaNum));
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_add_guild_member", "@nReturn");
}

int AdoManager::DeleteClubMember(DWORD dwChaNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();
        
    Ado.AppendIParamInteger("@ChaNum", static_cast<int> (dwChaNum));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_delete_guild_member", "@nReturn");        
}

int AdoManager::DeleteClubMemberWithNoPenalty( DWORD dwChaNum )
{
	sc::db::AdoExt Ado( m_GameDBConnString );
	Ado.AppendRParamInteger();
	Ado.AppendIParamInteger( "@ChaNum", static_cast< int >( dwChaNum ) );
	Ado.AppendOParamInteger( "@nReturn" );

	return Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_delete_guild_member_nopenalty", "@nReturn" );        
}

int AdoManager::SetClubBattle(DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance)
{
    sc::writeLogError("Do not call this function. SetClubBattle");
    return sc::db::DB_ERROR;
}

int AdoManager::EndClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance)
{
    sc::writeLogError("Do not call this function. EndClubBattle");
    return sc::db::DB_ERROR;
}

//! 클럽 배틀정보를 저장한다,(클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
//! \param dwClubP 주클럽 번호(클럽 배틀의 신청을 받은 주)
//! \param dwClubS 보조 클럽 번호(클럽 배틀의 신청을 한 클럽 )
//! \param nGuKillNum 클럽 배틀 킬수
//! \return
int AdoManager::SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum)
{
    sc::writeLogError("Do not call this function. SaveClubBattle");
    return sc::db::DB_ERROR;
}

//! 동맹배틀 전적을 초기화 시키긴다.
//! \param dwClub 클럽 번호
int AdoManager::ReSetAllianceBattle(DWORD dwClub)
{
    sc::writeLogError("Do not call this function. ReSetAllianceBattle");
    return sc::db::DB_ERROR;
}

//! 해당 클럽의 배틀중인 클럽 정보를 가져온다.
//! \param dwClub 클럽번호
//! \param &vMember 클럽배틀 정보
int AdoManager::GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo)
{
    sc::writeLogError("Do not call this function. GetClubBattleInfo");
    return sc::db::DB_ERROR;
}

//! 현재 모든지역의 선도 클럽데이터를 가져온다.
int AdoManager::GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB)
{
    sc::writeLogError("Do not call this function. GetClubRegion");
    return sc::db::DB_ERROR;
}

//! 지역과 그지역을 소유한 클럽 세율을 정한다.
//! \param dwRegionID : 지역 ID
//! \param dwClub : 클럽 ID
//! \param fTax : 세율
int AdoManager::SetClubRegion(DWORD dwRegionID, DWORD dwClub, float fTax)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamInteger  ("@RegionID", static_cast<int> (dwRegionID));
    Ado.AppendIParamInteger  ("@GuNum",    static_cast<int> (dwClub));
    Ado.AppendIParamFloat("@RegionTax", fTax);

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_add_guild_region", "@nReturn");
}

//! 해당 지역의 소유 상태를 리셋한다.
//! \param dwRegionID 지역 ID
//! \param dwClub 클럽 ID(기본 0 이다)
int AdoManager::DelClubRegion(DWORD dwRegionID, DWORD dwClub)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@RegionID", static_cast<int> (dwRegionID));
    Ado.AppendIParamInteger("@GuNum", static_cast<int> (dwClub));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_delete_guild_region", "@nReturn");
}

//! 해당클럽의 보유금액을 가져온다.(Storage)
//! \param dwClub 클럽번호
//! \param 리턴값은 >= 0, 0 보다 작으면 ERROR
LONGLONG AdoManager::GetClubMoney(DWORD dwClub)
{
	if ( dwClub < 1 )
		return sc::db::DB_ERROR;

	sc::db::AdoExt AdoGame( m_GameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@GuNum", static_cast< int >( dwClub ) );
	if ( !AdoGame.Execute4Cmd( "dbo.sp_GetGuildMoney", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( AdoGame.GetEOF() )
		return sc::db::DB_ERROR;

	LONGLONG llGuMoney = 0;
	do
	{
		AdoGame.GetCollect( "GuMoney", llGuMoney );
	} while ( AdoGame.Next() );

	return llGuMoney;
    //sc::writeLogError("Do not call this function. GetClubMoney");
    //return sc::db::DB_ERROR;
}

//! 해당클럽의 보유금액을 세팅한다.(Storage)
//! \param dwClub : 클럽번호
//! \param llMoney : 보유금액(금액은 >= 0)
int AdoManager::SetClubMoney(DWORD dwClub, LONGLONG llMoney)
{
    sc::writeLogError("Do not call this function. SetClubMoney");
    return sc::db::DB_ERROR;
}

//! 해당클럽의 수입금액을 세팅한다.(Income money)
//! \param dwClub : 클럽번호
//! \param llMoney : 수입금액(금액은 >= 0)
int AdoManager::SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney)    
{
    sc::writeLogError("Do not call this function. SetClubIncomeMoney");
    return sc::db::DB_ERROR;
}

//! 해당클럽의 수입금액을 가져온다.(Income money)
//! \param dwClub : 클럽번호
//! \return 리턴값은 >= 0, 0 보다 작으면 ERROR
LONGLONG AdoManager::GetClubIncomeMoney(DWORD dwClub)
{
    sc::writeLogError("Do not call this function. GetClubIncomeMoney");
    return sc::db::DB_ERROR;
}

//! 해당클럽의 클럽창고를 읽어온다.
//! \param dwClub : 클럽번호
//! \param ByteStream : 버퍼
int AdoManager::ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream)    
{
    sc::writeLogError("Do not call this function. ReadClubStorage");
    return sc::db::DB_ERROR;
}

int AdoManager::GetGuildMigrationState( DWORD dwClub )   
{
	sc::writeLogError("Do not call this function. GetGuildMigrationState");
	return sc::db::DB_ERROR;
}

//! 해당클럽의 클럽창고를 저장한다.
//! \param dwClub : 클럽번호
//! \param pData : 창고위치
//! \param nSize : 버퍼 전체크기
int AdoManager::WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize)
{
    sc::writeLogError("Do not call this function. WriteClubStorage");
    return sc::db::DB_ERROR;
}

//! 부 클럽장을 세팅한다
//! \param dwClub : 클럽번호
//! \param dwChaNum : 캐릭터번호
//! \param dwSubMasterFlags : 플래그(0 으로 넣으면 일반길드원이 된다)
int AdoManager::SetClubMasterFlags(
        DWORD dwClub,
        DWORD dwChaNum,
        DWORD dwSubMasterFlags)
{
    sc::writeLogError("Do not call this function. SetClubMasterFlags");
    return sc::db::DB_ERROR;
}

//! 동맹 클럽을 결성한다.
//! \ db 는 클럽의 무결성을 체크하지 않는다.
//! \ A 가 B 의 보조인 상태에서 
//! \ B 가 C 의 보조로 들어가고
//! \ C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.	
//! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
//! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
int AdoManager::SetClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
    if (dwClubP == 0 || dwClubS == 0)
    {
        sc::writeLogError("SetClubAlliance:Check Club Number");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@nGuNump", static_cast<int> (dwClubP));
    Ado.AppendIParamInteger("@nGuNumS", static_cast<int> (dwClubS));
    
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.InsertGuildAlliance", "@nReturn");
}

//! 동맹 클럽을 해체한다.
//! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
//! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
int AdoManager::DelClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
    sc::writeLogError("Do not call this function. DelClubAlliance");
    return sc::db::DB_ERROR;
}

//! 동맹 클럽 리스트를 가져온다.
//! <주클럽,보조클럽>
int AdoManager::GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance)
{
    sc::writeLogError("Do not call this function. GetClubAlliance");
    return sc::db::DB_ERROR;
}

//! 클럽의 공지사항을 세팅한다.
int AdoManager::SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice)
{
    std::string TempClubNotice(ClubNotice);
    sc::string::SqlInjectionCheck(TempClubNotice);
    
    sc::db::AdoExt Ado(m_GameDBConnString);

    Ado.AppendIParamInteger("@GuNum", static_cast<int> (dwClub));
    Ado.AppendIParamVarchar("@GuNoticeChaName", ChaName.c_str(), ChaName.length());
    Ado.AppendIParamVarchar("@GuNotice", TempClubNotice.c_str(), TempClubNotice.length());

    if (Ado.ExecuteStoredProcedure("dbo.GuildInfoNoticeUpdate"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

//! 클럽의 선도클럽 인증 대리자를 세팅한다.
int AdoManager::SetClubDeputy(DWORD dwClub, DWORD dwDeputy)
{
    sc::writeLogError("Do not call this function. SetClubDeputy");
    return sc::db::DB_ERROR;
}

int AdoManager::SetClubAuthority(DWORD dwClub, DWORD dwMasterID)
{
    sc::writeLogError("Do not call this function. SetClubAuthority");
    return sc::db::DB_ERROR;
}   

int AdoManager::InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName)
{
    std::string TempAuthName(AuthName);
    sc::string::replaceForDb(TempAuthName);
    char ClubAuthName[CHR_ID_LENGTH] = {0};
    StringCchCopy(ClubAuthName, CHR_ID_LENGTH, TempAuthName.c_str());
    
    BYTE byteAuthIndex = static_cast<BYTE> (AuthIndex);

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT    (Ado);

    APPEND_IPARAM_INT    (Ado, "@GuNum",     ClubDbNum);
    APPEND_IPARAM_TINYINT(Ado, "@AuthIndex", byteAuthIndex);
    APPEND_IPARAM_INT    (Ado, "@AuthFlag",  AuthFlag);
    APPEND_IPARAM_VARCHAR(Ado, "@AuthName",  ClubAuthName, CHR_ID_LENGTH);
    
    return ExecuteStoredProcedure("dbo.InsertGuildAuth", Ado);
}

int AdoManager::UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName)
{
    std::string TempAuthName(AuthName);
    sc::string::replaceForDb(TempAuthName);
    char ClubAuthName[CHR_ID_LENGTH] = {0};
    StringCchCopy(ClubAuthName, CHR_ID_LENGTH, TempAuthName.c_str());
    
    BYTE byteAuthIndex = static_cast<BYTE> (AuthIndex);

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT    (Ado);

    APPEND_IPARAM_INT    (Ado, "@GuNum",     ClubDbNum);
    APPEND_IPARAM_TINYINT(Ado, "@AuthIndex", byteAuthIndex);
    APPEND_IPARAM_INT    (Ado, "@AuthFlag",  AuthFlag);
    APPEND_IPARAM_VARCHAR(Ado, "@AuthName",  ClubAuthName, CHR_ID_LENGTH);

    return ExecuteStoredProcedure("dbo.UpdateGuildAuth", Ado);
}

int AdoManager::ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB)
{
    BYTE byteAuthIndexA = static_cast<BYTE> (AuthIndexA);
    BYTE byteAuthIndexB = static_cast<BYTE> (AuthIndexB);

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT    (Ado);

    APPEND_IPARAM_INT    (Ado, "@GuNum",  ClubDbNum);
    APPEND_IPARAM_TINYINT(Ado, "@IndexA", byteAuthIndexA);
    APPEND_IPARAM_TINYINT(Ado, "@IndexB", byteAuthIndexB);

    return ExecuteStoredProcedure("dbo.ChangeGuildAuthIndex", Ado);
}

int AdoManager::ChangeClubMemberGrade(DWORD ChaDbNum, int Grade)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT(Ado, "@ChaNum",  ChaDbNum);
    APPEND_IPARAM_INT(Ado, "@GuPosition", Grade);

    return ExecuteStoredProcedure("dbo.ChaInfoGuPositionUpdate", Ado);
}

int AdoManager::ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName)
{
    sc::db::AdoExt Ado(m_GameDBConnString);

    char ClubNickName[CHAR_SZNAME] = {0};
    StringCchCopy(ClubNickName, CHAR_SZNAME, NickName.c_str());

    APPEND_IPARAM_INT    (Ado, "@ChaNum",  ChaDbNum);    
    APPEND_IPARAM_VARCHAR(Ado, "@ChaGuName", ClubNickName, CHAR_SZNAME);

    return ExecuteStoredProcedure("dbo.sp_ChaInfoChaGuNameUpdate", Ado);
}

int AdoManager::ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag)
{
    sc::db::AdoExt Ado(m_GameDBConnString);

    APPEND_IPARAM_INT(Ado, "@GuNum",        ClubDbNum);
    APPEND_IPARAM_INT(Ado, "@GuPublicType", Flag);

    return ExecuteStoredProcedure("dbo.GuildPublicTypeUpdate", Ado);
}

int AdoManager::ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    
    APPEND_IPARAM_INT(Ado, "@GuNum", static_cast<int> (ClubDbNum));
    if (AutoKickOut)
        APPEND_IPARAM_SMALL(Ado, "@GuAutoKickUse", 1);
    else
        APPEND_IPARAM_SMALL(Ado, "@GuAutoKickUse", 0);

    APPEND_IPARAM_SMALL(Ado, "@GuAutoKickGrade", GradeIndex);
    APPEND_IPARAM_SMALL(Ado, "@GuAutoKickDay",   Day);

    return ExecuteStoredProcedure("dbo.GuildAutoKickUpdate", Ado);
}

int AdoManager::ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice)
{
    std::string StrNotice(Notice);
    sc::string::replaceForDb(StrNotice);

    char szNotice[EMCLUB_NOTICE_LEN] = {0};
    StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, StrNotice.c_str());

    // dbo.GuildNoticeNewbieUpdate
    // @GuNum int
    // @GuNoticeNewbie varchar(500)

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT    (Ado, "@GuNum", static_cast<int> (ClubDbNum));
    APPEND_IPARAM_VARCHAR(Ado, "@GuNoticeNewbie", szNotice, EMCLUB_NOTICE_LEN);
    
    return ExecuteStoredProcedure("dbo.GuildNoticeNewbieUpdate", Ado);
}

int AdoManager::ChaClubSecedeTimeReset(DWORD ChaDbNum)
{
    // dbo.ChaInfoChaGuSecedeReset
    // @ChaNum int
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@ChaNum", static_cast<int> (ChaDbNum));
    return ExecuteStoredProcedure("dbo.ChaInfoChaGuSecedeReset", Ado);
}

int AdoManager::SaveClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems )
{
	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		// Club locker Item 일 경우 아이템 테이블에 ChaNum 이 ClubNum 이다. 
		SINVENITEM_SAVE& sItem = vecItems[loop];
		int DbState = sItem.sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState )
		{
			ItemInsert( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], INSERT_CLUB_LOCKER_ITEM );
		}
		else if ( db::DB_UPDATE == DbState )
		{
			ItemUpdate( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], UPDATE_CLUB_LOCKER_ITEM );
		}
		else if ( db::DB_DELETE == DbState )
		{
			ItemDelete( vecItems[loop].sItemCustom.GetGuid(), db::DB_DELETE, DELETE_CLUB_LOCKER_ITEM );
		}
		else
		{
			sc::writeLogError( sc::string::format( "AdoManager::SaveClubLockerItems unknown type %1%", DbState ) );
		}
	}
	return sc::db::DB_OK;
}

int AdoManager::TransferClubLockerItems( DWORD ClubNum /*= 0*/ )
{
	// ClubNum 0 모든 클럽 락커 아이템 변환
	// ClubNum <> 0 특정 클럽 락커 아이템 변환
	if ( 0 == ClubNum )
	{
		sc::db::AdoExt AdoGame( m_GameDBConnString );
		if ( !AdoGame.Execute4Cmd( "dbo.sp_GetAllGuildItemTransferStatus", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		if ( AdoGame.GetEOF() )
			return sc::db::DB_OK;

		std::vector< DWORD > vecGuNum;
		do
		{
			int GuNum = 0;
			AdoGame.GetCollect( "GuNum", GuNum );
			vecGuNum.push_back( GuNum );
		} while ( AdoGame.Next() );

		size_t GuNumSize = vecGuNum.size();
		for ( size_t loop = 0; loop < GuNumSize; loop++ )
		{
			if ( 0 == vecGuNum[loop] )
			{
				continue;
			}

			TransferClubLockerItems( vecGuNum[loop] );
		}
	}
	else
	{
		int Status = 0;
		int MigrationState = 1;
		if ( sc::db::DB_ERROR == GetGuildItemTransferStatus( ClubNum, Status, MigrationState ) )
		{
			return sc::db::DB_ERROR;
		}

		if ( Status != 1 )
		{
			sc::db::AdoExt AdoGame( m_GameDBConnString );
			APPEND_IPARAM_INT( AdoGame, "@GuNum", ClubNum );

			int TransferResult = 1;
			if ( !AdoGame.ExecuteStoredProcedure( "dbo.sp_GetGuildInven" ) )
			{
				TransferResult = 2;
			}
			else
			{
				if ( AdoGame.GetEOF() )
				{
					TransferResult = 1;
				}
				else
				{
					std::vector< BYTE > vBuffer;
					AdoGame.GetChunk( "GuStorage", vBuffer );

					GLClubAgent sClub( NULL );
					if ( vBuffer.size() >= sc::db::DB_IMAGE_MIN_SIZE )
					{
						se::ByteStream ByteStream( vBuffer );
						if ( 0 == MigrationState )
						{
							sClub.SETSTORAGE_BYBUF_FOR_MIGRATION( ByteStream );
						}
						else
						{
							sClub.SETSTORAGE_BYBUF( ByteStream );
						}

						std::vector< SINVENITEM_SAVE > vecItems;
						sClub.GETSTORAGE_BYVECTOR( vecItems );

						size_t ItemSize = vecItems.size();
						for ( size_t loop = 0; loop < ItemSize; loop++ )
						{
							// Club locker Item 일 경우 아이템 테이블에 ChaNum 이 ClubNum 이다. 
							SINVENITEM_SAVE& sItem = vecItems[loop];
							sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
							sItem.sItemCustom.GenerateBasicStat( false );
							sItem.sItemCustom.GenerateLinkSkill();
							sItem.sItemCustom.GenerateAddOption();
							ItemInsert( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], INSERT_CLUB_LOCKER_ITEM_FOR_TRANSFER );
						}
					}
				}
			}

			sc::db::AdoExt Ado( m_GameDBConnString );
			APPEND_RPARAM_INT( Ado );
			APPEND_IPARAM_INT( Ado, "@GuNum", ClubNum );
			APPEND_IPARAM_INT( Ado, "@Inven", TransferResult );
			APPEND_OPARAM_INT( Ado, "@Return");
			if ( -1 == Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_SetGuildItemTransferStatus", "@Return" ) )
			{
				sc::writeLogError( sc::string::format( "AdoManager::TransferClubLockerItems sp_SetGuildItemTransferStatus error GuNum %1%", ClubNum ) );
			}

			// 클럽정보에 MigrationState 업데이트 필요, sp_SetGuildItemTransferStatus 여기에서 처리한다.
			// 클럽정보 테이블에는 업데이트 해놓고, 클럽변환테이블은 그대로 둔다.
		}
	}
	return sc::db::DB_OK;
}

int AdoManager::GetGuildItemTransferStatus( DWORD ClubNum, int& Result, int& MigrationState )
{
	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_IPARAM_INT( Ado, "@GuNum", ClubNum );
	if ( !Ado.Execute4Cmd( "dbo.sp_GetGuildItemTransferStatus", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( Ado.GetEOF() )
	{
		Ado.PRINT("AdoManager::GetGuildItemTransferStatus no data. RecordSet is Empty.");
		return sc::db::DB_ERROR;
	}

	do
	{
		Ado.GetCollect( "Inven", Result );
		Ado.GetCollect( "MigrationState", MigrationState );
	} while ( Ado.Next() );

	return sc::db::DB_OK;
}

int AdoManager::UpdateClubName( DWORD dwClub, const std::string& ClubName )
{
	sc::db::AdoExt Ado( m_GameDBConnString );
	Ado.AppendRParamInteger();

	Ado.AppendIParamInteger( "@nGuNum", static_cast< int >( dwClub ) );
	Ado.AppendIParamVarchar( "@GuName", ClubName.c_str(), ClubName.length() );

	Ado.AppendOParamInteger( "@nReturn" );

	return Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_UpdateGuildName", "@nReturn" );
}

} // namespace db