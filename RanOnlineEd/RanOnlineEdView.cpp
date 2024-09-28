// RanOnlineEdView.cpp : implementation of the RanOnlineEdView class
//

#include "stdafx.h"
#include "RanOnlineEd.h"

#include "RanOnlineEdDoc.h"
#include "RanOnlineEdView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// RanOnlineEdView

IMPLEMENT_DYNCREATE(RanOnlineEdView, CView)

BEGIN_MESSAGE_MAP(RanOnlineEdView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_CREATE()
END_MESSAGE_MAP()

// RanOnlineEdView construction/destruction

RanOnlineEdView::RanOnlineEdView()
{
	// TODO: add construction code here

}

RanOnlineEdView::~RanOnlineEdView()
{
}

BOOL RanOnlineEdView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// RanOnlineEdView drawing

void RanOnlineEdView::OnDraw(CDC* /*pDC*/)
{
	RanOnlineEdDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// RanOnlineEdView printing

BOOL RanOnlineEdView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void RanOnlineEdView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void RanOnlineEdView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// RanOnlineEdView diagnostics

#ifdef _DEBUG
void RanOnlineEdView::AssertValid() const
{
	CView::AssertValid();
}

void RanOnlineEdView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

RanOnlineEdDoc* RanOnlineEdView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(RanOnlineEdDoc)));
	return (RanOnlineEdDoc*)m_pDocument;
}
#endif //_DEBUG


// RanOnlineEdView message handlers

int RanOnlineEdView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  여기에 특수화된 작성 코드를 추가합니다.

    return 0;
}
