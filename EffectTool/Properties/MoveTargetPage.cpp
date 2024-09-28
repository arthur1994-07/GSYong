// MoveTargetPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMoveTarget.h"
#include "EffGroupPage.h"

#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include "MoveTargetPage.h"
#include ".\movetargetpage.h"


// CMoveTargetPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMoveTargetPage, CPropertyPage)
CMoveTargetPage::CMoveTargetPage()
	: CPropertyPage(CMoveTargetPage::IDD)
{
}

CMoveTargetPage::~CMoveTargetPage()
{
}

void CMoveTargetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMoveTargetPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_RADIO_S_TARGET, OnBnClickedRadioSTarget)
	ON_BN_CLICKED(IDC_RADIO_S_DIRECT, OnBnClickedRadioSDirect)
	ON_BN_CLICKED(IDC_BUTTON_S_DIRECT, OnBnClickedButtonSDirect)
	ON_BN_CLICKED(IDC_CHECK_S_RANDOM, OnBnClickedCheckSRandom)
	ON_BN_CLICKED(IDC_CHECK_S_GUIDED, OnBnClickedCheckSGuided)
	ON_BN_CLICKED(IDC_RADIO_E_CRASH, OnBnClickedRadioECrash)
	ON_BN_CLICKED(IDC_RADIO_E_TIMEOVER, OnBnClickedRadioETimeover)
	ON_BN_CLICKED(IDC_RADIO_E_DONTMOVE, OnBnClickedRadioEDontmove)
	ON_BN_CLICKED(IDC_CHECK_GROUND_CRASH, OnBnClickedCheckGroundCrash)
	ON_BN_CLICKED(IDC_CHECK_A_EFF, OnBnClickedCheckAEff)
	ON_BN_CLICKED(IDC_RADIO_A_CRASH, OnBnClickedRadioACrash)
	ON_BN_CLICKED(IDC_RADIO_A_HIGH, OnBnClickedRadioAHigh)
	ON_BN_CLICKED(IDC_RADIO_A_CRASHS, OnBnClickedRadioACrashs)
	ON_BN_CLICKED(IDC_RADIO_A_TIMEOVER, OnBnClickedRadioATimeover)
	ON_BN_CLICKED(IDC_RADIO_A_DONTMOVE, OnBnClickedRadioADontmove)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnBnClickedButtonSelect)
END_MESSAGE_MAP()


// CMoveTargetPage 메시지 처리기입니다.

void CMoveTargetPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (MOVETARGET_PROPERTY*) pProp;

	char	szValue[256];

	CWnd*		pWnd;
	CButton*	pButton;

	//	Note : Start 조건
	//
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_S_TARGET );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_S_DIRECT );
	pButton->SetCheck ( FALSE );

	if ( m_pProperty->m_dwFlag & USE_S_TARGET )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_S_TARGET );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_S_DIRECT )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_S_DIRECT );
		pButton->SetCheck ( TRUE );
	}


	pButton = (CButton*)GetDlgItem ( IDC_CHECK_S_RANDOM );
	if ( m_pProperty->m_dwFlag & USE_S_RANDOM )		pButton->SetCheck( TRUE );
	else											pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_S_GUIDED );
	if ( m_pProperty->m_dwFlag & USE_S_GUIDED )		pButton->SetCheck( TRUE );
	else											pButton->SetCheck( FALSE );


	pWnd = GetDlgItem ( IDC_EDIT_S_RANDOM );
	_gcvt ( m_pProperty->m_fRandom_Angel, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_S_GUIDED_ACCEL );
	_gcvt ( m_pProperty->m_fGuided_Accel, 7, szValue );
	pWnd->SetWindowText ( szValue );

	//	Note : End 조건
	//
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_CRASH );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_TIMEOVER );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_DONTMOVE );
	pButton->SetCheck ( FALSE );

	if ( m_pProperty->m_dwFlag & USE_E_CRASH )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_CRASH );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_E_TIMEOVER )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_TIMEOVER );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_E_DONTMOVE )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_DONTMOVE );
		pButton->SetCheck ( TRUE );
	}

	//	Note : 지형 충돌 체크
	//
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_GROUND_CRASH );
	if ( m_pProperty->m_dwFlag & USE_GROUND_CRASH )	pButton->SetCheck( TRUE );
	else											pButton->SetCheck( FALSE );

	//	Note : 추가 효과 설정
	//
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_A_EFF );
	if ( m_pProperty->m_dwFlag & USE_A_EFF )		pButton->SetCheck( TRUE );
	else											pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_CRASH );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_HIGH );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_CRASHS );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_TIMEOVER );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_DONTMOVE );
	pButton->SetCheck ( FALSE );

	if ( m_pProperty->m_dwFlag & USE_A_CRASH )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_CRASH );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_A_HIGH )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_HIGH );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_A_CRASHS )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_CRASHS );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_A_TIMEOVER )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_A_TIMEOVER );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_dwFlag & USE_E_DONTMOVE )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_E_DONTMOVE );
		pButton->SetCheck ( TRUE );
	}


	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->SetWindowText ( m_pProperty->m_szFileName );
}

void CMoveTargetPage::OnBnClickedButtonPlay()
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

void CMoveTargetPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	CString strText;	
	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_S_RANDOM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fRandom_Angel = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_S_GUIDED_ACCEL );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fGuided_Accel = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szFileName, strText.GetString() );

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CMoveTargetPage::OnBnClickedButtonOk()
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



void CMoveTargetPage::OnBnClickedRadioSTarget()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_S_ELASER;
	m_pProperty->m_dwFlag |= USE_S_TARGET;
}

void CMoveTargetPage::OnBnClickedRadioSDirect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_S_ELASER;
	m_pProperty->m_dwFlag |= USE_S_DIRECT;
}

void CMoveTargetPage::OnBnClickedButtonSDirect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vCustom_Velocity, 100.f, MOVETARGETINDEX );
}

void CMoveTargetPage::OnBnClickedCheckSRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_S_RANDOM );
	if ( pButton->GetCheck() )		m_pProperty->m_dwFlag |= USE_S_RANDOM;
	else							m_pProperty->m_dwFlag &= ~USE_S_RANDOM;
}

void CMoveTargetPage::OnBnClickedCheckSGuided()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_S_GUIDED );
	if ( pButton->GetCheck() )		m_pProperty->m_dwFlag |= USE_S_GUIDED;
	else							m_pProperty->m_dwFlag &= ~USE_S_GUIDED;
}



void CMoveTargetPage::OnBnClickedRadioECrash()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_E_ELASER;
	m_pProperty->m_dwFlag |= USE_E_CRASH;
}

void CMoveTargetPage::OnBnClickedRadioETimeover()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_E_ELASER;
	m_pProperty->m_dwFlag |= USE_E_TIMEOVER;
}

void CMoveTargetPage::OnBnClickedRadioEDontmove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_E_ELASER;
	m_pProperty->m_dwFlag |= USE_E_DONTMOVE;
}



void CMoveTargetPage::OnBnClickedCheckGroundCrash()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_GROUND_CRASH );
	if ( pButton->GetCheck() )		m_pProperty->m_dwFlag |= USE_GROUND_CRASH;
	else							m_pProperty->m_dwFlag &= ~USE_GROUND_CRASH;
	
}



void CMoveTargetPage::OnBnClickedCheckAEff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_A_EFF );
	if ( pButton->GetCheck() )		m_pProperty->m_dwFlag |= USE_A_EFF;
	else							m_pProperty->m_dwFlag &= ~USE_A_EFF;
}

void CMoveTargetPage::OnBnClickedRadioACrash()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_A_ELASER;
	m_pProperty->m_dwFlag |= USE_A_CRASH;
}

void CMoveTargetPage::OnBnClickedRadioAHigh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_A_ELASER;
	m_pProperty->m_dwFlag |= USE_A_HIGH;
}

void CMoveTargetPage::OnBnClickedRadioACrashs()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_A_ELASER;
	m_pProperty->m_dwFlag |= USE_A_CRASHS;
}

void CMoveTargetPage::OnBnClickedRadioATimeover()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_A_ELASER;
	m_pProperty->m_dwFlag |= USE_A_TIMEOVER;
}

void CMoveTargetPage::OnBnClickedRadioADontmove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USE_A_ELASER;
	m_pProperty->m_dwFlag |= USE_A_DONTMOVE;
}

void CMoveTargetPage::OnBnClickedButtonSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FileName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
		pWnd->SetWindowText ( FileName );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}



