#include "StdAfx.h"
#include "UIOuterWindow.h"
#include "../../OuterInterface.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIOuterWindow::CUIOuterWindow(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindow(pEngineDevice)
    , m_pInterface(pInterface)
{
}

CUIOuterWindow::~CUIOuterWindow ()
{
}

void CUIOuterWindow::CreateBaseWidnow ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "OUTER_WINDOW_TITLE", "OUTER_WINDOW_TITLE_LEFT", "OUTER_WINDOW_TITLE_MID", "OUTER_WINDOW_TITLE_RIGHT", "OUTER_WINDOW_TEXTBOX", szWindowName );
	CreateBody ( "OUTER_WINDOW_BODY", "OUTER_WINDOW_BODY_LEFT", "OUTER_WINDOW_BODY_UP", "OUTER_WINDOW_BODY_MAIN", "OUTER_WINDOW_BODY_DOWN", "OUTER_WINDOW_BODY_RIGHT" );	
	
	ResizeControl ( szWindowKeyword );

//	SetTitleAlign ( CBasicTextBox::CENTER );
}

CBasicTextBox*	CUIOuterWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

CUIControl*	CUIOuterWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


void CUIOuterWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CUIOuterWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}