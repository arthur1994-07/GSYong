// MainFrm.h : CMainFrame 클래스의 인터페이스
//
#pragma once

#include "../MfcExLib/ExLibs/CoolDialogBar.h"
#include "./DlgUIEdit.h"
#include "./DlgTextureInfoEdit.h"
#include "./DialogMainView.h"

class EngineDeviceMan;
class CInterfaceAdmin;
class SUBPATH;

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
public:
	CDlgTextureInfoEdit m_cDlgEditText;
	CDlgMainView		m_cDlgMainView;
	CDlgUIEdit			m_DlgUIEdit;
	CCoolDialogBar		m_wndDialogBar;

protected:
    EngineDeviceMan* m_pEngineDevice;
    CInterfaceAdmin* m_pInterfaceAdmin;
    SUBPATH* m_pPath;

// 작업
public:
    EngineDeviceMan* GetEngineDevice() const { return m_pEngineDevice; }
    CInterfaceAdmin* GetInterfaceAdmin() const { return m_pInterfaceAdmin; }

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

