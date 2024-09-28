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

// 인벤아이템을 소모시킴.
// 2. 소모를 클라이언트에 알림.
// 3. b2AGENT 변수는 클라이언트에 알릴때 필드와의 연결성이 불확실 할 경우를 대비하여
// 에이젼트를 통해서 알릴수 있도록 함.
// DbUpdate 남은 개수가 0이라면 디비에서도 삭제한다.
bool GLChar::ConsumeInvenItem( WORD wPosX, WORD wPosY, bool b2AGENT/*= false*/, WORD wNum /*= 1*/, bool bTurnNumCheck /*= false*/, bool DbUpdate /*= true*/ )
{
	//	인벤의 아이템 확인.
	SINVENITEM* pINVENITEM = m_cInventory.FindPosItem( wPosX, wPosY );
	
	if ( !pINVENITEM )
		return false;

	//	아이템 정보 가져옴.
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
			// 통계 및 로그 수정
			// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ConsumeInvenItem ", m_CharDbNum ) );
		}

		// 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
		LogItemExchange(
            pINVENITEM->sItemCustom,
            gamelog::ID_CHAR, m_CharDbNum,
            gamelog::ID_CHAR, 0,
            ITEM_ROUTE_DELETE, 1, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_INVEN_USE,
			false );

		if ( pINVENITEM->sItemCustom.wTurnNum == 0 )
		{
			// 아이템 제거.
			InvenDeleteItem( wPosX, wPosY, false, FALSE, CONSUME_INVEN_ITEM, DbUpdate );

			// [자신에게] Inventory에 아이템 제거.
			GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
			NetMsg_Inven_Delete.dwChaNum = CharDbNum();
			if ( b2AGENT )
                SendToAgent( &NetMsg_Inven_Delete );
			else
                SendToClient( &NetMsg_Inven_Delete );
		}
		else
		{
			// [자신에게] Inventory 아이템 소모됨.
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

	// 쿨타임 적용
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

	// 정상적으로 사용되었는지 안되었는지 확인하지 않았는데 특별활동 처리?
	//
	//mjeon.activity
	//
	ActivityCheckItemUse( idItem.dwID );
	DoQuestItemUse( idItem );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ConsumeSlotItem ", m_CharDbNum ) );
	}

    // 소모성 아이템일 경우
	if ( pItem->ISINSTANCE() )
	{
		if ( sSLOTITEM.wTurnNum >= wNum )
            sSLOTITEM.wTurnNum -= wNum;

		// 아이템 개수 확인을 확실하게 하지 않았는데
		// 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
        LogItemExchange( 
            sSLOTITEM, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, sSLOTITEM.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_INSTANCE_ITEM_BY_SLOT_USE,
			false );

		if ( sSLOTITEM.wTurnNum == 0 )
		{
			//	HOLD 아이템 제거.
			RELEASE_SLOT_ITEM_SERVER( emSLOT, true );

			//	[자신에게] 슬롯에 있었던 아이템 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release( emSLOT );
			NetMsg_Release.emType = EMPUTONRELEASE_TYPE_TURN;
			SendToClient( &NetMsg_Release );
		}
		else
		{
			//	[자신에게] 연마제의 연마가능 횟수 감소.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_Update;
			NetMsg_Update.emSlot = emSLOT;
			NetMsg_Update.wTurnNum = sSLOTITEM.wTurnNum;
			SendToClient( &NetMsg_Update );
		}
	}
	else
	{
		// 아무것도 안했는데
		// 기한 아이템일 경우
		// 통계 및 로그 수정
		LogItemExchange( 
            sSLOTITEM, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, 0, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_SLOT_USE,
			false );
	}

	// 쿨타임 적용
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
	// guid 체크
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
		//	Inventory이 가득차서 아이템을 넣을수 없습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_INVEN_ERROR;
		SendToClient( &NetMsg );
		return false;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertToInven", m_CharDbNum ) );
	}

	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		// 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
		LogItemExchange(
			pItemDrop->sItemCustom,
			gamelog::ID_CHAR, 0,
			gamelog::ID_CHAR, m_CharDbNum,
			ITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_TO_INVEN,
			false );

		// 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
		// 아이템 점검 숫자를 감소시킴.
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

			//	기존 아이템 겹침 여유분.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				// 기존 아이템의 겹침을 변경. ( 꽉참 )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				// 이 아이템 개수 변경만 디비에 업데이트해야 하나?
				// 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				SendToClient( &NetMsg );

				// 잔여 수량 감소.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				// 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				// 클라이언트에 갯수 변경을 알림.
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
					// 아이템 Land에서 삭제.
					pLand->DropOutItem( pItemDrop->dwGlobID );
				}

				return true;
			}
		}

		// 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

		//	최대겹침아이템 수량.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	여분겹침아이템의 겹침수.
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
				sc::writeLogError( sc::string::format( "Char(%1%) inventory - 넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함2", CharDbNum() ) );
				return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
			}

			// 새로운 아이템을 넣어줌.
			SITEMCUSTOM sITEMCUSTOM = pItemDrop->sItemCustom;
			if ( wITEMNUM == ( i + 1 ) )
				sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
			else
				sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

			if ( i > 0 )
			{
				// 신규 guid 생성 필요
				// 이 경우가 되면 위에서 남긴 아이템 로그말고 새로운 아이템이 생성된거다. 
				sc::SeqUniqueGUID::Instance()->getGUID( sITEMCUSTOM.guid );
				sITEMCUSTOM.GenerateBasicStat( false );
				sITEMCUSTOM.GenerateLinkSkill();
				sITEMCUSTOM.GenerateAddOption();
			}

			// Inventory에 넣습니다.
			if ( !InvenInsertItem( sITEMCUSTOM, wInsertPosX, wInsertPosY, false, INSERTTOINVEN_PDROPITEM_EX, true, Force, true ) )
				return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			SINVENITEM* pInvenItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )
				return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			// Inventory에 아이템 넣어주는 메시지.
			InvenInsertSend( pInvenItem, FALSE, Force, bNew );
		}

		// 파티원에게 알려야할때만 전송
		// 파티원일 경우 다른 파티원에게 아이템 습득 알림.
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

		// 아이템 습득시 자동 퀘스트 시작 점검.
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
			// 아이템 Land에서 삭제.
			pLand->DropOutItem( pItemDrop->dwGlobID );
		}
	}
	else
	{
		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY, Force );
		if ( !bOk )
		{
			sc::writeLogError( sc::string::format( "Char(%1%) inventory - 넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함3", CharDbNum() ) );
			return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
		}

		// 아이템 줍기 성공.
		if ( !InvenInsertItem( pItemDrop->sItemCustom, wPosX, wPosY, false, INSERTTOINVEN_PDROPITEM_EX, true, Force, bNew ) )
			return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
		if ( !pInvenItem )
			return false;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		// 파티원에게 알려야할때만 전송
		// 파티원일 경우 다른 파티원에게 아이템 습득 알림.
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

		// 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
		LogItemExchange( 
			pInvenItem->sItemCustom, 
			gamelog::ID_CHAR, 0, 
			gamelog::ID_CHAR, m_CharDbNum, 
			ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_TO_INVEN,
			false );

		//	[자신에게] 메시지 발생.
		InvenInsertSend( pInvenItem, FALSE, Force, bNew );

		// 아이템 습득시 자동 퀘스트 시작 점검.
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
			// 아이템 Land에서 삭제.
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

		// 적이 건 스킬 효과만 제거
		if (pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY)
        {
			UPDATE_DATA_END(i);//RESETSKEFF(i);
        }
    }
}

void GLChar::DisableSkillFact()
{ // 도구 종속 옵션이 있는 지속 효과를 검사 후 삭제 처리 및 해당 클라이언트 및 주변에 통보;
	EMSLOT emRHand = GetCurRHand();	
	SITEM* pRightItem = GET_SLOT_ITEMDATA(emRHand);		

	for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
	{
		if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if (!pSkill)
            continue;

		// 스킬 자신 버프
		if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_SELF || pSkill->m_sBASIC.emIMPACT_REALM != REALM_SELF )
			continue;

		const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
		SKILL::GLSKILL_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

		// 스킬 도구 종속 없음
		if ( emSKILL_RITEM == SKILL::SKILLATT_NOCARE )
			continue;
				
		// 현재 들고 있는 아이템이 null이거나, 도구 조건이 맞지 않으면 삭제;
		// 단, 내가 아닌 다른 사람이 걸어준 버프는 삭제 하지 않음;
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
// Explanation: 현재는 파티에서 금액 분배시 사용되는 함수이다
//************************************
void GLChar::InsertMoney(LONGLONG lnAmount)
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertMoney", m_CharDbNum ) );
	}

	//	돈 습득
	lnAmount = CalculateInsertmoney(lnAmount);

	CheckMoneyUpdate(GetInvenMoney(), lnAmount, TRUE, "Insert Party Money.");

	AddInvenMoneyServer( lnAmount, ADD_INVEN_MONEY_PARTY_MONEY_DISTRIBUTION );

	// 금액 로그.
	if (lnAmount > EMMONEY_LOG)
	{
		// 통계 및 로그 수정
		LogMoneyExchange(gamelog::ID_CHAR, 0,           gamelog::ID_CHAR, m_CharDbNum, lnAmount,  ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_ADD_FROM_PARTY );
		// 통계 및 로그 수정
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

    //	금액 변화 클라이언트에 알려줌.
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
	//	돈 습득.	
	lnMoney = (LONGLONG)(lnMoney * m_pGaeaServer->GetMoneyGainRate(m_CHARINDEX,m_wLevel));

//	IP보너스
//#if defined(TH_PARAM) || defined(MYE_PARAM ) || defined(MY_PARAM) || defined ( PH_PARAM ) //|| defined(_RELEASED)
	if (m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
	{
		if ( m_emIPBonus == EMIP_BONUS_A ) // A등급
		{
			lnMoney *= 2;
		}
        else if ( m_emIPBonus == EMIP_BONUS_B ) // B등급
		{
			lnMoney = static_cast<LONGLONG>(lnMoney*1.75f);
		}
        else if ( m_emIPBonus == EMIP_BONUS_C ) // C등급
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on InsertToInven ", m_CharDbNum ) );
	}

	// 돈 습득
	if ( pMoneyDrop->bDropMonster )
		pMoneyDrop->lnAmount = CalculateInsertmoney( pMoneyDrop->lnAmount );	

	CheckMoneyUpdate( GetInvenMoney(), pMoneyDrop->lnAmount, TRUE, "Insert Money." );

	AddInvenMoneyServer( pMoneyDrop->lnAmount, ADD_INVEN_MONEY_GROUND_MONEY_PICK_UP );
	if ( GLHIDESET::CheckHideSet( EMHIDE_PICKUP_ITEM ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	// 금액 로그
	if ( pMoneyDrop->lnAmount > EMMONEY_LOG )
	{
		// 통계 및 로그 수정
		LogMoneyExchange( gamelog::ID_CHAR, 0,           gamelog::ID_CHAR, m_CharDbNum, pMoneyDrop->lnAmount, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_ADD_FROM_LAND );
		// 통계 및 로그 수정
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

	//	돈을 Land에서 삭제.
	pLand->DropOutMoney( pMoneyDrop->dwGlobID );

	return true;
}

// Field의 아이템,돈을 Inventory 에 넣기 시도.
//
HRESULT GLChar::MsgReqFieldToInven( NET_MSG_GENERIC* nmg )
{
    bool hrInsertToInven = true;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_FIELD_TO_INVEN* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_FIELD_TO_INVEN* >( nmg );

	// 사망확인
	if ( !IsValidBody() )
		return E_FAIL;

	if ( pNetMsg->emCrow == CROW_ITEM )
	{
		PITEMDROP pItemDrop = pLand->GetDropItem( pNetMsg->dwID );
		if ( !pItemDrop )
		{
			//	아이템이 사라졌습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// 아이템이 자신과 동일한 맵에 있는지 ( 선도전 종료후 비인증클럽원이 선도무기 획득하는 버그 대처 )		
		if ( pItemDrop->sMapID != m_mapID.getGaeaMapID() )
			return E_FAIL;

		if ( !pItemDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
		{
			//	아이템이 다른 사람에게 예약되어 있습니다.
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

		//	거리 체크
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
			//	거리가 멉니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

        // 필드에서 주운경우 시효성 체크( 클럽락커 버그 대처 )
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
				// 통계 및 로그 수정
				// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqFieldToInven ", m_CharDbNum ) );
			}

			if ( sCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
			{			
				CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
				CTime cTIME_LMT( sCUSTOM.tBORNTIME );
				cTIME_LMT += cSPAN + sCUSTOM.GetPeriodExBasic();

				if ( cTIME_CUR > cTIME_LMT )
				{
					//	시간 제한으로 아이템 삭제 로그 남김.
					// 통계 및 로그 수정
					LogItemExchange(
                        sCUSTOM,
                        gamelog::ID_CHAR, m_CharDbNum,
                        gamelog::ID_CHAR, 0,
                        ITEM_ROUTE_DELETE, sCUSTOM.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,
						false );

					//	시간 제한으로 아이템 삭제 알림.
					GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
					NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.GetNativeID();
					SendToClient( &NetMsgInvenDelTimeLmt );

					bDELETE = true;				

					// 코스툼 복구.
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

						// 코스튬 염색값 옮김. by luxes.
						//
						sITEM_NEW.dwMainColor = sCUSTOM.dwMainColor;
						sITEM_NEW.dwSubColor = sCUSTOM.dwSubColor;

						// 코스튬 능력치 옮김
						sITEM_NEW.costumeUser = sCUSTOM.costumeUser;
						sITEM_NEW.cPeriodExtensionCount = sITEM_NEW.cCostumePeriodExtensionCount;
						sITEM_NEW.SetPeriodExBasic(sITEM_NEW.GetPeriodExDisguise());
						sITEM_NEW.ResetPeriodExDisguise();

						// ?
						pItemDrop->sItemCustom.dwMainColor = 0;
						pItemDrop->sItemCustom.dwSubColor = 0;

						// 아이템 생성.
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

						// 생성된 코스툼은 바닥에서 다시 떨군다.
						DropItem( sITEM_NEW, m_vPosition );

						// 분리된 코스툼 로그.
						// 통계 및 로그 수정
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
			
			// 발라져 있는 시간제 코스튬 삭제 
			if ( !bDELETE && sCUSTOM.nidDISGUISE != SNATIVEID( false ) )
			{
				if ( sCUSTOM.tDISGUISE != 0 )
				{
					CTime cTIME_LMT( sCUSTOM.tDISGUISE + sCUSTOM.GetPeriodExDisguise());
					if ( cTIME_LMT.GetYear() != 1970 )	// 이 부분 이렇게 하지않고 유효한 시간인지 확인하는 함수로 하던데, 그거로 교체 필요하다.
					{
						if ( cTIME_CUR > cTIME_LMT )
						{
							//	시간 제한으로 아이템 삭제 알림.
							GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
							NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.nidDISGUISE;
							SendToClient( &NetMsgInvenDelTimeLmt );

							// 코스툼 정보 리셋.
							pItemDrop->sItemCustom.tDISGUISE = 0;
							pItemDrop->sItemCustom.nidDISGUISE = SNATIVEID( false );
							pItemDrop->sItemCustom.ResetPeriodExDisguise();

							// 코스튬 염색값 지움. by luxes.
							//
							pItemDrop->sItemCustom.dwMainColor = 0;
							pItemDrop->sItemCustom.dwSubColor = 0;

							// 코스튬 능력치 초기화
							pItemDrop->sItemCustom.ResetInvest();
						}
					}
				}
			}		
		}

		// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
		//
		GLPartyField* const pParty = GetMyParty();
		if ( pParty && !( pItemDrop->sItemCustom.IsEvent() != NULL ) )
		{
			const bool ballot = pParty->DOITEM_ALLOT( m_pGaeaServer, m_dwGaeaID, pItemDrop );
			if ( ballot )
                return S_OK;
		}

		// 이벤트 아이템 중복으로 가져가기 방지.
		if ( ( pItemDrop->sItemCustom.IsEvent() != NULL ) && m_cInventory.HaveEventItem() )
		{
			//	이미 이벤트 아이템을 가지고 있습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// 아이템 인벤에 넣음 ( 인벤이 가득 차있을 때는 습득 불가능. )
		hrInsertToInven = InsertToInven( pItemDrop, true, true, true, false, true );

/*
		// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
		// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
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
			//	돈이 사라졌습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		//	거리 체크
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
			//	거리가 멉니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;
			SendToClient( &NetMsg );
			return E_FAIL;			
		}

		if ( !pMoneyDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
		{
			//	돈이 다른 사람에게 예약되어 있습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_RESERVED;
			SendToClient( &NetMsg );
			return E_FAIL;
		}

		// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
		GLPartyField* const pParty = GetMyParty();
		if ( pParty )
		{
			bool ballot = pParty->DOMONEY_ALLOT( m_pGaeaServer, m_dwGaeaID, pMoneyDrop, pLand );
			if ( ballot )
			{
				return S_OK;
			}
		}

		//	돈 줍기 성공.
		hrInsertToInven = InsertToInven( pMoneyDrop );
	}

    // Note : 캐릭터가 돈 줍기 성공했을때, 바이크 타고 있으면 내리자.
    if ( !pNetMsg->bPet && hrInsertToInven )
    {
        if ( m_emVehicle != EMVEHICLE_OFF )
        {
			ToggleVehicle( false );
        }
    }

	return S_OK;
}

// Field의 아이템,돈을 손에 들기 시도.	( 손에 든 아이템이 없다고 가정함. )
//
HRESULT GLChar::MsgReqFieldToHold( NET_MSG_GENERIC* nmg )
{
	// 사용되지 않는다.
	return E_FAIL;

	/*
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_FIELD_TO_HOLD* pNetMsg = reinterpret_cast< GLMSG::SNETPC_REQ_FIELD_TO_HOLD* >( nmg );

	PITEMDROP pItemDrop = pLand->GetDropItem ( pNetMsg->dwGlobID );

	// 필드에서 아이템을 손에 드는경우는 인벤이 가득 찼을 경우 뿐이다.
	// 베트남탐닉일 경우 손에 드는걸 방지해야 한다.
	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
			return E_FAIL;
	}

	if ( !pItemDrop )
	{
		//	아이템이 사라졌습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_OFF;
		SendToClient( &NetMsg );
		return S_FALSE;
	}

	// 아이템이 자신과 동일한 맵에 있는지 ( 선도전 종료후 비인증클럽원이 선도무기 획득하는 버그 대처 )
	if ( pItemDrop->sMapID != m_mapID.getGaeaMapID() )
		return S_FALSE;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pItemDrop->sItemCustom.GetNativeID() );
	if ( !pItemData )
		return S_FALSE;

	if ( !pItemDrop->IsTakeItem( GetPartyID(), m_dwGaeaID ) )
	{
		//	아이템이 다른 사람에게 예약되어 있습니다.
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

	// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
	//
	GLPartyField* const pParty = GetMyParty();
	if ( pParty && !( pItemDrop->sItemCustom.IsEvent() != NULL ) )
	{
		bool ballot = pParty->DOITEM_ALLOT( m_pGaeaServer, m_dwGaeaID, pItemDrop );
		if ( ballot )
			return S_OK;
	}

	// 이벤트 아이템 중복으로 가져가기 방지.
	if ( ( pItemDrop->sItemCustom.IsEvent() != NULL ) && m_cInventory.HaveEventItem() )
	{
		//	이미 이벤트 아이템을 가지고 있습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;
		SendToClient( &NetMsg );
		return S_FALSE;
	}

	// Field에 있었던 아이템을 손에 듬.
	HOLD_ITEM( pItemDrop->sItemCustom );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqFieldToHold", m_CharDbNum ) );
	}

	// 아이템의 소유 이전 경로 기록    
	// 통계 및 로그 수정
	LogItemExchange(
        pItemDrop->sItemCustom,
        gamelog::ID_CHAR, 0,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_PICKUP_FIELD_ITEM,
		false );
    
	// 아이템 Land에서 삭제.
	pLand->DropOutItem ( pItemDrop->dwGlobID );

	return S_OK;
	*/
}

// Inventory의 아이템을 손에 들기 시도.
//
HRESULT GLChar::MsgReqInvenToHold(NET_MSG_GENERIC* nmg)
{
	// 시용되지 않는다.
	return E_FAIL;
	/*
	GLMSG::SNETPC_REQ_INVEN_TO_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_INVEN_TO_HOLD*> ( nmg );
	
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_INVEN_TO_HOLD,EMHOLD_FB_OFF);

	//	요청한 아이템 유효성 검사.
	SINVENITEM *pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	손에 든 아이템이 있을때.
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

	//	Inventory에 있던 아이템 백업.
	SINVENITEM sInven_BackUp = *pInvenItem;

	// Inventory에서 빼낼 아이템을 삭제.
    InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, true, FALSE);

	//	Inventory에 있었던 아이템을 손에 듬.
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

    // 이 코드 지우면 아이템 복사 된다.
    // 이동할 수 없는 인벤에서 옮길경우 아이템이 복사되는 문제가 있었다. 일단 급하게 이동을 할 수 없도록 변경 하였다.
    if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wHoldPosY )
        if ( m_cInventory.IsInsertable( pNetMsg->wPosX, pNetMsg->wPosY ) == FALSE )
            return S_FALSE;

	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wPosY )
		if ( m_cInventory.IsInsertable( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) == FALSE )
			return S_FALSE;

	SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.
	
	WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
	WORD wPosY = sInven_BackUp.wPosY;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
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

	// 병합 할수 있는 아이템인지 점검.
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
			// 겹침 수 조정.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

            //	Note : 들고 있는 아이템 지움.
            InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, FALSE, 190, true );

			// 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			SendToClient( &NetMsgInvenItem );
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	기존 아이템 갯수 변경.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경.
			pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

            // 인벤의 아이템 업데이트.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgHoldInvenItem;
            NetMsgHoldInvenItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgHoldInvenItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgHoldInvenItem.sItemCustom = pHoldInvenItem->sItemCustom;
            SendToClient( &NetMsgHoldInvenItem );
			
			// 인벤의 아이템 업데이트.
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
	// 복사 비용이 발생하지만 우선 테스트 해본다.
	if ( !InvenSwapTestInvenItem( pInvenItem, pHoldInvenItem ) )
	{
		return E_FAIL;
	}

	// 에러 대비 백업
	GLInventory BackupInventory;
	BackupInventory.Assign( m_cInventory );
	*/

	// #item
	// inven(Inven) <-> inven(Hold)
	// inven 아이템(Inven) wPosX, wPosY, sInven_BackUp
	// inven 아이템(Hold) wHoldPosX, wHoldPosY, sHold_BackUp

	// 아이템 삭제 처리
	{
		if ( !m_cInventory.DeleteItem( wPosX, wPosY ) )
		{
			// 그냥 아무것도 안했기 때문에 예외 처리 할 것도 없다.
			return S_FALSE;
		}

		if ( !m_cInventory.DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY 위치에 있던 아이템 처리
			if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				InvenDeleteSend( wPosX, wPosY, FALSE );

				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// 우선 바닥에 떨어뜨리고, 정상적으로 떨어지지 않았다면 에러 아이템 처리 목록에 들어간다.
				// 여기로 들어오는 경우의 거의 없겠지만, 자주 들어와서 거래불가 아이템을 거래하는 어뷰징이 발생하면
				// 우편으로 보내는 방법을 이용하자.
				DropItem( sInven_BackUp.sItemCustom, m_vPosition );
			}

			return S_FALSE;
		}
	}

	// 아이템 삽입 처리
	// 아이템 삽입 가능 여부는 확인하지 않는다. 예전에는 삽입 가능한지 확인 먼저하고
	// 인벤토리의 경우 프리미엄 라인에 강제로 넣어주었느나 이제 그렇게 하지 않는다.
	// 위에서 정상적으로 삭제되었다면 무조건 아이템이 삽입된다. 
	{
		if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// 위에서 삭제된 아이템 알림
			InvenDeleteSend( wPosX, wPosY, FALSE );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			// wPosX, wPosY 위치에 있던 삭제된 아이템 처리
			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sInven_BackUp.sItemCustom, m_vPosition );

			// wHoldPosX, wHoldPosY 위치에 있던 삭제된 아이템 처리
			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sHold_BackUp.sItemCustom, m_vPosition );

			return S_FALSE;
		}

		if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
		{
			// 위에서 삭제된 아이템 알림
			InvenDeleteSend( wPosX, wPosY, FALSE );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExInven insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold 아이템은 이미 정상적으로 삽입되어 있는 상황(wPosX wPosY 위치에)
			// Hold 위치에 아이템 삽입이 실패한 상황. 이미 정상적으로 삽입되어 있는 아이템(Hold 아이템) 알림
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

		// 아이템 확인
		pINSERT_INVEN_ITEM = m_cInventory.GetItem( wPosX, wPosY );
		pINSERT_HOLD_ITEM = m_cInventory.GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_INVEN_ITEM || !pINSERT_HOLD_ITEM )
		{
			// 예전의 서버 로그를 확인해도 삽입된 아이템이 NULL 인 로그는 없었다.
			// 이런 경우가 발생할 경우 접속 종료 시킨다. 캐릭터가 나가면 바로 위에까지는 성공했기 때문에 저장될 것이다.

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

	// 디비 업데이트
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

    //	유효기간 초과 점검.	
    HRESULT hrCheckDuration1 = CheckItemDuration( *pINSERT_INVEN_ITEM );
    HRESULT hrCheckDuration2 = CheckItemDuration( *pINSERT_HOLD_ITEM );
    if ( hrCheckDuration1 == E_FAIL || hrCheckDuration2 == E_FAIL )
        return E_FAIL;

	return S_OK;
}

// 유저락커(HoldItem) <-> 인벤토리
// 유저락커의 아이템을 들어서 인벤토리 아이템과 교환
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
        // 창고지기 유효성 체크
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

    //	창고정보를 DB에서 가져오지 않았을때.
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

    // 팻카드일경우 팻이 활동중이면
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

	// 프리미엄 라인 유효기간이 지났다면 그냥 리턴한다. 그렇지 않으면 바닥에 아이템이 떨어진다.
	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wPosY )
	{
		return S_FALSE;
	}

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.

    WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
    WORD wPosY = sInven_BackUp.wPosY;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

    if ( m_ServiceProvider == SP_VIETNAM )
    {
        if ( sInven_BackUp.sItemCustom.bVietnamGainItem )
            return S_FALSE;
    }

    const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );

    if ( !pInvenData )
        return S_FALSE;

    // 들고 있는 아이템이 아닌, 창고에 들어갈 아이템에 대해서, 거래 가능한지 여부를 검사
    // 거래옵션
	// 창고 저장할때는 거래옵션이 아니라 새로 추가된 창고 옵션을 검사
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageExInven", m_CharDbNum ) );
	}

    // 병합 할수 있는 아이템인지 점검.
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
            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_USER, m_dwUserID, 
                gamelog::ID_CHAR, m_CharDbNum, 
                ITEM_ROUTE_CHAR, sHold_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
				false );

            // 겹침 수 조정.
            pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			// Note : 들고 있는 아이템 지움. [자신에게] 메시지 발생.	- 창고에서 아이템 삭제.
			UserStorageDeleteItem( dwChannel, sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, 1, true  );

            // 인벤의 아이템 업데이트.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
            NetMsgInvenItem.wPosX = pNetMsg->wPosX;
            NetMsgInvenItem.wPosY = pNetMsg->wPosY;
            NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
            SendToClient( &NetMsgInvenItem );
        }
        else
        {
            WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_USER, m_dwUserID, 
                gamelog::ID_CHAR, m_CharDbNum, 
                ITEM_ROUTE_CHAR, pInvenData->sDrugOp.wPileNum - sInven_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
				false );

            //	기존 아이템 갯수 변경.
            pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

            //	손에 들고 있던 아이템 변경.
            pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

            // 창고 홀드 위치 아이템 업데이트.
            GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
            NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
            NetMsgStorageItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgStorageItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgStorageItem.wTurnNum = pHoldInvenItem->sItemCustom.wTurnNum;
            SendToClient( &NetMsgStorageItem );

            // 인벤의 아이템 업데이트.
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
	// inven 아이템 wPosX, wPosY, sInven_BackUp
	// user locker 아이템 wHoldPosX, wHoldPosY, sHold_BackUp

	// 아이템 삭제 처리
	{
		if ( !m_cInventory.DeleteItem( wPosX, wPosY ) )
		{
			// 그냥 아무것도 안했기 때문에 예외 처리 할 것도 없다.
			return S_FALSE;
		}

		if ( !m_cStorage[dwChannel].DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY 위치에 있던 아이템 처리
			if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				InvenDeleteSend( wPosX, wPosY, FALSE );

				sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// 우선 바닥에 떨어뜨리고, 정상적으로 떨어지지 않았다면 에러 아이템 처리 목록에 들어간다.
				// 여기로 들어오는 경우의 거의 없겠지만, 자주 들어와서 거래불가 아이템을 거래하는 어뷰징이 발생하면
				// 우편으로 보내는 방법을 이용하자.
				DropItem( sInven_BackUp.sItemCustom, m_vPosition );
			}

			return S_FALSE;
		}
	}

	// 아이템 삽입 처리
	// 아이템 삽입 가능 여부는 확인하지 않는다. 예전에는 삽입 가능한지 확인 먼저하고
	// 인벤토리의 경우 프리미엄 라인에 강제로 넣어주었느나 이제 그렇게 하지 않는다.
	// 위에서 정상적으로 삭제되었다면 무조건 아이템이 삽입된다. 
	{
		if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// 위에서 삭제된 아이템 알림
			InvenDeleteSend( wPosX, wPosY, FALSE );
			UserStorageDeleteSend( dwChannel, wHoldPosX, wHoldPosY );

			// wPosX, wPosY 위치에 있던 삭제된 아이템 처리
			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sInven_BackUp.sItemCustom, m_vPosition );

			// wHoldPosX, wHoldPosY 위치에 있던 삭제된 아이템 처리
			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sHold_BackUp.sItemCustom, m_vPosition );

			return S_FALSE;
		}

		if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
		{
			// 위에서 삭제된 아이템 알림
			InvenDeleteSend( wPosX, wPosY, FALSE );
			UserStorageDeleteSend( dwChannel, wHoldPosX, wHoldPosY );

			sc::writeLogError( sc::string::format( "critical error MsgReqStorageExInven insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold 아이템은 이미 정상적으로 삽입되어 있는 상황(wPosX wPosY 위치에)
			// Hold 위치에 아이템 삽입이 실패한 상황. 이미 정상적으로 삽입되어 있는 아이템(Hold 아이템) 알림
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

		// 아이템 확인
		pINSERT_INVEN_ITEM = m_cInventory.GetItem( wPosX, wPosY );
		pINSERT_HOLD_ITEM = m_cStorage[dwChannel].GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_INVEN_ITEM || !pINSERT_HOLD_ITEM )
		{
			// 예전의 서버 로그를 확인해도 삽입된 아이템이 NULL 인 로그는 없었다.
			// 이런 경우가 발생할 경우 접속 종료 시킨다. 캐릭터가 나가면 바로 위에까지는 성공했기 때문에 저장될 것이다.

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

    // 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
    LogItemExchange( 
        sHold_BackUp.sItemCustom, 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, sHold_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
		false );


    // 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
    LogItemExchange( 
        sInven_BackUp.sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_USER, m_dwUserID, 
        ITEM_ROUTE_USER_INVEN, sInven_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN,
		false );

    GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
    NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;							//	삭제될 아이템.
    NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
    NetMsg_Del_Insert.sInsert = *pINSERT_INVEN_ITEM;						//	삽입되는 아이템.

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsg_Del_Insert );
    SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

    GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Storage_Del_Insert;
    NetMsg_Storage_Del_Insert.dwChannel = dwChannel;
    NetMsg_Storage_Del_Insert.wDelX = sHold_BackUp.wPosX;					//	삭제될 아이템.
    NetMsg_Storage_Del_Insert.wDelY = sHold_BackUp.wPosY;
    NetMsg_Storage_Del_Insert.sInsert = *pINSERT_HOLD_ITEM;					//	삽입되는 아이템.
    SendToClient( &NetMsg_Storage_Del_Insert );

    // 유효기간 초과 점검.	
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

	//	자격조건에 부합하는지 검사;
	if ( !ACCEPT_ITEM( sInvenItem ) )
		return E_FAIL;

	//	해당 슬롯에 장착 가능한지 검사.
	bOk = EMSLOTCHECK_OK == CHECKSLOT_ITEM( sInvenItem.GetNativeID(), pNetMsg->emToSlot );
    if ( !bOk )
		return E_FAIL;

    bOk = IsCurUseArm( pNetMsg->emToSlot );
    if ( !bOk )
		return E_FAIL;

    // 바이크 타입일때 로드중이라면, 밑에 뭔가 빠진것같은데
	const SNATIVEID sItemNID( pInvenItem->sItemCustom.GetNativeID() );
    const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sItemNID );
    if ( !pInvenData )
		return E_FAIL;

    // 인벤에 넣어야할 아이템
    SITEMCUSTOM sToInvenItems[SLOT_TSIZE];
    EMSLOT      emToInvenSlot[SLOT_TSIZE];
    UINT        nToInvenNums = 0;

    // 해당 슬롯의 아이템
    SITEMCUSTOM sSlotItem = GET_SLOT_ITEM( pNetMsg->emToSlot );

    // 해당 슬롯에 아이템이 있다면
    if ( sSlotItem.GetNativeID() != NATIVEID_NULL() )
    {
        if ( !VALID_SLOT_ITEM( pNetMsg->emToSlot ) )
			return E_FAIL;

        const SITEM* pSlotData = GLogicData::GetInstance().GetItem( sSlotItem.GetNativeID() );

        // 병합 할수 있는 아이템인지 점검.
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
                // 인벤의 아이템 제거
                InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, true );

                // 슬롯의 아이템 업데이트
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
                // 인벤의 아이템 업데이트.
                pInvenItem->sItemCustom.wTurnNum = wTurnNum - pInvenData->sDrugOp.wPileNum;

                GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
                NetMsgInvenItem.wPosX       = pNetMsg->wPosX;
                NetMsgInvenItem.wPosY       = pNetMsg->wPosY;
                NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
                SendToClient( &NetMsgInvenItem );

                // 슬롯의 아이템 업데이트
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

    //	손에 드는 도구일 경우 다른 손의 아이템을 검사한다
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
                // 다른 한쪽이 소모성 아이템일경우
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

                // 다른 한쪽이 무기일 경우
            case ITEM_SUIT:
                {
                    // 둘다 한손 무기라면
                    if ( !ISBOTH_HANDHELD( pITEM_TO ) && !ISBOTH_HANDHELD( pITEM_ANOTHER ) )
                    {
                        bSlotRelease = false;
                    }
                    else
                    {
                        switch ( pITEM_TO->sBasicOp.emItemType )
                        {
                        // 타입이 소모성이라면
                        // 다른무기가 유효한지 검사는 CHECKSLOT_ITEM에서 한다.
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

    // 인벤의 빈 공간 갯수
    DWORD dwEmptyCount = m_cInventory.GetAmountEmpty( FALSE );

    WORD wVALIDY = m_cInventory.GetValidCellY( FALSE );

    // 착용하려는 아이템에, 착용중인 아이템을 놓을 수 있다면, 빈 공간 갯수는 +1 의 효과이다
    if ( pNetMsg->wPosY < wVALIDY )
    {
        dwEmptyCount += 1;
    }

    // 필요한 인벤의 칸 보다, 삽입하려는 아이템의 갯수가 더 많다면 실패
    if ( dwEmptyCount < nToInvenNums )
    {
        return S_FALSE;
    }

    //	Inventory에서 아이템 삭제
    InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, false );

    // 인벤에 아이템 삽입.
    for ( UINT i=0; i<nToInvenNums; ++i )
    {
        const SITEM* pItem = GLogicData::GetInstance().GetItem( sToInvenItems[i].GetNativeID() );

        WORD wInvenPosX = 0;
        WORD wInvenPosY = 0;

        // 처음은 인벤의 위치와 교환 한다.
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

        //	[모두에게] 자신의 착용 복장이 바뀜을 알림.
        RELEASE_SLOT_ITEM_SERVER( emToInvenSlot[i] );

        GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutRelease( emToInvenSlot[i] );
        SendToClient( &NetMsg_PutRelease );

        GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutRelease_Brd( emToInvenSlot[i] );
		NetMsg_PutRelease_Brd.dwGaeaID = m_dwGaeaID;
        SendMsgViewAround( &NetMsg_PutRelease_Brd );

        InvenInsertItem( sToInvenItems[i], wInvenPosX, wInvenPosY, true, 1, false );
		
		// 저장 위치 및 좌표 디비 업데이트
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

			// 강화복구
			sInvenItem.TempEnchantRecovery();

			// 로그
			LogItemExchange( sInvenItem, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );
		}
	}

    //	슬롯에 아이템 삽입.
    SLOT_ITEM_SERVER( sInvenItem, pNetMsg->emToSlot );
	
	// 저장 위치 및 좌표 디비 업데이트
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

    //	아이템 변경시 값 재 환산 필요함.
    INIT_DATA( FALSE, FALSE );

    // 무기에 따른 버프를 초기화 한다.
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

    // 인벤에 넣어야할 아이템
    SITEMCUSTOM sToInvenItems[SLOT_TSIZE];
    EMSLOT      emToInvenSlot[SLOT_TSIZE];
    UINT        nToInvenNums = 0;

    // 해당 슬롯의 아이템
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

    //	오른손의 무기를 뺀다면 왼손도 같이 제거한다
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

    // 인벤에 아이템 삽입과 슬롯의 아이템 삭제
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

        //	[모두에게] 자신의 착용 복장이 바뀜을 알림.
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

				// 강화복구
				sToInvenItems[i].TempEnchantRecovery();

				JustInvenTypeUpdate = false;

				// 로그
				LogItemExchange( sToInvenItems[i], gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );
			}
		}

        InvenInsertItem( sToInvenItems[i], wInvenPosX, wInvenPosY, true, 1, false );

		// 디비 업데이트
		InvenUpdateItem( m_cInventory.GetItem( wInvenPosX, wInvenPosY ), JustInvenTypeUpdate );
    }

    //	아이템 변경시 값 재 환산 필요함.
    INIT_DATA( FALSE, FALSE );

    if ( pNetMsg->emFromSlot == SLOT_VEHICLE )
    {
        m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, false );
    }

    // 무기에 따른 버프를 초기화 한다.
    DisableSkillFact();

    return S_OK;
}

HRESULT GLChar::MsgReqVNGainToHold( NET_MSG_GENERIC* nmg )
{
	// 처리하지 않는다.
	return E_FAIL;

	/*
	GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD*> ( nmg );

	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD,EMHOLD_FB_OFF);

	//	요청한 아이템 유효성 검사.
	SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	손에 든 아이템이 있을때.
	if ( HaveHoldItem() )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//	Inventory에 있던 아이템 백업.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	Inventory에서 빼낼 아이템을 삭제.
    InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, true, TRUE);

	//	Inventory에 있었던 아이템을 손에 듬.
	HOLD_ITEM(sInven_BackUp.sItemCustom);

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqVNGainExHold( NET_MSG_GENERIC* nmg )
{
	// 처리하지 않는다.
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

	SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.

	WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	손에 든 아이템 백업.

	const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( sItemHold.GetNativeID() );
	if ( !pInvenData || !pHoldData )
		return S_FALSE;

	// 병합 할수 있는 아이템인지 점검.
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
			// 겹침 수 조정.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	손 있었던 아이템 제거.
			RELEASE_HOLD_ITEM ();

			// 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			SendToClient(&NetMsgInvenItem);

			//	[자신에게] 손 있었던 아이템 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			SendToClient(&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

			//	기존 아이템 갯수 변경.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경.
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM(sItemHold);
			
			// 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			SendToClient(&NetMsgInvenItem);
		}

		return S_OK;
	}

	//	Inventory에 있던 아이템 삭제.
	BOOL bOk = m_cVietnamInventory.DeleteItem( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	// 종전 손에든 아이템과 들려는 아이템의 크기가 틀릴 경우.
	//	들고 있는 아이템이 들어갈 공간을 찾는다.
//	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	동일위치에 넣을수 있는지 검사.
		BOOL bOk = m_cVietnamInventory.IsInsertable ( wPosX, wPosY );
		if ( !bOk )
		{
			//	들고있던 아이템 Inventory에 넣을 공간이 있는지 검색.
			bOk = m_cVietnamInventory.FindInsertable( wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : Inventory에서 삭제했던 아이템을 다시 넣음.
				sInven_BackUp.sItemCustom.bVietnamGainItem = true;
				m_cVietnamInventory.InsertItem( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

				return S_FALSE;
			}
		}
	}

	//	들고있던 아이템 Inventory에 넣음.
	sItemHold.bVietnamGainItem = true;
	m_cVietnamInventory.InsertItem( sItemHold, wPosX, wPosY );
	SINVENITEM* pINSERT_ITEM = m_cVietnamInventory.GetItem(wPosX,wPosY);
	if ( !pINSERT_ITEM )
	{
		sc::writeLogError(sc::string::format("error MsgReqInvenExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.GetNativeID().wMainID, sItemHold.GetNativeID().wSubID, wPosX, wPosY ));

		return S_FALSE;
	}

	//	Inventory에 있었던 아이템을 손에 듬.
	HOLD_ITEM(sInven_BackUp.sItemCustom);

	//	[자신에게] Inventory에 있던 아이템 삭제, 및 아이템 삽입.
	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이템.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.bVietnamInven = TRUE;
	if (pINSERT_ITEM)
        NetMsg_Del_Insert.sInsert = *pINSERT_ITEM;	//	삽입되는 아이템.

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, NetMsg_Del_Insert);

	SendToClient(NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer);
	
	return S_OK;
	*/
}

HRESULT GLChar::MsgReqHoldToVNGain( NET_MSG_GENERIC* nmg )
{
	// 처리하지 않는다.
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

	//	인벤 위치.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cVietnamInventory.IsInsertable ( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	Inventory이 가득찻음.
		return E_FAIL;
	}

	//	들고있던 아이템 Inventory에 넣음.
	SITEMCUSTOM& sITEM_NEW = m_PutOnItems[SLOT_HOLD];
	sITEM_NEW.bVietnamGainItem = true;
	bOk = m_cVietnamInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
	if (!bOk)
        return E_FAIL;

	const SINVENITEM* pINVENITEM = m_cVietnamInventory.FindPosItem( wInsertPosX, wInsertPosY );

	//	성공시 아이템 제거.
	RELEASE_HOLD_ITEM ();

	//	[자신에게] Inventory에 아이템 삽입.
    InvenInsertSend(pINVENITEM, TRUE);

	//	[자신에게] 손 있었던 아이템 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	SendToClient(&NetMsg_ReleaseHold);	

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqVNInvenToInven( NET_MSG_GENERIC* nmg )
{
	// 처리하지 않는다.
	return E_FAIL;

	/*
	if ( m_dwVietnamInvenCount <= 0 )
		return E_FAIL;

	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN* pNetMsg = (GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN *)nmg;

	SINVENITEM* pInvenItem(NULL);
	pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );

	//	살려고 하는 아이템이 없습니다.
	if ( !pInvenItem )
		return E_FAIL;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	WORD wInsertPosX(0), wInsertPosY(0);
	BOOL bITEM_SPACE(false);
	bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

	//	인벤에 여유공간이 없을 경우 취소됨.
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
				//	[자신에게] 해당 아이템을 Inventory에서 삭제.
				InvenDeleteSend(sINVENITEM.wPosX, sINVENITEM.wPosY, FALSE);

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sINVENITEM.sItemCustom.GetNativeID();
				SendToClient(&NetMsgInvenDelTimeLmt);

				// 아이템 삭제
				InvenDeleteItem(sINVENITEM.wPosX, sINVENITEM.wPosY, false, FALSE);
			
				// 코스툼 복구.
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

					// 코스튬 염색값 옮김. by luxes.
					//
					sITEM_NEW.dwMainColor = sINVENITEM.sItemCustom.dwMainColor;
					sITEM_NEW.dwSubColor = sINVENITEM.sItemCustom.dwSubColor;

					// 코스튬 능력치 옮김
					sITEM_NEW.costumeUser = sINVENITEM.sItemCustom.costumeUser;

					pINVENITEM->sItemCustom.dwMainColor = (DWORD)0;
					pINVENITEM->sItemCustom.dwSubColor = (DWORD)0;

					// 아이템 생성
                    sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sITEM_NEW.cChnID = (BYTE) GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE) GetFieldSvrID();
					RegItemGen( sITEM_NEW.guid );

					// 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cInventory.IsInsertable ( wInsertPosX, wInsertPosY );
					if ( !bOk )
						return E_FAIL;

					// 인벤에 넣음.
					m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

					InvenInsertSend(pINSERT_ITEM);

					GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
					if ( !pTarChar )
					{		
						// 통계 및 로그 수정
						// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
						sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqVNInvenToInven", m_CharDbNum ) );
					}

					// 분리된 코스툼 로그
					// 통계 및 로그 수정
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

	// 분리 요청 아이템 찾음.
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	// 아이템 정보 가져오기.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	// 분리 가능한 아이템인지 점검.
	bool bSPLIT( false );
	bSPLIT = ( pItem->ISINSTANCE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= 1 )					return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	// 원본 아이템의 변화된 갯수.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	// 분리되어 새로 생기는 아이템.
	SITEMCUSTOM sNEW_ITEM( pInvenItem->sItemCustom );
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;
	sc::SeqUniqueGUID::Instance()->getGUID( sNEW_ITEM.guid );
	sNEW_ITEM.GenerateBasicStat( false );
	sNEW_ITEM.GenerateLinkSkill();
	sNEW_ITEM.GenerateAddOption();

	// #item
	// 아이템 분리할 때 디비 에러가 발생
	// 확인해보니 분리되는 아이템을 그대로 원본에서 복사해서 guid 만 생성해서 인벤삽입 및 디비 업데이트했다.
	// 그러니 디비에는 없는 아이템인데 업데이트가 오니 문제가 발생
	// 이런거 꽤 있을 것 같다. 아이템 그대로 복사했을 때는 디비 상태에 맞게 디비 상태도 변경해야 한다.
	// 아이템 디비 상태도 변경해야함, 업데이트에서 삽입으로 변경이 안되기 때문에 강제로 변경한다.
	sNEW_ITEM.SetDbState( db::DB_INSERT, true );

	// 새로 생기는 아이템 위치 찾기.
	WORD wInvenPosX( 0 ), wInvenPosY( 0 );
	bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )
		return E_FAIL;

	// 새로 생긴 아이템 넣기.
	if ( !InvenInsertItem( sNEW_ITEM, wInvenPosX, wInvenPosY, true, 1, true ) )
	{
		return E_FAIL;
	}

	// 원본 아이템의 갯수 변경 시킴.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	// #item
	// 테스트
	// 개수 변경 디비 업데이트
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemUpdateTurnNum( pInvenItem->GetGuid(), pInvenItem->sItemCustom.wTurnNum, 19770929 ) ) );
	}

	// 원본 아이템 갯수 변경.
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
	// 사용되지 않는 것 같아 주석 처리함
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

    //	성공시 아이템 제거.
    RELEASE_HOLD_ITEM();

    //	[자신에게] 손 있었던 아이템 제거.
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

	//	인벤 위치.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		// 빈자리가 아님
		return E_FAIL;
	}

	//	들고있던 아이템 Inventory에 넣음.	
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

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.

	//	성공시 아이템 제거.
	if ( !InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, FALSE, 1, false ) )
	{
		// 넣었던 아이템 삭제 하고 리턴
		InvenDeleteItem( wInsertPosX, wInsertPosY, false, FALSE, 1, false );
		return E_FAIL;
	}

	// 위치만 디비에 업데이트한다.
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

	//	[자신에게] Inventory에 아이템 삽입.
    InvenInsertSend( pInsertItem );

	//	유효기간 초과 점검.
    HRESULT hrCheckDuration = CheckItemDuration( *pInsertItem );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;

/*
	// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
	// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
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

        // 창고지기 유효성 체크
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

    //	창고정보를 DB에서 가져오지 않았을때.
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

	//	인벤 위치.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
	BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	Inventory이 가득찻음.
		return E_FAIL;
	}

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.

	//	Note : 들고 있는 아이템 지움.
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

	// 락커 아이템 삭제 클라에 알림
	UserStorageDeleteSend( dwChannel, pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	// 디비 업데이트(저장위치만 창고에서 인벤토리로 변경되었기 때문에 저장위치 및 좌표만 디비에서 업데이트한다.)
	InvenUpdateItem( pInsertItem, true );

	// Inventory에 아이템 삽입된 아이템 알림.
    InvenInsertSend( pInsertItem );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageToInven", m_CharDbNum ) );
	}

    // 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
    LogItemExchange( 
        pInsertItem->CustomItem(), 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pInsertItem->CustomItem().wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MOVE_STORAGE_TO_INVEN,
		false );

	//	유효기간 초과 점검.
    HRESULT hrCheckDuration = CheckItemDuration( *pInsertItem );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;

/*
	// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
	// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
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

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = pHoldInvenItem->wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = pHoldInvenItem->wPosY;
	const SITEMCUSTOM& sITEMCUSTOM = pHoldInvenItem->CustomItem();

	const SNATIVEID& itemID( sITEMCUSTOM.GetRealNativeID() );
	if ( m_pPrivateMarket->IsOpen() == true )
	{
		sc::writeLogError( "While opening Personal Store, it's not possible to drop items. there is a high possibility of being hacked or packet abusing case." );
		sc::writeLogError( sc::string::format( "If you find any case that this message is repeatedly left within a log, please report us it right away(%1%/%2%).", itemID.Mid(), itemID.Sid() ) );		
	}	

	// 임시로 확인을 위해서 추가
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

	// 아이템을 버릴 수 있는곳이어야만 버릴 수 있다;
	if ( pMapNode->IsDropBlockItem() )
		return S_FALSE;

	// 버리는 아이템이 팻카드 && 팻이 활성상태이면 버릴 수 없다
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
	
	//	거래옵션
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

	// GMCharEdit 로 넣은 아이템은 버리기 불가능하다.
	if ( sITEMCUSTOM.IsGM_GENITEM() )
		return S_FALSE;

	//	아이템 제거.
	if ( !InvenDeleteItem( wHoldPosX, wHoldPosY, true, sITEMCUSTOM.bVietnamGainItem, MSG_REQINVENTOFIELD, false ) )
	{
		return S_FALSE;
	}

	//	Item Drop 시도.
	{
		// 바닥에 떨어뜨린다.
		// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
		// 그외 디비업데이트 상태라면 디비 인벤타입 변경한다.
		// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
		// 누군가 줍는다면 자기 아이템으로 업데이트된다.
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

	// 위에서 디비 업데이트를 했는데 DropItem 실패한다면 예외처리를 어떻게 할 것인가?
	// 바닥에 버릴 때 holder 를 설정하지 않는것 같은데, 기획 의도? 바닥에 버린거 바로 다른 캐릭터가 주을 수 있나 보다
	if ( pLand->DropItem( vCollisionPos, &( sHold_BackUp.sItemCustom ) ) == NULL )
	{
		sc::writeLogError( sc::string::format( "MsgReqInvenToField DropItem error chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sHold_BackUp.GetGuid() ).c_str() ) );
		m_pGaeaServer->InsertErrorItem( sHold_BackUp.GetGuid(), 301 );
        return S_FALSE;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenToField", m_CharDbNum ) );
	}

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
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
	// 사용되지 않는다.
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

	//	양손 모두 착용 무기 있을 경우.	활+화살, 창+부적. 세트로 동작하는 무기류가 있음 이때 모두 빼내야함.
	//
	if ( pNetMsg->emSlot==emRHand )
	{
		if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_BOW) || (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_SPEAR) ||
			 (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_PISTOL) ||  (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_RAILGUN) ||
			 (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_PORTALGUN) )
		{
			if ( pITEM_LEFT )	//	화살, 부적 빼내야 함.
			{
				SITEMCUSTOM sITEM_LHAND = GET_SLOT_ITEM(emLHand);

				WORD wInvenPosX, wInvenPosY;
				BOOL bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
				if ( bOk )
				{
					//	종전 아이템 Inventory에 넣음.
					m_cInventory.InsertItem( sITEM_LHAND, wInvenPosX, wInvenPosY );

					//	[자신에게] Inventory에 아이템 삽입.
					InvenInsertSend(m_cInventory.GetItem(wInvenPosX, wInvenPosY));

					//	[모두에게] 디스플레이 아이템이 아닌걸로 가정하여 무시.
				}
				// 인벤토리 넣기에 실패하였을 경우.
				else
				{
					//	Item Drop 시도.
					pLand->DropItem ( m_vPosition, &sITEM_LHAND );

					GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
					if ( !pTarChar )
					{		
						// 통계 및 로그 수정
						// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
						sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqSlotToHold", m_CharDbNum ) );
					}

					// 아이템의 소유 이전 경로 기록
					// 통계 및 로그 수정
					LogItemExchange(
                        sITEM_LHAND,
                        gamelog::ID_CHAR, m_CharDbNum,
                        gamelog::ID_CHAR, 0,
                        ITEM_ROUTE_GROUND, sITEM_LHAND.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_SLOT_TO_FIELD,
						false);
				}

				//	왼쪽 슬롯 지워줌.
				RELEASE_SLOT_ITEM(emLHand);

				//	[자신에게] SLOT에 있었던 아이템 제거.
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

	//	손에 들게 함.
	HOLD_ITEM(GET_SLOT_ITEM(pNetMsg->emSlot));

	//	SLOT 아이템 제거.
	RELEASE_SLOT_ITEM ( pNetMsg->emSlot );

	//	아이템 변경시 값 재 환산 필요함.
	INIT_DATA( FALSE, FALSE );

	//	[자신에게] SLOT에 있었던 아이템 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(pNetMsg->emSlot);
	SendToClient(&NetMsg_PutOn_Release);

	//	[모두에게] 자신의 착용 복장이 바뀜을 알림.
	GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutOn_Release_Brd;
	NetMsg_PutOn_Release_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Release_Brd.emSlot = pNetMsg->emSlot;
	SendMsgViewAround(&NetMsg_PutOn_Release_Brd);

	if (pNetMsg->emSlot == SLOT_VEHICLE)
		m_pGaeaServer->SaveVehicle(m_ClientSlot, m_dwGaeaID, false);

	// 무기에 따른 버프를 초기화 한다.
	DisableSkillFact();	

	return S_OK;
	*/
}

HRESULT GLChar::MsgReqHoldToSlot(NET_MSG_GENERIC* nmg)
{
	// 사용되지 않는다.
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
	
	//	자격조건에 부합하는지 검사;
	if ( !ACCEPT_ITEM ( sItemCustom ) )
		return E_FAIL;
	
	//	해당 슬롯에 장착 가능한지 검사.
	bOk = EMSLOTCHECK_OK == CHECKSLOT_ITEM ( sItemCustom.GetNativeID(), pNetMsg->emSlot );
	if ( !bOk )
		return E_FAIL;

	//	해당 슬롯이 비였는지 점검.
	bOk = ISEMPTY_SLOT ( sItemCustom.GetNativeID(), pNetMsg->emSlot );
	if ( !bOk )
		return E_FAIL;

	//	만약 양손 무기일 경우 슬롯을 "오른손 들기" 슬롯으로.
	if ( ISBOTH_HANDHELD(pItem) )
		pNetMsg->emSlot = GetCurRHand();

	//	해당 슬롯에 아이템 장착.
	SLOT_ITEM ( sItemCustom, pNetMsg->emSlot );

	//	손에 든 아이템 제거
	RELEASE_HOLD_ITEM ();

	//	아이템 변경시 값 재 환산 필요함.
	INIT_DATA( FALSE, FALSE );

	//	[자신에게] 손에 있었던 아이템 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(SLOT_HOLD);
	SendToClient(&NetMsg_PutOn_Release);

	//	[자신에게] 손에 있었던 아이템을 Slot에 착용.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.m_Slot = pNetMsg->emSlot;
	NetMsg_PutOn.m_Item = sItemCustom;
    msgpack::sbuffer SendBufferB;
    msgpack::pack(SendBufferB, NetMsg_PutOn);
	SendToClient(NET_MSG_GCTRL_PUTON_UPDATE, SendBufferB);

	//	[모두에게] 복장이 바뀜을 알려줌.
	GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.m_GaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.m_Slot   = pNetMsg->emSlot;
	NetMsg_PutOn_Brd.m_Item.Assign(sItemCustom);
    msgpack::sbuffer SendBufferC;
    msgpack::pack(SendBufferC, NetMsg_PutOn_Brd);
	SendMsgViewAround(NET_MSG_GCTRL_PUTON_UPDATE_BRD, SendBufferC);

	// 무기에 따른 버프를 초기화 한다.
	DisableSkillFact();

	return S_OK;
	*/
}


HRESULT GLChar::MsgReqSlotChange(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_SLOT_CHANGE* pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_CHANGE*> ( nmg );

	if ( IsUseArmSub() ) SetUseArmSub( FALSE ); // 주무기
	else				SetUseArmSub( TRUE );  // 보조무기

	//	아이템 변경시 값 재 환산 필요함.
	INIT_DATA( FALSE, FALSE, m_sCONFTING.sOption.fHP_RATE );

	//	[자신에게] 무기 스왑
	GLMSG::SNETPC_PUTON_CHANGE NetMsg_PutOn;
	NetMsg_PutOn.bUseArmSub = IsUseArmSub();
	NetMsg_PutOn.fCONFT_HP_RATE = m_sCONFTING.sOption.fHP_RATE;
	SendToClient(&NetMsg_PutOn);

	//	[모두에게] 무기 스왑을 알림.
	GLMSG::SNETPC_PUTON_CHANGE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.bUseArmSub = IsUseArmSub();
	SendMsgViewAround(&NetMsg_PutOn_Brd);

	GLMSG::SNETPC_PUTON_CHANGE_AG NetMsg_PutOn_AG;
	NetMsg_PutOn_AG.dwChaNum = CharDbNum();
	NetMsg_PutOn_AG.bUseArmSub = IsUseArmSub();
	SendToAgent(&NetMsg_PutOn_AG );


	// 무기에 따른 버프를 초기화 한다.
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
		// 아이템을 넣을 공간이 없다.
		return false;
	}

	WORD wPILENUM = pItem->sDrugOp.wPileNum;

	// 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	//		아이템 점검 숫자를 감소시킴.
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

		//	기존 아이템 겹침 여유분.
		WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

		if ( wREQINSRTNUM > wSURPLUSNUM )
		{
			// 기존 아이템의 겹침을 변경. ( 꽉참 )
			sITEMCUSTOM.wTurnNum = wPILENUM;

			// 클라이언트에 갯수 변경을 알림.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
			NetMsg.dwChaNum = CharDbNum();
			NetMsg.wPosX = sINVENITEM.wPosX;
			NetMsg.wPosY = sINVENITEM.wPosY;
			NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
			SendToClient( &NetMsg );

			// 잔여 수량 감소.
			wREQINSRTNUM -= wSURPLUSNUM;

			UPDATED_ITEM inserted( SNATIVEID( sINVENITEM.wPosX, sINVENITEM.wPosY ), wSURPLUSNUM );
			vecInsertedItems.push_back( inserted );
		}
		else
		{
			// 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
			sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

			// 클라이언트에 갯수 변경을 알림.
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

	// 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

	//	최대겹침아이템 수량.
	WORD wONENUM = wREQINSRTNUM / wPILENUM;
	WORD wITEMNUM = wONENUM;

	//	여분겹침아이템의 겹침수.
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
			sc::writeLogError( sc::string::format( "Char(%1%) inventory - 넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함5", CharDbNum() ) );
			return false;
		}

		// 새로운 아이템을 넣어줌.
		SITEMCUSTOM sNewItem( sItemCustom.GetNativeID() );
		sNewItem.tBORNTIME = sItemCustom.tBORNTIME;
		sNewItem.cGenType = sItemCustom.cGenType;
		sc::SeqUniqueGUID::Instance()->getGUID( sNewItem.guid );
		sNewItem.GenerateBasicStat( false );
		sNewItem.GenerateLinkSkill();
		sNewItem.GenerateAddOption();

		if ( wITEMNUM == ( i + 1 ) )
			sNewItem.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
		else
			sNewItem.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

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

	// 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	// 아이템 점검 숫자를 감소시킴.
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

			// 갯수변경 메시지.
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

//! NPC 로부터 아이템 구입
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
        // 상인 NPC가 존재하지 않거나 상인이 아님
        return E_FAIL;
    }

    // Note : 대화문에서 구매 조건 체크.
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
    
    // 판매개수 체크
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
			// 상점 NPC 유효성 체크
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
			// 상점 NPC 유효성 체크
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
	// 기존에 교환 페이지에서 돈타입으로 아이템 구매 신청하면 무조건 구매됨;
	GLInventorySale* pInven = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( pPacket->dwChannel ) );
	if ( !pInven || pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_1 )
		return E_FAIL;

	SINVENITEM* pSaleItem( NULL );
	pSaleItem = pInven->GetItem( pPacket->wPosX, pPacket->wPosY );

	// 살려고 하는 아이템이 없습니다.
	if ( !pSaleItem )
        return E_FAIL;

	// 아이템 정보 없음
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pSaleItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

	volatile float fSHOP_RATE = GET_PK_SHOP2BUY();
	if ( fSHOP_RATE==0.0f )
        return E_FAIL;

	volatile float fSHOP_2RATE = fSHOP_RATE + pLand->GetCommissionRate();

	LONGLONG dwPrice = pCrowData->GetNpcSellPrice( pItem->sBasicOp.sNativeID.dwID );
	volatile LONGLONG llPRICE2 = 0; // 물품가격
	if ( dwPrice == 0 )
	{
		dwPrice = pItem->sBasicOp.dwBuyPrice;
		llPRICE2 = dwPrice * static_cast< LONGLONG >( fSHOP_2RATE ) / 100;
	}
    else
    {
		llPRICE2 = dwPrice;
	}

	// 구매가격=물품가격 X 개수
	volatile LONGLONG llPRICE3 = static_cast< LONGLONG >( llPRICE2*pPacket->wBuyNum );
	if ( GetInvenMoney() < llPRICE3 )
	{
		//	돈이 부족합니다.
		return E_FAIL;
	}

	/*
	// 아이템 구매시에는 포인트나 기여도를 확인하지 않도록 수정 
	// 착용 조건에서 검사하므로 문제가 되지 않음

    if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	활동 포인트가 부족합니다.
		return E_FAIL;
    }

	if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
	{
		//	기여 포인트가 부족합니다.
		return E_FAIL;
	}
	*/

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqBuyFromNpc", m_CharDbNum ) );
	}

	// 인벤에 여유 공간이 있는지 검사
	CTime cTIME = CTime::GetCurrentTime();
	BOOL bITEM_SPACE( FALSE );
	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pPacket->wBuyNum * pItem->GETAPPLYNUM() );

		bITEM_SPACE = m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );
		
		//	인벤에 여유공간이 없을 경우 취소됨.
		if ( !bITEM_SPACE )
            return E_FAIL;

		// 구입을 수행합니다.
		CheckMoneyUpdate( GetInvenMoney(), llPRICE3, FALSE, "Buy From Npc.");

		SubtractInvenMoneyServer( llPRICE3, SUBTRACT_INVEN_MONEY_ITEM_BUY_NPC );	// 돈을 지불.

		// 돈 로그.
		// 통계 및 로그 수정
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPRICE3, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_BUY_FROM_NPC );
		// 통계 및 로그 수정
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

		// 아이템을 인벤에 넣어준다.
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

				// 실제 개수가 변경되고, 삽입된 아이템의 로그를 남긴다. vecInsertedItems 안에 xy 좌표와 삽입된 아이템 개수가 들어 있다.
				// 기존 아이템에 겹쳐진 것도 있을테고, 신규로 생성된 아이템도 있을꺼다.
				// 통계 및 로그 수정
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
		//	일반 아이템의 경우.
		WORD wInsertPosX( 0 ), wInsertPosY( 0 );
		bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

		//	인벤에 여유공간이 없을 경우 취소됨.
		if ( !bITEM_SPACE )
			return E_FAIL;

		// 새로운 아이템을 생성합니다
		SITEMCUSTOM sNewItem( pSaleItem->sItemCustom );
		sNewItem.tBORNTIME = cTIME.GetTime();
		sNewItem.wTurnNum = 1;
		sNewItem.cGenType = EMGEN_SHOP;
		sc::SeqUniqueGUID::Instance()->getGUID( sNewItem.guid );
		sNewItem.GenerateBasicStat( false );
		sNewItem.GenerateLinkSkill();
		sNewItem.GenerateAddOption();

		// 돈을 지불합니다.
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

		// 인벤토리에 넣어줍니다
		if ( !InvenInsertItem( sNewItem, wInsertPosX, wInsertPosY, true, 
			  INSERT_INVEN_BUY_FROM_NPC_NOT_PILEITEM, true, false, true ) )
		{
			return S_OK;
		}

		// 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
		LogItemExchange(
            sNewItem,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_CHAR, sNewItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_BUY_ITEM_FROM_NPC,
			true );

        //	돈 로그.
		// 통계 및 로그 수정
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPRICE2, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_BUY_FROM_NPC );
		// 통계 및 로그 수정
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
			// 클럽의 수익 발생.
			std::tr1::shared_ptr< GLClubField > pClub = GetClub( pLand->m_dwGuidClubID );
			if ( pClub )
			{
				pClub->AddProfitMoeny( llCommission );

				// 클럽 수입이 1억 이상일 경우 로그를 남긴다.
				if ( llCommission > 	EMCLUBINCOMEMONEY_LOG )
                {   
					// 통계 및 로그 수정
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

	//	금액 변화 메시지.
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
		// 상인 NPC가 존재하지 않거나 상인이 아님
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
			// 상점 NPC 유효성 체크 
			GLSummonField* pCrow = pLand->GetSummon( pPacket->dwNpcGlobID );
			if (!pCrow) return E_FAIL;

			if ( !pCrow->IsSummonNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )		return E_FAIL;

			if( pCrow->GetSummonType() != SUMMON_TYPE_NPC ) return E_FAIL;
		}
		break;
	case CROW_NPC    :
		{
			// 상점 NPC 유효성 체크 
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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
		// 필요재료 체크
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

		// 넣을수 있는 체크
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
		case EX_POINT_TYPE :	// 란포인트 차감
			{
				std::vector< sExNeed >::iterator _iter = vListExChange.begin();

				while ( _iter != vListExChange.end() )
				{
					PointShopPointSub( _iter->iNeedCount, pointshop::NPC_EXCHANGE_ITEM,  pointshop::POINT_SUB_TYPE_EXCHANGE_NPC );

					++_iter;
				}				
			}
			break;
		case EX_LVP_TYPE :		// 생활포인트 차감
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
				//	겹침 아이템일 경우.
				WORD wPILENUM  = pExItem->sDrugOp.wPileNum;
				SNATIVEID sNID = pExItem->sBasicOp.sNativeID;
				WORD wREQINSRTNUM = ( pPacket->itemlist[i].wBuyNum * pExItem->GETAPPLYNUM() );

				//	아이템의 소유 이전 경로 기록.
				sITEM_NEW.cGenType  = EMGEN_SHOP;
				sITEM_NEW.wTurnNum  = ( pPacket->itemlist[i].wBuyNum * pExItem->GETAPPLYNUM() );
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();

				// 아이템 교환 기능 일때도 랜덤옵션이 1개이상 붙도록 RM 1172
				// 20160101 khkim 겹침 아이템은 랜덤옵션이 무의미함 생성하지 않는다. 실제 InsertPileItem 이 함수안에서도 랜덤옵션은 들어가지 않는다.
				//if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
				//	LogItemRandomOption( sITEM_NEW );

				// #item 아이템 로그 vecInsertedItems 사용해서 다시 남기자. 나중에 작업 필요
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

					// 새로운 아이템을 생성합니다
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

					// 아이템 교환 기능 일때도 랜덤옵션이 1개이상 붙도록 RM 1172
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

    // 기본적으로 성공. 실패할 경우 실패 이유 대입.
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

    // 아이템 정보 없음
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

    // 구매가격=물품가격
    volatile LONGLONG llPrice = Price;
    if ( GetInvenMoney() < llPrice )
    {
        //	돈이 부족합니다.
        hr = E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_MONEY;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

	/*
	// 아이템 구매시에는 포인트나 기여도를 확인하지 않도록 수정 
	// 착용 조건에서 검사하므로 문제가 되지 않음
    if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	활동 포인트가 부족합니다.
        hr =  E_FAIL;
        NetMsgFB.emFB = EMBUYREBUYITEM_CAN_NOT_BUY_ITEM;
        NetMsgFAB.emFB = NetMsgFB.emFB;

        SendToClient( &NetMsgFB );
        SendToAgent( &NetMsgFAB );

        return hr;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	기여 포인트가 부족합니다.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgBuyRebuyItem", m_CharDbNum ) );
	}
	
	DWORD dwVehicleID = sItemCustom.m_VehicleDbNum;
	
    // 인벤에 여유 공간이 있는지 검사
    BOOL bITEM_SPACE( FALSE );
    if ( pItem->ISPILE() )
    {
        //	겹침 아이템일 경우.
        WORD wPILENUM = pItem->sDrugOp.wPileNum;
        SNATIVEID sNID = pItem->sBasicOp.sNativeID;

        //	넣기 요청된 아이템수. ( 잔여량. )
        WORD wREQINSRTNUM = sItemCustom.wTurnNum;

        bITEM_SPACE = m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );

        //	인벤에 여유공간이 없을 경우 취소됨.
        if ( !bITEM_SPACE )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_INVEN;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

        // 구입을 수행합니다.
        CheckMoneyUpdate( GetInvenMoney(), llPrice, FALSE, "ReBuy From Npc." );

		SubtractInvenMoneyServer( llPrice, SUBTRACT_INVEN_MONEY_ITEM_REBUY_NPC );	// 돈을 지불.

		// 통계 및 로그 수정
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_REBUY );
		// 통계 및 로그 수정
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

        // 아이템을 인벤에 넣어준다.
        // 겹침가능 아이템의 경우 InsertPileItem 안에서 같은 아이템이 있으면 개수 채워준다. 
		// 개수 다 채워줘도 남을 경우에만 신규로 아이템을 생성한다. 신규 아이템이 생성된다. 
		// 아이템 젠타입은 EMGEN_REBUY 이다. 
		// 겹침아이템의 경우 상점에 팔면 디비에 아이템 테이블에 인벤타입이 0으로 업데이트 되었지만(일반 아이템과 동일한 로직으로)
		// 아래 일반 아이템 처리방식처럼 디비 업데이트 하지는 않는다. 겹침아이템이 인벤에 들어갈 때는 위에서 설명한 것처럼 기존 아이템에
		// 개수가 추가되거나 신규로 아이템이 생성되는 방식이다. 그래서 실제는 db::DB_INSERT 된다.

		// 아이템의 소유 이전 경로 기록.
		SITEMCUSTOM sNewItem( sNID );
		sNewItem.cGenType = EMGEN_REBUY;
		sNewItem.wTurnNum = wREQINSRTNUM;
		sNewItem.tBORNTIME = sItemCustom.tBORNTIME;	// rm #1438 : 기간제 아이템만 우선 적용해본다. 20151223 khkim 생성시간은 기본으로 넣어준다.

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

			// 통계 및 로그 수정
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
        //	일반 아이템의 경우.
        WORD wInsertPosX( 0 ), wInsertPosY( 0 );
        bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

        //	인벤에 여유공간이 없을 경우 취소됨.
        if ( !bITEM_SPACE )
        {
            hr = E_FAIL;
            NetMsgFB.emFB = EMBUYREBUYITEM_NOT_ENOUGH_INVEN;
            NetMsgFAB.emFB = NetMsgFB.emFB;

            SendToClient( &NetMsgFB );
            SendToAgent( &NetMsgFAB );

            return hr;
        }

        // 새로운 아이템을 생성합니다.
		// 확장성을 고려해서, 펫번호나 탈것번호를 아이템의 guid 로 사용할 경우를 대비해서 겹침불가 아이템은 guid 그대로 사용하도록 하자.
		// 복사생성자에서 guid 들어간다. guid 는 새로 생성하지 않는다.
        SITEMCUSTOM sNewItem( sItemCustom );
        sNewItem.tBORNTIME = sItemCustom.tBORNTIME;
        sNewItem.wTurnNum = sItemCustom.wTurnNum;
        sNewItem.cGenType = EMGEN_REBUY;

		// 이미 디비 아이템 테이블에 있는 아이템이다. 상점에 판매될 때 인벤타입만 0으로 변경이 되었다.
		// 업데이트가 필요한 아이템이다. 인벤에 다시 삽입되고, 디비에서는 InvenType 이 0 에서 인벤토리로 변경된다.
		sNewItem.SetDbState( db::DB_UPDATE );

        // 돈을 지불합니다.
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

        // 인벤토리에 넣어줍니다
		if ( !InvenInsertItem( sNewItem, wInsertPosX, wInsertPosY, true, INSERT_INVEN_REBUY_ITEM, true, false, true ) )
		{
			return S_OK;
		}

        // 아이템의 소유 이전 경로 기록
		// 통계 및 로그 수정
        LogItemExchange(
            sNewItem,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_SHOP, sNewItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_REBUY,
			true );

        //	돈 로그.
		// 통계 및 로그 수정
        LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_REBUY );
		// 통계 및 로그 수정
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
            // 클럽의 수익 발생.
            std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
            if ( pCLUB )
            {
                pCLUB->AddProfitMoeny( llCommission );

                // 클럽 수입이 1억 이상일 경우 로그를 남긴다.
                if ( llCommission > 	EMCLUBINCOMEMONEY_LOG )
                {   
					// 통계 및 로그 수정
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

    //	금액 변화 메시지.
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
// Desc: NPC 에게 아이템 판매
// *****************************************************
HRESULT GLChar::MsgReqSaleToNpc( NET_MSG_GENERIC* nmg )
{
	// 현재 아이템 먼저 삭제하고 돈을 넣어주는게 아닌 돈을 먼저 넣어주고 아이템을 삭제하고 있다.
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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
			// 상점 NPC 유효성 체크
			GLCrow* pTCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
			if (!pTCrow)
				return E_FAIL;

			if ( !pTCrow->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )	return E_FAIL;

			pCrow = dynamic_cast<ServerActor*>(pTCrow);
		}
		break;
	case CROW_SUMMON :
		{
			// 상점 NPC 유효성 체크
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
		//	상인 NPC가 존제하지 않습니다.
		return E_FAIL;
	}

	if (!pCrowData->IsBasicTalk( SNpcTalk::EM_MARKET ) )
	{
		//	상인이 아닙니다.
		return E_FAIL;
	}

	SITEMCUSTOM sITEM;
    bool        bHold = false;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

    if ( pNetMsg->wPosX == USHRT_MAX || pNetMsg->wPosY == USHRT_MAX )
    {
        if ( !pHoldInvenItem )
        {
            //	팔려고 하는 아이템이 없습니다.
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
            //	팔려고 하는 아이템이 없습니다.
            return E_FAIL;
        }

        sITEM = pInvenItem->sItemCustom;
    }

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( pItem == NULL )						
        return E_FAIL;

	// 팻아이디(DB접근용)
	DWORD dwPetID = sITEM.PetDbNum();
	DWORD dwVehicleID = sITEM.m_VehicleDbNum;

	//	거래옵션
	if ( !( sITEM.IsSale() != NULL ) )
	{
		//	팔기 가능하지 않음.
		return E_FAIL;
	}

//  클라이언트와 서버 아이템 계산 공식이 틀려서 수정함 
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
			// 클럽의 수익 발생.
			std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
			if ( pCLUB )
			{
				pCLUB->AddProfitMoeny( llCommission );

				// 클럽 수입이 1억 이상일 경우 로그를 남긴다.
				if ( llCommission > EMCLUBINCOMEMONEY_LOG )
                {
					// 통계 및 로그 수정
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

	//	금액 변화.
	CheckMoneyUpdate( GetInvenMoney(), llDisPRICE2, TRUE, "Sale To Npc." );

	AddInvenMoneyServer( llDisPRICE2, ADD_INVEN_MONEY_ITEM_SELL_TO_NPC );
    
	//	돈 로그.
	if ( llDisPRICE2 > EMMONEY_LOG )
	{   
		// 통계 및 로그 수정
		LogMoneyExchange( 
			gamelog::ID_CHAR, 
			0,
			gamelog::ID_CHAR, 
			m_CharDbNum, 
			llDisPRICE2, 
			ITEM_ROUTE_CHAR, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), 
			LOG_MONEY_SELL_TO_NPC );

		// 통계 및 로그 수정
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

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange(
        sITEM,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_SHOP, sITEM.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_SELL_TO_NPC,
		false );

    //	금액 변화 메시지.
    SendInvenMoneyUpdate();

	//	아이템 삭제.
    if ( bHold )
    {
		SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
		WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
		WORD wHoldPosY = sHold_BackUp.wPosY;

        ItemSaleToNpc( wHoldPosX, wHoldPosY, true, sHold_BackUp.CustomItem().bVietnamGainItem, true );
    }
    else
    {
        // 인벤 아이템 제거
        ItemSaleToNpc( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, true );
    }

    // 재매입 리스트에 추가.
    {
        volatile LONGLONG llRebuyPirce = llDisPRICE2;
        if ( llRebuyPirce < GLCONST_CHAR::wMinRebuyPrice )
            llRebuyPirce = GLCONST_CHAR::wMinRebuyPrice;

        volatile LONGLONG llFinalRebuyPirce = static_cast< LONGLONG >( llRebuyPirce * GLCONST_CHAR::fRebuyMultiplePrice );

        GLMSG::SNET_ADD_REBUY_ITEM_FA NetAddRebuyItem( sITEM, llFinalRebuyPirce );
        NetAddRebuyItem.dwChaNum = CharDbNum();
        SendToAgent( &NetAddRebuyItem );
    }

	// 팻카드일 경우 아이템을 상점에 팔면 팻DB 삭제
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && dwPetID != 0 )
	{
		// 팻을 사라지게 해준다.
		m_pGaeaServer->ReserveDropOutPet( SDROPOUTPETINFO( m_dwPetGUID, true, false ) );
        AddGameAdoJob(
            db::DbActionPtr(
                new db::PetDelete( m_CharDbNum, dwPetID ) ) );

		// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
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

	// 폭죽 사용.
	//
	GLMSG::SNETPC_REQ_FIRECRACKER_BRD NetMsgBrd;
	NetMsgBrd.nidITEM = sCUSTOM.GetNativeID();
	NetMsgBrd.vPOS = pNetMsg->vPOS;
	SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsgBrd );

	SendToClient(&NetMsgBrd);

	// 폭죽 사용을 알림.
	//
	NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_OK;
	SendToClient( &NetMsgFB );

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	// 아이템 소모.
	//
	ConsumeInvenItem( wHoldPosX, wHoldPosY );

	return S_OK;
}

HRESULT GLChar::MsgInvenVietnamGet( NET_MSG_GENERIC* nmg )
{
	// 사용하지 않는다. 삭제 처리
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

	// Note : 레벨 조건 확인
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

    // 체력 회복 아이템을 사용할수 없는 상황이다.
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
			// 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER != USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			// 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );
		}
	}

	// pk 등급이 살인마 등급 이상일 경우 회복약의 사용을 막는다.
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

			// Note : 주인의 피가 최대치가 아니면 사용 가능.
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

				//	상태이상 지속시간을 종료시킴.
				m_sSTATEBLOWS[i].fAGE = 0.0f;
			}

			//	상태이상 치료됨을 자신에게 알림.
			NetMsgBrd.emCrow = CROW_PC;
			NetMsgBrd.dwID = m_dwGaeaID;
			NetMsgBrd.dwCUREFLAG = pItem->sDrugOp.dwCureDISORDER;
			SendToClient( &NetMsgBrd );

			//	상태이상 치료됨을 모두에게 알림.
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

	// 아이템 소모.
	//
	ConsumeInvenItem( PosX, PosY );

	// 체력 수치 변화를 [자신,파티원,주위]의 클라이언트들에 알림.
	MsgSendUpdateState();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenLunchBox( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNETPC_REQ_INVEN_LUNCHBOX* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_LUNCHBOX* ) nmg;

	GLLandMan* const pLand( GetLandMan() );
	// Note : 도시락 사용 불가능 지역 확인
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

	// Note : 레벨 조건 확인
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

	// Note : 스킬이 존재하지 않는다면 실패
	//
	if ( !pSkill )
	{
		GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB	NetMsgFB;
		NetMsgFB.emFB = EMLUNCHBOX_FB_FAILED;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	// Note : 중복된 SkillFact를 제거한다
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

		// 제거됨을 알림
		GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB NetHoldResetFB;
		NetHoldResetFB.emCrow     = CROW_PC;
		NetHoldResetFB.dwID       = m_dwGaeaID;
		NetHoldResetFB.sSKILLID   = sSKILL_ID;
		NetHoldResetFB.wRESETSLOT = wOverwriteSlot;

		SendToClient( &NetHoldResetFB );

		// Note : 제거됨을 주변에게 알림
		GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgBRD;
		NetMsgBRD.emCrow                 = CROW_PC;
		NetMsgBRD.dwID                   = m_dwGaeaID;
		NetMsgBRD.emResetType			 = EMSKILLHOLDRESET_REQ;
		NetMsgBRD.bRESET[wOverwriteSlot] = true;
		SendMsgViewAround( &NetMsgBRD );
	}

	// Note : 최대 레벨보다 크다면 최대 레벨로 적용
	//
	if ( wSKILL_LEVEL >= pSkill->m_sBASIC.dwMAXLEVEL )
	{
		wSKILL_LEVEL = ( WORD )pSkill->m_sBASIC.dwMAXLEVEL;
	}

	// 스킬적용.
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
		// 발동된 스킬 값 MSG.
		//
        GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
        NetMsgBRD.emCrow    = CROW_PC;
        NetMsgBRD.dwID      = m_dwGaeaID;
        NetMsgBRD.skill_id  = sSKILL_ID;
        NetMsgBRD.wLEVEL    = wSKILL_LEVEL;
        NetMsgBRD.wSELSLOT  = static_cast< WORD >( dwSELSLOT );
        NetMsgBRD.sID       = STARGETID( CROW_PC, m_dwGaeaID ); //스킬 시전자는 자기자신

		// 스킬대상과 주변 클라이언트들에게 메세지 전송.
		SendToClient( &NetMsgBRD );
		SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );
	}

	// 소모성 아이템 제거.
	//
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

	// 도시락 사용 성공.
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

	// Note : 저장한다.
	//
	SaveSkillFact();

	return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxInfo( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_GET_CHARGED_ITEM_CF* pPacket = ( GLMSG::NET_GET_CHARGED_ITEM_CF* ) nmg;
	
	// DB에서 빌링아이템 정보 가져오기
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// RM #840 아이템 구매조건에 특별활동 및 기여도를 없애서 일부 아이템이 비정상적으로 사용되어서 수정 (박스, 코스튬 등)
	if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	활동 포인트가 부족합니다.
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_ERR_ACTIVITY;
		SendToClient( &NetMsgFB );
        return E_FAIL;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	기여 포인트가 부족합니다.
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION;
		SendToClient( &NetMsgFB );
        return E_FAIL;
    }

	// 인벤의 여유 공간 측정.
	//
	// 불필요한 Assign 삭제 
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
// 			// 인벤에 공간이 없는 것으로 판단됨.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenBoxOpen", m_CharDbNum ) );
	}

	// 상자안에 있는 아이템을 꺼내어서 넣는다.
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

		// 아이템 생성.
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

		// 아이템 에디트에서 입력한 개조 등급 적용
		sITEM_NEW.cDAMAGE = ( BYTE )pITEM->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pITEM->sBasicOp.wGradeDefense;

		//	랜덤 옵션 생성.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
            LogItemRandomOption( sITEM_NEW );

		// 인벤에 넣을 위치 찾음.
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
		if ( !bOk )
            return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

		// #item
		//	생성한 아이템 Inventory에 넣음. pInsertItem NULL 이면 서버 crash
		// InvenInsertItem 실패했을 때 처리 필요
		InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, 1, true, false, true );
		SINVENITEM* pInsertItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

		// 통계 및 로그 수정
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

	//	상자 삭제 로그.
	// 통계 및 로그 수정
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
		// 아이템 제거.
		InvenDeleteItem( wPosX, wPosY, false, FALSE, 1, true );

		//	[자신에게] Inventory에 아이템 제거.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
		NetMsg_Inven_Delete.dwChaNum = CharDbNum();
		SendToClient( &NetMsg_Inven_Delete );
	}
	else
	{
		//	[자신에게] Inventory 아이템 소모됨.
		GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
		NetMsg_Inven_Update.dwChaNum = CharDbNum();
		NetMsg_Inven_Update.wPosX = wPosX;
		NetMsg_Inven_Update.wPosY = wPosY;
		NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
		SendToClient( &NetMsg_Inven_Update );
	}

	// 상자를 성공적으로 열었음
	NetMsgFB.emFB = EMREQ_BOXOPEN_FB_OK;
	SendToClient( &NetMsgFB );

	return S_OK;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청
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
	// 빠르게 요청할 경우, 이미 처리된 메세지 온다;
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

    // CacheServer에 확률제어 데이터 등록 여부를 확인한다.
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
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
 * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용(MsgReqInvenRandomBoxOpenFB 함수 파라미터 변경)
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
	{   // 인벤토리의 해당 위치에 아이템이 없으면 오류
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
    {   // 인벤토리 해당 위치의 ItemID가 요청 BoxId가 아니면 오류
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
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenRandomBoxOpenFB", m_CharDbNum ) );
	}
    
	if ( sGENITEM.nidITEM == SNATIVEID( false ) )
	{
		//	상자 아이템 삭제 로그 남김.
		// 통계 및 로그 수정
		LogItemExchange( 
            pInvenItem->sItemCustom, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_RANDOMBOX,
			false );

		// 상자 아이템 삭제
        InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

		// 처리 결과 전송.
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

	// 아이템 발생.
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

	// 아이템 생성.
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

	// 아이템 에디트에서 입력한 개조 등급 적용
	sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
		LogItemRandomOption( sITEM_NEW );

	//	생성한 아이템 Inventory에 넣음.
	InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	SINVENITEM *pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem )
		return E_FAIL;

	//	[자신에게] 메시지 발생
    InvenInsertSend( pInsertItem, FALSE, false, true );

	//	상자 아이템 삭제 로그 남김.
	// 통계 및 로그 수정
	LogItemExchange( 
        pInvenItem->sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_RANDOMBOX,
		false );

    sItemBox = pInvenItem->sItemCustom;

	//	랜덤 아이템 생성 로그 남김.
	// 통계 및 로그 수정
	LogItemExchange( 
        pInsertItem->sItemCustom, 
        gamelog::ID_CHAR, 0, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_RANDOMBOX,
		false );

    sItemInBox = pInsertItem->sItemCustom;

	// 밑에 아이템 제거하는 로직 ConsumeInvenItem 으로 대체할 수 있다. 소모성 아이템이 아니라면?
	
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
		// 아이템 제거.
		InvenDeleteItem( wPosX, wPosY, false, FALSE, 1, true );

		//	[자신에게] Inventory에 아이템 제거.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete( wPosX, wPosY, FALSE );
		NetMsg_Inven_Delete.dwChaNum = CharDbNum();

		SendToClient( &NetMsg_Inven_Delete );
	}
	else
	{
		//	[자신에게] Inventory 아이템 소모됨.
		GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
		NetMsg_Inven_Update.dwChaNum = CharDbNum();
		NetMsg_Inven_Update.wPosX = wPosX;
		NetMsg_Inven_Update.wPosY = wPosY;
		NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;

		SendToClient( &NetMsg_Inven_Update );
	}

	// 상자를 성공적으로 열었음
	NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_OK;
	SendToClient( &NetMsgFB );

    if ( bRandomboxChance )
    {
        NetMsgEND.bComplete = true;
        NetMsgEND.nIndexAll = pNetMsg->nIndexAll;
        NetMsgEND.nIndexCha = pNetMsg->nIndexCha;
        SendToCache( &NetMsgEND );

        // 여기서 DB에 Randombox 확률제어 완료로그를 DB로 저장
        AddLogAdoJob( db::DbActionPtr(new db::LogRandomboxChanceComplete( pNetMsg->sBoxID,         /* RandomBox ID */
                                                                          pNetMsg->sItemID,        /* 당첨 Item ID */
                                                                          pNetMsg->nReqCount,      /* 목표 오픈 수량 */
                                                                          pNetMsg->dwTargetChaNum, /* 목표 캐릭터 번호 */
                                                                          pNetMsg->dwGMNum,        /* 등록 GM Number */
                                                                          pNetMsg->nOpenCount,     /* 실제 오픈 수량 */
                                                                          pNetMsg->dwChaNum        /* 당첨자 */
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

	// Note : 레벨 조건 확인
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

	// 인벤 여유공간 확인;
	WORD wINSERTX( 0 ), wINSERTY( 0 );
	BOOL bOK = m_cInventory.FindInsertable( wINSERTX, wINSERTY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMINVEN_SELFORMBOX_FB_INVENNOSPACE;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 선택아이템 생성추가;
	SITEMCUSTOM sITEM_NEW( sBoxItem.nidITEM );
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();
	sITEM_NEW.wTurnNum = sBoxItem.dwAMOUNT;
	// 타입 음;
	sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	LogItemBasicStatOption( sITEM_NEW );
	LogItemLinkSkillOption( sITEM_NEW );
	LogItemAddonOption( sITEM_NEW );

	// 아이템 에디트에서 입력한 개조 등급 적용
	sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
		LogItemRandomOption( sITEM_NEW );

	//	생성한 아이템 Inventory에 넣음.
	InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	SINVENITEM *pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem )
		return E_FAIL;

	//	[자신에게] 메시지 발생
	InvenInsertSend( pInsertItem, FALSE, false, true );

	//	상자 아이템 삭제 로그 남김.
	// 통계 및 로그 수정
	LogItemExchange( 
		pInvenItem->sItemCustom, 
		gamelog::ID_CHAR, m_CharDbNum, 
		gamelog::ID_CHAR, 0, 
		ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_SELECTBOX,
		false );

	ActivityCheckItemUse( pItem->sBasicOp.sNativeID.dwID );
	DoQuestItemUse( pItem->sBasicOp.sNativeID );

	// 상자 아이템 삭제
	InvenDeleteItem( wBoxPosX, wBoxPosY, true, FALSE, 1, true );

	// 상자를 성공적으로 열었음
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

	// 코스튬 기간 연장 값 이전;
	sITEM_NEW.SetPeriodExBasic(pInvenItem->sItemCustom.GetPeriodExDisguise());
	sITEM_NEW.cPeriodExtensionCount = pInvenItem->sItemCustom.cCostumePeriodExtensionCount;
	sITEM_NEW.ResetPeriodExDisguise();

	// 코스튬 염색값 옮김. by luxes.
	//
	sITEM_NEW.dwMainColor = pInvenItem->sItemCustom.dwMainColor;
	sITEM_NEW.dwSubColor = pInvenItem->sItemCustom.dwSubColor;

	// 코스튬 능력치 옮김
	sITEM_NEW.costumeUser = pInvenItem->sItemCustom.costumeUser;

	// 아이템 생성.
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = 1;
	sITEM_NEW.cGenType = EMGEN_BILLING;
	sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	// hold 아이템 인벤에 되돌림.
	WORD wInsertPosX, wInsertPosY;
	BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_NOINVEN;
		SendToClient( &NetMsgFB );
		return E_FAIL;	//	인벤 공간 부족으로 되돌림 실패.
	}

	//	들고있던 아이템 Inventory에 넣음.
	InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, 1, true );
	SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

	// 망각의 세탁을 함.
	//
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );

	// 코스튬 염색값 지움. by luxes.
	//
	pInvenItem->sItemCustom.dwMainColor = 0;
	pInvenItem->sItemCustom.dwSubColor = 0;

	// 코스튬 능력치 지움
	pInvenItem->sItemCustom.ResetInvest();

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenDisJunction", m_CharDbNum ) );
	}

	// 분리된 코스툼 로그.
	// 통계 및 로그 수정
	LogItemExchange( 
        pINSERT_ITEM->sItemCustom, 
        gamelog::ID_CHAR, 0, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_DISJUNCTION,
		false );

	// 일반 복장 아이템이 세탁하는 것을 로그 남김
	LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_DISJUNCTION );

    //SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    //WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    //WORD wHoldPosY = sHold_BackUp.wPosY;

	// 아이템 소모.
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
// Desc: 코스튬 입힘
// *****************************************************
HRESULT GLChar::MsgReqInvenDisguise( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_DISGUISE* pNetMsg = ( GLMSG::SNET_INVEN_DISGUISE* ) nmg;

	GLMSG::SNET_INVEN_DISGUISE_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// 유효한 인벤토리 인지 검사
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// 유효한 아이템인지 검사
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

	// 손에 들고 있는 아이템이 유효한지 검사
	const SITEMCUSTOM& sITEM_HOLD = pHoldInvenItem->CustomItem();
	if ( sITEM_HOLD.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 손에든 아이템의 정보 갖고 오기
    SITEM* pHold = GLogicData::GetInstance().GetItem( sITEM_HOLD.Id() );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 손에든 아이템이 코스튬인지 검사
	if ( !( sITEM_HOLD.IsDisguise() != NULL ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NODISGUISE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// RM #840 아이템 구매조건에 특별활동 및 기여도를 없애서 일부 아이템이 비정상적으로 사용되어서 수정 (박스, 코스튬 등)
	if ( !CHECK_ACTIVITY_POINT( *pHold ) ) 
    {
        //	활동 포인트가 부족합니다.
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ERR_ACTIVITY;
		SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pHold ) ) 
    {
        //	기여 포인트가 부족합니다.
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

	// 같은 클래스 인지 검사
	if ( ( pHold->sBasicOp.dwReqCharClass & pItem->sBasicOp.dwReqCharClass ) == NULL )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 복장류인지 검사
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

	// 같은 종류의 복장인지 검사
	if ( pHold->sSuitOp.emSuit != pItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 인벤토리의 복장이 코스튬이 입혀져 있는지 검사
	if ( pInvenItem->sItemCustom.nidDISGUISE != SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 겹침 아이템의 경우 코스튬 사용 불가
	if ( pItem->ISPILE() == true || pInvenItem->sItemCustom.wTurnNum > 1 )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_PILED;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 시한 코스툼일 경우.
	if ( pHold->sDrugOp.tTIME_LMT != 0 )
	{
		__time64_t &tTIME = pInvenItem->sItemCustom.tDISGUISE;
		CTime tCurTime = sITEM_HOLD.tBORNTIME;
		CTime tSeedTime( tTIME );
		CTimeSpan tLMT( pHold->sDrugOp.tTIME_LMT );

		// 코스툼 5분증가 제거 ( CRM #1601 );
		// http://crm.mincoms.com/Ticket/TicketView/1601

        // 코스튬장비에부착시 만료일 5분증가 수정요청
        // http://crm.mincoms.com/Ticket/TicketView/1480
        // 코스튬이 부착된 장비에 분리용세제 사용 시, 시효성 증가 문제 확인 요청 CRM #1480
		//tLMT += CTimeSpan(0,0,05,0);

		// 시간 충전.
		tSeedTime = tCurTime + tLMT;

		tTIME = tSeedTime.GetTime();
	}
		
	// 코스툼 스킨 지정.
	//
	pInvenItem->sItemCustom.nidDISGUISE = pHold->sBasicOp.sNativeID;

	// 코스튬 염색값 저장. by luxes.
	//
	pInvenItem->sItemCustom.dwMainColor = sITEM_HOLD.dwMainColor;
	pInvenItem->sItemCustom.dwSubColor = sITEM_HOLD.dwSubColor;

	// 코스튬 능력치 옮김.
	pInvenItem->sItemCustom.costumeUser = pHoldInvenItem->sItemCustom.costumeUser;
	pInvenItem->sItemCustom.cCostumePeriodExtensionCount = pHoldInvenItem->sItemCustom.cPeriodExtensionCount;
	pInvenItem->sItemCustom.SetPeriodExDisguise(pHoldInvenItem->sItemCustom.GetPeriodExBasic());

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	// 일반 복장 아이템이 코스툼 스킨으로 변환되는 값 로그 남김.
	LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COUSUME_ADD_BY_ITEM );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenDisguise", m_CharDbNum ) );
	}

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange( 
        sITEM_HOLD, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, 0, 
        ITEM_ROUTE_DELETE, sITEM_HOLD.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_COSTUME_ON_ITEM,
		false );

	// 인벤 홀드 아이템 삭제.
	//
	InvenDeleteItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, true, pHoldInvenItem->CustomItem().bVietnamGainItem, INSERT_INVEN_ITEM_IVEN_DISGUISE, true );

	// 발라진 아이템 저장?

	// 코스툼 스킨으로 변환 성공 알림.
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

	// 복장 세탁을 함
    SNATIVEID CostumeId = pInvenItem->sItemCustom.nidDISGUISE;
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );
	pInvenItem->sItemCustom.ResetPeriodExDisguise();

	// 코스튬 염색값 지움. by luxes.
	pInvenItem->sItemCustom.dwMainColor = 0;
	pInvenItem->sItemCustom.dwSubColor = 0;

	// 코스튬 능력치 지움
	//pInvenItem->sItemCustom.ResetInvest();

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );
	
    // 로그
    // 1. 삭제된 코스튬 아이템
    // 2. 삭제된 세탁제
    // 3. 코스튬이 삭제된 복장 아이템
    SITEMCUSTOM DeletedCostume( CostumeId );
    //SITEMCUSTOM DeletedCleanser( pHold->BasicId() );	// guid 가 없기 때문에 로그에 사용불가. pHoldInvenItem 으로 로그 남긴다.
    
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenCleanser", m_CharDbNum ) );
	}

	// 통계 및 로그 수정
    LogItemExchange( 
        pHoldInvenItem->CustomItem(), 
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, 0, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_CLEANSER_BY_USE,
		true );

	// 통계 및 로그 수정, 아이템에 붙어 있던 코스튬인데 분리가 되지 않고 바로 삭제가 되었기 때문에 guid 가 없다. 0000-0000-0000-0000 이렇게 아이템 guid 가 로그에 남을거다.
    LogItemExchange(
        DeletedCostume,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, 0, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_COSTUME_BY_CLEANSER,
		true );
	
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_CLEANSER );

	// 아이템 소모
	ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

	// 코스툼 스킨 리셋 변환 성공 알림
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

	// 가지고 있는 돈의 액수를 조절.
	CheckMoneyUpdate( GetInvenMoney(), pPacket->lnMoney, FALSE, "Drop Money." );

	SubtractInvenMoneyServer( pPacket->lnMoney, SUBTRACT_INVEN_MONEY_DROP_GROUND );

	// 금액 로그.
	if (pPacket->lnMoney > EMMONEY_LOG)
	{
		// 통계 및 로그 수정
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -pPacket->lnMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_DROP_INVNE_TO_FIELD);
		// 통계 및 로그 수정
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

	// 바닥에 떨어트린다.
	pLand->DropMoney(vCollisionPos, pPacket->lnMoney, FALSE);

	// 클라이언트에 돈 액수 변화를 알려줌.
	SendInvenMoneyUpdate();
	
	return S_OK;
}

// 잠금 상자 열기 시도;
// - sBox - 열쇠 리스트;
// - sRendomBox - 잠금 상자에서 나올 아이템;
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

	if ( !pInvenItem->sItemCustom.bVietnamGainItem ) // 잠금이 풀려있다면;
	{
		MsgFB.emFB = EMINVEN_LOCKBOX_FB_UNLOCK_ALREADY;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// 열쇠 리스트에 열쇠가 있는지 본다;
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

		if ( pHoldInvenItem->sItemCustom.wTurnNum < nNeedAmount )		// 열쇠 개수가 모자르다면;
		{
			MsgFB.emFB = EMINVEN_LOCKBOX_FB_SHORT_KEY;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		// 소모된 연마제 처리;
		ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, false, nNeedAmount );
		pInvenItem->sItemCustom.bVietnamGainItem = false;

		// 아이템 상태가 바뀌었음으로 아이템 정보를 전송해야한다;
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// bVietnamGainItem == false 여야 열려있는것;
	const SITEMCUSTOM& sCustomItem = pInvenItem->sItemCustom;
	if ( sCustomItem.bVietnamGainItem == true )
	{
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_LOCK;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 이 이후 아래는 랜덤박스와 거의 동일하다;
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
			// 통계 및 로그 수정
			// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenLockBoxOpen", m_CharDbNum ) );
		}

		if ( sGENITEM.nidITEM == SNATIVEID( false ) )
		{
			//	상자 아이템 삭제 로그 남김.
			// 통계 및 로그 수정
			LogItemExchange( 
				pInvenItem->sItemCustom, 
				gamelog::ID_CHAR, m_CharDbNum, 
				gamelog::ID_CHAR, 0, 
				ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_LOCKED_BOX_FAIL,
				false );

			// 상자 아이템 삭제
			InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

			// 처리 결과 전송.
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

		// 아이템 발생.
		WORD wINSERTX( 0 ), wINSERTY( 0 );
		BOOL bOK = m_cInventory.FindInsertable( wINSERTX, wINSERTY );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_FAIL;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// 아이템 생성.
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

		// 아이템 에디트에서 입력한 개조 등급 적용
		sITEM_NEW.cDAMAGE = ( BYTE )pITEM_DATA->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pITEM_DATA->sBasicOp.wGradeDefense;

		//	랜덤 옵션 생성.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );

		//	생성한 아이템 Inventory에 넣음.
		InvenInsertItem( sITEM_NEW, wINSERTX, wINSERTY, false, 1, true, false, true );
		SINVENITEM* pInsertItem = m_cInventory.GetItem( wINSERTX, wINSERTY );

		if ( !pInsertItem )
			return E_FAIL;

		//	[자신에게] 메시지 발생
		InvenInsertSend( pInsertItem, FALSE, false, true );

		//	상자 아이템 삭제 로그 남김.
		// 통계 및 로그 수정
		LogItemExchange( 
			pInvenItem->sItemCustom, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, 0, 
			ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_LOCKED_BOX,
			false );

		//	랜덤 아이템 생성 로그 남김.
		// 통계 및 로그 수정
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

		// 상자 아이템 삭제
		InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

		// 상자를 성공적으로 열었음
		NetMsgFB.emFB = EMINVEN_LOCKBOX_FB_OPEN_OK;
		NetMsgFB.nidITEM = sGENITEM.nidITEM;
		SendToClient( &NetMsgFB );
	}

	return S_OK;
}

// *****************************************************
// Desc: 아이템 인첸트 요청 처리
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
		BYTE ResultGrade = 0; // 연마결과수치

		DOTLGRINDING( pInvenItem->sItemCustom, pHold, ResultGrade, bReEnchant );

		if ( ResultGrade > 0 )
		{
			// 바로 디비에 적용한다.
			InvenUpdateItem( pInvenItem );

			ConsumeInvenItem( wHoldPosX, wHoldPosY, false, 1 );

			// 로그 기록.
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

	// 연마제 필요 수량
	WORD wGradeNum = GRINDING_CONSUME_NUM( pInvenItem->sItemCustom, pHold, sHOLDITEM );	
	
	if ( wGradeNum == 0 )
		return E_FAIL;

	if ( wGradeNum > sHOLDITEM.wTurnNum )
		return E_FAIL;

	// 연마 수행.
	// Note : 강화 옵션 아이템 검사
	DWORD dwGrindingOption( EMGRINDINGOPTION_NONE );
    DWORD dwSlotOption[SLOT_NSIZE_S_2] = { 0 };

	DWORD dwGrindingLogOption = 0;

	SITEM* pITEM = NULL;
	{
		// Note : 현재로썬 목과 팔찌만 검사한다.
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
						// Note : 소방주
					case ITEM_ANTI_DISAPPEAR:    
						dwOption |= EMGRINDINGOPTION_ANTIDISAPPEAR;    
						dwGrindingLogOption = 1;
						break;
						// Note : 강화주
					case ITEM_INC_GRINDINGRATE:  
						dwOption |= EMGRINDINGOPTION_INC_GRINDINGRATE; 
						dwGrindingLogOption = 2;
						break;
						// Note : 완화주
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

	// Note : 강화주와 완화주는 4미만 9이상 일때는 취소 한다.
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

    BYTE ResultGrade = 0; // 연마결과수치
	DWORD dwGrindingResult = DOGRINDING( pInvenItem->sItemCustom, pHold, sHOLDITEM, dwGrindingOption, ResultGrade );

	bool bRESET		= ( dwGrindingResult & EMGRINDINGRESULT_RESET ) ? true : false;
	bool bTERMINATE	= ( dwGrindingResult & EMGRINDINGRESULT_TERMINATE ) ? true : false;

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	// 소모된 연마제 처리.
    ConsumeInvenItem( wHoldPosX, wHoldPosY, false, wGradeNum );

	//소모된 아이템 처리.
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
 				// Note : 사용된 효과는 리셋한다.
 				//		  중복 차감이 안되게 하기 위해서.
 				dwUsingOption &= ~dwSlotOption[i];
 				ConsumeSlotItem( EMSLOT( i ) );
 			}
 		}
 	}

	// 로그 기록.
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ( EMITEM_ROUTE )ITEM_ROUTE_NORMAL_GRINDING, ( EMITEM_ROUTE_DETAIL )( LOG_ITEM_NORMAL_GRINDING + dwGrindingLogOption ) );

    if ( dwGrindingResult == EMGRINDINGRESULT_SUCCESS && ResultGrade > 9 && m_ClubDbNum != CLUB_NULL )
    {
        // [%1%]님이 [%2%]의 [%3%] %4%연마에 성공하셨습니다.
        std::string ClubLog( 
            sc::string::format( 
                ID2GAMEINTEXT( "CLUB_LOG_22" ),
                m_szName,
                pInvenItem->sItemCustom.GETNAME().c_str(),
                ID2GAMEWORD( "ITEM_ADVANCED_INFO", pHold->sGrindingOp.emTYPE ),
                static_cast< int >( ResultGrade ) ) );
        m_pGaeaServer->SENDCLUBLOG( m_ClubDbNum, club::LOG_ENCHANT, ClubLog );        
    }

	// 연마 성공 여부 반환.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenGrinding", m_CharDbNum ) );
	}

	// 아이템의 속성값 클라이언트에 알림.
	//
	if ( !bTERMINATE )
	{
		// 바로 디비에 적용한다.
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
		파괴될 경우 아이템 삭제 먼저하고 밑에서 코스튬 처리 한다. 이게 되나? 테스트 필요
		거래 불가 코스튬을 입혀놓고 인벤 공간을 0으로 해놓고 연마해서 아이템 파괴될 경우 코스튬이 분리되서 바닥에 떨어뜨려서 누군가 주스면 되었다.
		그래서 같이 파괴되도록 되어있나?
		기획 문의 결과
		아이템이 파괴될 경우 거기에 붙어있는 아이템은
		삭제된 그 자리에 넣어준다.
		*/

		// 삭제 후에 처리해야 되기 때문에 복사한다.
		SITEMCUSTOM sRestoredCostume( pInvenItem->sItemCustom );

		//	[자신에게] Inventory에 아이템 제거
		InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, DELETE_ITEM_GRINDING_TERMINATE, true );

		// 만약 코스튬이 발라져 있으면 코스튬을 분리하고 삭제한다.
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

			// 아이템 생성.
			//
			sITEM_NEW.SetPeriodExBasic( sRestoredCostume.GetPeriodExDisguise() );
			sITEM_NEW.cPeriodExtensionCount = sRestoredCostume.cCostumePeriodExtensionCount;
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.wTurnNum = 1;
			sITEM_NEW.cGenType = EMGEN_BILLING;
			sITEM_NEW.costumeUser = sRestoredCostume.costumeUser;	// rm #4525 아이템 소멸시 코스튬 능력치 주입 삭제 현상 수정 요청
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			// 무조건 파괴된 아이템이 있던 자리에 삽입한다.
			WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;
			BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
			if ( !bOk )
			{
				// 위에서 아이템을 삭제하고 그 자리에 넣는건데 자리가 없으면 안될듯
				return E_FAIL;
			}

			// 인벤에 넣음.
			InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, INSERT_ITEM_GRINDING_TERMINATE_RESTORE_COSTUME, true );

			// 분리된 코스툼 로그.
			// 통계 및 로그 수정
			LogItemExchange( 
				sITEM_NEW, 
				gamelog::ID_CHAR, m_CharDbNum, 
				gamelog::ID_CHAR, 0, 
				ITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_COSTUME_TO_FIELD_BY_ERROR,
				false );

			/*
			바닥에 떨주어주면 위에서 말한 문제가 발생할 수 있다. 거래 불가 아이템 드랍 위에 코드로 변경함
			BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
			if ( !bOk )
			{
				// 생성된 코스툼은 바닥에서 다시 떨군다.
				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				pLand->DropItem ( m_vPosition, &( cDropItem.sItemCustom ), EMGROUP_ONE, m_dwGaeaID );

				// 분리된 코스툼 로그.
				// 통계 및 로그 수정
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
				//	들고있던 아이템 Inventory에 넣음.
				m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
				SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

				//	[자신에게] 메시지 발생.
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
	
	// 둘다 적용가능할 수 없다;   
	if ( bBase && bCostume )
	{
		netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_DUPLICATION;
		SendToClient(&netMsgFB);
		return S_FALSE;
	}

	// 일반 아이템과 코스튬은 따로 처리해줘야한다;
	if ( bBase )
	{
		// 시효성 정보가 없다;
		if ( sCustomItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_NORMAL) == false )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// 횟수 오바;
		if ( nBaseMaxCount != 0 && nBaseMaxCount <= sCustomItem.cPeriodExtensionCount )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}

		__time64_t tPrevTime = sCustomItem.tBORNTIME + sCustomItem.GetPeriodExBasic();
		CTime cTimeExcCheck(tPrevTime + pHold->sDrugOp.dwCureVolume);
		// DB 데이터형 smalldate
		if ( cTimeExcCheck.GetYear() >= 2079 )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}
		// 연장 처리;
		// sCustomItem.tBORNTIME += pHold->sDrugOp.dwCureVolume;
		sCustomItem.SetPeriodExBasic( sCustomItem.GetPeriodExBasic() + pHold->sDrugOp.dwCureVolume );
		
		// 0이면 무한가능;
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
		// 시효성 정보가 없다;
		if ( sCustomItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_COSTUME) == false )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// 횟수 오바;
		if ( nCostumeMaxCount != 0 && nCostumeMaxCount <= sCustomItem.cCostumePeriodExtensionCount )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT;
			SendToClient(&netMsgFB);
			return S_FALSE;
		}

		// 코스튬 기간 연장 시 원본 아이템 만료일 보다 기간이 길어질 경우 확인 요청을한다;
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
		// DB 데이터형 smalldate;
		if ( cTimeExcCheck.GetYear() >= 2079 )
		{
			netMsgFB.emResult = EMREQ_PERIODEX_FB_FAIL;
			SendToClient(&netMsgFB); 
			return S_FALSE;
		}

		// 연장 처리;
		// sCustomItem.tDISGUISE += pHold->sDrugOp.dwCureVolume;
		sCustomItem.SetPeriodExDisguise( sCustomItem.GetPeriodExDisguise() + pHold->sDrugOp.dwCureVolume );
		
		// 0이면 무한가능;
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

	// 바로 디비에 적용한다;
	InvenUpdateItem( pInvenItem );

	GLMSG::SNET_INVEN_ITEM_UPDATE netUpdateMsg;
	netUpdateMsg.wPosX = pNetMsg->wPosX;
	netUpdateMsg.wPosY = pNetMsg->wPosY;
	netUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &netUpdateMsg );

	// 인벤 홀드 아이템 삭제;
	// InvenDeleteItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY, true, pHoldInvenItem->CustomItem().bVietnamGainItem, PERIOD_EXTENSION_ITEM, true );
	
	// 소모된 연마제 처리;
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

	// 레벨 조건 확인
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{		
		MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
		SendToClient( &MsgFB );		
		return E_FAIL;
	}

	// Note : 해당 아이템의 타입과 수량 갯수 확인
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
	// Note : 구 망각의 약
	case ITEM_SKP_STAT_RESET_A:
		{
			if ( ITEM_SKP_STAT_RESET_A != pNetMsg->emItemType )
			{
				MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
				SendToClient( &MsgFB );		
				return E_FAIL;
			}

			// Note : 균등하게 감소된 수치를 가져온다.
			GET_EVENLY_DECREASE_STATS( pResetStats, wAmount );
		}
		break;

	// Note : 망각의 신약 (스탯 선택 가능)
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

			// Note : 최대 초기화 되는 갯수를 넘을수 없다.
			if ( wAmount < wNowAmount )
			{
				MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
				SendToClient( &MsgFB );		
				return E_FAIL;
			}
		}
		break;

	// Note : 실패
	default:
		{
			MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	// 배운 스킬들 계산 후 게임머니 지급, 우편으로 지급하면 일일통계 어려움으로 인해 인벤토리로 지급한다.
	// 게임머니 변화 타입 및 아이템익스체인지 타입 추가되었다. 디비 및 웹지엠 수정 필요하다.
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


	// 스킬, 스텟 리셋.
	//
	WORD wPREREMAINDSKILL = ( WORD )m_dwSkillPoint;

	if ( wAmount == USHRT_MAX )
	{
		// Note : 전체 스킬 스텟 초기화
		RESET_STATS_SKILL( NULL );
		MsgFB.bRESETALL = TRUE;
	}
	else
	{
		RESET_STATS_SKILL( pResetStats );
	}

	// [자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// Note : 리셋 정보 세팅
	WORD wRETSTATSNUM = 0;
	WORD wRETSKILLNUM = 0;

	for ( WORD i=0; i<EMSIZE; ++i )
	{
		wRETSTATSNUM += pResetStats[i];
	}

	wRETSKILLNUM = ( WORD )( m_dwSkillPoint - wPREREMAINDSKILL );

	// 리셋 메시지 보냄.
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

    // 유효한 인벤토리 인지 검사
    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    // 유효한 아이템인지 검사
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

    // 손에 들고 있는 아이템이 유효한지 검사
    const SITEMCUSTOM& sITEM_HOLD = pHoldInvenItem->CustomItem();
    if ( sITEM_HOLD.GetNativeID() == SNATIVEID( false ) )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // 손에든 아이템의 정보 갖고 오기
    SITEM* pHold = GLogicData::GetInstance().GetItem( sITEM_HOLD.Id() );
    if ( !pHold )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // 손에든 아이템이 포장지인지 검사
    if ( pHold->sBasicOp.emItemType != ITEM_BOX_WRAPPER )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // 인벤토리 아이템이 포장가능한지 검사;
    if ( pInvenItem->sItemCustom.IsBoxWrappable() == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

    // PetDbNum을 포장지ID 저장 변수로 사용하고 있기 때문에 PetCard는 포장할 수 없다. 추후에 포장지변수를 추가한 후 에는 PetCard도 포장가능하도록 수정 필요;
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

    // 포장지 지정.;
    // 아이템 DB 바이너리 전환 작업 완료후 반드시 아이템 버전을 올려서 새로운 변수에 할당해야 함;
    if ( pInvenItem->sItemCustom.BoxWrapping( sITEM_HOLD.Id() ) == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_WRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	// 디비 업데이트
	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19705 ) ) );
	}
	else
	{
		// 보통 ItemUpdate 사용, 포장인 경우 아이템 아이디가 변경되기 때문에 EntireItemUpdate 사용
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::EntireItemUpdate( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19705 ) ) );
	}

    GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
    NetItemUpdate.wPosX = pNetMsg->wPosX;
    NetItemUpdate.wPosY = pNetMsg->wPosY;
    NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
    SendToClient( &NetItemUpdate );

    // 일반 아이템이 포장된 아이템으로 변환되는 값 로그 남김.;
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_WRAPPING_BY_ITEM );

    GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
    if ( !pTarChar )
    {		
        // 통계 및 로그 수정;
        // 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리;
        sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenBoxWrapping", m_CharDbNum ) );
    }

    // 아이템의 소유 이전 경로 기록;
    // 통계 및 로그 수정;
    LogItemExchange(
        sITEM_HOLD,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, sITEM_HOLD.wTurnNum, 
        ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_WRAPPING,
        false );

    // 인벤 홀드 아이템 삭제.;
    ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

    // 포장 성공 알림.;
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

    // 유효한 인벤토리 인지 검사
    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        NetMsgFB.emFB = EMREQ_BOX_UNWRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }
    // 포장지 지정.;
    // 아이템 DB 바이너리 전환 작업 완료후 반드시 아이템 버전을 올려서 새로운 변수에 할당해야 함;
    if ( pInvenItem->sItemCustom.BoxUnwrapping() == false )
    {
        NetMsgFB.emFB = EMREQ_BOX_UNWRAPPING_FB_FAILED;
        SendToClient( &NetMsgFB );
        return E_FAIL;
    }

	// 디비 업데이트
	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19706 ) ) );
	}
	else
	{
		// 보통 ItemUpdate 사용, 포장인 경우 아이템 아이디가 변경되기 때문에 EntireItemUpdate 사용
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::EntireItemUpdate( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 19706 ) ) );
	}

    GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
    NetItemUpdate.wPosX = pNetMsg->wPosX;
    NetItemUpdate.wPosY = pNetMsg->wPosY;
    NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
    SendToClient( &NetItemUpdate );

    // 포장 아이템이 풀린 아이템으로 변환되는 값 로그 남김.;
    LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_UNWRAPPING );

    // 포장 풀기 성공 알림.;
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

	// 유효한 인벤토리인지 검사;
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if ( !pItemData || pItemData->BasicType() != ITEM_INITIALIZE_COUNTRY )
	{
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// Initialize Country;
	m_pGaeaServer->InitializeCountry( dwClientID );

	// 아이템 소모;
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 케릭터 생성 슬롯 증가.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameDbJob(
            db::DbActionPtr(
                new db::CharacterCreateNumIncrease( m_dwUserID ) ) );
	}

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// 처리 성공 알림.
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

	// Note : 레벨 조건 확인
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

	// 창고 제한 시간 변경.
	//
	int nINDEX = pNetMsg->wSTORAGE - EMSTORAGE_CHANNEL_SPAN;
	__time64_t& tTIME = m_tSTORAGE[nINDEX];

	CTime tCurTime = CTime::GetCurrentTime();
	CTime tSeedTime( tTIME );
	CTimeSpan tLMT ( pITEM->sDrugOp.tTIME_LMT );
	CTimeSpan tADD( 0, 1, 30, 0 );

	// 아직 초기화 되지 않았거나, 충전시간 초과되었을 경우.
	if ( tTIME == 0 || tSeedTime < tCurTime )
	{
		tSeedTime = tCurTime + tLMT + tADD;
	}
	// 충전시간에 아직 여유가 있을때.
	else
	{
		tSeedTime = tSeedTime + tLMT + tADD;
	}

	// 사용시간 충전됨, 사용 가능으로 변경.
	tTIME = tSeedTime.GetTime();
	m_bSTORAGE[nINDEX] = true;

	// DB에 스토리지 충전시간 저장.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameAdoJob( 
            db::DbActionPtr( 
                new db::StorageDateSet( m_dwUserID, nINDEX + EMSTORAGE_CHANNEL_SPAN, tTIME ) ) );
	}

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// 처리 결과 알림.
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

	// Note : 레벨 조건 확인
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

	// 인벤 라인 증가.
	++m_wINVENLINE;

	// 현제 활성화된 인벤 라인 설정.
	//
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	// DB에 저장.
	//
	if ( m_pGaeaServer->GetDBMan() )
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new db::InvenAddNumSet( m_CharDbNum, m_wINVENLINE ) ) );
	}

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// 처리 결과 알림.
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
// Desc: 개조 기능 이용카드 요청 ( 카드만 소모시킴 )
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

	// Note : 레벨 조건 확인
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

	// Note : 레벨 조건 확인
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

    // 개인상점 검색 구매가 가능한 맵인가?
    if ( false == private_market::GLPrivateMarketDefine::Instance()->CanPurchase(m_mapID.getGaeaMapID()) )
    {
        MsgFB.emFB = EMREQ_PMARKET_SEARCH_OPEN_FB_BLOCK;
        SendToClient( &MsgFB );
        return E_FAIL;
    }

    // 아이템 유효성 검사
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
    
	// Note : 레벨 조건 확인
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

	// 처리 주체가 에이전트에서 필드로 변경됨
	/*
    // 아이템 사용에대한 유효성검사를 거치고 나면 에이전트 서버에 개인상점검색 카드 사용중임을
    // 알리고 이에 대한 응답을 받은 후에 아이템 소모 시킴 (캐쉬템이므로 신중하게 소모시켜야 함)
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

    //	[자신에게] Inventory 아이템 소모시킴.
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
// Desc: 긴급 창고 이용카드 요청 ( 카드만 소모시킴 )
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageOpen( NET_MSG_GENERIC* nmg )
{
	// 창고연결카드를 사용중이면
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	MsgFB.emFB = EMREQ_STORAGEOPEN_FB_OK;
	MsgFB.wPosX = pNetMsg->wPosX;
	MsgFB.wPosY = pNetMsg->wPosY;

	SendToClient( &MsgFB );	

	// 창고연결카드 사용여부
	m_bUsedStorageCard = true;

	if ( GLHIDESET::CheckHideSet( EMHIDE_USE_STORAGE_LINK ) )
	{
		RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

// *****************************************************
// Desc: 긴급 창고 사용 완료
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageClose( NET_MSG_GENERIC* nmg )
{
	// 창고연결카드 사용여부
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenPremiumSet", m_CharDbNum ) );
	}

	// 인벤의 여유 공간 측정
	// 불필요한 Assign 삭제 
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
// 			// 인벤에 공간이 없는 것으로 판단됨
// 			MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOTINVEN;
// 			SendToClient( &MsgFB );
// 			return E_FAIL;
// 		}
	}

	if ( BoxItemCount > m_cInventory.GetAmountEmpty( false ) )
	{
		// 인벤에 공간이 없는 것으로 판단됨
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOTINVEN;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

    // 상자안에 있는 아이템을 꺼내어서 넣는다.
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

		// 아이템 생성
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

		// 아이템 에디트에서 입력한 개조 등급 적용
		sITEM_NEW.cDAMAGE = ( BYTE )pONE->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = ( BYTE )pONE->sBasicOp.wGradeDefense;

		// 랜덤 옵션 생성.
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );
		
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
		if ( !bOk )
            return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

		InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, false, 1, true, false, true );
		const SINVENITEM* pInvenItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
		if ( !pInvenItem )
            return E_FAIL;

		// 로그 남김.
		// 통계 및 로그 수정
		LogItemExchange(
            pInvenItem->sItemCustom,
            gamelog::ID_CHAR, 0,
            gamelog::ID_CHAR, m_CharDbNum,
            ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_PREMIUMSET,
			false );

		//	[자신에게] 메시지 발생.
		InvenInsertSend( pInvenItem, FALSE, false, true );
	}

	// 프리미엄 설정.
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

	// 프리미엄 모드 ON
	m_tPREMIUM = tPREMIUM.GetTime();
	m_bPREMIUM = true;

	// 현재 활성화된 인벤 라인 설정
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	// DB에 저장
	AddGameDbJob(
        db::DbActionPtr(
            new db::PremiumTimeSet( m_dwUserID, m_tPREMIUM ) ) );

	// Note : CSetPremiumTime 클래스에서 모든 경우를 처리하도록 변경됨
	/*
	if ( m_pGLGaeaServer->GetDBMan() )
	{
		CDaumSetPremiumTime *pDbAction = new CDaumSetPremiumTime ( m_dwUserID, m_tPREMIUM );
		m_pGLGaeaServer->GetDBMan()->AddJob ( pDbAction );
	}
	*/
	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pPacket->m_PosX, pPacket->m_PosY );


	// 에이젼트로 거쳐서 메시지 전송.
	//		( 에이젼트에 기록되어 있는 프리미엄 기간을 갱신 시켜줘야 한다. )
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

	// Note : 레벨 조건 확인
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

	// 헤어를 변화 시킴.
	//
	m_wHair = wHAIR;

	// 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] Inventory 아이템 소모시킴.
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

	// Note : 레벨 조건 확인
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

	// 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 헤어를 변화 시킴.
	//
	m_wHairColor = pNetMsg->wHairColor;

	// 클라이언트에 처리 결과 전송.
	MsgFB.wHairColor = m_wHairColor;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.wHairColor = m_wHairColor;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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

	// Note : 레벨 조건 확인
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

	// 얼굴을 변화 시킴.
	//
	m_wFace = wFACE;

	// 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 얼굴 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] Inventory 아이템 소모시킴.
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

	// Note : 레벨 조건 확인
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

	// 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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

	// 아이템 빈공간 확인을 밑에서 하도록 위치 변경함
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 성별 변경전 성별 전용 아이템 착용 개수 체크
	int dwNeedfullInventory = 0;
	for ( int i = 0; i < SLOT_TSIZE; i++ )
	{
		// SITEMCUSTOM sItemcustom  에서 참조자로 변경함 const SITEMCUSTOM& sItemcustom
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

	// 성별 전용 아이템 개수와 인벤토리 빈공간 체크
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
	//	극강부 성별 변경 되게 풀었습니다. 
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

	// 클라이언트에 처리 결과 전송.
	MsgFB.emClass	 = m_emClass;
	MsgFB.wSex		 = m_wSex;
	MsgFB.wFace      = m_wFace;
	MsgFB.wHair      = m_wHair;
	MsgFB.wHairColor = m_wHairColor;

	MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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
	if ( spClub ) // 클럽에 속해있으면 학원 변경 불가능
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

		// Note : 레벨 조건 확인
		//
		if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
		{
			return E_FAIL;
		}

		// 기존 학원 정보는 wPreSchooldp 임시로 저장
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

		// 클라이언트 데이터 유효성 검사
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
	// 학원 변경 기능 사용시 퀘스트 변경
	bool bIsPlayRnattendanceQuest = false;
	//	진행중인 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pPROG = ( *iter ).second;

			// 학원 변경시 퀘스트 변경기능 테스트
			DWORD dwChangedID = 0;
			SNATIVEID sID = pPROG->m_sNID;
			int nRetMapping = pPROG->ConvertQuestGroup( wCurrentSchool, m_emClass, m_wSchool, dwChangedID );
			if ( nRetMapping == EMSCHOOL_CHANGE_ERROR )
			{
				// 에러
				sc::writeLogError( sc::string::format( "[QUEST MAPPING ERROR] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			else if ( nRetMapping == EMSCHOOL_CHANGE_NOT_EXIST )
			{
				// 매칭할 퀘스트가 없음
				sc::writeLogInfo( sc::string::format( "[QUEST MAPPING NOT FIND] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			// 매칭할 퀘스트가 없는 경우에는 의도적으로 삭제하도록 설정 가능
			else if ( nRetMapping == EMSCHOOL_CHANGE_DISGARD )
			{
				vecDelQuestList.push_back( pPROG->m_sNID.dwID );
				sc::writeLogError( sc::string::format( "[QUEST MAPPING DELETE] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
				continue;
			}
			else
			{
				// 그룹으로 매칭된 퀘스트가 존재하므로 변경함
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
				sc::writeLogDebug( sc::string::format( "MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, 진행중) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
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
					sc::writeLogDebug( sc::string::format( "MsgGameJoin (진행중) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
#endif

					SendToAgent( &NetMsg );
				}
			}
		}
	}

	bool bIsRnAttendanceCompleted = false;
	bool bIsEndRnattendanceQuest = false;

	//	완료된 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestEnd().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pPROG = ( *iter ).second;
			
			// 학원 변경시 퀘스트 변경기능 테스트
			DWORD dwChangedID = 0;
			SNATIVEID sID = pPROG->m_sNID;
			int nRetMapping = pPROG->ConvertQuestGroup( wCurrentSchool, m_emClass, m_wSchool, dwChangedID );
			if ( nRetMapping == EMSCHOOL_CHANGE_ERROR )
			{
				// 에러
				sc::writeLogError( sc::string::format( "[QUEST MAPPING ERROR] MsgReqInvenSchoolChange - COMPLETE QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			else if ( nRetMapping == EMSCHOOL_CHANGE_NOT_EXIST )
			{
				// 매칭할 퀘스트가 없음
				sc::writeLogInfo( sc::string::format( "[QUEST MAPPING NOT FIND] MsgReqInvenSchoolChange - COMPLETE QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
			}
			// 매칭할 퀘스트가 없는 경우에는 의도적으로 삭제하도록 설정 가능
			else if ( nRetMapping == EMSCHOOL_CHANGE_DISGARD )
			{
				vecDelQuestList.push_back( pPROG->m_sNID.dwID );
				sc::writeLogError( sc::string::format( "[QUEST MAPPING DELETE] MsgReqInvenSchoolChange - PROGERSS QUEST ID : %1% ( %2% / %3% ), Changed QUEST ID : %4%, School : %5%, Class : %6%", sID.dwID, sID.wMainID, sID.wSubID, dwChangedID, m_wSchool, m_emClass ) );
				continue;
			}
			else
			{
				// 그룹으로 매칭된 퀘스트가 존재하므로 변경함
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
				sc::writeLogDebug( sc::string::format( "MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, 진행완료) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
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
					sc::writeLogDebug( sc::string::format( "MsgGameJoin (진행완료) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum ) );
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

	// 학원 변경으로 인해서 삭제된 퀘스트는 저장되지 않도록 퀘스트 리스트에서 삭제한다
	for( size_t idx = 0; idx < vecDelQuestList.size(); idx++ )
	{
		m_cQuestPlay.DeleteProc( vecDelQuestList[idx] );
	}

	vecDelQuestList.clear();

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterSchoolChangeSet( 
                m_CharDbNum,
                m_wSchool ) ) );

	// 클라이언트에 처리 결과 전송.
	MsgFB.wSchool = m_wSchool;
	MsgFB.emFB = EMINVEN_SCHOOL_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	LogCharAction(
	m_CharDbNum,		//	당사자.
	gamelog::ACT_CHANGE_SCHOOL,	//	행위.
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

	// 아이템 가져옴. DB Action.
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
// Desc: 이름변경요청 처리
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
	
	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 캐릭터명
	CString strChaName( pNetMsg->szName );
	strChaName.Trim(_T(" ")); // 앞뒤 공백제거

	// 스페이스 찾기, 캐릭터이름 4글자 이하 에러, 캐릭터 만들 수 없음.
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

	// 태국어 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString( strChaName ) )
	    {
		    MsgFB.emFB = EMINVEN_RENAME_FB_THAICHAR_ERROR;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }
    }

	// 베트남 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if ( STRUTIL::CheckVietnamString( strChaName ) )
	    {
		    MsgFB.emFB = EMINVEN_RENAME_FB_VNCHAR_ERROR;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }
    }

	// DB에 저장요청
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
// Desc: 이름변경 결과 처리
// *****************************************************
HRESULT GLChar::MsgInvenRename( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME_FROM_DB* pNetMsg = ( GLMSG::SNETPC_INVEN_RENAME_FROM_DB* )nmg;
	
	// 클라이언트에 처리 결과 전송.
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

	// 주변에 이름변경을 알림.
	GLMSG::SNETPC_INVEN_RENAME_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy( NetMsgBrd.szName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround( &NetMsgBrd );

	// GLLandMan 이름맵 변경.
	GLMSG::SNETPC_CHANGE_NAMEMAP NetMsgNameMap;
	NetMsgNameMap.dwGaeaID = m_dwGaeaID;
	StringCchCopy( NetMsgNameMap.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy( NetMsgNameMap.szNewName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround( &NetMsgNameMap );

	// 이름변경을 친구와 클럽 맴버들에게 알림
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();
	NetMsgAgt.dwID = m_dwGaeaID;
	StringCchCopy( NetMsgAgt.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy( NetMsgAgt.szNewName, CHAR_SZNAME, pNetMsg->szName );
	SendToAgent( &NetMsgAgt );

	m_pGaeaServer->ChangeNameMap( this, m_szName, pNetMsg->szName );

	//	[자신에게] Inventory에 아이템 제거.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

// *****************************************************
// Desc: 우편함 열기 요청 (우체통)
// *****************************************************
HRESULT GLChar::MsgReqPostBoxOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_POSTBOX_OPEN* pNetMsg = ( GLMSG::SNETPC_POSTBOX_OPEN* ) nmg;

	// Note : 해당 우체통을 사용할 수 있는지 체크
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

	// Note : 우편함 아이디 등록
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NPC;
	m_sPOSTBOXID	  = pNetMsg->sPostBoxID;

	return S_OK;
}


// *****************************************************
// Desc: 우편함 열기 요청 (카드)
// *****************************************************
HRESULT GLChar::MsgReqPostBoxOpenCard( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD* pNetMsg = ( GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD* ) nmg;

	GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD_FB MsgFB;

	// Note : 우편함 사용 불가능이면
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	MsgFB.emFB			 = EMINVEN_POSTBOX_OPEN_CARD_FB_SUCCESS;
	MsgFB.sPostBoxCardID = pINVENITEM->sItemCustom.GetNativeID();
	SendToClient( &MsgFB );

	// Note : 우편함 아이디 등록
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_ITEM_CARD;
	m_sPOSTBOXID	  = pINVENITEM->sItemCustom.GetNativeID();

    //	[자신에게] Inventory에 아이템 제거.
    ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqPostBoxClose( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_POSTBOX_CLOSE* pNetMsg = ( GLMSG::SNETPC_POSTBOX_CLOSE* ) nmg;

	// Note : 우편함 아이디 해제
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

	// DB에 저장요청
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
// Desc: SMS 전송 결과 처리
// *****************************************************
HRESULT GLChar::MsgSendSMS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SEND_SMS_FROM_DB* pNetMsg = (GLMSG::SNETPC_SEND_SMS_FROM_DB *)nmg;

	// 클라이언트에 처리 결과 전송.
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

		//	[자신에게] Inventory 아이템 소모시킴.
		ConsumeInvenItem( RecvData.m_PosX, RecvData.m_PosY );
	}
	else
	{
		// GM 채팅을 DB에 저장한다
		//std::string strSendName = sc::string::format( "%1%", GetName() );
		std::string strSendName( GetName() );
		AddLogAdoJob( db::DbActionPtr( new db::LogGMChat( CHAT_TYPE_LOUDSPEAKER, UserLevel(), strSendName, std::string( "System or Unknown" ), RecvData.m_Msg ) ) );
	}

	// 확성기 사용.
	//
	GLMSG::SNETPC_CHAT_LOUDSPEAKER_AGT NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();

	// 링크 정보를 찾는다.
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

	// Agent결과 시간이 부족할경우 처리해준다. 돈을 다시 돌려준다.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqRebuildResult", m_CharDbNum ) );
	}

	switch( pNetMsg->emResult )
	{
	case EMREBUILD_RESULT_OPEN:		// 개조 인터페이스 열기
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

		// 개조 확정 지을시;
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

				// 바로 디비에 적용한다;
				InvenUpdateItem( pInvenItem );

				if ( EMTYPE_ITEM_REMODEL_RANDOMOPTION == pNetMsg->emTypeRemodel )
				{
					// 변경된 랜덤옵션의 로그를 저장한다;
					LogItemRandomOption( pInvenItem->sItemCustom );
					LogItemConversion(
						pInvenItem->sItemCustom,
						gamelog::ID_CHAR,
						m_CharDbNum,
						MONEY_ROUTE_NPC_REMAKE,
						LOG_ITEM_REMODEL_RESULT );
				}

				// 개조 되었으므로 아이템 정보를 전송해야한다;
				GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
				NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
				NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
				NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
				SendToClient( &NetItemUpdate );

				// 개조 성공했다;
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

			// 테스트 하자;
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

					// 개조 되었으므로 아이템 정보를 전송해야한다;
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
			// 금액이 맞지 않아서 개조를 진행할 수 없다;
			if ( m_i64RebuildInput != m_i64RebuildCost || GetInvenMoney() < m_i64RebuildCost )
			{
				pNetMsg->emResult = EMREBUILD_RESULT_MONEY;
				SendToClient( pNetMsg );
				return S_OK;
			}

			// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다;
			// http://172.16.2.87/redmine/issues/832
			SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
			if ( sRebuildItem.IsBoxWrapped() )
				return E_FAIL;

			const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
			if (NULL == pItem)
				return E_FAIL;

			// 장착 아이템만 개조가 가능하다;
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

	case EMREBUILD_RESULT_FAIL:		// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_DESTROY:	// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_MONEY:	// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_CLOSE:	// 개조 인터페이스 닫기
		{
			InitRebuildData();
			CloseRebuild();

			// 개조카드 아이템의 위치를 초기화 시킨다.
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

	// 팻카드이면서 팻이 활성화 되어 있으면 삭제할수 없다.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

			// Note : 레벨 조건 확인
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

			// 금액 변화 로그
			// 통계 및 로그 수정
			LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, -nCost, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_GARBAGE );
			// 통계 및 로그 수정
			LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

			// 금액 변화 알림.
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

	// 통계 및 로그 수정
	LogItemExchange(
		pINVENITEM_DEL->sItemCustom,
		gamelog::ID_CHAR, m_CharDbNum,
		gamelog::ID_CHAR, m_CharDbNum,
		ITEM_ROUTE_GARBAGE, pINVENITEM_DEL->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_TO_GARBAGE,
		false );

	// 펫 카드 일경우
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pINVENITEM_DEL->sItemCustom.PetDbNum() != 0 )
	{
		PGLPETFIELD pMyPet = GetMyPet();

		if ( pMyPet && pINVENITEM_DEL->sItemCustom.PetDbNum() == pMyPet->DbNum() ) 
		{
			m_pGaeaServer->ReserveDropOutPet( SDROPOUTPETINFO( pMyPet->m_dwGUID, true, false ) );

			// 팻의 악세사리를 검사한다.
			pMyPet->UpdateTimeLmtItem( this );

			// 넣어줄꺼 넣어준다.
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
					// 바닥에 떨어뜨린다.
					// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
					// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
					// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
					// 누군가 줍는다면 자기 아이템으로 업데이트된다.
					DropItem( cDropItem.sItemCustom, m_vPosition );
				}
			}

			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetDelete( m_CharDbNum, pINVENITEM_DEL->sItemCustom.PetDbNum() ) ) );					
			
			// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetListRestoreGet( m_CharDbNum, m_ClientSlot ) ) );
		}
		else
		{
			// new 한거 돌아와서 잘 삭제되나? 시간제 아이템 삭제 코드에도 이거랑 비슷한거 본거 같은데
			// 악세사리를 체크후 팻을 DB에서 삭제해준다.
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

	// 탈것일 경우
	if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && pINVENITEM_DEL->sItemCustom.m_VehicleDbNum != 0 )
	{
		// 위에 펫은 활성화되어 있는지 확인하는 코드가 들어 있고, 착용 아이템도 처리해준다. 탈것은 빠져있는데 의도된건것인지? 빼먹은건지?
		// 확인해보니 디비액션에서 돌아와서 위에 펫처럼 처리된다.
		// new 한거 돌아와서 잘 삭제되나? 시간제 아이템 삭제 코드에도 이거랑 비슷한거 본거 같은데
		// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
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

	// 인벤토리에서 아이템 제거
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
			// 장착 아이템이어야 바꿀 수 있다
			const SITEM* pItem = GLogicData::GetInstance().GetItem( pResistItem->sItemCustom.GetNativeID() );
			if ( pItem != NULL )
			{
				switch ( pItem->sBasicOp.emItemType )
				{
				case ITEM_SUIT:					
					if ( strlen( pItem->sRandomOpt.szNAME ) < 4 )
						break; // 랜덤옵션 파일이 지정되어 있어야 가능하다;

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

	// 개조 비용 전송
	MsgReqRebuildCostMoney();

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildCostMoney()
{
	GLMSG::SNET_REBUILD_COST_MONEY NetMsg;

	// 개조 아이템이 없으므로 0을 전송한다
	if ( !m_sRebuildItem.VALID() )
	{
		SendToClient( &NetMsg );
		return S_OK;
	}

	// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	if ( !pItem )
	{
		SendToClient( &NetMsg );
		return S_OK;
	}

	// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
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

	// 팻카드 여부 체크
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

	// 사료 여부 체크
	if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD )	
	{
		return E_FAIL;
	}

    WORD wHoldPosX = pHoldInvenItem->wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = pHoldInvenItem->wPosY;

	sNativeID = pHold->sBasicOp.sNativeID;
	switch ( pHold->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		{
			PGLPETFIELD pMyPet = GetMyPet();

			// Logic 에 팻이 있다면
			if ( pMyPet && pMyPet->DbNum() == pInvenItem->sItemCustom.PetDbNum() )
			{
				// 펫 사료먹기 이전 로그
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

				// 펫 사료먹기 이후 로그
				LogPetAction(
                    pMyPet->DbNum(), 
                    pInvenItem->sItemCustom.GetNativeID(), 
                    pet::EM_ACTION_FOOD_AFTER, 
                    pMyPet->Satiation() );

				// 팻에게 먹이를 준다.
                GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB( pMyPet->DbNum(), sNativeID, pMyPet->Satiation(), EMPET_REQ_GIVEFOOD_FB_OK );
				SendToClient( &NetMsgFB );
				if ( GLHIDESET::CheckHideSet( EMHIDE_USE_PET_CHANGE ) )
				{
					RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
					RestoreActStateByInvisible();
				}
			}
			// 아직 Logic 에 팻이 없다면
			else
			{
				// 팻 생성여부 확인
				if ( pInvenItem->sItemCustom.PetDbNum() <= 0 )
				{
					sc::writeLogError( sc::string::format( "Invalid ID, PetID %1% dwClientID %2%", pInvenItem->sItemCustom.PetDbNum(), m_ClientSlot));
					return E_FAIL;
				}

				// 팻의 포만감을 가져와서 갱신하고 다시 DB에 저장하는 코드
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
			//	손에 든 아이템 확인한다.
			//	아이템이 바뀌는 경우가 있다.
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

			// 펫 포만감 이전 로그
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

			// 펫 포만감 이전 로그
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
        // 조합 시작

        const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( m_pProductField->GetCurrentProductRecipeKey() );
        if ( !pProductRecipe )
		{
			sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateERROR - Count : %1%", m_pProductField->m_nProductCurrentCount+1 ) );
			NetMsgFb.nState = Product::EStateERROR;
			SendToClient( &NetMsgFb );

			// 모션 진행 중 취소하는 것이므로 주위에도 알림
			SendMsgViewAround( &NetMsgFb );
			return;
		}
            
        if ( m_pProductField->CheckProduct_FieldServer( pProductRecipe, this, NetMsgFb.nState ) == false )
        {
			sc::writeLogInfo( sc::string::format("- Make Product Item : Cancel CheckProduct_FieldServer - Count : %1%", m_pProductField->m_nProductCurrentCount+1 ) );
            SendToClient( &NetMsgFb );

            // 모션 진행 중 취소하는 것이므로 주위에도 알림
            SendMsgViewAround( &NetMsgFb );
            return;
        }

		// Agent로 보내는 패킷은 따로 분리한다;
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

			// 확인해야 할 재료가 있는지 확인한다;
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

				// 주위 캐릭터에도 내가 조합 중인것이 보이도록 알림;
				SendMsgViewAround( &NetMsgFb );
			}

            // 계속;
            SetMotion(
                EMANI_MAINTYPE( GLCONST_CHAR::sProductMotionID.wMainID ),
                EMANI_SUBTYPE( GLCONST_CHAR::sProductMotionID.wSubID ),
                pProductRecipe->fDelaySec,
                boost::bind( boost::mem_fn( &GLChar::CallbackProduct ), this ) );
        }
        else
        {
			sc::writeLogInfo( sc::string::format("- Make Product Item : RESET Product::EStateComplete - Count : %1%, %2%", m_pProductField->m_nProductCurrentCount+1, m_pProductField->GetProductCount() ) );
			
			// 제조를 초기화한다;
			m_pProductField->Reset();

            NetMsgFb.nState = Product::EStateComplete;
            SendToClient( &NetMsgFb );

            TurnAction( GLAT_IDLE );
        }
    }
    else
    {
		m_pProductField->CancleProduct(this);
        // 취소
		sc::writeLogInfo( sc::string::format("- Make Product Item : Product::EStateCancel - Count : %1%, Sec : %2%, Vaild : %3%", m_pProductField->m_nProductCurrentCount+1, m_fMotionSec, IsValidBody() ) );
        NetMsgFb.nKey = UINT_MAX;
        NetMsgFb.nState = Product::EStateCancel;
        SendToClient( &NetMsgFb );

        // 모션 진행 중 취소하는 것이므로 주위에도 알림
        SendMsgViewAround( &NetMsgFb );
    }
}

//hsshin 제작숙련도 추가로 인한 메시지 처리부분 수정 (서버로직쪽 작업자가 보시고 다시 구현 해주세요)
HRESULT GLChar::MsgReqItemMix( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_ITEM_MIX* pPacket = ( GLMSG::SNET_INVEN_ITEM_MIX* ) nmg;
	GLMSG::NET_INVEN_ITEM_MIX_FC NetMsgFB;

	SINVENITEM* pInvenItem[ITEMMIX_ITEMNUM];
	memset( pInvenItem, 0, sizeof( SINVENITEM* ) * ITEMMIX_ITEMNUM );

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	// NPC확인/NPC 체크
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

	// 조합 확인
	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( pPacket->dwKey );
	if ( !pProductRecipe ) 
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOMIX );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	금액 확인 
	if ( GetInvenMoney() < pProductRecipe->dwPrice )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOMONEY );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 인벤 아이템 확인
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

		// MID/SID도 같은지 체크해야된다.
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

	//	결과 아이템 체크
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqItemMix", m_CharDbNum ) );
	}

	WORD wInsertPosX = 0;
	WORD wInsertPosY = 0;
	BOOL bOk = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		// 불가능한 경우 입니다.
		NetMsgFB.SetResult( EMITEM_MIX_FB_NOINVEN );
		SendToClient( &NetMsgFB );
		return E_FAIL;	//	인벤 공간 부족으로 되돌림 실패.
	}	

	//	조합 아이템 삭제
	for (int i=0; i<ITEMMIX_ITEMNUM; ++i)
	{
		if ( !pPacket->sInvenPos[i].VALID() )
			continue;

		//  아이템 타입에 상관없이 무조껀 남긴다.
		// 통계 및 로그 수정
		LogItemExchange( 
			pInvenItem[i]->sItemCustom, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, 0, 
			ITEM_ROUTE_ITEM_MIX, pInvenItem[i]->sItemCustom.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MIX_DEL_ITEM,
			true );

		InvenDeleteItem( pInvenItem[i]->wPosX, pInvenItem[i]->wPosY, true, FALSE, 1, true );
	}

	//	소지금 
	if ( pProductRecipe->dwPrice > 0 )
	{
		CheckMoneyUpdate( GetInvenMoney(), pProductRecipe->dwPrice, FALSE, "ITEM_MIX" );

		SubtractInvenMoneyServer( pProductRecipe->dwPrice, SUBTRACT_INVEN_MONEY_ITEM_MIX );

		LONGLONG MixPrice = static_cast< LONGLONG >( pProductRecipe->dwPrice );
		// 통계 및 로그 수정
		LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -MixPrice, ITEM_ROUTE_ITEM_MIX, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MIX_CHARGE );
		// 통계 및 로그 수정
		LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );        

		SendInvenMoneyUpdate();
	}

	//	실패했을 경우
	float fRate = ( float )pProductRecipe->dwRate;
	if ( !sc::Random::getInstance().IsSucceed( fRate ) )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_FAIL );
		SendToClient( &NetMsgFB );
		return S_OK;	
	}

	//	결과 아이템 생성
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

	// 개조 등급 적용
	sITEM_NEW.cDAMAGE = pProductRecipe->nGradeAttack;
	sITEM_NEW.cDEFENSE = pProductRecipe->nGradeDefense;

	// 랜덤 옵션 생성
	if ( pProductRecipe->bRandomOption )
	{
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
			LogItemRandomOption( sITEM_NEW );
	}

	//	들고있던 아이템 Inventory에 넣음.
	InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, false, 1, true, false, true );
	SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );

	if ( !pINSERT_ITEM )
	{
		NetMsgFB.SetResult( EMITEM_MIX_FB_FAIL );
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//  아이템 타입에 상관없이 무조건 남긴다.
	// 통계 및 로그 수정
	LogItemExchange(
		pINSERT_ITEM->sItemCustom,
		gamelog::ID_CHAR, 0,
		gamelog::ID_CHAR, m_CharDbNum, 
		ITEM_ROUTE_ITEM_MIX, pINSERT_ITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MIX_MIX_RESULT,
		true );

	//	[자신에게] 메시지 발생
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

	// Note : 스테이지 패스권 사용 불가능 지역 확인
	if ( !pLand->IsInstantMap() )
	{
		return S_FALSE;
	}

	// Note : 아이템이 유효한지 확인
	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	// Note : 아이템 종류 확인
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_STAGE_PASS )
	{
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
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

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( NetMsg.wPosX, NetMsg.wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenEnterNumCharge( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF* ) nmg;	

	// Note : 아이템이 유효한지 확인
	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	// Note : 아이템 종류 확인
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_ENTER_NUM_CHARGE )
	{
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
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

	// Note : 아이템이 유효한지 확인
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
		//	[자신에게] Inventory 아이템 소모시킴.
		ConsumeInvenItem( NetMsg.wPosX, NetMsg.wPosY );
		GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC Msg;
		Msg.dwMapID = pItem->sBasicOp.sSubID.Id();
		SendToClient( &Msg );
	}


	return S_OK;
}

// *****************************************************
// Desc: 코스튬 능력치 부여
// *****************************************************
HRESULT GLChar::MsgReqInvenCostumeStats( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLMSG::SNET_INVEN_COSTUME_STATS* pNetMsg = ( GLMSG::SNET_INVEN_COSTUME_STATS* ) nmg;

	GLMSG::SNET_INVEN_COSTUME_STATS_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// 유효한 인벤토리 인지 검사
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// 유효한 아이템인지 검사
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

	// 손에 들고 있는 아이템이 유효한지 검사
	const SITEMCUSTOM& scSrcItem = pSrcInvenItem->CustomItem();
	if ( scSrcItem.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_CS_FB_FAIL1;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	// 손에든 아이템의 정보 갖고 오기
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

			//	시간 제한으로 아이템 삭제 알림.
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
			NetMsgInvenDelTimeLmt.nidITEM = scSrcItem.GetNativeID();
			SendToClient( &NetMsgInvenDelTimeLmt );

			// 아이템 삭제
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
	
	// 이미 적용된 능력치가 있다.
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
	
	// 바로 디비에 적용한다.
	InvenUpdateItem( pInvenItem );

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	SendToClient( &NetItemUpdate );

	// 로그
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

	// 유효한 인벤토리 인지 검사
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}
	
	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// 유효한 아이템인지 검사
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

	// 손에 들고 있는 아이템이 유효한지 검사
	const SITEMCUSTOM& scSrcItem = pSrcInvenItem->CustomItem();
	if ( scSrcItem.GetNativeID() == SNATIVEID( false ) )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 손에든 아이템의 정보 갖고 오기
	SITEM* pSrcItem = GLogicData::GetInstance().GetItem( scSrcItem.Id() );
	if ( !pSrcItem )
	{
		NetMsgFB.emFB = EMREQ_CS_RELEASE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// #item 아이템타입 확인하는 부분이 없는 것 같아 추가함
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

	// 바로 디비에 적용한다.
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

	// 유효한 인벤토리 아이템인지 검사한다;
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( NULL == pInvenItem )
	{
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// 유효한 아이템인지 검사한다;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		return E_FAIL;
	}

	// 유효한 아이템 타입인지 검사한다;
	if ( ITEM_UNLOCK_CHARACTER_SLOT != pItem->sBasicOp.emItemType )
	{
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 캐릭터 슬롯을 확장한다;
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

	// 캐릭터 슬롯을 더이상 확장할 수 없다는 것을 알린다;
	if ( false == bSuccess )
	{
		UnlockCharSlot( 0 );
	}

	// 아이템 소모;
	ConsumeInvenItem( wPosX, wPosY );

	return S_OK;
}

void GLChar::LogRandomOptionAll()
{
	// 인벤토리
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

	// 장착창
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
	
    // 돈을 지불합니다.
    CheckMoneyUpdate( GetInvenMoney(), Price, FALSE, "RanMobile Connection" );

	SubtractInvenMoneyServer( Price, bExtend == false ? SUBTRACT_INVEN_MONEY_RANMOBILE_CONNECTION : SUBTRACT_INVEN_MONEY_RANMOBILE_EXTENDTIME  );

    //	돈 로그.
	// 통계 및 로그 수정
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -Price, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), bExtend == false ? LOG_MONEY_RANMOBILE_CONNECTION : LOG_MONEY_RANMOBILE_EXTENDTIME );
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

    //	금액 변화 메시지.
    SendInvenMoneyUpdate();

}
//
//void GLChar::RanMobileCharacterConnectionFee( LONGLONG Price )
//{
//    // 구매가격=물품가격
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
//    // 돈을 지불합니다.
//    CheckMoneyUpdate( GetInvenMoney(), llPrice, FALSE, "RanMobile Connection" );
//
//	SubtractInvenMoneyServer( llPrice, SUBTRACT_INVEN_MONEY_RANMOBILE_CONNECTION );
//
//    //	돈 로그.
//	// 통계 및 로그 수정
//    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llPrice, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_RANMOBILE_CONNECTION );
//	// 통계 및 로그 수정
//    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
//
//    //	금액 변화 메시지.
//    SendInvenMoneyUpdate();
//
//
//}

void GLChar::RanMobileDailyBonus( const SNATIVEID& ItemID )
{
}

// 보석 (1) 아니면 연장권 (2)
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

	// 아이템 발생; 공간 체크;
	WORD wINSERTX( 0 ), wINSERTY( 0 );
	//BOOL bOK = pChar->InvenFindInsertable( wINSERTX, wINSERTY );
	BOOL bOK = InvenFindInsertable( wINSERTX, wINSERTY );
	if ( !bOK )
	{
	// error
	}

	//	생성한 아이템 Inventory에 넣음;
	//pChar->InvenInsertItem( sCustomItemNEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	InvenInsertItem( sCustomItemNEW, wINSERTX, wINSERTY, false, 1, true, false, true );
	//SINVENITEM *pInsertItem = pChar->InvenGetItem( wINSERTX, wINSERTY );
	SINVENITEM *pInsertItem = InvenGetItem( wINSERTX, wINSERTY );

	if ( !pInsertItem ) 
		return;

	//	[자신에게] 메시지 발생;
	//pChar->InvenInsertSend( pInsertItem, FALSE, false, true );
	InvenInsertSend( pInsertItem, FALSE, false, true );

}
