// RainPointEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Frame/DxEffectRainDrop.h"

#include "RainPointEffEdit.h"
#include ".\rainpointeffedit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRainPointEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRainPointEffEdit, CPropertyPage)
CRainPointEffEdit::CRainPointEffEdit()
	: CPropertyPage(CRainPointEffEdit::IDD)
{
}

CRainPointEffEdit::~CRainPointEffEdit()
{
}

void CRainPointEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRainPointEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE2, OnBnClickedButtonTexture2)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE4, OnBnClickedButtonTexture4)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE5, OnBnClickedButtonTexture5)
	ON_BN_CLICKED(IDC_BUTTON_RAIN_TEXTURE, OnBnClickedButtonRainTexture)
	ON_BN_CLICKED(IDC_CHECK_NEWEFFECT, OnBnClickedCheckNeweffect)
END_MESSAGE_MAP()


// CRainPointEffEdit 메시지 처리기입니다.
void CRainPointEffEdit::SetInstance ( DxEffectRainPoint *pRainPointEff )
{
	m_pRainPointEff = pRainPointEff;
	m_OldRainPointProperty = m_RainPointProperty = m_pRainPointEff->GetProperty();

	//	Note : 값 설정.
	//

	char szValue[256];
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_RATE );
	_gcvt ( m_RainPointProperty.m_fRainRate, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SNOW_SIZE );
	_gcvt ( m_RainPointProperty.m_fSnowSize, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_SIZE );
	_gcvt ( m_RainPointProperty.m_fLeavesSize, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_SPEED );
	_gcvt ( m_RainPointProperty.m_fLeavesSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_TEXTURE );
	pWnd->SetWindowText ( m_RainPointProperty.m_szRainTex );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_RainPointProperty.m_szCircleTex );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE2 );
	pWnd->SetWindowText ( m_RainPointProperty.m_szRainDropTex );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE3 );
	pWnd->SetWindowText ( m_RainPointProperty.m_szSnowTex );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE4 );
	pWnd->SetWindowText ( m_RainPointProperty.m_szLeavesTex );


    // 눈 효과를 위한..
	pWnd = GetDlgItem ( IDC_EDIT_PARTCOUNT );
	_gcvt ( m_RainPointProperty.m_SnowParticleData.Count, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_DISAPPEAR1 );
	_gcvt ( m_RainPointProperty.m_SnowParticleData.fMinDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_DISAPPEAR2 );
	_gcvt ( m_RainPointProperty.m_SnowParticleData.fMaxDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_FALLSPEED1 );
	_gcvt ( m_RainPointProperty.m_SnowParticleData.fMinFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_FALLSPEED2 );
	_gcvt ( m_RainPointProperty.m_SnowParticleData.fMaxFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// 비 효과를 위한..
	pWnd = GetDlgItem ( IDC_EDIT_RAIN_PARTCOUNT );
	_gcvt ( m_RainPointProperty.m_RainParticleData.Count, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR1 );
	_gcvt ( m_RainPointProperty.m_RainParticleData.fMinDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR2 );
	_gcvt ( m_RainPointProperty.m_RainParticleData.fMaxDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED1 );
	_gcvt ( m_RainPointProperty.m_RainParticleData.fMinFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED2 );
	_gcvt ( m_RainPointProperty.m_RainParticleData.fMaxFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// 낙엽 효과를 위한..
	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_PARTCOUNT );
	_gcvt ( m_RainPointProperty.m_LeavesParticleData.Count, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR1 );
	_gcvt ( m_RainPointProperty.m_LeavesParticleData.fMinDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR2 );
	_gcvt ( m_RainPointProperty.m_LeavesParticleData.fMaxDisappear, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED1 );
	_gcvt ( m_RainPointProperty.m_LeavesParticleData.fMinFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED2 );
	_gcvt ( m_RainPointProperty.m_LeavesParticleData.fMaxFallSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );


	// 새로운 비효과 추가를 위해
	ChangeEnableWindow( TRUE );


	

}

//void CRainPointEffEdit::OnBnClickedButtonSandGTexture()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
//	CFileDialog dlg ( TRUE, ".BMP", NULL, 
//		OFN_HIDEREADONLY, szFilter, this );
//
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
//
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString FilePath = dlg.GetFileName ();
//
//		CWnd *pWnd;
//		pWnd = GetDlgItem ( IDC_EDIT_SAND_G_TEXTURE );
//		pWnd->SetWindowText ( FilePath );
//	}
//}
//
//void CRainPointEffEdit::OnBnClickedButtonRainGTexture()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
//	CFileDialog dlg ( TRUE, ".BMP", NULL, 
//		OFN_HIDEREADONLY, szFilter, this );
//
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
//
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString FilePath = dlg.GetFileName ();
//
//		CWnd *pWnd;
//		pWnd = GetDlgItem ( IDC_EDIT_RAIN_G_TEXTURE );
//		pWnd->SetWindowText ( FilePath );
//	}
//}
//
//void CRainPointEffEdit::OnBnClickedButtonSnowGTexture()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
//	CFileDialog dlg ( TRUE, ".BMP", NULL, 
//		OFN_HIDEREADONLY, szFilter, this );
//
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
//
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString FilePath = dlg.GetFileName ();
//
//		CWnd *pWnd;
//		pWnd = GetDlgItem ( IDC_EDIT_SNOW_G_TEXTURE );
//		pWnd->SetWindowText ( FilePath );
//	}
//}
//
//void CRainPointEffEdit::OnBnClickedButtonLeavesGTexture()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
//	CFileDialog dlg ( TRUE, ".BMP", NULL, 
//		OFN_HIDEREADONLY, szFilter, this );
//
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
//
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString FilePath = dlg.GetFileName ();
//
//		CWnd *pWnd;
//		pWnd = GetDlgItem ( IDC_EDIT_LEAVES_G_TEXTURE );
//		pWnd->SetWindowText ( FilePath );
//	}
//}

void CRainPointEffEdit::OnBnClickedButtonRainTexture()
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
		pWnd = GetDlgItem ( IDC_EDIT_RAIN_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRainPointEffEdit::OnBnClickedButtonTexture()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRainPointEffEdit::OnBnClickedButtonTexture2()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRainPointEffEdit::OnBnClickedButtonTexture4()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE3 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRainPointEffEdit::OnBnClickedButtonTexture5()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE4 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRainPointEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_RATE );
	pWnd->GetWindowText ( strText );
	m_RainPointProperty.m_fRainRate = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SNOW_SIZE );
	pWnd->GetWindowText ( strText );
	m_RainPointProperty.m_fSnowSize = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_SIZE );
	pWnd->GetWindowText ( strText );
	m_RainPointProperty.m_fLeavesSize = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LEAVES_SPEED );
	pWnd->GetWindowText ( strText );
	m_RainPointProperty.m_fLeavesSpeed = (float)atof ( strText.GetString() );

	//pWnd = GetDlgItem ( IDC_EDIT_GROUND_SIZE );
	//pWnd->GetWindowText ( strText );
	//m_RainPointProperty.m_fGroundSize = (float)atof ( strText.GetString() );

	CString strTexture;

	//pWnd = GetDlgItem ( IDC_EDIT_SAND_G_TEXTURE );
	//pWnd->GetWindowText ( strTexture );
	//szTexture = strTexture.GetString(256);
	//strcpy ( m_RainPointProperty.m_szGroundTex, szTexture );

	//pWnd = GetDlgItem ( IDC_EDIT_RAIN_G_TEXTURE );
	//pWnd->GetWindowText ( strTexture );
	//szTexture = strTexture.GetString(256);
	//strcpy ( m_RainPointProperty.m_szRain_GTex, szTexture );

	//pWnd = GetDlgItem ( IDC_EDIT_SNOW_G_TEXTURE );
	//pWnd->GetWindowText ( strTexture );
	//szTexture = strTexture.GetString(256);
	//strcpy ( m_RainPointProperty.m_szSnow_GTex, szTexture );

	//pWnd = GetDlgItem ( IDC_EDIT_LEAVES_G_TEXTURE );
	//pWnd->GetWindowText ( strTexture );
	//szTexture = strTexture.GetString(256);
	//strcpy ( m_RainPointProperty.m_szLeaves_GTex, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_RAIN_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_RainPointProperty.m_szRainTex, strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_RainPointProperty.m_szCircleTex, strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE2 );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_RainPointProperty.m_szRainDropTex, strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE3 );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_RainPointProperty.m_szSnowTex, strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE4 );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_RainPointProperty.m_szLeavesTex, strTexture.GetString() );

	CButton* pButton = (CButton *)GetDlgItem ( IDC_CHECK_NEWEFFECT );
	if( pButton->GetCheck() )
	{
		m_RainPointProperty.m_bNewRainEffect = TRUE;
		GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME )->SetWindowText ( "[ 새로운 빗방울, 눈, 낙엽 ]" );

		GetDlgItem ( IDC_EDIT_PARTCOUNT )->GetWindowText( strTexture );
		m_RainPointProperty.m_SnowParticleData.Count = atoi( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_DISAPPEAR1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_SnowParticleData.fMinDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_DISAPPEAR2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_SnowParticleData.fMaxDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_FALLSPEED1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_SnowParticleData.fMinFallSpeed = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_FALLSPEED2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_SnowParticleData.fMaxFallSpeed = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_RAIN_PARTCOUNT )->GetWindowText( strTexture );
		m_RainPointProperty.m_RainParticleData.Count = atoi( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_RainParticleData.fMinDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_RainParticleData.fMaxDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_RainParticleData.fMinFallSpeed = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_RainParticleData.fMaxFallSpeed = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_LEAVES_PARTCOUNT )->GetWindowText( strTexture );
		m_RainPointProperty.m_LeavesParticleData.Count = atoi( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_LeavesParticleData.fMinDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_LeavesParticleData.fMaxDisappear = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED1 )->GetWindowText( strTexture );
		m_RainPointProperty.m_LeavesParticleData.fMinFallSpeed = (float)atof( strTexture.GetString() );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED2 )->GetWindowText( strTexture );
		m_RainPointProperty.m_LeavesParticleData.fMaxFallSpeed = (float)atof( strTexture.GetString() );
	}else{
		m_RainPointProperty.m_bNewRainEffect = FALSE;
		GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME )->SetWindowText ( "[ 빗방울, 눈, 낙엽 ]" );
	}


	//	Note : 속성을 적용.
	//
	m_pRainPointEff->SetProperty ( m_RainPointProperty );
	m_pRainPointEff->CleanUp ();

//	pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
//	pWnd->SetWindowText ( m_pRainPointEff->GetName() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pRainPointEff->Create ( pView->GetD3DDevice() );
}

void CRainPointEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CRainPointEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pRainPointEff->SetProperty ( m_OldRainPointProperty );
	m_pRainPointEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pRainPointEff->Create ( pView->GetD3DDevice() );
}

void CRainPointEffEdit::OnBnClickedCheckNeweffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();
	ChangeEnableWindow( FALSE );
}

void CRainPointEffEdit::ChangeEnableWindow( bool bValue )
{
	CButton* pButton = (CButton *)GetDlgItem ( IDC_CHECK_NEWEFFECT );

	bool bNewRainEffect = FALSE;
	if( m_RainPointProperty.m_bNewRainEffect && bValue )
		bNewRainEffect = TRUE;

	if( pButton->GetCheck() && bValue == FALSE )
		bNewRainEffect = TRUE;

	if( bNewRainEffect )
	{
		pButton->SetCheck( TRUE );
		GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME )->SetWindowText ( "[ 새로운 빗방울, 눈, 낙엽 ]" );

		GetDlgItem ( IDC_EDIT_PARTCOUNT )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_DISAPPEAR1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_DISAPPEAR2 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_FALLSPEED1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_FALLSPEED2 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_RAIN_PARTCOUNT )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR2 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED2 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_PARTCOUNT )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR2 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED1 )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED2 )->EnableWindow( TRUE );


		GetDlgItem ( IDC_EDIT_RATE )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_SNOW_SIZE )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_SIZE )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_SPEED )->EnableWindow( FALSE );

	}else{
		pButton->SetCheck( FALSE );
		GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME )->SetWindowText ( "[ 빗방울, 눈, 낙엽 ]" );

		GetDlgItem ( IDC_EDIT_PARTCOUNT )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_DISAPPEAR1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_DISAPPEAR2 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_FALLSPEED1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_FALLSPEED2 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_RAIN_PARTCOUNT )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_RAIN_DISAPPEAR2 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_RAIN_FALLSPEED2 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_PARTCOUNT )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_DISAPPEAR2 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED1 )->EnableWindow( FALSE );

		GetDlgItem ( IDC_EDIT_LEAVES_FALLSPEED2 )->EnableWindow( FALSE );


		GetDlgItem ( IDC_EDIT_RATE )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_SNOW_SIZE )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_SIZE )->EnableWindow( TRUE );

		GetDlgItem ( IDC_EDIT_LEAVES_SPEED )->EnableWindow( TRUE );
	}
}

