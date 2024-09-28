#include "../pch.h"
#include "../../RanLogic/Skill/GLRnSkillLearnSetting.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"

#include "./GLChar.h"

#include "../Club/GLClubServer.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../PK/GLSchoolFreePK.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Util/GLItemLimit.h"


#include "../../SigmaCore/DebugInclude.h"



HRESULT GLChar::MsgReqLearnSkill ( NET_MSG_GENERIC* nmg )
{
	if (!IsValidBody())
        return E_FAIL;

	GLMSG::SNETPC_REQ_LEARNSKILL *pNetMsg = (GLMSG::SNETPC_REQ_LEARNSKILL *) nmg;

	SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
        return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return S_FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_SKILL )
        return E_FAIL;

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL(sSKILL_ID) )
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
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

	//	스킬 배움 성공.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	SendToClient( &NetMsgFB );

	//	스킬 배울때 소모된 포인트 업데이트.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_SKILLBOOK) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	//	Note : passive skill 의 경우 케릭에 적용되는 속성값을 클라이언트들에 동기화.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKILL_ID );
	if ( !pSkill ) return E_FAIL;

	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
		SendMsgViewAround(&NetMsgBrd);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqSkillUp ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_SKILLUP *pNetMsg = (GLMSG::SNETPC_REQ_SKILLUP *) nmg;

	EMSKILL_LEARNCHECK emSKILL_LVLUPCHECK = EMSKILL_LEARN_UNKNOWN;
	SCHARSKILL* pCHARSKILL = GETLEARNED_SKILL(pNetMsg->skill_id);
	if ( !pCHARSKILL )
	{
		//	정상적으로 발생 할 수 없는 상황.
		return E_FAIL;
	}

	emSKILL_LVLUPCHECK = CHECKLEARNABLE_SKILL(pNetMsg->skill_id);
	if ( emSKILL_LVLUPCHECK!=EMSKILL_LEARN_OK )
	{
		//	랩업 조건이 부족합니다. FB 메시지.
		GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
		NetMsgFB.sSkill.sNativeID = pNetMsg->skill_id;
		NetMsgFB.emCHECK = emSKILL_LVLUPCHECK;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	Note : 스킬 LEVEL UP.
	//
	WORD wToLevel = pCHARSKILL->wLevel + 1;
	LVLUP_SKILL ( pNetMsg->skill_id, wToLevel );

	//	랩업. FB 메시지.
	GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
	
	NetMsgFB.sSkill.sNativeID = pNetMsg->skill_id;
	NetMsgFB.sSkill.wLevel = wToLevel;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;

	SendToClient( &NetMsgFB );

	//	스킬 배울때 소모된 포인트 업데이트.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	//	Note : passive skill 의 경우 케릭에 적용되는 속성값을 클라이언트들에 동기화.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->skill_id );
	if ( !pSkill )
		return E_FAIL;

	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
		SendMsgViewAround(&NetMsgBrd);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqRnSkillUp( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqRnSkillUp", m_CharDbNum ) );
	}

    GLMSG::SNETPC_REQ_RNSKILLUP *pNetMsg = (GLMSG::SNETPC_REQ_RNSKILLUP *) nmg;
    WORD nInt = pNetMsg->GetCount();
    const WORD MAXCOUNT = GLMSG::SNETPC_REQ_RNSKILLUP::EMMAX_COUNT;

    if( MAXCOUNT < nInt )
        return E_FAIL;

	DWORD dwCompleteType(0);

    for( unsigned int i=0 ; i < nInt ; i++)
    {
        SNATIVEID& sSkillID = pNetMsg->GetSkillID( i );
        if( sSkillID == NATIVEID_NULL() )
            return 0;
        /*
        EMSKILL_LEARN_OK		= 0,	//	배우기 성공.
        EMSKILL_LEARN_MAX		= 1,	//	더 이상 렙업을 할 수 없음.
        EMSKILL_LEARN_NOTTERM	= 2,	//	요구 조건이 충족되지 않아서 렙업을 할수 없음.
        EMSKILL_LEARN_NOTCLASS	= 3,	//	부서가 틀려서 배울수 없습니다.
        EMSKILL_LEARN_NOTBRIGHT	= 4,	//	빛/암 조건이 충족되지 않습니다.
        EMSKILL_LEARN_ALREADY	= 5,	//	이미 배운 스킬입니다.
        */

        EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL( sSkillID );
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
		if ( !pSkill ) 
			continue;

        SCHARSKILL* pCHARSKILL = GETLEARNED_SKILL( sSkillID );
        switch( emSKILL_LEARNCHECK )
        {
        case EMSKILL_LEARN_MAX:
            break;
        case EMSKILL_LEARN_NOTTERM:
            {
                //	랩업 조건이 부족합니다. FB 메시지.
                GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
                NetMsgFB.sSkill.sNativeID = sSkillID;
                NetMsgFB.emCHECK = emSKILL_LEARNCHECK;
                SendToClient( &NetMsgFB );
            }
            return E_FAIL;
        case EMSKILL_LEARN_NOTCLASS:
            break;
        case EMSKILL_LEARN_NOTBRIGHT:
            break;
        case EMSKILL_LEARN_ALREADY:
            break;
        case EMSKILL_LEARN_UNKNOWN:
            break;
        case EMSKILL_LEARN_OK:
            // 최초 스킬 습득
            if ( pCHARSKILL == NULL ) 
            {
                bool bUseScrool = GLogicData::GetInstance().GetRnSkillUseScrool( sSkillID );
                if( bUseScrool ) // 오로지 스킬 스크롤을 통해서 스킬을 습득 할 수 있다.
                {
                    
                    continue;
                }
                else            // 돈으로도 스킬을 배울 수 있다.
                {
                    // Note : 스크롤 사용여부와 골드소모량을 가지고 온다.
					// m_pSkill->m_sLEARN.sLVL_STEP[nLevel].dwReqMoney 
					
                    LONGLONG llUseMoney = pSkill->m_sLEARN.sLVL_STEP[0].dwReqMoney;//GLogicData::GetInstance().GetRnSkillUseMoney( sSkillID );

                    if( GetInvenMoney() < llUseMoney )
                       continue;

                    //  Note : 골드 소모
                    SubtractInvenMoneyServer( llUseMoney, SUBTRACT_INVEN_MONEY_SKILL_LEARN );
					// 통계 및 로그 수정
                    LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llUseMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_LEARN_SKILL);
					// 통계 및 로그 수정
                    LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
                    SendInvenMoneyUpdate();
                }
                                     
                //	Note : 스킬 습득함.
                LEARN_SKILL(sSkillID);

                //	Note : 스킬 습득으로 인한 퀘스트 시작 점검.
                QuestStartFromGetSKILL ( sSkillID );

                //	스킬 배움 성공.
                GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
                NetMsgFB.skill_id = sSkillID;
                NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
                SendToClient( &NetMsgFB );

				dwCompleteType |= GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEARN;

                //	스킬 배울때 소모된 포인트 업데이트.
                GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
                NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
                SendToClient( &NetMsgSkp );

                if ( GLHIDESET::CheckHideSet(EMHIDE_USE_SKILLBOOK) )
				{
					RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					RestoreActStateByInvisible();
				}

                //	Note : passive skill 의 경우 케릭에 적용되는 속성값을 클라이언트들에 동기화.
                if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
                {
                    GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = m_dwGaeaID;
                    NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
                    SendMsgViewAround(&NetMsgBrd);
                }
            }
            else // 스킬 레벨업
            {
				//	Note : 스킬 LEVEL UP.
				WORD wToLevel = pCHARSKILL->wLevel + 1;

				// 요구 돈 체크;
				LONGLONG llReqMoney = pSkill->m_sLEARN.sLVL_STEP[wToLevel].dwReqMoney;
				if( GetInvenMoney() < llReqMoney )
					continue;

				//  Note : 골드 소모
				SubtractInvenMoneyServer( llReqMoney, SUBTRACT_INVEN_MONEY_SKILL_LEARN );
				// 통계 및 로그 수정
				LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llReqMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_LEARN_SKILL);
				// 통계 및 로그 수정
				LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
				SendInvenMoneyUpdate();

                LVLUP_SKILL ( sSkillID, wToLevel );

                //	랩업. FB 메시지.
                GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
                NetMsgFB.sSkill.sNativeID = sSkillID;
                NetMsgFB.sSkill.wLevel = wToLevel;
                NetMsgFB.emCHECK = EMSKILL_LEARN_OK;

                SendToClient( &NetMsgFB );

                //	스킬 배울때 소모된 포인트 업데이트.
                GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
                NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
                SendToClient( &NetMsgSkp );

				dwCompleteType |= GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEVELUP;

                //	Note : passive skill 의 경우 케릭에 적용되는 속성값을 클라이언트들에 동기화.
                //
                if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
                {
                    GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = m_dwGaeaID;
                    NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
                    SendMsgViewAround(&NetMsgBrd);
                }
            }
            
        }
    }

	// 스킬 레벨업 완료
	GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE completeMsg;
	completeMsg.dwComplete = dwCompleteType;
	SendToClient( &completeMsg );

    return S_OK;
}

HRESULT	GLChar::MsgReqSkill_Targets( GLMSG::SNETPC_REQ_SKILL* pNetMsg, PGLSKILL	pSkill, WORD wLevel, WORD wSkillRange, WORD& wTARNUM, STARID* sTARIDS )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
		return E_FAIL;

    const bool 	bFreePKMap		= pLand->IsFreePK();
    const bool 	bGuidBattleMap	= pLand->m_bGuidBattleMap;
    const bool 	bBRIGHTEVENT		= IsBRIGHTEVENT();
    const bool 	bPKMap				= pLand->IsPKMap();
    const bool 	bCTF					= (IsCTFPlayer() == TRUE) ? true : false;
    const bool	benemy				= ( EMIMPACT_SIDE_ENEMY == pSkill->m_sBASIC.emIMPACT_SIDE );
    const bool	bRebirth				= pSkill->IsRebirth();
    const bool	bBuff					= pSkill->IsBuff();

    EMSERVICE_PROVIDER	ServiceProvider = GetServiceProvider();
    //#if !defined(KR_PARAM) && !defined(KRT_PARAM)
    if( ServiceProvider == SP_KOREA || ServiceProvider == SP_KOREA_TEST )
    {
        // 딜래이 체크
        if ( m_fSkillDelay < 0.3f )
            return E_FAIL;  // 05.11.30  0.5f -> 0.3f
    }
    //#endif	

    // 타겟 갯수에 대한 유효성 검사; 클라이언트 측에서 조작 가능;
    // 타겟의 거리 유효성 검사; 클라이언트 측에서 조작 가능;		
    // 기존 코드에는 아군의 경우 검사를 하지 않았음; -> 조작 가능;
    // 관통의 경우 관통 정도에 따라 따로 검사;
    if( ServiceProvider != SP_KOREA && ServiceProvider != SP_KOREA_TEST && ServiceProvider != SP_JAPAN )
    { // pNetMsg 안에 담긴 TARIDS를 레퍼런스로 넘겨 유효성 검사후 값 변경;
        GLSkillTargetArgv SkillTargetArgv(this, pNetMsg, pSkill, m_TargetID, wLevel);		
        switch ( pSkill->m_sBASIC.emIMPACT_TAR )
        {
        case TAR_SELF:
            if ( setTarget_Self(SkillTargetArgv) == false )
                return E_FAIL;
            break;
        case TAR_SPEC:
            if ( setTarget_Spec(SkillTargetArgv) == false )
                return E_FAIL;
            break;
        case TAR_SELF_TOSPEC:
            if ( setTarget_SelfToSpec(SkillTargetArgv) == false )
                return E_FAIL;
            break;
        case TAR_ZONE:
            if ( setTarget_Zone(SkillTargetArgv) == false )
                return E_FAIL;
            break;
        case TAR_LAND:
            if ( setTarget_Land(SkillTargetArgv) == false )
                return E_FAIL;
            break;
        }
    }

    WORD i = 0, j = 0;
    for( i = 0, j = 0; i < pNetMsg->btTARNUM && j < EMTARGET_NET; ++i )
    {
        //	Note : 유효성 검사.
        //		
        // skill 적용이 가능한 타켓인지를 검사;
        // 만약 중립적인 대상인 경우는 강제 공격한격으로 판정;
        STARGETID sTARID(pNetMsg->sTARIDS[i].GETCROW(),pNetMsg->sTARIDS[i].GETID());
        ServerActor* pTARGET = GetTarget(sTARID);

		if( pTARGET == NULL ) continue;

        const EMREACTIONABLE emREACTIONALBE  = IsReActionable( pTARGET, pSkill->m_sBASIC.emIMPACT_SIDE );
        switch ( emREACTIONALBE )
        {
        case EMREACTIONABLE_FALSE:
            continue;

        case EMREACTIONABLE_TRUE:
            break;

        case EMREACTIONABLE_FORCED_TRUE:
            // skill 적용이 가능한데 중립적대상이라서 강제 사용으로 판정되었을 경우;
            // 
            // 주 타겟 외에 타겟은 강제 공격 불가하다.;
            // PC의 경우 Client에서 타겟선별을 처리하여 서버로 정보를 보내는데
            // 주타겟외에 강제공격 타겟이 있다는 말은 패킷 조작일 가능성이 있다.;
            // 
            if ( m_TargetID != sTARID )
                continue;
            ReActionByForcedAction(pTARGET, pSkill->m_sBASIC.emIMPACT_SIDE);
            break;

        default:
            continue;
        }

        sTARIDS[j++] = pNetMsg->sTARIDS[i];
    } //for

    wTARNUM = j;

    return S_OK;
} //GLChar::MsgReqSkill_Targets

HRESULT	GLChar::MsgReqSkill_CheckToStartSkill( GLMSG::SNETPC_REQ_SKILL* pNetMsg, PGLSKILL pSkill, EMSKILLCHECK emCHECK )
{
	const SKILL::SEXT_DATA&		sEXT_DATA	= pSkill->m_sEXT_DATA;	
	SANIATTACK*					pAniAttack	= GetAniAttack( sEXT_DATA.emANIMTYPE, sEXT_DATA.emANISTYPE );
	if( !pAniAttack )
		return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
		return E_FAIL;

	WORD						wStrikeNum	= pAniAttack->m_wDivCount;
	if( wStrikeNum == 0 )
	{
		// 스킬 에니메이션에 타격 지점이 지정되지 않았습니다
		sc::writeLogError(
			sc::string::format(
			"MsgReqSkill Can't find target position of skill animation. ChaClass %1% animation %2%/%3%",
			m_CHARINDEX,
			sEXT_DATA.emANIMTYPE,
			sEXT_DATA.emANISTYPE));
		
		return E_FAIL;
	} //if

	if( emCHECK != EMSKILL_OK && emCHECK != EMSKILL_NOTSP )
	{
		//	Note : 스킬 구동 실패 FB 메시지.
		GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
		NetMsgFB.emSKILL_FB = emCHECK;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	} //if


	return S_OK;
} //GLChar::MsgReqSkill_CheckToStartSkill

HRESULT GLChar::MsgReqSkill ( NET_MSG_GENERIC* nmg )
{
	if ( IsActState(EM_ACT_OBSERVE) == true )
		return E_FAIL; // 관전 모드 중에는 이동 외에 어떠한 행동도 할 수 없다;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;	

	GLMSG::SNETPC_REQ_SKILL *pNetMsg = (GLMSG::SNETPC_REQ_SKILL *) nmg;
	SNATIVEID				skill_id = pNetMsg->skill_id;

	// 발동 스킬일경우 현재 발동 스킬인지 검사
	if ( pNetMsg->bDefenseSkill )
	{
		if (pNetMsg->skill_id != m_sDefenseSkill.m_dwSkillID)
            return E_FAIL;

		if (!m_bDefenseSkill)
            return E_FAIL;
	} //if

	// 대련 종료후 무적타임이면 공격 무시
	if( m_sCONFTING.IsPOWERFULTIME () ) 
		return E_FAIL;

	//	스킬 정보 가져옴.
	PGLSKILL	pSkill		= GLSkillMan::GetInstance().GetData( skill_id.wMainID, skill_id.wSubID );
	if( !pSkill )
		return E_FAIL;

	WORD		wSkillRange = GETSKILLRANGE_TAR(*pSkill);	
	WORD		wLevel		= 0;
	if( pNetMsg->bDefenseSkill )
	{
		wLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		SCHARSKILL *pSkill = GETLEARNED_SKILL( pNetMsg->skill_id );
		if( pSkill )
			wLevel = pSkill->wLevel;
	} //if..else

	// 타겟 지정.
	if( pNetMsg->btTARNUM )
	{
		m_TargetID.vPos   = pNetMsg->vTargetPosition;
		m_TargetID.emCrow = pNetMsg->sTARIDS[0].GETCROW();
		m_TargetID.GaeaId = pNetMsg->sTARIDS[0].GETID();
	} //if

	// 전체 타겟 리셋
	STARID defaultSTARID;
	for( int i=0; i<(EMTARGET_NET<<1); ++i )
		m_sTARIDS[ i ] = defaultSTARID;

////////////////////////////////////////////////////////

	//m_TargetID.vPos   = pNetMsg->vSelectTargetPosition;
	//m_TargetID.emCrow = pNetMsg->emTarCrow;
	//m_TargetID.GaeaId = pNetMsg->dwTarID;

	//BOOL bOk = FALSE;
	//ServerActor* pActorTarget = GetTarget( m_TargetID );
	//if (!pActorTarget)
	//	return E_FAIL;
	//bOk = pLand->m_SkillTargetSystem.SelectSkillTarget( this, pActorTarget, pNetMsg->skill_id, wLevel, GetPositionActor(), m_sTARIDS, m_wTARNUM);
	//if (!bOk)
	//	return E_FAIL;
/////////////////////////////////////////////////////////////

	// 타겟의 유효성 점검.
	if( MsgReqSkill_Targets( pNetMsg, pSkill, wLevel, wSkillRange, m_wTARNUM, m_sTARIDS ) == E_FAIL )
		return E_FAIL;
	
	// Note : 지면 위치 스킬이 아닐때 타겟이 없으면 시전 불가능
	if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_LAND && m_wTARNUM == 0 )
	{
		// 자신에게.
		GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
		NetMsg.emTarCrow		= m_TargetID.emCrow;
		NetMsg.dwTarID			= m_TargetID.GaeaId;
		NetMsg.nDamage			= 0;
        NetMsg.wSchool          = GetSchool();
		SendToClient( &NetMsg );

		return E_FAIL;
	}

	// 스킬 구동 조건 검사.
	DWORD				dwSkillCheckOption = 0;
	if( pNetMsg->bDefenseSkill )
		dwSkillCheckOption |= EMSKILLCHECKOPTION_DEFENSESKILL;

	EMSKILLCHECK		emCHECK = GLCHARLOGIC::CHECHSKILL( pNetMsg->skill_id, 1, m_emVehicle, dwSkillCheckOption );
	if( MsgReqSkill_CheckToStartSkill( pNetMsg, pSkill, emCHECK ) != S_OK )
		return E_FAIL;

	// HideSet Table 확인 후 은신 해제가 참이면 은신 해제;
	// 버프 스킬인 경우 패스;
	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_SKILL) && !pSkill->IsBuff() )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	if ( !GLCHARLOGIC::CHECKPROBABILITYSKILL(pSkill, wLevel) )
	{
		GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
		NetMsgFB.emSKILL_FB = EMSKILL_FAILED;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 부활 스킬일때, 부활이 금지된 맵이라면 부활 시키지 않는다;
	if ( pSkill->IsRebirth() )
	{
        const MapID& _mapIDCurrent(GetCurrentMap());
        const SNATIVEID& _baseMapID(_mapIDCurrent.getBaseMapID());
		const SNATIVEID& _gaeaMapID(_mapIDCurrent.getGaeaMapID());
		const SMAPNODE* pMapNode = m_pGaeaServer->FindMapNode( _baseMapID );
        if ( pMapNode == NULL )
        {
			sc::writeLogError(sc::string::format("can not find MapNode! currentMap ID : %1%/%2%, base ID : %3%/%4%", _gaeaMapID.wMainID, _gaeaMapID.wSubID, _baseMapID.wMainID, _baseMapID.wSubID));
            GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
            NetMsgFB.emSKILL_FB = EMSKILL_REBIRTHBLOCK;
            SendToClient( &NetMsgFB );
            return E_FAIL;
        }
		if ( pMapNode->IsRebirthBlock() == true )
		{
			GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
			NetMsgFB.emSKILL_FB = EMSKILL_REBIRTHBLOCK;
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}
	}

	// 선도전 중에 인증중인 캐릭터는 스킬을 사용할 수 없다;
	if ( pLand->m_bGuidBattleMap && 
		m_pGaeaServer->GuidanceCheckCertify( pLand->m_dwClubMapID, m_CharDbNum ) == EMCHECKCERTIFY_DOING )
	{
		GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
		NetMsgFB.emSKILL_FB = EMSKILL_FAILED;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

    // 스크립트가 있을 경우 스크립트에서 제어할 수 있도록 함;
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )
    {
        // 만약 Event Resurrect 스크립트가 있다면 스크립트의 처리를 따름;
        if ( _pInstance->EventSkill(CROW_PC, GetCharID()) == false )
        {
            GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
            NetMsgFB.emSKILL_FB = EMSKILL_FAILED;
            SendToClient( &NetMsgFB );
            return E_FAIL;
        }
    }	

	// SKILL 구동.
	SETACTIVESKILL ( skill_id );
	SetDefenseSkill( pNetMsg->bDefenseSkill );	

	if ( pNetMsg->bDefenseSkill )
		m_bDefenseSkill = false;

	const SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
	m_SKILLMTYPE = sEXT_DATA.emANIMTYPE;
	m_SKILLSTYPE = sEXT_DATA.emANISTYPE;

	SCHARSKILL				sSkillLevel;
	if( GetSkillLevel( skill_id, sSkillLevel ) )
	{
		if( pNetMsg->btSECONDTARNUM > 0 )
		{
			memcpy( m_sTARIDS + m_wTARNUM, pNetMsg->sTARIDS + pNetMsg->btTARNUM, sizeof( STARID ) * pNetMsg->btSECONDTARNUM );
			m_sTARIDS[m_wTARNUM + pNetMsg->btSECONDTARNUM].dwID = 0xFFFFFFFF;			//end mask
		} //if

		m_WorkingSkill.Start(
			sSkillLevel,
			this,
			emCHECK == EMSKILL_NOTSP,
			&m_TargetID,
			m_wTARNUM,
			m_sTARIDS,
			false );
	} //if

	if( isAction(GLAT_IDLE) || 
		!(pSkill->m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST))
	{ // 러닝캐스팅거나 Idle이면 액션변경 없이 스킬적용.
		TurnAction ( GLAT_SKILL );	
	} //if

	// 스킬이 정상적으로 발동되면 딜래이를 초기화
	m_fSkillDelay = 0.0f;	
	
	// SKILL 구동 성공시 - 자신의 주변 Char에게 자신의 Msg를 전달.
	SendMsgSkillSucessResult( pNetMsg->skill_id, wLevel, pNetMsg->vTargetPosition, m_TargetID );

    DurabilityDropUseSkill();
	DisableSkillFact();

	return S_OK;
}

HRESULT GLChar::MsgReqSkillHoldReset ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SKILLHOLD_RS*		pNetMsg = (GLMSG::SNETPC_REQ_SKILLHOLD_RS*)nmg;

	SNATIVEID sNativeID = pNetMsg->sSKILLID;
	WORD      wSlot     = pNetMsg->wRESETSLOT;

	if( wSlot >= SKILLFACT_SIZE )
		return E_FAIL;

	if( m_sSKILLFACT[wSlot].sNATIVEID != sNativeID )
		return E_FAIL;

	// 스킬효과 해제요청은 버프의 경우에만 가능하다 [8/22/2014 hsshin]
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( pSkill == NULL )
		return E_FAIL;
	if ( pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_BUFF )
		return E_FAIL;

	// 현재 구조 상 깔끔하게 지속형 스킬에 대한 종료 처리를 할 수 없음,
	// 해서 아래와 같은 식으로 처리하고는 있으나 반드시 바꿔야 함;
	// 기능(스킬) 별로 모듈화 하여 인터페이스로 진입, 진행, 종료, 갱신, 검사등을 구현하는 식으로;
	if ( m_sSKILLFACT[wSlot].IsHaveSPECADDON(EMSPECA_INVISIBLE) )
	{
		if ( m_sINVISIBLE.bPrevStateRun )
			SetActState(EM_ACT_RUN);
		else
			ReSetActState(EM_ACT_RUN);
	}

	UPDATE_DATA_END(wSlot, FALSE, FALSE, TRUE );	

	// 해당 유저에게 알림
	GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB		NetMsgFB;
	NetMsgFB.emCrow     	= CROW_PC;
	NetMsgFB.dwID       	= m_dwGaeaID;
	NetMsgFB.sSKILLID   	= sNativeID;
	NetMsgFB.wRESETSLOT 	= wSlot;
	SendToClient( &NetMsgFB );

	// 주변에게 알림
	GLMSG::SNETPC_SKILLHOLD_RS_BRD			NetMsgBRD;
	NetMsgBRD.emCrow        = CROW_PC;
	NetMsgBRD.dwID          = m_dwGaeaID;
	NetMsgBRD.emResetType   = EMSKILLHOLDRESET_REQ;
	NetMsgBRD.bRESET[wSlot] = true;
	SendMsgViewAround( &NetMsgBRD );

	return S_OK;
}

HRESULT GLChar::MsgReqSkillCancel( NET_MSG_GENERIC* nmg )
{
	//CONSOLEMSG_WRITE( "MsgReqSkillCancel" );
	// 액션 취소.
	if( isAction(GLAT_SKILL) )
		TurnAction ( GLAT_IDLE );

	// 자신의 주변 Char에게 자신의 Msg를 전달.
	GLMSG::SNETPC_SKILL_CANCEL_BRD		NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround( &NetMsgBRD );

	return S_OK;
} //GLChar::MsgReqSkillCancel

HRESULT GLChar::MsgReqSkillQSet ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SKILLQUICK_SET *pNetMsg = (GLMSG::SNETPC_REQ_SKILLQUICK_SET *) nmg;

	if ( EMSKILLQUICK_SIZE <= pNetMsg->wSLOT )
		return E_FAIL;

	//	Note : 배운 스킬이 아닐 경우 취소됨.
	if ( !ISLEARNED_SKILL(pNetMsg->skill_id) )
		return E_FAIL;

	//	Note : 슬롯에 넣어줌.
	m_sSKILLQUICK[pNetMsg->wSLOT] = pNetMsg->skill_id;

	//	Note : 클라이언트에 통보.
	GLMSG::SNETPC_REQ_SKILLQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = pNetMsg->wSLOT;
	NetMsgFB.skill_id = m_sSKILLQUICK[pNetMsg->wSLOT];
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqSkillQReSet ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SKILLQUICK_RESET *pNetMsg = (GLMSG::SNETPC_REQ_SKILLQUICK_RESET *) nmg;

	if ( EMSKILLQUICK_SIZE <= pNetMsg->wSLOT )
		return E_FAIL;

	//	Note : 슬롯에 넣어줌.
	m_sSKILLQUICK[pNetMsg->wSLOT] = NATIVEID_NULL();

	//	Note : 클라이언트에 통보.
	GLMSG::SNETPC_REQ_SKILLQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = pNetMsg->wSLOT;
	NetMsgFB.skill_id = m_sSKILLQUICK[pNetMsg->wSLOT];
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqSkillQSetActive(WORD Slot)
{
	//GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE* pNetMsg = (GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE*) nmg;
	if (EMSKILLQUICK_SIZE <= Slot)
        return E_FAIL;

	//	Note : 액티브된 슬롯 스킬을 지정.
	m_wSKILLQUICK_ACT = Slot;
	return S_OK;
}

void GLChar::CheckOneSec(__time64_t CurrentTime)
{
    if (CurrentTime != m_CurrentTime)
    {
        m_CurrentTime = CurrentTime;

        // Cool Time check
        for (COOLTIME_MAP_ITER iter=m_mapCoolTimeID.begin(); iter!=m_mapCoolTimeID.end(); )
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            if (CoolTime.tCoolTime < CurrentTime)
                iter = m_mapCoolTimeID.erase(iter);
            else
                ++iter;
        }

        for (COOLTIME_MAP_ITER iter=m_mapCoolTimeType.begin(); iter!=m_mapCoolTimeType.end(); )
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            if (CoolTime.tCoolTime < CurrentTime)
                iter = m_mapCoolTimeType.erase(iter);
            else
                ++iter;
        }        
    }
}

HRESULT GLChar::MsgReqResetSkillStatsBySkillUI( NET_MSG_GENERIC* nmg )
{
	WORD wResetStats[EMSIZE];
	memset( wResetStats, 0, sizeof( WORD ) * EMSIZE );
	WORD* pResetStats = wResetStats;
	WORD wAmount = USHRT_MAX;

	GLMSG::SNET_INVEN_RESET_SKST_FB	MsgFB;

	// Note : 균등하게 감소된 수치를 가져온다.
	GET_EVENLY_DECREASE_STATS( pResetStats, wAmount );

	// 스킬, 스텟 리셋.
	//
	WORD wPREREMAINDSKILL = ( WORD ) m_dwSkillPoint;

	// Note : 전체 스킬 스텟 초기화
	RESET_STATS_SKILL( NULL );
	MsgFB.bRESETALL = TRUE;

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
	MsgFB.emFB = EMREQ_RESET_SKST_FB_OK2;
	MsgFB.llGameMoney = 0;
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
