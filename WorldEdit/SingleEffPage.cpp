// SingleEffPage.cpp : 구현 파일입니다.
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

// CSingleEffPage 대화 상자입니다.

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


// CSingleEffPage 메시지 처리기입니다.
BOOL CSingleEffPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;

	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSingleEffPage::ListtingLandEff ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	PLANDEFF pLandEff = NULL;

	// Note : 선택된 것이 1개일 경우
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

// Note : 구조가 이상하지만..  이건 Clone 을 할때 호출이 되는 함수이다.
//			그래서 List를 업데이트 한 후 선택 위치를 바꾸고, 그다음 List 업데이트 하여서 정리를 한다.
void CSingleEffPage::UpdatePageClickDOWN()
{
	if( !m_bDlgInit )	return;

	// List 업데이트
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

	// Note : Tree의 재 정렬
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
		MessageBox ( "이팩트 파일이 지정되지 않았습니다.", "주의" );
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
		MessageBox ( "설정된 값으로 개체를 초기화하였으나 실패하였습니다.", "주의" );
	}

	pSetLandMan->BuildSingleEffTree ();

    WorldStateMachine::EditAsset(pLandEff->m_szName);

	ListtingLandEff ();

	//  삽질
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

	//	Note : 파일 오픈 다이알로그를 만듬.
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

		// Note : 이름의 셋팅
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
			strMsg.Format ( _T("'%s' 를 삭제 하시겠습니까? 삭제후 되살릴수 없습니다."), Text.GetString() );

			if ( IDYES == MessageBox ( strMsg, _T("주의"), MB_YESNO|MB_ICONQUESTION ) )
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_DSP_NAME );
	g_bLANDEFF_DSPNAME = (BOOL)pButton->GetCheck();
}

void CSingleEffPage::OnBnClickedButtonModify()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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