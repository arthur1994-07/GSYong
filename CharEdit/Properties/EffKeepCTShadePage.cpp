// EffKeepCTShadePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "EffKeepCTShadePage.h"
#include ".\effkeepctshadepage.h"


// CEffKeepCTShadePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffKeepCTShadePage, CPropertyPage)
CEffKeepCTShadePage::CEffKeepCTShadePage()
	: CPropertyPage(CEffKeepCTShadePage::IDD)
	, m_pEff(NULL)
	, m_rProperty(NULL)
{
}

CEffKeepCTShadePage::~CEffKeepCTShadePage()
{
}

void CEffKeepCTShadePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffKeepCTShadePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEX, OnBnClickedButtonTex)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CEffKeepCTShadePage 메시지 처리기입니다.

void CEffKeepCTShadePage::SetInstance( DxEffKeepData* pEff )
{
	if( pEff )
	{
		m_bCreateEff = FALSE;

		m_pEff = pEff; 
		m_rProperty = (EFFKEEP_PROPERTY_CTSHADE*)m_pEff->GetProperty();
	}
	else
	{
		m_bCreateEff = TRUE;

		m_pEff = DxEffKeepMan::GetInstance().CreateEffect
		(
			DxEffKeepData_CTShade::TYPEID,
			NULL
		);
		m_rProperty = (EFFKEEP_PROPERTY_CTSHADE*)m_pEff->GetProperty();
	}

	CWnd* pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_rProperty->szShadeTex );
}

void CEffKeepCTShadePage::OnBnClickedButtonTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CEffKeepCTShadePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );	
	StringCchCopy( m_rProperty->szShadeTex, 32, strText.GetString() );

	m_pEff->Create ( pView->GetD3dDevice() );

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		if( pView->GetSkinChar() )
		{
			pView->GetSkinChar()->Reset();
			pView->GetSkinChar()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		if( pView->GetAttBone() )
		{
			pView->GetAttBone()->Reset();
			pView->GetAttBone()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		if( pView->GetVehicle() )
		{
			pView->GetVehicle()->Reset();
			pView->GetVehicle()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}

	if( m_bCreateEff )
	{
		CCharEditView::g_sEffKeepMain.Insert( m_pEff );		// 효과를 삽입한다.
		m_bCreateEff = FALSE;
	}
}

void CEffKeepCTShadePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pEff = NULL;
	m_rProperty = NULL;
	m_pSheetTab->SetActivePage ( CHARTAB_CHAREFF );
}

void CEffKeepCTShadePage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_bCreateEff )
	{
		SAFE_DELETE( m_pEff );
		m_rProperty = NULL;
	}
	else
	{
		m_pEff = NULL;
		m_rProperty = NULL;
	}

	m_pSheetTab->SetActivePage( CHARTAB_CHAREFF );
}
