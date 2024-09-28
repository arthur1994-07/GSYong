// RanFieldServerDoc.cpp : implementation of the RanFieldServerDoc class
//

#include "stdafx.h"
#include "RanFieldServer.h"

#include "RanFieldServerDoc.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// RanFieldServerDoc

IMPLEMENT_DYNCREATE(RanFieldServerDoc, CDocument)

BEGIN_MESSAGE_MAP(RanFieldServerDoc, CDocument)
END_MESSAGE_MAP()


// RanFieldServerDoc construction/destruction

RanFieldServerDoc::RanFieldServerDoc()
{
	// TODO: add one-time construction code here

}

RanFieldServerDoc::~RanFieldServerDoc()
{
}

BOOL RanFieldServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// RanFieldServerDoc serialization

void RanFieldServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// RanFieldServerDoc diagnostics

#ifdef _DEBUG
void RanFieldServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void RanFieldServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// RanFieldServerDoc commands
