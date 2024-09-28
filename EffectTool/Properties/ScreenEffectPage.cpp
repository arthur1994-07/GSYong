// D:\Ran\EffectTool\Properties\ScreenEffectPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "./EffSheetWithTab.h"
#include "./EffGroupPage.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectScreen.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLua.h"

#include "ScreenEffectPage.h"


// CScreenEffectPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CScreenEffectPage, CPropertyPage)

CScreenEffectPage::CScreenEffectPage()
	: CPropertyPage(CScreenEffectPage::IDD)
{

}

CScreenEffectPage::~CScreenEffectPage()
{
}

void CScreenEffectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenEffectPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
END_MESSAGE_MAP()

// CScreenEffectPage �޽��� ó�����Դϴ�.
void CScreenEffectPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (SCREEN_PROPERTY*) pProp;

	CWnd* pWnd = GetDlgItem ( IDC_EDIT_EFFECT_NAME );
	pWnd->SetWindowText ( m_pProperty->m_szEffectName );
}

void CScreenEffectPage::OnBnClickedButtonPlay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

	// ���� refresh
	NSEffectLua::LuaLoad();

	//	Note : ����Ʈ ����.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().RemoveAllEff();

	DxEffSinglePropGMan::GetInstance().CleanUp();
	DxEffSinglePropGMan::GetInstance().Clear();

	DxEffGroupPlayer::GetInstance().SetTime( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CScreenEffectPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd *pWnd;

	CString strText;
	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_NAME );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szEffectName, strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CScreenEffectPage::OnBnClickedButtonOk()
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
