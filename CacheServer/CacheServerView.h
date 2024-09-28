// CacheServerView.h : interface of the CacheServerView class
//


#pragma once


class CacheServerView : public CView
{
protected: // create from serialization only
	CacheServerView();
	DECLARE_DYNCREATE(CacheServerView)

// Attributes
public:
	CacheServerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CacheServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CacheServerView.cpp
inline CacheServerDoc* CacheServerView::GetDocument() const
   { return reinterpret_cast<CacheServerDoc*>(m_pDocument); }
#endif

