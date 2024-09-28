// WebDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanSNS.h"
#include "WebDlg.h"
#include ".\webdlg.h"


// CWebDlg 대화 상자입니다.

IMPLEMENT_DYNCREATE(CWebDlg, CDHtmlDialog)

CWebDlg::CWebDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CWebDlg::IDD, CWebDlg::IDH, pParent)
	, m_csPIN(_T(""))
{
	m_pURL = NULL;
	m_bTwitter = FALSE;
}

CWebDlg::~CWebDlg()
{
}

void CWebDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER2, m_Browser);
	DDX_Text(pDX, IDC_EDIT_PIN, m_csPIN);
	DDX_Control(pDX, IDC_EDIT_PIN, m_ebPIN);
	DDX_Control(pDX, IDC_STATIC_PIN, m_sttPIN);
	DDX_Control(pDX, IDC_BUTTON_PIN, m_btPIN);
}

BOOL CWebDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	if (m_bTwitter)
	{
		m_ebPIN.EnableWindow(TRUE);
		m_sttPIN.EnableWindow(TRUE);
		m_btPIN.EnableWindow(TRUE);
	}
	else
	{
		m_ebPIN.EnableWindow(FALSE);
		m_sttPIN.EnableWindow(FALSE);
		m_btPIN.EnableWindow(FALSE);
	}
	    

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(CWebDlg, CDHtmlDialog)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_PIN, OnBnClickedButtonPin)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CWebDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CWebDlg 메시지 처리기입니다.

HRESULT CWebDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

HRESULT CWebDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

INT_PTR CWebDlg::DoModal()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDHtmlDialog::DoModal();
}

int CWebDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDHtmlDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.


	return 0;
}

void CWebDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDHtmlDialog::OnShowWindow(bShow, nStatus);

	if (m_pURL)
		m_Browser.Navigate(m_pURL, NULL, NULL, NULL, NULL);
	else
		m_Browser.Navigate("www.google.com", NULL, NULL, NULL, NULL);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CWebDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	UpdateData(TRUE);	//DDX

	m_csLastURL = m_Browser.get_LocationURL();

	CDHtmlDialog::OnClose();
}

void CWebDlg::OnBnClickedButtonPin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	OnOK();
}
