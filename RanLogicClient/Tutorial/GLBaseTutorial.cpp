#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/NSGUI.h"
#include "../../EngineLib/GUInterface/UIMan.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../RanUI/OldUI/Quest/QuestWindow.h"
#include "../../RanUI/OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"

#include "../Land/GLLandManClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "./GLBaseTutorial.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLBaseTutorial::GLBaseTutorial(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
    , m_bTutorial(false)
    , m_bControlMouse(true)
    , m_bMove(true)
    , m_bStep9UI(FALSE)
    , m_bLevelUp( false )
    , m_bFreeStep( false )
    , m_dwMainStep(INTRODUCE_STEP)
    , m_dwSubStep(ONE_)
{
    m_TutorialSound.push_back("tutorial\\001.wav");
    m_TutorialSound.push_back("tutorial\\002.wav");
    m_TutorialSound.push_back("tutorial\\003.wav");
    m_TutorialSound.push_back("tutorial\\004.wav");
    m_TutorialSound.push_back("tutorial\\005.wav");
    m_TutorialSound.push_back("tutorial\\006.wav");
    m_TutorialSound.push_back("tutorial\\007.wav");
    m_TutorialSound.push_back("tutorial\\008.wav");
    m_TutorialSound.push_back("tutorial\\009.wav");
    m_TutorialSound.push_back("tutorial\\010.wav");

    m_TutorialSound.push_back("tutorial\\011.wav");
    m_TutorialSound.push_back("tutorial\\012.wav");
    m_TutorialSound.push_back("tutorial\\013.wav");
    m_TutorialSound.push_back("tutorial\\014.wav");
    m_TutorialSound.push_back("tutorial\\015.wav");
    m_TutorialSound.push_back("tutorial\\016.wav");
    m_TutorialSound.push_back("tutorial\\017.wav");
    m_TutorialSound.push_back("tutorial\\018.wav");
    m_TutorialSound.push_back("tutorial\\019.wav");
    m_TutorialSound.push_back("tutorial\\020.wav");

    m_TutorialSound.push_back("tutorial\\021.wav");
    m_TutorialSound.push_back("tutorial\\022.wav");
    m_TutorialSound.push_back("tutorial\\023.wav");
    m_TutorialSound.push_back("tutorial\\024.wav");
    m_TutorialSound.push_back("tutorial\\025.wav");
    m_TutorialSound.push_back("tutorial\\026.wav");
    m_TutorialSound.push_back("tutorial\\027.wav");
    m_TutorialSound.push_back("tutorial\\028.wav");
    m_TutorialSound.push_back("tutorial\\029.wav");
    m_TutorialSound.push_back("tutorial\\030.wav");

    m_TutorialSound.push_back("tutorial\\031.wav");
    m_TutorialSound.push_back("tutorial\\032.wav");
    m_TutorialSound.push_back("tutorial\\033.wav");
    m_TutorialSound.push_back("tutorial\\034.wav");
    m_TutorialSound.push_back("tutorial\\035.wav");
    m_TutorialSound.push_back("tutorial\\036.wav");
    m_TutorialSound.push_back("tutorial\\037.wav");
    m_TutorialSound.push_back("tutorial\\038.wav");
    m_TutorialSound.push_back("tutorial\\039.wav"); // 반복
    m_TutorialSound.push_back("tutorial\\040.wav");

    m_TutorialSound.push_back("tutorial\\041.wav");
    m_TutorialSound.push_back("tutorial\\042.wav");
    m_TutorialSound.push_back("tutorial\\043.wav");
    m_TutorialSound.push_back("tutorial\\044.wav");
    m_TutorialSound.push_back("tutorial\\045.wav");
    m_TutorialSound.push_back("tutorial\\046.wav");
    m_TutorialSound.push_back("tutorial\\047.wav");
    m_TutorialSound.push_back("tutorial\\048.wav");
    m_TutorialSound.push_back("tutorial\\049.wav");
    m_TutorialSound.push_back("tutorial\\050.wav");

    m_TutorialSound.push_back("tutorial\\051.wav");
    m_TutorialSound.push_back("tutorial\\101.wav");
    m_TutorialSound.push_back("tutorial\\102.wav");
    m_TutorialSound.push_back("tutorial\\103.wav");
    m_TutorialSound.push_back("tutorial\\104.wav");
    m_TutorialSound.push_back("tutorial\\105.wav");
    m_TutorialSound.push_back("tutorial\\106.wav");
    m_TutorialSound.push_back("tutorial\\107.wav");
    m_TutorialSound.push_back("tutorial\\108.wav");
    m_TutorialSound.push_back("tutorial\\109.wav");

    m_TutorialSound.push_back("tutorial\\110.wav");
    m_TutorialSound.push_back("tutorial\\111.wav");
    m_TutorialSound.push_back("tutorial\\112.wav");
    m_TutorialSound.push_back("tutorial\\113.wav");
    m_TutorialSound.push_back("tutorial\\114.wav");
    m_TutorialSound.push_back("tutorial\\115.wav");
    m_TutorialSound.push_back("tutorial\\116.wav");
    m_TutorialSound.push_back("tutorial\\117.wav");
}

GLBaseTutorial::~GLBaseTutorial()
{
}

HRESULT GLBaseTutorial::InitData()
{
	m_bTutorial = false;
	m_bControlMouse = true;
	m_bMove = true;
	m_bStep9UI = FALSE;

	DxViewPort::GetInstance().SetTutorial( false );

	return TRUE;
}

HRESULT GLBaseTutorial::StartTutorialInClient( GLCharacter* pGLChar )
{
	m_bTutorial = true;
	DxViewPort::GetInstance().SetTutorial( true );

	m_pGLChar = pGLChar;

	m_dwMainStep = INTRODUCE_STEP;
	m_dwSubStep = ONE_;

	D3DXVECTOR3 vCamera = DxViewPort::GetInstance().GetFromPt();
	vCamera.y = 100.0f;
	DxViewPort::GetInstance().CameraJump ( vCamera );
	DxViewPort::GetInstance().SetTutorialCameraPos( vCamera );

	m_fCameraLenght = 95.0f;
	DxViewPort::GetInstance().SetTutorialCameraLength( m_fCameraLenght );

	m_fThetaX = 0.2f;
	m_fThetaY = 1.5f;

	DxViewPort::GetInstance().SetTutorialCameraThetaX( m_fThetaX );
	DxViewPort::GetInstance().SetTutorialCameraThetaY( m_fThetaY );

	DxViewPort::GetInstance().SetTutorialCamera();

	m_vStepFivePos = D3DXVECTOR3( 101, 0, 36 );
	m_fLMouseMoveCheck = 0.0f;
	m_fRMouseMoveCheck = 0.0f;
	m_fZoomIn = 0.0f;
	mfZoomOut = 0.0f;
	m_bClickMove[ 0 ] = false;
	m_bClickMove[ 1 ] = false;
	m_vClickMoveTargetPos[ 0 ] = D3DXVECTOR3( -50, 0, 106 );
	m_vClickMoveTargetPos[ 1 ] = D3DXVECTOR3( 74, 0, -78 );
	m_iMinimapTargetX = 30;
	m_iMinimapTargetY = 15;
	m_vMinimapTargetPos = D3DXVECTOR3( 161, 0, -122 );
	m_sHoldSkill = NATIVEID_NULL();
	m_vCameraRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_iHitCount = 0;
	m_sSkillID[ 0 ] = NATIVEID_NULL();
	m_sSkillID[ 1 ] = NATIVEID_NULL();
	m_bUseSkill[ 0 ] = false;
	m_bUseSkill[ 1 ] = false;
	m_sUseSkillID = NATIVEID_NULL();

	switch( pGLChar->m_emClass )
	{
	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
		{	// 검도
			SNATIVEID skill_id( 7, 8 );
			//	Note : 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 0 ] = skill_id;
			}

			skill_id = SNATIVEID( 7, 9 );
			//	Note : 스킬 정보 가져옴.
			pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 1 ] = skill_id;
			}
		}
		break;

	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
		{	// 격투
			SNATIVEID skill_id( 3, 8 );
			//	Note : 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 0 ] = skill_id;
			}

			skill_id = SNATIVEID( 3, 9 );
			//	Note : 스킬 정보 가져옴.
			pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 1 ] = skill_id;
			}
		}
		break;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
		{	// 양궁
			SNATIVEID skill_id( 11, 9 );
			//	Note : 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 0 ] = skill_id;
			}

			skill_id = SNATIVEID( 11, 10 );
			//	Note : 스킬 정보 가져옴.
			pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 1 ] = skill_id;
			}
		}
		break;

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
		{	// 기예
			SNATIVEID skill_id( 15, 6 );
			//	Note : 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 0 ] = skill_id;
			}

			skill_id = SNATIVEID( 15, 7 );
			//	Note : 스킬 정보 가져옴.
			pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 1 ] = skill_id;
			}
		}
		break;

	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
		{	// 과학부
			SNATIVEID skill_id( 39, 2 );
			//	Note : 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 0 ] = skill_id;
			}

			skill_id = SNATIVEID( 39, 5 );
			//	Note : 스킬 정보 가져옴.
			pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( pSkill )
			{
				m_sSkillID[ 1 ] = skill_id;
			}
		}
		break;

    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
        {
            // TODO : 인술부 튜토리얼 기본 스킬 장착. 이걸 파일로 빼던지 해야겠다.
        }
        break;

	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
		{
			// TODO : 인술부 튜토리얼 기본 스킬 장착. 이걸 파일로 빼던지 해야겠다.
		}
		break;

    case GLCC_ETC_M:
    case GLCC_ETC_W:
        {
            // TODO : 튜토리얼 기본 스킬 장착. 이걸 파일로 빼던지 해야겠다.
        }
        break;
	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
		{
			// TODO : 튜토리얼 기본 스킬 장착. 이걸 파일로 빼던지 해야겠다.
		}
		break;
	}

	m_bStep9UI = FALSE;
	m_bLevelUp = false;
	m_bFreeStep = false;
	m_bDrop = false;
	m_dwQuest = NATIVEID_NULL().dwID;
	m_dwQuestUI = NATIVEID_NULL().dwID;
	m_sItemID = SNATIVEID( 4, 9 );
	m_sMokgacID = SNATIVEID( 8, 25 );
	m_sMobID = SNATIVEID( 0, 44 );
	m_bUICheck = false;

	return S_OK;
}

HRESULT GLBaseTutorial::SetTutorialStep( int dwMainStep, int dwSubStep )
{
	m_dwMainStep = dwMainStep;
	m_dwSubStep = dwSubStep;

	return S_OK;
}

HRESULT GLBaseTutorial::NextMainStep()
{
	m_dwMainStep++;
	m_dwSubStep = ONE_;

	return S_OK;
}

HRESULT GLBaseTutorial::NextSubStep()
{
	m_dwSubStep++;

	switch ( m_dwMainStep )
	{
	case INTRODUCE_STEP:
		m_dwMainStep++;
		m_dwSubStep = ONE_;
		break;

	case EXPLAIN_INTERFACE_STEP:
		if ( m_dwSubStep > TEN_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;

	case MOUSECONTROL_AND_MOVE_STEP:
		if( m_dwSubStep > SIXTEEN_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;

	case QUICKSLOT_ACTIONSLOT_STEP:
		if( m_dwSubStep > TEN_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;

	case ATTACK_STEP:
		if( m_dwSubStep > NINE_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;

	case QUEST_AND_HUNT_STEP:
		if( m_dwSubStep > NINE_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;

	case LEVEL_UP_STEP:
		if ( m_dwSubStep > TWO_ )
		{
			// Memo : 퀘스트 받고 설명 단계 전으로 회귀.
			m_dwMainStep = QUEST_AND_HUNT_STEP;
			m_dwSubStep = FOUR_;
		}

	case QUEST_WINDOW_STEP:
		if ( m_dwSubStep > NINE_ )
		{
			m_dwMainStep = QUEST_AND_HUNT_STEP;
			m_dwSubStep = THREE_;
		}
		break;

	default:
		if ( m_dwSubStep > TEN_ )
		{
			m_dwMainStep++;
			m_dwSubStep = ONE_;
		}
		break;
	}

	return S_OK;
}

bool GLBaseTutorial::IsCanMoveInTutorial()
{
	return m_bMove;
}

bool GLBaseTutorial::IsCanMouseControlInTutorial()
{
	return m_bControlMouse;
}

bool GLBaseTutorial::IsSaveCharInTutorial()
{
	if ( m_dwMainStep == ATTACK_STEP )
	{
		return true;
	}

	return false;
}

int GLBaseTutorial::GetCurrentMainStep()
{
	return m_dwMainStep;
}
int GLBaseTutorial::GetCurrentSubStep()
{
	return m_dwSubStep;
}


D3DXVECTOR3 GLBaseTutorial::GetDefaultCameraPos()
{
	return m_vCameraPos;
}

float GLBaseTutorial::GetDefaultCameraLength()
{
	return m_fCameraLenght;
}

float GLBaseTutorial::GetDefaultCameraThetaX()
{
	return m_fThetaX;
}

float GLBaseTutorial::GetDefaultCameraThetaY()
{
	return m_fThetaY;
}

D3DXVECTOR3 GLBaseTutorial::GetStep5Pos()
{
	return m_vStepFivePos;
}

bool GLBaseTutorial::IsTutorial()
{
	return m_bTutorial;
}

void GLBaseTutorial::FrameMove( float fElapsedTime, GLCharacter* pGLChar )
{
#if 0
	m_bControlMouse = false;
	m_bMove = false;

	if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_VIEW_STEP ) )
	{
		m_pInterface->OpenTutorialViewStep();
	}

	switch ( m_dwMainStep )
	{
	case INTRODUCE_STEP:
		{
			m_bControlMouse = false;
			m_bMove = false;

			m_pInterface->SetTutorialViewStep( 0 );

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_SKIP_BUTTON ) )
			{
				m_pInterface->UiShowGroupTop( TUTORIAL_SKIP_BUTTON );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				D3DXVECTOR3 vTarPos;
				vTarPos = pGLChar->GetPosition();
				vTarPos.z += 5.0f;

				pGLChar->ActionMoveTo ( fElapsedTime, vTarPos+D3DXVECTOR3(0,+500,0), vTarPos+D3DXVECTOR3(0,-500,0), FALSE, TRUE );
			}
		}
		break;

	case EXPLAIN_INTERFACE_STEP:
		{
			m_bControlMouse = false;
			m_bMove = false;

			if ( m_dwSubStep == ONE_ )
			{
				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();
				}

				if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiShowGroupTop ( EXPLAIN_INTERFACE );
				}
			}
			else if ( m_dwSubStep == TWO_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}
			}
			else if ( m_dwSubStep == THREE_ )
			{
				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();
				}
			}
			else if ( m_dwSubStep == NINE_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiHideGroup ( EXPLAIN_INTERFACE );
				}
			}
			else if ( m_dwSubStep >= TEN_ )
			{
				m_pInterface->SetTutorialViewStep( 1 );

				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_THREE_0" );
				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT(strName) );

				NextMainStep();
			}
		}
		break;

	case MOUSECONTROL_AND_MOVE_STEP:
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_dwSubStep >= TWO_ && m_dwSubStep <= NINE_ )
			{
				m_bMove = false;
				m_bControlMouse = true;
			}

			if ( m_dwSubStep == TWO_ )
			{	// 마우스 컨트롤 좌, 우 배우기 단계.
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_12" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}
				}

				if ( !m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					const UIRECT& rcMouseTipWindow = m_pInterface->MouseTipGetGlobalPos();
					UIRECT rcChange = rcMouseTipWindow;

					rcChange.sizeX *= 0.8f;
					rcChange.sizeY *= 0.8f;

					m_pInterface->MouseTipResizeControl(rcMouseTipWindow, rcChange);
					m_pInterface->OpenMouseTip();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_LEFT ) )
				{
					m_pInterface->OpenTutorialInterfaceLeft();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_RIGHT ) )
				{
					m_pInterface->OpenTutorialInterfaceRight();
				}
			}
			else if ( m_dwSubStep == FOUR_ )
			{	// 마우스 컨트롤 상, 하 배우기 준비 단계.
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_LEFT ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_LEFT );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_RIGHT ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_RIGHT );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					m_pInterface->UiHideGroup( MOUSETIP_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_3" ) );
				}
			}
			else if ( m_dwSubStep == FIVE_ )
			{	// 마우스 컨트롤 상, 하 배우기 단계.
				if ( !m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					m_pInterface->OpenMouseTip();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_UP ) )
				{
					m_pInterface->OpenTutorialInterfaceUp();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_DOWN ) )
				{
					m_pInterface->OpenTutorialInterfaceDown();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_13" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 1 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}
			}
			else if ( m_dwSubStep == SEVEN_ )
			{	// 마우스 컨트롤 줌 인, 아웃 배우기 준비 단계.
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_UP ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_UP );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_DOWN ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_DOWN );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					m_pInterface->UiHideGroup( MOUSETIP_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_4" ) );
				}
			}
			else if ( m_dwSubStep == EIGHT_ )
			{	// 마우스 컨트롤 줌 인, 아웃 배우기 단계.
				if ( !m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					m_pInterface->OpenMouseTip();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_ZOOM_IN ) )
				{
					m_pInterface->OpenTutorialInterfaceZoomIn();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_ZOOM_OUT ) )
				{
					m_pInterface->OpenTutorialInterfaceZoomOut();
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_14" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 2 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}
			}
			else if ( m_dwSubStep == TEN_ )
			{
				m_bMove = false;
				m_bControlMouse = false;

				m_pInterface->SetTutorialViewStep( 2 );

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_ZOOM_IN ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_ZOOM_IN );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_INTERFACE_ZOOM_OUT ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_INTERFACE_ZOOM_OUT );
				}

				if ( m_pInterface->UiIsVisibleGroup( MOUSETIP_WINDOW ) )
				{
					m_pInterface->UiHideGroup( MOUSETIP_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogueIndex(5);

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_5" ) );
				}
			}
			else if ( m_dwSubStep == ELEVEN_ )
			{	// 특정 위치 도달 확인.
				m_bMove = true;
				m_bControlMouse = true;

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_0" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 3 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}

					// 바닥 effect.
					STARGETID vTARID( CROW_PC, m_pGaeaClient->GETMYGAEAID() + 0 );
					D3DXMATRIX matTrans;
					D3DXMatrixTranslation( &matTrans, m_vClickMoveTargetPos[ 0 ].x, m_vClickMoveTargetPos[ 0 ].y, m_vClickMoveTargetPos[ 0 ].z );

					DxEffGroupPlayer::GetInstance().NewEffGroup ( "t_ground.egp", matTrans, &vTARID, TRUE, FALSE );
					//
				}

				{
					D3DXVECTOR3 vPOS = m_vClickMoveTargetPos[ 0 ];
					D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPOS, NULL );

					long lResolution = CUIMan::GetResolution ();
					WORD X_RES = HIWORD(lResolution) - 30;
					WORD Y_RES = LOWORD(lResolution) - 30;

					if ( (vScreen.x<20) || (vScreen.y<20) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
					{
						if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
						}
					}
					else
					{
						if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->OpenBalloonText();
						}

					}
				}

				if ( !m_bClickMove[ 0 ] )
				{
					D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosition();
					vPos.y = 0.0f;
					D3DXVECTOR3 vTarPos = m_vClickMoveTargetPos[ 0 ];
					vTarPos.y = 0.0f;

					float fDist = D3DXVec3Length( &( vTarPos - vPos ) );

					if ( fDist <= 10.0f )
					{
						//m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );

						STARGETID vTARID(CROW_PC, m_pGaeaClient->GETMYGAEAID() + 0 );

						DxEffGroupPlayer::GetInstance().DeletePassiveEffect( vTARID );

						m_bClickMove[ 0 ] = true;

						if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
						}

						NextSubStep();
					}
				}
			}
			else if ( m_dwSubStep == TWELVE_ )
			{	// 특정 위치 이동 실습 단계 중간 대화창 띄우기.
				m_bMove = false;
				m_bControlMouse = false;

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogueIndex(8);

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_8" ) );
				}
			}
			else if ( m_dwSubStep == THIRTEEN_ )
			{	// 특정 위치 도달 확인.
				m_bMove = true;
				m_bControlMouse = true;

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_0" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 3 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}

					// 바닥 effect.
					STARGETID vTARID( CROW_PC, m_pGaeaClient->GETMYGAEAID() + 1 );
					D3DXMATRIX matTrans;
					D3DXMatrixTranslation( &matTrans, m_vClickMoveTargetPos[ 1 ].x, m_vClickMoveTargetPos[ 1 ].y, m_vClickMoveTargetPos[ 1 ].z );

					DxEffGroupPlayer::GetInstance().NewEffGroup ( "t_ground.egp", matTrans, &vTARID, TRUE, FALSE );
					//
				}

				{
					D3DXVECTOR3 vPOS = m_vClickMoveTargetPos[ 1 ];
					D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPOS, NULL );

					long lResolution = CUIMan::GetResolution ();
					WORD X_RES = HIWORD(lResolution) - 30;
					WORD Y_RES = LOWORD(lResolution) - 30;

					if ( (vScreen.x<20) || (vScreen.y<20) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
					{
						if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
						}
					}
					else
					{
						if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->OpenBalloonText();
						}

					}
				}

				if ( !m_bClickMove[ 1 ] )
				{
					D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosition();
					vPos.y = 0.0f;
					D3DXVECTOR3 vTarPos = m_vClickMoveTargetPos[ 1 ];
					vTarPos.y = 0.0f;

					float fDist = D3DXVec3Length( &( vTarPos - vPos ) );

					if ( fDist <= 10.0f )
					{
						//m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );

						STARGETID vTARID(CROW_PC, m_pGaeaClient->GETMYGAEAID() + 1 );

						DxEffGroupPlayer::GetInstance().DeletePassiveEffect( vTARID );

						m_bClickMove[ 1 ] = true;

						if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
						{
							m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
						}

						NextSubStep();
					}
				}
			}
			else if ( m_dwSubStep == FOURTEEN_ )
			{
				m_bMove = false;
				m_bControlMouse = false;

				m_pInterface->SetTutorialViewStep( 3 );

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogueIndex(9);

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_9" ) );
				}
			}
			else if ( m_dwSubStep == FIFTEEN_ )
			{	// 미니맵 클릭을 통한 특정 위치 도달 확인.
				m_bMove = true;
				m_bControlMouse = true;

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( !m_bUICheck )
				{	// 아직 미니맵을 열지 않았다면,
					if ( m_pInterface->UiIsVisibleGroup( MAP_WINDOW ) )
					{
						m_bUICheck = true;

						if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
						{
							m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
						}

						// 바닥 effect.
						STARGETID vTARID( CROW_PC, m_pGaeaClient->GETMYGAEAID() + 2 );
						D3DXMATRIX matTrans;
						D3DXMatrixTranslation( &matTrans, m_vMinimapTargetPos.x, m_vMinimapTargetPos.y, m_vMinimapTargetPos.z );

						DxEffGroupPlayer::GetInstance().NewEffGroup ( "t_ground.egp", matTrans, &vTARID, TRUE, FALSE );
						//
					}
					else
					{
						if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
						{
							m_pInterface->OpenTutorialDialogueExplainText();

							m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_9" ) );

							{
								// 음성 멈춤.
								StopSound();

								// 음성 재생.
								/*CString strTemp;
								strTemp.Format( "TUTO_SOUNDS%03d", 47 + 4 );
								std::string strFile = strTemp;

								if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
									DxSoundLib::GetInstance()->PlaySound( strFile );*/
							}
						}

						return;
					}
				}

				if ( m_pInterface->UiIsVisibleGroup( MAP_WINDOW ) )
				{
					// Memo : 미니맵을 나중에 열어야, 미니맵 클릭시 미니맵 UI가 판단하게 된다.
					if (m_pInterface->MinimapTargetIsEnable())
					{
						if ( !m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
							m_pInterface->UiShowGroupTop( MINIMAP_TARGET );
					}
					else
					{
						if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
							m_pInterface->UiHideGroup( MINIMAP_TARGET );
					}

					if ( !m_pInterface->UiIsVisibleGroup ( MAP_WINDOW ) )
					{
						m_pInterface->UiShowGroupFocus ( MAP_WINDOW );
					}
					//

					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_15" ) );

						{
							// 음성 멈춤.
							StopSound();

							// 음성 재생.
							/*CString strTemp;
							strTemp.Format( "TUTO_SOUNDS%03d", 47 + 4 );
							std::string strFile = strTemp;

							if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
								DxSoundLib::GetInstance()->PlaySound( strFile );*/
						}
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
					{
						m_pInterface->UiHideGroup( MINIMAP_TARGET );
					}

					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}

				D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosition();
				if ( vPos.x >= m_vMinimapTargetPos.x - 20.0f && vPos.x <= m_vMinimapTargetPos.x + 20.0f &&
					vPos.z >= m_vMinimapTargetPos.z - 20.0f && vPos.z <= m_vMinimapTargetPos.z + 20.0f )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );

					STARGETID vTARID(CROW_PC, m_pGaeaClient->GETMYGAEAID() + 2 );

					DxEffGroupPlayer::GetInstance().DeletePassiveEffect( vTARID );

					m_bUICheck = false;
					NextSubStep();
				}
			}
			else if ( m_dwSubStep == SIXTEEN_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
				{
					m_pInterface->UiHideGroup( MINIMAP_TARGET );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					m_pInterface->SetTutorialDialogueIndex(11);

					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_THREE_11" ) );
				}
			}
		}
		break;

	case QUICKSLOT_ACTIONSLOT_STEP:
		if ( m_dwSubStep == ONE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				//m_pInterface->SetTutorialDialogueIndex(0);
				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FOUR_0" ) );
			}
		}
		else if ( m_dwSubStep == TWO_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			m_pInterface->SetTutorialViewStep( 4 );

			// 인벤토리 우선 닫기.
			if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
			{
				if( m_pInterface->UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) )
					m_pInterface->UiHideGroup( INVENTORY_WINDOW_RENEW );
			}
			else
			{
				if( m_pInterface->UiIsVisibleGroup( INVENTORY_WINDOW ) )
					m_pInterface->UiHideGroup( INVENTORY_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
			{
				m_pInterface->UiHideGroup ( EXPLAIN_INTERFACE );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			NextSubStep();
		}
		else if ( m_dwSubStep == THREE_ )
		{	// 퀵슬롯 특정 아이템 등록 확인.
			m_bMove = false;
			m_bControlMouse = false;

			if ( !m_bUICheck )
			{
				// 아직 인벤토리를 열지 않았다면,
				if ( m_pInterface->IsInventoryOpen() )
				{
					m_bUICheck = true;

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}
				}
				else
				{
					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_10" ) );

						{
							// 음성 멈춤.
							StopSound();

							// 음성 재생.
							/*CString strTemp;
							strTemp.Format( "TUTO_SOUNDS%03d", 47 + 5 );
							std::string strFile = strTemp;

							if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
								DxSoundLib::GetInstance()->PlaySound( strFile );*/
						}
					}

					return;
				}
			}

			if ( m_pInterface->IsInventoryOpen() )
			{
				if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiShowGroupTop ( EXPLAIN_INTERFACE );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_1" ) );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 6 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}

				}

				if (m_pInterface->ExplainIsSlotBoxVisibleAtInven())
				{
					if (!m_pInterface->UiIsVisibleGroup(BALLOON_TEXT_WINDOW))
					{
						m_pInterface->OpenBalloonText();
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}
			}
			else
			{
				if ( !pGLChar->VALID_HOLD_ITEM() )
				{	// 인벤도 안 열렸고, 손에 들린거 없을때.
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}

			for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
			{
				if ( pGLChar->m_sACTIONQUICK[ i ].sNID == m_sItemID )
				{
					// 인벤토리 창 켜져 있으면 끄자.
					if( m_pInterface->IsInventoryOpen() )
					{
						if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
							m_pInterface->UiHideGroup( INVENTORY_WINDOW_RENEW );
						else
							m_pInterface->UiHideGroup( INVENTORY_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					m_bUICheck = false;
					NextSubStep();
				}
			}
		}
		else if ( m_dwSubStep == FOUR_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			m_pInterface->SetTutorialViewStep( 5 );

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FOUR_3" ) );
			}
		}
		else if ( m_dwSubStep == FIVE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			// 스킬창 우선 닫기.
			if ( !m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )
			{
				m_pInterface->UiHideGroup( SKILL_WINDOW );
			}
			//

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
			{
				m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			NextSubStep();
		}
		else if ( m_dwSubStep == SIX_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( !m_bUICheck )
			{	// 아직 스킬윈도우 열지 않았다면,
				if ( m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )
				{
					m_bUICheck = true;

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					m_pInterface->SkillWindowSetVisiblePageForTutorial();
				}
				else
				{
					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						m_pInterface->SetTutorialDialogueExplainText( ID2GAMEINTEXT( "TUTORIAL_MODE_11" ) );

						{
							// 음성 멈춤.
							StopSound();

							// 음성 재생.
							/*CString strTemp;
							strTemp.Format( "TUTO_SOUNDS%03d", 47 + 8 );
							std::string strFile = strTemp;

							if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
								DxSoundLib::GetInstance()->PlaySound( strFile );*/
						}
					}

					return;
				}
			}

			if ( m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )
			{

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSkillID[ 0 ] );

					CString strTemp;
					strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_2"), pSkill->GetName() );

					m_pInterface->SetTutorialDialogueExplainText( strTemp );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 9 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}
				}

				if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiShowGroupTop ( EXPLAIN_INTERFACE );
				}

				if ( m_pInterface->ExplainIsSlotBoxVisible(0))
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}
			}
			else
			{
				if ( m_sHoldSkill == NATIVEID_NULL() )
				{	// 스킬창 안 열렸고, 손에 든 스킬 없을때.
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}

			for ( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
			{
				if ( pGLChar->m_sSKILLQUICK[ i ] == m_sSkillID[ 0 ] )
				{
					m_sHoldSkill = NATIVEID_NULL();

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					NextSubStep();
				}
			}
		}
		else if ( m_dwSubStep == SEVEN_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSkillID[ 1 ] );

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_2"), pSkill->GetName() );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 9 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}

			if ( m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )
			{
				if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiShowGroupTop ( EXPLAIN_INTERFACE );
				}

				if ( m_pInterface->ExplainIsSlotBoxVisible(1) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}
			}
			else
			{
				if ( m_sHoldSkill == NATIVEID_NULL() )
				{	// 스킬창 안 열렸고, 손에 든 스킬 없을때.
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}

			for ( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
			{
				if ( pGLChar->m_sSKILLQUICK[ i ] == m_sSkillID[ 1 ] )
				{
					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )
					{
						m_pInterface->UiHideGroup( SKILL_WINDOW );
					}


					m_sHoldSkill = NATIVEID_NULL();

					m_bUICheck = false;
					NextSubStep();
				}
			}
		}
		else if ( m_dwSubStep == EIGHT_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FOUR_7" ) );
			}
		}
		else if ( m_dwSubStep == NINE_ )
		{
			m_bMove = true;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			D3DXVECTOR3 vTarPos = m_vStepFivePos;

			pGLChar->ActionMoveTo ( fElapsedTime, vTarPos+D3DXVECTOR3(0,+500,0), vTarPos+D3DXVECTOR3(0,-500,0), FALSE, TRUE );
			NextSubStep();
		}
		else if ( m_dwSubStep == TEN_ )
		{	// 특정 위치 강제 이동.
			m_bControlMouse = false;
			m_bMove = false;

			D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosition();
			D3DXVECTOR3 vTargetPt = m_vStepFivePos;

			if ( vPos.x == vTargetPt.x && vPos.z == vTargetPt.z )
				NextSubStep();
		}
		break;

	case ATTACK_STEP:
		if ( m_dwSubStep == ONE_ )
		{	// 대화문.
			m_bMove = false;
			m_bControlMouse = false;

			m_pInterface->SetTutorialViewStep( 6 );

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FIVE_0" ) );
			}
		}
		else if ( m_dwSubStep == TWO_ )
		{	// 목각인 HIT.
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_3") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 11 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}			

			if ( m_pInterface->IsVisibleTargetInfoMouseOver() )
			{
				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_TARGET", 0 ) );

				if ( m_pInterface->TargetInfoGetMouseOverName() == strTemp )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}
			}

			if ( m_iHitCount == 1 )
			{
				NextSubStep();
			}
		}
		else if ( m_dwSubStep == THREE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FIVE_2" ) );
			}
		}

		else if ( m_dwSubStep == FOUR_ )
		{	// 목각인 HIT.
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}


			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_3") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 11 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}

			if ( m_pInterface->IsVisibleTargetInfoMouseOver() )
			{
				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_TARGET", 0 ) );

				if ( m_pInterface->TargetInfoGetMouseOverName() == strTemp )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}
			}

			if ( m_iHitCount == 2 )
			{
				m_iHitCount = 0; // 때린 횟수 초기화.

				NextSubStep();
			}
		}
		else if ( m_dwSubStep == FIVE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FIVE_3" ) );
			}
		}
		else if ( m_dwSubStep == SIX_ )
		{	// 목각인 스킬 공격.
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_4") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 12 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}

			if ( m_pInterface->IsVisibleTargetInfoMouseOver() )
			{
				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_TARGET", 0 ) );

				if ( m_pInterface->TargetInfoGetMouseOverName() == strTemp )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}
			}

			if ( m_iHitCount == 1 )
			{
				NextSubStep();
			}
		}
		else if ( m_dwSubStep == SEVEN_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FIVE_4" ) );
			}
		}
		else if ( m_dwSubStep == EIGHT_ )
		{	// 목각인 스킬 공격.
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_16") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 12 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}

			if ( m_pInterface->IsVisibleTargetInfoMouseOver() )
			{
				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_TARGET", 0 ) );

				if ( m_pInterface->TargetInfoGetMouseOverName() == strTemp )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}
			}

			if ( m_iHitCount == 2 )
			{
				m_iHitCount = 0; // 때린 횟수 초기화.

				NextSubStep();
			}
		}
		else if ( m_dwSubStep == NINE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FIVE_5" ) );
			}
		}

		break;

	case QUEST_AND_HUNT_STEP:
		if ( m_dwSubStep == ONE_ )
		{
			m_bMove = false;
			m_bControlMouse = false;

			m_pInterface->SetTutorialViewStep( 7 );

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_SKIP_BUTTON ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_SKIP_BUTTON );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_SIX_0" ) );
			}
		}
		else if ( m_dwSubStep == TWO_ )
		{
			m_bMove = true;
			m_bControlMouse = true;
			// Note : 튜토리얼 퀘스트 받으면, 다음 단계로 넘어간다.
			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_5") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );

				{
					// 음성 멈춤.
					StopSound();

					// 음성 재생.
					/*CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", 47 + 13 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );*/
				}
			}
		}
		else if ( m_dwSubStep == THREE_ )
		{
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( MAP_WINDOW ) )
			{
				// Memo : 미니맵을 나중에 열어야, 미니맵 클릭시 미니맵 UI가 판단하게 된다.
				if (m_pInterface->MinimapTargetIsEnable())
				{
					if ( !m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
						m_pInterface->UiShowGroupTop( MINIMAP_TARGET );
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
						m_pInterface->UiHideGroup( MINIMAP_TARGET );
				}
				//
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
				{
					m_pInterface->UiHideGroup( MINIMAP_TARGET );
				}
			}


			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}
		}
		else if ( m_dwSubStep == FOUR_ )
		{
			m_bMove = true;
			m_bControlMouse = true;

			if ( m_pInterface->UiIsVisibleGroup( MINIMAP_TARGET ) )
			{
				m_pInterface->UiHideGroup( MINIMAP_TARGET );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
			}

			//
			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW )
				&& !m_pInterface->UiIsVisibleGroup( SYSTEM_MESSAGE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogueExplainText();

				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_8") );

				m_pInterface->SetTutorialDialogueExplainText( strTemp );
			}
			//

			if ( m_pInterface->IsVisibleTargetInfoMouseOver() )
			{
				CString strTemp;
				strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_TARGET", 1 ) );

				if ( m_pInterface->TargetInfoGetMouseOverName() == strTemp )
				{
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->OpenBalloonText();
					}
				}
			}
			else
			{
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
				{
					m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
				}
			}

			ITEMCLIENTDROPLIST *pItemList = m_pGaeaClient->GetLandManClient()->GetItemList();
			ITEMCLIENTDROPNODE *pItemCur = pItemList->m_pHead;

			MONEYCLIENTDROPLIST *pMoneyList = m_pGaeaClient->GetLandManClient()->GetMoneyList();
			MONEYCLIENTDROPNODE *pMoneyCur = pMoneyList->m_pHead;

			if ( pItemCur != NULL || pMoneyCur != NULL  )
			{
				bool bCheckList[ 2 ] = { false, false }; // 땅에 떨어져는 있지만 주울 수 있는지 체크.

				D3DXVECTOR3 vPos = pGLChar->GetPosition();

				for ( ; pItemCur; pItemCur = pItemCur->pNext )
				{
					const CItemClientDrop &sItemDrop = *pItemCur->Data;

					float fDist = D3DXVec3Length( &( sItemDrop.vPos - vPos ) );

					if ( fDist <= 100.0f )
					{
						bCheckList[ 0 ] = true;
						break;
					}

					/*if ( sItemDrop.bCanGet && fDist <= 100.0f )
					{
						bCheckList[ 0 ] = true;
						break;
					}*/
				}

				for ( ; pMoneyCur; pMoneyCur = pMoneyCur->pNext )
				{
					const CMoneyClientDrop &sMoneyDrop = *pMoneyCur->Data;

					float fDist = D3DXVec3Length( &( sMoneyDrop.vPos - vPos ) );

					if ( fDist <= 100.0f )
					{
						bCheckList[ 1 ] = true;
						break;
					}

					/*if ( sMoneyDrop.bCanGet && fDist <= 100.0f )
					{
						bCheckList[ 1 ] = true;
						break;
					}*/
				}

				if ( bCheckList[ 0 ] || bCheckList[ 1 ] )
				{	// 하나라도 주울 수 있는게 있다면, UI 표시.
					if ( !m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
					{
						m_pInterface->UiShowGroupTop( BALLOON_SPACE_BAR );
					}
				}
				else
				{	// 떨어진 아이템은 있으나, 주울 수 있는 건 없다. ( 거리가 100 이상. )
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
					{
						m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
					}
				}
			}
			else
			{	// 땅에 떨어진 아이템이 없다.
				if ( m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
				{
					m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
				}
			}

			if ( m_bDrop )
			{
				D3DXVECTOR3 vPOS = m_vDropPos;
				D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPOS, NULL );

				long lResolution = CUIMan::GetResolution ();
				WORD X_RES = HIWORD(lResolution) - 30;
				WORD Y_RES = LOWORD(lResolution) - 30;

				if ( (vScreen.x<20) || (vScreen.y<20) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
					{
						m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
					}
				}
			}
		}
		else if ( m_dwSubStep == FIVE_ )
		{	// 퀘스트 완료 단계.
			m_bMove = false;
			m_bControlMouse = false;

			m_pInterface->SetTutorialViewStep( 8 );

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
			{
				m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
			}

			if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
			{
				m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
			}

			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}

			if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
			{
				m_pInterface->OpenTutorialDialogue();

				m_pInterface->SetTutorialDialogueIndex(1);

				m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_SIX_1" ) );
			}
		}
		else if ( m_dwSubStep == SIX_ )
		{	// 모든 스텝 완료. 각자의 학원으로 이동하자.
			m_bMove = true;
			m_bControlMouse = true;

			for ( int i = MOUSETIP_WINDOW; i <= TUTORIAL_VIEW_STEP; i++ )
			{
				if ( m_pInterface->UiIsVisibleGroup( i ) )
				{
					m_pInterface->UiHideGroup( i );
				}
			}

			// Note : 맵을 이동하기 전에, 완료 메세지 부터 먼저 보내야 한다.
			// 튜토리얼 완료 메세지 보내기. 이래야 DB에 저장 가능하다.
			/*NET_TUTORIAL_COMPLETE NetTutoComplet;
			NetTutoComplet.dwGaeaID = pGLChar->m_dwGaeaID;

			m_pGaeaClient->NETSENDTOFIELD( &NetTutoComplet );*/
			//

			SNATIVEID sStartMapID = GLCONST_CHAR::nidSTARTMAP[ pGLChar->m_wSchool ];
			DWORD dwGateID = GLCONST_CHAR::dwSTARTGATE[ pGLChar->m_wSchool ];

			GLMSG::SNET_TUTORIAL_COMPLET_MOVE NetMsg;
			NetMsg.nidMAP.wMainID = GLCONST_CHAR::nidSTARTMAP[ pGLChar->m_wSchool ].wMainID;
			NetMsg.nidMAP.wSubID = GLCONST_CHAR::nidSTARTMAP[ pGLChar->m_wSchool ].wSubID;
			NetMsg.dwGateID = dwGateID;

			pGLChar->m_sStartMapID = sStartMapID;
			pGLChar->m_dwStartGate = dwGateID;

			DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
            pGlobalStage->NetSend((NET_MSG_GENERIC*) &NetMsg);
			//NETSENDTOAGENT( &NetMsg );

			m_bTutorial = false;
			DxViewPort::GetInstance().SetTutorialCanCameraMove( true );
			DxViewPort::GetInstance().SetTutorial( false );
			NextSubStep();
		}

		break;

	case LEVEL_UP_STEP:
		{
			if ( m_dwSubStep == ONE_ )
			{
				if ( !m_bLevelUp )
				{	// 한번도 레벨업 텍스트 띄운 적이 없다면 보여준다.
					m_bMove = false;
					m_bControlMouse = true;

					if ( m_pInterface->UiIsVisibleGroup( BALLOON_SPACE_BAR ) )
					{
						m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
					}

					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}

					if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
					{
						m_pInterface->SetTutorialDialogueIndex(0);
						m_pInterface->TutorialDialogueSoundIndexSet(46);

						m_pInterface->OpenTutorialDialogue();

						m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_SEVEN_0" ) );
					}
				}
				else
				{
					m_dwMainStep = QUEST_AND_HUNT_STEP;
					m_dwSubStep = FOUR_;
				}
			}
			else if ( m_dwSubStep == TWO_ )
			{
				m_bMove = false;
				m_bControlMouse = true;

				m_bLevelUp = true;

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				NextSubStep();
			}
		}
		break;

	case QUEST_WINDOW_STEP:
		{
			m_bMove = false;
			m_bControlMouse = false;

			if ( m_dwSubStep == ONE_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( DIALOGUE_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->SetTutorialDialogueIndex(0);
					m_pInterface->TutorialDialogueSoundIndexSet(42);
					m_pInterface->OpenTutorialDialogue();

					CString strName;
					strName.Format( "TUTORIAL_DIALOGUE_STEP_EIGHT_0" );
					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT(strName) );
				}

				if (!m_pInterface->UiIsVisibleGroup(QUEST_WINDOW))
				{
					m_pInterface->SetQuestWindowOpen(m_dwQuestUI);
					m_pInterface->SetQuestWindowShowPage(CQuestWindow::QUEST_LIST, true);
					m_pInterface->UiHideGroup(QUEST_WINDOW);
				}
			}
			else if ( m_dwSubStep == TWO_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW )
						&& !m_pInterface->UiIsVisibleGroup( SYSTEM_MESSAGE_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_18") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
					else
					{
						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_18") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW )
						&& !m_pInterface->UiIsVisibleGroup( SYSTEM_MESSAGE_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_17") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
					else
					{
						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_17") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
				}

                if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
                {
                    SetTutorialStep( QUEST_WINDOW_STEP, SEVEN_ );
                    DWORD dwCurSounIndex = m_pInterface->GetTutorialDialogueSoundIndex();
                    dwCurSounIndex += 2;
                    m_pInterface->TutorialDialogueSoundIndexSet( dwCurSounIndex );
                }
			}
			else if ( m_dwSubStep == THREE_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					CString strName;
					strName.Format( "TUTORIAL_DIALOGUE_STEP_EIGHT_1" );
					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT(strName) );

					if ( !m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
					{
						m_pInterface->SetQuestWindowOpen( m_dwQuestUI );
						m_pInterface->SetQuestWindowShowPage ( CQuestWindow::QUEST_MAIN, true );

						{
                            const long lResolution = CUIMan::GetResolution();
							WORD X_RES = HIWORD ( lResolution );
							WORD Y_RES = LOWORD ( lResolution );

							const UIRECT& rcWindow = m_pInterface->GetQuestWindowGlobalPos();
							const UIRECT& rcTarWindow = m_pInterface->QBoxButtonGetGlobalPos();

							D3DXVECTOR2 vPos;
							vPos.x = X_RES - rcWindow.sizeX * 1.001f;
							vPos.y = rcTarWindow.top - rcWindow.sizeY * 1.001f;

							m_pInterface->SetQuestWindowGlobalPos( vPos );
						}
					}
					else
					{
						m_pInterface->UiHideGroup( QUEST_WINDOW );

						m_pInterface->SetQuestWindowOpen( m_dwQuestUI );
						m_pInterface->SetQuestWindowShowPage ( CQuestWindow::QUEST_MAIN, true );

						{
                            const long lResolution = CUIMan::GetResolution();
							WORD X_RES = HIWORD ( lResolution );
							WORD Y_RES = LOWORD ( lResolution );

							const UIRECT& rcWindow = m_pInterface->GetQuestWindowGlobalPos();
							const UIRECT& rcTarWindow = m_pInterface->QBoxButtonGetGlobalPos();

							D3DXVECTOR2 vPos;
							vPos.x = X_RES - rcWindow.sizeX * 1.001f;
							vPos.y = rcTarWindow.top - rcWindow.sizeY * 1.001f;

							m_pInterface->SetQuestWindowGlobalPos( vPos );
						}
					}
				}

				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}
			else if ( m_dwSubStep == FOUR_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}
			else if ( m_dwSubStep == FIVE_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}

				NextSubStep();
			}
			else if ( m_dwSubStep == SIX_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_19") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
					else
					{
						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_19") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}

					if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
					{
						m_pInterface->OpenTutorialDialogueExplainText();

						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_17") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
					else
					{
						CString strTemp;
						strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_17") );

						m_pInterface->SetTutorialDialogueExplainText( strTemp );
					}
				}
			}
			else if ( m_dwSubStep == SEVEN_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogue();

					CString strName;
					strName.Format( "TUTORIAL_DIALOGUE_STEP_EIGHT_3" );
					m_pInterface->SetTutorialDialogText( ID2GAMEINTEXT(strName) );
				}

				if ( !m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					m_pInterface->SetQuestWindowOpen( m_dwQuestUI );

					{
                        const long lResolution = CUIMan::GetResolution();
						WORD X_RES = HIWORD ( lResolution );
						WORD Y_RES = LOWORD ( lResolution );

                        const UIRECT& rcWindow = m_pInterface->GetQuestWindowRenewalGlobalPos();
						const UIRECT& rcTarWindow = m_pInterface->QBoxButtonGetGlobalPos();

						D3DXVECTOR2 vPos;
						vPos.x = X_RES - rcWindow.sizeX * 1.001f;
						vPos.y = rcTarWindow.top - rcWindow.sizeY * 1.001f;

                        m_pInterface->SetQuestWindowRenewalGlobalPos( vPos );
					}
				}
				else
				{
					m_pInterface->SetQuestWindowAutoSelectPage( m_dwQuestUI );
					m_pInterface->SetQuestWindowShowPage ( CQuestWindow::QUEST_STEP, true );

					{
                        const long lResolution = CUIMan::GetResolution();
						WORD X_RES = HIWORD ( lResolution );
						WORD Y_RES = LOWORD ( lResolution );

						const UIRECT& rcWindow = m_pInterface->GetQuestWindowRenewalGlobalPos();
						const UIRECT& rcTarWindow = m_pInterface->QBoxButtonGetGlobalPos();

						D3DXVECTOR2 vPos;
						vPos.x = X_RES - rcWindow.sizeX * 1.001f;
						vPos.y = rcTarWindow.top - rcWindow.sizeY * 1.001f;

						m_pInterface->SetQuestWindowRenewalGlobalPos( vPos );
					}
				}

				NextSubStep();
			}
			else if ( m_dwSubStep == EIGHT_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					if ( !m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
					}
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
					{
						m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
					}
				}
			}
			else if ( m_dwSubStep == NINE_ )
			{
				if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
				}

				if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_WINDOW ) )
				{
					m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
				{
					m_pInterface->UiHideGroup( QUEST_WINDOW );
				}

				if ( m_pInterface->UiIsVisibleGroup( EXPLAIN_INTERFACE ) )
				{
					m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
				}

				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->OpenTutorialDialogueExplainText();

					CString strTemp;
					strTemp.Format( ID2GAMEINTEXT( "TUTORIAL_MODE_6") );

					m_pInterface->SetTutorialDialogueExplainText( strTemp );

					{
						// 음성 멈춤.
						StopSound();

						// 음성 재생.
						/*CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", 47 + 14 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );*/
					}
				}

				NextSubStep();
			}
		}
		break;
	}

	BOOL bCharMoveBlock = NSGUI::IsCharMoveBlock ();
	if ( bCharMoveBlock )
		m_pInterface->SetTutorialDialogueEnable( false );
	else
		m_pInterface->SetTutorialDialogueEnable( true );

	DxViewPort::GetInstance().SetTutorialCanCameraMove( m_bControlMouse );
	m_vCameraRotate = DxViewPort::GetInstance().GetTutorialCameraRotation();
	m_fCameraZoom = DxViewPort::GetInstance().GetTutorialCameraZoom();
#endif	
}

void GLBaseTutorial::CheckAttackForTutorial( const EMCROW emCrow, const DWORD dwID )
{
	SNATIVEID tempID = m_pGaeaClient->GetActiveMap()->GetCrow( dwID )->m_sNativeID;

	if ( ( m_dwMainStep == ATTACK_STEP && m_dwSubStep == TWO_ )
		|| ( m_dwMainStep == ATTACK_STEP && m_dwSubStep == FOUR_ ) )
	{
		if ( tempID == m_sMokgacID || tempID == m_sMobID )
		{
			m_iHitCount++;
			// 일반 공격했을때는, 타겟을 다시 공격하지 않게 하기 위함이다.
			m_pGaeaClient->GetCharacter()->ResetAction();
			m_pGaeaClient->GetCharacter()->GetActionID().RESET();
		}
	}
	else if ( ( m_dwMainStep == ATTACK_STEP && m_dwSubStep == SIX_ )
		|| ( m_dwMainStep == ATTACK_STEP && m_dwSubStep == EIGHT_ ) )
	{
		for ( int i = 0; i < 2; i++ )
		{
			if ( !m_bUseSkill[ i ] && m_sSkillID[ i ] == m_sUseSkillID )
			{
				if ( tempID == m_sMokgacID || tempID == m_sMobID )
				{
					m_iHitCount++;
					m_bUseSkill[ i ] = true;
				}
			}
		}
	}
}

void GLBaseTutorial::SkipTutorial()
{
	for ( int i = 0; i < 3; i++ )
	{
		STARGETID vTARID(CROW_PC, m_pGaeaClient->GETMYGAEAID() + i );

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect( vTARID );
	}

	m_dwMainStep = QUEST_AND_HUNT_STEP;
	m_dwSubStep = ONE_;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	// 아이템 퀵슬롯 장착 안되어 있으면 장착.
	bool bCheckItem = false;

	for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
	{
		if ( pChar->m_sACTIONQUICK[ i ].sNID == m_sItemID )
		{
			bCheckItem = true;
			break;
		}
	}

	if ( !bCheckItem )
	{
		SINVENITEM* findItem = pChar->m_cInventory.FindItem(m_sItemID);

		if ( findItem != NULL )
		{
			pChar->ReqInvenTo( findItem->wPosX, findItem->wPosY );

			for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
			{
				if ( pChar->m_sACTIONQUICK[ i ].sNID == NATIVEID_NULL() )
				{
					//pChar->ReqItemQuickSet ( (WORD) i );
					{
						const SITEMCUSTOM& sITEM = findItem->sItemCustom;

//#if defined(VN_PARAM) //vietnamtest%%%
						if (m_pGaeaClient->GetServiceProvider() == SP_VIETNAM)
						{
							if ( sITEM.bVietnamGainItem )
								return;
						}
//#endif

						SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEM.GetNativeID() );
						if (!pITEM)
                            return;

						if (pITEM->sBasicOp.emItemType != ITEM_EFFECT_RELEASE_CURE &&
							pITEM->sBasicOp.emItemType != ITEM_CURE &&
							pITEM->sBasicOp.emItemType != ITEM_RECALL &&
							pITEM->sBasicOp.emItemType != ITEM_TELEPORT_CARD &&
							pITEM->sBasicOp.emItemType != ITEM_PET_CARD)
                        {
							return;
                        }

						//	Note : 클라이언트의 슬롯에 바로 넣어줌.
						//m_sACTIONQUICK[wSLOT].wACT = EMACT_SLOT_DRUG;
						//m_sACTIONQUICK[wSLOT].sNID = sITEM.sNativeID;

						//	Note : 서버에 슬롯에 들어갈 정보 전송.
						GLMSG::SNETPC_REQ_ACTIONQUICK_SET NetMsg;
						NetMsg.wSLOT = (WORD)i;
						NetMsg.wACT = EMACT_SLOT_DRUG;

						m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
					}
					break;
				}
			}
		}
	}
	//

	// 스킬 퀵 장착 안되어 있으면 장착.
	bool bCheckSkill[ 2 ] = { false, false };
	for ( int i = 0; i < EMSKILLQUICK_SIZE; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			if ( pChar->m_sSKILLQUICK[ i ] == m_sSkillID[ j ] )
			{
				bCheckSkill[ j ] = true;
			}
		}
	}

	bool bCheckActiveSkill = false;

	for ( int i = 0; i < 2; i++ )
	{
		if ( !bCheckSkill[ i ] )
		{
			for ( int j = 0; j < EMSKILLQUICK_SIZE; j++ )
			{
				if ( pChar->m_sSKILLQUICK[ j ] == NATIVEID_NULL() )
				{
					pChar->m_sSKILLQUICK[ j ] = m_sSkillID[ i ];

					if ( !bCheckActiveSkill )
					{
						m_pGaeaClient->GetCharacter()->ReqSkillRunSet( j );
						bCheckActiveSkill = true;
					}

					break;
				}
			}
		}
	}

	{
		StopSound();
	}
}

bool GLBaseTutorial::IsIntroduceStep()
{
    if ( GetCurrentMainStep() == INTRODUCE_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsExplainInterfaceStep()
{
    if ( GetCurrentMainStep() == EXPLAIN_INTERFACE_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsMouseContNMoveStep()
{
    if ( GetCurrentMainStep() == MOUSECONTROL_AND_MOVE_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsQuickNActionSlotStep()
{
    if ( GetCurrentMainStep() == QUICKSLOT_ACTIONSLOT_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsAttackStep()
{
    if ( GetCurrentMainStep() == ATTACK_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsQuestNHuntStep()
{
    if ( GetCurrentMainStep() == QUEST_AND_HUNT_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsLevelUpStep()
{
    if ( GetCurrentMainStep() == LEVEL_UP_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsQuestWindowStep()
{
    if ( GetCurrentMainStep() == QUEST_WINDOW_STEP )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubOneStep()
{
    if ( GetCurrentSubStep() == ONE_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubTwoStep()
{
    if ( GetCurrentSubStep() == TWO_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubThreeStep()
{
    if ( GetCurrentSubStep() == THREE_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubFourStep()
{
    if ( GetCurrentSubStep() == FOUR_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubFiveStep()
{
    if ( GetCurrentSubStep() == FIVE_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubSixStep()
{
    if ( GetCurrentSubStep() == SIX_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubSevenStep()
{
    if ( GetCurrentSubStep() == SEVEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubEightStep()
{
    if ( GetCurrentSubStep() == EIGHT_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubNineStep()
{
    if ( GetCurrentSubStep() == NINE_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubTenStep()
{
    if ( GetCurrentSubStep() == TEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubElevenStep()
{
    if ( GetCurrentSubStep() == ELEVEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubTwelveStep()
{
    if ( GetCurrentSubStep() == TWELVE_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubThirteenStep()
{
    if ( GetCurrentSubStep() == THIRTEEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubFourteenStep()
{
    if ( GetCurrentSubStep() == FOURTEEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubFifteenStep()
{
    if ( GetCurrentSubStep() == FIFTEEN_ )
        return true;

    return false;
}

bool GLBaseTutorial::IsSubSixteenStep()
{
    if ( GetCurrentSubStep() == SIXTEEN_ )
        return true;

    return false;
}

void GLBaseTutorial::StopSound()
{
    for (size_t i=0; i<GetSoundSize(); ++i)
    {	
        std::string strFile = sc::string::format("TUTO_SOUNDS%03d", i);
        if (DxSoundLib::GetInstance()->IsPlaying(strFile))
            DxSoundLib::GetInstance()->StopSound(strFile);
    }
}

void GLBaseTutorial::PlaySound(DWORD Index)
{    
    std::string strFile = sc::string::format("TUTO_SOUNDS%03d", Index+1);
    if (!DxSoundLib::GetInstance()->IsPlaying(strFile))
        DxSoundLib::GetInstance()->PlaySound(strFile);
}

void GLBaseTutorial::CreateSound()
{
    for (size_t i=0; i<GetSoundSize(); ++i)
    {        
        std::string strFile = sc::string::format("TUTO_SOUNDS%03d", i+1);
        DxSoundLib::GetInstance()->CreateSound(
            strFile,
            m_TutorialSound[i],
            SFX_SOUND);
    }
}
