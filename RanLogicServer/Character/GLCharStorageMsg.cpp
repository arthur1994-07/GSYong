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

	//	인벤의 아이템 확인.
	SINVENITEM* pINVENITEM = m_cStorage[dwChannel].FindPosItem( wPosX, wPosY );
	if ( !pINVENITEM )
		return false;

	//	아이템 정보 가져옴.
	SNATIVEID idItem = pINVENITEM->sItemCustom.GetNativeID();

	const SITEM* pItem = GLogicData::GetInstance().GetItem( idItem );
	if ( !pItem )
		return false;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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
	
		// 아이템의 소유 이전 경로 기록		
		// 통계 및 로그 수정
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
			//	[자신에게] 인밴 아이템 소모됨.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;
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


HRESULT GLChar::MsgReqGetStorageSpecificItem( NET_MSG_GENERIC* nmg )
{	 
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM* pNetMsg = ( GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM* ) nmg;
	
	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	std::vector< DWORD > specificItemVec;

	for ( int i = 0; i < EMSTORAGE_CHANNEL; ++i )
	{
		// 클라이언트에 창고 전송
		// 루프를 돌면서 하나씩 전송
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

	//	Note : 창고의 기본 정보 전송.
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB NetMsgItemCount;
	NetMsgItemCount.dwItemCount = static_cast< DWORD >( specificItemVec.size() );
	SendToClient( &NetMsgItemCount );

	//	Note : 아이템을 하나씩 전송.
	for ( size_t i = 0; i < specificItemVec.size(); ++i )
	{
		GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB NetMsgItem;
		NetMsgItem.Data = specificItemVec[i];
		SendToClient( &NetMsgItem );
	}

	return S_OK;
}

// *****************************************************
// Desc: 창고정보 전송 (채널별로)
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
		// 창고지기 유효성 체크
		GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
		if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
        {
            return E_FAIL;
        }
	}

	// 요청 채널 유효성 체크
	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	Note : 창고의 기본 정보 전송.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_FB NetMsg_GetFb;
	NetMsg_GetFb.lnMoney = GetStorageMoney();
	NetMsg_GetFb.dwChannel = dwChannel;
	NetMsg_GetFb.dwNumStorageItem = m_cStorage[dwChannel].GetNumItems();
	SendToClient( &NetMsg_GetFb );

	//	Note : 아이템을 하나씩 전송.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_ITEM NetMsgItem;
	NetMsgItem.dwChannel = dwChannel;

	// 클라이언트에 창고 전송
	// 루프를 돌면서 하나씩 전송
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
// Desc: 창고아이템 사용 ( 약물 )
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
		// 창고지기 유효성 체크
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

	//	Note : pk 등급이 살인마 등급 이상일 경우 회복약의 사용을 막는다.
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
			//	Note : 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER!=USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			//	Note : 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg( m_sCONFTING.wRECOVER );
			SendToClient( &NetMsg );
		}
	}

	//	창고정보를 DB에서 가져오지 않았을때.
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

	//	아이템을 사용할수 없는 상황이다.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM ) && bCheck )
		return E_FAIL;

	//	아이템을 사용할수 없는 상황이다.
    if ( IsActionLimit( EMACTION_LIMIT_DRUG ) && bCheck )
		return E_FAIL;

    // 체력 회복 아이템을 사용할수 없는 상황이다.
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


	//	창고 아이템 소모됨.
	ConsumeStorageItem( dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 체력 수치 변화를 [자신,파티원,주위]의 클라이언트들에 알림.
	MsgSendUpdateState();

	return S_OK;
}

// *****************************************************
// Desc: 창고스킬 사용 ( 약물 )
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
		// 창고지기 유효성 체크
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

	//	창고정보를 DB에서 가져오지 않았을때.
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
		//	이미 습득한 스킬.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = EMSKILL_LEARN_ALREADY;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL( sSKILL_ID );
	if ( emSKILL_LEARNCHECK != EMSKILL_LEARN_OK )
	{
		//	스킬 습득 요구 조건을 충족하지 못합니다.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = emSKILL_LEARNCHECK;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	//	Note : 스킬 습득함.
	//
	LEARN_SKILL( sSKILL_ID );

	//	Note : 스킬 습득으로 인한 퀘스트 시작 점검.
	//
	QuestStartFromGetSKILL( sSKILL_ID );

	//	Note : 소모성 아이템 제거.
	//
	ConsumeStorageItem( pNetMsg->dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	스킬 배움 성공.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	SendToClient( &NetMsgFB );

	//	스킬 배울때 소모된 포인트 업데이트.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	return S_OK;
}

// *****************************************************
// Desc: 아이템 들기 시도
// *****************************************************
HRESULT GLChar::MsgReqStorageToHold( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_TO_HOLD* pNetMsg = ( GLMSG::SNETPC_REQ_STORAGE_TO_HOLD* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD, EMHOLD_FB_OFF );
	
	return S_OK;
}

// *****************************************************
// Desc: 창고에 아이템 들기 놓기 병합 (확장기능)
// *****************************************************
// 인벤토리(HoldItem) <-> 유저락커
// 인벤토리의 아이템을 들어서 유저락커 아이템과 교환
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqInvenExStorage", m_CharDbNum ) );
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
	
	//	거래옵션
	// 창고 저장할때는 거래옵션이 아니라 새로 추가된 창고 옵션을 검사
	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
		return FALSE;
	
	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		SendToClient( &NetMsg_fb );
		return S_FALSE;
	}

	// 프리미엄 라인 유효기간이 지났다면 그냥 리턴한다. 그렇지 않으면 바닥에 아이템이 떨어진다.
	if ( GLCONST_CHAR::wInvenDefaultLine + GetOnINVENLINE() <= pNetMsg->wHoldPosY )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	인밴에 있던 아이템 백업.	
	WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
	WORD wPosY = sInven_BackUp.wPosY;

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
	const SITEM* pHoldData = GLogicData::GetInstance().GetItem( sHold_BackUp.sItemCustom.GetNativeID() );
	if ( !pInvenData || !pHoldData )
		return S_FALSE;

	//	Note : 병합 할수 있는 아이템인지 점검.
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
            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_CHAR, m_CharDbNum, 
                gamelog::ID_USER, m_dwUserID, 
                ITEM_ROUTE_USER_INVEN, sHold_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_EXCHANGE_INVEN_WITH_STORAGE,
				false );

			//	Note : 겹침 수 조정.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	Note : 들고 있는 아이템 지움.
            InvenDeleteItem( sHold_BackUp.wPosX, sHold_BackUp.wPosY, true, sHold_BackUp.sItemCustom.bVietnamGainItem, 1, true );

			//	Note : 창고 아이템 업데이트.
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
			
            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange( 
                sHold_BackUp.sItemCustom, 
                gamelog::ID_CHAR, m_CharDbNum, 
                gamelog::ID_USER, m_dwUserID, 
                ITEM_ROUTE_USER_INVEN, pInvenData->sDrugOp.wPileNum - sInven_BackUp.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_EXCHANGE_INVEN_WITH_STORAGE,
				false );

			//	기존 아이템 갯수 변경. ( 꽉채움. )
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경. ( 갯수 줄임. )
			sHold_BackUp.sItemCustom.wTurnNum = wSplit;
            pHoldInvenItem->sItemCustom = sHold_BackUp.sItemCustom;

            // 인벤의 아이템 업데이트.
            GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
            NetMsgInvenItem.wPosX = pNetMsg->wHoldPosX;
            NetMsgInvenItem.wPosY = pNetMsg->wHoldPosY;
            NetMsgInvenItem.sItemCustom = pHoldInvenItem->CustomItem();
            SendToClient( &NetMsgInvenItem );
			
			//	Note : 창고 아이템 업데이트.
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
	// user locker 아이템 wPosX, wPosY, sInven_BackUp
	// inven 아이템 wHoldPosX, wHoldPosY, sHold_BackUp

	// 아이템 삭제 처리
	{
		if ( !m_cStorage[dwChannel].DeleteItem( wPosX, wPosY ) )
		{
			// 그냥 아무것도 안했기 때문에 예외 처리 할 것도 없다.
			return S_FALSE;
		}

		if ( !m_cInventory.DeleteItem( wHoldPosX, wHoldPosY ) )
		{
			// wPosX, wPosY 위치에 있던 아이템 처리
			if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );

				sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
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
		if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
		{
			// 위에서 삭제된 아이템 알림
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
			// 위에서 삭제된 아이템 알림
			UserStorageDeleteSend( dwChannel, wPosX, wPosY );
			InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			sc::writeLogError( sc::string::format( "critical error MsgReqInvenExStorage insert inven item fail. chanum: %1% guid: %2%", 
				m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

			// Hold 아이템은 이미 정상적으로 삽입되어 있는 상황(wPosX wPosY 위치에)
			// Hold 위치에 아이템 삽입이 실패한 상황. 이미 정상적으로 삽입되어 있는 아이템(Hold 아이템) 알림
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

		// 아이템 확인
		pINSERT_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
		pINSERT_IVEN_ITEM = m_cInventory.GetItem( wHoldPosX, wHoldPosY );
		if ( !pINSERT_STORAGE_ITEM || !pINSERT_IVEN_ITEM )
		{
			// 예전의 서버 로그를 확인해도 삽입된 아이템이 NULL 인 로그는 없었다.
			// 이런 경우가 발생할 경우 접속 종료 시킨다. 캐릭터가 나가면 바로 위에까지는 성공했기 때문에 저장될 것이다.

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

	// 디비 처리
	UserStorageUpdateItem( dwChannel, pINSERT_STORAGE_ITEM, true );
	InvenUpdateItem( pINSERT_IVEN_ITEM, true );

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange( 
        sHold_BackUp.sItemCustom, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_USER, m_dwUserID, 
        ITEM_ROUTE_USER_INVEN, sHold_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_INVEN_WITH_STORAGE,
		false );

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange( 
        sInven_BackUp.sItemCustom, 
        gamelog::ID_USER, m_dwUserID, 
        gamelog::ID_CHAR, m_CharDbNum, 
        ITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_EXCHANGE_INVEN_WITH_STORAGE,
		false );

	GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.dwChannel = dwChannel;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이템.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pINSERT_STORAGE_ITEM;				//	삽입되는 아이템.
	SendToClient( &NetMsg_Del_Insert );

    GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Inven_Del_Insert;
    NetMsg_Inven_Del_Insert.wDelX = sHold_BackUp.wPosX;				//	삭제될 아이템.
    NetMsg_Inven_Del_Insert.wDelY = sHold_BackUp.wPosY;
    NetMsg_Inven_Del_Insert.sInsert = *pINSERT_IVEN_ITEM;			//	삽입되는 아이템.

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsg_Inven_Del_Insert );
    SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

    //	유효기간 초과 점검.	
    HRESULT hrCheckDuration = CheckItemDuration( *pINSERT_IVEN_ITEM );
    if ( hrCheckDuration == E_FAIL )
        return E_FAIL;
	
	return S_OK;
}

// 인벤의 홀드 아이템 창고에 놓기
// 디비에는 실제 위치만 변경된다. 인벤토리->창고
HRESULT GLChar::MsgReqInvenToStorage( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_STORAGE* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_TO_STORAGE* ) nmg;
	GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb( NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB,EMHOLD_FB_OFF );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

	//	거래옵션
	// 창고 저장할때는 거래옵션이 아니라 새로 추가된 창고 옵션을 검사
	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
		return FALSE;

	// 팻카드일경우 팻이 활동중이면
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pMyPet->DbNum() == pHoldInvenItem->CustomItem().PetDbNum() )
            return FALSE;
	}

	BOOL bOk = m_cStorage[dwChannel].IsInsertable( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.

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

	// 디비 업데이트
	UserStorageUpdateItem( dwChannel, pINSERT_HOLD_STORAGE_ITEM, true );

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
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

//! Char Inven -> Locker, 돈 넣기
HRESULT GLChar::MsgReqStorageSaveMoney( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

    // 입금할 금액
    LONGLONG ReceiptMoney = pPacket->lnMoney;
	if ( ReceiptMoney < 0 || GetInvenMoney() < ReceiptMoney )
    {
        sc::writeLogError( sc::string::format( "HACKING! MsgReqStorageSaveMoney %1% < %2% CharDbNum %3%", GetInvenMoney(), ReceiptMoney, m_CharDbNum ) );
        return E_FAIL;
    }

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		GLCrow* pCrow = pLand->GetCrow( pPacket->dwNpcGlobID );
		if ( !pCrow )
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
            return E_FAIL;
	}

	CheckMoneyUpdate( GetInvenMoney(), ReceiptMoney, FALSE, "Storage Save Money." );

	SubtractInvenMoneyServer( ReceiptMoney, SUBTRACT_INVEN_MONEY_USER_STORAGE_SAVE );
	AddStorageMoney( ReceiptMoney );

	// 금액 로그
    // 거래후 캐릭터 인벤토리 전체 금액
    // 거래된 변화량
    // 거래후 캐릭터 락커 금액
	// 통계 및 로그 수정
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, ReceiptMoney,     ITEM_ROUTE_USER_INVEN, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MOVE_INVEN_TO_STORAGE );
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER,          0, GetInvenMoney(),        MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, GetStorageMoney(), MONEY_ROUTE_USER_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_STORAGE_CURRENT );

	// Inven 클라이언트에 돈 액수 변화를 알려줌. 
	SendInvenMoneyUpdate();

	// Locker 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = GetStorageMoney();
	SendToClient( &NetMsgStorage );

	return S_OK;
}

// *****************************************************
// Desc: 돈 찾기
// *****************************************************
HRESULT GLChar::MsgReqStorageDrawMoney( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar ) 
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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
		// 창고지기 유효성 체크
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

	// 금액 로그
    // 거래후 캐릭터 Inven 전체머니
    // Locker Money 변화량
    // 거래후 캐릭터 Locker 전체머니
	// 통계 및 로그 수정
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID,         -lnMoney, ITEM_ROUTE_USER_INVEN, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_MOVE_STORAGE_TO_INVEN );
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, GetStorageMoney(), MONEY_ROUTE_USER_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_STORAGE_CURRENT );
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER,          0,        GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
    
	// 클라이언트에 돈 액수 변화를 알려줌.
	SendInvenMoneyUpdate();

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = GetStorageMoney();
	SendToClient( &NetMsgStorage );

	return S_OK;
}

// *****************************************************
// Desc: 소모성 아이템 개수 분리
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
		// 창고지기 유효성 체크
		GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
		if ( !pCrow )					
            return E_FAIL;

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STORAGE ) )
        {
            return E_FAIL;
        }
	}

	GLInventory& sInventory = m_cStorage[pNetMsg->dwChannel];

	//	Note : 분리 요청 아이템 찾음.
	SINVENITEM* pInvenItem = sInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return E_FAIL;

	//	Note : 아이템 정보 가져오기.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	//	Note : 분리 가능한 아이템인지 점검.
	bool bSPLIT( false );
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )
		return E_FAIL;
	if ( pNetMsg->wSplit < 1 )
		return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )
		return E_FAIL;

	//	Note : 원본 아이템의 변화된 갯수.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : 분리되어 새로 생기는 아이템.
	SITEMCUSTOM sNEW_ITEM( pInvenItem->sItemCustom );
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;
	
	// guid 생성, 아이템이 분리될 때 새로운 guid 를 생성한다.
	sc::SeqUniqueGUID::Instance()->getGUID( sNEW_ITEM.guid );
	sNEW_ITEM.GenerateBasicStat( false );
	sNEW_ITEM.GenerateLinkSkill();
	sNEW_ITEM.GenerateAddOption();

	//	Note : 새로 생기는 아이템 위치 찾기.
	WORD wInvenPosX( 0 ), wInvenPosY( 0 );
	bOk = sInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )
		return E_FAIL;

	//	Note : 새로 생긴 아이템 넣기.
	if ( !UserStorageInsertItem( pNetMsg->dwChannel, sNEW_ITEM, wInvenPosX, wInvenPosY, false, 1, true ) )
	{
		return E_FAIL;
	}

	//	Note : 새로 생긴 아이템 정보 가져오기.
	SINVENITEM* pNewItem = sInventory.GetItem( wInvenPosX, wInvenPosY );
	if ( !pNewItem )
		return E_FAIL;

	//	Note : 원본 아이템의 갯수 변경 시킴.
	// 디비에 개수만 변경하는 함수 만들고 호출해야 한다.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : 인벤에 새로 분리되어 들어 가는 아이템.
	GLMSG::SNETPC_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	SendToClient( &NetMsgNew );

	//	Note : 원본 아이템 갯수 변경.
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
// - 홀드 아이템 창고에 놓기, 기존의 MsgReqStorageToStorage, MsgReqStorageExStorage을 병합함
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

	// 창고지기 유효성 체크
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

	//!	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )
		LoadStorageFromDB();

	// 놓을려는 위치에 아이템이 존재하는지 여부
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

	// 거래옵션, 거래 불가 아이템이라면
	// 창고 저장할때는 거래옵션이 아니라 새로 추가된 창고 옵션을 검사
	// 창고에서 창고로 이동인데 검사할 필요가 있나? 없을 것 같아서 주석 처리함 [8/31/2016 khkim]
// 	if ( pHoldInvenItem->sItemCustom.IsNotStorage() )
// 		return E_FAIL;

	// 팻카드일경우 팻이 활동중이면
	if ( isInvenItem == false && pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pMyPet->DbNum() == pHoldInvenItem->CustomItem().PetDbNum() )
			return S_FALSE;
	}

	SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
	WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
	WORD wHoldPosY = sHold_BackUp.wPosY;

	if ( !pHoldData )
		return S_FALSE;

	SINVENITEM* pINSERT_HOLD_STORAGE_ITEM = NULL;
	SINVENITEM* pINSERT_STORAGE_ITEM = NULL;

	// To 의 경우
	// 창고에서 창고 특정 좌표로 아이템 이동
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

		// 디비 업데이트
		UserStorageUpdateItem( dwChannel, pINSERT_HOLD_STORAGE_ITEM, true );
	}
	else // pNetMsg->nType == NET_MSG_GCTRL_REQ_STORAGE_EX_STORAGE, 서로 위치 변경 ex
	{
		SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( pInvenItem == NULL )
		{
			SendToClient( &NetMsg_fb );
			return S_FALSE;
		}

		SINVENITEM sInven_BackUp = *pInvenItem;		//	인밴에 있던 아이템 백업.	
		WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
		WORD wPosY = sInven_BackUp.wPosY;

		const SITEM* pInvenData = GLogicData::GetInstance().GetItem( sInven_BackUp.sItemCustom.GetNativeID() );
		if ( pInvenData == NULL )
			return S_FALSE;

		// 병합 할수 있는 아이템인지 점검.
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

			//! 두 아이템을 병합할 시 개수가 최대 개수를 넘어선다면
			if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
			{
				//	Note : 겹침 수 조정.
				pInvenItem->sItemCustom.wTurnNum = wTurnNum;

				//	Note : 들고 있는 아이템 지움.
				UserStorageDeleteItem( dwHoldChannel, wHoldPosX, wHoldPosY, true, 1, true );

				//	Note : 창고 아이템 업데이트.
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

				//	기존 아이템 갯수 변경. ( 꽉채움. )
				pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

				//	손에 들고 있던 아이템 변경. ( 갯수 줄임. )
				pHoldInvenItem->sItemCustom.wTurnNum = wSplit;

				// 창고 아이템 업데이트.
				GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
				NetMsgStorageItem.dwChannel = dwChannel;
				NetMsgStorageItem.wPosX = wPosX;
				NetMsgStorageItem.wPosY = wPosY;
				NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
				SendToClient( &NetMsgStorageItem );

				// 홀드 아이템 업데이트.
				NetMsgStorageItem.dwChannel = dwHoldChannel;
				NetMsgStorageItem.wPosX = wHoldPosX;
				NetMsgStorageItem.wPosY = wHoldPosY;
				NetMsgStorageItem.wTurnNum = wSplit;
				SendToClient( &NetMsgStorageItem );
			}

			return S_OK;
		}

		// user locker(Inven) <-> user locker(Hold)
		// user locker 아이템(Inven) dwChannel, wPosX, wPosY, sInven_BackUp
		// user locker 아이템(Hold) dwHoldChannel, wHoldPosX, wHoldPosY, sHold_BackUp

		// 아이템 삭제 처리
		{
			if ( !m_cStorage[dwChannel].DeleteItem( wPosX, wPosY ) )
			{
				// 그냥 아무것도 안했기 때문에 예외 처리 할 것도 없다.
				return S_FALSE;
			}

			if ( !m_cStorage[dwHoldChannel].DeleteItem( wHoldPosX, wHoldPosY ) )
			{
				// wPosX, wPosY 위치에 있던 아이템 처리
				if ( !m_cStorage[dwChannel].InsertItem( sInven_BackUp.CustomItem(), wPosX, wPosY ) )
				{
					UserStorageDeleteSend( dwChannel, wPosX, wPosY );

					sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage delete hold item fail, insert inven item fail. chanum: %1% guid: %2%", 
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
			if ( !m_cStorage[dwChannel].InsertItem( sHold_BackUp.CustomItem(), wPosX, wPosY ) )
			{
				// 위에서 삭제된 아이템 알림
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );
				UserStorageDeleteSend( dwHoldChannel, wHoldPosX, wHoldPosY );

				// wPosX, wPosY 위치에 있던 삭제된 아이템 처리
				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert hold item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				DropItem( sInven_BackUp.sItemCustom, m_vPosition );

				// wHoldPosX, wHoldPosY 위치에 있던 삭제된 아이템 처리
				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert hold item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sHold_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				DropItem( sHold_BackUp.sItemCustom, m_vPosition );

				return S_FALSE;
			}

			if ( !m_cStorage[dwHoldChannel].InsertItem( sInven_BackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
			{
				// 위에서 삭제된 아이템 알림
				UserStorageDeleteSend( dwChannel, wPosX, wPosY );
				UserStorageDeleteSend( dwHoldChannel, wHoldPosX, wHoldPosY );

				sc::writeLogError( sc::string::format( "critical error MsgReqStorageToStorage insert inven item fail. chanum: %1% guid: %2%", 
					m_CharDbNum, sc::string::uuidToString( sInven_BackUp.sItemCustom.GetGuid() ).c_str() ) );

				// Hold 아이템은 이미 정상적으로 삽입되어 있는 상황(wPosX wPosY 위치에)
				// Hold 위치에 아이템 삽입이 실패한 상황. 이미 정상적으로 삽입되어 있는 아이템(Hold 아이템) 알림
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

			// 아이템 확인
			pINSERT_STORAGE_ITEM = m_cStorage[dwChannel].GetItem( wPosX, wPosY );
			pINSERT_HOLD_STORAGE_ITEM = m_cStorage[dwHoldChannel].GetItem( wHoldPosX, wHoldPosY );
			if ( !pINSERT_STORAGE_ITEM || !pINSERT_HOLD_STORAGE_ITEM )
			{
				// 예전의 서버 로그를 확인해도 삽입된 아이템이 NULL 인 로그는 없었다.
				// 이런 경우가 발생할 경우 접속 종료 시킨다. 캐릭터가 나가면 바로 위에까지는 성공했기 때문에 저장될 것이다.

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
		
		// 디비 업데이트
		UserStorageUpdateItem( dwChannel, pINSERT_STORAGE_ITEM, true );
		UserStorageUpdateItem( dwHoldChannel, pINSERT_HOLD_STORAGE_ITEM, true );

		GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
		NetMsg_Del_Insert.dwChannel = dwChannel;
		NetMsg_Del_Insert.wDelX = pNetMsg->wPosX;						//	삭제될 아이템.
		NetMsg_Del_Insert.wDelY = pNetMsg->wPosY;
		NetMsg_Del_Insert.sInsert = *pINSERT_STORAGE_ITEM;				//	삽입되는 아이템.
		SendToClient( &NetMsg_Del_Insert );

		GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_HoldInsert;
		NetMsg_Del_HoldInsert.dwChannel = dwHoldChannel;
		NetMsg_Del_HoldInsert.wDelX = wHoldPosX;						//	삭제될 아이템.
		NetMsg_Del_HoldInsert.wDelY = wHoldPosY;
		NetMsg_Del_HoldInsert.sInsert = *pINSERT_HOLD_STORAGE_ITEM;		//	삽입되는 아이템.
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
			// db::DB_DELETE 상태인데 여기로 들어오면?
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
			// 저장 위치 및 좌표만 변경된다.
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
