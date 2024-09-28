// MainFrm.h : interface of the CMainFrame class
//
#pragma once

#include <vector>
#include "./Util/VisualFx.h"
#include "./Util/CToolBarCombo.h"
#include "./Database/DatabaseTable.h"
#include "./Database/DbBase.h"

namespace gmce
{
	class IDBManager;	
}

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	BOOL m_bCfgLoad; // Cfg Load 상태
	BOOL m_bDBConnect; // DB Connection 상태
	BOOL m_bLogin; // Login 상태
	int  m_nUserNum; // GM 의 사용자번호	
	int  m_nSvrGrp; // 선택된 서버그룹
	int  m_nUserLevel; // GM 레벨
	int  m_nSelectedServer; // 선택된 서버
	
	std::vector<viewServerInfo> m_vServerList;	//ODBC용

	TVisualFramework m_Framework;

	CString m_strAppPath;
	char	m_szAppPath[MAX_PATH];

	CString m_csCaption;

	gmce::IDBManager	*m_pDbMan;
	
#ifdef TIXML_USE_TICPP
	gmce::A_GROUP		m_DbInfo;
#else
	gmce::DB_INFO		m_DbInfo;
#endif


// Operations
public:
	void FreeMemory();

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
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBarCombo  m_comboBox;	

// Generated message map functions
protected:
	afx_msg LRESULT OnSelchangeDockCombo(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnDestroy();
	afx_msg void OnClose();

	BOOL DoLoginADO();
	BOOL DoLoginODBC();	
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual void PostNcDestroy();
};
