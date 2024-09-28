#include "../pch.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../Util/GLItemLimit.h"
#include "./GLChar.h"

#include "../../SigmaCore/DebugInclude.h"

bool GLChar::ConsumeStorageItem( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( dwChannel >= MAX_CLUBSTORAGE )
		return false;

	//	�κ��� ������ Ȯ��.
	SINVENITEM* pINVENITEM = m_cStorage[dwChannel].FindPosItem( wPosX, wPosY );
	if ( !pINVENITEM )
		return false;

	//	������ ���� ������.
	SNATIVEID idItem = pINVENITEM->sItemCustom.GetNativeID();

	const SITEM* pItem = GLogicData::GetInstance().GetItem( idItem );
	if ( !pItem )
		return false;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ConsumeStorageItem ", m_CharDbNum ) );
	}
	//
	//mjeon.activity
	//mjeon.attendance
	//
	ActivityCheckItemUse( idItem.dwID );
	DoQuestItemUse( idItem );

	if ( pItem->sDrugOp.bInstance )
	{
		if ( pINVENITEM->sItemCustom.wTurnNum > 0 )
			pINVENITEM->sItemCustom.wTurnNum--;
	
		// �������� ���� ���� ��� ���		
		// ��� �� �α� ����
		LogItemExchange( 
            pINVENITEM->sItemCustom, 
            gamelog::ID_CHAR, m_CharDbNum, 
            gamelog::ID_CHAR, 0, 
            ITEM_ROUTE_DELETE, 1, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_INVEN_ITEM,
			false );

		if ( pINVENITEM->sItemCustom.wTurnNum == 0 )
		{
			UserStorageDeleteItem( dwChannel, wPosX, wPosY, true, 1, true );
		}
		else
		{
			//	[�ڽſ���] �ι� ������ �Ҹ��.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;
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


HRESULT GLChar::MsgReqGetStorageSpecificItem( NET_MSG_GENERIC* nmg )
{	 
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM* pNetMsg = ( GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM* ) nmg;
	
	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	std::vector< DWORD > specificItemVec;

	for ( int i = 0; i < EMSTORAGE_CHANNEL; ++i )
	{
		// Ŭ���̾�Ʈ�� â�� ����
		// ������ ���鼭 �ϳ��� ����
		SINVENITEM* pInvenItem = NULL;
		const GLInventory::CELL_MAP* pInvenList = m_cStorage[i].GetItemList();
		GLInventory::CELL_MAP_CITER iter = pInvenList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pInvenList->end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SINVENITEM& invenItem = *(*iter).second;
			const SITEM* pItem = GLogicData::GetInstance().GetItem( invenItem.Id() );
			if ( pItem )
			{
				if ( pItem->BasicType() == pNetMsg->dwItemType )
					specificItemVec.push_back( invenItem.Id() );
			}
		}
	}

	//	Note : â���� �⺻ ���� ����.
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB NetMsgItemCount;
	NetMsgItemCount.dwItemCount = static_cast< DWORD >( specificItemVec.size() );
	SendToClient( &NetMsgItemCount );

	//	Note : �������� �ϳ��� ����.
	for ( size_t i = 0; i < specificItemVec.size(); ++i )
	{
		GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB NetMsgItem;
		NetMsgItem.Data = specificItemVec[i];
		SendToClient( &NetMsgItem );
	}

	return S_OK;
}

// *****************************************************
// Desc: â������ ���� (ä�κ���)
// *****************************************************
HRESULT GLChar::MsgReqGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_GETSTORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_GETSTORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_GETSTORAGE, EMHOLD_FB_OFF );

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

	// ��û ä�� ��ȿ�� üũ
	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	Note : â���� �⺻ ���� ����.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_FB NetMsg_GetFb;
	NetMsg_GetFb.lnMoney = GetStorageMoney();
	NetMsg_GetFb.dwChannel = dwChannel;
	NetMsg_GetFb.dwNumStorageItem = m_cStorage[dwChannel].GetNumItems();
	SendToClient( &NetMsg_GetFb );

	//	Note : �������� �ϳ��� ����.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_ITEM NetMsgItem;
	NetMsgItem.dwChannel = dwChannel;

	// Ŭ���̾�Ʈ�� â�� ����
	// ������ ���鼭 �ϳ��� ����
	{
		SINVENITEM* pInvenItem = NULL;
		const GLInventory::CELL_MAP* pInvenList = m_cStorage[dwChannel].GetItemList();
		GLInventory::CELL_MAP_CITER iter = pInvenList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pInvenList->end();
		for ( ; iter!=iter_end; ++iter )
		{
			pInvenItem = ( *iter ).second;
			NetMsgItem.Data = *pInvenItem;

            msgpack::sbuffer SendBuffer;
            msgpack::pack( SendBuffer, NetMsgItem );
			SendToClient( NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM, SendBuffer );
		}
	}

	return S_OK;
}

// *****************************************************
// Desc: â������� ��� ( �๰ )
// *****************************************************
HRESULT GLChar::MsgReqStorageDrug( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	
        return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_STORAGEDRUG* pNetMsg = ( GLMSG::SNETPC_REQ_STORAGEDRUG* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_GETSTORAGE, EMHOLD_FB_OFF );

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

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE( pNetMsg->dwChannel ) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	//	Note : pk ����� ���θ� ��� �̻��� ��� ȸ������ ����� ���´�.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL > GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
	{
		return E_FAIL;
	}

	if ( m_sCONFTING.IsFIGHTING() )
	{
		if ( !m_sCONFTING.IsRECOVE() )
		{
			//	Note : ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER!=USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			//	Note : ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );
		}
	}

	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	const DWORD dwChannel = pNetMsg->dwChannel;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sDrugOp.emDrug == ITEM_DRUG_NUNE )
		return E_FAIL;

	bool bCheck = true;

	if( pItem->sDrugOp.emDrug == ITEM_DRUG_EFFECT_RELEASE )
		bCheck = false;

	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM ) && bCheck )
		return E_FAIL;

	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
    if ( IsActionLimit( EMACTION_LIMIT_DRUG ) && bCheck )
		return E_FAIL;

    // ü�� ȸ�� �������� ����Ҽ� ���� ��Ȳ�̴�.
    switch(pItem->sDrugOp.emDrug)
    {
    case ITEM_DRUG_HP:
    case ITEM_DRUG_HP_MP:
    case ITEM_DRUG_HP_MP_SP:
    case ITEM_DRUG_HP_CURE:
    case ITEM_DRUG_HP_MP_SP_CURE:
        {
            if ( IsActionLimit( EMACTION_LIMIT_DRUG_HP ) )
            {
                return E_FAIL;
            }
        }
        break;
    };

	switch ( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_EFFECT_RELEASE :
		{
			if ( !ReleaseCureEffect( pItem->sDrugOp.dwERList ) )
				return E_FAIL;

			GLMSG::SNET_DRUG_FB netMsg;
			netMsg.emFB    = EMREQ_DRUG_CONSUME_SUCCESS;
			netMsg.sItemID = pInvenItem->sItemCustom.GetNativeID();

			SendToClient( &netMsg );
			SendMsgViewAround( &netMsg );
		}
		break;

	case ITEM_DRUG_HP:
		m_sHP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sHP.INCREASE( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_MP:
		m_sMP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sMP.INCREASE( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_SP:
		m_sSP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sSP.INCREASE( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_CP:
		m_sCP.INCREASE( int( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nCP_ADD + m_sSUM_PASSIVE.m_nCP_ADD );
		break;

	case ITEM_DRUG_HP_MP:
		m_sHP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sMP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sHP.INCREASE( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
		m_sMP.INCREASE( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_MP_SP:
		m_sMP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sSP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sMP.INCREASE( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		m_sSP.INCREASE( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_HP_MP_SP:
		m_sHP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nHP_ADD );
		m_sMP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nMP_ADD );
		m_sSP.INCREASE( static_cast< int >( pItem->sDrugOp.dwCureVolume ), pItem->sDrugOp.bRatio, m_nSP_ADD );
		m_sHP.INCREASE( ( m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL ), false, 0 );
		m_sMP.INCREASE( ( m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL ), false, 0 );
		m_sSP.INCREASE( ( m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL ), false, 0 );
		break;

	case ITEM_DRUG_CURE:
	case ITEM_DRUG_HP_CURE:
	case ITEM_DRUG_HP_MP_SP_CURE:
		break; 
	};


	//	â�� ������ �Ҹ��.
	ConsumeStorageItem( dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : ü�� ��ġ ��ȭ�� [�ڽ�,��Ƽ��,����]�� Ŭ���̾�Ʈ�鿡 �˸�.
	MsgSendUpdateState();

	return S_OK;
}

// *****************************************************
// Desc: â��ų ��� ( �๰ )
// *****************************************************
HRESULT GLChar::MsgReqStorageSkill( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )
		return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_GETSTORAGE, EMHOLD_FB_OFF );

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

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE( pNetMsg->dwChannel ) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	const DWORD dwChannel = pNetMsg->dwChannel;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_SKILL )
		return E_FAIL;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		//	�̹� ������ ��ų.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = EMSKILL_LEARN_ALREADY;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL( sSKILL_ID );
	if ( emSKILL_LEARNCHECK != EMSKILL_LEARN_OK )
	{
		//	��ų ���� �䱸 ������ �������� ���մϴ�.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = emSKILL_LEARNCHECK;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	//	Note : ��ų ������.
	//
	LEARN_SKILL( sSKILL_ID );

	//	Note : ��ų �������� ���� ����Ʈ ���� ����.
	//
	QuestStartFromGetSKILL( sSKILL_ID );

	//	Note : �Ҹ� ������ ����.
	//
	ConsumeStorageItem( pNetMsg->dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	��ų ��� ����.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	SendToClient( &NetMsgFB );

	//	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	return S_OK;
}

// *****************************************************
// Desc: ������ ��� �õ�
// *****************************************************
HRESULT GLChar::MsgReqStorageToHold( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_TO_HOLD* pNetMsg = ( GLMSG::SNETPC_REQ_STORAGE_TO_HOLD* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD, EMHOLD_FB_OFF );
	
	return S_OK;
}

// *****************************************************
// Desc: â�� ������ ��� ���� ���� (Ȯ����)
// *****************************************************
// �κ��丮(HoldItem) <-> ������Ŀ
// �κ��丮�� �������� �� ������Ŀ �����۰� ��ȯ
HRESULT GLChar::MsgReqInvenExStorage( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_EX_STORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_EX_STORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_EX_STORAGE, EMHOLD_FB_OFF );

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenExStorage", m_CharDbNum ) );
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

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		if ( pHoldInvenItem->CustomItem().bVietnamGainItem  )
			return S_FALSE;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pITEM )
		return false;
	
	//	�ŷ��ɼ�
	// â�� �����Ҷ��� �ŷ��ɼ��� �ƴ϶� ���� �߰��� â�� �ɼ��� �˻�
	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
		return FALSE;
	
	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	// �����̾� ���� ��ȿ�Ⱓ�� �����ٸ� �׳� �����Ѵ�. �׷��� ������ �ٴڿ� �������� ��������.
	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wHoldPosY )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	�ι꿡 �ִ� ������ ���.	
	WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
	WORD wPosY = sInven_BackUp.wPosY;

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( sHold_BackUp.sItemCustom.GetNativeID() );
	if ( !pInvenData || !pHoldData )
		return S_FALSE;

	//	Note : ���� �Ҽ� �ִ� ���������� ����.
	bool bMERGE( false );
	bMERGE = ( pInvenData->sBasicOp.sNativeID == pHoldData->sBasicOp.sNativeID );
	if ( bMERGE)
		bMERGE = ( pInvenData->ISPILE() );
	if ( bMERGE)
		bMERGE = ( sHold_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
	if ( bMERGE)
		bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sHold_BackUp.sItemCustom.wTurnNum;
		
		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
            // �������� ���� ���� ��� ���
			// ��� �� �α� ����
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_CHAR, m_CharDbNum, 
                gamelog::ID_USER, m_dwUserID, 
                ITEM_ROUTE_USER_INVEN, sHold_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_EXCHANGE_INVEN_WITH_STORAGE,
				false );

			//	Note : ��ħ �� ����.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	Note : ��� �ִ� ������ ����.
            InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, sHold_BackUp.sItemCustom.bVietnamGainItem, 1, true );

			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageUpdate;
			NetMsgStorageUpdate.dwChannel = dwChannel;
			NetMsgStorageUpdate.wPosX = pNetMsg->wPosX;
			NetMsgStorageUpdate.wPosY = pNetMsg->wPosY;
			NetMsgStorageUpdate.wTurnNum = wTurnNum;
			SendToClient( &NetMsgStorageUpdate );
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
            // �������� ���� ���� ��� ���
			// ��� �� �α� ����
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_CHAR, m_CharDbNum, 
                gamelog::ID_USER, m_dwUserID, 
                ITEM_ROUTE_USER_INVEN, pInvenData->sDrugOp.wPileNum - sInven_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_EXCHANGE_INVEN_WITH_STORAGE,
				false );

			//	���� ������ ���� ����. ( ��ä��. )
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	�տ� ��� �ִ� ������ ����. ( ���� ����. )
			sHold_BackUp.sItemCustom.wTurnNum = wSplit;
            pHoldInvenItem->sItemCustom = sHold_BackUp.sItemCustom;

            // �κ��� ������ ������Ʈ.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
            NetMsgInvenItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgInvenItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgInvenItem.sItemCustom = pHoldInvenItem->CustomItem();
            SendToClient( &NetMsgInvenItem );
			
			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
			NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
			NetMsgStorageItem.wPosX = pNetMsg->wPosX;
			NetMsgStorageItem.wPosY = pNetMsg->wPosY;
			NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
			SendToClient( &NetMsgStorageItem );
		}

		return S_OK;
	}

    SINVENITEM* pINSERT_STORAGE_ITEM = NULL;
    SINVENITEM* pINSERT_IVEN_ITEM = NULL;

	// #item
	// user locker(Inven) <-> inven(Hold)
	// user locker ������ wPosX, wPosY, sInven_BackUp
	// inven ������ wHoldPosX, wHoldPosY, sHold_BackUp

	// ������ ���� ó��
	{
		if ( !m_cStorage[dwChannel].DeleteItem( wPosX, wPosY ) )
		{
			// �׳� �ƹ��͵� ���߱� ������ ���� ó�� �� �͵� ����.
			return S_FALSE;
		}

		if ( !m_cInventory.DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY ��ġ�� �ִ� ������ ó��
			if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );

				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
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
		if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// ������ ������ ������ �˸�
			UserStorageDeleteSend( dwChannel, wPosX, wPosY );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sInven_BackUp.sItemCustom, m_vPosition );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage insert hold item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			DropItem( sHold_BackUp.sItemCustom, m_vPosition );

			return S_FALSE;
		}

		if ( !m_cInventory.InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
		{
			// ������ ������ ������ �˸�
			UserStorageDeleteSend( dwChannel, wPosX, wPosY );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold �������� �̹� ���������� ���ԵǾ� �ִ� ��Ȳ(wPosX wPosY ��ġ��)
			// Hold ��ġ�� ������ ������ ������ ��Ȳ. �̹� ���������� ���ԵǾ� �ִ� ������(Hold ������) �˸�
			SINVENITEM* pTempInsertedItem = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
			if ( !pTempInsertedItem )
			{
				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage insert inven item fail, and getitem fail, disconnect user. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				NET_DISCONNECT_CHARACTER NetMsg;
				NetMsg.ChaDbNum = m_CharDbNum;
				m_pGaeaServer->SENDTOAGENT( &NetMsg );
			}
			else
			{
				UserStorageInsertSend( dwChannel, pTempInsertedItem );
			}

			DropItem( sInven_BackUp.sItemCustom, m_vPosition, pTempInsertedItem == NULL ? false : true );

			return S_FALSE;
		}

		// ������ Ȯ��
		pINSERT_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
		pINSERT_IVEN_ITEM = m_cInventory.GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_STORAGE_ITEM || !pINSERT_IVEN_ITEM )
		{
			// ������ ���� �α׸� Ȯ���ص� ���Ե� �������� NULL �� �α״� ������.
			// �̷� ��찡 �߻��� ��� ���� ���� ��Ų��. ĳ���Ͱ� ������ �ٷ� ���������� �����߱� ������ ����� ���̴�.

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage result item null, disconnect user. chanum: %1% guid: %2% guid: %3%", 
				m_CharDbNum, 
				sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str(), 
				sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			NET_DISCONNECT_CHARACTER NetMsg;
			NetMsg.ChaDbNum = m_CharDbNum;
			m_pGaeaServer->SENDTOAGENT( &NetMsg );

			return S_FALSE;
		}
	}

	// ��� ó��
	UserStorageUpdateItem( dwChannel, pINSERT_STORAGE_ITEM, true );
	InvenUpdateItem( pINSERT_IVEN_ITEM, true );

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        sHold_BackUp.sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_USER, m_dwUserID, 
        ITEM_ROUTE_USER_INVEN, sHold_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_INVEN_WITH_STORAGE,
		false );

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        sInven_BackUp.sItemCustom, 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_INVEN_WITH_STORAGE,
		false );

	GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.dwChannel = dwChannel;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	������ ������.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pINSERT_STORAGE_ITEM;				//	���ԵǴ� ������.
	SendToClient( &NetMsg_Del_Insert );

    GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Inven_Del_Insert;
    NetMsg_Inven_Del_Insert.wDelX = sHold_BackUp.wPosX;				//	������ ������.
    NetMsg_Inven_Del_Insert.wDelY = sHold_BackUp.wPosY;
    NetMsg_Inven_Del_Insert.sInsert = *pINSERT_IVEN_ITEM;			//	���ԵǴ� ������.

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsg_Inven_Del_Insert );
    SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

    //	��ȿ�Ⱓ �ʰ� ����.	
    HRESULT hrCheckDuration = CheckItemDuration( *pINSERT_IVEN_ITEM );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;
	
	return S_OK;
}

// �κ��� Ȧ�� ������ â�� ����
// ��񿡴� ���� ��ġ�� ����ȴ�. �κ��丮->â��
HRESULT GLChar::MsgReqInvenToStorage( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_STORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_TO_STORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB,EMHOLD_FB_OFF );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenToStorage", m_CharDbNum ) );
	}

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
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

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	if ( m_ServiceProvider == SP_VIETNAM)
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

	//	�ŷ��ɼ�
	// â�� �����Ҷ��� �ŷ��ɼ��� �ƴ϶� ���� �߰��� â�� �ɼ��� �˻�
	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
		return FALSE;

	// ��ī���ϰ�� ���� Ȱ�����̸�
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pMyPet->DbNum() == pHoldInvenItem->CustomItem().PetDbNum() )
            return FALSE;
	}

	BOOL bOk = m_cStorage[dwChannel].IsInsertable( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.

	if ( !m_cInventory.DeleteItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), pNetMsg->wPosX, pNetMsg->wPosY ) )
	{
		if ( !m_cInventory.InsertItem( sHold_BackUp.CustomItem(), pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
		{
			DropItem( sHold_BackUp.sItemCustom, m_vPosition );
			SendToClient( &NetMsg_fb );
			return E_FAIL;
		}
	}

	SINVENITEM* pINSERT_HOLD_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINSERT_HOLD_STORAGE_ITEM )
	{
		NET_DISCONNECT_CHARACTER NetMsg;
		NetMsg.ChaDbNum = m_CharDbNum;
		m_pGaeaServer->SENDTOAGENT( &NetMsg );

		sc::writeLogError( 
			sc::string::format( 
			"critical error MsgReqInvenToStorage, pINSERT_HOLD_STORAGE_ITEM null, chanum: %1% guid: %2%", 
			m_CharDbNum, sc::string::uuidToString( sHold_BackUp.GetGuid() ).c_str() ) );
		
		return S_FALSE;
	}

	// ��� ������Ʈ
	UserStorageUpdateItem( dwChannel, pINSERT_HOLD_STORAGE_ITEM, true );

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        sHold_BackUp.CustomItem(), 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_USER, m_dwUserID, 
        ITEM_ROUTE_USER_INVEN, sHold_BackUp.CustomItem().wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_MOVE_INVEN_TO_STORAGE,
		false );

	InvenDeleteSend( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY, FALSE );

	GLMSG::SNETPC_STORAGE_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.dwChannel = dwChannel;
	NetMsg_Inven_Insert.Data = *pINSERT_HOLD_STORAGE_ITEM;
	SendToClient( &NetMsg_Inven_Insert );

	return S_OK;
}

//! Char Inven -> Locker, �� �ֱ�
HRESULT GLChar::MsgReqStorageSaveMoney( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageSaveMoney", m_CharDbNum ) );
	}

	GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY* pPacket = ( GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY* ) nmg;

	unsigned int Crc32 = sc::string::getCrc32( pPacket, sizeof( GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY )-sizeof( unsigned int ) );
	if ( pPacket->Crc32 != Crc32 )
	{
		sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageSaveMoney Crc32. CharDbNum %1%", m_CharDbNum ) );
		return E_FAIL;
	}

    if ( pPacket->m_MagicNum != GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY::MAGIC_NUM )
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageSaveMoney magic num. CharDbNum %1% MagicNum %2%", m_CharDbNum, pPacket->m_MagicNum ) );
        return E_FAIL;
    }

    // �Ա��� �ݾ�
    LONGLONG ReceiptMoney = pPacket->lnMoney;
	if ( ReceiptMoney < 0 || GetInvenMoney() < ReceiptMoney )
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageSaveMoney %1% < %2% CharDbNum %3%", GetInvenMoney(), ReceiptMoney, m_CharDbNum ) );
        return E_FAIL;
    }

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
		GLCrow* pCrow = pLand->GetCrow( pPacket->dwNpcGlobID );
		if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
            return E_FAIL;
	}

	CheckMoneyUpdate( GetInvenMoney(), ReceiptMoney, FALSE, "Storage Save Money." );

	SubtractInvenMoneyServer( ReceiptMoney, SUBTRACT_INVEN_MONEY_USER_STORAGE_SAVE );
	AddStorageMoney( ReceiptMoney );

	// �ݾ� �α�
    // �ŷ��� ĳ���� �κ��丮 ��ü �ݾ�
    // �ŷ��� ��ȭ��
    // �ŷ��� ĳ���� ��Ŀ �ݾ�
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, ReceiptMoney,     ITEM_ROUTE_USER_INVEN, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MOVE_INVEN_TO_STORAGE );
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER,          0, GetInvenMoney(),        MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, GetStorageMoney(), MONEY_ROUTE_USER_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_STORAGE_CURRENT );

	// Inven Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���. 
	SendInvenMoneyUpdate();

	// Locker Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = GetStorageMoney();
	SendToClient( &NetMsgStorage );

	return S_OK;
}

// *****************************************************
// Desc: �� ã��
// *****************************************************
HRESULT GLChar::MsgReqStorageDrawMoney( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar ) 
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqStorageDrawMoney", m_CharDbNum ) );
	}

	GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY* pPacket = ( GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY* ) nmg;

	unsigned int Crc32 = sc::string::getCrc32( pPacket, sizeof( GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY )-sizeof( unsigned int ) );
	if ( pPacket->Crc32 != Crc32 )
	{
		sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageDrawMoney Crc32. CharDbNum %1%", m_CharDbNum ) );
		return E_FAIL;
	}

    if ( pPacket->m_MagicNum != GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY::MAGIC_NUM )
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageDrawMoney magic num. ChaDbNum %1% MagicNum %2%", m_CharDbNum, pPacket->m_MagicNum ) );
        return E_FAIL;
    }

	LONGLONG lnMoney = pPacket->lnMoney;
	if (lnMoney < 0 || GetStorageMoney() < lnMoney)
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageDrawMoney %1% < %2% ChaDbNum %3%", GetStorageMoney(), lnMoney, m_CharDbNum ) );
		return E_FAIL;
    }

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
		GLCrow* pCrow = pLand->GetCrow( pPacket->dwNpcGlobID );
		if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
        {
            return E_FAIL;
        }
	}

	CheckMoneyUpdate( GetInvenMoney(), lnMoney, TRUE, "Storage Draw Money." );

	SubtractStorageMoney( lnMoney );
	AddInvenMoneyServer( lnMoney, ADD_INVEN_MONEY_USER_STORAGE_DRAW );

	// �ݾ� �α�
    // �ŷ��� ĳ���� Inven ��ü�Ӵ�
    // Locker Money ��ȭ��
    // �ŷ��� ĳ���� Locker ��ü�Ӵ�
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID,         -lnMoney, ITEM_ROUTE_USER_INVEN, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MOVE_STORAGE_TO_INVEN );
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, GetStorageMoney(), MONEY_ROUTE_USER_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_STORAGE_CURRENT );
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER,          0,        GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
    
	// Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	SendInvenMoneyUpdate();

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = GetStorageMoney();
	SendToClient( &NetMsgStorage );

	return S_OK;
}

// *****************************************************
// Desc: �Ҹ� ������ ���� �и�
// *****************************************************
HRESULT GLChar::MsgReqStorageSplit( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_STORAGE_SPLIT *pNetMsg = ( GLMSG::SNETPC_REQ_STORAGE_SPLIT* )nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB, EMHOLD_FB_OFF );
	
	BOOL bOk( FALSE );
	if ( EMSTORAGE_CHANNEL <= pNetMsg->dwChannel )
		return E_FAIL;

	if ( !IsKEEP_STORAGE( pNetMsg->dwChannel ) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
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

	GLInventory& sInventory = m_cStorage[pNetMsg->dwChannel];

	//	Note : �и� ��û ������ ã��.
	SINVENITEM* pInvenItem = sInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	//	Note : ������ ���� ��������.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	//	Note : �и� ������ ���������� ����.
	bool bSPLIT( false );
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )
		return E_FAIL;
	if ( pNetMsg->wSplit < 1 )
		return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )
		return E_FAIL;

	//	Note : ���� �������� ��ȭ�� ����.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : �и��Ǿ� ���� ����� ������.
	SITEMCUSTOM sNEW_ITEM( pInvenItem->sItemCustom );
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;
	
	// guid ����, �������� �и��� �� ���ο� guid �� �����Ѵ�.
	sc::SeqUniqueGUID::Instance()->getGUID( sNEW_ITEM.guid );
	sNEW_ITEM.GenerateBasicStat( false );
	sNEW_ITEM.GenerateLinkSkill();
	sNEW_ITEM.GenerateAddOption();

	//	Note : ���� ����� ������ ��ġ ã��.
	WORD wInvenPosX( 0 ), wInvenPosY( 0 );
	bOk = sInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )
		return E_FAIL;

	//	Note : ���� ���� ������ �ֱ�.
	if ( !UserStorageInsertItem( pNetMsg->dwChannel, sNEW_ITEM, wInvenPosX, wInvenPosY, false, 1, true ) )
	{
		return E_FAIL;
	}

	//	Note : ���� ���� ������ ���� ��������.
	SINVENITEM* pNewItem = sInventory.GetItem( wInvenPosX, wInvenPosY );
	if ( !pNewItem )
		return E_FAIL;

	//	Note : ���� �������� ���� ���� ��Ŵ.
	// ��� ������ �����ϴ� �Լ� ����� ȣ���ؾ� �Ѵ�.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : �κ��� ���� �и��Ǿ� ��� ���� ������.
	GLMSG::SNETPC_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	SendToClient( &NetMsgNew );

	//	Note : ���� ������ ���� ����.
	GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	SendToClient( &NetMsgStorage );

	return S_OK;
}

// #item
//  [1/23/2013 gbgim]
// - Ȧ�� ������ â�� ����, ������ MsgReqStorageToStorage, MsgReqStorageExStorage�� ������
HRESULT GLChar::MsgReqStorageToStorage( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_STORAGE_TO_STORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_STORAGE_TO_STORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB, EMHOLD_FB_OFF );

	if ( ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL ) || ( pNetMsg->dwHoldChannel >= EMSTORAGE_CHANNEL ) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	if ( pNetMsg->dwChannel == pNetMsg->dwHoldChannel && pNetMsg->wPosX == pNetMsg->wHoldPosX && pNetMsg->wPosY == pNetMsg->wHoldPosY )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	// â������ ��ȿ�� üũ
	if ( !m_bUsedStorageCard )
	{
		GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
		if ( !pCrow )
			return E_FAIL;

		if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
			return E_FAIL;
	}

	if ( !IsKEEP_STORAGE( pNetMsg->dwChannel ) || !IsKEEP_STORAGE( pNetMsg->dwHoldChannel ) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	//!	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	// �������� ��ġ�� �������� �����ϴ��� ����
	bool isInvenItem = true;
	if ( m_cStorage[pNetMsg->dwChannel].FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY ) == NULL )
		isInvenItem = false;

	const DWORD dwChannel = pNetMsg->dwChannel;
	const DWORD dwHoldChannel = pNetMsg->dwHoldChannel;

	SINVENITEM* pHoldInvenItem = m_cStorage[dwHoldChannel].GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
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

	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pHoldData )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	// �ŷ��ɼ�, �ŷ� �Ұ� �������̶��
	// â�� �����Ҷ��� �ŷ��ɼ��� �ƴ϶� ���� �߰��� â�� �ɼ��� �˻�
	// â���� â��� �̵��ε� �˻��� �ʿ䰡 �ֳ�? ���� �� ���Ƽ� �ּ� ó���� [8/31/2016 khkim]
// 	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
// 		return E_FAIL;

	// ��ī���ϰ�� ���� Ȱ�����̸�
	if ( isInvenItem == false && pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pMyPet->DbNum() == pHoldInvenItem->CustomItem().PetDbNum() )
			return S_FALSE;
	}

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
	WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
	WORD wHoldPosY = sHold_BackUp.wPosY;

	if ( !pHoldData )
		return S_FALSE;

	SINVENITEM* pINSERT_HOLD_STORAGE_ITEM = NULL;
	SINVENITEM* pINSERT_STORAGE_ITEM = NULL;

	// To �� ���
	// â���� â�� Ư�� ��ǥ�� ������ �̵�
	if ( isInvenItem == false )
	{
		BOOL bOk = m_cStorage[dwChannel].IsInsertable( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !bOk )
		{
			SendToClient( &NetMsg_fb );
			return E_FAIL;
		}

		if ( !m_cStorage[dwHoldChannel].DeleteItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
		{
			SendToClient( &NetMsg_fb );
			return E_FAIL;
		}

		if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), pNetMsg->wPosX, pNetMsg->wPosY ) )
		{
			if ( !m_cStorage[dwHoldChannel].InsertItem( sHold_BackUp.CustomItem(), pNetMsg->wHoldPosX, pNetMsg->wHoldPosY ) )
			{
				DropItem( sHold_BackUp.sItemCustom, m_vPosition );
			}

			SendToClient( &NetMsg_fb );
			return E_FAIL;
		}

		SINVENITEM* pINSERT_HOLD_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !pINSERT_HOLD_STORAGE_ITEM )
		{
			NET_DISCONNECT_CHARACTER NetMsg;
			NetMsg.ChaDbNum = m_CharDbNum;
			m_pGaeaServer->SENDTOAGENT( &NetMsg );

			sc::writeLogError( 
				sc::string::format( 
				"critical error MsgReqHoldToStorage, pINSERT_HOLD_STORAGE_ITEM null chanum: %1% guid: %1%", 
				m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			return S_FALSE;
		}

		UserStorageDeleteSend( dwHoldChannel, pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
		UserStorageInsertSend( dwChannel, pINSERT_HOLD_STORAGE_ITEM );

		// ��� ������Ʈ
		UserStorageUpdateItem( dwChannel, pINSERT_HOLD_STORAGE_ITEM, true );
	}
	else // pNetMsg->nType == NET_MSG_GCTRL_REQ_STORAGE_EX_STORAGE, ���� ��ġ ���� ex
	{
		SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( pInvenItem == NULL )
		{
			SendToClient( &NetMsg_fb );
			return S_FALSE;
		}

		SINVENITEM sInven_BackUp = *pInvenItem;		//	�ι꿡 �ִ� ������ ���.	
		WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
		WORD wPosY = sInven_BackUp.wPosY;

		const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
		if ( pInvenData == NULL )
			return S_FALSE;

		// ���� �Ҽ� �ִ� ���������� ����.
		bool bMERGE( false );
		bMERGE = ( pInvenData->sBasicOp.sNativeID == pHoldData->sBasicOp.sNativeID );
		if ( bMERGE)
			bMERGE = ( pInvenData->ISPILE() );
		if ( bMERGE)
			bMERGE = ( sHold_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
		if ( bMERGE)
			bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum < pInvenData->sDrugOp.wPileNum );
		if ( bMERGE )
		{
			WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sHold_BackUp.sItemCustom.wTurnNum;

			//! �� �������� ������ �� ������ �ִ� ������ �Ѿ�ٸ�
			if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
			{
				//	Note : ��ħ �� ����.
				pInvenItem->sItemCustom.wTurnNum = wTurnNum;

				//	Note : ��� �ִ� ������ ����.
				UserStorageDeleteItem( dwHoldChannel, wHoldPosX, wHoldPosY, true, 1, true );

				//	Note : â�� ������ ������Ʈ.
				GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageUpdate;
				NetMsgStorageUpdate.dwChannel = dwChannel;
				NetMsgStorageUpdate.wPosX = wPosX;
				NetMsgStorageUpdate.wPosY = wPosY;
				NetMsgStorageUpdate.wTurnNum = wTurnNum;
				SendToClient( &NetMsgStorageUpdate );
			}
			else
			{
				WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

				//	���� ������ ���� ����. ( ��ä��. )
				pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

				//	�տ� ��� �ִ� ������ ����. ( ���� ����. )
				pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

				// â�� ������ ������Ʈ.
				GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
				NetMsgStorageItem.dwChannel = dwChannel;
				NetMsgStorageItem.wPosX = wPosX;
				NetMsgStorageItem.wPosY = wPosY;
				NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
				SendToClient( &NetMsgStorageItem );

				// Ȧ�� ������ ������Ʈ.
				NetMsgStorageItem.dwChannel = dwHoldChannel;
				NetMsgStorageItem.wPosX = wHoldPosX;
				NetMsgStorageItem.wPosY = wHoldPosY;
				NetMsgStorageItem.wTurnNum = wSplit;
				SendToClient( &NetMsgStorageItem );
			}

			return S_OK;
		}

		// user locker(Inven) <-> user locker(Hold)
		// user locker ������(Inven) dwChannel, wPosX, wPosY, sInven_BackUp
		// user locker ������(Hold) dwHoldChannel, wHoldPosX, wHoldPosY, sHold_BackUp

		// ������ ���� ó��
		{
			if ( !m_cStorage[dwChannel].DeleteItem( wPosX, wPosY ) )
			{
				// �׳� �ƹ��͵� ���߱� ������ ���� ó�� �� �͵� ����.
				return S_FALSE;
			}

			if ( !m_cStorage[dwHoldChannel].DeleteItem( wHoldPosX, wHoldPosY ) )
			{
				// wPosX, wPosY ��ġ�� �ִ� ������ ó��
				if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
				{
					UserStorageDeleteSend( dwChannel, wPosX, wPosY );

					sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
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
			if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				// ������ ������ ������ �˸�
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );
				UserStorageDeleteSend( dwHoldChannel, wHoldPosX, wHoldPosY );

				// wPosX, wPosY ��ġ�� �ִ� ������ ������ ó��
				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert hold item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				DropItem( sInven_BackUp.sItemCustom, m_vPosition );

				// wHoldPosX, wHoldPosY ��ġ�� �ִ� ������ ������ ó��
				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert hold item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				DropItem( sHold_BackUp.sItemCustom, m_vPosition );

				return S_FALSE;
			}

			if ( !m_cStorage[dwHoldChannel].InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
			{
				// ������ ������ ������ �˸�
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );
				UserStorageDeleteSend( dwHoldChannel, wHoldPosX, wHoldPosY );

				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// Hold �������� �̹� ���������� ���ԵǾ� �ִ� ��Ȳ(wPosX wPosY ��ġ��)
				// Hold ��ġ�� ������ ������ ������ ��Ȳ. �̹� ���������� ���ԵǾ� �ִ� ������(Hold ������) �˸�
				SINVENITEM* pTempInsertedItem = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
				if ( !pTempInsertedItem )
				{
					sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert inven item fail, and getitem fail, disconnect user. chanum: %1% guid: %2%", 
						m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

					NET_DISCONNECT_CHARACTER NetMsg;
					NetMsg.ChaDbNum = m_CharDbNum;
					m_pGaeaServer->SENDTOAGENT( &NetMsg );
				}
				else
				{
					UserStorageInsertSend( dwChannel, pTempInsertedItem );
				}

				DropItem( sInven_BackUp.sItemCustom, m_vPosition, pTempInsertedItem == NULL ? false : true );

				return S_FALSE;
			}

			// ������ Ȯ��
			pINSERT_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
			pINSERT_HOLD_STORAGE_ITEM = m_cStorage[dwHoldChannel].GetItem( wHoldPosX, wHoldPosY );
			if ( !pINSERT_STORAGE_ITEM || !pINSERT_HOLD_STORAGE_ITEM )
			{
				// ������ ���� �α׸� Ȯ���ص� ���Ե� �������� NULL �� �α״� ������.
				// �̷� ��찡 �߻��� ��� ���� ���� ��Ų��. ĳ���Ͱ� ������ �ٷ� ���������� �����߱� ������ ����� ���̴�.

				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage result item null, disconnect user. chanum: %1% guid: %2% guid: %3%", 
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
		UserStorageUpdateItem( dwChannel, pINSERT_STORAGE_ITEM, true );
		UserStorageUpdateItem( dwHoldChannel, pINSERT_HOLD_STORAGE_ITEM, true );

		GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
		NetMsg_Del_Insert.dwChannel = dwChannel;
		NetMsg_Del_Insert.wDelX = pNetMsg->wPosX;						//	������ ������.
		NetMsg_Del_Insert.wDelY = pNetMsg->wPosY;
		NetMsg_Del_Insert.sInsert = *pINSERT_STORAGE_ITEM;				//	���ԵǴ� ������.
		SendToClient( &NetMsg_Del_Insert );

		GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_HoldInsert;
		NetMsg_Del_HoldInsert.dwChannel = dwHoldChannel;
		NetMsg_Del_HoldInsert.wDelX = wHoldPosX;						//	������ ������.
		NetMsg_Del_HoldInsert.wDelY = wHoldPosY;
		NetMsg_Del_HoldInsert.sInsert = *pINSERT_HOLD_STORAGE_ITEM;		//	���ԵǴ� ������.
		SendToClient( &NetMsg_Del_HoldInsert );
	}

	return S_OK;
}

SINVENITEM* GLChar::UserStorageGetItem( const DWORD Channel, const WORD PosX, const WORD PosY )
{
	if ( Channel >= EMSTORAGE_CHANNEL )
	{
		return NULL;
	}

	return m_cStorage[Channel].GetItem( PosX, PosY );
}

SINVENITEM* GLChar::UserStorageFindPosItem( const DWORD Channel, const WORD PosX, const WORD PosY )
{
	if ( Channel >= EMSTORAGE_CHANNEL )
	{
		return NULL;
	}

	return m_cStorage[Channel].FindPosItem( PosX, PosY );
}

BOOL GLChar::UserStorageIsInsertable( const DWORD dwChannel, const WORD wPosX, const WORD wPosY )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )
	{
		return FALSE;
	}

	return m_cStorage[dwChannel].IsInsertable( wPosX, wPosY );
}

void GLChar::UserStorageInsertSend( DWORD dwChannel, const SINVENITEM* pInsertItem )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )
	{
		return;
	}

	if ( pInsertItem )
	{
		GLMSG::SNETPC_STORAGE_INSERT NetMsg;
		NetMsg.dwChannel = dwChannel;
		NetMsg.Data = *pInsertItem;
		SendToClient( &NetMsg );
	}
}

bool GLChar::UserStorageDeleteItem( DWORD dwChannel, WORD PosX, WORD PosY, bool bSendClient, int CallType, bool DbUpdate )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )
	{
		return false;
	}

	SINVENITEM* pDeleteItem = m_cStorage[dwChannel].GetItem( PosX, PosY );
	if ( !pDeleteItem )
	{
		return false;
	}

	sc::MGUID guid = pDeleteItem->GetGuid();
	int DbState = pDeleteItem->GetDbState();

	if ( !m_cStorage[dwChannel].DeleteItem( PosX, PosY ) )
	{
		return false;
	}

	if ( DbUpdate )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemDelete( guid, DbState, CallType ) ) );
	}

	if ( bSendClient )
	{
		UserStorageDeleteSend( dwChannel, PosX, PosY );
	}

	return true;
}

void GLChar::UserStorageDeleteSend( DWORD dwChannel, WORD PosX, WORD PosY )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )
	{
		return;
	}

	GLMSG::SNETPC_STORAGE_DELETE NetMsg;
	NetMsg.dwChannel = dwChannel;
	NetMsg.wPosX = PosX;
	NetMsg.wPosY = PosY;
	SendToClient( &NetMsg );
}

bool GLChar::UserStorageInsertItem( DWORD dwChannel, const SITEMCUSTOM& Item, WORD PosX, WORD PosY, bool bSendClient, int CallType, bool DbUpdate )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )
	{
		return false;
	}

	if ( !m_cStorage[dwChannel].InsertItem( Item, PosX, PosY ) )
		return false;

	SINVENITEM* pInsertItem = m_cStorage[dwChannel].GetItem( PosX, PosY );
	if ( !pInsertItem )
	{
		return false;
	}

	if ( true == DbUpdate )
	{
		int DbState = pInsertItem->GetDbState();
		if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_dwUserID, INVEN_LOCKER, pInsertItem->sSaveData, CallType, dwChannel ) ) );
		}
		else
		{
			// db::DB_DELETE �����ε� ����� ������?
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_dwUserID, INVEN_LOCKER, pInsertItem->sSaveData, CallType, dwChannel ) ) );
		}
		pInsertItem->SetDbState( db::DB_UPDATE, true );
	}

	if ( bSendClient )
	{
		UserStorageInsertSend( dwChannel, pInsertItem );
	}

	return true;
}

void GLChar::UserStorageUpdateItem( DWORD Channel, SINVENITEM* pInvenItem, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( !pInvenItem )
	{
		return;
	}

	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_dwUserID, INVEN_LOCKER, pInvenItem->sSaveData, 9567, Channel ) ) );

		pInvenItem->SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// ���� ��ġ �� ��ǥ�� ����ȴ�.
			m_pGaeaServer->UpdateItemInvenType( m_dwUserID, pInvenItem->sItemCustom, INVEN_LOCKER, pInvenItem->wPosX, pInvenItem->wPosY, Channel );
		}
		else
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_dwUserID, INVEN_LOCKER, pInvenItem->sSaveData, 9567, Channel ) ) );
		}
	}
}
