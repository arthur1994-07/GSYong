// D:\Ran\EffectTool\Properties\ScreenEffectPage.cpp : 구현 파일입니다.
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


// CScreenEffectPage 대화 상자입니다.

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

// CScreenEffectPage 메시지 처리기입니다.
void CScreenEffectPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (SCREEN_PROPERTY*) pProp;

	CWnd* pWnd = GetDlgItem ( IDC_EDIT_EFFECT_NAME );
	pWnd->SetWindowText ( m_pProperty->m_szEffectName );
}

void CScreenEffectPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	// 정보 refresh
	NSEffectLua::LuaLoad();

	//	Note : 이펙트 생성.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
