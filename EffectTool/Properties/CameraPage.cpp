// CameraPage.cpp : 구현 파일입니다.
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


// CCameraEffPage 대화 상자입니다.

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


// CCameraEffPage 메시지 처리기입니다.
void CCameraEffPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (CAMERA_PROPERTY*) pProp;

	CWnd*	pWnd;

	char	szValue[256];

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboCameraSet.ResetContent();

	strText.Format ( "1. Y축 흔들기" );
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;

	int nIndex, nData;
	nIndex = m_pComboCameraSet.GetCurSel();
	if ( nIndex == LB_ERR )
		return;

	nData = m_pComboCameraSet.GetItemData( nIndex );

	m_pProperty->m_iCameraSet = nData;

	CWnd *pWnd;

	//	Note : 전체적인 것에 대한 셋팅
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : Tap 적용
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CCameraEffPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//	Note : 설정 적용.
	//
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CCameraEffPage::OnCbnSelchangeComboCameraSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

		GetDlgItem( IDC_STATIC_POWER )->SetWindowText( _T("세기 조정 :") );

		GetDlgItem( IDC_STATIC_PLAY_NUM )->SetWindowText( _T("초당 흔들리는 수 :") );
		GetDlgItem( IDC_STATIC_MAX2 )->SetWindowText( _T("( MAX : 30 )") );
		break;

	case DxEffectCamera::CAMERA_ROTATION_Y:
		GetDlgItem( IDC_STATIC_POWER )->ShowWindow( TRUE );
		GetDlgItem( IDC_EDIT_POWER )->ShowWindow( TRUE );
		GetDlgItem( IDC_STATIC_MAX )->ShowWindow( TRUE );

		GetDlgItem( IDC_STATIC_POWER )->SetWindowText( _T("회전 속도 :") );

		GetDlgItem( IDC_STATIC_MAX )->SetWindowText( _T("(초당 수치만큼의 각도로 회전한다.)") );
		break;
	};

	// IDC_EDIT_PLAY_NUM _ MAX 값 체크.
	OnEnChangeEditPlayNum();
}

//void CCameraEffPage::OnEnChangeEditPower()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CPropertyPage::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

