// SporeEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "ColorPickerDlg.h"
#include "SheetWithTab.h"
#include "SetMultiSlider.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpore.h"

#include "SporeEffEdit.h"
#include ".\sporeeffedit.h"

// CSporeEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSporeEffEdit, CPropertyPage)
CSporeEffEdit::CSporeEffEdit()
	: CPropertyPage(CSporeEffEdit::IDD)
{
}

CSporeEffEdit::~CSporeEffEdit()
{
}

void CSporeEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSporeEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE6, OnBnClickedButtonTexture6)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTONCOLOR, OnBnClickedButtoncolor)
	ON_BN_CLICKED(IDC_RADIO_FLAT, OnBnClickedRadioFlat)
	ON_BN_CLICKED(IDC_RADIO_SLANT, OnBnClickedRadioSlant)
	ON_BN_CLICKED(IDC_CHECK_TEXROTATE, OnBnClickedCheckTexrotate)
	ON_BN_CLICKED(IDC_CHECK_SPORE, OnBnClickedCheckSpore)
	ON_BN_CLICKED(IDC_CHECK_FIREFLY, OnBnClickedCheckFirefly)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_2, OnBnClickedButtonColor2)
END_MESSAGE_MAP()


// CSporeEffEdit 메시지 처리기입니다.
void CSporeEffEdit::SetInstance ( DxEffectSpore *pEff )
{
	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );



	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SPORE );
	if ( m_Property.m_dwFlags & USESPORE )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_FIREFLY );
	if ( m_Property.m_dwFlags & USEFIREFLY )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );



	if ( m_Property.m_nFlat == 0 )
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_FLAT );
		pButton->SetCheck (TRUE);
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLANT );
		pButton->SetCheck (FALSE);
	}
	else
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_FLAT );
		pButton->SetCheck (FALSE);
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLANT );
		pButton->SetCheck (TRUE);
	}

	//	회전
	if ( m_Property.m_bRotate == TRUE )
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_TEXROTATE );
		pButton->SetCheck (TRUE);

		pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_TEXROTATE );
		pButton->SetCheck (FALSE);

		pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
		pWnd->EnableWindow ( FALSE );
	}


	char szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	_gcvt ( m_Property.m_nSec, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	_gcvt ( m_Property.m_fSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	_gcvt ( m_Property.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
	_gcvt ( m_Property.m_fRotate, 7, szValue );
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szSporeTex );
}

void CSporeEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;
	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	pWnd->GetWindowText ( strText );
	m_Property.m_nSec = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_Property.m_fSpeed = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fScale = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fRotate = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_Property.m_szSporeTex, strText.GetString() );

	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );

	m_pEff->ResetEdit();
}

void CSporeEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CSporeEffEdit::OnBnClickedButtonCancel()
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
}

void CSporeEffEdit::OnBnClickedButtonColor1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DXVECTOR3 vColor;
	vColor.x = m_Property.m_vFireFly_Color_1.x;
	vColor.y = m_Property.m_vFireFly_Color_1.y;
	vColor.z = m_Property.m_vFireFly_Color_1.z;
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
		m_Property.m_vFireFly_Color_1 = D3DXVECTOR4 ( (float)R, (float)G, (float)B, 255.f );

		m_pEff->SetProperty ( m_Property );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	}
}

void CSporeEffEdit::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DXVECTOR3 vColor;
	vColor.x = m_Property.m_vFireFly_Color_2.x;
	vColor.y = m_Property.m_vFireFly_Color_2.y;
	vColor.z = m_Property.m_vFireFly_Color_2.z;
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
		m_Property.m_vFireFly_Color_2 = D3DXVECTOR4 ( (float)R, (float)G, (float)B, 0.f );

		m_pEff->SetProperty ( m_Property );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	}
}

void CSporeEffEdit::OnBnClickedButtonTexture6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CSporeEffEdit::OnBnClickedButtonAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Alpha";
	DlgSetSlider.m_StrTemp4 = "알파값";

	DlgSetSlider.m_values[0] = m_Property.m_fAlphaRate1;
	DlgSetSlider.m_values[1] = m_Property.m_fAlphaRate2;

	DlgSetSlider.m_fStart = m_Property.m_fAlphaStart;
	DlgSetSlider.m_fMid1 = m_Property.m_fAlphaMid1;
	DlgSetSlider.m_fMid2 = m_Property.m_fAlphaMid2;
	DlgSetSlider.m_fEnd = m_Property.m_fAlphaEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_Property.m_fAlphaRate1 = DlgSetSlider.m_values[0];
		m_Property.m_fAlphaRate2 = DlgSetSlider.m_values[1];
	
		m_Property.m_fAlphaStart = DlgSetSlider.m_fStart;
		m_Property.m_fAlphaMid1 = DlgSetSlider.m_fMid1;
		m_Property.m_fAlphaMid2 = DlgSetSlider.m_fMid2;
		m_Property.m_fAlphaEnd = DlgSetSlider.m_fEnd;

		m_pEff->ResetEdit();
	}
}

void CSporeEffEdit::OnBnClickedButtoncolor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

		m_pEff->ResetEdit();
	}
}

void CSporeEffEdit::OnBnClickedRadioFlat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FLAT );
	pButton->SetCheck (TRUE);
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLANT );
	pButton->SetCheck (FALSE);

	m_Property.m_nFlat = 0;
}

void CSporeEffEdit::OnBnClickedRadioSlant()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FLAT );
	pButton->SetCheck (FALSE);
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLANT );
	pButton->SetCheck (TRUE);

	m_Property.m_nFlat = 1;
}

void CSporeEffEdit::OnBnClickedCheckTexrotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	CWnd	*pWnd;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TEXROTATE );
	if ( pButton->GetCheck () )
	{
		m_Property.m_bRotate	= TRUE;
		pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_bRotate	= FALSE;
		pWnd = GetDlgItem ( IDC_EDIT_TEXROTATE );
		pWnd->EnableWindow ( FALSE );
	}
}

void CSporeEffEdit::OnBnClickedCheckSpore()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SPORE );

	if ( pButton->GetCheck () )		m_Property.m_dwFlags	|= USESPORE;
	else							m_Property.m_dwFlags	&= ~USESPORE;
}

void CSporeEffEdit::OnBnClickedCheckFirefly()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_FIREFLY );

	if ( pButton->GetCheck () )		m_Property.m_dwFlags	|= USEFIREFLY;
	else							m_Property.m_dwFlags	&= ~USEFIREFLY;
}
