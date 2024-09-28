#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/GLContrlPartyMsg.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"

#include "GLPartyManagerClient.h"
#include "GLPartyTenderManagerClient.h"

// 파티/원정대 탈퇴 및 해제시 삭제 해 줄것;

GLTenderItemCustomClient::GLTenderItemCustomClient(const MapID& mapID, const SITEMCUSTOM& ItemCustom, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition)
: GLTenderItemCustom(mapID, fInsertTime, vGeneratePosition)
, m_ItemCustom(ItemCustom)
, m_emTenderState(EMPARTY_TENDER_STATE_FREE)
, m_fPercent(0.0f)
, m_fAccumTime(0.0f)
, m_bSelected(false)
, m_bShow(false)
, m_bNew(true)
{
}

GLPartyTenderManagerClient::GLPartyTenderManagerClient(GLGaeaClient* const pGaeaClient)
: m_pGaeaClient(pGaeaClient)
, m_bNeedRefresh(false)
, m_dwSelectedItem(GAEAID_NULL)
{	
}

GLPartyTenderManagerClient::~GLPartyTenderManagerClient(void)
{
}

void GLPartyTenderManagerClient::Reset(void)
{
	//!! SF_TODO
	//m_pInterface->ToggleWindowTenderItem(false);
	//m_pInterface->ToggleWindowMasterTransfer(false);
	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDU_Close);
	m_ItemCustomMap.clear();
	m_ItemCustomVector.clear();
	m_bNeedRefresh = false;	
}

void GLPartyTenderManagerClient::FrameMove_LeaderShare(const GLPartyID& PartyID)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( pPartyManager->isMaster() == false )
	{
		//!! SF_TODO
		//m_pInterface->ToggleWindowMasterTransfer(false);
		//m_pInterface->OpenPartyTenderNotifyButton(false);
		return;
	}

	//!! SF_TODO
	//if ( m_pInterface->IsVisible(PARTY_DISTRIBUTION_WINDOW) == TRUE )
	//	return;

	for ( ItemCustomVectorIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
		GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		if( sTenderItemCustom.m_bNew )
		{
			//!! SF_TODO
			//m_pInterface->PartyTenderNotify( true );
			return;
		}
	}

	//!! SF_TODO
	//m_pInterface->PartyTenderNotify( false );
}

void GLPartyTenderManagerClient::FrameMove_ItemDice(void)
{
	//!! SF_TODO
	//if ( m_pInterface->IsVisible(PARTY_TENDER_WINDOW) == TRUE )
	//	return;

	for ( ItemCustomVectorIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
		GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		if( sTenderItemCustom.m_bNew )
		{
			//!! SF_TODO
			//m_pInterface->PartyTenderNotify( true );
			return;
		}
	}

	//!! SF_TODO
	//m_pInterface->PartyTenderNotify( false );
}

const HRESULT GLPartyTenderManagerClient::FrameMove(const float fTime, const float fElapsedTime)
{	
	const GLPartyID& PartyID = m_pGaeaClient->GetCharacter()->GetPartyID();
	if ( PartyID.isValidParty() == false )
		return S_OK;

	ItemCustomVectorIter iterItem = m_ItemCustomVector.begin();	
	if ( iterItem == m_ItemCustomVector.end() )
		return S_OK;

	const float fUTenderTime = PartyID.isValidExpedition() == true ? GLCONST_CHAR::fUExpeditionTenderTime : GLCONST_CHAR::fUPartyTenderTime;
	bool bIsRangeItem(false), bItemNew(false), bDeleteItem(false);
	for ( ItemCustomVectorIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
		GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		sTenderItemCustom.m_fAccumTime += fElapsedTime;
		const float fPercent = sTenderItemCustom.m_fAccumTime * fUTenderTime;
		
		if ( fPercent > 1.0f )
		{
			iterItem = m_ItemCustomVector.erase(iterItem);
			m_ItemCustomMap.erase(iter);
			m_bNeedRefresh = bDeleteItem = true;
		}
		else
		{
			++iterItem;
			sTenderItemCustom.m_fPercent = fPercent;			
		}

		if( isRangeTenderItem(dwItemID) )
		{
			bIsRangeItem = true;
		}

		if ( sTenderItemCustom.m_bNew && isRangeTenderItem(dwItemID) )
		{
			bItemNew = true;
		}

		// UI에서 Get할때 꺼주던가 여기서 해주던가해야함;
		if (sTenderItemCustom.m_bNew)
		{
			sTenderItemCustom.m_bNew = false;
		}
	}

	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	const SPartyOption& PartyOption = pPartyManager->GetPartyOption();

	//!! SF_TODO
// 	if( bItemNew )
// 	{
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
// 			"-b", true);
// 	}
	if ( !bIsRangeItem )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
			"-b", false);
	}
	if ( bDeleteItem )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
	}
	
	switch ( PartyOption.GetItemOption() )
	{
	case EMPARTY_ITEM_DICE:
		{
			if ( bItemNew )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
					"-b", true);
			}
			FrameMove_ItemDice();		
		}
		break;
	case EMPARTY_ITEM_LEADER_SHARE:
		{
			if ( bItemNew && pPartyManager->isMaster() == true )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
					"-b", true);
			}
			FrameMove_LeaderShare(PartyID);
		}
		break;
// 	default:
// 		Reset();
// 		return S_OK;
	}

	if ( !bIsRangeItem )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_OpenPartyTenderNotifyButton,
			"-b", false);
	}
	if ( bDeleteItem )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
	}

	return S_OK;
}

const bool GLPartyTenderManagerClient::MsgPartyAddTenderItem(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_ADD_TENDER_ITEM* const pMsg = 
		(GLMSG::SNET_PARTY_ADD_TENDER_ITEM*)pMessage;	
	
	SITEM* pItem = GLogicData::GetInstance().GetItem (pMsg->ItemCustom.GetNativeID());
	if ( pItem == NULL )
		return false;

	switch ( pItem->sBasicOp.emItemType )
	{
	case ITEM_QITEM:
	case ITEM_BOX:
		return false;
	}	
 	
	// 여기서 타입 검사후 유아이 호출하자;
	GLTenderItemCustomClient TenderItemCustom(pMsg->mapID, pMsg->ItemCustom, pMsg->fInsertTime, pMsg->vGeneratePosition);
	m_ItemCustomMap.insert(ItemCustomMapValue(pMsg->dwItemID, TenderItemCustom));
	m_ItemCustomVector.push_back(pMsg->dwItemID);
	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
	m_bNeedRefresh = true;

	return true;
}

const bool GLPartyTenderManagerClient::MsgPartyTenderItem(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_TENDER_ITEM* const pTenderMessage = (GLMSG::SNET_PARTY_TENDER_ITEM*)pMessage;

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	if ( pTenderMessage->dwGaeaID != dwMyGaeaID )
		return false;

	for ( DWORD _i = pTenderMessage->nItem; _i; --_i )
	{
		const DWORD dwItemID = pTenderMessage->dwItemIDs[_i - 1];
		ItemCustomMapIter iterItem = m_ItemCustomMap.find(dwItemID);
		if ( iterItem == m_ItemCustomMap.end() )
			continue;

		GLTenderItemCustomClient& TenderItemCustom = iterItem->second;
		TenderItemCustom.m_emTenderState = EMPARTY_TENDER_STATE_TENDER;		
	}
	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
	m_bNeedRefresh = true;

	return true;
}

/* 
 * 아이템을 실제로 분배하여 인벤에 넣어주는것은 서버에서 하기 때문에;
 * 기존 리스트에서 아이템 이름을 가져오지 않음;
 * 이미 타임 아웃 되었기 때문에 기존 리스트에 존재 하지도 않음; 
 * 아이템을 플레이어의 인벤에 넣는 주체는 서버에 작성 되어 있는 기존 함수(InsertToInven) 그대로 사용 함;
 * 입찰 메니저 클라이언트는 입찰 진행 및 결과 알림의 역할만 함;
*/
const bool GLPartyTenderManagerClient::MsgPartyTenderItemFb(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_TENDER_ITEM_FB* const pTenderMessage = (GLMSG::SNET_PARTY_TENDER_ITEM_FB*)pMessage;	
	
	GLPartyManagerClient* pPartyMan = m_pGaeaClient->GetPartyManager();
	pPartyMan->TakeItem(pTenderMessage->dwOwnerGaeaID);

	GLPartyClient* pParty = pPartyMan->GetParty( pTenderMessage->PartyID );
	if ( pParty == NULL )

		return false;

	GLPartySlotClient* pMyChar = pParty->GetMemberByGaeaID( m_pGaeaClient->GetCharacter()->GetGaeaID() );
	if ( pMyChar == NULL )
		return false;

	if ( pParty->GetPartyID() != pPartyMan->GetMyParty()->GetPartyID() )
		return false;

	if ( pTenderMessage->nMember > 1 )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::GUILD,
			sc::string::format(ID2GAMEINTEXT("PARTY_TENDER_DICE_BEGIN"), 
			pTenderMessage->cItemName));
	}

	for ( DWORD _i = pTenderMessage->nMember; _i; --_i )
	{
		GLPartySlotClient* pMember = pParty->GetMemberByGaeaID( pTenderMessage->sDiceResult[_i - 1].dwGaeaID );
		if ( pMember == NULL )
			continue;
		
		if ( pMember->m_sMapID != pMyChar->m_sMapID )
			continue;

		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::IDCOLOR,
			sc::string::format(ID2GAMEINTEXT("PARTY_TENDER_DICE_RESULT"), 
			pMember->m_szName,
			DWORD(pTenderMessage->sDiceResult[_i - 1].btResult)));
	}

	{
		ItemCustomVectorIter iterItem = 
			std::find(m_ItemCustomVector.begin(), m_ItemCustomVector.end(), pTenderMessage->dwItemID);
		if ( iterItem != m_ItemCustomVector.end() )		
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
			m_ItemCustomVector.erase(iterItem);
			m_bNeedRefresh = true;
		}
	}
	{
		ItemCustomMapIter iterItem = m_ItemCustomMap.find(pTenderMessage->dwItemID);
		if ( iterItem == m_ItemCustomMap.end() )
			return false;
		else
		{
			m_ItemCustomMap.erase(iterItem);
			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
			m_bNeedRefresh = true;
		}		
	}
	
	return true;
}

const bool GLPartyTenderManagerClient::MsgPartyJunkItem(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_JUNK_ITEM* const pJunkMessage = (GLMSG::SNET_PARTY_JUNK_ITEM*)pMessage;	
	const DWORD dwMasterGaeaID = m_pGaeaClient->GetPartyManager()->GetMasterGaeaID();
	if ( dwMasterGaeaID == GAEAID_NULL )
		return false;

	if ( dwMasterGaeaID != pJunkMessage->dwGaeaID )
		return false;

	const DWORD dwItemID = pJunkMessage->dwItemID;
	{
		ItemCustomVectorIter iterItem = 
			std::find(m_ItemCustomVector.begin(), m_ItemCustomVector.end(), dwItemID);

		if ( iterItem != m_ItemCustomVector.end() )
			m_ItemCustomVector.erase(iterItem);
	}

	{
		ItemCustomMapIter iterItem = m_ItemCustomMap.find(dwItemID);
		if ( iterItem != m_ItemCustomMap.end() )
			m_ItemCustomMap.erase(iterItem);
	}

	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::IDCOLOR,
		sc::string::format(ID2GAMEINTEXT("PARTY_DISTRIBUTION_ITEM_JUNK"), 
		pJunkMessage->cItemName));

	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
	m_bNeedRefresh = true;

	return true;
}

const bool GLPartyTenderManagerClient::MsgProcess(NET_MSG_GENERIC* pMessage)
{
	switch ( pMessage->nType )
	{
	case NET_MSG_GCTRL_PARTY_ADD_TENDER_ITEM:
		return MsgPartyAddTenderItem(pMessage);	
	case NET_MSG_GCTRL_PARTY_TENDER_ITEM:
		return MsgPartyTenderItem(pMessage);
	case NET_MSG_GCTRL_PARTY_TENDER_ITEM_FB:
		return MsgPartyTenderItemFb(pMessage);
	case NET_MSG_GCTRL_PARTY_JUNK_ITEM:
		return MsgPartyJunkItem(pMessage);
	}
	return false;
}

const DWORD GLPartyTenderManagerClient::GetNItem(void) const
{
	return m_ItemCustomVector.size();
}
const DWORD GLPartyTenderManagerClient::GetItemID(const DWORD dwIndex) const
{
	if ( !(dwIndex < m_ItemCustomVector.size()) )
		return GAEAID_NULL;

	const DWORD dwItemID = m_ItemCustomVector[dwIndex];
	if ( m_ItemCustomMap.find(dwItemID) == m_ItemCustomMap.end() )
		return GAEAID_NULL;

	return dwItemID;
}


GLTenderItemCustomClient* const GLPartyTenderManagerClient::GetTenderItemCustom(const DWORD dwIndex, const bool bDistribution /* = false */)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return NULL;

	if ( (pCharacter->IsPartyMaster() == false) && bDistribution )
		return NULL;

	const GLPartyID& PartyID = pCharacter->GetPartyID();
	if ( PartyID.isValidParty() == false )
	{ // 유효한 파티인지 검사;
		Reset();
		return NULL;
	}

	if ( dwIndex >= m_ItemCustomVector.size() )
		return NULL; // 인덱스 유효성 검사;

	const DWORD dwItemID = m_ItemCustomVector[dwIndex];
	ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
	if ( iter == m_ItemCustomMap.end() )
		return NULL; // 아이템 유효성 검사;	
	
	GLTenderItemCustomClient& ItemCustom = iter->second;

	if ( ItemCustom.m_mapID != m_pGaeaClient->GetActiveMapID() )
		return NULL; // 같은 지역에 속한 아이템인지 검사;

	const float fLimitRange = PartyID.isValidExpedition() ? ::pow(GLCONST_CHAR::fExpeditionTenderRange + 20.0f, 2.0f) : ::pow(GLCONST_CHAR::fPartyTenderRange + 20.0f, 2.0f);
	const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(ItemCustom.m_vGeneratePosition - pCharacter->GetPosition()));
	if ( fDistance > fLimitRange )
	{
		if ( ItemCustom.m_bShow == true )
		{
			ItemCustom.m_bShow = false;
			ItemCustom.m_bSelected = false;
			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_PDD_Update);
			m_bNeedRefresh = true;
		}
		return NULL; // 반경 유효성 검사;	
	}

	if ( ItemCustom.m_bShow == false )
	{
		ItemCustom.m_bShow = true;
		m_bNeedRefresh = true;

		if( ItemCustom.m_bNew )
			ItemCustom.m_bNew = false;
	}

	return &ItemCustom;
}

GLTenderItemCustomClient* const GLPartyTenderManagerClient::GetDistributionItemCustom(const DWORD dwIndex)
{
	return GLPartyTenderManagerClient::GetTenderItemCustom(dwIndex, true);
}

const GLTenderItemCustomClient* const GLPartyTenderManagerClient::GetSelectedItemCustom(void) const
{
	ItemCustomMapCIter iter = m_ItemCustomMap.find(m_dwSelectedItem);
	if ( iter == m_ItemCustomMap.end() )
		return NULL;

	const GLTenderItemCustomClient& ItemCustom = iter->second;
	return &ItemCustom;
}

const bool GLPartyTenderManagerClient::isTendering(void) const
{
	return m_ItemCustomVector.size() ? true : false;
}
const bool GLPartyTenderManagerClient::isNeedRefresh(void) const
{
	return m_bNeedRefresh;
}
const bool GLPartyTenderManagerClient::isExistNoneAvailableItem(void) const
{
	const GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	for ( ItemCustomVectorCIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapCIter iter = m_ItemCustomMap.find(dwItemID);
		const GLTenderItemCustomClient& sTenderItemCustom = iter->second;

        if ( sTenderItemCustom.m_bSelected == false )
            continue;

		if ( sTenderItemCustom.m_emTenderState == EMPARTY_TENDER_STATE_NONEAVAILABLE )
			return true;
	}

	return false;
};

const bool GLPartyTenderManagerClient::isRangeTenderItem(const DWORD dwItemID)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return false;

	const GLPartyID& PartyID = pCharacter->GetPartyID();
	if ( PartyID.isValidParty() == false )
	{ // 유효한 파티인지 검사;
		return false;
	}

	ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
	if ( iter == m_ItemCustomMap.end() )
		return false; // 아이템 유효성 검사;	

	GLTenderItemCustomClient& ItemCustom = iter->second;

	if ( ItemCustom.m_mapID != m_pGaeaClient->GetActiveMapID() )
		return false; // 같은 지역에 속한 아이템인지 검사;

	const float fLimitRange = PartyID.isValidExpedition() ? ::pow(GLCONST_CHAR::fExpeditionTenderRange + 20.0f, 2.0f) : ::pow(GLCONST_CHAR::fPartyTenderRange + 20.0f, 2.0f);
	const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(ItemCustom.m_vGeneratePosition - pCharacter->GetPosition()));
	if ( fDistance > fLimitRange )
	{
		return false;
	}

	return true;
}

void GLPartyTenderManagerClient::RefreshTenderItemIsNew()
{
	//!! SF_TODO
	//if( !m_pInterface->UiIsVisibleGroup(PARTY_TENDER_WINDOW) && !m_pInterface->UiIsVisibleGroup(PARTY_DISTRIBUTION_WINDOW) )
	//	return;

	for ( ItemCustomVectorIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
		GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		if( sTenderItemCustom.m_bNew && isRangeTenderItem( dwItemID ) )
		{
			sTenderItemCustom.m_bNew = false;
		}
	}
}

void GLPartyTenderManagerClient::DoneRefresh(void)
{
	m_bNeedRefresh = false;
}

const bool GLPartyTenderManagerClient::SetSelect(const DWORD dwSelectSlotIndex)
{
	if ( dwSelectSlotIndex >= m_ItemCustomVector.size() )
		return false;

	const DWORD dwItemID = m_ItemCustomVector[dwSelectSlotIndex];
	ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
	if ( iter == m_ItemCustomMap.end() )
		return false;

	GLTenderItemCustomClient& ItemCustom = iter->second;
	const bool bSelected = !ItemCustom.m_bSelected;
	ItemCustom.m_bSelected = bSelected;
	
	m_dwSelectedItem = bSelected ? dwItemID : GAEAID_NULL;
	
	m_bNeedRefresh = true;
	return bSelected;
}

void GLPartyTenderManagerClient::SetBeginIndex(const DWORD dwBeginIndex)
{
	m_dwBeginIndex = dwBeginIndex;
}

void GLPartyTenderManagerClient::ResetSelectByItemID(const DWORD dwItemID)
{
	ItemCustomMapIter iterItem = m_ItemCustomMap.find(dwItemID);
	if ( iterItem == m_ItemCustomMap.end() )
		return;

	GLTenderItemCustomClient& ItemCustom = iterItem->second;
	ItemCustom.m_bSelected = false;
	m_dwSelectedItem = GAEAID_NULL;
	m_bNeedRefresh = true;
}

void GLPartyTenderManagerClient::ResetSelectAll()
{
	for ( ItemCustomVectorCIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{	
		const DWORD dwItemID = *iterItem;

		ResetSelectByItemID(dwItemID);
	}

	m_dwSelectedItem = GAEAID_NULL;
	m_bNeedRefresh = true;
}

void GLPartyTenderManagerClient::TenderItem(void) const
{
	const GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const GLPartyID& PartyID = pCharacter->GetPartyID();	
	const DWORD dwGaeaID = pCharacter->GetGaeaID();
	GLMSG::SNET_PARTY_TENDER_ITEM TenderMessage(PartyID, dwGaeaID);

	for ( ItemCustomVectorCIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); ++iterItem )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapCIter iter = m_ItemCustomMap.find(dwItemID);
		const GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		if ( sTenderItemCustom.m_bSelected == false )
			continue; // 선택되지 않았거나 입찰 가능한 아이템이 아닌 경우 패스;

		TenderMessage.AddItem(dwItemID);
	}

	m_pGaeaClient->NETSENDTOFIELD(&TenderMessage);
}

void GLPartyTenderManagerClient::AbandonItem(void)
{
	const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const GLPartyID& PartyID = pCharacter->GetPartyID();	
	const DWORD dwGaeaID = pCharacter->GetGaeaID();

	for ( ItemCustomVectorIter iterItem = m_ItemCustomVector.begin(); iterItem != m_ItemCustomVector.end(); )
	{		
		const DWORD dwItemID = *iterItem;

		ItemCustomMapIter iter = m_ItemCustomMap.find(dwItemID);
		GLTenderItemCustomClient& sTenderItemCustom = iter->second;

		if ( sTenderItemCustom.m_emTenderState == EMPARTY_TENDER_STATE_TENDER )
		{
			++iterItem;
			continue;
		}

		iterItem = m_ItemCustomVector.erase(iterItem);
		m_ItemCustomMap.erase(iter);
		m_bNeedRefresh = true;
	}
}

void GLPartyTenderManagerClient::TransferItem(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( pPartyManager->isParty() == false )
		return;

	const bool bExpedition = pPartyManager->isExpedition();
	if ( bExpedition )
	{
		if ( (pPartyManager->isExpeditionMaster() == false) )
			return; // 원정대중이고 원정대장이 아니면 실패;
	}
	else
	{
		if ( pPartyManager->isPartyMaster() == false )
			return; // 파티중이고 파티장이 아니면 실패;
	}

	if ( m_dwSelectedItem == GAEAID_NULL )
		return; // 선택된 아이템이 없으면 실패;	

	ItemCustomMapCIter iterItem = m_ItemCustomMap.find(m_dwSelectedItem);
	if ( iterItem == m_ItemCustomMap.end() )
		return; // 존재 하지 않는 아이템이면 실패;

	const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const GLPartyID& PartyID = pCharacter->GetPartyID();
	const DWORD dwGaeaID = pCharacter->GetGaeaID();
	GLMSG::SNET_PARTY_TRANSFER_ITEM TransferMessage(PartyID, dwGaeaID, m_dwSelectedItem);

	const DWORD nCount = bExpedition ? MAX_PARTYGROUP : 1;
	for ( DWORD _i = 0; _i < nCount; ++_i )
	{
		GLPartyClient* const pParty = pPartyManager->GetParty(_i);
		if ( pParty == NULL )
			return;

		const GLPartyID& PartyID = pParty->GetPartyID();
		const DWORD dwPartyID = PartyID.GetPartyID();
		for ( DWORD _j = 0; _j < pParty->GetNMember(); ++_j )
		{
			GLPartySlotClient* pMember = pParty->GetMember(_j);
			if ( pMember == NULL )
				continue;

			if ( pMember->isMemberState(EMPARTY_MEMBER_STATE_SELECTED) == false )
				continue;

			TransferMessage.AddMember(pMember->m_dwGaeaID, dwPartyID);
		}		
	}

	m_pGaeaClient->NETSENDTOFIELD(&TransferMessage);
}

void GLPartyTenderManagerClient::JunkItem(void)
{	
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( pPartyManager->isParty() == false )
		return;

	if ( pPartyManager->isExpedition() )
	{
		if ( (pPartyManager->isExpeditionMaster() == false) )
			return; // 원정대중이고 원정대장이 아니면 실패;
	}
	else
	{
		if ( pPartyManager->isPartyMaster() == false )
			return; // 파티중이고 파티장이 아니면 실패;
	}

	if ( m_dwSelectedItem == GAEAID_NULL )
		return; // 선택된 아이템이 없으면 실패;	

	ItemCustomMapCIter iterItem = m_ItemCustomMap.find(m_dwSelectedItem);
	if ( iterItem == m_ItemCustomMap.end() )
		return; // 존재 하지 않는 아이템이면 실패;

	const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const GLPartyID& PartyID = pCharacter->GetPartyID();
	const DWORD dwGaeaID = pCharacter->GetGaeaID();
	GLMSG::SNET_PARTY_JUNK_ITEM JunkMessage(PartyID, dwGaeaID, m_dwSelectedItem);
	m_pGaeaClient->NETSENDTOFIELD(&JunkMessage);	
}
