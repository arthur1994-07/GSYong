// RCSkyBoxManPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../EngineLib/Common/SUBPATH.H"
#include "../../EngineLib/DxTools/SkyBox/DxSkyBoxMan.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "SkyBoxManPage.h"

// CSkyBoxManPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkyBoxManPage, CPropertyPage)
CSkyBoxManPage::CSkyBoxManPage()
	: CPropertyPage(CSkyBoxManPage::IDD)
	, m_pSkyBoxMan(NULL)
	, m_pSkyBoxManSRC(NULL)
	, m_rFogLocalMan(NULL)
{
}

CSkyBoxManPage::~CSkyBoxManPage()
{
}

void CSkyBoxManPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SKYBOX, m_ctrlSKYBOX);
}


BEGIN_MESSAGE_MAP(CSkyBoxManPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SKYBOX, OnNMClickListSkybox)
	ON_BN_CLICKED(IDC_RADIO_BLEND_ADD, &CSkyBoxManPage::OnBnClickedRadioBlendAdd)
	ON_BN_CLICKED(IDC_RADIO_BLEND_ALPHA, &CSkyBoxManPage::OnBnClickedRadioBlendAlpha)
	ON_BN_CLICKED(IDC_RADIO_BLEND_BASE, &CSkyBoxManPage::OnBnClickedRadioBlendBase)
	ON_BN_CLICKED(IDC_RADIO_BLEND_LIGHT_SHAFTS, &CSkyBoxManPage::OnBnClickedRadioBlendLightShafts)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CSkyBoxManPage::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CSkyBoxManPage::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_SHAFTS_APPLY, &CSkyBoxManPage::OnBnClickedButtonLightShaftsApply)
	ON_BN_CLICKED(IDC_EDIT_LIGHT_SHAFTS_DIR_SET, &CSkyBoxManPage::OnBnClickedEditLightShaftsDirSet)
END_MESSAGE_MAP()


// CSkyBoxManPage 메시지 처리기입니다.
BOOL CSkyBoxManPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_ctrlSKYBOX.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = 
	{ 
		int(nListWidth*0.6f), 
		int(nListWidth*0.2f),
		int(nListWidth*0.2f),
	};

	m_ctrlSKYBOX.SetExtendedStyle ( m_ctrlSKYBOX.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlSKYBOX.InsertColumn ( 0, _T("Name"),	LVCFMT_CENTER, nColWidth[0] );
	m_ctrlSKYBOX.InsertColumn ( 1, _T("Alpha"),	LVCFMT_CENTER, nColWidth[1] );
	m_ctrlSKYBOX.InsertColumn ( 2, _T("Scale"),	LVCFMT_CENTER, nColWidth[2] );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSkyBoxManPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_WEIGHT );
	SetWindowText_gcvt( pWnd, m_pSkyBoxMan->m_fLightShaftsWeight, 7 );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_DECAY );
	SetWindowText_gcvt( pWnd, m_pSkyBoxMan->m_fLightShaftsDecay, 7 );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_LAST );
	SetWindowText_gcvt( pWnd, m_pSkyBoxMan->m_fLightShaftsLast, 7 );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_DIR_X );
	SetWindowText_gcvt ( pWnd, m_pSkyBoxMan->m_vLightShaftsDir.x, 7 );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_DIR_Y );
	SetWindowText_gcvt ( pWnd, m_pSkyBoxMan->m_vLightShaftsDir.y, 7 );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_DIR_Z );
	SetWindowText_gcvt ( pWnd, m_pSkyBoxMan->m_vLightShaftsDir.z, 7 );

	//CWorldEditView::m_ViewType = SKYBOXMANAGER_VIEW;
	//g_sViewMode = RCVM_SKYBOXMANAGER;
	UpdateSkyBoxList();

	return CPropertyPage::OnSetActive();
}

void CSkyBoxManPage::SetInstance( DxSkyBoxMan* pSkyBoxMan, DxSkyBoxMan* pSkyBoxManSRC, DxFogLocalMan* pFogLocalMan )
{
	m_pSkyBoxMan = pSkyBoxMan;
	m_pSkyBoxManSRC = pSkyBoxManSRC;
	m_rFogLocalMan	= pFogLocalMan;
}

void CSkyBoxManPage::OnBnClickedButtonNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	m_pSkyBoxMan->InsertSkyBox();

	UpdateSkyBoxList();
}

void CSkyBoxManPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	m_pSkyBoxMan->DeleteSkyBox( nItem );

	UpdateSkyBoxList();
}

void CSkyBoxManPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActiveBackUpPage( SKYBOXMANINDEX );
}

void CSkyBoxManPage::OnBnClickedButtonOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("X-File (*.x)|*.x");
	CFileDialog dlg ( TRUE, _T(".x"), NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	TCHAR szFullPath[1024];
	StringCchCopy( szFullPath, 1024, pFrame->m_szAppPath );
	StringCchCat( szFullPath, 1024, pPath->ObjFileObject().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	dlg.m_ofn.lpstrInitialDir = szFullPath;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_MESH );
		pWnd->SetWindowText ( FilePath );
	}
}

void CSkyBoxManPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	CWnd* pWnd;

	CString strName;
	pWnd = GetDlgItem( IDC_EDIT_MESH );
	pWnd->GetWindowText( strName );
	pSkyBox->m_strName = strName.GetString();

	
	pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
	pWnd->GetWindowText( strName );
	pSkyBox->m_fRotationBaseY = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATION_SPEED );
	pWnd->GetWindowText( strName );
	pSkyBox->m_fRotationSPEED = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SCALE );
	pWnd->GetWindowText( strName );
	pSkyBox->m_fScale = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText( strName );
	pSkyBox->m_fHeight = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POWER );
	pWnd->GetWindowText( strName );
	pSkyBox->m_fPower = (float)_tstof( strName.GetString() );

	UpdateSkyBoxList();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	m_pSkyBoxMan->Reload( pView->GetD3DDevice(), nItem );

	m_ctrlSKYBOX.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );

	//// LocalFog 때문에 이 작업이 필요함.
	//if( m_pSkyBoxManSRC && m_rFogLocalMan )
	//{
	//	m_pSkyBoxManSRC->Clone( pView->GetD3DDevice(), m_pSkyBoxMan );
	//	m_pSkyBoxManSRC->Reload( pView->GetD3DDevice() );

	//	pView->GetSetLandMan()->SetFogLocalMan( pView->GetD3DDevice(), m_rFogLocalMan );
	//}
}

void CSkyBoxManPage::UpdateSkyBoxList()
{
	m_ctrlSKYBOX.DeleteAllItems ();
	
	VEC_SKYBOX_ITER iter = m_pSkyBoxMan->m_pvecSKYBOX->begin();

	TCHAR szName[128];
	TSTRING strText;

	int nCount(0);
	for( ; iter!=m_pSkyBoxMan->m_pvecSKYBOX->end(); ++iter, ++nCount )
	{
		m_ctrlSKYBOX.InsertItem( nCount, (*iter)->m_strName.c_str() );

		switch( (*iter)->m_emBlend )
		{
		case DxSkyBox::BLEND_NONE:
			m_ctrlSKYBOX.SetItemText( nCount, 1, _T("0") );
			break;
		case DxSkyBox::BLEND_ADD:
			m_ctrlSKYBOX.SetItemText( nCount, 1, _T("1") );
			break;
		case DxSkyBox::BLEND_ALPHA:
			m_ctrlSKYBOX.SetItemText( nCount, 1, _T("2") );
			break;
		};
		
		GetText_gcvt( szName, (*iter)->m_fScale, 7 );
		m_ctrlSKYBOX.SetItemText( nCount, 2, szName );

		//GetText_gcvt( szName, (*iter)->m_fHeight, 7 );
		//m_ctrlSKYBOX.SetItemText( nCount, 2, szName );
	}
}

void CSkyBoxManPage::OnNMClickListSkybox(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	CWnd* pWnd;
	CButton* pButton;

	pWnd = GetDlgItem( IDC_EDIT_MESH );
	pWnd->SetWindowText( pSkyBox->m_strName.c_str() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
	SetWindowText_gcvt( pWnd, pSkyBox->m_fRotationBaseY, 7 );

	pWnd = GetDlgItem( IDC_EDIT_ROTATION_SPEED );
	SetWindowText_gcvt( pWnd, pSkyBox->m_fRotationSPEED, 7 );

	pWnd = GetDlgItem( IDC_EDIT_SCALE );
	SetWindowText_gcvt( pWnd, pSkyBox->m_fScale, 7 );

	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	SetWindowText_gcvt( pWnd, pSkyBox->m_fHeight, 7 );

	pWnd = GetDlgItem( IDC_EDIT_POWER );
	SetWindowText_gcvt( pWnd, pSkyBox->m_fPower, 7 );

	
	pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_BASE );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_ADD );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_ALPHA );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_LIGHT_SHAFTS );
	pButton->SetCheck( FALSE );

	switch( pSkyBox->m_emBlend )
	{
	case DxSkyBox::BLEND_NONE:
		{
			pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_BASE );
			pButton->SetCheck( TRUE );
		}
		break;
	case DxSkyBox::BLEND_ADD:
		{
			pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_ADD );
			pButton->SetCheck( TRUE );
		}
		break;
	case DxSkyBox::BLEND_ALPHA:
		{
			pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_ALPHA );
			pButton->SetCheck( TRUE );
		}
		break;

	case DxSkyBox::BLEND_LIGHT_SHAFTS:
		{
			pButton = (CButton*)GetDlgItem( IDC_RADIO_BLEND_LIGHT_SHAFTS );
			pButton->SetCheck( TRUE );
		}
		break;
	};
}

void CSkyBoxManPage::OnBnClickedRadioBlendAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	pSkyBox->m_emBlend = DxSkyBox::BLEND_ADD;
}

void CSkyBoxManPage::OnBnClickedRadioBlendAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	pSkyBox->m_emBlend = DxSkyBox::BLEND_ALPHA;
}

void CSkyBoxManPage::OnBnClickedRadioBlendBase()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	pSkyBox->m_emBlend = DxSkyBox::BLEND_NONE;
}

void CSkyBoxManPage::OnBnClickedRadioBlendLightShafts()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	DxSkyBox* pSkyBox = m_pSkyBoxMan->GetSkyBox( nItem );
	if( !pSkyBox )	return;

	pSkyBox->m_emBlend = DxSkyBox::BLEND_LIGHT_SHAFTS;
}

void CSkyBoxManPage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	m_pSkyBoxMan->ItemUp( m_pSkyBoxMan, nItem );

	UpdateSkyBoxList();
}

void CSkyBoxManPage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlSKYBOX.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( _T("위치를 선택하여 주십시요."), _T("정보"), MB_OK );
		return;
	}
	
	int nItem = m_ctrlSKYBOX.GetNextSelectedItem(pos);

	m_pSkyBoxMan->ItemDown( m_pSkyBoxMan, nItem );

	UpdateSkyBoxList();
}

void CSkyBoxManPage::OnBnClickedButtonLightShaftsApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;
	CString strName;

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_WEIGHT );
	pWnd->GetWindowText( strName );
	m_pSkyBoxMan->m_fLightShaftsWeight = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_DECAY );
	pWnd->GetWindowText( strName );
	m_pSkyBoxMan->m_fLightShaftsDecay = (float)_tstof( strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LIGHT_SHAFTS_LAST );
	pWnd->GetWindowText( strName );
	m_pSkyBoxMan->m_fLightShaftsLast = (float)_tstof( strName.GetString() );	
}

void CSkyBoxManPage::OnBnClickedEditLightShaftsDirSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pSkyBoxMan->m_vLightShaftsDir, SKYBOXMANINDEX );
}
