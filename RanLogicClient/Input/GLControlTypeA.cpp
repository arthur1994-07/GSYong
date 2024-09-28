#include "../pch.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"

#include "../../RanLogic/Skill/GLSkill.h"

#include "./GLControlTypeA.h"

#include "../RanParamClient.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLControlTypeA::GLControlTypeA( GLGaeaClient* pGaeaClient )
: GLControlBase(pGaeaClient)
{
}

GLControlTypeA::~GLControlTypeA()
{

}

void GLControlTypeA::DoInput( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	//! 스킬 행동 우선일 경우 스킬 정보 가져옴.
	DWORD dwRunSkillKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_RUNSKILL );

	PGLSKILL pRunSkill(NULL);
	if ( dwRunSkillKeyState&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )
	{
		SNATIVEID sRunSkill = pCharacter->GetskillRunSlot();

		SCHARSKILL sTemp;
		if( pCharacter->ISLEARNED_SKILLEX( sRunSkill, sTemp) )
		{
			pRunSkill = GLSkillMan::GetInstance().GetData ( sRunSkill.wMainID, sRunSkill.wSubID );
		}

		//SCHARDATA2::SKILL_MAP_ITER learniter = pCharacter->m_ExpSkills.find ( sRunSkill.dwID );
		//if ( learniter!=pCharacter->m_ExpSkills.end() )
		//{
		//	SCHARSKILL &sSkill = (*learniter).second;
		//	pRunSkill = GLSkillMan::GetInstance().GetData ( sRunSkill.wMainID, sRunSkill.wSubID );
		//}
	}

	//! 피행위자 찾기 속성.
	EMFIND_TAR emFINDTAR(ENFIND_TAR_NULL);
	if ( pRunSkill )
	{
		if ( pRunSkill->IsRebirth() )
		{
			switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
			{
			default:				break;
			case EMIMPACT_SIDE_ENEMY:		break;
			case EMIMPACT_SIDE_PARTY:			emFINDTAR = EMFIND_TAR_OUR_DIE;		break;
			case EMIMPACT_SIDE_ANYBODY:		emFINDTAR = EMFIND_TAR_ANY_DIE;		break;
			};
		}
		else
		{
			switch ( pRunSkill->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_ZONE:
			case TAR_LAND:
				emFINDTAR = ENFIND_TAR_LAND;
				break;

			default:
				switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
				{
				default:				break;
				case EMIMPACT_SIDE_ENEMY:		emFINDTAR = EMFIND_TAR_ENEMY;	break;
				case EMIMPACT_SIDE_PARTY:			emFINDTAR = EMFIND_TAR_OUR;		break;
				case EMIMPACT_SIDE_ANYBODY:		emFINDTAR = EMFIND_TAR_ANY;		break;
				};	
				break;

			};
		}
	}

	//! 행동과 관련된 피행위자 찾기.
	DWORD dwTarP2PMenuKeyState  = m_pKeySetting->GetKeyState( EMSHORTCUTS_TAR_P2PMENU );
	DWORD dwTarOnlyItemKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_TAR_ONLYITEM );
	DWORD dwTarFindOption		= 0;
	
	if ( dwTarP2PMenuKeyState&DXKEY_DOWNED )
	{
		dwTarFindOption |= EMFIND_OPTION_P2PMENU;
	}

	if ( dwTarOnlyItemKeyState&DXKEY_DOWNED )
	{
		dwTarFindOption |= EMFIND_OPTION_ONLYITEM;
	}

	ClientActor* pACTTAR(NULL);
	SACTIONTAR sACTAR = pCharacter->FindActionTarget ( vTargetPt, vFromPt, dwTarFindOption, emFINDTAR );

	bool bBRIGHTEVENT = m_pGaeaClient->IsBRIGHTEVENT();

	//// 타겟이 변경되었으면 커서 타겟 정보 표시
	//if( pCharacter->GetCursorID() != sACTAR.sTARID )
	//	m_pGaeaClient->SetCursorTargetInfo( sACTAR.sTARID );

	//! 타겟 정보 표시
	pCharacter->ResetCursorID();
	pCharacter->SetCursorID(sACTAR.sTARID);

	if( true == m_pGaeaClient->IsVisibleToTargetInfoMouseOver(sACTAR) )
	{
		pACTTAR = m_pGaeaClient->GetCopyActor ( sACTAR.sTARID );

		if( NULL != pACTTAR && false == pACTTAR->IsHideNaming() )
		{
			//#ifdef CH_PARAM
			// 중국 : 오른쪽 버튼으로 타켓 지정시 카메라 제어 잠김.
			// DxViewPort::GetInstance().SetHandling( false );
			//#endif

			D3DXVECTOR3 vCenterTarget = sACTAR.sTARID.vPos;
			vCenterTarget.y += 10.f;

			D3DXVECTOR3 vFromTEMP = vFromPt;
			//!! SF_TODO
			if( FALSE == m_pGaeaClient->IsMapCollsion( vCenterTarget, vFromTEMP, EMCC_CULL_NONE_ONLY_WALL ) )
				GfxInterfaceBase::GetInstance()->SetTargetInfoMouseOver ( sACTAR.sTARID );
		}
	}

	//! 커서 처리
	GLControlBase::DefaultCursorUpdate(fTime, fElapsedTime, sACTAR);

	//! 액션키
	bool bActionBlocking = m_pKeySetting->IsBlockKeyState( EMSHORTCUTS_ACTION_CURSOR );

	m_pKeySetting->ResetBlockKeyState( EMSHORTCUTS_ACTION_CURSOR );
	DWORD dwActionKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_ACTION_CURSOR );

	if ( bActionBlocking )
	{
		m_pKeySetting->BlockKeyState( EMSHORTCUTS_ACTION_CURSOR );
	}

	//! 연속 강제 행동 모드 인지 체크.
	const bool bcontinue = RANPARAM::bFORCED_ATTACK;
	const bool bDominateControl = m_pKeySetting->GetKeyState( EMSHORTCUTS_DOMINATE_CONTROL ) & DXKEY_DOWNED;	

	GLREACTION sReaction;
	bool bSelect(false);
	bool bReaction(false);

	D3DXVECTOR3	vMoveTo(0,0,0); 

	// 오브젝트 Select 만 막음
	BOOL bSelectLock = pCharacter->IsSelectTargetLock();

	if ( !pCharacter->isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) && !bSelectLock)
	{
		STARGETID sTARID = sACTAR.sTARID;				

		if ( pRunSkill )
		{				
			pCharacter->SetSelectID(sTARID);
			DoInputRunSkill(pCharacter, pRunSkill, sACTAR, vFromPt, vTargetPt, sTARID, dwRunSkillKeyState, sReaction, fTime);
		}
		else if ( dwActionKeyState&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )		
			DoInputNotRunSkill(pCharacter, dwActionKeyState, sReaction, sACTAR, sTARID, bcontinue, fTime, bReaction, bSelect);

		if ( bReaction )
			m_pKeySetting->BlockKeyState( EMSHORTCUTS_ACTION_CURSOR );

		if( bSelect )
		{
			if( pCharacter->GetSelectID() != sACTAR.sTARID )
				pCharacter->SetSelectID( sACTAR.sTARID );
			m_pGaeaClient->addActorDamageRecord(sACTAR.sTARID);
		} //if		
	} //if

	//! 이동 처리
	if ( !bReaction )
		GLControlBase::DefaultMovingUpdate(fTime, fElapsedTime);

	if ( !(dwActionKeyState&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP)) )
		m_pKeySetting->ResetBlockKeyState( EMSHORTCUTS_ACTION_CURSOR );
}

void GLControlTypeA::DoUpdate( float fTime, float fElapsedTime )
{
	//! 선택 대상 업데이트
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->SelectTargetUpdate( false );
}

void GLControlTypeA::DoInputRunSkill( GLCharacter* pCharacter, 
									 GLSKILL* pRunSkill, 
									 SACTIONTAR &sACTAR, 
									 D3DXVECTOR3& vFromPt,
									 D3DXVECTOR3& vTargetPt,
									 STARGETID &sTARID, 
									 DWORD dwRunSkillKeyState, 
									 GLREACTION &sReaction, 
									 float fTime ) 
{
	pCharacter->ResetReservationAction();
	pCharacter->SetDefenseSkill( false );

	//	Note : 스킬 발동 시도.
	//
	EMIMPACT_TARGET emTAR = pRunSkill->m_sBASIC.emIMPACT_TAR;
	bool bSKILL_ACT = ( emTAR==TAR_SELF || emTAR==TAR_ZONE || emTAR==TAR_LAND );

	//	Note : 스킬 속성 검사.
	bool bSK2ENEMY(false), bSK2OUR(false), bSK2ANYBODY(false);
	switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
	{
	default:				break;
	case EMIMPACT_SIDE_ENEMY:
		bSK2ENEMY = true;
		break;

	case EMIMPACT_SIDE_PARTY:
		bSK2OUR = true;
		break;

	case EMIMPACT_SIDE_ANYBODY:
		bSK2ANYBODY = true;
		break;
	};

	if ( bSKILL_ACT && sACTAR.emACTAR==EMACTAR_NULL )
	{
		D3DXVECTOR3 vCollisionPos;
		DWORD dwCollisionID;
		BOOL bCollision;
		m_pGaeaClient->GetActiveMap()->GetNaviMesh()->IsCollision
			(
			vFromPt, vTargetPt,
			vCollisionPos, dwCollisionID, bCollision
			);

		if ( bCollision )
		{
			sTARID.vPos = vCollisionPos;
		}
		else
		{
			if ( emTAR==TAR_ZONE || emTAR==TAR_LAND )	
				bSKILL_ACT = false;
		}
	}

	if ( dwRunSkillKeyState&DXKEY_DOWN )
	{
		sReaction.SETPRINTFAILED(true);
	}
	else
	{
		sReaction.SETPRINTFAILED(false);
	}

	GLREACTION_SKILL sSkillReaction;
	sSkillReaction.COPYATTR( sReaction );
	sSkillReaction.SetTARID( sTARID );
	sSkillReaction.SetPOS( pCharacter->GetActionID().vPos );
	sSkillReaction.SetSKILLID( pRunSkill->m_sBASIC.sNATIVEID );

	switch ( sACTAR.emACTAR )
	{
	case EMACTAR_NULL:
		if(bSKILL_ACT)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_SUMMON_ATTACK:
		if(bSK2ENEMY)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_PC_PVP:
		if(bSK2ENEMY)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_MOB:
		if(bSK2ENEMY)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_NPC:
		break;
	case EMACTAR_PC_OUR:
		if(bSK2OUR)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_PC_P2P:
		break;
	case EMACTAR_PC_ANY:
		if(bSK2ANYBODY)
			pCharacter->DoReaction(fTime,sSkillReaction);
		break;
	case EMACTAR_ITEM:
		break;
	case EMACTAR_MATERIAL:
		break;
	case EMACTAR_SUMMON_NPC:
		break;
	};
}

void GLControlTypeA::DoInputNotRunSkill( GLCharacter* pCharacter, 
										DWORD dwActionKeyState, 
										GLREACTION &sReaction, 
										SACTIONTAR &sACTAR, 
										STARGETID &sTARID,
										bool bcontinue, 
										float fTime, 
										bool &bReaction, 
										bool &bSelect ) 
{
	pCharacter->ResetReservationAction();

	if ( pCharacter->GetDominate().emController != EMSPEC_DOMINATE_TYPE_SELF )
		return;

	if ( dwActionKeyState&DXKEY_DOWN )
	{
		sReaction.SETPRINTFAILED(true);
	}
	else
	{
		sReaction.SETPRINTFAILED(false);
	}

    //도발에 걸려있을 때 다른 적 대상에게 액션을 취할 수가 없다.
    if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) 
         && ( sACTAR.emACTAR == EMACTAR_PC_PVP || sACTAR.emACTAR == EMACTAR_PC_P2P || sACTAR.emACTAR == EMACTAR_MOB )
         && ( sTARID != pCharacter->m_sTaunt.sTauntTargetID ) )
    {
        return;
    }
	
	if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) && 
		 ( sACTAR.emACTAR == EMACTAR_PC_PVP || sACTAR.emACTAR == EMACTAR_PC_P2P || sACTAR.emACTAR == EMACTAR_MOB ) && 
		 ( sTARID != RF_PROVOKE( pCharacter->m_EffSkillVarSet ).GetHostID() ) )
	{
		return;
	}
	//	Note : 상대에 따른 반응.
	//
	switch ( sACTAR.emACTAR )
	{
	case EMACTAR_NULL:	
	case EMACTAR_PC_OUR:
	case EMACTAR_PC_ANY:
		break;

	case EMACTAR_SUMMON_ATTACK:
	case EMACTAR_PC_PVP:
		{
			GLREACTION_PVP sPVPReaction;
			sPVPReaction.COPYATTR( sReaction );
			sPVPReaction.SetTARID( sTARID );
			sPVPReaction.SetPOS( pCharacter->GetActionID().vPos );
			sPVPReaction.SETPRESERVATION( bcontinue );
			pCharacter->DoReaction( fTime, sPVPReaction );
			bReaction = true;
		}	
		break;

	case EMACTAR_MOB:
		{
			GLREACTION_MOB sMOBReaction;
			sMOBReaction.COPYATTR( sReaction );
			sMOBReaction.SetTARID( sTARID );
			sMOBReaction.SetPOS( pCharacter->GetActionID().vPos );
			sMOBReaction.SETPRESERVATION( bcontinue );
			pCharacter->DoReaction( fTime, sMOBReaction );	
			bReaction = true;
		}
		break;

	case EMACTAR_PC_P2P:
		{
            // Note : 아래 주석은 실수로 여겨져 주석처리. 정상동작 확인 후 삭제.
			//if ( dwActionKeyState&(DXKEY_UP|DXKEY_DUP) )
			//{
				GLREACTION_P2P sP2PReaction;
				sP2PReaction.COPYATTR( sReaction );
				sP2PReaction.SetTARID( sTARID );
				sP2PReaction.SetPOS( pCharacter->GetActionID().vPos );
				pCharacter->DoReaction( fTime, sP2PReaction );
				bReaction = true;
			//}
		}
		break;
	case EMACTAR_SUMMON_NPC:
		{
			GLREACTION_SUMMON_NPC sNPCReaction;
			sNPCReaction.COPYATTR( sReaction );
			sNPCReaction.SetTARID( sTARID );
			sNPCReaction.SetPOS( pCharacter->GetActionID().vPos );

			if ( dwActionKeyState&(DXKEY_UP|DXKEY_DUP) )
			{
				sNPCReaction.SetONLYTRACE(false);
			}
			else
			{
				sNPCReaction.SetONLYTRACE(true);
			}

			pCharacter->DoReaction( fTime, sNPCReaction );
			bReaction = true;
		}
		break;
	case EMACTAR_NPC:
		{
			GLREACTION_NPC sNPCReaction;
			sNPCReaction.COPYATTR( sReaction );
			sNPCReaction.SetTARID( sTARID );
			sNPCReaction.SetPOS( pCharacter->GetActionID().vPos );

			if ( dwActionKeyState&(DXKEY_UP|DXKEY_DUP) )
			{
				sNPCReaction.SetONLYTRACE(false);
			}
			else
			{
				sNPCReaction.SetONLYTRACE(true);
			}

			pCharacter->DoReaction( fTime, sNPCReaction );
			bReaction = true;
		}
		break;

	case EMACTAR_ITEM:
		{
			GLREACTION_ITEM sItemReaction;
			sItemReaction.COPYATTR( sReaction );
			sItemReaction.SetTARID( sTARID );
			sItemReaction.SetPOS( pCharacter->GetActionID().vPos );

			if ( dwActionKeyState&(DXKEY_UP|DXKEY_DUP) )
			{
				sItemReaction.SetONLYTRACE(false);
			}
			else
			{
				sItemReaction.SetONLYTRACE(true);
			}

			pCharacter->DoReaction( fTime, sItemReaction );
			bReaction = true;
		}		
		break;

	case EMACTAR_MATERIAL:
		{
			GLREACTION_GATHERING sGatheringReaction;
			sGatheringReaction.COPYATTR( sReaction );
			sGatheringReaction.SetTARID( sTARID );
			sGatheringReaction.SetPOS( pCharacter->GetActionID().vPos );

			if ( dwActionKeyState&(DXKEY_UP|DXKEY_DUP) )
			{
				sGatheringReaction.SetONLYTRACE(false);
			}
			else
			{
				sGatheringReaction.SetONLYTRACE(true);
			}

			pCharacter->DoReaction( fTime, sGatheringReaction );	
			bReaction = true;
		}	
		break;
	};

	if( sACTAR.emACTAR == EMACTAR_PC_PVP || sACTAR.emACTAR == EMACTAR_PC_P2P || 
		sACTAR.emACTAR == EMACTAR_MOB || sACTAR.emACTAR == EMACTAR_NPC || sACTAR.emACTAR == EMACTAR_SUMMON_ATTACK ||
		sACTAR.emACTAR == EMACTAR_SUMMON_NPC)
	{
		bSelect = true;
	}
	else
	{
		DxEffGameHelp::GetInstance().BlindTargetEff();
		pCharacter->ResetSelectID();
	}
}

// 카메라 컨트롤;
void GLControlTypeA::DoInputCamera( float fTime, float fElapsedTime )
{
	if ( !IsCharacterBlock( fTime, fElapsedTime ) && !IsCameraBlock() )
		DefaultCameraUpdate(fTime, fElapsedTime); 
}