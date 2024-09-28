#include "../../pch.h"

#include "../../Club/GLClubAgent.h"
#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "./DbActionGameClub.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db 
{

ClubCreate::ClubCreate(
    DWORD ClientSlot,
    const std::string& ClubName,
    const std::string& ChaName,
    DWORD ChaDbNum,
    wb::EM_COUNTRY Country,
    bool bLobby)
    : DbAction(GAME_DB, ClientSlot)
    , m_ClubName(ClubName)
    , m_ChaDbNum(ChaDbNum)
    , m_Country(Country)
    , m_bLobby(bLobby)
    , m_ChaName(ChaName)
{
}

int ClubCreate::Execute(NetServer* pServer)
{
    int RetCode = m_pDbManager->CreateClub(m_ClubName, m_ChaDbNum, m_Country);
	
	GLMSG::SNET_CLUB_NEW_DB2AGT NetMsgDb2Agt(m_ClientSlot, m_ChaDbNum, 0, m_Country, m_ChaName.c_str(), m_bLobby);
	NetMsgDb2Agt.SetClubName(m_ClubName);

	switch (RetCode)
	{
	case -1:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_ALREADY;
		break;

	case -2:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_NAME;
		break;

	case CLUB_NULL:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_FAIL;
		break;

	default:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_OK;
		NetMsgDb2Agt.m_ClubDbNum = RetCode;
		break;
	};

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->InsertMsg(0, (char*) &NetMsgDb2Agt);
	return RetCode;
}

/**
* 클럽을 삭제한다.
* \param dwClub : 클럽번호
* \param dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
* \return 0 : 길드 삭제 실패
*         1 : 길드 삭제 성공
*/
ClubDelete::ClubDelete(
	DWORD dwClub, 
	DWORD dwChaNum)
	: m_dwClub( dwClub )
	, m_dwChaNum( dwChaNum )
{
}

int ClubDelete::Execute(NetServer* pServer)
{
    return m_pDbManager->DeleteClub(m_dwClub, m_dwChaNum);
}

/**
* 클럽랭크를 세팅한다.
* \param dwClub : 클럽번호
* \param dwRank : 랭킹
*/
ClubRankSet::ClubRankSet(
	DWORD dwClub,
	DWORD dwRank )
	: m_dwClub( dwClub )
	, m_dwRank( dwRank )
{   
}
 
int ClubRankSet::Execute(NetServer* pServer)
{
    return m_pDbManager->SetClubRank(m_dwClub, m_dwRank);
}

/**
* 클럽마크 이미지를 설정한다.
* \param dwClub : 클럽번호
* \param dwMarkVer : 마크버전
* \param pData : 데이터 포인터
* \param nSize : 데이터 size
*/
ClubMarkImageWrite::ClubMarkImageWrite(
	DWORD dwClub,
	DWORD dwMarkVer,
	BYTE* pData,
	int nSize) 
	: m_nSize(0)
	, m_dwMarkVer(0)
	, m_dwClub(0)
{
	if (pData != NULL)
	{
		int nMaxSize = (int) (sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

		m_dwClub = dwClub;
		m_dwMarkVer = dwMarkVer;
	    
		if ( nSize <= 0 || nSize > nMaxSize )
		{
			m_nSize = 0;
		}
		else
		{
			m_nSize = nSize;
			memcpy ( m_aryMark, pData, nSize );
		}
	}
}

ClubMarkImageWrite::~ClubMarkImageWrite()
{    
}

int ClubMarkImageWrite::Execute(NetServer* pServer)
{
    if (m_nSize > 0)
        return m_pDbManager->WriteClubMarkImage(m_dwClub, m_dwMarkVer, m_aryMark, m_nSize);
    else
        return sc::db::DB_ERROR;
}

/**
* 클럽 해체시간을 설정한다.
* dwClub : 클럽번호
* tDiss : 해체시간
*/
ClubDissolutionTimeSet::ClubDissolutionTimeSet(
	DWORD dwClub,
	__time64_t tDiss )
	: m_dwClub( dwClub )
{    
    m_tDiss = tDiss;
}

int ClubDissolutionTimeSet::Execute(
	NetServer* pServer )
{
    return m_pDbManager->SetClubDissolutionTime(m_dwClub, m_tDiss);
}

/**
* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
* \param dwClub 클럽번호
* \param tSec 시간
* \return 
*/
ClubAllianceTimeSet::ClubAllianceTimeSet(
	DWORD dwClub,
	__time64_t tSec )
	: m_dwClub( dwClub )
{
	m_tSec   = tSec;
}

int ClubAllianceTimeSet::Execute(
	NetServer* pServer )
{
	return m_pDbManager->SetClubAllianceSec( m_dwClub, m_tSec );
}

/**
* 클럽의 동맹해산 시간을 정한다.
* \param dwClub 클럽번호
* \param tDis 해산시간
* \return 
*/
ClubAllianceDisolveTimeSet::ClubAllianceDisolveTimeSet(
	DWORD dwClub,
	__time64_t tDis )
	: m_dwClub( dwClub )
{
	m_tDis   = tDis;
}
    
int ClubAllianceDisolveTimeSet::Execute(
	NetServer* pServer )
{
	return m_pDbManager->SetClubAllianceDis( m_dwClub, m_tDis );
}

ClubAuthorityTimeSet::ClubAuthorityTimeSet(
	DWORD dwClub,
	__time64_t tAuthority )
	: m_dwClub( dwClub )
{
	m_tAuthority   = tAuthority;
}
    
int ClubAuthorityTimeSet::Execute(
	NetServer* pServer )
{
	return m_pDbManager->SetClubAuthorityTime( m_dwClub, m_tAuthority );
}

ClubMemberAdd::ClubMemberAdd(
	DWORD dwClub,
	DWORD dwChaNum)
	: m_dwClub(dwClub)
	, m_dwChaNum(dwChaNum)
{    
}

ClubMemberAdd::~ClubMemberAdd() 
{
}

int ClubMemberAdd::Execute(NetServer* pServer)
{
    return m_pDbManager->AddClubMember(m_dwClub, m_dwChaNum);
}

ClubMemberDelete::ClubMemberDelete(
	DWORD dwChaNum)
	: m_dwChaNum(dwChaNum)
{
}

ClubMemberDelete::~ClubMemberDelete()
{
}

int ClubMemberDelete::Execute(NetServer* pServer)
{
    return m_pDbManager->DeleteClubMember(m_dwChaNum);
}

ClubMemberDeleteWithNoPenalty::ClubMemberDeleteWithNoPenalty( DWORD dwChaNum)
: m_dwChaNum( dwChaNum )
{
}

ClubMemberDeleteWithNoPenalty::~ClubMemberDeleteWithNoPenalty()
{
}

int ClubMemberDeleteWithNoPenalty::Execute( NetServer* pServer )
{
	return m_pDbManager->DeleteClubMemberWithNoPenalty( m_dwChaNum );
}

/**
* 해당클럽의 보유금액을 세팅한다. (Storage)
* \param dwClub : 클럽번호
* \param llMoney : 보유금액 (금액은 >= 0)
*/
ClubMoneySet::ClubMoneySet(
	DWORD dwClub,
	LONGLONG llMoney )
	: m_dwClub( dwClub )
	, m_llMoney( llMoney )
{	
}

int ClubMoneySet::Execute(
	NetServer* pServer )
{
	return m_pDbManager->SetClubMoney( m_dwClub, m_llMoney );
}

/**
* 해당클럽의 수입금액을 세팅한다. (Income money)
* \param dwClub : 클럽번호
* \param llMoney : 수입금액 (금액은 >= 0)
*/
ClubIncomeMoneySet::ClubIncomeMoneySet(
	DWORD dwClub,
	LONGLONG llMoney )
	: m_dwClub( dwClub )
	, m_llMoney( llMoney )
{	
}

int ClubIncomeMoneySet::Execute(
	NetServer* pServer )
{
	return m_pDbManager->SetClubIncomeMoney( m_dwClub, m_llMoney );
}

/**
* 해당클럽의 클럽창고를 저장한다.
* \param dwClub : 클럽번호
* \param pData : 창고위치
* \param nSize : 버퍼 전체크기
*/
ClubStorageWrite::ClubStorageWrite( DWORD dwClub, GLClubAgent* pClub )
	: m_dwClub( dwClub )
{
	if ( pClub )
	{
		pClub->GETSTORAGE_BYVECTOR( m_vecItems );
	}	
}

ClubStorageWrite::~ClubStorageWrite()
{
}
	
int ClubStorageWrite::Execute(NetServer* pServer)
{
	return m_pDbManager->SaveClubLockerItems( m_dwClub, m_vecItems );
}

ClubStorageGet::ClubStorageGet( DWORD _dwClientID, DWORD _dwCharID, DWORD _dwClubID )
	: m_dwClientID( _dwClientID )
	, m_dwCharID( _dwCharID )
	, m_dwClubID( _dwClubID )
{
}

int ClubStorageGet::Execute( NetServer* pServer )
{
	int nRetCode = 0;
	LONGLONG lnMoney = m_pDbManager->GetClubMoney( m_dwClubID );
	if ( lnMoney < 0 )
		return NET_OK;

	std::vector< SINVENITEM_SAVE > vecItems;
	nRetCode = m_pDbManager->GetClubLockerItems( m_dwClubID, vecItems );
	if (nRetCode == sc::db::DB_ERROR)
	{
		return NET_OK;
	}

	CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );
	if ( !pAgentServer )
	{
		return NET_ERROR;
	}

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		GLMSG::SNET_CLUB_STORAGE_GET_DB NetMsg;
		NetMsg.m_dwClubID = m_dwClubID;
		NetMsg.sItem = vecItems[loop];
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, NetMsg );
		pAgentServer->InsertMsg( DBACTION_CLIENT_ID, NET_MSG_GCTRL_CLUB_STORAGE_GET_DB, PackBuffer );
	}

	GLMSG::SNET_CLUB_STORAGE_GET_DB_END NetMsgEnd;
	NetMsgEnd.m_dwCharID = m_dwCharID;
	NetMsgEnd.m_dwClubID = m_dwClubID;
	NetMsgEnd.m_lnStorageMoney = lnMoney;

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, NetMsgEnd );
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, NET_MSG_GCTRL_CLUB_STORAGE_GET_DB_END, PackBuffer );
	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////
// 부 클럽장을 세팅한다
// dwClub : 클럽번호
// dwChaNum : 캐릭터번호
// dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
ClubMasterFlagsSet::ClubMasterFlagsSet(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags)
{
	m_dwClub           = dwClub;
	m_dwChaNum         = dwChaNum;
	m_dwSubMasterFlags = dwSubMasterFlags;
}

int ClubMasterFlagsSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubMasterFlags(m_dwClub, m_dwChaNum, m_dwSubMasterFlags);
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽을 결성한다.
// dwClubP : 주 클럽번호 (동맹 클럽의 주)
// dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
// * 주의 *
// db 는 클럽의 무결성을 체크하지 않는다.
// A 가 B 의 보조인 상태에서 
// B 가 C 의 보조로 들어가고
// C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.
ClubAllianceSet::ClubAllianceSet(DWORD dwClubP, DWORD dwClubS)
{
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
}

int ClubAllianceSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubAlliance(m_dwClubP, m_dwClubS);
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽을 해체한다.
// dwClubP : 주 클럽번호 (동맹 클럽의 주)
// dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
ClubAllianceDelete::ClubAllianceDelete(DWORD dwClubP, DWORD dwClubS)
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
}

int ClubAllianceDelete::Execute(NetServer* pServer)
{
	return m_pDbManager->DelClubAlliance(m_dwClubP, m_dwClubS);
}


///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 시작한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
ClubBattleSet::ClubBattleSet(DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, bool bAlliance )
	: m_dwClubP ( dwClubP )
	, m_dwClubS ( dwClubS )
	, m_dwEndTime ( dwEndTime )
	, m_bAlliance ( bAlliance )
{	
}

int ClubBattleSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubBattle(m_dwClubP, m_dwClubS, m_dwEndTime, (int)m_bAlliance );
}

///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 종료한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
// nFlag :	종료 Flag
// nGuKillNum : 킬수
// nGuDeathNum : 데스수
ClubBattleEnd::ClubBattleEnd(
	DWORD dwClubP, 
	DWORD dwClubS, 
	int nFlag, 
	int nGuKillNum, 
	int nGuDeathNum, 
	bool bAlliance )
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
	m_nFlag = nFlag;
	m_nGuKillNum = nGuKillNum;
	m_nGuDeathNum = nGuDeathNum;
	m_bAlliance = bAlliance;
}

int ClubBattleEnd::Execute(NetServer* pServer)
{
	return m_pDbManager->EndClubBattle(m_dwClubP, m_dwClubS, m_nFlag, m_nGuKillNum, 
									m_nGuDeathNum, m_bAlliance);
}


///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 종료한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
// nFlag :	종료 Flag
// nGuKillNum : 킬수
// nGuDeathNum : 데스수
ClubBattleSave::ClubBattleSave(
	DWORD dwClubP, 
	DWORD dwClubS, 
	int nGuKillNum, 
	int nGuDeathNum )
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
	m_nGuKillNum = nGuKillNum;
	m_nGuDeathNum = nGuDeathNum;
}

int ClubBattleSave::Execute(NetServer* pServer)
{
	return m_pDbManager->SaveClubBattle(m_dwClubP, m_dwClubS, m_nGuKillNum, m_nGuDeathNum);
}

AllianceBattleReSet:: AllianceBattleReSet( DWORD dwClub )
	: m_dwClub ( dwClub )
{
}

int AllianceBattleReSet::Execute(NetServer* pServer)
{
	return m_pDbManager->ReSetAllianceBattle( m_dwClub );
}

//! 클럽의 공지사항을 세팅한다.
ClubNoticeSet::ClubNoticeSet(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice)
    : m_ChaName(ChaName)
    , m_dwClub(dwClub)
    , m_Notice(ClubNotice)
{
	//memset(m_szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));	
	//m_dwClub = dwClub;
	//CString strTemp(szClubNotice);    
	//StringCchCopy(m_szNotice, EMCLUB_NOTICE_LEN+1, strTemp.GetString());
}

ClubNoticeSet::~ClubNoticeSet()
{
}

int ClubNoticeSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubNotice(m_ChaName, m_dwClub, m_Notice);
}

///////////////////////////////////////////////////////////////////////////
// 클럽의 선도클럽 인증 대리자를 세팅한다.
ClubDeputySet::ClubDeputySet(DWORD dwClub, DWORD dwDeputy)
{
	m_dwClub   = dwClub;
	m_dwDeputy = dwDeputy;
}

int ClubDeputySet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubDeputy(m_dwClub, m_dwDeputy);
}

ClubAuthoritySet::ClubAuthoritySet(DWORD dwClub, DWORD dwMasterID)
{
	m_dwClub   = dwClub;
	m_dwMasterID = dwMasterID;
}

int ClubAuthoritySet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetClubAuthority(m_dwClub, m_dwMasterID);
}

ClubRegionSet::ClubRegionSet(DWORD dwRegionID, DWORD dwClub, float fTax)
{
    m_dwRegionID = dwRegionID;
    m_dwClub = dwClub;
    m_fTax = fTax;
}

ClubRegionSet::~ClubRegionSet()
{
}

int ClubRegionSet::Execute(NetServer* pServer)
{
    return m_pDbManager->SetClubRegion(m_dwRegionID, m_dwClub, m_fTax);
}

ClubRegionDelete::ClubRegionDelete(DWORD dwRegionID, DWORD dwClub)
{
    m_dwRegionID = dwRegionID;
    m_dwClub     = dwClub;
}

ClubRegionDelete::~ClubRegionDelete()
{
}

int ClubRegionDelete::Execute(NetServer* pServer)
{
    return m_pDbManager->DelClubRegion(m_dwRegionID, m_dwClub);
}

InsertClubAuth::InsertClubAuth(
    DWORD ClubDbNum,
    size_t AuthIndex,
    DWORD AuthFlag,
    const std::string& AuthName)
    : m_ClubDbNum(ClubDbNum)
    , m_AuthIndex(AuthIndex)
    , m_AuthFlag(AuthFlag)
    , m_AuthName(AuthName)
{
}

InsertClubAuth::~InsertClubAuth()
{
}

int InsertClubAuth::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->InsertClubAuth(m_ClubDbNum, m_AuthIndex, m_AuthFlag, m_AuthName);
    }
    else
    {
        sc::writeLogError("InsertClubAuth m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

UpdateClubAuth::UpdateClubAuth(
    DWORD ClubDbNum,
    size_t AuthIndex,
    DWORD AuthFlag,
    const std::string& AuthName)
    : m_ClubDbNum(ClubDbNum)
    , m_AuthIndex(AuthIndex)
    , m_AuthFlag(AuthFlag)
    , m_AuthName(AuthName)
{
}

UpdateClubAuth::~UpdateClubAuth()
{
}

int UpdateClubAuth::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->UpdateClubAuth(m_ClubDbNum, m_AuthIndex, m_AuthFlag, m_AuthName);
    }
    else
    {
        sc::writeLogError("UpdateClubAuth m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

ChangeClubAuthIndex::ChangeClubAuthIndex(
    DWORD ClubDbNum,
    size_t IndexA,
    size_t IndexB)
    : m_ClubDbNum(ClubDbNum)
    , m_IndexA(IndexA)
    , m_IndexB(IndexB)
{
}

ChangeClubAuthIndex::~ChangeClubAuthIndex()
{
}

int ChangeClubAuthIndex::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChangeClubAuthIndex(m_ClubDbNum, m_IndexA, m_IndexB);
    }
    else
    {
        sc::writeLogError("ChangeClubAuthIndex m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

ChangeClubMemberGrade::ChangeClubMemberGrade(DWORD ChaDbNum, int Grade)
    : m_ChaDbNum(ChaDbNum)
    , m_Grade(Grade)
{
}
 
ChangeClubMemberGrade::~ChangeClubMemberGrade()
{
}

int ChangeClubMemberGrade::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChangeClubMemberGrade(m_ChaDbNum, m_Grade);
    }
    else
    {
        sc::writeLogError("ChangeClubMemberGrade m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

//! 클럽 닉네임 업데이트
ClubNickUpdate::ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName)
    : m_ChaDbNum(ChaDbNum)
    , m_NickName(NickName)
{
}

ClubNickUpdate::~ClubNickUpdate()
{
}

int ClubNickUpdate::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ClubNickUpdate(m_ChaDbNum, m_NickName);
    }
    else
    {
        sc::writeLogError("ClubNickUpdate m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

//! 클럽 공개 범위 설정
ClubPublicFlagUpdate::ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag)
    : m_ClubDbNum(ClubDbNum)
    , m_Flag(Flag)
{    
}

ClubPublicFlagUpdate::~ClubPublicFlagUpdate()
{
}

int ClubPublicFlagUpdate::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ClubPublicFlagUpdate(m_ClubDbNum, m_Flag);
    }
    else
    {
        sc::writeLogError("ClubPublicFlagUpdate m_pSubDbManager NULL");
        return sc::db::DB_ERROR;
    }
}

//! 클럽 자동 멤버 탈퇴 설정
ClubAutoKickUpdate::ClubAutoKickUpdate(
    DWORD ClubDbNum,
    size_t GradeIndex,
    WORD Day,
    bool AutoKickOut)
    : m_ClubDbNum(ClubDbNum)
    , m_GradeIndex(GradeIndex)
    , m_Day(Day)
    , m_AutoKickOut(AutoKickOut)
{
}

ClubAutoKickUpdate::~ClubAutoKickUpdate()
{
}

int ClubAutoKickUpdate::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() == db::ADO)
    {
        return m_pDbManager->ClubAutoKickUpdate(m_ClubDbNum, m_GradeIndex, m_Day, m_AutoKickOut);
    }
    else
    {
        sc::writeLogError("ClubAutoKickUpdate use ADO function");
        return sc::db::DB_ERROR;
    }
}

ClubNewbieNoticeUpdate::ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice)
    : m_ClubDbNum(ClubDbNum)
    , m_Notice(Notice)
{
}

ClubNewbieNoticeUpdate::~ClubNewbieNoticeUpdate()
{
}

int ClubNewbieNoticeUpdate::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() == db::ADO)
    {
        return m_pDbManager->ClubNewbieNoticeUpdate(m_ClubDbNum, m_Notice);
    }
    else
    {
        sc::writeLogError("ClubNewbieNoticeUpdate use ADO function");
        return sc::db::DB_ERROR;
    }
}

ChaClubSecedeTimeReset::ChaClubSecedeTimeReset(DWORD ChaDbNum)
    : m_ChaDbNum(ChaDbNum)
{
}

ChaClubSecedeTimeReset::~ChaClubSecedeTimeReset()
{
}

int ChaClubSecedeTimeReset::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() == db::ADO)
    {
        return m_pDbManager->ChaClubSecedeTimeReset(m_ChaDbNum);
    }
    else
    {
        sc::writeLogError("ChaClubSecedeTimeReset use ADO function");
        return sc::db::DB_ERROR;
    }
}

///////////////////////////////////////////////////////////////////////////////

ClubRename::ClubRename( DWORD ClubNum, DWORD ChaDbNum, const std::string& ClubName )
: m_ClubNum( ClubNum )
, m_ChaDbNum( ChaDbNum )
, m_ClubName( ClubName )
{
}

int ClubRename::Execute( NetServer* pServer )
{
	if ( db::ADO != m_pDbManager->GetConnectionType() )
	{
		sc::writeLogError( "ClubRename::Execute. Use ADO" );
		return NET_ERROR;
	}

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		return NET_ERROR;
	}

	int RetCode = m_pDbManager->UpdateClubName( m_ClubNum, m_ClubName );

	// field 서버 메시지에 넣어준다.
	GLMSG::SNET_CLUB_RENAME_DF NetMsg;
	NetMsg.ClubNum = m_ClubNum;
	NetMsg.ChaDbNum = m_ChaDbNum;
	NetMsg.nResult = RetCode;
	NetMsg.SetClubName( m_ClubName.c_str() );

	pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	return RetCode;
}

} // namespace db 
