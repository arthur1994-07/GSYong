// GMCharEditDoc.h : interface of the CGMCharEditDoc class
//


#pragma once

class CGMCharEditDoc : public CDocument
{
protected: // create from serialization only
	CGMCharEditDoc();
	DECLARE_DYNCREATE(CGMCharEditDoc)

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
	virtual ~CGMCharEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};


