// LeftView.cpp : 구현 파일입니다.
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


// LeftView 진단입니다.

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


// LeftView 메시지 처리기입니다.

void LeftView::OnBnClickedButtonStart()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    RanFieldServerMainFrame *pFrame = (RanFieldServerMainFrame*) AfxGetMainWnd();
    pFrame->StartServer();
}
