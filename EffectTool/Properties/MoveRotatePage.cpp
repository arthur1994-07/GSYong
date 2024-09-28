// MoveRotatePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMoveRotate.h"
#include "EffGroupPage.h"

#include "MoveRotatePage.h"

#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include ".\moverotatepage.h"

// CMoveRotatePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMoveRotatePage, CPropertyPage)
CMoveRotatePage::CMoveRotatePage()
	: CPropertyPage(CMoveRotatePage::IDD)
{
}

CMoveRotatePage::~CMoveRotatePage()
{
}

void CMoveRotatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMoveRotatePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnBnClickedButtonMove)
	ON_BN_CLICKED(IDC_CHECK_INITRAND, OnBnClickedCheckInitrand)
	ON_BN_CLICKED(IDC_CHECK_GOTOCENTER, OnBnClickedCheckGotocenter)
	//ON_BN_CLICKED(IDC_CHECK_PARENT_ROTATE, OnBnClickedCheckParentRotate)
END_MESSAGE_MAP()


// CMoveRotatePage 메시지 처리기입니다.
void CMoveRotatePage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (MOVEROTATE_PROPERTY*) pProp;

	char	szValue[256];

	CWnd*		pWnd;
	CButton*	pButton;

	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_X );
	_gcvt ( m_pProperty->m_fRotateAngelX, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_Y );
	_gcvt ( m_pProperty->m_fRotateAngelY, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_Z );
	_gcvt ( m_pProperty->m_fRotateAngelZ, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_INITRAND );
	if ( m_pProperty->m_dwFlag & USERANDROTATE )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_GOTOCENTER );
	if ( m_pProperty->m_dwFlag & USEGOTOCENTER )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );	

	//pButton = (CButton*)GetDlgItem ( IDC_CHECK_PARENT_ROTATE );
	//if ( m_pProperty->m_dwFlag & USEPARENTROTATE )	pButton->SetCheck ( TRUE );
	//else											pButton->SetCheck ( FALSE );	
}
void CMoveRotatePage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CMoveRotatePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	CString strText;	
	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_X );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fRotateAngelX = (float) atof ( strText.GetString() );


	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_Y );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fRotateAngelY = (float) atof ( strText.GetString() );


	pWnd = GetDlgItem ( IDC_EDIT_ROTATE_Z );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fRotateAngelZ = (float) atof ( strText.GetString() );



	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CMoveRotatePage::OnBnClickedButtonOk()
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

void CMoveRotatePage::OnBnClickedButtonMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vVelocity, 20.f, MOVEROTATEINDEX );
}

void CMoveRotatePage::OnBnClickedCheckInitrand()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_INITRAND );
	if ( pButton->GetCheck () )		m_pProperty->m_dwFlag |= USERANDROTATE;
	else							m_pProperty->m_dwFlag &= ~USERANDROTATE;
}

void CMoveRotatePage::OnBnClickedCheckGotocenter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_GOTOCENTER );
	if ( pButton->GetCheck () )		m_pProperty->m_dwFlag |= USEGOTOCENTER;
	else							m_pProperty->m_dwFlag &= ~USEGOTOCENTER;
}

//void CMoveRotatePage::OnBnClickedCheckParentRotate()
//{
//	CButton*	pButton;
//	pButton = (CButton*)GetDlgItem ( IDC_CHECK_PARENT_ROTATE );
//	if ( pButton->GetCheck () )		m_pProperty->m_dwFlag |= USEPARENTROTATE;
//	else							m_pProperty->m_dwFlag &= ~USEPARENTROTATE;
//}
