// EffTS_NormalMap.cpp : ���� �����Դϴ�.
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
#include "EffTS_NormalMap.h"
#include ".\effts_normalmap.h"


// CEffTS_NormalMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffTS_NormalMap, CPropertyPage)
CEffTS_NormalMap::CEffTS_NormalMap()
	: CPropertyPage(CEffTS_NormalMap::IDD)
{
}

CEffTS_NormalMap::~CEffTS_NormalMap()
{
}

void CEffTS_NormalMap::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffTS_NormalMap, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_NORMAL, OnBnClickedButtonTextureNormal)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SPECULAR, OnBnClickedButtonTextureSpecular)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SPECULAR2, OnBnClickedButtonTextureSpecular2)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR, OnBnClickedCheckSpecular)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR2, OnBnClickedCheckSpecular2)
	ON_BN_CLICKED(IDC_CHECK_GLONORMAP, OnBnClickedCheckGlonormap)
END_MESSAGE_MAP()


// CEffTS_NormalMap �޽��� ó�����Դϴ�.
void CEffTS_NormalMap::Apply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	StringCchCopy( m_PropNormalMap.szNormalMap, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_NORMAL ).GetString() );
	StringCchCopy( m_PropNormalMap.szSpecularTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR ).GetString() );
	StringCchCopy( m_PropNormalMap.szGlossinessTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2 ).GetString() );

    m_PropNormalMap.bUseSpecularTex		= GetWin_Check( this, IDC_CHECK_SPECULAR ) ? true : false;
	m_PropNormalMap.bUseGlossinessTex	= GetWin_Check( this, IDC_CHECK_SPECULAR2 ) ? true : false;
	m_PropNormalMap.bUseGlossNormalAlpha  = GetWin_Check( this, IDC_CHECK_GLONORMAP ) ? true : false;

}

void CEffTS_NormalMap::SetProperty( EFFCHAR_PROPERTY_TS_NORMAL Property )
{
	m_PropNormalMap = Property; 

	SetWin_Text( this, IDC_EDIT_TEXTURE_NORMAL, m_PropNormalMap.szNormalMap );
	SetWin_Check( this, IDC_CHECK_SPECULAR, m_PropNormalMap.bUseSpecularTex );
	SetWin_Check( this, IDC_CHECK_SPECULAR2, m_PropNormalMap.bUseGlossinessTex );


	if( m_PropNormalMap.bUseSpecularTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR, m_PropNormalMap.szSpecularTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR, "" );
	}

	if( m_PropNormalMap.bUseGlossinessTex )
	{
		SetWin_Check( this, IDC_CHECK_GLONORMAP, m_PropNormalMap.bUseGlossNormalAlpha );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2, m_PropNormalMap.szGlossinessTex );

		if( m_PropNormalMap.bUseGlossNormalAlpha )
		{
			SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE);
		}else{
			SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE);
		}
	}else{
		SetWin_Check( this, IDC_CHECK_GLONORMAP, FALSE );
		SetWin_Enable( this, IDC_CHECK_GLONORMAP, FALSE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2, "" );
	}	



}

BOOL CEffTS_NormalMap::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffTS_NormalMap::OnBnClickedButtonTextureNormal()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_NORMAL );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTS_NormalMap::OnBnClickedButtonTextureSpecular()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_SPECULAR );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTS_NormalMap::OnBnClickedButtonTextureSpecular2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_SPECULAR2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTS_NormalMap::OnBnClickedCheckSpecular()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_PropNormalMap.bUseSpecularTex = GetWin_Check( this, IDC_CHECK_SPECULAR ) ? true : false;

	if( m_PropNormalMap.bUseSpecularTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, TRUE );
	}else{
		ZeroMemory( m_PropNormalMap.szSpecularTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, FALSE );
	}
}

void CEffTS_NormalMap::OnBnClickedCheckSpecular2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_PropNormalMap.bUseGlossinessTex = GetWin_Check( this, IDC_CHECK_SPECULAR2 ) ? true : false;

	if( m_PropNormalMap.bUseGlossinessTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE );
		SetWin_Enable( this, IDC_CHECK_GLONORMAP, TRUE );
	}else{
		ZeroMemory( m_PropNormalMap.szGlossinessTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE );
		SetWin_Enable( this, IDC_CHECK_GLONORMAP, FALSE );
		SetWin_Check( this, IDC_CHECK_GLONORMAP, FALSE );
	}
}



void CEffTS_NormalMap::OnBnClickedCheckGlonormap()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_PropNormalMap.bUseGlossNormalAlpha = GetWin_Check( this, IDC_CHECK_GLONORMAP ) ? true : false;

	if( m_PropNormalMap.bUseGlossNormalAlpha )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE);
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE);
	}
}
