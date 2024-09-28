// LeftView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanFieldServerMainFrm.h"
#include "RanFieldServer.h"
#include "LeftView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// LeftView

IMPLEMENT_DYNCREATE(LeftView, CFormView)

LeftView::LeftView()
	: CFormView(LeftView::IDD)
{

}

LeftView::~LeftView()
{
}

void LeftView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LeftView, CFormView)
    ON_BN_CLICKED(IDC_BUTTON_START, &LeftView::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// LeftView �����Դϴ�.

#ifdef _DEBUG
void LeftView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LeftView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LeftView �޽��� ó�����Դϴ�.

void LeftView::OnBnClickedButtonStart()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    RanFieldServerMainFrame *pFrame = (RanFieldServerMainFrame*) AfxGetMainWnd();
    pFrame->StartServer();
}
