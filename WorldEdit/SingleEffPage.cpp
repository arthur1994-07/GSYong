// SingleEffPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "sheetWithTab.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../EngineLib/DxLand/DXLANDEFF.h"
#include "../EngineLib/DxTools/DxViewPort.h"

#include "SingleEffPage.h"
#include ".\singleeffpage.h"
#include "State/WorldStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern D3DXVECTOR3			g_vPICK_RAY_DIR;
extern D3DXVECTOR3			g_vPICK_RAY_ORIG;
extern BOOL					g_bLANDEFF_DSPNAME;

// CSingleEffPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSingleEffPage, CPropertyPage)
CSingleEffPage::CSingleEffPage()
	: CPropertyPage(CSingleEffPage::IDD),
	m_bDlgInit(FALSE)
{
}

CSingleEffPage::~CSingleEffPage()
{
}

void CSingleEffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_EffListBox);
}


BEGIN_MESSAGE_MAP(CSingleEffPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_EFFECT, OnLbnSelchangeListEffect)
	ON_BN_CLICKED(IDC_CHECK_DSP_NAME, OnBnClickedCheckDspName)
    ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CSingleEffPage::OnBnClickedButtonModify)
END_MESSAGE_MAP()


// CSingleEffPage �޽��� ó�����Դϴ�.
BOOL CSingleEffPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bDlgInit = TRUE;

	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSingleEffPage::ListtingLandEff ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	PLANDEFF pLandEff = NULL;

	// Note : ���õ� ���� 1���� ���
	DWORD dwSelCount = m_EffListBox.GetSelCount();

	CString StrOldSelect;
	if( dwSelCount == 1 )
	{
		int nIndex = m_EffListBox.GetCurSel();
		if( nIndex!=LB_ERR )
		{
			pLandEff = (PLANDEFF) m_EffListBox.GetItemData( nIndex );
			StrOldSelect = pLandEff->m_szName;
		}
	}

	m_EffListBox.ResetContent ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	PLANDEFF pCur = pSetLandMan->GetLandEffList ();
	while ( pCur )
	{
		int nIndex = m_EffListBox.AddString ( pCur->m_szName );
		m_EffListBox.SetItemData ( nIndex, (DWORD)pCur );

		pCur = pCur->m_pNext;
	}

	if( dwSelCount == 1 )
	{
		int nIndex = m_EffListBox.SelectString( -1, StrOldSelect.GetString() );
		if( nIndex!=LB_ERR )
		{
			m_EffListBox.SetSel( nIndex );
		}
	}

	if( pLandEff )
	{
		CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME );
		pWnd->SetWindowText( pLandEff->m_szName );

		pWnd = GetDlgItem( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText( pLandEff->m_szFileName );
	}
}

// Note : ������ �̻�������..  �̰� Clone �� �Ҷ� ȣ���� �Ǵ� �Լ��̴�.
//			�׷��� List�� ������Ʈ �� �� ���� ��ġ�� �ٲٰ�, �״��� List ������Ʈ �Ͽ��� ������ �Ѵ�.
void CSingleEffPage::UpdatePageClickDOWN()
{
	if( !m_bDlgInit )	return;

	// List ������Ʈ
	ListtingLandEff();

	int nIndex(LB_ERR);
	if( m_EffListBox.GetSelCount() == 1 )
	{
		nIndex = m_EffListBox.GetCurSel ();
	}

	if( nIndex==LB_ERR )
		return;

	PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData( nIndex );
	SetDialog( pLandEff );
}

void CSingleEffPage::UpdatePageClickUP()
{
	if( !m_bDlgInit )	return;

	// Note : Tree�� �� ����
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();
	pSetLandMan->BuildSingleEffTree ();
}

void CSingleEffPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	ListtingLandEff ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();
}

void CSingleEffPage::OnBnClickedButtonNew()
{
	CString strName;
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText( strName );
	if( !strName.GetLength() )
	{
		MessageBox ( "����Ʈ ������ �������� �ʾҽ��ϴ�.", "����" );
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	DXLANDEFF *pLandEff;
	pLandEff = new DXLANDEFF;

	char szNewName[DXLANDEFF::LANDEFF_MAXSZ];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", strName.GetString(), i++ );
	} while ( pView->GetSetLandMan()->FindLandEff(szNewName) );

	strcpy ( pLandEff->m_szName, szNewName );

	pLandEff->SetPosition( pView->GetCollisionPos() );

	pView->ActiveEditMatrix2( &pLandEff->m_matWorld );
	pSetLandMan->AddLandEff( pLandEff );
	pSetLandMan->BuildSingleEffTree ();

	HRESULT hr;
	hr = pLandEff->SetEffect ( szNewName, pLandEff->m_matWorld, strName.GetString(), 1.f, 1.f, pView->GetD3DDevice() );
	if ( FAILED(hr) )
	{
		MessageBox ( "������ ������ ��ü�� �ʱ�ȭ�Ͽ����� �����Ͽ����ϴ�.", "����" );
	}

	pSetLandMan->BuildSingleEffTree ();

    WorldStateMachine::EditAsset(pLandEff->m_szName);

	ListtingLandEff ();

	//  ����
	int nSize = m_EffListBox.GetCount();
	if( nSize!=LB_ERR )
	{
		m_EffListBox.SetCurSel( 0 );
	}
}

void CSingleEffPage::OnBnClickedButtonSearch()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", m_strPrevEffName.c_str(),
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( WriteName );

		m_strPrevEffName = WriteName;

		// Note : �̸��� ����
		char szNewName[DXLANDEFF::LANDEFF_MAXSZ];
		int i=0;
		do
		{
			sprintf( szNewName, "%s[%d]", WriteName.GetString(), i++ );
		} while( pView->GetSetLandMan()->FindLandEff(szNewName) );

		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText( szNewName );
		
	}
}

void CSingleEffPage::OnBnClickedButtonDel()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	//int nIndex = m_EffListBox.GetCurSel ();
	//if ( nIndex==LB_ERR )	return;

	//PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );

	//pSetLandMan->DelLandEff ( pLandEff );
	//pSetLandMan->BuildSingleEffTree ();

	for( int nIndex=0; nIndex<m_EffListBox.GetCount(); ++nIndex )
	{
		int nCheck = m_EffListBox.GetSel( nIndex );

		if( nCheck > 0 )
		{
			CString Text;
			m_EffListBox.GetText ( nIndex, Text );

			CString strMsg;
			strMsg.Format ( _T("'%s' �� ���� �Ͻðڽ��ϱ�? ������ �ǻ츱�� �����ϴ�."), Text.GetString() );

			if ( IDYES == MessageBox ( strMsg, _T("����"), MB_YESNO|MB_ICONQUESTION ) )
			{
				PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );
				pSetLandMan->DelLandEff ( pLandEff );
				pSetLandMan->BuildSingleEffTree();
                WorldStateMachine::EditAsset(Text.GetBuffer());
			}
		}
	}

	UpdatePageClickDOWN();
}

BOOL CSingleEffPage::OnKillActive()
{
	UpdateContrl ();

	g_bLANDEFF_DSPNAME = FALSE;

	return CPropertyPage::OnKillActive();
}

void CSingleEffPage::OnLbnSelchangeListEffect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	int nIndex(LB_ERR);
	if( m_EffListBox.GetSelCount() == 1 )
	{
		nIndex = m_EffListBox.GetCurSel ();
	}
	if( nIndex==LB_ERR )	
		return;

	PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );
	SetDialog( pLandEff );
}

void CSingleEffPage::SetDialog( DXLANDEFF* pLandEff )
{
	if( !pLandEff )	
        return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	pView->ActiveEditMatrix2( &pLandEff->m_matWorld );

	D3DXVECTOR3 vPos( pLandEff->m_matWorld._41, pLandEff->m_matWorld._42, pLandEff->m_matWorld._43 );
	DxViewPort::GetInstance().CameraJump( vPos );

    CString Str;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME );
	pWnd->SetWindowText( pLandEff->m_szName );

	pWnd = GetDlgItem( IDC_EDIT_FINDSTR );
	pWnd->SetWindowText( pLandEff->m_szFileName );

    pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER_DAY );
    Str.Format ( "%3.2f", pLandEff->m_fColorPowerDay );
    pWnd->SetWindowText( Str.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER_NIGHT );
    Str.Format ( "%3.2f", pLandEff->m_fColorPowerNight );
    pWnd->SetWindowText( Str.GetString() );
}

void CSingleEffPage::OnBnClickedCheckDspName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_DSP_NAME );
	g_bLANDEFF_DSPNAME = (BOOL)pButton->GetCheck();
}

void CSingleEffPage::OnBnClickedButtonModify()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    int nIndex(LB_ERR);
	if( m_EffListBox.GetSelCount() == 1 )
	{
		nIndex = m_EffListBox.GetCurSel ();
	}

	if( nIndex==LB_ERR )	
		return;

	PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );
    if ( !pLandEff )
        return;

    CWnd* pWnd;
    CString strText;
    pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER_DAY );
    pWnd->GetWindowText( strText );
	pLandEff->m_fColorPowerDay = static_cast<float>( atof ( strText.GetString() ) );

    pWnd = GetDlgItem( IDC_EDIT_COLOR_POWER_NIGHT );
    pWnd->GetWindowText( strText );
	pLandEff->m_fColorPowerNight = static_cast<float>( atof ( strText.GetString() ) );
}

BOOL CSingleEffPage::OnSetActive()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

    CWorldEditView::m_ViewType = EFFECT_SET_VIEW;

    return CPropertyPage::OnSetActive();
}

void CSingleEffPage::SelectSingleEffectList( DXLANDEFF* pEff )
{
	for( int nIndex=0; nIndex<m_EffListBox.GetCount(); ++nIndex )
	{
		PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData( nIndex );
		if( pLandEff == pEff )
		{
			m_EffListBox.ResetContent();
			ListtingLandEff();
			m_EffListBox.SetSel( nIndex );

			SetDialog( pLandEff );

			return;
		}
	}
}