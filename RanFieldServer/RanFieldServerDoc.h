// RanFieldServerDoc.h : interface of the RanFieldServerDoc class
//


#pragma once


class RanFieldServerDoc : public CDocument
{
protected: // create from serialization only
	RanFieldServerDoc();
	DECLARE_DYNCREATE(RanFieldServerDoc)

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
	virtual ~RanFieldServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


