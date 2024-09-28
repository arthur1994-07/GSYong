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
* Ŭ���� �����Ѵ�.
* \param dwClub : Ŭ����ȣ
* \param dwChaNum : Ŭ���� �����Ϸ��� ĳ���͹�ȣ(ĳ���� ��ȣ�� Ŭ�� �����Ͱ� �ƴϸ� ������ ���� �ʴ´�)
* \return 0 : ��� ���� ����
*         1 : ��� ���� ����
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
* Ŭ����ũ�� �����Ѵ�.
* \param dwClub : Ŭ����ȣ
* \param dwRank : ��ŷ
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
* Ŭ����ũ �̹����� �����Ѵ�.
* \param dwClub : Ŭ����ȣ
* \param dwMarkVer : ��ũ����
* \param pData : ������ ������
* \param nSize : ������ size
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
* Ŭ�� ��ü�ð��� �����Ѵ�.
* dwClub : Ŭ����ȣ
* tDiss : ��ü�ð�
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
* Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
* \param dwClub Ŭ����ȣ
* \param tSec �ð�
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
* Ŭ���� �����ػ� �ð��� ���Ѵ�.
* \param dwClub Ŭ����ȣ
* \param tDis �ػ�ð�
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
* �ش�Ŭ���� �����ݾ��� �����Ѵ�. (Storage)
* \param dwClub : Ŭ����ȣ
* \param llMoney : �����ݾ� (�ݾ��� >= 0)
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
* �ش�Ŭ���� ���Աݾ��� �����Ѵ�. (Income money)
* \param dwClub : Ŭ����ȣ
* \param llMoney : ���Աݾ� (�ݾ��� >= 0)
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
* �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
* \param dwClub : Ŭ����ȣ
* \param pData : â����ġ
* \param nSize : ���� ��üũ��
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
// �� Ŭ������ �����Ѵ�
// dwClub : Ŭ����ȣ
// dwChaNum : ĳ���͹�ȣ
// dwSubMasterFlags : �÷��� (0 ���� ������ �Ϲݱ����� �ȴ�)
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
// ���� Ŭ���� �Ἲ�Ѵ�.
// dwClubP : �� Ŭ����ȣ (���� Ŭ���� ��)
// dwClubS : ���� Ŭ����ȣ (�� Ŭ�� ������ ���� ���� Ŭ��)
// * ���� *
// db �� Ŭ���� ���Ἲ�� üũ���� �ʴ´�.
// A �� B �� ������ ���¿��� 
// B �� C �� ������ ����
// C �� A �� ������ ���� ������ ü�� ������ �߻��Ѵ�.
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
// ���� Ŭ���� ��ü�Ѵ�.
// dwClubP : �� Ŭ����ȣ (���� Ŭ���� ��)
// dwClubS : ���� Ŭ����ȣ (�� Ŭ�� ������ ���� ���� Ŭ��)
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
// Ŭ����Ʋ�� �����Ѵ�.
// dwClubP : �� Ŭ����ȣ (�ڱ�Ŭ��)
// dwClubS : ���� Ŭ����ȣ (���Ŭ��)
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
// Ŭ����Ʋ�� �����Ѵ�.
// dwClubP : �� Ŭ����ȣ (�ڱ�Ŭ��)
// dwClubS : ���� Ŭ����ȣ (���Ŭ��)
// nFlag :	���� Flag
// nGuKillNum : ų��
// nGuDeathNum : ������
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
// Ŭ����Ʋ�� �����Ѵ�.
// dwClubP : �� Ŭ����ȣ (�ڱ�Ŭ��)
// dwClubS : ���� Ŭ����ȣ (���Ŭ��)
// nFlag :	���� Flag
// nGuKillNum : ų��
// nGuDeathNum : ������
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

//! Ŭ���� ���������� �����Ѵ�.
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
// Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
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

//! Ŭ�� �г��� ������Ʈ
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

//! Ŭ�� ���� ���� ����
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

//! Ŭ�� �ڵ� ��� Ż�� ����
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

	// field ���� �޽����� �־��ش�.
	GLMSG::SNET_CLUB_RENAME_DF NetMsg;
	NetMsg.ClubNum = m_ClubNum;
	NetMsg.ChaDbNum = m_ChaDbNum;
	NetMsg.nResult = RetCode;
	NetMsg.SetClubName( m_ClubName.c_str() );

	pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	return RetCode;
}

} // namespace db 
