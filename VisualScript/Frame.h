/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "./SequenceView.h"
#include "./TargetView.h"
#include "./SequenceProperties.h"
#include "./SequenceTree.h"

namespace ic
{
    class CBaseObject;
}

namespace vs
{
    class CSequence;
}

class CEnumPropertyDlg;

class CFrame : public CXTPFrameWnd
{
protected:
	DECLARE_DYNAMIC( CFrame )
    DECLARE_MESSAGE_MAP()

protected:
	CXTPStatusBar m_wndStatusBar;
    CXTPDockingPaneManager m_paneManager;
	CXTPOfficeBorder< CSequenceView > m_wndSequenceView;
    CXTPOfficeBorder< CTargetView > m_wndTargetView;
	CXTPOfficeBorder< CSequenceProperties > m_wndProperties;
    CXTPOfficeBorder< CSequenceTree > m_wndSequenceTree;
    CEnumPropertyDlg* m_pEnumPropertyDlg;

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
    VOID ChangeActiveView( HWND hWnd );
    VOID ChangeActiveSequnce( vs::CSequence* pSequence );
    VOID RefreshPropertiesControl();
    VOID RebuildTreeControl();

    VOID OpenEnumPropertyDlg( ic::CBaseObject* pObject );
    TSTRING GetKeyFromEnumPropertyDlg();
};