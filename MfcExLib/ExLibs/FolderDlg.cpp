#include "StdAfx.h"
#include "./FolderDlg.h"

CString SelectFolder ( HWND hWnd, TCHAR* szInitpath )
{
	ITEMIDLIST* pDLBrowser;
	BROWSEINFO	browseInfo;
	
	memset(&browseInfo, 0, sizeof(BROWSEINFO));
	browseInfo.hwndOwner  = hWnd;
	browseInfo.pidlRoot   = NULL;
	browseInfo.pszDisplayName = "Folder Dialog";
	browseInfo.lpszTitle  = "Select for Folder";
	browseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn    = NULL;
	pDLBrowser = ::SHBrowseForFolder( &browseInfo );

	CString strFolderPath;
	if ( pDLBrowser != NULL )
	{
		TCHAR szFolderPath[MAX_PATH] = {0};
		SHGetPathFromIDList ( pDLBrowser, szFolderPath );
		strFolderPath = szFolderPath;
	}
	return strFolderPath;
}

CFolderDlg::CFolderDlg(LPCTSTR szTitle,
					   LPCTSTR szSelPath,
					   CWnd* pHWndParent,
					   UINT	uFlags)
					   : CCommonDialog(pHWndParent)
					   , m_hWnd(NULL)
{
	::ZeroMemory ( &m_bi, sizeof( BROWSEINFO ) );
	::lstrcpy(m_szFolPath, szSelPath);
	::lstrcpy(m_szSelPath, szSelPath);

	m_bi.hwndOwner	= pHWndParent->GetSafeHwnd();
	m_bi.pidlRoot	= NULL;
	m_bi.lpszTitle	= szTitle;
	m_bi.ulFlags	= uFlags;
	m_bi.lpfn		= (BFFCALLBACK)&BrowseCallbackProc;
	m_bi.lParam		= (LPARAM)this;

	m_bi.pszDisplayName = new TCHAR[ MAX_PATH ];
	ZeroMemory ( m_bi.pszDisplayName, (MAX_PATH * sizeof(TCHAR)) );

	if( szSelPath )
		SetSelectedFolder( szSelPath );

	ASSERT_VALID( this );
	//return ;
	//////////////////////////////////////////////////////////////////////////
	if( !szSelPath )
	{
		if ( m_bi.pidlRoot )
		{
			CoTaskMemFree((void*)m_bi.pidlRoot);
			m_bi.pidlRoot = NULL;
		}
		return ;
	}

	ASSERT( AfxIsValidString( szSelPath, MAX_PATH ) );

	HRESULT		  hResult	  = S_FALSE;
	IShellFolder* pDeskFolder = NULL;

	hResult = ::SHGetDesktopFolder( &pDeskFolder );
	if ( hResult == S_OK )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPTSTR       pszRoot  = const_cast< LPTSTR >( szSelPath );

		USES_CONVERSION;

		hResult = pDeskFolder->ParseDisplayName(
			NULL, NULL, T2W( pszRoot ), NULL, &pidlRoot, NULL );

		if( hResult == S_OK )
		{
			if ( m_bi.pidlRoot )
			{
				CoTaskMemFree((void*)m_bi.pidlRoot);
				m_bi.pidlRoot = NULL;
			}
			m_bi.pidlRoot = pidlRoot;
		}

		SAFE_RELEASE( pDeskFolder );		
	}
}

CFolderDlg::~CFolderDlg()
{
	if ( m_bi.pidlRoot )
	{
		CoTaskMemFree((void*)m_bi.pidlRoot);
		m_bi.pidlRoot = NULL;
	}
	if (m_bi.pszDisplayName)
		SAFE_DELETE_ARRAY(m_bi.pszDisplayName);

	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
}

INT CFolderDlg::DoModal(void)
{
	ASSERT_VALID( this );	
	ASSERT( m_bi.lpfn != NULL );

	INT_PTR nRet = -1;
	m_bi.hwndOwner = PreModal();

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder( &m_bi );
	if( pItemIDList )
	{
		if( ::SHGetPathFromIDList( pItemIDList, m_szSelPath/*m_szFolPath*/ ) )
			nRet = IDOK;

		if ( pItemIDList )
		{
			CoTaskMemFree((void*)pItemIDList);
			pItemIDList = NULL;
		}
	}
	else
		nRet = IDCANCEL;

	PostModal();
	return ( nRet );
}

INT CALLBACK CFolderDlg::BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CFolderDlg* pThis = (CFolderDlg*)lpData;
	ASSERT( pThis != NULL );

	INT nRet = 0;
	pThis->m_hWnd = hWnd;

	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		break;
	case BFFM_VALIDATEFAILED:
		break;
	case BFFM_IUNKNOWN:
		break;
	default:
		ASSERT( FALSE );
		break;
	}
	pThis->m_hWnd = NULL;
	return nRet;
}

void CFolderDlg::OnInitialized()
{
	if( ::lstrlen( m_szSelPath ) > 0 )
		SetSelection( m_szSelPath );
}

void CFolderDlg::SetSelection( LPITEMIDLIST pItemIDList )
{
	ASSERT( m_hWnd != NULL );
	::SendMessage( 
		m_hWnd, BFFM_SETSELECTION, 
		(WPARAM)FALSE, (LPARAM)pItemIDList
		);
}

void CFolderDlg::SetSelection( LPCTSTR pszPath )
{
	ASSERT( m_hWnd != NULL );
	ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );
	::SendMessage( 
		m_hWnd, BFFM_SETSELECTION, 
		(WPARAM)TRUE, (LPARAM)pszPath);
}

BOOL CFolderDlg::SetSelectedFolder( LPCTSTR pszPath )
{
	ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );
	return (BOOL)::lstrcpy( m_szSelPath, pszPath );
}