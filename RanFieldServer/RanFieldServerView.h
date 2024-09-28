// RanFieldServerView.h : interface of the RanFieldServerView class
//


#pragma once


#ifdef __NEVER_DEFINED__
#define BTWindow<CView> CView
#endif

#ifdef _DEBUG
class RanFieldServerView : public CView
#else
class RanFieldServerView : public BTWindow<CView>
#endif
{
protected: // create from serialization only
	RanFieldServerView();
	DECLARE_DYNCREATE(RanFieldServerView)

// Attributes
public:
	RanFieldServerDoc* GetDocument() const;

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
	virtual ~RanFieldServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RanFieldServerView.cpp
inline RanFieldServerDoc* RanFieldServerView::GetDocument() const
   { return reinterpret_cast<RanFieldServerDoc*>(m_pDocument); }
#endif

