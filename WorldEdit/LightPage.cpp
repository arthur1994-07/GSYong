// LightPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "LightPage.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/G-Logic/GLPeriod.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include ".\lightpage.h"
#include "State/WorldStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLightPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightPage, CPropertyPage)
CLightPage::CLightPage()
	: CPropertyPage(CLightPage::IDD)
	, m_bDlgInit(FALSE)
    , m_fAttMax0(0.f)
    , m_fAttMax1(0.f)
    , m_fAttMax2(0.f)
{
}

CLightPage::~CLightPage()
{
}

void CLightPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIGHTEDITSEP, m_ctrLightEditSep);
    DDX_Control(pDX, IDC_LIST_LIGHT, m_ListLight);
    DDX_Control(pDX, IDC_COMBO_LIGHTTYPE, m_ComboLightType);
    DDX_Control(pDX, IDC_SLIDER_ATT0, m_ctrlATT0);
    DDX_Control(pDX, IDC_SLIDER_ATT1, m_ctrlATT1);
    DDX_Control(pDX, IDC_SLIDER_ATT2, m_ctrlATT2);
}


BEGIN_MESSAGE_MAP(CLightPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_CHECK_LIGHTINGENABLE, OnBnClickedCheckLightingenable)
	ON_BN_CLICKED(IDC_CHECK_ENABLELIGHT, OnBnClickedCheckEnablelight)
	ON_CBN_SELCHANGE(IDC_COMBO_LIGHTTYPE, OnCbnSelchangeComboLighttype)
	ON_BN_CLICKED(IDC_BUTTON_DIR, OnBnClickedButtonDir)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_AMBIENT, OnBnClickedButtonAmbient)
	ON_BN_CLICKED(IDC_BUTTON_SPECULAR, OnBnClickedButtonSpecular)
	ON_BN_CLICKED(IDC_CHECK_RANGE_REND, OnBnClickedCheckRangeRend)
	ON_BN_CLICKED(IDC_CHECK_NIGHTANDDAY, OnBnClickedCheckNightandday)
	ON_BN_CLICKED(IDC_BUTTON_TODAY, OnBnClickedButtonToday)
	ON_BN_CLICKED(IDC_BUTTON_TONIGHT, OnBnClickedButtonTonight)
	ON_BN_CLICKED(IDC_CHECK_DAYPOINTOFF, OnBnClickedCheckDaypointoff)
	ON_BN_CLICKED(IDC_BUTTON_TIMESET, OnBnClickedButtonTimeset)
	ON_LBN_SELCHANGE(IDC_LIST_LIGHT, OnLbnSelchangeListLight)
	ON_BN_CLICKED(IDC_CHECK_DSP_NAME, OnBnClickedCheckDspName)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT0, &CLightPage::OnNMCustomdrawSliderAtt0)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT1, &CLightPage::OnNMCustomdrawSliderAtt1)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ATT2, &CLightPage::OnNMCustomdrawSliderAtt2)
    ON_EN_CHANGE(IDC_EDIT_RANGE, &CLightPage::OnEnChangeEditRange)
    ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_LSC, &CLightPage::OnBnClickedButtonDiffuseLsc)
    ON_BN_CLICKED(IDC_BUTTON_AMBIENT_LSC, &CLightPage::OnBnClickedButtonAmbientLsc)
    ON_EN_CHANGE(IDC_EDIT_LM_DDP, &CLightPage::OnEnChangeEditLmDdp)
    ON_BN_CLICKED(IDC_CHECK_ENABLE_PL_NIGHT, &CLightPage::OnBnClickedCheckEnablePlNight)
	ON_EN_CHANGE(IDC_EDIT_OVER_LIGHTING, &CLightPage::OnEnChangeEditOverLighting)
END_MESSAGE_MAP()

void CLightPage::SetLightType ( D3DLIGHTTYPE Type )
{
	switch ( Type )
	{
	case D3DLIGHT_POINT:
		{
			SetLightType ( D3DLIGHT_FORCE_DWORD );

			SetWin_Combo_Sel ( this, IDC_COMBO_LIGHTTYPE, "POINT" );

			GetDlgItem(IDC_LIGHTEDITSEP)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_COMBO_LIGHTTYPE)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_EDIT_LIGHTNAME)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_BUTTON_RANGE)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_EDIT_RANGE)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_BUTTON_DIFFUSE)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DG)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DB)->ShowWindow(SW_SHOWNORMAL);

			//GetDlgItem(IDC_BUTTON_SPECULAR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SG)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SB)->ShowWindow(SW_SHOWNORMAL);

            GetDlgItem(IDC_STATIC_ATT)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_SLIDER_ATT0)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_SLIDER_ATT1)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_SLIDER_ATT2)->ShowWindow(SW_SHOWNORMAL);

            GetDlgItem(IDC_STATIC_END_LINE)->ShowWindow(SW_SHOWNORMAL);
	        GetDlgItem(IDC_EDIT_PL_END_LINE)->ShowWindow(SW_SHOWNORMAL);

            GetDlgItem(IDC_STATIC_ENABLE_PL)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_CHECK_ENABLELIGHT)->ShowWindow(SW_SHOWNORMAL);
            GetDlgItem(IDC_CHECK_ENABLE_PL_NIGHT)->ShowWindow(SW_SHOWNORMAL);
		}
		break;

	case D3DLIGHT_SPOT:
		{
			SetLightType ( D3DLIGHT_FORCE_DWORD );

			SetWin_Combo_Sel ( this, IDC_COMBO_LIGHTTYPE, "SPOT" );

			GetDlgItem(IDC_LIGHTEDITSEP)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_COMBO_LIGHTTYPE)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_EDIT_LIGHTNAME)->ShowWindow(SW_HIDE);
		}
		break;

	case D3DLIGHT_DIRECTIONAL:
		{
			SetLightType ( D3DLIGHT_FORCE_DWORD );

			SetWin_Combo_Sel ( this, IDC_COMBO_LIGHTTYPE, "DIRECTIONAL" );

			GetDlgItem(IDC_LIGHTEDITSEP)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_COMBO_LIGHTTYPE)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_EDIT_LIGHTNAME)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_BUTTON_DIR)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_EDIT_DIRX)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_EDIT_DIRY)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_EDIT_DIRZ)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_BUTTON_DIFFUSE)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DG)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_DB)->ShowWindow(SW_SHOWNORMAL);

			GetDlgItem(IDC_BUTTON_AMBIENT)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_AR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_AG)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_AB)->ShowWindow(SW_SHOWNORMAL);

			//GetDlgItem(IDC_BUTTON_SPECULAR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SR)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SG)->ShowWindow(SW_SHOWNORMAL);
			//GetDlgItem(IDC_EDIT_SB)->ShowWindow(SW_SHOWNORMAL);

            GetDlgItem(IDC_STATIC_ENABLE_PL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CHECK_ENABLELIGHT)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_CHECK_ENABLE_PL_NIGHT)->ShowWindow(SW_HIDE);
		}
		break;

	case (0xFFFF):
		{
			SetLightType ( D3DLIGHT_FORCE_DWORD );

			GetDlgItem(IDC_LIGHTEDITSEP)->ShowWindow(SW_SHOWNORMAL);
			GetDlgItem(IDC_COMBO_LIGHTTYPE)->ShowWindow(SW_SHOWNORMAL);
		}
		break;

	case D3DLIGHT_FORCE_DWORD:
		{
			GetDlgItem(IDC_LIGHTEDITSEP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMBO_LIGHTTYPE)->ShowWindow(SW_HIDE);
			
			GetDlgItem(IDC_EDIT_LIGHTNAME)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BUTTON_DIR)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DIRX)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DIRY)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DIRZ)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BUTTON_RANGE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_RANGE)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BUTTON_DIFFUSE)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_DR)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_DG)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_DB)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BUTTON_AMBIENT)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_AR)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_AG)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_AB)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BUTTON_SPECULAR)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_SR)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_SG)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_SB)->ShowWindow(SW_HIDE);

            GetDlgItem(IDC_STATIC_ATT)->ShowWindow(SW_HIDE);
	        GetDlgItem(IDC_SLIDER_ATT0)->ShowWindow(SW_HIDE);
	        GetDlgItem(IDC_SLIDER_ATT1)->ShowWindow(SW_HIDE);
	        GetDlgItem(IDC_SLIDER_ATT2)->ShowWindow(SW_HIDE);

            GetDlgItem(IDC_STATIC_END_LINE)->ShowWindow(SW_HIDE);
	        GetDlgItem(IDC_EDIT_PL_END_LINE)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_STATIC_ENABLE_PL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CHECK_ENABLELIGHT)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_CHECK_ENABLE_PL_NIGHT)->ShowWindow(SW_HIDE);
		}
		break;
	};
}

BOOL CLightPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	DWORD nIndex;
	nIndex = m_ComboLightType.AddString ( "POINT" );
	m_ComboLightType.SetItemData ( nIndex, D3DLIGHT_POINT );

	nIndex = m_ComboLightType.AddString ( "SPOT" );
	m_ComboLightType.SetItemData ( nIndex, D3DLIGHT_SPOT );

	nIndex = m_ComboLightType.AddString ( "DIRECTIONAL" );
	m_ComboLightType.SetItemData ( nIndex, D3DLIGHT_DIRECTIONAL );

	nIndex = m_ComboLightType.FindString ( 0, "POINT" );
	m_ComboLightType.SetCurSel ( nIndex );


	ListingLights ();

	CButton *pButton;
	pButton = (CButton*) GetDlgItem(IDC_CHECK_LIGHTINGENABLE);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsEnableLighting () );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_RANGE_REND);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsEnableRangeRend () );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_NIGHTANDDAY);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsNightAndDay() );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_DAYPOINTOFF);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsWhenDayPointOff() );

    // WorldEdit상에서는 디폴트값을 0으로 맞춘다.
    GLPeriod::GetInstance().SetTIMETOVBR( 0.f );
	SetWin_Num_float ( this, IDC_EDIT_TIME_VBR, 0 );
	GLPeriod::GetInstance().SetHour ( DxLightMan::GetInstance()->GetForceStopTime() );
	SetWin_Num_int ( this, IDC_EDIT_TIME, DxLightMan::GetInstance()->GetForceStopTime() );

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

void CLightPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	ListingLights ();

	CButton *pButton;
	pButton = (CButton*) GetDlgItem(IDC_CHECK_LIGHTINGENABLE);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsEnableLighting () );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_RANGE_REND);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsEnableRangeRend () );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_NIGHTANDDAY);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsNightAndDay() );

	pButton = (CButton*) GetDlgItem(IDC_CHECK_DAYPOINTOFF);
	pButton->SetCheck ( DxLightMan::GetInstance()->IsWhenDayPointOff() );

	SetWin_Num_float ( this, IDC_EDIT_TIME_VBR, GLPeriod::GetInstance().GetTIMETOVBR() );
	SetWin_Num_int ( this, IDC_EDIT_TIME, DxLightMan::GetInstance()->GetForceStopTime() );

	SetLightType ( D3DLIGHT_FORCE_DWORD );
}

void CLightPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	
	for( int nIndex=0; nIndex<m_ListLight.GetCount(); ++nIndex )
	{
		int nCheck = m_ListLight.GetSel( nIndex );

		if( nCheck > 0 )
		{
			CString Text;
			m_ListLight.GetText ( nIndex, Text );

			if ( Text == "DIRECTIONAL-[NOON]" || Text == "DIRECTIONAL-[NIGHT]" )
			{
				MessageBox ( "'DIRECTIONAL Light'는 삭제 할수 없습니다.", "ERROR", MB_OK|MB_ICONERROR );
				return;
			}

			CString strMsg;
			strMsg.Format ( "'%s' 를 삭제 하시겠습니까? 삭제후 되살릴수 없습니다.", Text.GetString() );

			if ( IDYES == MessageBox ( strMsg, "주의", MB_YESNO|MB_ICONQUESTION ) )
			{
				DXLIGHT* pLight = (DXLIGHT*) m_ListLight.GetItemData ( nIndex );
				DxLightMan::GetInstance()->DelLight( Text.GetString () );
				DxLightMan::GetInstance()->MakeAABBTree();
                WorldStateMachine::EditAsset(Text.GetString());
			}
		}
	}
    
	UpdatePageClickDOWN();
}

void CLightPage::ModifyLightData ( const DXLIGHT &light )
{
	GetDlgItem(IDC_EDIT_LIGHTNAME)->SetWindowText(light.m_szLightName);

	char szValue[256];
	_gcvt ( light.m_Light.Direction.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
	_gcvt ( light.m_Light.Direction.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
	_gcvt ( light.m_Light.Direction.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

	_gcvt ( light.m_Light.Range, 7, szValue );
	GetDlgItem(IDC_EDIT_RANGE)->SetWindowText(szValue);

	_gcvt ( light.m_fOverLighting, 7, szValue );
	GetDlgItem(IDC_EDIT_OVER_LIGHTING)->SetWindowText( szValue );

    m_ctrlATT0.SetPos( light.m_nAttenuation0 );
	m_ctrlATT1.SetPos( light.m_nAttenuation1 );
	m_ctrlATT2.SetPos( light.m_nAttenuation2 );

    // 수치값 확인
    SetEndLineValue( &light );

    // PointLight 활성화 설정.
	{
		CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLELIGHT);
		pButton->SetCheck ( light.m_bEnablePL_Day );

		pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLE_PL_NIGHT);
		pButton->SetCheck ( light.m_bEnablePL_Night );
	}
}

//	Note : 새로운 Light 생성.
void CLightPage::OnBnClickedButtonCreate()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_COMBO_LIGHTTYPE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(TRUE);

	SetLightType ( D3DLIGHT_POINT );
	int nIndex = m_ComboLightType.FindString ( 0, "POINT" );
	m_ComboLightType.SetCurSel ( nIndex );

	//	Note : 새로운 LIGHT 를 초기화 시킨다.
	DXLIGHT* pThisLight = new DXLIGHT;
	pThisLight->SetDefault ();
	pThisLight->m_Light.Type = D3DLIGHT_POINT;
	pThisLight->m_Light.Position = vCollPos;
	pThisLight->m_Light.Ambient.r = 0.f;
	pThisLight->m_Light.Ambient.g = 0.f;
	pThisLight->m_Light.Ambient.b = 0.f;
	pThisLight->m_Light.Ambient.a = 0.f;
	//pThisLight->m_Light.Range			= pThisLight->m_Light.Range;
	//pThisLight->m_Light.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pThisLight->m_Light.Range * 0.5f );
	//pThisLight->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pThisLight->m_Light.Range * pThisLight->m_Light.Range * 0.5f );
	pThisLight->m_Light.Attenuation0 = 1.f;
	pThisLight->m_Light.Attenuation1 = 1.0f/pThisLight->m_Light.Range;
	pThisLight->m_matWorld._41 = vCollPos.x;
	pThisLight->m_matWorld._42 = vCollPos.y;
	pThisLight->m_matWorld._43 = vCollPos.z;

	char szNum[256];
	
	DWORD dwNum = 0;
	while (1)
	{
		if ( dwNum >= 1000 )	sprintf ( szNum, "[%d]", dwNum++ );
		else					sprintf ( szNum, "[%03d]", dwNum++ );

		StringCchCopy( pThisLight->m_szLightName, 256, "UNNAMED" );
		StringCchCat( pThisLight->m_szLightName, 256, szNum );

		if ( !DxLightMan::GetInstance()->GetLight ( pThisLight->m_szLightName ) )
			break;
	}

	// 선택되었다는 것을 없애준다.
	// 이 코드가 없으면 ModifyLightData 에서 선택된 Light 들에게 Default 값이 셋팅된다.
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		DXLIGHT* pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);
		pLightCur->m_bEditCheck = FALSE;
	}

	//	Note : 새로운 라이트 추가.
	if( !DxLightMan::GetInstance()->GetLight(pThisLight->m_szLightName) )
	{
		DxLightMan::GetInstance()->AddLight( pThisLight );
		DxLightMan::GetInstance()->MakeAABBTree();
	}

	ListingLights();

	//	Note : 인터페이스에 값 업대이트.
	ModifyLightData ( *pThisLight );
	pView->ActiveLight ( pThisLight );

	// Note : 위치 정리
	//  삽질
	int nSize = m_ListLight.GetCount();
	if( nSize!=LB_ERR )
	{
		m_ListLight.SetFocus();
		m_ListLight.SetSel( nSize-1, TRUE );
	}

	// Note : 다이얼 로그..셋팅
	ResetDialog( pThisLight->m_szLightName );

	// Note : 생성이 되면 선택된 것과 같은 느낌이 나야함.
	OnLbnSelchangeListLight();
    WorldStateMachine::EditAsset(pThisLight->m_szLightName);
}

//	Note : 모든 Light들의 Enable/Disable 제어.
//
void CLightPage::OnBnClickedCheckLightingenable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LIGHTINGENABLE);
	DxLightMan::GetInstance()->EnableLighting ( pButton->GetCheck() );
}

//	Note : Light 의 활성화 비활성화.
//
void CLightPage::OnBnClickedCheckEnablelight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLELIGHT);
	pLight->m_bEnablePL_Day = pButton->GetCheck();
}

void CLightPage::OnBnClickedCheckEnablePlNight()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLE_PL_NIGHT);
	pLight->m_bEnablePL_Night = pButton->GetCheck();
}

void CLightPage::OnCbnSelchangeComboLighttype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//DWORD nIndex = m_ComboLightType.GetCurSel ();
	//DWORD nData = m_ComboLightType.GetItemData ( nIndex );

	//SetLightType ( D3DLIGHTTYPE(nData) );
}

void CLightPage::OnBnClickedButtonDir()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &pLight->m_Light.Direction, LIGHTPAGEINDEX );
}

void CLightPage::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode 에서는 셋팅이 불가능 합니다.") );
		return;
	}

	//D3DCOLORVALUE &d3dColor = m_pThisLight->m_Light.Diffuse;	// 기본 값으로 바뀌었다.
	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	D3DCOLORVALUE &d3dColor = pLight->m_BaseDiffuse;
	COLORREF Color = RGB ( int(d3dColor.r*255.0f), int(d3dColor.g*255.0f), int(d3dColor.b*255.0f) );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

	pLight->m_Light.Diffuse	= d3dColor;
	pLight->m_BaseDiffuse	= d3dColor;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )
		{
			pLightCur->m_Light.Diffuse	= d3dColor;
			pLightCur->m_BaseDiffuse	= d3dColor;
		}
	}
}

void CLightPage::OnBnClickedButtonAmbient()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode 에서는 셋팅이 불가능 합니다.") );
		return;
	}

	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	D3DCOLORVALUE d3dColor = pLight->m_Light.Ambient;
	COLORREF Color = RGB ( int(d3dColor.r*255.0f), int(d3dColor.g*255.0f), int(d3dColor.b*255.0f) );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

	pLight->m_Light.Ambient	= d3dColor;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
			pLightCur->m_Light.Ambient	= d3dColor;
		}
	}
}

void CLightPage::OnBnClickedButtonSpecular()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	D3DCOLORVALUE d3dColor = pLight->m_Light.Specular;
	COLORREF Color = RGB ( int(d3dColor.r*255.0f), int(d3dColor.g*255.0f), int(d3dColor.b*255.0f) );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

	pLight->m_Light.Specular	= d3dColor;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
			pLightCur->m_Light.Specular	= d3dColor;
		}
	}
}

void CLightPage::KeyBoard_Enter()
{
	//DXLIGHT* pLight = GetCurLight();
	//if( !pLight )	return;

 //   float fRange = 0.f;
	//CString strText;
	//CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_RANGE);
	//pWnd->GetWindowText( strText );
 //   fRange = (float) atof ( strText.GetString() );

	////float fRange = 0.f;
	////float fAttenuation1 = 0.f;
	////fRange = (float) atof ( strText.GetString() );
	////if( fRange > 0.f )
	////{
	////	fAttenuation1 = 1.0f/fRange;
 ////       
	////}
	////else	fAttenuation1 = 0.f;

 ////   pLight->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_Light.Range * pLightCur->m_Light.Range * 10.f );


	////pLight->m_Light.Range			= fRange;
	////pLight->m_Light.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_Light.Range * 10.f );
 ////   pLight->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_Light.Range * pLightCur->m_Light.Range * 10.f );

	//DXLIGHT* pLightCur;
	//for( int i=2; i<m_ListLight.GetCount(); ++i)
	//{
	//	pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

	//	if( pLightCur->m_bEditCheck )	
	//	{
	//		pLightCur->m_Light.Range		= fRange;
	//        pLightCur->m_Light.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_Light.Range * 0.1f );
 //           pLightCur->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_Light.Range * pLightCur->m_Light.Range * 0.1f );
	//	}
	//}
}

BOOL CLightPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	pButton->SetCheck( FALSE );

	OnBnClickedCheckDspName();

	return CPropertyPage::OnKillActive();
}

void CLightPage::OnBnClickedCheckRangeRend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem(IDC_CHECK_RANGE_REND);
	DxLightMan::GetInstance()->EnableRangeRend ( pButton->GetCheck() );
}

void CLightPage::OnBnClickedCheckNightandday()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_NIGHTANDDAY);
	DxLightMan::GetInstance()->EnableNightAndDay ( pButton->GetCheck() );
}

void CLightPage::OnBnClickedButtonToday()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	float fTIMETOVBR = GetWin_Num_float ( this, IDC_EDIT_TIME_VBR );
	GLPeriod::GetInstance().TestToNoon ( fTIMETOVBR );
}

void CLightPage::OnBnClickedButtonTonight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	float fTIMETOVBR = GetWin_Num_float ( this, IDC_EDIT_TIME_VBR );
	GLPeriod::GetInstance().TestToNight ( fTIMETOVBR );
}

void CLightPage::OnBnClickedCheckDaypointoff()
{
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_DAYPOINTOFF);
	DxLightMan::GetInstance()->EnableWhenDayPointOff ( pButton->GetCheck() );
}

void CLightPage::OnBnClickedButtonTimeset()
{
	GLPeriod::GetInstance().SetTIMETOVBR ( GetWin_Num_float ( this, IDC_EDIT_TIME_VBR ) );

	DWORD dwTime = GetWin_Num_int ( this, IDC_EDIT_TIME );
	GLPeriod::GetInstance().SetHour ( dwTime );
	DxLightMan::GetInstance()->SetForceStopTime( dwTime );
}

void CLightPage::OnLbnSelchangeListLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bDirc = FALSE;

	DXLIGHT* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		if( i==0 || i==1 )
		{
			if( bDirc )
			{
				// Note : Direction에서 2번 걸렸다.
				if( m_ListLight.GetSel(i) > 0 )
				{
					MessageBox( "Direction이 중복 선택되었습니다." );
					ListingLights();
					return;
				}
			}

			// Note : Direction 1번 걸렸다.
			if( m_ListLight.GetSel(i) > 0 )	bDirc = TRUE;
			continue;
		}

		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);
		if( m_ListLight.GetSel(i) > 0 )	pLightCur->m_bEditCheck = TRUE;
		else							pLightCur->m_bEditCheck = FALSE;

		// Note : 걸려있다.
		if( pLightCur->m_bEditCheck && bDirc )
		{
			// Note : Direction이 1번 선택된 후 또 걸린다면 에러 코드 및 Dialog 초기화 후 Return;
			MessageBox( "Direction과 동시에 Point가 선택되었습니다." );
			ListingLights();
			return;
		}
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
}

void CLightPage::ListingLights()
{
	m_ListLight.ResetContent ();

	SetLightType( D3DLIGHT_FORCE_DWORD );
	m_ListLight.AddString ( "DIRECTIONAL-[NOON]" );
	m_ListLight.AddString ( "DIRECTIONAL-[NIGHT]" );

	DXLIGHT* pCurLight(NULL);
    LIST_DXLIGHT& listPointLight = DxLightMan::GetInstance()->GetLIST_DXLIGHT();
    LIST_DXLIGHT_ITER iter = listPointLight.begin();
    for( ; iter!=listPointLight.end(); ++iter )
    {
        pCurLight = (*iter);

		DWORD nIndex = m_ListLight.AddString ( pCurLight->m_szLightName );
		m_ListLight.SetItemData ( nIndex, (DWORD) pCurLight );
	}
}

DXLIGHT* CLightPage::GetCurLight()
{
	DXLIGHT* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		if( i==0 )
		{
			if( m_ListLight.GetSel(i) > 0 )
			{
				return DxLightMan::GetInstance()->GetDirectNoonLight();
			}
		}
		else if( i==1 )
		{
			if( m_ListLight.GetSel(i) > 0 )
			{
				return DxLightMan::GetInstance()->GetDirectNightLight();
			}
		}
		else
		{
			pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

			if( pLightCur->m_bEditCheck )
			{
				return pLightCur;
			}
		}
	}

	return NULL;
}

void CLightPage::ResetDialog()
{
    // Note : 선택된 것이 1개일 경우
	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 ) return;

		CString Text;
		m_ListLight.GetText ( nIndex, Text );

		// Note : 데이터에 맞게 재 설정한다.
		ResetDialog( Text );

		return;
	}

	// Note : 선택된 것이 2개일 경우 ( Direction 이 들어갈 위험은 이전 함수에서 없애 버렸다. )
	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )
		{
			ResetDialog( pLightCur->m_szLightName );	// 대표 한넘으로 본다.
			return;
		}
	}
}

void CLightPage::ResetDialog( CString Text )
{
	int nIndex;

	if( Text == "DIRECTIONAL-[NOON]" )
	{
		SetLightType ( D3DLIGHT_DIRECTIONAL );
		GetDlgItem(IDC_COMBO_LIGHTTYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(FALSE);

		nIndex = m_ComboLightType.FindString ( 0, "DIRECTIONAL-[NOON]" );
		m_ComboLightType.SetCurSel ( nIndex );

		DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNoonLight ();

		ModifyLightData ( *pLight );

		// Note : 커서 없애기
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}
	else if( Text == "DIRECTIONAL-[NIGHT]" )
	{
		SetLightType ( D3DLIGHT_DIRECTIONAL );
		GetDlgItem(IDC_COMBO_LIGHTTYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(FALSE);

		nIndex = m_ComboLightType.FindString ( 0, "DIRECTIONAL-[NIGHT]" );
		m_ComboLightType.SetCurSel ( nIndex );

		DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNightLight ();

		ModifyLightData ( *pLight );

		// Note : 커서 없애기
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}
	else
	{
		//	Note : 편집할 Light를 가져온다.
		GetDlgItem(IDC_COMBO_LIGHTTYPE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(TRUE);

		//	Note : 편집할 Light를 가져온다.
		DXLIGHT* pLight = DxLightMan::GetInstance()->GetLight( Text.GetString() );

		if ( pLight )
		{
			m_ComboLightType.SetCurSel ( m_ComboLightType.GetItemData( pLight->m_Light.Type ) );
			SetLightType ( pLight->m_Light.Type );
			ModifyLightData ( *pLight );

			D3DXVECTOR3 vPos( pLight->m_matWorld._41, pLight->m_matWorld._42, pLight->m_matWorld._43 );
			DxViewPort::GetInstance().CameraJump( vPos );

			//	Note : 라이트의 위치 편집을 활성화 한다.
			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
			pView->ActiveLight( pLight );
		}
		else
		{
			SetLightType( D3DLIGHT_FORCE_DWORD );

			// Note : 커서 없애기
			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
			pView->DeActiveEditMatrix();
		}
	}
}

void CLightPage::UpdatePageClickDOWN()
{
	if( !m_bDlgInit )	return;

	// List 업데이트
	ListingLights();

	// Note : 위치 정리
	int nSize = m_ListLight.GetCount();
	if( nSize > 2 )		m_ListLight.SetSel( nSize-1, TRUE );
	else				m_ListLight.SetSel( 0, TRUE );

	// Note : 다이얼 로그..셋팅
	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;
	ResetDialog( pLight->m_szLightName );
}

void CLightPage::UpdatePageClickUP()
{
	if( !m_bDlgInit )	return;

	DxLightMan::GetInstance()->MakeAABBTree();
}

void CLightPage::OnBnClickedCheckDspName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	DxLightMan::GetInstance()->SetDSPName( (BOOL)pButton->GetCheck() );
}

BOOL CLightPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetSetLandMan()->ActiveMap ();

    char szValue[256];
    _gcvt ( DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower(), 7, szValue );
	GetDlgItem(IDC_EDIT_LM_DDP)->SetWindowText(szValue);

	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	
        return CPropertyPage::OnSetActive();

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_DIRX );
	_gcvt ( pLight->m_Light.Direction.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem( IDC_EDIT_DIRY );
	_gcvt ( pLight->m_Light.Direction.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem( IDC_EDIT_DIRZ );
	_gcvt ( pLight->m_Light.Direction.z, 7, szValue );
	pWnd->SetWindowText ( szValue );		

	pWnd = GetDlgItem(IDC_EDIT_OVER_LIGHTING);
	pWnd->SetWindowText( _T("2.0") );

	return CPropertyPage::OnSetActive();
}

void CLightPage::OnNMCustomdrawSliderAtt0(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation0 = m_ctrlATT0.GetPos();
			pLightCur->m_Light.Attenuation0	= m_ctrlATT0.GetPos() * 0.01f;

            SetEndLineValue( pLightCur );
		}
	}
}

void CLightPage::OnNMCustomdrawSliderAtt1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation1 = m_ctrlATT1.GetPos();
			pLightCur->m_Light.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_Light.Range * 0.5f );

            SetEndLineValue( pLightCur );
		}
	}
}

void CLightPage::OnNMCustomdrawSliderAtt2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
            pLightCur->m_nAttenuation2 = m_ctrlATT2.GetPos();
			pLightCur->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_Light.Range * pLightCur->m_Light.Range * 0.5f );

            SetEndLineValue( pLightCur );
		}
	}
}

void CLightPage::SetEndLineValue( const DXLIGHT* pLight )
{
    float fEndLineValue = 1.f / ( pLight->m_Light.Attenuation0 + 
                                  (pLight->m_Light.Attenuation1 * pLight->m_Light.Range) + 
                                  (pLight->m_Light.Attenuation2 * pLight->m_Light.Range * pLight->m_Light.Range) );

    char szValue[256];
    CWnd* pWnd = GetDlgItem( IDC_EDIT_PL_END_LINE );
    _gcvt ( fEndLineValue, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CLightPage::OnEnChangeEditRange()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
    // ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
    // 해당 알림 메시지를 보냅니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

    DXLIGHT* pLight = GetCurLight();
	if( !pLight )	return;

    float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_RANGE);
	pWnd->GetWindowText( strText );
    fRange = (float) atof ( strText.GetString() );

	DXLIGHT* pLightCur;
	for( int i=2; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);

		if( pLightCur->m_bEditCheck )	
		{
			pLightCur->m_Light.Range		= fRange;
	        pLightCur->m_Light.Attenuation1	= m_ctrlATT1.GetPos() * 0.01f / ( pLightCur->m_Light.Range * 0.5f );
            pLightCur->m_Light.Attenuation2	= m_ctrlATT2.GetPos() * 0.01f / ( pLightCur->m_Light.Range * pLightCur->m_Light.Range * 0.5f );
		}
	}
}

void CLightPage::OnBnClickedButtonDiffuseLsc()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode 에서는 셋팅이 불가능 합니다.") );
		return;
	}

    const D3DXVECTOR3& vShadowColor = DxLightMan::GetInstance()->GetShadowColorDay();

	COLORREF Color = RGB ( int(vShadowColor.x*255.0f), int(vShadowColor.y*255.0f), int(vShadowColor.z*255.0f) );
	CColorPickerDlg colorDlg ( Color );

    D3DCOLORVALUE d3dColor;
    d3dColor.r = vShadowColor.x;
    d3dColor.g = vShadowColor.y;
    d3dColor.b = vShadowColor.z;
	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

    DxLightMan::GetInstance()->SetShadowColorDay( d3dColor );
}

void CLightPage::OnBnClickedButtonAmbientLsc()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode 에서는 셋팅이 불가능 합니다.") );
		return;
	}

    const D3DXVECTOR3& vShadowColor = DxLightMan::GetInstance()->GetShadowColorNight();

	COLORREF Color = RGB ( int(vShadowColor.x*255.0f), int(vShadowColor.y*255.0f), int(vShadowColor.z*255.0f) );
	CColorPickerDlg colorDlg ( Color );

    D3DCOLORVALUE d3dColor;
    d3dColor.r = vShadowColor.x;
    d3dColor.g = vShadowColor.y;
    d3dColor.b = vShadowColor.z;
	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor = colorDlg.GetColor ();

		d3dColor.r = GetRValue ( retColor ) / 255.0f;
		d3dColor.g = GetGValue ( retColor ) / 255.0f;
		d3dColor.b = GetBValue ( retColor ) / 255.0f;
	}

    DxLightMan::GetInstance()->SetShadowColorNight( d3dColor );
}

void CLightPage::OnEnChangeEditLmDdp()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
    // ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
    // 해당 알림 메시지를 보냅니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString strText;
    CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_LM_DDP);
	pWnd->GetWindowText( strText );
    float fDiffusePower = static_cast<float>( atof ( strText.GetString() ) );

    DxLightMan::GetInstance()->SetLightMapDirectionDiffusePower( fDiffusePower );
}

void CLightPage::OnEnChangeEditOverLighting()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DXLIGHT* pLight = GetCurLight();
	if( !pLight )	
		return;

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