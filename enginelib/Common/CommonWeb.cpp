#include "pch.h"
#include "./CommonWeb.h"

#include "../../MfcExLib/ExLibs/MinWebbrowser.h"

#include "../DxTools/DxInputDevice.h"
#include "../DxTools/TextureManager.h"
#include "../TextTexture/TextUtil.h"

#include "../SigmaCore/DebugInclude.h"

namespace COMMON_WEB
{
	enum
	{
		WEB_CTRL_CNT = 10,
		TEXTURE_SIZE = 1024,
	};

    typedef std::list< MinWebbrowser* > WEB_LIST;
    typedef WEB_LIST::iterator WEB_ITOR;

	static CWnd* g_pParentWnd = NULL;
	static MinWebbrowser g_cWebBrowser[ WEB_CTRL_CNT ];
    static WEB_LIST g_listWeb;

	static LPDIRECT3DDEVICEQ g_pd3dDevice = NULL;
	static LPDIRECT3DTEXTUREQ g_pTexture = NULL;
	static LPDIRECT3DSURFACEQ g_pSurface = NULL;
}

namespace COMMON_WEB
{
	VOID Create( CWnd* pParent, TSTRING strAppPath )
	{
		if( !pParent )
			return;

        RECT rtBound = { 0, 0, 1, 1 };

		for( INT i=0; i<WEB_CTRL_CNT; ++i )
		{
            g_cWebBrowser[ i ].Create(
                strAppPath,
				WS_OVERLAPPED,
				rtBound,
				pParent,
				(INT)AFX_IDW_PANE_FIRST + i );
		}

        g_pParentWnd = pParent;
	}

	VOID Destroy()
	{
        for( INT i=0; i<WEB_CTRL_CNT; ++i )
            g_cWebBrowser[ i ].Destroy();
	}

	BOOL IsCreate()
	{
        return ( g_pParentWnd != NULL ) ? TRUE : FALSE;
	}

	INT IssueNewID()
	{
		for( INT i=1; i<WEB_CTRL_CNT; ++i )
		{
			if( g_cWebBrowser[ i ].GetIdentity() >= 0 )
				continue;

			g_cWebBrowser[ i ].SetIdentity( i );

			return i;
		}

		g_cWebBrowser[ 0 ].SetIdentity( 0 );

		return 0;
	}

    VOID SetFocusParent()
	{
		if( g_pParentWnd )
			g_pParentWnd->SetFocus();
	}

    VOID TranslateAccelerator( MSG& msg )
    {
        if( g_listWeb.empty() )
            return;

        MinWebbrowser* pWeb = g_listWeb.front();
        if( pWeb )
            pWeb->TranslateAccelerator( msg );
    }

	VOID CheckIntegrity( CString& strUrl )
	{
        g_cWebBrowser[ 0 ].CheckIntegrity( strUrl );
	}

    VOID SetVisible( INT iID, BOOL bVisible )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return;

        WEB_ITOR it = g_listWeb.begin();
        while( it != g_listWeb.end() )
        {
            if( *it == &g_cWebBrowser[ iID ] )
                g_listWeb.erase( it++ );
            else
                ++it;
        }

        if( bVisible )
            g_listWeb.push_back( &g_cWebBrowser[ iID ] );
	}

	VOID Navigate( INT iID, const TCHAR* pszPath, BOOL bShow, const TCHAR* pszPOST/* = NULL*/ )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return;

        g_cWebBrowser[ iID ].Navigate( CString( pszPath ), bShow, pszPOST );
	}

	VOID MoveWindow( INT iID, INT iX, INT iY, INT iWidth, INT iHeight, BOOL bRepaint )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return;

		g_cWebBrowser[ iID ].MoveWindow(
            iX,
            iY,
            iWidth,
            iHeight,
            bRepaint,
            FLOAT( TEXTURE_SIZE ) );
	}

	VOID Refresh( INT iID )
	{
		if( !GetVisible( iID ) )
			return;

		g_cWebBrowser[ iID ].Refresh();
	}

    INT GetTopWindowID()
    {
        if( g_listWeb.empty() )
            return INT_MIN;

        MinWebbrowser* pWeb = g_listWeb.front();
        if( pWeb )
            return pWeb->GetIdentity();

        return INT_MIN;
    }

    BOOL GetVisible( INT iID )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return FALSE;

        WEB_ITOR it = g_listWeb.begin();
        while( it != g_listWeb.end() )
        {
            if( *it == &g_cWebBrowser[ iID ] )
                return TRUE;
            ++it;
        }

        return FALSE;
	}

	BOOL IsEditFocus( INT iID )
	{
		if( !GetVisible( iID ) )
			return FALSE;

        return g_cWebBrowser[ iID ].IsEditFocus();
	}

    BOOL IsCompleteLoad( INT iID )
    {
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return FALSE;

        return g_cWebBrowser[ iID ].IsCompleteLoad();
    }

	VOID SetCompleteLoad( INT iID, BOOL bCompleteLoad )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return;

		g_cWebBrowser[ iID ].SetCompleteLoad( bCompleteLoad );
	}

    TSTRING GetDocument( INT iID )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return TSTRING( _T( "" ) );

        TSTRING strDocument;
        g_cWebBrowser[ iID ].GetDocument( strDocument );

        return strDocument;
	}

	VOID* GetWebBrowser( INT iID )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return NULL;

		return g_cWebBrowser[ iID ].GetWebBrowser();
	}

	TSTRING GetLocationUrl( INT iID )
	{
		if( iID < 0 || iID >= WEB_CTRL_CNT )
			return TSTRING( _T( "" ) );

		TSTRING strUrl;
        g_cWebBrowser[ iID ].GetLocationUrl( strUrl );

		return strUrl;
	}

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_pd3dDevice = pd3dDevice;

		UINT uiWidth( TEXTURE_SIZE );
		UINT uiHeight( TEXTURE_SIZE );
		D3DFORMAT emFormat( D3DFMT_R5G6B5 );

        HRESULT hr = D3DXCreateTexture(
            pd3dDevice,
            uiWidth,
            uiHeight,
            1,
            0,
		    emFormat,
            D3DPOOL_MANAGED,
            &g_pTexture );

		if( g_pTexture )
			g_pTexture->GetSurfaceLevel( 0, &g_pSurface );
	}

	VOID Render()
	{
		if( !g_pd3dDevice || !g_pTexture || !g_pSurface )
			return;

        if( g_listWeb.empty() )
            return;

        CTextUtil::Get()->Render( TRUE );

        g_pd3dDevice->SetFVF( VERTEXRHW::FVF );
		g_pd3dDevice->SetTexture( 0, g_pTexture );

        WEB_ITOR it = g_listWeb.begin();
        while( it != g_listWeb.end() )
        {
            MinWebbrowser* pWeb = *it;
            ++it;

            HDC hdcTex = NULL;
		    g_pSurface->GetDC( &hdcTex );
            pWeb->StretchBlt( hdcTex );
		    g_pSurface->ReleaseDC( hdcTex );

		    g_pd3dDevice->DrawPrimitiveUP(
                D3DPT_TRIANGLESTRIP,
                2,
                pWeb->GetPrintVertex(),
                sizeof( VERTEXRHW ) );
        }

        g_pd3dDevice->SetTexture( 0, NULL );

        CTextUtil::Get()->Render( FALSE );
	}

	VOID DeleteDeviceObjects()
	{
		SAFE_RELEASE( g_pSurface );
        SAFE_RELEASE( g_pTexture );

		g_pd3dDevice = NULL;
	}

	VOID TestVisible( DxInputDevice* pInput, WORD nWidth, WORD nHeight )
	{
		if( !pInput )
			return;

		if( ( pInput->GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED ) &&
			( pInput->GetKeyState( DIK_S ) & DXKEY_UP ) )
		{
			static INT s_iWebID = IssueNewID();

			WORD wX_RES = nWidth;
			WORD wY_RES = nHeight;

			if( !GetVisible( s_iWebID ) )
			{
                const WORD CONST_WEB_WIDTH = 700;
				const WORD CONST_WEB_HEIGHT = 500;

				WORD wWidth = new_min( CONST_WEB_WIDTH, wX_RES );
				WORD wHeight = new_min( CONST_WEB_HEIGHT, wY_RES );

				WORD wLeft = ( wX_RES - wWidth ) / 2;
				WORD wTop = ( wY_RES - wHeight ) / 2;

                SetVisible( s_iWebID, TRUE );
                Navigate( s_iWebID, _T( "http://www.ran-online.co.kr" ), TRUE );
				MoveWindow( s_iWebID, (INT)wLeft, (INT)wTop, wWidth, wHeight, TRUE );
			}
			else
			{
				SetVisible( s_iWebID, FALSE );
				Navigate( s_iWebID, _T( "" ), FALSE );
				MoveWindow( s_iWebID, (INT)wX_RES, (INT)wY_RES, 0, 0, FALSE );
                SetFocusParent();
			}
		}
	}
}