// ParticleSysPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffectParticleSys.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxTools/TextureManager.h"
#include "EffGroupPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "ParticleSysPage.h"
#include ".\particlesyspage.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



// CParticleSysPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CParticleSysPage, CPropertyPage)
CParticleSysPage::CParticleSysPage()
	: CPropertyPage(CParticleSysPage::IDD),
	m_pSheetTab(NULL),
	m_pProperty(NULL)
{
}

CParticleSysPage::~CParticleSysPage()
{
}

void CParticleSysPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_TYPE, m_tabTYPE);
}


BEGIN_MESSAGE_MAP(CParticleSysPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_EDITSOUND, OnBnClickedButtonEditsound)
END_MESSAGE_MAP()


BOOL CParticleSysPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_tabTYPE.InsertItem(0, _T("Emitter"));
	m_tabTYPE.InsertItem(1, _T("Particle-M"));
	m_tabTYPE.InsertItem(2, _T("Particle-D"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


// CParticleSysPage �޽��� ó�����Դϴ�.
void CParticleSysPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (PARTICLESYS_PROPERTY*) pProp;	
	m_tabTYPE.Init ( m_pProperty, m_pSheetTab );
	m_tabTYPE.SetCurFocus(0);
}

void CParticleSysPage::OnBnClickedButtonPlay()
{
	//	Note : ���� ����.
	//
	OnBnClickedButtonApply();

	//	Note : ����Ʈ ����.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CParticleSysPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_tabTYPE.Apply ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CParticleSysPage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : Tap ����
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CParticleSysPage::OnBnClickedButtonEditsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, PARTICLEPAGEINDEX );
}


BOOL CParticleSysPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_tabTYPE.SetActive();

	return CPropertyPage::OnSetActive();
}
