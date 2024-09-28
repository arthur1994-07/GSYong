// SkinMeshPage.cpp : ���� �����Դϴ�.
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


// CSkinMeshPage ��ȭ �����Դϴ�.

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


// CSkinMeshPage �޽��� ó�����Դϴ�.
void CSkinMeshPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (SKINMESH_PROPERTY*) pProp;

	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->SetWindowText ( m_pProperty->m_szMeshFile );
}

void CSkinMeshPage::OnBnClickedButtonPlay()
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

void CSkinMeshPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;
	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile, strText.GetString() );

	if( strlen(m_pProperty->m_szMeshFile) == 0 ) 
	{
		MessageBox( "��Ų ������ �ҷ��;� �մϴ�", "�˸�" );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GetWin_Text( this, IDC_EDIT_FILE ).GetLength() == 0 ) 
	{
		MessageBox( "��Ų ������ �ҷ��;� �մϴ�", "�˸�" );
		return;
	}

	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : Tap ����
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CSkinMeshPage::OnBnClickedButtonRead()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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


