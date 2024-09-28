// Project3Doc.h : interface of the CTool_VisualMaterialDoc class
//


#pragma once


class CTool_VisualMaterialDoc : public CDocument
{
protected: // create from serialization only
	CTool_VisualMaterialDoc();
	DECLARE_DYNCREATE(CTool_VisualMaterialDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CTool_VisualMaterialDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFileNew();
    afx_msg void OnFileSave();
    afx_msg void OnFileOpen();
};
