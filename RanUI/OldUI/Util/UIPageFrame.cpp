
// bjju.sns

#include "StdAfx.h"

#include "./UIPage.h"
#include "./UIPageFrame.h"
#include "./UIWindowObject.h"
#include "./UIWindowObjectController.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIPageFrame::CUIPageFrame(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pPage			( NULL )
	, m_dwAniType		( PAGE_ANIMATION_NONE )
	, m_fAniTime		( 0.0f )
	, m_fAniSpeedPerSec ( 1.0f )
	, m_pAniPageDest	( NULL )
	, m_bAniPlay		( FALSE )
{
	//  Blank
}

CUIPageFrame::~CUIPageFrame ()
{
	m_ControlContainer.RemoveAll( false );
}

void CUIPageFrame::PageOpen ( CUIWindowObject* pParentWindow, CUIPage* pPage, BOOL bAnimation )
{
	if ( m_pPage == pPage )
	{
		return;
	}

	if ( m_pPage && bAnimation )
	{
		BeginOpenAnimation ( pParentWindow, pPage );
	}
	else
	{
		PageClose ();

		if ( !pPage )
		{
			return;
		}

		pPage->SetParent		( this );
		pPage->SetVisibleSingle	( TRUE );
		RegisterControl			( pPage );
		SetGlobalPos ( GetGlobalPos() );

		pPage->Open ( pParentWindow );
		m_pPage = pPage;
	}
}

void CUIPageFrame::PageClose ()
{
	if ( m_bAniPlay )
	{
		m_fAniTime = 1.0f;
		UpdateAnimation ( 0.0f );
	}

	if ( m_pPage )
	{
		m_pPage->Close();

		m_ControlContainer.EraseControl( m_pPage->GetWndID(), FALSE );

		m_pPage->SetParent( NULL );
		m_pPage->SetVisibleSingle( FALSE );
		m_pPage = NULL;
	}
}

void CUIPageFrame::BeginSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->BeginSubControl();
	}
}

void CUIPageFrame::OpenSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->OpenSubControl();
	}
}

void CUIPageFrame::CloseSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->CloseSubControl();
	}
}

void CUIPageFrame::EndSubControl ()
{
	if ( m_pPage )
	{
		m_pPage->EndSubControl();
	}
}

void CUIPageFrame::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	UpdateAnimation	( fElapsedTime );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIPageFrame::UpdateAnimation ( float fElapsedTime )
{
	if ( !m_bAniPlay )
	{
		return;
	}

	FLOAT fSpeed = m_fAniSpeedPerSec*fElapsedTime;
	m_fAniTime += fSpeed;

	if ( m_fAniTime >= 1.0f )
	{
		m_fAniTime = 1.0f;
		m_bAniPlay = FALSE;
	}

	switch ( m_dwAniType )
	{
	default:
	case PAGE_ANIMATION_NONE:
		{
			PageClose ();
			m_pPage		= m_pAniPageDest;
			m_bAniPlay	= FALSE;
			return;
		}
		break;

	case PAGE_ANIMATION_ALPHA:
		{
			if ( m_pPage )
			{
				WORD wAlpha = (WORD)(255.0f*(1.0f-m_fAniTime));
				m_pPage->SetDiffuseAlpha( wAlpha );
				m_pPage->SetNoMessage( TRUE );
				m_pPage->SetNoUpdate ( TRUE );
			}

			if ( m_pAniPageDest )
			{
				WORD wAlpha = (WORD)(255.0f*(m_fAniTime));
				m_pAniPageDest->SetDiffuseAlpha( wAlpha );
				m_pAniPageDest->SetNoMessage( TRUE );
				m_pAniPageDest->SetNoUpdate ( TRUE );
			}
		}
		break;
	};

	if ( m_fAniTime >= 1.0f )
	{
		if ( m_pPage )
		{
			m_pPage->SetNoMessage    ( FALSE );
            m_pPage->SetNoUpdate     ( FALSE );
            m_pPage->SetDiffuseAlpha ( 255 );
		}

		if ( m_pAniPageDest )
		{
			m_pAniPageDest->SetNoMessage    ( FALSE );
            m_pAniPageDest->SetNoUpdate     ( FALSE );
            m_pAniPageDest->SetDiffuseAlpha ( 255 );
		}

		PageClose ();
		m_pPage			= m_pAniPageDest;
		m_bAniPlay		= FALSE;
		m_pAniPageDest	= NULL;
	}
}

void CUIPageFrame::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    AddMessageEx( dwMsg );

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CUIPageFrame::BeginOpenAnimation ( CUIWindowObject* pParentWindow, CUIPage* pPage )
{
	m_fAniTime	 = 0.0f;
	m_bAniPlay	 = FALSE;

	switch ( m_dwAniType )
	{
	default:
	case PAGE_ANIMATION_NONE:
		{
			PageClose ();

			if ( !pPage )
			{
				return;
			}

			pPage->SetParent		( this );
			pPage->SetVisibleSingle	( TRUE );
			RegisterControl			( pPage );
			SetGlobalPos ( GetGlobalPos() );

			pPage->Open ( pParentWindow );
			m_pPage = pPage;
		}
		break;

	case PAGE_ANIMATION_ALPHA:
		{
			if ( m_pPage )
			{
				m_pPage->SetDiffuseAlpha( 0 );
				m_pPage->SetNoMessage( TRUE );
				m_pPage->SetNoUpdate ( TRUE );
			}

			if ( pPage )
			{
				pPage->SetParent		( this );
				pPage->SetVisibleSingle	( TRUE );
				RegisterControl			( pPage );
				SetGlobalPos ( GetGlobalPos() );

				pPage->Open ( pParentWindow );
			}

			m_pAniPageDest	= pPage;
			m_fAniTime		= 0.0f;
			m_bAniPlay		= TRUE;
		}
		break;
	};
}
