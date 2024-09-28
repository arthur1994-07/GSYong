#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../Ranlogicclient/GLGaeaClient.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/RANPARAM.h"
#include "../Land/GLLandManClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "../GLGaeaClient.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "GLPartyTenderManagerClient.h"
#include "GLPartyManagerClient.h"

GLPartyManagerClient::GLPartyManagerClient(GLGaeaClient* const pGaeaClient)
: m_pGaeaClient(pGaeaClient)
, m_pExpedition(new GLExpeditionClient)
{	
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		m_ptrPartyClient[_i - 1] = GLPartyClientPtr(new GLPartyClient(pGaeaClient, _i - 1));
}

GLPartyManagerClient::~GLPartyManagerClient(void)
{	
	delete m_pExpedition;
}

void GLPartyManagerClient::Reset(void)
{
	m_PartyID.Reset();
	m_pExpedition->Reset();
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )			
		m_ptrPartyClient[_i - 1]->ResetParty();
}

void GLPartyManagerClient::ResetCheckReady(void)
{
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )			
		m_ptrPartyClient[_i - 1]->ResetCheckReady();
}

void GLPartyManagerClient::Lure(const DWORD dwGaeaID)
{
	if ( m_PartyID.isValidExpedition() == true )
	{
		if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
			return;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			if ( m_ptrPartyClient[_i - 1]->isMember(dwGaeaID) == true )
				return;
		}		
	}
	else if ( m_PartyID.isValidParty() == true )
	{
		if ( m_ptrPartyClient[0]->isMember(dwGaeaID) == true )
			return;
	}

	// ��ʸ�Ʈ�� ��Ƽ�� �Ҽ��� ����.
	// TOURNAMENT_UNIQ_DISABLE
	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
		return;


	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == true )
	{
		GLMSG::SNET_PARTY_LURE NetMsg(dwGaeaID, m_ptrPartyClient[0]->GetOption());
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	else
	{
		SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);
		GLMSG::SNET_PARTY_LURE NetMsg(dwGaeaID, sOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}	
}

void GLPartyManagerClient::LureByDbNum(const DWORD dwDbNum)
{	
	if ( m_PartyID.isValidExpedition() == true )
	{
		if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
			return;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			if ( m_ptrPartyClient[_i - 1]->isMemberByDbNum(dwDbNum) == true )
			{	
				GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(m_PartyID.GetPartyID());
				ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_REJOIN;
				ErrorMsg.addCharName(m_ptrPartyClient[_i - 1]->GetMemberChaNameByChaDbNum(dwDbNum) );
				m_pGaeaClient->NETSENDTOMYSELF(&ErrorMsg);
				return;
			}
		}		
	}
	else if ( m_PartyID.isValidParty() == true )
	{
		if ( m_ptrPartyClient[0]->isMemberByDbNum(dwDbNum) == true )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(m_PartyID.GetPartyID());
			ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_REJOIN;
			ErrorMsg.addCharName(m_ptrPartyClient[0]->GetMemberChaNameByChaDbNum(dwDbNum) );
			m_pGaeaClient->NETSENDTOMYSELF(&ErrorMsg);
			return;
		}
	}

	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == true )
	{
		GLMSG::SNET_PARTY_LURE_WITH_CHARNUM NetMsg(m_ptrPartyClient[0]->GetOption());
		NetMsg.dwChaNum = dwDbNum;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	else
	{
		SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);
		GLMSG::SNET_PARTY_LURE_WITH_CHARNUM NetMsg(sOption);
		NetMsg.dwChaNum = dwDbNum;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
}

void GLPartyManagerClient::Secede(const DWORD dwGaeaID, BYTE eType)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;
    const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
    if ( dwMyGaeaID != dwGaeaID )   //�߹��϶� (�ڱ��ڽ��� �ƴѻ���� Ż���ų ��)
    {
        //������Ȯ��(���������̰ų� ��Ƽ���̾����)
        const bool bExpedition = isExpedition();
        const bool bRequesterIsMaster=  bExpedition ? isExpeditionMaster() : isPartyMaster();   //�������ΰ�?
        
        if ( bRequesterIsMaster == false )  //������ ���ٸ�
            return;
	}
    //�������� ������ ��Ƽ����ŭ ��Ƽ��� 1ȸ ���� ����
    const DWORD dwLoopCnt = m_PartyID.isValidExpedition() ? MAX_PARTYGROUP : 1;

    for ( DWORD _i = dwLoopCnt; _i; --_i )
    {
        GLPartyClient* const pParty = m_ptrPartyClient[_i - 1].get();
        if ( pParty->isMember(dwGaeaID) == true )   //�ڱⰡ ���� ��Ƽ���
        {
            //Ż�� ��û
			GLMSG::SNET_PARTY_SECEDE NetMsg(pParty->GetPartyID(), dwGaeaID, static_cast<EMPARTY_ACTION_TYPE>(eType) );
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
            return;
        }
    }
}

void GLPartyManagerClient::Dissolve(void)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( isMasterAuthority() == false )
		return;

	GLMSG::SNET_PARTY_DISSOLVE NetMsg(m_PartyID);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}
void GLPartyManagerClient::Authority(const DWORD dwNewMasterGaeaID, const DWORD dwPartySlot, const bool bExpedition)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( dwPartySlot > (MAX_PARTYGROUP - 1) )
		return;

	if ( isMasterAuthority() == false )
		return;

	GLPartyClient* const pParty = m_ptrPartyClient[dwPartySlot].get();
	GLPartyClient* const pMasterParty = m_ptrPartyClient[MASTER_PARTY].get();
	if ( pParty->GetPartyID().isValidExpedition() && (bExpedition == false) )
		return;

	if ( pParty->isMember(dwNewMasterGaeaID) == false )
		return;

	const DWORD dwCurrentMasterGaeaID = pMasterParty->GetMember(MASTER_PARTY)->m_dwGaeaID;
	if ( dwCurrentMasterGaeaID == GAEAID_NULL )
		return;

	GLMSG::SNET_PARTY_AUTHORITY NetMsg(pParty->GetPartyID(), dwNewMasterGaeaID, dwCurrentMasterGaeaID, dwPartySlot);

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyManagerClient::CheckReadyRequest(void)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( m_PartyID.isValidExpedition() == false )
		return;

	if ( isMasterAuthority() == false )
		return;

	const DWORD dwMasterGaeaID = m_pExpedition->GetMasterGaeaID();
	
	GLMSG::SNET_EXPEDITION_CHECK_READY_REQUEST_CA NetMsg(m_PartyID, dwMasterGaeaID);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyManagerClient::CheckReadyAnswer(const bool bReady)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( m_PartyID.isValidExpedition() == false )
		return;

	const DWORD dwMasterGaeaID = m_pExpedition->GetMasterGaeaID();
	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	const DWORD dwPartyIndex = m_pExpedition->GetPartySlotIndex(m_PartyID);

	GLMSG::SNET_EXPEDITION_CHECK_READY_ANSWER_CA NetMsg(m_PartyID, dwMasterGaeaID, dwMyGaeaID, dwPartyIndex, bReady);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	
}

void GLPartyManagerClient::SwapSlot(const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo)
{	
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( isMasterAuthority() == false )
		return;

	const DWORD dwMasterGaeaID = m_pGaeaClient->GETMYGAEAID();
	if ( (SwapSlotFrom.dwGaeaID == dwMasterGaeaID) || 
		SwapSlotTo.dwGaeaID == dwMasterGaeaID )
		return;

	if ( SwapSlotFrom.dwGaeaID == SwapSlotTo.dwGaeaID )
		return;

	if ( (SwapSlotFrom.nPartyIndex > (MAX_PARTYGROUP - 1)) || 
		(SwapSlotTo.nPartyIndex > (MAX_PARTYGROUP - 1)) )
		return;

	if ( (SwapSlotFrom.dwGaeaID != GAEAID_NULL) && (m_ptrPartyClient[SwapSlotFrom.nPartyIndex]->isMember(SwapSlotFrom.dwGaeaID) == false) )
		return;

	if ( (SwapSlotTo.dwGaeaID != GAEAID_NULL) && (m_ptrPartyClient[SwapSlotTo.nPartyIndex]->isMember(SwapSlotTo.dwGaeaID) == false) )
		return;	
	
	GLMSG::SNET_EXPEDITION_SWAP_SLOT NetMsg(m_PartyID, dwMasterGaeaID, SwapSlotFrom, SwapSlotTo);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyManagerClient::Nominate(const GLSwapSlotInformation& _NewSubMasterInfo)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		return;

	if ( _NewSubMasterInfo.nPartyIndex == MASTER_PARTY )
		return; // �������� ��Ƽ�� ���� �ִٸ� ����;

	if ( _NewSubMasterInfo.nPartyIndex > (MAX_PARTYGROUP - 1) )
		return; // �׷� ��ȣ ��ȿ�� �˻�;	

	if ( isMasterAuthority() == false )
		return; // ��û�ڰ� ���������� �ƴ� ��� ����;	

	if ( m_ptrPartyClient[_NewSubMasterInfo.nPartyIndex]->isMember(_NewSubMasterInfo.dwGaeaID) == false )
		return; // �ش� ��Ƽ�� ������ ���� ���� �ʴ� ������ ����;	

	if ( m_ptrPartyClient[_NewSubMasterInfo.nPartyIndex]->isMaster(_NewSubMasterInfo.dwGaeaID) == true )
		return; // �ش� ��Ƽ���� �̹� ������ �����̸� ����;

	GLPartySlotClient* pSubMasterSlot = m_ptrPartyClient[_NewSubMasterInfo.nPartyIndex]->GetMember(MASTER_PARTY);
	if ( pSubMasterSlot == NULL )
		return; // ������ ��������� ��ȿ�� �˻�;

	const GLSwapSlotInformation PrevSubMasterInfo(pSubMasterSlot->m_dwGaeaID, _NewSubMasterInfo.nPartyIndex);

	const DWORD dwMasterGaeaID = m_pGaeaClient->GETMYGAEAID();
	GLMSG::SNET_EXPEDITION_SWAP_SLOT NetMsg(m_PartyID, dwMasterGaeaID, _NewSubMasterInfo, PrevSubMasterInfo);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLPartyManagerClient::TakeItem(const DWORD dwGaeaID)
{
	const DWORD nLoopCount = m_PartyID.isValidExpedition() ? MAX_PARTYGROUP : 1;

	for ( DWORD _i = 0; _i < nLoopCount; ++_i )
	{
		GLPartySlotClient* const pMember = m_ptrPartyClient[_i]->GetMemberByGaeaID(dwGaeaID);
		if ( pMember == NULL )
			continue;

		pMember->SetMemberState(EMPARTY_MEMBER_STATE_TAKEITEM);
	}
}

void GLPartyManagerClient::Expedition(void)
{	
	if ( m_PartyID.isValidExpedition() == true || isMasterAuthority() == false )
		return;

	GLPartyClient* pParty(GetMyParty());
	if ( pParty == NULL )
		return;

	for ( unsigned int _index(MAXPARTY); _index--; )
	{
		GLPartySlotClient* const _pMember(pParty->GetMember(_index));
		if ( _pMember == NULL )
			continue;

		SMAPNODE* pMapNode(m_pGaeaClient->FindMapNode(_pMember->m_sMapID.getBaseMapID()));
		if ( pMapNode == NULL )
			return;

		const bool bPartyMap(pMapNode->IsPartyMap());
		const bool bExpeditionMap(pMapNode->IsExpeditionMap());
		const bool bInstantMap(pMapNode->IsInstantMap());
		if ( bPartyMap || bExpeditionMap || bInstantMap )
			return;
	}	

	GLMSG::SNET_EXPEDITION_NEW_CA ExpeditionMessage(m_PartyID);	
	m_pGaeaClient->NETSENDTOAGENT(&ExpeditionMessage);
}

void GLPartyManagerClient::MasterChatting(void)
{
	if ( m_PartyID.isValidExpedition() == false )
		return;

	if ( GetMyParty()->isMaster() == false )
		return;	

	const bool bMasterChatting = m_pExpedition->isMasterChatting() ? FALSE : TRUE;
	m_pExpedition->SetMasterChatting(bMasterChatting);
}

void GLPartyManagerClient::ChangeOption(const SPartyOption& PartyOption)
{
	DWORD dwMasterGaeaID = GAEAID_NULL;
	if ( m_PartyID.isValidExpedition() == true )
		dwMasterGaeaID = m_pExpedition->GetMasterGaeaID();
	else if ( m_PartyID.isValidParty() == true )
	{
		const GLPartySlotClient* const pMaster = m_ptrPartyClient[0]->GetMember(MASTER_PARTY);
		if ( pMaster == NULL )
			return;

		dwMasterGaeaID = pMaster->m_dwGaeaID;
	}
	else
		return;

	const SPartyOption& CurrentPartyOption = GetPartyOption();
	switch ( CurrentPartyOption.GetItemOption() )
	{
	case EMPARTY_ITEM_DICE:
	case EMPARTY_ITEM_LEADER_SHARE:
		{
			GLPartyTenderManagerClient* pTenderManager = m_pGaeaClient->GetPartyTenderManager();
			if ( pTenderManager->isTendering() == false )
				break;

			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT("PARTY_OPTION_CHANGE_FAIL"));
			return;
		}
		break;
	}

	GLMSG::SNET_PARTY_CHANGE_OPTION OptionMessage(dwMasterGaeaID, m_PartyID, PartyOption);
	m_pGaeaClient->NETSENDTOAGENT(&OptionMessage);
}

/**
 * ���� ���� ���� ��Ƽ ������ ��ȯ;
 */
GLPartyClient* const GLPartyManagerClient::GetMyParty(void)
{
	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() )
	{
		if ( m_PartyID.isValidExpedition() )
		{
			const DWORD dwPartyIndex = m_pExpedition->GetPartySlotIndex(m_PartyID);
			if ( dwPartyIndex != DEFSLOT_NULL )
				return m_ptrPartyClient[dwPartyIndex].get();			
		}
	}	
	
	return m_ptrPartyClient[MASTER_PARTY].get();
}

GLPartyClient* const GLPartyManagerClient::GetParty(const DWORD dwPartySlotIndex)
{	
	return m_ptrPartyClient[dwPartySlotIndex].get();
}

GLPartyClient* const GLPartyManagerClient::GetParty(const GLPartyID& PartyID)
{
	const DWORD dwPartyID = PartyID.GetPartyID();
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		if ( m_ptrPartyClient[_i - 1]->GetPartyID().GetPartyID() == dwPartyID )
			return m_ptrPartyClient[_i - 1].get();
	}

	return NULL;
}

const GLPartyID& GLPartyManagerClient::GetPartyID(void) const
{
	return m_PartyID;
}

const SPartyOption& GLPartyManagerClient::GetPartyOption(void) const
{
	return m_ptrPartyClient[MASTER_PARTY]->GetOption();
}

const DWORD GLPartyManagerClient::GetMasterGaeaID(void) const
{
	GLPartySlotClient* const pMaster = m_ptrPartyClient[0]->GetMember(MASTER_PARTY);
	if ( pMaster == NULL )
		return GAEAID_NULL;

	return pMaster->m_dwGaeaID;
}

const DWORD GLPartyManagerClient::GetNMember(void) const
{	
	if ( m_PartyID.isValidParty() == false )
		return 0; // ��Ƽ ���� �ƴϸ� 0�� ��ȯ;

	if ( m_PartyID.isValidExpedition() )
	{
		DWORD nMember = 0;
		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
			nMember += m_ptrPartyClient[_i - 1]->GetNMember();
		return nMember;
	}

	// ������ ��Ƽ ������ �˻縦 �Ͽ��� ������ ���⼭ 0�� ��ȯ �Ǹ� �ȵ�;
	return m_ptrPartyClient[MASTER_PARTY]->GetNMember(); 
}

const bool GLPartyManagerClient::_refreshExpeditionMaster(void)
{
	if ( m_PartyID.isValidExpedition() == false )
		return false;

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		const GLPartySlotClient* const pMember = m_ptrPartyClient[_i - 1]->GetMember(MASTER_PARTY);			
		const DWORD dwMasterGaeaID = pMember == NULL ? GAEAID_NULL : pMember->m_dwGaeaID;
		m_pExpedition->SetMasterGaeaID(dwMasterGaeaID, _i - 1);
	}

	return true;
}

/*
 * Party ������ ���������� �Ϸ� �Ǿ�����,
 * ��� ������ ����(��Ŷ ������) ���� ���ſ� ������ �߻��� ���,
 * �������� ��Ƽ/������ ������ �������� ��û;
 * ���������� ��ƼID�� GaeaID, ClientID�� ���� ��ȿ�� �˻�;
*/
const bool GLPartyManagerClient::RequestRefreshParty(const GLPartyID& PartyID, const DWORD emPartyRefreshType)
{	
	GLMSG::SNET_PARTY_REFRESH_REQ NetMsg(PartyID, m_pGaeaClient->GETMYGAEAID());
	NetMsg.emPartyRefreshRequest = EMPARTY_REFRESH_REQ(emPartyRefreshType);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	return true;
}

const bool GLPartyManagerClient::MsgProcess(NET_MSG_GENERIC* pMessage)
{
	// ��ʸ�Ʈ�� ��Ƽ�� �Ҽ��� ����.
	// TOURNAMENT_UNIQ_DISABLE
	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
	{
		switch ( pMessage->nType )
		{
			case NET_MSG_GCTRL_PARTY_LURE_TAR:
				// ������ Ŭ���̾�Ʈ���� �����ص� �Ǵ��� ���� �� ����
			case NET_MSG_GCTRL_PARTY_FNEW:
				// ��Ƽ ���� ����
			case NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME:
				// Ŭ���̾�Ʈ���� ������ �ص� �Ǵ��� ���� �ڵ� ����
				return true;
		}
	}
	switch ( pMessage->nType )
	{
	case NET_MSG_GCTRL_PARTY_LURE_TAR:
		return MsgPartyLureTarget(pMessage);
	case NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME:
		return MsgPartyLureTargetWithCharName(pMessage);	
	case NET_MSG_GCTRL_PARTY_LURE_FB:
		return MsgPartyLureFb(pMessage);
	case NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME:
		return MsgPartyLureFbWithCharName(pMessage);
	case NET_MSG_GCTRL_PARTY_FNEW_UPDATE:
		return MsgPartyFNewUpdate(pMessage);
	case NET_MSG_GCTRL_PARTY_FNEW:
		return MsgPartyFNew(pMessage);
	case NET_MSG_GCTRL_PARTY_ADD:
		return MsgPartyAdd(pMessage);
	case NET_MSG_GCTRL_PARTY_DEL:
		return MsgPartyDel(pMessage);
	case NET_MSG_GCTRL_PARTY_DISSOLVE:
		return MsgPartyDissolve(pMessage);
	case NET_MSG_GCTRL_PARTY_AUTHORITY:
		return MsgPartyAuthority(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
		return MsgPartyMoveMap(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_DETAIL:
		return MsgPartyMbrDetail(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_DETAIL_PARTY:
		return MsgPartyMbrDetailParty(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_LEVELUP:
		return MsgPartyMbrLevelUp(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_UPDATE:
		return MsgPartyMbrUpdate(pMessage);	
	case NET_MSG_GCTRL_PARTY_MBR_RENAME:
		return MsgPartyMbrRename(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD:
		return MsgPartyMbrItemPickUp(pMessage);
	case NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD:
		return MsgPartyMbrItemPickUpLink(pMessage);
	case NET_MSG_GCTRL_PARTY_LINK_JOIN_FB:
		return MsgPartyLinkJoinFB(pMessage);
	case NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS:
		return MsgPartyLinkJoinANS(pMessage);
	case NET_MSG_GCTRL_PARTY_LINK_LURE_FB:
		return MsgPartyLinkLureFB(pMessage);
	case NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB:
		return MsgPartyLinkRefuseFB(pMessage);		
	case NET_MSG_GCTRL_PARTY_CHANGE_OPTION:
		return MsgPartyChangeOption(pMessage);	
	case NET_MSG_GCTRL_EXPEDITION_FNEW_FB:
		return MsgExpeditionFNew(pMessage);
	case NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA: // Client(Master) -> Agent ->Client(Here);
		return MsgExpeditionCheckReadyRequest(pMessage);
	case NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA:
		return MsgExpeditionCheckReadyAnswer(pMessage);
	case NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT:
		return MsgExpeditionSwapSlot(pMessage);	
	case NET_MSG_GCTRL_PARTY_ERROR:
		return MsgPartyError(pMessage);
	case NET_MSG_GCTRL_EXPEDITION_ERROR :
		return MsgExpeditionError( pMessage );
	}

	return false;
}

const bool GLPartyManagerClient::isMasterAuthority(void)
{
	GLCharacter* const pMyChar = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyChar->GetGaeaID();	

	return m_ptrPartyClient[MASTER_PARTY]->isMaster(dwMyGaeaID);
}

const DWORD GLPartyManagerClient::GetPartySlotIndex(const GLPartyID& PartyID)
{
	if ( PartyID.isValidExpedition() )
		return m_pExpedition->GetPartySlotIndex(PartyID);
	else if ( PartyID.isValidParty() )
		return MASTER_PARTY;

	return DEFSLOT_NULL;
}

const bool GLPartyManagerClient::isMasterChatting(void) const
{
	if ( m_PartyID.isValidExpedition() == false )
		return false;

	return m_pExpedition->isMasterChatting();
}

const bool GLPartyManagerClient::isExpedition(void)
{
	return m_PartyID.isValidExpedition();
}
const bool GLPartyManagerClient::isParty(void)
{
	return m_ptrPartyClient[0]->isValid();
}

const bool GLPartyManagerClient::isPartyMaster(void) const
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return false;

	if ( m_PartyID.isValidParty() == false )
		return false; // ��Ƽ ���°� �ƴϸ� ����;
	
	if ( m_PartyID.isValidExpedition() == true )
		return false; // ������ ���¸� ����;	

	const DWORD dwMyGaeaID = pCharacter->GetGaeaID();
	return m_ptrPartyClient[0]->isMaster(dwMyGaeaID);
}

const bool GLPartyManagerClient::isExpeditionMaster(void) const
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return false;

	if ( m_PartyID.isValidParty() == false )
		return false; // ��Ƽ ���°� �ƴϸ� ����;

	if ( m_PartyID.isValidExpedition() == false )
		return false; // ������ ���°� �ƴϸ� ����;

	const DWORD dwMyGaeaID= pCharacter->GetGaeaID();
	if ( m_pExpedition->isMaster(dwMyGaeaID) )
		return true;

	return false;
}

const bool GLPartyManagerClient::isMaster(void) const
{
	if ( m_PartyID.isValidExpedition() == true )
		return isExpeditionMaster();
	
	return isPartyMaster();
}

const bool GLPartyManagerClient::isExpeditionSubMaster(void) const
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return false;

	if ( m_PartyID.isValidExpedition() == false )
		return false; // ������ ���°� �ƴϸ� ����;
	
	const DWORD dwPartyIndex = m_pExpedition->GetPartySlotIndex(m_PartyID);
	if ( dwPartyIndex == DEFPARTY_NULL )
		return false; // ��ȿ���� ���� ��Ƽ��� ����;

	const DWORD dwMyGaeaID= pCharacter->GetGaeaID();
	if ( m_ptrPartyClient[dwPartyIndex]->isMaster(dwMyGaeaID) )
		return true;

	return false;
}

void GLPartyManagerClient::ReqPartyInviteAns( bool bAccept, DWORD dwPartyID,
											  DWORD dwHashKey, DWORD dwMasterID, DWORD dwTimeKey )
{
	if( true == bAccept )
	{
		GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_OK,
			dwPartyID, dwHashKey, dwMasterID, dwTimeKey );
		m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
	}
	else
	{
		GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE,
			dwPartyID, 0, dwMasterID, 0 );
		m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
	}
}