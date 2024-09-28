// CacheServerView.cpp : implementation of the CacheServerView class
//

#include "stdafx.h"
#include "CacheServer.h"

#include "CacheServerDoc.h"
#include "CacheServerView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CacheServerView

IMPLEMENT_DYNCREATE(CacheServerView, CView)

BEGIN_MESSAGE_MAP(CacheServerView, CView)
END_MESSAGE_MAP()

// CacheServerView construction/destruction

CacheServerView::CacheServerView()
{
	// TODO: add construction code here

}

CacheServerView::~CacheServerView()
{
}

BOOL CacheServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CacheServerView drawing

void CacheServerView::OnDraw(CDC* /*pDC*/)
{
	CacheServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CacheServerView diagnostics

#ifdef _DEBUG
void CacheServerView::AssertValid() const
{
	CView::AssertValid();
}

void CacheServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CacheServerDoc* CacheServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CacheServerDoc)));
	return (CacheServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CacheServerView message handlers
