// CacheServerDoc.cpp : implementation of the CacheServerDoc class
//

#include "stdafx.h"
#include "CacheServer.h"
#include "CacheServerMainFrm.h"
#include "CacheServerDoc.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CacheServerDoc

IMPLEMENT_DYNCREATE(CacheServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CacheServerDoc, CDocument)
END_MESSAGE_MAP()


// CacheServerDoc construction/destruction

CacheServerDoc::CacheServerDoc()
{
	// TODO: add one-time construction code here

}

CacheServerDoc::~CacheServerDoc()
{
}

BOOL CacheServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CacheServerDoc serialization

void CacheServerDoc::Serialize(CArchive& ar)
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


// CacheServerDoc diagnostics

#ifdef _DEBUG
void CacheServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CacheServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CacheServerDoc commands

BOOL CacheServerDoc::SaveModified()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    SetModifiedFlag(FALSE);
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    if (true == pFrame->IsServerRunning())
    {
        AfxMessageBox("First stop the server!!!!!", MB_OK);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
    //return CDocument::SaveModified();
}
