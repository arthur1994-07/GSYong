#include "StdAfx.h"
#include "MessageWindow.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMessageWindow::CMessageWindow(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pTextBox(NULL)
{
}

CMessageWindow::~CMessageWindow ()
{
}

void CMessageWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessage ( "BASIC_MESSAGE_LINE_BOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );	

	pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_MESSAGE_LINE_TEXTBOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_MESSAGE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
	pTextBox->ResetAlignFlag ();
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CMessageWindow::SetMessage ( const CString& strMessage )
{
	if ( m_pTextBox )
	{
		m_pTextBox->SetText ( strMessage );
	}
}

void CMessageWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
