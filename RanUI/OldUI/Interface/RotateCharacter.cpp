#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "CreateCharacterWindow.h"
#include "CreateCharacterMenu.h"
#include "RotateCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRotateCharacter::CRotateCharacter(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pButtonLeft( NULL )
    , m_pButtonRight( NULL )
{
}

CRotateCharacter::~CRotateCharacter()
{
}

void CRotateCharacter::CreateSubControl ()
{
	m_pButtonLeft = new CBasicButton(m_pEngineDevice);
	m_pButtonLeft->CreateSub( this, "ROTATE_CHAR_LEFT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ROTATE_LEFT_BUTTON );
	m_pButtonLeft->CreateFlip( "ROTATE_CHAR_LEFT_F", CBasicButton::CLICK_FLIP );
	m_pButtonLeft->SetVisibleSingle ( FALSE );
	RegisterControl( m_pButtonLeft );

	m_pButtonRight = new CBasicButton(m_pEngineDevice);
	m_pButtonRight->CreateSub( this, "ROTATE_CHAR_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ROTATE_RIGHT_BUTTON );
	m_pButtonRight->CreateFlip( "ROTATE_CHAR_RIGHT_F", CBasicButton::CLICK_FLIP );
	m_pButtonRight->SetAlignFlag ( UI_FLAG_RIGHT );
	m_pButtonRight->SetVisibleSingle ( FALSE );
	RegisterControl( m_pButtonRight );
}

CUIControl*	CRotateCharacter::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CRotateCharacter::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CRotateCharacter::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_pInterface->GetCreateCharacterWindow()->GetMenu()->IsVisible() &&
		m_pInterface->GetCreateCharacterWindow()->GetMenu()->GetMenuSelect() )
	{
		m_pButtonLeft->SetVisibleSingle( TRUE );
		m_pButtonRight->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pButtonLeft->SetVisibleSingle( FALSE );
		m_pButtonRight->SetVisibleSingle( FALSE );
	}
}

void CRotateCharacter::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ROTATE_LEFT_BUTTON:
	case ROTATE_RIGHT_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
			{
				INT nIndex = ControlID - ROTATE_LEFT_BUTTON;
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				DxLobyStage *pLobyStage = pGlobalStage->GetLobyStage();
				pLobyStage->RotateChar( nIndex == 0 ? TRUE : FALSE );
			}
		}
		break;
	}
}

void CRotateCharacter::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}