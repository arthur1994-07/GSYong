// RanOnlineEdMainFrm.h : interface of the RanOnlineEdFrame class
//


#pragma once

class LeftView;
class RightView;
class EngineDeviceMan;

class RanOnlineEdFrame : public CFrameWnd
{
	
protected: // create from serialization only
	RanOnlineEdFrame();
	DECLARE_DYNCREATE(RanOnlineEdFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
    LeftView* m_pLeft;
    RightView* m_pRight;
    EngineDeviceMan* m_pEngineDevice;

public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~RanOnlineEdFrame();
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
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void PostNcDestroy();
};


