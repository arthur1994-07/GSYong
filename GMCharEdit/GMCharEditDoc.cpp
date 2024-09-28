// GMCharEditDoc.cpp : implementation of the CGMCharEditDoc class
//

#include "stdafx.h"
#include "GMCharEdit.h"

#include "MainFrm.h"
#include "GMCharEditDoc.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CGMCharEditDoc

IMPLEMENT_DYNCREATE(CGMCharEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CGMCharEditDoc, CDocument)
END_MESSAGE_MAP()


// CGMCharEditDoc construction/destruction

CGMCharEditDoc::CGMCharEditDoc()
{
	// TODO: add one-time construction code here

}

CGMCharEditDoc::~CGMCharEditDoc()
{
}

BOOL CGMCharEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CGMCharEditDoc serialization

void CGMCharEditDoc::Serialize(CArchive& ar)
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


// CGMCharEditDoc diagnostics

#ifdef _DEBUG
void CGMCharEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGMCharEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGMCharEditDoc commands

BOOL CGMCharEditDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame && pFrame->m_Framework.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}