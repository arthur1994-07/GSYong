// LightPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/Light/DxShadowLight.h"
#include "../EngineLib/DxTools/Light/NSShadowLight.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/G-Logic/GLPeriod.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "ShadowLightPage.h"
#include "State/WorldStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CShadowLightPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CShadowLightPage, CPropertyPage)
CShadowLightPage::CShadowLightPage()
	: CPropertyPage(CShadowLightPage::IDD)
	, m_bDlgInit(FALSE)
    , m_fAttMax0(0.f)
    , m_fAttMax1(0.f)
    , m_fAttMax2(0.f)
{
}

CShadowLightPage::~CShadowLightPage()
{
}

void CShadowLightPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LIGHT, m_ListLight);
	DDX_Control(pDX, IDC_LIST_COLOR, m_wndList);
    DDX_Control(pDX, IDC_SLIDER_ATT0, m_ctrlATT0);
    DDX_Control(pDX, IDC_SLIDER_ATT1, m_ctrlATT1);
    DDX_Control(pDX, IDC_SLIDER_ATT2, m_ctrlATT2);
}


BEGIN_MESSAGE_MAP(CShadowLightPage, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_DIRECTION_LIGHT, OnBnClickedCheckDirectionLight)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_CHECK_RANGE_REND, OnBnClickedCheckRangeRend)
	ON_LBN_SELCHANGE(IDC_LIST_LIGHT, OnLbnSelchangeListLight)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT0, &CShadowLightPage::OnNMCustomdrawSliderAtt0)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT1, &CShadowLightPage::OnNMCustomdrawSliderAtt1)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT2, &CShadowLightPage::OnNMCustomdrawSliderAtt2)
    ON_EN_CHANGE(IDC_EDIT_RANGE, &CShadowLightPage::OnEnChangeEditRange)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CShadowLightPage::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CShadowLightPage::OnBnClickedButtonDown)
	ON_EN_CHANGE(IDC_EDIT_LIGHTNAME, &CShadowLightPage::OnEnChangeEditLightname)
	ON_EN_CHANGE(IDC_EDIT_THETA, &CShadowLightPage::OnEnChangeEditTheta)
	ON_EN_CHANGE(IDC_EDIT_PHI, &CShadowLightPage::OnEnChangeEditPhi)
	ON_EN_CHANGE(IDC_EDIT_FALL_OFF, &CShadowLightPage::OnEnChangeEditFallOff)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_INSERT, OnBnClickedButtonColorInsert)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_STATE, OnBnClickedButtonColorState)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_DELETE, OnBnClickedButtonColorDelete)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_TIME_APPLY, OnBnClickedButtonColorTimeApply)
	ON_NOTIFY(NM_CLICK, IDC_LIST_COLOR, OnNMDblclkListColor)
	ON_BN_CLICKED(IDC_CHECK_FLASH, OnBnClickedCheckFlash)	
	ON_BN_CLICKED(IDC_BUTTON_SIN_APPLY, OnBnClickedButtonSinApply)
	ON_EN_CHANGE(IDC_EDIT_PL_END_LINE_LIMIT, &CShadowLightPage::OnEnChangeEditPLEndLineLimit)
	ON_EN_CHANGE(IDC_EDIT_OVER_LIGHTING, &CShadowLightPage::OnEnChangeEditOverLighting)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTION_LIGHT_DOCUMENT, &CShadowLightPage::OnBnClickedButtonDirectionLightDocument)
END_MESSAGE_MAP()

BOOL CShadowLightPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_wndList.SetColumnHeader(_T("Time, 50 ; Color, 200"));
	m_wndList.SetGridLines(TRUE);
	m_wndList.SortItems(0,FALSE);

	ListingLights ();

    m_ctrlATT0.SetRange( 0, 100 );
	m_ctrlATT1.SetRange( 0, 100 );
	m_ctrlATT2.SetRange( 0, 100 );

	m_ctrlATT0.SetPos( 100 );
	m_ctrlATT1.SetPos( 0 );
	m_ctrlATT2.SetPos( 0 );

	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CShadowLightPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetSetLandMan()->ActiveMap ();

	ListingLights ();

	CButton *pButton;
	pButton = (CButton*) GetDlgItem(IDC_CHECK_DIRECTION_LIGHT);
	pButton->SetCheck ( pView->GetSetLandMan()->GetShadowLightData()->m_bDirectionLight );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_RANGE_REND);
	pButton->SetCheck ( NSShadowLight::g_bRangeRender );

	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_EDIT_THETA);
	pWnd->SetWindowText( _T("45") );

	pWnd = GetDlgItem(IDC_EDIT_PHI);
	pWnd->SetWindowText( _T("90") );

	pWnd = GetDlgItem(IDC_EDIT_FALL_OFF);
	pWnd->SetWindowText( _T("1.0") );

	pWnd = GetDlgItem(IDC_EDIT_COLOR_TIME);
	pWnd->SetWindowText( _T("1.0") );

	pWnd = GetDlgItem(IDC_EDIT_SIN_CYCLE_TIME);
	pWnd->SetWindowText( _T("1.0") );

	pWnd = GetDlgItem(IDC_EDIT_SIN_POWER);
	pWnd->SetWindowText( _T("0.0") );

	pWnd = GetDlgItem(IDC_EDIT_SIN_ADD);
	pWnd->SetWindowText( _T("1.0") );

	pWnd = GetDlgItem(IDC_EDIT_PL_END_LINE_LIMIT);
	pWnd->SetWindowText( _T("1.0") );

	pWnd = GetDlgItem(IDC_EDIT_OVER_LIGHTING);
	pWnd->SetWindowText( _T("2.0") );

	return CPropertyPage::OnSetActive();
}

void CShadowLightPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	// vector 이기에 뒤쪽에서부터 지워준다.
	for( int nIndex=m_ListLight.GetCount()-1; nIndex>=0; --nIndex )
	{
		int nCheck = m_ListLight.GetSel( nIndex );

		if( nCheck > 0 )
		{
			CString strText;
			m_ListLight.GetText( nIndex, strText );

			CString strMsg;
			strMsg.Format ( "'%s' 를 삭제 하시겠습니까? 삭제후 되살릴수 없습니다.", strText.GetString() );

			if ( IDYES == MessageBox ( strMsg, "주의", MB_YESNO|MB_ICONQUESTION ) )
			{
				pView->GetSetLandMan()->GetShadowLightData_NoneConst()->Delete( nIndex );
				NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
                //WorldStateMachine::EditAsset(Text.GetString());
			}
		}
	}

	// 리스트에 등록
	ListingLights();
}

void CShadowLightPage::ModifyLightData ( const DxShadowSpotLight* pSpotLight )
{
	GetDlgItem(IDC_EDIT_LIGHTNAME)->SetWindowText( pSpotLight->m_strName.c_str() );

	char szValue[256];

	_gcvt ( pSpotLight->m_sLight.Theta, 7, szValue );
	GetDlgItem(IDC_EDIT_THETA)->SetWindowText( szValue );

	_gcvt ( pSpotLight->m_sLight.Phi, 7, szValue );
	GetDlgItem(IDC_EDIT_PHI)->SetWindowText( szValue );

	_gcvt ( pSpotLight->m_sLight.Falloff, 7, szValue );
	GetDlgItem(IDC_EDIT_FALL_OFF)->SetWindowText( szValue );

	_gcvt ( pSpotLight->m_sLight.Range, 7, szValue );
	GetDlgItem(IDC_EDIT_RANGE)->SetWindowText(szValue);

    m_ctrlATT0.SetPos( pSpotLight->m_nAttenuation0 );
	m_ctrlATT1.SetPos( pSpotLight->m_nAttenuation1 );
	m_ctrlATT2.SetPos( pSpotLight->m_nAttenuation2 );

    // 수치값 확인
    SetEndLineValue( &pSpotLight->m_sLight );
}

//	Note : 새로운 Light 생성.
void CShadowLightPage::OnBnClickedButtonCreate()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(TRUE);

	//	Note : 새로운 LIGHT 를 초기화 시킨다.
	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLight = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
	spShadowSpotLight->SetDefault( vCollPos );
	spShadowSpotLight->InsertColor( 255, 255, 255 );

	// 선택되었다는 것을 없애준다.
	// 이 코드가 없으면 ModifyLightData 에서 선택된 Light 들에게 Default 값이 셋팅된다.
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		DxShadowSpotLight* pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		pLightCur->m_bEditCheck = FALSE;
	}

	//	Note : 새로운 라이트 추가.
	pView->GetSetLandMan()->GetShadowLightData_NoneConst()->Insert( spShadowSpotLight );
	NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );

	// 리스트에 등록
	ListingLights();

	// Note : 위치 정리
	int nSize = m_ListLight.GetCount();
	if( nSize!=LB_ERR )
	{
		m_ListLight.SetFocus();
		m_ListLight.SetSel( nSize-1, TRUE );
	}

	// Note : 다이얼 로그..셋팅
	ResetDialog( spShadowSpotLight.get() );

	// Note : 생성이 되면 선택된 것과 같은 느낌이 나야함.
	OnLbnSelchangeListLight();
    //WorldStateMachine::EditAsset(pThisLight->m_szLightName);
}

//	Note : 모든 Light들의 Enable/Disable 제어.
//
void CShadowLightPage::OnBnClickedCheckDirectionLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CButton *pButton = (CButton*) GetDlgItem( IDC_CHECK_DIRECTION_LIGHT );
	if ( pButton->GetCheck() )
	{
		pView->GetSetLandMan()->GetShadowLightData_NoneConst()->m_bDirectionLight = TRUE;
	}
	else
	{
		pView->GetSetLandMan()->GetShadowLightData_NoneConst()->m_bDirectionLight = FALSE;
	}
}

void CShadowLightPage::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode 에서는 셋팅이 불가능 합니다.") );
		return;
	}

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	
		return;

	D3DCOLORVALUE &d3dColor = pLight->m_sLight.Diffuse;
	COLORREF Color = RGB ( int(d3dColor.r*255.0f), int(d3dColor.g*255.0f), int(d3dColor.b*255.0f) );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )
		{
			pLightCur->m_sLight.Diffuse	= d3dColor;
		}
	}
}

void CShadowLightPage::UpdateKeyBoardMouse()
{
	if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
	{
		// Light를 이동하는 경우가 있기 때문에 Tree 를 다시 만들어준다.
		//UpdatePageClickUP();

		// Light를 회전할 수가 있기 때문에 Direction 을 알맞게 변경해준다.
		UpdateSpotLightDirection();
	}
	//if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWN )
	//{
	//	// Clone 될 때 List를 새로 생성한다.
	//	UpdatePageClickDOWN();
	//}
}

BOOL CShadowLightPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();

	// 선택되었다는 것을 없애준다.
	// 이 코드가 없으면 OnSetActive -> OnEnChangeEditTheta 이런식으로 디폴트 값이 셋팅되는 문제가 생길 수 있다.
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		DxShadowSpotLight* pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		pLightCur->m_bEditCheck = FALSE;
	}

	return CPropertyPage::OnKillActive();
}

void CShadowLightPage::OnBnClickedCheckRangeRend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem(IDC_CHECK_RANGE_REND);
	if ( pButton->GetCheck() )
	{
		NSShadowLight::g_bRangeRender = TRUE;
	}
	else
	{
		NSShadowLight::g_bRangeRender = FALSE;
	}
}

void CShadowLightPage::OnLbnSelchangeListLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		if( m_ListLight.GetSel(i) > 0 )	pLightCur->m_bEditCheck = TRUE;
		else							pLightCur->m_bEditCheck = FALSE;
	}

	// Note : Dialog Setting
	ResetDialog();

	// Note : 2개 이상의 중복일 경우 이동/회전/스케일 은 없다.
	if( m_ListLight.GetSelCount() > 1 )	
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView();
		pView->DeActiveEditMatrix();
	}

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	char szValue[256];
	_gcvt ( pLight->m_fSinCycleTime, 7, szValue );
	GetDlgItem(IDC_EDIT_SIN_CYCLE_TIME)->SetWindowText( szValue );

	_gcvt ( pLight->m_fSinPower, 7, szValue );
	GetDlgItem(IDC_EDIT_SIN_POWER)->SetWindowText( szValue );

	_gcvt ( pLight->m_fSinAdd, 7, szValue );
	GetDlgItem(IDC_EDIT_SIN_ADD)->SetWindowText( szValue );

	_gcvt ( pLight->m_fAttEndLineValue, 7, szValue );
	GetDlgItem(IDC_EDIT_PL_END_LINE_LIMIT)->SetWindowText( szValue );

	_gcvt ( pLight->m_fOverLighting, 7, szValue );
	GetDlgItem(IDC_EDIT_OVER_LIGHTING)->SetWindowText( szValue );

	UpdateColorList();
}

void CShadowLightPage::ListingLights()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	m_ListLight.ResetContent ();

	const ShadowLightData* pShadowSpotLight = pView->GetSetLandMan()->GetShadowLightData();
	const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>> vecShadowSpotLight = pShadowSpotLight->m_vecShadowSpotLight;

	for ( DWORD i=0; i<vecShadowSpotLight.size(); ++i )
	{
		std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLight = vecShadowSpotLight[i];

		TSTRINGSTREAM sStream;
		sStream << "[" << i << "] " << spShadowSpotLight->m_strName;

		DWORD nIndex = m_ListLight.AddString ( sStream.str().c_str() );
		m_ListLight.SetItemData ( nIndex, reinterpret_cast<DWORD>( spShadowSpotLight.get() ) );
	}
}

DxShadowSpotLight* CShadowLightPage::GetCurLight()
{
	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )
		{
			return pLightCur;
		}
	}

	return NULL;
}

void CShadowLightPage::ResetDialog()
{
	DxShadowSpotLight* pLightCur;

    // Note : 선택된 것이 1개일 경우
	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 ) return;

		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(nIndex) );

		// Note : 데이터에 맞게 재 설정한다.
		ResetDialog( pLightCur );

		return;
	}

	// Note : 선택된 것이 2개일 경우
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )
		{
			ResetDialog( pLightCur );	// 대표 한넘으로 본다.
			return;
		}
	}
}

void CShadowLightPage::ResetDialog( DxShadowSpotLight* pSpotLight )
{
	if ( pSpotLight )
	{
		ModifyLightData ( pSpotLight );

		D3DXVECTOR3 vPos( pSpotLight->m_matWorld._41, pSpotLight->m_matWorld._42, pSpotLight->m_matWorld._43 );
		DxViewPort::GetInstance().CameraJump( vPos );

		//	Note : 라이트의 위치 편집을 활성화 한다.
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->ActiveSpotLight( &pSpotLight->m_matWorld );
	}
	else
	{
		// Note : 커서 없애기
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}
}

//void CShadowLightPage::UpdatePageClickDOWN()
//{
//	if( !m_bDlgInit )	return;
//
//	// List 업데이트
//	ListingLights();
//
//	// Note : 위치 정리
//	int nSize = m_ListLight.GetCount();
//	if( nSize > 2 )		m_ListLight.SetSel( nSize-1, TRUE );
//	else				m_ListLight.SetSel( 0, TRUE );
//
//	// Note : 다이얼 로그..셋팅
//	DXLIGHT* pLight = GetCurLight();
//	if( !pLight )	return;
//	ResetDialog( pLight->m_szLightName );
//}
//
//void CShadowLightPage::UpdatePageClickUP()
//{
//	if( !m_bDlgInit )	return;
//
//	DxLightMan::GetInstance()->MakeAABBTree();
//}

void CShadowLightPage::OnNMCustomdrawSliderAtt0(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation0			= m_ctrlATT0.GetPos();
			pLightCur->m_sLight.Attenuation0	= m_ctrlATT0.GetPos() * 0.01f;

            SetEndLineValue( &pLightCur->m_sLight );
		}
	}
}

void CShadowLightPage::OnNMCustomdrawSliderAtt1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation1			= m_ctrlATT1.GetPos();
			pLightCur->m_sLight.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_sLight.Range * 0.5f );

            SetEndLineValue( &pLightCur->m_sLight );
		}
	}
}

void CShadowLightPage::OnNMCustomdrawSliderAtt2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation2			= m_ctrlATT2.GetPos();
			pLightCur->m_sLight.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_sLight.Range * pLightCur->m_sLight.Range * 0.5f );

            SetEndLineValue( &pLightCur->m_sLight );
		}
	}
}

void CShadowLightPage::SetEndLineValue( const D3DLIGHTQ* pLight )
{
    float fEndLineValue = 1.f / ( pLight->Attenuation0 + 
                                  (pLight->Attenuation1 * pLight->Range) + 
                                  (pLight->Attenuation2 * pLight->Range * pLight->Range) );

    char szValue[256];
    CWnd* pWnd = GetDlgItem( IDC_EDIT_PL_END_LINE );
    _gcvt ( fEndLineValue, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CShadowLightPage::OnEnChangeEditRange()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
    // ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
    // 해당 알림 메시지를 보냅니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

    DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

    float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_RANGE);
	pWnd->GetWindowText( strText );
    fRange = (float) atof ( strText.GetString() );

	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )	
		{
			pLightCur->m_sLight.Range			= fRange;
	        pLightCur->m_sLight.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_sLight.Range * 0.5f );
            pLightCur->m_sLight.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_sLight.Range * pLightCur->m_sLight.Range * 0.5f );
		}
	}
}

void CShadowLightPage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 )
			return;

		if ( pView->GetSetLandMan()->GetShadowLightData_NoneConst()->LightItemUp( nIndex ) )
		{
			// 리스트에 등록
			ListingLights();

			m_ListLight.SetSel( nIndex-1, TRUE );

			NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
		}
	}
}

void CShadowLightPage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 )
			return;

		if ( pView->GetSetLandMan()->GetShadowLightData_NoneConst()->LightItemDown( nIndex ) )
		{
			// 리스트에 등록
			ListingLights();

			m_ListLight.SetSel( nIndex+1, TRUE );

			NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
		}
	}
}

// Light를 회전할 수가 있기 때문에 Direction 을 알맞게 변경해준다.
//	위치값도 정상적으로 셋팅해준다.
void CShadowLightPage::UpdateSpotLightDirection()
{
	DxShadowSpotLight* pSpotLight = GetCurLight();
	if( !pSpotLight )	
		return;

	D3DXVECTOR3 vDirection = D3DXVECTOR3( 0.f, -1.f, 0.01f );
	D3DXVec3TransformNormal( &vDirection, &vDirection, &pSpotLight->m_matWorld );
	D3DXVec3Normalize( &vDirection, &vDirection );

	pSpotLight->m_sLight.Direction.x = vDirection.x;
	pSpotLight->m_sLight.Direction.y = vDirection.y;
	pSpotLight->m_sLight.Direction.z = vDirection.z;

	D3DXVECTOR3 vUpVec(0.f,1.f,0.f);
	D3DXVECTOR3 vFromPt( pSpotLight->m_matWorld._41, pSpotLight->m_matWorld._42, pSpotLight->m_matWorld._43 );
	D3DXVECTOR3 vLookatPt( vFromPt );
	vLookatPt.x += pSpotLight->m_sLight.Direction.x;
	vLookatPt.y += pSpotLight->m_sLight.Direction.y;
	vLookatPt.z += pSpotLight->m_sLight.Direction.z;

	D3DXMatrixLookAtLH ( &pSpotLight->m_matCamera, &vFromPt, &vLookatPt, &vUpVec );

	pSpotLight->m_sLight.Position.x = pSpotLight->m_matWorld._41;
	pSpotLight->m_sLight.Position.y = pSpotLight->m_matWorld._42;
	pSpotLight->m_sLight.Position.z = pSpotLight->m_matWorld._43;
}

// 새로생성된 것으로 인한 List 재 정리
void CShadowLightPage::UpdatePage_List_AddLastItem()
{
	// 리스트에 등록
	ListingLights();

	m_ListLight.SetSel( m_ListLight.GetCount()-1, TRUE );

	// Note : Dialog Setting
	ResetDialog();
}

void CShadowLightPage::OnEnChangeEditLightname()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DWORD nIndex = m_ListLight.GetCurSel ();
	if ( nIndex==-1 )
		return;

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_LIGHTNAME);
	pWnd->GetWindowText( strText );

	pLight->m_strName = strText.GetString();

	// 리스트에 등록
	ListingLights();

	m_ListLight.SetSel( nIndex, TRUE );
}

void CShadowLightPage::OnEnChangeEditTheta()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_THETA);
	pWnd->GetWindowText( strText );
	pLight->m_sLight.Theta = (float) atof ( strText.GetString() );
}

void CShadowLightPage::OnEnChangeEditPhi()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_PHI);
	pWnd->GetWindowText( strText );
	pLight->m_sLight.Phi = (float) atof ( strText.GetString() );
}

void CShadowLightPage::OnEnChangeEditFallOff()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_FALL_OFF);
	pWnd->GetWindowText( strText );
	pLight->m_sLight.Falloff = (float) atof ( strText.GetString() );
}

void CShadowLightPage::OnBnClickedButtonColorInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	int ThisR = 255;
	int ThisG = 255;
	int ThisB = 255;

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pLight->InsertColor( GetRValue(SelectColor), GetGValue(SelectColor), GetBValue(SelectColor) );

		UpdateColorList();
	}
}

void CShadowLightPage::OnBnClickedButtonColorState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	
		return;

	int ThisR = pLight->m_vecColor[nIndex].m_dwR;
	int ThisG = pLight->m_vecColor[nIndex].m_dwG;
	int ThisB = pLight->m_vecColor[nIndex].m_dwB;

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pLight->ChageColor( nIndex, GetRValue(SelectColor), GetGValue(SelectColor), GetBValue(SelectColor) );

		UpdateColorList();
	}
}

void CShadowLightPage::OnBnClickedButtonColorDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	
		return;

	pLight->DeleteColor( nIndex );

	UpdateColorList();
}

void CShadowLightPage::OnBnClickedButtonColorTimeApply()
{
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	
		return;

	float fTime(0.f);
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_COLOR_TIME);
	pWnd->GetWindowText( strText );
	fTime = (float) atof ( strText.GetString() );
	pLight->SetColorTime( nIndex, fTime );

	UpdateColorList();
}

void CShadowLightPage::OnNMDblclkListColor(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	
		return;

	char szValue[256];
	_gcvt ( pLight->m_vecColor[nIndex].m_fTime, 7, szValue );
	GetDlgItem(IDC_EDIT_COLOR_TIME)->SetWindowText( szValue );
	((CButton*)GetDlgItem(IDC_CHECK_FLASH))->SetCheck( pLight->m_vecColor[nIndex].m_bFlash );
}

void CShadowLightPage::UpdateColorList()
{
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	m_wndList.DeleteAllItems();

	char szValue[256];
	for( DWORD i=0; i<pLight->m_vecColor.size(); ++i )
	{
		const int IDX = m_wndList.InsertItem(i, _T(""));

		_gcvt ( pLight->m_vecColor[i].m_fTime, 7, szValue );
		m_wndList.SetItemText( IDX, 0, szValue );

		m_wndList.SetItemText( IDX, 1, _T("") );

		int ThisR = pLight->m_vecColor[i].m_dwR;
		int ThisG = pLight->m_vecColor[i].m_dwG;
		int ThisB = pLight->m_vecColor[i].m_dwB;

		m_wndList.SetItemBkColor( i, -1, RGB(ThisR, ThisG, ThisB) );
	}
}

void CShadowLightPage::OnBnClickedCheckFlash()
{
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	
		return;

	CString strText;
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_FLASH);
	BOOL bCheck = pButton->GetCheck();
	pLight->SetFlash( nIndex, bCheck );

	UpdateColorList();
}

void CShadowLightPage::OnBnClickedButtonSinApply()
{
	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )
		return;

	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_SIN_CYCLE_TIME);
	pWnd->GetWindowText( strText );
	pLight->m_fSinCycleTime = (float) atof ( strText.GetString() );

	pWnd = (CWnd*) GetDlgItem(IDC_EDIT_SIN_POWER);
	pWnd->GetWindowText( strText );
	pLight->m_fSinPower = (float) atof ( strText.GetString() );

	pWnd = (CWnd*) GetDlgItem(IDC_EDIT_SIN_ADD);
	pWnd->GetWindowText( strText );
	pLight->m_fSinAdd = (float) atof ( strText.GetString() );
}

void CShadowLightPage::OnEnChangeEditPLEndLineLimit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_PL_END_LINE_LIMIT);
	pWnd->GetWindowText( strText );
	pLight->m_fAttEndLineValue = (float) atof ( strText.GetString() );
}

void CShadowLightPage::OnEnChangeEditOverLighting()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_OVER_LIGHTING);
	pWnd->GetWindowText( strText );
	pLight->m_fOverLighting = (float) atof ( strText.GetString() );
	if ( pLight->m_fOverLighting > 3.f )
	{
		// 3 을 초과 할 수는 없다.
		pWnd->SetWindowText( "3.0" );
		pLight->m_fOverLighting = 3.f;
	}
}

void CShadowLightPage::OnBnClickedButtonDirectionLightDocument()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"실내맵을 경우 Direction Light 체크박스를 비 활성화 해야 한다.. \r\n"
		"또한 LIGHT EDIT (빛) 셋팅 시 DIRECTIONLIGHT-[NOON] 의 Diffuse 값은 Zero로 셋팅 하고, Ambient 값으로 약간의 조명을 넣고, Point Light 및 Spot Light 로 주 조명을 셋팅하도록 한다.\r\n"
		;

	AfxMessageBox( strText.c_str() );
}
