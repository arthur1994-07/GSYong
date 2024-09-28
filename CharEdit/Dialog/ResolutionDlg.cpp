// ResolutionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EngineLib/DxTools/NSCharAniSequence.h"
#include "ResolutionDlg.h"

#pragma warning(disable:4996)
// CResolutionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CResolutionDlg, CDialog)

CResolutionDlg::CResolutionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResolutionDlg::IDD, pParent)
	, m_dwResolutionINDEX(0)
	, m_dwAA_INDEX(4)
	, m_fAddTime(0.f)
	, m_fColorPower(0.f)
{

}

CResolutionDlg::~CResolutionDlg()
{
}

void CResolutionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResolutionDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_1, &CResolutionDlg::OnBnClickedRadioResolution1)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_2, &CResolutionDlg::OnBnClickedRadioResolution2)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_3, &CResolutionDlg::OnBnClickedRadioResolution3)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_4, &CResolutionDlg::OnBnClickedRadioResolution4)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_5, &CResolutionDlg::OnBnClickedRadioResolution5)
	ON_BN_CLICKED(IDOK, &CResolutionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_AA_1X, &CResolutionDlg::OnBnClickedRadioAa1x)
	ON_BN_CLICKED(IDC_RADIO_AA_2X, &CResolutionDlg::OnBnClickedRadioAa2x)
	ON_BN_CLICKED(IDC_RADIO_AA_4X, &CResolutionDlg::OnBnClickedRadioAa4x)
	ON_EN_CHANGE(IDC_EDIT_CUSTOM_SCNWIDTH, &CResolutionDlg::OnEnChangeCustomScnWidth)
	ON_EN_CHANGE(IDC_EDIT_CUSTOM_SCNHEIGHT,&CResolutionDlg::OnEnChangeCustomScnHeight)
END_MESSAGE_MAP()

// CResolutionDlg 메시지 처리기입니다.

// 해상도
// 640*480
// 720*480
// 800*600
// 1280*720

BOOL CResolutionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	CString		Str;

	pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_1 );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_2 );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_3 );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_4 );
	pButton->SetCheck( FALSE );

	switch( m_dwResolutionINDEX )
	{
	case 0:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_1 );
		pButton->SetCheck( TRUE );
		break;
	case 1:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_2 );
		pButton->SetCheck( TRUE );
		break;
	case 2:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_3 );
		pButton->SetCheck( TRUE );
		break;
	case 3:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_4 );
		pButton->SetCheck( TRUE );
		break;
	case 4:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_RESOLUTION_5 );
		pButton->SetCheck( TRUE );

		char szWidth[255], szHeight[255];
		_itoa_s((INT)NSCharAniSequence::GetCustomScnWidth(), szWidth, 255, 10);
		_itoa_s((INT)NSCharAniSequence::GetCustomScnHeight(), szHeight, 255, 10);

		GetDlgItem( IDC_EDIT_CUSTOM_SCNWIDTH )->SetWindowText(szWidth);
		GetDlgItem( IDC_EDIT_CUSTOM_SCNHEIGHT )->SetWindowText(szHeight);
		break;
	};

	switch( m_dwAA_INDEX )
	{
	case 1:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_AA_1X );
		pButton->SetCheck( TRUE );
		break;
	case 2:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_AA_2X );
		pButton->SetCheck( TRUE );
		break;
	case 4:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_AA_4X );
		pButton->SetCheck( TRUE );
		break;
	};

	pWnd = GetDlgItem( IDC_EDIT_ADD_TIME );
	Str.Format( _T("%3.3f"), m_fAddTime );
	pWnd->SetWindowText ( Str );

	pWnd = GetDlgItem( IDC_EDIT_FPS );
	Str.Format( _T("%d"), m_dwFPS );
	pWnd->SetWindowText ( Str );

	pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER );
	Str.Format( _T("%3.3f"), m_fColorPower );
	pWnd->SetWindowText ( Str );	

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_CURRENTFRAME );
	pButton->SetCheck(FALSE);
	m_bOnlyCurrentFrame = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CResolutionDlg::OnBnClickedRadioResolution1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwResolutionINDEX = 0;
}

void CResolutionDlg::OnBnClickedRadioResolution2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwResolutionINDEX = 1;
}

void CResolutionDlg::OnBnClickedRadioResolution3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwResolutionINDEX = 2;
}

void CResolutionDlg::OnBnClickedRadioResolution4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwResolutionINDEX = 3;
}

void CResolutionDlg::OnBnClickedRadioResolution5()
{
	m_dwResolutionINDEX = 4;
}

void CResolutionDlg::OnBnClickedRadioAa1x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwAA_INDEX = 1;
}

void CResolutionDlg::OnBnClickedRadioAa2x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwAA_INDEX = 2;
}

void CResolutionDlg::OnBnClickedRadioAa4x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwAA_INDEX = 4;
}

void CResolutionDlg::OnEnChangeCustomScnWidth(void)
{
	CString strIdcText;
	GetDlgItem(IDC_EDIT_CUSTOM_SCNWIDTH)->GetWindowText( strIdcText );
	NSCharAniSequence::SetCustomScnWidth((int)atoi(strIdcText.GetString()));
}

void CResolutionDlg::OnEnChangeCustomScnHeight(void)
{
	CString strIdcText;
	GetDlgItem(IDC_EDIT_CUSTOM_SCNHEIGHT)->GetWindowText( strIdcText );
	NSCharAniSequence::SetCustomScnHeight((int)atoi(strIdcText.GetString()));
}

void CResolutionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CString		Str;

	pWnd = GetDlgItem( IDC_EDIT_ADD_TIME );
	pWnd->GetWindowText ( Str );
	m_fAddTime = (float) _tstof ( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_FPS );
	pWnd->GetWindowText ( Str );
	m_dwFPS = _tstoi ( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER );
	pWnd->GetWindowText ( Str );
	m_fColorPower = (float) _tstof ( Str.GetString() );

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_CURRENTFRAME );
	m_bOnlyCurrentFrame = pButton->GetCheck();

	OnOK();
}

