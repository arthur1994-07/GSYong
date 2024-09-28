// EffEditBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../Properties/EffSheetWithTab.h"

#include "EffEditBar.h"


// CEffEditBar ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffEditBar, CDialog)
CEffEditBar::CEffEditBar(CWnd* pParent /*=NULL*/)
	: CDialog(CEffEditBar::IDD, pParent),
	m_psheetWithTab(NULL)
{
}

CEffEditBar::~CEffEditBar()
{
}

void CEffEditBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffEditBar, CDialog)
END_MESSAGE_MAP()


// CEffEditBar �޽��� ó�����Դϴ�.

BOOL CEffEditBar::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTab = new CsheetWithTab ( SheetRect, this );
	m_psheetWithTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTab->SetActivePage ( GROUPPAGEINDEX );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffEditBar::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	SAFE_DELETE(m_psheetWithTab);

	CDialog::PostNcDestroy();
}

void CEffEditBar::SelectEffProp ( DWORD dwProp )
{
	if ( m_psheetWithTab )
	{
		m_psheetWithTab->m_EffGroupPage.SelectEffProp ( (EFF_PROPERTY*) dwProp );
	}
}

