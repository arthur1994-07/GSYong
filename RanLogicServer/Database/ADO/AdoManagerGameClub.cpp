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

//! ���ο� Ŭ���� �����Ѵ�.
//! \param strClubName Ŭ���̸�
//! \param dwChaNum    Ŭ���� �����Ϸ��� ĳ���� ��ȣ (�� ĳ���Ͱ� Ŭ���� �����Ͱ� �ȴ�)	
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

//! Ŭ���� �����Ѵ�.
//! dwClub : Ŭ����ȣ
//! dwChaNum : Ŭ���� �����Ϸ��� ĳ���͹�ȣ(ĳ���� ��ȣ�� Ŭ�� �����Ͱ� �ƴϸ� ������ ���� �ʴ´�)
int AdoManager::DeleteClub(DWORD dwClub, DWORD dwChaNum)    
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamInteger("@GuNum",  static_cast<int> (dwClub));
    Ado.AppendIParamInteger("@ChaNum", static_cast<int> (dwChaNum));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_delete_guild", "@nReturn");
}

//! Ŭ����ũ�� �����Ѵ�.
//! dwClub : Ŭ����ȣ
//! dwRank : ��ŷ
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

//! Ŭ�� ��ü�ð��� �����Ѵ�.
//! \param dwClub Ŭ����ȣ
//! \param tDiss ��ü�ð�
int AdoManager::SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss)
{
    sc::writeLogError("Do not call this function. SetClubDissolutionTime");
    return sc::db::DB_ERROR;
}

//! Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
//! \param dwClub Ŭ����ȣ
//! \param tSec �ð�
int AdoManager::SetClubAllianceSec(DWORD dwClub, __time64_t tSec)
{
    sc::writeLogError("Do not call this function. SetClubAllianceSec");
    return sc::db::DB_ERROR;
}

//! Ŭ���� �����ػ� �ð��� ���Ѵ�.
//! \param dwClub Ŭ����ȣ
//! \param tDis �ػ�ð�
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

//! Ŭ���� ���� ������ �����´�.
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

//! Ŭ�� ��Ʋ������ �����Ѵ�,(Ŭ�� ��Ʋ�� ����Ǳ��� ����������� �ʿ��Ѱ�� ��Ʋ ������ ������ �ʿ��� )
//! \param dwClubP ��Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� ���� ��)
//! \param dwClubS ���� Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
//! \param nGuKillNum Ŭ�� ��Ʋ ų��
//! \return
int AdoManager::SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum)
{
    sc::writeLogError("Do not call this function. SaveClubBattle");
    return sc::db::DB_ERROR;
}

//! ���͹�Ʋ ������ �ʱ�ȭ ��Ű���.
//! \param dwClub Ŭ�� ��ȣ
int AdoManager::ReSetAllianceBattle(DWORD dwClub)
{
    sc::writeLogError("Do not call this function. ReSetAllianceBattle");
    return sc::db::DB_ERROR;
}

//! �ش� Ŭ���� ��Ʋ���� Ŭ�� ������ �����´�.
//! \param dwClub Ŭ����ȣ
//! \param &vMember Ŭ����Ʋ ����
int AdoManager::GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo)
{
    sc::writeLogError("Do not call this function. GetClubBattleInfo");
    return sc::db::DB_ERROR;
}

//! ���� ��������� ���� Ŭ�������͸� �����´�.
int AdoManager::GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB)
{
    sc::writeLogError("Do not call this function. GetClubRegion");
    return sc::db::DB_ERROR;
}

//! ������ �������� ������ Ŭ�� ������ ���Ѵ�.
//! \param dwRegionID : ���� ID
//! \param dwClub : Ŭ�� ID
//! \param fTax : ����
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

//! �ش� ������ ���� ���¸� �����Ѵ�.
//! \param dwRegionID ���� ID
//! \param dwClub Ŭ�� ID(�⺻ 0 �̴�)
int AdoManager::DelClubRegion(DWORD dwRegionID, DWORD dwClub)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@RegionID", static_cast<int> (dwRegionID));
    Ado.AppendIParamInteger("@GuNum", static_cast<int> (dwClub));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_delete_guild_region", "@nReturn");
}

//! �ش�Ŭ���� �����ݾ��� �����´�.(Storage)
//! \param dwClub Ŭ����ȣ
//! \param ���ϰ��� >= 0, 0 ���� ������ ERROR
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

//! �ش�Ŭ���� �����ݾ��� �����Ѵ�.(Storage)
//! \param dwClub : Ŭ����ȣ
//! \param llMoney : �����ݾ�(�ݾ��� >= 0)
int AdoManager::SetClubMoney(DWORD dwClub, LONGLONG llMoney)
{
    sc::writeLogError("Do not call this function. SetClubMoney");
    return sc::db::DB_ERROR;
}

//! �ش�Ŭ���� ���Աݾ��� �����Ѵ�.(Income money)
//! \param dwClub : Ŭ����ȣ
//! \param llMoney : ���Աݾ�(�ݾ��� >= 0)
int AdoManager::SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney)    
{
    sc::writeLogError("Do not call this function. SetClubIncomeMoney");
    return sc::db::DB_ERROR;
}

//! �ش�Ŭ���� ���Աݾ��� �����´�.(Income money)
//! \param dwClub : Ŭ����ȣ
//! \return ���ϰ��� >= 0, 0 ���� ������ ERROR
LONGLONG AdoManager::GetClubIncomeMoney(DWORD dwClub)
{
    sc::writeLogError("Do not call this function. GetClubIncomeMoney");
    return sc::db::DB_ERROR;
}

//! �ش�Ŭ���� Ŭ��â�� �о�´�.
//! \param dwClub : Ŭ����ȣ
//! \param ByteStream : ����
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

//! �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
//! \param dwClub : Ŭ����ȣ
//! \param pData : â����ġ
//! \param nSize : ���� ��üũ��
int AdoManager::WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize)
{
    sc::writeLogError("Do not call this function. WriteClubStorage");
    return sc::db::DB_ERROR;
}

//! �� Ŭ������ �����Ѵ�
//! \param dwClub : Ŭ����ȣ
//! \param dwChaNum : ĳ���͹�ȣ
//! \param dwSubMasterFlags : �÷���(0 ���� ������ �Ϲݱ����� �ȴ�)
int AdoManager::SetClubMasterFlags(
        DWORD dwClub,
        DWORD dwChaNum,
        DWORD dwSubMasterFlags)
{
    sc::writeLogError("Do not call this function. SetClubMasterFlags");
    return sc::db::DB_ERROR;
}

//! ���� Ŭ���� �Ἲ�Ѵ�.
//! \ db �� Ŭ���� ���Ἲ�� üũ���� �ʴ´�.
//! \ A �� B �� ������ ���¿��� 
//! \ B �� C �� ������ ����
//! \ C �� A �� ������ ���� ������ ü�� ������ �߻��Ѵ�.	
//! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
//! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
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

//! ���� Ŭ���� ��ü�Ѵ�.
//! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
//! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
int AdoManager::DelClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
    sc::writeLogError("Do not call this function. DelClubAlliance");
    return sc::db::DB_ERROR;
}

//! ���� Ŭ�� ����Ʈ�� �����´�.
//! <��Ŭ��,����Ŭ��>
int AdoManager::GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance)
{
    sc::writeLogError("Do not call this function. GetClubAlliance");
    return sc::db::DB_ERROR;
}

//! Ŭ���� ���������� �����Ѵ�.
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

//! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
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
		// Club locker Item �� ��� ������ ���̺� ChaNum �� ClubNum �̴�. 
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
	// ClubNum 0 ��� Ŭ�� ��Ŀ ������ ��ȯ
	// ClubNum <> 0 Ư�� Ŭ�� ��Ŀ ������ ��ȯ
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
							// Club locker Item �� ��� ������ ���̺� ChaNum �� ClubNum �̴�. 
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

			// Ŭ�������� MigrationState ������Ʈ �ʿ�, sp_SetGuildItemTransferStatus ���⿡�� ó���Ѵ�.
			// Ŭ������ ���̺��� ������Ʈ �س���, Ŭ����ȯ���̺��� �״�� �д�.
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