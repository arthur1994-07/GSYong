// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__924CC443_0EA9_41DC_8FA6_DF80D0AB7A9B__INCLUDED_)
#define AFX_MAINFRM_H__924CC443_0EA9_41DC_8FA6_DF80D0AB7A9B__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../MfcExLib/ExLibs/ToolBarEx.h"
#include "DlgWorldEdit.h"
#include "../MfcExLib/ExLibs/CoolDialogBar.h"
#include "./PiecePage/DlgPieceEdit.h"
#include "TrueColorToolBarWithCombo.h"
#include "AssetDlg.h"
#include "SettingIni.h"

class CMainFrame : public CFrameWnd, public CSettingIni
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	CString m_strAppPath;
	char	m_szAppPath[1024];

// Attributes
public:
	CDlgEdit			m_cDlgEdit;
	CCoolDialogBar		m_wndDialogBar1;
	//CToolBarWithCombo   m_wndToolBar;
	CTrueColorToolBarWithCombo	m_wndTrueToolBar;

    CDlgPieceEdit		m_cDlgEditPIECE;	
    CAssetDlg           m_cDlgAsset;
protected:
	void UpdateMenuAct ();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar			m_wndStatusBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewBoxline();
	afx_msg void OnUpdateViewBoxline(CCmdUI* pCmdUI);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnViewPieceCheck();
	afx_msg void OnUpdateViewPieceCheck(CCmdUI* pCmdUI);
	afx_msg void OnViewFrame();
	afx_msg void OnViewOctree();
	afx_msg void OnViewPrefab();
	afx_msg void OnViewNavimesh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewCurMove();
	afx_msg void OnViewCurSelect();
	afx_msg void OnViewRotate();
	afx_msg void OnViewScale();
	afx_msg void OnViewSelectpoint();
	afx_msg void OnViewCurnone();
	afx_msg void OnViewMovetoframe();
	afx_msg void OnViewMovetopoint();
	afx_msg void OnCharMove();	
	afx_msg void OnViewCharmovetopoint();
	afx_msg void OnViewEditbox();
	afx_msg void OnFileNew();
	afx_msg void OnViewPiece();
	afx_msg void OnViewCurTileDraw();
	afx_msg void OnViewCurTileGet();
	afx_msg void OnViewCurTileDel();
	afx_msg void OnViewAll90();
	afx_msg void OnViewFpsVisible();
	afx_msg void OnViewConsoleVisible();
	afx_msg void OnViewEditPanel();
	afx_msg void OnGameTest();
    afx_msg void OnCheckMipmap();
	afx_msg void OnCheckRenderTextureLightmap();
	afx_msg void OnCheckRenderLightmap();
	afx_msg void OnCheckRenderLightmapChecker();
	afx_msg void OnRadiositSettingInfo();
	afx_msg void OnToolBarSpecCam();
	afx_msg void OnToolBarSmallRefresh();
	afx_msg void OnViewCharChange();
protected:
    virtual void PostNcDestroy();
public:
	//afx_msg void OnUpdateCheckRenderTextureLightmap(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateCheckRenderLightmap(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateCheckRenderLightmapChecker(CCmdUI *pCmdUI);
	afx_msg void OnLoadingLightmapCheckerTexture();
	afx_msg void OnCheckRenderLightmapPoint();
    afx_msg void OnUpdateAssetView(CCmdUI *pCmdUI);
    afx_msg void OnAssetView();    
    afx_msg void OnClose();    
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnViewFogOff();
	afx_msg void OnLodOff();
	afx_msg void OnPieceInstancingOff();
	afx_msg void OnCheckWaterRealTimeReflection();
	afx_msg void OnCheckWaterCubeMap();
	afx_msg void OnCheckWaterDiffuseTexture();
	afx_msg void OnChangeTempCubemap();
	afx_msg void OnRenderPS_3_0_HIGH();
	afx_msg void OnRenderPS_3_0();
	afx_msg void OnRenderPS_2_0();
	afx_msg void OnRenderPS_1_1();
	afx_msg void OnRenderPS_FIXED();
	afx_msg void OnRenderPS_Char_SS();
	afx_msg void OnRenderPS_Char_Default();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__924CC443_0EA9_41DC_8FA6_DF80D0AB7A9B__INCLUDED_)
