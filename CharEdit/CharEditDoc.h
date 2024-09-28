// CharEditDoc.h : interface of the CCharEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAREDITDOC_H__14EB9155_A6E8_4993_89EF_5F76234332D4__INCLUDED_)
#define AFX_CHAREDITDOC_H__14EB9155_A6E8_4993_89EF_5F76234332D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCharEditDoc : public CDocument
{
protected: // create from serialization only
	CCharEditDoc();
	DECLARE_DYNCREATE(CCharEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCharEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCharEditDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAREDITDOC_H__14EB9155_A6E8_4993_89EF_5F76234332D4__INCLUDED_)
