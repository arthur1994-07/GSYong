// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__1567670D_BF1D_4091_A2B9_79966A309663__INCLUDED_)
#define AFX_MAINFRM_H__1567670D_BF1D_4091_A2B9_79966A309663__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./Dialog/DlgEdit.h"
#include "./Dialog/DlgTimeLine.h"
#include "../MfcExLib/ExLibs/CoolDialogBar.h"
#include "./Util/TrueColorToolBar.h"
#include "./Util/SettingIni.h"

enum MOVETYPE
{
	MOVE_STAY =0, MOVE_WALK, MOVE_RUN,
};

class DlgLog;
class CAssetDlg;

class CMainFrame : public CFrameWnd, public CSettingIni
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	CString m_strAppPath;
	char m_szAppPath[1024];

	
// Attributes
public:
	CDlgEdit		m_cDlgEdit;
	CDlgTimeLine	m_cDlgTimeLine;
	CCoolDialogBar	m_wndDialogBar1;
	CCoolDialogBar	m_wndDialogBar2;

	float			m_fRunSpeed;
	float			m_fWalkSpeed;

protected:
	void	SetTitleName ( CString Name );

public:
	void ReStoreDialogBar ();
	
// Operations
public:
	void SetTimeLine ( DWORD dwStartKey, DWORD dwEndKey, DWORD dwUnitTime, BYTE aniType = 1  );
	
	// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
	void ChangeTimeOfTimeLine( DWORD dwToTime );

	// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
	void OnButtonPlayOfCDlgTimeLine();

	void SetMoveSpeed( float fRunSpeed, float fWalkSpeed ) { m_fRunSpeed = fRunSpeed; m_fWalkSpeed = fWalkSpeed; }
	float GetRunSpeed() { return m_fRunSpeed; }
	float GetWalkSpeed() { return m_fWalkSpeed; }

    CAssetDlg* GetAssetDlg() { return m_pAssetDlg; }

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
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
    DlgLog* m_pLogDlg;
    CAssetDlg* m_pAssetDlg;

	CTrueColorToolBar m_wndTrueToolBar;

private:
	void UpdateMenuAct();
	void ResetSkinPiece();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnViewPlane();
	afx_msg void OnUpdateViewPlane(CCmdUI* pCmdUI);
	afx_msg void OnViewMovePiece();
	afx_msg void OnUpdateViewMovePiece(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnItemlevelDefault();
	afx_msg void OnItemlevelLevel1();
	afx_msg void OnItemlevelLevel2();
	afx_msg void OnItemlevelLevel3();
	afx_msg void OnItemlevelLevel4();
	afx_msg void OnItemlevelLevel5();
	afx_msg void OnItemlevelLevel6();
	afx_msg void OnItemlevelLevel7();
	afx_msg void OnItemlevelLevel8();
	afx_msg void OnItemlevelLevel9();
	afx_msg void OnItemlevelLevel10();
	afx_msg void OnItemlevelLevel11();
	afx_msg void OnViewAabbboxVisible();
	afx_msg void OnViewEffectSphereVisible();
	afx_msg void OnUpdateViewBgtex(CCmdUI *pCmdUI);
	afx_msg void OnViewBgtex();
	afx_msg void OnLoadBgtex();
	afx_msg void OnSaveSoundCsv();
    afx_msg void OnViewAmbientChange();
    afx_msg void OnClose();
    afx_msg void OnMenuLogWindow();
    afx_msg void OnAssetview();
    afx_msg void OnUpdateAssetview(CCmdUI *pCmdUI);        
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRenderPS_3_0();
	afx_msg void OnRenderPS_2_0();
	afx_msg void OnRenderPS_1_1();
	afx_msg void OnRenderPS_FIXED_HIGH();
	afx_msg void OnRenderPS_FIXED();
	afx_msg void OnFindInverseScale();
	afx_msg void OnFindDontHaveTexture();
	afx_msg void OnMenuLandLoad();
	afx_msg void OnMenuLandErase();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__1567670D_BF1D_4091_A2B9_79966A309663__INCLUDED_)
