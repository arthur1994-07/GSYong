#include "StdAfx.h"
#include "./ActivityListButton.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ActivityListButton::ActivityListButton(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pTextBox( NULL )
    , m_pButton( NULL ) 
{
}

ActivityListButton::~ActivityListButton()
{
}

void ActivityListButton::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    m_pTextBox = new CBasicTextBox(m_pEngineDevice);
	m_pTextBox->CreateSub ( this, "SPEC_ACT_LIST_BUTTON_TEXT" );
    m_pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	m_pTextBox->SetFont ( pFont9 );
	RegisterControl ( m_pTextBox );

    m_pButton = new CBasicButton(m_pEngineDevice);
	m_pButton->CreateSub ( this, "SPEC_ACT_LIST_BUTTON" );
	m_pButton->CreateFlip ( "SPEC_ACT_LIST_BUTTON_FLIP", CBasicButton::RADIO_FLIP );
	RegisterControl ( m_pButton );
}

void ActivityListButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void ActivityListButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}


void  ActivityListButton::SetText ( const CString& strText )
{ 
    m_pTextBox->SetOneLineText ( strText );
}

void ActivityListButton::SetFlip ( BOOL bFlip )
{
    m_pButton->SetFlip( bFlip );
}