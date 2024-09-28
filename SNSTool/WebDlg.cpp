// WebDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanSNS.h"
#include "WebDlg.h"
#include ".\webdlg.h"


// CWebDlg ��ȭ �����Դϴ�.

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
	    

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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



// CWebDlg �޽��� ó�����Դϴ�.

HRESULT CWebDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

HRESULT CWebDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

INT_PTR CWebDlg::DoModal()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	return CDHtmlDialog::DoModal();
}

int CWebDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDHtmlDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.


	return 0;
}

void CWebDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDHtmlDialog::OnShowWindow(bShow, nStatus);

	if (m_pURL)
		m_Browser.Navigate(m_pURL, NULL, NULL, NULL, NULL);
	else
		m_Browser.Navigate("www.google.com", NULL, NULL, NULL, NULL);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CWebDlg::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.


	UpdateData(TRUE);	//DDX

	m_csLastURL = m_Browser.get_LocationURL();

	CDHtmlDialog::OnClose();
}

void CWebDlg::OnBnClickedButtonPin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.


	OnOK();
}
