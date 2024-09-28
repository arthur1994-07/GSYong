// RenderLight.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "RenderLight.h"


// CRenderLight ��ȭ �����Դϴ�.

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


// CRenderLight �޽��� ó�����Դϴ�.

BOOL CRenderLight::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CRenderLight::OnEnChangeEditSpeed()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_fSpeed = (float) atof ( strText.GetString() );
}

void CRenderLight::OnEnChangeEditRate()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_RATE );
	pWnd->GetWindowText ( strText );
	m_nRate = atoi ( strText.GetString() );
}

void CRenderLight::OnEnChangeEditColorpower()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString		strText;
	CWnd*		pWnd;
	pWnd	= GetDlgItem ( IDC_EDIT_COLORPOWER );
	pWnd->GetWindowText ( strText );
	m_fColorPower = (float) atof ( strText.GetString() );
}

void CRenderLight::OnBnClickedCheckNearlight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_NEARLIGHT );
	if ( pButton->GetCheck() )	m_bNearLight = TRUE;
	else						m_bNearLight = FALSE;
}
