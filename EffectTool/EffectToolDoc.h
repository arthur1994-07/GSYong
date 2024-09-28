// EffectToolDoc.h : interface of the CEffectToolDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTTOOLDOC_H__87284713_3450_4F4F_A033_49FF693F08C4__INCLUDED_)
#define AFX_EFFECTTOOLDOC_H__87284713_3450_4F4F_A033_49FF693F08C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEffectToolDoc : public CDocument
{
protected: // create from serialization only
	CEffectToolDoc();
	DECLARE_DYNCREATE(CEffectToolDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	// Version 을 추가한다.
	void SetTitleAddVersion( LPCTSTR lpszTitle );

	virtual ~CEffectToolDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEffectToolDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOLDOC_H__87284713_3450_4F4F_A033_49FF693F08C4__INCLUDED_)
