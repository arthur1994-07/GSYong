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

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL(sSKILL_ID) )
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
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

	//	��ų ��� ����.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	SendToClient( &NetMsgFB );

	//	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_SKILLBOOK) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	//	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
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
		//	���������� �߻� �� �� ���� ��Ȳ.
		return E_FAIL;
	}

	emSKILL_LVLUPCHECK = CHECKLEARNABLE_SKILL(pNetMsg->skill_id);
	if ( emSKILL_LVLUPCHECK!=EMSKILL_LEARN_OK )
	{
		//	���� ������ �����մϴ�. FB �޽���.
		GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
		NetMsgFB.sSkill.sNativeID = pNetMsg->skill_id;
		NetMsgFB.emCHECK = emSKILL_LVLUPCHECK;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	Note : ��ų LEVEL UP.
	//
	WORD wToLevel = pCHARSKILL->wLevel + 1;
	LVLUP_SKILL ( pNetMsg->skill_id, wToLevel );

	//	����. FB �޽���.
	GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
	
	NetMsgFB.sSkill.sNativeID = pNetMsg->skill_id;
	NetMsgFB.sSkill.wLevel = wToLevel;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;

	SendToClient( &NetMsgFB );

	//	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	SendToClient( &NetMsgSkp );

	//	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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
        EMSKILL_LEARN_OK		= 0,	//	���� ����.
        EMSKILL_LEARN_MAX		= 1,	//	�� �̻� ������ �� �� ����.
        EMSKILL_LEARN_NOTTERM	= 2,	//	�䱸 ������ �������� �ʾƼ� ������ �Ҽ� ����.
        EMSKILL_LEARN_NOTCLASS	= 3,	//	�μ��� Ʋ���� ���� �����ϴ�.
        EMSKILL_LEARN_NOTBRIGHT	= 4,	//	��/�� ������ �������� �ʽ��ϴ�.
        EMSKILL_LEARN_ALREADY	= 5,	//	�̹� ��� ��ų�Դϴ�.
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
                //	���� ������ �����մϴ�. FB �޽���.
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
            // ���� ��ų ����
            if ( pCHARSKILL == NULL ) 
            {
                bool bUseScrool = GLogicData::GetInstance().GetRnSkillUseScrool( sSkillID );
                if( bUseScrool ) // ������ ��ų ��ũ���� ���ؼ� ��ų�� ���� �� �� �ִ�.
                {
                    
                    continue;
                }
                else            // �����ε� ��ų�� ��� �� �ִ�.
                {
                    // Note : ��ũ�� ��뿩�ο� ���Ҹ��� ������ �´�.
					// m_pSkill->m_sLEARN.sLVL_STEP[nLevel].dwReqMoney 
					
                    LONGLONG llUseMoney = pSkill->m_sLEARN.sLVL_STEP[0].dwReqMoney;//GLogicData::GetInstance().GetRnSkillUseMoney( sSkillID );

                    if( GetInvenMoney() < llUseMoney )
                       continue;

                    //  Note : ��� �Ҹ�
                    SubtractInvenMoneyServer( llUseMoney, SUBTRACT_INVEN_MONEY_SKILL_LEARN );
					// ��� �� �α� ����
                    LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llUseMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_LEARN_SKILL);
					// ��� �� �α� ����
                    LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
                    SendInvenMoneyUpdate();
                }
                                     
                //	Note : ��ų ������.
                LEARN_SKILL(sSkillID);

                //	Note : ��ų �������� ���� ����Ʈ ���� ����.
                QuestStartFromGetSKILL ( sSkillID );

                //	��ų ��� ����.
                GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
                NetMsgFB.skill_id = sSkillID;
                NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
                SendToClient( &NetMsgFB );

				dwCompleteType |= GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEARN;

                //	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
                GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
                NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
                SendToClient( &NetMsgSkp );

                if ( GLHIDESET::CheckHideSet(EMHIDE_USE_SKILLBOOK) )
				{
					RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					RestoreActStateByInvisible();
				}

                //	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
                if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
                {
                    GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = m_dwGaeaID;
                    NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
                    SendMsgViewAround(&NetMsgBrd);
                }
            }
            else // ��ų ������
            {
				//	Note : ��ų LEVEL UP.
				WORD wToLevel = pCHARSKILL->wLevel + 1;

				// �䱸 �� üũ;
				LONGLONG llReqMoney = pSkill->m_sLEARN.sLVL_STEP[wToLevel].dwReqMoney;
				if( GetInvenMoney() < llReqMoney )
					continue;

				//  Note : ��� �Ҹ�
				SubtractInvenMoneyServer( llReqMoney, SUBTRACT_INVEN_MONEY_SKILL_LEARN );
				// ��� �� �α� ����
				LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -llReqMoney, ITEM_ROUTE_GROUND, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_LEARN_SKILL);
				// ��� �� �α� ����
				LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
				SendInvenMoneyUpdate();

                LVLUP_SKILL ( sSkillID, wToLevel );

                //	����. FB �޽���.
                GLMSG::SNETPC_REQ_SKILLUP_FB	NetMsgFB;
                NetMsgFB.sSkill.sNativeID = sSkillID;
                NetMsgFB.sSkill.wLevel = wToLevel;
                NetMsgFB.emCHECK = EMSKILL_LEARN_OK;

                SendToClient( &NetMsgFB );

                //	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
                GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
                NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
                SendToClient( &NetMsgSkp );

				dwCompleteType |= GLMSG::SNETPC_REQ_RNSKILLUP_COMPLETE::FLG_LEVELUP;

                //	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
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

	// ��ų ������ �Ϸ�
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
        // ������ üũ
        if ( m_fSkillDelay < 0.3f )
            return E_FAIL;  // 05.11.30  0.5f -> 0.3f
    }
    //#endif	

    // Ÿ�� ������ ���� ��ȿ�� �˻�; Ŭ���̾�Ʈ ������ ���� ����;
    // Ÿ���� �Ÿ� ��ȿ�� �˻�; Ŭ���̾�Ʈ ������ ���� ����;		
    // ���� �ڵ忡�� �Ʊ��� ��� �˻縦 ���� �ʾ���; -> ���� ����;
    // ������ ��� ���� ������ ���� ���� �˻�;
    if( ServiceProvider != SP_KOREA && ServiceProvider != SP_KOREA_TEST && ServiceProvider != SP_JAPAN )
    { // pNetMsg �ȿ� ��� TARIDS�� ���۷����� �Ѱ� ��ȿ�� �˻��� �� ����;
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
        //	Note : ��ȿ�� �˻�.
        //		
        // skill ������ ������ Ÿ�������� �˻�;
        // ���� �߸����� ����� ���� ���� �����Ѱ����� ����;
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
            // skill ������ �����ѵ� �߸�������̶� ���� ������� �����Ǿ��� ���;
            // 
            // �� Ÿ�� �ܿ� Ÿ���� ���� ���� �Ұ��ϴ�.;
            // PC�� ��� Client���� Ÿ�ټ����� ó���Ͽ� ������ ������ �����µ�
            // ��Ÿ�ٿܿ� �������� Ÿ���� �ִٴ� ���� ��Ŷ ������ ���ɼ��� �ִ�.;
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
		// ��ų ���ϸ��̼ǿ� Ÿ�� ������ �������� �ʾҽ��ϴ�
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
		//	Note : ��ų ���� ���� FB �޽���.
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
		return E_FAIL; // ���� ��� �߿��� �̵� �ܿ� ��� �ൿ�� �� �� ����;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;	

	GLMSG::SNETPC_REQ_SKILL *pNetMsg = (GLMSG::SNETPC_REQ_SKILL *) nmg;
	SNATIVEID				skill_id = pNetMsg->skill_id;

	// �ߵ� ��ų�ϰ�� ���� �ߵ� ��ų���� �˻�
	if ( pNetMsg->bDefenseSkill )
	{
		if (pNetMsg->skill_id != m_sDefenseSkill.m_dwSkillID)
            return E_FAIL;

		if (!m_bDefenseSkill)
            return E_FAIL;
	} //if

	// ��� ������ ����Ÿ���̸� ���� ����
	if( m_sCONFTING.IsPOWERFULTIME () ) 
		return E_FAIL;

	//	��ų ���� ������.
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

	// Ÿ�� ����.
	if( pNetMsg->btTARNUM )
	{
		m_TargetID.vPos   = pNetMsg->vTargetPosition;
		m_TargetID.emCrow = pNetMsg->sTARIDS[0].GETCROW();
		m_TargetID.GaeaId = pNetMsg->sTARIDS[0].GETID();
	} //if

	// ��ü Ÿ�� ����
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

	// Ÿ���� ��ȿ�� ����.
	if( MsgReqSkill_Targets( pNetMsg, pSkill, wLevel, wSkillRange, m_wTARNUM, m_sTARIDS ) == E_FAIL )
		return E_FAIL;
	
	// Note : ���� ��ġ ��ų�� �ƴҶ� Ÿ���� ������ ���� �Ұ���
	if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_LAND && m_wTARNUM == 0 )
	{
		// �ڽſ���.
		GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
		NetMsg.emTarCrow		= m_TargetID.emCrow;
		NetMsg.dwTarID			= m_TargetID.GaeaId;
		NetMsg.nDamage			= 0;
        NetMsg.wSchool          = GetSchool();
		SendToClient( &NetMsg );

		return E_FAIL;
	}

	// ��ų ���� ���� �˻�.
	DWORD				dwSkillCheckOption = 0;
	if( pNetMsg->bDefenseSkill )
		dwSkillCheckOption |= EMSKILLCHECKOPTION_DEFENSESKILL;

	EMSKILLCHECK		emCHECK = GLCHARLOGIC::CHECHSKILL( pNetMsg->skill_id, 1, m_emVehicle, dwSkillCheckOption );
	if( MsgReqSkill_CheckToStartSkill( pNetMsg, pSkill, emCHECK ) != S_OK )
		return E_FAIL;

	// HideSet Table Ȯ�� �� ���� ������ ���̸� ���� ����;
	// ���� ��ų�� ��� �н�;
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

	// ��Ȱ ��ų�϶�, ��Ȱ�� ������ ���̶�� ��Ȱ ��Ű�� �ʴ´�;
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

	// ������ �߿� �������� ĳ���ʹ� ��ų�� ����� �� ����;
	if ( pLand->m_bGuidBattleMap && 
		m_pGaeaServer->GuidanceCheckCertify( pLand->m_dwClubMapID, m_CharDbNum ) == EMCHECKCERTIFY_DOING )
	{
		GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
		NetMsgFB.emSKILL_FB = EMSKILL_FAILED;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

    // ��ũ��Ʈ�� ���� ��� ��ũ��Ʈ���� ������ �� �ֵ��� ��;
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )
    {
        // ���� Event Resurrect ��ũ��Ʈ�� �ִٸ� ��ũ��Ʈ�� ó���� ����;
        if ( _pInstance->EventSkill(CROW_PC, GetCharID()) == false )
        {
            GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
            NetMsgFB.emSKILL_FB = EMSKILL_FAILED;
            SendToClient( &NetMsgFB );
            return E_FAIL;
        }
    }	

	// SKILL ����.
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
	{ // ����ĳ���ðų� Idle�̸� �׼Ǻ��� ���� ��ų����.
		TurnAction ( GLAT_SKILL );	
	} //if

	// ��ų�� ���������� �ߵ��Ǹ� �����̸� �ʱ�ȭ
	m_fSkillDelay = 0.0f;	
	
	// SKILL ���� ������ - �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
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

	// ��ųȿ�� ������û�� ������ ��쿡�� �����ϴ� [8/22/2014 hsshin]
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( pSkill == NULL )
		return E_FAIL;
	if ( pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_BUFF )
		return E_FAIL;

	// ���� ���� �� ����ϰ� ������ ��ų�� ���� ���� ó���� �� �� ����,
	// �ؼ� �Ʒ��� ���� ������ ó���ϰ�� ������ �ݵ�� �ٲ�� ��;
	// ���(��ų) ���� ���ȭ �Ͽ� �������̽��� ����, ����, ����, ����, �˻���� �����ϴ� ������;
	if ( m_sSKILLFACT[wSlot].IsHaveSPECADDON(EMSPECA_INVISIBLE) )
	{
		if ( m_sINVISIBLE.bPrevStateRun )
			SetActState(EM_ACT_RUN);
		else
			ReSetActState(EM_ACT_RUN);
	}

	UPDATE_DATA_END(wSlot, FALSE, FALSE, TRUE );	

	// �ش� �������� �˸�
	GLMSG::SNETPC_REQ_SKILLHOLD_RS_FB		NetMsgFB;
	NetMsgFB.emCrow     	= CROW_PC;
	NetMsgFB.dwID       	= m_dwGaeaID;
	NetMsgFB.sSKILLID   	= sNativeID;
	NetMsgFB.wRESETSLOT 	= wSlot;
	SendToClient( &NetMsgFB );

	// �ֺ����� �˸�
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
	// �׼� ���.
	if( isAction(GLAT_SKILL) )
		TurnAction ( GLAT_IDLE );

	// �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
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

	//	Note : ��� ��ų�� �ƴ� ��� ��ҵ�.
	if ( !ISLEARNED_SKILL(pNetMsg->skill_id) )
		return E_FAIL;

	//	Note : ���Կ� �־���.
	m_sSKILLQUICK[pNetMsg->wSLOT] = pNetMsg->skill_id;

	//	Note : Ŭ���̾�Ʈ�� �뺸.
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

	//	Note : ���Կ� �־���.
	m_sSKILLQUICK[pNetMsg->wSLOT] = NATIVEID_NULL();

	//	Note : Ŭ���̾�Ʈ�� �뺸.
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

	//	Note : ��Ƽ��� ���� ��ų�� ����.
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

	// Note : �յ��ϰ� ���ҵ� ��ġ�� �����´�.
	GET_EVENLY_DECREASE_STATS( pResetStats, wAmount );

	// ��ų, ���� ����.
	//
	WORD wPREREMAINDSKILL = ( WORD ) m_dwSkillPoint;

	// Note : ��ü ��ų ���� �ʱ�ȭ
	RESET_STATS_SKILL( NULL );
	MsgFB.bRESETALL = TRUE;

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
