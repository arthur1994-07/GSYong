// FootCollPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../charedit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxFootColl.h"

#include "FootCollPage.h"


// CFootCollPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFootCollPage, CPropertyPage)

CFootCollPage::CFootCollPage()
: CPropertyPage(CFootCollPage::IDD)
, m_pSelectFootPoint(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

CFootCollPage::~CFootCollPage()
{
}

void CFootCollPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFootCollPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CFootCollPage::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_RADIO_POINT_2, &CFootCollPage::OnBnClickedRadioPoint2)
	ON_BN_CLICKED(IDC_RADIO_POINT_4, &CFootCollPage::OnBnClickedRadioPoint4)
	ON_BN_CLICKED(IDC_BUTTON_POINT_2_FRONT, &CFootCollPage::OnBnClickedButtonPoint2Front)
	ON_BN_CLICKED(IDC_BUTTON_POINT_2_BACK, &CFootCollPage::OnBnClickedButtonPoint2Back)
	ON_BN_CLICKED(IDC_BUTTON_POINT_4_FRONT_LEFT, &CFootCollPage::OnBnClickedButtonPoint4FrontLeft)
	ON_BN_CLICKED(IDC_BUTTON_POINT_4_FRONT_RIGHT, &CFootCollPage::OnBnClickedButtonPoint4FrontRight)
	ON_BN_CLICKED(IDC_BUTTON_POINT_4_BACK_LEFT, &CFootCollPage::OnBnClickedButtonPoint4BackLeft)
	ON_BN_CLICKED(IDC_BUTTON_POINT_4_BACK_RIGHT, &CFootCollPage::OnBnClickedButtonPoint4BackRight)
	ON_BN_CLICKED(IDC_RADIO_POINT_NONE, &CFootCollPage::OnBnClickedRadioPointNone)
END_MESSAGE_MAP()


// CFootCollPage 메시지 처리기입니다.

BOOL CFootCollPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CFootCollPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButtonPoint2 = (CButton*)GetDlgItem( IDC_RADIO_POINT_2 );
	pButtonPoint2->SetCheck( FALSE );
	CButton* pButtonPoint4 = (CButton*)GetDlgItem( IDC_RADIO_POINT_4 );
	pButtonPoint4->SetCheck( FALSE );
	CButton* pButtonPoint0 = (CButton*)GetDlgItem( IDC_RADIO_POINT_NONE );
	pButtonPoint0->SetCheck( FALSE );

	m_nParentSel = pFrame->m_cDlgEdit.m_comboPage.GetCurSel();
	switch ( m_nParentSel )
	{
	case CHARTAB_CHARFILE:	// 캐릭터파일;
		{
			m_pFootColl = pView->GetSkinCharData()->GetFootColl();
		}
		break;
	case CHARTAB_VEHICLE: // Vehicle 파일;
		{
			m_pFootColl = pView->GetVehicleData()->GetFootColl();
		}
		break;
	default:
		MessageBox( "오류-FootCollPage는 캐릭터파일, Vehicle파일에서만 접근이 가능합니다" );
		break;
	};

	switch( m_pFootColl->m_emMode )
	{
	case DxFootColl::MODE_NONE:
		UpdateButton( FALSE, FALSE );
		pButtonPoint0->SetCheck( TRUE );
		break;
	case DxFootColl::MODE_POINT2:
		UpdateButton( TRUE, FALSE );
		pButtonPoint2->SetCheck( TRUE );
		break;
	case DxFootColl::MODE_POINT4:
		UpdateButton( FALSE, TRUE );
		pButtonPoint4->SetCheck( TRUE );
		break;
	};

	return CPropertyPage::OnSetActive();
}

BOOL CFootCollPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();

	m_pSelectFootPoint = NULL;
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CFootCollPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pSheetTabChar->SetActivePage( CHARTAB_BONE_SETTING );
}

// Matrix를 알맞게 변환한다.
void CFootCollPage::UpdateKeyBoardMouse()
{
	if ( !(DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & (DXKEY_DOWNED|DXKEY_UP)) )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	if( m_pSelectFootPoint )
	{
		m_pSelectFootPoint->x = m_matWorld._41;
		m_pSelectFootPoint->y = m_matWorld._42;
		m_pSelectFootPoint->z = m_matWorld._43;
	}
}

void CFootCollPage::OnBnClickedRadioPoint2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	m_pFootColl->m_emMode = DxFootColl::MODE_POINT2;

	UpdateButton( TRUE, FALSE );

	m_pSelectFootPoint = NULL;
	pView->DeActiveEditMatrix();
}

void CFootCollPage::OnBnClickedRadioPoint4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	m_pFootColl->m_emMode = DxFootColl::MODE_POINT4;

	UpdateButton( FALSE, TRUE );

	m_pSelectFootPoint = NULL;
	pView->DeActiveEditMatrix();
}

void CFootCollPage::OnBnClickedRadioPointNone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	m_pFootColl->m_emMode = DxFootColl::MODE_NONE;
	m_pFootColl->CleanUp();

	UpdateButton( FALSE, FALSE );

	m_pSelectFootPoint = NULL;
	pView->DeActiveEditMatrix();
}

void CFootCollPage::UpdateButton( BOOL bPoint2, BOOL bPoint4 )
{
	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_BUTTON_POINT_2_FRONT );
	pWnd->EnableWindow( bPoint2 );
	pWnd = GetDlgItem( IDC_BUTTON_POINT_2_BACK );
	pWnd->EnableWindow( bPoint2 );

	pWnd = GetDlgItem( IDC_BUTTON_POINT_4_FRONT_LEFT );
	pWnd->EnableWindow( bPoint4 );
	pWnd = GetDlgItem( IDC_BUTTON_POINT_4_FRONT_RIGHT );
	pWnd->EnableWindow( bPoint4 );
	pWnd = GetDlgItem( IDC_BUTTON_POINT_4_BACK_LEFT );
	pWnd->EnableWindow( bPoint4 );
	pWnd = GetDlgItem( IDC_BUTTON_POINT_4_BACK_RIGHT );
	pWnd->EnableWindow( bPoint4 );
}

void CFootCollPage::OnBnClickedButtonPoint2Front()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_2[0] );
}

void CFootCollPage::OnBnClickedButtonPoint2Back()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_2[1] );
}

void CFootCollPage::OnBnClickedButtonPoint4FrontLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_4[0] );
}

void CFootCollPage::OnBnClickedButtonPoint4FrontRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_4[1] );
}

void CFootCollPage::OnBnClickedButtonPoint4BackLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_4[2] );
}

void CFootCollPage::OnBnClickedButtonPoint4BackRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	ActiveEditMatrix( m_pFootColl->m_vecFootColl_4[3] );
}

void CFootCollPage::ActiveEditMatrix( const D3DXVECTOR3& vPos )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	m_pSelectFootPoint = const_cast<D3DXVECTOR3 *>( &vPos );

	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;
	pView->ActiveEditMatrix( &m_matWorld );
}


