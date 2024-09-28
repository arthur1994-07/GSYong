#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../MfcExLib/RanFilter.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Trade/GLTradeClient.h"
#include "../Land/GLLandManClient.h"
#include "../StaticHoldItem.h"
#include "./GLPetClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLPetClient::ReqGoto ( D3DXVECTOR3 vTarPos, bool bToPickUp )
{
	D3DXVECTOR3 vPos(vTarPos);

	// �÷��� ����
	ReSetAllSTATE ();

	// ���� �������� �ֿ췯 ���ٸ�
	if ( bToPickUp )
	{
		if ( !m_pOwner || !m_pOwner->IsValidBody() )
		{
			return;
		}

		BOOL bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                vTarPos+D3DXVECTOR3(0,-10,0) );

		if ( !bGoto ) 
		{
			m_actorMove.Stop ();
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			SetSTATE ( EM_PETACT_STOP );
			m_sSkillTARGET.RESET ();
			return;
		}

		// �������� �ֿﶧ�� ������ �پ��.
		SetSTATE ( EM_PETACT_MOVE );
		SetSTATE ( EM_PETACT_RUN_PICKUP_ITEM );
		SetMoveState ( IsSTATE ( EM_PETACT_MOVE ) );
	
        GLMSG::SNETPET_REQ_GOTO NetMsg( m_dwGUID, m_vPos, vTarPos, m_dwActionFlag );
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		return;
	}

	// ������ �̵�Ÿ��
	STARGETID sTarget = m_pOwner->GetActionID ();

	// ���κ��� �ռ�������
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetPosition ();
	D3DXVECTOR3 vDist = m_vPos - sTarget.vPos;
	float fPetToTar = D3DXVec3Length(&vDist);
	vDist = vOwnerPos - sTarget.vPos;
	float fOwnerToTar = D3DXVec3Length(&vDist);

	vDist = vOwnerPos - m_vPos;
	float fDist = D3DXVec3Length(&vDist);

	// �̵����϶��� ��ٷ��ش�
	if ( fDist < GLCONST_PET::fWalkArea && m_pOwner->isAction ( GLAT_MOVE ) )
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		SetSTATE ( EM_PETACT_WAIT );

		GLMSG::SNETPET_REQ_STOP NetMsg(m_dwGUID, m_dwActionFlag, m_vPos, false );
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		return;
	}

	BOOL bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                vTarPos+D3DXVECTOR3(0,-10,0) );

	// Ȥ�� ������ ���̸�
	if ( !bGoto )
	{
		for ( WORD i = 0; i < 8; ++i )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::RANDPOS[i];

			vTarPos = vPos + vRandPos;
			bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                   vTarPos+D3DXVECTOR3(0,-10,0) );
			if ( bGoto ) break;
		}
	}

	if ( !bGoto ) 
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		SetSTATE ( EM_PETACT_STOP );
		return;
	}

	// [����] 
	// �̵��޽��� ���۽� �׻� ������ Run ���¸� ����� �Ѵ�.
	SetSTATE ( EM_PETACT_MOVE );
	SetMoveState ( m_pOwner->IsActState ( EM_ACT_RUN ) );

    GLMSG::SNETPET_REQ_GOTO NetMsg( m_dwGUID, m_vPos, vTarPos, m_dwActionFlag );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReqStop ( bool bStopAttack )
{
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	m_actorMove.Stop ();
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	m_sSkillTARGET.RESET ();

	GLMSG::SNETPET_REQ_STOP NetMsg(m_dwGUID, m_dwActionFlag, m_vPos, bStopAttack );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReqGiveFood( WORD wPosX, WORD wPosY )
{
	// ��Ȱ�� ���¿����� ��Ḧ �� �� �ִ�.
	//if ( !IsVALID() )							return;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	GLCharacter* pOwner = m_pGaeaClient->GetCharacter();
	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	// ��ī�� ���� üũ
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	// ��� ���� üũ
	if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	SPETCARDINFO sPet;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if (!pGlobalStage->IsEmulator())
	{
		// ��ī���� ������ ������ ���� ���̴�.
		PETCARDINFO_MAP_ITER iter = pOwner->m_mapPETCardInfo.find( pInvenItem->sItemCustom.PetDbNum() );
		if ( iter==pOwner->m_mapPETCardInfo.end() )
        {
            holditem::ReleaseHoldItem();

            return;
        }

		sPet = (*iter).second;
	}
	else
	{
		sPet.m_nFull  = m_nFull;
		sPet.m_emTYPE = m_emTYPE;
	}

	// �������� ����á����
	if ( sPet.m_nFull >= MAX_PET_SATIATION )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	GLMSG::SNETPET_REQ_GIVEFOOD NetMsg(wPosX, wPosY, sPet.m_emTYPE);
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();
}

void GLPetClient::ReqInputName ( WORD wPosX, WORD wPosY )
{
	if (!IsVALID())
        return; // Ȱ�����¿����� �̸����� ����
	if (m_pGaeaClient->GetTradeClient()->Valid())
        return;

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if (!pInvenItem)
        return;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if (!pItem)
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_RENAME )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_BADITEM") );
		return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_Rename );

	m_pOwner->m_wInvenPosX1 = wPosX;
	m_pOwner->m_wInvenPosY1 = wPosY;
}

void GLPetClient::ReqRename ( const char* szCharName )
{
	if( !szCharName ) return;

	CString strTEMP( szCharName );

	// �±��� ���� ���� üũ	
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString(strTEMP) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR"));
		    return;
	    }
    }

	// ��Ʈ�� ���� ���� üũ 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if( STRUTIL::CheckVietnamString( strTEMP ) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR"));
		    return;
	    }
    }


	BOOL bFILTER0 = STRUTIL::CheckString( strTEMP );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strTEMP );
	if ( bFILTER0 || bFILTER1 )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		return;
	}

	if ( strlen ( szCharName ) > PETNAMESIZE )
	{
		// �ʹ� ���.
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_TOOLONG") );
		return;
	}

	GLMSG::SNETPET_REQ_RENAME NetMsg(szCharName,m_pOwner->m_wInvenPosX1,m_pOwner->m_wInvenPosY1);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_pOwner->m_wInvenPosX1 = 0;
	m_pOwner->m_wInvenPosY1 = 0;
}

void GLPetClient::ReqInputColor ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID() )							return; // Ȱ�����¿����� ���� ����
	if ( m_pGaeaClient->GetTradeClient()->Valid() )	return;

	if ( !GLCONST_PET::IsChangeablePetColor ( m_emTYPE ) )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE") );
		return;
	}

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
        return;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_COLOR )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		return;
	}

	if ( IsUsePetSkinPack() )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PetColor_ItemUse );

	m_pOwner->m_wInvenPosX1 = wPosX;
	m_pOwner->m_wInvenPosY1 = wPosY;
}
void GLPetClient::ReqChangeColor ( WORD wColor )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR NetMsg( wColor, m_pOwner->m_wInvenPosX1, m_pOwner->m_wInvenPosY1 );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_pOwner->m_wInvenPosX1 = 0;
	m_pOwner->m_wInvenPosY1 = 0;
}
void GLPetClient::ReqInputStyle ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID() )							return; // Ȱ�����¿����� ���� ����
	if ( m_pGaeaClient->GetTradeClient()->Valid() )	return;

	PETSTYLE sPetStyle = GLCONST_PET::GetPetStyle ( m_emTYPE );
	if ( sPetStyle.wSTYLENum == 0 )				return; // chf ������ ����!!

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_STYLE )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		return;
	}

	if ( IsUsePetSkinPack() )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		return;
	}

	StyleInitData();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StylePetUse );

	m_pOwner->m_wInvenPosX2 = wPosX;
	m_pOwner->m_wInvenPosY2 = wPosY;
}
void GLPetClient::ReqChangeStyle ( WORD wStyle )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE NetMsg( wStyle, m_pOwner->m_wInvenPosX2, m_pOwner->m_wInvenPosY2 );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_pOwner->m_wInvenPosX2 = 0;
	m_pOwner->m_wInvenPosY2 = 0;
}

// ������ Ŭ���ϱ⸸ �ϸ� ȣ��Ǿ����� �Ѵ�.
void GLPetClient::ReqChangeAccessory( EMSUIT emSUIT )
{
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
    {
        holditem::ReleaseHoldItem();

        return;
    }
	//if ( emSUIT == ACCETYPESIZE )					return;

	//	Note : �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
	//
	if ( m_pOwner->isAction(GLAT_ATTACK, GLAT_SKILL) )
    {
        holditem::ReleaseHoldItem();

		return;
    }

    SITEMCUSTOM sHoldItemCustom = holditem::GetHoldItem();
	//SITEMCUSTOM sHoldItemCustom = m_pOwner->GET_HOLD_ITEM();
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( emSUIT );

	//	SLOT <-> INVEN
	if ( sHoldItemCustom.GetNativeID() != NATIVEID_NULL() && sSlotItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
		if ( !pHoldItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) )
        {
            holditem::ReleaseHoldItem();

            return;
        }
		
		SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
		if ( !pSlotItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
        {
            holditem::ReleaseHoldItem();

            return;
        }

        if ( holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            GLMSG::SNETPET_REQ_INVEN_EX_SLOT NetMsg(emSUIT);
            NetMsg.wHoldPosX = holditem::GetHoldPosX();
            NetMsg.wHoldPosY = holditem::GetHoldPosY();
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }

        holditem::ReleaseHoldItem();
	}
	// INVEN -> SLOT
	else if ( sHoldItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
		if ( !pHoldItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) )
        {
            holditem::ReleaseHoldItem();

            return;
        }

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != emSUIT )
        {
            holditem::ReleaseHoldItem();

            return;
        }

        if ( holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            GLMSG::SNETPET_REQ_INVEN_TO_SLOT NetMsg(emSUIT);
            NetMsg.wHoldPosX = holditem::GetHoldPosX();
            NetMsg.wHoldPosY = holditem::GetHoldPosY();
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }

        holditem::ReleaseHoldItem();
	}
	// HOLD
	else if ( sSlotItemCustom.GetNativeID() != NATIVEID_NULL() )
        holditem::HoldItem( holditem::PET_SLOT, sSlotItemCustom, emSUIT, 0 );
}

void GLPetClient::ReqRemoveSlotItem ( EMSUIT emSuit )
{
	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( emSuit );

	SITEM* pSlotItem = GLogicData::GetInstance().GetItem ( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// �Ϲ� ����
		return;
	}

	BOOL bOk = m_pOwner->m_cInventory.FindInsertable ( wPosX, wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		return;
	}

	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM NetMsg(emSuit);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReqChangeActiveSkill( SNATIVEID sSkillID, WORD wSlot )
{
    if ( !IsVALID() )
        return;

	// ��ų ��Ҹ� ����
	if ( sSkillID == NATIVEID_NULL() )
	{
		GLMSG::NET_PET_REQ_SKILL_CHANGE_CF NetMsg( NATIVEID_NULL(), wSlot );
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		return;
	}

	// ����� �ʾҰų� ��Ÿ�� �����, ���Ժ���
	if ( !CheckSkill( sSkillID, wSlot ) )
	{
		return;
	}

	//��ų�� �ߺ����� ����Ϸ� ���� ���
	if ( IsSkillDuplicated( sSkillID, wSlot ) )
	{
		return;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )	return;

	STARGETID sTARG( CROW_PET,m_dwGUID, m_vPos );

	//	Note : ��ų ���۰� ���ÿ� ��Ÿ���� ����Ʈ. �ߵ�.
	//
	EMELEMENT emELMT = EMELEMENT_SPIRIT;

	if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_vDir );

	SANIMSTRIKE sStrike;
	sStrike.m_emPiece = PIECE_HAIR;
	sStrike.m_emEffect = EMSF_TARGET;
	sStrike.m_dwFrame = 0;

	if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE03==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE03_POS, pSkill->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );

	GLMSG::NET_PET_REQ_SKILL_CHANGE_CF NetMsg( sSkillID, wSlot );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReqLearnSkill ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID () )							return;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )	return;
	//if( m_pOwner->ValidRebuildOpen() )			return;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );

	// �ֿ� ��ų���� ����????
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_SKILL )	return;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::NEGATIVE,
            ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY"));
		return;
	}

	//	Note : ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPET_REQ_LEARNSKILL NetMsg(wPosX,wPosY);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return;
}

void GLPetClient::ReqLearnSkill ( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( !IsVALID () )								return;
	if ( !m_pOwner->IsVALID_STORAGE(dwChannel) )	return;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )		return;
	//if( m_pOwner->ValidRebuildOpen() )			return;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_pOwner->m_cStorage[dwChannel].FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )							return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	
	// �ֿ� ��ų���� ����????
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_SKILL )	return;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::NEGATIVE,
            ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY"));
		return;
	}

	//	Note : ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPET_REQ_LEARNSKILL NetMsg(wPosX,wPosY,true,dwChannel);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return;
}

void GLPetClient::ReqLearnDualSkill ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID () )							return;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )	return;
	//if( m_pOwner->ValidRebuildOpen() )			return;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );

	// �ֿ� �ߺ���ų ��� ī������
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_DUALSKILL )	return;

	// �̹�	��� ��������
	if ( 2 <= m_wMaxSkillSlot )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::NEGATIVE,
            ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY", EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL));
		return;
	}

	//	Note : ��ų Ȯ���� ��û�մϴ�.
	//
	GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION NetMsg(EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL,wPosX,wPosY);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return;
}

void GLPetClient::ReqGetItem ( DWORD dwGetItemFlag )
{
	if ( !m_pOwner || !m_pOwner->IsValidBody() )
	{
		return;
	}

	GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
	if ( !pLAND ) return;

	DWORD emCrow = 0;

	if ( dwGetItemFlag&EMPETSKILL_GETALL )		emCrow |= (CROW_EX_ITEM|CROW_EX_MONEY);
	if ( dwGetItemFlag&EMPETSKILL_GETRARE )     emCrow |= (CROW_EX_ITEM);
	if ( dwGetItemFlag&EMPETSKILL_GETPOTIONS )  emCrow |= (CROW_EX_ITEM);
	if ( dwGetItemFlag&EMPETSKILL_GETMONEY )	emCrow |= (CROW_EX_MONEY);
	if ( dwGetItemFlag&EMPETSKILL_GETSTONE )    emCrow |= (CROW_EX_ITEM);
	if ( dwGetItemFlag&EMPETSKILL_GETQBOX )		emCrow |= (CROW_EX_ITEM);
	if ( dwGetItemFlag&EMPETSKILL_GETMATERIALS )		emCrow |= (CROW_EX_ITEM);

    //  ���� �ڵ�� �ֿ�� �ִ� �������� �����ϸ� ������ ������ ���û�ϴ� ���¿�����
    //  Ư����Ȳ���� ������ ���������� ��û�� �Ͽ� �ֿ�� �ִ� �������� �����ϸ� �ٷ� 
    //  �ݴ� ���·� �����Ͽ���.
	VEC_SK_ITER_RANGE pairRange;
	VEC_SK_TAR* pDetectMap = m_pGaeaClient->DetectCrow ( emCrow, m_vPos, MAX_VIEWRANGE / 6.0f );
	if ( pDetectMap )
	{
        VEC_SK_TAR::iterator pos_beg = pDetectMap->begin();
        VEC_SK_TAR::iterator pos_end = pDetectMap->end();

        for ( ; pos_beg != pos_end; ++pos_beg )
        {
            STARGETID sTarget = *pos_beg;
            
            if ( sTarget.emCrow == CROW_MONEY )
            {
                std::tr1::shared_ptr<CMoneyClientDrop> spDropMoney = pLAND->GetMoney(sTarget.GaeaId);
                if (!spDropMoney) 
                    continue;
                if (!spDropMoney->bCanGet) 
                    continue;

                m_vecDroppedItems.push_back( sTarget );

            }
            else if ( sTarget.emCrow == CROW_ITEM )
            {
                std::tr1::shared_ptr<CItemClientDrop> spDropItem = pLAND->GetItem(sTarget.GaeaId);
                if (!spDropItem)
                    continue;
                if (!spDropItem->bCanGet)
                    continue;

				SITEM *pItem = GLogicData::GetInstance().GetItem(spDropItem->sItemClient.sNativeID);
				if ( !pItem ) 
                    continue;

                bool bDetect = false;

				if ( pItem->sBasicOp.emItemType==ITEM_QITEM ) 
				{
					if ( dwGetItemFlag&EMPETSKILL_GETQBOX )							// Q�ڽ� ��Ʈ���� 
					{
						//!! SF_TODO
 						/*if ( m_pInterface->QBoxButtonGetEnable() == false)
						{
							if ( this->IsQBoxLockMessageNotifyTime() )
								m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("QBOX_OPTION_DISABLE_MSG") );
						}
 						else */
						if(  true == (GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
							NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock)) )	// QBox Lock�� �ɷ� ���� �ʴ� ���
							bDetect = true;
					}
					//bDetect = true;
					//Ȯ�ο� �ڵ�
				}
                else if ( dwGetItemFlag&EMPETSKILL_GETALL )
                {
                    bDetect = true;
                }
                else
                {
                    if ( dwGetItemFlag&EMPETSKILL_GETRARE )
                    {
                        if ( pItem->sBasicOp.emLevel == EMITEM_LEVEL_HIGH )
					    {
						    bDetect = true;					
					    }
                    }
                    
                    if ( dwGetItemFlag&EMPETSKILL_GETPOTIONS )
			        {
					    if ( pItem->sBasicOp.emItemType == ITEM_CURE || pItem->sBasicOp.emItemType == ITEM_EFFECT_RELEASE_CURE )
                        {
                            bDetect = true;
                        }
                    }
                    
                    if ( dwGetItemFlag&EMPETSKILL_GETSTONE )
					{
						if ( pItem->sBasicOp.emItemType == ITEM_GRINDING || pItem->sBasicOp.emItemType == ITEM_MATERIALS )
						{
							bDetect = true; 							
						}
					}

					if ( dwGetItemFlag&EMPETSKILL_GETMATERIALS )
					{
						if ( pItem->sBasicOp.emItemType == ITEM_MATERIALS )
						{
							bDetect = true; 							
						}
					}
                }

                if ( bDetect ) 
                    m_vecDroppedItems.push_back( sTarget );
            }
        }
	}
}
