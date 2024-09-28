#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIWindowTitle.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "PostBoxPage.h"
#include "PostBoxWindowSub.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxWindowSub::CPostBoxWindowSub ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CPostBoxWindowObject ( pInterface, pEngineDevice )
	, m_pHeadText ( NULL )
{
	// Blank
}

CPostBoxWindowSub::~CPostBoxWindowSub ()
{
	// Blank
}

void CPostBoxWindowSub::CreateSubControlEx ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// Note : 페이지 프레임
	{
		CreatePageFrame( "POSTBOX_WINDOW_SUB_PAGE" );
	}

	// Note : 헤드 텍스트
	{
		m_pHeadText = CreateStaticControl ( "POSTBOX_WINDOW_SUB_TEXT_HEAD", pFont, TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );
	}
}

void CPostBoxWindowSub::PageOpen ( CPostBoxPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	CPostBoxWindowObject::PageOpen ( pPage );

	// Note : 타이틀
	if ( m_pTitle )
	{
		m_pTitle->SetTitleName( pPage->GetTitleName().GetString() );
	}

	// Note : 헤드 텍스트
	if ( m_pHeadText )
	{
		m_pHeadText->ClearText();
		m_pHeadText->AddText( pPage->GetTitleName(), NS_UITEXTCOLOR::WHITE );
	}
}

void CPostBoxWindowSub::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CPostBoxWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxWindowSub::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CPostBoxWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CUIControl* pParent = GetParent();
	if ( pParent )
	{
		//
	}

}

