#include "stdafx.h"

#include "../../../InternalCommonLib/dxstdafx.h"

#include "../../../EngineLib/DxBase/RenderDevice.h"
#include "../../../EngineLib/DxTools/DxInputDevice.h"

#include "../Frame.h"
#include "./ClientView.h"

#include "../../../SigmaCore/DebugInclude.h"

CClientView::CClientView()
    : m_pFrame( NULL )
    , m_nToolBarID( 0 )
{
}

CClientView::~CClientView()
{
}

LRESULT CClientView::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT lr = CWnd::WindowProc( message, wParam, lParam );

    switch( message )
    {
    case WM_CREATE:
        {
            m_pFrame = DYNAMIC_DOWNCAST( CFrame, GetParent() );

            if( m_nToolBarID != 0 )
            {
                m_wndToolBar.CreateToolBar(
                    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                    this );

                m_wndToolBar.LoadToolBar( m_nToolBarID );
            }
        }
        break;

    case WM_SIZE:
        {
            if( m_wndToolBar.GetSafeHwnd() )
	        {
		        CSize sz = m_wndToolBar.CalcDockingLayout(
                    LOWORD( lParam ),
                    LM_HORZ | LM_HORZDOCK | LM_COMMIT );

		        m_wndToolBar.MoveWindow( 0, 0, LOWORD( lParam ), sz.cy );
		        m_wndToolBar.Invalidate( FALSE );
	        }
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        {
			if( m_pFrame )
                m_pFrame->ChangeActiveView( GetTargetWnd() );

            SetCapture();
        }
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        {
            ReleaseCapture();
        }
        break;
    }

    lr |= CD3DViewport::WindowProc( message, wParam, lParam );

    return lr;
}

VOID CClientView::CalcSourceRect()
{
    CD3DViewport::CalcSourceRect();

    if( m_wndToolBar.GetSafeHwnd() )
    {
        CRect cToolBarRect;
        m_wndToolBar.GetClientRect( &cToolBarRect );

        INT nToolBarHeight = cToolBarRect.Height();

        if( m_cSourceRect.Height() <= nToolBarHeight )
            m_cSourceRect.top = m_cSourceRect.bottom = nToolBarHeight;
        else
            m_cSourceRect.bottom -= nToolBarHeight;
    }
}

VOID CClientView::CalcDestRect()
{
    CD3DViewport::CalcDestRect();

    if( m_wndToolBar.GetSafeHwnd() )
    {
        CRect cToolBarRect;
        m_wndToolBar.GetClientRect( &cToolBarRect );

        INT nToolBarHeight = cToolBarRect.Height();

        if( m_cDestRect.Height() <= nToolBarHeight )
            m_cDestRect.top = m_cDestRect.bottom = nToolBarHeight;
        else
            m_cDestRect.top += nToolBarHeight;
    }
}

ic::Int2 CClientView::GetClientPos( const ic::Int2& sScreen )
{
    const CRect& cDestRect = GetDestRect();
    POINT sClient = { sScreen.x, sScreen.y };
    ScreenToClient( &sClient );

    return ic::Int2( sClient.x, sClient.y - cDestRect.top );
}

ic::Int2 CClientView::GetClientMousePos()
{
    ic::Int2 sScreen;
    sScreen.x = DxInputDevice::GetInstance().GetMouseLocateX();
    sScreen.y = DxInputDevice::GetInstance().GetMouseLocateY();

    return GetClientPos( sScreen );
}

BOOL CClientView::InViewClientPos( const ic::Int2& sClient )
{
    const CRect& cSourceRect = GetSourceRect();
    POINT ptPoint = { sClient.x, sClient.y };

    return cSourceRect.PtInRect( ptPoint );
}

BOOL CClientView::InViewClientMousePos()
{
    return InViewClientPos( GetClientMousePos() );
}