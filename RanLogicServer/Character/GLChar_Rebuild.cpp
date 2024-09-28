
#include "pch.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/RandomAddon/RandomAddon.h"
#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GLChar.h"

void GLChar::_SetStampInfo(SINVENITEM* pInvenItem)
{
	if (!pInvenItem)
		return;

	m_ItemStampPosX = pInvenItem->wPosX;
	m_ItemStampPosY = pInvenItem->wPosY;
	SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->GetNativeID());
	if ( pItem )
		m_ItemStampFixMax = pItem->sSuitOp.wReModelNum;
}

HRESULT GLChar::_RebuildRandomOption(GLMSG::SNET_REBUILD_RESULT* pNetMsg)
{
	// 아래 유효성 검사는 제외한다; 
	// 윗단 함수에서 다 확인하는것들이다;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pRebuildItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	// 개조카드 사용 불능이라면 사용 못하도록 한다;
	if ( !pRebuildItem || pRebuildItem->sRandomOpt.bUnableRemodelCard == true )
		return E_FAIL;

	// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다 ( ???? 개조창에 아이템을 올릴때 점검하는 것만으로는 패킷 조작을 막을 수가 없다. );
	const RandomOption::Entity* _pSetRandomOption( GLItemMan::GetInstance().GetItemRandomOpt( sRebuildItem.GetNativeID() ) );
	if ( !_pSetRandomOption )
		return E_FAIL;

	// 랜덤옵션 파일이 지정되어 있어야 가능하다;
	if ( strlen( pRebuildItem->sRandomOpt.szNAME ) <= 3 )
		return E_FAIL;

	SINVENITEM* pStampItem( m_cInventory.GetItem( m_ItemStampPosX, m_ItemStampPosY ) );
	SITEM* pItemStamp = NULL;
	if ( pNetMsg->fixOption.isValid() == true )
	{
		if ( pStampItem == NULL )
		{
			if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
			{
				pStampItem = m_cInventory.FindItem(ITEM_STAMP_CARD, ITEM_VAlUE_SUIT_REMODELNUM, m_ItemStampFixMax);
				if ( pStampItem )
				{
					_SetStampInfo(pStampItem);
				}				
			}
			
			if ( pStampItem == NULL )
			{
				GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
				MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
				SendToClient( &MsgFB );
				return E_FAIL;
			}
		}

		pItemStamp = GLogicData::GetInstance().GetItem( pStampItem->sItemCustom.GetNativeID() );
		if ( pItemStamp == NULL )
			return E_FAIL;

		if ( pItemStamp->sSuitOp.wReModelNum != 0 && pItemStamp->sSuitOp.wReModelNum !=  pNetMsg->fixOption.getNSize() )
		{
			GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_STAMP_ERROR;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		if ( pItemStamp->sBasicOp.emItemType != ITEM_STAMP_CARD )
		{
			GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		// Note : 레벨 조건 확인
		//
		if ( !SIMPLE_CHECK_ITEM( pStampItem->sItemCustom ) )
		{
			return E_FAIL;
		}
	}
	else
	{
	}

	// 개조카드를 사용해서 시도할 경우 카드를 체크하고 소모 시킨다.
	if ( m_ItemRemodelPosX < EInvenX && m_ItemRemodelPosY < EInvenTotalLine )
	{
		GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;

		SINVENITEM* pINVENITEM = m_cInventory.GetItem( m_ItemRemodelPosX, m_ItemRemodelPosY );
		if ( !pINVENITEM )
		{
			pINVENITEM = m_cInventory.FindItem(ITEM_REMODEL);
			if ( !pINVENITEM )
			{
				MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
				SendToClient( &MsgFB );
				return E_FAIL;
			}
			m_ItemRemodelPosX = pINVENITEM->wPosX;
			m_ItemRemodelPosY = pINVENITEM->wPosY;
		}

		if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
			return E_FAIL;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
		if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_REMODEL )
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

		//	[자신에게] Inventory 아이템 소모시킴.
		ConsumeInvenItem( m_ItemRemodelPosX, m_ItemRemodelPosY );
	}
	else // 개조 NPC를 이용할 경우 NPC를 체크한다.
	{
		GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
		if ( !pCrow )	
			return E_FAIL;

		if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_ITEM_REBUILD ) )
		{
			return E_FAIL;
		}
	}

	if ( pNetMsg->fixOption.isValid() == true )
	{
		// 인장 카드를 사용한 경우 인장 카드 아이템을 소모 시킨다;
		GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			//	[자신에게] Inventory 아이템 소모시킴.
			ConsumeInvenItem( m_ItemStampPosX, m_ItemStampPosY );
		}
		else
		{
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	float fDestroyRate = sc::Random::getInstance().GetFloatPercent();
	if ( fDestroyRate < _pSetRandomOption->getRateRandomOptionDestroy() )
	{
		// 아이템 삭제 로그 남김
		// 통계 및 로그 수정
		LogItemExchange( 
			sRebuildItem, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, m_CharDbNum, 
			ITEM_ROUTE_DELETE, sRebuildItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_REMODEL_FAIL,
			false );

		// 인벤토리에서 아이템 제거
		// #item 아이템 제거될 때 붙어있던 아이템 처리는 하지않나?
		InvenDeleteItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY, true, FALSE, 1, true );

		// 개조 실패로 아이템이 소멸되었다
		pNetMsg->emResult = EMREBUILD_RESULT_DESTROY;
		SendToClient( pNetMsg );
	}
	else
	{
		SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
		if ( pInvenItem )
		{
			// 결과물을 바로 적용하고 기존 정보를 저장시킨다;
			if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
			{
				// 기존 아이템 정보를 저장시킨다;
				m_sBackUpRebuildInvenItem = *pInvenItem;

				// 랜덤 옵션을 재조정한다;
				if ( GLUseFeatures::GetInstance().IsUsingStampCard() )
					pInvenItem->sItemCustom.GENERATE_RANDOM_OPT( false, pNetMsg->fixOption, pItemStamp );
				else
					pInvenItem->sItemCustom.GENERATE_RANDOM_OPT( false );

				// 바로 디비에 적용한다;
				InvenUpdateItem( pInvenItem );

				// 변경된 랜덤옵션의 로그를 저장한다;
				LogItemRandomOption( pInvenItem->sItemCustom );
				LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, MONEY_ROUTE_NPC_REMAKE, LOG_ITEM_REMODEL_RESULT );

				// 개조 되었으므로 아이템 정보를 전송해야한다;
				GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
				NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
				NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
				NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
				SendToClient( &NetItemUpdate );

				// 개조 성공했다;
				pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
				pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				// 여기서 던져지는건 이전 정보이다; 
				// 현재 내정보는 개조되고 위에 SNET_INVEN_ITEM_UPDATE를 통해 갱신되었다;
				pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
				SendToClient( pNetMsg );
			}
			// 기존 상태를 유지하고 결과물을 받는다, 선택에 따라 결과물을 적용한다;
			else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
			{
				// 아이템을 백업한다;
				m_sBackUpRebuildInvenItem = *pInvenItem;
				// 랜덤 옵션을 재조정한다;
				const bool bUsingStampCard( GLUseFeatures::GetInstance().IsUsingStampCard() );
				if ( bUsingStampCard == true )
					m_sBackUpRebuildInvenItem.sItemCustom.GENERATE_RANDOM_OPT( false, pNetMsg->fixOption, pItemStamp );
				else
					m_sBackUpRebuildInvenItem.sItemCustom.GENERATE_RANDOM_OPT( false );

				pNetMsg->emResult = EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT;
				pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
				SendToClient( pNetMsg );
			}
		}
	}
	return S_OK;
}

HRESULT GLChar::_RebuildAddon(GLMSG::SNET_REBUILD_RESULT* pNetMsg)
{
	// 아래 유효성 검사는 제외한다; 
	// 윗단 함수에서 다 확인하는것들이다;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	RandomAddon::Entity sEntity;
	if ( !pItem->sSuitOp.szRandomAddonScript ||
		E_FAIL == RandomAddon::Manager::GetInstance().Find( pItem->sSuitOp.szRandomAddonScript, sEntity ) )
		return E_FAIL;

	// 인장카드 확인;
	SINVENITEM* pStampItem( m_cInventory.GetItem( m_ItemStampPosX, m_ItemStampPosY ) );
	SITEM* pItemStamp = NULL;
	if ( pNetMsg->fixOption.isValid() == true )
	{
		if ( pStampItem == NULL )
		{
			if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
			{
				pStampItem = m_cInventory.FindItem(ITEM_STAMP_CARD, ITEM_VAlUE_SUIT_REMODELNUM, m_ItemStampFixMax);
				if ( pStampItem )
				{
					_SetStampInfo(pStampItem);
				}				
			}

			if ( pStampItem == NULL )
			{
				GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
				MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
				SendToClient( &MsgFB );
				return E_FAIL;
			}
		}

		pItemStamp = GLogicData::GetInstance().GetItem( pStampItem->sItemCustom.GetNativeID() );
		if ( pItemStamp == NULL )
			return E_FAIL;

		if ( pItemStamp->sSuitOp.wReModelNum != 0 && pItemStamp->sSuitOp.wReModelNum !=  pNetMsg->fixOption.getNSize() )
		{
			GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_STAMP_ERROR;
			SendToClient( &MsgFB );
			return E_FAIL;
		}

		if ( pItemStamp->sBasicOp.emItemType != ITEM_STAMP_CARD )
		{
			GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	// 가산능력치변경카드를 가져온다;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_ADDSTATS_CARD );
	if ( NULL == pInvenCardItem )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOITEM;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// 가산능력치변경카드의 쿨타임을 체크한다;
	if ( CheckCoolTime( pInvenCardItem->sItemCustom.GetNativeID() ) )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_COOLTIME;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 가산능력치변경카드를 사용할 아이템을 가져온다;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOTITEM;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// 카드 아이템을 소모시킨다;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// 인장 카드를 사용한 경우 인장 카드 아이템을 소모 시킨다;
	if ( pNetMsg->fixOption.isValid() == true )
	{
		GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			//	[자신에게] Inventory 아이템 소모시킴.
			ConsumeInvenItem( m_ItemStampPosX, m_ItemStampPosY );
		}
		else
		{
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}
	// 결과물을 바로 적용하고 기존 정보를 저장시킨다;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// 기존 아이템 정보를 저장시킨다;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 가산능력치를 변경한다;
		const bool bUsingStampCard( GLUseFeatures::GetInstance().IsUsingStampCard() );
		if ( bUsingStampCard == true )
			pInvenItem->sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem, pNetMsg->fixOption, pItemStamp );
		else
			pInvenItem->sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem );

		// 바로 DB 에 적용한다;
		InvenUpdateItem( pInvenItem );

		// 로그
		LogItemAddonOption( pInvenItem->sItemCustom );

		// 변경 했으므로 아이템 정보를 전송해야한다;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// 개조를 성공했음을 알린다;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = false;//pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// 기존 상태를 유지하고 결과물을 받는다, 선택에 따라 결과물을 적용한다;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 가산 능력치를 변경한다;
		const bool bUsingStampCard( GLUseFeatures::GetInstance().IsUsingStampCard() );
		if ( bUsingStampCard == true )
			m_sBackUpRebuildInvenItem.sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem, pNetMsg->fixOption, pItemStamp );
		else
			m_sBackUpRebuildInvenItem.sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem );

		pNetMsg->emResult = EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT;
		pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	return S_OK;
}

HRESULT GLChar::_RebuildBasicStats(GLMSG::SNET_REBUILD_RESULT* pNetMsg)
{
	GLMSG::SNET_INVEN_CHANGE_BASICSTATS_FB sNetMsgFB;
	// 아래 유효성 검사는 제외한다; 
	// 윗단 함수에서 다 확인하는것들이다;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	// 아이템이 기본능력치 변경 가능한 아이템인지 확인한다;
	if ( ( false == pItem->IsEditableBasicAttackDamage() ) &&
		( false == pItem->IsEditableBasicDefence() ) )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_DONTCHANGE;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// 기본능력치변경카드를 가져온다;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_BASICSTATS_CARD );
	if ( NULL == pInvenCardItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 기본능력치변경카드를 사용할 아이템을 가져온다;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	SITEM* pStampItem = NULL;
	if ( 0 < pNetMsg->wFixBasicStatsIndex )
	{
		SINVENITEM* pStampInvenItem( m_cInventory.GetItem( m_ItemStampPosX, m_ItemStampPosY ) );
		if ( NULL == pStampInvenItem )
		{
			if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
			{
				pStampInvenItem = m_cInventory.FindItem(ITEM_STAMP_CARD, ITEM_VAlUE_SUIT_REMODELNUM, m_ItemStampFixMax);
				if ( pStampInvenItem )
				{
					_SetStampInfo(pStampInvenItem);
				}				
			}

			if ( NULL == pStampInvenItem )
			{
				sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
				SendToClient( &sNetMsgFB );

				return E_FAIL;
			}
		}

		pStampItem = GLogicData::GetInstance().GetItem( pStampInvenItem->sItemCustom.GetNativeID() );
		if ( NULL == pStampItem )
		{
			sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
			SendToClient( &sNetMsgFB );

			return E_FAIL;
		}

		int nFixCount = 0;
		if ( pNetMsg->wFixBasicStatsIndex & EMREBUILD_FIXED_BASICSTATS_ATTACKDAMAGE )
			nFixCount++;
		if ( pNetMsg->wFixBasicStatsIndex & EMREBUILD_FIXED_BASICSTATS_DEFENCE )
			nFixCount++;

		if ( pStampItem->sSuitOp.wReModelNum != 0 &&
			pStampItem->sSuitOp.wReModelNum != nFixCount )
		{
			sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_STAMP_ERROR;
			SendToClient( &sNetMsgFB );

			return E_FAIL;
		}

		if ( pStampItem->sBasicOp.emItemType != ITEM_STAMP_CARD )
		{
			sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
			SendToClient( &sNetMsgFB );

			return E_FAIL;
		}

		// 인장 카드를 사용한 경우 인장 카드 아이템을 소모 시킨다;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampInvenItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			// 인장카드 아이템을 소모시킨다;
			ConsumeInvenItem( m_ItemStampPosX, m_ItemStampPosY );
		}
		else
		{
			sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
			SendToClient( &sNetMsgFB );

			return E_FAIL;
		}
	}

	// 카드 아이템을 소모시킨다;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// 결과물을 바로 적용하고 기존 정보를 저장시킨다;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// 기존 아이템 정보를 저장시킨다;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 아이템 기본능력치를 변경한다;
		pInvenItem->sItemCustom.GenerateBasicStat( false, pNetMsg->wFixBasicStatsIndex );

		// 바로 DB 에 적용한다;
		InvenUpdateItem( pInvenItem );

		// 로그
		LogItemBasicStatOption( pInvenItem->sItemCustom );

		// 변경 했으므로 아이템 정보를 전송해야한다;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// 개조를 성공했음을 알린다;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = pNetMsg->wFixBasicStatsIndex != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// 기존 상태를 유지하고 결과물을 받는다, 선택에 따라 결과물을 적용한다;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 아이템 기본능력치를 변경한다;
		m_sBackUpRebuildInvenItem.sItemCustom.GenerateBasicStat( false, pNetMsg->wFixBasicStatsIndex );
		pNetMsg->emResult = EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT;
		pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}

	return S_OK;
}

HRESULT GLChar::_RebuildLinkSkill(GLMSG::SNET_REBUILD_RESULT* pNetMsg)
{
	// 아래 유효성 검사는 제외한다; 
	// 윗단 함수에서 다 확인하는것들이다;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_FB sNetMsgFB;

	// 아이템이 아이템스킬 변경 가능한 아이템인지 확인한다;
	if ( -1 == pItem->GetLinkSkillIndex() )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_DONTCHANGE;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// 아이템스킬변경카드를 가져온다;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_ITEMSKILL_CARD );
	if ( NULL == pInvenCardItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// 아이템스킬변경카드의 쿨타임을 체크한다;
	if ( CheckCoolTime( pInvenCardItem->sItemCustom.GetNativeID() ) )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_COOLTIME;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 아이템스킬변경카드를 사용할 아이템을 가져온다;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOTITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// 카드 아이템을 소모시킨다;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// 결과물을 바로 적용하고 기존 정보를 저장시킨다;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// 기존 아이템 정보를 저장시킨다;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 아이템스킬을 변경한다;
		pInvenItem->sItemCustom.GenerateLinkSkill();

		// 바로 DB 에 적용한다;
		InvenUpdateItem( pInvenItem );

		// 로그
		LogItemLinkSkillOption( pInvenItem->sItemCustom );

		// 변경 했으므로 아이템 정보를 전송해야한다;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// 개조를 성공했음을 알린다;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = false;//pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// 기존 상태를 유지하고 결과물을 받는다, 선택에 따라 결과물을 적용한다;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		// 아이템을 백업한다;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// 아이템스킬을 변경한다;
		m_sBackUpRebuildInvenItem.sItemCustom.GenerateLinkSkill();
		pNetMsg->emResult = EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT;
		pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	return S_OK;
}