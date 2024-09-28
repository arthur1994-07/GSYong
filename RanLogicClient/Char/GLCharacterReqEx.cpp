#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../Enginelib/GUInterface/GameTextControl.h"
#include "../../Enginelib/GUInterface/UITextControl.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Summon/GLSummonClient.h"
#include "../Pet/GLPetClient.h"
#include "../GLGaeaClient.h"
#include "../StaticHoldItem.h"
#include "./GLCharacter.h"
#include "../../RanLogic/Prison/LimitPrison.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


bool GLCharacter::ReqInvenDrugDrug( EMITEM_DRUG DrugType )
{
    switch (DrugType)
    {
    case ITEM_DRUG_HP:
        {
            // ������ �ǰ� �ִ�ġ�� �ƴϸ� ��� ����
            if (m_sHP.nMax != m_sHP.nNow)
                return false;

            // ��ȯ���� �ǰ� �ִ�ġ�� �ƴϸ� ��� ����.
            //
            if ( !m_dwSummonGUIDList.empty() )
            {                
                SUMMONID_ITER SummonIter    = m_dwSummonGUIDList.begin();
                SUMMONID_ITER SummonIterEnd = m_dwSummonGUIDList.end();

                for ( ; SummonIter!=SummonIterEnd; ++SummonIter )
                {
                    std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon((*SummonIter));
                    if (!spSummon || !spSummon->IsValidBody())
                        continue;
                    if (spSummon->GETMAXHP() != spSummon->GETHP())
                        return false;
                }
            }
            return true;
        }
        break;

    case ITEM_DRUG_MP:
        if ( m_sMP.nMax == m_sMP.nNow )
            return true;
        break;

    case ITEM_DRUG_SP:
        if ( m_sSP.nMax == m_sSP.nNow )
            return true;
        break;

    case ITEM_DRUG_CP:
        if ( m_sCP.wMax == m_sCP.wNow )
            return true;
        break;

    case ITEM_DRUG_HP_MP:
        if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nNow == m_sMP.nNow) )
            return true;
        break;
    case ITEM_DRUG_MP_SP:
        if ( (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nNow == m_sSP.nNow) )
            return true;
        break;

    case ITEM_DRUG_HP_MP_SP:
        if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nMax == m_sSP.nNow) )
            return true;
        break;

    case ITEM_DRUG_CURE:
        if (!ISSTATEBLOW())
            return true;
        break;

    case ITEM_DRUG_HP_CURE:
        if (m_sHP.nMax == m_sHP.nNow && !ISSTATEBLOW())
            return true;
        break;

    case ITEM_DRUG_HP_MP_SP_CURE:
        if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nMax == m_sSP.nNow) && !ISSTATEBLOW() )
            return true;
        break;
	case ITEM_DRUG_TENDENCY_RECOVERY :
		{
			int iBrightMax = GLCONST_CHAR::nBrightMAX + 1;

			if( m_nBright >= iBrightMax )
			{
				return true;
			}
		}
		//if m_nBright
		break;
	case ITEM_DRUG_EFFECT_RELEASE :
		{
			
		}
		break;
	case ITEM_DRUG_MACROTIME:
		{
			return false;
		}
		break;
    default:
        break;
    };

    return false;
}


void GLCharacter::ReqInvenDrugNotHoldItem( const SITEM* pItem, WORD wPosX, WORD wPosY, const SINVENITEM* pInvenItem )
{
	PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();

	if( pLandManClient ) {
		if( PRISONLIMIT.IsLimitStatus(m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID() ), UserLevel(), PML_ITEM) )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PRISION_LIMIT") );
			return;
		}
	}
		
    if ( pItem->IsSuitable() )
    {
        ReqInvenToSlot(wPosX, wPosY);
        return;
    }

    EMITEM_TYPE BasicType = pItem->BasicType();

    switch (BasicType)
    {	
	case ITEM_EFFECT_RELEASE_CURE :
    case ITEM_CURE:
        ReqInvenCure(wPosX, wPosY);
        break;

    case ITEM_SKILL:
        ReqInvenSkill(wPosX, wPosY);
        break;

    case ITEM_RECALL:
        ReqReCall(wPosX, wPosY);
        break;
    
    case ITEM_TELEPORT_CARD:
        ReqTeleport(wPosX, wPosY);
        break;

    case ITEM_BOX:
        ReqBoxOpen(wPosX, wPosY);
        break;
        
    case ITEM_RANDOMITEM:
        ReqRandumBoxOpen(wPosX, wPosY);
        break;

	case ITEM_LOCK_BOX:
		ReqLockBoxOpen(wPosX, wPosY);
		break;

	case ITEM_SELECTIVEFORM_BOX:
		SelectiveformBoxOpen(wPosX, wPosY);
		break;

    case ITEM_CHARACTER_CARD:
        ReqCharCard(wPosX, wPosY);
        break;

    case ITEM_STORAGE_CARD:
        //! �������̽��� ������ â�� �����ϰ�.
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_LockerExpand_Open,
			"-n, -n", wPosX, wPosY );
        break;

    case ITEM_INVEN_CARD:
        ReqInvenLineCard(wPosX, wPosY);
        break;

    case ITEM_STORAGE_CONNECT:
        ReqStorageOpenCard(wPosX, wPosY);
        break;
    
    case ITEM_REMODEL:
        ReqRemodelOpenCard(wPosX, wPosY);
        break;

    case ITEM_GARBAGE_CARD:
        ReqGabargeOpenCard(wPosX, wPosY);
        break;

    case ITEM_PREMIUMSET:
        ReqPremiumSet(wPosX, wPosY);
        break;
    
    case ITEM_SKP_STAT_RESET_A:
        ResetSkillStats(wPosX, wPosY);
        break;
    
    case ITEM_SKP_STAT_RESET_B:
        ResetSkillStats(wPosX, wPosY);
        break;

    case ITEM_POSTBOX_CARD:
        ReqPostBoxOpenCard(wPosX, wPosY);
        break;

    case ITEM_PRIVATEMARKET:
        ReqInvenDrugPrivateMarket(wPosX, wPosY);
        break;

    case ITEM_HAIR:
        ReqInvenHairChange(wPosX, wPosY);
        break;

    case ITEM_HAIR_COLOR:
        InvenHairColorChange(wPosX, wPosY);
        break;

    case ITEM_HAIR_STYLE:
        InvenHairStyleChange(wPosX, wPosY);
        break;

    case ITEM_FACE:
        ReqInvenFaceChange( wPosX, wPosY );
        break;
        
    case ITEM_FACE_STYLE:
        InvenFaceStyleChange( wPosX, wPosY );
        break;

    case ITEM_TAXI_CARD:
        InvenUseTaxiCard( wPosX, wPosY );
        break;

    case ITEM_NPC_RECALL:
        InvenUseNpcRecall( wPosX, wPosY );
        break;

    case ITEM_GENDER_CHANGE:
        InvenGenderChange(wPosX, wPosY);
        break;

    case ITEM_RENAME:
        InvenRename(wPosX, wPosY);
        break;

    case ITEM_VIETNAM_EXPGET:
        ReqInvenVietnamGet ( wPosX, wPosY, TRUE );
        break;

    case ITEM_VIETNAM_ITEMGET:
        ReqInvenVietnamGet ( wPosX, wPosY, FALSE );
        break;

    case ITEM_PET_RENAME:
        m_pGaeaClient->GetPetClient()->ReqInputName(wPosX, wPosY);
        break;
    
    case ITEM_PET_COLOR:
        m_pGaeaClient->GetPetClient()->ReqInputColor(wPosX, wPosY);
        break;

    case ITEM_PET_STYLE:
        m_pGaeaClient->GetPetClient()->ReqInputStyle(wPosX, wPosY);
        break;

    case ITEM_PET_SKILL:
        m_pGaeaClient->GetPetClient()->ReqLearnSkill(wPosX, wPosY);
        break;

    case ITEM_VEHICLE_COLOR:
        InvenVehicleColorChange(wPosX, wPosY);
        break;

    case ITEM_BIKE_BOOST_CARD:
        ReqInvenBikeBoost(wPosX, wPosY);
        break;

    case ITEM_PET_CARD:
        ReqInvenDrugPetCard(pInvenItem, wPosX, wPosY);
        break;

    case ITEM_LUNCHBOX:
        ReqInvenDrugLunchBox(wPosX, wPosY);
        break;
    
    case ITEM_PET_DUALSKILL:
        m_pGaeaClient->GetPetClient()->ReqLearnDualSkill(wPosX, wPosY);
        break;

    case ITEM_POINT_CARD_REFUND: // Point Card
    case ITEM_POINT_CARD_NOT_REFUND:
        m_pGaeaClient->ReqInvenDrugPointCard(wPosX, wPosY);
        break;

	case ITEM_PRODUCT_BOOK:
        m_pGaeaClient->GetProductClient()->ReqLearnProductBook( wPosX, wPosY );
		break;

	case ITEM_PRODUCT_RESET:
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ProductReset_Open, 
			"-dw, -dw, -dw",
			pItem->sDrugOp.GetCureVolumn(),
			wPosX, wPosY );
		break;

    case ITEM_REPAIR_CARD:
		ReqItemRepairCardUse( wPosX, wPosY );
        break;

    case ITEM_PMARKET_SEARCH_CARD:
        ReqPMarketSearchOpen(wPosX,wPosY);
        break;

	case ITEM_STAGE_PASS:
		ReqStagePass(wPosX, wPosY);
		break;
	case ITEM_ENTER_NUM_CHARGE:
		ReqEnterNumCharge(wPosX, wPosY);
		break;

	case ITEM_CARDGAME_CARD:
		m_pGaeaClient->GetTexasHoldemManager()->RequestInvenDrugCardGameCard(wPosX, wPosY);
		break;

    case ITEM_CONSIGMENT_SALE_EXTEND_CARD:
        ReqConsignmentSlotExpiration(wPosX, wPosY);
        break;

    case ITEM_WRAPPED_BOX:
        ReqBoxUnwrapping(wPosX, wPosY);
        break;

	case ITEM_INITIALIZE_COUNTRY:
		ReqInvenChangeCountry(wPosX, wPosY);
		break;

	case ITEM_SCHOOL_CHANGE_0:
	case ITEM_SCHOOL_CHANGE_1:
	case ITEM_SCHOOL_CHANGE_2:
		ReqInvenSchoolChange(wPosX, wPosY);
		break;
	case ITEM_EXP_CARD:
		ReqJumpingLevelUp(wPosX, wPosY);
		break;
	case ITEM_EXP_COMPERSSOR:
		ReqInvenExpCompressor(wPosX, wPosY);
		break;
	case ITEM_EXP_CAPSULE:
		ReqInvenExpCapsule(wPosX, wPosY);
		break;
	case ITEM_CLUBRENAME:
		InvenClubRename( wPosX, wPosY );
		break;
	case ITEM_CHANGE_ITEMSKILL_CARD:
		ReqInvenChangeItemSkill( wPosX, wPosY );
		break;
	case ITEM_CHANGE_BASICSTATS_CARD:
		ReqInvenChangeBasicStats( wPosX, wPosY );
		break;
	case ITEM_CHANGE_ADDSTATS_CARD:
		ReqInvenChangeAddStats( wPosX, wPosY );
		break;
	case ITEM_UNLOCK_CHARACTER_SLOT:
		ReqInvenUnlockCharacterSlot( wPosX, wPosY );
		break;
    default:
        break;
    }
}

void GLCharacter::ReqInvenDrugPrivateMarket( WORD wPosX, WORD wPosY )
{
    // �������� ������ ���� ���� �Ұ�
    if ( !isAction ( GLAT_IDLE ) || !m_ReservationAction.IsEmpty() || m_emVehicle )
    {
        m_pGaeaClient->PrintMsgTextDlg ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_NOIDLE") );				
    }
    else
    {
        m_wPMPosX = (wPosX);
        m_wPMPosY = (wPosY);

		//! �������̽��� ���� ���� ����.
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_TitleMsgBox_Open );
    }
}

void GLCharacter::ReqInvenDrugPetCard( const SINVENITEM* pInvenItem, WORD wPosX, WORD wPosY )
{
    // ���� ��ȯ�Ǿ� ������
    if (m_pGaeaClient->GetPetClient()->IsVALID())
    {
        GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg(
            m_pGaeaClient->GetPetClient()->m_dwGUID,
            pInvenItem->sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
    else if (!m_pGaeaClient->GetPetClient()->IsReady())
    {
        if (S_OK == ReqUsePetCard(wPosX, wPosY))
        {
            m_wInvenPosX1 = wPosX;
            m_wInvenPosY1 = wPosY;
            m_pGaeaClient->GetPetClient()->SetReady( true );
        }
    }
    else
    {
    }
}

void GLCharacter::ReqInvenDrugLunchBox( WORD wPosX, WORD wPosY )
{
    WORD wSlot = 0;

    const SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( pInvenItem == 0 )
		return; // �κ��� �ش� ��ġ�� �������� ���� ���� ����;
    
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( pItem == 0 )
		return; // �κ��� ���� �ϴ� �������� ���� ���� �ʴ� ��������;
	
	if ( pItem->BasicType() != ITEM_LUNCHBOX )
		return; // ���ö��� �ƴ�;
	

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( pItem->sSkillBookOp.sSkill_ID );			
	if ( pSkill == NULL )
		return; // ���ö��� ����� ȿ��(��ų)�� ���� ���� ����;

	DWORD typeExpLunch(-1);
	if ( GLUseFeatures::GetInstance().IsUsingExtensionLunchSlot() == true )
	{
		if ( pSkill->HaveSpec(EMSPECA_EXP_RATE) )
			typeExpLunch = EMSPECA_EXP_RATE;
		else if ( pSkill->HaveSpec(EMSPECA_RANDOM_EXP_RATE) )
			typeExpLunch = EMSPECA_RANDOM_EXP_RATE;
	}
	else
	{
		if ( pSkill->HaveSpec(EMSPECA_EXP_RATE) )
			typeExpLunch = EMSPECA_EXP_RATE;
	}

	const EMSKILLFACT_TYPE emType(typeExpLunch == -1 ? EMSKILLFACT_TYPE_LUNCHBOX : EMSKILLFACT_TYPE_EXP_LUNCHBOX);
	if ( IS_REMAIND_SKILLFACTARRAY(emType) == true )
	{ // �ƹ��� ȿ���� �ɷ� ���� �ʴٸ� ��� ���� ��û;
		ReqInvenLunchBox(wPosX, wPosY);
	}
	else
	{ // �׷��� �ʴٸ� Ȯ�� UI�� ���;
		switch ( emType )
		{
		case EMSKILLFACT_TYPE_EXP_LUNCHBOX:
			{				
				const SNATIVEID& previousSkillID = GetSkillFactId(SKILLFACT_INDEX_EXP_LUNCHBOX_START);
				GLSKILL* pPreviousSkill = GLSkillMan::GetInstance().GetData(previousSkillID);
				// ������ �ɷ� �ִ� ȿ���� ��ȿ���� �ʴٸ� ���� ��û;
				if ( pPreviousSkill == NULL )
				{
					ReqInvenLunchBox(wPosX, wPosY);
				}
				// ������ �ɷ��ִ� ȿ���� �����ϴٸ� ��ø Ȯ�� UI�� ���;
				else if ( (previousSkillID == pItem->sSkillBookOp.sSkill_ID) && (typeExpLunch == EMSPECA_RANDOM_EXP_RATE) )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_Buff_LunchBoxOverlap,
						"-b, -s, -s, -n, -n",
						true, pPreviousSkill->GetName(), pSkill->GetName(), wPosX, wPosY );
				}
				// �� �� ���� ��𿡵� �������� �ʴ´ٸ� �Ʒ� ��ü Ȯ�� UI�� ���;
				else
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_Buff_LunchBoxOverlap,
						"-b, -s, -s, -n, -n",
						false, pPreviousSkill->GetName(), pSkill->GetName(), wPosX, wPosY );
				}
			}
			break;
		case EMSKILLFACT_TYPE_LUNCHBOX:			
			{
				const SNATIVEID& previousSkillID = GetSkillFactId(SKILLFACT_INDEX_LUNCHBOX_START);
				GLSKILL* pPreviousSkill = GLSkillMan::GetInstance().GetData(previousSkillID);
				// ������ �ɷ� �ִ� ȿ���� ��ȿ���� �ʴٸ� ���� ��û;
				if ( pPreviousSkill == NULL )
				{
					ReqInvenLunchBox(wPosX, wPosY);
				}
				// �� �� ���� ��𿡵� �������� �ʴ´ٸ� �Ʒ� ��ü Ȯ�� UI�� ���;
				else
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_Buff_LunchBoxOverlap,
						"-b, -s, -s, -n, -n",
						false, pPreviousSkill->GetName(), pSkill->GetName(), wPosX, wPosY );
				}
			}
			break;
		}
	}	
}

HRESULT GLCharacter::ReqInvenChangeCountry ( WORD wPosX, WORD wPosY )
{
	if ( NULL == m_pGaeaClient )
		return E_FAIL;

	// �κ��� �������� �ִ��� üũ;
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( NULL == pInvenItem )
		return E_FAIL;

	// ������ ���� ���� üũ;
	SITEM* pItem = GLogicData::GetInstance().GetItem(
		pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
		return E_FAIL;

	// �������� ���� �ʱ�ȭ ���������� üũ;
	if ( pItem->BasicType() != ITEM_INITIALIZE_COUNTRY )
		return E_FAIL;

	Country::SCOUNTRY_INFO sCountryInfo = m_pGaeaClient->GetCountryInfo();
	if( false == sCountryInfo.ISNONCOUNTRY() )
	{
		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_UseResetCountryCard,
		"-b, -n, -n", 
		sCountryInfo.ISNONCOUNTRY(),
		wPosX, wPosY );
}

ESlotHold GLCharacter::ReqInvenDrugHoldItem( const SITEM* pHold, const SITEM* pItem, WORD wPosX, WORD wPosY )
{
    EMITEM_TYPE BasicType = pHold->BasicType();
    switch (BasicType)
    {
	case ITEM_TEMP_ENCHANT :
		ReqTLGrinding(wPosX,wPosY);
		break;
    case ITEM_PET_REVIVE: // �ֺ�Ȱī�带 �ٸ����    
        ReqInvenDrugHoldPetRevive(pItem, wPosX, wPosY);
        break;

    case ITEM_GRINDING:
        ReqGrinding(wPosX, wPosY);
		return ESlotHold::Hold;

    case ITEM_CLEANSER:
        ReqCleanser(wPosX, wPosY);
        break;

    case ITEM_DISJUNCTION:
        ReqDisJunction(wPosX, wPosY);
        break;

    case ITEM_PET_SKIN_PACK:
        ReqPetSkinPackOpen(wPosX, wPosY);
        break;
        
    case ITEM_PET_FOOD:
        m_pGaeaClient->GetPetClient()->ReqGiveFood(wPosX, wPosY);
        break;

    case ITEM_VEHICLE_OIL:
        ReqVehicleGiveBattery(wPosX, wPosY);
        break;

    case ITEM_LOCK_BOX_KEY:
        ReqLockBoxUnlock(wPosX, wPosY);
        break;

    case ITEM_BOX_WRAPPER:
        ReqBoxWrapping(wPosX, wPosY);
        break;

	case ITEM_COSTUME_STATS :
		ReqCostumeStat(wPosX, wPosY);
		break;
	case ITEM_COSTUME_EFFECT_RELEASE : 
		ReqCostumeStatRelease(wPosX, wPosY);
		break;

	case ITEM_PERIODEXTANSION:
		ReqPeriodExtend(wPosX, wPosY);
		break;

    default:
        break;
    }

	return ESlotHold::Release;
}

void GLCharacter::ReqInvenDrugHoldPetRevive( const SITEM* pItem, WORD wPosX, WORD wPosY )
{
    if (pItem->sBasicOp.emItemType == ITEM_PET_CARD)
    {			
        m_wInvenPosX1 = wPosX;
        m_wInvenPosY1 = wPosY;

        m_sTempItemCustom = holditem::GetHoldItem();
        holditem::ReleaseHoldItem();

        // ���⼭ �ֺ�Ȱ�޽���â�� ����.
		//!! SF_TODO
        //if ( !m_pInterface->UiIsVisibleGroup( PET_REBIRTH_DIALOGUE ) ) // monster7j
        //{
        //    m_pInterface->UiShowGroupFocus( PET_REBIRTH_DIALOGUE );
        //}
    }
}

void GLCharacter::RequestConflict(lua_tinker::table _lua_table)
{
	SCONFT_OPTION sOption;
	sOption.bBOUND = _lua_table.get<bool>(1); 
	sOption.wRECOVER = GLCONST_CHAR::wCONFT_RC_TYPE[_lua_table.get<int>(2)]; 
	sOption.fHP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[_lua_table.get<int>(3)]; 
	sOption.fTAR_HP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[_lua_table.get<int>(4)]; // �̺�Ʈ��� �����ؼ� ������ ����
	m_pGaeaClient->GetCharacter()->ReqConflict ( m_PtoPTargetID, sOption ); // ������ ���� Ŭ�� ���� ������Ʈ ��û
}

void	GLCharacter::AnswerConflict(bool _Ans)
{
	if (_Ans)
	{
		//	Note : ��� �����ÿ� �޽��� ����.
		GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(m_emConflictReqType, m_ConflictReqID, EMCONFRONT_AGREE, m_sReqConflictOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);
	}
	else
	{
		//	Note : ��� �����ÿ� �޽��� ����.
		GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(m_emConflictReqType, m_ConflictReqID, EMCONFRONT_REFUSE, m_sReqConflictOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);
	}
}
