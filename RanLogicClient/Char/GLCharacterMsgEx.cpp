#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/SNS/GLSNSData.h"

#include "../Effect/GLFactEffect.h"
#include "../SNS/GLSNSClient.h"
#include "../Land/GLLandManClient.h"
#include "../Level/GLLevelFileClient.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

#include "./StaticHoldItem.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgAttackBlocking(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_BLOCKING *pNetMsg = (GLMSG::SNETPC_ATTACK_BLOCKING *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	const STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveBlocking(ActorID);	
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCharacter::MsgAttackAvoid(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_AVOID *pNetMsg = (GLMSG::SNETPC_ATTACK_AVOID *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	const STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveAVoid( ActorID );	
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCharacter::MsgAttackDamage(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_DAMAGE *pNetMsg = (GLMSG::SNETPC_ATTACK_DAMAGE *) pMsg;

    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	const STARGETID ActorID(m_emCrow, m_dwGaeaID,  m_vPosition);	
	pActor->ReceiveDamage(pNetMsg->nDamage, pNetMsg->dwDamageFlag, m_wSchool, ActorID);
	m_pGaeaClient->logDamageRecord(ActorID, pNetMsg->nDamage);

    if ( (pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRITICAL ) || ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) )	
        DxViewPort::GetInstance().CameraQuake ( 0.5f, 1.0f/25.0f, 1.0f );
}
void GLCharacter::MsgAttackHeal(NET_MSG_GENERIC* pMsg){
	GLMSG::SNETPC_ATTACK_HEAL *pNetMsg = (GLMSG::SNETPC_ATTACK_HEAL *) pMsg;

	ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
	if ( pActor )
	{
		STARGETID sACTOR(GetCrow(),GetGaeaID(),GetPosition());	
		pActor->ReceiveHeal( pNetMsg->nHeal, pNetMsg->dwHealFlag, SCHOOL_NONE, sACTOR );		
	}
}


void GLCharacter::MsgActionBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)pMsg;

	const EMACTIONTYPE eActionType = GetAction();

    if ( pNetMsg->emAction == GLAT_FALLING && eActionType >= GLAT_FALLING && eActionType != GLAT_FEIGN_DIE )
        return;
    else
    {
		// ��Ƴ��� ��� ��Ȱ ����Ʈ;
		if ( eActionType == GLAT_DIE && pNetMsg->emAction == GLAT_IDLE )
		{
			SetActState(EM_ACT_DIE);

			if ( !IsActState( EM_REQ_VISIBLENONE ) && !IsActState( EM_REQ_VISIBLEOFF ) )
			{
				D3DXMATRIX matEffect;
				D3DXMatrixTranslation( &matEffect, GetPosition().x, GetPosition().y, GetPosition().z );

				STARGETID sTargetID( CROW_PC, GetGaeaID(), GetPosition() );
				DxEffGroupPlayer::GetInstance().NewEffGroup(
					GLCONST_CHAR::strREBIRTH_EFFECT.c_str(),
					matEffect,
					&sTargetID,
					FALSE, 
					FALSE );
			}
		}

        TurnAction ( pNetMsg->emAction );
    }

    if ( pNetMsg->emAction == GLAT_FALLING )
    {
        // ����� ������ ����
        // ������� �׾��ٸ� PVP�� ó��
        if( pNetMsg->emCrowAssault != CROW_PC )
            DurabilityDropDeath( FALSE );
        else
            DurabilityDropDeath( TRUE );

    }

	if ( pNetMsg->dwFLAG != 0 )
	{
		ReSetActState(EM_ACT_ALL);
		SetActState(pNetMsg->dwFLAG);
	}
}

void GLCharacter::MsgUpdateState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_STATE *pNetMsg = (GLMSG::SNETPC_UPDATE_STATE *)pMsg;
    UpdateClientState ( pNetMsg );
}

void GLCharacter::MsgUpdateHireSummonState(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_UPDATE_HIRE_SUMMON_STATE* pNetMsg =
		static_cast< GLMSG::SNETPC_UPDATE_HIRE_SUMMON_STATE* >( pMsg );
	if ( pNetMsg )
	{
		m_sSummonable.hireInfo.dwGaeaID = pNetMsg->dwHireSummonGaeaID;
		m_sBasicSummonID = pNetMsg->sBasicSummonID;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_UpdateSummonAction,
			"-n, -b", GLAT_DIE, true );
	}
}

void GLCharacter::MsgUpdateCharacterSlotState ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_UPDATE_CHARSLOT_FC* pNetMsg =
		static_cast< GLMSG::SNETPC_UPDATE_CHARSLOT_FC* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	m_wSlotIndex = pNetMsg->wCharSlotNum;
	memcpy( m_arrLockSlotState, pNetMsg->arrSlotLockState, sizeof(bool)*EMCHAR_SLOT_DATA_SIZE );

	m_nOpenCharSlot = pNetMsg->nOpenCharSlot;
}

void GLCharacter::MsgUpdateExp(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_EXP *pNetMsg = (GLMSG::SNETPC_UPDATE_EXP *)pMsg;

    std::string strText;
    if ( pNetMsg->lnNowExp < m_sExperience.lnNow )
    {
		// �����̳� ����ġ ĸ���� ���ؼ� ����ġ�� �����ϴ°�� ���� �޼����� ǥ������ ����
		if( pNetMsg->bJumping == FALSE )
		{
			LONGLONG ExpTemp = ( m_sExperience.lnNow > m_sExperience.lnMax ) ? m_sExperience.lnMax - pNetMsg->lnNowExp : m_sExperience.lnNow - pNetMsg->lnNowExp;
			if ( 0 != ExpTemp )
			{
				// http://172.16.2.87/redmine/issues/4377 �ý��� �޽����� ����ġ �Ҹ� ��µǴ� ����
				// �켱�� 0�� ��쿡�� ������� �ʵ��ϸ� �����Ѵ�. ���̵��� �� ĳ���� �����ϰ� �ٽ� �ҷ����� FrameMove����
				// ������ ���� ����ġ�� ���� ����ġ�� �ٸ� ��� SNETPC_UPDATE_EXP �� �����µ� ���� ����ġ�� ��� ���̰� ���� ������ ����.
				// '0' �Ҹ� ���� '-38738383838' �Ҹ� �̷� �޽����� ��µȴٰ� �ϴµ� ������ �����ʾƼ� �켱�� ó������ �ʰ�
				// ������ �Ǹ� �ٽ� ó���Ѵ�.
				strText = sc::string::format( ID2GAMEINTEXT("DIS_EXP"), ExpTemp ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
			}
		}
    }
    else if ( pNetMsg->lnNowExp > m_sExperience.lnNow )
    {
#ifdef _DEBUG
        strText = sc::string::format( ID2GAMEINTEXT("INC_EXP2"), pNetMsg->lnNowExp-m_sExperience.lnNow ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
#endif
    }
	
	m_sExperience.lnNow = pNetMsg->lnNowExp;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}

void GLCharacter::MsgUpdateMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAR_UPDATE_MONEY_FC* pPacket = (GLMSG::NET_CHAR_UPDATE_MONEY_FC*) pMsg;
	SetInvenMoney( pPacket->GetMoney() );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_UpdateMoney,
		"-l", pPacket->GetMoney() );
}

void GLCharacter::MsgUpdateSp(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_SP *pNetMsg = (GLMSG::SNETPC_UPDATE_SP *)pMsg;

    m_sSP.nNow = pNetMsg->nNowSP;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}

void GLCharacter::MsgUpdateCp(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_CP *pNetMsg = (GLMSG::SNETPC_UPDATE_CP *)pMsg;

    m_sCP.wNow = pNetMsg->wNowCP;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}

void GLCharacter::MsgUpdateLp(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_LP *pNetMsg = (GLMSG::SNETPC_UPDATE_LP *)pMsg;

    int nDxLiving = pNetMsg->nLP - m_nLiving;
    m_nLiving = pNetMsg->nLP;

    if (nDxLiving == 0)
        return;

    std::string strText;

    if ( nDxLiving > 0 )
    {
        strText = sc::string::format( ID2GAMEINTEXT("INC_LIVING"), nDxLiving ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
    }
    else
    {
        strText = sc::string::format( ID2GAMEINTEXT("DIS_LIVING"), nDxLiving ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );
}

void GLCharacter::MsgUpdateSkillPoint(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_SKP *pNetMsg = (GLMSG::SNETPC_UPDATE_SKP *)pMsg;

    int nDx = int(pNetMsg->dwSkillPoint) - int(m_dwSkillPoint);

    m_dwSkillPoint = pNetMsg->dwSkillPoint;

	if( 0 < nDx )
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, sc::string::format( ID2GAMEINTEXT( "INC_SKP" ), nDx ) );
	else
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT( "DIS_SKP" ), nDx ) );

	// ��ųâ�� ������Ʈ �ؾ��Ѵ�.
	if( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdatePoint );
}

void GLCharacter::MsgUpdateBright(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_BRIGHT *pNetMsg = (GLMSG::SNETPC_UPDATE_BRIGHT *)pMsg;

    int nDx = int(pNetMsg->nBright) - int(m_nBright);

    m_nBright = pNetMsg->nBright;

    std::string strText;		
    if ( nDx > 0 )
    {
        strText = sc::string::format( ID2GAMEINTEXT("INC_BRIGHT"), nDx ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
    }
    else
    {
        strText = sc::string::format( ID2GAMEINTEXT("DIS_BRIGHT"), nDx ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );
}

void GLCharacter::MsgUpdateStats(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_STATS *pNetMsg = (GLMSG::SNETPC_UPDATE_STATS *)pMsg;

    int nDx = int(pNetMsg->wStatsPoint) - int(m_wStatsPoint);

    m_wStatsPoint = pNetMsg->wStatsPoint;

    std::string strText;
    if ( nDx > 0 )
    {
        strText = sc::string::format( ID2GAMEINTEXT("INC_STATS"), nDx ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
    }
    else
    {
        strText = sc::string::format( ID2GAMEINTEXT("DIS_STATS"), nDx ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
    }
}

void GLCharacter::MsgPickUpMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PICKUP_MONEY *pNetMsg = (GLMSG::SNETPC_PICKUP_MONEY *)pMsg;

	SetInvenMoney( pNetMsg->lnMoney );

    std::string strText = sc::string::format( ID2GAMEINTEXT("PICKUP_MONEY"), pNetMsg->lnPickUp ).c_str();
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
    DxSoundLib::GetInstance()->PlaySound ( "PICKUP_ITEM" );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_UpdateMoney,
		"-l", pNetMsg->lnMoney );
}

void GLCharacter::MsgUpdatePickupItem(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PICKUP_ITEM *pNetMsg = (GLMSG::SNETPC_PICKUP_ITEM *)pMsg;

    SITEM * pITEM = GLogicData::GetInstance().GetItem ( pNetMsg->sNID_ITEM );

    if ( pITEM )
    {
        std::string strText = sc::string::format( ID2GAMEINTEXT("PICKUP_ITEM"), pITEM->GetName() ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        DxSoundLib::GetInstance()->PlaySound ( "PICKUP_ITEM" );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLCharacter::MsgReqHoldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_HOLD_FB *pNetMsg = (GLMSG::SNETPC_REQ_HOLD_FB *)pMsg;

    switch ( pNetMsg->emHoldMsg )
    {
    case NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB:
        {
            switch ( pNetMsg->emHoldFB )
            {
            case EMHOLD_FB_NONKEEPSTORAGE:
                m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("STORAGE_SPAN_END") );
                break;
            };
        }
        break;
    };
}

void GLCharacter::MsgReqItemSort(NET_MSG_GENERIC* pMsg)
{
    if ( E_FAIL == MsgReqInvenItemSort( pMsg ) )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_REQ_ITEM_SORT_FAIL") );
    }
}

void GLCharacter::MsgReqItemSortStart(NET_MSG_GENERIC* pMsg)
{
    if ( E_FAIL == MsgReqInvenItemSortStart( pMsg ) )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_REQ_ITEM_SORT_FAIL") );
    }
}

void GLCharacter::MsgReqItemSortData(NET_MSG_GENERIC* pMsg)
{
    if ( E_FAIL == MsgReqInvenItemSortData( pMsg ) )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_REQ_ITEM_SORT_FAIL") );
    }
}

void GLCharacter::MsgReqItemSortEnd(NET_MSG_GENERIC* pMsg)
{
    if ( E_FAIL == MsgReqInvenItemSortEnd( pMsg ) )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_REQ_ITEM_SORT_FAIL") );
    }
}

void GLCharacter::MsgItemSortFail(NET_MSG_GENERIC* pMsg)
{
    SetEnableSendItemSortMsg();

	GLMSG::SNET_ITEM_SORT_FB* pNetMsg = (GLMSG::SNET_ITEM_SORT_FB*)pMsg;

    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_REQ_ITEM_SORT_FAIL") );
}

void GLCharacter::MsgInvenInsert(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_GCTRL_INVEN_INSERT
    GLMSG::SNETPC_INVEN_INSERT* pTemp = (GLMSG::SNETPC_INVEN_INSERT*) pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETPC_INVEN_INSERT RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if (RecvData.bVietnamInven)
		{
			m_cVietnamInventory.InsertItem( RecvData.Data.sItemCustom, 
				RecvData.Data.wPosX, RecvData.Data.wPosY, 
				false, RecvData.bNew );
		}
        else
        {
			m_cInventory.InsertItem( RecvData.Data.sItemCustom, 
				RecvData.Data.wPosX, RecvData.Data.wPosY, 
				RecvData.bAllLine, RecvData.bNew );
		}
	}
	else
	{
//#else
		m_cInventory.InsertItem( RecvData.Data.sItemCustom, 
			RecvData.Data.wPosX, RecvData.Data.wPosY, 
			RecvData.bAllLine, RecvData.bNew );
	}
//#endif

    // PET
    // ��ī�� ������ ��û�Ѵ�.
    SITEM* pItem = GLogicData::GetInstance().GetItem(RecvData.Data.sItemCustom.GetNativeID());
    if (pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && RecvData.Data.sItemCustom.PetDbNum() != 0)
    {
		if(	m_pGaeaClient->GetPetClient()->DbNum() !=  RecvData.Data.sItemCustom.PetDbNum() )
		{
			GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(RecvData.Data.sItemCustom.PetDbNum());
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
    }

    // VEHICLE
    // Ż�� ������ ��û�Ѵ�.
    pItem = GLogicData::GetInstance().GetItem ( RecvData.Data.sItemCustom.GetNativeID() );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && RecvData.Data.sItemCustom.m_VehicleDbNum != 0)
    {
        GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
        NetMsg.dwVehicleID = RecvData.Data.sItemCustom.m_VehicleDbNum;
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
		"-n, -n", RecvData.Data.wPosX, RecvData.Data.wPosY );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLCharacter::MsgReqVNInvenToInvenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB *pNetMsg = (GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB *)pMsg;

    m_cInventory.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->wNewPosX, pNetMsg->wNewPosY );
    m_cVietnamInventory.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLCharacter::MsgInvenDelete(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_DELETE *pNetMsg = (GLMSG::SNETPC_INVEN_DELETE *)pMsg;
    /*
    // PET
    // ��ī�� ������ �������ش�.
    SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( pInvenItem )
    {
    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    }
    */
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( pNetMsg->bVietnamInven )
		{
			m_cVietnamInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		}else{
			m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		}
	}
	else
	{
//#else
		m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	}
//#endif

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );

	//  [1/17/2013 gbgim]
	// - �κ����� �����Ϸ��� �������� Ȧ�� ������(���콺�� �� ������)�̶��
	//	 Ȧ�� �������� ������
	if ( holditem::IsHoldPlace( holditem::INVENTORY) )
	{
		if ( holditem::GetHoldPosX() == pNetMsg->wPosX && holditem::GetHoldPosY() == pNetMsg->wPosY )
			holditem::ReleaseHoldItem();
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
		"-n, -n", pNetMsg->wPosX, pNetMsg->wPosY );
}

void GLCharacter::MsgItemCoolTimeUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ITEM_COOLTIME_UPDATE *pNetMsg = (GLMSG::SNETPC_ITEM_COOLTIME_UPDATE *)pMsg;

    ITEM_COOLTIME sCoolTime;
    sCoolTime.dwID	= pNetMsg->dwID;
    sCoolTime.dwCoolID	= pNetMsg->dwCoolID;
    sCoolTime.tCoolTime	= pNetMsg->tCoolTime;
    sCoolTime.tUseTime	= pNetMsg->tUseTime;

    SetCoolTime ( sCoolTime, pNetMsg->emCoolType );
}

void GLCharacter::MsgItemCoolTimeError(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ITEM_COOLTIME_ERROR *pNetMsg = (GLMSG::SNET_ITEM_COOLTIME_ERROR *)pMsg;

    SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sNativeID );
    if (!pItem)
        return;

    std::string strText = sc::string::format( ID2GAMEINTEXT("ITEM_COOLTIME"), pItem->GetName() ).c_str();
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
}

void GLCharacter::MsgInvenDelInsert(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_GCTRL_INVEN_DEL_INSERT
    // GLMSG::SNETPC_INVEN_DEL_AND_INSERT *pNetMsg = (GLMSG::SNETPC_INVEN_DEL_AND_INSERT *)pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETPC_INVEN_DEL_AND_INSERT RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    /*
    // PET
    // ��ī�� ������ �������ش�.
    SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
    if ( pInvenItem )
    {
    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    }
    */
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( RecvData.bVietnamInven )
		{
			m_cVietnamInventory.DeleteItem ( RecvData.wDelX, RecvData.wDelY );
			m_cVietnamInventory.InsertItem ( RecvData.sInsert.sItemCustom, RecvData.sInsert.wPosX, RecvData.sInsert.wPosY );
		}
        else
        {
			m_cInventory.DeleteItem ( RecvData.wDelX, RecvData.wDelY );
			m_cInventory.InsertItem ( RecvData.sInsert.sItemCustom, RecvData.sInsert.wPosX, RecvData.sInsert.wPosY );

			// PET
			// ��ī�� ������ ��û�Ѵ�.
			SITEM* pItem = GLogicData::GetInstance().GetItem ( RecvData.sInsert.sItemCustom.GetNativeID() );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && RecvData.sInsert.sItemCustom.PetDbNum() != 0  )
			{
				if(	m_pGaeaClient->GetPetClient()->DbNum() !=  RecvData.sInsert.sItemCustom.PetDbNum())
				{
					GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(RecvData.sInsert.sItemCustom.PetDbNum());
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				}
			}

			// VEHICLE
			// Ż�� ������ ��û�Ѵ�.
			pItem = GLogicData::GetInstance().GetItem ( RecvData.sInsert.sItemCustom.GetNativeID() );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && RecvData.sInsert.sItemCustom.m_VehicleDbNum != 0 ) 
			{
				GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
				NetMsg.dwVehicleID = RecvData.sInsert.sItemCustom.m_VehicleDbNum;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}
		}
	}
	else
	{
//#else
		m_cInventory.DeleteItem ( RecvData.wDelX, RecvData.wDelY );
		m_cInventory.InsertItem ( RecvData.sInsert.sItemCustom, RecvData.sInsert.wPosX, RecvData.sInsert.wPosY );
		// PET
		// ��ī�� ������ ��û�Ѵ�.
		SITEM* pItem = GLogicData::GetInstance().GetItem ( RecvData.sInsert.sItemCustom.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && RecvData.sInsert.sItemCustom.PetDbNum() != 0 )
		{
			if(	m_pGaeaClient->GetPetClient()->DbNum() !=  RecvData.sInsert.sItemCustom.PetDbNum())
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(RecvData.sInsert.sItemCustom.PetDbNum());
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}
		}

		// VEHICLE
		// Ż�� ������ ��û�Ѵ�.
		pItem = GLogicData::GetInstance().GetItem ( RecvData.sInsert.sItemCustom.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && RecvData.sInsert.sItemCustom.m_VehicleDbNum != 0 )
		{
			GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
			NetMsg.dwVehicleID = RecvData.sInsert.sItemCustom.m_VehicleDbNum;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
//#endif

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
		"-n, -n", RecvData.sInsert.wPosX, RecvData.sInsert.wPosY );
}

void GLCharacter::MsgPutonRelease(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PUTON_RELEASE *pNetMsg = (GLMSG::SNETPC_PUTON_RELEASE *)pMsg;

    EMITEM_TYPE emTYPE_RELOAD = ITEM_NSIZE;
    SNATIVEID sNID_RELOAD = NATIVEID_NULL();
    EMSLOT emLHand = GetCurLHand();

    SITEMCUSTOM sItemCustom = GLCHARLOGIC::GET_SLOT_ITEM(pNetMsg->emSlot);
    SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID());
    if (!pItem)
        return;

    if ( pNetMsg->emType == EMPUTONRELEASE_TYPE_REFRESH )
    {
        //	�������� ���������� �˾ƺ�.
        if ( pNetMsg->emSlot == emLHand && GLCHARLOGIC::VALID_SLOT_ITEM(pNetMsg->emSlot) )
        {
            if ( pItem->sBasicOp.emItemType==ITEM_ARROW || pItem->sBasicOp.emItemType==ITEM_CHARM ||
                pItem->sBasicOp.emItemType==ITEM_BULLET )
            {
                emTYPE_RELOAD = pItem->sBasicOp.emItemType;
                sNID_RELOAD = sItemCustom.GetNativeID();
            }
        }
    }

    // Note : Ư���� �����ۿ� ���� �޼��� ó��
    //		  �տ� ��� �ִ� �������� �ƴ϶��.
    if ( pItem && pNetMsg->emSlot != SLOT_HOLD )
    {
        // Note : ������������ ���� ������ �Ҹ�
        //
        if ( pNetMsg->emType == EMPUTONRELEASE_TYPE_TURN )
        {
            switch ( pItem->sBasicOp.emItemType )
            {
            case ITEM_ANTI_DISAPPEAR:
            case ITEM_INC_GRINDINGRATE:
            case ITEM_ANTI_RESET:
                {
                    std::string strText = sc::string::format( ID2GAMEINTEXT("ITEM_GRINDING_DISAPPEAR"), sItemCustom.GETNAME() ).c_str();
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
                }
                break;
            }
        }
        // Note : �װ��� �ƴ϶��.
        else
        {
            switch ( pItem->sBasicOp.emItemType )
            {
            case ITEM_ANTI_DISAPPEAR: 
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIDISAPPEAR_FREE") ); 
                break;
            case ITEM_INC_GRINDINGRATE:
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_INC_GRINDINGRATE_FREE") ); 
                break;
            case ITEM_ANTI_RESET:
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIRESET_FREE") ); 
                break;
            }	
        }
    }

    //	SLOT ����.
    GLCHARLOGIC::RELEASE_SLOT_ITEM ( pNetMsg->emSlot );
    GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
	GLCHARLOGIC::UPDATE_DATA( 0.0f, 0.0f, TRUE );
	this->SfSetWeaponBoostInfo();

    //	���� ����.
    UpdateSuit( TRUE );

    //	Note : ���ϸ��̼� �ʱ�ȭ.
    ReSelectAnimation();

    if ( emTYPE_RELOAD!=ITEM_NSIZE )
    {
        SINVENITEM* pInvenItem = m_cInventory.FindItem ( emTYPE_RELOAD, sNID_RELOAD );
        if ( !pInvenItem )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_Character_RemovePutOnCount,
				"-n", (int) emLHand );
            return;
		}

        GLMSG::SNETPC_REQ_INVEN_TO_SLOT NetMsg;
        NetMsg.emToSlot = emLHand;
        NetMsg.wPosX = pInvenItem->wPosX;
        NetMsg.wPosY = pInvenItem->wPosY;

        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }

    // Ż�� �������̸� ���� ��Ȱ��
    if ( pNetMsg->emSlot == SLOT_VEHICLE )
    {
        m_sVehicle.SetActiveValue( false );
        m_sVehicle.RESET ();

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_EquipVehicle,
			"-b, -n, -n, -n", false, -1, -1, 0 );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_UpdateSlot );
}

void GLCharacter::MsgPutonUpdate(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;    
    GLMSG::SNETPC_PUTON_UPDATE Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    // ȭ���̳� �������� �������� �ڵ� �Ű����� ���
    if (Data.m_SlotRelease != SLOT_TSIZE)
        GLCHARLOGIC::RELEASE_SLOT_ITEM(static_cast<EMSLOT> (Data.m_SlotRelease));

    // Ư���� �����ۿ� ���� �������� ó��
    if (Data.m_Slot < SLOT_NSIZE_S)
    {
        const SITEM* pItem_Dest   = GLogicData::GetInstance().GetItem(m_PutOnItems[Data.m_Slot].GetNativeID());
        const SITEM* pItem_Source = GLogicData::GetInstance().GetItem(Data.m_Item.GetNativeID());

        if (pItem_Dest)
        {
            switch ( pItem_Dest->sBasicOp.emItemType )
            {
            case ITEM_ANTI_DISAPPEAR: 
                m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIDISAPPEAR_FREE")); 
                break;
            case ITEM_INC_GRINDINGRATE:
                m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_INC_GRINDINGRATE_FREE")); 
                break;
            case ITEM_ANTI_RESET:
                m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIRESET_FREE")); 
                break;
            }
        }

        if ( pItem_Source )
        {
            switch ( pItem_Source->sBasicOp.emItemType )
            {
            case ITEM_ANTI_DISAPPEAR: 
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIDISAPPEAR_USE") ); 
                break;
            case ITEM_INC_GRINDINGRATE:
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_INC_GRINDINGRATE_USE") ); 
                break;
            case ITEM_ANTI_RESET:
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EQUIPMENT_ANTIRESET_USE") ); 
                break;
            }
        }
    }

    //	Note : ������ ���� �䱸ġ�� ������ ��� ���� ���, SP ��� ��ġ.
    //

    //	SLOT ����.
    GLCHARLOGIC::SLOT_ITEM(Data.m_Item, static_cast<EMSLOT> (Data.m_Slot));

    if ( Data.m_Slot != SLOT_HOLD )
    {
		//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014)
        WORD wACP = GLCHARLOGIC::CALC_ACCEPTP_EX ( Data.m_Item );
        if ( wACP > 0 )
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("ITEMPUTON_ACCEPT_SP"), wACP ).c_str();	
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, strText );	
        }

        GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
		GLCHARLOGIC::UPDATE_DATA( 0.0f, 0.0f, TRUE );
		this->SfSetWeaponBoostInfo();

        //	���� ����.
        UpdateSuit( TRUE );

        //	Note : ���ϸ��̼� �ʱ�ȭ.
        ReSelectAnimation();				
    }

    // Ż�� ������ ������ ������ �����ɴϴ�.
    if ( Data.m_Slot == SLOT_VEHICLE )
    {
        ReqVehicleUpdate();
    }

    // Note : �Ϸ� �� Ư������ ó��
    if (m_pITEMS[Data.m_Slot])
    {
        switch (m_pITEMS[Data.m_Slot]->sBasicOp.emItemType )
        {
            // Note : ��ȭ�� ��ȭ�� ���� ��� �Ұ�
        case ITEM_INC_GRINDINGRATE:
        case ITEM_ANTI_RESET:
            {
                DWORD dwOption( EMGRINDINGOPTION_NONE );

                for ( DWORD i=0; i<SLOT_NSIZE_S; ++i )
                {
                    if ( !m_pITEMS[i] )
                    {
                        continue; 
                    }

                    switch ( m_pITEMS[i]->sBasicOp.emItemType )
                    {
                    case ITEM_INC_GRINDINGRATE	: dwOption |= EMGRINDINGOPTION_INC_GRINDINGRATE; break;
                    case ITEM_ANTI_RESET		: dwOption |= EMGRINDINGOPTION_ANTIRESET;		 break;
                    }
                }

                // Note : ��ȭ�� ��ȭ�� ���� ����� �Ұ����մϴ�.
                if ( (dwOption&EMGRINDINGOPTION_INC_GRINDINGRATE) && (dwOption&EMGRINDINGOPTION_ANTIRESET) )
                {
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EQUIPMENT_GRINDINGITEM_DUPLICATION_WARNING_0") ); 
                }
            }
            break;
        }
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_UpdateSlot );
}

void GLCharacter::MsgPutonChange(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PUTON_CHANGE *pNetMsg = (GLMSG::SNETPC_PUTON_CHANGE *)pMsg;

    // ��/�������� ����
    SetUseArmSub( pNetMsg->bUseArmSub );

	GLCHARLOGIC::INIT_DATA ( FALSE, FALSE, pNetMsg->fCONFT_HP_RATE );
	GLCHARLOGIC::UPDATE_DATA( 0.0f, 0.0f, TRUE );
	this->SfSetWeaponBoostInfo();

	//	���� ����.
	UpdateSuit( TRUE );

    //	Note : ���ϸ��̼� �ʱ�ȭ.
    ReSelectAnimation();

	m_bReqSwapArms = FALSE;

	// ���������� �ش� ������ �Ϸ�Ǿ����� �˸�
	GLREACTION_SWAP_ARMS sReaction;
	m_ReservationAction.FeedbackReaction(sReaction);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

	// �������̽� ����
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Character_ArmSwap,
		"-b", pNetMsg->bUseArmSub );
}

void GLCharacter::MsgReqLevelUpFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_LEVELUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_LEVELUP_FB *)pMsg;

    STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
    DxEffGroupPlayer::GetInstance().NewEffGroup
        (
        GLCONST_CHAR::strLEVELUP_EFFECT.c_str(),
        GetTransMatrix(),
        &sTargetID,
		TRUE,
		TRUE
        );

	// ������ ��� pNetMsg->bCalExp �� false �̿��� �Ѵ�. �׷��� ������ Ŭ���̾�Ʈ ȭ�� ����ġ�� - �� ǥ�õȴ�.
	LEVLEUP ( pNetMsg->bInitNowLevel, pNetMsg->bJumping, pNetMsg->bCalExp );

    //	Note : �����Ŀ� ����� ��ġ ����.
    m_wLevel = pNetMsg->wLevel;
    m_wStatsPoint = pNetMsg->wStatsPoint;
    m_dwSkillPoint = pNetMsg->dwSkillPoint;

    // �ּ�����
    if ( m_dwUserLvl < USER_USER_GM )
    {
        SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pNetMsg->sMapID );
        if ( pMapNode )
        {
            GLLevelFileClient cLevelFile(m_pGaeaClient);
            if( cLevelFile.LoadFile ( pMapNode->m_LevelFile.c_str(), NULL ) )

            {
                const SLEVEL_REQUIRE* pRequire = cLevelFile.GetLevelRequire ();
                EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
                if( pRequire )
					emReqFail = pRequire->ISCOMPLETE ( this );
                if( emReqFail == EMREQUIRE_LEVEL )
                {
					//!! SF_TODO
                    //m_pInterface->SetMapCheckType( UI::EMMAPCHECK_LEVELUP );
                    //m_pInterface->UiShowGroupFocus ( MAP_REQUIRE_CHECK );
                }
            }

        }
    }		 



#ifndef CH_PARAM_USEGAIN 
    //** Add EventTime
    if( m_bEventStart )
    {
        if( m_bEventApply )
        {
            if( m_wLevel < m_EventStartLv || m_wLevel > m_EventEndLv )
            {
                m_bEventApply = false;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_BonusTime_EndEvent );
            }
        }else{
            if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
            {
                m_bEventApply = true;
                const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
                m_tLoginTime =  crtTime.GetTime();

				int nTime = 0;
				int nRemainTime = 0;
				if( false == m_bEventBuster )
				{
					nTime = m_EventStartTime;
					nRemainTime = m_RemainEventTime;
				}
				else
				{
					nTime = m_EventBusterTime;
					nRemainTime = m_RemainBusterTime;
				}

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_BonusTime_StartEvent,
					"-b, -n, -n, -f, -f",
					m_bEventBuster, nTime, nRemainTime, m_EventBusterExpGain, m_EventBusterItemGain );
            }
        }
    }
#endif

    std::string strText = sc::string::format( ID2GAMEINTEXT("LEVELUP"), m_wLevel ).c_str();
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, strText );

    if ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
        && m_pGaeaClient->GetTutorial()->IsSubFourStep() )
    {
        if ( !m_pGaeaClient->GetTutorial()->IsLevelUpOne() )
        {	// �ѹ��� ������ �ؽ�Ʈ�� ������ ���ٸ�,
            // Memo : Ʃ�丮�� �� ������ �ÿ��� ������ �ؽ�Ʈ �����ش�.
            m_pGaeaClient->GetTutorial()->SetTutorialStep( m_pGaeaClient->GetTutorial()->LEVEL_UP_STEP
                , m_pGaeaClient->GetTutorial()->ONE_ );
        }
    }


    // Note : ���� �ʱ�ȭ â�� �����ִٸ� ����Ȱ����� �����Ѵ�.
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StatsReset_ReCalc );

    //
    //mjeon.sns
    //

    char buffer[32]={0,};

    _itoa_s((int)m_wLevel, buffer, 10);

    std::string strLevel(buffer);

    RANPARAM::SNSFACEBOOK_SET& sOptFaceBook = RANPARAM::sSNSFaceBook;
    RANPARAM::SNSTWITTER_SET&  sOptTwitter  = RANPARAM::sSNSTwitter;

    if ( sOptFaceBook.bLEVELUP && GLCONST_SNS::dw_REGISTER_LEVEL_UP <= m_wLevel )
    {
        m_pGaeaClient->GetSNSClient()->FBWallPost ( strLevel, EMSNS_LEVEL_UP, EMSNSREQTYPE_AUTOREQ );
    }

    if ( sOptTwitter.bLEVELUP && GLCONST_SNS::dw_REGISTER_LEVEL_UP <= m_wLevel )
    {
        std::string msg = strLevel + ID2GAMEINTEXT("EMSNS_DESC_LEVELUP");

        m_pGaeaClient->GetSNSClient()->TTTweetMsg( msg, EMSNSREQTYPE_AUTOREQ );	
    }
}

void GLCharacter::MsgReqStatsUpFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_STATSUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP_FB *)pMsg;
    STATSUP(pNetMsg->emStats);
	INIT_DATA(FALSE,FALSE);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_ApplyStatPoint );
}

void GLCharacter::MsgReqStatsUpExFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_STATSUP_EX_FB *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP_EX_FB*)pMsg;

	for(int i = 0; i < EMSIZE; ++i)
	{
		for(int k=0; k < pNetMsg->emStatsArray[i]; ++k)
			STATSUP(EMSTATS(i));
	}

	INIT_DATA(FALSE,FALSE);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_ApplyStatPoint );
}

void GLCharacter::MsgReqLearnSkillFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_LEARNSKILL_FB *pNetMsg = (GLMSG::SNETPC_REQ_LEARNSKILL_FB *)pMsg;

    // different enum types compared
    if ( pNetMsg->emCHECK == EMSKILL_LEARN_OK )
    {
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->skill_id.wMainID, pNetMsg->skill_id.wSubID );
		if( !pSkill )
			return;

        LEARN_SKILL_SIMPLE( pNetMsg->skill_id );

		// ��ų�� ������� �� ��ų�� ���Ե� Effect�� �̸� �ε��۾��� �Ѵ�;
		PreLoadEffect( pSkill );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
    }
    else
    {
        //	��� ���� ���� ���� �޽��� ���.
    }
}

void GLCharacter::MsgReqSkillUpFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_SKILLUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILLUP_FB *)pMsg;
    if ( pNetMsg->emCHECK == EMSKILL_LEARN_OK )
    {
        //	Note : ��ų ���� ������.
        GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->sSkill.sNativeID );
        if (!pSkill)
            return;

        SKILL_MAP_ITER iter_del = m_ExpSkills.find ( pNetMsg->sSkill.sNativeID.dwID );
        if ( iter_del!=m_ExpSkills.end() )	m_ExpSkills.erase ( iter_del );

        m_ExpSkills[pNetMsg->sSkill.sNativeID.dwID] = pNetMsg->sSkill;

		UPDATE_CHANGE_CHARSKILL( pNetMsg->sSkill );

		SET_VALIDATION_EXPSKILL_MAP();

        //	Note : ��ú� ��ų�� ��ȭ �Ҷ� �ʱ� ��ġ���� ��� �� ����Ѵ�.
        //
        if( pSkill->HavePassiveFunc() )
            INIT_DATA ( FALSE, FALSE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
}

void GLCharacter::MsgReqRnSkillUpComplete(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE* pNetMsg = (GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE*)pMsg;
	if ( pNetMsg->dwComplete & GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEARN )
	{
		STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
		DxEffGroupPlayer::GetInstance().NewEffGroup	(
			GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
			GetTransMatrix(),
			&sTargetID,
			FALSE, 
			TRUE );
	}
	else if ( pNetMsg->dwComplete & GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEVELUP )
	{
		STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
		DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strSKILL_UP_EFFECT.c_str(),
			GetTransMatrix(),
			&sTargetID,
			FALSE, 
			TRUE );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
}

void GLCharacter::MsgReqSkillFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_SKILL_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILL_FB *)pMsg;
    if ( pNetMsg->emSKILL_FB != EMSKILL_OK )
    {
//         if( pNetMsg->emSKILL_FB == EMSKILL_NOTREBIRTH )
//         {
//             std::string strText = sc::string::format( ID2GAMEINTEXT("NON_REBIRTH_SKILL_MSG"), pNetMsg->szName ).c_str();
//             m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
//         }else{
//             if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
// 				TurnAction(GLAT_IDLE);			
// 		}		
		switch ( pNetMsg->emSKILL_FB )
		{
		case EMSKILL_NOTREBIRTH:
			{
				const std::string strText = sc::string::format( ID2GAMEINTEXT("NON_REBIRTH_SKILL_MSG"), pNetMsg->szName ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
			}
			break;
		case EMSKILL_REBIRTHBLOCK:
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NON_REBIRTH_BLOCK") );
			break;
		default:
			{
				if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
					TurnAction(GLAT_IDLE);	
			}
			break;
		}
	}

    if( pNetMsg->emSKILL_FB == EMSKILL_OK )
    {
        DurabilityDropUseSkill();
    }

    if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
    {
        if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
            && m_pGaeaClient->GetTutorial()->IsSubSixStep()
            || m_pGaeaClient->GetTutorial()->IsAttackStep()
            && m_pGaeaClient->GetTutorial()->IsSubEightStep() )
        {
            m_pGaeaClient->GetTutorial()->CheckAttackForTutorial( m_sActionID.emCrow, m_sActionID.GaeaId);
        }
    }
}

void GLCharacter::MsgSkillDealyAction( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION *NetMsgDealy = (GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION *)pMsg;
	NetMsgDealy->skill_id;
	NetMsgDealy->wLEVEL;
	SCHARSKILL sSkill( NetMsgDealy->skill_id, NetMsgDealy->wLEVEL);
	SKILLTARGETDATA sTargetData;

	for(int i = 0; i < NetMsgDealy->wTARNUM; i ++ )
	{
		sTargetData.TargetIDs[i] = NetMsgDealy->sTARIDS[i];
		sTargetData.nTarget++;
	}
	sTargetData.TargetID = NetMsgDealy->sTARIDS[0];

	m_pWorkingSkill->StartDealyAction( sSkill, this, &sTargetData, false );

	EMSLOT				emRHand = GetCurRHand();
	SANIMSTRIKE			sStrike;
	sStrike.m_emPiece	= VALID_SLOT_ITEM( emRHand ) ? PIECE_RHAND : PIECE_GLOVE;
	sStrike.m_emEffect	= EMSF_TARGET;
	sStrike.m_dwFrame	= 0;

	SkillEffectDealyAction( sStrike, NetMsgDealy->skill_id, &sTargetData );
}

void GLCharacter::MsgReqSkillHoldRsFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB *)pMsg;

    WORD wSlot = pNetMsg->wRESETSLOT;

    if( wSlot < SKILLFACT_SIZE && m_sSKILLFACT[wSlot].sNATIVEID != NATIVEID_NULL() )
    {
        EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX(wSlot);
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[wSlot].sNATIVEID );

		if( emType == EMSKILLFACT_TYPE_LUNCHBOX )
        {
			GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
			if( NULL == pLand )
				return;

			// ���� ���� �Ұ��� ����
			bool bForbid = pLand ? pLand->IsLunchBoxForbid() : true;
			if( true == bForbid )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Buff_ImpossibleDelete,
					"-s", pSkill->GetName() );

				return;
			}

           if( pSkill )
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("LUNCHBOX_EXTINCTION"), pSkill->GetName() ).c_str();
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, strText );
            } //if
        } //if	

		//FB
		// ���� ���� �� ����ϰ� ������ ��ų�� ���� ���� ó���� �� �� ����,
		// �ؼ� �Ʒ��� ���� ������ ó���ϰ�� ������ �ݵ�� �ٲ�� ��;
		// ���(��ų) ���� ���ȭ �Ͽ� �������̽��� ����, ����, ����, ����, �˻���� �����ϴ� ������;
		UPDATE_DATA_END(wSlot,FALSE, FALSE, TRUE );
    }
}


void GLCharacter::MsgReqInvenLunchBoxFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB *pNetMsg = (GLMSG::SNETPC_REQ_INVEN_LUNCHBOX_FB *)pMsg;
	const SNATIVEID& skillID = pNetMsg->sSKILLID;
    if ( pNetMsg->emFB != EMLUNCHBOX_FB_OK )
		return;

	// MsgReqInvenLunchBox����SendToClient�� �����⶧���� ������ �̹� ��ų�� ����Ǿ�( �ڽ� )
	// �ѹ��� �� �ʿ�� ������. �ѹ����ؼ� �޼����� �ΰ�����.
	//RecvExperience( skillID, pNetMsg->wLEVEL );
}

void GLCharacter::MsgSkillConsumeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLCONSUME_FB *pNetMsg = (GLMSG::SNETPC_SKILLCONSUME_FB *)pMsg;

    EMSLOT emLHand = GetCurLHand();

    SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);

    if ( pItem && pItem->sDrugOp.bInstance )
		m_PutOnItems[emLHand].wTurnNum = pNetMsg->wTurnNum;

    m_sHP.nNow = pNetMsg->nNowHP;
    m_sMP.nNow = pNetMsg->nNowMP;
    m_sSP.nNow = pNetMsg->nNowSP;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}


void GLCharacter::MsgSkillFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)pMsg;

    if( pNetMsg->emCrow != CROW_PC )
        return;

    m_sHP.VARIATION ( pNetMsg->nVAR_HP );
    m_sMP.VARIATION ( pNetMsg->nVAR_MP );
    m_sSP.VARIATION ( pNetMsg->nVAR_SP );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

    //	������ �޽���.
    if ( pNetMsg->nVAR_HP < 0 )
    {
        if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_SHOCK )
			ReceiveShock ();
        else
			ReceiveSwing ();

        //	Note : ��ų�� ����Ʈ�� ������ �ߵ���Ŵ.
        STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
        sACTOR.vPos = m_pGaeaClient->GetTargetPos ( sACTOR );

		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( 
			vPos, static_cast<WORD>(-pNetMsg->nVAR_HP), pNetMsg->dwDamageFlag, 
			UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );

        SKT_EFF_HOLDOUT ( sACTOR, pNetMsg->dwDamageFlag );
		RELEASE_ENDURE_PROC();

        if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
        {
            // ����Ÿ�� ����Ʈ
			const D3DXVECTOR3& vPosition = GetPosition();
            D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

            D3DXVECTOR3 vDIR_ORG(1,0,0);
            float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

            D3DXMATRIX matTrans;
            D3DXMatrixRotationY ( &matTrans, fdir_y );
            matTrans._41 = vPosition.x;
            matTrans._42 = vPosition.y + 10.0f;
            matTrans._43 = vPosition.z;

            //	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
            DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, TRUE );
        }

		if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CA )
		{			
			const D3DXVECTOR3& vPosition = GetPosition();
			STARGETID sMyID(CROW_PC,m_dwGaeaID, vPosition);

			D3DXMATRIX matTrans;
			D3DXMatrixTranslation(&matTrans, vPosition.x, vPosition.y, vPosition.z );

			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y + 10.0f;
			matTrans._43 = vPosition.z;

			//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCA_EFFECT.c_str(), matTrans, &sMyID, TRUE, TRUE );
		}

        // �ǰݽ� ������ ����
        // ���� ��밡 �������
        // ��ų ���� Ŭ���̾�Ʈ���� ���޵Ǵ� �޽���
        if( pNetMsg->sACTOR.emCrow == CROW_PC )
            DurabilityDropHit( abs( pNetMsg->nVAR_HP ), TRUE );
        else
            DurabilityDropHit( abs( pNetMsg->nVAR_HP ), FALSE );

		m_pGaeaClient->logDamageRecord(sACTOR, -pNetMsg->nVAR_HP, pNetMsg->sSkillID, pNetMsg->wSkillLevel, pNetMsg->dwDamageFlag);
        //
    }
}


void GLCharacter::MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)pMsg;

    //	������ ��ų�� ��� ��ų ���� �߰���.
    if ( pNetMsg->skill_id != NATIVEID_NULL() )
    {
        if ( pNetMsg->fPARAM )
            m_sLINKHP.fRef = pNetMsg->fPARAM; //MaxHP ������ ������ ���� ����

		switch( pNetMsg->sEffInfo.eType )
		{
		case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_ATTACK :
			{
				RECEIVE_SKILLCA( pNetMsg );

				{
					const D3DXVECTOR3& vPosition = GetPosition();
					STARGETID sMyID(CROW_PC,m_dwGaeaID, vPosition);

					D3DXMATRIX matTrans;
					D3DXMatrixTranslation(&matTrans, vPosition.x, vPosition.y, vPosition.z );

					matTrans._41 = vPosition.x;
					matTrans._42 = vPosition.y + 10.0f;
					matTrans._43 = vPosition.z;

					//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
					DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCA_EFFECT.c_str(), matTrans, &sMyID, TRUE, TRUE );
				}
			}
			break;
		case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET :
			{
				RECEIVE_SKILLFACT ( pNetMsg );
			}
			break;
		default :
			{
				RECEIVE_SKILLFACT ( pNetMsg->skill_id
					, pNetMsg->wLEVEL
					, pNetMsg->wSELSLOT
					, pNetMsg->sID 
					, pNetMsg->sTIME
					);
			}
			break;
		}
		
		        
        FACTEFF::NewSkillFactEffect(
			STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), 
			m_pSkinChar, pNetMsg->skill_id, GetTransMatrix(), GetDirect() );
    }
}


void GLCharacter::MsgSkillHoldRsBrd(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_SKILLHOLD_RS_BRD*		pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD*)pMsg;

	//	Note : ��ų fact ���� ����.
	//		�ٷ� ���� ���� �ʰ� ���⼭ �ð� �����Ͽ� ���� ����ǰ� ��. ( ����Ʈ ���� ����. )
	for( size_t i = 0; i < SKILLFACT_SIZE; ++i )
	{
		if( pNetMsg->bRESET[i] && m_sSKILLFACT[i].sNATIVEID != NATIVEID_NULL() )
		{
			EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX( i );
			if( emType == EMSKILLFACT_TYPE_LUNCHBOX )
			{
				if ( pNetMsg->emResetType != EMSKILLHOLDRESET_MOVEFIELDSRV )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[i].sNATIVEID );
					if( pSkill )
					{
						std::string strText = sc::string::format( ID2GAMEINTEXT("LUNCHBOX_EXTINCTION"), pSkill->GetName() ).c_str();
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, strText );
					} //if
				} //if
			} //if

			//BRD
			// ���� ���� �� ����ϰ� ������ ��ų�� ���� ���� ó���� �� �� ����,
			// �ؼ� �Ʒ��� ���� ������ ó���ϰ�� ������ �ݵ�� �ٲ�� ��;
			// ���(��ų) ���� ���ȭ �Ͽ� �������̽��� ����, ����, ����, ����, �˻���� �����ϴ� ������;
			GLCharacter::UPDATE_DATA_END(i);						
		} //if
	} //for
}

void GLCharacter::MsgSkillCancelBrd(NET_MSG_GENERIC* pMsg)
{
    if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
		TurnAction ( GLAT_IDLE );
	}
}

void GLCharacter::MsgSaveSkillFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SAVESKILLFACT_BRD*	pNetMsg = (GLMSG::SNETPC_SAVESKILLFACT_BRD*)pMsg;
	const DWORD nSkillFactSize(pNetMsg->nSkillFactSize);
    for( DWORD i = 0; i < nSkillFactSize; ++i )
    {
        SCHARSAVESKILLFACT &sSAVESKILLFACT = pNetMsg->sSAVESKILLFACT[i];
        if( sSAVESKILLFACT.sNATIVEID == NATIVEID_NULL() )
            continue;

		/// ���ö��� ��� ���������� ������ �� ����;
		/// Age �� 0 ������ �� ����;
		EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX ( sSAVESKILLFACT.wSLOT );
		SSKILLFACT::STIME sTime( sSAVESKILLFACT );
		if ( sTime.fAGE < 0.f ||
			( _SKILLFACT_INFINITY == sTime.fAGE &&
			( EMSKILLFACT_TYPE_EXP_LUNCHBOX == emType || EMSKILLFACT_TYPE_LUNCHBOX == emType ) ) )
		{
			sc::writeLogError( sc::string::format(
				"[ Skill ] [ Invalid SkillFact %1%. ID : %2%/%3%, Age : %4%, Life : %5%, IsEXPIREDTIME : %6% ]",
				sSAVESKILLFACT.wSLOT,
				sSAVESKILLFACT.sNATIVEID.Mid(),
				sSAVESKILLFACT.sNATIVEID.Sid(),
				sTime.fAGE,
				sTime.fLIFE,
				sTime.IsEXPIREDTIME ? "TRUE" : "FALSE" ) );

			sSAVESKILLFACT.Reset();

			continue;
		}

		// QQQ;
        if( RECEIVE_SKILLFACT( sSAVESKILLFACT.sNATIVEID, 
							   static_cast<WORD>(sSAVESKILLFACT.cLEVEL), 
							   sSAVESKILLFACT.wSLOT, 
                               sSAVESKILLFACT.sID,
							   SSKILLFACT::STIME(sSAVESKILLFACT), 
							   sSAVESKILLFACT.dwCount, sSAVESKILLFACT.fLaunchTime, sSAVESKILLFACT.fLaunchValue) )
        {
            EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX ( sSAVESKILLFACT.wSLOT );
            if( emType == EMSKILLFACT_TYPE_LUNCHBOX )		// ���ö��̰� ���Ĺ��� ��� �Ұ��� �����̸� ��Ȱ��ȭ �޼����� ����ش�.
            {
                GLLandManClient*	pLand			= m_pGaeaClient->GetActiveMap();
                BOOL				bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
                if( bLunchBoxForbid )
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("LUNCHBOX_DISABLE_AREA") );
            } //if
        } //if
    } //for
} //GLCharacter::MsgSaveSkillFactBrd


void GLCharacter::MsgStateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();
    if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
    {
        FACTEFF::DeleteBlowSingleEffect ( STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, m_sSTATEBLOWS );
        for( int i = 0; i < EMBLOW_MULTI; ++i )
			m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; 
    } //if

    SSTATEBLOW*		pSTATEBLOW = NULL;
    if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
		pSTATEBLOW = &m_sSTATEBLOWS[0];
    else
		pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];

    pSTATEBLOW->emBLOW		= pNetMsg->emBLOW;
    pSTATEBLOW->fAGE		= pNetMsg->fAGE;
    pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
    pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;
    pSTATEBLOW->wSchool		= pNetMsg->wSchool;

    FACTEFF::NewBlowEffect(
		STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
		m_pSkinChar, pSTATEBLOW->emBLOW, GetTransMatrix(), GetDirect() );	// ȿ�� ����.
} //GLCharacter::MsgStateBlowBrd

void GLCharacter::MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)pMsg;

    for ( int i=0; i<EMBLOW_MULTI; ++i )
    {
        EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;

        if ( emBLOW==EMBLOW_NONE )						continue;

        EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
        if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

        DISABLEBLOW(i);
        FACTEFF::DeleteBlowEffect ( STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), m_pSkinChar, emBLOW );
    }
}

void GLCharacter::MsgInvenDrugUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_INVEN_DRUG_UPDATE *)pMsg;
    SINVENITEM* pInvenItem = NULL;
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( pNetMsg->bVietnamInven )
		{
			pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		}else{
			pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		}
		if ( pInvenItem )
		{
			pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
		}
	}
	else
	{
//#else
		pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( pInvenItem )
		{
			pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
		}
	}
//#endif

	// �κ��丮 ������Ʈ
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
		"-n, -n", pNetMsg->wPosX, pNetMsg->wPosY );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLCharacter::MsgPutonDrugUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PUTON_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_PUTON_DRUG_UPDATE *)pMsg;

    EMSLOT emSlot = pNetMsg->emSlot;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( m_PutOnItems[emSlot].GetNativeID() );

    if ( m_PutOnItems[emSlot].wTurnNum > pNetMsg->wTurnNum )
    {
        if ( pItem )
        {
            switch ( pItem->sBasicOp.emItemType )
            {
            case ITEM_INC_GRINDINGRATE:	
                m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("ITEM_USE_INC_GRINDINGRATE") );
                break;
            case ITEM_ANTI_RESET:	
                m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("ITEM_USE_ANTIRESET") );	
                break;
            }
        }
    }

    m_PutOnItems[emSlot].wTurnNum = pNetMsg->wTurnNum;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Character_UpdatePutOnCount,
		"-n, -n, -n",
		(int) emSlot,
		pNetMsg->wTurnNum,
		NULL != pItem ? (int) pItem->BasicType() : -1 );
}

void GLCharacter::MsgGetChaExtraInfo(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *pNetMsg = (GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *)nmg;

	m_nContributionPoint = pNetMsg->nContributionPoint;
}

void GLCharacter::MsgDominateMoveToFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB* pNetMsg = (GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB*)pMsg;
	switch ( pNetMsg->emFB )
	{	
	case EMDOMINATE_MOVETO_FB_FAIL:
	case EMDOMINATE_MOVETO_FB_NA_TARGET:
	case EMDOMINATE_MOVETO_FB_OUT_OF_DISTANCE:
	case EMDOMINATE_MOVETO_FB_NOT_DOMINATED:
		m_sDOMINATE.bDominate = FALSE;
		break;
	case EMDOMINATE_MOVETO_FB_CANCEL:
		for ( DWORD _i = 0; _i < SKILLFACT_SIZE; ++_i )
		{
			SSKILLFACT& sSKEFF = GetSkillFact(_i);
			if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )
				continue;
			if ( !sSKEFF.bEnable )
				continue;
			if ( sSKEFF.IsHaveSPECADDON(EMSPECA_DOMINATION) )
			{				
				FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), m_pSkinChar, sSKEFF.sNATIVEID);

				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, sSKEFF.sNATIVEID.dwID, _i );
				sSKEFF.RESET();
			}				
		}
		m_sDOMINATE.emController = EMSPEC_DOMINATE_TYPE_SELF;
		break;
	}
}

void GLCharacter::MsgDominateMoveToFc(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_DOMINATE_MOVETO_FC* pNetMsg = (GLMSG::SNETPC_DOMINATE_MOVETO_FC*)pMsg;
	m_ReservationAction.Clear();
	TurnAction(GLAT_IDLE);
	ResetSelectID();
	ResetCursorID();
	ActionMoveTo(pNetMsg->fTime, pNetMsg->vFromPt, pNetMsg->vTargetPt);
}

void GLCharacter::MsgGoto(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_GOTO* pNetMsg = (GLMSG::SNETPC_GOTO*)pMsg;
	pNetMsg->vCurPos;
	pNetMsg->vTarPos;

	if ( ClientActor::GotoLocation(pNetMsg->vTarPos, 10.0f) == FALSE )
		return;

	m_bMiniMapMove = false;
	m_fMiniMapMoveTime = 0.0f;

	TurnAction ( GLAT_MOVE );
	m_sActionID.vPos = pNetMsg->vTarPos;
}

void GLCharacter::MsgSelectTarget(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SELECT_TARGET*	pNetMsg = (GLMSG::SNETPC_SELECT_TARGET*)pMsg;

    ResetReservationAction();
    SetSelectID( STARGETID(  pNetMsg->emTarCrow, pNetMsg->dwTarID ) );
} //GLCharacter::MsgSelectTarget


void GLCharacter::MsgSkillDelay( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_SKILL_DELAY*	pNetMsg = (GLMSG::SNETPC_SKILL_DELAY*)pMsg;

	GLCHARLOGIC::SET_SKILLDELAY_INC ( pNetMsg->fSkillDelay, 100, EMSPEC_SKILL_ASING_AFTER, pNetMsg->sUseSkillID );

}

void GLCharacter::MsgExchangeItemNpc(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC* pPacket = (GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC*) pMsg;

	std::string strText;

	switch( pPacket->ReValue )
	{
	case EMREQ_NEI_EX_FAILED	: m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_EXCHANGE_FAILED") ); break;
	case EMREQ_NEI_EX_SUCCESS	: 
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem(pPacket->sNID);
			
			if( pItem ) {				
				strText = sc::string::format(" \"%1%\" %2%", pItem->GetNameStr(), ID2GAMEINTEXT("BUYITEM_EXCHANGE_SUCCESS") );
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strText.c_str() );
			}

			//!! SF_TODO
			/*CInnerInterface* pInter = dynamic_cast<CInnerInterface*>(m_pInterface);

			if( pInter ) pInter->UpdateExChangeList();*/
		}
		break;
	case EMREQ_NEI_EX_NITEM_FB	: m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_ITEM_EXCHANGE") ); break;
	case EMREQ_NEI_EX_NRPT_FB	: m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_RANPT_EXCHANGE") );break;
	case EMREQ_NEI_EX_NLVPT_FB	: m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_LVPT_EXCHANGE") ); break;
	case EMREQ_NEI_EX_NSPACE_FB	: m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOSPACE") );       break;
	}
}

void GLCharacter::MsgForceresetSkillStatFC ( NET_MSG_GENERIC* _pMsg )
{
	GLMSG::SNETPC_FORCERESET_STAT_SKILL* pNetMsg =
		static_cast< GLMSG::SNETPC_FORCERESET_STAT_SKILL* >( _pMsg );
	if ( NULL == pNetMsg )
		return;

	RESET_STATS_SKILL( NULL );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_ApplyStatPoint );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateSlot );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );

	// ��ųâ�� ������Ʈ �ؾ��Ѵ�;
	if ( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
	}

	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::NEGATIVE,
		ID2GAMEINTEXT( "TEXT_FORCERESET_SKILL_STAT" ) );
}