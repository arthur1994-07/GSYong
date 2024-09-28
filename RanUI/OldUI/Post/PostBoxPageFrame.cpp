#include "StdAfx.h"

#include "PostBoxPage.h"
#include "PostBoxPageFrame.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxPageFrame::CPostBoxPageFrame(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pPage ( NULL )
{
	//  Blank
}

CPostBoxPageFrame::~CPostBoxPageFrame ()
{
	m_ControlContainer.RemoveAll( false );
}

void CPostBoxPageFrame::PageOpen ( CPostBoxWindowObject* pParentWindow, CPostBoxPage* pPage )
{
	if ( m_pPage == pPage )
	{
		return;
	}

	PageClose ();

	if ( !pPage )
	{
		return;
	}

	const UIRECT& rcLocalPos = GetLocalPos ();

	pPage->SetParent ( this );
	pPage->SetVisibleSingle( TRUE );
	RegisterControl( pPage );
	SetGlobalPos ( GetGlobalPos() );

	pPage->Open ( pParentWindow );

	m_pPage = pPage;
}

void CPostBoxPageFrame::PageClose ()
{
	if ( m_pPage )
	{
		m_pPage->Close();

		m_ControlContainer.EraseControl( m_pPage->GetWndID(), FALSE );

		m_pPage->SetParent( NULL );
		m_pPage->SetVisibleSingle( FALSE );
		m_pPage = NULL;
	}
}

void CPostBoxPageFrame::BeginSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->BeginSubControl();
	}
}

void CPostBoxPageFrame::OpenSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->OpenSubControl();
	}
}

void CPostBoxPageFrame::CloseSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->CloseSubControl();
	}
}

void CPostBoxPageFrame::EndSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->EndSubControl();
	}
}

void CPostBoxPageFrame::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxPageFrame::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    AddMessageEx( dwMsg );

    //  Memo : 아래와 같이 디자인 된 코드를 위와 같이 변경하였다.
	/*switch ( ControlID )
	{
	default:
		AddMessageEx( dwMsg );
		break;
	}*/

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

}

