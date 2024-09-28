// EffEditBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../Properties/EffSheetWithTab.h"

#include "EffEditBar.h"


// CEffEditBar 대화 상자입니다.

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


// CEffEditBar 메시지 처리기입니다.

BOOL CEffEditBar::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTab = new CsheetWithTab ( SheetRect, this );
	m_psheetWithTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTab->SetActivePage ( GROUPPAGEINDEX );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffEditBar::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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

