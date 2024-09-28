// LightMapEffEdit.cpp : ���� �����Դϴ�.
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

// CLightMapEffEdit ��ȭ �����Դϴ�.

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


// CLightMapEffEdit �޽��� ó�����Դϴ�.
void CLightMapEffEdit::SetInstance ( DxEffectLightMap *pLightMapEff )
{
	m_pLightMapEff = pLightMapEff;
	m_OldLightMapProperty = m_LightMapProperty = m_pLightMapEff->GetProperty();

	//	Note : �� ����.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pLightMapEff->GetName() );
}

void CLightMapEffEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.


	//	Note : �Ӽ��� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	OnBnClickedButtonApply();

	m_pSheetTab->SetActivePage ( 0 );
	m_pLightMapEff = NULL;
}

void CLightMapEffEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_pSheetTab->SetActivePage ( 0 );

	//	Note : �Ӽ��� �ǵ���.
	//
	m_pLightMapEff->SetProperty ( m_OldLightMapProperty );
	m_pLightMapEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pLightMapEff->Create ( pView->GetD3DDevice() );

	m_pSheetTab->SetActivePage ( 0 );
	m_pLightMapEff = NULL;
}
