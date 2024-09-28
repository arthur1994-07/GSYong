#pragma once

#include "./ChildView.h"
#include "./EditListPage.h"
#include "./ControlProperties.h"

class CFrame : public CXTPFrameWnd
{
protected:
	DECLARE_DYNAMIC( CFrame )
	DECLARE_MESSAGE_MAP()

protected:
	CXTPStatusBar m_wndStatusBar;
	CXTPDockingPaneManager m_paneManager;
	CChildView m_wndView;
	CEditListPage* m_wndEditList;
	CXTPOfficeBorder< CControlProperties > m_wndProperties;

public:
	CFrame();
	virtual ~CFrame();

public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	virtual BOOL OnCmdMsg( UINT nID, INT nCode, VOID* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

#ifdef _DEBUG
	virtual VOID AssertValid() const;
	virtual VOID Dump( CDumpContext& dc ) const;
#endif

protected:
	afx_msg INT OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg VOID OnClose();
    afx_msg VOID OnSize( UINT nType, INT cx, INT cy );
	afx_msg VOID OnSetFocus( CWnd *pOldWnd );
    afx_msg VOID OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
	afx_msg VOID OnCustomize();
	afx_msg LRESULT OnDockingPaneNotify( WPARAM wParam, LPARAM lParam );
    afx_msg VOID OnShowPane( UINT nID );
    afx_msg VOID OnUpdateShowPane( CCmdUI* pCmdUI );

public:
    BOOL OnPaneNotifyShowWindow( LPARAM lParam );
    BOOL OnPaneNotifyClosePane( LPARAM lParam );

public:
    VOID OnCreateFrame();
    VOID OnDestroyFrame();
    BOOL OnLoop();

public:
    void FindItem( const std::string strItem );

	VOID ChangeActiveView( HWND hWnd );

	VOID RefreshAll();
	VOID RefreshEditListPage();
	VOID RefreshControlProperties();
};