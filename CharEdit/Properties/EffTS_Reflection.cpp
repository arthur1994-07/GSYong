// EffTS_Reflection.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxEffect/Char/DxEffCharTotalShader.h"

#include "sheetWithTabChar.h"
#include "sheetWithTabTS.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "sheetWithTabTS.h"
#include "EffTS_Reflection.h"
#include ".\effts_reflection.h"


// CEffTS_Reflection 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffTS_Reflection, CPropertyPage)
CEffTS_Reflection::CEffTS_Reflection()
	: CPropertyPage(CEffTS_Reflection::IDD)
{
}

CEffTS_Reflection::~CEffTS_Reflection()
{
}

void CEffTS_Reflection::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffTS_Reflection, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_REFLECTTEX, OnBnClickedButtonTextureReflecttex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_REFLECTTEX2, OnBnClickedButtonTextureReflecttex2)
END_MESSAGE_MAP()


// CEffTS_Reflection 메시지 처리기입니다.
void CEffTS_Reflection::Apply()
{
	m_PropReflection.fReflectActivity = GetWin_Num_float( this,IDC_EDIT_REFLECTACT );
	m_PropReflection.bUseReflectAlpha  = GetWin_Check( this, IDC_CHECK_REFLECTTEX2 );
	StringCchCopy( m_PropReflection.szReflectTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX ).GetString() );
	StringCchCopy( m_PropReflection.szReflectAlphaTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2 ).GetString() );
}


void CEffTS_Reflection::SetProperty( EFFCHAR_PROPERTY_TS_REFLECTION Property ) 
{
	m_PropReflection = Property; 

	SetWin_Num_float( this,IDC_EDIT_REFLECTACT, m_PropReflection.fReflectActivity );
	SetWin_Check( this, IDC_CHECK_REFLECTTEX2, m_PropReflection.bUseReflectAlpha );
	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_PropReflection.szReflectTex );
	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_PropReflection.szReflectAlphaTex );
}

void CEffTS_Reflection::OnBnClickedButtonTextureReflecttex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_REFLECTTEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTS_Reflection::OnBnClickedButtonTextureReflecttex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_REFLECTTEX2 );
		pWnd->SetWindowText ( FilePath );
	}
}
