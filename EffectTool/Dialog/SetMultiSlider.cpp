// SetMultiSlider.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "SetMultiSlider.h"


// CSetMultiSlider 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetMultiSlider, CDialog)
CSetMultiSlider::CSetMultiSlider(CWnd* pParent /*=NULL*/)
	: CDialog(CSetMultiSlider::IDD, pParent)
{
	m_dwFlag = 0L;
	m_bUse	= FALSE;
	m_fTemp	= 1.f;
	m_fWidth	= 1.f;

	m_values = (float*)malloc(sizeof(float) * 2);

	m_values[0] = 20.f;
	m_values[1] = 80.f;

	m_fStart = 0.f;
	m_fMid1 = 0.f;
	m_fMid2 = 0.f;
	m_fEnd = 0.f;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CSetMultiSlider::~CSetMultiSlider()
{
	free(m_values);

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CSetMultiSlider::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
}


BEGIN_MESSAGE_MAP(CSetMultiSlider, CDialog)
	ON_EN_CHANGE(IDC_EDIT_START, OnEnChangeEditStart)
	ON_EN_CHANGE(IDC_EDIT_MID1, OnEnChangeEditMid1)
	ON_EN_CHANGE(IDC_EDIT_MID2, OnEnChangeEditMid2)
	ON_EN_CHANGE(IDC_EDIT_END, OnEnChangeEditEnd)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_USE, OnBnClickedCheckUse)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_TEMP, OnEnChangeEditTemp)
END_MESSAGE_MAP()


// CSetMultiSlider 메시지 처리기입니다.

BOOL CSetMultiSlider::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	GetDlgItem ( IDC_CHECK_USE ) -> ShowWindow ( TRUE );
	GetDlgItem ( IDC_EDIT_WIDTH ) -> ShowWindow ( TRUE );
	GetDlgItem ( IDC_STATIC_WIDTH ) -> ShowWindow ( TRUE );
	GetDlgItem ( IDC_EDIT_TEMP ) -> ShowWindow ( TRUE );
	GetDlgItem ( IDC_STATIC_TEMP ) -> ShowWindow ( TRUE );

	if ( !( m_dwFlag & USECHECK ) )
	{
		GetDlgItem ( IDC_CHECK_USE ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEEDIT ) )
	{
		GetDlgItem ( IDC_EDIT_TEMP ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_STATIC_TEMP ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEWIDTH ) )
	{
		GetDlgItem ( IDC_EDIT_WIDTH ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_STATIC_WIDTH ) -> ShowWindow ( FALSE );
	}

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_USE );

	if ( m_bUse )	pButton->SetCheck( TRUE );
	else			pButton->SetCheck( FALSE );

	m_colors = RGB(255, 0, 0);

	int range;
	float min, max, temp;

	//Set the maximum number of markers you need
	int m_NumMarkers = 2;

	//Set the min/max range for the slider
	min = 0;
	max = 100;
	if(max < min)
	{
		temp = max;
		max = min;
		min = temp;
	}
	//m_Min.Format("%.1f",min);
	//m_Max.Format("%.1f",max);
	range = (int)(max - min);

	int LayOut = MLTSLD_DISPLAY_VALUES | MLTSLD_DISPLAY_TOOLTIPS | MLTSLD_PREVENT_CROSSING | MLTSLD_ALLOW_SET_POS;
	m_Slider.Init(min, max, 10, m_NumMarkers, m_values, &m_colors, LayOut);

	CString Str;
	Str.Format ( "%3.3f", m_fWidth );
	GetDlgItem ( IDC_EDIT_WIDTH ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fTemp );
	GetDlgItem ( IDC_EDIT_TEMP ) -> SetWindowText ( Str );

	Str.Format ( "%3.3f", m_fStart );
	GetDlgItem ( IDC_EDIT_START ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fMid1 );
	GetDlgItem ( IDC_EDIT_MID1 ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fMid2 );
	GetDlgItem ( IDC_EDIT_MID2 ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fEnd );
	GetDlgItem ( IDC_EDIT_END ) -> SetWindowText ( Str );


	if ( !m_StrTitle.IsEmpty() )		SetWindowText ( m_StrTitle );

	if ( !m_StrUse.IsEmpty() )
		GetDlgItem(IDC_CHECK_USE)->SetWindowText(m_StrUse);

	if ( !m_StrWidth.IsEmpty() )
		GetDlgItem(IDC_STATIC_WIDTH)->SetWindowText(m_StrWidth);

	if ( !m_StrTemp.IsEmpty() )
		GetDlgItem(IDC_STATIC_TEMP)->SetWindowText(m_StrTemp);

	if ( !m_StrTemp4.IsEmpty() )
		GetDlgItem(IDC_STATIC_TEMP4)->SetWindowText(m_StrTemp4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSetMultiSlider::OnEnChangeEditStart()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_START)->GetWindowText ( Str );
	m_fStart = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditMid1()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_MID1)->GetWindowText ( Str );
	m_fMid1 = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditMid2()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_MID2)->GetWindowText ( Str );
	m_fMid2 = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditEnd()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_END)->GetWindowText ( Str );
	m_fEnd = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int count;

	count  = m_Slider.GetMarkers(m_values);
}

void CSetMultiSlider::OnBnClickedCheckUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_USE );

	if ( m_bUse )
	{
		m_bUse = FALSE;
		pButton->SetCheck( FALSE );
	}
	else
	{
		m_bUse = TRUE;
		pButton->SetCheck( TRUE );
	}
}

void CSetMultiSlider::OnEnChangeEditTemp()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_TEMP)->GetWindowText ( Str );
	m_fTemp = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditWidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Str;
	GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText ( Str );
	m_fWidth = (float) atof ( Str.GetString() );
}

