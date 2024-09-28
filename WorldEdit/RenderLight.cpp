// RenderLight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "RenderLight.h"


// CRenderLight 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRenderLight, CDialog)
CRenderLight::CRenderLight(CWnd* pParent /*=NULL*/)
	: CDialog(CRenderLight::IDD, pParent)
{
}

CRenderLight::~CRenderLight()
{
}

void CRenderLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRenderLight, CDialog)
	ON_EN_CHANGE(IDC_EDIT_SPEED, OnEnChangeEditSpeed)
	ON_EN_CHANGE(IDC_EDIT_RATE, OnEnChangeEditRate)
	ON_EN_CHANGE(IDC_EDIT_COLORPOWER, OnEnChangeEditColorpower)
	ON_BN_CLICKED(IDC_CHECK_NEARLIGHT, OnBnClickedCheckNearlight)
END_MESSAGE_MAP()


// CRenderLight 메시지 처리기입니다.

BOOL CRenderLight::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_NEARLIGHT );
	if ( m_bNearLight )			pButton->SetCheck ( TRUE );
	else						pButton->SetCheck ( FALSE );

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_COL );
	

	pWnd	= GetDlgItem ( IDC_EDIT_SPEED );
	_gcvt ( m_fSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd	= GetDlgItem ( IDC_EDIT_RATE );
	_gcvt ( m_nRate, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd	= GetDlgItem ( IDC_EDIT_COLORPOWER );
	_gcvt ( m_fColorPower, 7, szValue );
	pWnd->SetWindowText ( szValue );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRenderLight::OnEnChangeEditSpeed()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_fSpeed = (float) atof ( strText.GetString() );
}

void CRenderLight::OnEnChangeEditRate()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_RATE );
	pWnd->GetWindowText ( strText );
	m_nRate = atoi ( strText.GetString() );
}

void CRenderLight::OnEnChangeEditColorpower()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_COLORPOWER );
	pWnd->GetWindowText ( strText );
	m_fColorPower = (float) atof ( strText.GetString() );
}

void CRenderLight::OnBnClickedCheckNearlight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_NEARLIGHT );
	if ( pButton->GetCheck() )	m_bNearLight = TRUE;
	else						m_bNearLight = FALSE;
}
