// LeftView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CacheServer.h"
#include "CacheServerMainFrm.h"
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
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &LeftView::OnBnClickedButtonClearLog)
    ON_BN_CLICKED(IDC_BUTTON_LOG_ITEM_EXCHANGE, &LeftView::OnBnClickedButtonLogItemExchange)
	ON_BN_CLICKED(IDC_BUTTON_ADO_TEST1, &LeftView::OnBnClickedButtonAdoTest1)
    ON_BN_CLICKED(IDC_BUTTON_ADO_TEST2, &LeftView::OnBnClickedButtonAdoTest2)
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
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->StartServer();
}

void LeftView::OnBnClickedButtonClearLog()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->ClearLogMessage();
}

void LeftView::OnBnClickedButtonLogItemExchange()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->LogItemExchange();
}

void LeftView::SetStartButtonText(const std::string& Text)
{
    CButton* pButton = (CButton*) GetDlgItem(IDC_BUTTON_START);
    pButton->SetWindowText(Text.c_str());
}

void LeftView::OnBnClickedButtonAdoTest1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->AdoTest1();
}

void LeftView::OnBnClickedButtonAdoTest2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->AdoTest2();
}
