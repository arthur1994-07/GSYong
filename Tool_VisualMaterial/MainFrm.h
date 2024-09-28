// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class DxVMCommand;
class EngineDeviceMan;

#include "./Properties/PropertyGridDlg.h"
#include "./Edit/MaterialExpressionsWnd.h"
#include "./Edit/PreviewWnd.h"

class CMainFrame : public CXTPFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
    CString m_strAppPath;
	TCHAR	m_szAppPath[1024];

protected:
    EngineDeviceMan* m_pEngineDevice;

public:
    EngineDeviceMan* GetEngineDevice() const { return m_pEngineDevice; }

public:
    static TSTRING g_strVertexInstructionsInfo;
    static TSTRING g_strPixelInstructionsInfo;

// 
public:
    void SelectCommand( DxVMCommand* pVMCommand );
    void SetSource( const std::string& strFX, const std::string& strSimpleFX );

    HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice );
	void	OnLostDevice();
    void	OnFrameMove( float fElapsedTime );
    HRESULT PreviewRender();

private:
    void GetInstructionInfo( const TSTRING& strCommand, TSTRING& strInstructionInfo );

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar               m_wndStatusBar;
	CXTPDockingPaneManager      m_paneManager;
    CPropertyGridDlg            m_sPropertyGridDlg;
    CXTPOfficeBorder< CEdit >   m_wndSource;
    CPreviewWnd                 m_wndPreview;
    CMaterialExpressionsWnd     m_wndMaterialExpressions;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnCustomize();
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
    afx_msg void OnShowPane( UINT nID );
    afx_msg void OnUpdateShowPane( CCmdUI* pCmdUI );
    afx_msg void OnHome();
    afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
    afx_msg void OnUpdateLineCurve(CCmdUI* pCmdUI);
	afx_msg void OnLineCurve();
    afx_msg void OnUpdateGridToggle(CCmdUI* pCmdUI);
	afx_msg void OnGridToggle();
	DECLARE_MESSAGE_MAP()
};
