#pragma once

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

#include < AfxDlgs.h >
#include < AtlConv.h >	

extern CString SelectFolder ( HWND hWnd, TCHAR* szInitpath = NULL );

class CFolderDlg : public CCommonDialog
{
private:
	HWND		m_hWnd;
	BROWSEINFO	m_bi;

	TCHAR	m_szSelPath[ MAX_PATH ];
	TCHAR	m_szFolPath[ MAX_PATH ];

public:
	CFolderDlg( 
		LPCTSTR szTitle = NULL,
		LPCTSTR szSelPath = NULL,
		CWnd*	pHWndParent = NULL,
		UINT	uFlags = BIF_RETURNONLYFSDIRS);
	virtual ~CFolderDlg();

protected:
	virtual void OnInitialized(void);

	void SetSelection( LPITEMIDLIST pItemIDList );
	void SetSelection( LPCTSTR pszPath );	
	void SetStatusText( LPCTSTR pszText );
	BOOL SetSelectedFolder( LPCTSTR pszPath );
	
public:
	INT DoModal(void);
	LPCTSTR GetSelectFolderPath() const {return m_szSelPath;}

private:
	static INT CALLBACK BrowseCallbackProc(
		HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

private:
	
};