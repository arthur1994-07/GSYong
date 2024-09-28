// LevelMainFrm.h : CLevelMainFrame 클래스의 인터페이스
//
#pragma once

#include "../MfcExLib/ExLibs/ToolBarEx.h"
#include "./Dialogue/DlgLevelEdit.h"
#include "../MfcExLib/ExLibs/CoolDialogBar.h"

class GLGaeaServer;

class CLevelMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CLevelMainFrame();
	DECLARE_DYNCREATE(CLevelMainFrame)

public:
	CString m_strAppPath;
	char m_szAppPath[1024];

// 특성
public:
	CDlgLevelEdit		m_DlgLevelEdit;
	CCoolDialogBar		m_wndDialogBar1;
	CToolBarWithCombo   m_wndToolBar;

private:
    GLGaeaServer* m_pGaeaServer;

// 작업
public:
    GLGaeaServer* GetGLGaeaServer() { return m_pGaeaServer; }

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현
public:
	virtual ~CLevelMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewCurnone();
	afx_msg void OnViewSelect();
	afx_msg void OnViewRotate();
	afx_msg void OnViewScale();
	afx_msg void OnViewMove();
	afx_msg void OnViewCameraChar();
	afx_msg void OnViewCameraTo();
	afx_msg void OnViewSelectMobsch();
	afx_msg void OnViewAll90();
	afx_msg void OnViewNavigationMesh();
};


