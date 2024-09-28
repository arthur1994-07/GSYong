#include "StdAfx.h"
#include "InfoDisplayLink.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CInfoDisplayLink::CInfoDisplayLink(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CBasicVarTextBox(pEngineDevice)
    , m_pInterface ( pInterface )	
	, m_pClose ( NULL )
{
	
}

CInfoDisplayLink::~CInfoDisplayLink()
{
}

void CInfoDisplayLink::CreateSubControl ()
{
	m_pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_VAR_LINE_BOX_LINK", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_VAR_LINE_BOX_LINK );
	pLineBox->CreateBaseBoxVarTextBox ( "BASIC_VAR_LINE_BOX_LINK" );
	pLineBox->SetNoUpdate ( true );
	RegisterControl ( pLineBox );
	m_pLineBox = pLineBox;

	CUIControl* pSelfDummy = new CUIControl(m_pEngineDevice);
	pSelfDummy->CreateSub ( this, "BASIC_VAR_LINE_BOX_LINK", BASIC_VAR_LINE_BOX_LINK_DUMMY );
	pSelfDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pSelfDummy );
	m_pSelfDummy = pSelfDummy;

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "VAR_TEXT_BOX_LINK", UI_FLAG_XSIZE | UI_FLAG_YSIZE, VAR_TEXT_BOX_LINK );
	pTextBox->SetFont ( m_pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetNoUpdate ( true );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;

	m_pClose = new CBasicButton(m_pEngineDevice);	
	m_pClose->CreateSub ( this, "INFO_DISPLAY_LINK_CLOSE", UI_FLAG_RIGHT, ET_CONTROL_BUTTON );
	m_pClose->CreateFlip ( "INFO_DISPLAY_LINK_CLOSE_F", CBasicButton::CLICK_FLIP );
	m_pClose->CreateMouseOver ( "INFO_DISPLAY_LINK_CLOSE_OVER" );
	RegisterControl ( m_pClose );
}

VOID CInfoDisplayLink::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ET_CONTROL_BUTTON:
		
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			m_pInterface->UiHideGroup ( GetWndID() );			
		}
		break;
	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

}

void CInfoDisplayLink::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	한번만 설정한다.
	if ( !m_bBLOCK_MOUSETRACKING )
	{
		m_bBLOCK_MOUSETRACKING = true;
		RePosControl( x, y );	
	}
}

void CInfoDisplayLink::RePosControl ( int x, int y )
{

	const UIRECT& rcTextBoxLocalPos = m_pTextBox->GetLocalPos ();

	const UIRECT& rcLocalPosDummy = m_pSelfDummy->GetLocalPos ();
	const UIRECT& rcOriginPos = GetLocalPos ();

	float fCloseSize = m_pClose->GetGlobalPos().sizeY;

	AlignSubControl ( rcOriginPos, rcLocalPosDummy );

	if ( m_pTextBox )
	{
		int nTotalLine = m_pTextBox->GetTotalLine ();
		const float fAllLine = m_pTextBox->CalcMaxHEIGHT ( 0, nTotalLine );
		const float fTextBoxGapX = (float)m_pTextBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
		const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY + fCloseSize;
		
//		UIRECT rcLocalNewPos = UIRECT ( float(x) + m_vMousePointGap.x, float(y) + m_vMousePointGap.y, 
//			rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );
		UIRECT rcLocalNewPos = UIRECT ( rcOriginPos.left, rcOriginPos.top, 
			rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );

		AlignSubControl ( rcLocalPosDummy, rcLocalNewPos );

		SetGlobalPos ( rcLocalNewPos );		
	}
}

void CInfoDisplayLink::SetVisibleSingle ( BOOL bVisible )
{
	CBasicVarTextBox::SetVisibleSingle( bVisible );
	if ( bVisible ) m_bBLOCK_MOUSETRACKING = false;
}