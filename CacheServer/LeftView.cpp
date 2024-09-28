// LeftView.cpp : ���� �����Դϴ�.
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
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->StartServer();
}

void LeftView::OnBnClickedButtonClearLog()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->ClearLogMessage();
}

void LeftView::OnBnClickedButtonLogItemExchange()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->AdoTest1();
}

void LeftView::OnBnClickedButtonAdoTest2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
    pFrame->AdoTest2();
}
