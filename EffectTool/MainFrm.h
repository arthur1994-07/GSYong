// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A1C4D012_9880_4526_ACF0_C22A795B20A7__INCLUDED_)
#define AFX_MAINFRM_H__A1C4D012_9880_4526_ACF0_C22A795B20A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../MfcExLib/ExLibs/ToolBarEx.h"
#include "../MfcExLib/ExLibs/CoolDialogBar.h"
#include "./Dialog/EffEditBar.h"
#include "./Dialog/EffLiftTimeBar.h"
#include "../Enginelib/ServiceProviderDefine.h"

class CsheetWithTab;
class CEffGroupPage;
class EngineDeviceMan;
class SUBPATH;

class EngineSingletone
{
private:
    EngineSingletone();
    ~EngineSingletone();
    
    static EngineSingletone* m_pEngineSingletone;
    EngineDeviceMan* m_pEngineDevice;

public:
    void CreateDevice(EMSERVICE_PROVIDER ServiceProvier, SUBPATH* pPath);
    static EngineSingletone* GetInstance();
    static void ReleaseInstance();
    EngineDeviceMan* EngineDevice();
};

class DlgLog;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CString				m_strAppPath;
	char				m_szAppPath[1024];

	CEffEditBar			m_DlgEditBar;
	CEffLiftTimeBar		m_DlgLiftTimeBar;
	CCoolDialogBar		m_wndDialogBar1;
	CCoolDialogBar		m_wndDialogBar2;
	CToolBarWithCombo   m_wndToolBar;
    DlgLog* m_pLogDlg;

public:
	CEffGroupPage* GetEffGroupPage();
	CStProg* GetTimeLine() { return &m_DlgLiftTimeBar.m_stEffTimeLine; }
	BOOL ListingTimeLine(EFF_PROPGROUP* pEffPropGroup) { return m_DlgLiftTimeBar.ListingTimeLine(pEffPropGroup); }



// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
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
	CStatusBar  m_wndStatusBar;    

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewCurnone();
	afx_msg void OnViewMove();
	afx_msg void OnViewRotate();
	afx_msg void OnViewScale();
	afx_msg void OnViewChar();
	afx_msg void OnViewMap();
	afx_msg void OnViewEditbox();		//ID_VIEW_EDITBOX
	afx_msg void OnViewParticleRange();	//ID_VIEW_PARTICLERANGE
	afx_msg void OnLoadBgtex();
	afx_msg void OnViewBgtex();
	afx_msg void OnUpdateViewBgtex(CCmdUI *pCmdUI);
    afx_msg void OnClose();
    afx_msg void OnMenuLogWindow();
	afx_msg void OnAllFileLoading();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A1C4D012_9880_4526_ACF0_C22A795B20A7__INCLUDED_)
