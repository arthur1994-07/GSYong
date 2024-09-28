#include "StdAfx.h"
#include "./MinWebbrowser.h"

#include <exdispid.h>
#include <afxctl.h>
#include <mshtmlc.h>
#include <comutil.h>

#include "../../SigmaCore/DebugInclude.h"

namespace
{
	const INT HEAD_COUNT = 4;
	const CString HEAD_STRING = _T( "http" );
	const CString BLANK_PAGE = _T( "about:blank" );

	CString VariantToString( VARIANT * va )
	{
		CString strReturn;
		switch( va->vt )
		{
		case VT_BSTR:
			return CString( va->bstrVal );
		case VT_BSTR | VT_BYREF:
			return CString( *va->pbstrVal );
		case VT_I4:
			strReturn.Format( _T( "%d" ), va->lVal );
			return strReturn;
		case VT_I4 | VT_BYREF:
			strReturn.Format( _T( "%d" ), *va->plVal );
			return strReturn;
		case VT_R8:
			strReturn.Format( _T( "%f" ), va->dblVal );
			return strReturn;
		default:
			return CString( "" );
		}
	}
}

BEGIN_INTERFACE_MAP( MinWebbrowser, CWebBrowser2 )
    INTERFACE_PART( MinWebbrowser, DIID_DWebBrowserEvents2, Dispatch )
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP( MinWebbrowser, CWebBrowser2 )
    DISP_FUNCTION_ID(
        MinWebbrowser,
        "BeforeNavigate2",
        DISPID_BEFORENAVIGATE2,
        BeforeNavigate2,
        VT_EMPTY,
        VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL )
    DISP_FUNCTION_ID(
        MinWebbrowser,
        "NewWindow2",
        DISPID_NEWWINDOW2,
        NewWindow2,
        VT_EMPTY,
        VTS_PDISPATCH VTS_PBOOL )
    DISP_FUNCTION_ID(
        MinWebbrowser,
        "DocumentComplete",
        DISPID_DOCUMENTCOMPLETE,
        DocumentComplete,
        VT_EMPTY,
        VTS_DISPATCH VTS_PVARIANT )
END_DISPATCH_MAP()

MinWebbrowser::MinWebbrowser()
    : m_dwCookie( 0 )
    , m_bCompleteLoad( FALSE )
    , m_nIdentity( INT_MIN )
{
    memset( &m_rtPrint, 0, sizeof( m_rtPrint ) );

    EnableAutomation();
}

MinWebbrowser::~MinWebbrowser()
{
}

IWebBrowser2* MinWebbrowser::GetIWebBrowser2()
{
    LPUNKNOWN pUnknown = CWnd::GetControlUnknown();
    if( !pUnknown )
        return NULL;

    IWebBrowser2* pIWebBrowser2 = NULL;
    HRESULT hr = pUnknown->QueryInterface( IID_IWebBrowser2, (VOID**)&pIWebBrowser2 );
    if( FAILED( hr ) )
        return NULL;

    return pIWebBrowser2;
}

BOOL MinWebbrowser::Create( TSTRING& strAppRoot, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
    m_strAppRoot = strAppRoot;

    BOOL bReturn = CWebBrowser2::Create( NULL, dwStyle, rect, pParentWnd, nID );
    if( !bReturn )
        return FALSE;

    CComPtr< IWebBrowser2 > pIWebBrowser2 = GetIWebBrowser2();
    if( !pIWebBrowser2 )
        return FALSE;

    bReturn = ::AfxConnectionAdvise(
        pIWebBrowser2,
        DIID_DWebBrowserEvents2,
        GetInterface( &IID_IUnknown ),
        TRUE,
        &m_dwCookie );

    CWebBrowser2::Navigate( BLANK_PAGE.GetString(), NULL, NULL, NULL, NULL );

    return bReturn;
}

VOID MinWebbrowser::Destroy()
{
    if( m_dwCookie == 0 )
        return;

    CComPtr< IWebBrowser2 > pIWebBrowser2 = GetIWebBrowser2();
    if( !pIWebBrowser2 )
        return;

    ::AfxConnectionUnadvise(
        pIWebBrowser2,
        DIID_DWebBrowserEvents2,
        GetInterface( &IID_IUnknown ),
        FALSE,
        m_dwCookie );

     m_dwCookie = 0;
}

VOID MinWebbrowser::BeforeNavigate2( LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel )
{
    SetCompleteLoad( FALSE );
}

VOID MinWebbrowser::NewWindow2( LPDISPATCH* ppDisp, BOOL* pCancel )
{
    //m_cPopupWindow.put_RegisterAsBrowser( TRUE );
    //*ppDisp = m_cPopupWindow.get_Application();
    //m_cPopupWindow.ShowWindow( SW_SHOW );
}

VOID MinWebbrowser::DocumentComplete( LPDISPATCH pDisp, VARIANT* pURL )
{
	CString strURL = VariantToString( pURL );
	if ( strURL == BLANK_PAGE )
	{
		return;
	}

	HRESULT   hr;
	LPUNKNOWN lpUnknown;
	LPUNKNOWN lpUnknownWB = NULL;
	LPUNKNOWN lpUnknownDC = NULL;

	lpUnknown = CWnd::GetControlUnknown();
	ASSERT( lpUnknown );

	if ( lpUnknown )
	{
		// Get the pointer to the IUnknown interface of the WebBrowser
		// control being hosted. The pointer to the IUnknown returned from
		// GetControlUnknown is not the pointer to the IUnknown of the
		// WebBrowser control. It's actually a pointer to the IOleObject.
		//
		hr = lpUnknown->QueryInterface( IID_IUnknown, ( LPVOID* )&lpUnknownWB );
		ASSERT( SUCCEEDED( hr ) );

		if ( FAILED( hr ) )
			return;

		// Get the pointer to the IUnknown of the object that fired this
		// event.
		//
		hr = pDisp->QueryInterface( IID_IUnknown, ( LPVOID* )&lpUnknownDC );

		ASSERT( SUCCEEDED( hr ) );

		if ( SUCCEEDED( hr ) && lpUnknownWB == lpUnknownDC )
		{
			// The document has finished loading.
			SetCompleteLoad( TRUE );
		}

		if ( lpUnknownWB )
			lpUnknownWB->Release();

		if ( lpUnknownDC )
			lpUnknownDC->Release();
	}
}

BOOL MinWebbrowser::IsEditFocus()
{
    //HWND hwndWeb = CWnd::GetSafeHwnd();
	//HIMC hImc = ::ImmGetContext( hwndWeb );
	//if( !hImc )
	//	return FALSE;

	//DWORD dwConversion = 0, dwSentence = 0;
	//BOOL bOk = ::ImmGetConversionStatus( hImc, &dwConversion, &dwSentence );
	//::ImmReleaseContext( hwndWeb, hImc );

    return TRUE;
}

BOOL MinWebbrowser::GetDocument( TSTRING& strDocument )
{
    CComPtr< IDispatch > pDispatch = CWebBrowser2::get_Document();
	if( !pDispatch )
		return FALSE;

	CComQIPtr< IHTMLDocument2, &IID_IHTMLDocument2 > pIHtml = pDispatch;
	CComPtr< IHTMLElement > pIBody;
	pIHtml->get_body( &pIBody );
	if( !pIBody )
		return FALSE;

	BSTR pTxt;
	pIBody->get_innerHTML( &pTxt );
	if( !pTxt )
		return FALSE;

    size_t nLength = wcslen( pTxt );
    LPSTR pStr = new CHAR[ nLength + 1 ];
    wcstombs( pStr, pTxt, nLength );
    strDocument = pStr;
    SAFE_DELETE( pStr );

	return TRUE;
}

VOID* MinWebbrowser::GetWebBrowser()
{
	return GetIWebBrowser2();
}

BOOL MinWebbrowser::GetLocationUrl( TSTRING& strUrl )
{
    strUrl = CWebBrowser2::get_LocationURL().GetString();

    return TRUE;
}

VOID MinWebbrowser::TranslateAccelerator( MSG& msg )
{
    CComPtr< IDispatch > pDispatch = CWebBrowser2::get_Document();
	if( !pDispatch )
		return;

    CComPtr< IOleInPlaceActiveObject > pIOIPAO = NULL;
    pDispatch->QueryInterface( IID_IOleInPlaceActiveObject, (void**)&pIOIPAO );
    if( !pIOIPAO )
        return;

    pIOIPAO->TranslateAccelerator( &msg );
}

VOID MinWebbrowser::CheckIntegrity( CString& strUrl )
{
    if( strUrl.IsEmpty() )
	{
		strUrl = BLANK_PAGE;
		return;
	}

	CString strHead = strUrl.Left( HEAD_COUNT );
	if( strHead.CompareNoCase( HEAD_STRING.GetString() ) == 0 )
		return;

	CString strFullPath = m_strAppRoot.c_str();
	//strFullPath.Append( _T( "\\" ) );
	strFullPath.Append( strUrl );

	strUrl = strFullPath;
}

VOID MinWebbrowser::Navigate( CString strUrl, BOOL bShow, const TCHAR* pszPOST /*= NULL*/ )
{
    CheckIntegrity( strUrl );

	if ( pszPOST )
	{
		CString strHeader = _T( "Content-Type: application/x-www-form-urlencoded\r\n" );
		CString strPostData = pszPOST;

		// HTTP 헤더 
		VARIANT vHeader;
		vHeader.vt = VT_BSTR;
		vHeader.bstrVal = strHeader.AllocSysString();

		// POST 데이터 변환
		SAFEARRAY FAR *sfPost = NULL;
		SAFEARRAYBOUND bound;
		bound.cElements = strPostData.GetLength();
		bound.lLbound = 0;
		sfPost = SafeArrayCreate( VT_UI1, 1, &bound );
		for ( long lIndex = 0; lIndex < ( signed )bound.cElements; lIndex++ )
		{
			SafeArrayPutElement( sfPost, &lIndex, strPostData.GetBuffer( bound.cElements ) + lIndex );
		}
		strPostData.ReleaseBuffer();

		VARIANT vaPost;
		vaPost.vt = VT_ARRAY;
		vaPost.parray = sfPost;

		CWebBrowser2::Navigate( strUrl.GetString(), NULL, NULL, &vaPost, &vHeader );	

		SysFreeString( vHeader.bstrVal );
		SafeArrayDestroy( sfPost );

		/*
		VARIANT vPOST = {0};
		HRESULT hr;
		hr = GetPostData( &vPOST, pszPOST );
		if ( NOERROR == hr )
		{
			CWebBrowser2::Navigate( strUrl.GetString(), NULL, NULL, &vPOST, NULL );	
		}
		else
		{
			CWebBrowser2::Navigate( strUrl.GetString(), NULL, NULL, NULL, NULL );
		}
		*/
	}
	else
	{
		CWebBrowser2::Navigate( strUrl.GetString(), NULL, NULL, NULL, NULL );
	}

    if( bShow )
        CWnd::ShowWindow( SW_SHOW );
}

VOID MinWebbrowser::MoveWindow( INT nX, INT nY, INT nWidth, INT nHeight, BOOL bRepaint, FLOAT fPrint )
{
    m_rtPrint.left = nX;
    m_rtPrint.top = nY;
    m_rtPrint.right = nWidth;
    m_rtPrint.bottom = nHeight;

    CWnd::MoveWindow( nX, nY, nWidth, nHeight, bRepaint );

    if( !bRepaint )
        return;

    m_vPrint[ 0 ].vPos = D3DXVECTOR4( FLOAT( nX ), FLOAT( nY ), 1.0f, 1.0f );
	m_vPrint[ 1 ].vPos = D3DXVECTOR4( FLOAT( nX + nWidth ), FLOAT( nY ), 1.0f, 1.0f );
	m_vPrint[ 2 ].vPos = D3DXVECTOR4( FLOAT( nX ), nY + FLOAT( nHeight ), 1.0f, 1.0f );
	m_vPrint[ 3 ].vPos = D3DXVECTOR4( FLOAT( nX + nWidth ), FLOAT( nY + nHeight ), 1.0f, 1.0f );

	for( INT i=0; i<4; ++i )
	{
		m_vPrint[ i ].vPos.x -= 0.5f;
		m_vPrint[ i ].vPos.y -= 0.5f;
	}

	FLOAT fU = FLOAT( nWidth ) / fPrint;
	FLOAT fV = FLOAT( nHeight ) / fPrint;

	m_vPrint[ 0 ].vTex = D3DXVECTOR2( 0.0f, 0.0f );
	m_vPrint[ 1 ].vTex = D3DXVECTOR2( fU, 0.0f );
	m_vPrint[ 2 ].vTex = D3DXVECTOR2( 0.0f, fV );
	m_vPrint[ 3 ].vTex = D3DXVECTOR2( fU, fV );
}

VOID MinWebbrowser::Refresh()
{
    CWebBrowser2::Refresh();
}

BOOL MinWebbrowser::StretchBlt( HDC hdcTarget )
{
    HWND hwndWeb = CWnd::GetSafeHwnd();
    HDC hdcWeb = ::GetDC( hwndWeb );

    BOOL bReturn = ::StretchBlt(
	    hdcTarget,
	    0,
	    0,
	    m_rtPrint.right,
	    m_rtPrint.bottom,
	    hdcWeb,
	    0,
	    0,
	    m_rtPrint.right,
	    m_rtPrint.bottom,
	    SRCCOPY );

    ::ReleaseDC( hwndWeb, hdcWeb );

    return bReturn;
}

// Pack some data into a SAFEARRAY of BYTEs. Return in a VARIANT
HRESULT MinWebbrowser::GetPostData( LPVARIANT pvPostData, const TCHAR* pszPOST )
{
	HRESULT hr;
	LPSAFEARRAY psa;
	LPCTSTR cszPostData = pszPOST;
	UINT cElems = lstrlen( cszPostData );
	LPSTR pPostData;

	if ( !pvPostData )
	{
		return E_POINTER;
	}

	VariantInit( pvPostData );

	psa = SafeArrayCreateVector( VT_UI1, 0, cElems );
	if ( !psa )
	{
		return E_OUTOFMEMORY;
	}

	hr = SafeArrayAccessData( psa, ( LPVOID* )&pPostData );
	memcpy( pPostData, cszPostData, cElems );
	hr = SafeArrayUnaccessData( psa );

	V_VT( pvPostData ) = VT_ARRAY | VT_UI1;
	V_ARRAY( pvPostData ) = psa;
	return NOERROR;
}
