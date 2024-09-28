// RanOnlineEdView.h : interface of the RanOnlineEdView class
//


#pragma once

#ifdef __NEVER_DEFINED__
#define BTWindow<CView> CView
#endif

#ifdef _DEBUG
class RanOnlineEdView : public CView
#else
class RanOnlineEdView : public BTWindow<CView>
#endif
{
protected: // create from serialization only
	RanOnlineEdView();
	DECLARE_DYNCREATE(RanOnlineEdView)

// Attributes
public:
	RanOnlineEdDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~RanOnlineEdView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in RanOnlineEdView.cpp
inline RanOnlineEdDoc* RanOnlineEdView::GetDocument() const
   { return reinterpret_cast<RanOnlineEdDoc*>(m_pDocument); }
#endif

