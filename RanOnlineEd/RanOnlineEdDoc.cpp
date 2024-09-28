// RanOnlineEdDoc.cpp : implementation of the RanOnlineEdDoc class
//

#include "stdafx.h"
#include "RanOnlineEd.h"

#include "RanOnlineEdDoc.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// RanOnlineEdDoc

IMPLEMENT_DYNCREATE(RanOnlineEdDoc, CDocument)

BEGIN_MESSAGE_MAP(RanOnlineEdDoc, CDocument)
END_MESSAGE_MAP()


// RanOnlineEdDoc construction/destruction

RanOnlineEdDoc::RanOnlineEdDoc()
{
	// TODO: add one-time construction code here

}

RanOnlineEdDoc::~RanOnlineEdDoc()
{
}

BOOL RanOnlineEdDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// RanOnlineEdDoc serialization

void RanOnlineEdDoc::Serialize(CArchive& ar)
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


// RanOnlineEdDoc diagnostics

#ifdef _DEBUG
void RanOnlineEdDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void RanOnlineEdDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// RanOnlineEdDoc commands
