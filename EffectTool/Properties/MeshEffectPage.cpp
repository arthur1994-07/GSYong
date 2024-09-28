// MeshEffectPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "EffGroupPage.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "MeshEffectPage.h"
#include ".\mesheffectpage.h"


// CMeshEffectPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMeshEffectPage, CPropertyPage)
CMeshEffectPage::CMeshEffectPage()
	: CPropertyPage(CMeshEffectPage::IDD)
		,m_pSheetTab(NULL)
		,m_pProperty(NULL)
{
}

CMeshEffectPage::~CMeshEffectPage()
{
}

void CMeshEffectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_TYPE, m_tabTYPE);
}

BEGIN_MESSAGE_MAP(CMeshEffectPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_EDITSOUND, OnBnClickedButtonEditsound)
	ON_BN_CLICKED(IDC_BUTTON_POSRESET, OnBnClickedButtonPosreset)
END_MESSAGE_MAP()


//	Note: CMeshEffectPage 메시지 처리기입니다.
void CMeshEffectPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (MESH_PROPERTY*) pProp;
	m_tabTYPE.Init ( m_pProperty, m_pSheetTab );
	m_tabTYPE.SetCurFocus(0);
}

//	Note: 모든 데이타를 셋하여 준다.
void CMeshEffectPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	
	m_tabTYPE.Apply ();
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CMeshEffectPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CMeshEffectPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : Tap 적용
	//
	CEffectToolView::m_iSheetTap = 0;
}

BOOL CMeshEffectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_tabTYPE.InsertItem(0, _T("Transform"));
	m_tabTYPE.InsertItem(1, _T("Display"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMeshEffectPage::OnBnClickedButtonEditsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, MESHPAGEINDEX );
}
BOOL CMeshEffectPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_tabTYPE.SetActive();

	return CPropertyPage::OnSetActive();
}

void CMeshEffectPage::OnBnClickedButtonPosreset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	D3DXMATRIX *matIdentity = pView->m_sObjectMRS.GetMatrix();
	D3DXMatrixIdentity( matIdentity );
	pView->m_sObjectMRS.SetMatrix( matIdentity );
}
