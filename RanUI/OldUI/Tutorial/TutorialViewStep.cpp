#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/TipBox.h"

#include "TutorialViewStep.h"

#include "../../InnerInterface.h"
#include "../Interface/SkillTrayTab.h"
#include "../Util/UIWindowEx.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTutorialViewStep::CTutorialViewStep(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pInterface( pInterface )
    , m_pMain(NULL)
    , m_pTitle(NULL)
    , m_iIndex(-1)
{
	for ( int i = 0; i < 9; i++ )
	{
		m_pStepBox[ i ] = NULL;
	}
}

CTutorialViewStep::~CTutorialViewStep(void)
{
}

void CTutorialViewStep::CreateSubControl()
{
	{
		m_pMain = new CUIWindowEx( m_pInterface, m_pEngineDevice );
		m_pMain->CreateSub( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TUTORIAL_VIEW_STEP_MAIN );
		m_pMain->CreateBaseWindowBody( "TUTORIAL_VIEW_STEP_MAIN" );
		RegisterControl( m_pMain );
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		CBasicTextBox* pTitle = new CBasicTextBox(m_pEngineDevice);
		pTitle->CreateSub( this, "TUTORIAL_VIEW_STEP_TITLE" );
		pTitle->SetFont( pFont );
		pTitle->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTitle );
		m_pTitle = pTitle;

		if ( m_pTitle )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_VIEW_STEP_TITLE_TEXT" );
			m_pTitle->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
			m_pTitle->SetVisibleSingle( TRUE );
		}
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		for ( int i = 0; i < 9; i++ )
		{
			CString strName;
			strName.Format( "TUTORIAL_VIEW_STEP_STEP_TEXT_%1d", i );

			CBasicTextBox* pTempText = new CBasicTextBox(m_pEngineDevice);
			pTempText->CreateSub( this, strName );
			pTempText->SetFont( pFont );
			pTempText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
			RegisterControl( pTempText );
			m_pStepBox[ i ] = pTempText;

			if ( m_pStepBox[ i ] )
			{
				m_pStepText[ i ] = ID2GAMEINTEXT( "TUTORIAL_VIEW_STEP_STEP", i );
				m_pStepBox[ i ]->SetOneLineText( m_pStepText[ i ], NS_UITEXTCOLOR::DARKGRAY );
				m_pStepBox[ i ]->SetVisibleSingle( TRUE );
			}
		}
	}
}

void CTutorialViewStep::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() ) return ;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CTutorialViewStep::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	m_pMain->ResetExclusiveControl();

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	NSGUI::ResetCharMoveBlock();
}

HRESULT CTutorialViewStep::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED( hr ) ) return hr;

	{
		const UIRECT& rcWindow = GetGlobalPos();
		const UIRECT& rcTarget = m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTarget.right + rcWindow.sizeX * 0.01f;
		vPos.y = rcTarget.top + rcTarget.sizeY * 0.5f - rcWindow.sizeY * 0.5f;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	SetOnStep( m_iIndex );

	return S_OK;
}

void CTutorialViewStep::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

void CTutorialViewStep::SetOnStep( int nIndex )
{
	if ( nIndex < 0 || nIndex > 8 )	return;

	for ( int i = 0; i < 9; i++ )
	{
		m_pStepBox[ i ]->ClearText();
		m_pStepBox[ i ]->SetOneLineText( m_pStepText[ i ], NS_UITEXTCOLOR::DARKGRAY );
	}

	m_pStepBox[ nIndex ]->ClearText();
	m_pStepBox[ nIndex ]->SetOneLineText( m_pStepText[ nIndex ], NS_UITEXTCOLOR::WHITE );

	m_iIndex = nIndex;
}

MyTutorialViewStep::MyTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CTutorialViewStep( pInterface, pEngineDevice )
{
}

void MyTutorialViewStep::CreateUIWindowAndRegisterOwnership()
{
    CTutorialViewStep::Create( TUTORIAL_VIEW_STEP, "TUTORIAL_VIEW_STEP_MAIN_BOX", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
    CTutorialViewStep::CreateSubControl();
    CTutorialViewStep::m_pInterface->UiRegisterControl( this );		
    CTutorialViewStep::m_pInterface->UiShowGroupBottom( TUTORIAL_VIEW_STEP );
    CTutorialViewStep::m_pInterface->UiHideGroup( TUTORIAL_VIEW_STEP );
}

const UIRECT& MyTutorialViewStep::GetUIWindowGlobalPos()
{
    return CTutorialViewStep::GetGlobalPos();
}

void MyTutorialViewStep::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CTutorialViewStep::SetGlobalPos( vPos );
}

void MyTutorialViewStep::SetUIWindowLocalPos( const D3DXVECTOR2& vPos )
{
    CTutorialViewStep::SetLocalPos( vPos );
}

void MyTutorialViewStep::SetOnStep( int nIndex )
{
    CTutorialViewStep::SetOnStep( nIndex );
}