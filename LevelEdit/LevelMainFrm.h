// LevelMainFrm.h : CLevelMainFrame Ŭ������ �������̽�
//
#pragma once

#include "../MfcExLib/ExLibs/ToolBarEx.h"
#include "./Dialogue/DlgLevelEdit.h"
#include "../MfcExLib/ExLibs/CoolDialogBar.h"

class GLGaeaServer;

class CLevelMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CLevelMainFrame();
	DECLARE_DYNCREATE(CLevelMainFrame)

public:
	CString m_strAppPath;
	char m_szAppPath[1024];

// Ư��
public:
	CDlgLevelEdit		m_DlgLevelEdit;
	CCoolDialogBar		m_wndDialogBar1;
	CToolBarWithCombo   m_wndToolBar;

private:
    GLGaeaServer* m_pGaeaServer;

// �۾�
public:
    GLGaeaServer* GetGLGaeaServer() { return m_pGaeaServer; }

// ������
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CLevelMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;

// �޽��� �� �Լ��� �����߽��ϴ�.
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


