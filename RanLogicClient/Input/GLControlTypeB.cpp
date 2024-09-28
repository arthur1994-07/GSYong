#include "../pch.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"

#include "../../RanLogic/Skill/GLSkill.h"

#include "./GLControlTypeB.h"

#include "../RanParamClient.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Macro/MacroManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLControlTypeB::GLControlTypeB( GLGaeaClient* pGaeaClient )
: GLControlBase(pGaeaClient)
{
}

GLControlTypeB::~GLControlTypeB()
{

}

void GLControlTypeB::DoInit()
{
}

void GLControlTypeB::DoInput( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

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

	ClientActor* pCursorTar(NULL);
	SACTIONTAR sCursorTar = SACTIONTAR();
	sCursorTar = pCharacter->FindActionTarget ( vTargetPt, vFromPt, dwTarFindOption, ENFIND_TAR_NULL ); 

	bool bBRIGHTEVENT = m_pGaeaClient->IsBRIGHTEVENT();

	// 타겟이 변경되었으면 커서 타겟 정보 표시
	//if( pCharacter->GetCursorID() != sCursorTar.sTARID )
	//	m_pGaeaClient->SetCursorTargetInfo( sCursorTar.sTARID );

	//! 타겟 정보 표시
	pCharacter->ResetCursorID();
	pCharacter->SetCursorID( sCursorTar.sTARID );

	if ( m_pGaeaClient->IsVisibleToTargetInfoMouseOver(sCursorTar) )
	{
		pCursorTar = m_pGaeaClient->GetCopyActor ( sCursorTar.sTARID );

		if ( pCursorTar && pCursorTar->IsHideNaming() == false )
		{
			//#ifdef CH_PARAM
			// 중국 : 오른쪽 버튼으로 타켓 지정시 카메라 제어 잠김.
			// DxViewPort::GetInstance().SetHandling( false );
			//#endif	 	

			D3DXVECTOR3 vCenterTarget = sCursorTar.sTARID.vPos;
			vCenterTarget.y += 10.f;

			D3DXVECTOR3 vFromTEMP = vFromPt;
			if( !m_pGaeaClient->IsMapCollsion( vCenterTarget, vFromTEMP, EMCC_CULL_NONE_ONLY_WALL ) )
			{
				//!! SF_TODO
				GfxInterfaceBase::GetInstance()->SetTargetInfoMouseOver ( sCursorTar.sTARID );
			}
		}
	}

	//! 커서 처리
	GLControlBase::DefaultCursorUpdate(fTime, fElapsedTime, sCursorTar);

    DWORD dwActionCursorKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_RUNSKILL );
    DWORD dwActionSelectKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_ACTION_SELECT );
    DWORD dwActionKeyState		 = 0;
    bool  bActionCursor			 = false;
    bool  bAction				 = false;
    SACTIONTAR sActionTar = SACTIONTAR();

	//! 주변 타겟 선택 관련 처리
	DWORD dwSelectAroundKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_TARGET_SELECT_AROUND );
	if ( dwSelectAroundKeyState&DXKEY_DOWN )
	{
		DoSelectAround();
	}

	//! 타겟 선택 관련 처리
	SKILLID sScopeSkillID = pCharacter->GetScopeSkillRun();

	//! 범위 지정 스킬 발동
	if ( sScopeSkillID != SKILLID_NULL )
	{
		m_pKeySetting->ResetBlockKeyState( EMSHORTCUTS_TARGET_SELECT );
		DWORD dwTargetSelectKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_TARGET_SELECT );
		m_pKeySetting->BlockKeyState( EMSHORTCUTS_TARGET_SELECT );

		if ( dwTargetSelectKeyState&DXKEY_UP )
		{
			pCharacter->ScopeSkillActive();
		}
	}
	//! 타겟 선택 처리
	else
	{
		// 오브젝트 Select 만 막음
		BOOL bSelectLock = pCharacter->IsSelectTargetLock();

		bool bBlocking = m_pKeySetting->IsBlockKeyState( EMSHORTCUTS_TARGET_SELECT );

		m_pKeySetting->ResetBlockKeyState( EMSHORTCUTS_TARGET_SELECT );
		DWORD dwTargetSelectKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_TARGET_SELECT );

        if ( dwTargetSelectKeyState&(DXKEY_DOWN|DXKEY_UP) && !bSelectLock )
		{
			switch(sCursorTar.emACTAR)
			{
            case EMACTAR_ITEM:
                {
                    dwActionKeyState  = dwTargetSelectKeyState;
                    sActionTar.sTARID = sCursorTar.sTARID;
                    bActionCursor	  = true;
                    sActionTar.emACTAR = EMACTAR_ITEM;
                    bAction			  = pCharacter->GetDominate().emController == EMSPEC_DOMINATE_TYPE_SELF;
                    bBlocking = true;
                }
                break;
			case EMACTAR_PC_PVP:
			case EMACTAR_PC_OUR:
			case EMACTAR_PC_ANY:
			case EMACTAR_MOB:
			case EMACTAR_NPC:
			case EMACTAR_SUMMON_ATTACK:
			case EMACTAR_SUMMON_NPC :
				{
					// 입력이 먼저 들어오면 입력으로 타켓을 바꾼다.
					if( !RF_PROVOKE( pCharacter->m_EffSkillVarSet ).IsOn() )
						RF_PROVOKE( pCharacter->m_EffSkillVarSet ).ResetPreTarget();

					pCharacter->SetSelectID( sCursorTar.sTARID );
					m_pGaeaClient->addActorDamageRecord(sCursorTar.sTARID);
					bBlocking = true;
				}
				break;
            case EMACTAR_PC_P2P:
                {
					// 입력이 먼저 들어오면 입력으로 타켓을 바꾼다.
					if( !RF_PROVOKE( pCharacter->m_EffSkillVarSet ).IsOn() )
						RF_PROVOKE( pCharacter->m_EffSkillVarSet ).ResetPreTarget();

                    pCharacter->SetSelectID( sCursorTar.sTARID );
                    bBlocking = true;

                    if (dwTarFindOption & EMFIND_OPTION_P2PMENU )
                    {
                        dwActionKeyState  = dwActionCursorKeyState;
                        sActionTar.sTARID = sCursorTar.sTARID;
                        bActionCursor	  = true;
                        sActionTar.emACTAR = EMACTAR_PC_P2P;
                        bAction = pCharacter->GetDominate().emController == EMSPEC_DOMINATE_TYPE_SELF;
                    }

                }
			};
		}
		else if ( dwTargetSelectKeyState&DXKEY_IDLE || dwTargetSelectKeyState == 0 )
		{
			bBlocking = false;
		}

		if ( bBlocking )
		{
			m_pKeySetting->BlockKeyState( EMSHORTCUTS_TARGET_SELECT );
		}
		else
		{
			m_pKeySetting->ResetBlockKeyState( EMSHORTCUTS_TARGET_SELECT );
		}
	}

	//! 어택/액션키 발동
	if ( dwActionCursorKeyState&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )
	{
		dwActionKeyState  = dwActionCursorKeyState;
		sActionTar.sTARID = sCursorTar.sTARID;
		bActionCursor	  = true;		
		bAction			  = pCharacter->GetDominate().emController == EMSPEC_DOMINATE_TYPE_SELF;
	}
	else if ( dwActionSelectKeyState&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )
	{
		dwActionKeyState  = dwActionSelectKeyState;
		sActionTar.sTARID = pCharacter->GetSelectID();

		if ( sActionTar.sTARID != TARGETID_NULL )
		{
			bAction	= pCharacter->GetDominate().emController == EMSPEC_DOMINATE_TYPE_SELF;
		}
	}

	//! 대상에 따른 액션 처리
	if ( bAction )
	{
		switch( sActionTar.sTARID.emCrow )
		{
		case CROW_PC:
			{
                if ( !(dwTarP2PMenuKeyState&(DXKEY_DOWNED))  )
                {
                    if ( pCharacter->IsReActionable( sActionTar.sTARID, EMIMPACT_SIDE_ENEMY ) )
				    {
					    sActionTar.emACTAR = EMACTAR_PC_PVP;
				    }
				    else
				    {
					    sActionTar.emACTAR = EMACTAR_PC_ANY;
				    }
                }
			}
			break;

		case CROW_MOB:
			{
				sActionTar.emACTAR = EMACTAR_MOB;
			}
			break;

		case CROW_NPC:
			{
				sActionTar.emACTAR = EMACTAR_NPC;
			}
			break;

		case CROW_SUMMON:
			{
				if ( pCharacter->IsReActionable( sActionTar.sTARID, EMIMPACT_SIDE_ENEMY ) )
				{
					sActionTar.emACTAR = EMACTAR_SUMMON_ATTACK;
				}
				else
				{
					if( sCursorTar.emACTAR == EMACTAR_SUMMON_NPC )
						sActionTar.emACTAR = EMACTAR_SUMMON_NPC;
					else
						sActionTar.emACTAR = EMACTAR_PC_ANY;
				}
			}
			break;

            //마우스 좌클릭으로 작동하도록 수정
		case CROW_ITEM:
		case CROW_MONEY:
			//{
			//	sActionTar.emACTAR = EMACTAR_ITEM;
			//}
			break;

		case CROW_MATERIAL:
			{
				sActionTar.emACTAR = EMACTAR_MATERIAL;
			}
			break;

		default:
			{
				sActionTar.sTARID  = TARGETID_NULL;
				sActionTar.emACTAR = EMACTAR_NULL;
			}
		};
	}

	//! 연속 강제 행동 모드 인지 체크.
	const bool bcontinue = RANPARAM::bFORCED_ATTACK;	
	const bool bDominateControl = m_pKeySetting->GetKeyState( EMSHORTCUTS_DOMINATE_CONTROL ) & DXKEY_DOWNED;	

	GLREACTION sReaction;
	bool bReaction(false);

	D3DXVECTOR3	vMoveTo(0,0,0); 
	if ( bAction && (sActionTar.sTARID != TARGETID_NULL) )
	{
		bReaction = DoInputReaction(sActionTar, pCharacter, dwActionKeyState, bActionCursor, sReaction, bcontinue, fTime);
	}

	//! 이동 처리
	if ( !bReaction )
	{
		GLControlBase::DefaultMovingUpdate(fTime, fElapsedTime);
	}

	// [1/7/2013 gbgim]
	// - 타겟(몹 제외, 플레이어만)의 상태를 체크하고 쓰러지는 중이라면 타겟을 품
	// - 부활 스킬 관련으로 문제로인해서 추가함
	{
		std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetChar( pCharacter->GetSelectID().GaeaId );
		if( spChar && spChar->GetAction() == EMACTIONTYPE(GLAT_FALLING) )
		{
			pCharacter->ResetSelectID();
		}
	}
}

void GLControlTypeB::DoUpdate( float fTime, float fElapsedTime )
{
	//! 선택 대상 업데이트
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->SelectTargetUpdate( true );

	//! 범위 지정스킬 업데이트
	pCharacter->ScopeSkillUpdate();
}

bool GLControlTypeB::DoInputReaction( SACTIONTAR &sActionTar, 
									 GLCharacter* pCharacter, 
									 DWORD dwActionKeyState, 
									 bool bCursorAction,
									 GLREACTION &sReaction, 
									 bool bcontinue,
									 float fTime )
{
	if ( !pCharacter )
	{
		return false;
	}

	bool bReaction(false);
	STARGETID sTARID = sActionTar.sTARID;

	if ( dwActionKeyState&DXKEY_DOWN )
	{
		sReaction.SETPRINTFAILED(true);
	}
	else
	{
		sReaction.SETPRINTFAILED(false);
	}

	//	Note : 만약 커서액션이라면 현재 등록된 스킬을 사용한다.
	if ( bCursorAction )
	{
		switch ( sActionTar.emACTAR )
		{
		case EMACTAR_SUMMON_ATTACK:
		case EMACTAR_PC_PVP:
		case EMACTAR_MOB:
			{		
				if ( dwActionKeyState&DXKEY_DOWN )
				{
					SNATIVEID sRunSkill = pCharacter->GetskillRunSlot();

					if ( !sRunSkill.IsValidNativeID() )
					{
						break;
					}
					
					SCHARSKILL sTemp;

					if( !pCharacter->ISLEARNED_SKILLEX( sRunSkill, sTemp) )
					{
						break;
					}

					//SCHARDATA2::SKILL_MAP_ITER learniter = pCharacter->m_ExpSkills.find ( sRunSkill.dwID );
					//if ( learniter==pCharacter->m_ExpSkills.end() )
					//{
					//	break;
					//}

					WORD wFindSlot = EMSKILLQUICK_SIZE;

					for ( WORD i=0; i<EMSKILLQUICK_SIZE; ++i )
					{
						if( pCharacter->IsCheckedSkillFlagSpecial(EMSPECA_DISGUISE) )
						{
							if( sRunSkill == pCharacter->GetChangeQuick(i) )
							{
								wFindSlot = i;
								break;
							}
						}
						else
						{
							if ( sRunSkill == pCharacter->m_sSKILLQUICK[i] )
							{
								wFindSlot = i;
								break;
							}
						}						
					}

					if ( wFindSlot == EMSKILLQUICK_SIZE )
					{
						break;
					}

					PGLSKILL pRunSkill = GLSkillMan::GetInstance().GetData ( sRunSkill.wMainID, sRunSkill.wSubID );

					if ( !pRunSkill )
					{
						break;
					}

					//! 스킬의 속성에 따라 타겟을 변경한다.
					EMIMPACT_TARGET emTARGET = pRunSkill->m_sBASIC.emIMPACT_TAR;

					switch ( emTARGET )
					{
					case TAR_SELF:
					case TAR_SPEC:
					case TAR_SELF_TOSPEC:
						{
							//! 모든 예약 취소
							pCharacter->ResetReservationAction();
							pCharacter->SetSelectID( sTARID );
						}
					};

					const BOOL bSkillActive(pCharacter->SkillActive( wFindSlot ));
					if ( (bSkillActive == TRUE) && GLMacroManager::GetInstance().IsActive() )
						GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_LOCK_USERINPUT);
				}
				return true;
			}
			break;
		};
	}

	//! 모든 예약 취소
	pCharacter->ResetReservationAction();

	//	Note : 상대에 따른 반응.
	//
	switch ( sActionTar.emACTAR )
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
			GLREACTION_P2P sP2PReaction;
			sP2PReaction.COPYATTR( sReaction );
			sP2PReaction.SetTARID( sTARID );
			sP2PReaction.SetPOS( pCharacter->GetActionID().vPos );
			pCharacter->DoReaction( fTime, sP2PReaction );
			bReaction = true;
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
	
	//! 리액션한 대상을 선택한다
	switch ( sActionTar.emACTAR )
	{
	case EMACTAR_SUMMON_ATTACK:
	case EMACTAR_PC_PVP:
	case EMACTAR_PC_P2P:
	case EMACTAR_MOB:
	case EMACTAR_NPC:
	case EMACTAR_SUMMON_NPC:
		{
			pCharacter->SetSelectID( sTARID );
		}
		break;
	};

	return bReaction;
}



//! 주변 타겟 선택 로직
void GLControlTypeB::DoSelectAround()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	GASSERT(m_pKeySetting);
	GASSERT(pCharacter);

    //도발이 걸려있다면 주변 타켓을 선택하는 기능이 안된다.
    if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        return;
    }

	if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
	{
		return;
	}

	STARGETID sTarget = m_pKeySetting->DoLogic_SelectAround(pCharacter);
	pCharacter->SetSelectID( sTarget, false );
}

// 카메라 컨트롤;
void GLControlTypeB::DoInputCamera( float fTime, float fElapsedTime ) 
{ 
	if ( !IsCharacterBlock( fTime, fElapsedTime ) && !IsCameraBlock() )
		DefaultCameraUpdate(fTime, fElapsedTime); 
}