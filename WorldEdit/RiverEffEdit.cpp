// RiverEffEdit.cpp : ���� �����Դϴ�.
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
#include "../EngineLib/DxEffect/Frame/DxEffectRiver.h"

#include "RiverEffEdit.h"
#include ".\rivereffedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern	BOOL	g_bDebugRiverWireFrame;

// RiverEffEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CRiverEffEdit, CPropertyPage)
CRiverEffEdit::CRiverEffEdit()
	: CPropertyPage(CRiverEffEdit::IDD)
{
	m_pEff = NULL;
}

CRiverEffEdit::~CRiverEffEdit()
{
}

void CRiverEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRiverEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_DARK, OnBnClickedCheckDark)
	ON_BN_CLICKED(IDC_CHECK_FLASH, OnBnClickedCheckFlash)
	ON_BN_CLICKED(IDC_CHECK_REFLECT, OnBnClickedCheckReflect)
	ON_BN_CLICKED(IDC_BUTTON_DARK_DIFFUSE, OnBnClickedButtonDarkDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_FLASH_DIFFUSE, OnBnClickedButtonFlashDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_DARK_TEX, OnBnClickedButtonDarkTex)
	ON_BN_CLICKED(IDC_BUTTON_FLASH_TEX, OnBnClickedButtonFlashTex)
	ON_BN_CLICKED(IDC_RADIO_OCEANE, OnBnClickedRadioOceane)
	ON_BN_CLICKED(IDC_RADIO_TRANS_LIGHT, OnBnClickedRadioTransLight)
	ON_BN_CLICKED(IDC_CHECK_SEE, OnBnClickedCheckSee)
	ON_BN_CLICKED(IDC_CHECK_SAME_HEIGHT, OnBnClickedCheckSameHeight)
END_MESSAGE_MAP()


// RiverEffEdit �޽��� ó�����Դϴ�.
void CRiverEffEdit::SetInstance ( DxEffectRiver *pEff )
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView *)pFrame->GetActiveView();
	//pView->SetActiveEffect( pEff );

	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	CWnd*		pWnd;
	CButton*	pButton;

	if ( m_pEff->m_bPS14_EDIT )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TRANS_LIGHT );
		pButton->SetCheck ( FALSE );
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_OCEANE );
		pButton->SetCheck ( TRUE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TRANS_LIGHT );
		pButton->SetCheck ( TRUE );
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_OCEANE );
		pButton->SetCheck ( FALSE );
	}

	m_pEff->m_bEnable_EDIT = TRUE;		// Ȱ��ȭ ����

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DARK );
	if ( m_Property.m_dwFlag&USEDARK )			pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_FLASH );
	if ( m_Property.m_dwFlag&USEFLASH )			pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_REFLECT );
	if ( m_Property.m_dwFlag&USEREFLECT )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_REFLECT_NEW );
	if ( m_Property.m_dwFlag&USEREFLECT_NEW )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SEE );
	if ( m_Property.m_dwFlag&USESEE )			pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SAME_HEIGHT );
	if ( m_Property.m_dwFlag&USESAMEHEIGHT )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );


	char szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SCALE_U );
	_gcvt ( m_Property.m_fDarkScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	//pWnd = GetDlgItem ( IDC_EDIT_DARK_SCALE_V );
	//_gcvt ( m_Property.m_vDarkScale.y, 7, szValue );
	//pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SPEED_U );
	_gcvt ( m_Property.m_vDarkVel.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SPEED_V );
	_gcvt ( m_Property.m_vDarkVel.y, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_FLASH_SCALE_U );
	_gcvt ( m_Property.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	//pWnd = GetDlgItem ( IDC_EDIT_FLASH_SCALE_V );
	//_gcvt ( m_Property.m_vScale.y, 7, szValue );
	//pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_FLASH_SPEED );
	_gcvt ( m_Property.m_fVel, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_ALPHA );
	_gcvt ( m_Property.m_fBumpAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_DARK_TEX );
	pWnd->SetWindowText ( m_Property.m_szTexDark );

	pWnd = GetDlgItem ( IDC_EDIT_FLASH_TEX );
	pWnd->SetWindowText ( m_Property.m_szTexFlash );

	//	Note : �� ����.
	//
	pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );
}

void CRiverEffEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString strText;
	CWnd*		pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SCALE_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_fDarkScale = (float) atof ( strText.GetString() );

	//pWnd = GetDlgItem ( IDC_EDIT_DARK_SCALE_V );
	//pWnd->GetWindowText ( strText );
	//m_Property.m_vDarkScale.y = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SPEED_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vDarkVel.x = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DARK_SPEED_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vDarkVel.y = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_FLASH_SCALE_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_fScale = (float) atof ( strText.GetString() );

	//pWnd = GetDlgItem ( IDC_EDIT_FLASH_SCALE_V );
	//pWnd->GetWindowText ( strText );
	//m_Property.m_vScale.y = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_FLASH_SPEED );
	pWnd->GetWindowText ( strText );
	m_Property.m_fVel = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_Property.m_fBumpAlpha = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DARK_TEX );
	pWnd->GetWindowText ( strText );
	strcpy ( m_Property.m_szTexDark, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_FLASH_TEX );
	pWnd->GetWindowText ( strText );
	strcpy ( m_Property.m_szTexFlash, strText.GetString() );

	//	Note : �Ӽ��� ����.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );

	m_pEff->ReSetDiffuse ();
}

void CRiverEffEdit::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : �ǵ���
	//
	g_bDebugRiverWireFrame	= FALSE;
	m_pEff->m_bEnable_EDIT = FALSE;
	m_pEff->m_bPS14_EDIT = FALSE;

	//	Note : ��Ʈ���� ������
	//
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
//	pView->ResetActiveEffect();
}

void CRiverEffEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( 0 );

	m_pEff->m_bEnable_EDIT = FALSE;
	m_pEff->m_bPS14_EDIT = FALSE;

	//	Note : �Ӽ��� �ǵ���.
	//
	m_pEff->SetProperty ( m_OldProperty );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );

//	//	Note : CView ���� Active Effect ���� ��� - ��Ʈ���� ������
//	//
//	pView->ResetActiveEffect();
}

void CRiverEffEdit::OnBnClickedCheckDark()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_DARK);
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USEDARK;
	else						m_Property.m_dwFlag &= ~USEDARK;
}

void CRiverEffEdit::OnBnClickedCheckFlash()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_FLASH);
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USEFLASH;
	else						m_Property.m_dwFlag &= ~USEFLASH;
}

void CRiverEffEdit::OnBnClickedCheckReflect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_REFLECT);
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USEREFLECT;
	else						m_Property.m_dwFlag &= ~USEREFLECT;
}

void CRiverEffEdit::OnBnClickedButtonDarkDiffuse()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	D3DXVECTOR3 vColor = m_Property.m_vDarkColor;
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
		m_Property.m_vDarkColor = D3DXVECTOR3 ( (float)R, (float)G, (float)B );

		m_pEff->SetProperty ( m_Property );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

		m_pEff->ReSetDiffuse ();
	}
}

void CRiverEffEdit::OnBnClickedButtonFlashDiffuse()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	D3DXVECTOR3 vColor = m_Property.m_vColor;
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
		m_Property.m_vColor = D3DXVECTOR3 ( (float)R, (float)G, (float)B );

		m_pEff->SetProperty ( m_Property );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

		m_pEff->ReSetDiffuse ();
	}
}

void CRiverEffEdit::OnBnClickedButtonDarkTex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_DARK_TEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRiverEffEdit::OnBnClickedButtonFlashTex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FLASH_TEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CRiverEffEdit::OnBnClickedRadioOceane()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pEff->m_bPS14_EDIT = TRUE;
}

void CRiverEffEdit::OnBnClickedRadioTransLight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pEff->m_bPS14_EDIT = FALSE;
}

void CRiverEffEdit::OnBnClickedCheckSee()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEE);
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USESEE;
	else						m_Property.m_dwFlag &= ~USESEE;
}

void CRiverEffEdit::OnBnClickedCheckSameHeight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_SAME_HEIGHT);
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USESAMEHEIGHT;
	else						m_Property.m_dwFlag &= ~USESAMEHEIGHT;
}
