// GMCharEditView.h : interface of the CGMCharEditView class
//


#pragma once


class CGMCharEditView : public CFormView
{
protected: // create from serialization only
	CGMCharEditView();
	DECLARE_DYNCREATE(CGMCharEditView)

public:
	enum{ IDD = IDD_GMCHAREDIT_FORM };

// Attributes
public:
	CString m_strAppPath;
	char	m_szAppPath[MAX_PATH];
	CGMCharEditDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CGMCharEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GMCharEditView.cpp
inline CGMCharEditDoc* CGMCharEditView::GetDocument() const
   { return reinterpret_cast<CGMCharEditDoc*>(m_pDocument); }
#endif

