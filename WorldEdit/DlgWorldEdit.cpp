// DlgEdit.cpp : implementation file
//

#include "stdafx.h"

#include "worldedit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxTools/SkyBox/DxSkyBoxMan.h"
#include "../MfcExLib/ExLibs/DxControlFunc.h"
#include "SheetWithTab.h"
#include "dlgworldedit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit dialog

CDlgEdit::CDlgEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEdit)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_EDITTYPE, m_comboEditType);
}


BEGIN_MESSAGE_MAP(CDlgEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgEdit)
	ON_CBN_SELCHANGE(IDC_COMBO_EDITTYPE, OnCbnSelchangeComboEdittype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit message handlers

BOOL CDlgEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	SheetRect.bottom = 900;

	m_psheetWithTab = new CsheetWithTab ( SheetRect, this );
	m_psheetWithTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTab->SetActivePageEX( FRAMEPAGEINDEX, FRAMEPAGEINDEX );

	DWORD nIndex;
	nIndex = m_comboEditType.AddString ( g_strEditSheet[FRAMEPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, FRAMEPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[LIGHTPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, LIGHTPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[SHADOWLIGHTINDEX] );
	m_comboEditType.SetItemData ( nIndex, SHADOWLIGHTINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[WEATHERPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, WEATHERPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[CAMERAPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, CAMERAPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[SOUNDPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, SOUNDPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[SINGLEEFFPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, SINGLEEFFPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[SKINOBJPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, SKINOBJPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[LANDGATEPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, LANDGATEPAGEINDEX );

//	nIndex = m_comboEditType.AddString ( g_strEditSheet[NATUREINDEX] );
//	m_comboEditType.SetItemData ( nIndex, NATUREINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[BGMEDITPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, BGMEDITPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[FOGPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, FOGPAGEINDEX );

    nIndex = m_comboEditType.AddString ( g_strEditSheet[SKYBOXMANINDEX] );
	m_comboEditType.SetItemData ( nIndex, SKYBOXMANINDEX );

	//nIndex = m_comboEditType.AddString ( g_strEditSheet[MAPEDITPAGEINDEX] );
	//m_comboEditType.SetItemData ( nIndex, MAPEDITPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[STATICMESHCREATEINDEX] );
	m_comboEditType.SetItemData ( nIndex, STATICMESHCREATEINDEX );

	//nIndex = m_comboEditType.AddString ( g_strEditSheet[PIECEMANAGERINDEX] );
	//m_comboEditType.SetItemData ( nIndex, PIECEMANAGERINDEX );

    nIndex = m_comboEditType.AddString ( g_strEditSheet[PIECEPOINTPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, PIECEPOINTPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[CAMCOLLPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, CAMCOLLPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheet[WATERSURFPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, WATERSURFPAGEINDEX );
	
	nIndex = m_comboEditType.AddString( g_strEditSheet[ PREFABPAGEINDEX ] );
	m_comboEditType.SetItemData( nIndex, PREFABPAGEINDEX );

	//	Note : Set Active Page
	//
	nIndex = m_comboEditType.FindString ( 0, g_strEditSheet[FRAMEPAGEINDEX] );
	m_comboEditType.SetCurSel ( nIndex );

	return TRUE;
	// return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgEdit::SelectFrameTree( const char *szFrame )
{
	if ( !szFrame )			return;
	if ( !m_psheetWithTab )	return;

	HTREEITEM hFindTreeItem = FindTreeItem ( m_psheetWithTab->m_FramePage.m_TreeFrame, szFrame );
	if ( hFindTreeItem )
	{
		m_psheetWithTab->m_FramePage.m_TreeFrame.SelectItem ( hFindTreeItem );

		m_psheetWithTab->m_FramePage.SelectChangedTreeFrame();
	}
}

void CDlgEdit::SelectPieceFrameTree( const char *szFrame )
{
	if ( !szFrame )			return;
	if ( !m_psheetWithTab )	return;

	HTREEITEM hFindTreeItem = FindTreeItem( m_psheetWithTab->m_StaticMeshCreate.m_TreeFrame, szFrame );
	if ( hFindTreeItem )
	{
		m_psheetWithTab->m_StaticMeshCreate.m_TreeFrame.SelectItem ( hFindTreeItem );

		m_psheetWithTab->m_StaticMeshCreate.SelectChangedTreeFrame();
	}
}

void CDlgEdit::SelectPieceList( DxPiece* pPiece, int nCurType )
{
	if ( !pPiece )			return;
	if ( !m_psheetWithTab )	return;

    if( m_psheetWithTab )	
        m_psheetWithTab->SelectPieceList( pPiece, nCurType );
}

void CDlgEdit::SelectPrefabList( DxPrefab* pPrefab )
{
	if ( !pPrefab )			return;
	if ( !m_psheetWithTab )	return;

	m_psheetWithTab->m_PrefabPage.SelectPrefabList( pPrefab );
}

void CDlgEdit::SelectTexEffList( const TSTRING& strName )
{
    if ( strName.empty() )	return;
	if ( !m_psheetWithTab )	return;

    if( m_psheetWithTab )	
        m_psheetWithTab->SelectTexEffList( strName );
}

void CDlgEdit::SelectCamCollList( const char *szTexture )
{
	if ( !szTexture || szTexture[0] == NULL ) return;

	m_psheetWithTab->m_CamcollPage.SelectMesh(szTexture);
}

void CDlgEdit::SelectSingleEffectList( DXLANDEFF* pEff )
{
	if ( !m_psheetWithTab )	return;

	m_psheetWithTab->m_SingleEffPage.SelectSingleEffectList( pEff );
}

void CDlgEdit::InsertPiece()
{
	if ( !m_psheetWithTab )	return;

	m_psheetWithTab->m_PiecePointPage.InsertPiece();
}

void CDlgEdit::OnCbnSelchangeComboEdittype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD nIndex = m_comboEditType.GetCurSel ();
	if ( nIndex == -1 )		return;

	DWORD dwData = m_comboEditType.GetItemData ( nIndex );

    switch( dwData )
	{
	case SKYBOXMANINDEX:
		m_psheetWithTab->ActiveSkyBoxManPage( &DxSkyBoxMan::GetInstance(), NULL, NULL, FRAMEPAGEINDEX );
		break;
	default:
		m_psheetWithTab->SetActivePageEX( dwData, dwData );
		break;
	};
}

void CDlgEdit::SetAcitvePage(int nPage)
{
    DWORD dwData = 0;

    for (int i = 0; i < m_comboEditType.GetCount(); ++i)
    {
        dwData = m_comboEditType.GetItemData(i);

        if (dwData == nPage)
        {
            if (i != m_comboEditType.GetCurSel())
            {
                m_comboEditType.SetCurSel(i);
                OnCbnSelchangeComboEdittype();
            }

            break;
        }
    }
}

void CDlgEdit::ResetContent ()
{
	m_psheetWithTab->m_FramePage.ResetContent ();
}

void CDlgEdit::UpdatePage_Always()
{
	if( m_psheetWithTab )	m_psheetWithTab->UpdatePage_Always();
}

void CDlgEdit::UpdatePage_Event( BOOL bUp )
{
	if( m_psheetWithTab )	m_psheetWithTab->UpdatePage_Event( bUp );
}

void CDlgEdit::UpdatePage_ListChange()
{
	if( m_psheetWithTab )	m_psheetWithTab->UpdatePage_ListChange();
}

void CDlgEdit::UpdatePage_List_AddLastItem()
{
	if( m_psheetWithTab )	m_psheetWithTab->UpdatePage_List_AddLastItem();
}

void CDlgEdit::KeyBoard_Enter()
{
	if( m_psheetWithTab )	m_psheetWithTab->KeyBoard_Enter();
}

void CDlgEdit::UpdateKeyBoardMouse()
{
	if( m_psheetWithTab )	m_psheetWithTab->UpdateKeyBoardMouse();
}

int CDlgEdit::GetActiveIndex()
{
	if( m_psheetWithTab )	return m_psheetWithTab->GetActiveIndex();
	else					return -1;
}

void CDlgEdit::UpdateCamColl()
{
	 if( m_psheetWithTab ) m_psheetWithTab->m_FramePage.UpdateCamColl();
}

void CDlgEdit::UpdateContrl ()
{
	m_comboEditType.SetCurSel ( FRAMEPAGEINDEX );
	m_psheetWithTab->SetActivePageEX( FRAMEPAGEINDEX, FRAMEPAGEINDEX );
	
	m_psheetWithTab->m_FramePage.UpdateContrl ();
	m_psheetWithTab->m_LightPage.UpdateContrl ();
	m_psheetWithTab->m_CameraPage.UpdateContrl ();
	m_psheetWithTab->m_SoundPage.UpdateContrl ();
	m_psheetWithTab->m_LandSkinObj.UpdateContrl ();
	m_psheetWithTab->m_LandGatePage.UpdateContrl ();
	m_psheetWithTab->m_BgmEditPage.UpdateContrl ();
	m_psheetWithTab->m_WeatherPage.UpdateContrl ();
	m_psheetWithTab->m_FogPage.UpdateContrl ();
	m_psheetWithTab->m_MapEditPage.UpdateContrl ();
//	m_psheetWithTab->m_StaticMeshCreate.UpdatePage();
	//	m_psheetWithTab->m_PieceManagerPage.UpdatePage();
	m_psheetWithTab->m_CamcollPage.UpdateContrl ();
}

void CDlgEdit::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE(m_psheetWithTab);

	CDialog::PostNcDestroy();
}

void CDlgEdit::SetActivePagePrefab()
{
	m_psheetWithTab->SetActivePage( PREFABPAGEINDEX );
	SetActiveIndex( PREFABPAGEINDEX );
}

void CDlgEdit::SetActivePagePiecePoint()
{
	m_psheetWithTab->SetActivePage( PIECEPOINTPAGEINDEX );
	SetActiveIndex( PIECEPOINTPAGEINDEX );
}

void CDlgEdit::SetActiveIndex( const INT emEditType )
{
	const int nIndex = m_comboEditType.FindString( 0, g_strEditSheet[ emEditType ] );
	m_comboEditType.SetCurSel( nIndex );
}

void CDlgEdit::SetViewFrame()
{
	if ( GetActiveIndex() == CAMERAPAGEINDEX )
	{
		m_psheetWithTab->m_CameraPage.UpdateContrl();
	}
}

void CDlgEdit::SetViewOctree()
{
	if ( GetActiveIndex() == CAMERAPAGEINDEX )
	{
		m_psheetWithTab->m_CameraPage.UpdateContrl();
	}
}

void CDlgEdit::SetViewPrefab()
{
	if ( GetActiveIndex() == CAMERAPAGEINDEX )
	{
		m_psheetWithTab->m_CameraPage.UpdateContrl();
	}
}