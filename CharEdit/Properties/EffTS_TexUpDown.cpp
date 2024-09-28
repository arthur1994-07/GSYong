// EffTS_TexUpDown.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxEffect/Char/DxEffCharTotalShader.h"

#include "sheetWithTabTS.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "sheetWithTabTS.h"
#include "EffTS_TexUpDown.h"
#include ".\effts_texupdown.h"


// CEffTS_TexUpDown 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffTS_TexUpDown, CPropertyPage)
CEffTS_TexUpDown::CEffTS_TexUpDown()
	: CPropertyPage(CEffTS_TexUpDown::IDD)
{
}

CEffTS_TexUpDown::~CEffTS_TexUpDown()
{
}

void CEffTS_TexUpDown::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffTS_TexUpDown, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_GLOWTEX, OnBnClickedButtonTextureGlowtex)
END_MESSAGE_MAP()


// CEffTS_TexUpDown 메시지 처리기입니다.
void CEffTS_TexUpDown::Apply()
{
	m_PropTexUpDown.fTexUpDownBright   = GetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT );
	m_PropTexUpDown.fTexUpDownAlpha	  = GetWin_Num_float( this, IDC_EDIT_GLOWALPHA );
	m_PropTexUpDown.fTexUpDownAniSpeed = GetWin_Num_float( this, IDC_EDIT_GLOWSPEED );
	StringCchCopy( m_PropTexUpDown.szTexUpDownTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX ).GetString() );
}

void CEffTS_TexUpDown::SetProperty( EFFCHAR_PROPERTY_TS_TEXUPDOWN Property ) 
{
	m_PropTexUpDown = Property; 

	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_PropTexUpDown.fTexUpDownBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWALPHA,  m_PropTexUpDown.fTexUpDownAlpha );
	SetWin_Num_float( this, IDC_EDIT_GLOWSPEED,  m_PropTexUpDown.fTexUpDownAniSpeed );
	SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, m_PropTexUpDown.szTexUpDownTex );

}


void CEffTS_TexUpDown::OnBnClickedButtonTextureGlowtex()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_GLOWTEX );
		pWnd->SetWindowText ( FilePath );
	}
}
