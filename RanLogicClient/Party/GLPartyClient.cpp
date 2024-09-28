#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "./GLPartyClient.h"
#include "../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../Ranlogicclient/GLGaeaClient.h"

#include "./GLPartyManagerClient.h"
#include "./GLPartyTenderManagerClient.h"
#include "../Chat/GLChatMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const bool GLPartySlotClient::isOneSelf(GLGaeaClient* const pGaeaClient) const
{
	return (pGaeaClient->GETMYGAEAID() == m_dwGaeaID);
}
const bool GLPartySlotClient::isOneSelf(const DWORD dwGaeaID) const
{
	if ( dwGaeaID == GAEAID_NULL )
		return false;

	return (dwGaeaID == m_dwGaeaID);
}

const GLPartySlotClient& GLPartySlotClient::operator =(const GLPartySlotClient& rhs)
{
	m_dwGaeaID = rhs.m_dwGaeaID;
	m_dwDbNum = rhs.m_dwDbNum;
	m_emClass = rhs.m_emClass;
	m_sMapID = rhs.m_sMapID;

	m_vPosition = rhs.m_vPosition;
	m_sHP = rhs.m_sHP;

	m_Level = rhs.m_Level;
	m_wSchool = rhs.m_wSchool;

	m_dwPartyMemberState = rhs.m_dwPartyMemberState;
	ResetMemberState(EMPARTY_MEMBER_STATE_SELECTED);	

	StringCchCopy ( m_szName, CHAR_SZNAME, rhs.m_szName );

	return *this;
}

const GLPartySlotClient& GLPartySlotClient::operator =(const GLPARTY_FNET& rhs)
{
	m_dwGaeaID = rhs.m_dwGaeaID;
	m_dwDbNum = rhs.m_dwDbNum;
	m_emClass = rhs.m_emClass;
	m_wSchool = rhs.m_wSchool;
	m_sMapID = rhs.m_sMapID;	
	m_Level = rhs.m_wLevel;

	if ( rhs.m_bConfront == true )
		SetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);	
	else
		ResetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);

	m_dwDbNum = rhs.m_dwDbNum;	

	StringCchCopy ( m_szName, CHAR_SZNAME, rhs.m_szName );

	return *this;
}

GLPartySlotClient::GLPartySlotClient(void)
: m_dwGaeaID(GAEAID_NULL)
, m_dwDbNum(DBNUM_NULL)
, m_emClass(GLCC_FIGHTER_M)
, m_wSchool(0)
, m_Level(0)
, m_vPosition(0.0f, 0.0f, 0.0f)
, m_dwPartyMemberState(EMPARTY_MEMBER_STATE_NULL)
{
	memset(m_szName, 0, sizeof(char) * (CHAR_SZNAME));
}

GLPartySlotClient::GLPartySlotClient(const GLPARTY_FNET& rhs)
: m_dwGaeaID(rhs.m_dwGaeaID)
, m_dwDbNum(rhs.m_dwDbNum)
, m_emClass(rhs.m_emClass)
, m_wSchool(rhs.m_wSchool)
, m_sMapID(rhs.m_sMapID)
, m_Level(rhs.m_wLevel)
, m_vPosition(0.0f, 0.0f, 0.0f)
, m_dwPartyMemberState(EMPARTY_MEMBER_STATE_NULL)
{
	StringCchCopy ( m_szName, CHAR_SZNAME, rhs.m_szName );
}

GLPartySlotClient::GLPartySlotClient(const GLPartySlotClient& rhs)
: m_dwGaeaID(rhs.m_dwGaeaID)
, m_dwDbNum(rhs.m_dwDbNum)
, m_emClass(rhs.m_emClass)
, m_wSchool(rhs.m_wSchool)
, m_sMapID(rhs.m_sMapID)
, m_Level(rhs.m_Level)
, m_sHP(rhs.m_sHP)
, m_vPosition(rhs.m_vPosition)
, m_dwPartyMemberState(rhs.m_dwPartyMemberState)
{	
	ResetMemberState(EMPARTY_MEMBER_STATE_SELECTED);	

	StringCchCopy ( m_szName, CHAR_SZNAME, rhs.m_szName );
}

void GLPartySlotClient::Reset(void)
{
	m_dwGaeaID = GAEAID_NULL;
	m_dwDbNum = DBNUM_NULL;

	m_dwPartyMemberState = EMPARTY_MEMBER_STATE_NULL;
}

const bool GLPartySlotClient::isValid(void) const
{
	if ( m_dwGaeaID != GAEAID_NULL )
		return true;
	return false;
}

const bool GLPartySlotClient::isValidMap(void) const
{
    return m_sMapID.isValid();
}

const bool GLPartySlotClient::isMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState) const
{
	return (m_dwPartyMemberState & DWORD(emPartyMemberState) ? true : false);
}

void GLPartySlotClient::SetMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState)
{
	m_dwPartyMemberState |= DWORD(emPartyMemberState);
}
void GLPartySlotClient::ResetMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState)
{
	m_dwPartyMemberState &= DWORD(~emPartyMemberState);
}
void GLPartySlotClient::InverseMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState)
{
	m_dwPartyMemberState ^= DWORD(emPartyMemberState);
}


void GLPartySlotClient::SetName(const char* const szName)
{
	if ( szName != NULL )
		StringCchCopy(m_szName, CHAR_SZNAME, szName);
}


GLPartyClient::GLPartyClient(GLGaeaClient* pGaeaClient, const DWORD dwPartyIndex /* = MASTER_PARTY*/)
 : m_pGaeaClient(pGaeaClient)
 , m_nPartyIndex(dwPartyIndex)
 , m_bQBoxEnable(true)
{
	m_mapParty.reserve(MAXPARTY);
}

GLPartyClient::~GLPartyClient(void)
{
}

void GLPartyClient::Lure ( DWORD dwGaeaID )
{
	if ( GetMemberByGaeaID(dwGaeaID) != NULL )
		return;
	
	SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);
	sOption.SetItemFilter( (EMPARTY_ITEM_FILTER_OPTION)RANPARAM::dwPARTY_GET_ITEM_Filter);
	
	// 토너먼트중 파티를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
		return;

	//	Note : 파티원 모집 요청.
	//
	GLMSG::SNET_PARTY_LURE NetMsg( dwGaeaID, sOption );
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyClient::LureName( const char* szName )
{
	DWORD dwDbnum = m_pGaeaClient->FindChaDbNumAtCache( std::string( szName ) );
	if( dwDbnum == m_pGaeaClient->GetCharacter()->GetCharID() )
		return;

	SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);
	sOption.SetItemFilter( (EMPARTY_ITEM_FILTER_OPTION)RANPARAM::dwPARTY_GET_ITEM_Filter);

	// 서버에 상대편과의 파티 요청

	GLMSG::SNET_PARTY_LURE_WITH_CHARNUM NetMsg( sOption );
	NetMsg.dwChaNum = dwDbnum;
	m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
}

void GLPartyClient::Secede(const DWORD dwIndex)
{
	const GLPartySlotClient* const pMember = GetMember(dwIndex);
	
	if ( pMember == NULL )
		return;
	
	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();	

    if ( pMember->isOneSelf(dwMyGaeaID) == false )  //추방인가? (자기자신이 아닌자를 탈퇴시킬 때)
    {
        //마스터만이 추방 가능
        if ( isMaster(dwMyGaeaID) == false )
            return;
    }
	//	Note : 탈퇴 ( 혹은 강퇴 ) 요청.
	//
	GLMSG::SNET_PARTY_SECEDE NetMsg( m_PartyID, pMember->m_dwGaeaID);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyClient::Dissolve(void)
{
	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();		
	if ( isMaster(dwMyGaeaID) == false )
        return;

	//	Note : 파티 해체 요청;
	//
	GLMSG::SNET_PARTY_DISSOLVE NetMsg(m_PartyID);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyClient::Authority(const DWORD dwIndex)
{
	const GLPartySlotClient* const pNewMaster = GetMember(dwIndex);
	if ( pNewMaster == NULL )
        return;

	const GLPartySlotClient* const pCurrentMaster = GetMember(MASTER_PARTY);
	if ( pCurrentMaster == NULL )
		return;

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	const DWORD dwCurrentMasterGaeaID = pCurrentMaster->m_dwGaeaID;	
	// dwCurrentMasterGaeaID == GAEAID_NULL일 수 있음;

	if ( (isMaster(dwMyGaeaID) == false) || (pNewMaster->isOneSelf(dwMyGaeaID) == true) )
		return; // 마스터가 아니거나 자기 자신에게 위임 하려 하면 실패;	

	//	Note : 위임 요청;
	//
	GLMSG::SNET_PARTY_AUTHORITY NetMsg(m_PartyID, pNewMaster->m_dwGaeaID, dwCurrentMasterGaeaID);

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyClient::SetPartyID(const GLPartyID& PartyID) 
{
	m_PartyID = PartyID;
	if ( m_PartyID.isValidParty() )
		m_pGaeaClient->GetCharacter()->m_bParty = TRUE;
}

void GLPartyClient::ResetParty ()
{	
	m_PartyID.Reset();
	m_sOption.Reset();
	m_mapParty.clear();
	m_bQBoxEnable = false;
	m_pGaeaClient->GetCharacter()->m_bParty = FALSE;

	GLPartyTenderManagerClient* pTenderManager =  m_pGaeaClient->GetPartyTenderManager();
	if ( pTenderManager )
	{
		pTenderManager->Reset();
		// 버튼이 활성화 되어있다면 꺼야한다;
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
			"-b", false);
	}
	
	if (m_pGaeaClient->GetCharacter()->GetConftingType() == EMCONFT_PARTY)
		m_pGaeaClient->GetCharacter()->ResetConfting();
}

void GLPartyClient::ResetCheckReady(void)
{
	for ( PARTYMAP_ITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		GLPartySlotClient &sMember = (*iter).second;
		sMember.ResetMemberState(EMPARTY_MEMBER_STATE_READY);
	}
}

const DWORD GLPartyClient::GetNMember(void) const
{
	return m_mapParty.size();
}

const DWORD GLPartyClient::GetMemberIndex(const DWORD dwGaeaID) const
{
	DWORD dwSlotIndex = 0;
	for ( PARTYMAP_CITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{				
		if ( iter->first == dwGaeaID )
		{
			return dwSlotIndex;
		}
		++dwSlotIndex;
	}

	return DEFSLOT_NULL;
}

const DWORD GLPartyClient::GetPartyIndex(void) const
{
	return m_nPartyIndex;
}

GLPartySlotClient* const GLPartyClient::GetMember(const DWORD dwIndex)
{
	if ( dwIndex >= m_mapParty.size() )
        return NULL;
	
	return &m_mapParty[dwIndex].second;
}

GLPartySlotClient* const GLPartyClient::GetMemberByGaeaID(const DWORD dwGaeaID)
{
	if ( dwGaeaID == GAEAID_NULL )
		return NULL;

	PARTYMAP_ITER iter = find_if(m_mapParty.begin(), m_mapParty.end(), FindMemberKey(dwGaeaID));
	if ( iter == m_mapParty.end() )
		return NULL;

	return &iter->second;
}

GLPartySlotClient* const GLPartyClient::GetMemberByChaDbNum( const DWORD& dwChaDbNum )
{
    DWORD nMEMBER_NUM = GetNMember();
    for( DWORD i = 0; i < nMEMBER_NUM; ++i )
    {
        GLPartySlotClient* const pPartySlot = GetMember( i );

        if( pPartySlot && pPartySlot->m_dwDbNum == dwChaDbNum )
            return pPartySlot;
    }

    return NULL;

}

const char* GLPartyClient::GetMemberChaName( const DWORD dwIndex )
{
    GLPartySlotClient* const pPartySlot = GetMember( dwIndex );
    if( pPartySlot )
        return pPartySlot->m_szName;

    return NULL;
}

const char* GLPartyClient::GetMemberChaNameByGaeaID( const DWORD dwGaeaID )
{
    GLPartySlotClient* const pPartySlot = GetMemberByGaeaID( dwGaeaID );
    if( pPartySlot )
        return pPartySlot->m_szName;

    return NULL;
}

const char* GLPartyClient::GetMemberChaNameByChaDbNum( const DWORD& dwChaDbNum )
{
    GLPartySlotClient* const pPartySlot = GetMemberByChaDbNum( dwChaDbNum );
    if( pPartySlot )
        return pPartySlot->m_szName;

    return NULL;
}

const bool GLPartyClient::isMaster(void) const
{
	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();

	if ( m_mapParty.empty() )
		return false;

	return m_mapParty[MASTER_PARTY].first == dwMyGaeaID;
}
const bool GLPartyClient::isMaster(const DWORD dwGaeaID) const
{
	if ( dwGaeaID == GAEAID_NULL )
		return false;

	if ( m_mapParty.empty() )
		return false;
	
	return m_mapParty[MASTER_PARTY].first == dwGaeaID;
}

const bool GLPartyClient::isMember(const DWORD dwGaeaID) const
{
	if ( dwGaeaID == GAEAID_NULL )
		return false;
	
	if ( m_mapParty.empty() )
		return false;

	PARTYMAP_CITER iter = find_if(m_mapParty.begin(), m_mapParty.end(), FindMemberKey(dwGaeaID));
	if ( iter == m_mapParty.end() )
		return false;

	return true;
}

const bool GLPartyClient::isMemberByDbNum(const DWORD dwDbNum) const
{
	if ( m_mapParty.empty() )
		return false;	

	for ( PARTYMAP_CITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		const GLPartySlotClient &sMember = (*iter).second;
		if ( sMember.m_dwDbNum == dwDbNum )
			return true;
	}

	return false;
}

const bool GLPartyClient::isValid(void)
{
	return m_PartyID.isValidParty();
}

const bool GLPartyClient::isMember(const char* const szCharName) const
{
	for ( PARTYMAP_CITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		const GLPartySlotClient &sMember = (*iter).second;
		if ( !_tcscmp( sMember.m_szName, szCharName ) )
			return true;
	}

	return false;
}

const bool GLPartyClient::removeMember(const DWORD dwGaeaID)
{
	PARTYMAP_ITER iter = find_if(m_mapParty.begin(), m_mapParty.end(), FindMemberKey(dwGaeaID));
	if ( iter == m_mapParty.end() )
		return false;

	m_mapParty.erase(iter);	
	return true;
}

LuaTable GLPartyClient::MemberToLuaTable( const GLPartySlotClient* const pMember )
{
	LuaTable tbPartyMem( GLWidgetScript::GetInstance().GetLuaState() );

	if( NULL == pMember )
		return tbPartyMem;

	int nPosX = 0, nPosY = 0;
	m_pGaeaClient->ConvertWorldToMap( pMember->m_sMapID.getBaseMapID(), pMember->m_vPosition, nPosX, nPosY );

	tbPartyMem.set( "dwGaeaID", pMember->m_dwGaeaID );
	tbPartyMem.set( "dwDbNum", pMember->m_dwDbNum );
	tbPartyMem.set( "emClass", pMember->m_emClass );
	tbPartyMem.set( "dwMapID", pMember->m_sMapID.getBaseMapID().dwID );
	tbPartyMem.set( "dwGaeaMapID", pMember->m_sMapID.getGaeaMapID().dwID );
	tbPartyMem.set( "nPosX", nPosX );
	tbPartyMem.set( "nPosY", nPosY );
	tbPartyMem.set( "dwMaxHP", pMember->m_sHP.dwMax );
	tbPartyMem.set( "dwNowHP", pMember->m_sHP.dwNow );
	tbPartyMem.set( "nLevel", pMember->m_Level );
	tbPartyMem.set( "wSchool", pMember->m_wSchool );
	tbPartyMem.set( "dwState", pMember->m_dwPartyMemberState );
	tbPartyMem.set( "strName", pMember->m_szName );

	LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );
	tbPos.set( "x", pMember->m_vPosition.x );
	tbPos.set( "y", pMember->m_vPosition.y );
	tbPos.set( "z", pMember->m_vPosition.z );
	tbPartyMem.set( "tbPosition", tbPos );

	return tbPartyMem;
}

LuaTable GLPartyClient::GetMemberForWidget( DWORD dwIndex )
{
	return MemberToLuaTable( GetMember( dwIndex ) );
}

void GLPartyClient::ChangeOption( WORD wItemOp, WORD wMoneyOp, WORD wItemFilter )
{
	SPartyOption sOption = m_sOption;
	sOption.SetItemOption( (EMPARTY_ITEM_OPTION) wItemOp );
	sOption.SetMoneyOption( (EMPARTY_MONEY_OPTION) wMoneyOp );
	sOption.SetItemFilter( (EMPARTY_ITEM_FILTER_OPTION) wItemFilter );

	m_pGaeaClient->GetPartyManager()->ChangeOption( sOption );
}

BOOL GLPartyClient::MsgPartyFNewUpdate(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_FNEW_UPDATE *pNetMsg = ( GLMSG::SNET_PARTY_FNEW_UPDATE * ) nmg;

	ResetParty ();
	SetPartyID (pNetMsg->PartyID);
	m_sOption = pNetMsg->sOption;
	RANPARAM::dwPARTY_GET_ITEM = pNetMsg->sOption.GetItemOption();
	RANPARAM::dwPARTY_GET_MONEY = pNetMsg->sOption.GetMoneyOption();
	RANPARAM::dwPARTY_GET_ITEM_Filter = pNetMsg->sOption.GetItemFilter();

	for ( DWORD _i = 0; _i < pNetMsg->nMember; ++_i )
	{
		GLPARTY_FNET& sPartyNet = pNetMsg->sPartyInfo[_i];

		GLPartySlotClient sPartyClient(sPartyNet);
		m_mapParty.push_back(PARTYPAIRCLIENT(sPartyClient.m_dwGaeaID, sPartyClient));
	}

	// 파티를 처음 생성했을 때 QBox옵션을 ON 상태로 바꾼다.
	m_bQBoxEnable = true;
	//!! SF_TODO
	//m_pInterface->QBoxButtonSetEnable(true);

	return TRUE;
}

BOOL GLPartyClient::MsgPartyFNew(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_FNEW *pNetMsg = ( GLMSG::SNET_PARTY_FNEW * ) nmg;

	ResetParty ();
	SetPartyID (pNetMsg->PartyID);
	m_sOption = pNetMsg->sOption;
	RANPARAM::dwPARTY_GET_ITEM = pNetMsg->sOption.GetItemOption();
	RANPARAM::dwPARTY_GET_MONEY = pNetMsg->sOption.GetMoneyOption();
	RANPARAM::dwPARTY_GET_ITEM_Filter = pNetMsg->sOption.GetItemFilter();

	for ( DWORD _i = 0; _i < pNetMsg->nMember; ++_i )
	{
		GLPARTY_FNET& sPartyNet = pNetMsg->sPartyInfo[_i];

		GLPartySlotClient sPartyClient(sPartyNet);
		m_mapParty.push_back(PARTYPAIRCLIENT(sPartyClient.m_dwGaeaID, sPartyClient));
	}

	// 파티를 처음 생성했을 때 QBox옵션을 ON 상태로 바꾼다.
	m_bQBoxEnable = true;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
		"-b", true );


	// 파티 , 생성 가입시 파티의 현재 조건을 채팅에 띄어주는 구문입니다.
	// http://172.16.2.87/redmine/issues/4094 [파티]파티분배 알림 수정 요청

	const DWORD dwItemOption(pNetMsg->sOption.GetItemOption());
	const DWORD dwMoneyOption(pNetMsg->sOption.GetMoneyOption());
	const DWORD dwItemFilter(pNetMsg->sOption.GetItemFilter());

	CString strItemOption, strItemFilter, strMoneyOption;
	switch ( dwItemOption )
	{
	case EMPARTY_ITEM_LEADER_SHARE:
	case EMPARTY_ITEM_DICE:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
			strItemFilter.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
			strItemOption.Format("%s(%s)", 
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption),
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
		}
		break;
	default:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
		}
	}

	strMoneyOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", dwMoneyOption));

	// 변경 메세지 출력;
	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
		sc::string::format(ID2GAMEINTEXT("PARTY_OPTION_CHANGE"), strItemOption.GetString(), strMoneyOption.GetString()));	

	return TRUE;
}

BOOL GLPartyClient::MsgPartyAdd(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_ADD *pNetMsg = ( GLMSG::SNET_PARTY_ADD * ) nmg;
	if ( m_PartyID != pNetMsg->PartyID )
		return FALSE;

	if ( isMember(pNetMsg->sPartyInfo.m_dwGaeaID) == true )
		return FALSE;

	m_mapParty.push_back(PARTYPAIRCLIENT(pNetMsg->sPartyInfo.m_dwGaeaID, pNetMsg->sPartyInfo));

	return TRUE;
}

BOOL GLPartyClient::MsgPartyDel(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_DEL *pNetMsg = ( GLMSG::SNET_PARTY_DEL * ) nmg;
	if ( m_PartyID != pNetMsg->PartyID )
		return FALSE;	

	const DWORD delMemberGaeaID(pNetMsg->delMemberGaeaID);
	if ( isMember(delMemberGaeaID) == false )
		return FALSE;

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	if ( delMemberGaeaID == dwMyGaeaID )
		ResetParty(); // 자기 자신이 탈퇴 할 때;
	else
		removeMember(delMemberGaeaID); // 다른 사람이 탈퇴 할 때;	
	
	return TRUE;
}

BOOL GLPartyClient::MsgPartyDissolve(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_DISSOLVE *pNetMsg = ( GLMSG::SNET_PARTY_DISSOLVE * ) nmg;

	ResetParty ();

	return TRUE;
}

BOOL GLPartyClient::MsgPartyAuthority( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_AUTHORITY *pNetMsg = (GLMSG::SNET_PARTY_AUTHORITY*)nmg;
	
	if ( m_PartyID != pNetMsg->PartyID )
		return FALSE;	

	PARTYMAP_ITER iterOldMaster = m_mapParty.begin();	
	if ( iterOldMaster == m_mapParty.end() )
		return FALSE;

	PARTYMAP_ITER iterNewMaster = find_if(m_mapParty.begin(), m_mapParty.end(), FindMemberKey(pNetMsg->dwNewMasterGaeaID));
	if ( iterNewMaster == m_mapParty.end() )
		return FALSE;

	const PARTYPAIRCLIENT  copyOldMaster = *iterOldMaster;
	*iterOldMaster = *iterNewMaster;
	*iterNewMaster = copyOldMaster;

	DWORD dwNewKey = iterOldMaster->first;
	DWORD dwOldKey = iterNewMaster->first;


	// 파티장이 바뀌었을때 QBox옵션을 ON 상태로 바꾼다.
	m_bQBoxEnable = true;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
		"-b", true );

	// 이렇게 바꾼이유 Agent removePartyMember함수를 봐라.
	if( dwNewKey != dwOldKey )
	{

	m_pGaeaClient->PrintMsgText(
        NS_UITEXTCOLOR::LIGHTSKYBLUE,
        sc::string::format(
            ID2GAMEINTEXT("PARTY_AUTHORITY_CHANGE"),
            iterNewMaster->second.m_szName,
            iterOldMaster->second.m_szName));
	}
	else
	{

	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::LIGHTSKYBLUE,
		sc::string::format(
		ID2GAMEINTEXT("PARTY_AUTHORITY_CHANGE"),
		pNetMsg->szCharName,
		iterOldMaster->second.m_szName ));
	}

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMoveMap(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_MOVEMAP *pNetMsg = ( GLMSG::SNET_PARTY_MBR_MOVEMAP * ) nmg;

	GLPartySlotClient* pMember = GetMemberByGaeaID(pNetMsg->dwGaeaID);
	if ( pMember == NULL )
		return FALSE;

	pMember->m_sMapID = pNetMsg->mapID;

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrDetail(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_DETAIL *pNetMsg = ( GLMSG::SNET_PARTY_MBR_DETAIL * ) nmg;

	GLPartySlotClient* pMember = GetMemberByGaeaID ( pNetMsg->dwGaeaID );
	if ( !pMember )
		return FALSE;
	
	pMember->m_vPosition = pNetMsg->vPos;
	pMember->m_sHP = pNetMsg->sHP;
	pMember->m_Level = pNetMsg->Level;	

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrDetailParty(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_DETAIL_PARTY *pNetMsg = ( GLMSG::SNET_PARTY_MBR_DETAIL_PARTY * ) nmg;

	const DWORD nMember = pNetMsg->nMember;
	if ( nMember > MAXPARTY )
		return FALSE;

	for ( DWORD _i = nMember; _i; --_i )
	{
		const GLMSG::SNET_PARTY_MBR_DETAIL_PARTY::MEMBER_DETAIL_INFORMATION& MemberDetail =
			pNetMsg->MemberDetail[_i - 1];
		GLPartySlotClient* const pMember = GetMemberByGaeaID(MemberDetail.dwGaeaID);
		if ( pMember == NULL )
			return FALSE;

		pMember->m_vPosition = MemberDetail.vPos;
		pMember->m_sHP = MemberDetail.sHP;
		pMember->m_Level = MemberDetail.Level;
	}

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrLevelUp(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_LEVEL_UP* pNetMsg = (GLMSG::SNET_PARTY_MBR_LEVEL_UP*) nmg;
	GLPartySlotClient* pMember = GetMemberByGaeaID ( pNetMsg->dwGaeaID );
	if ( !pMember )	
		return FALSE;

	pMember->m_Level = pNetMsg->Level;

	return TRUE;
}


BOOL GLPartyClient::MsgPartyLureFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_LURE_FB *pNetMsg = ( GLMSG::SNET_PARTY_LURE_FB * ) nmg;

	std::string strNAME;

    std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( pNetMsg->dwGaeaID );
    if ( pChar )
        strNAME = pChar->GetName();

    CString strText;
	switch ( pNetMsg->emAnswer )
	{
	
	case EMPARTY_LURE_REFUSE:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE"), strNAME.c_str()));
		break;
	
	case EMPARTY_LURE_FULL:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format( ID2GAMEINTEXT("PARTY_LURE_FULL"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHER:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format( ID2GAMEINTEXT("PARTY_LURE_OTHER"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_OTHERSCHOOL") );
		break;

	case EMPARTY_LURE_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CLUBBATTLE") );
		break;

	case EMPARTY_LURE_CTF:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CTF") );
		break;
	case EMPARTY_LURE_TIME_OVER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_TIME_OVER") );
		break;
	};

	return TRUE;
}

BOOL GLPartyClient::MsgPartyLureFbWithCharName( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_PARTY_LURE_FB_WITH_CHARNAME *pNetMsg = ( GLMSG::SNET_PARTY_LURE_FB_WITH_CHARNAME* ) nmg;

    std::string strNAME = stringformat( "%1%", pNetMsg->szCharName );

    CString strText;
    switch ( pNetMsg->emAnswer )
    {
    case EMPARTY_LURE_REFUSE:		
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE"), strNAME.c_str()));
        break;

    case EMPARTY_LURE_FULL:		
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format( ID2GAMEINTEXT("PARTY_LURE_FULL"), strNAME.c_str()));
        break;

    case EMPARTY_LURE_OTHER:		
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format( ID2GAMEINTEXT("PARTY_LURE_OTHER"), strNAME.c_str()));
        break;

    case EMPARTY_LURE_OTHERSCHOOL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_OTHERSCHOOL") );
        break;

    case EMPARTY_LURE_CLUBBATTLE:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CLUBBATTLE") );
        break;

    case EMPARTY_LURE_CTF:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CTF") );
        break;
    };

    return TRUE;
}

BOOL GLPartyClient::MsgPartyLinkJoinFB( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_LINK_JOIN_FB *pNetMsg = ( GLMSG::SNET_PARTY_LINK_JOIN_FB * ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPARTY_LINK_JOIN_FB_OK:	//	이런 메세지는 안온다.
		break;
	case EMPARTY_LINK_JOIN_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_FAIL") );
		break;
	case EMPARTY_LINK_JOIN_FB_FULL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_FULL") );
		break;
	case EMPARTY_LINK_JOIN_FB_NOCHAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOCHAR") );
		break;
	case EMPARTY_LINK_JOIN_FB_REJOIN:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_REJOIN") );
		break;
	case EMPARTY_LINK_JOIN_FB_NOMASTER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOMASTER") );
		break;
	case EMPARTY_LINK_JOIN_FB_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_OTHERSCHOOL") );
		break;
	case EMPARTY_LINK_JOIN_FB_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CLUBBATTLE") );
		break;
	case EMPARTY_LINK_JOIN_FB_CHANNEL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CHANNEL") );
		break;
	}

	return TRUE;
}

BOOL GLPartyClient::MsgPartyLinkJoinANS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_LINK_JOIN_ANS *pNetMsg = ( GLMSG::SNET_PARTY_LINK_JOIN_ANS * ) nmg;

	GLMSG::NET_CHAT_LINK_FB	NetChatFB;

	//	링크 정보를 셋팅한다.
	{
		CString strChatMsg, strTemp;

		// 링크 정보를 찾는다.
		SLINK_TYPE sLinkType0, sLinkType1;
		sLinkType0.wMType = PARTY_LINK;
		sLinkType0.wSType = PARTY_LINK_ACCEPT;
		sLinkType1.wMType = PARTY_LINK;
		sLinkType1.wSType = PARTY_LINK_REFUSE;

		strChatMsg += COMMENT::CHARCLASS[ CharClassToIndex( pNetMsg->emClass )].c_str();		
	
		//strTemp.Format( "%s%d%d%s", strLINK_LEFT_SIMBOL, sLinkType0.wMType, sLinkType0.wSType, strLINK_RIGHT_SIMBOL );			
		//strChatMsg += strTemp;
		//
		//strTemp.Format( "%s%d%d%s", strLINK_LEFT_SIMBOL, sLinkType1.wMType, sLinkType1.wSType, strLINK_RIGHT_SIMBOL );	
		//strChatMsg += strTemp;

		int nStart = sc::string::ansiToUnicode(strChatMsg.GetString()).length();
		strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 2 ), strLINK_RIGHT_SIMBOL );
		strChatMsg += strTemp.GetString();
		int nEnd = nStart + sc::string::ansiToUnicode(strTemp.GetString()).length();

		NetChatFB.ADD( &pNetMsg->dwGaeaID, sizeof(DWORD) * 3);
		NetChatFB.sLinkDataRecv[0].sLinkType = sLinkType1;
		NetChatFB.sLinkDataRecv[0].sLinkPos.nStart = nStart;
		NetChatFB.sLinkDataRecv[0].sLinkPos.nEnd = nEnd;
		NetChatFB.sLinkDataRecv[0].dwSize = sizeof(DWORD) * 3;	

		nStart = nEnd;
		strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 3 ), strLINK_RIGHT_SIMBOL );
		strChatMsg += strTemp.GetString();
		nEnd = nStart + sc::string::ansiToUnicode(strTemp.GetString()).length();

		NetChatFB.ADD( &pNetMsg->dwGaeaID, sizeof(DWORD));
		NetChatFB.sLinkDataRecv[1].sLinkType = sLinkType1;
		NetChatFB.sLinkDataRecv[1].sLinkPos.nStart = nStart;
		NetChatFB.sLinkDataRecv[1].sLinkPos.nEnd = nEnd;
		NetChatFB.sLinkDataRecv[1].dwSize = sizeof(DWORD);
		
		NetChatFB.emType = CHAT_TYPE_PRIVATE;
		NetChatFB.SetName(pNetMsg->szName);
		NetChatFB.SetChatMsg(strChatMsg.GetString());

		GLChatMan::GetInstance().MsgChatLinkMessage( &NetChatFB );
	}

	return TRUE;
}

BOOL GLPartyClient::MsgPartyLinkLureFB( NET_MSG_GENERIC* nmg )
{

	GLMSG::SNET_PARTY_LINK_LURE_FB *pNetMsg = ( GLMSG::SNET_PARTY_LINK_LURE_FB * ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPARTY_LINK_LURE_FB_OK:	//	이런 메세지는 안온다.
		break;
	case EMPARTY_LINK_LURE_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FAIL") );
		break;
	case EMPARTY_LINK_LURE_FB_FULL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_FULL") );
		break;
	case EMPARTY_LINK_LURE_FB_NOCHAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_NOCHAR") );
		break;
	case EMPARTY_LINK_LURE_FB_REJOIN:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_REJOIN") );
		break;
	case EMPARTY_LINK_LURE_FB_NOMASTER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_NOMASTER") );
		break;
	case EMPARTY_LINK_LURE_FB_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_OTHERSCHOOL") );
		break;
	case EMPARTY_LINK_LURE_FB_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_CLUBBATTLE") );
		break;
	case EMPARTY_LINK_LURE_FB_CHANNEL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_CHANNEL") );
		break;
	}

	return TRUE;
}

BOOL GLPartyClient::MsgPartyLinkRefuseFB( NET_MSG_GENERIC* nmg )
{

	GLMSG::SNET_PARTY_LINK_REFUSE_FB *pNetMsg = ( GLMSG::SNET_PARTY_LINK_REFUSE_FB * ) nmg;

	GLMSG::NET_CHAT_LINK_FB	NetChatFB;

	CString strChatMsg = ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT",4 );

	NetChatFB.SetData(CHAT_TYPE_PRIVATE, pNetMsg->szName, strChatMsg.GetString());

	GLChatMan::GetInstance().MsgChatLinkMessage( &NetChatFB );

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrUpdate(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_UPDATE *pNetMsg = (GLMSG::SNET_PARTY_MBR_UPDATE *) nmg;

	GLPartySlotClient* pMember = GetMemberByGaeaID ( pNetMsg->dwGaeaID );
	if ( pMember == NULL )
		return FALSE;

	if( pMember->m_vPosition == pNetMsg->vPos &&
		pMember->m_sHP == pNetMsg->sHP &&
		pMember->m_Level == pNetMsg->wLevel )
		return FALSE;
	
	pMember->m_vPosition = pNetMsg->vPos;
	pMember->m_sHP = pNetMsg->sHP;
	pMember->m_Level = pNetMsg->wLevel;

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrItemPickUp(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD *pNetMsg = (GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD *) nmg;

	GLPartySlotClient *pMember = GetMemberByGaeaID ( pNetMsg->dwGaeaID );
	if (!pMember)
        return FALSE;

    SITEM *pITEM = GLogicData::GetInstance().GetItem ( pNetMsg->sNID_ITEM );
    if (!pITEM)
        return FALSE;

    m_pGaeaClient->PrintMsgText(
        NS_UITEXTCOLOR::ITEMMONEY,
        sc::string::format( ID2GAMEINTEXT("PARTY_MBR_PICKUP_BRD"), pMember->m_szName, pITEM->GetName()));

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrItemPickUpLink(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD *pNetMsg = (GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD *) nmg;

	GLPartySlotClient *pMember = GetMemberByGaeaID ( pNetMsg->dwGaeaID );
	if (!pMember)
        return FALSE;

	//!! SF_TODO
	//m_pInterface->DisplayChatPartyPickupLinkMessage( pMember->m_szName, pNetMsg->sItem );
	GLChatMan::GetInstance().MsgChatItemLinkMessage( pMember->m_szName, pNetMsg->sItem );

	return TRUE;
}

BOOL GLPartyClient::MsgPartyMbrRename(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MEMBER_RENAME* pNetMsg = ( GLMSG::SNET_PARTY_MEMBER_RENAME*) nmg;

	GLPartySlotClient* pMember = GetMemberByGaeaID(pNetMsg->dwGaeaID);
	if (pMember)
    {        
	    pMember->SetName(pNetMsg->szName);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}

HRESULT GLPartyClient::MsgProcess(NET_MSG_GENERIC* nmg)
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_PARTY_FNEW_UPDATE:
		MsgPartyFNewUpdate( nmg );
		break;
	case NET_MSG_GCTRL_PARTY_FNEW:
		MsgPartyFNew ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_LURE_FB:
		MsgPartyLureFb ( nmg );
		break;

    case NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME:
        MsgPartyLureFbWithCharName( nmg );
        break;

	case NET_MSG_GCTRL_PARTY_ADD:
		MsgPartyAdd ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_DEL:
		MsgPartyDel ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_AUTHORITY:
		MsgPartyAuthority( nmg );
		break;
	
	case NET_MSG_GCTRL_PARTY_DISSOLVE:
		MsgPartyDissolve ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
		MsgPartyMoveMap ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_MBR_DETAIL:
		MsgPartyMbrDetail ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_MBR_LEVELUP:
		MsgPartyMbrLevelUp(nmg);
		break;	

	case NET_MSG_GCTRL_PARTY_MBR_UPDATE:
		MsgPartyMbrUpdate ( nmg );
		break;
	
	case NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD:
		MsgPartyMbrItemPickUp ( nmg );
		break;

    case NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD:
        MsgPartyMbrItemPickUpLink ( nmg );
        break;

	case NET_MSG_GCTRL_PARTY_MBR_RENAME:
		MsgPartyMbrRename ( nmg );
		break;
	case NET_MSG_GCTRL_PARTY_LINK_JOIN_FB:
		MsgPartyLinkJoinFB ( nmg );		
		break;
	case NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS:
		MsgPartyLinkJoinANS ( nmg );		
		break;
	case NET_MSG_GCTRL_PARTY_LINK_LURE_FB:
		MsgPartyLinkLureFB ( nmg );
		break;
	case NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB:
		MsgPartyLinkRefuseFB ( nmg );
		break;
	};

	return S_OK;
}

BOOL GLPartyClient::SetConfront(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_CONFRONTPTY_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_START2_CLT *) nmg;
	
	for ( PARTYMAP_ITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		GLPartySlotClient &sMember = (*iter).second;
		sMember.ResetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);
	}

	for ( DWORD _i = 0; _i < MAXPARTY; ++_i )
	{
		const DWORD dwMemberGaeaID = pNetMsg->dwCONFRONT_MY_MEMBER[_i];
		GLPartySlotClient* const pMember = GetMemberByGaeaID(dwMemberGaeaID);
		if ( pMember == NULL )
			continue;

		pMember->SetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);
	}

	return TRUE;
}

const DWORD GLPartyClient::GetNConfrontMember(void) const
{
	DWORD nMember(0);
	for ( PARTYMAP_CITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		const GLPartySlotClient& sMember = (*iter).second;
		if ( sMember.isMemberState(EMPARTY_MEMBER_STATE_CONFRONT) == true )
			++nMember;;
	}

	return nMember;
}

void GLPartyClient::ReSetConfront(void)
{
	for ( PARTYMAP_ITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		GLPartySlotClient &sMember = (*iter).second;
		sMember.ResetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);
	}
}

void GLPartyClient::ResetConfrontMember ( DWORD dwGaeaID )
{
	GLPartySlotClient* pMember = GetMemberByGaeaID(dwGaeaID);
	if ( pMember == NULL )
		return;
	
	pMember->ResetMemberState(EMPARTY_MEMBER_STATE_CONFRONT);
}

const bool GLPartyClient::IsLevelPartyMember(const WORD wMin, const WORD wMax)
{
	for ( PARTYMAP_CITER iter = m_mapParty.begin(); iter != m_mapParty.end(); ++iter )
	{
		const GLPartySlotClient& sMember = (*iter).second;

		if (WORD(sMember.m_Level) < wMin)
		{
			return false;
		}

		if (WORD(sMember.m_Level) > wMax)
		{
			return false;
		}
	}

	return true;
}

GLExpeditionClient::GLExpeditionClient(void)
: m_bMasterChatting(false)
{
}

GLExpeditionClient::~GLExpeditionClient(void)
{
}

const bool GLExpeditionClient::isMasterChatting(void) const
{
	return m_bMasterChatting;
}
void GLExpeditionClient::SetMasterChatting(const bool bMasterChatting)
{
	m_bMasterChatting = bMasterChatting;
}

void GLPartyClient::InvitePartyReq()
{
	DWORD _GaeaID = m_pGaeaClient->GetCharacter()->GetTarPtoP();
	Lure(_GaeaID);
}
