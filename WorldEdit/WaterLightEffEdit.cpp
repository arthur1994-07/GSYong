// WaterLightEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "ColorPickerDlg.h"
#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectWaterLight.h"

#include "WaterLightEffEdit.h"

// CWaterLightEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaterLightEffEdit, CPropertyPage)
CWaterLightEffEdit::CWaterLightEffEdit()
	: CPropertyPage(CWaterLightEffEdit::IDD),
	m_pWaterEff(NULL)
{
}

CWaterLightEffEdit::~CWaterLightEffEdit()
{
}

void CWaterLightEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaterLightEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_MOVE, OnBnClickedButtonTextureMove)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_ROTATE, OnBnClickedButtonTextureRotate)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_WINDDIR, OnBnClickedButtonWinddir)
END_MESSAGE_MAP()


// CWaterLightEffEdit 메시지 처리기입니다.
void CWaterLightEffEdit::SetInstance ( DxEffectWaterLight *pWaterEff )
{
	m_pWaterEff = pWaterEff;
	m_OldWaterProperty = m_WaterProperty = m_pWaterEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pWaterEff->GetName() );

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	_gcvt ( m_WaterProperty.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// 바람
	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	_gcvt ( m_WaterProperty.m_fVel, 7, szValue );
	pWnd->SetWindowText ( szValue );

	_gcvt ( m_WaterProperty.m_vDir.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
	_gcvt ( m_WaterProperty.m_vDir.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
	_gcvt ( m_WaterProperty.m_vDir.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

	// 디퓨즈
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( m_WaterProperty.m_iAlphaUp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA2 );
	_gcvt ( m_WaterProperty.m_iAlphaMiddle, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA3 );
	_gcvt ( m_WaterProperty.m_iAlphaDown, 7, szValue );
	pWnd->SetWindowText ( szValue );

	_gcvt ( m_WaterProperty.m_vColor.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	_gcvt ( m_WaterProperty.m_vColor.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	_gcvt ( m_WaterProperty.m_vColor.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);



	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_MOVE );
	pWnd->SetWindowText ( m_WaterProperty.m_szTexMove );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_ROTATE );
	pWnd->SetWindowText ( m_WaterProperty.m_szTexRotate );

}
void CWaterLightEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;

	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_fScale = (float)atof ( strText.GetString() );

	// 바람
	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_fVel = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRX );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_vDir.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRY );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_vDir.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRZ );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_vDir.z = (float)atof ( strText.GetString() );

	// 디퓨즈
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_iAlphaUp = atoi ( strText.GetString() );

    pWnd = GetDlgItem ( IDC_EDIT_ALPHA2 );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_iAlphaMiddle = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA3 );
	pWnd->GetWindowText ( strText );
	m_WaterProperty.m_iAlphaDown = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DR );
	pWnd->GetWindowText ( strText );
	int fDR = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DG );
	pWnd->GetWindowText ( strText );
	int fDG = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DB );
	pWnd->GetWindowText ( strText );
	int fDB = atoi ( strText.GetString() );

	m_WaterProperty.m_vColor = D3DXVECTOR3 ( (float)fDR, (float)fDG, (float)fDB );


	// 텍스쳐
	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_MOVE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_WaterProperty.m_szTexMove, strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_ROTATE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_WaterProperty.m_szTexRotate, strTexture.GetString() );

	//	Note : 속성을 적용.
	//
	m_pWaterEff->SetProperty ( m_WaterProperty );
	m_pWaterEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pWaterEff->Create ( pView->GetD3DDevice() );

	m_pWaterEff->ReMakeWaterLight ( pView->GetD3DDevice() );
}

void CWaterLightEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	m_pSheetTab->SetActivePage ( 0 );
	m_pWaterEff = NULL;
}

void CWaterLightEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pWaterEff->SetProperty ( m_OldWaterProperty );
	m_pWaterEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pWaterEff->Create ( pView->GetD3DDevice() );

	m_pSheetTab->SetActivePage ( 0 );
	m_pWaterEff = NULL;
}

void CWaterLightEffEdit::OnBnClickedButtonTextureMove()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_MOVE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CWaterLightEffEdit::OnBnClickedButtonTextureRotate()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_ROTATE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CWaterLightEffEdit::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	D3DXVECTOR3 vColor = m_WaterProperty.m_vColor;
	COLORREF retColor;
	COLORREF Color = RGB ( vColor.x, vColor.y, vColor.z );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		int	R,G,B;
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_WaterProperty.m_vColor = D3DXVECTOR3 ( (float)R, (float)G, (float)B );

		char szValue[128];
		sprintf ( szValue, "%d", R );
		GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
		sprintf ( szValue, "%d", G );
		GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
		sprintf ( szValue, "%d", B );
		GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);

		m_pWaterEff->SetProperty ( m_WaterProperty );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		m_pWaterEff->ReMakeWaterLight ( pView->GetD3DDevice() );
	}
}

void CWaterLightEffEdit::OnBnClickedButtonWinddir()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_WaterProperty.m_vDir, WATERLIGHTPAGEINDEX );
}

BOOL CWaterLightEffEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( m_pWaterEff )
	{
		char szValue[256];

		_gcvt ( m_WaterProperty.m_vDir.x, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
		_gcvt ( m_WaterProperty.m_vDir.y, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
		_gcvt ( m_WaterProperty.m_vDir.z, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

		m_pWaterEff->SetProperty ( m_WaterProperty );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		m_pWaterEff->ReMakeWaterLight ( pView->GetD3DDevice() );
	}

	return CPropertyPage::OnSetActive();
}
