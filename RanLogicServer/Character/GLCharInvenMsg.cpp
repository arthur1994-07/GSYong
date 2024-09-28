#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../RanLogic/ItemPeriodExtension/ItemPeriodExtension.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ProductMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RandomAddon/RandomAddon.h"
#include "../../RanLogic/Skill/GLSkillReset.h"

#include "../../MfcExLib/RanFilter.h"

#include "../Club/GLClubServer.h"
#include "../Party/GLPartyField.h"
#include "../Guidance/GLGuidance.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../Util/GLItemLimit.h"
#include "../Database/DBAction/DbActionGameVehicle.h"
#include "../Database/DBAction/DbActionGamePet.h"
#include "../Database/DBAction/DbActionItemPeriodEx.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionShop.h"
#include "../Vehicle/GLVehicleField.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "./GLChar.h"
#include "../RanLogicServer/TriggerSystem/ServerTriggerSystem.h"
#include "../RanLogicServer/TriggerSystem/ServerFindTriggerMessage.h"
#include "../RanLogicServer/TriggerSystem/ServerPortalOpenTrigger.h"
#include "../RanLogicServer/TriggerSystem/ServerMapInfoMessage.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// �κ��������� �Ҹ��Ŵ.
// 2. �Ҹ� Ŭ���̾�Ʈ�� �˸�.
// 3. b2AGENT ������ Ŭ���̾�Ʈ�� �˸��� �ʵ���� ���Ἲ�� ��Ȯ�� �� ��츦 ����Ͽ�
// ������Ʈ�� ���ؼ� �˸��� �ֵ��� ��.
// DbUpdate ���� ������ 0�̶�� ��񿡼��� �����Ѵ�.
bool GLChar::ConsumeInvenItem( WORD wPosX, WORD wPosY, bool b2AGENT/*= false*/, WORD wNum /*= 1*/, bool bTurnNumCheck /*= false*/, bool DbUpdate /*= true*/ )
{
	//	�κ��� ������ Ȯ��.
	SINVENITEM* pINVENITEM = m_cInventory.FindPosItem( wPosX, wPosY );
	
	if ( !pINVENITEM )
		return false;

	//	������ ���� ������.
	SNATIVEID idItem = pINVENITEM->sItemCustom.GetNativeID();

	const SITEM* pItem = GLogicData::GetInstance().GetItem( idItem );
	if ( !pItem )
		return false;
	
	//
	//mjeon.activity
	//
	ActivityCheckItemUse( idItem.dwID );
	DoQuestItemUse( idItem );

	if ( pItem->sDrugOp.bInstance || ( bTurnNumCheck && pINVENITEM->sItemCustom.wTurnNum > wNum ) )
	{
		if ( pINVENITEM->sItemCustom.wTurnNum >= wNum )
            pINVENITEM->sItemCustom.wTurnNum -= wNum;
	
		GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
		if ( !pTarChar )
		{		
			// ��� �� �α� ����
			// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ConsumeInvenItem ", m_CharDbNum ) );
		}

		// �������� ���� ���� ��� ���
		// ��� �� �α� ����
		LogItemExchange(
            pINVENITEM->sItemCustom,
            gamelog::ID_CHAR, m_CharDbNum,
            gamelog::ID_CHAR, 0,
            ITEM_ROUTE_DELETE, 1, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_INVEN_USE,
			false );

		if ( pINVENITEM->sItemCustom.wTurnNum == 0 )
		{
			// ������ ����.
			InvenDeleteItem( wPosX, wPosY, false, FALSE, CONSUME_INVEN_ITEM, DbUpdate );

			// [�ڽſ���] Inventory�� ������ ����.
			GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
			NetMsg_Inven_Delete.dwChaNum = CharDbNum();
			if ( b2AGENT )
                SendToAgent( &NetMsg_Inven_Delete );
			else
                SendToClient( &NetMsg_Inven_Delete );
		}
		else
		{
			// [�ڽſ���] Inventory ������ �Ҹ��.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.dwChaNum = CharDbNum();
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;

			if ( b2AGENT )
                SendToAgent( &NetMsg_Inven_Update );
			else
                SendToClient( &NetMsg_Inven_Update );
		}
	}

	// ��Ÿ�� ����
	if ( pItem->sBasicOp.IsCoolTime() ) 
	{
		SetCoolTime( pINVENITEM->sItemCustom.GetNativeID() , pItem->sBasicOp.emCoolType );
	}

	return true;
}

bool GLChar::ConsumeSlotItem( EMSLOT emSLOT, WORD wNum /*= 1*/ )
{
	if ( emSLOT >= SLOT_TSIZE )
		return false;

	SITEMCUSTOM& sSLOTITEM = m_PutOnItems[emSLOT];
	SNATIVEID idItem = sSLOTITEM.GetNativeID();

	const SITEM* pItem = GLogicData::GetInstance().GetItem( idItem );
	if ( !pItem )
		return false;

	// ���������� ���Ǿ����� �ȵǾ����� Ȯ������ �ʾҴµ� Ư��Ȱ�� ó��?
	//
	//mjeon.activity
	//
	ActivityCheckItemUse( idItem.dwID );
	DoQuestItemUse( idItem );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ConsumeSlotItem ", m_CharDbNum ) );
	}

    // �Ҹ� �������� ���
	if ( pItem->ISINSTANCE() )
	{
		if ( sSLOTITEM.wTurnNum >= wNum )
            sSLOTITEM.wTurnNum -= wNum;

		// ������ ���� Ȯ���� Ȯ���ϰ� ���� �ʾҴµ�
		// �������� ���� ���� ��� ���
		// ��� �� �α� ����
        LogItemExchange( 
            sSLOTITEM, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, sSLOTITEM.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_INSTANCE_ITEM_BY_SLOT_USE,
			false );

		if ( sSLOTITEM.wTurnNum == 0 )
		{
			//	HOLD ������ ����.
			RELEASE_SLOT_ITEM_SERVER( emSLOT, true );

			//	[�ڽſ���] ���Կ� �־��� ������ ����.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release( emSLOT );
			NetMsg_Release.emType = EMPUTONRELEASE_TYPE_TURN;
			SendToClient( &NetMsg_Release );
		}
		else
		{
			//	[�ڽſ���] �������� �������� Ƚ�� ����.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_Update;
			NetMsg_Update.emSlot = emSLOT;
			NetMsg_Update.wTurnNum = sSLOTITEM.wTurnNum;
			SendToClient( &NetMsg_Update );
		}
	}
	else
	{
		// �ƹ��͵� ���ߴµ�
		// ���� �������� ���
		// ��� �� �α� ����
		LogItemExchange( 
            sSLOTITEM, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, 0, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_SLOT_USE,
			false );
	}

	// ��Ÿ�� ����
	if ( pItem->sBasicOp.IsCoolTime() )
	{
		SetCoolTime( sSLOTITEM.GetNativeID() , pItem->sBasicOp.emCoolType );
	}

	return true;
}

bool GLChar::IsInsertToInven( PITEMDROP pItemDrop, bool Force /*= false*/ )
{
	GASSERT( pItemDrop && "GLChar::IsItemToInven()" );
	if ( !pItemDrop )
		return false;

	return IsInsertToInven( pItemDrop->sItemCustom, Force );
}

bool GLChar::InsertToInven( CItemDrop* pItemDrop, bool bParty, bool bPickMsg, bool bDrop, bool Force /*= false*/, bool bNew /*= false*/ )
{
	// guid üũ
	if ( pItemDrop->sItemCustom.IS_GUID_NULL() )
	{
		GASSERT( 0 && "GLChar::InsertToInven() pItemDrop->sItemCustom.guid is 00000000-0000-0000-0000-000000000000" );
		return false;
	}

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	GASSERT( pItemDrop && "GLChar::InsertToInven()" );
	if ( !pItemDrop )
		return false;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pItemDrop->sItemCustom.GetNativeID() );
	if ( !pItem )
		return false;

	bool bITEM_SPACE = IsInsertToInven( pItemDrop, Force );
	if ( !bITEM_SPACE )
	{
		//	Inventory�� �������� �������� ������ �����ϴ�.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_INVEN_ERROR;
		SendToClient( &NetMsg );
		return false;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertToInven", m_CharDbNum ) );
	}

	if ( pItem->ISPILE() )
	{
		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		// �������� ���� ���� ��� ���
		// ��� �� �α� ����
		LogItemExchange(
			pItemDrop->sItemCustom,
			gamelog::ID_CHAR, 0,
			gamelog::ID_CHAR, m_CharDbNum,
			ITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_TO_INVEN,
			false );

		// �� �������� �ִ� �������� �ִ��� �˻��Ͽ� �κ��� �����ϴ�
		// ������ ���� ���ڸ� ���ҽ�Ŵ.
		const GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
		GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM& sINVENITEM = *( *iter ).second;
			SITEMCUSTOM& sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.GetNativeID() != sNID )
				continue;
			if ( sITEMCUSTOM.wTurnNum >= wPILENUM )
				continue;

			//	���� ������ ��ħ ������.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				// ���� �������� ��ħ�� ����. ( ���� )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				// �� ������ ���� ���游 ��� ������Ʈ�ؾ� �ϳ�?
				// Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				SendToClient( &NetMsg );

				// �ܿ� ���� ����.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				// ���� �����ۿ� ��ġ�� �� �����ε� ���� ���� �������� ����� ��.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				// Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				SendToClient( &NetMsg );

				if ( bParty && m_PartyID.isValidParty() ) 
				{			
					if ( EMITEM_LEVEL_NORMAL < pItem->sBasicOp.emLevel )
					{
						GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD NetMsg( m_PartyID );
						NetMsg.dwGaeaID  = m_dwGaeaID;
						NetMsg.sItem     = pItemDrop->sItemCustom;
						m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
					}
					else
					{
						GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg(m_PartyID);
						NetMsg.dwGaeaID  = m_dwGaeaID;
						NetMsg.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
						m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
					}
				}

				if ( bPickMsg )
				{
					GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
					NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
					SendToClient( &NetMsgPickUp );
					if ( GLHIDESET::CheckHideSet( EMHIDE_PICKUP_ITEM ) )
					{
						RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
						RestoreActStateByInvisible();
					}
				}

				if ( bDrop )
				{
					// ������ Land���� ����.
					pLand->DropOutItem( pItemDrop->dwGlobID );
				}

				return true;
			}
		}

		// ������ �κ��� ������ �������� ���� �ľ��� �κ��� �� ������ �ִ��� �˻�.

		//	�ִ��ħ������ ����.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	���а�ħ�������� ��ħ��.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )
			wITEMNUM += 1;
		if ( wSPLITNUM == 0 && wITEMNUM >= 1 )
			wSPLITNUM = wPILENUM;

		for ( WORD i = 0; i < wITEMNUM; ++i )
		{
			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			BOOL bSPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY, Force );
			if ( !bSPACE )
			{
				sc::writeLogError( sc::string::format( "Char(%1%) inventory - ���� ������ �̸� üũ�� �ϰ� �������� �־����� ������ ������2", CharDbNum() ) );
				return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.
			}

			// ���ο� �������� �־���.
			SITEMCUSTOM sITEMCUSTOM = pItemDrop->sItemCustom;
			if ( wITEMNUM == ( i + 1 ) )
				sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	������ �������� �ܿ���.
			else
				sITEMCUSTOM.wTurnNum = wPILENUM;	//	�ƴ� ���� ������.

			if ( i > 0 )
			{
				// �ű� guid ���� �ʿ�
				// �� ��찡 �Ǹ� ������ ���� ������ �α׸��� ���ο� �������� �����ȰŴ�. 
				sc::SeqUniqueGUID::Instance()->getGUID( sITEMCUSTOM.guid );
				sITEMCUSTOM.GenerateBasicStat( false );
				sITEMCUSTOM.GenerateLinkSkill();
				sITEMCUSTOM.GenerateAddOption();
			}

			// Inventory�� �ֽ��ϴ�.
			if ( !InvenInsertItem( sITEMCUSTOM, wInsertPosX, wInsertPosY, false, INSERTTOINVEN_PDROPITEM_EX, true, Force, true ) )
				return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

			SINVENITEM* pInvenItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )
				return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

			// Inventory�� ������ �־��ִ� �޽���.
			InvenInsertSend( pInvenItem, FALSE, Force, bNew );
		}

		// ��Ƽ������ �˷����Ҷ��� ����
		// ��Ƽ���� ��� �ٸ� ��Ƽ������ ������ ���� �˸�.
		//
		if ( bParty && m_PartyID.isValidParty() ) 
		{			
			if ( EMITEM_LEVEL_NORMAL < pItem->sBasicOp.emLevel )
			{
				GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD NetMsg( m_PartyID );
				NetMsg.dwGaeaID  = m_dwGaeaID;
				NetMsg.sItem     = pItemDrop->sItemCustom;
				m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
			}
			else
			{
				GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg( m_PartyID );
				NetMsg.dwGaeaID  = m_dwGaeaID;
				NetMsg.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
				m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
			}
		}

		// ������ ����� �ڵ� ����Ʈ ���� ����.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.GetNativeID() );

		if ( bPickMsg )
		{
			GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
			NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
			SendToClient( &NetMsgPickUp );
			if ( GLHIDESET::CheckHideSet( EMHIDE_PICKUP_ITEM ) )
			{
				RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
				RestoreActStateByInvisible();
			}
		}

		if ( bDrop )
		{
			// ������ Land���� ����.
			pLand->DropOutItem( pItemDrop->dwGlobID );
		}
	}
	else
	{
		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY, Force );
		if ( !bOk )
		{
			sc::writeLogError( sc::string::format( "Char(%1%) inventory - ���� ������ �̸� üũ�� �ϰ� �������� �־����� ������ ������3", CharDbNum() ) );
			return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.
		}

		// ������ �ݱ� ����.
		if ( !InvenInsertItem( pItemDrop->sItemCustom, wPosX, wPosY, false, INSERTTOINVEN_PDROPITEM_EX, true, Force, bNew ) )
			return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
		if ( !pInvenItem )
			return false;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		// ��Ƽ������ �˷����Ҷ��� ����
		// ��Ƽ���� ��� �ٸ� ��Ƽ������ ������ ���� �˸�.
		if ( m_PartyID.isValidParty() ) 
		{			
			if ( EMITEM_LEVEL_NORMAL < pItem->sBasicOp.emLevel )
			{
				GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD NetMsg( m_PartyID );
				NetMsg.dwGaeaID  = m_dwGaeaID;
				NetMsg.sItem     = pItemDrop->sItemCustom;
				m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
			}
			else
			{
				GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg( m_PartyID );
				NetMsg.dwGaeaID  = m_dwGaeaID;
				NetMsg.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
				m_pGaeaServer->GetPartyManager()->SendMsgToMember( m_PartyID, m_mapID, &NetMsg );
			}
		}

		// �������� ���� ���� ��� ���
		// ��� �� �α� ����
		LogItemExchange( 
			pInvenItem->sItemCustom, 
			gamelog::ID_CHAR, 0, 
			gamelog::ID_CHAR, m_CharDbNum, 
			ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_TO_INVEN,
			false );

		//	[�ڽſ���] �޽��� �߻�.
		InvenInsertSend( pInvenItem, FALSE, Force, bNew );

		// ������ ����� �ڵ� ����Ʈ ���� ����.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.GetNativeID() );

		if ( bPickMsg )
		{
			GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
			NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.GetBoxWrappedItemID();
			SendToClient( &NetMsgPickUp );
			if ( GLHIDESET::CheckHideSet( EMHIDE_PICKUP_ITEM ) )
			{
				RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
				RestoreActStateByInvisible();
			}
		}

		if ( bDrop )
		{
			// ������ Land���� ����.
			pLand->DropOutItem( pItemDrop->dwGlobID );
		}
	}

	return true;
}

void GLChar::DisableDebuffSkillFact()
{
	for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
	{
		if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if (!pSkill)
            continue;

		// ���� �� ��ų ȿ���� ����
		if (pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY)
        {
			UPDATE_DATA_END(i);//RESETSKEFF(i);
        }
    }
}

void GLChar::DisableSkillFact()
{ // ���� ���� �ɼ��� �ִ� ���� ȿ���� �˻� �� ���� ó�� �� �ش� Ŭ���̾�Ʈ �� �ֺ��� �뺸;
	EMSLOT emRHand = GetCurRHand();	
	SITEM* pRightItem = GET_SLOT_ITEMDATA(emRHand);		

	for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
	{
		if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if (!pSkill)
            continue;

		// ��ų �ڽ� ����
		if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_SELF || pSkill->m_sBASIC.emIMPACT_REALM != REALM_SELF )
			continue;

		const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
		SKILL::GLSKILL_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

		// ��ų ���� ���� ����
		if ( emSKILL_RITEM == SKILL::SKILLATT_NOCARE )
			continue;
				
		// ���� ��� �ִ� �������� null�̰ų�, ���� ������ ���� ������ ����;
		// ��, ���� �ƴ� �ٸ� ����� �ɾ��� ������ ���� ���� ����;
		if ( (!pRightItem || !CHECHSKILL_ITEM(emSKILL_RITEM, CONVERT_ITEMATT( pRightItem->sSuitOp.emAttack ), bHiddenWeapon)) && (m_sSKILLFACT[i].sID.GaeaId == GetGaeaID()) )
		{
			UPDATE_DATA_END(i);
		}
	}
}

//************************************
// Method:    InsertMoney
// FullName:  GLChar::InsertMoney
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LONGLONG lnAmount
// Explanation: ����� ��Ƽ���� �ݾ� �й�� ���Ǵ� �Լ��̴�
//************************************
void GLChar::InsertMoney(LONGLONG lnAmount)
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertMoney", m_CharDbNum ) );
	}

	//	�� ����
	lnAmount = CalculateInsertmoney(lnAmount);

	CheckMoneyUpdate(GetInvenMoney(), lnAmount, TRUE, "Insert Party Money.");

	AddInvenMoneyServer( lnAmount, ADD_INVEN_MONEY_PARTY_MONEY_DISTRIBUTION );

	// �ݾ� �α�.
	if (lnAmount > EMMONEY_LOG)
	{
		// ��� �� �α� ����
		LogMoneyExchange(gamelog::ID_CHAR, 0,           gamelog::ID_CHAR, m_CharDbNum, lnAmount,  ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_ADD_FROM_PARTY );
		// ��� �� �α� ����
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0,           GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
	}


	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);
		CString strTemp;
		strTemp.Format(
			"Insert Party Money, [%s][%s], Insert Amount:[%I64d], Have Money:[%I64d]",
			m_szUID, m_szName,  lnAmount, GetInvenMoney() );

		TracingMsg.SetLog(strTemp.GetString());
		SendToAgent(&TracingMsg);
	}*/

    //	�ݾ� ��ȭ Ŭ���̾�Ʈ�� �˷���.
	if (lnAmount != 0)		
		SendInvenMoneyUpdate();

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_GAIN_MONEY NetMsg2;
		NetMsg2.gainMoney = m_lVNGainSysMoney;
		SendToClient(&NetMsg2);
	}
//#endif
}

LONGLONG GLChar::CalculateInsertmoney( LONGLONG lnAmount )
{
	LONGLONG lnMoney = lnAmount;
	//	�� ����.	
	lnMoney = (LONGLONG)(lnMoney * m_pGaeaServer->GetMoneyGainRate(m_CHARINDEX,m_wLevel));

//	IP���ʽ�
//#if defined(TH_PARAM) || defined(MYE_PARAM ) || defined(MY_PARAM) || defined ( PH_PARAM ) //|| defined(_RELEASED)
	if (m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
	{
		if ( m_emIPBonus == EMIP_BONUS_A ) // A���
		{
			lnMoney *= 2;
		}
        else if ( m_emIPBonus == EMIP_BONUS_B ) // B���
		{
			lnMoney = static_cast<LONGLONG>(lnMoney*1.75f);
		}
        else if ( m_emIPBonus == EMIP_BONUS_C ) // C���
		{
			lnMoney = static_cast<LONGLONG>(lnMoney*1.5f);
		}
	}
//#endif

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if ( m_ChinaGainType == GAINTYPE_HALF )
	{
		lnMoney /= 2;
	}else if ( m_ChinaGainType == GAINTYPE_EMPTY )
	{
		lnMoney = 0;
	}
#endif

	//NeedToSaveValue_B
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( m_dwVietnamGainType == GAINTYPE_HALF )
		{
			lnMoney /= 2;
			m_lVNGainSysMoney += lnMoney;
		}
        else if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			m_lVNGainSysMoney += lnMoney;
			lnMoney = 0;		
		}
	}
//#endif

	return lnMoney;
}

bool GLChar::InsertToInven( CMoneyDrop* pMoneyDrop )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertToInven ", m_CharDbNum ) );
	}

	// �� ����
	if ( pMoneyDrop->bDropMonster )
		pMoneyDrop->lnAmount = CalculateInsertmoney( pMoneyDrop->lnAmount );	

	CheckMoneyUpdate( GetInvenMoney(), pMoneyDrop->lnAmount, TRUE, "Insert Money." );

	AddInvenMoneyServer( pMoneyDrop->lnAmount, ADD_INVEN_MONEY_GROUND_MONEY_PICK_UP );
	if ( GLHIDESET::CheckHideSet( EMHIDE_PICKUP_ITEM ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	// �ݾ� �α�
	if ( pMoneyDrop->lnAmount > EMMONEY_LOG )
	{
		// ��� �� �α� ����
		LogMoneyExchange( gamelog::ID_CHAR, 0,           gamelog::ID_CHAR, m_CharDbNum, pMoneyDrop->lnAmount, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_ADD_FROM_LAND );
		// ��� �� �α� ����
		LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0,           GetInvenMoney(),            MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	}

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount( m_szUID );

		CString strTemp;
		strTemp.Format( "Insert Money, [%s][%s], Insert Amount:[%I64d], Have Money:[%I64d]", m_szUID, m_szName, pMoneyDrop->lnAmount, GetInvenMoney() );

		TracingMsg.SetLog( strTemp.GetString() );
		SendToAgent( &TracingMsg );
	}*/
	
	if ( m_ServiceProvider != SP_VIETNAM || ( m_ServiceProvider == SP_VIETNAM && pMoneyDrop->lnAmount != 0 ) )
	{
		GLMSG::SNETPC_PICKUP_MONEY NetMsg;
		NetMsg.lnMoney = GetInvenMoney();
		NetMsg.lnPickUp = pMoneyDrop->lnAmount;
		SendToClient( &NetMsg );
	}

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		GLMSG::SNETPC_VIETNAM_GAIN_MONEY NetMsg2;
		NetMsg2.gainMoney = m_lVNGainSysMoney;
		SendToClient( &NetMsg2 );
	}

	//	���� Land���� ����.
	pLand->DropOutMoney( pMoneyDrop->dwGlobID );

	return true;
}

// Field�� ������,���� Inventory �� �ֱ� �õ�.
//
HRESULT GLChar::MsgReqFieldToInven( NET_MSG_GENERIC* nmg )
{
    bool hrInsertToInven = true;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_FIELD_TO_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_FIELD_TO_INVEN* >( nmg );

	// ���Ȯ��
	if ( !IsValidBody() )
		return E_FAIL;

	if ( pNetMsg->emCrow == CROW_ITEM )
	{
		PITEMDROP pItemDrop = pLand->GetDropItem( pNetMsg->dwID );
		if ( !pItemDrop )
		{
			//	�������� ��������ϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// �������� �ڽŰ� ������ �ʿ� �ִ��� ( ������ ������ ������Ŭ������ �������� ȹ���ϴ� ���� ��ó )		
		if ( pItemDrop->sMapID != m_mapID.getGaeaMapID() )
			return E_FAIL;

		if ( !pItemDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
		{
			//	�������� �ٸ� ������� ����Ǿ� �ֽ��ϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_RESERVED;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pItemDrop->sItemCustom.GetNativeID() );
		if ( !pItem )
			return E_FAIL;

		if ( pItem->sBasicOp.emItemType == ITEM_QITEM )
		{
			TouchQItem( pNetMsg->dwID );
			return S_OK;
		}

		//	�Ÿ� üũ
		D3DXVECTOR3 vPos;

		if ( pNetMsg->bPet )	
		{
			PGLPETFIELD pMyPet = GetMyPet();
			if ( pMyPet )
                vPos = pMyPet->m_vPos;
		}	
		else vPos = m_vPosition;

		const D3DXVECTOR3 &vTarPos = pItemDrop->vPos;

		D3DXVECTOR3 vDistance = vPos - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wTakeAbleDis = wTakeRange + 15;

		if ( fDistance > wTakeAbleDis )
		{
			//	�Ÿ��� �ٴϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

        // �ʵ忡�� �ֿ��� ��ȿ�� üũ( Ŭ����Ŀ ���� ��ó )
		{	
			GLItemMan& glItemMan = GLItemMan::GetInstance();
			SITEMCUSTOM sCUSTOM = pItemDrop->sItemCustom;
			const CTime cTIME_CUR = CTime::GetCurrentTime();
			
			SITEM* pITEM = glItemMan.GetItem( sCUSTOM.GetNativeID() );
			if ( !pITEM )
				return E_FAIL;

			bool bDELETE( false );

			GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
			if ( !pTarChar )
			{
				// ��� �� �α� ����
				// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqFieldToInven ", m_CharDbNum ) );
			}

			if ( sCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
			{			
				CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
				CTime cTIME_LMT( sCUSTOM.tBORNTIME );
				cTIME_LMT += cSPAN + sCUSTOM.GetPeriodExBasic();

				if ( cTIME_CUR > cTIME_LMT )
				{
					//	�ð� �������� ������ ���� �α� ����.
					// ��� �� �α� ����
					LogItemExchange(
                        sCUSTOM,
                        gamelog::ID_CHAR, m_CharDbNum,
                        gamelog::ID_CHAR, 0,
                        ITEM_ROUTE_DELETE, sCUSTOM.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,
						false );

					//	�ð� �������� ������ ���� �˸�.
					GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
					NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.GetNativeID();
					SendToClient( &NetMsgInvenDelTimeLmt );

					bDELETE = true;				

					// �ڽ��� ����.
					if ( sCUSTOM.nidDISGUISE != SNATIVEID( false ) )
					{
						SITEM* pONE = glItemMan.GetItem( sCUSTOM.nidDISGUISE );
						if ( !pONE )
							return E_FAIL;

						SITEMCUSTOM sITEM_NEW( sCUSTOM.nidDISGUISE );
						CTime cTIME = CTime::GetCurrentTime();
						if ( sCUSTOM.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
						{
							cTIME = CTime( sCUSTOM.tDISGUISE );

							CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
							cTIME -= tLMT;
						}

						// �ڽ�Ƭ ������ �ű�. by luxes.
						//
						sITEM_NEW.dwMainColor = sCUSTOM.dwMainColor;
						sITEM_NEW.dwSubColor = sCUSTOM.dwSubColor;

						// �ڽ�Ƭ �ɷ�ġ �ű�
						sITEM_NEW.costumeUser = sCUSTOM.costumeUser;
						sITEM_NEW.cPeriodExtensionCount = sITEM_NEW.cCostumePeriodExtensionCount;
						sITEM_NEW.SetPeriodExBasic(sITEM_NEW.GetPeriodExDisguise());
						sITEM_NEW.ResetPeriodExDisguise();

						// ?
						pItemDrop->sItemCustom.dwMainColor = 0;
						pItemDrop->sItemCustom.dwSubColor = 0;

						// ������ ����.
						//
						sITEM_NEW.tBORNTIME = cTIME.GetTime();
						sITEM_NEW.wTurnNum = 1;
						sITEM_NEW.cGenType = EMGEN_BILLING;
						//sITEM_NEW.cChnID = static_cast< BYTE >( GetServerChannel() );
						//sITEM_NEW.cFieldID = static_cast< BYTE >( GetFieldSvrID() );
						sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
						sITEM_NEW.GenerateBasicStat( false );
						sITEM_NEW.GenerateLinkSkill();
						sITEM_NEW.GenerateAddOption();

						// ������ �ڽ����� �ٴڿ��� �ٽ� ������.
						DropItem( sITEM_NEW, m_vPosition );

						// �и��� �ڽ��� �α�.
						// ��� �� �α� ����
						LogItemExchange(
                            sITEM_NEW,
                            gamelog::ID_CHAR, m_CharDbNum,
                            gamelog::ID_CHAR, 0,
                            ITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum, 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ROUTING_FROM_FIELD,
							false );
					}
					
					pLand->DropOutItem ( pItemDrop->dwGlobID );

					return E_FAIL;
				}
			}
			
			// �߶��� �ִ� �ð��� �ڽ�Ƭ ���� 
			if ( !bDELETE && sCUSTOM.nidDISGUISE != SNATIVEID( false ) )
			{
				if ( sCUSTOM.tDISGUISE != 0 )
				{
					CTime cTIME_LMT( sCUSTOM.tDISGUISE + sCUSTOM.GetPeriodExDisguise());
					if ( cTIME_LMT.GetYear() != 1970 )	// �� �κ� �̷��� �����ʰ� ��ȿ�� �ð����� Ȯ���ϴ� �Լ��� �ϴ���, �װŷ� ��ü �ʿ��ϴ�.
					{
						if ( cTIME_CUR > cTIME_LMT )
						{
							//	�ð� �������� ������ ���� �˸�.
							GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
							NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.nidDISGUISE;
							SendToClient( &NetMsgInvenDelTimeLmt );

							// �ڽ��� ���� ����.
							pItemDrop->sItemCustom.tDISGUISE = 0;
							pItemDrop->sItemCustom.nidDISGUISE = SNATIVEID( false );
							pItemDrop->sItemCustom.ResetPeriodExDisguise();

							// �ڽ�Ƭ ������ ����. by luxes.
							//
							pItemDrop->sItemCustom.dwMainColor = 0;
							pItemDrop->sItemCustom.dwSubColor = 0;

							// �ڽ�Ƭ �ɷ�ġ �ʱ�ȭ
							pItemDrop->sItemCustom.ResetInvest();
						}
					}
				}
			}		
		}

		// ��Ƽ���̰� �̺�Ʈ �������� �ƴҶ� ��Ƽ �������� �˻��� ó��.
		//
		GLPartyField* const pParty = GetMyParty();
		if ( pParty && !( pItemDrop->sItemCustom.IsEvent() != NULL ) )
		{
			const bool ballot = pParty->DOITEM_ALLOT( m_pGaeaServer, m_dwGaeaID, pItemDrop );
			if ( ballot )
                return S_OK;
		}

		// �̺�Ʈ ������ �ߺ����� �������� ����.
		if ( ( pItemDrop->sItemCustom.IsEvent() != NULL ) && m_cInventory.HaveEventItem() )
		{
			//	�̹� �̺�Ʈ �������� ������ �ֽ��ϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// ������ �κ��� ���� ( �κ��� ���� ������ ���� ���� �Ұ���. )
		hrInsertToInven = InsertToInven( pItemDrop, true, true, true, false, true );

/*
		// ȹ���� �������� ��ī�� && ������ ���̶�� �������� �����Ѵ�.
		// �־��̵� 0 �� �ƴϸ� ��ī���̸鼭 DB�� ���� ������ ���̴�.
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pItemDrop->sItemCustom.dwPetID != 0 )
		{
			CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, pItemDrop->sItemCustom.dwPetID );
			IDbManager* pDBMan = m_pGLGaeaServer->GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
*/

	}
	else if ( pNetMsg->emCrow == CROW_MONEY )
	{
		PMONEYDROP pMoneyDrop = pLand->GetDropMoney ( pNetMsg->dwID );
		if ( !pMoneyDrop )
		{
			//	���� ��������ϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		//	�Ÿ� üũ
		D3DXVECTOR3 vPos;

		if ( pNetMsg->bPet )	
		{
			PGLPETFIELD pMyPet = GetMyPet();
			if ( pMyPet )
				vPos = pMyPet->m_vPos;
		}	
		else vPos = m_vPosition;

		const D3DXVECTOR3 &vTarPos = pMoneyDrop->vPos;

		D3DXVECTOR3 vDistance = vPos - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wTakeAbleDis = wTakeRange + 15;

		if ( fDistance > wTakeAbleDis )
		{
			//	�Ÿ��� �ٴϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;
			SendToClient( &NetMsg );
			return E_FAIL;			
		}

		if ( !pMoneyDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
		{
			//	���� �ٸ� ������� ����Ǿ� �ֽ��ϴ�.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_RESERVED;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// ��Ƽ���̰� �̺�Ʈ �������� �ƴҶ� ��Ƽ �������� �˻��� ó��.
		GLPartyField* const pParty = GetMyParty();
		if ( pParty )
		{
			bool ballot = pParty->DOMONEY_ALLOT( m_pGaeaServer, m_dwGaeaID, pMoneyDrop, pLand );
			if ( ballot )
			{
				return S_OK;
			}
		}

		//	�� �ݱ� ����.
		hrInsertToInven = InsertToInven( pMoneyDrop );
	}

    // Note : ĳ���Ͱ� �� �ݱ� ����������, ����ũ Ÿ�� ������ ������.
    if ( !pNetMsg->bPet && hrInsertToInven )
    {
        if ( m_emVehicle != EMVEHICLE_OFF )
        {
			ToggleVehicle( false );
        }
    }

	return S_OK;
}

// Field�� ������,���� �տ� ��� �õ�.	( �տ� �� �������� ���ٰ� ������. )
//
HRESULT GLChar::MsgReqFieldToHold( NET_MSG_GENERIC* nmg )
{
	// ������ �ʴ´�.
	return E_FAIL;

	/*
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_FIELD_TO_HOLD* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_FIELD_TO_HOLD* >( nmg );

	PITEMDROP pItemDrop = pLand->GetDropItem ( pNetMsg->dwGlobID );

	// �ʵ忡�� �������� �տ� ��°��� �κ��� ���� á�� ��� ���̴�.
	// ��Ʈ��Ž���� ��� �տ� ��°� �����ؾ� �Ѵ�.
	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
			return E_FAIL;
	}

	if ( !pItemDrop )
	{
		//	�������� ��������ϴ�.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_OFF;
		SendToClient( &NetMsg );
		return S_FALSE;
	}

	// �������� �ڽŰ� ������ �ʿ� �ִ��� ( ������ ������ ������Ŭ������ �������� ȹ���ϴ� ���� ��ó )
	if ( pItemDrop->sMapID != m_mapID.getGaeaMapID() )
		return S_FALSE;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pItemDrop->sItemCustom.GetNativeID() );
	if ( !pItemData )
		return S_FALSE;

	if ( !pItemDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
	{
		//	�������� �ٸ� ������� ����Ǿ� �ֽ��ϴ�.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_RESERVED;
		SendToClient( &NetMsg );
		return S_FALSE;
	}

	if ( pItemData->sBasicOp.emItemType == ITEM_QITEM )
	{
		TouchQItem( pNetMsg->dwGlobID );
		return S_OK;
	}

	// ��Ƽ���̰� �̺�Ʈ �������� �ƴҶ� ��Ƽ �������� �˻��� ó��.
	//
	GLPartyField* const pParty = GetMyParty();
	if ( pParty && !( pItemDrop->sItemCustom.IsEvent() != NULL ) )
	{
		bool ballot = pParty->DOITEM_ALLOT( m_pGaeaServer, m_dwGaeaID, pItemDrop );
		if ( ballot )
			return S_OK;
	}

	// �̺�Ʈ ������ �ߺ����� �������� ����.
	if ( ( pItemDrop->sItemCustom.IsEvent() != NULL ) && m_cInventory.HaveEventItem() )
	{
		//	�̹� �̺�Ʈ �������� ������ �ֽ��ϴ�.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;
		SendToClient( &NetMsg );
		return S_FALSE;
	}

	// Field�� �־��� �������� �տ� ��.
	HOLD_ITEM( pItemDrop->sItemCustom );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqFieldToHold", m_CharDbNum ) );
	}

	// �������� ���� ���� ��� ���    
	// ��� �� �α� ����
	LogItemExchange(
        pItemDrop->sItemCustom,
        gamelog::ID_CHAR, 0,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_PICKUP_FIELD_ITEM,
		false );
    
	// ������ Land���� ����.
	pLand->DropOutItem ( pItemDrop->dwGlobID );

	return S_OK;
	*/
}

// Inventory�� �������� �տ� ��� �õ�.
//
HRESULT GLChar::MsgReqInvenToHold(NET_MSG_GENERIC* nmg)
{
	// �ÿ���� �ʴ´�.
	return E_FAIL;
	/*
	GLMSG::SNETPC_REQ_INVEN_TO_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_INVEN_TO_HOLD*> ( nmg );
	
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_INVEN_TO_HOLD,EMHOLD_FB_OFF);

	//	��û�� ������ ��ȿ�� �˻�.
	SINVENITEM *pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	�տ� �� �������� ������.
	if ( HaveHoldItem() )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pITEM )
		return S_FALSE;

	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && m_bGetPetFromDB )
	{
		sc::writeLogError(sc::string::format("PetCard Inven to Hold Error, m_bGetPetFromDB : %d",
			m_bGetPetFromDB ));


		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	Inventory�� �ִ� ������ ���.
	SINVENITEM sInven_BackUp = *pInvenItem;

	// Inventory���� ���� �������� ����.
    InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, true, FALSE);

	//	Inventory�� �־��� �������� �տ� ��.
	HOLD_ITEM(sInven_BackUp.sItemCustom);

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqInvenExInven( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_EX_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_INVEN_EX_INVEN* >( nmg );

    if ( pNetMsg->wPosX == pNetMsg->wHoldPosX &&
        pNetMsg->wPosY == pNetMsg->wHoldPosY )
        return S_FALSE;

	GLLandMan* const pLand( GetLandMan() );
	if ( !pLand )
		return E_FAIL;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
	{
		return S_FALSE;
	}

	SINVENITEM *pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

    // �� �ڵ� ����� ������ ���� �ȴ�.
    // �̵��� �� ���� �κ����� �ű��� �������� ����Ǵ� ������ �־���. �ϴ� ���ϰ� �̵��� �� �� ������ ���� �Ͽ���.
    if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wHoldPosY )
        if ( m_cInventory.IsInsertable( pNetMsg->wPosX, pNetMsg->wPosY ) == FALSE )
            return S_FALSE;

	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wPosY )
		if ( m_cInventory.IsInsertable( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) == FALSE )
			return S_FALSE;

	SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory�� �ִ� ������ ���.
	
	WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
	WORD wPosY = sInven_BackUp.wPosY;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( sInven_BackUp.sItemCustom.bVietnamGainItem )
			return S_FALSE;
	}

	const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( sHold_BackUp.CustomItem().GetNativeID() );
	if ( !pInvenData || !pHoldData )
		return S_FALSE;

	if ( pInvenData->sBasicOp.emItemType == ITEM_PET_CARD && m_bGetPetFromDB )
	{
		sc::writeLogError( sc::string::format( "PetCard Inven Ex Hold Error, m_bGetPetFromDB : %d",
			m_bGetPetFromDB ) );

		return S_FALSE;
	}

	// ���� �Ҽ� �ִ� ���������� ����.
	bool bMERGE( false );
	bMERGE = ( pInvenData->sBasicOp.sNativeID == pHoldData->sBasicOp.sNativeID );
	if ( bMERGE )
		bMERGE = ( pInvenData->ISPILE() );
	if ( bMERGE )
		bMERGE = ( sHold_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
		bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sHold_BackUp.CustomItem().wTurnNum;
		
		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
			// ��ħ �� ����.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

            //	Note : ��� �ִ� ������ ����.
            InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, FALSE, 190, true );

			// �κ��� ������ ������Ʈ.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			SendToClient( &NetMsgInvenItem );
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	���� ������ ���� ����.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	�տ� ��� �ִ� ������ ����.
			pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

            // �κ��� ������ ������Ʈ.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgHoldInvenItem;
            NetMsgHoldInvenItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgHoldInvenItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgHoldInvenItem.sItemCustom = pHoldInvenItem->sItemCustom;
            SendToClient( &NetMsgHoldInvenItem );
			
			// �κ��� ������ ������Ʈ.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			SendToClient( &NetMsgInvenItem );
		}

		return S_OK;
	}

    SINVENITEM* pINSERT_INVEN_ITEM = NULL;
    SINVENITEM* pINSERT_HOLD_ITEM = NULL;

	/*
	// ���� ����� �߻������� �켱 �׽�Ʈ �غ���.
	if ( !InvenSwapTestInvenItem( pInvenItem, pHoldInvenItem ) )
	{
		return E_FAIL;
	}

	// ���� ��� ���
	GLInventory BackupInventory;
	BackupInventory.Assign( m_cInventory );
	*/

	// #item
	// inven(Inven) <-> inven(Hold)
	// inven ������(Inven) wPosX, wPosY, sInven_BackUp
	// inven ������(Hold) wHoldPosX, wHoldPosY, sHold_BackUp

	// ������ ���� ó��
	{
		if ( !m_cInventory.DeleteItem( wPosX, wPosY ) )
		{
			// �׳� �ƹ��͵� ���߱� ������ ���� ó�� �� �͵� ����.
			return S_FALSE;
		}

		if ( !m_cInventory.DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY ��ġ�� �ִ� ������ ó��
			if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				InvenDeleteSend( wPosX, wPosY, FALSE );

				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// �켱 �ٴڿ� ����߸���, ���������� �������� �ʾҴٸ� ���� ������ ó�� ��Ͽ� ����.
				// ����� ������ ����� ���� ��������, ���� ���ͼ� �ŷ��Ұ� �������� �ŷ��ϴ� ���¡�� �߻��ϸ�
				// �������� ������ ����� �̿�����.
				DropItem( sInven_BackUp.sItemCustom, m_vPosition );
			}

			return S_FALSE;
		}
	}

	// ������ ���� ó��
	// ������ ���� ���� ���δ� Ȯ������ �ʴ´�. �������� ���� �������� Ȯ�� �����ϰ�
	// �κ��丮�� ��� �����̾� ���ο� ������ �־��־����� ���� �׷��� ���� �ʴ´�.
	// ������ ���������� �����Ǿ��ٸ� ������ �������� ���Եȴ�. 
	{
		if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// ������ ������ ������ �˸�
			InvenDeleteSend( wPosX, wPosY, FALSE );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			// wPosX, wPosY ��ġ�� �ִ� ������ ������ ó��
			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sInven_BackUp.sItemCustom, m_vPosition );

			// wHoldPosX, wHoldPosY ��ġ�� �ִ� ������ ������ ó��
			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sHold_BackUp.sItemCustom, m_vPosition );

			return S_FALSE;
		}

		if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
		{
			// ������ ������ ������ �˸�
			InvenDeleteSend( wPosX, wPosY, FALSE );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold �������� �̹� ���������� ���ԵǾ� �ִ� ��Ȳ(wPosX wPosY ��ġ��)
			// Hold ��ġ�� ������ ������ ������ ��Ȳ. �̹� ���������� ���ԵǾ� �ִ� ������(Hold ������) �˸�
			SINVENITEM* pTempInsertedItem = m_cInventory.GetItem( wPosX, wPosY );
			if ( !pTempInsertedItem )
			{
				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert inven item fail, and getitem fail, disconnect user. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				NET_DISCONNECT_CHARACTER NetMsg;
				NetMsg.ChaDbNum = m_CharDbNum;
				m_pGaeaServer->SENDTOAGENT( &NetMsg );
			}
			else
			{
				InvenInsertSend( pTempInsertedItem, FALSE );
			}

			DropItem( sInven_BackUp.sItemCustom, m_vPosition, pTempInsertedItem == NULL ? false : true );

			return S_FALSE;
		}

		// ������ Ȯ��
		pINSERT_INVEN_ITEM = m_cInventory.GetItem( wPosX, wPosY );
		pINSERT_HOLD_ITEM = m_cInventory.GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_INVEN_ITEM || !pINSERT_HOLD_ITEM )
		{
			// ������ ���� �α׸� Ȯ���ص� ���Ե� �������� NULL �� �α״� ������.
			// �̷� ��찡 �߻��� ��� ���� ���� ��Ų��. ĳ���Ͱ� ������ �ٷ� ���������� �����߱� ������ ����� ���̴�.

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven result item null, disconnect user. chanum: %1% guid: %2% guid: %3%", 
				m_CharDbNum, 
				sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str(), 
				sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			NET_DISCONNECT_CHARACTER NetMsg;
			NetMsg.ChaDbNum = m_CharDbNum;
			m_pGaeaServer->SENDTOAGENT( &NetMsg );

			return S_FALSE;
		}
	}

	// ��� ������Ʈ
	InvenUpdateItem( pINSERT_INVEN_ITEM, true );
	InvenUpdateItem( pINSERT_HOLD_ITEM, true );

	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pINSERT_INVEN_ITEM;

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsg_Del_Insert );
	SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

    NetMsg_Del_Insert = GLMSG::SNETPC_INVEN_DEL_AND_INSERT();
    NetMsg_Del_Insert.wDelX = sHold_BackUp.wPosX;
    NetMsg_Del_Insert.wDelY = sHold_BackUp.wPosY;
    NetMsg_Del_Insert.sInsert = *pINSERT_HOLD_ITEM;

    msgpack::sbuffer SendBuffer2;
    msgpack::pack( SendBuffer2, NetMsg_Del_Insert );
    SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer2 );

    //	��ȿ�Ⱓ �ʰ� ����.	
    HRESULT hrCheckDuration1 = CheckItemDuration( *pINSERT_INVEN_ITEM );
    HRESULT hrCheckDuration2 = CheckItemDuration( *pINSERT_HOLD_ITEM );
    if ( hrCheckDuration1 == E_FAIL || hrCheckDuration2 == E_FAIL )
        return E_FAIL;

	return S_OK;
}

// ������Ŀ(HoldItem) <-> �κ��丮
// ������Ŀ�� �������� �� �κ��丮 �����۰� ��ȯ
HRESULT GLChar::MsgReqStorageExInven( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

    GLMSG::SNETPC_REQ_STORAGE_EX_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_STORAGE_EX_INVEN* >( nmg );
    GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB,EMHOLD_FB_OFF );

    if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
    {
        SendToClient( &NetMsg_fb );
        return E_FAIL;
    }

    if ( !m_bUsedStorageCard )
    {
        // â������ ��ȿ�� üũ
        GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
        if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
        {
            return E_FAIL;
        }
    }

    if ( !IsKEEP_STORAGE( pNetMsg->dwChannel ) )
    {
        NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
        SendToClient( &NetMsg_fb );
        return S_FALSE;
    }

    //	â�������� DB���� �������� �ʾ�����.
    if ( !m_bServerStorage )
        LoadStorageFromDB();

    const DWORD dwChannel = pNetMsg->dwChannel;

    SINVENITEM* pHoldInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

    if ( !pHoldInvenItem )
    {
        SendToClient( &NetMsg_fb );
        return S_FALSE;
    }

    if ( m_ServiceProvider == SP_VIETNAM )
    {
        if ( pHoldInvenItem->CustomItem().bVietnamGainItem  )
            return FALSE;
    }

    const SITEM* pHoldData = GLogicData::GetInstance().GetItem( pHoldInvenItem->CustomItem().GetNativeID() );
    if ( !pHoldData )
    {
        SendToClient( &NetMsg_fb );
        return E_FAIL;
    }

    // ��ī���ϰ�� ���� Ȱ�����̸�
    if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
    {
        PGLPETFIELD pMyPet = GetMyPet();
        if ( pMyPet && pMyPet->IsValid () && pMyPet->DbNum() == pHoldInvenItem->CustomItem().PetDbNum() )
            return FALSE;
    }

    SINVENITEM *pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pInvenItem )
    {
        return S_FALSE;
    }

	// �����̾� ���� ��ȿ�Ⱓ�� �����ٸ� �׳� �����Ѵ�. �׷��� ������ �ٴڿ� �������� ��������.
	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wPosY )
	{
		return S_FALSE;
	}

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory�� �ִ� ������ ���.

    WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
    WORD wPosY = sInven_BackUp.wPosY;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

    if ( m_ServiceProvider == SP_VIETNAM )
    {
        if ( sInven_BackUp.sItemCustom.bVietnamGainItem )
            return S_FALSE;
    }

    const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );

    if ( !pInvenData )
        return S_FALSE;

    // ��� �ִ� �������� �ƴ�, â�� �� �����ۿ� ���ؼ�, �ŷ� �������� ���θ� �˻�
    // �ŷ��ɼ�
	// â�� �����Ҷ��� �ŷ��ɼ��� �ƴ϶� ���� �߰��� â�� �ɼ��� �˻�
	if ( sInven_BackUp.sItemCustom.IsNotStorage() )
		return FALSE;

    if ( pInvenData->sBasicOp.emItemType == ITEM_PET_CARD && m_bGetPetFromDB )
    {
        sc::writeLogError( sc::string::format( "PetCard Inven Ex Hold Error, m_bGetPetFromDB : %d",
            m_bGetPetFromDB ) );

        return S_FALSE;
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageExInven", m_CharDbNum ) );
	}

    // ���� �Ҽ� �ִ� ���������� ����.
    bool bMERGE( false );
    bMERGE = ( pInvenData->sBasicOp.sNativeID == pHoldData->sBasicOp.sNativeID );
    if ( bMERGE )
		bMERGE = ( pInvenData->ISPILE() );
    if ( bMERGE )
		bMERGE = ( sHold_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
    if ( bMERGE )
		bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
    if ( bMERGE )
    {
        WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sHold_BackUp.CustomItem().wTurnNum;

        if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
        {
            // �������� ���� ���� ��� ���
			// ��� �� �α� ����
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_USER, m_dwUserID, 
                gamelog::ID_CHAR, m_CharDbNum, 
                ITEM_ROUTE_CHAR, sHold_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
				false );

            // ��ħ �� ����.
            pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			// Note : ��� �ִ� ������ ����. [�ڽſ���] �޽��� �߻�.	- â���� ������ ����.
			UserStorageDeleteItem( dwChannel, sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, 1, true  );

            // �κ��� ������ ������Ʈ.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
            NetMsgInvenItem.wPosX = pNetMsg->wPosX;
            NetMsgInvenItem.wPosY = pNetMsg->wPosY;
            NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
            SendToClient( &NetMsgInvenItem );
        }
        else
        {
            WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

            // �������� ���� ���� ��� ���
			// ��� �� �α� ����
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_USER, m_dwUserID, 
                gamelog::ID_CHAR, m_CharDbNum, 
                ITEM_ROUTE_CHAR, pInvenData->sDrugOp.wPileNum - sInven_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
				false );

            //	���� ������ ���� ����.
            pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

            //	�տ� ��� �ִ� ������ ����.
            pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

            // â�� Ȧ�� ��ġ ������ ������Ʈ.
            GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
            NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
            NetMsgStorageItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgStorageItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgStorageItem.wTurnNum = pHoldInvenItem->sItemCustom.wTurnNum;
            SendToClient( &NetMsgStorageItem );

            // �κ��� ������ ������Ʈ.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
            NetMsgInvenItem.wPosX = pNetMsg->wPosX;
            NetMsgInvenItem.wPosY = pNetMsg->wPosY;
            NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
            SendToClient( &NetMsgInvenItem );
        }

        return S_OK;
    }

    SINVENITEM* pINSERT_INVEN_ITEM = NULL;
    SINVENITEM* pINSERT_HOLD_ITEM = NULL;

	// #item
	// inven(Inven) <-> user locker(Hold)
	// inven ������ wPosX, wPosY, sInven_BackUp
	// user locker ������ wHoldPosX, wHoldPosY, sHold_BackUp

	// ������ ���� ó��
	{
		if ( !m_cInventory.DeleteItem( wPosX, wPosY ) )
		{
			// �׳� �ƹ��͵� ���߱� ������ ���� ó�� �� �͵� ����.
			return S_FALSE;
		}

		if ( !m_cStorage[dwChannel].DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY ��ġ�� �ִ� ������ ó��
			if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				InvenDeleteSend( wPosX, wPosY, FALSE );

				sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// �켱 �ٴڿ� ����߸���, ���������� �������� �ʾҴٸ� ���� ������ ó�� ��Ͽ� ����.
				// ����� ������ ����� ���� ��������, ���� ���ͼ� �ŷ��Ұ� �������� �ŷ��ϴ� ���¡�� �߻��ϸ�
				// �������� ������ ����� �̿�����.
				DropItem( sInven_BackUp.sItemCustom, m_vPosition );
			}

			return S_FALSE;
		}
	}

	// ������ ���� ó��
	// ������ ���� ���� ���δ� Ȯ������ �ʴ´�. �������� ���� �������� Ȯ�� �����ϰ�
	// �κ��丮�� ��� �����̾� ���ο� ������ �־��־����� ���� �׷��� ���� �ʴ´�.
	// ������ ���������� �����Ǿ��ٸ� ������ �������� ���Եȴ�. 
	{
		if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// ������ ������ ������ �˸�
			InvenDeleteSend( wPosX, wPosY, FALSE );
			UserStorageDeleteSend( dwChannel, wHoldPosX, wHoldPosY );

			// wPosX, wPosY ��ġ�� �ִ� ������ ������ ó��
			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sInven_BackUp.sItemCustom, m_vPosition );

			// wHoldPosX, wHoldPosY ��ġ�� �ִ� ������ ������ ó��
			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sHold_BackUp.sItemCustom, m_vPosition );

			return S_FALSE;
		}

		if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
		{
			// ������ ������ ������ �˸�
			InvenDeleteSend( wPosX, wPosY, FALSE );
			UserStorageDeleteSend( dwChannel, wHoldPosX, wHoldPosY );

			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold �������� �̹� ���������� ���ԵǾ� �ִ� ��Ȳ(wPosX wPosY ��ġ��)
			// Hold ��ġ�� ������ ������ ������ ��Ȳ. �̹� ���������� ���ԵǾ� �ִ� ������(Hold ������) �˸�
			SINVENITEM* pTempInsertedItem = m_cInventory.GetItem( wPosX, wPosY );
			if ( !pTempInsertedItem )
			{
				sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert inven item fail, and getitem fail, disconnect user. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				NET_DISCONNECT_CHARACTER NetMsg;
				NetMsg.ChaDbNum = m_CharDbNum;
				m_pGaeaServer->SENDTOAGENT( &NetMsg );
			}
			else
			{
				InvenInsertSend( pTempInsertedItem, FALSE );
			}

			DropItem( sInven_BackUp.sItemCustom, m_vPosition, pTempInsertedItem == NULL ? false : true );

			return S_FALSE;
		}

		// ������ Ȯ��
		pINSERT_INVEN_ITEM = m_cInventory.GetItem( wPosX, wPosY );
		pINSERT_HOLD_ITEM = m_cStorage[dwChannel].GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_INVEN_ITEM || !pINSERT_HOLD_ITEM )
		{
			// ������ ���� �α׸� Ȯ���ص� ���Ե� �������� NULL �� �α״� ������.
			// �̷� ��찡 �߻��� ��� ���� ���� ��Ų��. ĳ���Ͱ� ������ �ٷ� ���������� �����߱� ������ ����� ���̴�.

			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven result item null, disconnect user. chanum: %1% guid: %2% guid: %3%", 
				m_CharDbNum, 
				sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str(), 
				sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			NET_DISCONNECT_CHARACTER NetMsg;
			NetMsg.ChaDbNum = m_CharDbNum;
			m_pGaeaServer->SENDTOAGENT( &NetMsg );

			return S_FALSE;
		}
	}

    // �������� ���� ���� ��� ���
	// ��� �� �α� ����
    LogItemExchange( 
        sHold_BackUp.sItemCustom, 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, sHold_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
		false );


    // �������� ���� ���� ��� ���
	// ��� �� �α� ����
    LogItemExchange( 
        sInven_BackUp.sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_USER, m_dwUserID, 
        ITEM_ROUTE_USER_INVEN, sInven_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
		false );

    GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
    NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;							//	������ ������.
    NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
    NetMsg_Del_Insert.sInsert = *pINSERT_INVEN_ITEM;						//	���ԵǴ� ������.

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsg_Del_Insert );
    SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

    GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Storage_Del_Insert;
    NetMsg_Storage_Del_Insert.dwChannel = dwChannel;
    NetMsg_Storage_Del_Insert.wDelX = sHold_BackUp.wPosX;					//	������ ������.
    NetMsg_Storage_Del_Insert.wDelY = sHold_BackUp.wPosY;
    NetMsg_Storage_Del_Insert.sInsert = *pINSERT_HOLD_ITEM;					//	���ԵǴ� ������.
    SendToClient( &NetMsg_Storage_Del_Insert );

    // ��ȿ�Ⱓ �ʰ� ����.	
    HRESULT hrCheckDuration1 = CheckItemDuration( *pINSERT_INVEN_ITEM );
    if ( hrCheckDuration1 == E_FAIL  )
        return E_FAIL;

    return S_OK;
}

HRESULT GLChar::MsgReqInvenToSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_SLOT* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_INVEN_TO_SLOT* >( nmg );
	if ( pNetMsg->emToSlot >= SLOT_TSIZE )
        return S_FALSE;

    if ( m_sCONFTING.IsCONFRONTING() )
        return S_FALSE;

    if ( m_emVehicle == EMVEHICLE_PASSENGER && pNetMsg->emToSlot == SLOT_VEHICLE )	
        return S_FALSE;

    if ( pNetMsg->emToSlot== SLOT_VEHICLE && m_bGetVehicleFromDB )
    {
        sc::writeLogError( sc::string::format( "Vehilce Slot Insert Error, m_bGetVehicleFromDB : %1%", m_bGetVehicleFromDB ) );
        return S_FALSE;
    }

	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
        return E_FAIL;

    SITEMCUSTOM sInvenItem = pInvenItem->sItemCustom;

	BOOL bOk( FALSE );

	//	�ڰ����ǿ� �����ϴ��� �˻�;
	if ( !ACCEPT_ITEM( sInvenItem ) )
		return E_FAIL;

	//	�ش� ���Կ� ���� �������� �˻�.
	bOk = EMSLOTCHECK_OK == CHECKSLOT_ITEM( sInvenItem.GetNativeID(), pNetMsg->emToSlot );
    if ( !bOk )
		return E_FAIL;

    bOk = IsCurUseArm( pNetMsg->emToSlot );
    if ( !bOk )
		return E_FAIL;

    // ����ũ Ÿ���϶� �ε����̶��, �ؿ� ���� �����Ͱ�����
	const SNATIVEID sItemNID( pInvenItem->sItemCustom.GetNativeID() );
    const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sItemNID );
    if ( !pInvenData )
		return E_FAIL;

    // �κ��� �־���� ������
    SITEMCUSTOM sToInvenItems[SLOT_TSIZE];
    EMSLOT      emToInvenSlot[SLOT_TSIZE];
    UINT        nToInvenNums = 0;

    // �ش� ������ ������
    SITEMCUSTOM sSlotItem = GET_SLOT_ITEM( pNetMsg->emToSlot );

    // �ش� ���Կ� �������� �ִٸ�
    if ( sSlotItem.GetNativeID() != NATIVEID_NULL() )
    {
        if ( !VALID_SLOT_ITEM( pNetMsg->emToSlot ) )
			return E_FAIL;

        const SITEM* pSlotData = GLogicData::GetInstance().GetItem( sSlotItem.GetNativeID() );

        // ���� �Ҽ� �ִ� ���������� ����.
        bool bMERGE( false );
        bMERGE = ( pInvenData->sBasicOp.sNativeID == pSlotData->sBasicOp.sNativeID );
        if ( bMERGE)
			bMERGE = ( pInvenData->ISPILE() );
        if ( bMERGE)
			bMERGE = ( sSlotItem.wTurnNum < pInvenData->sDrugOp.wPileNum );
        if ( bMERGE)
			bMERGE = ( sInvenItem.wTurnNum < pInvenData->sDrugOp.wPileNum );
        if ( bMERGE )
        {
            WORD wTurnNum = sInvenItem.wTurnNum + sSlotItem.wTurnNum;

            if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
            {
                // �κ��� ������ ����
                InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, true );

                // ������ ������ ������Ʈ
                sSlotItem.wTurnNum = wTurnNum;
                SLOT_ITEM_SERVER( sSlotItem, pNetMsg->emToSlot );

                GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
                NetMsgPutOn.m_Slot = pNetMsg->emToSlot;
                NetMsgPutOn.m_Item = sSlotItem;
                msgpack::sbuffer SendBuffer;
                msgpack::pack( SendBuffer, NetMsgPutOn );
                SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBuffer );
            }
            else
            {
                // �κ��� ������ ������Ʈ.
                pInvenItem->sItemCustom.wTurnNum = wTurnNum - pInvenData->sDrugOp.wPileNum;

                GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
                NetMsgInvenItem.wPosX       = pNetMsg->wPosX;
                NetMsgInvenItem.wPosY       = pNetMsg->wPosY;
                NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
                SendToClient( &NetMsgInvenItem );

                // ������ ������ ������Ʈ
                sSlotItem.wTurnNum = pInvenData->sDrugOp.wPileNum;
                SLOT_ITEM_SERVER( sSlotItem, pNetMsg->emToSlot );

                GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
                NetMsgPutOn.m_Slot = pNetMsg->emToSlot;
                NetMsgPutOn.m_Item = sSlotItem;
                msgpack::sbuffer SendBuffer;
                msgpack::pack( SendBuffer, NetMsgPutOn );
                SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBuffer );
            }

            return S_OK;
        }

        sToInvenItems[ nToInvenNums ] = GET_SLOT_ITEM( pNetMsg->emToSlot );
        emToInvenSlot[ nToInvenNums ] = pNetMsg->emToSlot;
        nToInvenNums++;
    }

    //	�տ� ��� ������ ��� �ٸ� ���� �������� �˻��Ѵ�
    if ( pInvenData->sSuitOp.emSuit == SUIT_HANDHELD )
    {
        EMSLOT emToHand      = pNetMsg->emToSlot == GetCurLHand() ? GetCurLHand() : GetCurRHand ();
        EMSLOT emAnotherHand = pNetMsg->emToSlot == GetCurLHand() ? GetCurRHand() : GetCurLHand ();

        const SITEM* pITEM_TO = pInvenData;
        SITEM* pITEM_ANOTHER = NULL;

        if ( VALID_SLOT_ITEM( emAnotherHand ) )  
            pITEM_ANOTHER = GLogicData::GetInstance().GetItem( GET_SLOT_ITEM( emAnotherHand ).GetNativeID() );

        if ( pITEM_ANOTHER )
        {
            BOOL bSlotRelease = true;

			const EMCHARINDEX emClass = GETCHARINDEX();
            switch ( pITEM_ANOTHER->sBasicOp.emItemType )
            {
                // �ٸ� ������ �Ҹ� �������ϰ��
			case ITEM_ARROW:
				{
					if ( GLCI_ARCHER_M == emClass || GLCI_ARCHER_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_BOW:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

			case ITEM_CHARM:
				{
					if ( GLCI_SPIRIT_M == emClass || GLCI_SPIRIT_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_SPEAR:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

			case ITEM_BULLET: 
				{
					if ( GLCI_SCIENTIST_M == emClass || GLCI_SCIENTIST_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_PISTOL:
						case ITEMATT_RAILGUN:
						case ITEMATT_PORTALGUN:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

			case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
				{
					if ( GLCI_ACTOR_M == emClass || GLCI_ACTOR_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_ACTOR_HAMMER:
						case ITEMATT_ACTOR_SHIELD:
						case ITEMATT_ACTOR_UMBRELLA:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

                // �ٸ� ������ ������ ���
            case ITEM_SUIT:
                {
                    // �Ѵ� �Ѽ� ������
                    if ( !ISBOTH_HANDHELD( pITEM_TO ) && !ISBOTH_HANDHELD( pITEM_ANOTHER ) )
                    {
                        bSlotRelease = false;
                    }
                    else
                    {
                        switch ( pITEM_TO->sBasicOp.emItemType )
                        {
                        // Ÿ���� �Ҹ��̶��
                        // �ٸ����Ⱑ ��ȿ���� �˻�� CHECKSLOT_ITEM���� �Ѵ�.
                        case ITEM_ARROW:
                        case ITEM_CHARM:
                        case ITEM_BULLET: 
						case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
                            {
                                bSlotRelease = false;
                            }
                            break;
                        };
                    }
                }
                break;
            }


            if ( bSlotRelease ) 
            {
                sToInvenItems[ nToInvenNums ] = GET_SLOT_ITEM( emAnotherHand );
                emToInvenSlot[ nToInvenNums ] = emAnotherHand;
                nToInvenNums++;
            }
        }
    }

	WORD wInvenPosX = 0;
	WORD wInvenPosY = 0;

    // �κ��� �� ���� ����
    DWORD dwEmptyCount = m_cInventory.GetAmountEmpty( FALSE );

    WORD wVALIDY = m_cInventory.GetValidCellY( FALSE );

    // �����Ϸ��� �����ۿ�, �������� �������� ���� �� �ִٸ�, �� ���� ������ +1 �� ȿ���̴�
    if ( pNetMsg->wPosY < wVALIDY )
    {
        dwEmptyCount += 1;
    }

    // �ʿ��� �κ��� ĭ ����, �����Ϸ��� �������� ������ �� ���ٸ� ����
    if ( dwEmptyCount < nToInvenNums )
    {
        return S_FALSE;
    }

    //	Inventory���� ������ ����
    InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, false );

    // �κ��� ������ ����.
    for ( UINT i=0; i<nToInvenNums; ++i )
    {
        const SITEM* pItem = GLogicData::GetInstance().GetItem( sToInvenItems[i].GetNativeID() );

        WORD wInvenPosX = 0;
        WORD wInvenPosY = 0;

        // ó���� �κ��� ��ġ�� ��ȯ �Ѵ�.
        if ( i == 0 )
        {
            wInvenPosX = pNetMsg->wPosX;
            wInvenPosY = pNetMsg->wPosY;

            BOOL bOk = m_cInventory.IsInsertable( wInvenPosX, wInvenPosY );

            if ( !bOk )
            {
                sc::writeLogError( std::string( "GLChar::MsgReqInvenToSlot(), m_cInventory.IsInsertable() is failed(Replace)" ) );

                bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
                if ( !bOk )
                {
                    sc::writeLogError( std::string( "GLChar::MsgReqInvenToSlot(), m_cInventory.FindInsertable() not found empty InvenPos(Replace)" ) );
                    continue;
                }
            }
        }
        else
        {
            BOOL bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
            if ( !bOk )
            {
                sc::writeLogError( std::string( "GLChar::MsgReqInvenToSlot(), m_cInventory.FindInsertable() not found empty InvenPos" ) );
                break;
            }
        }

        if ( emToInvenSlot[i] == SLOT_VEHICLE )
        {
            m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, false );		
        }

        //	[��ο���] �ڽ��� ���� ������ �ٲ��� �˸�.
        RELEASE_SLOT_ITEM_SERVER( emToInvenSlot[i] );

        GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutRelease( emToInvenSlot[i] );
        SendToClient( &NetMsg_PutRelease );

        GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutRelease_Brd( emToInvenSlot[i] );
		NetMsg_PutRelease_Brd.dwGaeaID = m_dwGaeaID;
        SendMsgViewAround( &NetMsg_PutRelease_Brd );

        InvenInsertItem( sToInvenItems[i], wInvenPosX, wInvenPosY, true, 1, false );
		
		// ���� ��ġ �� ��ǥ ��� ������Ʈ
		InvenUpdateItem( m_cInventory.GetItem( wInvenPosX, wInvenPosY ), true );
    }

	if ( sInvenItem.IsTempEnchant())
	{
		const CTime cTIME_CUR = CTime::GetCurrentTime();

		CTime cTimeLimit( sInvenItem.sTLGrind.tFireTime );

		if ( cTIME_CUR > cTimeLimit )
		{
			GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

			netMsgFb.eResult   = EMREQ_TEMP_ENCHANT_FIRETIME;
			netMsgFb.fireItemID = sInvenItem.GetNativeID();
			SendToClient( &netMsgFb );

			// ��ȭ����
			sInvenItem.TempEnchantRecovery();

			// �α�
			LogItemExchange( sInvenItem, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );
		}
	}

    //	���Կ� ������ ����.
    SLOT_ITEM_SERVER( sInvenItem, pNetMsg->emToSlot );
	
	// ���� ��ġ �� ��ǥ ��� ������Ʈ
	SlotUpdateItem( sInvenItem, pNetMsg->emToSlot, true );

	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.m_Slot = pNetMsg->emToSlot;
	NetMsg_PutOn.m_Item = sInvenItem;
    msgpack::sbuffer SendBufferB;
    msgpack::pack( SendBufferB, NetMsg_PutOn );
	SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBufferB );

    GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsg_PutOn_Brd;
    NetMsg_PutOn_Brd.m_GaeaID = m_dwGaeaID;
    NetMsg_PutOn_Brd.m_Slot   = pNetMsg->emToSlot;
    NetMsg_PutOn_Brd.m_Item.Assign( sInvenItem );
    msgpack::sbuffer SendBufferC;
    msgpack::pack( SendBufferC, NetMsg_PutOn_Brd );
    SendMsgViewAround( NET_MSG_GCTRL_PUTON_UPDATE_BRD, SendBufferC );

    //	������ ����� �� �� ȯ�� �ʿ���.
    INIT_DATA( FALSE, FALSE );

    // ���⿡ ���� ������ �ʱ�ȭ �Ѵ�.
    DisableSkillFact();

	return S_OK;
}

HRESULT GLChar::MsgReqSlotToInven( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_SLOT_TO_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_SLOT_TO_INVEN* >( nmg );

    if ( pNetMsg->emFromSlot < 0 )
        return S_FALSE;

    if ( pNetMsg->emFromSlot >= SLOT_TSIZE )	
        return S_FALSE;

    if ( !VALID_SLOT_ITEM( pNetMsg->emFromSlot ) ) 
        return S_FALSE;
   
    if ( m_sCONFTING.IsCONFRONTING() )		                                        
        return S_FALSE;

    if ( m_emVehicle == EMVEHICLE_PASSENGER && pNetMsg->emFromSlot == SLOT_VEHICLE )	
        return S_FALSE;

    if ( pNetMsg->emFromSlot == SLOT_VEHICLE && m_bGetVehicleFromDB )
    {
        sc::writeLogError( sc::string::format( "Vehilce Slot Release Error, m_bGetVehicleFromDB : %1%", m_bGetVehicleFromDB));
        return S_FALSE;
    }

    // �κ��� �־���� ������
    SITEMCUSTOM sToInvenItems[SLOT_TSIZE];
    EMSLOT      emToInvenSlot[SLOT_TSIZE];
    UINT        nToInvenNums = 0;

    // �ش� ������ ������
    sToInvenItems[nToInvenNums] = GET_SLOT_ITEM( pNetMsg->emFromSlot );
    emToInvenSlot[nToInvenNums] = pNetMsg->emFromSlot;
    nToInvenNums++;

    WORD wInvenPosX=0, wInvenPosY=0;
    BOOL bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
    if ( !bOk )
	{
        sc::writeLogError( std::string( "GLChar::MsgReqSlotToInven(), m_cInventory.FindInsertable() not found empty InvenPos" ) );
        return E_FAIL;
    }

    //	�������� ���⸦ ���ٸ� �޼յ� ���� �����Ѵ�
    if ( pNetMsg->emFromSlot == GetCurRHand() )
    {
        EMSLOT emLHand = GetCurLHand();

        if ( VALID_SLOT_ITEM( emLHand ) )
        {
            SITEM* pLHandItem = GET_SLOT_ITEMDATA( emLHand );
			if ( !pLHandItem )
			{
				return E_FAIL;
			}

            switch ( pLHandItem->sBasicOp.emItemType )
            {
            case ITEM_ARROW:
            case ITEM_CHARM:
            case ITEM_BULLET: 
			case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
                {
                    sToInvenItems[nToInvenNums] = GET_SLOT_ITEM( emLHand );
                    emToInvenSlot[nToInvenNums] = emLHand;
                    nToInvenNums++;

                    bOk = m_cInventory.ValidCheckInsrt( nToInvenNums, false );
                    if ( !bOk )
                    {
                        sc::writeLogError( std::string( "GLChar::MsgReqSlotToInven(), m_cInventory.IsInsertable() failed" ) );
                        return E_FAIL;
                    }
                }
                break;
            };
        }
    }

    // �κ��� ������ ���԰� ������ ������ ����
    for ( UINT i=0; i<nToInvenNums; ++i )
    {
        WORD wInvenPosX = pNetMsg->wPosX;
        WORD wInvenPosY = pNetMsg->wPosY;

        BOOL bOk = m_cInventory.IsInsertable( wInvenPosX, wInvenPosY );
        if ( !bOk )
        {
            bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
            if ( !bOk )
            {
                sc::writeLogError( std::string( "GLChar::MsgReqSlotToInven(), m_cInventory.FindInsertable() not found empty InvenPos" ) );
                continue;
            }
        }

        //	[��ο���] �ڽ��� ���� ������ �ٲ��� �˸�.
        RELEASE_SLOT_ITEM_SERVER( emToInvenSlot[i] );

        GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutRelease( emToInvenSlot[i] );
        SendToClient( &NetMsg_PutRelease );

		GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutRelease_Brd( emToInvenSlot[i] );
		NetMsg_PutRelease_Brd.dwGaeaID = m_dwGaeaID;
        SendMsgViewAround( &NetMsg_PutRelease_Brd );

		bool JustInvenTypeUpdate = true;
		if ( sToInvenItems[i].IsTempEnchant())
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTime cTimeLimit( sToInvenItems[i].sTLGrind.tFireTime );

			if ( cTIME_CUR > cTimeLimit )
			{
				GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

				netMsgFb.eResult   = EMREQ_TEMP_ENCHANT_FIRETIME;
				netMsgFb.fireItemID = sToInvenItems[i].GetNativeID();
				SendToClient(&netMsgFb);

				// ��ȭ����
				sToInvenItems[i].TempEnchantRecovery();

				JustInvenTypeUpdate = false;

				// �α�
				LogItemExchange( sToInvenItems[i], gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );
			}
		}

        InvenInsertItem( sToInvenItems[i], wInvenPosX, wInvenPosY, true, 1, false );

		// ��� ������Ʈ
		InvenUpdateItem( m_cInventory.GetItem( wInvenPosX, wInvenPosY ), JustInvenTypeUpdate );
    }

    //	������ ����� �� �� ȯ�� �ʿ���.
    INIT_DATA( FALSE, FALSE );

    if ( pNetMsg->emFromSlot == SLOT_VEHICLE )
    {
        m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, false );
    }

    // ���⿡ ���� ������ �ʱ�ȭ �Ѵ�.
    DisableSkillFact();

    return S_OK;
}

HRESULT GLChar::MsgReqVNGainToHold( NET_MSG_GENERIC* nmg )
{
	// ó������ �ʴ´�.
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD*> ( nmg );

	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD,EMHOLD_FB_OFF);

	//	��û�� ������ ��ȿ�� �˻�.
	SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	�տ� �� �������� ������.
	if ( HaveHoldItem() )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	Inventory�� �ִ� ������ ���.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	Inventory���� ���� �������� ����.
    InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, true, TRUE);

	//	Inventory�� �־��� �������� �տ� ��.
	HOLD_ITEM(sInven_BackUp.sItemCustom);

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqVNGainExHold( NET_MSG_GENERIC* nmg )
{
	// ó������ �ʴ´�.
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_VNGAIN_EX_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_VNGAIN_EX_HOLD*> ( nmg );

	if ( !HaveHoldItem() )
	{
		return S_FALSE;
	}

	SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory�� �ִ� ������ ���.

	WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	�տ� �� ������ ���.

	const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( sItemHold.GetNativeID() );
	if ( !pInvenData || !pHoldData )
		return S_FALSE;

	// ���� �Ҽ� �ִ� ���������� ����.
	bool bMERGE(false);
	bMERGE = (pInvenData->sBasicOp.sNativeID==pHoldData->sBasicOp.sNativeID);
	if (bMERGE)	bMERGE = ( pInvenData->ISPILE() );
	if (bMERGE) bMERGE = ( sItemHold.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if (bMERGE) bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sItemHold.wTurnNum;

		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
			// ��ħ �� ����.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	�� �־��� ������ ����.
			RELEASE_HOLD_ITEM ();

			// �κ��� ������ ������Ʈ.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			SendToClient(&NetMsgInvenItem);

			//	[�ڽſ���] �� �־��� ������ ����.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			SendToClient(&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

			//	���� ������ ���� ����.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	�տ� ��� �ִ� ������ ����.
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM(sItemHold);
			
			// �κ��� ������ ������Ʈ.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			SendToClient(&NetMsgInvenItem);
		}

		return S_OK;
	}

	//	Inventory�� �ִ� ������ ����.
	BOOL bOk = m_cVietnamInventory.DeleteItem( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	// ���� �տ��� �����۰� ����� �������� ũ�Ⱑ Ʋ�� ���.
	//	��� �ִ� �������� �� ������ ã�´�.
//	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	������ġ�� ������ �ִ��� �˻�.
		BOOL bOk = m_cVietnamInventory.IsInsertable ( wPosX, wPosY );
		if ( !bOk )
		{
			//	����ִ� ������ Inventory�� ���� ������ �ִ��� �˻�.
			bOk = m_cVietnamInventory.FindInsertable( wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : Inventory���� �����ߴ� �������� �ٽ� ����.
				sInven_BackUp.sItemCustom.bVietnamGainItem = true;
				m_cVietnamInventory.InsertItem( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

				return S_FALSE;
			}
		}
	}

	//	����ִ� ������ Inventory�� ����.
	sItemHold.bVietnamGainItem = true;
	m_cVietnamInventory.InsertItem( sItemHold, wPosX, wPosY );
	SINVENITEM* pINSERT_ITEM = m_cVietnamInventory.GetItem(wPosX,wPosY);
	if ( !pINSERT_ITEM )
	{
		sc::writeLogError(sc::string::format("error MsgReqInvenExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.GetNativeID().wMainID, sItemHold.GetNativeID().wSubID, wPosX, wPosY ));

		return S_FALSE;
	}

	//	Inventory�� �־��� �������� �տ� ��.
	HOLD_ITEM(sInven_BackUp.sItemCustom);

	//	[�ڽſ���] Inventory�� �ִ� ������ ����, �� ������ ����.
	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	������ ������.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.bVietnamInven = TRUE;
	if (pINSERT_ITEM)
        NetMsg_Del_Insert.sInsert = *pINSERT_ITEM;	//	���ԵǴ� ������.

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, NetMsg_Del_Insert);

	SendToClient(NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer);
	
	return S_OK;
	*/
}

HRESULT GLChar::MsgReqHoldToVNGain( NET_MSG_GENERIC* nmg )
{
	// ó������ �ʴ´�.
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_HOLD_TO_VNGAIN* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_VNGAIN*> ( nmg );

	if ( !HaveHoldItem() )
	{
		return S_FALSE;
	}

	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( GET_HOLD_ITEM().GetNativeID() );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	�κ� ��ġ.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cVietnamInventory.IsInsertable ( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	Inventory�� ��������.
		return E_FAIL;
	}

	//	����ִ� ������ Inventory�� ����.
	SITEMCUSTOM& sITEM_NEW = m_PutOnItems[SLOT_HOLD];
	sITEM_NEW.bVietnamGainItem = true;
	bOk = m_cVietnamInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
	if (!bOk)
        return E_FAIL;

	const SINVENITEM* pINVENITEM = m_cVietnamInventory.FindPosItem( wInsertPosX, wInsertPosY );

	//	������ ������ ����.
	RELEASE_HOLD_ITEM ();

	//	[�ڽſ���] Inventory�� ������ ����.
    InvenInsertSend(pINVENITEM, TRUE);

	//	[�ڽſ���] �� �־��� ������ ����.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	SendToClient(&NetMsg_ReleaseHold);	

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqVNInvenToInven( NET_MSG_GENERIC* nmg )
{
	// ó������ �ʴ´�.
	return E_FAIL;

	/*
	if ( m_dwVietnamInvenCount <= 0 )
		return E_FAIL;

	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN* pNetMsg = (GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN *)nmg;

	SINVENITEM* pInvenItem(NULL);
	pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );

	//	����� �ϴ� �������� �����ϴ�.
	if ( !pInvenItem )
		return E_FAIL;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	WORD wInsertPosX(0), wInsertPosY(0);
	BOOL bITEM_SPACE(false);
	bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

	//	�κ��� ���������� ���� ��� ��ҵ�.
	if ( !bITEM_SPACE )
		return E_FAIL;

	SITEMCUSTOM sITEM_NEW = pInvenItem->sItemCustom;

	sITEM_NEW.bVietnamGainItem = false;
	m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
	m_cVietnamInventory.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );

	SINVENITEM sINVENITEM = *m_cInventory.FindPosItem( wInsertPosX, wInsertPosY );
	SINVENITEM* pINVENITEM = m_cInventory.FindPosItem( wInsertPosX, wInsertPosY );

	m_dwVietnamInvenCount--;
	GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE NetMsgFB;
	NetMsgFB.dwVietnamInvenCount = m_dwVietnamInvenCount;
	SendToClient( &NetMsgFB );

	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB NetMsg;
	NetMsg.wPosX    = pNetMsg->wPosX;
	NetMsg.wPosY    = pNetMsg->wPosY;
	NetMsg.wNewPosX = wInsertPosX;
	NetMsg.wNewPosY = wInsertPosY;
	NetMsg.Data		= sINVENITEM;
	SendToClient( &NetMsg );	

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( sINVENITEM.sItemCustom.GetNativeID() );
	//if ( pITEM && pITEM->IsTIMELMT() )
	if ( pITEM && ( sINVENITEM.sItemCustom.IsTimeLimit( false ) ) )
	{

		if ( pITEM->sDrugOp.tTIME_LMT != 0 )
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sINVENITEM.sItemCustom.tBORNTIME);
			cTIME_LMT += cSPAN;

			WORD wInsertPosX = sINVENITEM.wPosX, wInsertPosY = sINVENITEM.wPosY;


			if ( cTIME_CUR > cTIME_LMT )
			{
				//	[�ڽſ���] �ش� �������� Inventory���� ����.
				InvenDeleteSend(sINVENITEM.wPosX, sINVENITEM.wPosY, FALSE);

				//	�ð� �������� ������ ���� �˸�.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sINVENITEM.sItemCustom.GetNativeID();
				SendToClient(&NetMsgInvenDelTimeLmt);

				// ������ ����
				InvenDeleteItem(sINVENITEM.wPosX, sINVENITEM.wPosY, false, FALSE);
			
				// �ڽ��� ����.
				if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
				{
					const SITEM* pONE = GLogicData::GetInstance().GetItem( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )
						return E_FAIL;

					SITEMCUSTOM sITEM_NEW( sINVENITEM.sItemCustom.nidDISGUISE );
					CTime cTIME = CTime::GetCurrentTime();
					if ( sINVENITEM.sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
					{
						cTIME = CTime(sINVENITEM.sItemCustom.tDISGUISE);

						CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
						cTIME -= tLMT;
					}

					// �ڽ�Ƭ ������ �ű�. by luxes.
					//
					sITEM_NEW.dwMainColor = sINVENITEM.sItemCustom.dwMainColor;
					sITEM_NEW.dwSubColor = sINVENITEM.sItemCustom.dwSubColor;

					// �ڽ�Ƭ �ɷ�ġ �ű�
					sITEM_NEW.costumeUser = sINVENITEM.sItemCustom.costumeUser;

					pINVENITEM->sItemCustom.dwMainColor = (DWORD)0;
					pINVENITEM->sItemCustom.dwSubColor = (DWORD)0;

					// ������ ����
                    sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sITEM_NEW.cChnID = (BYTE) GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE) GetFieldSvrID();
					RegItemGen( sITEM_NEW.guid );

					// �κ��� �� ���� �˻�.
					BOOL bOk = m_cInventory.IsInsertable ( wInsertPosX, wInsertPosY );
					if ( !bOk )
						return E_FAIL;

					// �κ��� ����.
					m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

					InvenInsertSend(pINSERT_ITEM);

					GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
					if ( !pTarChar )
					{		
						// ��� �� �α� ����
						// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
						sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqVNInvenToInven", m_CharDbNum ) );
					}

					// �и��� �ڽ��� �α�
					// ��� �� �α� ����
					LogItemExchange(
                        pINSERT_ITEM->sItemCustom,
                        gamelog::ID_CHAR, 0,
                        gamelog::ID_CHAR, m_CharDbNum,
                        ITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CHANGE_INVEN_TO_INVEN,
						false);
				}
			}
		}
	}	

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqVNGainInvenReset( NET_MSG_GENERIC* nmg )
{
	m_cVietnamInventory.DeleteItemAll();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenItemSort( NET_MSG_GENERIC* nmg )
{
    if ( EInvenX == 0 )
        return E_FAIL;

    if ( GetRemainTimeItemSort() > 0.0f )
        return E_FAIL;

    if ( !IsValidBody() )
        return E_FAIL;

    ResetLimitTimeItemSort();

    GLMSG::SNETPC_REQ_ITEM_SORT* pNetMsg = ( GLMSG::SNETPC_REQ_ITEM_SORT* )nmg;

    if ( ( pNetMsg->nMoveCount <= 0 && pNetMsg->bHasPile == false ) || pNetMsg->nMoveCount >= EInvenTotalSlot )
        return E_FAIL;

    GLInventory* pInvenOrigin = NULL;

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		pInvenOrigin = &m_cVietnamInventory;
	}
	else
	{
		pInvenOrigin = &m_cInventory;
	}

    if ( !pInvenOrigin )
        return E_FAIL;

    GLInventory cInvenClone;
    cInvenClone.Assign( *pInvenOrigin );

    GLInventory cInvenTemp;
    cInvenTemp.SetAddLine( pInvenOrigin->GETAddLine(), pInvenOrigin->IsLimit() );
    cInvenTemp.SetState( pInvenOrigin->GetSizeX(), pInvenOrigin->GetSizeY() );

    SINVENITEM* pInvenItem = NULL;
    SITEM* pItem = NULL;
    BOOL bInsertItemOK = FALSE;
    BOOL bDeleteItemOK = FALSE;

    for ( WORD i = 0; i < pNetMsg->nMoveCount; i++ )
    {
		SINVEN_MOVE_ITEM_SORT& sInvenSort = pNetMsg->sMoveItem[ i ];

		WORD nOriginX = sInvenSort.nOrigin % EInvenX;
		WORD nOriginY = sInvenSort.nOrigin / EInvenX;

        pInvenItem = cInvenClone.GetItem( nOriginX, nOriginY );
        if ( !pInvenItem )
            return E_FAIL;

        pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if ( !pItem )
            return E_FAIL;

		WORD nTargetX = sInvenSort.nTarget % EInvenX;
		WORD nTargetY = sInvenSort.nTarget / EInvenX;

        bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, nTargetX, nTargetY, true );
        if ( !bInsertItemOK )
            return E_FAIL;

        bDeleteItemOK = cInvenClone.DeleteItem( nOriginX, nOriginY );
        if ( !bDeleteItemOK )
            return E_FAIL;
    }

    const GLInventory::CELL_MAP* pMapInvenItem = cInvenClone.GetItemList();
    if ( !pMapInvenItem->empty() )
    {
        GLInventory::CELL_MAP_CITER itr = pMapInvenItem->begin();
        GLInventory::CELL_MAP_CITER endItr = pMapInvenItem->end();

        for ( ; itr != endItr; itr++ )
        {
            const GLInventory::CELL_KEY& keyClone = itr->first;

            pInvenItem = itr->second;
            if ( !pInvenItem )
                return E_FAIL;

            bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, keyClone.first, keyClone.second, true );

			if ( !bInsertItemOK )
                return E_FAIL;
        }
    }

    if ( pNetMsg->bHasPile )
    {
        cInvenTemp.OptimalPile();
    }

    pInvenOrigin->Assign( cInvenTemp );

    SendToClient( pNetMsg );

    return S_OK;
}

int GLChar::MsgReqInvenItemSortStart( NET_MSG_GENERIC* nmg )
{
    if ( EInvenX == 0 )
        return -1;

    if ( GetRemainTimeItemSort() > 0.0f )
        return -2;

    if ( !IsValidBody() )
        return -3;

	GLMSG::SNETPC_REQ_ITEM_SORT_START* pNetMsg = ( GLMSG::SNETPC_REQ_ITEM_SORT_START* )nmg;

	SendToClient( pNetMsg );

	return 1;
}

int GLChar::MsgReqInvenItemSortData( NET_MSG_GENERIC* nmg )
{
    if ( EInvenX == 0 )
        return -1;

    if ( GetRemainTimeItemSort() > 0.0f )
        return -2;

    if ( !IsValidBody() )
        return -3;

	GLMSG::SNETPC_REQ_ITEM_SORT_DATA* pNetMsg = ( GLMSG::SNETPC_REQ_ITEM_SORT_DATA* )nmg;

	if ( pNetMsg->nCurCount <= 0 || pNetMsg->nStartIdx < 0 || pNetMsg->nStartIdx >= EInvenTotalSlot )
		return -4;

	for( int i = 0; i < pNetMsg->nCurCount; i++ )
	{
		m_sSwapInventory[pNetMsg->nStartIdx + i] = pNetMsg->sMoveItem[i];
	}

	SendToClient( pNetMsg );

	return 1;
}

int GLChar::MsgReqInvenItemSortEnd( NET_MSG_GENERIC* nmg )
{
    if ( EInvenX == 0 )
        return -1;

   if ( GetRemainTimeItemSort() > 0.0f )
	    return -2;

    if (!IsValidBody())
        return -3;

	GLMSG::SNETPC_REQ_ITEM_SORT_END* pNetMsg = ( GLMSG::SNETPC_REQ_ITEM_SORT_END* )nmg;

     if ( (pNetMsg->nTotalCount <= 0 && pNetMsg->bHasPile == false) || pNetMsg->nTotalCount >= EInvenTotalSlot )
        return -4;

	GLInventory* pInvenOrigin = NULL;

	if (m_ServiceProvider == SP_VIETNAM)
	{
		pInvenOrigin = &m_cVietnamInventory;
	}
	else
	{
		pInvenOrigin = &m_cInventory;
	}

    if ( !pInvenOrigin )
        return -5;

    GLInventory cInvenClone;
    cInvenClone.Assign( *pInvenOrigin );

    GLInventory cInvenTemp;
    cInvenTemp.SetAddLine( pInvenOrigin->GETAddLine(), pInvenOrigin->IsLimit() );
    cInvenTemp.SetState( pInvenOrigin->GetSizeX(), pInvenOrigin->GetSizeY() );

    SINVENITEM* pInvenItem = NULL;
    SITEM*  pItem = NULL;
    BOOL bInsertItemOK = FALSE;
    BOOL bDeleteItemOK = FALSE;


	for ( WORD i = 0; i < pNetMsg->nTotalCount; i++ )
    {
		SINVEN_MOVE_ITEM_SORT& sInvenSort = m_sSwapInventory[ i ];

		WORD nOriginX = sInvenSort.nOrigin % EInvenX;
		WORD nOriginY = sInvenSort.nOrigin / EInvenX;

        pInvenItem = cInvenClone.GetItem( nOriginX, nOriginY );
        if ( !pInvenItem )
            return -6;

        pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if ( !pItem )
            return -7;

		WORD nTargetX = sInvenSort.nTarget % EInvenX;
		WORD nTargetY = sInvenSort.nTarget / EInvenX;

        bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, nTargetX, nTargetY, true );
        if ( !bInsertItemOK )
            return -8;

        bDeleteItemOK = cInvenClone.DeleteItem( nOriginX, nOriginY );
        if ( !bDeleteItemOK )
            return -9;
    }

    const GLInventory::CELL_MAP* pMapInvenItem = cInvenClone.GetItemList();
    if ( !pMapInvenItem->empty() )
    {
        GLInventory::CELL_MAP_CITER itr = pMapInvenItem->begin();
        GLInventory::CELL_MAP_CITER endItr = pMapInvenItem->end();

        for ( ; itr != endItr; itr++ )
        {
            const GLInventory::CELL_KEY& keyClone = itr->first;

            pInvenItem = itr->second;
            if ( !pInvenItem )
                return -10;

            bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, keyClone.first, keyClone.second, true );

			if ( !bInsertItemOK )
                return -11;
        }
    }

    if ( pNetMsg->bHasPile )
    {
		std::vector< DELETED_ITEM > DeletedItems;
        cInvenTemp.OptimalPileServer( DeletedItems );
		size_t ItemSize = DeletedItems.size();
		for ( size_t loop = 0; loop < ItemSize; loop++ )
		{
			DELETED_ITEM& DeletedItem = DeletedItems[loop];
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemDelete( DeletedItem.guid, DeletedItem.DbState, 89077 ) ) );
		}
    }

    pInvenOrigin->Assign( cInvenTemp );

	ClearSwapInventory();
    ResetLimitTimeItemSort();

	SendToClient( pNetMsg );

	return 1;
}

HRESULT GLChar::MsgActivityDoneItemTake( NET_MSG_GENERIC* nmg )
{
	//validate the activity-done request.

	GLMSG::SNET_ACTIVITY_DONE_ITEMTAKE* msg = ( GLMSG::SNET_ACTIVITY_DONE_ITEMTAKE* )nmg;

	DWORD dwActivityID	= msg->dwActivityID;
	DWORD dwItemID		= msg->dwItemID;

	
	UINT nValueBase = m_pActivityMan->GetBaseValue( dwActivityID );
	
	if ( nValueBase == 0 )	//the activity data does not exist
	{
		sc::writeLogError( sc::string::format( "Activity(%d) data does not exist.", dwActivityID ) );
		return E_FAIL;
	}

	BOOL bValid = ISHAVEITEM( dwItemID, nValueBase );	// nValue >= nValueBase

	//
	//prepare the response message.
	//
	GLMSG::SNET_ACTIVITY_DONE_ITEMTAKE_FB msgFB;
	msgFB.dwActivityID	= dwActivityID;
	msgFB.dwItemID		= dwItemID;
	msgFB.bUpdated		= bValid;
	
	SendToClient( &msgFB );
	
	if ( bValid )
	{
		ActivityCheckItemTake( dwItemID, dwActivityID );

		//at this point, additional update-result will be sent by Update routine in ActivityManager.
		// NET_MSG_ACTIVITY_UPDATE_COMPLETED or NET_MSG_ACTIVITY_UPDATE_CLOSED
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenSplit( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_SPLIT* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_SPLIT* )nmg;
	
	BOOL bOk( FALSE );

	// �и� ��û ������ ã��.
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	// ������ ���� ��������.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	// �и� ������ ���������� ����.
	bool bSPLIT( false );
	bSPLIT = ( pItem->ISINSTANCE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= 1 )					return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	// ���� �������� ��ȭ�� ����.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	// �и��Ǿ� ���� ����� ������.
	SITEMCUSTOM sNEW_ITEM( pInvenItem->sItemCustom );
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;
	sc::SeqUniqueGUID::Instance()->getGUID( sNEW_ITEM.guid );
	sNEW_ITEM.GenerateBasicStat( false );
	sNEW_ITEM.GenerateLinkSkill();
	sNEW_ITEM.GenerateAddOption();

	// #item
	// ������ �и��� �� ��� ������ �߻�
	// Ȯ���غ��� �и��Ǵ� �������� �״�� �������� �����ؼ� guid �� �����ؼ� �κ����� �� ��� ������Ʈ�ߴ�.
	// �׷��� ��񿡴� ���� �������ε� ������Ʈ�� ���� ������ �߻�
	// �̷��� �� ���� �� ����. ������ �״�� �������� ���� ��� ���¿� �°� ��� ���µ� �����ؾ� �Ѵ�.
	// ������ ��� ���µ� �����ؾ���, ������Ʈ���� �������� ������ �ȵǱ� ������ ������ �����Ѵ�.
	sNEW_ITEM.SetDbState( db::DB_INSERT, true );

	// ���� ����� ������ ��ġ ã��.
	WORD wInvenPosX( 0 ), wInvenPosY( 0 );
	bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )
		return E_FAIL;

	// ���� ���� ������ �ֱ�.
	if ( !InvenInsertItem( sNEW_ITEM, wInvenPosX, wInvenPosY, true, 1, true ) )
	{
		return E_FAIL;
	}

	// ���� �������� ���� ���� ��Ŵ.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	// #item
	// �׽�Ʈ
	// ���� ���� ��� ������Ʈ
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemUpdateTurnNum( pInvenItem->GetGuid(), pInvenItem->sItemCustom.wTurnNum, 19770929 ) ) );
	}

	// ���� ������ ���� ����.
	GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsgInven;
	NetMsgInven.dwChaNum = CharDbNum();
	NetMsgInven.wPosX = pNetMsg->wPosX;
	NetMsgInven.wPosY = pNetMsg->wPosY;
	NetMsgInven.wTurnNum = wORGIN_SPLIT;
	SendToClient( &NetMsgInven );

	return S_OK;
}

HRESULT GLChar::MsgReqHoldToInven( NET_MSG_GENERIC* nmg )
{
	// ������ �ʴ� �� ���� �ּ� ó����
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_HOLD_TO_INVEN* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_INVEN*> ( nmg );

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( pNetMsg->bUseVietnamInven && m_dwVietnamInvenCount <= 0 )
			return E_FAIL;
	}
//#endif

	if ( !HaveHoldItem() )
	{
		return S_FALSE;
	}

	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( GET_HOLD_ITEM().GetNativeID() );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

    if ( !InvenInsertItem( GET_HOLD_ITEM(), pNetMsg->wPosX, pNetMsg->wPosY, true ) )
    {
        if ( !InvenInsertItem( GET_HOLD_ITEM(), true, true ) )
            return E_FAIL;
    }

    //	������ ������ ����.
    RELEASE_HOLD_ITEM();

    //	[�ڽſ���] �� �־��� ������ ����.
    GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
    SendToClient(&NetMsg_ReleaseHold);

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqInvenToInven( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_INVEN_TO_INVEN* >( nmg );

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( pNetMsg->bUseVietnamInven && m_dwVietnamInvenCount <= 0 )
			return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
	{
		return S_FALSE;
	}

	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( pHoldInvenItem->CustomItem().GetNativeID() );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	�κ� ��ġ.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		// ���ڸ��� �ƴ�
		return E_FAIL;
	}

	//	����ִ� ������ Inventory�� ����.	
	if ( m_ServiceProvider == SP_VIETNAM )
	{
		SITEMCUSTOM& sCustom = pHoldInvenItem->sItemCustom;

		if ( sCustom.bVietnamGainItem && m_dwVietnamInvenCount <= 0 )
			return E_FAIL;
		
		sCustom.bVietnamGainItem = false;
	}

	if ( !InvenInsertItem( pHoldInvenItem->CustomItem(), wInsertPosX, wInsertPosY, false, 1, false ) )
	{
		return E_FAIL;
	}

	SINVENITEM* pInsertItem = m_cInventory.FindPosItem( wInsertPosX, wInsertPosY );
	if ( !pInsertItem )
        return E_FAIL;

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.

	//	������ ������ ����.
	if ( !InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, FALSE, 1, false ) )
	{
		// �־��� ������ ���� �ϰ� ����
		InvenDeleteItem( wInsertPosX, wInsertPosY, false, FALSE, 1, false );
		return E_FAIL;
	}

	// ��ġ�� ��� ������Ʈ�Ѵ�.
	InvenUpdateItem( pInsertItem, true );

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( pNetMsg->bUseVietnamInven )
		{
			m_dwVietnamInvenCount--;

			GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE NetMsgFB;
			NetMsgFB.dwVietnamInvenCount = m_dwVietnamInvenCount;
			SendToClient( &NetMsgFB );
		}
	}

	//	[�ڽſ���] Inventory�� ������ ����.
    InvenInsertSend( pInsertItem );

	//	��ȿ�Ⱓ �ʰ� ����.
    HRESULT hrCheckDuration = CheckItemDuration( *pInsertItem );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;

/*
	// ȹ���� �������� ��ī�� && ������ ���̶�� �������� �����Ѵ�.
	// �־��̵� 0 �� �ƴϸ� ��ī���̸鼭 DB�� ���� ������ ���̴�.
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD && sINVENITEM.m_Item.dwPetID != 0 )
	{
		CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, sINVENITEM.m_Item.dwPetID );
		IDbManager* pDBMan = m_pGLGaeaServer->GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );
	}
*/
	return S_OK;
}

// #item
HRESULT GLChar::MsgReqStorageToInven( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_TO_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_STORAGE_TO_INVEN* >( nmg );
    GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB,EMHOLD_FB_OFF );

    if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
    {
        SendToClient( &NetMsg_fb );
        return E_FAIL;
    }

    if ( !m_bUsedStorageCard )
    {
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
			return E_FAIL;

        // â������ ��ȿ�� üũ
        GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
        if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
        {
            return E_FAIL;
        }
    }

	// TEST [12/24/2012 gbgim]
// 	if ( !IsKEEP_STORAGE(pNetMsg->dwChannel) )
// 	{
// 		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
// 		SendToClient( &NetMsg_fb );
// 		return S_FALSE;
// 	}

    //	â�������� DB���� �������� �ʾ�����.
    if ( !m_bServerStorage )
        LoadStorageFromDB();

    const DWORD dwChannel = pNetMsg->dwChannel;

    SINVENITEM* pHoldInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

    if ( !pHoldInvenItem )
    {
        SendToClient( &NetMsg_fb );
        return S_FALSE;
    }

    if ( m_ServiceProvider == SP_VIETNAM )
    {
        if ( pHoldInvenItem->CustomItem().bVietnamGainItem  )
            return FALSE;
    }

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( pNetMsg->bUseVietnamInven && m_dwVietnamInvenCount <= 0 )
			return E_FAIL;
	}

	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( pHoldInvenItem->CustomItem().GetNativeID() );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	�κ� ��ġ.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	Inventory�� ��������.
		return E_FAIL;
	}

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.

	//	Note : ��� �ִ� ������ ����.
	if ( !m_cStorage[dwChannel].DeleteItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
	{
		return E_FAIL;
	}

	if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), wInsertPosX, wInsertPosY ) )
	{
		if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
		{
			DropItem( sHold_BackUp.sItemCustom, m_vPosition );
		}
		return E_FAIL;
	}

	SINVENITEM* pInsertItem = m_cInventory.FindPosItem( wInsertPosX, wInsertPosY );
	if ( !pInsertItem )
	{
		NET_DISCONNECT_CHARACTER NetMsg;
		NetMsg.ChaDbNum = m_CharDbNum;
		m_pGaeaServer->SENDTOAGENT( &NetMsg );

		sc::writeLogError( sc::string::format( "critical error MsgReqStorageToInven pInsertItem null, disconnect user. chanum: %1% guid: %2%", 
			m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

        return E_FAIL;
	}

	// ��Ŀ ������ ���� Ŭ�� �˸�
	UserStorageDeleteSend( dwChannel, pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	// ��� ������Ʈ(������ġ�� â���� �κ��丮�� ����Ǿ��� ������ ������ġ �� ��ǥ�� ��񿡼� ������Ʈ�Ѵ�.)
	InvenUpdateItem( pInsertItem, true );

	// Inventory�� ������ ���Ե� ������ �˸�.
    InvenInsertSend( pInsertItem );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageToInven", m_CharDbNum ) );
	}

    // �������� ���� ���� ��� ���
	// ��� �� �α� ����
    LogItemExchange( 
        pInsertItem->CustomItem(), 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pInsertItem->CustomItem().wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MOVE_STORAGE_TO_INVEN,
		false );

	//	��ȿ�Ⱓ �ʰ� ����.
    HRESULT hrCheckDuration = CheckItemDuration( *pInsertItem );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;

/*
	// ȹ���� �������� ��ī�� && ������ ���̶�� �������� �����Ѵ�.
	// �־��̵� 0 �� �ƴϸ� ��ī���̸鼭 DB�� ���� ������ ���̴�.
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD && sINVENITEM.m_Item.dwPetID != 0 )
	{
		CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, sINVENITEM.m_Item.dwPetID );
		IDbManager* pDBMan = m_pGLGaeaServer->GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );
	}
*/
	return S_OK;
}

// #item
HRESULT GLChar::MsgReqInvenToField( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_FALSE;

	GLMSG::SNETPC_REQ_INVEN_TO_FIELD* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_INVEN_TO_FIELD* >( nmg );

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
	if ( !pHoldInvenItem )
		return S_FALSE;

	D3DXVECTOR3 vCollisionPos;
	BOOL bCollision = pLand->IsCollisionNavi(
		pNetMsg->vPos + D3DXVECTOR3(0,+5,0),
		pNetMsg->vPos + D3DXVECTOR3(0,-5,0),
		vCollisionPos );

	if ( !bCollision )
		return S_FALSE;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = pHoldInvenItem->wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = pHoldInvenItem->wPosY;
	const SITEMCUSTOM& sITEMCUSTOM = pHoldInvenItem->CustomItem();

	const SNATIVEID& itemID( sITEMCUSTOM.GetRealNativeID() );
	if ( m_pPrivateMarket->IsOpen() == true )
	{
		sc::writeLogError( "While opening Personal Store, it's not possible to drop items. there is a high possibility of being hacked or packet abusing case." );
		sc::writeLogError( sc::string::format( "If you find any case that this message is repeatedly left within a log, please report us it right away(%1%/%2%).", itemID.Mid(), itemID.Sid() ) );		
	}	

	// �ӽ÷� Ȯ���� ���ؼ� �߰�
	if ( pNetMsg->sItem != sITEMCUSTOM )
	{
		const SNATIVEID& tempItemID( pNetMsg->sItem.GetRealNativeID() );

		sc::writeLogError( sc::string::format( "[ITEM DROP ERROR] MsgReqInvenToField ChaDBNum:%1%, X:%2%, Y:%3%, InvenMid:%4%, InvenSid%5%, ClientMid:%6%, ClientSid:%7%", m_CharDbNum, pNetMsg->wHoldPosX, pNetMsg->wHoldPosY, itemID.Mid(), itemID.Sid(), tempItemID.Mid(), tempItemID.Sid() ) );
		sc::writeLogError( sc::string::format( "[ITEM DROP ERROR] InvenMid:%1%, InvenSid%2%, ClientMid:%3%, ClientSid:%4%", sITEMCUSTOM.GetNativeID().Mid(), sITEMCUSTOM.GetNativeID().Sid(), pNetMsg->sItem.GetNativeID().Mid(), pNetMsg->sItem.GetNativeID().Sid() ) );
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( itemID );
	if ( pITEM == NULL )
        return false;	

	const SMAPNODE* const pMapNode( m_pGaeaServer->FindMapNode( GetCurrentMap().getBaseMapID() ) );
	if ( pMapNode == NULL )
		return S_FALSE;

	// �������� ���� �� �ִ°��̾�߸� ���� �� �ִ�;
	if ( pMapNode->IsDropBlockItem() )
		return S_FALSE;

	// ������ �������� ��ī�� && ���� Ȱ�������̸� ���� �� ����
	switch ( pITEM->sBasicOp.emItemType )
	{
	case ITEM_PET_CARD:
		{
			PGLPETFIELD pMyPet = GetMyPet();
			if ( pMyPet && pMyPet->IsValid() && sITEMCUSTOM.PetDbNum() == pMyPet->DbNum() )
				return S_FALSE;
		}
		break;
	case ITEM_QITEM:
		{
			if ( pMapNode->IsQBoxDrop() == false )
				return S_FALSE;
		}
		break;
	}	
	
	//	�ŷ��ɼ�
	SITEMCUSTOM sItemCustom( itemID );
	if ( !( sItemCustom.IsThrow() != NULL ) )
	{
		return S_FALSE;
	}

	if ( !( sITEMCUSTOM.IsThrow() != NULL ) )
	{
		sc::writeLogError( sc::string::format( "[ITEM DROP ERROR] IsThrow() ERROR ChaDBNum:%1%, Mid:%2%, Sid%3%", m_CharDbNum, sITEMCUSTOM.GetNativeID().Mid(), sITEMCUSTOM.GetNativeID().Sid() ) );
		return S_FALSE;
	}

	// GMCharEdit �� ���� �������� ������ �Ұ����ϴ�.
	if ( sITEMCUSTOM.IsGM_GENITEM() )
		return S_FALSE;

	//	������ ����.
	if ( !InvenDeleteItem( wHoldPosX, wHoldPosY, true, sITEMCUSTOM.bVietnamGainItem, MSG_REQINVENTOFIELD, false ) )
	{
		return S_FALSE;
	}

	//	Item Drop �õ�.
	{
		// �ٴڿ� ����߸���.
		// ������ ���°� �ʱ�����̰ų� �������ؾ��ϴ� ���¶�� ������ ������Ÿ������ ��� �켱 �����Ѵ�.
		// �׿� ��������Ʈ ���¶�� ��� �κ�Ÿ�� �����Ѵ�.
		// ���߿� ���忡�� ������� �ȴٸ�(�ƹ��� ���� �ʰ�) ���� ���� Ÿ������ ������Ʈ �ǰ�(���忡 �۾� �ʿ�)
		// ������ �ݴ´ٸ� �ڱ� ���������� ������Ʈ�ȴ�.
		int DbState = sHold_BackUp.GetDbState();
		if ( db::DB_NONE ==  DbState || db::DB_INSERT == DbState )
		{
			AddGameAdoJob(
				db::DbActionPtr(
				new db::ItemInsert( m_CharDbNum, INVEN_DELETED, sHold_BackUp.sSaveData, MSG_REQINVENTOFIELD ) ) );

			sHold_BackUp.SetDbState( db::DB_UPDATE, true );
		}
		else
		{
			AddGameAdoJob(
				db::DbActionPtr(
				new db::ItemUpdate( m_CharDbNum, INVEN_DELETED, sHold_BackUp.sSaveData, MSG_REQINVENTOFIELD ) ) );
		}
	}

	// ������ ��� ������Ʈ�� �ߴµ� DropItem �����Ѵٸ� ����ó���� ��� �� ���ΰ�?
	// �ٴڿ� ���� �� holder �� �������� �ʴ°� ������, ��ȹ �ǵ�? �ٴڿ� ������ �ٷ� �ٸ� ĳ���Ͱ� ���� �� �ֳ� ����
	if ( pLand->DropItem( vCollisionPos, &( sHold_BackUp.sItemCustom ) ) == NULL )
	{
		sc::writeLogError( sc::string::format( "MsgReqInvenToField DropItem error chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sHold_BackUp.GetGuid() ).c_str() ) );
		m_pGaeaServer->InsertErrorItem( sHold_BackUp.GetGuid(), 301 );
        return S_FALSE;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenToField", m_CharDbNum ) );
	}

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        sHold_BackUp.CustomItem(), 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_GROUND, sHold_BackUp.CustomItem().wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_INVEN_TO_FIELD,
		false );

	return S_OK;
}

BOOL GLChar::ISBOTH_HANDHELD (const SITEM* pITEM ) const
{
	GASSERT ( pITEM );
	return ( ( pITEM->sSuitOp.emSuit==SUIT_HANDHELD ) && pITEM->sSuitOp.IsBOTHHAND() );
}

HRESULT GLChar::MsgReqSlotToHold(NET_MSG_GENERIC* nmg)
{
	// ������ �ʴ´�.
	return E_FAIL;

	/*
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_FALSE;

	GLMSG::SNETPC_REQ_SLOT_TO_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_TO_HOLD*> ( nmg );
	if ( pNetMsg->emSlot >= SLOT_TSIZE )
		return S_FALSE;

	if ( HaveHoldItem () )
		return S_FALSE;
	if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )
		return S_FALSE;
	if ( m_sCONFTING.IsCONFRONTING() )
		return S_FALSE;

	if ( m_emVehicle == EMVEHICLE_PASSENGER && pNetMsg->emSlot == SLOT_VEHICLE )
		return S_FALSE;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pITEM_LEFT=NULL, *pITEM_RIGHT=NULL;
	if ( VALID_SLOT_ITEM(emLHand) )	pITEM_LEFT = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emLHand).GetNativeID());
	if ( VALID_SLOT_ITEM(emRHand) )	pITEM_RIGHT = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emRHand).GetNativeID());

	//	��� ��� ���� ���� ���� ���.	Ȱ+ȭ��, â+����. ��Ʈ�� �����ϴ� ������� ���� �̶� ��� ��������.
	//
	if ( pNetMsg->emSlot==emRHand )
	{
		if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_BOW) || (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_SPEAR) ||
			 (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_PISTOL) ||  (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_RAILGUN) ||
			 (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_PORTALGUN) )
		{
			if ( pITEM_LEFT )	//	ȭ��, ���� ������ ��.
			{
				SITEMCUSTOM sITEM_LHAND = GET_SLOT_ITEM(emLHand);

				WORD wInvenPosX, wInvenPosY;
				BOOL bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
				if ( bOk )
				{
					//	���� ������ Inventory�� ����.
					m_cInventory.InsertItem( sITEM_LHAND, wInvenPosX, wInvenPosY );

					//	[�ڽſ���] Inventory�� ������ ����.
					InvenInsertSend(m_cInventory.GetItem(wInvenPosX, wInvenPosY));

					//	[��ο���] ���÷��� �������� �ƴѰɷ� �����Ͽ� ����.
				}
				// �κ��丮 �ֱ⿡ �����Ͽ��� ���.
				else
				{
					//	Item Drop �õ�.
					pLand->DropItem ( m_vPosition, &sITEM_LHAND );

					GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
					if ( !pTarChar )
					{		
						// ��� �� �α� ����
						// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
						sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqSlotToHold", m_CharDbNum ) );
					}

					// �������� ���� ���� ��� ���
					// ��� �� �α� ����
					LogItemExchange(
                        sITEM_LHAND,
                        gamelog::ID_CHAR, m_CharDbNum,
                        gamelog::ID_CHAR, 0,
                        ITEM_ROUTE_GROUND, sITEM_LHAND.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_SLOT_TO_FIELD,
						false);
				}

				//	���� ���� ������.
				RELEASE_SLOT_ITEM(emLHand);

				//	[�ڽſ���] SLOT�� �־��� ������ ����.
				GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emLHand);
				SendToClient(&NetMsg_PutOn_Release);
			}
		}
	}

	if (pNetMsg->emSlot== SLOT_VEHICLE && m_bGetVehicleFromDB)
	{
		sc::writeLogError(
            sc::string::format(
                "Vehilce Slot Release Error, m_bGetVehicleFromDB : %1%",
			    m_bGetVehicleFromDB));
		return S_FALSE;
	}

	//	�տ� ��� ��.
	HOLD_ITEM(GET_SLOT_ITEM(pNetMsg->emSlot));

	//	SLOT ������ ����.
	RELEASE_SLOT_ITEM ( pNetMsg->emSlot );

	//	������ ����� �� �� ȯ�� �ʿ���.
	INIT_DATA( FALSE, FALSE );

	//	[�ڽſ���] SLOT�� �־��� ������ ����.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(pNetMsg->emSlot);
	SendToClient(&NetMsg_PutOn_Release);

	//	[��ο���] �ڽ��� ���� ������ �ٲ��� �˸�.
	GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutOn_Release_Brd;
	NetMsg_PutOn_Release_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Release_Brd.emSlot = pNetMsg->emSlot;
	SendMsgViewAround(&NetMsg_PutOn_Release_Brd);

	if (pNetMsg->emSlot == SLOT_VEHICLE)
		m_pGaeaServer->SaveVehicle(m_ClientSlot, m_dwGaeaID, false);

	// ���⿡ ���� ������ �ʱ�ȭ �Ѵ�.
	DisableSkillFact();	

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqHoldToSlot(NET_MSG_GENERIC* nmg)
{
	// ������ �ʴ´�.
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_HOLD_TO_SLOT* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_SLOT*> ( nmg );
	if ( pNetMsg->emSlot >= SLOT_TSIZE )
		return S_FALSE;

	if ( !HaveHoldItem () )
		return S_FALSE;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( GET_HOLD_ITEM().GetNativeID() );
	if ( !pItem )
		return S_FALSE;
	if ( m_sCONFTING.IsCONFRONTING() )
		return S_FALSE;

	if ( m_emVehicle == EMVEHICLE_PASSENGER && pNetMsg->emSlot == SLOT_VEHICLE )
		return S_FALSE;

	const SITEMCUSTOM sItemCustom = GET_HOLD_ITEM ();

	BOOL bOk(FALSE);

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( sItemCustom.bVietnamGainItem )
			return S_FALSE;
	}
//#endif
	
	//	�ڰ����ǿ� �����ϴ��� �˻�;
	if ( !ACCEPT_ITEM ( sItemCustom ) )
		return E_FAIL;
	
	//	�ش� ���Կ� ���� �������� �˻�.
	bOk = EMSLOTCHECK_OK == CHECKSLOT_ITEM ( sItemCustom.GetNativeID(), pNetMsg->emSlot );
	if ( !bOk )
		return E_FAIL;

	//	�ش� ������ �񿴴��� ����.
	bOk = ISEMPTY_SLOT ( sItemCustom.GetNativeID(), pNetMsg->emSlot );
	if ( !bOk )
		return E_FAIL;

	//	���� ��� ������ ��� ������ "������ ���" ��������.
	if ( ISBOTH_HANDHELD(pItem) )
		pNetMsg->emSlot = GetCurRHand();

	//	�ش� ���Կ� ������ ����.
	SLOT_ITEM ( sItemCustom, pNetMsg->emSlot );

	//	�տ� �� ������ ����
	RELEASE_HOLD_ITEM ();

	//	������ ����� �� �� ȯ�� �ʿ���.
	INIT_DATA( FALSE, FALSE );

	//	[�ڽſ���] �տ� �־��� ������ ����.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(SLOT_HOLD);
	SendToClient(&NetMsg_PutOn_Release);

	//	[�ڽſ���] �տ� �־��� �������� Slot�� ����.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.m_Slot = pNetMsg->emSlot;
	NetMsg_PutOn.m_Item = sItemCustom;
    msgpack::sbuffer SendBufferB;
    msgpack::pack(SendBufferB, NetMsg_PutOn);
	SendToClient(NET_MSG_GCTRL_PUTON_UPDATE, SendBufferB);

	//	[��ο���] ������ �ٲ��� �˷���.
	GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.m_GaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.m_Slot   = pNetMsg->emSlot;
	NetMsg_PutOn_Brd.m_Item.Assign(sItemCustom);
    msgpack::sbuffer SendBufferC;
    msgpack::pack(SendBufferC, NetMsg_PutOn_Brd);
	SendMsgViewAround(NET_MSG_GCTRL_PUTON_UPDATE_BRD, SendBufferC);

	// ���⿡ ���� ������ �ʱ�ȭ �Ѵ�.
	DisableSkillFact();

	return S_OK;
	*/
}


HRESULT GLChar::MsgReqSlotChange(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_SLOT_CHANGE* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_CHANGE*> ( nmg );

	if ( IsUseArmSub() ) SetUseArmSub( FALSE ); // �ֹ���
	else				SetUseArmSub( TRUE );  // ��������

	//	������ ����� �� �� ȯ�� �ʿ���.
	INIT_DATA( FALSE, FALSE, m_sCONFTING.sOption.fHP_RATE );

	//	[�ڽſ���] ���� ����
	GLMSG::SNETPC_PUTON_CHANGE NetMsg_PutOn;
	NetMsg_PutOn.bUseArmSub = IsUseArmSub();
	NetMsg_PutOn.fCONFT_HP_RATE = m_sCONFTING.sOption.fHP_RATE;
	SendToClient(&NetMsg_PutOn);

	//	[��ο���] ���� ������ �˸�.
	GLMSG::SNETPC_PUTON_CHANGE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.bUseArmSub = IsUseArmSub();
	SendMsgViewAround(&NetMsg_PutOn_Brd);

	GLMSG::SNETPC_PUTON_CHANGE_AG NetMsg_PutOn_AG;
	NetMsg_PutOn_AG.dwChaNum = CharDbNum();
	NetMsg_PutOn_AG.bUseArmSub = IsUseArmSub();
	SendToAgent(&NetMsg_PutOn_AG );


	// ���⿡ ���� ������ �ʱ�ȭ �Ѵ�.
	DisableSkillFact();
	
	return S_OK;
}

bool GLChar::InsertPileItem( const SITEMCUSTOM& sItemCustom, WORD wREQINSRTNUM, std::vector< UPDATED_ITEM >& vecInsertedItems, bool bNew )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if ( !pItem )
		return false;

	if ( !pItem->ISPILE() )
		return false;

	if ( !m_cInventory.ValidPileInsrt( wREQINSRTNUM, pItem->sBasicOp.sNativeID, pItem->sDrugOp.wPileNum ) )
	{
		// �������� ���� ������ ����.
		return false;
	}

	WORD wPILENUM = pItem->sDrugOp.wPileNum;

	// �� �������� �ִ� �������� �ִ��� �˻��Ͽ� �κ��� �����ϴ�
	//		������ ���� ���ڸ� ���ҽ�Ŵ.
	const GLInventory::CELL_MAP& ItemMap = *m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
	GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		SINVENITEM& sINVENITEM = *( *iter ).second;
		SITEMCUSTOM& sITEMCUSTOM = sINVENITEM.sItemCustom;

		if ( sITEMCUSTOM.GetNativeID() != sItemCustom.GetNativeID() )
			continue;

		if ( sITEMCUSTOM.wTurnNum >= wPILENUM )
			continue;

		//	���� ������ ��ħ ������.
		WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

		if ( wREQINSRTNUM > wSURPLUSNUM )
		{
			// ���� �������� ��ħ�� ����. ( ���� )
			sITEMCUSTOM.wTurnNum = wPILENUM;

			// Ŭ���̾�Ʈ�� ���� ������ �˸�.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
			NetMsg.dwChaNum = CharDbNum();
			NetMsg.wPosX = sINVENITEM.wPosX;
			NetMsg.wPosY = sINVENITEM.wPosY;
			NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
			SendToClient( &NetMsg );

			// �ܿ� ���� ����.
			wREQINSRTNUM -= wSURPLUSNUM;

			UPDATED_ITEM inserted( SNATIVEID( sINVENITEM.wPosX, sINVENITEM.wPosY ), wSURPLUSNUM );
			vecInsertedItems.push_back( inserted );
		}
		else
		{
			// ���� �����ۿ� ��ġ�� �� �����ε� ���� ���� �������� ����� ��.
			sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

			// Ŭ���̾�Ʈ�� ���� ������ �˸�.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
			NetMsg.dwChaNum = CharDbNum();
			NetMsg.wPosX = sINVENITEM.wPosX;
			NetMsg.wPosY = sINVENITEM.wPosY;
			NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
			SendToClient( &NetMsg );

			UPDATED_ITEM inserted( SNATIVEID( sINVENITEM.wPosX, sINVENITEM.wPosY ), wREQINSRTNUM );
			vecInsertedItems.push_back( inserted );

			return true;
		}
	}

	// ������ �κ��� ������ �������� ���� �ľ��� �κ��� �� ������ �ִ��� �˻�.

	//	�ִ��ħ������ ����.
	WORD wONENUM = wREQINSRTNUM / wPILENUM;
	WORD wITEMNUM = wONENUM;

	//	���а�ħ�������� ��ħ��.
	WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
	if ( wSPLITNUM > 0 )
		wITEMNUM += 1;
	if ( wSPLITNUM == 0 && wITEMNUM >= 1 )
		wSPLITNUM = wPILENUM;

	for ( WORD i = 0; i < wITEMNUM; ++i )
	{
		WORD wInsertPosX( 0 ), wInsertPosY( 0 );
		BOOL bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

		if ( bITEM_SPACE == FALSE )
		{
			sc::writeLogError( sc::string::format( "Char(%1%) inventory - ���� ������ �̸� üũ�� �ϰ� �������� �־����� ������ ������5", CharDbNum() ) );
			return false;
		}

		// ���ο� �������� �־���.
		SITEMCUSTOM sNewItem( sItemCustom.GetNativeID() );
		sNewItem.tBORNTIME = sItemCustom.tBORNTIME;
		sNewItem.cGenType = sItemCustom.cGenType;
		sc::SeqUniqueGUID::Instance()->getGUID( sNewItem.guid );
		sNewItem.GenerateBasicStat( false );
		sNewItem.GenerateLinkSkill();
		sNewItem.GenerateAddOption();

		if ( wITEMNUM == ( i + 1 ) )
			sNewItem.wTurnNum = wSPLITNUM;	//	������ �������� �ܿ���.
		else
			sNewItem.wTurnNum = wPILENUM;	//	�ƴ� ���� ������.

		if ( !InvenInsertItem( sNewItem, wInsertPosX, wInsertPosY, true, INSERT_INVEN_PILE_ITEM, true, false, bNew ) )
		{
			sc::writeLogError( sc::string::format( "InsertPileItem InvenInsertItem error. chanum %1%", CharDbNum() ) );
		}

		UPDATED_ITEM inserted( SNATIVEID( wInsertPosX, wInsertPosY ), sNewItem.wTurnNum );
		vecInsertedItems.push_back( inserted );
	}

	return true;
}

bool GLChar::DeletePileItem( SNATIVEID sNID, WORD wREQDELNUM )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );
	if ( !pItem )
		return false;
	if ( !pItem->ISPILE() )
		return false;

	std::vector< SNATIVEID > vecDEL;

	// �� �������� �ִ� �������� �ִ��� �˻��Ͽ� �κ��� �����ϴ�
	// ������ ���� ���ڸ� ���ҽ�Ŵ.
	const GLInventory::CELL_MAP& ItemMap = *m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
	for ( ; iter!=ItemMap.end(); ++iter )
	{
		if ( wREQDELNUM == 0 )
			break;

		SINVENITEM& sINVENITEM = *( *iter ).second;
		SITEMCUSTOM& sITEMCUSTOM = sINVENITEM.sItemCustom;
		if ( sITEMCUSTOM.GetNativeID() != sNID )
			continue;

		if ( sITEMCUSTOM.wTurnNum > wREQDELNUM )
		{
			sITEMCUSTOM.wTurnNum -= wREQDELNUM;
			wREQDELNUM = 0;

			// �������� �޽���.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE	NetMsgUpdate;
			NetMsgUpdate.dwChaNum = CharDbNum();
			NetMsgUpdate.wPosX = sINVENITEM.wPosX;
			NetMsgUpdate.wPosY = sINVENITEM.wPosY;
			NetMsgUpdate.wTurnNum = sITEMCUSTOM.wTurnNum;
			SendToClient( &NetMsgUpdate );
		}
		else
		{
			wREQDELNUM -= sITEMCUSTOM.wTurnNum;
			sITEMCUSTOM.wTurnNum = 0;

			vecDEL.push_back( SNATIVEID( sINVENITEM.wPosX, sINVENITEM.wPosY ) );
		}
	}

	if ( vecDEL.empty() )
        return true;

	for ( DWORD i = 0; i < vecDEL.size(); ++i )
        InvenDeleteItem( vecDEL[i].wMainID, vecDEL[i].wSubID, true, FALSE, DELETE_INVEN_ITEM_PILEITEM, true );

	return true;
}

//! NPC �κ��� ������ ����
HRESULT GLChar::MsgReqBuyFromNpc( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_BUY_FROM_NPC* pPacket = ( GLMSG::SNETPC_REQ_BUY_FROM_NPC* ) nmg;
    if ( pPacket->Crc32 != sc::string::getCrc32( pPacket, sizeof( GLMSG::SNETPC_REQ_BUY_FROM_NPC )-sizeof( unsigned int ) ) )
    {
        sc::writeLogError( sc::string::format( "MsgReqBuyFromNpc CRC ChaDbNum %1%", m_CharDbNum ) );
        return E_FAIL;
    }

	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pPacket->sNID );
    if ( !pCrowData || !pCrowData->IsBasicTalk( SNpcTalk::EM_MARKET ) )
    {
        // ���� NPC�� �������� �ʰų� ������ �ƴ�
        return E_FAIL;
    }

    // Note : ��ȭ������ ���� ���� üũ.
    {
        std::tr1::shared_ptr< NpcTalkMan > spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if ( !spTalkMan )
            return E_FAIL;

        std::tr1::shared_ptr< CNpcDialogueSet > spDlgSet = spTalkMan->Find( pCrowData->GetTalkFile() );
        if ( !spDlgSet )
            return E_FAIL;

        DWORD dwID = 1;
        std::tr1::shared_ptr< CNpcDialogue > spDialogue = spDlgSet->GetDialogue( dwID );
        if ( !spDialogue )
            return E_FAIL;

        DWORD dwPartyMemNum = 0;
		GLPartyField* const pParty = GetParty( GetPartyID() );
        if ( pParty != NULL )
            dwPartyMemNum = pParty->GetNMember();

		CTime cCurSvrTime = CTime::GetCurrentTime();
        DWORD dwIndex = spDialogue->FindConditionSatisfied( this, dwPartyMemNum, cCurSvrTime.GetHour() );
        if ( dwIndex==UINT_MAX )
            return E_FAIL;

        std::tr1::shared_ptr< CNpcDialogueCase > spCase = spDialogue->GetDlgCase( dwIndex );
        if ( !spCase )
            return E_FAIL;

        std::tr1::shared_ptr< CNpcTalkControl > spNpcTalkControl = spCase->GetTalkControl();
        if ( !spNpcTalkControl )
            return E_FAIL;

        CNpcTalkControl::NPCMAP* pMap = spNpcTalkControl->GetTalkMap();
        if ( !pMap )
            return E_FAIL;

        CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
        CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();

        for ( ; iter != iter_end; ++iter )
        {
            std::tr1::shared_ptr< SNpcTalk > spNpcTalk = iter->second;
            if ( !spNpcTalk )
                return E_FAIL;

            if ( spNpcTalk->m_nACTION == SNpcTalk::EM_BASIC && 
				spNpcTalk->m_dwACTION_NO == SNpcTalk::EM_MARKET )
            {
                if ( !spNpcTalk->DoTEST( this, dwPartyMemNum, cCurSvrTime.GetHour() ) )
                    return E_FAIL;
            }
        }
    }
    
    // �ǸŰ��� üũ
    if ( pCrowData->GetSaleNum() <= pPacket->dwChannel )
        return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	ServerActor* pCrow = NULL;

	switch( pPacket->emCrow )
	{
	case CROW_NPC :
		{
			// ���� NPC ��ȿ�� üũ
			GLCrow* pTCrow = pLand->GetCrow( pPacket->dwNpcGlobID );
			if (!pTCrow)
				return E_FAIL;

			if ( !pTCrow->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )
			{
				return E_FAIL;
			}

			pCrow = dynamic_cast<ServerActor*>(pTCrow);
		}
		break;
	case CROW_SUMMON :
		{
			// ���� NPC ��ȿ�� üũ
			GLSummonField* pTCrow = pLand->GetSummon( pPacket->dwNpcGlobID );
			if (!pTCrow)
				return E_FAIL;

			if( !pTCrow->IsSummonNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) ) return E_FAIL;

			if( pTCrow->GetSummonType() != SUMMON_TYPE_NPC ) return E_FAIL;

			pCrow = dynamic_cast<ServerActor*>(pTCrow);
		}
		break;
	}

	//  [12/15/2015 gbgim]
	// ������ ��ȯ ���������� ��Ÿ������ ������ ���� ��û�ϸ� ������ ���ŵ�;
	GLInventorySale* pInven = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( pPacket->dwChannel ) );
	if ( !pInven || pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_1 )
		return E_FAIL;

	SINVENITEM* pSaleItem( NULL );
	pSaleItem = pInven->GetItem( pPacket->wPosX, pPacket->wPosY );

	// ����� �ϴ� �������� �����ϴ�.
	if ( !pSaleItem )
        return E_FAIL;

	// ������ ���� ����
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pSaleItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

	volatile float fSHOP_RATE = GET_PK_SHOP2BUY();
	if ( fSHOP_RATE==0.0f )
        return E_FAIL;

	volatile float fSHOP_2RATE = fSHOP_RATE + pLand->GetCommissionRate();

	LONGLONG dwPrice = pCrowData->GetNpcSellPrice( pItem->sBasicOp.sNativeID.dwID );
	volatile LONGLONG llPRICE2 = 0; // ��ǰ����
	if ( dwPrice == 0 )
	{
		dwPrice = pItem->sBasicOp.dwBuyPrice;
		llPRICE2 = dwPrice * static_cast< LONGLONG >( fSHOP_2RATE ) / 100;
	}
    else
    {
		llPRICE2 = dwPrice;
	}

	// ���Ű���=��ǰ���� X ����
	volatile LONGLONG llPRICE3 = static_cast< LONGLONG >( llPRICE2*pPacket->wBuyNum );
	if ( GetInvenMoney() < llPRICE3 )
	{
		//	���� �����մϴ�.
		return E_FAIL;
	}

	/*
	// ������ ���Žÿ��� ����Ʈ�� �⿩���� Ȯ������ �ʵ��� ���� 
	// ���� ���ǿ��� �˻��ϹǷ� ������ ���� ����

    if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
		return E_FAIL;
    }

	if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
	{
		//	�⿩ ����Ʈ�� �����մϴ�.
		return E_FAIL;
	}
	*/

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqBuyFromNpc", m_CharDbNum ) );
	}

	// �κ��� ���� ������ �ִ��� �˻�
	CTime cTIME = CTime::GetCurrentTime();
	BOOL bITEM_SPACE( FALSE );
	if ( pItem->ISPILE() )
	{
		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		WORD wREQINSRTNUM = ( pPacket->wBuyNum * pItem->GETAPPLYNUM() );

		bITEM_SPACE = m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );
		
		//	�κ��� ���������� ���� ��� ��ҵ�.
		if ( !bITEM_SPACE )
            return E_FAIL;

		// ������ �����մϴ�.
		CheckMoneyUpdate( GetInvenMoney(), llPRICE3, FALSE, "Buy From Npc.");

		SubtractInvenMoneyServer( llPRICE3, SUBTRACT_INVEN_MONEY_ITEM_BUY_NPC );	// ���� ����.

		// �� �α�.
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPRICE3, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_BUY_FROM_NPC );
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

		/*if ( m_bTracingUser )
		{
			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount( m_szUID );

			CString strTemp;
			strTemp.Format( "Buy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]", m_szUID, m_szName,  llPRICE3, GetInvenMoney() );

			TracingMsg.SetLog( strTemp.GetString() );
			SendToAgent( &TracingMsg );
		}*/

		// �������� �κ��� �־��ش�.
		SITEMCUSTOM sNewItem( sNID );
		sNewItem.tBORNTIME = cTIME.GetTime();
		sNewItem.cGenType = EMGEN_SHOP;
		std::vector< UPDATED_ITEM > vecInsertedItems;
		if ( InsertPileItem( sNewItem, wREQINSRTNUM, vecInsertedItems, true ) )
		{
			size_t InsertedItemSize = vecInsertedItems.size();
			for ( size_t loop = 0; loop < InsertedItemSize; loop++ )
			{
				const SINVENITEM* pInvenItem =  m_cInventory.GetItem( vecInsertedItems[loop].sPosition.wMainID, vecInsertedItems[loop].sPosition.wSubID );
				if ( !pInvenItem )
				{
					continue;
				}

				// ���� ������ ����ǰ�, ���Ե� �������� �α׸� �����. vecInsertedItems �ȿ� xy ��ǥ�� ���Ե� ������ ������ ��� �ִ�.
				// ���� �����ۿ� ������ �͵� �����װ�, �űԷ� ������ �����۵� ��������.
				// ��� �� �α� ����
				LogItemExchange( 
					pInvenItem->sItemCustom, 
					gamelog::ID_CHAR, 0, 
					gamelog::ID_CHAR, m_CharDbNum, 
					ITEM_ROUTE_CHAR, vecInsertedItems[loop].wCount, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_BUY_ITEM_FROM_NPC, 
					true );
			}
		}
	}
	else
	{
		//	�Ϲ� �������� ���.
		WORD wInsertPosX( 0 ), wInsertPosY( 0 );
		bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

		//	�κ��� ���������� ���� ��� ��ҵ�.
		if ( !bITEM_SPACE )
			return E_FAIL;

		// ���ο� �������� �����մϴ�
		SITEMCUSTOM sNewItem( pSaleItem->sItemCustom );
		sNewItem.tBORNTIME = cTIME.GetTime();
		sNewItem.wTurnNum = 1;
		sNewItem.cGenType = EMGEN_SHOP;
		sc::SeqUniqueGUID::Instance()->getGUID( sNewItem.guid );
		sNewItem.GenerateBasicStat( false );
		sNewItem.GenerateLinkSkill();
		sNewItem.GenerateAddOption();

		// ���� �����մϴ�.
		CheckMoneyUpdate( GetInvenMoney(), llPRICE2, FALSE, "Buy From Npc." );
		SubtractInvenMoneyServer( llPRICE2, SUBTRACT_INVEN_MONEY_ITEM_BUY_NPC );

		/*if ( m_bTracingUser )
		{
			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount( m_szUID );

			CString strTemp;
			strTemp.Format(
				"Buy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]",
				m_szUID, m_szName, llPRICE3, GetInvenMoney() );

			TracingMsg.SetLog( strTemp.GetString() );
			SendToAgent( &TracingMsg );
		}*/

		// �κ��丮�� �־��ݴϴ�
		if ( !InvenInsertItem( sNewItem, wInsertPosX, wInsertPosY, true, 
			  INSERT_INVEN_BUY_FROM_NPC_NOT_PILEITEM, true, false, true ) )
		{
			return S_OK;
		}

		// �������� ���� ���� ��� ���
		// ��� �� �α� ����
		LogItemExchange(
            sNewItem,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_CHAR, sNewItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_BUY_ITEM_FROM_NPC,
			true );

        //	�� �α�.
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPRICE2, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_BUY_FROM_NPC );
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	}

    volatile LONGLONG llPRICE1 = dwPrice * static_cast< LONGLONG >( fSHOP_RATE ) / 100;
    volatile LONGLONG llCommission = ( llPRICE2 - llPRICE1 ) * static_cast< LONGLONG >( pPacket->wBuyNum );

	BOOL bRecall = FALSE;

	switch( pPacket->emCrow )
	{
	case CROW_NPC :
		{
			GLCrow* pTCrow = dynamic_cast<GLCrow*>(pCrow);

			if( pTCrow && pTCrow->m_bNpcRecall )
			{
				llCommission = LONGLONG ( llCommission * GLCONST_CHAR::fEARNING_RATE_NPC + 0.1f );

				pTCrow->m_lnCommission += llCommission;

				bRecall = TRUE;
			}
		}
		break;
	}

	if( !bRecall && pLand->m_dwGuidClubID != CLUB_NULL )
	{
		GLGuidance* pGuid = pLand->m_pGuidance;
		if ( pGuid && !pGuid->m_bBattle )
		{
			// Ŭ���� ���� �߻�.
			std::tr1::shared_ptr< GLClubField > pClub = GetClub( pLand->m_dwGuidClubID );
			if ( pClub )
			{
				pClub->AddProfitMoeny( llCommission );

				// Ŭ�� ������ 1�� �̻��� ��� �α׸� �����.
				if ( llCommission > 	EMCLUBINCOMEMONEY_LOG )
                {   
					// ��� �� �α� ����
					LogMoneyExchange(
                        gamelog::ID_CHAR, 0, 
                        gamelog::ID_CLUB, pClub->m_DbNum,
                        llCommission,
                        MONEY_ROUTE_CLUB_INCOME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION );
                }
			}
		}
	}
    pLand->AddBuyCommissionMoney( llCommission );

	//	�ݾ� ��ȭ �޽���.
	SendInvenMoneyUpdate();

	return S_OK;
}

HRESULT GLChar::MsgReqExchangeBuyFromNpc( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC* pPacket = ( GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC * ) nmg;

	if ( pPacket->Crc32 != sc::string::getCrc32( pPacket, sizeof( GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC ) - sizeof( unsigned int ) ) )
	{
		sc::writeLogError( sc::string::format( "MsgReqExchangeBuyFromNpc CRC ChaDbNum %1%", m_CharDbNum ) );
		return E_FAIL;
	}

	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pPacket->sNID );
	if ( !pCrowData || !pCrowData->IsBasicTalk( SNpcTalk::EM_MARKET ) )
	{
		// ���� NPC�� �������� �ʰų� ������ �ƴ�
		return E_FAIL;
	}

	{
		std::tr1::shared_ptr< NpcTalkMan > spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
		if ( !spTalkMan )
			return E_FAIL;

		std::tr1::shared_ptr< CNpcDialogueSet > spDlgSet = spTalkMan->Find( pCrowData->GetTalkFile() );
		if ( !spDlgSet )
			return E_FAIL;

		DWORD dwID = 1;
		std::tr1::shared_ptr< CNpcDialogue > spDialogue = spDlgSet->GetDialogue( dwID );
		if ( !spDialogue )
			return E_FAIL;

		DWORD dwPartyMemNum = 0;
		GLPartyField* const pParty = GetParty( GetPartyID() );
		if ( pParty != NULL )
			dwPartyMemNum = pParty->GetNMember();

		CTime cCurSvrTime = CTime::GetCurrentTime();
		DWORD dwIndex = spDialogue->FindConditionSatisfied( this, dwPartyMemNum, cCurSvrTime.GetHour() );
		if ( dwIndex==UINT_MAX )
			return E_FAIL;

		std::tr1::shared_ptr< CNpcDialogueCase > spCase = spDialogue->GetDlgCase( dwIndex );
		if ( !spCase )
			return E_FAIL;

		std::tr1::shared_ptr< CNpcTalkControl > spNpcTalkControl = spCase->GetTalkControl();
		if ( !spNpcTalkControl )
			return E_FAIL;

		CNpcTalkControl::NPCMAP* pMap = spNpcTalkControl->GetTalkMap();
		if ( !pMap )
			return E_FAIL;

		CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
		CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();
		for ( ; iter != iter_end; ++iter )
		{
			std::tr1::shared_ptr< SNpcTalk > spNpcTalk = iter->second;
			if ( !spNpcTalk )
				return E_FAIL;

			if ( spNpcTalk->m_nACTION == SNpcTalk::EM_BASIC && 
				spNpcTalk->m_dwACTION_NO == SNpcTalk::EM_MARKET )
			{
				if ( !spNpcTalk->DoTEST( this, dwPartyMemNum, cCurSvrTime.GetHour() ) )
					return E_FAIL;
			}
		}
	}

	//if (pCrowData->GetSaleNum() <= pPacket->dwChannel)
	//	return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	switch( pPacket->emCrow )
	{
	case CROW_SUMMON :
		{
			// ���� NPC ��ȿ�� üũ 
			GLSummonField* pCrow = pLand->GetSummon( pPacket->dwNpcGlobID );
			if (!pCrow) return E_FAIL;

			if ( !pCrow->IsSummonNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )		return E_FAIL;

			if( pCrow->GetSummonType() != SUMMON_TYPE_NPC ) return E_FAIL;
		}
		break;
	case CROW_NPC    :
		{
			// ���� NPC ��ȿ�� üũ 
			GLCrow* pCrow = pLand->GetCrow( pPacket->dwNpcGlobID );

			if (!pCrow) return E_FAIL;
			if ( !pCrow->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )
			{
				return E_FAIL;
			}
		}
		break;
	}
	
	//GLInventorySale* pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven(pPacket->dwChannel));

	//if( !pInven || pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_2 )
	//	return E_FAIL;

	GLInventorySale* pInven = NULL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqExchangeBuyFromNpc", m_CharDbNum ) );
	}

	GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC MsgToClient;

	MsgToClient.emCrow = pPacket->emCrow;

	for( int i = 0; i < GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC::MAX_ITEM_LIST; ++i )
	{	
		if (pCrowData->GetSaleNum() <= pPacket->itemlist[i].dwChannel) continue;

		pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven(pPacket->itemlist[i].dwChannel));

		if( pInven == NULL ) continue;

		if( pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_2 ) continue;

		SINVENITEM* pSaleItem = pInven->FindPosItem( pPacket->itemlist[i].wPosX, pPacket->itemlist[i].wPosY );
		if ( pSaleItem == NULL )
			continue;
				
		EEXTYPE etype = pInven->GetExChangeType( pSaleItem->GetNativeID(), pPacket->itemlist[i].wPosX, pPacket->itemlist[i].wPosY );

		const SITEM* pExItem = GLogicData::GetInstance().GetItem( pSaleItem->GetNativeID() );

		std::vector< sExNeed > vListExChange;
		
		if ( pExItem == NULL || etype == EX_NONE )
			continue;

		BOOL bNeed = pInven->GetIsNeedConditionHave( vListExChange, &m_cInventory, pSaleItem->GetNativeID(), pPacket->itemlist[i].wPosX, pPacket->itemlist[i].wPosY, PointShopPoint(), m_nLiving, pPacket->itemlist[i].wBuyNum );
		// �ʿ���� üũ
		if ( !bNeed )
		{			
			switch( etype )
			{
			case EX_ITEM_TYPE  : MsgToClient.ReValue = EMREQ_NEI_EX_NITEM_FB; break;
			case EX_POINT_TYPE : MsgToClient.ReValue = EMREQ_NEI_EX_NRPT_FB;  break;
			case EX_LVP_TYPE   : MsgToClient.ReValue = EMREQ_NEI_EX_NLVPT_FB; break;
			}

			SendToClient( &MsgToClient );
			continue;
		}

		// ������ �ִ� üũ
		BOOL bInsert = pInven->IsUserExInsertEnable( &m_cInventory, pSaleItem->GetNativeID(), pPacket->itemlist[i].wPosX, pPacket->itemlist[i].wPosY, 0, 0, pPacket->itemlist[i].wBuyNum );
		if ( !bInsert )
		{
			MsgToClient.ReValue = EMREQ_NEI_EX_NSPACE_FB;
			SendToClient( &MsgToClient );
			continue;
		}

		bool bEraseFail = false;
					
		switch( etype )
		{
		case EX_ITEM_TYPE :
			{
				std::vector< sExNeed >::iterator _iter = vListExChange.begin();
				
				while ( _iter != vListExChange.end() )
				{
					SNATIVEID itemId = ( *_iter ).sNativeID;
					__int64 iCount64 = ( *_iter ).iNeedCount;

					const SITEM* pItem = GLogicData::GetInstance().GetItem( itemId );

					SINVENITEM* pInvenItem = m_cInventory.FindItem( itemId );

					if ( pItem && pInvenItem ) {

						bool bResult = pItem->ISPILE() ? DeletePileItem( itemId, static_cast< WORD >( iCount64 ) ) : InvenCountDeleteItem( pInvenItem->CustomItem(), static_cast< WORD >( iCount64 ), true, FALSE );

						if ( !bResult ) {
							bEraseFail = true;
						}
						else {
							LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, static_cast< int >( iCount64 ), GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_NPC_EXCHANGE, true );
						}
					}
					else {
						bEraseFail = true;
					}
					
					++_iter;
				}						
			}
			break;
		case EX_POINT_TYPE :	// ������Ʈ ����
			{
				std::vector< sExNeed >::iterator _iter = vListExChange.begin();

				while ( _iter != vListExChange.end() )
				{
					PointShopPointSub( _iter->iNeedCount, pointshop::NPC_EXCHANGE_ITEM,  pointshop::POINT_SUB_TYPE_EXCHANGE_NPC );

					++_iter;
				}				
			}
			break;
		case EX_LVP_TYPE :		// ��Ȱ����Ʈ ����
			{

				std::vector< sExNeed >::iterator _iter = vListExChange.begin();

				while ( _iter != vListExChange.end() )
				{
					int iLp = static_cast< int >( _iter->iNeedCount );

					iLp *= -1;

					ReceiveLivingPoint( iLp );

					++_iter;
				}	
			}
			break;
		default: bEraseFail = true; break;
		}

		if ( !bEraseFail ) {

			SITEMCUSTOM sITEM_NEW = pSaleItem->sItemCustom;

			if ( pExItem->ISPILE() )
			{
				//	��ħ �������� ���.
				WORD wPILENUM  = pExItem->sDrugOp.wPileNum;
				SNATIVEID sNID = pExItem->sBasicOp.sNativeID;
				WORD wREQINSRTNUM = ( pPacket->itemlist[i].wBuyNum * pExItem->GETAPPLYNUM() );

				//	�������� ���� ���� ��� ���.
				sITEM_NEW.cGenType  = EMGEN_SHOP;
				sITEM_NEW.wTurnNum  = ( pPacket->itemlist[i].wBuyNum * pExItem->GETAPPLYNUM() );
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();

				// ������ ��ȯ ��� �϶��� �����ɼ��� 1���̻� �ٵ��� RM 1172
				// 20160101 khkim ��ħ �������� �����ɼ��� ���ǹ��� �������� �ʴ´�. ���� InsertPileItem �� �Լ��ȿ����� �����ɼ��� ���� �ʴ´�.
				//if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
				//	LogItemRandomOption( sITEM_NEW );

				// #item ������ �α� vecInsertedItems ����ؼ� �ٽ� ������. ���߿� �۾� �ʿ�
				std::vector< UPDATED_ITEM > vecInsertedItems;
				InsertPileItem( sITEM_NEW, wREQINSRTNUM, vecInsertedItems, true );

				LogItemExchange( sITEM_NEW, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_EXCHANGE, static_cast< int >( sITEM_NEW.wTurnNum ), GetCurrentMap(), LOG_ITEM_GET_ITEM_BY_NPC, true );
			}
			else
			{
				for( int i = 0; i < pPacket->itemlist[i].wBuyNum; ++i )
				{
					WORD wInsertPosX( 0 ), wInsertPosY( 0 );
					m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

					// ���ο� �������� �����մϴ�
					SNATIVEID sNID        = pSaleItem->sItemCustom.GetNativeID();					
					CTime cTIME           = CTime::GetCurrentTime();
					sITEM_NEW.tBORNTIME   = cTIME.GetTime();

					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_SHOP;
					sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

					LogItemBasicStatOption( sITEM_NEW );
					LogItemLinkSkillOption( sITEM_NEW );
					LogItemAddonOption( sITEM_NEW );

					// ������ ��ȯ ��� �϶��� �����ɼ��� 1���̻� �ٵ��� RM 1172
					if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
						LogItemRandomOption( sITEM_NEW );

					InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, 19, true, false, true );

					LogItemExchange( sITEM_NEW, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_EXCHANGE, static_cast<int>(sITEM_NEW.wTurnNum), GetCurrentMap(), LOG_ITEM_GET_ITEM_BY_NPC, true );
				}
			}
			
			MsgToClient.sNID    = pSaleItem->GetNativeID();
			MsgToClient.ReValue = EMREQ_NEI_EX_SUCCESS;
		}
		else
		{
			MsgToClient.ReValue = EMREQ_NEI_EX_FAILED;
		}

		SendToClient( &MsgToClient );
	}

	return S_OK;
}

HRESULT GLChar::MsgBuyRebuyItem( const SITEMCUSTOM& sItemCustom, LONGLONG Price )
{
    GLMSG::SNET_BUY_REBUY_ITEM_FC NetMsgFB( sItemCustom, Price );
    GLMSG::SNET_BUY_REBUY_ITEM_FA NetMsgFAB( sItemCustom, Price );
    NetMsgFAB.dwChaNum = CharDbNum();

    // �⺻������ ����. ������ ��� ���� ���� ����.
    HRESULT hr = S_OK;
    NetMsgFB.emFB = EMBUYREBUYITEM_SUCCESS;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
    {
        hr = E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

    // ������ ���� ����
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
    if ( !pItem )
    {
        hr =  E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

    volatile double fSHOP_RATE = GET_PK_SHOP2BUY();
    if ( fSHOP_RATE==0.0f )
    {
        hr = E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

    // ���Ű���=��ǰ����
    volatile LONGLONG llPrice = Price;
    if ( GetInvenMoney() < llPrice )
    {
        //	���� �����մϴ�.
        hr = E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_MONEY;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

	/*
	// ������ ���Žÿ��� ����Ʈ�� �⿩���� Ȯ������ �ʵ��� ���� 
	// ���� ���ǿ��� �˻��ϹǷ� ������ ���� ����
    if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
        hr =  E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_CAN_NOT_BUY_ITEM;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
        hr = E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_CAN_NOT_BUY_ITEM;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }
	*/

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgBuyRebuyItem", m_CharDbNum ) );
	}
	
	DWORD dwVehicleID = sItemCustom.m_VehicleDbNum;
	
    // �κ��� ���� ������ �ִ��� �˻�
    BOOL bITEM_SPACE( FALSE );
    if ( pItem->ISPILE() )
    {
        //	��ħ �������� ���.
        WORD wPILENUM = pItem->sDrugOp.wPileNum;
        SNATIVEID sNID = pItem->sBasicOp.sNativeID;

        //	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
        WORD wREQINSRTNUM = sItemCustom.wTurnNum;

        bITEM_SPACE = m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );

        //	�κ��� ���������� ���� ��� ��ҵ�.
        if ( !bITEM_SPACE )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_INVEN;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

        // ������ �����մϴ�.
        CheckMoneyUpdate( GetInvenMoney(), llPrice, FALSE, "ReBuy From Npc." );

		SubtractInvenMoneyServer( llPrice, SUBTRACT_INVEN_MONEY_ITEM_REBUY_NPC );	// ���� ����.

		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_REBUY );
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

		/*if ( m_bTracingUser )
		{
			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount( m_szUID );

			CString strTemp;
			strTemp.Format(
				"Buy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]",
				m_szUID, m_szName,  llPrice, GetInvenMoney() );

			TracingMsg.SetLog( strTemp.GetString() );
			SendToAgent( &TracingMsg );
		}*/

        // �������� �κ��� �־��ش�.
        // ��ħ���� �������� ��� InsertPileItem �ȿ��� ���� �������� ������ ���� ä���ش�. 
		// ���� �� ä���൵ ���� ��쿡�� �űԷ� �������� �����Ѵ�. �ű� �������� �����ȴ�. 
		// ������ ��Ÿ���� EMGEN_REBUY �̴�. 
		// ��ħ�������� ��� ������ �ȸ� ��� ������ ���̺� �κ�Ÿ���� 0���� ������Ʈ �Ǿ�����(�Ϲ� �����۰� ������ ��������)
		// �Ʒ� �Ϲ� ������ ó�����ó�� ��� ������Ʈ ������ �ʴ´�. ��ħ�������� �κ��� �� ���� ������ ������ ��ó�� ���� �����ۿ�
		// ������ �߰��ǰų� �űԷ� �������� �����Ǵ� ����̴�. �׷��� ������ db::DB_INSERT �ȴ�.

		// �������� ���� ���� ��� ���.
		SITEMCUSTOM sNewItem( sNID );
		sNewItem.cGenType = EMGEN_REBUY;
		sNewItem.wTurnNum = wREQINSRTNUM;
		sNewItem.tBORNTIME = sItemCustom.tBORNTIME;	// rm #1438 : �Ⱓ�� �����۸� �켱 �����غ���. 20151223 khkim �����ð��� �⺻���� �־��ش�.

		std::vector< UPDATED_ITEM > vecInsertedItems;
        bool bInsert = InsertPileItem( sNewItem, wREQINSRTNUM, vecInsertedItems, true );
        if ( !bInsert )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_INVEN;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

		size_t InsertedItemSize = vecInsertedItems.size();
		for ( size_t loop = 0; loop < InsertedItemSize; loop++ )
		{
			const SINVENITEM* pInvenItem =  m_cInventory.GetItem( vecInsertedItems[loop].sPosition.wMainID, vecInsertedItems[loop].sPosition.wSubID );
			if ( !pInvenItem )
			{
				continue;
			}

			// ��� �� �α� ����
			LogItemExchange(
				pInvenItem->sItemCustom,
				gamelog::ID_CHAR, 0,
				gamelog::ID_CHAR, m_CharDbNum,
				ITEM_ROUTE_SHOP, vecInsertedItems[loop].wCount, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_REBUY,
				true );
		}
    }
    else
    {
        //	�Ϲ� �������� ���.
        WORD wInsertPosX( 0 ), wInsertPosY( 0 );
        bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

        //	�κ��� ���������� ���� ��� ��ҵ�.
        if ( !bITEM_SPACE )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_INVEN;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

        // ���ο� �������� �����մϴ�.
		// Ȯ�强�� ����ؼ�, ���ȣ�� Ż�͹�ȣ�� �������� guid �� ����� ��츦 ����ؼ� ��ħ�Ұ� �������� guid �״�� ����ϵ��� ����.
		// ��������ڿ��� guid ����. guid �� ���� �������� �ʴ´�.
        SITEMCUSTOM sNewItem( sItemCustom );
        sNewItem.tBORNTIME = sItemCustom.tBORNTIME;
        sNewItem.wTurnNum = sItemCustom.wTurnNum;
        sNewItem.cGenType = EMGEN_REBUY;

		// �̹� ��� ������ ���̺� �ִ� �������̴�. ������ �Ǹŵ� �� �κ�Ÿ�Ը� 0���� ������ �Ǿ���.
		// ������Ʈ�� �ʿ��� �������̴�. �κ��� �ٽ� ���Եǰ�, ��񿡼��� InvenType �� 0 ���� �κ��丮�� ����ȴ�.
		sNewItem.SetDbState( db::DB_UPDATE );

        // ���� �����մϴ�.
        CheckMoneyUpdate( GetInvenMoney(), llPrice, FALSE, "ReBuy From Npc." );

		SubtractInvenMoneyServer( llPrice, SUBTRACT_INVEN_MONEY_ITEM_REBUY_NPC );

		/*if ( m_bTracingUser )
		{
			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount( m_szUID );

			CString strTemp;
			strTemp.Format(
				"ReBuy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]",
				m_szUID, m_szName,  llPrice, GetInvenMoney() );

			TracingMsg.SetLog( strTemp.GetString() );
			SendToAgent( &TracingMsg );
		}*/

        // �κ��丮�� �־��ݴϴ�
		if ( !InvenInsertItem( sNewItem, wInsertPosX, wInsertPosY, true, INSERT_INVEN_REBUY_ITEM, true, false, true ) )
		{
			return S_OK;
		}

        // �������� ���� ���� ��� ���
		// ��� �� �α� ����
        LogItemExchange(
            sNewItem,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_SHOP, sNewItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_REBUY,
			true );

        //	�� �α�.
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_REBUY );
		// ��� �� �α� ����
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

        SINVENITEM* pInvenItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
        if ( !pInvenItem )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

        HRESULT hrCheckDuration = CheckItemDuration( *pInvenItem );
        if ( hrCheckDuration == E_FAIL )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_OVER_DURATION;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }
    }
    
    volatile LONGLONG llPRICE1 = llPrice * static_cast<LONGLONG>( fSHOP_RATE ) / 100;
    volatile LONGLONG llCommission = llPrice - llPRICE1;

    if ( pLand->m_dwGuidClubID != CLUB_NULL )
    {
        GLGuidance* pGuid = pLand->m_pGuidance;
        if ( pGuid && !pGuid->m_bBattle )
        {
            // Ŭ���� ���� �߻�.
            std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
            if ( pCLUB )
            {
                pCLUB->AddProfitMoeny( llCommission );

                // Ŭ�� ������ 1�� �̻��� ��� �α׸� �����.
                if ( llCommission > 	EMCLUBINCOMEMONEY_LOG )
                {   
					// ��� �� �α� ����
                    LogMoneyExchange(
                        gamelog::ID_CHAR, 0, 
                        gamelog::ID_CLUB, pCLUB->m_DbNum,
                        llCommission,
                        MONEY_ROUTE_CLUB_INCOME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION );
                }
/*
                sc::writeLogInfo(
                    sc::string::format(
                        "ReBuyFromNpc ClubID %1% CharID %2% Commission %3% IncomeMoney %4%",
                        pCLUB->m_DbNum,
                        m_CharDbNum,
                        llCommission,
                        pCLUB->GetProfitMoney()));
*/
            }
        }
    }
    pLand->AddBuyCommissionMoney( llCommission );

    //	�ݾ� ��ȭ �޽���.
    SendInvenMoneyUpdate();

    NetMsgFAB.emFB = NetMsgFB.emFB;

    SendToClient( &NetMsgFB );
    SendToAgent( &NetMsgFAB );

	if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && dwVehicleID != 0 )
	{
		AddGameAdoJob( db::DbActionPtr( new CRebuyVehicle( m_ClientSlot, m_CharDbNum, dwVehicleID ) ) );
	}

    return hr;
}

// *****************************************************
// Desc: NPC ���� ������ �Ǹ�
// *****************************************************
HRESULT GLChar::MsgReqSaleToNpc( NET_MSG_GENERIC* nmg )
{
	// ���� ������ ���� �����ϰ� ���� �־��ִ°� �ƴ� ���� ���� �־��ְ� �������� �����ϰ� �ִ�.
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqSaleToNpc", m_CharDbNum ) );
	}

	GLMSG::SNETPC_REQ_SALE_TO_NPC* pNetMsg = ( GLMSG::SNETPC_REQ_SALE_TO_NPC* ) nmg;
    if ( pNetMsg->Crc32 != sc::string::getCrc32( pNetMsg, sizeof( GLMSG::SNETPC_REQ_SALE_TO_NPC )-sizeof( unsigned int ) ) )
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqSaleToNpc ChaDbNum %1%", m_CharDbNum ) );
        return E_FAIL;
    }

	ServerActor* pCrow = NULL;
	
	switch( pNetMsg->emCrow )
	{
	case CROW_NPC :
		{
			// ���� NPC ��ȿ�� üũ
			GLCrow* pTCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
			if (!pTCrow)
				return E_FAIL;

			if ( !pTCrow->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )	return E_FAIL;

			pCrow = dynamic_cast<ServerActor*>(pTCrow);
		}
		break;
	case CROW_SUMMON :
		{
			// ���� NPC ��ȿ�� üũ
			GLSummonField* pTCrow = pLand->GetSummon( pNetMsg->dwNpcGlobID );
			if (!pTCrow)
				return E_FAIL;

			if ( !pTCrow->IsSummonNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) ) return E_FAIL;

			if( pTCrow->GetSummonType() != SUMMON_TYPE_NPC ) return E_FAIL;

			pCrow = dynamic_cast<ServerActor*>(pTCrow);
		}
		break;
	}
	
    SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pNetMsg->sNID );
	if ( !pCrowData )
	{
		//	���� NPC�� �������� �ʽ��ϴ�.
		return E_FAIL;
	}

	if (!pCrowData->IsBasicTalk( SNpcTalk::EM_MARKET ) )
	{
		//	������ �ƴմϴ�.
		return E_FAIL;
	}

	SITEMCUSTOM sITEM;
    bool        bHold = false;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

    if ( pNetMsg->wPosX == USHRT_MAX || pNetMsg->wPosY == USHRT_MAX )
    {
        if ( !pHoldInvenItem )
        {
            //	�ȷ��� �ϴ� �������� �����ϴ�.
            return E_FAIL;
        }

        sITEM = pHoldInvenItem->CustomItem();
        bHold = true;
    }
    else
    {
        SINVENITEM* pInvenItem = m_cInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );

        if ( !pInvenItem )
        {
            //	�ȷ��� �ϴ� �������� �����ϴ�.
            return E_FAIL;
        }

        sITEM = pInvenItem->sItemCustom;
    }

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( pItem == NULL )						
        return E_FAIL;

	// �־��̵�(DB���ٿ�)
	DWORD dwPetID = sITEM.PetDbNum();
	DWORD dwVehicleID = sITEM.m_VehicleDbNum;

	//	�ŷ��ɼ�
	if ( !( sITEM.IsSale() != NULL ) )
	{
		//	�ȱ� �������� ����.
		return E_FAIL;
	}

//  Ŭ���̾�Ʈ�� ���� ������ ��� ������ Ʋ���� ������ 
/* 
	volatile float fSHOP_SALE_RATE = GET_PK_SHOP2SALE();
	if ( fSHOP_SALE_RATE==0.0f )			return E_FAIL;

	volatile LONGLONG llPRICE = LONGLONG( pItem->GETPRICE(sITEM.wTurnNum) * GLCONST_CHAR::fDISPRICE );

	volatile float fSHOP_SALE_2RATE = fSHOP_SALE_RATE - pLand->m_fCommissionRate;
	volatile float fSHOP_SALE_2RATE_C = (DOUBLE(fSHOP_SALE_2RATE) * 0.01f );
	volatile LONGLONG llDisPRICE2 = LONGLONG( llPRICE * fSHOP_SALE_2RATE_C );
*/

	volatile float fSHOP_SALE_RATE = GET_PK_SHOP2SALE();
	if ( fSHOP_SALE_RATE == 0.0f )			
        return E_FAIL;
	
	volatile LONGLONG dwPrice = pItem->GETSELLPRICE ( sITEM.wTurnNum );  

	volatile float fSHOP_SALE_2RATE = fSHOP_SALE_RATE - pLand->GetCommissionRate();
	volatile float fSHOP_SALE_2RATE_C = fSHOP_SALE_2RATE * 0.01f;

	volatile LONGLONG llDisPRICE2 = LONGLONG( dwPrice * fSHOP_SALE_2RATE_C );


    volatile float fSHOP_RATE_C = fSHOP_SALE_RATE * 0.01f;
    volatile LONGLONG llDisPRICE1 = LONGLONG ( dwPrice * fSHOP_RATE_C );
    volatile LONGLONG llCommission = llDisPRICE1 - llDisPRICE2;

	BOOL bRecall = FALSE;

	switch( pNetMsg->emCrow )
	{
	case CROW_NPC :
		{
			GLCrow* pTCrow = dynamic_cast<GLCrow*>(pCrow);

			if( pTCrow && pTCrow->m_bNpcRecall )
			{
				llCommission = LONGLONG ( llCommission * GLCONST_CHAR::fEARNING_RATE_NPC + 0.1f );

				pTCrow->m_lnCommission += llCommission;

				bRecall = TRUE;
			}
		}
		break;
	}

	if ( !bRecall && pLand->m_dwGuidClubID != CLUB_NULL )
	{
		GLGuidance *pGuid = pLand->m_pGuidance;
		if ( pGuid && !pGuid->m_bBattle )
		{
			// Ŭ���� ���� �߻�.
			std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
			if ( pCLUB )
			{
				pCLUB->AddProfitMoeny( llCommission );

				// Ŭ�� ������ 1�� �̻��� ��� �α׸� �����.
				if ( llCommission > EMCLUBINCOMEMONEY_LOG )
                {
					// ��� �� �α� ����
                    LogMoneyExchange(
                        gamelog::ID_CHAR, m_CharDbNum, 
                        gamelog::ID_CLUB, pCLUB->m_DbNum,
                        llCommission,
                        MONEY_ROUTE_CLUB_INCOME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION );
                }
			}
		}
	}
    pLand->AddSellCommissionMoney( llCommission );

	//	�ݾ� ��ȭ.
	CheckMoneyUpdate( GetInvenMoney(), llDisPRICE2, TRUE, "Sale To Npc." );

	AddInvenMoneyServer( llDisPRICE2, ADD_INVEN_MONEY_ITEM_SELL_TO_NPC );
    
	//	�� �α�.
	if ( llDisPRICE2 > EMMONEY_LOG )
	{   
		// ��� �� �α� ����
		LogMoneyExchange( 
			gamelog::ID_CHAR, 
			0,
			gamelog::ID_CHAR, 
			m_CharDbNum, 
			llDisPRICE2, 
			ITEM_ROUTE_CHAR, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), 
			LOG_MONEY_SELL_TO_NPC );

		// ��� �� �α� ����
		LogMoneyExchange( 
			gamelog::ID_CHAR, 
			m_CharDbNum, 
			gamelog::ID_CHAR, 
			0,
			GetInvenMoney(),
			MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), 
			LOG_MONEY_CHAR_INVEN_CURRENT );
	}

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount( m_szUID );

		CString strTemp;
		strTemp.Format(
			"Sale To Npc, [%s][%s], Sale price:[%I64d], Have Money:[%I64d]",
			m_szUID, m_szName,  llDisPRICE2, GetInvenMoney() );

		TracingMsg.SetLog( strTemp.GetString() );
		SendToAgent( &TracingMsg );
	}*/

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange(
        sITEM,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_SHOP, sITEM.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_SELL_TO_NPC,
		false );

    //	�ݾ� ��ȭ �޽���.
    SendInvenMoneyUpdate();

	//	������ ����.
    if ( bHold )
    {
		SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
		WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
		WORD wHoldPosY = sHold_BackUp.wPosY;

        ItemSaleToNpc( wHoldPosX, wHoldPosY, true, sHold_BackUp.CustomItem().bVietnamGainItem, true );
    }
    else
    {
        // �κ� ������ ����
        ItemSaleToNpc( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, true );
    }

    // ����� ����Ʈ�� �߰�.
    {
        volatile LONGLONG llRebuyPirce = llDisPRICE2;
        if ( llRebuyPirce < GLCONST_CHAR::wMinRebuyPrice )
            llRebuyPirce = GLCONST_CHAR::wMinRebuyPrice;

        volatile LONGLONG llFinalRebuyPirce = static_cast< LONGLONG >( llRebuyPirce * GLCONST_CHAR::fRebuyMultiplePrice );

        GLMSG::SNET_ADD_REBUY_ITEM_FA NetAddRebuyItem( sITEM, llFinalRebuyPirce );
        NetAddRebuyItem.dwChaNum = CharDbNum();
        SendToAgent( &NetAddRebuyItem );
    }

	// ��ī���� ��� �������� ������ �ȸ� ��DB ����
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && dwPetID != 0 )
	{
		// ���� ������� ���ش�.
		m_pGaeaServer->ReserveDropOutPet( SDROPOUTPETINFO( m_dwPetGUID, true, false ) );
        AddGameAdoJob(
            db::DbActionPtr(
                new db::PetDelete( m_CharDbNum, dwPetID ) ) );

		// ���� �����Ǹ� ��Ȱ������ Ŭ���̾�Ʈ�� �˷���.
		AddGameAdoJob(
            db::DbActionPtr(
                new db::PetListRestoreGet( m_CharDbNum, m_ClientSlot ) ) );
	}

	if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && dwVehicleID != 0 )
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new CDeleteVehicle( m_ClientSlot, m_CharDbNum, dwVehicleID ) ) );
	}

	return S_OK;
}


HRESULT GLChar::MsgReqFireCracker(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_FIRECRACKER* pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER *) nmg;

	GLMSG::SNETPC_REQ_FIRECRACKER_FB NetMsgFB;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

    SITEM* pITEM = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pITEM->sBasicOp.sNativeID ) )
        return E_FAIL;

	const SITEMCUSTOM& sCUSTOM = pHoldInvenItem->CustomItem();

	if ( pITEM->sBasicOp.emItemType != ITEM_FIRECRACKER )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( pITEM->sBasicOp.strTargetEffect.empty() )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ���� ���.
	//
	GLMSG::SNETPC_REQ_FIRECRACKER_BRD NetMsgBrd;
	NetMsgBrd.nidITEM = sCUSTOM.GetNativeID();
	NetMsgBrd.vPOS = pNetMsg->vPOS;
	SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsgBrd );

	SendToClient(&NetMsgBrd);

	// ���� ����� �˸�.
	//
	NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_OK;
	SendToClient( &NetMsgFB );

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	// ������ �Ҹ�.
	//
	ConsumeInvenItem( wHoldPosX, wHoldPosY );

	return S_OK;
}

HRESULT GLChar::MsgInvenVietnamGet( NET_MSG_GENERIC* nmg )
{
	// ������� �ʴ´�. ���� ó��
	return E_FAIL;
}

//HRESULT GLChar::MsgReqInvenDrug( NET_MSG_GENERIC* nmg )
HRESULT GLChar::MsgReqInvenDrug( WORD PosX, WORD PosY, bool bPetSkill )
{
	if ( !IsValidBody() )
        return E_FAIL;

	//GLMSG::SNETPC_REQ_INVENDRUG* pNetMsg = (GLMSG::SNETPC_REQ_INVENDRUG*) nmg;

	SINVENITEM* pInvenItem = m_cInventory.GetItem( PosX, PosY );
	if (!pInvenItem)
        return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sDrugOp.emDrug == ITEM_DRUG_NUNE )
        return E_FAIL;

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	const SMAPNODE* pMapNode = m_pGaeaServer->FindMapNode( GetCurrentMap() );

	if ( pMapNode == 0 || pMapNode->IsBlockDrugItem( pItem->BasicType() ) )
		return S_FALSE;

	bool bCheck = true;

	if ( pItem->sDrugOp.emDrug == ITEM_DRUG_EFFECT_RELEASE )
		bCheck = false;

	if ( IsActionLimit( EMACTION_LIMIT_ITEM ) && !bPetSkill && bCheck )
        return E_FAIL;
	if ( IsActionLimit( EMACTION_LIMIT_DRUG ) && !bPetSkill && bCheck )
        return E_FAIL;

    // ü�� ȸ�� �������� ����Ҽ� ���� ��Ȳ�̴�.
    switch ( pItem->sDrugOp.emDrug )
    {
    case ITEM_DRUG_HP:
    case ITEM_DRUG_HP_MP:
    case ITEM_DRUG_HP_MP_SP:
    case ITEM_DRUG_HP_CURE:
    case ITEM_DRUG_HP_MP_SP_CURE:
        {
            if ( IsActionLimit( EMACTION_LIMIT_DRUG_HP ) && !bPetSkill )
                return E_FAIL;
        }
        break;
    };

	if ( m_sCONFTING.IsFIGHTING() )
	{
		if ( !m_sCONFTING.IsRECOVE() )
		{
			// ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER != USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			// ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );
		}
	}

	// pk ����� ���θ� ��� �̻��� ��� ȸ������ ����� ���´�.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL > GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
	{
		return E_FAIL;
	}

	switch ( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_EFFECT_RELEASE :
		{
			if ( !ReleaseCureEffect( pItem->sDrugOp.dwERList ) ) return E_FAIL;

			GLMSG::SNET_DRUG_FB netMsg;

			netMsg.emCrow  = CROW_PC;
			netMsg.dwID    = GetGaeaID();
			netMsg.emFB    = EMREQ_DRUG_CONSUME_SUCCESS;
			netMsg.sItemID = pInvenItem->sItemCustom.GetNativeID();

			SendToClient( &netMsg );
			SendMsgViewAround( &netMsg );
		}
		break;
    case ITEM_DRUG_HP:
				{
			BOOL bUseAble = FALSE;

			// Note : ������ �ǰ� �ִ�ġ�� �ƴϸ� ��� ����.
			//
			if ( m_sHP.nMax != m_sHP.nNow )
			{
				m_sHP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
				m_sHP.INCREASE ( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
				bUseAble = TRUE;
			}

			if ( !bUseAble )
			{
				return E_FAIL;
			}
		}
		break;

	case ITEM_DRUG_MP:
		if ( m_sMP.nNow == m_sMP.nMax )
			return E_FAIL;
		m_sMP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sMP.INCREASE ( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_SP:
		if ( m_sSP.nNow == m_sSP.nMax )
			return E_FAIL;
		m_sSP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sSP.INCREASE ( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_CP:
		if ( m_sCP.wNow == m_sCP.wMax )
			return E_FAIL;

		m_sCP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nCP_ADD + m_sSUM_PASSIVE.m_nCP_ADD );
		break;

	case ITEM_DRUG_HP_MP:
		if ( ( m_sHP.nNow == m_sHP.nMax ) && ( m_sMP.nNow == m_sMP.nMax ) )
			return E_FAIL;
		m_sHP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sMP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sHP.INCREASE ( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
		m_sMP.INCREASE ( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_MP_SP:
		if ( ( m_sMP.nNow == m_sMP.nMax ) && ( m_sSP.nNow == m_sSP.nMax ) )
			return E_FAIL;
		m_sMP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sSP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sMP.INCREASE ( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		m_sSP.INCREASE ( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_HP_MP_SP:
		if ( ( m_sHP.nNow == m_sHP.nMax ) && ( m_sMP.nNow == m_sMP.nMax ) && ( m_sSP.nNow == m_sSP.nMax ) )
			return E_FAIL;
		m_sHP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sMP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sSP.INCREASE ( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sHP.INCREASE ( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
		m_sMP.INCREASE ( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		m_sSP.INCREASE ( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	//case ITEM_DRUG_HP:
	//case ITEM_DRUG_MP:
	//case ITEM_DRUG_SP:
	//case ITEM_DRUG_HP_MP:
	//case ITEM_DRUG_MP_SP:
	//case ITEM_DRUG_HP_MP_SP:
	//	RECEIVE_DRUGFACT(pItem->sDrugOp.emDrug,pItem->sDrugOp.wCureVolume,pItem->sDrugOp.bRatio);
	//	break;

	case ITEM_DRUG_CURE:
	case ITEM_DRUG_HP_CURE:
	case ITEM_DRUG_HP_MP_SP_CURE:
		{
			BOOL bSTATEBLOW = ISSTATEBLOW();

			if ( pItem->sDrugOp.emDrug == ITEM_DRUG_HP_CURE )
			{
				if ( ( m_sHP.nNow == m_sHP.nMax ) && !bSTATEBLOW )
					return E_FAIL;

				m_sHP.INCREASE( pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio, m_nHP_ADD );
				bSTATEBLOW = TRUE;
			}

			if ( pItem->sDrugOp.emDrug == ITEM_DRUG_HP_MP_SP_CURE )
			{
				if ( ( m_sHP.nNow == m_sHP.nMax ) && ( m_sMP.nNow == m_sMP.nMax ) && ( m_sSP.nNow == m_sSP.nMax ) && !bSTATEBLOW )
					return E_FAIL;

				m_sHP.INCREASE ( pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio, m_nHP_ADD );
				m_sMP.INCREASE ( pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio, m_nMP_ADD );
				m_sSP.INCREASE ( pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio, m_nSP_ADD );
				m_sHP.INCREASE ( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
				m_sMP.INCREASE ( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
				m_sSP.INCREASE ( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );;
				bSTATEBLOW = TRUE;
			}

			if ( !bSTATEBLOW ) return E_FAIL;

			GLMSG::SNETPC_CURESTATEBLOW_BRD	NetMsgBrd;
			for ( int i=0; i<EMBLOW_MULTI; ++i )
			{
				if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_NONE )
					continue;

				EMDISORDER emDIS = STATE_TO_DISORDER( m_sSTATEBLOWS[i].emBLOW );
				if ( !( pItem->sDrugOp.dwCureDISORDER & emDIS ) )
					continue;

				//	�����̻� ���ӽð��� �����Ŵ.
				m_sSTATEBLOWS[i].fAGE = 0.0f;
			}

			//	�����̻� ġ����� �ڽſ��� �˸�.
			NetMsgBrd.emCrow = CROW_PC;
			NetMsgBrd.dwID = m_dwGaeaID;
			NetMsgBrd.dwCUREFLAG = pItem->sDrugOp.dwCureDISORDER;
			SendToClient( &NetMsgBrd );

			//	�����̻� ġ����� ��ο��� �˸�.
			SendMsgViewAround( &NetMsgBrd );
		}
		break;
	case ITEM_DRUG_TENDENCY_RECOVERY :
		{
			int iBrightMax = GLCONST_CHAR::nBrightMAX + 1;

			if ( m_nBright < iBrightMax )
			{
				int iBright = m_nBright;

				iBright += pItem->sDrugOp.dwCureVolume;

				m_nBright = iBright; 

				GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
				NetMsg.nBright = m_nBright;
				SendToClient( &NetMsg );

				GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.nBright = m_nBright;
				SendMsgViewAround( &NetMsgBrd );
			}
		}
		break;
	case ITEM_DRUG_MACROTIME:
		{
			if ( m_fRemainMacroTime + pItem->sDrugOp.dwCureVolume > GLCONST_CHAR::fMacro_MaxFillTime )
			{
				GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC MacroMsg;
				MacroMsg.dwMsg = GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC::MACROSTATE_TIMEFILL_FAILD_MAXTIME;
				MacroMsg.dwData = pItem->sDrugOp.dwCureVolume;
				SendToClient( &MacroMsg );
				return FALSE;
			}
			m_fRemainMacroTime += pItem->sDrugOp.dwCureVolume;
			GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC MacroMsg;
			MacroMsg.dwMsg = GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC::MACROSTATE_TIMEFILL_NOTIFY;
			MacroMsg.dwData = pItem->sDrugOp.dwCureVolume;
			SendToClient( &MacroMsg );
		}
		break;
	};

	// ������ �Ҹ�.
	//
	ConsumeInvenItem( PosX, PosY );

	// ü�� ��ġ ��ȭ�� [�ڽ�,��Ƽ��,����]�� Ŭ���̾�Ʈ�鿡 �˸�.
	MsgSendUpdateState();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenLunchBox( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNETPC_REQ_INVEN_LUNCHBOX* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_LUNCHBOX* ) nmg;

	GLLandMan* const pLand( GetLandMan() );
	// Note : ���ö� ��� �Ұ��� ���� Ȯ��
	BOOL bLunchBoxForbid = pLand == NULL ? TRUE : pLand->IsLunchBoxForbid();
	if ( bLunchBoxForbid )
	{
		return S_FALSE;
	}

	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
	{
		return S_FALSE;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_LUNCHBOX )
	{
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB	NetMsgFB;
		NetMsgFB.emFB = EMLUNCHBOX_FB_FAILED;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	SNATIVEID sSKILL_ID    = pItem->sSkillBookOp.sSkill_ID;
	WORD      wSKILL_LEVEL = pItem->sSkillBookOp.wSkill_Level;
	DWORD     dwSELSLOT( 0 );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKILL_ID );

	// Note : ��ų�� �������� �ʴ´ٸ� ����
	//
	if ( !pSkill )
	{
		GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB	NetMsgFB;
		NetMsgFB.emFB = EMLUNCHBOX_FB_FAILED;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	// Note : �ߺ��� SkillFact�� �����Ѵ�
	//
	bool bExpLunch( false );
	if ( GLUseFeatures::GetInstance().IsUsingExtensionLunchSlot() == true )
		bExpLunch = pSkill->HaveSpec( EMSPECA_EXP_RATE ) || pSkill->HaveSpec( EMSPECA_RANDOM_EXP_RATE );

	const EMSKILLFACT_TYPE emSkillFactType( bExpLunch == true ? EMSKILLFACT_TYPE_EXP_LUNCHBOX : EMSKILLFACT_TYPE_LUNCHBOX );

	WORD wOverwriteSlot = 0;
	if ( IS_DUPLICATE_SKILLFACT( pSkill, emSkillFactType, wOverwriteSlot ) )
	{
		sc::writeLogInfo( sc::string::format(
			"[ Skill ] [ IS_DUPLICATE_SKILLFACT function is TRUE. ( Slot : %1%, ID : %2%/%3% ) ]",
			wOverwriteSlot,
			sSKILL_ID.Mid(),
			sSKILL_ID.Sid() ) );

		UPDATE_DATA_END( wOverwriteSlot );

		// ���ŵ��� �˸�
		GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB NetHoldResetFB;
		NetHoldResetFB.emCrow     = CROW_PC;
		NetHoldResetFB.dwID       = m_dwGaeaID;
		NetHoldResetFB.sSKILLID   = sSKILL_ID;
		NetHoldResetFB.wRESETSLOT = wOverwriteSlot;

		SendToClient( &NetHoldResetFB );

		// Note : ���ŵ��� �ֺ����� �˸�
		GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgBRD;
		NetMsgBRD.emCrow                 = CROW_PC;
		NetMsgBRD.dwID                   = m_dwGaeaID;
		NetMsgBRD.emResetType			 = EMSKILLHOLDRESET_REQ;
		NetMsgBRD.bRESET[wOverwriteSlot] = true;
		SendMsgViewAround( &NetMsgBRD );
	}

	// Note : �ִ� �������� ũ�ٸ� �ִ� ������ ����
	//
	if ( wSKILL_LEVEL >= pSkill->m_sBASIC.dwMAXLEVEL )
	{
		wSKILL_LEVEL = ( WORD )pSkill->m_sBASIC.dwMAXLEVEL;
	}

	// ��ų����.
	//
	if ( !RECEIVE_SKILLFACT( sSKILL_ID, wSKILL_LEVEL, dwSELSLOT, STARGETID( CROW_PC, m_dwGaeaID ), _SKILLFACT_DEFAULTTIME, _SKILLFACT_DEFAULTCUSTOMCOUNT, emSkillFactType ) )
	{
		GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB	NetMsgFB;
		NetMsgFB.emFB = EMLUNCHBOX_FB_FAILED;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	if ( dwSELSLOT < SKILLFACT_SIZE )
	{
		// �ߵ��� ��ų �� MSG.
		//
        GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
        NetMsgBRD.emCrow    = CROW_PC;
        NetMsgBRD.dwID      = m_dwGaeaID;
        NetMsgBRD.skill_id  = sSKILL_ID;
        NetMsgBRD.wLEVEL    = wSKILL_LEVEL;
        NetMsgBRD.wSELSLOT  = static_cast< WORD >( dwSELSLOT );
        NetMsgBRD.sID       = STARGETID( CROW_PC, m_dwGaeaID ); //��ų �����ڴ� �ڱ��ڽ�

		// ��ų���� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
		SendToClient( &NetMsgBRD );
		SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );
	}

	// �Ҹ� ������ ����.
	//
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

	// ���ö� ��� ����.
	//
	GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB	NetMsgFB;
	NetMsgFB.sSKILLID = sSKILL_ID;
	NetMsgFB.wLEVEL   = wSKILL_LEVEL;
	NetMsgFB.wSLOT    = ( WORD )dwSELSLOT;
	NetMsgFB.emFB     = EMLUNCHBOX_FB_OK;
	SendToClient( &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LUNCH ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	// Note : �����Ѵ�.
	//
	SaveSkillFact();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxInfo( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_GET_CHARGED_ITEM_CF* pPacket = ( GLMSG::NET_GET_CHARGED_ITEM_CF* ) nmg;
	
	// DB���� ���������� ���� ��������
	if ( m_pGaeaServer->GetDBMan() )
	{
        CString strUID( m_szUID );
		AddGameDbJob( 
            db::DbActionPtr( 
                new db::GetPurchaseItem( strUID, m_CharDbNum, pPacket->m_bFirstJoin ) ) );
	}
	return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxOpen( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_BOXOPEN* pNetMsg = ( GLMSG::SNET_INVEN_BOXOPEN* ) nmg;

	GLMSG::SNET_INVEN_BOXOPEN_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_BOX )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOBOX;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_EMPTY;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_ERR_ACTIVITY;
		SendToClient( &NetMsgFB );
        return E_FAIL;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION;
		SendToClient( &NetMsgFB );
        return E_FAIL;
    }

	// �κ��� ���� ���� ����.
	//
	// ���ʿ��� Assign ���� 
// 	GLInventory cInvenTemp;
// 	cInvenTemp.Assign( m_cInventory );

	DWORD BoxItemCount = 0;
	int nSize = min( ITEM::SBOX::ITEM_SIZE, ( int )pItem->sBox.vecItems.size() );
	for ( int i=0; i<nSize; ++i )
	{
		if ( SNATIVEID( false ) == pItem->sBox.vecItems[i].nidITEM )
			continue;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pItem->sBox.vecItems[i].nidITEM );
		if ( !pITEM )
		{
			NetMsgFB.emFB = EMREQ_BOXOPEN_FB_INVALIDITEM;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		BoxItemCount++;
// 
// 		SITEMCUSTOM sCUSTOM( pItem->sBox.vecItems[i].nidITEM );
// 		BOOL bOK = cInvenTemp.InsertItem( sCUSTOM );
// 		if ( !bOK )
// 		{
// 			// �κ��� ������ ���� ������ �Ǵܵ�.
// 			//
// 			NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOTINVEN;
// 			SendToClient( &NetMsgFB );
// 			return E_FAIL;
// 		}
	}
    
	if ( BoxItemCount > m_cInventory.GetAmountEmpty( false ) )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOTINVEN;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenBoxOpen", m_CharDbNum ) );
	}

	// ���ھȿ� �ִ� �������� ����� �ִ´�.
	int nBoxSize = min( ITEM::SBOX::ITEM_SIZE, ( int )pItem->sBox.vecItems.size() );
	for ( int i=0; i<nBoxSize; ++i )
	{
		if ( SNATIVEID( false ) == pItem->sBox.vecItems[i].nidITEM )
		{
			continue;
		}

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pItem->sBox.vecItems[i].nidITEM );
		if ( !pITEM )
			return E_FAIL;

		// ������ ����.
		//
		SITEMCUSTOM sITEM_NEW( pItem->sBox.vecItems[i].nidITEM );
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = static_cast< WORD >( pItem->sBox.vecItems[i].dwAMOUNT );
		sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		LogItemBasicStatOption( sITEM_NEW );
		LogItemLinkSkillOption( sITEM_NEW );
		LogItemAddonOption( sITEM_NEW );

		// ������ ����Ʈ���� �Է��� ���� ��� ����
		sITEM_NEW.cDAMAGE = ( BYTE )pITEM->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pITEM->sBasicOp.wGradeDefense;

		//	���� �ɼ� ����.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
            LogItemRandomOption( sITEM_NEW );

		// �κ��� ���� ��ġ ã��.
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
		if ( !bOk )
            return S_OK;	//	�κ� ���� �������� �ǵ��� ����.

		// #item
		//	������ ������ Inventory�� ����. pInsertItem NULL �̸� ���� crash
		// InvenInsertItem �������� �� ó�� �ʿ�
		InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, 1, true, false, true );
		SINVENITEM* pInsertItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

		// ��� �� �α� ����
		LogItemExchange(
            pInsertItem->sItemCustom,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_CHAR, pInsertItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_BOX,
			false );
	}

	//
	//mjeon.activity
	//
	ActivityCheckItemUse( pItem->sBasicOp.sNativeID.dwID );
	DoQuestItemUse( pItem->sBasicOp.sNativeID );

	//	���� ���� �α�.
	// ��� �� �α� ����
	LogItemExchange( 
        pInvenItem->sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_BOX,
		false );

	if ( pInvenItem->sItemCustom.wTurnNum >= 1 )
	{
		pInvenItem->sItemCustom.wTurnNum--;
	}

	if ( pInvenItem->sItemCustom.wTurnNum == 0 )
	{
		// ������ ����.
		InvenDeleteItem( wPosX, wPosY, false, FALSE, 1, true );

		//	[�ڽſ���] Inventory�� ������ ����.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
		NetMsg_Inven_Delete.dwChaNum = CharDbNum();
		SendToClient( &NetMsg_Inven_Delete );
	}
	else
	{
		//	[�ڽſ���] Inventory ������ �Ҹ��.
		GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
		NetMsg_Inven_Update.dwChaNum = CharDbNum();
		NetMsg_Inven_Update.wPosX = wPosX;
		NetMsg_Inven_Update.wPosY = wPosY;
		NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
		SendToClient( &NetMsg_Inven_Update );
	}

	// ���ڸ� ���������� ������
	NetMsgFB.emFB = EMREQ_BOXOPEN_FB_OK;
	SendToClient( &NetMsgFB );

	return S_OK;
}

/*
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.16, Randombox Open�� Ȯ������ ���� ���� ��û
 * modified : 
 */
HRESULT GLChar::MsgReqInvenRandomBoxOpen( DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_RANDOMBOXOPEN* pNetMsg = ( GLMSG::SNET_INVEN_RANDOMBOXOPEN* ) nmg;

	GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	//  [3/2/2015 gbgim];
	// ������ ��û�� ���, �̹� ó���� �޼��� �´�;
	if ( !pInvenItem )
	{
//		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
//		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_RANDOMITEM )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pItem->sRandomBox.VALID() )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_EMPTY;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

    // CacheServer�� Ȯ������ ������ ��� ���θ� Ȯ���Ѵ�.
    GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH NetMsgFH;
    NetMsgFH.dwGaeaID = dwGaeaID;
    NetMsgFH.dwChaNum = m_CharDbNum;
    NetMsgFH.wPosX = wPosX;
    NetMsgFH.wPosY = wPosY;
    NetMsgFH.sBoxID = pInvenItem->sItemCustom.GetNativeID();

    SendToCache( &NetMsgFH );

	return S_OK;
}

/*
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.16, Randombox Open�� Ȯ������ ���� ���� ��û ����
 * modified : sckim, 2016.09.20, Redmine #4544 Ȯ��/������ ����Ʈ ���� ��� ����(MsgReqInvenRandomBoxOpenFB �Լ� �Ķ���� ����)
 */
HRESULT GLChar::MsgReqInvenRandomBoxOpenFB( NET_MSG_GENERIC* nmg, float& ItemRate, SITEMCUSTOM& sItemBox, SITEMCUSTOM& sItemInBox )
{
	//if ( !IsValidBody() )
	//	return E_FAIL;
    BOOL bRandomboxChance = false;

	GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF* pNetMsg = ( GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF* ) nmg;

	GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB NetMsgFB;

	GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH NetMsgEND;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

    ITEM::SRANDOMITEM sGENITEM;
    sGENITEM.nidITEM = SNATIVEID( false );
    if ( pNetMsg->wRepFlag == GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF::REP_FLAG_WINNER )
    {
        sGENITEM.nidITEM = pNetMsg->sItemID;
        bRandomboxChance = true;
    }

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{   // �κ��丮�� �ش� ��ġ�� �������� ������ ����
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
		SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

		return E_FAIL;
	}
    if ( pInvenItem->sItemCustom.GetNativeID() != pNetMsg->sBoxID )
    {   // �κ��丮 �ش� ��ġ�� ItemID�� ��û BoxId�� �ƴϸ� ����
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
		SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

		return E_FAIL;
    }

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_RANDOMITEM )
    {
	    NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
	    SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

	    return E_FAIL;
    }

    if ( !pItem->sRandomBox.VALID() )
    {
	    NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_EMPTY;
	    SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

	    return E_FAIL;
    }
    
    float fNowRate = sc::Random::getInstance().GetFloatPercent();
    float fLOW = 0;

    for ( DWORD i=0; i<pItem->sRandomBox.vecBOX.size(); ++i )
    {
	    ITEM::SRANDOMITEM sITEM = pItem->sRandomBox.vecBOX[i];

        if ( sGENITEM.nidITEM == SNATIVEID( false ) )
        {
            if ( fLOW <= fNowRate && fNowRate < ( fLOW + sITEM.fRATE ) )
	        {
		        sGENITEM = sITEM;

                ItemRate = sITEM.fRATE;

		        break;
	        }
        }
        else if ( sITEM.nidITEM == sGENITEM.nidITEM )
        {
            ItemRate = sITEM.fRATE;

            break;
        }

        fLOW += sITEM.fRATE;
    }

 #ifndef _RELEASE
    if ( sGENITEM.nidITEM == SNATIVEID( false ) )
    {
        sc::writeLogInfo( "Select item in randombox error : not found" );
    }
    else
    {
        sc::writeLogInfo( sc::string::format( "Select item in randombox : [%1%],[%2%],[%3%]", sGENITEM.nidITEM.wMainID, sGENITEM.nidITEM.wSubID, ItemRate ) );
    }
 #endif

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenRandomBoxOpenFB", m_CharDbNum ) );
	}
    
	if ( sGENITEM.nidITEM == SNATIVEID( false ) )
	{
		//	���� ������ ���� �α� ����.
		// ��� �� �α� ����
		LogItemExchange( 
            pInvenItem->sItemCustom, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_RANDOMBOX,
			false );

		// ���� ������ ����
        InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

		// ó�� ��� ����.
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_MISS;
		SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

		return E_FAIL;
	}

	const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem( sGENITEM.nidITEM );
	if ( !pITEM_DATA )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_BADITEM;
		SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

		return E_FAIL;
	}

	// ������ �߻�.
	//
	WORD wINSERTX( 0 ), wINSERTY( 0 );
	BOOL bOK = m_cInventory.FindInsertable( wINSERTX, wINSERTY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_NOINVEN;
		SendToClient( &NetMsgFB );

        if ( bRandomboxChance )
        {
            NetMsgEND.bComplete = false;
            NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
            NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
            SendToCache( &NetMsgEND );
        }

		return E_FAIL;
	}

	// ������ ����.
	//
	SITEMCUSTOM sITEM_NEW( sGENITEM.nidITEM );

	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = pITEM_DATA->GETAPPLYNUM();
	sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	LogItemBasicStatOption( sITEM_NEW );
	LogItemLinkSkillOption( sITEM_NEW );
	LogItemAddonOption( sITEM_NEW );

	// ������ ����Ʈ���� �Է��� ���� ��� ����
	sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

	//	���� �ɼ� ����.
	if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
		LogItemRandomOption( sITEM_NEW );

	//	������ ������ Inventory�� ����.
	InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	SINVENITEM *pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem )
		return E_FAIL;

	//	[�ڽſ���] �޽��� �߻�
    InvenInsertSend( pInsertItem, FALSE, false, true );

	//	���� ������ ���� �α� ����.
	// ��� �� �α� ����
	LogItemExchange( 
        pInvenItem->sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_RANDOMBOX,
		false );

    sItemBox = pInvenItem->sItemCustom;

	//	���� ������ ���� �α� ����.
	// ��� �� �α� ����
	LogItemExchange( 
        pInsertItem->sItemCustom, 
        gamelog::ID_CHAR, 0, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_RANDOMBOX,
		false );

    sItemInBox = pInsertItem->sItemCustom;

	// �ؿ� ������ �����ϴ� ���� ConsumeInvenItem ���� ��ü�� �� �ִ�. �Ҹ� �������� �ƴ϶��?
	
	//
	//mjeon.activity
	//
	ActivityCheckItemUse( pItem->sBasicOp.sNativeID.dwID );
	DoQuestItemUse( pItem->sBasicOp.sNativeID );

	if ( pInvenItem->sItemCustom.wTurnNum >= 1 )
	{
		pInvenItem->sItemCustom.wTurnNum--;
	}

	if ( pInvenItem->sItemCustom.wTurnNum == 0 )
	{
		// ������ ����.
		InvenDeleteItem( wPosX, wPosY, false, FALSE, 1, true );

		//	[�ڽſ���] Inventory�� ������ ����.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
		NetMsg_Inven_Delete.dwChaNum = CharDbNum();

		SendToClient( &NetMsg_Inven_Delete );
	}
	else
	{
		//	[�ڽſ���] Inventory ������ �Ҹ��.
		GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
		NetMsg_Inven_Update.dwChaNum = CharDbNum();
		NetMsg_Inven_Update.wPosX = wPosX;
		NetMsg_Inven_Update.wPosY = wPosY;
		NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;

		SendToClient( &NetMsg_Inven_Update );
	}

	// ���ڸ� ���������� ������
	NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_OK;
	SendToClient( &NetMsgFB );

    if ( bRandomboxChance )
    {
        NetMsgEND.bComplete = true;
        NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
        NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
        SendToCache( &NetMsgEND );

        // ���⼭ DB�� Randombox Ȯ������ �Ϸ�α׸� DB�� ����
        AddLogAdoJob( db::DbActionPtr(new db::LogRandomboxChanceComplete( pNetMsg->sBoxID,         /* RandomBox ID */
                                                                          pNetMsg->sItemID,        /* ��÷ Item ID */
                                                                          pNetMsg->nReqCount,      /* ��ǥ ���� ���� */
                                                                          pNetMsg->dwTargetChaNum, /* ��ǥ ĳ���� ��ȣ */
                                                                          pNetMsg->dwGMNum,        /* ��� GM Number */
                                                                          pNetMsg->nOpenCount,     /* ���� ���� ���� */
                                                                          pNetMsg->dwChaNum        /* ��÷�� */
                                                                         ) ) );
    }


 #ifndef _RELEASE
        sc::writeLogInfo("MsgReqInvenRandomBoxOpenFB Success!!");
 #endif

	return S_OK;
}

HRESULT GLChar::MsgReqInvenSelformBoxSelectOpen( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{	
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenSelformBoxSelectOpen", m_CharDbNum ) );
	}

	GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN* pNetMsg = ( GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN* )nmg;
	GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN_FB NetMsgFB;

	const WORD wSelPos( pNetMsg->nSelPos )
		, wBoxPosX( pNetMsg->nBoxPosX ), wBoxPosY( pNetMsg->nBoxPosY );

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wBoxPosX, wBoxPosY );
	if ( !pInvenItem )
		return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_SELECTIVEFORM_BOX )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( wSelPos >= ITEM::SBOX::ITEM_SIZE )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	const ITEM::SBOX_ITEM& sBoxItem = pItem->sBox.vecItems[wSelPos];
	const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem( sBoxItem.nidITEM );
	if ( pITEM_DATA == NULL )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// �κ� �������� Ȯ��;
	WORD wINSERTX( 0 ), wINSERTY( 0 );
	BOOL bOK = m_cInventory.FindInsertable( wINSERTX, wINSERTY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_INVENNOSPACE;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ���þ����� �����߰�;
	SITEMCUSTOM sITEM_NEW( sBoxItem.nidITEM );
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();
	sITEM_NEW.wTurnNum = sBoxItem.dwAMOUNT;
	// Ÿ�� ��;
	sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	LogItemBasicStatOption( sITEM_NEW );
	LogItemLinkSkillOption( sITEM_NEW );
	LogItemAddonOption( sITEM_NEW );

	// ������ ����Ʈ���� �Է��� ���� ��� ����
	sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

	//	���� �ɼ� ����.
	if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
		LogItemRandomOption( sITEM_NEW );

	//	������ ������ Inventory�� ����.
	InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	SINVENITEM *pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem )
		return E_FAIL;

	//	[�ڽſ���] �޽��� �߻�
	InvenInsertSend( pInsertItem, FALSE, false, true );

	//	���� ������ ���� �α� ����.
	// ��� �� �α� ����
	LogItemExchange( 
		pInvenItem->sItemCustom, 
		gamelog::ID_CHAR, m_CharDbNum, 
		gamelog::ID_CHAR, 0, 
		ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_SELECTBOX,
		false );

	ActivityCheckItemUse( pItem->sBasicOp.sNativeID.dwID );
	DoQuestItemUse( pItem->sBasicOp.sNativeID );

	// ���� ������ ����
	InvenDeleteItem( wBoxPosX, wBoxPosY, true, FALSE, 1, true );

	// ���ڸ� ���������� ������
	NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_OK;
	//NetMsgFB.nidITEM = sBoxItem.nidITEM;
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenDisJunction( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_DISJUNCTION* pNetMsg = ( GLMSG::SNET_INVEN_DISJUNCTION* ) nmg;

	GLMSG::SNET_INVEN_DISJUNCTION_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) )
		return E_FAIL;

	if ( pHold->sBasicOp.emItemType != ITEM_DISJUNCTION )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pInvenItem->sItemCustom.nidDISGUISE == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_NONEED;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SITEMCUSTOM sITEM_NEW( pInvenItem->sItemCustom.nidDISGUISE );
	if ( sITEM_NEW.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_BADITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	const SITEM* pONE = GLogicData::GetInstance().GetItem( sITEM_NEW.GetNativeID() );
	if ( !pONE )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_BADITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	CTime cTIME = CTime::GetCurrentTime();

	if ( pInvenItem->sItemCustom.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
	{
		cTIME = CTime( pInvenItem->sItemCustom.tDISGUISE );

		CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
		cTIME -= tLMT;
	}

	// �ڽ�Ƭ �Ⱓ ���� �� ����;
	sITEM_NEW.SetPeriodExBasic(pInvenItem->sItemCustom.GetPeriodExDisguise());
	sITEM_NEW.cPeriodExtensionCount = pInvenItem->sItemCustom.cCostumePeriodExtensionCount;
	sITEM_NEW.ResetPeriodExDisguise();

	// �ڽ�Ƭ ������ �ű�. by luxes.
	//
	sITEM_NEW.dwMainColor = pInvenItem->sItemCustom.dwMainColor;
	sITEM_NEW.dwSubColor = pInvenItem->sItemCustom.dwSubColor;

	// �ڽ�Ƭ �ɷ�ġ �ű�
	sITEM_NEW.costumeUser = pInvenItem->sItemCustom.costumeUser;

	// ������ ����.
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = 1;
	sITEM_NEW.cGenType = EMGEN_BILLING;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	// hold ������ �κ��� �ǵ���.
	WORD wInsertPosX, wInsertPosY;
	BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_NOINVEN;
		SendToClient( &NetMsgFB );
		return E_FAIL;	//	�κ� ���� �������� �ǵ��� ����.
	}

	//	����ִ� ������ Inventory�� ����.
	InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, 1, true );
	SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

	// ������ ��Ź�� ��.
	//
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );

	// �ڽ�Ƭ ������ ����. by luxes.
	//
	pInvenItem->sItemCustom.dwMainColor = 0;
	pInvenItem->sItemCustom.dwSubColor = 0;

	// �ڽ�Ƭ �ɷ�ġ ����
	pInvenItem->sItemCustom.ResetInvest();

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenDisJunction", m_CharDbNum ) );
	}

	// �и��� �ڽ��� �α�.
	// ��� �� �α� ����
	LogItemExchange( 
        pINSERT_ITEM->sItemCustom, 
        gamelog::ID_CHAR, 0, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_DISJUNCTION,
		false );

	// �Ϲ� ���� �������� ��Ź�ϴ� ���� �α� ����
	LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_DISJUNCTION );

    //SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    //WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    //WORD wHoldPosY = sHold_BackUp.wPosY;

	// ������ �Ҹ�.
	//
    //ConsumeInvenItem( wHoldPosX, wHoldPosY );
	ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

	NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_OK;
	SendToClient( &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_DISGUISE_SPLIT ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}


// *****************************************************
// Desc: �ڽ�Ƭ ����
// *****************************************************
HRESULT GLChar::MsgReqInvenDisguise( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_DISGUISE* pNetMsg = ( GLMSG::SNET_INVEN_DISGUISE* ) nmg;

	GLMSG::SNET_INVEN_DISGUISE_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// ��ȿ�� �κ��丮 ���� �˻�
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// ��ȿ�� ���������� �˻�
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

	// �տ� ��� �ִ� �������� ��ȿ���� �˻�
	const SITEMCUSTOM& sITEM_HOLD = pHoldInvenItem->CustomItem();
	if ( sITEM_HOLD.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �տ��� �������� ���� ���� ����
    SITEM* pHold = GLogicData::GetInstance().GetItem( sITEM_HOLD.Id() );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �տ��� �������� �ڽ�Ƭ���� �˻�
	if ( !( sITEM_HOLD.IsDisguise() != NULL ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NODISGUISE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	if ( !CHECK_ACTIVITY_POINT( *pHold ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ERR_ACTIVITY;
		SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pHold ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ERR_CONTRIBUTION;
		SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	if ( pInvenItem->sItemCustom.IsDisguise() != NULL )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ���� Ŭ���� ���� �˻�
	if ( ( pHold->sBasicOp.dwReqCharClass & pItem->sBasicOp.dwReqCharClass ) == NULL )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ��������� �˻�
	if ( pHold->sBasicOp.emItemType != ITEM_SUIT )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOTSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_SUIT )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOTSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ���� ������ �������� �˻�
	if ( pHold->sSuitOp.emSuit != pItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �κ��丮�� ������ �ڽ�Ƭ�� ������ �ִ��� �˻�
	if ( pInvenItem->sItemCustom.nidDISGUISE != SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ��ħ �������� ��� �ڽ�Ƭ ��� �Ұ�
	if ( pItem->ISPILE() == true || pInvenItem->sItemCustom.wTurnNum > 1 )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_PILED;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ���� �ڽ����� ���.
	if ( pHold->sDrugOp.tTIME_LMT != 0 )
	{
		__time64_t &tTIME = pInvenItem->sItemCustom.tDISGUISE;
		CTime tCurTime = sITEM_HOLD.tBORNTIME;
		CTime tSeedTime( tTIME );
		CTimeSpan tLMT( pHold->sDrugOp.tTIME_LMT );

		// �ڽ��� 5������ ���� ( CRM #1601 );
		// http://crm.mincoms.com/Ticket/TicketView/1601

        // �ڽ�Ƭ��񿡺����� ������ 5������ ������û
        // http://crm.mincoms.com/Ticket/TicketView/1480
        // �ڽ�Ƭ�� ������ ��� �и��뼼�� ��� ��, ��ȿ�� ���� ���� Ȯ�� ��û CRM #1480
		//tLMT += CTimeSpan(0,0,05,0);

		// �ð� ����.
		tSeedTime = tCurTime + tLMT;

		tTIME = tSeedTime.GetTime();
	}
		
	// �ڽ��� ��Ų ����.
	//
	pInvenItem->sItemCustom.nidDISGUISE = pHold->sBasicOp.sNativeID;

	// �ڽ�Ƭ ������ ����. by luxes.
	//
	pInvenItem->sItemCustom.dwMainColor = sITEM_HOLD.dwMainColor;
	pInvenItem->sItemCustom.dwSubColor = sITEM_HOLD.dwSubColor;

	// �ڽ�Ƭ �ɷ�ġ �ű�.
	pInvenItem->sItemCustom.costumeUser = pHoldInvenItem->sItemCustom.costumeUser;
	pInvenItem->sItemCustom.cCostumePeriodExtensionCount = pHoldInvenItem->sItemCustom.cPeriodExtensionCount;
	pInvenItem->sItemCustom.SetPeriodExDisguise(pHoldInvenItem->sItemCustom.GetPeriodExBasic());

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	// �Ϲ� ���� �������� �ڽ��� ��Ų���� ��ȯ�Ǵ� �� �α� ����.
	LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COUSUME_ADD_BY_ITEM );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenDisguise", m_CharDbNum ) );
	}

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        sITEM_HOLD, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_DELETE, sITEM_HOLD.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_COSTUME_ON_ITEM,
		false );

	// �κ� Ȧ�� ������ ����.
	//
	InvenDeleteItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, true, pHoldInvenItem->CustomItem().bVietnamGainItem, INSERT_INVEN_ITEM_IVEN_DISGUISE, true );

	// �߶��� ������ ����?

	// �ڽ��� ��Ų���� ��ȯ ���� �˸�.
	//
	NetMsgFB.emFB = EMREQ_DISGUISE_FB_OK;
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenCleanser( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_CLEANSER* pNetMsg = ( GLMSG::SNET_INVEN_CLEANSER* ) nmg;

	GLMSG::SNET_INVEN_CLEANSER_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}	

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) )
		return E_FAIL;

	if ( pInvenItem->sItemCustom.nidDISGUISE == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NONEED;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pHold->BasicType() != ITEM_CLEANSER )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NONEED;
		SendToClient( &NetMsgFB );
		return S_FALSE;	
	}

	// ���� ��Ź�� ��
    SNATIVEID CostumeId = pInvenItem->sItemCustom.nidDISGUISE;
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );
	pInvenItem->sItemCustom.ResetPeriodExDisguise();

	// �ڽ�Ƭ ������ ����. by luxes.
	pInvenItem->sItemCustom.dwMainColor = 0;
	pInvenItem->sItemCustom.dwSubColor = 0;

	// �ڽ�Ƭ �ɷ�ġ ����
	//pInvenItem->sItemCustom.ResetInvest();

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );
	
    // �α�
    // 1. ������ �ڽ�Ƭ ������
    // 2. ������ ��Ź��
    // 3. �ڽ�Ƭ�� ������ ���� ������
    SITEMCUSTOM DeletedCostume( CostumeId );
    //SITEMCUSTOM DeletedCleanser( pHold->BasicId() );	// guid �� ���� ������ �α׿� ���Ұ�. pHoldInvenItem ���� �α� �����.
    
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenCleanser", m_CharDbNum ) );
	}

	// ��� �� �α� ����
    LogItemExchange( 
        pHoldInvenItem->CustomItem(), 
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, 0, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_CLEANSER_BY_USE,
		true );

	// ��� �� �α� ����, �����ۿ� �پ� �ִ� �ڽ�Ƭ�ε� �и��� ���� �ʰ� �ٷ� ������ �Ǿ��� ������ guid �� ����. 0000-0000-0000-0000 �̷��� ������ guid �� �α׿� �����Ŵ�.
    LogItemExchange(
        DeletedCostume,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, 0, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_COSTUME_BY_CLEANSER,
		true );
	
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_CLEANSER );

	// ������ �Ҹ�
	ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

	// �ڽ��� ��Ų ���� ��ȯ ���� �˸�
	NetMsgFB.emFB = EMREQ_CLEANSER_FB_OK;
	SendToClient( &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_DISGUISE_REMOVE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqMoneyToField(NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqMoneyToField", m_CharDbNum ) );
	}

	GLMSG::SNETPC_REQ_MONEY_TO_FIELD* pPacket = (GLMSG::SNETPC_REQ_MONEY_TO_FIELD*) nmg;
    if (pPacket->m_MagicNum != GLMSG::SNETPC_REQ_MONEY_TO_FIELD::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgReqMoneyToField Money %1% CharDbNum %2% MagicNum %3%",
                pPacket->lnMoney,
                m_CharDbNum,
                pPacket->m_MagicNum));
        return E_FAIL;
    }

	if (!GLCONST_CHAR::bMONEY_DROP2FIELD)
        return S_FALSE;

	if (pPacket->lnMoney < 0)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgReqMoneyToField Money %1% CharDbNum %2%",
                pPacket->lnMoney,
                m_CharDbNum));
        return E_FAIL;
    }

	if (GetInvenMoney() < pPacket->lnMoney)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgReqMoneyToField Money %1% < %2% CharDbNum %2%",
                GetInvenMoney(),
                pPacket->lnMoney,
                m_CharDbNum));
        return E_FAIL;
    }

	D3DXVECTOR3 vCollisionPos;
	BOOL bCollision = pLand->IsCollisionNavi
	(
		pPacket->vPos + D3DXVECTOR3(0,+5,0),
		pPacket->vPos + D3DXVECTOR3(0,-5,0),
		vCollisionPos
	);

	if ( !bCollision )
		return S_FALSE;

	// ������ �ִ� ���� �׼��� ����.
	CheckMoneyUpdate( GetInvenMoney(), pPacket->lnMoney, FALSE, "Drop Money." );

	SubtractInvenMoneyServer( pPacket->lnMoney, SUBTRACT_INVEN_MONEY_DROP_GROUND );

	// �ݾ� �α�.
	if (pPacket->lnMoney > EMMONEY_LOG)
	{
		// ��� �� �α� ����
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -pPacket->lnMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_DROP_INVNE_TO_FIELD);
		// ��� �� �α� ����
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(),         MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
	}

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);

		CString strTemp;
		strTemp.Format(
			"DropMoney!!, [%s][%s], Drop Amount:[%I64d], Have Money:[%I64d]",
			m_szUID, m_szName,  pPacket->lnMoney, GetInvenMoney() );

		TracingMsg.SetLog(strTemp.GetString());
		SendToAgent(&TracingMsg );
	}*/

	// �ٴڿ� ����Ʈ����.
	pLand->DropMoney(vCollisionPos, pPacket->lnMoney, FALSE);

	// Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	SendInvenMoneyUpdate();
	
	return S_OK;
}

// ��� ���� ���� �õ�;
// - sBox - ���� ����Ʈ;
// - sRendomBox - ��� ���ڿ��� ���� ������;
HRESULT GLChar::MsgReqInvenLockBoxUnLock( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_LOCKBOX_UNLOCK* pNetMsg = ( GLMSG::SNET_INVEN_LOCKBOX_UNLOCK* )nmg;
	GLMSG::SNET_INVEN_LOCKBOX_FB MsgFB;

	SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
	if ( !pHoldInvenItem )
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->sItemCustom.GetNativeID() );
	if ( !pHold || pHold->sBasicOp.emItemType != ITEM_LOCK_BOX_KEY )
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( !pHold->ISINSTANCE() )
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_KEYINSTANCE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const WORD wPosX = pNetMsg->wPosX, wPosY = pNetMsg->wPosY;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_LOCK_BOX )
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( !pInvenItem->sItemCustom.bVietnamGainItem ) // ����� Ǯ���ִٸ�;
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_ALREADY;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// ���� ����Ʈ�� ���谡 �ִ��� ����;
	int nFindIdx, nNeedAmount;
	if ( pItem->sBox.FIND( pHold->BasicId(), nFindIdx, nNeedAmount ) )
	{
		SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
		if ( !pHoldInvenItem )
		{
			MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_INCORRECT;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		if ( pHoldInvenItem->sItemCustom.wTurnNum < nNeedAmount )		// ���� ������ ���ڸ��ٸ�;
		{
			MsgFB.emFB = EMINVEN_LOCKBOX_FB_SHORT_KEY;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		// �Ҹ�� ������ ó��;
		ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, false, nNeedAmount );
		pInvenItem->sItemCustom.bVietnamGainItem = false;

		// ������ ���°� �ٲ�������� ������ ������ �����ؾ��Ѵ�;
		GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
		NetItemUpdate.wPosX = wPosX;
		NetItemUpdate.wPosY = wPosY;
		NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &NetItemUpdate );

		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_OK;
		SendToClient( &MsgFB );
		return S_OK;
	}
	
	MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_INCORRECT;
	SendToClient( &MsgFB );
	return E_FAIL;
}

HRESULT GLChar::MsgReqInvenLockBoxOpen( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_LOCKBOX_OPEN* pNetMsg = ( GLMSG::SNET_INVEN_LOCKBOX_OPEN* )nmg;
	GLMSG::SNET_INVEN_LOCKBOX_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_LOCK_BOX )
	{
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pItem->sRandomBox.VALID() )
	{
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// bVietnamGainItem == false ���� �����ִ°�;
	const SITEMCUSTOM& sCustomItem = pInvenItem->sItemCustom;
	if ( sCustomItem.bVietnamGainItem == true )
	{
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_LOCK;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// �� ���� �Ʒ��� �����ڽ��� ���� �����ϴ�;
	{
		if ( !pItem->sRandomBox.VALID() )
		{
			NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		float fNowRate = sc::Random::getInstance().GetFloatPercent();
		float fLOW = 0;
		ITEM::SRANDOMITEM sGENITEM;
		sGENITEM.nidITEM = SNATIVEID( false );
		for ( DWORD i=0; i<pItem->sRandomBox.vecBOX.size(); ++i )
		{
			ITEM::SRANDOMITEM sITEM = pItem->sRandomBox.vecBOX[i];
			
			if ( fLOW <= fNowRate && fNowRate < ( fLOW + sITEM.fRATE ) )
			{
				sGENITEM = sITEM;
				break;
			}
			fLOW += sITEM.fRATE;
		}

		GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
		if ( !pTarChar )
		{		
			// ��� �� �α� ����
			// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenLockBoxOpen", m_CharDbNum ) );
		}

		if ( sGENITEM.nidITEM == SNATIVEID( false ) )
		{
			//	���� ������ ���� �α� ����.
			// ��� �� �α� ����
			LogItemExchange( 
				pInvenItem->sItemCustom, 
				gamelog::ID_CHAR, m_CharDbNum, 
				gamelog::ID_CHAR, 0, 
				ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_LOCKED_BOX_FAIL,
				false );

			// ���� ������ ����
			InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

			// ó�� ��� ����.
			NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem( sGENITEM.nidITEM );
		if ( !pITEM_DATA )
		{
			NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// ������ �߻�.
		WORD wINSERTX( 0 ), wINSERTY( 0 );
		BOOL bOK = m_cInventory.FindInsertable( wINSERTX, wINSERTY );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// ������ ����.
		SITEMCUSTOM sITEM_NEW( sGENITEM.nidITEM );

		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = pITEM_DATA->GETAPPLYNUM();
		sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		LogItemBasicStatOption( sITEM_NEW );
		LogItemLinkSkillOption( sITEM_NEW );
		LogItemAddonOption( sITEM_NEW );

		// ������ ����Ʈ���� �Է��� ���� ��� ����
		sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

		//	���� �ɼ� ����.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );

		//	������ ������ Inventory�� ����.
		InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
		SINVENITEM* pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

		if ( !pInsertItem )
			return E_FAIL;

		//	[�ڽſ���] �޽��� �߻�
		InvenInsertSend( pInsertItem, FALSE, false, true );

		//	���� ������ ���� �α� ����.
		// ��� �� �α� ����
		LogItemExchange( 
			pInvenItem->sItemCustom, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, 0, 
			ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_LOCKED_BOX,
			false );

		//	���� ������ ���� �α� ����.
		// ��� �� �α� ����
		LogItemExchange( 
			pInsertItem->sItemCustom, 
			gamelog::ID_CHAR, 0, 
			gamelog::ID_CHAR, m_CharDbNum, 
			ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_LOCKED_BOX,
			false );

		//
		//mjeon.activity
		//
		ActivityCheckItemUse( pItem->sBasicOp.sNativeID.dwID );
		DoQuestItemUse( pItem->sBasicOp.sNativeID );

		// ���� ������ ����
		InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

		// ���ڸ� ���������� ������
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_OK;
		NetMsgFB.nidITEM = sGENITEM.nidITEM;
		SendToClient( &NetMsgFB );
	}

	return S_OK;
}

// *****************************************************
// Desc: ������ ��þƮ ��û ó��
// *****************************************************
HRESULT GLChar::MsgReqInvenTLGrinding( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );

	if ( pLand == NULL ) return E_FAIL;

	GLMSG::SNET_INVEN_TL_GRINDING* pNetMsg = ( GLMSG::SNET_INVEN_TL_GRINDING* )nmg;

	//////// HOLD ITEM ///////////////////////////////////////////////////////////////////////////
	WORD wHoldPosX = pNetMsg->wHoldPosX;
	WORD wHoldPosY = pNetMsg->wHoldPosY;

	SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( wHoldPosX, wHoldPosY );

	if ( !pHoldInvenItem ) return S_FALSE;

	SITEMCUSTOM& sHOLDITEM = pHoldInvenItem->sItemCustom;
	const SITEM* pHold = GLogicData::GetInstance().GetItem( sHOLDITEM.GetNativeID() );

	if ( !pHold ) return E_FAIL;

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////

	//////// TARGET ITEM /////////////////////////////////////////////////////////////////////////
	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );

	if ( !pInvenItem ) return E_FAIL;	
	//////////////////////////////////////////////////////////////////////////////////////////////

	EMGRINDINGCONDITION eResult = GRINDING_TL_CONDITION_CHECK( pInvenItem->sItemCustom, pHold, sHOLDITEM, CTime::GetCurrentTime() );

	bool bReEnchant = false;

	if ( pNetMsg->eResult == EMREQ_TEMP_ENCHANT_EXIST && eResult == EMGRINDINGCONDITION_RE_ENCHANT )
	{
		eResult = EMGRINDINGCONDITION_SUCCESS;

		bReEnchant = true;
	}

	if ( eResult == EMGRINDINGCONDITION_SUCCESS )
	{
		BYTE ResultGrade = 0; // ���������ġ

		DOTLGRINDING( pInvenItem->sItemCustom, pHold, ResultGrade, bReEnchant );

		if ( ResultGrade > 0 )
		{
			// �ٷ� ��� �����Ѵ�.
			InvenUpdateItem( pInvenItem );

			ConsumeInvenItem( wHoldPosX, wHoldPosY, false, 1 );

			// �α� ���.
			LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, LOG_ITEM_TEMP_ENCHANT );

			GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
			NetItemUpdate.wPosX = pNetMsg->wPosX;
			NetItemUpdate.wPosY = pNetMsg->wPosY;
			NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
			SendToClient( &NetItemUpdate );
		}
	}
	
	GLMSG::SNET_INVEN_TL_GRINDING ClientMsg;

	switch( eResult )
	{
	case EMGRINDINGCONDITION_SUCCESS    : ClientMsg.eResult = EMREQ_TEMP_ENCHANT_SUCCESS;	break;
	case EMGRINDINGCONDITION_MAX        :
	case EMGRINDINGCONDITION_FAILED     : ClientMsg.eResult = EMREQ_TEMP_ENCHANT_FAIL;      break;
	case EMGRINDINGCONDITION_TIME_LIMIT :
	case EMGRINDINGCONDITION_NOITEM     : ClientMsg.eResult = EMREQ_TEMP_ENCHANT_NOT;       break;
	case EMGRINDINGCONDITION_DEFCLASS   : ClientMsg.eResult = EMREQ_TEMP_ENCHANT_NOT_ITEM;  break;
	case EMGRINDINGCONDITION_MIN        : ClientMsg.eResult = EMREQ_TEMP_ENCHANT_GRADE_NOT; break;
	case EMGRINDINGCONDITION_RE_ENCHANT : 
		{
			ClientMsg =* pNetMsg;
			ClientMsg.eResult = EMREQ_TEMP_ENCHANT_EXIST;
		}
		break;
	}

	SendToClient(&ClientMsg);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenGrinding( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNET_INVEN_GRINDING* pNetMsg = ( GLMSG::SNET_INVEN_GRINDING* )nmg;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
		return S_FALSE;

	SITEMCUSTOM& sHOLDITEM = pHoldInvenItem->sItemCustom;
	const SITEM* pHold = GLogicData::GetInstance().GetItem( sHOLDITEM.GetNativeID() );
	if ( !pHold )
		return E_FAIL;

	if ( pHold->sBasicOp.emItemType != ITEM_GRINDING )
		return S_FALSE;

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) )
		return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( GRINDING_CONDITION_CHECK( pInvenItem->sItemCustom, pHold, sHOLDITEM ) != EMGRINDINGCONDITION_SUCCESS )
		return S_FALSE;

	BYTE cGrade = 0;
	cGrade = pInvenItem->sItemCustom.GETGRADE( pHold->sGrindingOp.emTYPE );

	// ������ �ʿ� ����
	WORD wGradeNum = GRINDING_CONSUME_NUM( pInvenItem->sItemCustom, pHold, sHOLDITEM );	
	
	if ( wGradeNum == 0 )
		return E_FAIL;

	if ( wGradeNum > sHOLDITEM.wTurnNum )
		return E_FAIL;

	// ���� ����.
	// Note : ��ȭ �ɼ� ������ �˻�
	DWORD dwGrindingOption( EMGRINDINGOPTION_NONE );
    DWORD dwSlotOption[SLOT_NSIZE_S_2] = { 0 };

	DWORD dwGrindingLogOption = 0;

	SITEM* pITEM = NULL;
	{
		// Note : ����ν� ��� ��� �˻��Ѵ�.
		//
		for ( DWORD i = 0; i < SLOT_NSIZE_S_2; ++i )
		{
			pITEM = GET_SLOT_ITEMDATA( EMSLOT( i ) );
			if ( pITEM )
			{
				if ( !IsCoolTime( pITEM->sBasicOp.sNativeID ) )
				{
					DWORD dwOption( EMGRINDINGOPTION_NONE );

					switch ( pITEM->sBasicOp.emItemType )
					{
						// Note : �ҹ���
					case ITEM_ANTI_DISAPPEAR:    
						dwOption |= EMGRINDINGOPTION_ANTIDISAPPEAR;    
						dwGrindingLogOption = 1;
						break;
						// Note : ��ȭ��
					case ITEM_INC_GRINDINGRATE:  
						dwOption |= EMGRINDINGOPTION_INC_GRINDINGRATE; 
						dwGrindingLogOption = 2;
						break;
						// Note : ��ȭ��
					case ITEM_ANTI_RESET:        
						dwOption |= EMGRINDINGOPTION_ANTIRESET;        
						dwGrindingLogOption = 3;
						break;
					}

					dwSlotOption[i] = dwOption;
					dwGrindingOption |= dwOption;
				}
			}
		}
	}

	// Note : ��ȭ�ֿ� ��ȭ�ִ� 4�̸� 9�̻� �϶��� ��� �Ѵ�.
	if ( cGrade < GRADE_NORMAL || GRADE_HIGH <= cGrade )
	{
		if ( dwGrindingOption & EMGRINDINGOPTION_INC_GRINDINGRATE )  
		{
			dwGrindingOption &= ~EMGRINDINGOPTION_INC_GRINDINGRATE;
			dwGrindingLogOption = 0;
		}
		if ( dwGrindingOption & EMGRINDINGOPTION_ANTIRESET )
		{
			dwGrindingOption &= ~EMGRINDINGOPTION_ANTIRESET;
			dwGrindingLogOption = 0;
		}
	}

    BYTE ResultGrade = 0; // ���������ġ
	DWORD dwGrindingResult = DOGRINDING( pInvenItem->sItemCustom, pHold, sHOLDITEM, dwGrindingOption, ResultGrade );

	bool bRESET		= ( dwGrindingResult & EMGRINDINGRESULT_RESET ) ? true : false;
	bool bTERMINATE	= ( dwGrindingResult & EMGRINDINGRESULT_TERMINATE ) ? true : false;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	// �Ҹ�� ������ ó��.
    ConsumeInvenItem( wHoldPosX, wHoldPosY, false, wGradeNum );

	//�Ҹ�� ������ ó��.
 	DWORD dwUsingOption( EMGRINDINGOPTION_NONE );
 
 	if ( dwGrindingResult&EMGRINDINGRESULT_ANTIDISAPPEAR	) dwUsingOption |= EMGRINDINGOPTION_ANTIDISAPPEAR;
 	if ( dwGrindingResult&EMGRINDINGRESULT_INC_GRINDINGRATE ) dwUsingOption |= EMGRINDINGOPTION_INC_GRINDINGRATE;
 	if ( dwGrindingResult&EMGRINDINGRESULT_ANTIRESET		) dwUsingOption |= EMGRINDINGOPTION_ANTIRESET;
 
 	if ( dwUsingOption != EMGRINDINGOPTION_NONE )
 	{
 		for ( DWORD i = 0; i < SLOT_TSIZE/*SLOT_NSIZE_S_2*/; ++i )
 		{
 			if ( dwSlotOption[i] & dwUsingOption )
 			{
 				// Note : ���� ȿ���� �����Ѵ�.
 				//		  �ߺ� ������ �ȵǰ� �ϱ� ���ؼ�.
 				dwUsingOption &= ~dwSlotOption[i];
 				ConsumeSlotItem( EMSLOT( i ) );
 			}
 		}
 	}

	// �α� ���.
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ( EMITEM_ROUTE )ITEM_ROUTE_NORMAL_GRINDING, ( EMITEM_ROUTE_DETAIL )( LOG_ITEM_NORMAL_GRINDING + dwGrindingLogOption ) );

    if ( dwGrindingResult == EMGRINDINGRESULT_SUCCESS && ResultGrade > 9 && m_ClubDbNum != CLUB_NULL )
    {
        // [%1%]���� [%2%]�� [%3%] %4%������ �����ϼ̽��ϴ�.
        std::string ClubLog( 
            sc::string::format( 
                ID2GAMEINTEXT( "CLUB_LOG_22" ),
                m_szName,
                pInvenItem->sItemCustom.GETNAME().c_str(),
                ID2GAMEWORD( "ITEM_ADVANCED_INFO", pHold->sGrindingOp.emTYPE ),
                static_cast< int >( ResultGrade ) ) );
        m_pGaeaServer->SENDCLUBLOG( m_ClubDbNum, club::LOG_ENCHANT, ClubLog );        
    }

	// ���� ���� ���� ��ȯ.
	//
	GLMSG::SNET_INVEN_GRINDING_FB NetMsgFB;
	NetMsgFB.bRESET        = bRESET;
	NetMsgFB.bTERMINATE    = bTERMINATE;
	NetMsgFB.dwGrindResult = dwGrindingResult;
	NetMsgFB.emGrindFB	   = dwGrindingResult & EMGRINDINGRESULT_SUCCESS ? EMGRINDING_SUCCEED : EMGRINDING_FAIL;
	NetMsgFB.cGradePrev	   = cGrade;
	NetMsgFB.cGradeCurrent = pInvenItem->sItemCustom.GETGRADE( pHold->sGrindingOp.emTYPE );

	SendToClient( &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_GRINDER ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	//
	//mjeon.attendance
	//	
	if ( pHold->sGrindingOp.emCLASS == EMGRINDING_CLASS_ARM )
	{
		if ( dwGrindingResult&EMGRINDINGRESULT_SUCCESS )
		{
			AttendanceCheckEnchantWC( TRUE );
			AttendanceCheckEnchantW();
		}
		else
		{
			AttendanceCheckEnchantWC( FALSE );
		}		 
	}	
	
	if ( pHold->sGrindingOp.emCLASS == EMGRINDING_CLASS_CLOTH )
	{
		if ( dwGrindingResult & EMGRINDINGRESULT_SUCCESS )
		{
			AttendanceCheckEnchantAC( TRUE );
			AttendanceCheckEnchantA();
		}
		else
		{
			AttendanceCheckEnchantAC( FALSE );
		}
	}

	if ( dwGrindingResult & EMGRINDINGRESULT_SUCCESS )
	{
		DoQuestItemGrinding( pInvenItem->sItemCustom.GetNativeID(), pHold->sGrindingOp.emCLASS, true );
	}
	else
	{
		DoQuestItemGrinding( pInvenItem->sItemCustom.GetNativeID(), pHold->sGrindingOp.emCLASS, false );
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenGrinding", m_CharDbNum ) );
	}

	// �������� �Ӽ��� Ŭ���̾�Ʈ�� �˸�.
	//
	if ( !bTERMINATE )
	{
		// �ٷ� ��� �����Ѵ�.
		InvenUpdateItem( pInvenItem );

		GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
		NetItemUpdate.wPosX = pNetMsg->wPosX;
		NetItemUpdate.wPosY = pNetMsg->wPosY;
		NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &NetItemUpdate );
	}
	else
	{
		// LOG_ITEM_DEL_LOCKED_BOX ?
		LogItemExchange( 
			pInvenItem->sItemCustom, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, 0, 
			ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_LOCKED_BOX,
			false );

		/*
		�ı��� ��� ������ ���� �����ϰ� �ؿ��� �ڽ�Ƭ ó�� �Ѵ�. �̰� �ǳ�? �׽�Ʈ �ʿ�
		�ŷ� �Ұ� �ڽ�Ƭ�� �������� �κ� ������ 0���� �س��� �����ؼ� ������ �ı��� ��� �ڽ�Ƭ�� �и��Ǽ� �ٴڿ� ����߷��� ������ �ֽ��� �Ǿ���.
		�׷��� ���� �ı��ǵ��� �Ǿ��ֳ�?
		��ȹ ���� ���
		�������� �ı��� ��� �ű⿡ �پ��ִ� ��������
		������ �� �ڸ��� �־��ش�.
		*/

		// ���� �Ŀ� ó���ؾ� �Ǳ� ������ �����Ѵ�.
		SITEMCUSTOM sRestoredCostume( pInvenItem->sItemCustom );

		//	[�ڽſ���] Inventory�� ������ ����
		InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, DELETE_ITEM_GRINDING_TERMINATE, true );

		// ���� �ڽ�Ƭ�� �߶��� ������ �ڽ�Ƭ�� �и��ϰ� �����Ѵ�.
		if ( sRestoredCostume.nidDISGUISE != SNATIVEID( false ) )
		{			
			GLItemMan& glItemMan = GLItemMan::GetInstance();

			SITEM* pONE = glItemMan.GetItem( sRestoredCostume.nidDISGUISE );
			if ( !pONE )
				return E_FAIL;

			SITEMCUSTOM sITEM_NEW( sRestoredCostume.nidDISGUISE );

			CTime cTIME = CTime::GetCurrentTime();
			if ( sRestoredCostume.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
			{
				cTIME = CTime( sRestoredCostume.tDISGUISE );

				CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
				cTIME -= tLMT;
			}

			// ������ ����.
			//
			sITEM_NEW.SetPeriodExBasic( sRestoredCostume.GetPeriodExDisguise() );
			sITEM_NEW.cPeriodExtensionCount = sRestoredCostume.cCostumePeriodExtensionCount;
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.wTurnNum = 1;
			sITEM_NEW.cGenType = EMGEN_BILLING;
			sITEM_NEW.costumeUser = sRestoredCostume.costumeUser;	// rm #4525 ������ �Ҹ�� �ڽ�Ƭ �ɷ�ġ ���� ���� ���� ���� ��û
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			// ������ �ı��� �������� �ִ� �ڸ��� �����Ѵ�.
			WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
			BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
			if ( !bOk )
			{
				// ������ �������� �����ϰ� �� �ڸ��� �ִ°ǵ� �ڸ��� ������ �ȵɵ�
				return E_FAIL;
			}

			// �κ��� ����.
			InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, INSERT_ITEM_GRINDING_TERMINATE_RESTORE_COSTUME, true );

			// �и��� �ڽ��� �α�.
			// ��� �� �α� ����
			LogItemExchange( 
				sITEM_NEW, 
				gamelog::ID_CHAR, m_CharDbNum, 
				gamelog::ID_CHAR, 0, 
				ITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_COSTUME_TO_FIELD_BY_ERROR,
				false );

			/*
			�ٴڿ� ���־��ָ� ������ ���� ������ �߻��� �� �ִ�. �ŷ� �Ұ� ������ ��� ���� �ڵ�� ������
			BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
			if ( !bOk )
			{
				// ������ �ڽ����� �ٴڿ��� �ٽ� ������.
				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				pLand->DropItem ( m_vPosition, &( cDropItem.sItemCustom ), EMGROUP_ONE, m_dwGaeaID );

				// �и��� �ڽ��� �α�.
				// ��� �� �α� ����
				LogItemExchange( 
                    sITEM_NEW, 
                    gamelog::ID_CHAR, m_CharDbNum, 
                    gamelog::ID_CHAR, 0, 
                    ITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_COSTUME_TO_FIELD_BY_ERROR,
					false );
			}
            else
            {
				//	����ִ� ������ Inventory�� ����.
				m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
				SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

				//	[�ڽſ���] �޽��� �߻�.
				InvenInsertSend( pINSERT_ITEM );
			}*/
		}
	}
	return S_OK;
}

HRESULT GLChar::MsgReqInvenPeriodExtend( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNET_INVEN_PERIODEXTEND* pNetMsg = static_cast<GLMSG::SNET_INVEN_PERIODEXTEND*>(nmg);
	GLMSG::SNET_INVEN_PERIODEXTEND_FB netMsgFB(pNetMsg->wPosX, pNetMsg->wPosY, pNetMsg->wHoldPosX, pNetMsg->wHoldPosY);
	SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
	if ( !pHoldInvenItem )
		return S_FALSE;

	SITEMCUSTOM& sHOLDITEM = pHoldInvenItem->sItemCustom;
	const SITEM* pHold = GLogicData::GetInstance().GetItem( sHOLDITEM.GetNativeID() );
	if ( !pHold )
		return E_FAIL;

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) )
		return S_FALSE;

	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	SITEMCUSTOM& sCustomItem = pInvenItem->sItemCustom;
	INT nBaseMaxCount(-1), nCostumeMaxCount(-1);
	const bool bBase = ItemPeriodExtension::Manager::GetInstance().IsExtendableItem(sHOLDITEM.GetNativeID(), sCustomItem.GetNativeID(), nBaseMaxCount) == S_OK
		, bCostume = ItemPeriodExtension::Manager::GetInstance().IsExtendableItem(sHOLDITEM.GetNativeID(), sCustomItem.nidDISGUISE, nCostumeMaxCount) == S_OK;
	
	// �Ѵ� ���밡���� �� ����;   
	if ( bBase && bCostume )
	{
		netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_DUPLICATION;
		SendToClient(&netMsgFB);
		return S_FALSE;
	}

	// �Ϲ� �����۰� �ڽ�Ƭ�� ���� ó��������Ѵ�;
	if ( bBase )
	{
		// ��ȿ�� ������ ����;
		if ( sCustomItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_NORMAL) == false )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// Ƚ�� ����;
		if ( nBaseMaxCount != 0 && nBaseMaxCount <= sCustomItem.cPeriodExtensionCount )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}

		__time64_t tPrevTime = sCustomItem.tBORNTIME + sCustomItem.GetPeriodExBasic();
		CTime cTimeExcCheck(tPrevTime + pHold->sDrugOp.dwCureVolume);
		// DB �������� smalldate
		if ( cTimeExcCheck.GetYear() >= 2079 )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}
		// ���� ó��;
		// sCustomItem.tBORNTIME += pHold->sDrugOp.dwCureVolume;
		sCustomItem.SetPeriodExBasic( sCustomItem.GetPeriodExBasic() + pHold->sDrugOp.dwCureVolume );
		
		// 0�̸� ���Ѱ���;
		if ( nBaseMaxCount != 0 )
		{
			sCustomItem.cPeriodExtensionCount += 1;
			netMsgFB.wRemainCount = nBaseMaxCount - sCustomItem.cPeriodExtensionCount;
		}
		else
		{
			netMsgFB.wRemainCount = -1;
		}

		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::ItemPeriodExLogInsert( m_CharDbNum
				, sCustomItem.GetNativeID().Mid()
				, sCustomItem.GetNativeID().Sid()
				, nBaseMaxCount - sCustomItem.cPeriodExtensionCount
				, tPrevTime
				, sCustomItem.tBORNTIME + sCustomItem.GetPeriodExBasic())));

		netMsgFB.emResult = EMREQ_PERIODEX_FB_SUCCESS_BASE;
	}
	else if ( bCostume )
	{
		// ��ȿ�� ������ ����;
		if ( sCustomItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_COSTUME) == false )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// Ƚ�� ����;
		if ( nCostumeMaxCount != 0 && nCostumeMaxCount <= sCustomItem.cCostumePeriodExtensionCount )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// �ڽ�Ƭ �Ⱓ ���� �� ���� ������ ������ ���� �Ⱓ�� ����� ��� Ȯ�� ��û���Ѵ�;
		if ( sCustomItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_NORMAL) )
		{
			const SITEM* pBaseItem = GLogicData::GetInstance().GetItem( sCustomItem.GetNativeID() );
			if ( pBaseItem == NULL )
				return E_FAIL;

			if ( pNetMsg->bReconfirm == false &&
				sCustomItem.tBORNTIME + sCustomItem.GetPeriodExBasic() + pBaseItem->sDrugOp.tTIME_LMT < sCustomItem.tDISGUISE + sCustomItem.GetPeriodExDisguise() + pHold->sDrugOp.dwCureVolume )
			{
				netMsgFB.emResult = EMREQ_PERIODEX_FB_CHECK_EXPIRATIONDATE;
				SendToClient(&netMsgFB);
				return S_FALSE;
			}
		}

		__time64_t tPrevTime = sCustomItem.GetPeriodExDisguise() + sCustomItem.GetPeriodExDisguise();

		CTime cTimeExcCheck(tPrevTime + pHold->sDrugOp.dwCureVolume);
		// DB �������� smalldate;
		if ( cTimeExcCheck.GetYear() >= 2079 )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}

		// ���� ó��;
		// sCustomItem.tDISGUISE += pHold->sDrugOp.dwCureVolume;
		sCustomItem.SetPeriodExDisguise( sCustomItem.GetPeriodExDisguise() + pHold->sDrugOp.dwCureVolume );
		
		// 0�̸� ���Ѱ���;
		if ( nCostumeMaxCount != 0 )
		{
			sCustomItem.cCostumePeriodExtensionCount += 1;
			netMsgFB.wRemainCount = nCostumeMaxCount - sCustomItem.cCostumePeriodExtensionCount;
		}
		else
		{
			netMsgFB.wRemainCount = -1;
		}

		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::ItemPeriodExLogInsert( m_CharDbNum
			, sCustomItem.nidDISGUISE.Mid()
			, sCustomItem.nidDISGUISE.Sid()
			, nCostumeMaxCount - sCustomItem.cCostumePeriodExtensionCount
			, tPrevTime
			, sCustomItem.GetPeriodExDisguise())));

		netMsgFB.emResult = EMREQ_PERIODEX_FB_SUCCESS_COSTUME;
	}
	else
	{
		netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
		SendToClient(&netMsgFB);
		return S_FALSE;
	}

	// �ٷ� ��� �����Ѵ�;
	InvenUpdateItem( pInvenItem );

	GLMSG::SNET_INVEN_ITEM_UPDATE netUpdateMsg;
	netUpdateMsg.wPosX = pNetMsg->wPosX;
	netUpdateMsg.wPosY = pNetMsg->wPosY;
	netUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &netUpdateMsg );

	// �κ� Ȧ�� ������ ����;
	// InvenDeleteItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, true, pHoldInvenItem->CustomItem().bVietnamGainItem, PERIOD_EXTENSION_ITEM, true );
	
	// �Ҹ�� ������ ó��;
	ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

	SendToClient(&netMsgFB);
}

HRESULT GLChar::MsgReqInvenResetSkSt( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_RESET_SKST* pNetMsg = ( GLMSG::SNET_INVEN_RESET_SKST* )nmg;

	GLMSG::SNET_INVEN_RESET_SKST_FB	MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// ���� ���� Ȯ��
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{		
		MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
		SendToClient( &MsgFB );		
		return E_FAIL;
	}

	// Note : �ش� �������� Ÿ�԰� ���� ���� Ȯ��
	WORD* pResetStats = pNetMsg->wResetStats;
	WORD  wAmount	  = pITEM->sDrugOp.dwCureVolume;

	if ( !pResetStats )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
		SendToClient( &MsgFB );		
		return E_FAIL;
	}

	if ( wAmount != pNetMsg->wAmount )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
		SendToClient( &MsgFB );		
		return E_FAIL;
	}

	switch ( pITEM->sBasicOp.emItemType )
	{
	// Note : �� ������ ��
	case ITEM_SKP_STAT_RESET_A:
		{
			if ( ITEM_SKP_STAT_RESET_A != pNetMsg->emItemType )
			{
				MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
				SendToClient( &MsgFB );		
				return E_FAIL;
			}

			// Note : �յ��ϰ� ���ҵ� ��ġ�� �����´�.
			GET_EVENLY_DECREASE_STATS( pResetStats, wAmount );
		}
		break;

	// Note : ������ �ž� (���� ���� ����)
	case ITEM_SKP_STAT_RESET_B:
		{
			if ( ITEM_SKP_STAT_RESET_B != pNetMsg->emItemType )
			{
				MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
				SendToClient( &MsgFB );		
				return E_FAIL;
			}

			WORD wNowAmount = 0;

			for ( WORD i=0; i<EMSIZE; ++i )
			{
				wNowAmount += pResetStats[i];
			}

			// Note : �ִ� �ʱ�ȭ �Ǵ� ������ ������ ����.
			if ( wAmount < wNowAmount )
			{
				MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
				SendToClient( &MsgFB );		
				return E_FAIL;
			}
		}
		break;

	// Note : ����
	default:
		{
			MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	// ��� ��ų�� ��� �� ���ӸӴ� ����, �������� �����ϸ� ������� ��������� ���� �κ��丮�� �����Ѵ�.
	// ���ӸӴ� ��ȭ Ÿ�� �� �������ͽ�ü���� Ÿ�� �߰��Ǿ���. ��� �� ������ ���� �ʿ��ϴ�.
	LONGLONG ReturnMoney = GLSkillResetDefine::Instance()->GetLearnedSkillCost( m_ExpSkills );
	AddInvenMoneyServer( ReturnMoney, ADD_INVEN_MONEY_SKILL_RESET );
	SendInvenMoneyUpdate();

	LogMoneyExchange( 
		gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, 
		ReturnMoney, 
		ITEM_ROUTE_SKILL_RESET, 
		GetCurrentMap(), 
		LOG_MONEY_SKILL_RESET );

	LogMoneyExchange( 
		gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, 
		GetInvenMoney(), 
		MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, 
		GetCurrentMap(), 
		LOG_MONEY_CHAR_INVEN_CURRENT );


	// ��ų, ���� ����.
	//
	WORD wPREREMAINDSKILL = ( WORD )m_dwSkillPoint;

	if ( wAmount == USHRT_MAX )
	{
		// Note : ��ü ��ų ���� �ʱ�ȭ
		RESET_STATS_SKILL( NULL );
		MsgFB.bRESETALL = TRUE;
	}
	else
	{
		RESET_STATS_SKILL( pResetStats );
	}

	// [�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// Note : ���� ���� ����
	WORD wRETSTATSNUM = 0;
	WORD wRETSKILLNUM = 0;

	for ( WORD i=0; i<EMSIZE; ++i )
	{
		wRETSTATSNUM += pResetStats[i];
	}

	wRETSKILLNUM = ( WORD )( m_dwSkillPoint - wPREREMAINDSKILL );

	// ���� �޽��� ����.
	//
	MsgFB.emFB       = EMREQ_RESET_SKST_FB_OK;
	MsgFB.llGameMoney = ReturnMoney;
	MsgFB.wSTATS_NUM = wRETSTATSNUM;
	MsgFB.wSKILL_NUM = wRETSKILLNUM;
	memcpy( MsgFB.wResetStats, pResetStats, sizeof( WORD ) * EMSIZE );
	SendToClient( &MsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_RESET_SKILL_STAT ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	RemoveSkillFactofNormalBuf();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxWrapping( NET_MSG_GENERIC* nmg )
{
    if ( !IsValidBody() )
        return E_FAIL;

    GLMSG::SNET_INVEN_BOX_WRAPPING* pNetMsg = ( GLMSG::SNET_INVEN_BOX_WRAPPING* ) nmg;

    GLMSG::SNET_INVEN_BOX_WRAPPING_FB NetMsgFB;

    WORD wPosX = pNetMsg->wPosX;
    WORD wPosY = pNetMsg->wPosY;

    // ��ȿ�� �κ��丮 ���� �˻�
    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    // ��ȿ�� ���������� �˻�
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

    if ( !pHoldInvenItem )
        return E_FAIL;

    // �տ� ��� �ִ� �������� ��ȿ���� �˻�
    const SITEMCUSTOM& sITEM_HOLD = pHoldInvenItem->CustomItem();
    if ( sITEM_HOLD.GetNativeID() == SNATIVEID( false ) )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // �տ��� �������� ���� ���� ����
    SITEM* pHold = GLogicData::GetInstance().GetItem( sITEM_HOLD.Id() );
    if ( !pHold )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // �տ��� �������� ���������� �˻�
    if ( pHold->sBasicOp.emItemType != ITEM_BOX_WRAPPER )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // �κ��丮 �������� ���尡������ �˻�;
    if ( pInvenItem->sItemCustom.IsBoxWrappable() == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // PetDbNum�� ������ID ���� ������ ����ϰ� �ֱ� ������ PetCard�� ������ �� ����. ���Ŀ� ������������ �߰��� �� ���� PetCard�� ���尡���ϵ��� ���� �ʿ�;
    if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    if ( pInvenItem->sItemCustom.IsBoxWrapped() )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	if ( pInvenItem->sItemCustom.IsTempEnchant() )
	{
		NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

    // ������ ����.;
    // ������ DB ���̳ʸ� ��ȯ �۾� �Ϸ��� �ݵ�� ������ ������ �÷��� ���ο� ������ �Ҵ��ؾ� ��;
    if ( pInvenItem->sItemCustom.BoxWrapping( sITEM_HOLD.Id() ) == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	// ��� ������Ʈ
	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19705 ) ) );
	}
	else
	{
		// ���� ItemUpdate ���, ������ ��� ������ ���̵� ����Ǳ� ������ EntireItemUpdate ���
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::EntireItemUpdate( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19705 ) ) );
	}

    GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
    NetItemUpdate.wPosX = pNetMsg->wPosX;
    NetItemUpdate.wPosY = pNetMsg->wPosY;
    NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
    SendToClient( &NetItemUpdate );

    // �Ϲ� �������� ����� ���������� ��ȯ�Ǵ� �� �α� ����.;
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_WRAPPING_BY_ITEM );

    GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
    if ( !pTarChar )
    {		
        // ��� �� �α� ����;
        // ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��;
        sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenBoxWrapping", m_CharDbNum ) );
    }

    // �������� ���� ���� ��� ���;
    // ��� �� �α� ����;
    LogItemExchange(
        sITEM_HOLD,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, sITEM_HOLD.wTurnNum, 
        ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_WRAPPING,
        false );

    // �κ� Ȧ�� ������ ����.;
    ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

    // ���� ���� �˸�.;
    NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_SUCCESS;
    SendToClient( &NetMsgFB );

    return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxUnwrapping( NET_MSG_GENERIC* nmg )
{
    if ( !IsValidBody() )
        return E_FAIL;

    GLMSG::SNET_INVEN_BOX_UNWRAPPING* pNetMsg = ( GLMSG::SNET_INVEN_BOX_UNWRAPPING* ) nmg;

    GLMSG::SNET_INVEN_BOX_UNWRAPPING_FB NetMsgFB;

    WORD wPosX = pNetMsg->wPosX;
    WORD wPosY = pNetMsg->wPosY;

    // ��ȿ�� �κ��丮 ���� �˻�
    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        NetMsgFB.emFB = EMREQ_BOX_UNWRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }
    // ������ ����.;
    // ������ DB ���̳ʸ� ��ȯ �۾� �Ϸ��� �ݵ�� ������ ������ �÷��� ���ο� ������ �Ҵ��ؾ� ��;
    if ( pInvenItem->sItemCustom.BoxUnwrapping() == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_UNWRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }

	// ��� ������Ʈ
	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19706 ) ) );
	}
	else
	{
		// ���� ItemUpdate ���, ������ ��� ������ ���̵� ����Ǳ� ������ EntireItemUpdate ���
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::EntireItemUpdate( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19706 ) ) );
	}

    GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
    NetItemUpdate.wPosX = pNetMsg->wPosX;
    NetItemUpdate.wPosY = pNetMsg->wPosY;
    NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
    SendToClient( &NetItemUpdate );

    // ���� �������� Ǯ�� ���������� ��ȯ�Ǵ� �� �α� ����.;
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_UNWRAPPING );

    // ���� Ǯ�� ���� �˸�.;
    NetMsgFB.emFB = EMREQ_BOX_UNWRAPPING_FB_SUCCESS;
    SendToClient( &NetMsgFB );

    return S_OK;
}

HRESULT GLChar::MsgInvenInitializeCountry( NET_MSG_GENERIC* pMsg, DWORD dwClientID )
{
	if ( !IsValidBody() )
		return E_FAIL;

	if ( NULL == m_pGaeaServer )
		return E_FAIL;

	GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_CF* pCountryMsg =
		static_cast< GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_CF* >( pMsg );
	if ( NULL == pCountryMsg )
		return E_FAIL;

	WORD wPosX = pCountryMsg->m_wPosX;
	WORD wPosY = pCountryMsg->m_wPosY;

	// ��ȿ�� �κ��丮���� �˻�;
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if ( !pItemData || pItemData->BasicType() != ITEM_INITIALIZE_COUNTRY )
	{
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// Initialize Country;
	m_pGaeaServer->InitializeCountry( dwClientID );

	// ������ �Ҹ�;
	ConsumeInvenItem( wPosX, wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqCharCard( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_CHARCARD* pNetMsg = ( GLMSG::SNET_INVEN_CHARCARD* )nmg;

	GLMSG::SNET_INVEN_CHARCARD_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_CHARCARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_CHARACTER_CARD )
	{
		MsgFB.emFB = EMREQ_CHARCARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// �ɸ��� ���� ���� ����.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameDbJob(
            db::DbActionPtr(
                new db::CharacterCreateNumIncrease( m_dwUserID ) ) );
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// ó�� ���� �˸�.
	MsgFB.emFB = EMREQ_CHARCARD_FB_OK;
	SendToClient( &MsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqStorageCard( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_STORAGECARD* pNetMsg = ( GLMSG::SNET_INVEN_STORAGECARD* )nmg;

	GLMSG::SNET_INVEN_STORAGECARD_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_STORAGE_CARD )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( pNetMsg->wSTORAGE < EMSTORAGE_CHANNEL_SPAN || pNetMsg->wSTORAGE >= ( EMSTORAGE_CHANNEL_SPAN + EMSTORAGE_CHANNEL_SPAN_SIZE ) )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_INVNUM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// â�� ���� �ð� ����.
	//
	int nINDEX = pNetMsg->wSTORAGE - EMSTORAGE_CHANNEL_SPAN;
	__time64_t& tTIME = m_tSTORAGE[nINDEX];

	CTime tCurTime = CTime::GetCurrentTime();
	CTime tSeedTime( tTIME );
	CTimeSpan tLMT ( pITEM->sDrugOp.tTIME_LMT );
	CTimeSpan tADD( 0, 1, 30, 0 );

	// ���� �ʱ�ȭ ���� �ʾҰų�, �����ð� �ʰ��Ǿ��� ���.
	if ( tTIME == 0 || tSeedTime < tCurTime )
	{
		tSeedTime = tCurTime + tLMT + tADD;
	}
	// �����ð��� ���� ������ ������.
	else
	{
		tSeedTime = tSeedTime + tLMT + tADD;
	}

	// ���ð� ������, ��� �������� ����.
	tTIME = tSeedTime.GetTime();
	m_bSTORAGE[nINDEX] = true;

	// DB�� ���丮�� �����ð� ����.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameAdoJob( 
            db::DbActionPtr( 
                new db::StorageDateSet( m_dwUserID, nINDEX + EMSTORAGE_CHANNEL_SPAN, tTIME ) ) );
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// ó�� ��� �˸�.
	CTimeSpan tDX = tSeedTime - tCurTime;
	MsgFB.emFB = EMREQ_STORAGECARD_FB_OK;
	MsgFB.tSTORAGE_LIMIT = tSeedTime.GetTime();
	MsgFB.tSPAN = tDX.GetTimeSpan();
	MsgFB.wSTORAGE = pNetMsg->wSTORAGE;
	SendToClient( &MsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_STORAGE_EXTENSION ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenLine( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_INVENLINE* pNetMsg = ( GLMSG::SNET_INVEN_INVENLINE* )nmg;

	GLMSG::SNET_INVEN_INVENLINE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_INVENLINE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_INVEN_CARD )
	{
		MsgFB.emFB = EMREQ_INVENLINE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		if ( m_wINVENLINE >= GLCONST_CHAR::wInvenExtendLine )
		{
			MsgFB.emFB = EMREQ_INVENLINE_FB_MAXLINE;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}
	else
	{
		if ( m_wINVENLINE >= EInvenY - GLCONST_CHAR::wInvenDefaultLine - EInvenPremiumLine )
		{
			MsgFB.emFB = EMREQ_INVENLINE_FB_MAXLINE;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	// �κ� ���� ����.
	++m_wINVENLINE;

	// ���� Ȱ��ȭ�� �κ� ���� ����.
	//
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	// DB�� ����.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new db::InvenAddNumSet( m_CharDbNum, m_wINVENLINE ) ) );
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// ó�� ��� �˸�.
	MsgFB.emFB = EMREQ_INVENLINE_FB_OK;
	MsgFB.wINVENLINE = m_wINVENLINE;
	SendToClient( &MsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_INVEN_EXTENSION ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

// *****************************************************
// Desc: ���� ��� �̿�ī�� ��û ( ī�常 �Ҹ��Ŵ )
// *****************************************************
HRESULT GLChar::MsgReqInvenRemodelOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_REMODELOPEN* pNetMsg = ( GLMSG::SNET_INVEN_REMODELOPEN* )nmg;

	GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( pITEM->sBasicOp.emItemType != ITEM_REMODEL )
	{
		MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	m_ItemRemodelPosX = pNetMsg->wPosX;
	m_ItemRemodelPosY = pNetMsg->wPosY;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wStampCardPosX, pNetMsg->wStampCardPosY );
		_SetStampInfo(pInvenItem);
	}
	else
	{
		m_ItemStampPosX = USHRT_MAX;
		m_ItemStampPosY = USHRT_MAX;
	}

	MsgFB.emFB = EMREQ_REMODELOPEN_FB_OK;

	SendToClient( &MsgFB );

	return S_OK;
}


HRESULT GLChar::MsgReqInvenGarbageOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_GARBAGEOPEN* pNetMsg = ( GLMSG::SNET_INVEN_GARBAGEOPEN* )nmg;

	GLMSG::SNET_INVEN_GARBAGEOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_GARBAGE_CARD )
	{
		MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	m_ItemGarbagePosX = pNetMsg->wPosX;
	m_ItemGarbagePosY = pNetMsg->wPosY;

	MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_OK;

	SendToClient( &MsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenPMarketSearchOpen( NET_MSG_GENERIC* nmg )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return E_FAIL;
    }

    const GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_CF* pNetMsg = ( GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_CF* )nmg;
    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FB MsgFB;

    // ���λ��� �˻� ���Ű� ������ ���ΰ�?
    if ( false == private_market::GLPrivateMarketDefine::Instance()->CanPurchase(m_mapID.getGaeaMapID()) )
    {
        MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_BLOCK;
        SendToClient( &MsgFB );
        return E_FAIL;
    }

    // ������ ��ȿ�� �˻�
    SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pINVENITEM )
    {
        MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_NOITEM;
        SendToClient( &MsgFB );
        return E_FAIL;
    }
    
    if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
        return E_FAIL;

    const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
    if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_PMARKET_SEARCH_CARD )
    {
        MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_NOITEM;
        SendToClient( &MsgFB );
        return E_FAIL;
    }
    
	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	m_bUsedPMarketSeacrhCard = TRUE;

	MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_OK;
	MsgFB.wPosX = pNetMsg->wPosX;
	MsgFB.wPosY = pNetMsg->wPosY;

	SendToClient( &MsgFB );

	// ó�� ��ü�� ������Ʈ���� �ʵ�� �����
	/*
    // ������ ��뿡���� ��ȿ���˻縦 ��ġ�� ���� ������Ʈ ������ ���λ����˻� ī�� ���������
    // �˸��� �̿� ���� ������ ���� �Ŀ� ������ �Ҹ� ��Ŵ (ĳ�����̹Ƿ� �����ϰ� �Ҹ���Ѿ� ��)
    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FA MsgFA;
    MsgFA.dwGaeaID = this->GetGaeaID();
    MsgFA.wPosX = pNetMsg->wPosX;
    MsgFA.wPosY = pNetMsg->wPosY;
    SendToAgent( &MsgFA );
    */

    return S_OK;
}

HRESULT GLChar::MsgReqInvenPMarketSearchOpenFB( NET_MSG_GENERIC* nmg )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return E_FAIL;
    }

    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_AF* pNetMsg = ( GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_AF* )nmg;

    //	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
    ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FB MsgFB;
    MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_OK;
    MsgFB.wPosX = pNetMsg->wPosX;
    MsgFB.wPosY = pNetMsg->wPosY;

    SendToClient( &MsgFB );
	*/

    return S_OK;
}

// *****************************************************
// Desc: ��� â�� �̿�ī�� ��û ( ī�常 �Ҹ��Ŵ )
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageOpen( NET_MSG_GENERIC* nmg )
{
	// â����ī�带 ������̸�
	if ( m_bUsedStorageCard )
		return E_FAIL;

	GLMSG::SNET_INVEN_STORAGEOPEN* pNetMsg = ( GLMSG::SNET_INVEN_STORAGEOPEN* )nmg;

	GLMSG::SNET_INVEN_STORAGEOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_STORAGEOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_STORAGE_CONNECT )
	{
		MsgFB.emFB = EMREQ_STORAGEOPEN_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	MsgFB.emFB = EMREQ_STORAGEOPEN_FB_OK;
	MsgFB.wPosX = pNetMsg->wPosX;
	MsgFB.wPosY = pNetMsg->wPosY;

	SendToClient( &MsgFB );	

	// â����ī�� ��뿩��
	m_bUsedStorageCard = true;

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_STORAGE_LINK ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

// *****************************************************
// Desc: ��� â�� ��� �Ϸ�
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageClose( NET_MSG_GENERIC* nmg )
{
	// â����ī�� ��뿩��
	m_bUsedStorageCard = false;

	return S_OK;
}

HRESULT GLChar::MsgReqInvenPremiumSet( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_PREMIUMSET* pPacket = ( GLMSG::SNET_INVEN_PREMIUMSET* ) nmg;

	GLMSG::SNET_INVEN_PREMIUMSET_FB MsgFB;
	MsgFB.dwChaNum = CharDbNum();

	const SINVENITEM* pINVENITEM = m_cInventory.GetItem( pPacket->m_PosX, pPacket->m_PosY );
	if (!pINVENITEM)
	{
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime(pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_PREMIUMSET )
	{
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenPremiumSet", m_CharDbNum ) );
	}

	// �κ��� ���� ���� ����
	// ���ʿ��� Assign ���� 
// 	GLInventory cInvenTemp;
// 	cInvenTemp.Assign( m_cInventory );

	DWORD BoxItemCount = 0;
	int nBoxSize = min( ITEM::SBOX::ITEM_SIZE, ( int )pITEM->sBox.vecItems.size() );
	for ( int i=0; i<nBoxSize; ++i )
	{
		if ( SNATIVEID( false ) == pITEM->sBox.vecItems[i].nidITEM )
		{
			continue;
		}

		const SITEM* pONE = GLogicData::GetInstance().GetItem( pITEM->sBox.vecItems[i].nidITEM );
		if ( !pONE )
		{
			MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		BoxItemCount++;
// 		SITEMCUSTOM sCUSTOM( pITEM->sBox.vecItems[i].nidITEM );
// 
// 		BOOL bOK = cInvenTemp.InsertItem( sCUSTOM );
// 		if ( !bOK )
// 		{
// 			// �κ��� ������ ���� ������ �Ǵܵ�
// 			MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOTINVEN;
// 			SendToClient( &MsgFB );
// 			return E_FAIL;
// 		}
	}

	if ( BoxItemCount > m_cInventory.GetAmountEmpty( false ) )
	{
		// �κ��� ������ ���� ������ �Ǵܵ�
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOTINVEN;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

    // ���ھȿ� �ִ� �������� ����� �ִ´�.
	for ( int i=0; i<nBoxSize; ++i )
	{
		if ( SNATIVEID( false ) == pITEM->sBox.vecItems[i].nidITEM )
		{
			continue;
		}

		const SITEM* pONE = GLogicData::GetInstance().GetItem( pITEM->sBox.vecItems[i].nidITEM );
		if ( !pONE )
			return E_FAIL;

		SITEMCUSTOM sITEM_NEW( pITEM->sBox.vecItems[i].nidITEM );

		// ������ ����
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = ( WORD )pITEM->sBox.vecItems[i].dwAMOUNT;
		sITEM_NEW.cGenType = pINVENITEM->sItemCustom.cGenType;
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		LogItemBasicStatOption( sITEM_NEW );
		LogItemLinkSkillOption( sITEM_NEW );
		LogItemAddonOption( sITEM_NEW );

		// ������ ����Ʈ���� �Է��� ���� ��� ����
		sITEM_NEW.cDAMAGE = ( BYTE )pONE->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pONE->sBasicOp.wGradeDefense;

		// ���� �ɼ� ����.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );
		
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
		if ( !bOk )
            return S_OK;	//	�κ� ���� �������� �ǵ��� ����.

		InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, false, 1, true, false, true );
		const SINVENITEM* pInvenItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
		if ( !pInvenItem )
            return E_FAIL;

		// �α� ����.
		// ��� �� �α� ����
		LogItemExchange(
            pInvenItem->sItemCustom,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_PREMIUMSET,
			false );

		//	[�ڽſ���] �޽��� �߻�.
		InvenInsertSend( pInvenItem, FALSE, false, true );
	}

	// �����̾� ����.
	//
	CTime tPREMIUM( m_tPREMIUM );
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tLMT( pITEM->sDrugOp.tTIME_LMT );
	CTimeSpan tADD( 0, 1, 30, 0 );

	if ( m_tPREMIUM == 0 || tPREMIUM < tCurTime )
	{
		tPREMIUM = tCurTime + tLMT + tADD;
	}
	else
	{
		tPREMIUM = tPREMIUM + tLMT + tADD;
	}

	// �����̾� ��� ON
	m_tPREMIUM = tPREMIUM.GetTime();
	m_bPREMIUM = true;

	// ���� Ȱ��ȭ�� �κ� ���� ����
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	// DB�� ����
	AddGameDbJob(
        db::DbActionPtr(
            new db::PremiumTimeSet( m_dwUserID, m_tPREMIUM ) ) );

	// Note : CSetPremiumTime Ŭ�������� ��� ��츦 ó���ϵ��� �����
	/*
	if ( m_pGLGaeaServer->GetDBMan() )
	{
		CDaumSetPremiumTime *pDbAction = new CDaumSetPremiumTime ( m_dwUserID, m_tPREMIUM );
		m_pGLGaeaServer->GetDBMan()->AddJob ( pDbAction );
	}
	*/
	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pPacket->m_PosX, pPacket->m_PosY );


	// ������Ʈ�� ���ļ� �޽��� ����.
	//		( ������Ʈ�� ��ϵǾ� �ִ� �����̾� �Ⱓ�� ���� ������� �Ѵ�. )
	//
	CTimeSpan tDX = tPREMIUM - tCurTime;
	MsgFB.emFB = EMREQ_PREMIUMSET_FB_OK;
	MsgFB.tLMT = m_tPREMIUM;
	MsgFB.tSPAN = tDX.GetTimeSpan();
	SendToAgent( &MsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIR_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_HAIR_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_HAIR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	bool bVALIDCLASS = ( NULL != ( pITEM->sBasicOp.dwReqCharClass & m_emClass ) );
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADCLASS;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	WORD wHAIR = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex( m_emClass );
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHAIRNUM <= wHAIR || MAX_HAIR <= wHAIR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( sCHARCONST.strHAIR_CPS[wHAIR].empty() )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// �� ��ȭ ��Ŵ.
	//
	m_wHair = wHAIR;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LOOK_CHANGE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}
	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairStyleChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIRSTYLE_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_HAIRSTYLE_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_HAIR_STYLE )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	bool bVALIDCLASS = ( NULL != ( pITEM->sBasicOp.dwReqCharClass & m_emClass ) );
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADCLASS;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	EMCHARINDEX emIndex = CharClassToIndex( m_emClass );
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHAIRNUM <= pNetMsg->wHairStyle || MAX_HAIR <= pNetMsg->wHairStyle )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	m_wHair = pNetMsg->wHairStyle;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
    AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterHairStyleSet( m_CharDbNum, m_wHair ) ) );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LOOK_CHANGE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairColorChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );

	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_HAIR_COLOR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// �� ��ȭ ��Ŵ.
	//
	m_wHairColor = pNetMsg->wHairColor;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.wHairColor = m_wHairColor;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.wHairColor = m_wHairColor;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
    AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterHairColorSet( m_CharDbNum, m_wHairColor ) ) );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LOOK_CHANGE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenFaceChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_FACE_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_FACE_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_FACE_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );		
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_FACE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	bool bVALIDCLASS = ( NULL != ( pITEM->sBasicOp.dwReqCharClass & m_emClass ) );
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADCLASS;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	WORD wFACE = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex( m_emClass );
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHEADNUM <= wFACE || MAX_HEAD <= wFACE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( sCHARCONST.strHEAD_CPS[wFACE].empty() )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// ���� ��ȭ ��Ŵ.
	//
	m_wFace = wFACE;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// �� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenFaceStyleChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_FACESTYLE_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_FACESTYLE_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_FACE_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{		
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_FACE_STYLE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	bool bVALIDCLASS = ( NULL != ( pITEM->sBasicOp.dwReqCharClass & m_emClass ) );
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADCLASS;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	EMCHARINDEX emIndex = CharClassToIndex( m_emClass );
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHEADNUM <= pNetMsg->wFaceStyle || MAX_HEAD <= pNetMsg->wFaceStyle )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	m_wFace = pNetMsg->wFaceStyle;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterFaceStyleSet( m_CharDbNum, m_wFace ) ) );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LOOK_CHANGE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}
	return S_OK;
}

HRESULT GLChar::MsgReqInvenGenderChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_GENDER_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_GENDER_CHANGE* )nmg;
	GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB MsgFB;

	// ������ ����� Ȯ���� �ؿ��� �ϵ��� ��ġ ������
	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( pITEM->sBasicOp.emItemType != ITEM_GENDER_CHANGE )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_ITEMTYPE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// ���� ������ ���� ���� ������ ���� ���� üũ
	int dwNeedfullInventory = 0;
	for ( int i = 0; i < SLOT_TSIZE; i++ )
	{
		// SITEMCUSTOM sItemcustom  ���� �����ڷ� ������ const SITEMCUSTOM& sItemcustom
		const SITEMCUSTOM& sItemcustom = GetCharData2().m_PutOnItems[i];
		if ( NATIVEID_NULL() == sItemcustom.GetNativeID() )
		{
			continue;
		}

		SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemcustom.GetNativeID() );
		if ( !pITEM )
		{
			continue;
		}

		if ( !( GLCC_WOMAN & pITEM->sBasicOp.dwReqCharClass ) || !( GLCC_MAN & pITEM->sBasicOp.dwReqCharClass ) )
		{
			dwNeedfullInventory++;
		}
		else
		{
			if ( sItemcustom.nidDISGUISE != NATIVEID_NULL() )
			{
				SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemcustom.nidDISGUISE );
				if ( pITEM )
				{
					if ( !( GLCC_WOMAN & pITEM->sBasicOp.dwReqCharClass ) || !( GLCC_MAN & pITEM->sBasicOp.dwReqCharClass ) )
					{
						dwNeedfullInventory++;
					}
				}
			}
		}
	}

	// ���� ���� ������ ������ �κ��丮 ����� üũ
	DWORD dwAmountEmpty = m_cInventory.GetAmountEmpty( FALSE );
	int dwEmptyInventory = 0;

	dwEmptyInventory = static_cast< int >( dwAmountEmpty ) - dwNeedfullInventory;

	if ( dwEmptyInventory < 0 )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

/*
	//	�ذ��� ���� ���� �ǰ� Ǯ�����ϴ�. 
	if ( m_emClass == GLCC_EXTREME_M || m_emClass == GLCC_EXTREME_W )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_BADCLASS;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}
*/

	EMCHARCLASS emClass = CharClassToSex( m_emClass );
	EMCHARINDEX emIndex = CharClassToIndex( emClass );

	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( pNetMsg->wFace >= sCHARCONST.dwHEADNUM || pNetMsg->wHair >= sCHARCONST.dwHAIRNUM_SELECT )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_NOTVALUE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

    m_emClass = emClass;
	m_wHair = pNetMsg->wHair;
	m_wFace = pNetMsg->wFace;	
	m_wHairColor = HAIRCOLOR::GetHairColor( emIndex, m_wHair );

	m_wSex = m_wSex ? 0 : 1;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.emClass	 = m_emClass;
	MsgFB.wSex		 = m_wSex;
	MsgFB.wFace      = m_wFace;
	MsgFB.wHair      = m_wHair;
	MsgFB.wHairColor = m_wHairColor;

	MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterGenderChangeSet(
                m_CharDbNum,
                m_emClass,
                m_wSex,
                m_wFace,
                m_wHair,
                m_wHairColor ) ) );

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_LOOK_CHANGE ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenSchoolChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_SCHOOL_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_SCHOOL_CHANGE* )nmg;
	GLMSG::SNETPC_INVEN_SCHOOL_CHANGE_FB MsgFB;

	std::vector< DWORD > vecDelQuestList;
	std::vector< DWORD >::iterator iterDelQuestList;

	WORD wCurrentSchool = 0;

	wCurrentSchool = m_wSchool;

	std::tr1::shared_ptr< GLClubField > spClub = GetMyClub();
	if ( spClub ) // Ŭ���� ���������� �п� ���� �Ұ���
	{
		MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_CLUB_ERROR;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if ( !pChar )
		return E_FAIL;

	DWORD wPreSchool = m_wSchool;

	if ( pNetMsg->wSchool >= 100 && pNetMsg->wSchool <= 102 && pChar->UserLevel() >= USER_GM3 )
	{
		switch( pNetMsg->wSchool )
		{
		case 100:
			m_wSchool = 0;
			break;
		case 101:
			m_wSchool = 1;
			break;
		case 102:
			m_wSchool = 2;
			break;
		}
	}
	else
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
			return E_FAIL;

		if ( pLand->IsChangeSchool() == false )
		{
			MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_MAP_ERROR;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
		
		SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !pINVENITEM )
		{
			MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
			return E_FAIL;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
		
		if ( !pITEM || pITEM->sBasicOp.emItemType < ITEM_SCHOOL_CHANGE_0 || pITEM->sBasicOp.emItemType > ITEM_SCHOOL_CHANGE_2 )
		{
			MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_ITEMTYPE;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		// Note : ���� ���� Ȯ��
		//
		if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
		{
			return E_FAIL;
		}

		// ���� �п� ������ wPreSchooldp �ӽ÷� ����
		switch( pITEM->sBasicOp.emItemType )
		{
		case ITEM_SCHOOL_CHANGE_0:
			m_wSchool = 0;
			break;
		case ITEM_SCHOOL_CHANGE_1:
			m_wSchool = 1;
			break;
		case ITEM_SCHOOL_CHANGE_2:
			m_wSchool = 2;
			break;
		}

		// Ŭ���̾�Ʈ ������ ��ȿ�� �˻�
		if ( pNetMsg->wSchool != m_wSchool )
		{
			MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_NOTVALUE;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		if ( wPreSchool == m_wSchool )
		{
			MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_SCHOOL_ERROR;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �п� ���� ��� ���� ����Ʈ ����
	bool bIsPlayRnattendanceQuest = false;
	//	�������� ����Ʈ ��� ����.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pPROG = ( *iter ).second;

			// �п� ����� ����Ʈ ������ �׽�Ʈ
			DWORD dwChangedID = 0;
			SNATIVEID sID = pPROG->m_sNID;
			int nRetMapping = pPROG->ConvertQuestGroup( wCurrentSchool, m_emClass, m_wSchool, dwChangedID );
			if ( nRetMapping == EMSCHOOL_CHANGE_ERROR )
			{
				// ����
				sc::writeLogError( sc::string::format( "[QUEST MAPPING ERROR] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			else if ( nRetMapping == EMSCHOOL_CHANGE_NOT_EXIST )
			{
				// ��Ī�� ����Ʈ�� ����
				sc::writeLogInfo( sc::string::format( "[QUEST MAPPING NOT FIND] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			// ��Ī�� ����Ʈ�� ���� ��쿡�� �ǵ������� �����ϵ��� ���� ����
			else if ( nRetMapping == EMSCHOOL_CHANGE_DISGARD )
			{
				vecDelQuestList.push_back( pPROG->m_sNID.dwID );
				sc::writeLogError( sc::string::format( "[QUEST MAPPING DELETE] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
				continue;
			}
			else
			{
				// �׷����� ��Ī�� ����Ʈ�� �����ϹǷ� ������
				sc::writeLogError( sc::string::format( "[QUEST MAPPING FIND] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}

			//bReset = ResetRnattendanceQuest(pPROG);
			bIsPlayRnattendanceQuest = IsPlayRnattendanceQuest( pPROG );

			se::ByteStream cByteStream;
			pPROG->SET_BYBUFFER( cByteStream );

			LPBYTE pBuff( NULL );
			DWORD dwSize( 0 );
			cByteStream.GetBuffer( pBuff, dwSize );

			if ( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNETLOBBY_QUEST_PROG NetMsg( pPROG->m_sNID.dwID );
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.SETSTREAM( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug( sc::string::format( "MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, ������) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
#endif

				SendToAgent( &NetMsg );
			}
			else
			{
				int nRepetIdx = ( dwSize / MAX_QUEST_PACKET_SIZE ) + ( ( dwSize % MAX_QUEST_PACKET_SIZE == 0 ) ? 0 : 1 );
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNETLOBBY_QUEST_PROG NetMsg( pPROG->m_sNID.dwID );
					NetMsg.dwSeq = i+1;
					NetMsg.dwChaNum = CharDbNum();

					if ( i == nRepetIdx-1 )
					{
						NetMsg.bLastSeq = TRUE;
						NetMsg.SETSTREAM ( pBuff + ( i * MAX_QUEST_PACKET_SIZE ), dwSize );
					}
					else
					{
						NetMsg.bLastSeq = FALSE;
						NetMsg.SETSTREAM ( pBuff + ( i * MAX_QUEST_PACKET_SIZE ) , MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug( sc::string::format( "MsgGameJoin (������) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
#endif

					SendToAgent( &NetMsg );
				}
			}
		}
	}

	bool bIsRnAttendanceCompleted = false;
	bool bIsEndRnattendanceQuest = false;

	//	�Ϸ�� ����Ʈ ��� ����.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestEnd().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pPROG = ( *iter ).second;
			
			// �п� ����� ����Ʈ ������ �׽�Ʈ
			DWORD dwChangedID = 0;
			SNATIVEID sID = pPROG->m_sNID;
			int nRetMapping = pPROG->ConvertQuestGroup( wCurrentSchool, m_emClass, m_wSchool, dwChangedID );
			if ( nRetMapping == EMSCHOOL_CHANGE_ERROR )
			{
				// ����
				sc::writeLogError( sc::string::format( "[QUEST MAPPING ERROR] MsgReqInvenSchoolChange - COMPLETE QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			else if ( nRetMapping == EMSCHOOL_CHANGE_NOT_EXIST )
			{
				// ��Ī�� ����Ʈ�� ����
				sc::writeLogInfo( sc::string::format( "[QUEST MAPPING NOT FIND] MsgReqInvenSchoolChange - COMPLETE QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			// ��Ī�� ����Ʈ�� ���� ��쿡�� �ǵ������� �����ϵ��� ���� ����
			else if ( nRetMapping == EMSCHOOL_CHANGE_DISGARD )
			{
				vecDelQuestList.push_back( pPROG->m_sNID.dwID );
				sc::writeLogError( sc::string::format( "[QUEST MAPPING DELETE] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
				continue;
			}
			else
			{
				// �׷����� ��Ī�� ����Ʈ�� �����ϹǷ� ������
				sc::writeLogError( sc::string::format( "[QUEST MAPPING FIND] MsgReqInvenSchoolChange - COMPLETE QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}			

			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find( pPROG->m_sNID.dwID );

			if ( pQUEST )
			{
				if ( pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
					bIsRnAttendanceCompleted = true;

				GLQuestMan::GetInstance().SetRnattendanceGQuestProg( pPROG );
			}
			bIsEndRnattendanceQuest = IsPlayRnattendanceQuest(pPROG);

			se::ByteStream cByteStream;
			pPROG->SET_BYBUFFER( cByteStream );

			LPBYTE pBuff( NULL );
			DWORD dwSize( 0 );
			cByteStream.GetBuffer( pBuff, dwSize );

			if ( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNETLOBBY_QUEST_END NetMsg( pPROG->m_sNID.dwID );
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.SETSTREAM( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug( sc::string::format( "MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, ����Ϸ�) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
#endif
				SendToAgent( &NetMsg );
			}
			else
			{
				int nRepetIdx = ( dwSize / MAX_QUEST_PACKET_SIZE ) + ( ( dwSize%MAX_QUEST_PACKET_SIZE == 0 ) ? 0 : 1 );
				
				for ( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNETLOBBY_QUEST_END NetMsg( pPROG->m_sNID.dwID );
					NetMsg.dwSeq = i+1;
					NetMsg.dwChaNum = CharDbNum();

					if ( i == nRepetIdx-1 )
					{
						NetMsg.bLastSeq = TRUE;
						NetMsg.SETSTREAM( pBuff + ( i * MAX_QUEST_PACKET_SIZE ), dwSize );
					}
					else
					{
						NetMsg.bLastSeq = FALSE;
						NetMsg.SETSTREAM( pBuff + ( i * MAX_QUEST_PACKET_SIZE ), MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug( sc::string::format( "MsgGameJoin (����Ϸ�) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
#endif

					SendToAgent( &NetMsg );
				}
			}
		}
	}
	if ( bIsRnAttendanceCompleted && !bIsPlayRnattendanceQuest && !bIsEndRnattendanceQuest )
	{
		DWORD dwQuestId = GLQuestMan::GetInstance().GetRnattendanceQuestId();
		GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindEnd( dwQuestId );
		if ( pQUEST_PROG )
		{
			CTime tStartTime( pQUEST_PROG->m_tStartTime );

			m_vecAttendanceSelectQuest.clear();
			m_vecAttendanceSelectQuest = GLQuestMan::GetInstance().FindAttendanceQuest( GETLEVEL(), 
				3, 
				static_cast< unsigned >( tStartTime.GetSecond() ) );

			GLMSG::SNETPC_RNATTENDANCE_QUEST_LIST_FC msg;
			{
				BOOST_FOREACH( DWORD vecAttendanceSelectQuest, m_vecAttendanceSelectQuest )
				{
					msg.vecQuestIDList.push_back( vecAttendanceSelectQuest );
				}
			}

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, msg );
			SendToClient( NET_MSG_RNATTENDANCE_QUEST_LIST_FC, PackBuffer);
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// �п� �������� ���ؼ� ������ ����Ʈ�� ������� �ʵ��� ����Ʈ ����Ʈ���� �����Ѵ�
	for( size_t idx = 0; idx < vecDelQuestList.size(); idx++ )
	{
		m_cQuestPlay.DeleteProc( vecDelQuestList[idx] );
	}

	vecDelQuestList.clear();

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterSchoolChangeSet( 
                m_CharDbNum,
                m_wSchool ) ) );

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.wSchool = m_wSchool;
	MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	LogCharAction(
	m_CharDbNum,		//	�����.
	gamelog::ACT_CHANGE_SCHOOL,	//	����.
	gamelog::ID_SCHOOL, ( ( wPreSchool + 1 ) * 10 ) + ( m_wSchool + 1 ),
	0,							//	exp
	0,							//	bright
	0,							//	life
	0,							//	money
	( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
	);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenChargedItem( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
        return E_FAIL;

	GLMSG::SNET_CHARGED_ITEM_GET* pPacket = ( GLMSG::SNET_CHARGED_ITEM_GET* ) nmg;

	GLMSG::SNET_CHARGED_ITEM_GET_FB NetMsgFB;

	MAPSHOP_ITER iter = m_mapCharged.find( pPacket->szPurKey );
	if ( iter == m_mapCharged.end() )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	SHOPPURCHASE& sPURCHASE = (*iter).second;
	
	SNATIVEID nidITEM( sPURCHASE.wItemMain, sPURCHASE.wItemSub );
	
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( nidITEM );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	WORD wPosX, wPosY;
	BOOL bOK = m_cInventory.FindInsertable( wPosX, wPosY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOINVEN;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ������ ������. DB Action.
	AddShopAdoJob(
        db::DbActionPtr(
            new db::PurchaseItem_Get(
                m_ClientSlot,
                m_dwUserID,
                pPacket->szPurKey,
                nidITEM, 
				pPacket->dwID ) ) );    
	return S_OK;
}

// *****************************************************
// Desc: �̸������û ó��
// *****************************************************
HRESULT GLChar::MsgReqInvenRename( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME* pNetMsg = ( GLMSG::SNETPC_INVEN_RENAME* ) nmg;

	GLMSG::SNETPC_INVEN_RENAME_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_RENAME )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}
	
	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// ĳ���͸�
	CString strChaName( pNetMsg->szName );
	strChaName.Trim(_T(" ")); // �յ� ��������

	// �����̽� ã��, ĳ�����̸� 4���� ���� ����, ĳ���� ���� �� ����.
	if ( ( strChaName.FindOneOf(" ") != -1 ) || ( strChaName.GetLength() < 4 ) ) 
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_LENGTH;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strChaName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strChaName );
	if ( bFILTER0 || bFILTER1 )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_BAD;
		SendToClient( &MsgFB );
		return S_FALSE;			
	}

	if ( m_ServiceProvider == SP_GS )
		strChaName = sc::string::utf8_to_ansi( std::string(strChaName.GetString()) ).c_str();

    if ( m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST )
    {
        if ( !sc::string::goodChaNameKorea( std::string( strChaName.GetString() ) ) )
        {
            MsgFB.emFB = EMINVEN_RENAME_FROM_DB_BAD;
            SendToClient( &MsgFB );
            return S_FALSE;	
        }
    }

	// �±��� ���� ���� üũ 
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString( strChaName ) )
	    {
		    MsgFB.emFB = EMINVEN_RENAME_FB_THAICHAR_ERROR;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }
    }

	// ��Ʈ�� ���� ���� üũ 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if ( STRUTIL::CheckVietnamString( strChaName ) )
	    {
		    MsgFB.emFB = EMINVEN_RENAME_FB_VNCHAR_ERROR;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }
    }

	// DB�� �����û
    AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterRename(
                m_ClientSlot, 
                m_CharDbNum,
				strChaName.GetString(),
				//pNetMsg->szName,
                pNetMsg->wPosX,
                pNetMsg->wPosY ) ) );	
	return S_OK;
}

// *****************************************************
// Desc: �̸����� ��� ó��
// *****************************************************
HRESULT GLChar::MsgInvenRename( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME_FROM_DB* pNetMsg = ( GLMSG::SNETPC_INVEN_RENAME_FROM_DB* )nmg;
	
	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	GLMSG::SNETPC_INVEN_RENAME_FB MsgFB;

	if ( pNetMsg->emFB == EMINVEN_RENAME_FROM_DB_BAD )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_BAD;
		StringCchCopy( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
		SendToClient( &MsgFB );
		return S_FALSE;
	}
	else if ( pNetMsg->emFB == EMINVEN_RENAME_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_FAIL;
		StringCchCopy( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	StringCchCopy( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
	MsgFB.emFB = EMINVEN_RENAME_FB_OK;
	SendToClient( &MsgFB );

	// �ֺ��� �̸������� �˸�.
	GLMSG::SNETPC_INVEN_RENAME_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy( NetMsgBrd.szName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround( &NetMsgBrd );

	// GLLandMan �̸��� ����.
	GLMSG::SNETPC_CHANGE_NAMEMAP NetMsgNameMap;
	NetMsgNameMap.dwGaeaID = m_dwGaeaID;
	StringCchCopy( NetMsgNameMap.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy( NetMsgNameMap.szNewName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround( &NetMsgNameMap );

	// �̸������� ģ���� Ŭ�� �ɹ��鿡�� �˸�
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();
	NetMsgAgt.dwID = m_dwGaeaID;
	StringCchCopy( NetMsgAgt.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy( NetMsgAgt.szNewName, CHAR_SZNAME, pNetMsg->szName );
	SendToAgent( &NetMsgAgt );

	m_pGaeaServer->ChangeNameMap( this, m_szName, pNetMsg->szName );

	//	[�ڽſ���] Inventory�� ������ ����.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

// *****************************************************
// Desc: ������ ���� ��û (��ü��)
// *****************************************************
HRESULT GLChar::MsgReqPostBoxOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_POSTBOX_OPEN* pNetMsg = ( GLMSG::SNETPC_POSTBOX_OPEN* ) nmg;

	// Note : �ش� ��ü���� ����� �� �ִ��� üũ
	if ( !IsPOSTBOXUSEABLE( EMPOSTBOXID_TYPE_NPC, pNetMsg->sPostBoxID ) )
	{
		GLMSG::SNETPC_POSTBOX_OPEN_FB MsgFB;
		MsgFB.emFB = EMINVEN_POSTBOX_OPEN_FB_FAIL;
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	GLMSG::SNETPC_POSTBOX_OPEN_FB MsgFB;
	MsgFB.emFB		 = EMINVEN_POSTBOX_OPEN_FB_SUCCESS;
	MsgFB.sPostBoxID = pNetMsg->sPostBoxID;
	SendToClient( &MsgFB );

	// Note : ������ ���̵� ���
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NPC;
	m_sPOSTBOXID	  = pNetMsg->sPostBoxID;

	return S_OK;
}


// *****************************************************
// Desc: ������ ���� ��û (ī��)
// *****************************************************
HRESULT GLChar::MsgReqPostBoxOpenCard( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD* pNetMsg = ( GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD* ) nmg;

	GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD_FB MsgFB;

	// Note : ������ ��� �Ұ����̸�
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		MsgFB.emFB = EMINVEN_POSTBOX_OPEN_CARD_FB_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_POSTBOX_CARD )
	{
		MsgFB.emFB = EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	MsgFB.emFB			 = EMINVEN_POSTBOX_OPEN_CARD_FB_SUCCESS;
	MsgFB.sPostBoxCardID = pINVENITEM->sItemCustom.GetNativeID();
	SendToClient( &MsgFB );

	// Note : ������ ���̵� ���
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_ITEM_CARD;
	m_sPOSTBOXID	  = pINVENITEM->sItemCustom.GetNativeID();

    //	[�ڽſ���] Inventory�� ������ ����.
    ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqPostBoxClose( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_POSTBOX_CLOSE* pNetMsg = ( GLMSG::SNETPC_POSTBOX_CLOSE* ) nmg;

	// Note : ������ ���̵� ����
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NONE;
	m_sPOSTBOXID	  = NATIVEID_NULL();

	return S_OK;
}

HRESULT GLChar::MsgReqSendSMS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SEND_SMS* pNetMsg = ( GLMSG::SNETPC_SEND_SMS* ) nmg;

	GLMSG::SNETPC_SEND_SMS_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wItemPosX, pNetMsg->wItemPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMSMS_SEND_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( pITEM->sBasicOp.emItemType != ITEM_SMS )
	{
		MsgFB.emFB = EMSMS_SEND_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// DB�� �����û
    AddGameAdoJob(
        db::DbActionPtr(
            new db::SendSMS(
                m_ClientSlot,
                m_CharDbNum,
                pNetMsg->dwReceiveChaNum,
                pNetMsg->szPhoneNumber,
                m_szName,
                pNetMsg->szSmsMsg,
                pNetMsg->wItemPosX,
                pNetMsg->wItemPosY ) ) );
	return S_OK;
}

// *****************************************************
// Desc: SMS ���� ��� ó��
// *****************************************************
HRESULT GLChar::MsgSendSMS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SEND_SMS_FROM_DB* pNetMsg = (GLMSG::SNETPC_SEND_SMS_FROM_DB *)nmg;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	GLMSG::SNETPC_SEND_SMS_FB MsgFB;

	if ( pNetMsg->emFB == EMSMS_SEND_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMSMS_SEND_FROM_DB_FAIL;
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	MsgFB.emFB = EMSMS_SEND_FB_OK;
	SendToClient( &MsgFB );

	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return TRUE;
}

HRESULT GLChar::MsgLoudSpeaker( NET_MSG_GENERIC* nmg )
{
    // NET_MSG_CHAT_LOUDSPEAKER
	// GLMSG::SNETPC_CHAT_LOUDSPEAKER* pNetMsg = (GLMSG::SNETPC_CHAT_LOUDSPEAKER *) nmg;
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::SNETPC_CHAT_LOUDSPEAKER RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
    {
        sc::writeLogError( "GLChar::MsgLoudSpeaker" );
        return E_FAIL;
    }

	GLLandManager* pLandManager = GetParentLandManager();
	if ( !pLandManager )
		return E_FAIL;

	GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB NetMsgFB;

	bool bchannel_all( false );

	if ( m_dwUserLvl < USER_USER_GM )
	{
		bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();
		if ( bActiveFactionChatting )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_BLOCK;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}

		SINVENITEM* pINVENITEM = m_cInventory.GetItem( RecvData.m_PosX, RecvData.m_PosY );
		if ( !pINVENITEM )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			SendToClient( &NetMsgFB );
			return S_OK;
		}

		if ( CheckCoolTime(pINVENITEM->sItemCustom.GetNativeID() ) )
            return E_FAIL;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
		if ( !pITEM )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			SendToClient( &NetMsgFB );
			return S_OK;
		}

		if ( pITEM->sBasicOp.emItemType != ITEM_LOUDSPEAKER )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			SendToClient( &NetMsgFB );
			return S_OK;
		}

		if ( IsCHATBLOCK() )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_BLOCK;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}

		bchannel_all = ( pINVENITEM->sItemCustom.IsChannel_All() != NULL );
		if ( m_pPrivateMarket->IsOpen() ) 
		{
			WORD wTurn = ( WORD ) m_cInventory.CountTurnItem( pINVENITEM->sItemCustom.GetNativeID() );
			WORD wMarketTurn = ( WORD )m_pPrivateMarket->GetItemTurnNum( pINVENITEM->sItemCustom.GetNativeID() );

			if ( wTurn <= wMarketTurn )
			{
				NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
				SendToClient( &NetMsgFB );
				return S_OK;
			}
		}

		//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
		ConsumeInvenItem( RecvData.m_PosX, RecvData.m_PosY );
	}
	else
	{
		// GM ä���� DB�� �����Ѵ�
		//std::string strSendName = sc::string::format( "%1%", GetName() );
		std::string strSendName( GetName() );
		AddLogAdoJob( db::DbActionPtr( new db::LogGMChat( CHAT_TYPE_LOUDSPEAKER, UserLevel(), strSendName, std::string( "System or Unknown" ), RecvData.m_Msg ) ) );
	}

	// Ȯ���� ���.
	//
	GLMSG::SNETPC_CHAT_LOUDSPEAKER_AGT NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();

	// ��ũ ������ ã�´�.
    BYTE LinkData[CHAT_LINK_DATA_SIZE] = {0};

	int nCnt = 0;
	for ( size_t i=0; i<RecvData.m_vLinkDataBasic.size(); ++i )
	{
		if (!RecvData.m_vLinkDataBasic[i].VALID())
			continue;			
		
		DWORD dwSize = GetChatLinkData( RecvData.m_vLinkDataBasic[i], LinkData, CHAT_LINK_DATA_SIZE );
		if ( dwSize > 0 ) 
		{
			if ( NetMsgAgt.ADD( LinkData, dwSize ) )
			{
				NetMsgAgt.sLinkDataRecv[nCnt].sLinkType = RecvData.m_vLinkDataBasic[i].sLinkType;
				NetMsgAgt.sLinkDataRecv[nCnt].sLinkPos = RecvData.m_vLinkDataBasic[i].sLinkPos;
				NetMsgAgt.sLinkDataRecv[nCnt].dwSize = dwSize;
				nCnt++;
			}
		}
	}

	StringCbCopy( NetMsgAgt.szMSG, CHAT_MSG_SIZE+1, RecvData.m_Msg.c_str() );
	NetMsgAgt.bchannel_all = bchannel_all;
	SendToAgent( &NetMsgAgt );

	return S_OK;
}

HRESULT GLChar::MsgChatPartyRecruitFB(EMCHAT_PARTY_RECRUIT emFB, float fPartyRecruitTime)
{
	switch (emFB) 
	{
	case EMCHAT_PARTY_RECRUIT_FAIL:
	case EMCHAT_PARTY_RECRUIT_OK:
	case EMCHAT_PARTY_RECRUIT_NOMONEY:
	case EMCHAT_PARTY_RECRUIT_BLOCK:
		break;

	// Agent��� �ð��� �����Ұ�� ó�����ش�. ���� �ٽ� �����ش�.
	case EMCHAT_PARTY_RECRUIT_TIME:
		{
			DWORD dwCharge = (DWORD)GLCONST_CHAR::dwCHAT_PARTY_RECRUIT_CHARGE;
			if (dwCharge > 0 )
			{
				CheckMoneyUpdate(GetInvenMoney(), dwCharge, TRUE, "CHAT_PARTY_RECRUIT_EX");

				AddInvenMoneyServer( dwCharge, ADD_INVEN_MONEY_PARTY_RECRUIT_CHAT_COST_BACK );

				SendInvenMoneyUpdate();
			}
		}
		break;	
	}

    GLMSG::SNETPC_CHAT_PARTY_RECRUIT_FB MsgFC;
    MsgFC.emFB = emFB;
    MsgFC.fPartyRecruitTime = fPartyRecruitTime;
	SendToClient(&MsgFC);

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildResult( NET_MSG_GENERIC* nmg )	// ITEMREBUILD_MARK
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNET_REBUILD_RESULT* pNetMsg = ( GLMSG::SNET_REBUILD_RESULT* )nmg;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqRebuildResult", m_CharDbNum ) );
	}

	switch( pNetMsg->emResult )
	{
	case EMREBUILD_RESULT_OPEN:		// ���� �������̽� ����
		{
			InitRebuildData();
			OpenRebuild();
			m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL;
		}
		break;

	case EMREBUILD_RESULT_CANCEL:
		{
			SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
			if ( pInvenItem )
			{
				pNetMsg->emResult = EMREBUILD_RESULT_SUCCESS;
				if ( EMTYPE_ITEM_REMODEL_RANDOMOPTION == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				else if ( EMTYPE_ITEM_REMODEL_ADDSTATS == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				else if ( EMTYPE_ITEM_REMODEL_BASICSTATS == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->wFixBasicStatsIndex != 0;
				else
					pNetMsg->bUseStampCard = false;
				pNetMsg->sItemCustom = pInvenItem->sItemCustom;
				SendToClient( pNetMsg );
			}

			InitRebuildData();
		}
		break;

		// ���� Ȯ�� ������;
	case EMREBUILD_RESULT_SUCCESS:
		{
			SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
			if ( pInvenItem )
			{
				if ( m_sBackUpRebuildInvenItem.sItemCustom.GetGuid() != pInvenItem->sItemCustom.GetGuid() )
				{
					pNetMsg->emResult = EMREBUILD_RESULT_FAIL;
					SendToClient( pNetMsg );

					return E_FAIL;
				}

				*pInvenItem = m_sBackUpRebuildInvenItem;

				// �ٷ� ��� �����Ѵ�;
				InvenUpdateItem( pInvenItem );

				if ( EMTYPE_ITEM_REMODEL_RANDOMOPTION == pNetMsg->emTypeRemodel )
				{
					// ����� �����ɼ��� �α׸� �����Ѵ�;
					LogItemRandomOption( pInvenItem->sItemCustom );
					LogItemConversion(
						pInvenItem->sItemCustom,
						gamelog::ID_CHAR,
						m_CharDbNum,
						MONEY_ROUTE_NPC_REMAKE,
						LOG_ITEM_REMODEL_RESULT );
				}

				// ���� �Ǿ����Ƿ� ������ ������ �����ؾ��Ѵ�;
				GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
				NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
				NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
				NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
				SendToClient( &NetItemUpdate );

				// ���� �����ߴ�;
				pNetMsg->emResult = EMREBUILD_RESULT_SUCCESS;
				if ( EMTYPE_ITEM_REMODEL_RANDOMOPTION == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				if ( EMTYPE_ITEM_REMODEL_ADDSTATS == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				else if ( EMTYPE_ITEM_REMODEL_BASICSTATS == pNetMsg->emTypeRemodel )
					pNetMsg->bUseStampCard = pNetMsg->wFixBasicStatsIndex != 0;
				else
					pNetMsg->bUseStampCard = false;
				pNetMsg->sItemCustom = pInvenItem->sItemCustom;
				SendToClient( pNetMsg );
			}

			InitRebuildData();
		}
		break;
	case EMREBUILD_CHANGE_REBUILDTYPE_KEEPORIGIN_CACHERESULT:
	case EMREBUILD_CHANGE_REBUILDTYPE_APPLYRESULT_CACHEORIGINAL:
		{
			m_sBackUpRebuildInvenItem = SINVENITEM();
			if ( pNetMsg->emResult == EMREBUILD_CHANGE_REBUILDTYPE_KEEPORIGIN_CACHERESULT )
				m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT;
			else
				m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL;

			SendToClient( pNetMsg );
			return S_OK;
		}
		break;

	case EMREBUILD_TRY_SELECT_INTO_CACHE:
		{
			if ( m_wRebuildType != EMREBUILDTYPE::EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL &&
				m_wRebuildType != EMREBUILDTYPE::EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
				return E_FAIL;

			// �׽�Ʈ ����;
			if ( m_sBackUpRebuildInvenItem != SINVENITEM() )
			{
				SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
				if ( pInvenItem )
				{
					if ( m_sBackUpRebuildInvenItem.sItemCustom.GetGuid() != pInvenItem->sItemCustom.GetGuid() )
					{
						pNetMsg->emResult = EMREBUILD_RESULT_SELECT_INTO_CACHE_FAIL;
						SendToClient( pNetMsg );
						return E_FAIL;
					}

					pInvenItem->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
					m_sBackUpRebuildInvenItem = SINVENITEM();

					InvenUpdateItem( pInvenItem );

					switch ( pNetMsg->emTypeRemodel )
					{
					case EMTYPE_ITEM_REMODEL_RANDOMOPTION:
						{
							LogItemRandomOption( pInvenItem->sItemCustom );
							LogItemConversion(
								pInvenItem->sItemCustom,
								gamelog::ID_CHAR,
								m_CharDbNum,
								MONEY_ROUTE_NPC_REMAKE,
								LOG_ITEM_REMODEL_SELECT_RESULT );
						}
						break;
					case EMTYPE_ITEM_REMODEL_BASICSTATS:
						{
							LogItemBasicStatOption( pInvenItem->sItemCustom );
						}
						break;
					case EMTYPE_ITEM_REMODEL_ADDSTATS:
						{
							LogItemAddonOption( pInvenItem->sItemCustom );
						}
						break;
					case EMTYPE_ITEM_REMODEL_ITEMSKILL:
						{
							LogItemLinkSkillOption( pInvenItem->sItemCustom );
						}
						break;
					}

					// ���� �Ǿ����Ƿ� ������ ������ �����ؾ��Ѵ�;
					GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
					NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
					NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
					NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
					SendToClient( &NetItemUpdate );

					pNetMsg->emResult = EMREBUILD_RESULT_SELECT_INTO_CACHE_OK;
					pNetMsg->sItemCustom = SITEMCUSTOM();
				}
			}
			else
			{
				pNetMsg->emResult = EMREBUILD_RESULT_SELECT_INTO_CACHE_FAIL;
			}
			SendToClient( pNetMsg );
		}
		break;
	case EMREBUILD_RESULT_TRY:
		{
			// �ݾ��� ���� �ʾƼ� ������ ������ �� ����;
			if ( m_i64RebuildInput != m_i64RebuildCost || GetInvenMoney() < m_i64RebuildCost )
			{
				pNetMsg->emResult = EMREBUILD_RESULT_MONEY;
				SendToClient( pNetMsg );
				return S_OK;
			}

			// ����â�� ������ �ø��� ���������Ƿ� �̷� ���� �߻����� ���ƾ��Ѵ�;
			// http://172.16.2.87/redmine/issues/832
			SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
			if ( sRebuildItem.IsBoxWrapped() )
				return E_FAIL;

			const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
			if (NULL == pItem)
				return E_FAIL;

			// ���� �����۸� ������ �����ϴ�;
			if (ITEM_SUIT != pItem->sBasicOp.emItemType)
				return E_FAIL;

			switch ( pNetMsg->emTypeRemodel )
			{
			case EMTYPE_ITEM_REMODEL_NONE:	return E_FAIL;
			case EMTYPE_ITEM_REMODEL_RANDOMOPTION:	
				if ( FAILED( this->_RebuildRandomOption(pNetMsg) ) )
					return E_FAIL;
				break;
			case EMTYPE_ITEM_REMODEL_ADDSTATS:
				if ( FAILED( this->_RebuildAddon(pNetMsg) ) )
					return E_FAIL;
				break;
			case EMTYPE_ITEM_REMODEL_BASICSTATS:
				if ( FAILED( this->_RebuildBasicStats(pNetMsg) ) )
					return E_FAIL;
				break;
			case EMTYPE_ITEM_REMODEL_ITEMSKILL:
				if ( FAILED( this->_RebuildLinkSkill(pNetMsg) ) )
					return E_FAIL;
				break;
			}
		}
		break;

	case EMREBUILD_RESULT_FAIL:		// �̷� ��Ŷ�� �� �� ����
	case EMREBUILD_RESULT_DESTROY:	// �̷� ��Ŷ�� �� �� ����
	case EMREBUILD_RESULT_MONEY:	// �̷� ��Ŷ�� �� �� ����
	case EMREBUILD_RESULT_CLOSE:	// ���� �������̽� �ݱ�
		{
			InitRebuildData();
			CloseRebuild();

			// ����ī�� �������� ��ġ�� �ʱ�ȭ ��Ų��.
			m_ItemRemodelPosX = EInvenX;
			m_ItemRemodelPosY = EInvenTotalLine;
		}
		break;
	}

	return S_OK;
}

HRESULT GLChar::MsgReqGarbageResult( NET_MSG_GENERIC* nmg )	// ITEMREBUILD_MARK
{
	GLMSG::SNET_GARBAGE_RESULT* pNetMsg = ( GLMSG::SNET_GARBAGE_RESULT* )nmg;

	GLLandMan* const pLand( GetLandMan() );
	if ( !pLand )
	{
		return E_FAIL;
	}

	GLMSG::SNET_GARBAGE_RESULT_FB MsgFB;
	SINVENITEM* pINVENITEM_DEL = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM_DEL )
	{
		MsgFB.emResult = EMGARBAGE_RESULT_FB_FAIL;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM_DEL->sItemCustom.GetNativeID() );
	if ( !pItem || !( pINVENITEM_DEL->sItemCustom.IsGarbage() != NULL ) )
	{
		MsgFB.emResult = EMGARBAGE_RESULT_FB_ITEMTYPE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// ��ī���̸鼭 ���� Ȱ��ȭ �Ǿ� ������ �����Ҽ� ����.
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pINVENITEM_DEL->sItemCustom.PetDbNum() == pMyPet->DbNum() )
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_FAIL;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqGarbageResult ", m_CharDbNum ) );
	}

	BYTE nType = 0;
	LONGLONG nCost = 0;
	if ( !IsValidGarbageUse( pItem->BasicId(), nType, nCost ) )
	{
		if ( nType == EGarbageMoney ||
			nType == EGarbagePoint )
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_MONEY;
		}
		else
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_NOITEM;
		}
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	switch ( nType )
	{
	case EGarbageCard:
		{
			SINVENITEM* pINVENITEM = m_cInventory.GetItem( m_ItemGarbagePosX, m_ItemGarbagePosY );
			if ( !pINVENITEM )
				return E_FAIL;

			const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
			if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_GARBAGE_CARD )
			{
				return E_FAIL;
			}

			// Note : ���� ���� Ȯ��
			//
			if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
			{
				return E_FAIL;
			}

			ConsumeInvenItem( m_ItemGarbagePosX, m_ItemGarbagePosY );

			m_ItemGarbagePosX = EInvenX;
			m_ItemGarbagePosY = EInvenTotalLine;
		}
		break;

	case EGarbageMoney:
		{
			CheckMoneyUpdate( GetInvenMoney(), nCost, FALSE, "Garbage Cost" );

			if ( GetInvenMoney() >= nCost )
				SubtractInvenMoneyServer( nCost, SUBTRACT_INVEN_MONEY_ITEM_GARBAGE );
			else
				SetInvenMoney( 0 );

			// �ݾ� ��ȭ �α�
			// ��� �� �α� ����
			LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, -nCost, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_GARBAGE );
			// ��� �� �α� ����
			LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

			// �ݾ� ��ȭ �˸�.
			SendInvenMoneyUpdate();
		}
		break;

	case EGarbagePoint:
		{
			PointShopPointSub( NATIVEID_NULL(), nCost,  pointshop::POINT_SUB_TYPE_GRABAGE );
		}
		break;

	default:
		return E_FAIL;
	}

	// ��� �� �α� ����
	LogItemExchange(
		pINVENITEM_DEL->sItemCustom,
		gamelog::ID_CHAR, m_CharDbNum,
		gamelog::ID_CHAR, m_CharDbNum,
		ITEM_ROUTE_GARBAGE, pINVENITEM_DEL->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_TO_GARBAGE,
		false );

	// �� ī�� �ϰ��
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pINVENITEM_DEL->sItemCustom.PetDbNum() != 0 )
	{
		PGLPETFIELD pMyPet = GetMyPet();

		if ( pMyPet && pINVENITEM_DEL->sItemCustom.PetDbNum() == pMyPet->DbNum() ) 
		{
			m_pGaeaServer->ReserveDropOutPet( SDROPOUTPETINFO( pMyPet->m_dwGUID, true, false ) );

			// ���� �Ǽ��縮�� �˻��Ѵ�.
			pMyPet->UpdateTimeLmtItem( this );

			// �־��ٲ� �־��ش�.
			for ( WORD i = 0; i < ACCETYPESIZE; ++i )
			{
				if ( pMyPet->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
				{
					continue;
				}

				CItemDrop cDropItem;
				cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
				if ( IsInsertToInven ( &cDropItem ) )
				{
					InsertToInven( &cDropItem, false, false, false );
				}
				else
				{
					// �ٴڿ� ����߸���.
					// ������ ���°� �ʱ�����̰ų� �������ؾ��ϴ� ���¶�� ������ ������Ÿ������ ��� �켱 �����Ѵ�.
					// �׿� ��������Ʈ ���¶�� ��� �κ�Ÿ�Ը� �����Ѵ�.
					// ���߿� ���忡�� ������� �ȴٸ�(�ƹ��� ���� �ʰ�) ���� ���� Ÿ������ ������Ʈ �ǰ�(���忡 �۾� �ʿ�)
					// ������ �ݴ´ٸ� �ڱ� ���������� ������Ʈ�ȴ�.
					DropItem( cDropItem.sItemCustom, m_vPosition );
				}
			}

			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetDelete( m_CharDbNum, pINVENITEM_DEL->sItemCustom.PetDbNum() ) ) );					
			
			// ���� �����Ǹ� ��Ȱ������ Ŭ���̾�Ʈ�� �˷���.
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetListRestoreGet( m_CharDbNum, m_ClientSlot ) ) );
		}
		else
		{
			// new �Ѱ� ���ƿͼ� �� �����ǳ�? �ð��� ������ ���� �ڵ忡�� �̰Ŷ� ����Ѱ� ���� ������
			// �Ǽ��縮�� üũ�� ���� DB���� �������ش�.
			GLPetField* pNewPet = new GLPetField( m_pGaeaServer );
            AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetGet(
                        pNewPet, 
                        pINVENITEM_DEL->sItemCustom.PetDbNum(), 
                        m_ClientSlot, 
                        m_CharDbNum,
                        pINVENITEM_DEL->wPosX,
                        pINVENITEM_DEL->wPosY,
                        true ) ) );
		}
	}

	// Ż���� ���
	if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && pINVENITEM_DEL->sItemCustom.m_VehicleDbNum != 0 )
	{
		// ���� ���� Ȱ��ȭ�Ǿ� �ִ��� Ȯ���ϴ� �ڵ尡 ��� �ְ�, ���� �����۵� ó�����ش�. Ż���� �����ִµ� �ǵ��Ȱǰ�����? ����������?
		// Ȯ���غ��� ���׼ǿ��� ���ƿͼ� ���� ��ó�� ó���ȴ�.
		// new �Ѱ� ���ƿͼ� �� �����ǳ�? �ð��� ������ ���� �ڵ忡�� �̰Ŷ� ����Ѱ� ���� ������
		// �Ǽ��縮�� üũ�� Ż���� DB���� �������ش�.
		GLVEHICLE* pNewVehicle = new GLVEHICLE;
        AddGameAdoJob(
            db::DbActionPtr(
                new CGetVehicle(
                    pNewVehicle, 
                    pINVENITEM_DEL->sItemCustom.m_VehicleDbNum, 
                    m_ClientSlot,
                    m_CharDbNum, 
                    true ) ) );
	}

	// �κ��丮���� ������ ����
    InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, DELETE_GARBAGE_RESULT, true );

	MsgFB.emResult = EMGARBAGE_RESULT_FB_OK;
	SendToClient( &MsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildMoveItem( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_REBUILD_MOVE_ITEM* pNetMsg = ( GLMSG::SNET_REBUILD_MOVE_ITEM* )nmg;
	GLMSG::SNET_REBUILD_MOVE_ITEM NetMsg;	

	if ( pNetMsg->wPosX != USHRT_MAX && pNetMsg->wPosY != USHRT_MAX )
	{
		SINVENITEM* pResistItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( pResistItem )
		{
			// ���� �������̾�� �ٲ� �� �ִ�
			const SITEM* pItem = GLogicData::GetInstance().GetItem( pResistItem->sItemCustom.GetNativeID() );
			if ( pItem != NULL )
			{
				switch ( pItem->sBasicOp.emItemType )
				{
				case ITEM_SUIT:					
					if ( strlen( pItem->sRandomOpt.szNAME ) < 4 )
						break; // �����ɼ� ������ �����Ǿ� �־�� �����ϴ�;

					NetMsg.wPosX = pNetMsg->wPosX;
					NetMsg.wPosY = pNetMsg->wPosY;

					m_sRebuildItem.SET( NetMsg.wPosX, NetMsg.wPosY );
					break;
				case ITEM_STAMP_CARD:
					NetMsg.emItemType = GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP;
					NetMsg.wPosX = pNetMsg->wPosX;
					NetMsg.wPosY = pNetMsg->wPosY;
					m_ItemStampPosX = pNetMsg->wPosX;
					m_ItemStampPosY = pNetMsg->wPosY;
					_SetStampInfo(pResistItem);

					m_sStampItem.SET( NetMsg.wPosX, NetMsg.wPosY );
					break;
				}
			}			
		}
	}
	else
	{
		switch ( pNetMsg->emItemType )
		{
		case GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_EQUIPMENT:
			m_sRebuildItem.RESET();
			break;
		case GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP:
			NetMsg.emItemType = GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP;
			m_sStampItem.RESET();
			break;
		}		
	}

	SendToClient( &NetMsg );

	// ���� ��� ����
	MsgReqRebuildCostMoney();

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildCostMoney()
{
	GLMSG::SNET_REBUILD_COST_MONEY NetMsg;

	// ���� �������� �����Ƿ� 0�� �����Ѵ�
	if ( !m_sRebuildItem.VALID() )
	{
		SendToClient( &NetMsg );
		return S_OK;
	}

	// ����â�� ������ �ø��� ���������Ƿ� �̷� ���� �߻����� ���ƾ��Ѵ�
	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	if ( !pItem )
	{
		SendToClient( &NetMsg );
		return S_OK;
	}

	// ����â�� ������ �ø��� ���������Ƿ� �̷� ���� �߻����� ���ƾ��Ѵ�
	RandomOption::Entity* const _pSetRandomOption( GLItemMan::GetInstance().GetItemRandomOpt( sRebuildItem.GetNativeID() ) );
	if ( _pSetRandomOption == NULL )
	{
		SendToClient( &NetMsg );
		return S_OK;
	}

	m_i64RebuildCost = ( LONGLONG )( pItem->sBasicOp.dwBuyPrice * double( _pSetRandomOption->getRateMoneySpend() ) );

	NetMsg.i64CostMoney = m_i64RebuildCost;

	SendToClient( &NetMsg );

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildInputMoney( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_REBUILD_INPUT_MONEY* pNetMsg = ( GLMSG::SNET_REBUILD_INPUT_MONEY* )nmg;

	m_i64RebuildInput = ( LONGLONG )max( 0, pNetMsg->i64InputMoney );
	m_i64RebuildInput = ( LONGLONG )min( m_i64RebuildInput, GetInvenMoney() );

	pNetMsg->i64InputMoney = m_i64RebuildInput;

	SendToClient( pNetMsg );

	return S_OK;
}

HRESULT	GLChar::MsgGiveFood( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GIVEFOOD* pNetMsg = ( GLMSG::SNETPET_REQ_GIVEFOOD* ) nmg;
	
	SNATIVEID sNativeID;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) 
	{
		return E_FAIL;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem ) 
	{
		return E_FAIL;
	}

	// ��ī�� ���� üũ
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )	
	{
		return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pHold ) 
	{
		return E_FAIL;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) 
		return E_FAIL;

	// ��� ���� üũ
	if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD )	
	{
		return E_FAIL;
	}

    WORD wHoldPosX = pHoldInvenItem->wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = pHoldInvenItem->wPosY;

	sNativeID = pHold->sBasicOp.sNativeID;
	switch ( pHold->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		{
			PGLPETFIELD pMyPet = GetMyPet();

			// Logic �� ���� �ִٸ�
			if ( pMyPet && pMyPet->DbNum() == pInvenItem->sItemCustom.PetDbNum() )
			{
				// �� ���Ա� ���� �α�
				LogPetAction(
                    pMyPet->DbNum(), 
                    pInvenItem->sItemCustom.GetNativeID(), 
                    pet::EM_ACTION_FOOD_BEFORE, 
                    pMyPet->Satiation() );

				bool bOK = pMyPet->IncreaseFull( pHold->sDrugOp.dwCureVolume, pHold->sDrugOp.bRatio );
				if ( !bOK )
                    return E_FAIL;
				
				if ( pMyPet->IsValid() )
                    pMyPet->m_fTIMER = 0.0f;

                ConsumeInvenItem( wHoldPosX, wHoldPosY );

				// �� ���Ա� ���� �α�
				LogPetAction(
                    pMyPet->DbNum(), 
                    pInvenItem->sItemCustom.GetNativeID(), 
                    pet::EM_ACTION_FOOD_AFTER, 
                    pMyPet->Satiation() );

				// �ֿ��� ���̸� �ش�.
                GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB( pMyPet->DbNum(), sNativeID, pMyPet->Satiation(), EMPET_REQ_GIVEFOOD_FB_OK );
				SendToClient( &NetMsgFB );
				if ( GLHIDESET::CheckHideSet( EMHIDE_USE_PET_CHANGE ) )
				{
					RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
					RestoreActStateByInvisible();
				}
			}
			// ���� Logic �� ���� ���ٸ�
			else
			{
				// �� �������� Ȯ��
				if ( pInvenItem->sItemCustom.PetDbNum() <= 0 )
				{
					sc::writeLogError( sc::string::format( "Invalid ID, PetID %1% dwClientID %2%", pInvenItem->sItemCustom.PetDbNum(), m_ClientSlot));
					return E_FAIL;
				}

				// ���� �������� �����ͼ� �����ϰ� �ٽ� DB�� �����ϴ� �ڵ�
                AddGameAdoJob(
                    db::DbActionPtr(
                        new db::PetFullGet(
                            m_CharDbNum, 
                            pInvenItem->sItemCustom.PetDbNum(), 
                            m_ClientSlot,
                            pHold->sDrugOp.dwCureVolume, 
                            pHold->sDrugOp.bRatio,
                            sNativeID,
                            pNetMsg->emPetTYPE,
                            wHoldPosX,
                            wHoldPosY ) ) );				
			}
		}
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT GLChar::MsgGetPetFullFromDB( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB* pIntMsg = ( GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB* ) nmg;
	switch ( pIntMsg->emFB )
	{
	case EMPET_REQ_GETFULL_FROMDB_FAIL:
		{
			GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB( 0, NATIVEID_NULL(), 0, EMPET_REQ_GETFULL_FROMDB_FAIL );
			SendToClient( &NetMsgFB );
		}
		break;

	case EMPET_REQ_GETFULL_FROMDB_OK:
		{
			//	�տ� �� ������ Ȯ���Ѵ�.
			//	�������� �ٲ�� ��찡 �ִ�.
            SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pIntMsg->wHoldPosX, pIntMsg->wHoldPosY );
            if ( !pHoldInvenItem )
                return E_FAIL;

            SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
			if ( !pHold ) 
			{
				sc::writeLogError(sc::string::format("Pet Give Full Error CharID : %d", m_CharDbNum));				
				return E_FAIL;
			}

			if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD || pHold->sBasicOp.sNativeID != pIntMsg->sNativeID ) 
			{
				sc::writeLogError( sc::string::format( "Pet Give Full Error CharID : %d, Item %d/%d", m_CharDbNum, pHold->sBasicOp.sNativeID.wMainID, pHold->sBasicOp.sNativeID.wSubID ) );
				return E_FAIL;
			}

			// �� ������ ���� �α�
			LogPetAction(
                pIntMsg->dwPetID, 
                pIntMsg->sNativeID,
                pet::EM_ACTION_FOOD_BEFORE, 
                pIntMsg->nFull );

			int nFull = pIntMsg->nFull;
			int nMaxFull = GLCONST_PET::pGLPET[pIntMsg->emType]->m_nFull;
			if ( nFull >= nMaxFull ) break;

			if ( pIntMsg->bRatio )
			{
				nFull += ( nMaxFull * pIntMsg->wCureVolume ) / 100;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}
			else
			{
				nFull += pIntMsg->wCureVolume;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}

			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetFullSet( m_CharDbNum, pIntMsg->dwPetID, nFull ) ) );

			ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

			// �� ������ ���� �α�
			LogPetAction(
                pIntMsg->dwPetID,
                pIntMsg->sNativeID,
                pet::EM_ACTION_FOOD_AFTER,
                nFull );

			GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB( pIntMsg->dwPetID, pIntMsg->sNativeID, nFull, EMPET_REQ_GIVEFOOD_FB_OK );
			SendToClient( &NetMsgFB );

			if ( GLHIDESET::CheckHideSet( EMHIDE_USE_PET_CHANGE ) )
			{
				RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
				RestoreActStateByInvisible();
			}
		}
	}

	return S_OK;
}

void GLChar::CallbackProduct()
{
	if ( m_pProductField == NULL )
	{
		sc::writeLogError( std::string("- Make Product Item : m_pProductField is NULL") );
		return;
	}

    GLMSG::SNET_PRODUCT_FB NetMsgFb;
    NetMsgFb.dwChaDbNum = CharDbNum();
    NetMsgFb.nKey = m_pProductField->GetCurrentProductRecipeKey();
    NetMsgFb.nCount = m_pProductField->GetProductCount();
	NetMsgFb.dwCharGaeaID = GetGaeaID();

    if ( m_fMotionSec <= 0.0f && IsValidBody() )
    {
        // ���� ����

        const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( m_pProductField->GetCurrentProductRecipeKey() );
        if ( !pProductRecipe )
		{
			sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateERROR - Count : %1%", m_pProductField->m_nProductCurrentCount+1 ) );
			NetMsgFb.nState = Product::EStateERROR;
			SendToClient( &NetMsgFb );

			// ��� ���� �� ����ϴ� ���̹Ƿ� �������� �˸�
			SendMsgViewAround( &NetMsgFb );
			return;
		}
            
        if ( m_pProductField->CheckProduct_FieldServer( pProductRecipe, this, NetMsgFb.nState ) == false )
        {
			sc::writeLogInfo( sc::string::format("- Make Product Item : Cancel CheckProduct_FieldServer - Count : %1%", m_pProductField->m_nProductCurrentCount+1 ) );
            SendToClient( &NetMsgFb );

            // ��� ���� �� ����ϴ� ���̹Ƿ� �������� �˸�
            SendMsgViewAround( &NetMsgFb );
            return;
        }

		// Agent�� ������ ��Ŷ�� ���� �и��Ѵ�;
		GLMSG::SNET_PRODUCT_FA NetMsgFA;
		NetMsgFA.dwChaDbNum = CharDbNum();
		NetMsgFA.nKey = m_pProductField->GetCurrentProductRecipeKey();
		NetMsgFA.nCount = m_pProductField->GetProductCount();
		NetMsgFA.llMoney = GetInvenMoney();

        m_pProductField->DoProductMeterial( m_pProductField->GetCurrentProductRecipeKey(), this );
        bool bSuccess = m_pProductField->DoProductResult( m_pProductField->GetCurrentProductRecipeKey(), this, &NetMsgFA );

		NetMsgFb.nState = bSuccess ? Product::EStateSuccess : Product::EStateRateFail;
		if ( bSuccess == false )
		{
			NetMsgFb.nState = Product::EStateRateFail;
			SendToClient( &NetMsgFb );
		}

        if ( ++m_pProductField->m_nProductCurrentCount < m_pProductField->GetProductCount() )
        {
			bool bNeedConfirm = true;

			// Ȯ���ؾ� �� ��ᰡ �ִ��� Ȯ���Ѵ�;
			bNeedConfirm = m_pProductField->NeedConfirmProductMaterial( m_pProductField->GetCurrentProductRecipeKey(), this );
			if ( bNeedConfirm )
			{
				sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateNeedConfirm - Count : %1%, %2%", m_pProductField->m_nProductCurrentCount+1, m_pProductField->GetProductCount() ) );
				m_pProductField->m_bConfirmMaterial = true;

				NetMsgFb.nState = Product::EStateNeedConfirm;
				SendToClient( &NetMsgFb );

				return;
			}
			else
			{
				sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateOngoingMotion - Count : %1%, %2%", m_pProductField->m_nProductCurrentCount+1, m_pProductField->GetProductCount() ) );
				NetMsgFb.nState = Product::EStateOngoingMotion;
				SendToClient( &NetMsgFb );

				// ���� ĳ���Ϳ��� ���� ���� ���ΰ��� ���̵��� �˸�;
				SendMsgViewAround( &NetMsgFb );
			}

            // ���;
            SetMotion(
                EMANI_MAINTYPE( GLCONST_CHAR::sProductMotionID.wMainID ),
                EMANI_SUBTYPE( GLCONST_CHAR::sProductMotionID.wSubID ),
                pProductRecipe->fDelaySec,
                boost::bind( boost::mem_fn( &GLChar::CallbackProduct ), this ) );
        }
        else
        {
			sc::writeLogInfo( sc::string::format("- Make Product Item : RESET Product::EStateComplete - Count : %1%, %2%", m_pProductField->m_nProductCurrentCount+1, m_pProductField->GetProductCount() ) );
			
			// ������ �ʱ�ȭ�Ѵ�;
			m_pProductField->Reset();

            NetMsgFb.nState = Product::EStateComplete;
            SendToClient( &NetMsgFb );

            TurnAction( GLAT_IDLE );
        }
    }
    else
    {
		m_pProductField->CancleProduct(this);
        // ���
		sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateCancel - Count : %1%, Sec : %2%, Vaild : %3%", m_pProductField->m_nProductCurrentCount+1, m_fMotionSec, IsValidBody() ) );
        NetMsgFb.nKey = UINT_MAX;
        NetMsgFb.nState = Product::EStateCancel;
        SendToClient( &NetMsgFb );

        // ��� ���� �� ����ϴ� ���̹Ƿ� �������� �˸�
        SendMsgViewAround( &NetMsgFb );
    }
}

//hsshin ���ۼ��õ� �߰��� ���� �޽��� ó���κ� ���� (���������� �۾��ڰ� ���ð� �ٽ� ���� ���ּ���)
HRESULT GLChar::MsgReqItemMix( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_ITEM_MIX* pPacket = ( GLMSG::SNET_INVEN_ITEM_MIX* ) nmg;
	GLMSG::NET_INVEN_ITEM_MIX_FC NetMsgFB;

	SINVENITEM* pInvenItem[ITEMMIX_ITEMNUM];
	memset( pInvenItem, 0, sizeof( SINVENITEM* ) * ITEMMIX_ITEMNUM );

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	// NPCȮ��/NPC üũ
	GLCrow* pCrow = pLand->GetCrow( pPacket->dwNpcGlobID );
	if ( !pCrow )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NONPC );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_ITEM_MIX ) )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NONPC );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ���� Ȯ��
	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( pPacket->dwKey );
	if ( !pProductRecipe ) 
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOMIX );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	�ݾ� Ȯ�� 
	if ( GetInvenMoney() < pProductRecipe->dwPrice )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOMONEY );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// �κ� ������ Ȯ��
	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		if ( !pPacket->sInvenPos[i].VALID() )
			continue;

		pInvenItem[i] = m_cInventory.GetItem( pPacket->sInvenPos[i].wPosX, pPacket->sInvenPos[i].wPosY );
		if ( !pInvenItem[i] )
		{
			NetMsgFB.SetResult( EMITEM_MIX_FB_NOITEM );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem[i]->sItemCustom.GetNativeID() );
		if ( !pItem ) 
		{
			NetMsgFB.SetResult( EMITEM_MIX_FB_NOITEM );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}
	}

	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i ) 
	{
		if ( pProductRecipe->sMeterialItem[i].sNID == NATIVEID_NULL() )
			continue;

		if ( !pInvenItem[i] )
		{
			NetMsgFB.SetResult( EMITEM_MIX_FB_NOMIXNUM );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// MID/SID�� ������ üũ�ؾߵȴ�.
		if ( pProductRecipe->sMeterialItem[i].sNID != pInvenItem[i]->sItemCustom.GetNativeID() )
		{
			NetMsgFB.SetResult( EMITEM_MIX_FB_NOMIXNUM );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		if ( pProductRecipe->sMeterialItem[i].nNum != pInvenItem[i]->sItemCustom.wTurnNum )
		{
			NetMsgFB.SetResult( EMITEM_MIX_FB_NOMIXNUM );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}	
	}

	//	��� ������ üũ
	const SITEM* pItemResult = GLogicData::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
	if ( !pItemResult )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOITEM );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqItemMix", m_CharDbNum ) );
	}

	WORD wInsertPosX = 0;
	WORD wInsertPosY = 0;
	BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		// �Ұ����� ��� �Դϴ�.
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOINVEN );
		SendToClient( &NetMsgFB );
		return E_FAIL;	//	�κ� ���� �������� �ǵ��� ����.
	}	

	//	���� ������ ����
	for (int i=0; i<ITEMMIX_ITEMNUM; ++i)
	{
		if ( !pPacket->sInvenPos[i].VALID() )
			continue;

		//  ������ Ÿ�Կ� ������� ������ �����.
		// ��� �� �α� ����
		LogItemExchange( 
			pInvenItem[i]->sItemCustom, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, 0, 
			ITEM_ROUTE_ITEM_MIX, pInvenItem[i]->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MIX_DEL_ITEM,
			true );

		InvenDeleteItem( pInvenItem[i]->wPosX, pInvenItem[i]->wPosY, true, FALSE, 1, true );
	}

	//	������ 
	if ( pProductRecipe->dwPrice > 0 )
	{
		CheckMoneyUpdate( GetInvenMoney(), pProductRecipe->dwPrice, FALSE, "ITEM_MIX" );

		SubtractInvenMoneyServer( pProductRecipe->dwPrice, SUBTRACT_INVEN_MONEY_ITEM_MIX );

		LONGLONG MixPrice = static_cast< LONGLONG >( pProductRecipe->dwPrice );
		// ��� �� �α� ����
		LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -MixPrice, ITEM_ROUTE_ITEM_MIX, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MIX_CHARGE );
		// ��� �� �α� ����
		LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );        

		SendInvenMoneyUpdate();
	}

	//	�������� ���
	float fRate = ( float )pProductRecipe->dwRate;
	if ( !sc::Random::getInstance().IsSucceed( fRate ) )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_FAIL );
		SendToClient( &NetMsgFB );
		return S_OK;	
	}

	//	��� ������ ����
	SITEMCUSTOM sITEM_NEW( pProductRecipe->sResultItem.sNID );

	sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
	sITEM_NEW.wTurnNum = pProductRecipe->sResultItem.nNum;
	sITEM_NEW.cGenType = EMGEN_ITEMMIX;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	LogItemBasicStatOption( sITEM_NEW );
	LogItemLinkSkillOption( sITEM_NEW );
	LogItemAddonOption( sITEM_NEW );

	// ���� ��� ����
	sITEM_NEW.cDAMAGE = pProductRecipe->nGradeAttack;
	sITEM_NEW.cDEFENSE = pProductRecipe->nGradeDefense;

	// ���� �ɼ� ����
	if ( pProductRecipe->bRandomOption )
	{
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );
	}

	//	����ִ� ������ Inventory�� ����.
	InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, false, 1, true, false, true );
	SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

	if ( !pINSERT_ITEM )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_FAIL );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//  ������ Ÿ�Կ� ������� ������ �����.
	// ��� �� �α� ����
	LogItemExchange(
		pINSERT_ITEM->sItemCustom,
		gamelog::ID_CHAR, 0,
		gamelog::ID_CHAR, m_CharDbNum, 
		ITEM_ROUTE_ITEM_MIX, pINSERT_ITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MIX_MIX_RESULT,
		true );

	//	[�ڽſ���] �޽��� �߻�
	InvenInsertSend( pINSERT_ITEM, FALSE, false, true );

	NetMsgFB.SetData(
		EMITEM_MIX_FB_OK,
		pINSERT_ITEM->sItemCustom.GetNativeID(),
		pINSERT_ITEM->sItemCustom.wTurnNum );
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenStagePass( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_CF* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_CF* ) nmg;

	// Note : �������� �н��� ��� �Ұ��� ���� Ȯ��
	if ( !pLand->IsInstantMap() )
	{
		return S_FALSE;
	}

	// Note : �������� ��ȿ���� Ȯ��
	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	// Note : ������ ���� Ȯ��
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_STAGE_PASS )
	{
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	int nStage = static_cast< int >( pItem->sDrugOp.dwCureVolume );

	GLLandMan* pLandMan = GetLandMan();

	if ( !pLandMan )
	{
		return FALSE;
	}

	GLPartyField* pParty = GetMyParty();

	if ( pParty )
		pParty->SetMemberRevival();

	DWORD dwMapID = 0;
	DWORD dwGateID = 0;
	const SPVEInDunID id = pLandMan->GetPVEIndunID();

	if ( nStage > 1 )
	{
		TriggerSystem::SServerFindStageTriggerMessage msg;
		msg.id = id.dwMapID;
		msg.level = id.nLevel;
		msg.stage = nStage - 1;
		TriggerSystem::SendMessage( &msg );

		if ( msg.trigger == 0 )
		{
			return FALSE;
		}

		TriggerSystem::SServerFindPortalOpenTriggerMessage fmsg;
		msg.trigger->SendMessage( &fmsg );

		if ( fmsg.trigger == 0 )
		{
			return FALSE;
		}

		const TriggerSystem::CServerPortalOpenTrigger* potri = 0;

		if ( fmsg.trigger->CastTrigger(potri) == false )
		{
			return FALSE;
		}

		dwMapID = potri->GetToMapID().dwID;
		dwGateID = potri->GetToGateID();

		pLandMan->TriggerStop();
	}
	else
	{
		TriggerSystem::SServerMapInfoMessage msg;
		msg.mapinfo.mapid = id.dwMapID;
		TriggerSystem::SendMessage( &msg );
		dwMapID = msg.mapinfo.mapid.dwID;
		dwGateID = msg.mapinfo.gateid;
	}

	if ( SNATIVEID( false ) == dwMapID )
	{
		return FALSE;
	}

	GLMSG::NET_PVE_MOVE_REQ_FA NetMsgFld;
	NetMsgFld.sInDunID = id;
	NetMsgFld.dwToMapID = dwMapID;
	NetMsgFld.dwToGateID = dwGateID;
	NetMsgFld.nStage = nStage;
	NetMsgFld.bIsItem = true;
	NetMsgFld.wPosX = pNetMsg->wPosX;
	NetMsgFld.wPosY = pNetMsg->wPosY;
	NetMsgFld.dwCharDbNum = CharDbNum();
	msgpack::sbuffer buf;
	msgpack::pack( buf, NetMsgFld );
	m_pGaeaServer->SENDTOAGENT( NET_MSG_PVE_MOVE_REQ_FA, buf );

	return TRUE;

}

HRESULT GLChar::MsgReqInvenStagePassOk(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_AF NetMsg;

	if ( CastNetMsg( nmg, NetMsg ) == false )
	{
		return FALSE;
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( NetMsg.wPosX, NetMsg.wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenEnterNumCharge( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF* ) nmg;	

	// Note : �������� ��ȿ���� Ȯ��
	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	// Note : ������ ���� Ȯ��
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_ENTER_NUM_CHARGE )
	{
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FA NetMsgFld;
	NetMsgFld.wPosX = pNetMsg->wPosX;
	NetMsgFld.wPosY = pNetMsg->wPosY;
	NetMsgFld.dwCharDbNum = CharDbNum();
	NetMsgFld.dwMapID = pItem->sBasicOp.sSubID.Id();
	NetMsgFld.nLevel = pItem->sDrugOp.dwCureVolume;

	msgpack::sbuffer buf;
	msgpack::pack( buf, NetMsgFld );
	m_pGaeaServer->SENDTOAGENT( NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA, buf );

	return TRUE;

}

HRESULT GLChar::MsgReqInvenEnterNumChargeOk( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF NetMsg;

	if ( CastNetMsg( nmg, NetMsg ) == false )
	{
		return FALSE;
	}

	// Note : �������� ��ȿ���� Ȯ��
	SINVENITEM* pInvenItem = m_cInventory.GetItem( NetMsg.wPosX, NetMsg.wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_ENTER_NUM_CHARGE )
	{
		return E_FAIL;
	}

	if ( NetMsg.bOk )
	{
		//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
		ConsumeInvenItem( NetMsg.wPosX, NetMsg.wPosY );
		GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC Msg;
		Msg.dwMapID = pItem->sBasicOp.sSubID.Id();
		SendToClient( &Msg );
	}


	return S_OK;
}

// *****************************************************
// Desc: �ڽ�Ƭ �ɷ�ġ �ο�
// *****************************************************
HRESULT GLChar::MsgReqInvenCostumeStats( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_COSTUME_STATS* pNetMsg = ( GLMSG::SNET_INVEN_COSTUME_STATS* ) nmg;

	GLMSG::SNET_INVEN_COSTUME_STATS_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// ��ȿ�� �κ��丮 ���� �˻�
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// ��ȿ�� ���������� �˻�
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	SINVENITEM* pSrcInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pSrcInvenItem )
		return E_FAIL;

	// �տ� ��� �ִ� �������� ��ȿ���� �˻�
	const SITEMCUSTOM& scSrcItem = pSrcInvenItem->CustomItem();
	if ( scSrcItem.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	// �տ��� �������� ���� ���� ����
	SITEM* pSrcItem = GLogicData::GetInstance().GetItem( scSrcItem.Id() );
	if ( !pSrcItem )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( scSrcItem.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) && pSrcItem->sDrugOp.tTIME_LMT != 0 )
	{
		const CTime cTIME_CUR = CTime::GetCurrentTime();

		CTimeSpan cSPAN( pSrcItem->sDrugOp.tTIME_LMT );
		CTime cTIME_LMT( scSrcItem.tBORNTIME );
		cTIME_LMT += cSPAN + scSrcItem.GetPeriodExBasic();

		WORD wInsertPosX = pNetMsg->wHoldPosX, wInsertPosY = pNetMsg->wHoldPosY;

		if ( cTIME_CUR > cTIME_LMT )
		{

			InvenDeleteSend( wInsertPosX, wInsertPosY, FALSE );

			//	�ð� �������� ������ ���� �˸�.
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
			NetMsgInvenDelTimeLmt.nidITEM = scSrcItem.GetNativeID();
			SendToClient( &NetMsgInvenDelTimeLmt );

			// ������ ����
			InvenDeleteItem( wInsertPosX, wInsertPosY, false, FALSE, 1, true );

			NetMsgFB.emFB = EMREQ_CS_FB_FAIL7;
			SendToClient( &NetMsgFB );
			return S_FALSE; 
		}
	}

	
	if ( pSrcItem->sBasicOp.emItemType != ITEM_COSTUME_STATS && 
		 pSrcItem->sDrugOp.emDrug != ITEM_DRUG_COSTUME_STATS )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL2;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pInvenItem->sItemCustom.IsDisguise() && pInvenItem->sItemCustom.nidDISGUISE == NATIVEID_NULL() )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL3;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !COSSDATA.IsPartsData( pItem->sSuitOp.emSuit ) )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL5;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pInvenItem->sItemCustom.nidDISGUISE != NATIVEID_NULL() && pInvenItem->sItemCustom.tDISGUISE != 0 )
	{
		CTime tTime( pInvenItem->sItemCustom.tDISGUISE + pInvenItem->sItemCustom.GetPeriodExDisguise());
		CTime tStatTime = CTime::GetCurrentTime();

		tStatTime += pSrcItem->sDrugOp.tDuration;

		if ( tStatTime >= tTime )
		{
			NetMsgFB.emFB = EMREQ_CS_FB_FAIL4;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}
	}
	else
	{
		CTime tTime( pInvenItem->sItemCustom.tBORNTIME + pInvenItem->sItemCustom.GetPeriodExBasic() );
		CTime tStatTime = CTime::GetCurrentTime();

		tTime     += pItem->sDrugOp.tTIME_LMT;
		tStatTime += pSrcItem->sDrugOp.tDuration;

		if ( tStatTime >= tTime && pInvenItem->sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
		{
			NetMsgFB.emFB = EMREQ_CS_FB_FAIL4;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}
	}

	bool bResult = pSrcItem->ISPILE() ? DeletePileItem( scSrcItem.GetNativeID(), 1 ) : InvenDeleteItem( pSrcInvenItem->wPosX, pSrcInvenItem->wPosY, true, pSrcInvenItem->sItemCustom.bVietnamGainItem, 1, true );

	if ( bResult )
	{
		LogItemExchange( scSrcItem, gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, 1, GetCurrentMap(), LOG_ITEM_DEL_COSTUME_STATE, true );	
	}
	else
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL6;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const SITEMCUSTOM& scDstItem = pInvenItem->CustomItem();

	EMITEM_ROUTE eLogType = ITEM_ROUTE_COSTUME_STATE_INSERT;
	
	// �̹� ����� �ɷ�ġ�� �ִ�.
	if ( scDstItem.IsCSApplied() ) {
		pInvenItem->sItemCustom.ResetInvest();

		eLogType = ITEM_ROUTE_COSTUME_STATE_REINSERT;
	}

	for( int i = 0; i < COSTUMEUSERVALUE::sCostumeUser::MAX_COS_SIZE; ++i )
	{		
		pInvenItem->sItemCustom.costumeUser.Invest[i].cStatType = static_cast< BYTE >( pNetMsg->sInvestInfo[i].eStatsNum ); 
		pInvenItem->sItemCustom.costumeUser.Invest[i].wInvestPt = pNetMsg->sInvestInfo[i].wInvestPt;
	}

	CTime cTIME = CTime::GetCurrentTime();
	
	CTimeSpan cSPAN( pSrcItem->sDrugOp.tDuration );
	cTIME += cSPAN;

	pInvenItem->sItemCustom.costumeUser.tEndTime = cTIME.GetTime();
	
	// �ٷ� ��� �����Ѵ�.
	InvenUpdateItem( pInvenItem );

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	// �α�
	LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, eLogType, 1, GetCurrentMap(), eLogType == ITEM_ROUTE_COSTUME_STATE_INSERT ? LOG_ITEM_COSTUME_STATE_NEW : LOG_ITEM_COSTUME_STATE_OVERWRITE, true );

	LogItemCostumeState( m_CharDbNum, pInvenItem->sItemCustom);

	NetMsgFB.emFB = EMREQ_CS_FB_SUCCESS;
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenCostumeStats_Release( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_COSTUME_RELEASE* pNetMsg = ( GLMSG::SNET_INVEN_COSTUME_RELEASE* ) nmg;

	GLMSG::SNET_INVEN_COSTUME_RELEASE_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// ��ȿ�� �κ��丮 ���� �˻�
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}
	
	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// ��ȿ�� ���������� �˻�
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( !pInvenItem->sItemCustom.IsCSApplied() )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL2;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	SINVENITEM* pSrcInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pSrcInvenItem )
		return E_FAIL;

	// �տ� ��� �ִ� �������� ��ȿ���� �˻�
	const SITEMCUSTOM& scSrcItem = pSrcInvenItem->CustomItem();
	if ( scSrcItem.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �տ��� �������� ���� ���� ����
	SITEM* pSrcItem = GLogicData::GetInstance().GetItem( scSrcItem.Id() );
	if ( !pSrcItem )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// #item ������Ÿ�� Ȯ���ϴ� �κ��� ���� �� ���� �߰���
	if ( pSrcItem->sBasicOp.emItemType != ITEM_COSTUME_EFFECT_RELEASE )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( IsActionLimit( EMACTION_LIMIT_ITEM ) )
		return E_FAIL;

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;

	pInvenItem->sItemCustom.costumeUser.ResetInvest();
	NetItemUpdate.wPosX       = pInvenItem->wPosX;
	NetItemUpdate.wPosY       = pInvenItem->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );
	
	ConsumeInvenItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	// �ٷ� ��� �����Ѵ�.
	InvenUpdateItem( pInvenItem );

	///////////////////////////////////////////////////////////////////////////////////
	NetMsgFB.emFB    = EMREQ_CS_RELEASE_FB_SUCCESS;
	NetMsgFB.sItemID = pInvenItem->GetNativeID();

	SendToClient( &NetMsgFB );
	///////////////////////////////////////////////////////////////////////////////////
	
	LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_COSTUME_STATE_DELETE, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL, true );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenChangeItemSkill ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_FB sNetMsgFB;

	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	if ( false == IsValidBody() )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_CHANGE_ITEMSKILL_CARD );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_OK;
	SendToClient( &sNetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenChangeBasicStats ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_BASICSTATS_FB sNetMsgFB;

	GLMSG::SNET_INVEN_CHANGE_BASICSTATS_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_BASICSTATS_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	if ( false == IsValidBody() )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_CHANGE_BASICSTATS_CARD );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_OK;
	SendToClient( &sNetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenChangeAddStats ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sNetMsgFB;

	GLMSG::SNET_INVEN_CHANGE_ADDSTATS_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ADDSTATS_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	if ( false == IsValidBody() )
	{
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_CHANGE_ADDSTATS_CARD );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	sNetMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_OK;
	SendToClient( &sNetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenUnlockCharacterSlot ( NET_MSG_GENERIC* _pNetMsg )
{
	if ( false == IsValidBody() )
	{
		return E_FAIL;
	}

	GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF* pNetMsg =
		static_cast< GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF* >( _pNetMsg );
	if ( NULL == pNetMsg )
	{
		return E_FAIL;
	}

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// ��ȿ�� �κ��丮 ���������� �˻��Ѵ�;
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( NULL == pInvenItem )
	{
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// ��ȿ�� ���������� �˻��Ѵ�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		return E_FAIL;
	}

	// ��ȿ�� ������ Ÿ������ �˻��Ѵ�;
	if ( ITEM_UNLOCK_CHARACTER_SLOT != pItem->sBasicOp.emItemType )
	{
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// ĳ���� ������ Ȯ���Ѵ�;
	bool bSuccess = false;
	for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		if ( true == m_arrLockSlotState[ i ] )
		{
			UnlockCharSlot( i );
			bSuccess = true;
			break;
		}
	}

	// ĳ���� ������ ���̻� Ȯ���� �� ���ٴ� ���� �˸���;
	if ( false == bSuccess )
	{
		UnlockCharSlot( 0 );
	}

	// ������ �Ҹ�;
	ConsumeInvenItem( wPosX, wPosY );

	return S_OK;
}

void GLChar::LogRandomOptionAll()
{
	// �κ��丮
	const GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
	GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		SINVENITEM& sINVENITEM = *(*iter).second;
		SITEMCUSTOM& sITEMCUSTOM = sINVENITEM.sItemCustom;
		if ( FALSE == sITEMCUSTOM.IsSetRandOpt() )
			continue;
		if ( m_pItemLimit )
		{
			// InsertOption
			// 0 default
			// 1 force insert
			m_pItemLimit->LogItemRandomOption( sITEMCUSTOM, 1 );
		}
	}

	// ����â
	for ( int i = 0; i < SLOT_TSIZE; ++i )
	{
		if ( m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
			continue;

		SITEMCUSTOM& sITEMCUSTOM = m_PutOnItems[i];
		if ( FALSE == sITEMCUSTOM.IsSetRandOpt() )
			continue;
		if ( m_pItemLimit )
		{
			// InsertOption
			// 0 default
			// 1 force insert
			m_pItemLimit->LogItemRandomOption( sITEMCUSTOM, 1 );
		}
	}
}

void GLChar::RanMobileCharacterConnectionFee( LONGLONG Price, bool bExtend )
{

    if ( GetInvenMoney() < Price )
    {
		// error
		sc::writeLogError(std::string("[RAN MOBILE] RanMobileCharacterConnectionFee Money ERROR"));
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// error
		sc::writeLogError(std::string("[RAN MOBILE] RanMobileCharacterConnectionFee Cha ERROR"));
	}
	
    // ���� �����մϴ�.
    CheckMoneyUpdate( GetInvenMoney(), Price, FALSE, "RanMobile Connection" );

	SubtractInvenMoneyServer( Price, bExtend == false ? SUBTRACT_INVEN_MONEY_RANMOBILE_CONNECTION : SUBTRACT_INVEN_MONEY_RANMOBILE_EXTENDTIME  );

    //	�� �α�.
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -Price, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), bExtend == false ? LOG_MONEY_RANMOBILE_CONNECTION : LOG_MONEY_RANMOBILE_EXTENDTIME );
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

    //	�ݾ� ��ȭ �޽���.
    SendInvenMoneyUpdate();

}
//
//void GLChar::RanMobileCharacterConnectionFee( LONGLONG Price )
//{
//    // ���Ű���=��ǰ����
//    volatile LONGLONG llPrice = Price;
//    if ( GetInvenMoney() < llPrice )
//    {
//		// error
//    }
//
//	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
//	if ( !pTarChar )
//	{		
//		// error
//	}
//	
//    // ���� �����մϴ�.
//    CheckMoneyUpdate( GetInvenMoney(), llPrice, FALSE, "RanMobile Connection" );
//
//	SubtractInvenMoneyServer( llPrice, SUBTRACT_INVEN_MONEY_RANMOBILE_CONNECTION );
//
//    //	�� �α�.
//	// ��� �� �α� ����
//    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_RANMOBILE_CONNECTION );
//	// ��� �� �α� ����
//    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
//
//    //	�ݾ� ��ȭ �޽���.
//    SendInvenMoneyUpdate();
//
//
//}

void GLChar::RanMobileDailyBonus( const SNATIVEID& ItemID )
{
}

// ���� (1) �ƴϸ� ����� (2)
bool GLChar::RanMobileDeleteInvenItem( int nType, const SNATIVEID& ItemID, int nCount )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemID );
	if( NULL == pItem )
		return false;

	SINVENITEM* pInvenItem = m_cInventory.FindItem( ItemID );
	if( NULL == pInvenItem )
		return false;

	if( nCount > m_cInventory.CountTurnItem( ItemID ) )
		return false;

	SINVENITEM* pDelInvenItem = m_cInventory.FindItem( ItemID );
	if( NULL == pDelInvenItem )
		return false;

	ConsumeInvenItem( pDelInvenItem->wPosX, pDelInvenItem->wPosY, false, nCount );

	return true;
}

void GLChar::RanMobileBuyMPointItem( const SNATIVEID& ItemID )
{
	SITEMCUSTOM sCustomItemNEW( ItemID );
	CTime cTIME = CTime::GetCurrentTime();
	sCustomItemNEW.tBORNTIME = cTIME.GetTime();
	sCustomItemNEW.cGenType = EMGEN_DECOMPOSE;
	sc::SeqUniqueGUID::Instance()->getGUID( sCustomItemNEW.guid );
	sCustomItemNEW.GenerateBasicStat( false );
	sCustomItemNEW.GenerateLinkSkill();
	sCustomItemNEW.GenerateAddOption();

	SITEM* pITEM = GLogicData::GetInstance().GetItem( sCustomItemNEW.GetNativeID() );
	if ( !pITEM )
		return; // error

	//GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	//if ( !pChar )
	//{		
	//	// error
	//	return;
	//}

	// ������ �߻�; ���� üũ;
	WORD wINSERTX( 0 ), wINSERTY( 0 );
	//BOOL bOK = pChar->InvenFindInsertable( wINSERTX, wINSERTY );
	BOOL bOK = InvenFindInsertable( wINSERTX, wINSERTY );
	if ( !bOK )
	{
	// error
	}

	//	������ ������ Inventory�� ����;
	//pChar->InvenInsertItem( sCustomItemNEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	InvenInsertItem( sCustomItemNEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	//SINVENITEM *pInsertItem = pChar->InvenGetItem( wINSERTX, wINSERTY );
	SINVENITEM *pInsertItem = InvenGetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem ) 
		return;

	//	[�ڽſ���] �޽��� �߻�;
	//pChar->InvenInsertSend( pInsertItem, FALSE, false, true );
	InvenInsertSend( pInsertItem, FALSE, false, true );

}
