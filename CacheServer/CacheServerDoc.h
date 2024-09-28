// CacheServerDoc.h : interface of the CacheServerDoc class
//


#pragma once


class CacheServerDoc : public CDocument
{
protected: // create from serialization only
	CacheServerDoc();
	DECLARE_DYNCREATE(CacheServerDoc)

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
	virtual ~CacheServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
    virtual BOOL SaveModified();
};


