// GroupPropPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxTools/DxEditMat.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "GroupPropPage.h"
#include ".\groupproppage.h"


// CGroupPropPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGroupPropPage, CPropertyPage)
CGroupPropPage::CGroupPropPage()
	: CPropertyPage(CGroupPropPage::IDD),
	m_pSheetTab(NULL)
{
}

CGroupPropPage::~CGroupPropPage()
{
}

void CGroupPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGroupPropPage, CPropertyPage)

	ON_BN_CLICKED(IDC_BUTTONOK, OnBnClickedButtonok)
	ON_BN_CLICKED(IDC_CHECK_NEVERDIE, OnBnClickedCheckNeverdie)
	ON_BN_CLICKED(IDC_RADIO_AVERAGE, OnBnClickedRadioAverage)
	ON_BN_CLICKED(IDC_RADIO_SLOWFAST, OnBnClickedRadioSlowfast)
	ON_BN_CLICKED(IDC_RADIO_FASTSLOW, OnBnClickedRadioFastslow)
END_MESSAGE_MAP()


// CGroupPropPage 메시지 처리기입니다.
void CGroupPropPage::SetProperty ( EFF_PROPGROUP* pProp )
{
	assert(pProp);

	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NEVERDIE );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_NEVERDIE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETPOS );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_TARGETPOS );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETID );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_TARGETID );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_NOTARGET );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_NONE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGET_LINE );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_TARGETLINE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TAGERT_CAMERA_MOVE );
	pButton->SetCheck ( pProp->m_dwFlag & EFF_CFG_TAGERT_CAMERA_MOVE );


	pButton = (CButton*) GetDlgItem ( IDC_RADIO_AVERAGE );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLOWFAST );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FASTSLOW );
	pButton->SetCheck ( FALSE );

	if ( pProp->m_dwFlag & EFF_CFG_AVERAGE )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_AVERAGE );
		pButton->SetCheck ( TRUE );
	}
	else if ( pProp->m_dwFlag & EFF_CFG_SLOWFAST )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_SLOWFAST );
		pButton->SetCheck ( TRUE );
	}
	else if ( pProp->m_dwFlag & EFF_CFG_FASTSLOW )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_FASTSLOW );
		pButton->SetCheck ( TRUE );
	}

	m_pGroupProp = pProp;
}

void CGroupPropPage::OnBnClickedButtonok()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETPOS );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_TARGETPOS;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_TARGETPOS;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETID );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_TARGETID;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_TARGETID;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_NOTARGET );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_NONE;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_NONE;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGET_LINE );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_TARGETLINE;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_TARGETLINE;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TAGERT_CAMERA_MOVE );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_TAGERT_CAMERA_MOVE;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_TAGERT_CAMERA_MOVE;

	DXAFFINEPARTS	AffineParts;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	D3DXVECTOR3 vPos = pView->m_sObjectMRS.GetPosition();
	m_pGroupProp->m_vLocal.y = vPos.y;

	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();
}




void CGroupPropPage::OnBnClickedCheckNeverdie()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NEVERDIE );
	if ( pButton->GetCheck () )		m_pGroupProp->m_dwFlag |= EFF_CFG_NEVERDIE;
	else							m_pGroupProp->m_dwFlag &= ~EFF_CFG_NEVERDIE;
}

void CGroupPropPage::OnBnClickedRadioAverage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pGroupProp->m_dwFlag &= ~EFF_CFG_ACCEL;
	m_pGroupProp->m_dwFlag |= EFF_CFG_AVERAGE;
}

void CGroupPropPage::OnBnClickedRadioSlowfast()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pGroupProp->m_dwFlag &= ~EFF_CFG_ACCEL;
	m_pGroupProp->m_dwFlag |= EFF_CFG_SLOWFAST;
}

void CGroupPropPage::OnBnClickedRadioFastslow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pGroupProp->m_dwFlag &= ~EFF_CFG_ACCEL;
	m_pGroupProp->m_dwFlag |= EFF_CFG_FASTSLOW;
}