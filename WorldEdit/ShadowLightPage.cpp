// LightPage.cpp : ���� �����Դϴ�.
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

// CShadowLightPage ��ȭ �����Դϴ�.

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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CShadowLightPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	// vector �̱⿡ ���ʿ������� �����ش�.
	for( int nIndex=m_ListLight.GetCount()-1; nIndex>=0; --nIndex )
	{
		int nCheck = m_ListLight.GetSel( nIndex );

		if( nCheck > 0 )
		{
			CString strText;
			m_ListLight.GetText( nIndex, strText );

			CString strMsg;
			strMsg.Format ( "'%s' �� ���� �Ͻðڽ��ϱ�? ������ �ǻ츱�� �����ϴ�.", strText.GetString() );

			if ( IDYES == MessageBox ( strMsg, "����", MB_YESNO|MB_ICONQUESTION ) )
			{
				pView->GetSetLandMan()->GetShadowLightData_NoneConst()->Delete( nIndex );
				NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
                //WorldStateMachine::EditAsset(Text.GetString());
			}
		}
	}

	// ����Ʈ�� ���
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

    // ��ġ�� Ȯ��
    SetEndLineValue( &pSpotLight->m_sLight );
}

//	Note : ���ο� Light ����.
void CShadowLightPage::OnBnClickedButtonCreate()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem(IDC_EDIT_LIGHTNAME)->EnableWindow(TRUE);

	//	Note : ���ο� LIGHT �� �ʱ�ȭ ��Ų��.
	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLight = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
	spShadowSpotLight->SetDefault( vCollPos );
	spShadowSpotLight->InsertColor( 255, 255, 255 );

	// ���õǾ��ٴ� ���� �����ش�.
	// �� �ڵ尡 ������ ModifyLightData ���� ���õ� Light �鿡�� Default ���� ���õȴ�.
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		DxShadowSpotLight* pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		pLightCur->m_bEditCheck = FALSE;
	}

	//	Note : ���ο� ����Ʈ �߰�.
	pView->GetSetLandMan()->GetShadowLightData_NoneConst()->Insert( spShadowSpotLight );
	NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );

	// ����Ʈ�� ���
	ListingLights();

	// Note : ��ġ ����
	int nSize = m_ListLight.GetCount();
	if( nSize!=LB_ERR )
	{
		m_ListLight.SetFocus();
		m_ListLight.SetSel( nSize-1, TRUE );
	}

	// Note : ���̾� �α�..����
	ResetDialog( spShadowSpotLight.get() );

	// Note : ������ �Ǹ� ���õ� �Ͱ� ���� ������ ������.
	OnLbnSelchangeListLight();
    //WorldStateMachine::EditAsset(pThisLight->m_szLightName);
}

//	Note : ��� Light���� Enable/Disable ����.
//
void CShadowLightPage::OnBnClickedCheckDirectionLight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if ( CWorldEditView::m_ViewType==OCTREE_VIEW )
	{
		AfxMessageBox( _T("OctreeMode ������ ������ �Ұ��� �մϴ�.") );
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
		// Light�� �̵��ϴ� ��찡 �ֱ� ������ Tree �� �ٽ� ������ش�.
		//UpdatePageClickUP();

		// Light�� ȸ���� ���� �ֱ� ������ Direction �� �˸°� �������ش�.
		UpdateSpotLightDirection();
	}
	//if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWN )
	//{
	//	// Clone �� �� List�� ���� �����Ѵ�.
	//	UpdatePageClickDOWN();
	//}
}

BOOL CShadowLightPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();

	// ���õǾ��ٴ� ���� �����ش�.
	// �� �ڵ尡 ������ OnSetActive -> OnEnChangeEditTheta �̷������� ����Ʈ ���� ���õǴ� ������ ���� �� �ִ�.
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		DxShadowSpotLight* pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		pLightCur->m_bEditCheck = FALSE;
	}

	return CPropertyPage::OnKillActive();
}

void CShadowLightPage::OnBnClickedCheckRangeRend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	DxShadowSpotLight* pLightCur;
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );
		if( m_ListLight.GetSel(i) > 0 )	pLightCur->m_bEditCheck = TRUE;
		else							pLightCur->m_bEditCheck = FALSE;
	}

	// Note : Dialog Setting
	ResetDialog();

	// Note : 2�� �̻��� �ߺ��� ��� �̵�/ȸ��/������ �� ����.
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

    // Note : ���õ� ���� 1���� ���
	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 ) return;

		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(nIndex) );

		// Note : �����Ϳ� �°� �� �����Ѵ�.
		ResetDialog( pLightCur );

		return;
	}

	// Note : ���õ� ���� 2���� ���
	for( int i=0; i<m_ListLight.GetCount(); ++i)
	{
		pLightCur = reinterpret_cast<DxShadowSpotLight*>( m_ListLight.GetItemData(i) );

		if( pLightCur->m_bEditCheck )
		{
			ResetDialog( pLightCur );	// ��ǥ �ѳ����� ����.
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

		//	Note : ����Ʈ�� ��ġ ������ Ȱ��ȭ �Ѵ�.
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->ActiveSpotLight( &pSpotLight->m_matWorld );
	}
	else
	{
		// Note : Ŀ�� ���ֱ�
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}
}

//void CShadowLightPage::UpdatePageClickDOWN()
//{
//	if( !m_bDlgInit )	return;
//
//	// List ������Ʈ
//	ListingLights();
//
//	// Note : ��ġ ����
//	int nSize = m_ListLight.GetCount();
//	if( nSize > 2 )		m_ListLight.SetSel( nSize-1, TRUE );
//	else				m_ListLight.SetSel( 0, TRUE );
//
//	// Note : ���̾� �α�..����
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    // TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
    // CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
    // ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
    // �ش� �˸� �޽����� �����ϴ�.

    // TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 )
			return;

		if ( pView->GetSetLandMan()->GetShadowLightData_NoneConst()->LightItemUp( nIndex ) )
		{
			// ����Ʈ�� ���
			ListingLights();

			m_ListLight.SetSel( nIndex-1, TRUE );

			NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
		}
	}
}

void CShadowLightPage::OnBnClickedButtonDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	if( m_ListLight.GetSelCount() == 1 )
	{
		DWORD nIndex = m_ListLight.GetCurSel ();
		if ( nIndex==-1 )
			return;

		if ( pView->GetSetLandMan()->GetShadowLightData_NoneConst()->LightItemDown( nIndex ) )
		{
			// ����Ʈ�� ���
			ListingLights();

			m_ListLight.SetSel( nIndex+1, TRUE );

			NSShadowLight::SetActive( pView->GetSetLandMan()->GetShadowLightData_NoneConst() );
		}
	}
}

// Light�� ȸ���� ���� �ֱ� ������ Direction �� �˸°� �������ش�.
//	��ġ���� ���������� �������ش�.
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

// ���λ����� ������ ���� List �� ����
void CShadowLightPage::UpdatePage_List_AddLastItem()
{
	// ����Ʈ�� ���
	ListingLights();

	m_ListLight.SetSel( m_ListLight.GetCount()-1, TRUE );

	// Note : Dialog Setting
	ResetDialog();
}

void CShadowLightPage::OnEnChangeEditLightname()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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

	// ����Ʈ�� ���
	ListingLights();

	m_ListLight.SetSel( nIndex, TRUE );
}

void CShadowLightPage::OnEnChangeEditTheta()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	DxShadowSpotLight* pLight = GetCurLight();
	if( !pLight )	return;

	float fRange = 0.f;
	CString strText;
	CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_OVER_LIGHTING);
	pWnd->GetWindowText( strText );
	pLight->m_fOverLighting = (float) atof ( strText.GetString() );
	if ( pLight->m_fOverLighting > 3.f )
	{
		// 3 �� �ʰ� �� ���� ����.
		pWnd->SetWindowText( "3.0" );
		pLight->m_fOverLighting = 3.f;
	}
}

void CShadowLightPage::OnBnClickedButtonDirectionLightDocument()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TSTRING strText = 
		"�ǳ����� ��� Direction Light üũ�ڽ��� �� Ȱ��ȭ �ؾ� �Ѵ�.. \r\n"
		"���� LIGHT EDIT (��) ���� �� DIRECTIONLIGHT-[NOON] �� Diffuse ���� Zero�� ���� �ϰ�, Ambient ������ �ణ�� ������ �ְ�, Point Light �� Spot Light �� �� ������ �����ϵ��� �Ѵ�.\r\n"
		;

	AfxMessageBox( strText.c_str() );
}
