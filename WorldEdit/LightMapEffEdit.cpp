// LightMapEffEdit.cpp : 구현 파일입니다.
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
#include "../EngineLib/DxEffect/Frame/DxEffectLightMap.h"

#include "LightMapEffEdit.h"

// CLightMapEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightMapEffEdit, CPropertyPage)
CLightMapEffEdit::CLightMapEffEdit()
	: CPropertyPage(CLightMapEffEdit::IDD),
	m_pLightMapEff(NULL)
{
}

CLightMapEffEdit::~CLightMapEffEdit()
{
}

void CLightMapEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLightMapEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CLightMapEffEdit 메시지 처리기입니다.
void CLightMapEffEdit::SetInstance ( DxEffectLightMap *pLightMapEff )
{
	m_pLightMapEff = pLightMapEff;
	m_OldLightMapProperty = m_LightMapProperty = m_pLightMapEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pLightMapEff->GetName() );
}

void CLightMapEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	//	Note : 속성을 적용.
	//
	m_pLightMapEff->SetProperty ( m_LightMapProperty );
	m_pLightMapEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pLightMapEff->Create ( pView->GetD3DDevice() );

//	m_pLightMapEff->ReMakeWaterLight ( pView->GetD3DDevice() );
}

void CLightMapEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	m_pSheetTab->SetActivePage ( 0 );
	m_pLightMapEff = NULL;
}

void CLightMapEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pLightMapEff->SetProperty ( m_OldLightMapProperty );
	m_pLightMapEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pLightMapEff->Create ( pView->GetD3DDevice() );

	m_pSheetTab->SetActivePage ( 0 );
	m_pLightMapEff = NULL;
}
