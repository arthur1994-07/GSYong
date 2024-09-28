#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectTargetInfluence.h"
#include "EffGroupPage.h"

#include "TargetInfluencePage.h"

#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include "./TargetInfluencePage.h"

IMPLEMENT_DYNAMIC(CTargetInfluencePage, CPropertyPage)
CTargetInfluencePage::CTargetInfluencePage()
: CPropertyPage(CTargetInfluencePage::IDD)
{
}

CTargetInfluencePage::~CTargetInfluencePage()
{
}

void CTargetInfluencePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTargetInfluencePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_CHECK_PARENT_ROTATE, OnBnClickedCheckParentRotate)
END_MESSAGE_MAP()

void CTargetInfluencePage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (TARGETINFLUENCE_PROPERTY*)pProp;
	//CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_PARENT_ROTATE );
	if ( m_pProperty->m_dwFLAG & USEPARENTROTATE )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );
}

void CTargetInfluencePage::OnBnClickedButtonApply()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();


	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CTargetInfluencePage::OnBnClickedButtonOk()
{
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();
	// 속성을 적용;
	OnBnClickedButtonApply();
	CEffectToolView::m_iSheetTap = 0;
}

void CTargetInfluencePage::OnBnClickedCheckParentRotate()
{
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_PARENT_ROTATE );
	if ( pButton->GetCheck () )		m_pProperty->m_dwFLAG |= USEPARENTROTATE;
	else							m_pProperty->m_dwFLAG &= ~USEPARENTROTATE;
}
