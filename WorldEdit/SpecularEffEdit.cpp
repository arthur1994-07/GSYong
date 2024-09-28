// SpecularEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpecular.h"

#include "SpecularEffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CSpecularEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSpecularEffEdit, CPropertyPage)
CSpecularEffEdit::CSpecularEffEdit()
	: CPropertyPage(CSpecularEffEdit::IDD)
{
}

CSpecularEffEdit::~CSpecularEffEdit()
{
}

void CSpecularEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpecularEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_ALPHA, OnBnClickedRadioAlpha)
END_MESSAGE_MAP()


// CSpecularEffEdit 메시지 처리기입니다.
void CSpecularEffEdit::SetInstance ( DxEffectSpecular *pEff )
{
	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	GetDlgItem ( IDC_STATIC_TEXTURE )->ShowWindow ( FALSE );
	GetDlgItem ( IDC_EDIT_TEXTURE )->ShowWindow ( FALSE );
	GetDlgItem ( IDC_BUTTON_TEXTURE )->ShowWindow ( FALSE );

	CButton*	pButton;

	if ( m_Property.m_bAll )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
		pButton->SetCheck ( TRUE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALPHA );
		pButton->SetCheck ( FALSE );
	}
	else
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
		pButton->SetCheck ( FALSE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALPHA );
		pButton->SetCheck ( TRUE );
	}

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );
}

void CSpecularEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );
}

void CSpecularEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CSpecularEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pEff->SetProperty ( m_OldProperty );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );
}

void CSpecularEffEdit::OnBnClickedRadioAll()
{
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
	if ( pButton->GetCheck () )		m_Property.m_bAll = TRUE;
	else							m_Property.m_bAll = FALSE;

	m_pEff->SetProperty ( m_Property );
}

void CSpecularEffEdit::OnBnClickedRadioAlpha()
{
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALPHA );
	if ( pButton->GetCheck () )		m_Property.m_bAll = FALSE;
	else							m_Property.m_bAll = TRUE;

	m_pEff->SetProperty ( m_Property );
}
