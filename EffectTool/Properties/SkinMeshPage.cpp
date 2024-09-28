// SkinMeshPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "EffGroupPage.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../../EngineLib/DxEffect/Single/DxEffectSkinMesh.h"

#include "SkinMeshPage.h"


// CSkinMeshPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkinMeshPage, CPropertyPage)
CSkinMeshPage::CSkinMeshPage()
	: CPropertyPage(CSkinMeshPage::IDD)
{
}

CSkinMeshPage::~CSkinMeshPage()
{
}

void CSkinMeshPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSkinMeshPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_READ, OnBnClickedButtonRead)
END_MESSAGE_MAP()


// CSkinMeshPage 메시지 처리기입니다.
void CSkinMeshPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (SKINMESH_PROPERTY*) pProp;

	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->SetWindowText ( m_pProperty->m_szMeshFile );
}

void CSkinMeshPage::OnBnClickedButtonPlay()
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

void CSkinMeshPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;
	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile, strText.GetString() );

	if( strlen(m_pProperty->m_szMeshFile) == 0 ) 
	{
		MessageBox( "스킨 파일을 불러와야 합니다", "알림" );
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CSkinMeshPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GetWin_Text( this, IDC_EDIT_FILE ).GetLength() == 0 ) 
	{
		MessageBox( "스킨 파일을 불러와야 합니다", "알림" );
		return;
	}

	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : Tap 적용
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CSkinMeshPage::OnBnClickedButtonRead()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	//CString szFilter = "Skin (*.chf)";
	CString szFilter = "Charactor File (*.chf)|*.chf|";
	CFileDialog dlg ( TRUE, ".chf", NULL, OFN_HIDEREADONLY, szFilter, this );

	//CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
	//	(CFramePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );
}


