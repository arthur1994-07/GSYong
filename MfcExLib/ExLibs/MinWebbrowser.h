/**
 * \date	2010/10/04
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <afxwin.h>
#include "../EngineLib/DxTools/DxVertexFVF.h"
#include "./Webbrowser2.h"

class MinWebbrowser : public CWebBrowser2
{
    DECLARE_INTERFACE_MAP()
    DECLARE_DISPATCH_MAP()

protected:
    TSTRING m_strAppRoot;
    DWORD m_dwCookie;
    BOOL m_bCompleteLoad;
    INT m_nIdentity;
    RECT m_rtPrint;
    VERTEXRHW m_vPrint[ 4 ];

public:
    MinWebbrowser();
    ~MinWebbrowser();

protected:
    IWebBrowser2* GetIWebBrowser2();

public:
    BOOL Create( TSTRING& strAppRoot, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
    VOID Destroy();

    VOID BeforeNavigate2( LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel );
    VOID NewWindow2( LPDISPATCH* ppDisp, BOOL* pCancel );
    VOID DocumentComplete( LPDISPATCH pDisp, VARIANT* pURL );

public:
    BOOL IsEditFocus();
    BOOL GetDocument( TSTRING& strDocument );
	VOID* GetWebBrowser();
    BOOL GetLocationUrl( TSTRING& strUrl );

    VOID TranslateAccelerator( MSG& msg );
    VOID CheckIntegrity( CString& strUrl );
    VOID Navigate( CString strUrl, BOOL bShow, const TCHAR* pszPOST = NULL );
    VOID MoveWindow( INT nX, INT nY, INT nWidth, INT nHeight, BOOL bRepaint, FLOAT fPrint );
    VOID Refresh();
    BOOL StretchBlt( HDC hdcTarget );

public:
    VOID SetCompleteLoad( BOOL bCompleteLoad ) { m_bCompleteLoad = bCompleteLoad; }
    VOID SetIdentity( INT nIdentity ) { m_nIdentity = nIdentity; }

public:
    BOOL IsCompleteLoad() { return m_bCompleteLoad; }
    INT GetIdentity() { return m_nIdentity; }
    RECT* GetPrintRect() { return &m_rtPrint; }
    VERTEXRHW* GetPrintVertex() { return m_vPrint; }
	HRESULT GetPostData( LPVARIANT pvPostData, const TCHAR* pszPOST );

};