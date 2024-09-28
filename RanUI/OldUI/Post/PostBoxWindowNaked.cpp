#include "StdAfx.h"

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
#include "PostBoxWindowNaked.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxWindowNaked::CPostBoxWindowNaked ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPostBoxWindowObject ( pInterface, pEngineDevice )
{
	// Blank
}

CPostBoxWindowNaked::~CPostBoxWindowNaked ()
{
	// Blank
}

void CPostBoxWindowNaked::CreateSubControlEx ()
{
	// Note : 페이지 프레임
	{
		CreatePageFrame( "POSTBOX_WINDOW_NAKED_PAGE" );
	}
}

void CPostBoxWindowNaked::PageOpen ( CPostBoxPage* pPage )
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
}

void CPostBoxWindowNaked::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CPostBoxWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxWindowNaked::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CPostBoxWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

