// MainFrm.h : CMainFrame Ŭ������ �������̽�
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
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư��
public:
	CDlgTextureInfoEdit m_cDlgEditText;
	CDlgMainView		m_cDlgMainView;
	CDlgUIEdit			m_DlgUIEdit;
	CCoolDialogBar		m_wndDialogBar;

protected:
    EngineDeviceMan* m_pEngineDevice;
    CInterfaceAdmin* m_pInterfaceAdmin;
    SUBPATH* m_pPath;

// �۾�
public:
    EngineDeviceMan* GetEngineDevice() const { return m_pEngineDevice; }
    CInterfaceAdmin* GetInterfaceAdmin() const { return m_pInterfaceAdmin; }

// ������
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

