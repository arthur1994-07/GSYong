// RanFieldServerView.cpp : implementation of the RanFieldServerView class
//

#include "stdafx.h"
#include "RanFieldServer.h"
#include "RanFieldServerDoc.h"
#include "RanFieldServerView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// RanFieldServerView

IMPLEMENT_DYNCREATE(RanFieldServerView, CView)

BEGIN_MESSAGE_MAP(RanFieldServerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// RanFieldServerView construction/destruction

RanFieldServerView::RanFieldServerView()
{
	// TODO: add construction code here    
}

RanFieldServerView::~RanFieldServerView()
{
}

BOOL RanFieldServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// RanFieldServerView drawing

void RanFieldServerView::OnDraw(CDC* /*pDC*/)
{
	RanFieldServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// RanFieldServerView printing

BOOL RanFieldServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void RanFieldServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void RanFieldServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// RanFieldServerView diagnostics

#ifdef _DEBUG
void RanFieldServerView::AssertValid() const
{
	CView::AssertValid();
}

void RanFieldServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

RanFieldServerDoc* RanFieldServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(RanFieldServerDoc)));
	return (RanFieldServerDoc*)m_pDocument;
}
#endif //_DEBUG


// RanFieldServerView message handlers
