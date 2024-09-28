#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLuaPlay.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/stl_Func.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../RanParamClient.h"

#include "../Macro/MacroManager.h"
#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../GLogicExClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Party/GLPartyClient.h"
#include "../Strike/GLStrikeM.h"
#include "../Land/GLLandManClient.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../Input/GLInputManager.h"
#include "./GLCharacter.h"

#include "../GfxInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/**
 * ��ų��� ����Ȯ�� �� �õ�.
 *
 * @return ��ų��밡���ϸ� TRUE.
 */
BOOL GLCharacter::TryToUseSkill()
{
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( pLAND && pLAND->IsPeaceZone() )
		return FALSE;

	if ( IsActState(EM_ACT_PEACEMODE) )

		ReqTogglePeaceMode ();

	return TRUE;
} //GLCharacter::TryToUseSkill

/**
 * 
 */
ClientActor* GLCharacter::GetCrowOnLand( const EMCROW emCrow, const DWORD dwGaeaId )
{
	if( !m_pGaeaClient )
		return NULL;

	return m_pGaeaClient->GetCrow( emCrow, dwGaeaId );
} //GLCharacter::GetCrowOnLand

void GLCharacter::StartSkillWaitProc ()
{
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( pLAND && pLAND->IsPeaceZone() )
		return;

	if ( IsActState(EM_ACT_PEACEMODE) )
	{
		ReqTogglePeaceMode ();
	}

	if( m_pSkinChar )  
		m_pSkinChar->ResetStrikeAttachBone();

	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	PGLSKILL		pSkill = GLSkillMan::GetInstance().GetData( m_sActiveSkill.wMainID, m_sActiveSkill.wSubID );
	if( !pSkill )
		return;

	const D3DXVECTOR3 vReDirect = UpdateSkillDirection(
		m_pGaeaClient, GetPosition(), GetDirect(), m_sActiveSkill, m_sSkillTargetData.TargetID.vPos, m_sSkillTargetData.TargetIDs );
	ClientActor::SetDirect(vReDirect);

	m_emANIMAINSKILL	= pSkill->m_sEXT_DATA.emANIMTYPE;
	m_emANISUBSKILL		= pSkill->m_sEXT_DATA.emANISTYPE;
	if (m_sVehicle.PassengerId(0) == m_dwGaeaID)
	{
		if( m_sActiveSkill.wMainID == EMSKILL_BIKE )
		{
			SITEM* pItem = NULL;
			pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
			if( pItem )
			{
				m_emANIMAINSKILL = EMANI_MAINTYPE ( AN_BIKE_A +  pItem->sVehicle.emPartsType );
				m_emANIVehicleSKILL = pSkill->m_sEXT_DATA.emANIVehicleMTYPE;
			} //if
		}
		else
		{
			m_emANIVehicleSKILL = AN_VEHICLE_STAY;
		} //if..else
	} //if
}

void GLCharacter::StartSkillProc ()
{
	SCHARSKILL		sSkillLevel;
	if( GetSkillLevel( m_sActiveSkill, sSkillLevel ) )
		m_pWorkingSkill->Start( sSkillLevel, this, &m_sSkillTargetData, false );
}

void GLCharacter::SetHiddenWeaponPiece()
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sActiveSkill.wMainID, m_sActiveSkill.wSubID );
	if( !pSkill )
		return;

	// ��ų�� ���Ǵ� ���⸦ �����Ѵ�;
	const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
	if( !bHiddenWeapon )
		return;

	SNATIVEID sHiddenWeapon = pSkill->m_sLEARN.sHiddenWeapon;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sHiddenWeapon );
	if( pItem )
	{
		// [shhan][2014.12.18] rm #571 - �ذ��� ��ݽ�ų(�Ƿ�/�ذ��۷����) �� �޼� ���� �ܻ���.
		//						����� ���� ResetHiddenWeaponPiece ���� UpdateSuit �� �ߴ��� 
		//						��ձ��� ���� ��� �� �Ѽչ��� ��ų�� ����ϸ� �ٸ��Ѽտ� ������ ���̴� ���װ� �־���.
		//						�׷��� Ȱ��ȭ �� ��ġ�� ������ �ǵ��� �����۾���.
		EMPIECECHAR emType(PIECE_RHAND_HIDDEN);
		if ( m_pSkinChar->SetPieceHiddenWeapon ( pItem->GetWearingFile(m_CHARINDEX),	m_pd3dDevice, 0, EMSCD_ALL_THREAD, emType, pItem->GetSelfBodyEffect() ) == S_OK )
		{
			if ( emType == PIECE_RHAND_HIDDEN || emType == PIECE_LHAND_HIDDEN )
			{
				m_pSkinChar->SetPartRend( emType, TRUE );
			}
		}
		if ( m_pSkinChar->SetPieceHiddenWeapon ( pItem->GetWearingFileEx(m_CHARINDEX),	m_pd3dDevice, 0, EMSCD_ALL_THREAD,  emType, pItem->GetSelfBodyEffect() ) == S_OK )
		{
			if ( emType == PIECE_RHAND_HIDDEN || emType == PIECE_LHAND_HIDDEN )
			{
				m_pSkinChar->SetPartRend( emType, TRUE );
			}
		}
	}
}

void GLCharacter::ResetHiddenWeaponPiece()
{
	PDXCHARPART pRHAND_HIDDEN = m_pSkinChar->GetPiece( PIECE_RHAND_HIDDEN );
	PDXCHARPART pLHAND_HIDDEN = m_pSkinChar->GetPiece( PIECE_LHAND_HIDDEN );
	if( pRHAND_HIDDEN->IsActiveCharPart() || pLHAND_HIDDEN->IsActiveCharPart() )
	{
		m_pSkinChar->SetPartRend ( PIECE_RHAND_HIDDEN, FALSE );
		m_pSkinChar->SetPartRend ( PIECE_LHAND_HIDDEN, FALSE );

		// [shhan][2014.11.20] �����ο��� UpdateSuit �� �ϸ� ���䰡 ���µǸ鼭 ��鸮�� ������ �־ ���ƺ�.
		//						�ϴ� �ּ��ص� �� �� �ѵ�, ������ �Ǹ� ���䵵 �����ϸ鼭 �����۾� �ʿ���.
		//UpdateSuit();
	}
}

void GLCharacter::ACCOUNTBIKESKILL ( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
	if ( !pSkill )															
		return;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];


	//	��ų ������ ���.
	const float fDecreaseT(sSKILL_DATA.fDELAYTIME*GetDecR_SkillDelayT(pSkill->m_sBASIC.dwGRADE-1))
		, fDelayTime(sSKILL_DATA.fDELAYTIME-fDecreaseT);
    SKILL::SKILLDELAY sSkilldelay;
	sSkilldelay.fDelayTime = GLOGICEX::SKILLDELAY(pSkill->m_sBASIC.dwGRADE,sSkill.wLevel,GETLEVEL(),fDelayTime);


	sSkilldelay.fDefultMaxDelayTime = sSkilldelay.fDelayTime;
	//	Note : �����̻��� ������ ����.
	sSkilldelay.fDelayTime = sSkilldelay.fDelayTime * m_fSTATE_DELAY;
    sSkilldelay.fMAXDelayTime = sSkilldelay.fDelayTime;


	//	Note : ���������� �޽��� ������ ��ŭ ���ҽ��� �ش�.
	if ( bServer )
		APPLY_MSGDELAY ( sSkilldelay.fDelayTime );


	m_SKILLDELAY.insert ( std::make_pair(sSkill.sNativeID.dwID,sSkilldelay) );

	EMSLOT emLHand = GetCurLHand();

	if ( sSKILL_DATA.nUSE_BATTERY != 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(SLOT_VEHICLE);
		if ( pItem )
		{
			//	�Ѿ� ���� ������ �������� �ʴ´�.
			INT &nTurnNum = m_sVehicle.m_nFull;
			if ( nTurnNum >= sSKILL_DATA.nUSE_BATTERY )
				nTurnNum -= sSKILL_DATA.nUSE_BATTERY;
			else										
				nTurnNum = 0;
		}
	}

	//m_sVehicle.m_nFull -= GLCONST_VEHICLE::nFullDecrVehicle[m_sVehicle.m_emTYPE];

	//	���� ü�� �Ҹ�.
	m_sHP.DECREASE(int(sSKILL_DATA.wUSE_HP * wStrikeNum));
	m_sMP.DECREASE(int(sSKILL_DATA.wUSE_MP * wStrikeNum));

	//  CP �Ҹ� 
	m_sCP.DECREASE(sSKILL_DATA.wUSE_CP * wStrikeNum);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

}

// *****************************************************
// Desc: ��ų�ߵ����� ó��
// *****************************************************

/**
 * 
 */
void GLCharacter::AttackTargetAfterSkill( const STARGETID& sTargetID )
{
	//! ���� ���� �ൿ ��� ���� üũ.
	// bool	bcontinue = RANPARAM::bFORCED_ATTACK ? true : false;
	if ( RANPARAM::bFORCED_ATTACK == false )
		return;

	switch ( sTargetID.emCrow )
	{
	case CROW_PC:
	case CROW_SUMMON:
		{
			if ( IsReActionable( sTargetID, EMIMPACT_SIDE_ENEMY ) )
			{
				GLREACTION_PVP sPvPReaction;
				sPvPReaction.SetTARID(sTargetID);
				sPvPReaction.SetPOS(GetActionID().vPos);
				//sPvPReaction.SetNONTRACE(true);
				sPvPReaction.SetNONCANCEL(true);
				sPvPReaction.SETPRESERVATION(true);
				m_ReservationAction.RegisterReaction( sPvPReaction );
			}
		}
		break;

	case CROW_MOB:
		{
			if ( IsReActionable( sTargetID, EMIMPACT_SIDE_ENEMY ) )
			{
				GLREACTION_MOB sMobReaction;
				sMobReaction.SetTARID(sTargetID);
				sMobReaction.SetPOS(GetActionID().vPos);
				//sMobReaction.SetNONTRACE(true);
				sMobReaction.SetNONCANCEL(true);
				sMobReaction.SETPRESERVATION(true);
				m_ReservationAction.RegisterReaction( sMobReaction );
			}
		}
		break;

	default:
		break;
	};
} //GLCharacter::AttackTargetAfterSkill

/**
 * ��ų�� �����Ѵ�.
 */
void GLCharacter::ReserveSkill( const SNATIVEID &sSkillID, GLREACTION_SKILL& sSkillReaction )
{
	sSkillReaction.SetRESERVED(true);
	sSkillReaction.SetRETARGETING(true);

	//! �Ȱ��� ��ų�� �����Ҽ� ����.
	GLREACTION_ITER Finditer = m_ReservationAction.Find_Skill_Reaction( sSkillID );
	if ( Finditer != m_ReservationAction.EndIter() )
		return;

	GLREACTION_ITER Skilliter = m_ReservationAction.Find_Last_Of_Reaction( REACT_SKILL );
	//! ��ų�� ���� �������� �ִ´�.
	if ( Skilliter != m_ReservationAction.EndIter() )
	{
		m_ReservationAction.RegisterReaction( ++Skilliter, sSkillReaction );

		//! ���� �ִϸ��̼��� �����ش�
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
			"-dw", sSkillID.dwID );
	}
	else				//! ���� ���ٸ� �Ϲ� ���� �տ� �ִ´�
	{
		GLREACTION_ITER Attackiter = m_ReservationAction.Find_First_Of_Attack_Reaction();

		//! ���ݸ��׼��� ��ų�� ������� �ʰ� �����Ѵ�
		if ( Attackiter != m_ReservationAction.EndIter() )
		{
			GLREACTION& sReaction(*Attackiter);
			switch ( sReaction.GETTYPE() ) 
			{
			case REACT_PVP:
				{
					GLREACTION_PVP& sPVP(*Attackiter);
					sPVP.SetNONCANCEL(true);
				}
				break;

			case REACT_MOB:
				{
					GLREACTION_MOB& sMOB(*Attackiter);
					sMOB.SetNONCANCEL(true);
				}
				break;
			}; //switch
		} //if
		m_ReservationAction.RegisterReaction( Attackiter, sSkillReaction );

		//! ���� ��ų �������̶��
		if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )			//! ���� �ִϸ��̼��� �����ش�
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
				"-dw", sSkillID.dwID );
		}

	} //if..else
} //GLCharacter::ReserveSkill

/**
 * Ư����ü�� ���� ��ų�ߵ�.
 */

bool GLCharacter::SkillActiveSelfToSpec( const SNATIVEID &sSkillID, WORD wSkillQuickSlot )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	//! ��ų ������ �ƴ϶�� ��ų ��� ���߿� ��ų�� ����� �� ����
	if ( !RANPARAM::bSKILL_RESERVATION )
	{
		if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) && !IsExceedSkillHitFrameOfLast() )
			return false;
	}

	//! ��ų�� �Ӽ����� �ൿ�� �����Ѵ�
	EMIMPACT_SIDE		emSIDE		= pSkill->m_sBASIC.emIMPACT_SIDE;
	STARGETID&			sTargetID	= GetSelectID();
	//! �ش� Ÿ���� ��ȿ���� �˻�
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		if( !pActor )
			return false;

		//! ������ ����ϰ� ���õ� Ÿ���� ��ȿ���� �ʴٸ�
		if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			if ( !pActor->IsValidBody() )
				sTargetID.RESET();
		} //if
	} //if

	if ( sTargetID.ISNULL() )				//! ���õ� Ÿ���� ���ٸ�
	{
		if ( emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			//! ���� ���õ� Ÿ���� ������ �������̶��, �� Ÿ���� �����Ѵ�
			if ( isAction(GLAT_ATTACK) )
			{
				SetSelectID(m_sActionID);
				sTargetID = m_sActionID;
			}
			else
			{
				GLInputManager::GetInstance().ExternEvent_SelectAround();
				sTargetID = GetSelectID();
			} //if..else

			//  [12/5/2013 gbgim];
			// - ���ེų�� ��� �ش� ���� �������, ������ �ٸ� ���� ����ڰ� ���� ��� 
			// ���� �������δ� ó�Ⱑ �����ʰ� NULL���� ���� �׷��� ������ �ѹ��� �˻��Ѵ�;
			if ( sTargetID.ISNULL() )
				return false;
		}
		else if ( emSIDE == EMIMPACT_SIDE_ANYBODY || emSIDE == EMIMPACT_SIDE_PARTY )				//! ���� PC���� ��밡���� ��ų�̰� Ÿ���� ���ٸ� �ڽſ��� ����.
		{
			sTargetID = STARGETID(CROW_PC, GetGaeaID(), GetPosition());
		} //if..else
	} //if

	if( !RF_PROVOKE( m_EffSkillVarSet).IsOn() )
	{
		//! ��ų�� ��� ���������� �˻��Ѵ�
		EMSKILLCHECK	emSkillCheck = SkillCheck ( sSkillID, sTargetID, 1, true, true, false );
		if( emSkillCheck != EMSKILL_OK && 
			emSkillCheck != EMSKILL_NOTSP ) 
			return false;
	}
	
	//! ���� ó���� ������ �ٽ� ����� �ֵ��� �Ѵ�
	m_ReservationAction.Clear( REACT_MOB );
	m_ReservationAction.Clear( REACT_PVP );

	//! �ʿ���� ���� �׼��� �������ش�.
	m_ReservationAction.Clear( REACT_P2P );
	m_ReservationAction.Clear( REACT_NPC );
	m_ReservationAction.Clear( REACT_ITEM );
	m_ReservationAction.Clear( REACT_GATHERING );

	GLREACTION_SKILL		sSkillReaction;
	sSkillReaction.SetSKILLID(sSkillID);
	sSkillReaction.SetTARID(sTargetID);
	sSkillReaction.SetCHANGERUNSKILL(wSkillQuickSlot);

	//! ��ų ������ ���
	if ( RANPARAM::bSKILL_RESERVATION )
	{
		ReserveSkill( sSkillID, sSkillReaction );
	}
	else
	{
		m_ReservationAction.Clear( REACT_TRACE );
		m_ReservationAction.Clear( REACT_SKILL );
		m_ReservationAction.RegisterReaction( sSkillReaction );
	} //if..else

	//! ��ų �����, ����� �����ϵ��� �Ѵ�;
	if ( pSkill->IsHarm() && !GLMacroManager::GetInstance().IsActive() )
		AttackTargetAfterSkill( sTargetID );

	return true;
} //GLCharacter::SkillActiveSelfToSpec

/**
 * ������ų �ߵ�.
 */
bool GLCharacter::SkillActiveZone( const SNATIVEID &sSkillID, WORD wSkillQuickSlot )
{
	//! ��ų�� ��밡������ �˻�
	EMSKILLCHECK emSkillCheck = SkillCheck ( sSkillID, STARGETID(), 1, true, true, false );

	if( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP ) 
	{
		return false;
	}

	//SKILL_MAP_ITER learniter = m_ExpSkills.find( sSkillID.dwID );
	//if( learniter == m_ExpSkills.end() )
		//return false;	// ��� ��ų�� �ƴ� ���.
	SCHARSKILL charSkill;

	if( !ISLEARNED_SKILLEX( sSkillID, charSkill ) ) return false;

	//SCHARSKILL&		sSkill = (*learniter).second;
	//SetScopeSkillRun( SKILLID(sSkillID, sSkill.wLevel) );
	SetScopeSkillRun( SKILLID(sSkillID, charSkill.wLevel) );
	return true;
} //GLCharacter::SkillActiveZone

bool GLCharacter::SkillAutoActiveZone( const SNATIVEID &sSkillID, WORD wSkillQuickSlot )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	//! ��ų ������ �ƴ϶�� ��ų ��� ���߿� ��ų�� ����� �� ����;
	if ( !RANPARAM::bSKILL_RESERVATION )
	{
		if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) && !IsExceedSkillHitFrameOfLast() )
			return false;
	}


	//! ��ų�� ��밡������ �˻�;
	//EMSKILLCHECK emSkillCheck = SkillCheck ( sSkillID, STARGETID(), 1, true, true, false );
	EMSKILLCHECK emSkillCheck = SkillCheck ( sSkillID, STARGETID(), 1, true, !RF_PROVOKE( m_EffSkillVarSet ).IsOn(), false );
	if( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP ) 
	{
		return false;
	}

	//SKILL_MAP_ITER learniter = m_ExpSkills.find( sSkillID.dwID );
	//if( learniter == m_ExpSkills.end() )
		//return false;	// ��� ��ų�� �ƴ� ���.
	
	//SCHARSKILL&		sSkill = (*learniter).second;
	SCHARSKILL	sSkill;

	if( !ISLEARNED_SKILLEX( sSkillID, sSkill ) ) return false;

	//! ��ų�� �Ӽ����� �ൿ�� �����Ѵ�;
	EMIMPACT_SIDE		emSIDE		= pSkill->m_sBASIC.emIMPACT_SIDE;
	STARGETID&			sTargetID	= GetSelectID();
	//! �ش� Ÿ���� ��ȿ���� �˻�;
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		if( !pActor )
			return false;

		//! ������ ����ϰ� ���õ� Ÿ���� ��ȿ���� �ʴٸ�;
		if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			if ( !pActor->IsValidBody() )
				sTargetID.RESET();
		} //if
	} //if

	if ( sTargetID.ISNULL() )				//! ���õ� Ÿ���� ���ٸ�;
	{
		if ( emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			//! ���� ���õ� Ÿ���� ������ �������̶��, �� Ÿ���� �����Ѵ�;
			if ( isAction(GLAT_ATTACK) )
			{
				SetSelectID(m_sActionID);
				sTargetID = m_sActionID;
			}
			else
			{
				GLInputManager::GetInstance().ExternEvent_SelectAround();
				sTargetID = GetSelectID();
			} //if..else

			//  [12/5/2013 gbgim];
			// - ���ེų�� ��� �ش� ���� �������, ������ �ٸ� ���� ����ڰ� ���� ���; 
			// ���� �������δ� ó�Ⱑ �����ʰ� NULL���� ���� �׷��� ������ �ѹ��� �˻��Ѵ�;
			if ( sTargetID.ISNULL() )
				return false;
		}
		else if ( emSIDE == EMIMPACT_SIDE_ANYBODY || emSIDE == EMIMPACT_SIDE_PARTY )				//! ���� PC���� ��밡���� ��ų�̰� Ÿ���� ���ٸ� �ڽſ��� ����.
		{
			sTargetID = STARGETID(CROW_PC, GetGaeaID(), GetPosition());
		} //if..else
	} //if

	//! �ʿ���� ���� �׼��� �������ش�.;
	m_ReservationAction.Clear( REACT_P2P );
	m_ReservationAction.Clear( REACT_NPC );
	m_ReservationAction.Clear( REACT_ITEM );
	m_ReservationAction.Clear( REACT_GATHERING );

	GLREACTION_SKILL sSkillReaction;
	sSkillReaction.SetSKILLID(sSkillID);
	sSkillReaction.SetTARID(STARGETID(CROW_NULL, EMTARGET_NULL, sTargetID.vPos));
	sSkillReaction.SetCHANGERUNSKILL(FindQuickSkillSlot(SKILLID(sSkillID, sSkill.wLevel)));
	//! ��ų ������ ���;
	if ( RANPARAM::bSKILL_RESERVATION )
	{
		sSkillReaction.SetRESERVED(true);

		//! �Ȱ��� ��ų�� �����Ҽ� ����.;
		GLREACTION_ITER Finditer = m_ReservationAction.Find_Skill_Reaction( sSkillID );
		if ( Finditer != m_ReservationAction.EndIter() )
		{
			return false;
		}

		GLREACTION_ITER Skilliter = m_ReservationAction.Find_Last_Of_Reaction( REACT_SKILL );
		//! ��ų�� ���� �������� �ִ´�.;
		if ( Skilliter != m_ReservationAction.EndIter() )
		{
			m_ReservationAction.RegisterReaction( ++Skilliter, sSkillReaction );
			//! ���� �ִϸ��̼��� �����ش�;
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
				"-dw", sSkillID.dwID );
		}
		//! ���� ���ٸ� �Ϲ� ���� �տ� �ִ´�;
		else
		{
			GLREACTION_ITER Attackiter = m_ReservationAction.Find_First_Of_Attack_Reaction();

			//! ���ݸ��׼��� ��ų�� ������� �ʰ� �����Ѵ�;
			if ( Attackiter != m_ReservationAction.EndIter() )
			{
				GLREACTION& sReaction(*Attackiter);
				switch ( sReaction.GETTYPE() ) 
				{
				case REACT_PVP:
					{
						GLREACTION_PVP& sPVP(*Attackiter);
						sPVP.SetNONCANCEL(true);
					}
					break;

				case REACT_MOB:
					{
						GLREACTION_MOB& sMOB(*Attackiter);
						sMOB.SetNONCANCEL(true);
					}
					break;
				};
			}
			m_ReservationAction.RegisterReaction( Attackiter, sSkillReaction );

			//! ���� ��ų �������̶��;
			if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
			{
				//! ���� �ִϸ��̼��� �����ش�;
				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
					"-dw", sSkillID.dwID );
			}
		}
	}
	//! ��ų ������ �ƴ� ���;
	else
	{
		m_ReservationAction.Clear( REACT_MOB );
		m_ReservationAction.Clear( REACT_PVP );
		m_ReservationAction.Clear( REACT_TRACE );
		m_ReservationAction.Clear( REACT_SKILL );
		m_ReservationAction.RegisterReaction( sSkillReaction );
	}
}

SNATIVEID GLCharacter::GetSkillIDQSlot(WORD wSlot) const
{
	return RF_DISGUISE( m_EffSkillVarSet ).IsOn() ? m_sChangeQUICK[wSlot] : m_sSKILLQUICK[wSlot];
}

/**
 * �����Կ��� ��ų�ߵ�.
 */
bool GLCharacter::SkillActive( const WORD wSkillQuickSlot )
{
    // ���� ���¶�� ��ų �ߵ��� ���� �ʴ´�.
    if( IsDie() == TRUE)
        return false;
	
	return SkillActive( RF_DISGUISE( m_EffSkillVarSet ).IsOn() ? m_sChangeQUICK[wSkillQuickSlot] : m_sSKILLQUICK[wSkillQuickSlot], wSkillQuickSlot );
	//return SkillActive( m_sSKILLQUICK[wSkillQuickSlot], wSkillQuickSlot );
} //GLCharacter::SkillActive

/**
 * ��ų�ߵ�.
 *
 * @param sSkillID					[in] reservation skill's id
 * @param wSkillQuickSlot			[in] quick slot index.
 */
bool GLCharacter::SkillActive( const SNATIVEID &sSkillID, WORD wSkillQuickSlot, bool bAutoMatic )
{
	if ( sSkillID == NATIVEID_NULL() ) 
		return false;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );

	if ( !pSkill ) 
		return false;

	SetDefenseSkill( false );

	switch( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
	case TAR_SPEC:
	case TAR_SELF_TOSPEC:
		return SkillActiveSelfToSpec( sSkillID, wSkillQuickSlot );

	case TAR_LAND:
	case TAR_ZONE:
		return ( (bAutoMatic == false)?SkillActiveZone(sSkillID, wSkillQuickSlot) : SkillAutoActiveZone(sSkillID, wSkillQuickSlot) );

	default:
		return false;
	}; //switch
} //GLCharacter::SkillActive

//! �������� ��ų�� ������ ����ϴ� �κ�
void GLCharacter::ScopeSkillActive()
{
	//! ��ų ������ �ƴ϶�� ��ų ��� ���߿� ��ų�� ����� �� ����
	if ( !RANPARAM::bSKILL_RESERVATION )
	{
		if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
		{
			return;
		}
	}

	const SKILLID& sSkillID = GetScopeSkillRun();
	
	if ( sSkillID == SKILLID_NULL )
	{
		return;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.mid(), sSkillID.sid() );

	if ( !pSkill ) 
	{
		SetScopeSkillRun( SKILLID_NULL );
		return;
	}

	//! Ŀ���� ������ �ȿ� ������츸
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}

	D3DXVECTOR3 vTarPos = D3DXVECTOR3(0.0f,0.0f,0.0f);

	switch ( pSkill->m_sBASIC.emIMPACT_REALM )
	{
	default:
		{
			SetScopeSkillRun( SKILLID_NULL );
			return;
		}

	case REALM_SELF:
	case REALM_ZONE:
	case REALM_KEEP_ZONE:
		//! ����
		{		
			bool bEnable = true;
			float fRadius = 0.0f;
			D3DXVECTOR3 vCollPos;

			if ( !SkillActivationInfoCircle( sSkillID, vFromPt, vTargetPt, bEnable, vTarPos, fRadius) )
			{
				return;
			}

			if ( !bEnable )
			{
				return;
			}
		}
		break;

	case REALM_FANWIZE:
		//! ��ä��
		{
			//! ���� ������Ʈ.
			D3DXVECTOR3 vDir;
			float fRadius = 0.0f;
			float fDegree = 0.0f;

			if ( !SkillActivationInfoFanwise( sSkillID, vFromPt, vTargetPt, vDir, fRadius, fDegree ) )
			{
				return;
			}

			vTarPos = GetPosition() + (vDir * fRadius);
		}
		break;
	};

	//! ��ų�� ��밡������ �˻�
	EMSKILLCHECK emSkillCheck = SkillCheck ( sSkillID.sSKILLID, STARGETID(), 1, true, true, false );

	if( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP ) 
	{
		SetScopeSkillRun( SKILLID_NULL );
		return;
	}

	//! �ʿ���� ���� �׼��� �������ش�.
	m_ReservationAction.Clear( REACT_P2P );
	m_ReservationAction.Clear( REACT_NPC );
	m_ReservationAction.Clear( REACT_ITEM );
	m_ReservationAction.Clear( REACT_GATHERING );

	GLREACTION_SKILL sSkillReaction;
	sSkillReaction.SetSKILLID(sSkillID.sSKILLID);
	sSkillReaction.SetTARID(STARGETID(CROW_NULL, EMTARGET_NULL, vTarPos));
	sSkillReaction.SetCHANGERUNSKILL(FindQuickSkillSlot(sSkillID));

	//! ��ų ������ ���
	if ( RANPARAM::bSKILL_RESERVATION )
	{
		sSkillReaction.SetRESERVED(true);

		//! �Ȱ��� ��ų�� �����Ҽ� ����.
		GLREACTION_ITER Finditer = m_ReservationAction.Find_Skill_Reaction( sSkillID.sSKILLID );
		if ( Finditer != m_ReservationAction.EndIter() )
		{
			return;
		}

		GLREACTION_ITER Skilliter = m_ReservationAction.Find_Last_Of_Reaction( REACT_SKILL );
		//! ��ų�� ���� �������� �ִ´�.
		if ( Skilliter != m_ReservationAction.EndIter() )
		{
			m_ReservationAction.RegisterReaction( ++Skilliter, sSkillReaction );
			//! ���� �ִϸ��̼��� �����ش�
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
				"-dw", sSkillID.sSKILLID.dwID );
		}
		//! ���� ���ٸ� �Ϲ� ���� �տ� �ִ´�
		else
		{
			GLREACTION_ITER Attackiter = m_ReservationAction.Find_First_Of_Attack_Reaction();

			//! ���ݸ��׼��� ��ų�� ������� �ʰ� �����Ѵ�
			if ( Attackiter != m_ReservationAction.EndIter() )
			{
				GLREACTION& sReaction(*Attackiter);
				switch ( sReaction.GETTYPE() ) 
				{
				case REACT_PVP:
					{
						GLREACTION_PVP& sPVP(*Attackiter);
						sPVP.SetNONCANCEL(true);
					}
					break;

				case REACT_MOB:
					{
						GLREACTION_MOB& sMOB(*Attackiter);
						sMOB.SetNONCANCEL(true);
					}
					break;
				};
			}
			m_ReservationAction.RegisterReaction( Attackiter, sSkillReaction );

			//! ���� ��ų �������̶��
			if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
			{
				//! ���� �ִϸ��̼��� �����ش�
				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_SkillReserve_SetSkill,
					"-dw", sSkillID.sSKILLID.dwID );
			}
		}
	}
	//! ��ų ������ �ƴ� ���
	else
	{
		m_ReservationAction.Clear( REACT_MOB );
		m_ReservationAction.Clear( REACT_PVP );
		m_ReservationAction.Clear( REACT_TRACE );
		m_ReservationAction.Clear( REACT_SKILL );
		m_ReservationAction.RegisterReaction( sSkillReaction );
	}

	SetScopeSkillRun( SKILLID_NULL );
}

EMSKILLCHECK GLCharacter::SkillCheck ( const SNATIVEID& sSkillID, 
									  const STARGETID& sTargetID, 
									  WORD wStrikeNum, 
									  bool bPrintFail, 
									  bool bCheckOnlyAvailability,
									  bool bNotLearn )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );

	if ( !pSkill )
	{
		if ( bPrintFail )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_DOFAIL") );
		}

		return EMSKILL_FAILED;
	}

    bool bSpecRebirth = false;
    for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
    {

        if ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_REBIRTH )
        {
            bSpecRebirth = true;
            break;
        }
    }

	//! �����̶�� �Ұ���
	if ( !bCheckOnlyAvailability )
	{
		if ( m_bSTATE_STUN )	
			return EMSKILL_FAILED;
	}

	//! ��ȭ�����̶�� �Ұ���
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( pLAND && pLAND->IsPeaceZone() && !bSpecRebirth )	
		return EMSKILL_FAILED;

	//	Note : ���� ��ų���� �˻�.
	//
	if ( !bNotLearn )
	{
		if ( !ISLEARNED_SKILL ( sSkillID ) )	
		{
			return EMSKILL_FAILED;
		}
	}

	//	��ų�� ����Ҽ� ���� ��Ȳ�̴�.
	if ( !bCheckOnlyAvailability )
	{
		if ( IsActionLimit( EMACTION_LIMIT_SKILL ) )
		{
			//	���콺 Ŭ���Ҷ��� �޼��� ���
			if ( bPrintFail ) 
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_SKILL") );
			}

			return EMSKILL_FAILED;
		}
	}

	// �������� ĳ���ʹ� ��ų�� ����� �� ����;
	if ( m_bCLUB_CERTIFY )
	{
		//	���콺 Ŭ���Ҷ��� �޼��� ���
		if ( bPrintFail ) 
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GUIDANCE_CERTIFY_SKILLBLOCK") );
		}
		// TurnAction( GLAT_IDLE );
		return EMSKILL_FAILED;
	}

	//! ��ų �Ӽ��� ���� �˻�
	EMIMPACT_TARGET emTARGET = pSkill->m_sBASIC.emIMPACT_TAR;

	if ( emTARGET==TAR_SPEC || emTARGET==TAR_SELF_TOSPEC )
	{
		if ( sTargetID == STARGETID() )
		{			
			if ( bPrintFail )
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_REQ_SELECT_TARGET") );
			}

			return EMSKILL_FAILED;
		}

        //���߿� ���� �˻� (���ߴ��� Ÿ�ٸ����Ը� ��ų�� ����� �� �ִ�.
        if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT) )
        {
            if ( sTargetID != m_sTaunt.sTauntTargetID )
                return EMSKILL_FAILED;
        }

		if ( IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE) )
		{
			if ( sTargetID != RF_PROVOKE( m_EffSkillVarSet ).GetHostID() )
				return EMSKILL_FAILED;
		}

		ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID.emCrow, sTargetID.GaeaId );
		if ( pTarget )
		{
			// Note : ���� �Ұ����� CROW�ϰ�� ��ų ���
			if ( pTarget->IsNonDamaged() )
			{
				if ( bPrintFail )
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CROW_NONDAMAGED") );
				}

				return EMSKILL_FAILED;
			}

			//! ��� �˻�
			if ( !IsReActionable( sTargetID, pSkill->m_sBASIC.emIMPACT_SIDE ) )
			{
				if ( bPrintFail )
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_WRONG_TARGET") );
					// ���� ��ų�� �����ϱ� ����
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
				}

				return EMSKILL_FAILED;
			}

            if (!bSpecRebirth)
            {
			    //! ����� �״� ���̶��
			    if ( pTarget->isAction(GLAT_FALLING, GLAT_DIE) )
			    {
				    if ( bPrintFail )
				    {
					    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_WRONG_TARGET") );
						// ���ེų ���Ÿ� ����;
						GLWidgetScript::GetInstance().LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
				    }

				    return EMSKILL_FAILED;
			    }
            }
		}
		else
		{	
			if ( bPrintFail )
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOT_FOUND_TARGET") );
			}

			return EMSKILL_FAILED;
		}
	}

	//!	������ �浹�ϴ��� �˻�.
	//  ��� ���ɼ��� üũ�Ѵٸ� �˻����� �ʴ´�.
	//
	if ( !bCheckOnlyAvailability )
	{
		const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

		if ( emTARGET!=TAR_SELF && sTargetID.GaeaId!=EMTARGET_NULL )
		{
			D3DXVECTOR3 vPos1 = GetPosition();	vPos1.y += 15.0f;
			D3DXVECTOR3 vPos2 = vTarPos;		vPos2.y += 15.0f;

			BOOL bCollision = TRUE;
			D3DXVECTOR3 vCollision;
			LPDXFRAME pDxFrame = NULL;

			DxLandMan *pLandMan = m_pGaeaClient->GetActiveMap()->GetLandMan();
			pLandMan->IsCollision ( vPos1, vPos2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );
			if ( bCollision )						
			{
				if ( IsActState(EM_ACT_MACROMODE) )
					GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_RESERVEACTIONMOVE_FAIL);

				return EMSKILL_FAILED;
			}
		}

		const PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( pSkill->m_sEXT_DATA.emANIMTYPE, pSkill->m_sEXT_DATA.emANISTYPE );
		if ( !pAnicont )
		{
			// �ε����� �� �ִ�.
			//sc::writeLogError(
			//	sc::string::format(
			//	"��Ų ���ϸ��̼��� �������� �ʽ��ϴ�. ANIM %1%, ANIS %2%",
			//	pSkill->m_sEXT_DATA.emANIMTYPE,
			//	pSkill->m_sEXT_DATA.emANISTYPE));

			return EMSKILL_FAILED;
		}
	}

	DWORD dwSkillCheckOption = 0;

	if ( bNotLearn )
	{
		dwSkillCheckOption |= EMSKILLCHECKOPTION_DEFENSESKILL;
	}

	//! ��밡�������� üũ�Ѵٸ�, ���� ���⵵ üũ�Ѵ�
	if ( bCheckOnlyAvailability )
	{
		dwSkillCheckOption |= EMSKILLCHECKOPTION_SUBARMS;
	}

	EMSKILLCHECK emSkillCheck = GLCHARLOGIC::CHECHSKILL ( sSkillID, 1, m_emVehicle, dwSkillCheckOption );

	if ( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP)
	{
		if ( bPrintFail )
		{
			switch ( emSkillCheck )
			{
			case EMSKILL_DELAYTIME:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_DELAYTIME") );
				break;

			case EMSKILL_NOTITEM:
				{	
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE && pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{						
							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_2"),
								CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_LITEM).GetString(),
								CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_RITEM).GetString()));
						}
						else if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE || pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{
							std::string strITEM;
							if (pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_LITEM).GetString();
							if (pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_RITEM).GetString();

							if (strITEM.empty())
								m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM"));						

							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_1"), strITEM.c_str()));
						}
					}
					else
					{
						if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE && pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{						
							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_2"),
								COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_LITEM].c_str(),
								COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_RITEM].c_str()));
						}
						else if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE || pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{
							std::string strITEM;
							if (pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_LITEM].c_str();
							if (pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_RITEM].c_str();

							if (strITEM.empty())
								m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM"));						

							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_1"), strITEM.c_str()));
						}
					}
				}
				
				break;

			case EMSKILL_NOTARROW:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM_ARROW") );
				break;

			case EMSKILL_NOTCHARM:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM_CHARM") );
				break;

			case EMSKILL_NOTHP:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTHP") );
				break;

			case EMSKILL_NOTMP:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTMP") );
				break;

			case EMSKILL_NOTSP:
				GASSERT(0&&"SP�� �����ϴ��� ��ų�� �ߵ��Ǿ� �մϴ�.");
				break;

			case EMSKILL_NOTCP:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTCP") );
				break;

			case EMSKILL_NOTEXP:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTEXP") );
				break;

			case EMSKILL_NOTBULLET:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM_BULLET") );
				break;		

			case EMSKILL_NOTBATTERY:
				break;

			case EMSKILL_NOTDRIVER:
				break;

			default:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_DOFAIL") );
				break;
			};
		}
	}

	return emSkillCheck;
}

//! ���� ��ų�� �ߵ��ϴ� �κ�
const bool GLCharacter::SkillReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_SKILL& sSkillReaction = InOutReaction;

	const SNATIVEID& sSkillID   = sSkillReaction.GetSKILLID();
	const bool	     bPrintFail = sSkillReaction.ISPRINTFAILED();

	//! ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )
	{
		sSkillReaction.SETPRESERVATION(false);
		return false;
	}

	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		EMANI_MAINTYPE eAniMType = pSkill->m_sEXT_DATA.emANIMTYPE;
		EMANI_SUBTYPE  eAniSType = pSkill->m_sEXT_DATA.emANISTYPE;

		if( m_pSkinChar && !m_pSkinChar->IsAni( eAniMType, eAniSType) )
		{
			return false;
		}
	}

	//! ���࿡ ��ų �ߵ��߿� �ٽ� �ߵ��� ��û�������� ó��
	if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
	{
		//! ������ ��Ʈ �������� ���� �ʾ�����
		if ( !IsExceedSkillHitFrameOfLast() )
		{
			// ��ų�����̶�� ����
			if ( sSkillReaction.IsRESERVED() )
			{
				m_ReservationAction.ReRegisterReaction( m_ReservationAction.FrontIter(), sSkillReaction );
			}

			return false;
		}

		////! ��ų ĵ��
		//TurnAction(GLAT_IDLE);
	}

	STARGETID sTargetID = sSkillReaction.GetTARID();

	//! Ÿ�� ��Ž�� �ɼ��� �ִٸ�
	if ( sSkillReaction.IsRETARGETING() )
	{
		const EMIMPACT_TARGET emTARGET = pSkill->m_sBASIC.emIMPACT_TAR;
		switch ( emTARGET )
		{
		case TAR_SPEC:
		case TAR_SELF_TOSPEC:
			{
				STARGETID& sSelectID = GetSelectID();

				//! ���� ������ Ÿ������ �����Ѵ�.
				if ( !sSelectID.ISNULL() )
				{
					sTargetID = sSelectID;
				}
			}
			break;		
		};
	}

	//if ( m_bSTATE_STUN )	
	//    return false;

	//PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	//if ( pLAND && pLAND->IsPeaceZone() )	
   //     return false;

    // Note : ��� NPC���� â�� �ݴ´�.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseAllNpc );

	////	Note : ������ ���.
	//SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	//m_sREACTION.RESET();

	m_idACTIVESKILL = NATIVEID_NULL();

	////	Note : ���� ��ų���� �˻�.
	////
	//if ( !ISLEARNED_SKILL ( m_sActiveSkill ) && !IsDefenseSkill() )	return;

	//! ���빫�⸦ üũ�ؼ� ��� ������ ����� �ٲ��ش�
	EMCHARCLASS		emClass = m_emClass;
	if( emClass == GLCC_EXTREME_M || emClass == GLCC_EXTREME_W ) 
	{
		if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )	
			return false;

		EMSLOT emRHand, emRHand_S;

		if ( IsUseArmSub() )
		{
			emRHand = SLOT_RHAND_S;
			emRHand_S = SLOT_RHAND;
		}
		else
		{
			emRHand = SLOT_RHAND;
			emRHand_S = SLOT_RHAND_S;
		}

		const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
		GLITEM_ATT emITEM_RIGHT = ITEMATT_NOTHING;
		SKILL::GLSKILL_ATT emSKILL_RIGHT = pSkill->m_sBASIC.emUSE_RITEM;

		if ( emSKILL_RIGHT!=SKILL::SKILLATT_NOCARE )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emRHand);

			//	�������� ���ų� ��ų ���� ���� �������� �ƴҶ� 
			if ( !pItem || !CHECHSKILL_ITEM(emSKILL_RIGHT,CONVERT_ITEMATT( pItem->sSuitOp.emAttack ),bHiddenWeapon) )
			{
				// Sub���⸦ üũ�ؼ� ������� ������ �����Ѵ�.
				SITEM* pItem_S = GET_SLOT_ITEMDATA(emRHand_S);
				if ( pItem_S && CHECHSKILL_ITEM(emSKILL_RIGHT,CONVERT_ITEMATT( pItem_S->sSuitOp.emAttack ),bHiddenWeapon) )
				{
                    if ( !IsReqSwapArms() )
                    {
					    GLREACTION_ITER iterReaction = m_ReservationAction.EndIter();

					    //! ���� ����
					    GLREACTION_SWAP_ARMS sReaction;
					    iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
    		
					    //! ���� �翹��
					    m_ReservationAction.ReRegisterReaction( iterReaction, sSkillReaction );
                    }
				}
				else if ( bPrintFail )
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE && pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{						
							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_2"),
								CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_LITEM).GetString(),
								CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_RITEM).GetString()));
						}
						else if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE || pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{
							std::string strITEM;
							if (pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_LITEM).GetString();
							if (pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = CGameTextMan::GetInstance().GetCommentText("SKILLATTACK", pSkill->m_sBASIC.emUSE_RITEM).GetString();

							if (strITEM.empty())
								m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM"));						

							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_1"), strITEM.c_str()));
						}
					}
					else
					{
						if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE && pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{						
							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_2"),
								COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_LITEM].c_str(),
								COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_RITEM].c_str()));
						}
						else if ( pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE || pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE )
						{
							std::string strITEM;
							if (pSkill->m_sBASIC.emUSE_LITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_LITEM].c_str();
							if (pSkill->m_sBASIC.emUSE_RITEM!=SKILL::SKILLATT_NOCARE)
								strITEM = COMMENT::SKILLATTACK[pSkill->m_sBASIC.emUSE_RITEM].c_str();

							if (strITEM.empty())
								m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SKILL_NOTITEM"));						

							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(ID2GAMEINTEXT("SKILL_NOTITEM_1"), strITEM.c_str()));
						}
					}
				}

				return false;
			}
		}
	}

	EMIMPACT_TARGET emTARGET = pSkill->m_sBASIC.emIMPACT_TAR;
	if ( emTARGET != TAR_SELF && emTARGET != TAR_ZONE && emTARGET != TAR_LAND )
	{
		ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID.emCrow, sTargetID.GaeaId );

		if ( !pTarget )
		{
			sSkillReaction.SETPRESERVATION(false);
			return false;
		}
	}

	GLLandManClient* pLandManClient = m_pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return false;

	const D3DXVECTOR3&	vPosition	= GetPosition();
	//! �����Ÿ� üũ
	if( emTARGET != TAR_SELF )
	{
//		if( CheckSkillUsingDistance() )
		const D3DXVECTOR3&	vTarPos		= sTargetID.vPos;
		
		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLandManClient->IsLayerSimpleCollision( vPosition, vTarPos, 5.f ) )
			return false;

		WORD        wAttackRange   = GETBODYRADIUS() + GETSKILLRANGE_TAR(*pSkill);
		WORD        wAttackAbleDis = wAttackRange + 2;
		FLOAT       fDistance;

		if( pSkill->CheckSpecialaAddon( EMSPECA_SWAPPOS ) )
		{ //checking movement distance.
			if( !ClientActor::LineOfSightTest( vTarPos, 10.0f ) )
				return false;
		}
		D3DXVECTOR3		vDistance = vPosition - vTarPos;
		fDistance = D3DXVec3Length( &vDistance );

		// ������ �������� ���� ���
		if( fDistance > wAttackAbleDis )
		{
			GLREACTION_ITER		iterReaction = m_ReservationAction.FrontIter();
			GLREACTION_TRACE	sReaction;
			sReaction.SetTARID( sSkillReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			// RM# 2446 [12/28/2015 gbgim];
			// Ÿ���� ������ ��� �����̵�� ������ �Ұ����ϳ� ��ġ������ ;
			// ��ų ����� ��ġ(sTargetID)�� ����ص�;
			// �� ��ų�� ����ǰ� �߰��� �ٸ������� �����̱� ����;
			// Ŭ���ϸ�(m_sActionID.vPos ���ŵ�, �̶� ���� ��ų�� ��������ϳ�;
			// ���� ��ų �ɼ��� �����Ǿ��ִٸ� ��������);
			// �Ÿ��� �־����ԵǸ鼭 �ٽ� ��ġ ������ �ٽ� �����̵�;
			// �׸��� �ٽ� �Ʒ��� ���� ���� �ɼ��� �翹���;
			// �ش� ������ �ذ��ϱ� ���� ������ ���� ������;
			// �̵��� ���µǴ� ������� ���� ResetReservationAction���� ������ ����ĳ��Ʈ��;
			// ���ܷ� �����������ϰԵǾ��ִ� �κ��� �����Ҽ��ֵ�����;

			// Ÿ���� ������ �Ÿ��� ���� ���ɰŸ����� �� �ָ� �ٽ� Ÿ������ ����
			float	fMoveDist = FLT_MAX;
			if( isAction( GLAT_MOVE ) )	
				fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) ); 

			if( fMoveDist > wAttackAbleDis )
			{
				//	Note : �̵����� ����.
				//
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo   = vTarPos - vPos * wAttackRange;
				vMoveTo.y = vTarPos.y;

				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			} //if

			//! ���� �翹��
			m_ReservationAction.ReRegisterReaction( iterReaction, sSkillReaction );
			return false;
		} //if
	} //if

	//	Note : ��ų ���� ���� �˻�.
	bool bNotLearned = false;
	if( IsDefenseSkill( sSkillID ) )
		bNotLearned = true;

	EMSKILLCHECK emSkillCheck = SkillCheck ( sSkillID, sTargetID, 1, bPrintFail, false, bNotLearned );
	if ( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP )
	{
		sSkillReaction.SETPRESERVATION(false);

		return false;
	}

    //! Ż �� ����������, ��ų�ߵ� �� ��.
    if( m_emVehicle != EMVEHICLE_OFF )
    {
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! Ż�� ������ ����
		GLREACTION_TOGGLE_VEHICLE sReaction;
		sReaction.SetACTIVE(false);
		iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );

		//! ���� �翹��
		m_ReservationAction.ReRegisterReaction( iterReaction, sSkillReaction );
        return true;
    } //if

	//	Note : Skill ��ǥ�� ������.
	//
	WORD wSkillLevel = 0;

	if( IsDefenseSkill(sSkillID) )
	{
		wSkillLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		//SKILL_MAP_ITER learniter = m_ExpSkills.find ( sSkillID.dwID );
		//if( learniter==m_ExpSkills.end() )
          //  return false;	// ��� ��ų�� �ƴ� ���.
		//SCHARSKILL& sSkill = (*learniter).second;

		//wSkillLevel = sSkill.wLevel;

		SCHARSKILL sSkill;
		if( !ISLEARNED_SKILLEX(sSkillID, sSkill) ) return false;

		wSkillLevel = sSkill.wLevel;
	} //if..else

	BOOL	bOK = ::SelectSkillTarget(m_pGaeaClient, sSkillID, wSkillLevel, sTargetID, this, vPosition, m_sSkillTargetData );
	if( !bOK )
	{
		//	��ǥ���� ���ų� ��ǥ ������ ��ȿ���� ������.
		if( bPrintFail )
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NOTARGET_SKILL") );

		sSkillReaction.SETPRESERVATION(false);
		return false;
	} //if
	
	//	Note : ���� ȸ���� �ʿ��� ��� ������ ư��.
	//
	if( emTARGET == TAR_SPEC		|| 
		emTARGET == TAR_SELF_TOSPEC )
	{
		if( m_sSkillTargetData.TargetIDs[0].dwID != GAEAID_NULL )
		{
			STARGETID sRotateTarID (m_sSkillTargetData.TargetIDs[0].emCrow, m_sSkillTargetData.TargetIDs[0].dwID);
			ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sRotateTarID );
			if( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - vPosition;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				ClientActor::SetDirect(vDirection);
			} //if
		} //if

		//	Note : ���� ��ġ�� �������� Transform ��Ʈ���� ���.
		//
		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
		D3DXMatrixRotationY ( &matYRot, fThetaY );

		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
	}
	else if ( emTARGET==TAR_ZONE || emTARGET == TAR_LAND )
	{
		D3DXVECTOR3 vDirection = m_sSkillTargetData.TargetID.vPos - vPosition;
		D3DXVec3Normalize ( &vDirection, &vDirection );
		ClientActor::SetDirect(vDirection);
	}
	else if ( emTARGET==TAR_SELF )
	{
		//	Note : ���� ȸ�� �ʿ� ����.
	}

	//! ��ų�� �ߵ���.

	if( sSkillReaction.IsCHANGERUNSKILL() )
		ReqSkillRunSet(sSkillReaction.GetCHANGERUNSKILL());

	SetActiveSkillRun( sSkillID );

	if( !isAction(GLAT_IDLE) &&
		( pSkill->m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST ) )
	{
		SCHARSKILL		sSkillLevel;
		GetSkillLevel( m_sActiveSkill, sSkillLevel );
		m_pWorkingSkill->Start( sSkillLevel, this, &m_sSkillTargetData, false );
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// GLAT_SKILL ���� GLAT_SKILL_WAIT ���� ����Ǿ���.
		// ��ų ���۽� Effect �� Ÿ�ݽ� Effect �� ������ �ణ�� �����̰� �� ���ɼ��� ������ �۾��Ǿ���.
		//
		TurnAction ( GLAT_SKILL_WAIT );
		PreservedReaction(sSkillReaction);
	} //if..else

	return true;
}

void GLCharacter::SKT_EFF_HOLDOUT ( STARGETID sTarget, DWORD dwDamageFlag )
{
	for (size_t i=0; i<SKILLFACT_SIZE; ++i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKEFF.sNATIVEID.wMainID, sSKEFF.sNATIVEID.wSubID );
		if ( !pSkill )
            continue;

		if ( !pSkill->m_sEXT_DATA.strHOLDOUT.empty() )
		{
			bool bEffect = false;

			for ( DWORD j = 0; j < pSkill->m_sAPPLY.vecSPEC.size(); ++j )
			{
				
				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REDUCE && 
					dwDamageFlag & DAMAGE_TYPE_PSY_REDUCE)
				{
					bEffect = true;
					break;
				}
			
				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REDUCE && 
					dwDamageFlag & DAMAGE_TYPE_MAGIC_REDUCE)
				{
					bEffect = true;
					break;
				}

				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REFLECTION && 
					dwDamageFlag & DAMAGE_TYPE_PSY_REFLECTION)
				{
					bEffect = true;
					break;
				}

				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REFLECTION && 
					 dwDamageFlag & DAMAGE_TYPE_MAGIC_REFLECTION)
				{
					bEffect = true;
					break;
				}

				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_CURSE && 
					 dwDamageFlag & DAMAGE_TYPE_CURSE)
				{
					bEffect = true;
					break;
				}

				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_ILLUSION && 
					dwDamageFlag & DAMAGE_TYPE_ILLUSION)
				{
					bEffect = true;
					break;
				}
			}

			if ( !bEffect )
				continue;


			const char* szZONE_EFF = pSkill->m_sEXT_DATA.strHOLDOUT.c_str();
			
			const D3DXVECTOR3& vPosition= GetPosition();
			D3DXVECTOR3 vDIR = sTarget.vPos - vPosition;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y;
			matTrans._43 = vPosition.z;

			//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, TRUE );
		}
	}
}

void GLCharacter::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	��Ʈ����ũ ��������.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vTARPOS = GetPosition() + GetDirect() * 60.0f;

		//	Ÿ�� ��ġ�� �˾Ƴ�.
		STRIKE::SSTRIKE sStrike;

		bool bAttBone = false;
		if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
		{
			bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
		}
		BOOL bOk = FALSE;

		if ( bAttBone )
		{		
			if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
				return;

			PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
			if ( !pDxAttBone ) 
				return;

			STRIKE::CSELECTOR Selector ( vTARPOS, pDxAttBone->m_AttBoneArray );
			bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );			
		}
		else
		{
			STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
			bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		}

		if ( bOk ) 
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	������� ��ǥ���� ȸ�� �Ӽ� �־.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );		
		matTrans = matYRot * GetTransMatrix();
	}

	//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget, TRUE, TRUE );
}

void GLCharacter::SK_EFF_TARZONE ( const STARGETID &_sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget(_sTarget);

	const D3DXVECTOR3& vTargetPosition = sTarget.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();
	
	//	���� ȸ��
	D3DXVECTOR3 vDIR = vPosition - vTargetPosition;

	D3DXVECTOR3 vDIR_ORG(1,0,0);
	float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

	D3DXMatrixRotationY ( &matTrans, fdir_y );

	//	��Ʈ����ũ ��������.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = m_pGaeaClient->GetSkinChar ( sTarget );

		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vTARPOS ( vPosition.x, vPosition.y+15, vPosition.z );;

		//	Ÿ�� ��ġ�� �˾Ƴ�.
		IMPACT_ARROW::SSTRIKE sStrike;
		IMPACT_ARROW::CSELECTOR Selector ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

//			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
			matTrans._41 = sStrike.vPos.x;
			matTrans._42 = sStrike.vPos.y;
			matTrans._43 = sStrike.vPos.z;
			sTarget.vPos = vTARPOS;
		}
	}

	//	������� ��ǥ��.
	if ( !bTRANS )
	{
		bTRANS = TRUE;
//		D3DXMatrixTranslation ( &matTrans, sTarget.vPos.x, sTarget.vPos.y, sTarget.vPos.z );
		matTrans._41 = vTargetPosition.x;
		matTrans._42 = vTargetPosition.y;
		matTrans._43 = vTargetPosition.z;
	}



	//	

	//	Note : ��ǥ ��ġ ����Ʈ �߻���Ŵ.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, TRUE );
}

void GLCharacter::SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &_sTarget, STARGETID *pResultTarget )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;
	const D3DXVECTOR3& vPosition = GetPosition();

	//	��Ʈ����ũ ��������.
	BOOL bTRANS(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSA == SKILL::EMPOS_STRIKE )
	{
		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vSTRPOS ( sTarget.vPos.x, sTarget.vPos.y+15, sTarget.vPos.z );;

		D3DXVECTOR3 vDir = vSTRPOS - vPosition;
		float fLength = D3DXVec3Length ( &vDir );

		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vSTRPAR = vPosition + vDir*fLength * 10.0f;

		//	��Ʈ����ũ ��ġ�� �˾Ƴ�.
		STRIKE::SSTRIKE sStrike;

		bool bAttBone = false;
		if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
		{
			bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
		}

		bool bExtreme = false;
		const SITEM* pRightItem = GET_SLOT_ITEMDATA( GetCurRHand() );
		if( pRightItem )
		{
			bExtreme = ( pRightItem->sSuitOp.emAttack == ITEMATT_EXTREME_GLOVE && (m_emClass & GLCC_EXTREME) );
		}

		BOOL bOk = FALSE;

		if ( bAttBone )
		{		
			if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
				return;

			PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
			if ( !pDxAttBone ) 
				return;

			STRIKE::CSELECTOR Selector ( vSTRPAR, pDxAttBone->m_AttBoneArray );
			bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		}
		else if( bExtreme )
		{
			STRIKE::CSELECTOR Selector ( vSTRPAR, m_pSkinChar->m_PartArray );
			if( sStrikeEff.m_emPiece == PIECE_RHAND )
				bOk = Selector.SELECT ( PIECE_RHAND_HIDDEN, sStrike );
			else if( sStrikeEff.m_emPiece == PIECE_LHAND  )
				bOk = Selector.SELECT ( PIECE_LHAND_HIDDEN, sStrike );
		}
		else
		{
			STRIKE::CSELECTOR Selector ( vSTRPAR, m_pSkinChar->m_PartArray );
			bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		}

		if ( bOk )
		{
			bTRANS = TRUE;
			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}
	
	//	������� ȸ�� �Ӽ� �־.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * GetTransMatrix();		
	}

	//	��ǥ ���� ����
	BOOL bIMPACT(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSB == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = m_pGaeaClient->GetSkinChar ( sTarget );
		if ( !pSkin )
            return;

		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vTARPOS ( vPosition.x, vPosition.y + 15.0f, vPosition.z );;

		//	Ÿ�� ��ġ�� �˾Ƴ�.
		IMPACT_ARROW::SSTRIKE sImpact;
		IMPACT_ARROW::CSELECTOR Selmpact ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selmpact.SELECT ( sImpact );
		if ( bOk )
		{
			bIMPACT = TRUE;
			sTarget.vPos = sImpact.vPos;
		}
	}

	if ( !bIMPACT )
	{
		//sTarget.vPos = sTarget.vPos;
		sTarget.vPos.y += 15.0f;
	}

	D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARG(emELMT), matTrans, &sTarget, TRUE, TRUE );

	if ( spEffSingleG.get() )
	{
		if( pResultTarget != NULL )
			spEffSingleG->m_ResultTargetID = *pResultTarget;
		else
			spEffSingleG->m_ResultTargetID = STARGETID(CROW_MOB, EMTARGET_NULL);


		//	Note : ��ǥ ��ġ ����Ʈ.

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

		//	Note : ��ǥ �� ����Ʈ.
		if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
	else
	{
		//	Note : ��ǥ ��ġ ����Ʈ.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );

		////	Note : ��ǥ �� ����Ʈ.
		//if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		//if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
}

void GLCharacter::SkillEffect ( const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )
        return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	const D3DXVECTOR3& vPosition = GetPosition();

	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			const D3DXVECTOR3& vPosition = m_sSkillTargetData.TargetID.vPos;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
			if ( spEffSingleG.get() )
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
			}
			else
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
	}
	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//		�ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( m_sSkillTargetData.nTarget == 0 )
            return;

		STARGETID sTarget(
            m_sSkillTargetData.TargetIDs[m_sSkillTargetData.nTarget-1].emCrow,
			m_sSkillTargetData.TargetIDs[m_sSkillTargetData.nTarget-1].dwID);

		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
            return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);

		//	Note : ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ.
		//		( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
		if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
		{
			SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
		}
		//	�ܵ����� ��ǥ ������ �߻�.
		else
		{
			//	Note : ��ǥ ��ġ ����Ʈ.
			if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			//	Note : ��ǥ �� ����Ʈ.
			if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
			if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		}
	}
	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//
	else
	{
		//	Note : ��ǥ ������ �ִ� ���.
		//
		if ( m_sSkillTargetData.nTarget )
		{
			for ( WORD i=0; i<m_sSkillTargetData.nTarget; ++i )
			{
				STARGETID sTarget(
					m_sSkillTargetData.TargetIDs[i].emCrow,
					m_sSkillTargetData.TargetIDs[i].dwID);

				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
                    continue;
				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				//	Note : ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ.
				//		( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
				if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
				{
					SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
				}
				//	�ܵ����� ��ǥ ������ �߻�.
				else
				{
					//	Note : ��ǥ ��ġ ����Ʈ.
					if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

					if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

					//	Note : ��ǥ �� ����Ʈ.
					if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
					if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				}

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )
                    break;
			}			
		}
	}

	STARGETID sTARG(GETCROW(),m_dwGaeaID);

	if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );
}

void GLCharacter::SkillEffectDealyAction( const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillId.wMainID, sSkillId.wSubID );
	if ( !pSkill )
		return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	const D3DXVECTOR3& vPosition = GetPosition();

	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			const D3DXVECTOR3& vPosition = sSkillTargetData->TargetID.vPos;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
			if ( spEffSingleG.get() )
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
			}
			else
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
	}
	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//		�ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( sSkillTargetData->nTarget == 0 )
			return;

		STARGETID sTarget(
			sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].emCrow,
			sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].dwID);

		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
			return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);

		//	Note : ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ.
		//		( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
		if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
		{
			SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
		}
		//	�ܵ����� ��ǥ ������ �߻�.
		else
		{
			//	Note : ��ǥ ��ġ ����Ʈ.
			if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			//	Note : ��ǥ �� ����Ʈ.
			if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
			if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		}
	}
	//	Note : Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	//
	else
	{
		//	Note : ��ǥ ������ �ִ� ���.
		//
		if ( sSkillTargetData->nTarget )
		{
			for ( WORD i=0; i<sSkillTargetData->nTarget; ++i )
			{
				STARGETID sTarget(
					sSkillTargetData->TargetIDs[i].emCrow,
					sSkillTargetData->TargetIDs[i].dwID);

				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
					continue;
				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				//	Note : ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ.
				//		( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
				if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
				{
					SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
				}
				//	�ܵ����� ��ǥ ������ �߻�.
				else
				{
					//	Note : ��ǥ ��ġ ����Ʈ.
					if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

					if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

					//	Note : ��ǥ �� ����Ʈ.
					if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
					if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				}

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )
					break;
			}			
		}
	}

	STARGETID sTARG(GETCROW(),m_dwGaeaID);

	if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );
}

/**
 * ��ų�׼�ó��(GLAT_SKILL)
 *
 * @param fElapsedTime						[in] ���������� ����ð�.
 * @return ��⵿�� ��ȯ �� ��Ȳ�̸� FALSE
 */
BOOL GLCharacter::SkillProc( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime;
	return m_pWorkingSkill->MoveNext();
}

/**
 * �������� ��Ʈ �������� �ʰ� �ߴ°�?
 */
BOOL GLCharacter::IsExceedSkillHitFrameOfLast()
{
	if( !isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
		return TRUE;

	//GASSERT( m_pSkinChar->GETCURANIMNODE() );

	//GASSERT(pAnicontNode);
	const DxSkinAnimation*	pAnimCont = m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if( pAnimCont->m_wStrikeCount == 0 )
		return TRUE;

	if( m_nattSTEP < pAnimCont->m_wStrikeCount )
		return FALSE;

	int nThisKey = int(pAnimCont->m_dwSTime) + int(m_fattTIMER*UNITANIKEY_PERSEC);
	if( nThisKey >= int(pAnimCont->m_dwETime) )
		return TRUE;

	return FALSE;
}

void GLCharacter::RestoreActStateByInvisible()
{
	if ( !IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		return;

	if ( m_sINVISIBLE.bPrevStateRun )
		SetActState(EM_ACT_RUN);
	else
		ReSetActState(EM_ACT_RUN);
}

void GLCharacter::AddSkillFact_SpecialAddon_Client( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF )
{
	SKILL::SSPEC_ADDON_LEVEL	sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1		= sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
	sAddon.sSPEC.fRate		= sSPEC.fRate;	// ȿ�� ���� ����. by luxes.
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	// ȿ�� ���� ����. by luxes.
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );	
} //AddSkillFact_SpecialAddon

void GLCharacter::RECEIVE_SKILLFACT_Dominate(const SKILL::SSPEC& sSPEC)
{
	if ( EMSPEC_DOMINATE_TYPE(sSPEC.dwFLAG) == EMSPEC_DOMINATE_TYPE_FIX )
	{
		m_ReservationAction.Clear();
		TurnAction(GLAT_IDLE);

		// ��Ʈ�� �Ҵ� ���·� ����
		m_bSTATE_CONTROL = false;

		// ���� ��ų�� �����ϱ� ����
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
	}
}

void GLCharacter::RECEIVE_SKILLFACT_Taunt( const STARGETID& sTargetID, const float fDistance )
{
    m_sTaunt.fTauntDistance = fDistance;
    m_sTaunt.sTauntTargetID = sTargetID;
}

void GLCharacter::RECEIVE_SKILLFACT_Invisible(const float fVAR2)
{
	m_sINVISIBLE.bPrevStateRun = IsActState(EM_ACT_RUN);
	switch ( EMSPEC_INVISIBLE_ANI_TYPE(DWORD(fVAR2)) ){
	case EMSPEC_INVISIBLE_ANI_TYPE_WALK:
		ReSetActState(EM_ACT_RUN);
		break;
	case EMSPEC_INVISIBLE_ANI_TYPE_RUN:
		SetActState(EM_ACT_RUN);
		break;
	}
} //GLCharacter::RECEIVE_SKILLFACT_Invisible

void GLCharacter::RECEIVE_SKILLFACT_RandomExpRate(const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const float fLife, const WORD wLevel, SSKILLFACT& sSKILLEF, const float* const pLaunchTime, const float* const pLaunchValue)
{
	if ( pLaunchTime == 0)
		return;

	m_ExperienceRate.clear();

	const DWORD nMaxSize((DWORD)sSPEC.fVAR2);

	for ( DWORD _i(MAX_SIZE); _i--; )
	{
		const float fLife(pLaunchTime[_i]);
		const float fRate(pLaunchValue[_i]);

		m_ExperienceRate.push_back(sSkillID, nMaxSize, fLife, fRate);
	}

	//RECEIVE_SKILLFACT �κп��� ó���ؼ� �ѹ��� ó���� �ʿ�� �����.
	//RecvExperience( sSkillID, wLevel, *pLaunchTime );
}

const BOOL GLCharacter::RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA)
{
	if ( (this->m_bSkillFactsSpecial[EMSPECA_AIRBORNE] == true) && (sSPEC.dwFLAG == 0) )
		return FALSE;

	const float _fTimeLife(sSKILL_DATA.fLIFE);
	const float _fTimeLevitate(sSPEC.fVAR1);
	const float _fHeightMax(sSPEC.fVAR2);	
	this->m_sAirborne.push_back(_fTimeLife, _fTimeLevitate, _fHeightMax);

	return TRUE;
}

const bool GLCharacter::RECEIVE_SKILLFACT_Special(PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, const STARGETID& sID, SSKILLFACT& sSKILLEF, const SNATIVEID& skill_id, const float* const pLaunchTime, const float* const pLaunchValue )
{
	bool bHold = false;

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
        const EMSPEC_ADDON		emAddon = pSkill->m_sAPPLY.vecSPEC[i].emSPEC;
        const SKILL::SSPEC&		sSPEC = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel];
		switch ( emAddon )
		{
		case EMSPECA_COUNTATTACK :
			bHold = TRUE;
			sSKILLEF.pcCAData = pSkill->m_sAPPLY.sCAData.sSPEC_CA[wlevel];
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
        case EMSPECA_TAUNT:
            bHold = true;
            RECEIVE_SKILLFACT_Taunt( sID, sSPEC.fVAR2 );
            AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
            break;
        case EMSPECA_ENDURE_VARY:   // ����
            bHold = true;
            GLCHARLOGIC::SetFactTime( sSPEC.fVAR1, sSPEC.dwFLAG );
            AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
            break;
        case EMSPECA_SKILLDELAY: //����
			{
				const SKILL::SSPEC& sSPEC_SkillDelay = sSPEC;
				bHold = true;
				//GLCHARLOGIC::SET_SKILLDELAY_INC ( sSPEC.fVAR1, sSPEC.fVAR2, sSPEC_SkillDelay.dwFLAG, skill_id );
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC_SkillDelay, sSKILLEF );
			}
            break;
        case EMSPECA_LINK_HP:
			{
            bHold = true;
            GLOGICEX::VARIATION( m_sHP.nMax, INT_MAX, static_cast<int>(static_cast<float>(m_sHP.nMax) * m_sLINKHP.fRef) );
            m_sHP.LIMIT();
            GLCHARLOGIC::SET_MAXHP_RATE ( emAddon, sSPEC, sSKILLEF, m_sLINKHP.fRef );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
			}
            break;
		case EMSPECA_INVISIBLE:
			bHold = true;
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);			
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_DOMINATION:
			bHold = true;
			RECEIVE_SKILLFACT_Dominate(sSPEC);			
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_RANDOM_EXP_RATE:
			bHold = true;
			RECEIVE_SKILLFACT_RandomExpRate(sSPEC, pSkill->GetId(), pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE, wlevel, sSKILLEF, pLaunchTime, pLaunchValue);
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_AIRBORNE:
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, pSkill->m_sAPPLY.sDATA_LVL[wlevel]) == TRUE )
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_ENTRANCE_EXIT :
			{
				RECEIVE_SKILLFACT_EntranceExit( pSkill->GetId(), sSPEC, wlevel, sID);
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_SCREEN_THROW : 
			{
				sSKILLEF.sID = sID;
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_DISGUISE :
			{
				RECEIVE_SKILLFACT_Distinguish( pSkill->GetId(), sSPEC, wlevel, sID );
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_PROVOKE    :
			{
				if( RECEIVE_SKILLFACT_Provoke( pSkill->GetId(), sSPEC, wlevel, sID ) )
				{
					sSKILLEF.sID = sID;
					bHold = TRUE;
					AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
				}				
			}			
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_IGNORED_DAMAGE:
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_ITEMDROP_RATE:
		case EMSPECA_MONEYDROP_RATE:
		case EMSPECA_EXP_RATE:
		case EMSPECA_NPC_TALK:
		case EMSPECA_ITEMDROP_SPECIFIC:
		case EMSPECA_VEHICLE_GETOFF: // ���� : Client
		case EMSPECA_TURN_STATE_BATTLE:
		case EMSPECA_TURN_STATE_PEACE:		
		case EMSPECA_RECVISIBLE:
		case EMSPECA_RELEASE_ENDURE:
		case EMSPECA_RELEASE_PET:
		case EMSPECA_IMMUNE:
		case EMSPECA_STIGMA:
		case EMSPECA_HALLUCINATE:		
		case EMSPECA_ILLUSION:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_VIEW_RANGE_RATE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:		
		case EMSPECA_FEIGN_DEATH :
		case EMSPECA_CAMERA_HIT :
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			bHold = true;
        }
	} //for


	return bHold;
} // GLCharacter::RECEIVE_SKILLFACT_Special

const bool GLCharacter::RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	const size_t nADDON = pSkill->m_sAPPLY.vecADDON.size();

    for( DWORD i = 0; i < nADDON; ++i )
    {		
        SKILL::SIMPACT_ADDON_LEVEL sAddon;

        sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
        sAddon.fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
        sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel]; // ȿ�� ���� ����. by luxes.
        sSKILLEF.vecADDON.push_back( sAddon );
    } //for

    return nADDON ? true : false;
} //GLCharacter::RECEIVE_SKILLFACT_Addon

const bool GLCharacter::RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF ){
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
	case SKILL::EMFOR_SUMMON_ACTIVETIME:		
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fBASIC_VAR;
		return true;
	};

	return false;
} // GLCharacter::RECEIVE_SKILLFACT_Basic


const BOOL GLCharacter::RECEIVE_SKILLCA( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{	
	if( pMsg == NULL ) return FALSE;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	if ( pMsg->wSELSLOT >= SKILLFACT_SIZE )
		return FALSE;

	SSKILLFACT sSKILLEF;

	bool bHold = false;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SSPEC_ADDON>::iterator   _viterI;
	std::vector<SKILL::SSPEC_ADDON> vAddon = pSkill->m_sAPPLY.vecSPEC;

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;
	
	for( int i = 0; i < SKILL::MAX_ADDON; ++i)
	{
		_viterI = vAddon.begin();

		while( _viterI != vAddon.end() )
		{
			if( _viterI->emSPEC == static_cast<EMSPEC_ADDON>(pMsg->sEffInfo.iRecvEAIdx[i]) )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viterI );

				vAddon.erase(_viterI);
				break;
			}

			++_viterI;
		}
		
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( static_cast<EMIMPACT_ADDON>(pMsg->sEffInfo.iRecvSAIdx[i]) ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}
		
	}

	bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, _SKILLFACT_DEFAULTCUSTOMCOUNT, pMsg->sID, sSKILLEF, pMsg->skill_id, 0, 0 );

	if ( bHold )
	{
		sSKILLEF.sNATIVEID	= pMsg->skill_id;
		sSKILLEF.wLEVEL		= pMsg->wLEVEL;
		sSKILLEF.bEnable    = TRUE;

		if ( pMsg->sTIME.IsEXPIREDTIME )
		{
			sSKILLEF.SetAgeInfinite();
			sSKILLEF.nEXPIREDTIME = pMsg->sTIME.IsEXPIREDTIME;
		}
		else
		{
			if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )
			{
				float fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
				sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
				sSKILLEF.fLIFE = sSKILLEF.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1); //����
			}
			else
				sSKILLEF.fAGE = pMsg->sTIME.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
		}

		if (pMsg->wSELSLOT >= SKILLFACT_SIZE)
			return FALSE;

		m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;

		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		{
			m_dwTransformSkill = pMsg->wSELSLOT;
			m_IdSkillTransform = pMsg->skill_id;
			m_sSKILLFACT[pMsg->wSELSLOT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
			m_sSKILLFACT[pMsg->wSELSLOT].bRanderSpecialEffect = FALSE;

			if( m_pSkinChar )
			{
				for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
				{
					if( m_wSex == 1 )
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
					else
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
				}
			}

			// ���� �� �Ŀ� Skin Char�� �ٲ�� ������ ����Ʈ�� �ٽ� �����Ѵ�.
			const D3DXVECTOR3& vPosition = GetPosition();
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
				if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

				FACTEFF::NewSkillFactEffect ( 
					STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
					m_pSkinChar, sSKEFF.sNATIVEID, GetTransMatrix(), GetDirect() );

			}
		}

		return TRUE;
	}

	return FALSE;

}

const BOOL GLCharacter::RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{
	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	if ( pMsg->wSELSLOT >= SKILLFACT_SIZE )
		return FALSE;

	SSKILLFACT sSKILLEF;

	bool bHold = false;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SSPEC_ADDON>::iterator   _viterI;
	std::vector<SKILL::SSPEC_ADDON> vAddon = pSkill->m_sAPPLY.vecSPEC;

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;
	
	for( int i = 0; i < SKILL::MAX_ADDON; ++i)
	{
		_viterI = vAddon.begin();

		while( _viterI != vAddon.end() )
		{
			if( _viterI->emSPEC == static_cast<EMSPEC_ADDON>(pMsg->sEffInfo.iRecvEAIdx[i]) )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viterI );

				vAddon.erase(_viterI);
				break;
			}

			++_viterI;
		}
		
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( static_cast<EMIMPACT_ADDON>(pMsg->sEffInfo.iRecvSAIdx[i]) ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}

	}

	bHold |= RECEIVE_SKILLFACT_Basic( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, _SKILLFACT_DEFAULTCUSTOMCOUNT, pMsg->sID, sSKILLEF, pMsg->skill_id, 0, 0 );

	//	Note : ������ ��ų�� ��� ���� ������ ã�Ƽ� ��ų ȿ���� �־���.
	//
	if ( bHold )
	{
		sSKILLEF.sNATIVEID	= pMsg->skill_id;
		sSKILLEF.wLEVEL		= pMsg->wLEVEL;
		sSKILLEF.bEnable    = TRUE;

		if ( pMsg->sTIME.IsEXPIREDTIME )
		{
			sSKILLEF.SetAgeInfinite();
			sSKILLEF.nEXPIREDTIME = pMsg->sTIME.nEXPIREDTIME;

			// UI ��Ÿ��
			sSKILLEF.fAGE = sSKILLEF.nEXPIREDTIME - m_pGaeaClient->GetCurrentTime().GetTime() + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			if ( pMsg->sTIME.fLIFE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[ pMsg->wLEVEL ].fLIFE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			else
				sSKILLEF.fLIFE = pMsg->sTIME.fLIFE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
		}
		else
		{
			float fSkillLifeInc = 1.0f;
			if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )
			{
				fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
				sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			}
			else
				sSKILLEF.fAGE = pMsg->sTIME.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);

			// UI ��Ÿ��
			if ( pMsg->sTIME.fLIFE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[ pMsg->wLEVEL ].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			else
				sSKILLEF.fLIFE = pMsg->sTIME.fLIFE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
		}

		if (pMsg->wSELSLOT >= SKILLFACT_SIZE)
			return FALSE;

		m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;

		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		{
			m_dwTransformSkill = pMsg->wSELSLOT;
			m_IdSkillTransform = pMsg->skill_id;
			m_sSKILLFACT[pMsg->wSELSLOT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
			m_sSKILLFACT[pMsg->wSELSLOT].bRanderSpecialEffect = FALSE;

			if( m_pSkinChar )
			{
				for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
				{
					if( m_wSex == 1 )
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
					else
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
				}
			}

			// ���� �� �Ŀ� Skin Char�� �ٲ�� ������ ����Ʈ�� �ٽ� �����Ѵ�.
			const D3DXVECTOR3& vPosition = GetPosition();
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
				if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

				FACTEFF::NewSkillFactEffect ( 
					STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
					m_pSkinChar, sSKEFF.sNATIVEID, GetTransMatrix(), GetDirect() );

			}
		}

		m_pGaeaClient->PartyMemberAddBuff( m_dwGaeaID, pMsg->skill_id.dwID, pMsg->wSELSLOT );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Buff_Reg,
			"-dw, -n, -dw, -s, -n, -f, -f",
			pMsg->wSELSLOT,
			(int) NSWIDGET_SCRIPT::SkillFactSlotToType( pMsg->wSELSLOT ),
			pMsg->skill_id.dwID,
			pSkill->GetName(),
			sSKILLEF.wLEVEL,
			sSKILLEF.fAGE,
			sSKILLEF.fLIFE );

		if( (SKILLFACT_INDEX_LUNCHBOX_START <= pMsg->wSELSLOT && SKILLFACT_INDEX_LUNCHBOX_END > pMsg->wSELSLOT) ||
			(SKILLFACT_INDEX_EXP_LUNCHBOX_START <= pMsg->wSELSLOT && SKILLFACT_INDEX_EXP_LUNCHBOX_END > pMsg->wSELSLOT) )
			RecvExperience( pMsg->skill_id, sSKILLEF.wLEVEL, sSKILLEF.fAGE );

		return TRUE;
	}

	//GASSERT ( 0 && "���� ��ų������ ��ȿ���� ����." );

	return FALSE;
}

const BOOL GLCharacter::RECEIVE_SKILLFACT( const SNATIVEID& skill_id, const WORD wlevel, const DWORD dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime, const DWORD dwCount, const float* const pLaunchTime, const float* const pLaunchValue )
{
	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
        return FALSE;

    if ( dwSELECT >= SKILLFACT_SIZE )
        return FALSE;
	
	SSKILLFACT sSKILLEF;

	bool bHold = false;

	bHold |= RECEIVE_SKILLFACT_Basic( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( pSkill, wlevel, dwCount, sID, sSKILLEF, skill_id, pLaunchTime, pLaunchValue );

	//	Note : ������ ��ų�� ��� ���� ������ ã�Ƽ� ��ų ȿ���� �־���.
	//
	if ( bHold )
	{
		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.bEnable    = TRUE;

		float fAge = 0.0f;
		float fLife = 0.0f;
		float fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );

        if ( sTime.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
			sSKILLEF.bIsExpiredTime = true;

			// UI ��Ÿ��
			sSKILLEF.fAGE = sSKILLEF.nEXPIREDTIME - m_pGaeaClient->GetCurrentTime().GetTime() + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			if ( sTime.fLIFE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			else
				sSKILLEF.fLIFE = sTime.fLIFE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
        }
        else
        {
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )
            {
               fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
			   sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
            }
            else
				sSKILLEF.fAGE = sTime.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);

			// UI ��Ÿ��
			if ( sTime.fLIFE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[ wlevel ].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			else
				sSKILLEF.fLIFE = sTime.fLIFE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
        }

		if (dwSELECT >= SKILLFACT_SIZE)
			return FALSE;

		m_sSKILLFACT[dwSELECT] = sSKILLEF;

		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		{
			m_dwTransformSkill = dwSELECT;
			m_sSKILLFACT[dwSELECT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
			m_sSKILLFACT[dwSELECT].bRanderSpecialEffect = FALSE;

			if( m_pSkinChar )
			{
				for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
				{
					if( m_wSex == 1 )
                    {
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() )
                            continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
                    else
                    {
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
                            continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
				}
			}

			// ���� �� �Ŀ� Skin Char�� �ٲ�� ������ ����Ʈ�� �ٽ� �����Ѵ�.
			const D3DXVECTOR3& vPosition = GetPosition();
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
				if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

				FACTEFF::NewSkillFactEffect ( 
					STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
					m_pSkinChar, sSKEFF.sNATIVEID, GetTransMatrix(), GetDirect() );

			}
		}

		m_pGaeaClient->PartyMemberAddBuff( m_dwGaeaID, skill_id.dwID, dwSELECT );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Buff_Reg,
			"-dw, -n, -dw, -s, -n, -f, -f, -b",
			dwSELECT,
			(int) NSWIDGET_SCRIPT::SkillFactSlotToType( dwSELECT ),
			skill_id.dwID,
			pSkill->GetName(),
			sSKILLEF.wLEVEL,
			sSKILLEF.fAGE,
			sSKILLEF.fLIFE,
			sTime.IsEXPIREDTIME );

		if( (SKILLFACT_INDEX_LUNCHBOX_START <= dwSELECT && SKILLFACT_INDEX_LUNCHBOX_END > dwSELECT) ||
			(SKILLFACT_INDEX_EXP_LUNCHBOX_START <= dwSELECT && SKILLFACT_INDEX_EXP_LUNCHBOX_END > dwSELECT) )
			RecvExperience( skill_id, sSKILLEF.wLEVEL, sSKILLEF.fAGE );

		return TRUE;
	}

	//GASSERT ( 0 && "���� ��ų������ ��ȿ���� ����." );

    return FALSE;
}

/**
 * ���� ȿ���� �ð��� ����� ��� Ȥ�� Ư�� ���ǿ� ���� ���� �Ǵ� ��� ȣ�� �Ǵ� �Լ�; 
 * @ Param dwSkillfactIndex		[in] ����� ����ȿ���� �ε���;
**/
void GLCharacter::UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel, BOOL bForceEnd, BOOL bOverLabEnd )
{
	if ( dwSkillfactIndex >= SKILLFACT_SIZE )
		return;

	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[dwSkillfactIndex];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	DxSkinChar* pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform );
	if ( pRenderChar == NULL )
		pRenderChar = m_pSkinChar;

	const size_t nSize = sSKILLFACT.vecSPEC.size();
	for ( unsigned int _i = 0; _i < nSize; ++_i )
	{
		const SKILL::SSPEC_ADDON_LEVEL& spec = sSKILLFACT.vecSPEC[_i];
		switch ( spec.emSPEC )
		{
		case EMSPECA_COUNTATTACK :
			{	
				m_SkillCAIndex.AllReset();
			}
			break;
		case EMSPECA_INVISIBLE:
			if ( m_sINVISIBLE.bPrevStateRun )
				SetActState(EM_ACT_RUN);
			else
				ReSetActState(EM_ACT_RUN);			
			break;
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DOMINATION:
			m_ReservationAction.Clear();
			pRenderChar->DEFAULTANI();	
			pRenderChar->StopLoopSound();
			m_sHALLUCINATE.RESET();
			TurnAction(GLAT_IDLE);
			break;
		case EMSPECA_RANDOM_EXP_RATE:
			m_ExperienceRate.clear();		
			break;
		case EMSPECA_SCREEN_THROW :
			RF_SCREEN_THROW( m_EffSkillVarSet ).ResetValue( spec.sSPEC.fVAR1 );
			break;
		case EMSPECA_CAMERA_HIT :
			RF_CAMERA_HIT( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_FEIGN_DEATH :
			RF_FEIGN_DEATH( m_EffSkillVarSet ).End( dynamic_cast<GLCHARLOGIC*>(this), bForceEnd );
			break;
		case EMSPECA_DISGUISE:
			RF_DISGUISE( m_EffSkillVarSet ).ResetValue();

			Stop_Disguise( );
			break;
		case EMSPECA_PROVOKE:
			RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
			break;
		}
	}

	FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), m_pSkinChar, sSKILLFACT.sNATIVEID);

	m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, sSKILLFACT.sNATIVEID.dwID, dwSkillfactIndex );
	sSKILLFACT.RESET();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Buff_Deleted,
		"-dw", dwSkillfactIndex );
}

void GLCharacter::UPDATE_DATA_Additional(const float fElapsedTime)
{
	const float fIncreaseRate(m_ExperienceRate.updateTime(fElapsedTime));
	if ( fIncreaseRate > 0.0f )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(m_ExperienceRate.getSkillID());
		if ( pSkill != NULL )
		{			
			const DWORD nIncreasePercent(DWORD(fIncreaseRate * 100.0f));
			const std::string strDescription(sc::string::format(ID2GAMEINTEXT("SKILLFACT_ACCUM_COUNT_PERCENT"), nIncreasePercent));			

			const DWORD nAccumCount(m_ExperienceRate.size());
			const DWORD nTotalRate(DWORD(m_ExperienceRate.getTotalRate() * 100.0f));			
			
			const std::string strInformation(sc::string::format(ID2GAMEINTEXT("SKILLFACT_ACCUM_COUNT_INFORMATION"), nAccumCount, nTotalRate));			
			const std::string strFullText(sc::string::format( ID2GAMEINTEXT("SKILLFACT_ACCUM_REMOVE_MESSAGE"), pSkill->GetName(), strDescription, strInformation));
			
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strFullText);

			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_Buff_SetExpAccume,
			//	"-dw, -dw, -dw",
			//	nTotalRate, nAccumCount, m_ExperienceRate.getMaxSize() );
		}
	}

	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID();

		if( !sID.ISNULL() )
		{			
			ClientActor* pActor = GetCrowOnLand( sID.emCrow, sID.GaeaId );

			if( pActor ) {
				ResetReservationAction();
				SetSelectID( sID );
			}

			RF_PROVOKE( m_EffSkillVarSet ).ResetPreTarget();
		}
	}
}

void GLCharacter::MsgDefenseSkillActive(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE* pPacket = (GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE*) pMsg;
    BOOL bMove(FALSE);
    D3DXVECTOR3 vMoveTo;

    PGLSKILL pRunSkill = GLSkillMan::GetInstance().GetData ( pPacket->sNativeID );
    if ( !pRunSkill )
        return;

    ClientActor* pCOPY = m_pGaeaClient->GetCopyActor ( pPacket->emTarCrow, pPacket->dwTarID );
    if ( !pCOPY )
        return;

    if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) && IsDefenseSkill() )
        return;

    // ��ų ĵ�� �޼���
    ResetAction();

    STARGETID	sTID( pPacket->emTarCrow, pPacket->dwTarID);
    sTID.vPos = pCOPY->GetPosition();

    SetDefenseSkill( true );

	GLREACTION_SKILL sReaction;
	sReaction.SetSKILLID(pPacket->sNativeID);
	sReaction.SetTARID(sTID);
	sReaction.SetDEFENSE(true);

    SkillReaction ( sReaction );
}

void GLCharacter::MsgBeginSkill(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_BEGIN_SKILL_FC* pPacket = (GLMSG::SNETPC_BEGIN_SKILL_FC*) pMsg;	

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( pPacket->sSkillID );

	if ( pSkill == NULL )
		return;

	for( DWORD _i = 0; _i < pSkill->m_sAPPLY.vecSPEC.size(); ++_i )
	{
		const EMSPEC_ADDON&		emAddon = pSkill->m_sAPPLY.vecSPEC[_i].emSPEC;
		const SKILL::SSPEC&		sSPEC = pSkill->m_sAPPLY.vecSPEC[_i].sSPEC[pPacket->wLevel];
		switch ( emAddon )
		{			
		case EMSPECA_DOMINATION:
			{
				m_sDOMINATE.bDominate = TRUE;
				m_sDOMINATE.fRadiusMoveable = sSPEC.fVAR2;
				m_sDOMINATE.sControlTargetID = pPacket->sTargetID;				
				TurnAction(GLAT_IDLE);
			}
			break;
		case EMSPECA_HALLUCINATE:
			if ( sSPEC.dwFLAG & EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE )
			{
				GLCharacter::ResetSelectID(); // ���� ��� ID ��;
				GLCharacter::ResetCursorID();
				GLCharacter::ResetActionID();
				this->m_ReservationAction.Clear();
			}		
			break;
		case EMSPECA_RANDOM_EXP_RATE:
			m_ExperienceRate.push_back(pPacket->sSkillID, DWORD(sSPEC.fVAR2), pPacket->fLife, pPacket->fVAR1);
			break;
		case EMSPECA_ENTRANCE_EXIT :
		case EMSPECA_SCREEN_THROW  :
		case EMSPECA_CAMERA_HIT    :
			{
				GLCharacter::ResetSelectID(); // ���� ��� ID ��;
				GLCharacter::ResetCursorID();
				GLCharacter::ResetActionID();
			}
			break;
		case EMSPECA_FEIGN_DEATH :
			{
				GLCharacter::ResetSelectID(); // ���� ��� ID ��;
				GLCharacter::ResetCursorID();
				GLCharacter::ResetActionID();
			}
			break;
		}
	}
}

void GLCharacter::MsgEndSkill(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_END_SKILL_FC* pPacket = (GLMSG::SNETPC_END_SKILL_FC*)pMsg;
	UPDATE_DATA_END(pPacket->dwSkillfactIndex, FALSE, pPacket->bForceEnd, pPacket->bOverLabEnd );
}
void GLCharacter::MsgEndSkillBrd(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_END_SKILL_BRD* pPacket = (GLMSG::SNETPC_END_SKILL_BRD*)pMsg;
	UPDATE_DATA_END(pPacket->dwSkillfactIndex, FALSE, pPacket->bForceEnd );
}

void GLCharacter::MsgReqSkillQFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_SKILLQUICK_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILLQUICK_FB *)pMsg;
    m_sSKILLQUICK[pNetMsg->wSLOT]  = pNetMsg->skill_id;
	m_sChangeQUICK[pNetMsg->wSLOT] = pNetMsg->skill_id;

	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		if( pNetMsg->skill_id.IsValidNativeID() )
		{
			DWORD dwTableID = RF_DISGUISE( m_EffSkillVarSet ).dwTableID - 1;

			GSCL_INST.GetChangeSkill( dwTableID, m_sChangeQUICK[pNetMsg->wSLOT].dwID );
		}		
	}

    //	Note : �� ��ų�� ������ ��ų�� �������� ���� ��� ��������.
    if ( GetskillRunSlot()==NATIVEID_NULL() )
        ReqSkillRunSet(pNetMsg->wSLOT);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlotIndex, "-n", pNetMsg->wSLOT );

	if ( GLMacroManager::GetInstance().IsActive() )
		GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLSLOTINFO_UPDATE);
}

void GLCharacter::ACCOUNTSKILL( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer )
{
	GLCHARLOGIC::ACCOUNTSKILL( sSkill, wStrikeNum, bServer );

	GfxInterfaceBase::GetInstance()->SetSkillSlotCoolDown( sSkill.sNativeID );
}

void GLCharacter::GetSkillCoolDownTime( const SNATIVEID& sID, float& fCoolDownTime, float& fCoolDownRatio )
{
	SKILL::DELAY_MAP_ITER iter = m_SKILLDELAY.find( sID.dwID );
	if( iter == m_SKILLDELAY.end() )
		return;

	SKILL::SKILLDELAY sSkillDelay = (*iter).second;

	fCoolDownTime = sSkillDelay.fDelayTime;
	if( 0.0f != fCoolDownTime )
	{
		if( sSkillDelay.fMAXDelayTime == 0.0f )
			sSkillDelay.fMAXDelayTime = 1.0f;

		fCoolDownRatio = 1.0f - (fCoolDownTime / sSkillDelay.fMAXDelayTime);
	}
}

float GLCharacter::GetSkillFactAge( DWORD dwSkillID )
{
	for( int i = 0; i < SKILLFACT_SIZE; ++i )
	{
		if( dwSkillID != m_sSKILLFACT[ i ].sNATIVEID.dwID )
			continue;

		if( 0 < m_sSKILLFACT[ i ].nEXPIREDTIME )
			return (m_sSKILLFACT[ i ].nEXPIREDTIME - m_pGaeaClient->GetCurrentTime().GetTime());
		else
			return m_sSKILLFACT[ i ].fAGE;
	}

	return 0.0f;
}

LuaTable GLCharacter::GetBuffList()
{
	LuaTable tbBuffList( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableIndex = 1;
	for( int i = 0; i < SKILLFACT_SIZE; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[ i ].sNATIVEID );
		if( NULL == pSkill )
			continue;

		float fAge = 0.0f;
		if( 0 < m_sSKILLFACT[ i ].nEXPIREDTIME )
			fAge = m_sSKILLFACT[ i ].nEXPIREDTIME - m_pGaeaClient->GetCurrentTime().GetTime();
		else
			fAge = m_sSKILLFACT[ i ].fAGE;

		LuaTable tbBuff( GLWidgetScript::GetInstance().GetLuaState() );
		tbBuff.set( 1, i );
		tbBuff.set( 2, NSWIDGET_SCRIPT::SkillFactSlotToType( i ) );
		tbBuff.set( 3, m_sSKILLFACT[ i ].sNATIVEID.dwID );
		tbBuff.set( 4, m_sSKILLFACT[ i ].wLEVEL );
		tbBuff.set( 5, fAge );
		tbBuff.set( 6, m_sSKILLFACT[ i ].fLIFE );
		tbBuff.set( 7, pSkill->GetName() );
		tbBuff.set( 8, m_sSKILLFACT[ i ].bIsExpiredTime );

		tbBuffList.set( nTableIndex++, tbBuff );
	}

	return tbBuffList;
}

void GLCharacter::RecvExperience( const SNATIVEID& sID, WORD wLevel, float fAge )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sID );
	if ( pSkill == NULL )
		return;

	int nBuffIndex = SKILLFACT_INDEX_EXP_LUNCHBOX_START;
	for( int i = 0; i < SKILLFACT_SIZE; ++i )
	{
		if( sID == m_sSKILLFACT[ i ].sNATIVEID )
		{
			nBuffIndex = i;
			break;
		}
	}

	CString strDesc;  
	DWORD dwIncreaseRate(0), dwTotalRate(0), dwAccumCount(1), dwMaxCount(1);
	SKILL::SSPEC_ADDON specAddon;

	if( pSkill->HaveSpec( EMSPECA_RANDOM_EXP_RATE ) == true )
	{
		dwIncreaseRate = DWORD( m_ExperienceRate.getRateLatest() * 100.0f );

		dwAccumCount = (DWORD) m_ExperienceRate.size();
		dwTotalRate = DWORD( m_ExperienceRate.getTotalRate() * 100.0f );
		dwMaxCount = m_ExperienceRate.getMaxSize();
	}
	else if ( pSkill->HaveSpec( EMSPECA_EXP_RATE, specAddon ) == true )
	{
		dwIncreaseRate = dwTotalRate = static_cast<DWORD>(specAddon.sSPEC[0].fVAR1 * 100.0f);
	}

	pSkill->GetDescFormatByLevel( strDesc, wLevel, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );		

	CTime cCurTime( m_pGaeaClient->GetCurrentTime() );
	m_pGaeaClient->GetConvertServerTime( cCurTime );

	int nTime[ 4 ] = { 0 };
	if( 0.0f == fAge )
	{
		SSKILLFACT::GetTime( nTime[ 0 ], nTime[ 1 ], nTime[ 2 ], nTime[ 3 ], 
			cCurTime.GetTime(), pSkill->m_sAPPLY.sDATA_LVL[ wLevel ].fLIFE );
	}
	else
	{
		SSKILLFACT::GetTime( nTime[ 0 ], nTime[ 1 ], nTime[ 2 ], nTime[ 3 ], 
			cCurTime.GetTime(), fAge );
	}

	LuaTable tbTime( GLWidgetScript::GetInstance().GetLuaState() );
	for( int i = 0; i < 4; ++i )
		tbTime.set( i + 1, nTime[ i ] );

	GLWidgetCallFuncValue args[ 8 ];
	args[ 0 ].SetDWORD( dwIncreaseRate );
	args[ 1 ].SetDWORD( dwTotalRate );
	args[ 2 ].SetDWORD( dwAccumCount );
	args[ 3 ].SetUInt( dwMaxCount );
	args[ 4 ].SetString( pSkill->GetName() );
	args[ 5 ].SetString( strDesc.GetBuffer() );
	args[ 6 ].SetTable( &tbTime );
	args[ 7 ].SetInt( nBuffIndex );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Buff_RecvLunchBox, args, 8 );
}

void GLCharacter::Play_ScreenThrow(DWORD dwIndex, const STARGETID& sID )
{
	if( dwIndex >= 	GLCONST_CHAR::SCREEN_EFFECT_NUM	) {
		return;
	}

	if( !m_SkillSTrow.GetEnableCheck( sID, dwIndex, RF_SCREEN_THROW(m_EffSkillVarSet).IsOn(dwIndex)) ) return;

	STARGETID sTargetID( CROW_PC,m_dwGaeaID, GetPosition() );
	
	std::string& strEffFileName = GLCONST_CHAR::strScreenEffect[dwIndex];

	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	if( !sID.ISNULL() )
	{
		ClientActor* pAttacker = m_pGaeaClient->GetCopyActor( sID );

		if( pAttacker ) {
			D3DXVECTOR3 vPos = pAttacker->GetPositionActor();
			
			matTrans._41 = vPos.x;
			matTrans._42 = vPos.y;
			matTrans._43 = vPos.z;
		}
	}
			
	NSEffectPlay::StartPlay( m_pd3dDevice, strEffFileName.c_str(), matTrans, sTargetID );

}

void GLCharacter::Stop_ScreenThrow( DWORD dwIndex )
{
	if( dwIndex >= 	GLCONST_CHAR::SCREEN_EFFECT_NUM	) {
		return;
	}

	std::string& strEffFileName = GLCONST_CHAR::strScreenEffect[dwIndex];

	NSEffectPlay::Delete( strEffFileName.c_str() );
}

void GLCharacter::Play_CameraHit( WORD wEffNum )
{
	if( wEffNum >= GLCONST_CHAR::CAMERA_EFFECT_NUM ) return;

	if( !m_SkillCHit.GetEnableCheck( wEffNum, RF_CAMERA_HIT( m_EffSkillVarSet ).IsOn()) ) return;

	std::string& strEffFileName = GLCONST_CHAR::strCameraEffect[wEffNum];

	STARGETID sTargetID( CROW_PC,m_dwGaeaID, GetPosition() );

	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	NSEffectPlay::StartPlay( m_pd3dDevice, strEffFileName.c_str(), matTrans, sTargetID );
}

void GLCharacter::Stop_CameraHit( WORD wEffNum )
{
	if( wEffNum >= 	GLCONST_CHAR::CAMERA_EFFECT_NUM	) {
		return;
	}

	std::string& strEffFileName = GLCONST_CHAR::strCameraEffect[wEffNum];

	NSEffectPlay::Delete( strEffFileName.c_str() );
}

void GLCharacter::End_FeignDeath( BOOL bForce )
{
	ReSetActState(EM_ACT_FEIGN_DIE);

	EMACTIONTYPE eAction = GetAction();

	if( eAction != GLAT_SKILL && eAction != GLAT_SKILL_WAIT )
		TurnAction(GLAT_IDLE);
}

void GLCharacter::Play_Disguise( int iIdx, BOOL bOn )
{
	if( !bOn || iIdx == 0 ) return;

	DxSkinCharData* pCharData = m_pTransformSet->GetDistinguishCharData( ( m_emClass & GLCC_MAN ) ? 1 : 0, iIdx, true );

	if( pCharData )
	{
		BOOL bAmbientUp = NSLightMapAmbientColor::g_bActiveLightMapAmbientColor ? FALSE : TRUE;

		m_pSkinChar->SetCharData( pCharData, m_pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		m_pSkinChar->DEFAULTANI();
		m_pSkinChar->StopLoopSound();

		// ���⸦ ���� UpdateSuit
		UpdateSuit_Weapon();

		const D3DXVECTOR3& vPosition = GetPosition();

		for ( int i = 0; i < SKILLFACT_SIZE; ++i )
		{
			SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
			if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )
				continue;

			FACTEFF::NewSkillFactEffect 
			( 
				STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
				m_pSkinChar, 
				sSKEFF.sNATIVEID, 
				GetTransMatrix(), 
				GetDirect() 
			);
		}
		
		QuickChangeSkill();

		SCHARSKILL sTemp;
		if( ISLEARNED_SKILLEX( m_sRunSkill, sTemp ) )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_LastSkillID,
				"-dw", sTemp.sNativeID.dwID );
		}
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );

		INIT_DATA ( FALSE, FALSE );

		if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
			BuildRandomQuick();

		if ( GLMacroManager::GetInstance().IsActive() )
			GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLSLOTINFO_UPDATE);

		ResetAction();
	}
}

void GLCharacter::Stop_Disguise( )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().FindData(GLCONST_CHAR::szCharSkin[emIndex]);

	if( pCharData )
	{
		BOOL bAmbientUp = NSLightMapAmbientColor::g_bActiveLightMapAmbientColor ? FALSE : TRUE;

		m_pSkinChar->SetCharData( pCharData, m_pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		UpdateSuit(TRUE);	

		SNATIVEID sID;

		SKILL_MAP_ITER learniter = m_ExpSkills.find ( m_sRunSkill.dwID );

		if ( learniter == m_ExpSkills.end() )
		{
			SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( m_sRunSkill.dwID );

			if( _iter != m_ExpChangeSkills.end() )
			{
				sID = _iter->second.sNativeID;					
			}
		}

		QuickChangeReset();

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_LastSkillID,
			"-dw", sID.dwID );

		INIT_DATA ( FALSE, FALSE );

		if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
			BuildRandomQuick();

		if ( GLMacroManager::GetInstance().IsActive() )
			GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLSLOTINFO_UPDATE);
	}
}

const bool GLCharacter::RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	return false;
}

const bool GLCharacter::RECEIVE_SKILLFACT_Distinguish( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	return true;
}

const bool GLCharacter::RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{	
	if( IsCheckedSkillFlagSpecial( EMSPECA_COUNTATTACK ) || IsCheckedSkillFlagSpecial(EMSPECA_DOMINATION ) )
	{
		return false;
	}

	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		if( RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID().ISNULL() )
			RF_PROVOKE( m_EffSkillVarSet ).SetPreTargetID( m_sSelectID );
	}

	ResetReservationAction();
	SetSelectID( STARGETID(  sID.emCrow, sID.GaeaId ) );

	BuildRandomQuick();

	return true;
}

void GLCharacter::QuickChangeSkill()
{
	BOOL  bChange   = RF_DISGUISE( m_EffSkillVarSet).IsOn();
	int   dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

	bChange = (dwTableID < 0) ? FALSE : bChange;

	if( bChange ) {
		bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( !bChange ) 
	{
		QuickChangeReset();
		return;
	}

	for( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
	{
		m_sChangeQUICK[i] = m_sSKILLQUICK[i];

		GSCL_INST.GetChangeSkill( dwTableID, m_sChangeQUICK[i].dwID );
	}	
}

void GLCharacter::QuickChangeReset()
{
	for( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
	{
		m_sChangeQUICK[i] = NATIVEID_NULL();
	}	
}

BOOL GLCharacter::BuildRandomQuick()
{
	m_vQuickNum.clear();
	
	BOOL bDistinguish =  RF_DISGUISE( m_EffSkillVarSet ).IsOn();

	m_vQuickNum.push_back( 0xFFFF );
	 
	for( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
	{
	    const SNATIVEID& sID = bDistinguish ? m_sChangeQUICK[i] : m_sSKILLQUICK[i];

		if( sID.IsValidNativeID() ) 
		{
			EMSKILLCHECK emSkillCheck = RandomSkillCheck(sID, GetSelectID(), 1, true );

			if( emSkillCheck == EMSKILL_OK || emSkillCheck == EMSKILL_NOTSP ) 
			{
				m_vQuickNum.push_back(i);
			}
		}
	}


	return TRUE;
}

void GLCharacter::UpdateSkillRandomBuild( float fTime, float fElapsedTime )
{	
	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) return;
	if( isAction(GLAT_SKILL, GLAT_SKILL_WAIT))   return;
	if( IsDie() ) return;

	if( m_ReservationAction.IsExistReaction( REACT_SKILL ) ) return;

	if( m_vQuickNum.size() == 0 ) {
		if( !BuildRandomQuick() ) return;
	}

	int iSize = m_vQuickNum.size();

	int RandValue  = sc::Random::RandomNumber(1, iSize ) - 1;

	if( m_vQuickNum[RandValue] == 0xFFFF )
	{
		SetActiveAttack( fTime );
	}
	else
	{
		const SNATIVEID& pSkillID = RF_DISGUISE( m_EffSkillVarSet ).IsOn() ? m_sChangeQUICK[m_vQuickNum[RandValue]] : m_sSKILLQUICK[m_vQuickNum[RandValue]];

		if( pSkillID.IsValidNativeID() )
		{
			EMSKILLCHECK emSkillCheck = GLCHARLOGIC::CHECHSKILL ( pSkillID, 1, m_emVehicle, 0 );

			if( emSkillCheck == EMSKILL_OK || emSkillCheck == EMSKILL_NOTSP )
				SkillActive( pSkillID, m_vQuickNum[RandValue], true );
		}
	}
	
	m_vQuickNum.erase( m_vQuickNum.begin() + RandValue );
}

void GLCharacter::SetActiveAttack( float fTime )
{
	if( m_sSelectID.ISNULL() ) return;

	GLREACTION sReaction;

	switch( m_sSelectID.emCrow )
	{
		case CROW_PC:
			{
				//const bool bcontinue = RANPARAM::bFORCED_ATTACK;
				GLREACTION_PVP sPVPReaction;
				sPVPReaction.COPYATTR( sReaction );
				sPVPReaction.SetTARID( m_sSelectID );
				sPVPReaction.SetPOS( GetActionID().vPos );
				sPVPReaction.SETPRESERVATION( false );
				DoReaction( fTime, sPVPReaction );
			}
			break;
		case CROW_MOB:
			{
				GLREACTION_MOB sMOBReaction;
				sMOBReaction.COPYATTR( sReaction );
				sMOBReaction.SetTARID( m_sSelectID );
				sMOBReaction.SetPOS( GetActionID().vPos );
				sMOBReaction.SETPRESERVATION( false );
				DoReaction( fTime, sMOBReaction );	
			}			
			break;
		default :
			return;
			
	}
}

EMSKILLCHECK GLCharacter::RandomSkillCheck ( const SNATIVEID& sSkillID, const STARGETID& sTargetID, WORD wStrikeNum, bool bCheckOnlyAvailability )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );

	if ( !pSkill )	return EMSKILL_FAILED;
	
	if( pSkill->m_sBASIC.emIMPACT_SIDE != EMIMPACT_SIDE_ENEMY ||
		pSkill->m_sBASIC.emIMPACT_TAR  == TAR_SELF ) return EMSKILL_FAILED;

	GLREACTION_ITER Finditer = m_ReservationAction. Find_Skill_Reaction( sSkillID );
	if ( Finditer != m_ReservationAction.EndIter() )
		return EMSKILL_FAILED;

	SCHARSKILL sSkill;
	
	if( !ISLEARNED_SKILLEX(sSkillID, sSkill) ) return EMSKILL_FAILED;

	WORD wSkillLevel = sSkill.wLevel;

	//BOOL	bOK = ::SelectSkillTarget(m_pGaeaClient, sSkillID, wSkillLevel, sTargetID, this, GetPosition(), m_sSkillTargetData );

	//if( !bOK ) return EMSKILL_FAILED;

	bool bSpecRebirth = false;
	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		if ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_REBIRTH )
		{
			bSpecRebirth = true;
			break;
		}
	}

	if ( !bCheckOnlyAvailability && m_bSTATE_STUN )
	{
		return EMSKILL_FAILED;
	}

	//! ��ȭ�����̶�� �Ұ���
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( pLAND && pLAND->IsPeaceZone() && !bSpecRebirth )	
		return EMSKILL_FAILED;

	// �������� ĳ���ʹ� ��ų�� ����� �� ����;
	if ( m_bCLUB_CERTIFY )
	{
		return EMSKILL_FAILED;
	}

	//! ��ų �Ӽ��� ���� �˻�
	EMIMPACT_TARGET emTARGET = pSkill->m_sBASIC.emIMPACT_TAR;

	if ( emTARGET==TAR_SPEC || emTARGET==TAR_SELF_TOSPEC )
	{
		if ( sTargetID == STARGETID() )
		{			
			return EMSKILL_FAILED;
		}

		ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID.emCrow, sTargetID.GaeaId );
		if ( pTarget )
		{
			// Note : ���� �Ұ����� CROW�ϰ�� ��ų ���
			if ( pTarget->IsNonDamaged() ) return EMSKILL_FAILED;

			//! ��� �˻�
			if ( !IsReActionable( sTargetID, pSkill->m_sBASIC.emIMPACT_SIDE ) )
			{
				return EMSKILL_FAILED;
			}

			if (!bSpecRebirth && pTarget->isAction(GLAT_FALLING, GLAT_DIE) )
			{
				return EMSKILL_FAILED;
			}
		}
		else
		{	
			return EMSKILL_FAILED;
		}
	}

	//!	������ �浹�ϴ��� �˻�.
	//  ��� ���ɼ��� üũ�Ѵٸ� �˻����� �ʴ´�.
	//
	if ( !bCheckOnlyAvailability )
	{
		const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

		if ( emTARGET!=TAR_SELF && sTargetID.GaeaId!=EMTARGET_NULL )
		{
			D3DXVECTOR3 vPos1 = GetPosition();	vPos1.y += 15.0f;
			D3DXVECTOR3 vPos2 = vTarPos;		vPos2.y += 15.0f;

			BOOL bCollision = TRUE;
			D3DXVECTOR3 vCollision;
			LPDXFRAME pDxFrame = NULL;

			DxLandMan *pLandMan = m_pGaeaClient->GetActiveMap()->GetLandMan();
			pLandMan->IsCollision ( vPos1, vPos2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );
			if ( bCollision )						
				return EMSKILL_FAILED;
		}

		const PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( pSkill->m_sEXT_DATA.emANIMTYPE, pSkill->m_sEXT_DATA.emANISTYPE );

		if ( !pAnicont ) return EMSKILL_FAILED;
	}

	DWORD dwSkillCheckOption = 0;

	//! ��밡�������� üũ�Ѵٸ�, ���� ���⵵ üũ�Ѵ�
	if ( bCheckOnlyAvailability )
	{
		dwSkillCheckOption |= EMSKILLCHECKOPTION_SUBARMS;
	}

	EMSKILLCHECK emSkillCheck = GLCHARLOGIC::CHECHSKILL ( sSkillID, 1, m_emVehicle, dwSkillCheckOption );

	return emSkillCheck;
}