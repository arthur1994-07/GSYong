
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
	// �Ʒ� ��ȿ�� �˻�� �����Ѵ�; 
	// ���� �Լ����� �� Ȯ���ϴ°͵��̴�;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pRebuildItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	// ����ī�� ��� �Ҵ��̶�� ��� ���ϵ��� �Ѵ�;
	if ( !pRebuildItem || pRebuildItem->sRandomOpt.bUnableRemodelCard == true )
		return E_FAIL;

	// ����â�� ������ �ø��� ���������Ƿ� �̷� ���� �߻����� ���ƾ��Ѵ� ( ???? ����â�� �������� �ø��� �����ϴ� �͸����δ� ��Ŷ ������ ���� ���� ����. );
	const RandomOption::Entity* _pSetRandomOption( GLItemMan::GetInstance().GetItemRandomOpt( sRebuildItem.GetNativeID() ) );
	if ( !_pSetRandomOption )
		return E_FAIL;

	// �����ɼ� ������ �����Ǿ� �־�� �����ϴ�;
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

		// Note : ���� ���� Ȯ��
		//
		if ( !SIMPLE_CHECK_ITEM( pStampItem->sItemCustom ) )
		{
			return E_FAIL;
		}
	}
	else
	{
	}

	// ����ī�带 ����ؼ� �õ��� ��� ī�带 üũ�ϰ� �Ҹ� ��Ų��.
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

		// Note : ���� ���� Ȯ��
		//
		if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
		{
			return E_FAIL;
		}

		//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
		ConsumeInvenItem( m_ItemRemodelPosX, m_ItemRemodelPosY );
	}
	else // ���� NPC�� �̿��� ��� NPC�� üũ�Ѵ�.
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
		// ���� ī�带 ����� ��� ���� ī�� �������� �Ҹ� ��Ų��;
		GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
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
		// ������ ���� �α� ����
		// ��� �� �α� ����
		LogItemExchange( 
			sRebuildItem, 
			gamelog::ID_CHAR, m_CharDbNum, 
			gamelog::ID_CHAR, m_CharDbNum, 
			ITEM_ROUTE_DELETE, sRebuildItem.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_REMODEL_FAIL,
			false );

		// �κ��丮���� ������ ����
		// #item ������ ���ŵ� �� �پ��ִ� ������ ó���� �����ʳ�?
		InvenDeleteItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY, true, FALSE, 1, true );

		// ���� ���з� �������� �Ҹ�Ǿ���
		pNetMsg->emResult = EMREBUILD_RESULT_DESTROY;
		SendToClient( pNetMsg );
	}
	else
	{
		SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
		if ( pInvenItem )
		{
			// ������� �ٷ� �����ϰ� ���� ������ �����Ų��;
			if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
			{
				// ���� ������ ������ �����Ų��;
				m_sBackUpRebuildInvenItem = *pInvenItem;

				// ���� �ɼ��� �������Ѵ�;
				if ( GLUseFeatures::GetInstance().IsUsingStampCard() )
					pInvenItem->sItemCustom.GENERATE_RANDOM_OPT( false, pNetMsg->fixOption, pItemStamp );
				else
					pInvenItem->sItemCustom.GENERATE_RANDOM_OPT( false );

				// �ٷ� ��� �����Ѵ�;
				InvenUpdateItem( pInvenItem );

				// ����� �����ɼ��� �α׸� �����Ѵ�;
				LogItemRandomOption( pInvenItem->sItemCustom );
				LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, MONEY_ROUTE_NPC_REMAKE, LOG_ITEM_REMODEL_RESULT );

				// ���� �Ǿ����Ƿ� ������ ������ �����ؾ��Ѵ�;
				GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
				NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
				NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
				NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
				SendToClient( &NetItemUpdate );

				// ���� �����ߴ�;
				pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
				pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
				// ���⼭ �������°� ���� �����̴�; 
				// ���� �������� �����ǰ� ���� SNET_INVEN_ITEM_UPDATE�� ���� ���ŵǾ���;
				pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
				SendToClient( pNetMsg );
			}
			// ���� ���¸� �����ϰ� ������� �޴´�, ���ÿ� ���� ������� �����Ѵ�;
			else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
			{
				// �������� ����Ѵ�;
				m_sBackUpRebuildInvenItem = *pInvenItem;
				// ���� �ɼ��� �������Ѵ�;
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
	// �Ʒ� ��ȿ�� �˻�� �����Ѵ�; 
	// ���� �Լ����� �� Ȯ���ϴ°͵��̴�;
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

	// ����ī�� Ȯ��;
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

	// ����ɷ�ġ����ī�带 �����´�;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_ADDSTATS_CARD );
	if ( NULL == pInvenCardItem )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOITEM;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// ����ɷ�ġ����ī���� ��Ÿ���� üũ�Ѵ�;
	if ( CheckCoolTime( pInvenCardItem->sItemCustom.GetNativeID() ) )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_COOLTIME;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// ����ɷ�ġ����ī�带 ����� �������� �����´�;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB sMsgFB;
		sMsgFB.emFlag = EMCHANGE_ADDSTATS_FB_NOTITEM;
		SendToClient( &sMsgFB );

		return E_FAIL;
	}

	// ī�� �������� �Ҹ��Ų��;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// ���� ī�带 ����� ��� ���� ī�� �������� �Ҹ� ��Ų��;
	if ( pNetMsg->fixOption.isValid() == true )
	{
		GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
			ConsumeInvenItem( m_ItemStampPosX, m_ItemStampPosY );
		}
		else
		{
			MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}
	// ������� �ٷ� �����ϰ� ���� ������ �����Ų��;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// ���� ������ ������ �����Ų��;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// ����ɷ�ġ�� �����Ѵ�;
		const bool bUsingStampCard( GLUseFeatures::GetInstance().IsUsingStampCard() );
		if ( bUsingStampCard == true )
			pInvenItem->sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem, pNetMsg->fixOption, pItemStamp );
		else
			pInvenItem->sItemCustom.sAddonInfo.Generate_RandomAddon( RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM, pItem );

		// �ٷ� DB �� �����Ѵ�;
		InvenUpdateItem( pInvenItem );

		// �α�
		LogItemAddonOption( pInvenItem->sItemCustom );

		// ���� �����Ƿ� ������ ������ �����ؾ��Ѵ�;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// ������ ���������� �˸���;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = false;//pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// ���� ���¸� �����ϰ� ������� �޴´�, ���ÿ� ���� ������� �����Ѵ�;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// ���� �ɷ�ġ�� �����Ѵ�;
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
	// �Ʒ� ��ȿ�� �˻�� �����Ѵ�; 
	// ���� �Լ����� �� Ȯ���ϴ°͵��̴�;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	// �������� �⺻�ɷ�ġ ���� ������ ���������� Ȯ���Ѵ�;
	if ( ( false == pItem->IsEditableBasicAttackDamage() ) &&
		( false == pItem->IsEditableBasicDefence() ) )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_DONTCHANGE;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// �⺻�ɷ�ġ����ī�带 �����´�;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_BASICSTATS_CARD );
	if ( NULL == pInvenCardItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// �⺻�ɷ�ġ����ī�带 ����� �������� �����´�;
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

		// ���� ī�带 ����� ��� ���� ī�� �������� �Ҹ� ��Ų��;
		if ( m_ItemStampPosX < EInvenX && m_ItemStampPosY < EInvenTotalLine )
		{
			if ( CheckCoolTime( pStampInvenItem->sItemCustom.GetNativeID() ) )
				return E_FAIL;

			// ����ī�� �������� �Ҹ��Ų��;
			ConsumeInvenItem( m_ItemStampPosX, m_ItemStampPosY );
		}
		else
		{
			sNetMsgFB.emFlag = EMCHANGE_BASICSTATS_FB_NOTITEM;
			SendToClient( &sNetMsgFB );

			return E_FAIL;
		}
	}

	// ī�� �������� �Ҹ��Ų��;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// ������� �ٷ� �����ϰ� ���� ������ �����Ų��;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// ���� ������ ������ �����Ų��;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// ������ �⺻�ɷ�ġ�� �����Ѵ�;
		pInvenItem->sItemCustom.GenerateBasicStat( false, pNetMsg->wFixBasicStatsIndex );

		// �ٷ� DB �� �����Ѵ�;
		InvenUpdateItem( pInvenItem );

		// �α�
		LogItemBasicStatOption( pInvenItem->sItemCustom );

		// ���� �����Ƿ� ������ ������ �����ؾ��Ѵ�;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// ������ ���������� �˸���;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = pNetMsg->wFixBasicStatsIndex != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// ���� ���¸� �����ϰ� ������� �޴´�, ���ÿ� ���� ������� �����Ѵ�;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// ������ �⺻�ɷ�ġ�� �����Ѵ�;
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
	// �Ʒ� ��ȿ�� �˻�� �����Ѵ�; 
	// ���� �Լ����� �� Ȯ���ϴ°͵��̴�;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );

	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_FB sNetMsgFB;

	// �������� �����۽�ų ���� ������ ���������� Ȯ���Ѵ�;
	if ( -1 == pItem->GetLinkSkillIndex() )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_DONTCHANGE;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// �����۽�ų����ī�带 �����´�;
	SINVENITEM* pInvenCardItem = m_cInventory.FindItem( ITEM_CHANGE_ITEMSKILL_CARD );
	if ( NULL == pInvenCardItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// �����۽�ų����ī���� ��Ÿ���� üũ�Ѵ�;
	if ( CheckCoolTime( pInvenCardItem->sItemCustom.GetNativeID() ) )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_COOLTIME;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	const SITEM* pCardItemData = GLogicData::GetInstance().GetItem( pInvenCardItem->GetNativeID() );
	if ( !pCardItemData || !SIMPLE_CHECK_ITEM( pInvenCardItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// �����۽�ų����ī�带 ����� �������� �����´�;
	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		sNetMsgFB.emFlag = EMCHANGE_ITEMSKILL_FB_NOTITEM;
		SendToClient( &sNetMsgFB );

		return E_FAIL;
	}

	// ī�� �������� �Ҹ��Ų��;
	ConsumeInvenItem( pInvenCardItem->wPosX, pInvenCardItem->wPosY );

	// ������� �ٷ� �����ϰ� ���� ������ �����Ų��;
	if ( m_wRebuildType == EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL )
	{
		// ���� ������ ������ �����Ų��;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// �����۽�ų�� �����Ѵ�;
		pInvenItem->sItemCustom.GenerateLinkSkill();

		// �ٷ� DB �� �����Ѵ�;
		InvenUpdateItem( pInvenItem );

		// �α�
		LogItemLinkSkillOption( pInvenItem->sItemCustom );

		// ���� �����Ƿ� ������ ������ �����ؾ��Ѵ�;
		GLMSG::SNET_INVEN_ITEM_UPDATE sNetItemUpdateMsg;
		sNetItemUpdateMsg.wPosX = m_sRebuildItem.wPosX;
		sNetItemUpdateMsg.wPosY = m_sRebuildItem.wPosY;
		sNetItemUpdateMsg.sItemCustom = pInvenItem->sItemCustom;
		SendToClient( &sNetItemUpdateMsg );

		// ������ ���������� �˸���;
		pNetMsg->emResult = EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL;
		pNetMsg->bUseStampCard = false;//pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	// ���� ���¸� �����ϰ� ������� �޴´�, ���ÿ� ���� ������� �����Ѵ�;
	else if ( m_wRebuildType == EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT )
	{
		// �������� ����Ѵ�;
		m_sBackUpRebuildInvenItem = *pInvenItem;

		// �����۽�ų�� �����Ѵ�;
		m_sBackUpRebuildInvenItem.sItemCustom.GenerateLinkSkill();
		pNetMsg->emResult = EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT;
		pNetMsg->bUseStampCard = pNetMsg->fixOption.getNSize() != 0;
		pNetMsg->sItemCustom = m_sBackUpRebuildInvenItem.sItemCustom;
		SendToClient( pNetMsg );
	}
	return S_OK;
}