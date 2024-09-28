// MeshEffectPage.cpp : ���� �����Դϴ�.
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


// CMeshEffectPage ��ȭ �����Դϴ�.

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


//	Note: CMeshEffectPage �޽��� ó�����Դϴ�.
void CMeshEffectPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (MESH_PROPERTY*) pProp;
	m_tabTYPE.Init ( m_pProperty, m_pSheetTab );
	m_tabTYPE.SetCurFocus(0);
}

//	Note: ��� ����Ÿ�� ���Ͽ� �ش�.
void CMeshEffectPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	
	m_tabTYPE.Apply ();
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CMeshEffectPage::OnBnClickedButtonPlay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	OnBnClickedButtonApply();

	//	Note : ����Ʈ ����.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CMeshEffectPage::OnBnClickedButtonOk()
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

BOOL CMeshEffectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_tabTYPE.InsertItem(0, _T("Transform"));
	m_tabTYPE.InsertItem(1, _T("Display"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CMeshEffectPage::OnBnClickedButtonEditsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, MESHPAGEINDEX );
}
BOOL CMeshEffectPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_tabTYPE.SetActive();

	return CPropertyPage::OnSetActive();
}

void CMeshEffectPage::OnBnClickedButtonPosreset()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	D3DXMATRIX *matIdentity = pView->m_sObjectMRS.GetMatrix();
	D3DXMatrixIdentity( matIdentity );
	pView->m_sObjectMRS.SetMatrix( matIdentity );
}
