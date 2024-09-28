// SetData.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CommentTool.h"
#include "SetData.h"
#include ".\setdata.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// SetData 대화 상자입니다.

IMPLEMENT_DYNAMIC(SetDataDlg, CDialog)
SetDataDlg::SetDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetDataDlg::IDD, pParent)
{
}

SetDataDlg::~SetDataDlg()
{
}

void SetDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SetDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// SetData 메시지 처리기입니다.

BOOL SetDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWin_Text ( this, IDC_INPUT_TEXT, m_strData.c_str() );

	GetDlgItem ( IDC_INPUT_TEXT )->SetFocus();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void SetDataDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str = GetWin_Text( this, IDC_INPUT_TEXT );
	m_strData = str.GetBuffer( 0 );
	
	OnOK();
}
