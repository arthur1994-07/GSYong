// MultiTexEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SetVar.h"
#include "ColorPickerDlg.h"
#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMultiTex.h"

#include "MultiTexEffEdit.h"


// CMultiTexEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMultiTexEffEdit, CPropertyPage)
CMultiTexEffEdit::CMultiTexEffEdit()
	: CPropertyPage(CMultiTexEffEdit::IDD)
{
}

CMultiTexEffEdit::~CMultiTexEffEdit()
{
}

void CMultiTexEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SRCBLEND, m_ComboSrcBlend);
	DDX_Control(pDX, IDC_COMBO_DESTBLEND, m_ComboDestBlend);
}


BEGIN_MESSAGE_MAP(CMultiTexEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_BASE, OnBnClickedCheckBase)
	ON_BN_CLICKED(IDC_CHECK_ADD01, OnBnClickedCheckAdd01)
	ON_BN_CLICKED(IDC_CHECK_ADD02, OnBnClickedCheckAdd02)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE01, OnBnClickedCheckSequence01)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE02, OnBnClickedCheckSequence02)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_ADD01, OnBnClickedButtonTextureAdd01)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_ADD02, OnBnClickedButtonTextureAdd02)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE01, OnBnClickedButtonSequence01)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE02, OnBnClickedButtonSequence02)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
END_MESSAGE_MAP()


// CMultiTexEffEdit 메시지 처리기입니다.
void CMultiTexEffEdit::SetInstance ( DxEffectMultiTex *pEff )
{
	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );

	char		szValue[256];
	CButton*	pButton;

	pWnd = GetDlgItem ( IDC_EDIT_BASE_U );
	_gcvt ( m_Property.m_vMoveTex1.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BASE_V );
	_gcvt ( m_Property.m_vMoveTex1.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ADD01_U );
	_gcvt ( m_Property.m_vMoveTex2.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ADD01_V );
	_gcvt ( m_Property.m_vMoveTex2.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ADD02_U );
	_gcvt ( m_Property.m_vMoveTex3.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ADD02_V );
	_gcvt ( m_Property.m_vMoveTex3.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE01_U );
	_gcvt ( m_Property.m_vScaleTex2.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE01_V );
	_gcvt ( m_Property.m_vScaleTex2.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE02_U );
	_gcvt ( m_Property.m_vScaleTex3.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE02_V );
	_gcvt ( m_Property.m_vScaleTex3.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_01 );
	pWnd->SetWindowText ( m_Property.m_szTexture2 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_02 );
	pWnd->SetWindowText ( m_Property.m_szTexture3 );


	if ( m_Property.m_bTex1 )
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_BASE );
		pButton->SetCheck ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_V );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_BASE );
		pButton->SetCheck ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_V );
		pWnd->EnableWindow ( FALSE );
	}

	if ( m_Property.m_bTex2 )
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD01 );
		pButton->SetCheck ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD01 );
		pWnd->EnableWindow ( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
		pButton->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( FALSE );					// 초기화
	}
	else
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD01 );
		pButton->SetCheck ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD01 );
		pWnd->EnableWindow ( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
		pButton->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( FALSE );	
	}

	if ( m_Property.m_bTex3 )
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD02 );
		pButton->SetCheck ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD02 );
		pWnd->EnableWindow ( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
		pButton->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( FALSE );					// 초기화
	}
	else
	{
		pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD02 );
		pButton->SetCheck ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD02 );
		pWnd->EnableWindow ( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
		pButton->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( FALSE );	
	}

	if ( m_Property.m_bTex2 && m_Property.m_bSequence2 )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( TRUE );
	}

	if ( m_Property.m_bTex3 && m_Property.m_bSequence3 )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( TRUE );
	}

	int nIndex;
	CString Str;

	m_ComboSrcBlend.ResetContent();
	m_ComboDestBlend.ResetContent();

	Str.Format ( "ZERO" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 1 );
	m_ComboDestBlend.SetItemData ( nIndex, 1 );
	Str.Format ( "ONE" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 2 );
	m_ComboDestBlend.SetItemData ( nIndex, 2 );
	Str.Format ( "SRCCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 3 );
	m_ComboDestBlend.SetItemData ( nIndex, 3 );
	Str.Format ( "INVSRCCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 4 );
	m_ComboDestBlend.SetItemData ( nIndex, 4 );
	Str.Format ( "SRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 5 );
	m_ComboDestBlend.SetItemData ( nIndex, 5 );
	Str.Format ( "INVSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 6 );
	m_ComboDestBlend.SetItemData ( nIndex, 6 );
	Str.Format ( "DESTALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 7 );
	m_ComboDestBlend.SetItemData ( nIndex, 7 );
	Str.Format ( "INVDESTALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 8 );
	m_ComboDestBlend.SetItemData ( nIndex, 8 );
	Str.Format ( "DESTCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 9 );
	m_ComboDestBlend.SetItemData ( nIndex, 9 );
	Str.Format ( "INVDESTCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 10 );
	m_ComboDestBlend.SetItemData ( nIndex, 10 );
	Str.Format ( "SRCALPHASAT" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 11 );
	m_ComboDestBlend.SetItemData ( nIndex, 11 );
	Str.Format ( "BOTHSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 12 );
	m_ComboDestBlend.SetItemData ( nIndex, 12 );
	Str.Format ( "BOTHINVSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 13 );
	m_ComboDestBlend.SetItemData ( nIndex, 12 );

	m_ComboSrcBlend.SetCurSel ( m_Property.m_SrcBlend - 1 );
	m_ComboDestBlend.SetCurSel ( m_Property.m_DestBlend - 1 );
}

void CMultiTexEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd*	pWnd;
	CString	strText;

	int nIndex, nData;

	pWnd = GetDlgItem ( IDC_EDIT_BASE_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex1.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BASE_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex1.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ADD01_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex2.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ADD01_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex2.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ADD02_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex3.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ADD02_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vMoveTex3.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE01_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vScaleTex2.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE01_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vScaleTex2.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE02_U );
	pWnd->GetWindowText ( strText );
	m_Property.m_vScaleTex3.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE02_V );
	pWnd->GetWindowText ( strText );
	m_Property.m_vScaleTex3.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_01 );
	pWnd->GetWindowText ( strText );
	strcpy ( m_Property.m_szTexture2, strText );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_02 );
	pWnd->GetWindowText ( strText );
	strcpy ( m_Property.m_szTexture3, strText );

	nIndex = m_ComboSrcBlend.GetCurSel ();
	nData = m_ComboSrcBlend.GetItemData ( nIndex );
	m_Property.m_SrcBlend = nData;

	nIndex = m_ComboDestBlend.GetCurSel ();
	nData = m_ComboDestBlend.GetItemData ( nIndex );
	m_Property.m_DestBlend = nData;

	m_pEff->ResetTexture ( pView->GetD3DDevice() );

	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	m_pEff->Create ( pView->GetD3DDevice() );
}

void CMultiTexEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	m_pSheetTab->SetActivePage ( 0 );
	m_pEff = NULL;
}

void CMultiTexEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pEff->SetProperty ( m_OldProperty );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );

	m_pSheetTab->SetActivePage ( 0 );
	m_pEff = NULL;
}

void CMultiTexEffEdit::OnBnClickedButtonTextureAdd01()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_01 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CMultiTexEffEdit::OnBnClickedButtonTextureAdd02()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_02 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CMultiTexEffEdit::OnBnClickedCheckBase()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.'
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_BASE );

	if ( pButton->GetCheck() )
	{
		m_Property.m_bTex1 = TRUE;

		pWnd = GetDlgItem( IDC_EDIT_BASE_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_V );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_bTex1 = FALSE;

		pWnd = GetDlgItem( IDC_EDIT_BASE_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_BASE_V );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMultiTexEffEdit::OnBnClickedCheckAdd01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD01 );

	if ( pButton->GetCheck() )
	{
		m_Property.m_bTex2 = TRUE;

		pWnd = GetDlgItem( IDC_EDIT_ADD01_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD01 );
		pWnd->EnableWindow ( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
		pButton->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( FALSE );					// 초기화
	}
	else
	{
		m_Property.m_bTex2 = FALSE;

		pWnd = GetDlgItem( IDC_EDIT_ADD01_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD01_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE01_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD01 );
		pWnd->EnableWindow ( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
		pButton->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( FALSE );	
	}

	pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
	if ( m_Property.m_bTex2 && pButton->GetCheck() )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( TRUE );
	}
}

void CMultiTexEffEdit::OnBnClickedCheckAdd02()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_ADD02 );

	if ( pButton->GetCheck() )
	{
		m_Property.m_bTex3 = TRUE;

		pWnd = GetDlgItem( IDC_EDIT_ADD02_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_U );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_V );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD02 );
		pWnd->EnableWindow ( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
		pButton->EnableWindow ( TRUE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( FALSE );	
	}
	else
	{
		m_Property.m_bTex3 = FALSE;

		pWnd = GetDlgItem( IDC_EDIT_ADD02_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ADD02_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_U );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_SIZE02_V );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_TEXTURE_ADD02 );
		pWnd->EnableWindow ( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
		pButton->EnableWindow ( FALSE );
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( FALSE );
	}

	pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
	if ( m_Property.m_bTex3 && pButton->GetCheck() )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( TRUE );
	}
}

void CMultiTexEffEdit::OnBnClickedCheckSequence01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE01 );
	if ( pButton->GetCheck() )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE01 );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMultiTexEffEdit::OnBnClickedCheckSequence02()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_SEQUENCE02 );
	if ( pButton->GetCheck() )
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem( IDC_BUTTON_SEQUENCE02 );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMultiTexEffEdit::OnBnClickedButtonSequence01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Animation";
	DlgSetVar.m_StrUse = "시퀀스사용";
	DlgSetVar.m_StrComm1 = "초";
	DlgSetVar.m_StrComm2 = "행";
	DlgSetVar.m_StrComm3 = "열";
	DlgSetVar.m_StrRadio1 = "단방향";
	DlgSetVar.m_StrRadio2 = "양방향";
	DlgSetVar.m_dwFlag = USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_iRadio = ((m_Property.m_bSequenceLoop2) ? 1 : 0 );
	DlgSetVar.m_fStart = m_Property.m_fAniTime01;
	DlgSetVar.m_fStartVar = (float)m_Property.m_nCol01;
	DlgSetVar.m_fEnd = (float)m_Property.m_nRow01;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		if ( DlgSetVar.m_iRadio == 0 )			m_Property.m_bSequenceLoop2 = FALSE;
		else if ( DlgSetVar.m_iRadio == 1 )		m_Property.m_bSequenceLoop2 = TRUE;

		m_Property.m_fAniTime01 = DlgSetVar.m_fStart;
		m_Property.m_nCol01 = (int)DlgSetVar.m_fStartVar;
		m_Property.m_nRow01 = (int)DlgSetVar.m_fEnd;
	}
}

void CMultiTexEffEdit::OnBnClickedButtonSequence02()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Animation";
	DlgSetVar.m_StrUse = "시퀀스사용";
	DlgSetVar.m_StrComm1 = "초";
	DlgSetVar.m_StrComm2 = "행";
	DlgSetVar.m_StrComm3 = "열";
	DlgSetVar.m_StrRadio1 = "단방향";
	DlgSetVar.m_StrRadio2 = "양방향";
	DlgSetVar.m_dwFlag = USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_iRadio = ((m_Property.m_bSequenceLoop3) ? 1 : 0 );
	DlgSetVar.m_fStart = m_Property.m_fAniTime02;
	DlgSetVar.m_fStartVar = (float)m_Property.m_nCol02;
	DlgSetVar.m_fEnd = (float)m_Property.m_nRow02;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		if ( DlgSetVar.m_iRadio == 0 )			m_Property.m_bSequenceLoop3 = FALSE;
		else if ( DlgSetVar.m_iRadio == 1 )		m_Property.m_bSequenceLoop3 = TRUE;

		m_Property.m_fAniTime02 = DlgSetVar.m_fStart;
		m_Property.m_nCol02 = (int)DlgSetVar.m_fStartVar;
		m_Property.m_nRow02 = (int)DlgSetVar.m_fEnd;
	}
}

void CMultiTexEffEdit::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_Property.m_vColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;
	R = (d3dColor&0xff0000)>>16;
	G = (d3dColor&0xff00)>>8;
	B = d3dColor&0xff;

	if ( IDOK == colorDlg.DoModal () )
	{
		DWORD dwAlpha = (0x00000000);

		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_Property.m_vColor = D3DCOLOR_ARGB ( 255, R, G, B );
	}

	m_pEff->SetProperty ( m_Property );
	
	m_pEff->UpdateDiffuse ();		// 바꾸어 준다.
}

void CMultiTexEffEdit::OnBnClickedButtonAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "알파값 변화";
	DlgSetVar.m_StrComm1 = "알파 평균값";
	DlgSetVar.m_StrComm2 = "알파 변동값";
	DlgSetVar.m_StrComm3 = "알파 속도";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 80.0f;

	DlgSetVar.m_fStart		= m_Property.m_fAlphaAVG;
	DlgSetVar.m_fStartVar	= m_Property.m_fAlphaDelta;
	DlgSetVar.m_fEnd		= m_Property.m_fAlphaSpeed;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_Property.m_fAlphaAVG	= DlgSetVar.m_fStart;
		m_Property.m_fAlphaDelta = DlgSetVar.m_fStartVar;
		m_Property.m_fAlphaSpeed = DlgSetVar.m_fEnd;
	}
}
