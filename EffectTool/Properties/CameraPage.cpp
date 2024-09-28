// CameraPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectCamera.h"
#include "EffGroupPage.h"

#include "CameraPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"


// CCameraEffPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCameraEffPage, CPropertyPage)
CCameraEffPage::CCameraEffPage()
	: CPropertyPage(CCameraEffPage::IDD)
{
}

CCameraEffPage::~CCameraEffPage()
{
}

void CCameraEffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA_SET, m_pComboCameraSet);
}


BEGIN_MESSAGE_MAP(CCameraEffPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_SET, &CCameraEffPage::OnCbnSelchangeComboCameraSet)
	//ON_EN_CHANGE(IDC_EDIT_POWER, &CCameraEffPage::OnEnChangeEditPower)
	ON_EN_CHANGE(IDC_EDIT_PLAY_NUM, &CCameraEffPage::OnEnChangeEditPlayNum)
END_MESSAGE_MAP()


// CCameraEffPage �޽��� ó�����Դϴ�.
void CCameraEffPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (CAMERA_PROPERTY*) pProp;

	CWnd*	pWnd;

	char	szValue[256];

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboCameraSet.ResetContent();

	strText.Format ( "1. Y�� ����" );
	nIndex = m_pComboCameraSet.AddString ( strText );
	m_pComboCameraSet.SetItemData ( nIndex, 1 );
	strText.Format ( "2. Rotation Y" );
	nIndex = m_pComboCameraSet.AddString ( strText );
	m_pComboCameraSet.SetItemData ( nIndex, 2 );

	m_pComboCameraSet.SetCurSel( m_pProperty->m_iCameraSet - 1 );

	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt( m_pProperty->m_fValue, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_PLAY_NUM );
	_gcvt( m_pProperty->m_fPlayNum, 7, szValue );
	pWnd->SetWindowText ( szValue );

	OnCbnSelchangeComboCameraSet();
}

void CCameraEffPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;

	int nIndex, nData;
	nIndex = m_pComboCameraSet.GetCurSel();
	if ( nIndex == LB_ERR )
		return;

	nData = m_pComboCameraSet.GetItemData( nIndex );

	m_pProperty->m_iCameraSet = nData;

	CWnd *pWnd;

	//	Note : ��ü���� �Ϳ� ���� ����
	//
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fValue = (float)atof ( strText.GetString() );


	pWnd = GetDlgItem ( IDC_EDIT_PLAY_NUM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fPlayNum = (float)atof ( strText.GetString() );



	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CCameraEffPage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : Tap ����
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CCameraEffPage::OnBnClickedButtonPlay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	Note : ���� ����.
	//
	OnBnClickedButtonApply();

	//	Note : ����Ʈ ����.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CCameraEffPage::OnCbnSelchangeComboCameraSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd* pWnd;
	DWORD arrayDlg[] = { IDC_STATIC_POWER, IDC_EDIT_POWER, IDC_STATIC_MAX, IDC_STATIC_PLAY_NUM, IDC_EDIT_PLAY_NUM, IDC_STATIC_MAX2 };
	for( DWORD i=0; i<sizeof(arrayDlg)/4; ++i )
	{
		pWnd = GetDlgItem ( arrayDlg[i] );
		pWnd->ShowWindow( FALSE );
	}

	int nIndex = m_pComboCameraSet.GetCurSel ();
	if ( nIndex == LB_ERR )
		return;

	switch( m_pComboCameraSet.GetItemData( nIndex ) )
	{
	case DxEffectCamera::CAMERA_MOVE_UP_DOWN:
		GetDlgItem( IDC_STATIC_POWER )->ShowWindow( TRUE );
		GetDlgItem( IDC_EDIT_POWER )->ShowWindow( TRUE );

		GetDlgItem( IDC_STATIC_PLAY_NUM )->ShowWindow( TRUE );
		GetDlgItem( IDC_EDIT_PLAY_NUM )->ShowWindow( TRUE );
		GetDlgItem( IDC_STATIC_MAX2 )->ShowWindow( TRUE );

		GetDlgItem( IDC_STATIC_POWER )->SetWindowText( _T("���� ���� :") );

		GetDlgItem( IDC_STATIC_PLAY_NUM )->SetWindowText( _T("�ʴ� ��鸮�� �� :") );
		GetDlgItem( IDC_STATIC_MAX2 )->SetWindowText( _T("( MAX : 30 )") );
		break;

	case DxEffectCamera::CAMERA_ROTATION_Y:
		GetDlgItem( IDC_STATIC_POWER )->ShowWindow( TRUE );
		GetDlgItem( IDC_EDIT_POWER )->ShowWindow( TRUE );
		GetDlgItem( IDC_STATIC_MAX )->ShowWindow( TRUE );

		GetDlgItem( IDC_STATIC_POWER )->SetWindowText( _T("ȸ�� �ӵ� :") );

		GetDlgItem( IDC_STATIC_MAX )->SetWindowText( _T("(�ʴ� ��ġ��ŭ�� ������ ȸ���Ѵ�.)") );
		break;
	};

	// IDC_EDIT_PLAY_NUM _ MAX �� üũ.
	OnEnChangeEditPlayNum();
}

//void CCameraEffPage::OnEnChangeEditPower()
//{
//	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
//	// CPropertyPage::OnInitDialog() �Լ��� ������ 
//	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
//	// �� �˸� �޽����� ������ �ʽ��ϴ�.
//
//	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	int nIndex = m_pComboCameraSet.GetCurSel ();
//	if ( nIndex == LB_ERR )
//		return;
//
//	char	szValue[256];
//	CString	strText;
//	CWnd* pWnd = GetDlgItem ( IDC_EDIT_POWER );
//	pWnd->GetWindowText ( strText );
//	m_pProperty->m_fValue = (float)atof ( strText.GetString() );
//
//	switch( m_pComboCameraSet.GetItemData( nIndex ) )
//	{
//	case DxEffectCamera::CAMERA_ROTATION_Y:
//		if ( m_pProperty->m_fValue > 1000 )
//		{
//			m_pProperty->m_fValue = 1000;
//			_gcvt( m_pProperty->m_fValue, 7, szValue );
//			GetDlgItem( IDC_EDIT_POWER )->SetWindowText( szValue );
//		}
//		break;
//	};
//}

void CCameraEffPage::OnEnChangeEditPlayNum()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_pComboCameraSet.GetCurSel ();
	if ( nIndex == LB_ERR )
		return;

	char	szValue[256];
	CString	strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_PLAY_NUM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fPlayNum = (float)atof ( strText.GetString() );

	switch( m_pComboCameraSet.GetItemData( nIndex ) )
	{
	case DxEffectCamera::CAMERA_MOVE_UP_DOWN:
		if ( m_pProperty->m_fPlayNum > 30 )
		{
			m_pProperty->m_fPlayNum = 30;
			_gcvt( m_pProperty->m_fPlayNum, 7, szValue );
			GetDlgItem( IDC_EDIT_PLAY_NUM )->SetWindowText( szValue );
		}
		break;
	};
}

