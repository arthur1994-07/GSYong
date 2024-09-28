// RanFieldServerMainFrm.h : interface of the RanFieldServerMainFrame class
//


#pragma once

#include "resource.h"
#include "LeftView.h"
#include "RightBottomView.h"
#include "RightTopView.h"

class CFieldServer;

#ifdef __NEVER_DEFINED__
#define BTWindow<CFrameWnd> CFrameWnd
#endif

#ifdef _DEBUG
class RanFieldServerMainFrame : public CFrameWnd
#else
class RanFieldServerMainFrame : public BTWindow<CFrameWnd>
#endif
{
	
protected: // create from serialization only
	RanFieldServerMainFrame();
	DECLARE_DYNCREATE(RanFieldServerMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
    CSplitterWnd m_wndSplitterLeft;
    CSplitterWnd m_wndSplitterRight;
    LeftView* m_pLeft;
    RightTopView* m_pRightTop;
    RightBottomView* m_pRightBottom;
    CFieldServer* m_pServer;

public:

// Operations
public:
    bool StartServer();

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~RanFieldServerMainFrame();
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
};


