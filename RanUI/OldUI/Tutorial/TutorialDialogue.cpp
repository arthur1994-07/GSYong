#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "./TutorialDialogue.h"
#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxSound/DxSoundLib.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTutorialDialogue::CTutorialDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface( pInterface )
    , m_pMain(NULL)
    , m_pMouse(NULL)
    , m_pMouseButton(NULL)
    , m_pTeacher(NULL)
    , m_pExplainText(NULL)
    , m_pName(NULL)
    , m_fFlowTime(0.0f)
    , m_dwIndex(0)
    , m_dwTextStep(0)
    , m_bEnableWork(true)
    , m_bEnableClick(false)
    , m_fClickTime(0.0f)
    , m_dwSoundIndex(0)
{
}

CTutorialDialogue::~CTutorialDialogue(void)
{
}

void CTutorialDialogue::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 13, _DEFAULT_FONT_SHADOW_FLAG );

	{
		m_pMain = new CTipBox(m_pEngineDevice);;
		m_pMain->CreateSub( this, "TUTORIAL_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TUTORIAL_TIP_BOX );
		m_pMain->CreateTipBox( "TUTORIAL_DIALOGUE_BOX" );
		RegisterControl( m_pMain );
	}

	{
		CBasicTextBox* pExplainText = new CBasicTextBox(m_pEngineDevice);
		pExplainText->CreateSub( this, "TUTORIAL_DIALOGUE_TEXT" );
		pExplainText->SetFont( pFont );
		pExplainText->SetTextAlign( TEXT_ALIGN_LEFT );
		RegisterControl( pExplainText );
		m_pExplainText = pExplainText;

		if ( m_pExplainText )
		{
			m_pInterface->GetTutorialDialogue()->SetDialogueText( ID2GAMEINTEXT("TUTORIAL_DIALOGUE_STEP_ONE_0") );
		}
	}

	{
		CBasicTextBox* pName = new CBasicTextBox(m_pEngineDevice);
		pName->CreateSub( this, "TUTORIAL_DIALOGUE_NAME" );
		pName->SetFont( pFont );
		pName->SetTextAlign( TEXT_ALIGN_CENTER_X );
		RegisterControl( pName );
		m_pName = pName;

		if ( m_pName )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT("TUTORIAL_DIALOGUE_NPC_NAME");
			m_pName->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}

	{
		CUIControl* pMouse = new CUIControl(m_pEngineDevice);
		pMouse->CreateSub( this, "TUTORIAL_DIALOGUE_MOUSE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl( pMouse );
		m_pMouse = pMouse;
		m_pMouse->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pMouseButton = new CUIControl(m_pEngineDevice);
		pMouseButton->CreateSub( this, "TUTORIAL_DIALOGUE_MOUSE_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl( pMouseButton );
		m_pMouseButton = pMouseButton;
		m_pMouseButton->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pTeacher = new CUIControl(m_pEngineDevice);
		pTeacher->CreateSub( this, "TUTORIAL_DIALOGUE_TEACHER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl( pTeacher );
		m_pTeacher = pTeacher;
	}

	// load tutorial sound.
	{
        GLBaseTutorial* pTutorial = m_pGaeaClient->GetTutorial();
        pTutorial->CreateSound();
	}
	//
}

void CTutorialDialogue::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_bEnableClick = false;
	m_fClickTime += fElapsedTime;

	if ( m_fClickTime >= 0.3f )
		m_bEnableClick = true;
	else
	{
		if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_EXPLAIN_VAR_TEXT ) )
		{
			m_pInterface->UiHideGroup( TUTORIAL_EXPLAIN_VAR_TEXT );
		}
	}

	if ( !m_bEnableClick ) return;

	if (m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep() && m_pGaeaClient->GetTutorial()->IsSubTwoStep())
	{
		m_bEnableClick = false;
		m_fClickTime = 0.0f;

		if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_EXPLAIN_VAR_TEXT ) )
		{
			m_pInterface->UiHideGroup( TUTORIAL_EXPLAIN_VAR_TEXT );
		}

		return;
	}

	m_fFlowTime += fElapsedTime;

	if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_EXPLAIN_VAR_TEXT ) )
	{
		m_pInterface->UiShowGroupTop( TUTORIAL_EXPLAIN_VAR_TEXT );
	}

	if ( m_fFlowTime >= 0.5f )
	{

		m_fFlowTime = 0.0f;
	}
}

HRESULT CTutorialDialogue::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	{
		const UIRECT& rcMouseTipWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = ( X_RES - rcMouseTipWindow.sizeX ) * 0.5f;
		vPos.y = Y_RES * 0.2f;
		SetGlobalPos( vPos );
		SetLocalPos( vPos );
	}

	return S_OK;
}

void CTutorialDialogue::SetDialogueText(const CString& strText)
{
	m_pExplainText->SetOneLineText( strText, NS_UITEXTCOLOR::WHITE );
	m_fClickTime = 0.0f;
}

void CTutorialDialogue::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( ( (dwMsg & UIMSG_LB_UP) && m_bEnableWork ) || ( (dwMsg & UIMSG_LB_UP) && CHECK_MOUSE_IN( dwMsg ) ) )
	{
		if ( !m_bEnableClick ) return;
		m_fClickTime = 0.0f;
		

		/*if ( !( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
			&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->THREE_
			&& m_dwIndex + 1 == 2 )
			&& !( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->ONE_
			&& m_dwIndex + 1 == 2 ) )*/
	    // 음성 멈춤.
        GLBaseTutorial* pTutorial = m_pGaeaClient->GetTutorial();
		pTutorial->StopSound();

		if (m_dwSoundIndex > 50)
			m_dwSoundIndex = 50;

		CString strCombine;

		if (pTutorial->IsIntroduceStep())
		{
			m_dwIndex++;

			if ( m_dwIndex > 1 )
			{
				pTutorial->NextMainStep();

				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_TWO_0" );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				// 음성 재생.
                pTutorial->PlaySound(m_dwSoundIndex);
				m_dwSoundIndex++;
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_ONE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep() )
		{
			if ( m_pGaeaClient->GetTutorial()->IsSubOneStep() )
			{
				m_dwIndex = 1;
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else if ( m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->THREE_ )
			{
				m_dwIndex++;

				if ( m_dwIndex > 8 )
				{
					m_pGaeaClient->GetTutorial()->NextSubStep();

					{	// 음성 재생.
						CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );

						m_dwSoundIndex++;
					}
				}
				else
				{
					CString strName;
					strName.Format( "TUTORIAL_DIALOGUE_STEP_TWO_%01d", m_dwIndex );
					strCombine = ID2GAMEINTEXT(strName);
					m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

					if ( m_dwIndex != 2 )
					{
						m_pGaeaClient->GetTutorial()->NextSubStep();
					}

					//if ( m_dwIndex != 2 )
					{	// 음성 재생.
						CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );

						m_dwSoundIndex++;
					}
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubOneStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 2 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_THREE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				//if ( m_dwIndex != 2 )
				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFourStep() )
		{
			m_dwIndex++;

			m_pGaeaClient->GetTutorial()->NextSubStep();
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
		{
			m_dwIndex++;

			m_pGaeaClient->GetTutorial()->NextSubStep();
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubTenStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 7 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_THREE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubTwelveStep() )
		{
			m_dwIndex++;

			m_pGaeaClient->GetTutorial()->NextSubStep();
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFourteenStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 10 )
			{
				if ( m_pInterface->UiIsVisibleGroup( MAP_WINDOW ) )
				{
					m_pInterface->UiHideGroup( MAP_WINDOW );
				}

				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_THREE_%d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
			&& m_pGaeaClient->GetTutorial()->IsSubSixteenStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 12 )
			{
				if ( m_pInterface->UiIsVisibleGroup( MAP_WINDOW ) )
				{
					m_pInterface->UiHideGroup( MAP_WINDOW );
				}

				SetDialogueText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_FOUR_0" ) );

				m_pGaeaClient->GetTutorial()->NextSubStep();

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_THREE_%d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
			&& m_pGaeaClient->GetTutorial()->IsSubOneStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 2 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FOUR_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFourStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 6 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FOUR_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
			&& m_pGaeaClient->GetTutorial()->IsSubEightStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 9 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FOUR_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				if ( m_dwIndex > 7 )
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubOneStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 1 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				if ( m_dwIndex > 7 )
				{
					if ( m_pInterface->UiIsVisibleGroup( BALLOON_TEXT_WINDOW ) )
					{
						m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
					}
				}

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 2 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFiveStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 3 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 4 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubEightStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 5 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubNineStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 6 )
			{
				SetDialogueText( ID2GAMEINTEXT( "TUTORIAL_DIALOGUE_STEP_SIX_0" ) );

				m_pGaeaClient->GetTutorial()->NextSubStep();

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_FIVE_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
			&& m_pGaeaClient->GetTutorial()->IsSubOneStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 0 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_SIX_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFiveStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 1 )
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_SIX_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsLevelUpStep()
			&& m_pGaeaClient->GetTutorial()->IsSubOneStep() )
		{
			m_dwIndex++;

			if ( m_dwIndex > 3 )
			{
				// Memo : 레벨업 텍스트 후 에는 다시 사냥 모드로 간다.
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				CString strName;
				strName.Format( "TUTORIAL_DIALOGUE_STEP_SEVEN_%01d", m_dwIndex );
				strCombine = ID2GAMEINTEXT(strName);
				m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

				{	// 음성 재생.
					CString strTemp;
					strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
					std::string strFile = strTemp;

					if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
						DxSoundLib::GetInstance()->PlaySound( strFile );

					m_dwSoundIndex++;
				}
			}
		}
		else if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
			&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->ONE_
			&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->EIGHT_ )
		{
			m_dwIndex++;

			if( m_dwIndex > 3)
			{
				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
			else
			{
				if ( m_dwIndex == 2 )
				{
					CString strName;
					strName.Format( "TUTORIAL_DIALOGUE_STEP_EIGHT_%01d", m_dwIndex );
					strCombine = ID2GAMEINTEXT(strName);
					m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

					{	// 음성 재생.
						CString strTemp;
						strTemp.Format( "TUTO_SOUNDS%03d", m_dwSoundIndex + 1 );
						std::string strFile = strTemp;

						if ( !DxSoundLib::GetInstance()->IsPlaying( strFile ) )
							DxSoundLib::GetInstance()->PlaySound( strFile );

						m_dwSoundIndex++;
					}
				}

				m_pGaeaClient->GetTutorial()->NextSubStep();
			}
		}
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialDialogue::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

void CTutorialDialogue::SetTutorialDialogueIndex( DWORD Index )
{
    m_dwIndex = Index;
}

DWORD CTutorialDialogue::GetTutorialDialogueIndex()
{
    return m_dwIndex;
}

void CTutorialDialogue::TutorialDialogueTextStepInc()
{
    m_dwTextStep++;
}

void CTutorialDialogue::SetTutorialDialogueSoundIndex( DWORD Index )
{
    m_dwSoundIndex = Index;
}

DWORD CTutorialDialogue::GetTutorialDialogueSoundIndex()
{
    return m_dwSoundIndex;
}

void CTutorialDialogue::TutorialDialogueSoundIndexInc()
{
    m_dwSoundIndex++;
}
//
CTutorialDialogueExplainText::CTutorialDialogueExplainText(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pMain(NULL)
    , m_pExplainText(NULL)
    , m_fFlowTime(0.0f)
{
}

CTutorialDialogueExplainText::~CTutorialDialogueExplainText(void)
{
}

void CTutorialDialogueExplainText::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 13, _DEFAULT_FONT_SHADOW_FLAG );

	{
		m_pMain = new CTipBox(m_pEngineDevice);;
		m_pMain->CreateSub( this, "TUTORIAL_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TUTORIAL_TIP_BOX );
		m_pMain->CreatePracticeModeBox( "TUTORIAL_DIALOGUE_EXPLAIN_TEXT_BOX" );
		RegisterControl( m_pMain );
	}

	{
		CBasicTextBox* pExplainText = new CBasicTextBox(m_pEngineDevice);
		pExplainText->CreateSub( this, "TUTORIAL_DIALOGUE_EXPLAIN_TEXT" );
		//pExplainText->CreateSub( this, "BASIC_VAR_TEXT" );
		pExplainText->SetFont( pFont );
		pExplainText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pExplainText );
		m_pExplainText = pExplainText;

		if ( m_pExplainText )
		{
			CString strCombine;
			strCombine.Format( "%s", "[실습모드] 좌/우로 시야 이동." );
			m_pExplainText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
}

void CTutorialDialogueExplainText::SetExplainText(const CString& strText)
{
	m_pExplainText->SetOneLineText(strText, NS_UITEXTCOLOR::WHITE);
}


void CTutorialDialogueExplainText::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

HRESULT CTutorialDialogueExplainText::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	{
		const UIRECT& rcMouseTipWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = ( X_RES - rcMouseTipWindow.sizeX ) * 0.5f;
		vPos.y = Y_RES * 0.12f;
		SetGlobalPos( vPos );
		SetLocalPos( vPos );
	}

	return S_OK;
}

void CTutorialDialogueExplainText::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( (dwMsg & UIMSG_LB_UP) )
	{
	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CTutorialDialogueExplainText::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

MyTutorialDialogue::MyTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CTutorialDialogue( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTutorialDialogue::CreateUIWindowAndRegisterOwnership()
{
    CTutorialDialogue::Create ( TUTORIAL_DIALOGUE_WINDOW, "TUTORIAL_DIALOGUE_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CTutorialDialogue::CreateSubControl ();
    CTutorialDialogue::SetAlignFlag ( UI_FLAG_CENTER_Y );
    CTutorialDialogue::m_pInterface->UiRegisterControl( this, true );
    CTutorialDialogue::m_pInterface->UiShowGroupTop( TUTORIAL_DIALOGUE_WINDOW );
    CTutorialDialogue::m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_WINDOW );
}

void MyTutorialDialogue::SetDialogueText( const UI::String& strText )
{
    CTutorialDialogue::SetDialogueText( UI::ToString( strText ) );
}

void MyTutorialDialogue::SetEnalbe( bool bEnableWork )
{
    CTutorialDialogue::SetEnalbe( bEnableWork );
}

const UIRECT& MyTutorialDialogue::GetUIWindowGlobalPos()
{
    return CTutorialDialogue::GetGlobalPos();
}

void MyTutorialDialogue::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CTutorialDialogue::SetGlobalPos( vPos );
}

void MyTutorialDialogue::SetUIWindowLocalPos( const D3DXVECTOR2& vPos )
{
    CTutorialDialogue::SetLocalPos( vPos );
}

void MyTutorialDialogue::SetTutorialDialogueIndex( DWORD Index )
{
    CTutorialDialogue::SetTutorialDialogueIndex( Index );
}

DWORD MyTutorialDialogue::GetTutorialDialogueIndex()
{
    return CTutorialDialogue::GetTutorialDialogueIndex();
}

void MyTutorialDialogue::TutorialDialogueTextStepInc()
{
    CTutorialDialogue::TutorialDialogueTextStepInc();
}

void MyTutorialDialogue::SetTutorialDialogueSoundIndex( DWORD Index )
{
    CTutorialDialogue::SetTutorialDialogueSoundIndex( Index );
}

DWORD MyTutorialDialogue::GetTutorialDialogueSoundIndex()
{
    return CTutorialDialogue::GetTutorialDialogueSoundIndex();
}

void MyTutorialDialogue::TutorialDialogueSoundIndexInc()
{
    CTutorialDialogue::TutorialDialogueSoundIndexInc();
}