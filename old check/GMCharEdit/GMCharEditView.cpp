// GMCharEditView.cpp : implementation of the CGMCharEditView class
//

#include "stdafx.h"
#include "GMCharEdit.h"

#include "MainFrm.h"
#include "GMCharEditDoc.h"
#include "GMCharEditView.h"

#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CGMCharEditView

IMPLEMENT_DYNCREATE(CGMCharEditView, CFormView)

BEGIN_MESSAGE_MAP(CGMCharEditView, CFormView)
END_MESSAGE_MAP()

// CGMCharEditView construction/destruction

CGMCharEditView::CGMCharEditView()
	: CFormView(CGMCharEditView::IDD)
{
	// TODO: add construction code here

}

CGMCharEditView::~CGMCharEditView()
{
}

void CGMCharEditView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGMCharEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CGMCharEditView::OnInitialUpdate()
{	
}


// CGMCharEditView diagnostics

#ifdef _DEBUG
void CGMCharEditView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGMCharEditView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGMCharEditDoc* CGMCharEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGMCharEditDoc)));
	return (CGMCharEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CGMCharEditView message handlers
