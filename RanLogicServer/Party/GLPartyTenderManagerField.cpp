//#include "../pch.h"

#include "../../RanLogic/Msg/GLContrlPartyMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Crow/GLCrow.h"

#include "GLPartyManagerField.h"
#include "GLPartyTenderManagerField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
// 

const bool DecideItemAuthority(GLGaeaServer* const pGaeaServer, const GLPartyID& PartyID, const DWORD dwGlobalID, const GLTenderItemCustomField& TenderItem);

GLTenderItemCustomField::GLTenderItemCustomField(const MapID& mapID, CItemDrop* pItemDrop, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition)
: GLTenderItemCustom(mapID, fInsertTime, vGeneratePosition)
, m_pItemDrop(pItemDrop)
{
	Reset();
}

void GLTenderItemCustomField::Reset(void)
{
	m_TenderMemberGaeaIDs.clear();
	m_AbandonMemberGaeaIDs.reserve(MAXPARTY * MAX_PARTYGROUP);	
}

GLPartyTenderManagerField::GLPartyTenderManagerField(GLGaeaServer* const pGaeaServer)
: m_pGaeaServer(pGaeaServer)
, m_fRefreshTime(0.0f)
, m_fCurrentTime(0.0f)
{
}

GLPartyTenderManagerField::~GLPartyTenderManagerField(void)
{
}

void GLPartyTenderManagerField::FrameMove(const float fTime, const float fElapsedTime)
{	
	m_fCurrentTime = fTime;
	if ( (fTime - m_fRefreshTime) < TENDER_ITEM_REFRESH_TIME )
		return;

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();

	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.begin();
	PartyItemCustomMapCIter iterPartyEnd = m_PartyItemCustomMap.end();
	while ( iterParty != iterPartyEnd )
	{
		const GLPartyID& PartyID = iterParty->first;		
		if ( pPartyManager->GetParty(PartyID) == NULL )
		{
			m_PartyItemCustomMap.erase(iterParty++);
			continue;
		}

		ItemCustomMap& mapItemCustom = iterParty->second;
		ItemCustomMapIter iterItem = mapItemCustom.begin();
		ItemCustomMapCIter iterItemEnd = mapItemCustom.end();

		const float fTenderTime = PartyID.isValidExpedition() == true ? GLCONST_CHAR::fExpeditionTenderTime : GLCONST_CHAR::fPartyTenderTime;
		while ( iterItem != iterItemEnd )
		{
			const DWORD dwItemID = iterItem->first;
			GLTenderItemCustomField& sTenderItemCustom = iterItem->second;
			if ( fTime - sTenderItemCustom.m_fInsertTime > fTenderTime )
			{
				DecideItemAuthority(m_pGaeaServer, PartyID, dwItemID, sTenderItemCustom);					
				mapItemCustom.erase(iterItem++);
				break;
			}
			else
				++iterItem;			
		}
		if ( mapItemCustom.size() == 0 )
			m_PartyItemCustomMap.erase(iterParty++);
		else
			++iterParty;
	}
}

const bool GLPartyTenderManagerField::isLootable(const GLPartyID& PartyID, const DWORD dwGlobalID) const
{
	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();	
	if ( pPartyManager->GetParty(PartyID) == NULL )		
		return false; // 유효하지 않은 파티라면 해킹;
	// 유효하지 않은 파티인데 파티 옵션 적용을 받을 수 없음;

	PartyItemCustomMapCIter iterParty = m_PartyItemCustomMap.find(PartyID);
	if ( iterParty == m_PartyItemCustomMap.end() )
		return true; // 입찰 진행 목록이 없다면 입찰 대상 아이템이 아니므로 루팅 가능;

	const ItemCustomMap& mapItemCustom = iterParty->second;
	ItemCustomMapCIter iterItem = mapItemCustom.find(dwGlobalID);
	if ( iterItem == mapItemCustom.end() )
		return true; // 입찰 진행 목록에서 빠져있다면 루팅 가능;

	return false;
}

void GLPartyTenderManagerField::_addItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop)
{
	GLTenderItemCustomField TenderItemCustom(mapID, pItemDrop, m_fCurrentTime, vGeneratePosition);		
	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.find(PartyID);
	if ( iterParty == m_PartyItemCustomMap.end() )
	{		
		ItemCustomMap mapItemCustom;
		mapItemCustom.insert(ItemCustomMapValue(pItemDrop->dwGlobID, TenderItemCustom));

		m_PartyItemCustomMap.insert(PartyItemCustomMapValue(PartyID, mapItemCustom));		
	}
	else
	{
		ItemCustomMap& mapItemCustom = iterParty->second;
		mapItemCustom.insert(ItemCustomMapValue(pItemDrop->dwGlobID, TenderItemCustom));
	}
	
	GLMSG::SNET_PARTY_ADD_TENDER_ITEM TenderItemMessage(PartyID, mapID, pItemDrop->sItemCustom, pItemDrop->dwGlobID, vGeneratePosition, m_fCurrentTime);	
	m_pGaeaServer->SENDTOPARTYCLIENT(PartyID, mapID, &TenderItemMessage);
}

const bool GLPartyTenderManagerField::AddTenderItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop)
{	
	const SITEM* const pItemData = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.GetNativeID());
	if ( pItemData == NULL )
		return false;	

	switch ( pItemData->sBasicOp.emItemType )
	{
	case ITEM_QITEM:
	case ITEM_BOX:
		return false;
	}

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();	
	if ( pPartyManager->GetParty(PartyID) == NULL )		
		return false;

	_addItem(PartyID, mapID, vGeneratePosition, pItemDrop);	

	return true;
}

const bool GLPartyTenderManagerField::AddDistributionItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop)
{
	const SITEM* const pItemData = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.GetNativeID());
	if ( pItemData == NULL )
		return false;	

	switch ( pItemData->sBasicOp.emItemType )
	{
	case ITEM_QITEM:
	case ITEM_BOX:
		return false;
	}

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();
	GLPartyField* const pParty = pPartyManager->GetParty(PartyID);
	if ( pParty == NULL )		
		return false;

	GLChar* const pMaster = m_pGaeaServer->GetChar(pParty->GetMasterGaeaID());
	if ( pMaster == NULL )
		return false; // 마스터가 존재 하지 않으면 실패(필드 서버는 존재 하지 않을 수 있음);

	if ( pMaster->GetCurrentMap() != mapID )
		return false; // 마스터와 동일한 맵이 아니면 실패;

	const D3DXVECTOR3& vMasterPosition = pMaster->GetPosition();
	const float fLimitRange = PartyID.isValidExpedition() ? ::pow(GLCONST_CHAR::fExpeditionTenderRange, 2.0f) : ::pow(GLCONST_CHAR::fPartyTenderRange, 2.0f);
	const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(vGeneratePosition - vMasterPosition));
	if ( fDistance > fLimitRange )
		return false;

	_addItem(PartyID, mapID, vGeneratePosition, pItemDrop);

	return true;
}


const bool GLPartyTenderManagerField::MsgProcess_TenderItem(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_TENDER_ITEM* const pTenderMessage = (GLMSG::SNET_PARTY_TENDER_ITEM*)pMessage;

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();
	const GLPartyID& PartyID = pTenderMessage->PartyID;
	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* pExpedition = pPartyManager->GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;
	}	

	GLPartyField* const pParty = pPartyManager->GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	GLChar* pChar = m_pGaeaServer->GetChar(pTenderMessage->dwGaeaID);
	if ( pChar == NULL )
		return false;

	if ( pParty->isMemberByGaeaID(pTenderMessage->dwGaeaID) == false )
		return false;

	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.find(PartyID);
	if ( iterParty == m_PartyItemCustomMap.end() )
		return false;

	if ( pTenderMessage->nItem > MAX_TENDERABLE_ITEM )
		return false;

	ItemCustomMap& mapItemCustom = iterParty->second;
	for ( DWORD _i = pTenderMessage->nItem; _i; --_i )
	{
		const DWORD dwItemID = pTenderMessage->dwItemIDs[_i - 1];
		ItemCustomMapIter iterItem = mapItemCustom.find(dwItemID);
		if ( iterItem == mapItemCustom.end() )
			continue;

		GLTenderItemCustomField& TenderItem = iterItem->second;
		if ( std::find(TenderItem.m_TenderMemberGaeaIDs.begin(), TenderItem.m_TenderMemberGaeaIDs.end(), pTenderMessage->dwGaeaID) !=
			TenderItem.m_TenderMemberGaeaIDs.end() )
			continue;

		TenderItem.m_TenderMemberGaeaIDs.push_back(pTenderMessage->dwGaeaID);
	}

	m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), pMessage);

	return true;
}

const bool GLPartyTenderManagerField::MsgProcess_JunkItem(NET_MSG_GENERIC* pMessage)
{
	GLMSG::SNET_PARTY_JUNK_ITEM* pJunkMessage = (GLMSG::SNET_PARTY_JUNK_ITEM*)pMessage;

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();	
	const GLPartyID& PartyID = pJunkMessage->PartyID;
	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* pExpedition = pPartyManager->GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;
	}	

	GLPartyField* const pParty = pPartyManager->GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	const DWORD dwMasterGaeaID = pJunkMessage->dwGaeaID;
	GLChar* pMaster = m_pGaeaServer->GetChar(dwMasterGaeaID);
	if ( pMaster == NULL )
		return false;

	if ( pParty->isMasterByGaeaID(dwMasterGaeaID) == false )
		return false;	

	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.find(PartyID);
	if ( iterParty == m_PartyItemCustomMap.end() )
		return false;

	ItemCustomMap& mapItemCustom = iterParty->second;
	ItemCustomMapIter iterItem = mapItemCustom.find(pJunkMessage->dwItemID);
	if ( iterItem == mapItemCustom.end() )
		return false;

	GLTenderItemCustomField& JunkItem = iterItem->second;
	pJunkMessage->SetName(JunkItem.m_pItemDrop->sItemCustom.GETNAME().c_str());

	mapItemCustom.erase(iterItem);

	m_pGaeaServer->SENDTOPARTYCLIENT(PartyID, pMaster->GetCurrentMap(), pJunkMessage);

	return true;
}

const bool GLPartyTenderManagerField::MsgProcess_TransferItem(NET_MSG_GENERIC* pMessage)
{
	const GLMSG::SNET_PARTY_TRANSFER_ITEM* const pTransferMessage = 
		(GLMSG::SNET_PARTY_TRANSFER_ITEM*)pMessage;

	const GLPartyID& PartyID = pTransferMessage->PartyID;
	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.find(PartyID);
	if ( iterParty == m_PartyItemCustomMap.end() )
		return false;	

	ItemCustomMap& mapItemCustom = iterParty->second;
	const DWORD dwItemID = pTransferMessage->dwItemID;
	ItemCustomMapIter iterItem = mapItemCustom.find(dwItemID);
	if ( iterItem == mapItemCustom.end() )
		return false;

	GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();
	GLTenderItemCustomField& TransferItem = iterItem->second;	

	const GLPARTY_FNET* const pMaster = pPartyManager->GetMasterSlot(PartyID);

	TransferItem.m_TenderMemberGaeaIDs.clear();	
	for ( DWORD _i = pTransferMessage->nMember; _i; --_i )
	{
		const DWORD dwPartyID = pTransferMessage->MemberInfos[_i - 1].dwPartyID;
		GLPartyField* pParty = pPartyManager->GetParty(dwPartyID);
		if ( pParty == NULL )
			return false;
		
		const DWORD dwGaeaID = pTransferMessage->MemberInfos[_i - 1].dwGaeaID;
		const GLPARTY_FNET* const pMember = pParty->GetMemberSlot(dwGaeaID);
		if ( pMember == NULL )
			continue;

		if ( pMaster->m_sMapID != pMember->m_sMapID )
			continue;

		TransferItem.m_TenderMemberGaeaIDs.push_back(dwGaeaID);
	}
	if ( DecideItemAuthority(m_pGaeaServer, PartyID, dwItemID, TransferItem) == true )
		mapItemCustom.erase(iterItem);
	
	return true;
}

const bool GLPartyTenderManagerField::MsgProcess(NET_MSG_GENERIC* pMessage)
{
	switch ( pMessage->nType )
	{
	case NET_MSG_GCTRL_PARTY_TENDER_ITEM:
		return MsgProcess_TenderItem(pMessage);
	case NET_MSG_GCTRL_PARTY_JUNK_ITEM:
		return MsgProcess_JunkItem(pMessage);
	case NET_MSG_GCTRL_PARTY_TRANSFER_ITEM:
		return MsgProcess_TransferItem(pMessage);
	}
	return false;
}

const bool GLPartyTenderManagerField::IsTenderMemberExist( const GLPartyID& PartyId, const CItemDrop* pItem )
{ 	
	PartyItemCustomMapIter iterParty = m_PartyItemCustomMap.find(PartyId);

	if ( iterParty == m_PartyItemCustomMap.end() || pItem == NULL ) return false;	
	
	ItemCustomMapIter iterItem = iterParty->second.find( pItem->dwGlobID );

	if ( iterItem == iterParty->second.end() )  return false;

	if( iterItem->second.m_TenderMemberGaeaIDs.size() == 0 ) return false;

	return true;
}

const bool GLPartyTenderManagerField::IsTenderOwnReset( DWORD dwPartyId, const CItemDrop* pItem)
{	
	/*
	GLPartyField* pParty = m_pGaeaServer->GetParty( dwPartyId );

	if( pParty ) {
		const SPartyOption& PartyOption = pParty->GetPartyOption();

		switch( PartyOption.GetItemOption())
		{
		case EMPARTY_ITEM_DICE :	// 주사위만 체크
			{
				const GLPartyID partyId( dwPartyId );

				if( !isLootable(partyId, pItem->dwGlobID) )			// 입찰중인 아이템
				{

				}

				// 입찰자가 있음.
				if( IsTenderMemberExist(partyId, pItem) )
				{
					
				}
				// 입찰자 없음.
				else
				{
					
				}
			}
			break;
		default :  break;
		}
	}*/

	return false;
}

const bool DecideItemAuthority(GLGaeaServer* const pGaeaServer, const GLPartyID& PartyID, const DWORD dwItemID, const GLTenderItemCustomField& TenderItem)
{	
	std::vector<BYTE> ShuffleBufferVector;
	ShuffleBufferVector.reserve( 100 );

	{ // 랜덤 배열 생성;
		for ( BYTE _i = 0; _i < 100; ++_i )
			ShuffleBufferVector.push_back( _i );		

		std::random_shuffle( ShuffleBufferVector.begin(), ShuffleBufferVector.end() );
	}

	GLMSG::SNET_PARTY_TENDER_ITEM_FB ResultMessage(PartyID, dwItemID, TenderItem.m_pItemDrop->sItemCustom.GETNAME().c_str());

	BYTE btBiggestNumber = 0;	
	
	const DWORD nMember = TenderItem.m_TenderMemberGaeaIDs.size();
	if ( nMember == 1 )
		ResultMessage.dwOwnerGaeaID = TenderItem.m_TenderMemberGaeaIDs[0];
	else
	{
		for ( DWORD _i = TenderItem.m_TenderMemberGaeaIDs.size(); _i; --_i )
		{
			const DWORD dwGaeaID = TenderItem.m_TenderMemberGaeaIDs[_i - 1];
			GLChar* const pCharacter = pGaeaServer->GetChar(dwGaeaID);
			if ( pCharacter == NULL )
				continue;

			const BYTE btNumber = ShuffleBufferVector[_i - 1];
			if ( btNumber > btBiggestNumber )
			{
				btBiggestNumber = btNumber;	
				ResultMessage.dwOwnerGaeaID = dwGaeaID;
			}
			ResultMessage.AddMember(dwGaeaID, btNumber);
		}
	}
	
	GLChar* pChar = pGaeaServer->GetChar(ResultMessage.dwOwnerGaeaID);
	if ( pChar == NULL )
		return false; // 해당 플레이어가 현재 필드 서버에 존재 하지 않음;

	if ( TenderItem.m_mapID != pChar->GetCurrentMap() )
		return false; // 해당 플레이어가 아이템과 같은 지역내에 있지 않음;	

	pGaeaServer->SENDTOPARTYCLIENT(PartyID, TenderItem.m_mapID, &ResultMessage);

	if ( pChar->InsertToInven(TenderItem.m_pItemDrop, true, true, true, false, true) == false )
	{ // 인벤토리가 가득찼다거나 하는 이유로 실패한 경우 우편으로 보냄;
		const SITEM* pItem = GLogicData::GetInstance().GetItem(TenderItem.m_pItemDrop->sItemCustom.GetBoxWrappedItemID());
		if (!pItem)
			return false;
		if ( EMITEM_LEVEL_NORMAL < pItem->sBasicOp.emLevel )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD NetMsg(PartyID);
			NetMsg.dwGaeaID  = ResultMessage.dwOwnerGaeaID;
			NetMsg.sItem     = TenderItem.m_pItemDrop->sItemCustom;
			pGaeaServer->SENDTOPARTYCLIENT(PartyID, TenderItem.m_mapID, &NetMsg);
		}
		else
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg(PartyID);
			NetMsg.dwGaeaID  = ResultMessage.dwOwnerGaeaID;
			NetMsg.sNID_ITEM = TenderItem.m_pItemDrop->sItemCustom.GetBoxWrappedItemID();
			pGaeaServer->SENDTOPARTYCLIENT(PartyID, TenderItem.m_mapID, &NetMsg);
		}

		const bool bResult = pGaeaServer->PostSendSystemMail(
			pChar->GetCharID(),
			std::string(pChar->m_szName),
			TenderItem.m_pItemDrop,
			(LONGLONG)0,
			ATTENDANCE_REWARD_POST_DESIGN,
			ATTENDANCE_REWARD_POST_SENDER,
			std::string(ID2SERVERTEXT("PARTY_TENDER_POST_SENDER_NAME")),
			std::string(ID2SERVERTEXT("PARTY_TENDER_POST_TITLE")),
			std::string(ID2SERVERTEXT("PARTY_TENDER_POST_CONTENT"))
			);

		if ( bResult == false )
			return false; // 우편 보내기 실패;

		// 성공적으로 우편을 보냈다면 아이템 삭제;
		GLLandMan* pLandManager = pChar->GetLandMan();
		pLandManager->DropOutItem(TenderItem.m_pItemDrop->dwGlobID);
	}

	return true;
}

