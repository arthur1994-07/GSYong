// CacheServerMainFrm.h : interface of the CacheServerMainFrame class
//


#pragma once

class LeftView;
class RightTopView;
class RightBottomView;
class CacheServer;

#ifdef __NEVER_DEFINED__
#define BTWindow<CFrameWnd> CFrameWnd
#endif

#ifdef _DEBUG
class CacheServerMainFrame : public CFrameWnd
#else
class CacheServerMainFrame : public BTWindow<CFrameWnd>
#endif
{
	
protected: // create from serialization only
	CacheServerMainFrame();
	DECLARE_DYNCREATE(CacheServerMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
    CSplitterWnd m_wndSplitterRight;

    LeftView* m_pLeftView;
    RightTopView* m_pRightTopView;
    RightBottomView* m_pRightBottomView;
    CacheServer* m_pServer;

public:
    bool StartServer();
    void ClearLogMessage();
    void LogItemExchange();
    bool IsServerRunning();

	//! Test code
	void AdoTest1();
    void AdoTest2();

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT OnUpdateLogConsole(WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CacheServerMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
protected:
	virtual void PostNcDestroy();
};


