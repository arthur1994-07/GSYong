// ShadowEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectShadow.h"

#include "ShadowEffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CShadowEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CShadowEffEdit, CPropertyPage)
CShadowEffEdit::CShadowEffEdit()
	: CPropertyPage(CShadowEffEdit::IDD)
{
}

CShadowEffEdit::~CShadowEffEdit()
{
}

void CShadowEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SS_SIZE, m_pComboSSSize);
}


BEGIN_MESSAGE_MAP(CShadowEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_MAKESS, OnBnClickedButtonMakess)
	ON_BN_CLICKED(IDC_CHECK_DIRECT, OnBnClickedCheckDirect)
	ON_BN_CLICKED(IDC_CHECK_POINT, OnBnClickedCheckPoint)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FILTER, OnBnClickedButtonTextureFilter)
	ON_BN_CLICKED(IDC_CHECK_USESS, OnBnClickedCheckUsess)
	ON_BN_CLICKED(IDC_BUTTON_TEX_DAYTIME, OnBnClickedButtonTexDaytime)
	ON_BN_CLICKED(IDC_BUTTON_TEX_NIGHT, OnBnClickedButtonTexNight)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_UV, OnBnClickedButtonCreateUv)
	ON_CBN_SELCHANGE(IDC_COMBO_SS_SIZE, OnCbnSelchangeComboSsSize)
END_MESSAGE_MAP()


// CShadowEffEdit 메시지 처리기입니다.
void CShadowEffEdit::SetInstance ( DxEffectShadow *pShadowEff )
{
	m_pShadowEff = pShadowEff;
	m_OldShadowProperty = m_ShadowProperty = m_pShadowEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pShadowEff->GetName() );

	CButton*	pButton;
	char		szValue[256];

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DIRECT );
	pWnd	= GetDlgItem ( IDC_EDIT_DIRECT );
	if ( m_ShadowProperty.m_dwFlag & USEDIRECTSHADOW )
	{
		pButton->SetCheck ( TRUE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd->EnableWindow ( FALSE );
	}

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_POINT );
	pWnd	= GetDlgItem ( IDC_EDIT_POINT );
	if ( m_ShadowProperty.m_dwFlag & USEPOINTSHADOW )
	{
		pButton->SetCheck ( TRUE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd->EnableWindow ( FALSE );
	}


	pButton = (CButton*)GetDlgItem ( IDC_CHECK_USESS );
	if ( m_ShadowProperty.m_dwFlag & USESTATICSHADOW )
	{
		pButton->SetCheck ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_DAYTIME );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_NIGHT );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_DAYTIME );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_NIGHT );
		pWnd->EnableWindow ( FALSE );
	}

	pWnd = GetDlgItem ( IDC_EDIT_DIRECT );
	_gcvt ( m_ShadowProperty.m_fShadowD, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_POINT );
	_gcvt ( m_ShadowProperty.m_fShadowP, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_X );
	_gcvt ( m_ShadowProperty.m_fViewMoveX, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_Z );
	_gcvt ( m_ShadowProperty.m_fViewMoveZ, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_TEX_FILTER );
	pWnd->SetWindowText ( m_ShadowProperty.m_szFilter );

	pWnd = GetDlgItem ( IDC_EDIT_TEX_DAYTIME );
	pWnd->SetWindowText ( m_ShadowProperty.m_szTexDayTime );

	pWnd = GetDlgItem ( IDC_EDIT_TEX_NIGHT );
	pWnd->SetWindowText ( m_ShadowProperty.m_szTexNight );


	CString	Str;
	int		nIndex;
	m_pComboSSSize.ResetContent();

	Str.Format ( "64" );
	nIndex = m_pComboSSSize.AddString ( Str );
	m_pComboSSSize.SetItemData ( nIndex, 0 );
	Str.Format ( "128" );
	nIndex = m_pComboSSSize.AddString ( Str );
	m_pComboSSSize.SetItemData ( nIndex, 1 );
	Str.Format ( "256" );
	nIndex = m_pComboSSSize.AddString ( Str );
	m_pComboSSSize.SetItemData ( nIndex, 2 );
	Str.Format ( "512" );
	nIndex = m_pComboSSSize.AddString ( Str );
	m_pComboSSSize.SetItemData ( nIndex, 3 );
	Str.Format ( "1024" );
	nIndex = m_pComboSSSize.AddString ( Str );
	m_pComboSSSize.SetItemData ( nIndex, 4 );

	if ( m_ShadowProperty.m_dwTexSize == 64 )			nIndex = 0;
	else if ( m_ShadowProperty.m_dwTexSize == 128 )		nIndex = 1;
	else if ( m_ShadowProperty.m_dwTexSize == 256 )		nIndex = 2;
	else if ( m_ShadowProperty.m_dwTexSize == 512 )		nIndex = 3;
	else if ( m_ShadowProperty.m_dwTexSize == 1024 )	nIndex = 4;

	m_pComboSSSize.SetCurSel ( 0 );
}

void CShadowEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CWnd*		pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_DIRECT );
	pWnd->GetWindowText ( strText );
	m_ShadowProperty.m_fShadowD = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_POINT );
	pWnd->GetWindowText ( strText );
	m_ShadowProperty.m_fShadowP = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_X );
	pWnd->GetWindowText ( strText );
	m_ShadowProperty.m_fViewMoveX = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_Z );
	pWnd->GetWindowText ( strText );
	m_ShadowProperty.m_fViewMoveZ = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEX_FILTER );
	pWnd->GetWindowText ( strText );
	strcpy ( m_ShadowProperty.m_szFilter, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEX_DAYTIME );
	pWnd->GetWindowText ( strText );
	strcpy ( m_ShadowProperty.m_szTexDayTime, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEX_NIGHT );
	pWnd->GetWindowText ( strText );
	strcpy ( m_ShadowProperty.m_szTexNight, strText.GetString() );

	int		nIndex, nData;
	nIndex = m_pComboSSSize.GetCurSel ();
	nData = m_pComboSSSize.GetItemData ( nIndex );
	m_ShadowProperty.m_dwTexSize = (1<<(nData+6));

	//	Note : 속성을 적용.
	//
	m_pShadowEff->SetProperty ( m_ShadowProperty );
	m_pShadowEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pShadowEff->Create ( pView->GetD3DDevice() );

	m_pShadowEff->ReSetTexUV ( pView->GetD3DDevice(), pView->GetSetLandMan()->GetFrameMesh() );
}

void CShadowEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CShadowEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pShadowEff->SetProperty ( m_OldShadowProperty );
	m_pShadowEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pShadowEff->Create ( pView->GetD3DDevice() );
}


void CShadowEffEdit::OnBnClickedButtonTextureFilter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEX_FILTER );
		pWnd->SetWindowText ( FilePath );
	}
}

void CShadowEffEdit::OnBnClickedButtonTexDaytime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEX_DAYTIME );
		pWnd->SetWindowText ( FilePath );
	}
}

void CShadowEffEdit::OnBnClickedButtonTexNight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = DxEffectShadow::GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEX_NIGHT );
		pWnd->SetWindowText ( FilePath );
	}
}


void CShadowEffEdit::OnBnClickedCheckDirect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DIRECT );
	if ( pButton->GetCheck () )
	{
		m_ShadowProperty.m_dwFlag |= USEDIRECTSHADOW;

		pWnd = GetDlgItem ( IDC_EDIT_DIRECT );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_ShadowProperty.m_dwFlag &= ~USEDIRECTSHADOW;

		pWnd = GetDlgItem ( IDC_EDIT_DIRECT );
		pWnd->EnableWindow ( FALSE );
	}

	m_pShadowEff->SetProperty ( m_ShadowProperty );
}

void CShadowEffEdit::OnBnClickedCheckPoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_POINT );
	if ( pButton->GetCheck () )
	{
		m_ShadowProperty.m_dwFlag |= USEPOINTSHADOW;

		pWnd = GetDlgItem ( IDC_EDIT_POINT );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_ShadowProperty.m_dwFlag &= ~USEPOINTSHADOW;

		pWnd = GetDlgItem ( IDC_EDIT_POINT );
		pWnd->EnableWindow ( FALSE );
	}

	m_pShadowEff->SetProperty ( m_ShadowProperty );
}

void CShadowEffEdit::OnBnClickedCheckUsess()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_USESS );
	if ( pButton->GetCheck () )
	{
		m_ShadowProperty.m_dwFlag |= USESTATICSHADOW;

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_DAYTIME );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_NIGHT );
		pWnd->EnableWindow ( TRUE );

		m_pShadowEff->ReSetTexUV ( pView->GetD3DDevice(), pView->GetSetLandMan()->GetFrameMesh() );

		m_ShadowProperty.m_vViewMax = m_pShadowEff->GetProperty().m_vViewMax;
		m_ShadowProperty.m_vViewMin = m_pShadowEff->GetProperty().m_vViewMin;
	}
	else
	{
		m_ShadowProperty.m_dwFlag &= ~USESTATICSHADOW;

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_DAYTIME );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_NIGHT );
		pWnd->EnableWindow ( FALSE );
	}

	m_pShadowEff->SetProperty ( m_ShadowProperty );
}

void CShadowEffEdit::OnBnClickedButtonMakess()	// 정적 그림자 생성
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	//DxSkinObject *pSkinObject = pView->GetSkinObject();

	CString StrFileName;
	CString strText;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_TEX_DAYTIME );
	pWnd->GetWindowText ( StrFileName );

	CString szFilter = "S_Shadow Tex File (*.dds)|*.dds|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".dds",StrFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CShadowEffEdit*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffectShadow::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		pWnd = GetDlgItem ( IDC_EDIT_DIRECT );
		pWnd->GetWindowText ( strText );
		m_ShadowProperty.m_fShadowD = (float)atof ( strText.GetString() );

		pWnd = GetDlgItem ( IDC_EDIT_POINT );
		pWnd->GetWindowText ( strText );
		m_ShadowProperty.m_fShadowP = (float)atof ( strText.GetString() );

		m_pShadowEff->SetProperty ( m_ShadowProperty );

		CString sFileName = dlg.GetFileName();

		if( SUCCEEDED( pView->GetD3DDevice()->BeginScene() ) )
		{
			m_pShadowEff->MakeStaticShadow ( pView->GetD3DDevice(), sFileName.GetString(), pView->GetSetLandMan(), m_ShadowProperty.m_dwTexSize );

			// End the scene.
			pView->GetD3DDevice()->EndScene();
		}

		pWnd = GetDlgItem ( IDC_EDIT_TEX_DAYTIME );
		pWnd->SetWindowText ( sFileName.GetString () );
	}
}


void CShadowEffEdit::OnBnClickedButtonCreateUv()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	m_pShadowEff->ReSetTexUV ( pView->GetD3DDevice(), pView->GetSetLandMan()->GetFrameMesh() );

	m_ShadowProperty.m_vViewMax = m_pShadowEff->GetProperty().m_vViewMax;
	m_ShadowProperty.m_vViewMin = m_pShadowEff->GetProperty().m_vViewMin;
}

void CShadowEffEdit::OnCbnSelchangeComboSsSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int		nIndex, nData;
	nIndex = m_pComboSSSize.GetCurSel ();
	nData = m_pComboSSSize.GetItemData ( nIndex );
	m_ShadowProperty.m_dwTexSize = (1<<(nData+6));

	//	Note : 속성을 적용.
	//
	m_pShadowEff->SetProperty ( m_ShadowProperty );
}
