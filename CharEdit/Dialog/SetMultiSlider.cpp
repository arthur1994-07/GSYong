// SetMultiSlider.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SetMultiSlider.h"


// CSetMultiSlider ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSetMultiSlider, CDialog)
CSetMultiSlider::CSetMultiSlider(CWnd* pParent /*=NULL*/)
	: CDialog(CSetMultiSlider::IDD, pParent)
{
	m_bUse	= FALSE;
	m_fTemp	= 1.f;

	m_values = (float*)malloc(sizeof(float) * 2);

	m_values[0] = 20.f;
	m_values[1] = 80.f;

	m_fStart = 0.f;
	m_fMid1 = 0.f;
	m_fMid2 = 0.f;
	m_fEnd = 0.f;
}

CSetMultiSlider::~CSetMultiSlider()
{
	free(m_values);
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
	ON_EN_CHANGE(IDC_EDIT_TEMP, OnEnChangeEditTemp)
END_MESSAGE_MAP()


// CSetMultiSlider �޽��� ó�����Դϴ�.

BOOL CSetMultiSlider::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	if ( !( m_dwFlag & USECHECK ) )
	{
		GetDlgItem ( IDC_CHECK_USE ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEEDIT ) )
	{
		GetDlgItem ( IDC_EDIT_TEMP ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_STATIC_TEMP ) -> ShowWindow ( FALSE );
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

	if ( !m_StrTemp.IsEmpty() )
		GetDlgItem(IDC_STATIC_TEMP)->SetWindowText(m_StrTemp);

	if ( !m_StrTemp4.IsEmpty() )
		GetDlgItem(IDC_STATIC_TEMP4)->SetWindowText(m_StrTemp4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSetMultiSlider::OnEnChangeEditStart()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_START)->GetWindowText ( Str );
	m_fStart = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditMid1()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_MID1)->GetWindowText ( Str );
	m_fMid1 = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditMid2()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_MID2)->GetWindowText ( Str );
	m_fMid2 = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnEnChangeEditEnd()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_END)->GetWindowText ( Str );
	m_fEnd = (float) atof ( Str.GetString() );
}

void CSetMultiSlider::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int count;

	count  = m_Slider.GetMarkers(m_values);
}

void CSetMultiSlider::OnBnClickedCheckUse()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_TEMP)->GetWindowText ( Str );
	m_fTemp = (float) atof ( Str.GetString() );
}
