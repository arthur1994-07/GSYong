#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "./ActivityHelpWindow.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CActivityHelpWindow::CActivityHelpWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, CUIGroup* pCaller, EngineDeviceMan* pEngineDevice) 
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pCaller(pCaller)
{
}

CActivityHelpWindow::~CActivityHelpWindow()
{
}

void CActivityHelpWindow::CreateSubControl()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    CUIControl* pControl;
    pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "ACTIVITY_HELP_WINDOW_BACKGROUND", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pControl->SetDiffuse( NS_UITEXTCOLOR::BLACK );
    pControl->SetUseRender( TRUE );
    RegisterControl( pControl );

    CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
    pTextBox->CreateSub( this, "ACTIVITY_HELP_WINDOW_TEXT", UI_FLAG_DEFAULT );
    pTextBox->SetFont( pFont );
    pTextBox->SetTextAlign( TEXT_ALIGN_LEFT );
    pTextBox->SetText( ID2GAMEINTEXT("ACTIVITY_HELP_TEXT", 0), NS_UITEXTCOLOR::WHITE );
    pTextBox->AddText( "" );
    pTextBox->AddText( ID2GAMEINTEXT("ACTIVITY_HELP_TEXT", 1), NS_UITEXTCOLOR::WHITE );
    pTextBox->AddText( "" );
    pTextBox->AddText( ID2GAMEINTEXT("ACTIVITY_HELP_TEXT", 2), NS_UITEXTCOLOR::WHITE );
    pTextBox->AddText( "" );
    pTextBox->AddText( ID2GAMEINTEXT("ACTIVITY_HELP_TEXT", 3), NS_UITEXTCOLOR::WHITE );
	pTextBox->AddText( "" );
	pTextBox->AddText( ID2GAMEINTEXT("ACTIVITY_HELP_TEXT", 4), NS_UITEXTCOLOR::WHITE );

    RegisterControl( pTextBox );
}

void CActivityHelpWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    NSGUI::SetCharMoveBlock ();

    switch ( ControlID )
    {
    case ET_CONTROL_BUTTON:
        {
            if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    }
}

void CActivityHelpWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );

    if ( bVisible )
    {
        m_pCaller->SetNoUpdate ( TRUE );
        m_pCaller->SetNoMessage( TRUE );
    }
    else
    {
        m_pCaller->SetNoUpdate ( FALSE );
        m_pCaller->SetNoMessage( FALSE );
    }
}
