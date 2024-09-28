// MaterialPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinDefine.h"
#include "../Enginelib/DxEffect/Char/DxEffChar.h"

#include "SkinInfoPage.h"


// CSkinInfoPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkinInfoPage, CPropertyPage)
CSkinInfoPage::CSkinInfoPage()
	: CPropertyPage(CSkinInfoPage::IDD)
{
}

CSkinInfoPage::~CSkinInfoPage()
{
}

void CSkinInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSkinInfoPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FACE_SPEC_COLOR, OnBnClickedButtonFaceSpecColor)
	ON_BN_CLICKED(IDC_BUTTON_BODY_SPEC_COLOR, OnBnClickedButtonBodySpecColor)
	ON_BN_CLICKED(IDC_BUTTON_BODY_SPEC2_COLOR, OnBnClickedButtonBodySpec2Color)
	ON_EN_CHANGE(IDC_EDIT_FACE_SPEC_POWER, &OnEnChangeEditFaceSpecPower)
	ON_EN_CHANGE(IDC_EDIT_BODY_SPEC_POWER, &OnEnChangeEditBodySpecPower)
	ON_EN_CHANGE(IDC_EDIT_BODY_SPEC2_POWER, &OnEnChangeEditBodySpec2Power)
	ON_BN_CLICKED(IDC_BUTTON_FAKE_SSS_COLOR, OnBnClickedButtonFakeSSSColor)
	//ON_EN_CHANGE(IDC_EDIT_SSS_COLOR_R, &OnEnChangeEditSSS_ColorR)
	//ON_EN_CHANGE(IDC_EDIT_SSS_COLOR_G, &OnEnChangeEditSSS_ColorG)
	//ON_EN_CHANGE(IDC_EDIT_SSS_COLOR_B, &OnEnChangeEditSSS_ColorB)
END_MESSAGE_MAP()


// CSkinInfoPage 메시지 처리기입니다.

BOOL CSkinInfoPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( pSkinInfo )
	{
		CWnd* pWnd;
		CString Str;
		pWnd = GetDlgItem( IDC_EDIT_FACE_SPEC_POWER );
		Str.Format ( _T("%3.1f"), pSkinInfo->m_vFaceSpecColor_Power.w );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_BODY_SPEC_POWER );
		Str.Format ( _T("%3.1f"), pSkinInfo->m_vBodySpecColor_Power.w );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_R );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.x );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_G );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.y );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_B );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.z );
		pWnd->SetWindowText( Str.GetString() );
	}

	return CPropertyPage::OnSetActive();
}


void CSkinInfoPage::OnBnClickedOk()
{
	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

SKIN_SPEC_SSS_INFO* CSkinInfoPage::GetSkinInfo()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxSkinChar* pSkinChar = pView->GetSkinChar();
	if ( !pSkinChar )
		return NULL;

	return &pSkinChar->m_sSkinInfo;
}

void CSkinInfoPage::OnBnClickedButtonFaceSpecColor()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	int ThisR = int ( pSkinInfo->m_vFaceSpecColor_Power.x * 255.0f );
	int ThisG = int ( pSkinInfo->m_vFaceSpecColor_Power.y * 255.0f );
	int ThisB = int ( pSkinInfo->m_vFaceSpecColor_Power.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pSkinInfo->m_vFaceSpecColor_Power.x = GetRValue(SelectColor)/255.0f;
		pSkinInfo->m_vFaceSpecColor_Power.y = GetGValue(SelectColor)/255.0f;
		pSkinInfo->m_vFaceSpecColor_Power.z = GetBValue(SelectColor)/255.0f;
	}
}

void CSkinInfoPage::OnBnClickedButtonBodySpecColor()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	int ThisR = int ( pSkinInfo->m_vBodySpecColor_Power.x * 255.0f );
	int ThisG = int ( pSkinInfo->m_vBodySpecColor_Power.y * 255.0f );
	int ThisB = int ( pSkinInfo->m_vBodySpecColor_Power.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pSkinInfo->m_vBodySpecColor_Power.x = GetRValue(SelectColor)/255.0f;
		pSkinInfo->m_vBodySpecColor_Power.y = GetGValue(SelectColor)/255.0f;
		pSkinInfo->m_vBodySpecColor_Power.z = GetBValue(SelectColor)/255.0f;
	}
}

void CSkinInfoPage::OnBnClickedButtonBodySpec2Color()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	int ThisR = int ( pSkinInfo->m_vBodySpec2Color_Power.x * 255.0f );
	int ThisG = int ( pSkinInfo->m_vBodySpec2Color_Power.y * 255.0f );
	int ThisB = int ( pSkinInfo->m_vBodySpec2Color_Power.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pSkinInfo->m_vBodySpec2Color_Power.x = GetRValue(SelectColor)/255.0f;
		pSkinInfo->m_vBodySpec2Color_Power.y = GetGValue(SelectColor)/255.0f;
		pSkinInfo->m_vBodySpec2Color_Power.z = GetBValue(SelectColor)/255.0f;
	}
}

void CSkinInfoPage::OnEnChangeEditFaceSpecPower()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_FACE_SPEC_POWER );
	pWnd->GetWindowText( Str );
	pSkinInfo->m_vFaceSpecColor_Power.w = static_cast<float>( _tstof( Str.GetString() ) );
}

void CSkinInfoPage::OnEnChangeEditBodySpecPower()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_BODY_SPEC_POWER );
	pWnd->GetWindowText( Str );
	pSkinInfo->m_vBodySpecColor_Power.w = static_cast<float>( _tstof( Str.GetString() ) );
}

void CSkinInfoPage::OnEnChangeEditBodySpec2Power()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_BODY_SPEC2_POWER );
	pWnd->GetWindowText( Str );
	pSkinInfo->m_vBodySpec2Color_Power.w = static_cast<float>( _tstof( Str.GetString() ) );
}

void CSkinInfoPage::OnBnClickedButtonFakeSSSColor()
{
	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
	if ( !pSkinInfo )
		return;

	int ThisR = int ( pSkinInfo->m_vSSS_Color.x * 255.0f );
	int ThisG = int ( pSkinInfo->m_vSSS_Color.y * 255.0f );
	int ThisB = int ( pSkinInfo->m_vSSS_Color.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pSkinInfo->m_vSSS_Color.x = GetRValue(SelectColor)/255.0f;
		pSkinInfo->m_vSSS_Color.y = GetGValue(SelectColor)/255.0f;
		pSkinInfo->m_vSSS_Color.z = GetBValue(SelectColor)/255.0f;

		CWnd* pWnd;
		CString Str;
		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_R );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.x );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_G );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.y );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_B );
		Str.Format ( _T("%3.2f"), pSkinInfo->m_vSSS_Color.z );
		pWnd->SetWindowText( Str.GetString() );
	}
}

//void CSkinInfoPage::OnEnChangeEditSSS_ColorR()
//{
//	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
//	if ( !pSkinInfo )
//		return;
//
//	CWnd* pWnd;
//	CString Str;
//	pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_R );
//	pWnd->GetWindowText( Str );
//	pSkinInfo->m_vSSS_Color.x = static_cast<float>( _tstof( Str.GetString() ) );
//}
//
//void CSkinInfoPage::OnEnChangeEditSSS_ColorG()
//{
//	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
//	if ( !pSkinInfo )
//		return;
//
//	CWnd* pWnd;
//	CString Str;
//	pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_G );
//	pWnd->GetWindowText( Str );
//	pSkinInfo->m_vSSS_Color.y = static_cast<float>( _tstof( Str.GetString() ) );
//}
//
//void CSkinInfoPage::OnEnChangeEditSSS_ColorB()
//{
//	SKIN_SPEC_SSS_INFO* pSkinInfo = GetSkinInfo();
//	if ( !pSkinInfo )
//		return;
//
//	CWnd* pWnd;
//	CString Str;
//	pWnd = GetDlgItem( IDC_EDIT_SSS_COLOR_B );
//	pWnd->GetWindowText( Str );
//	pSkinInfo->m_vSSS_Color.z = static_cast<float>( _tstof( Str.GetString() ) );
//}