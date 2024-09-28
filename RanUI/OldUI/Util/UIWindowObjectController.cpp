
// bjju.sns

#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "../../ModalCallerID.h"

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "./ModalWindow.h"
#include "./UIWindowEx.h"
#include "./UIWindowObjectController.h"
#include "./UIWindowObject.h"
#include "./UIPage.h"
#include "./UIPageRequest.h"

//#include "../RanClientLib/G-Logic/GLogicEx.h"
//#include "../RanClientLib/G-Logic/GLGaeaClient.h"

#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIWindowObjectController::CUIWindowObjectController (CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface   (pInterface)
    , m_pd3dDevice	 ( NULL )
	, m_OpenWindowID ( 0 )
	, m_pOpenPage	 ( NULL )
    , m_bOpen        ( FALSE )
{
	// Blank
}

CUIWindowObjectController::~CUIWindowObjectController ()
{
	Release ();
}

void CUIWindowObjectController::Release ()
{
	// Note : 페이지들을 제거
	if ( !m_vecPage.empty() )
	{
		VECPAGEITER iter = m_vecPage.begin();
		for ( ; iter!=m_vecPage.end(); ++iter )
		{
			CUIPage* pPage = (*iter);

			if ( !pPage )
			{
				continue;
			}

			delete pPage;
		}

		m_vecPage.clear();
	}

	// Note : 윈도우들을 제거
	if ( !m_MapWindow.empty() )
	{
		/*
		MAPWINDOWITER iter = m_MapWindow.begin();
		for ( ; iter!=m_MapWindow.end(); ++iter )
		{
			CUIWindowObject* pWindow = iter->second;

			if ( !pWindow )
			{
				continue;
			}

			delete pWindow;
		}
		*/

		m_MapWindow.clear();
	}
}

HRESULT	CUIWindowObjectController::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	VECPAGEITER iter = m_vecPage.begin();
	for ( ; iter!=m_vecPage.end(); ++iter )
	{
		CUIPage* pPage = (*iter);

		if ( !pPage )
		{
			continue;
		}

		hr = pPage->InitDeviceObjects( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}


void CUIWindowObjectController::SetVisibleSingle ( BOOL bVisible )
{
	if ( bVisible )
	{
		WindowBegin ( m_OpenWindowID );
	}
	else
	{
		WindowEnd ();
	}
}

void CUIWindowObjectController::Close ()
{
	if ( IsWindowCloseAble(TRUE) )
	{
		m_pInterface->UiHideGroup ( GetWndID() );
	}
}

void CUIWindowObjectController::WindowResize ()
{
	// Note : bjju.WindowObject (리사이징 추후에 필요시 수정) 
	//        현재 활성화 되어있는 윈도우의 사이즈만큼 리사이즈 해줘야함

	UIRECT rcOldLocalPos  = GetLocalPos ();
	UIRECT rcNewLoacalPos;

	DWORD dwSize	 = (DWORD)m_vecSlot.size();
	FLOAT fMaxHeight = 0.0f;

	for ( DWORD i=0; i<dwSize; ++i )
	{
		CUIWindowObject* &pSlot = m_vecSlot[ i ];

		if ( !pSlot )
		{
			continue;
		}

		const UIRECT& rcSlot = pSlot->GetLocalPos();

		rcNewLoacalPos.sizeX += rcSlot.sizeX;

		if ( fMaxHeight < rcSlot.sizeY )
		{
			fMaxHeight = rcSlot.sizeY;
		}
	}

	rcNewLoacalPos.sizeY  = fMaxHeight;
	rcNewLoacalPos.right  = rcNewLoacalPos.left		+ rcNewLoacalPos.sizeX;
	rcNewLoacalPos.bottom = rcNewLoacalPos.bottom	+ rcNewLoacalPos.sizeY;

	CUIControl::SetLocalPos ( rcNewLoacalPos );

	UIRECT rcGlobalPos = GetGlobalPos ();

	rcGlobalPos.right  = rcGlobalPos.left + rcNewLoacalPos.sizeX;
	rcGlobalPos.bottom = rcGlobalPos.top  + rcNewLoacalPos.sizeY;
	rcGlobalPos.sizeX  = rcNewLoacalPos.sizeX;
	rcGlobalPos.sizeY  = rcNewLoacalPos.sizeY;

	CUIControl::SetGlobalPos ( rcGlobalPos );
}

const BOOL CUIWindowObjectController::WindowRegister ( CUIWindowObject* pWindow, UIGUID WndControlID )
{
	if ( !pWindow )
	{
		return FALSE;
	}

	MAPWINDOWITER FindIter = m_MapWindow.find ( WndControlID );

	if ( FindIter != m_MapWindow.end() )
	{
		return FALSE;
	}

	m_MapWindow.insert( std::make_pair<UIGUID, CUIWindowObject*>(WndControlID, pWindow) );

	pWindow->SetParent				( this );
	pWindow->SetParentController	( this );
	pWindow->SetVisibleSingle		( FALSE );

	//pWindow->SetGlobalPos	  ( GetGlobalPos() );
	RegisterControl ( pWindow );

	return TRUE;
}

const BOOL CUIWindowObjectController::PageRegister ( CUIPage* pPage )
{
	m_vecPage.push_back( pPage );
	return TRUE;
}

void CUIWindowObjectController::WindowBegin ( UIGUID WndControlID )
{
	if ( m_pd3dDevice )
	{
        if ( IsVisible() && m_bOpen )
        {
            return;
        }

		if ( !IsWindowOpenAble() )
		{
			return;
		}

		CUIWindowObject* pWindow = NULL;

		MAPWINDOWITER Finditer = m_MapWindow.find( WndControlID );

		if ( Finditer == m_MapWindow.end() )
		{
			return;
		}

		pWindow = Finditer->second;

		// Note : 활성화 된 슬롯이 있다면 전부삭제
		if ( GetEnableSlotCount () )
		{
			for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
			{
				CUIWindowObject* &pSlot = m_vecSlot[ i ];

				if ( pSlot )
				{
					pSlot->SetSlot		( SLOT_NULL );
					pSlot->PageClose	();
					pSlot->WindowClose	();
					pSlot = NULL;
				}
			}
		}

		if ( !m_bOpen && m_pd3dDevice )
		{
			VECPAGEITER iter = m_vecPage.begin();
			for ( ; iter!=m_vecPage.end(); ++iter )
			{
				CUIPage* pPage = (*iter);

				if ( !pPage )
				{
					continue;
				}

				pPage->Begin();
			}

			
			// Note : 페이지 오픈
			WindowPageOpenSlot( pWindow, m_pOpenPage, 0 );

            m_bOpen = TRUE;
		}
	}

	CUIGroup::SetVisibleSingle ( TRUE );
}

void CUIWindowObjectController::WindowEnd ()
{
    if ( !IsVisible() && !m_bOpen )
    {
        return;
    }

	if ( m_pd3dDevice )
	{
		if ( !IsWindowCloseAble (TRUE) )
		{
			return;
		}

		// Note : 활성화 된 슬롯이 있다면 전부삭제
		if ( GetEnableSlotCount ()  )
		{
			for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
			{
				CUIWindowObject* &pSlot = m_vecSlot[ i ];

				if ( pSlot )
				{
					pSlot->SetSlot ( SLOT_NULL );
					pSlot->PageClose   ();
					pSlot->WindowClose ();
					pSlot = NULL;
				}
			}
		}

		if ( m_bOpen && m_pd3dDevice )
		{
			VECPAGEITER iter = m_vecPage.begin();
			for ( ; iter!=m_vecPage.end(); ++iter )
			{
				CUIPage* pPage = (*iter);

				if ( !pPage )
				{
					continue;
				}

				pPage->End();
			}
		}
	}

	// Note : 컨트롤 독점 리셋
	ResetExclusiveControl();

	// Note : 숨기기
	CUIGroup::SetVisibleSingle ( FALSE );
	m_bOpen = FALSE;
}

void CUIWindowObjectController::WindowPageOpen  ( UIGUID WndControlID, CUIPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	CUIWindowObject* pWindow = NULL;

	MAPWINDOWITER Finditer = m_MapWindow.find( WndControlID );

	if ( Finditer == m_MapWindow.end() )
	{
		return;
	}

	pWindow = Finditer->second;
	
	if ( !pWindow )
	{
		return;
	}

	// Note : 슬롯 설정
	DWORD dwSlot = 0;

	// Note : 이미 슬롯이 열려있다면 해당 슬롯을 가져온다.
	if ( pWindow->IsWindowOpen() )
	{
		dwSlot = pWindow->GetSlot();
	}
	// Note : 비어있는 슬롯을 찾는다.
	else
	{
		dwSlot = GetEmptySlot ();
	}

	if ( dwSlot == SLOT_NULL )
	{
		return;
	}

	WindowPageOpenSlot ( pWindow, pPage, dwSlot );
}

void CUIWindowObjectController::WindowPageOpen	( CUIWindowObject* pWindow, CUIPage* pPage )
{
	if ( !pWindow )
	{
		return;
	}

	DWORD dwSlot = 0;

	// Note : 이미 슬롯이 열려있다면 해당 슬롯을 가져온다.
	if ( pWindow->IsWindowOpen() )
	{
		dwSlot = pWindow->GetSlot();
	}
	// Note : 비어있는 슬롯을 찾는다.
	else
	{
		dwSlot = GetEmptySlot ();
	}

	if ( dwSlot == SLOT_NULL )
	{
		return;
	}

	WindowPageOpenSlot ( pWindow, pPage, dwSlot );
}

void CUIWindowObjectController::WindowPageOpenSlot  ( CUIWindowObject* pWindow, CUIPage* pPage, const DWORD dwSlot )
{
	if ( !pWindow )
	{
		return;
	}

	// Note : 해당 윈도우가 이미 열려있는 윈도우라면 닫고 새로 연다.
	if ( pWindow->IsWindowOpen() )
	{
		DWORD dwCurSlot = pWindow->GetSlot();

		if ( dwCurSlot == SLOT_NULL )
		{
			return;
		}

		m_vecSlot[ dwCurSlot ] = NULL;

		pWindow->SetSlot	 ( SLOT_NULL );
		pWindow->PageClose   ();
		pWindow->WindowClose ();
	}

	// Note : 사이즈 동기화
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();

	if ( dwSlotSize <= dwSlot )
	{
		m_vecSlot.resize( dwSlotSize + 1, NULL );
	}

	CUIWindowObject* &pSlot = m_vecSlot[ dwSlot ];

	// Note : 해당슬롯에 사용하던 윈도우 닫기
	if ( pSlot && pSlot->IsWindowOpen() )
	{
		pSlot->SetSlot ( SLOT_NULL );
		pSlot->PageClose   ();
		pSlot->WindowClose ();
	}

	pSlot = pWindow;
	pSlot->SetSlot( dwSlot );

	pSlot->WindowOpen ();
	pSlot->PageOpen   ( pPage );
	if(pPage)
		pPage->OnPageVisible(true);

	m_pOpenPage = pPage;
	/* bjju.WindowObject (리사이징 추후에 필요시 수정)
	// Note : 슬롯 리사이징
	CString strSlotControl;
	strSlotControl.Format( "POSTBOX_WINDOW_SLOT_%d", dwSlot );
	pSlot->SetResize( strSlotControl.GetString() );


	WindowResize ();
	*/
}

void CUIWindowObjectController::WindowPageClose ( CUIPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	WindowPageClose ( pPage->GetParentWindow() );
}

void CUIWindowObjectController::WindowPageClose ( UIGUID WndControlID )
{
	CUIWindowObject* pWindow = NULL;

	MAPWINDOWITER Finditer = m_MapWindow.find( WndControlID );

	if ( Finditer == m_MapWindow.end() )
	{
		return;
	}

	pWindow = Finditer->second;

	if ( !pWindow )
	{
		return;
	}

	WindowPageClose ( pWindow );
}

void CUIWindowObjectController::WindowPageClose ( CUIWindowObject* pWindow )
{
	if ( !pWindow )
	{
		return;
	}

	//  Note : 슬롯을 순서대로 제거한다 
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();
	BOOL  dwBegin    = pWindow->GetSlot();

	// Note : 메인 슬롯을 닫으려할때는 닫기 메서드를 호출
	if ( dwBegin == 0 )
	{
		Close();
		return;
	}

	if ( dwBegin != SLOT_NULL )
	{
		for ( DWORD i=dwBegin; i<dwSlotSize; ++i )
		{
			CUIWindowObject* &pSlot = m_vecSlot[ i ];

			if ( pSlot )
			{
				pSlot->PageClose   ();
				pSlot->WindowClose ();
				pSlot = NULL;
			}
		}

		WindowResize ();
	}

	// Note : 활성화 된 슬롯이 하나도 없다면
	if ( GetEnableSlotCount () == 0 )
	{
		// Note : 본창을 닫는다.
		Close();
	}
}

const BOOL CUIWindowObjectController::IsWindowOpenAble ( const BOOL bPrint )
{
	return TRUE;
}

const BOOL CUIWindowObjectController::IsWindowCloseAble ( const BOOL bPrint )
{
	return TRUE;
}

const BOOL CUIWindowObjectController::IsPageOpen ( CUIPage* pPage )
{
	if ( !pPage )
	{
		return FALSE;
	}

	for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
	{
		CUIWindowObject* &pSlot = m_vecSlot[ i ];

		if ( pSlot )
		{
			if ( pPage == pSlot->GetPage() )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

const DWORD	CUIWindowObjectController::GetEnableSlotCount ()
{
	// Note : 활성화 되어있는 슬롯의 갯수를 가져온다
	DWORD   dwSlotSize	   = (DWORD)m_vecSlot.size();
	DWORD   dwSlotCount	   = 0;

	for ( DWORD i=0; i<dwSlotSize; ++i )
	{
		if ( !m_vecSlot[i] )
		{
			continue;
		}

		++dwSlotCount;
	}

	return dwSlotCount;
}

const DWORD	CUIWindowObjectController::GetEmptySlot ()
{
	// Note : 비어있는 슬롯을 찾는다.
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();
	DWORD dwSlot	 = dwSlotSize;

	for ( DWORD i=0; i<dwSlotSize; ++i )
	{
		if ( !m_vecSlot[i] )
		{
			dwSlot = i;
			break;
		}
	}

	return dwSlot;
}

void CUIWindowObjectController::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	// Note : 닫기
	if ( dwMsg&UIMSG_WINDOW_OBJECT_CLOSE )
	{
		WindowPageClose ( ControlID );
	}
}


void CUIWindowObjectController::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	// Note : 활성화된 슬롯을 돌면서 요청 처리
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();
	DWORD dwSlot	 = dwSlotSize;

	for ( DWORD i=0; i<dwSlotSize; ++i )
	{
		if ( !m_vecSlot[i] )
		{
			continue;
		}

		CUIWindowObject* pWindow = m_vecSlot[i];

		if ( !pWindow )
		{
			continue;
		}

		while ( 1 )
		{
            SPAGEREQUEST sRequest = pWindow->RequestPop();
			if ( sRequest.dwID == WINDOW_PAGE_REQ_NONE ) break;

			UpdatePageRequest (  pWindow->GetWndID(), sRequest );
		}
	}
	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIWindowObjectController::UpdatePageRequest ( UIGUID ControlID, const SPAGEREQUEST& sRequest )
{
	switch ( sRequest.dwID )
	{
	case WINDOW_PAGE_REQ_NONE: 
		break;
	};
}


void CUIWindowObjectController::SetFocusControl ()
{
    CUIControl::SetFocusControl();

    std::for_each( m_MapWindow.begin(), m_MapWindow.end(), boost::bind(&CUIWindowObject::SetFocusControl, 
        boost::bind(&MAPWINDOW::value_type::second, _1) ) );

}

void CUIWindowObjectController::ResetFocusControl ()
{
    CUIControl::ResetFocusControl();

    std::for_each( m_MapWindow.begin(), m_MapWindow.end(), boost::bind(&CUIWindowObject::ResetFocusControl,
        boost::bind(&MAPWINDOW::value_type::second, _1) ) );
}