// PieceManagerEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/Common/SUBPATH.H"
#include "SheetWithTab.h"

#include "../EngineLib/DxTools/DxInputDevice.h"

#include "../EngineLib/DxLand/DxPiece/DxPieceManagerPSF.h"

#include "PieceManagerEdit.h"
#include ".\piecemanageredit.h"
#include "State/WorldStateMachine.h"

extern D3DXVECTOR3			g_vCLOSED_POINT;
extern std::string			g_strPICK_FRAME;

// CPieceManagerEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPieceManagerEdit, CPropertyPage)
CPieceManagerEdit::CPieceManagerEdit()
	: CPropertyPage(CPieceManagerEdit::IDD),
	m_bInitDlg(FALSE),
	m_bFrameSelect(TRUE)
{
}

CPieceManagerEdit::~CPieceManagerEdit()
{
}

void CPieceManagerEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EX_SEARCH, m_comboExSearch);
	DDX_Control(pDX, IDC_LIST_FRAMES, m_FrameList);
	DDX_Control(pDX, IDC_LIST_REPLACE, m_listReplace);
}


BEGIN_MESSAGE_MAP(CPieceManagerEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_REPLACEOBJ, OnBnClickedButtonSingleReplaceobj)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_REPLACEOBJ_APPLY, OnBnClickedButtonSingleReplaceobjApply)
	ON_BN_CLICKED(IDC_BUTTON_FRAMESEARCH, OnBnClickedButtonFramesearch)
	ON_BN_CLICKED(IDC_BUTTON_SELECTFILE, OnBnClickedButtonSelectfile)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE_PROCESSING, OnBnClickedButtonReplaceProcessing)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_CBN_SELCHANGE(IDC_COMBO_EX_SEARCH, OnCbnSelchangeComboExSearch)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REPLACE, OnNMClickListReplace)
	ON_BN_CLICKED(IDC_RADIO_FRAME, OnBnClickedRadioFrame)
	ON_BN_CLICKED(IDC_RADIO_POINT, OnBnClickedRadioPoint)
	ON_BN_CLICKED(IDC_BUTTON_TEXTEXPORT, OnBnClickedButtonTextexport)
END_MESSAGE_MAP()


// CPieceManagerEdit �޽��� ó�����Դϴ�.

BOOL CPieceManagerEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = FRAME_VIEW;

	// Note : ListControl
	RECT listRect;
	m_listReplace.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*0.5f), int(nListWidth*0.5f) };

	m_listReplace.SetExtendedStyle ( m_listReplace.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_listReplace.InsertColumn ( 0, _T("Piece"),	LVCFMT_CENTER, nColWidth[0] );
	m_listReplace.InsertColumn ( 1, _T("Frame"),	LVCFMT_CENTER, nColWidth[1] );

	// Note : Combo
	int nIndex;
	m_comboExSearch.ResetContent ();
	nIndex = m_comboExSearch.AddString( "NONE" );
	m_comboExSearch.SetItemData( nIndex, 0 );

	nIndex = m_comboExSearch.AddString( "AND" );
	m_comboExSearch.SetItemData( nIndex, 1 );

	nIndex = m_comboExSearch.AddString( "NOT" );
	m_comboExSearch.SetItemData( nIndex, 2 );

	m_comboExSearch.SetCurSel(0);

	// Note : Radio
	if( m_bFrameSelect )
	{
		((CButton*)GetDlgItem( IDC_RADIO_FRAME ))->SetCheck( TRUE );
		((CButton*)GetDlgItem( IDC_RADIO_POINT ))->SetCheck( FALSE );
	}

	m_bInitDlg = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CPieceManagerEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = PIECE_REPLACE_VIEW;

	ReSetListCtrl();

	return CPropertyPage::OnSetActive();
}

void CPieceManagerEdit::UpdatePage()
{
	if( !m_bInitDlg )		return;

	if( m_bFrameSelect )
	{
		GetDlgItem( IDC_EDIT_SET_FRAME )->SetWindowText( g_strPICK_FRAME.c_str() );
	}
	else
	{
		GetDlgItem( IDC_EDIT_SET_FRAME )->SetWindowText( "" );
	}
}

void CPieceManagerEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();

	m_pSheetTab->SetActiveBackUpPage( PIECEMANAGERINDEX );
}

void CPieceManagerEdit::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( "Please. Check Point" );
		return;
	}

	int nSize = m_listReplace.GetSelectedCount();
	if( nSize==0 )	return;

	int nItem = m_listReplace.GetNextSelectedItem( pos );
	for( int i=nItem; i<(nItem+nSize); ++i )
	{
		CString strName = m_listReplace.GetItemText( i, 1 );

		if( strName.GetLength() >= 1 )
		{
			pPieceManager->DeletePiece( pView->GetFrameMesh(), strName.GetString() );
		}
		//else
		//{
		//	pPieceManager->DeletePiece( (DWORD)nItem );
		//}
	}

    WorldStateMachine::EditAsset(_T("DELETE"));
	//WorldStateMachine::ClearFileNotifier();
	//WorldStateMachine::InsertFileNotifier();

	// Note : List Ctrl �缳��
	ReSetListCtrl();
}

void CPieceManagerEdit::ReSetListCtrl()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	m_listReplace.DeleteAllItems ();

    LPCSTR pPieceName = NULL;
	LPCSTR pFrameName = NULL;
    DxPieceManagerPSF::MAPCHANGEPIS_ITER iter = pPieceManager->m_mapChangePis.begin();
    for ( int i=0; iter!=pPieceManager->m_mapChangePis.end(); ++iter, ++i )
	{
		if( pPieceManager->IsGetName( pPieceManager->m_vecPointPis.size() + i, pPieceName, pFrameName ) )
		{
			m_listReplace.InsertItem( i, pPieceName );

			if( pFrameName )	m_listReplace.SetItemText( i, 1, pFrameName );
			else				m_listReplace.SetItemText( i, 1, "" );
		}
		else
		{
			MessageBox("Code ERROR. Please. Call Me to Sung-Hwan Han");
			return;
		}
	}

	//LPCSTR pPieceName = NULL;
	//LPCSTR pFrameName = NULL;
	//for ( DWORD i=0; i<pPieceManager->GetSize(); ++i )
	//{
	//	if( pPieceManager->IsGetName( i, pPieceName, pFrameName ) )
	//	{
	//		m_listReplace.InsertItem( i, pPieceName );

	//		if( pFrameName )	m_listReplace.SetItemText( i, 1, pFrameName );
	//		else				m_listReplace.SetItemText( i, 1, "" );
	//	}
	//	else
	//	{
	//		MessageBox("Code ERROR. Please. Call Me to Sung-Hwan Han");
	//		return;
	//	}
	//}

	pView->DeActiveEditMatrix();

}

void CPieceManagerEdit::OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( "Please. Check Point" );
		return;
	}
	
	int nItem = m_listReplace.GetNextSelectedItem( pos );
	CString strName = m_listReplace.GetItemText( nItem, 1 );

	if( strName.GetLength() >= 1 )
	{
		// Note : ��ġ �̵�
		D3DXMATRIX* pmatWorld(NULL);
		pmatWorld = pPieceManager->MoveCamera( pView->GetFrameMesh()->GetFrameRoot(), strName.GetString() );
		if( pmatWorld )	pView->ActiveEditMatrix2( pmatWorld );
	}
	//else
	//{
	//	D3DXMATRIX* pmatWorld(NULL);
	//	pmatWorld = pPieceManager->MoveCamera( (DWORD)nItem );
	//	if( pmatWorld )	pView->ActiveEditMatrix2( pmatWorld );
	//}

	// Note : ���� �޽����� ǥ�ø� �ؾ��Ѵ�.
	DxFrameMesh* pFrameMesh = pView->GetSetLandMan()->GetFrameMesh();
	DxFrame* pFrame = pFrameMesh->FindFrame( strName.GetString() );
	pFrameMesh->SetCollisionDetectedFrame( pFrame );

	pView->SetCurType( _CUR_SELECT );
}

//--------------------------------------------------------------------------------------
// Note : Single
//--------------------------------------------------------------------------------------
void CPieceManagerEdit::OnBnClickedRadioFrame()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bFrameSelect = TRUE;

	GetDlgItem( IDC_EDIT_SET_FRAME )->SetWindowText( g_strPICK_FRAME.c_str() );
}

void CPieceManagerEdit::OnBnClickedRadioPoint()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bFrameSelect = FALSE;

	GetDlgItem( IDC_EDIT_SET_FRAME )->SetWindowText( "" );
}

void CPieceManagerEdit::OnBnClickedButtonSingleReplaceobj()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString szFilter = "Piece File (*.pie)|*.pie";
	CFileDialog dlg ( TRUE, ".pie", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem( IDC_EDIT_SINGLE_REPLACEOBJ )->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CPieceManagerEdit::OnBnClickedButtonSingleReplaceobjApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_bFrameSelect )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

		if( !g_strPICK_FRAME.size() )
		{
			MessageBox ( "��ü�� �����ϼ���." );
			return;
		}

		CString strPieceName;
		GetDlgItem( IDC_EDIT_SINGLE_REPLACEOBJ )->GetWindowText( strPieceName );
		if( strPieceName.GetLength() <= 0 )
		{
			MessageBox ( "���������� �����ϼ���." );
			return;
		}

		pPieceManager->SetPiece( pView->GetD3DDevice(), pView->GetFrameMesh(), 
								g_strPICK_FRAME.c_str(), strPieceName.GetString() );

	} // if( m_bFrameSelect )
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

		CString strPieceName;
		GetDlgItem( IDC_EDIT_SINGLE_REPLACEOBJ )->GetWindowText( strPieceName );
		if( strPieceName.GetLength() <= 0 )
		{
			MessageBox ( "���������� �����ϼ���." );
			return;
		}

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		matWorld._41 = g_vCLOSED_POINT.x;
		matWorld._42 = g_vCLOSED_POINT.y;
		matWorld._43 = g_vCLOSED_POINT.z;
		pPieceManager->SetPiece( pView->GetD3DDevice(), strPieceName.GetString(), matWorld );
	}

    WorldStateMachine::EditAsset(0);
	//WorldStateMachine::ClearFileNotifier();
	//WorldStateMachine::InsertFileNotifier();

	// Note : List Ctrl �缳��
	ReSetListCtrl();
}

//--------------------------------------------------------------------------------------
// Note : �� Ư�� �ټ�
//--------------------------------------------------------------------------------------
void CPieceManagerEdit::OnBnClickedButtonFramesearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();
	if ( !pFrameMesh )	return;
	DxFrame *pDxFrame = pFrameMesh->GetFrameRoot();
	if ( !pDxFrame )		return;

	CString strWndText, strWndTextEx;
	GetDlgItem( IDC_EDIT_FINDSTR )->GetWindowText( strWndText );
	GetDlgItem( IDC_EDIT_FINDSTR_EX )->GetWindowText( strWndTextEx );

	int nIndex = m_comboExSearch.GetCurSel();
	EXOPTIONS emOption = (EXOPTIONS) m_comboExSearch.GetItemData ( nIndex );

	if ( strWndText.GetLength() < 2 )
	{
		MessageBox ( "�˻� �ܾ 2�� �̻��� �����Ͻʽÿ�." );
		return;
	}

	strWndText.MakeLower ();
	strWndTextEx.MakeLower ();

	m_FrameList.ResetContent ();
	FindFrame( pDxFrame, strWndText, strWndTextEx, emOption );
}

void CPieceManagerEdit::OnCbnSelchangeComboExSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	int nIndex = m_FrameList.GetCurSel ();
	if ( nIndex == -1 )	return;

	DxFrame *pDxFrame = (DxFrame*) m_FrameList.GetItemData ( nIndex );

	pFrameMesh->SetCollisionDetectedFrame ( pDxFrame );
	
	D3DXVECTOR3 vPos;
	vPos.x = pDxFrame->matCombined._41;
	vPos.y = pDxFrame->matCombined._42;
	vPos.z = pDxFrame->matCombined._43;
	DxViewPort::GetInstance().CameraJump ( vPos );
}


void CPieceManagerEdit::OnBnClickedButtonSelectfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	HRESULT hr = S_OK;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	CWnd *pWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Piece File |*.pie|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(
        TRUE,
        ".pie",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
		(CDlgEdit*)this);

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str());  //SUBPATH::OBJ_FILE_PIECE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;
	if ( dlg.DoModal() == IDOK )
	{
		pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
		pWnd->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CPieceManagerEdit::OnBnClickedButtonReplaceProcessing()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	CString sFileName;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
	pWnd->GetWindowText ( sFileName );
	if ( sFileName.IsEmpty() )	return;

	int nCount = m_FrameList.GetCount();
	for ( int i=0; i<nCount; i++ )
	{
		DxFrame* pDxFrame = (DxFrame*) m_FrameList.GetItemData ( i );

		pPieceManager->SetPiece( pView->GetD3DDevice(), pView->GetFrameMesh(), 
							pDxFrame->szName, sFileName.GetString() );
	}

    WorldStateMachine::EditAsset(0);
	//WorldStateMachine::ClearFileNotifier();
	//WorldStateMachine::InsertFileNotifier();

	// Note : List Ctrl �缳��
	ReSetListCtrl();
}

void CPieceManagerEdit::FindFrame( DxFrame *pframe, CString strFrameStr, CString strFrameStrEx, EXOPTIONS emOption )
{
	if ( pframe->szName )
	{
		CString strFrame = pframe->szName;
		strFrame.MakeLower();

		if ( strFrame.Find(strFrameStr)!=LB_ERR )
		{
			if ( emOption!=EMOPT_NONE && strFrameStrEx.GetLength() )
			{
				if ( emOption==EMOPT_AND )
				{
					if ( strFrame.Find(strFrameStrEx)!=LB_ERR )
					{
						//	Note : ã���� Frame ó��.
						//
						int nIndex = m_FrameList.AddString ( pframe->szName );
						m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
					}
				}
				else if ( emOption==EMOPT_NOT )
				{
					if ( strFrame.Find(strFrameStrEx)==LB_ERR )
					{
						//	Note : ã���� Frame ó��.
						//
						int nIndex = m_FrameList.AddString ( pframe->szName );
						m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
					}
				}
			}
			else
			{
				//	Note : ã���� Frame ó��.
				//
				int nIndex = m_FrameList.AddString ( pframe->szName );
				m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
			}
		}
	}

	if ( pframe->pframeFirstChild )
		FindFrame ( pframe->pframeFirstChild, strFrameStr, strFrameStrEx, emOption );
	
	if ( pframe->pframeSibling )
		FindFrame ( pframe->pframeSibling, strFrameStr, strFrameStrEx, emOption );
}

void CPieceManagerEdit::OnBnClickedButtonTextexport()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	//DxPieceManager* pPieceManager = pView->GetPieceManager();

	//CString szFilter = "PieceList Text (*.pit)|*.pit|";
	//CFileDialog dlg ( FALSE, ".pit", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

 //   CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
 //   const SUBPATH* pPath = pApp->GetSubPath();

	//char szPath[MAX_PATH] = {0};
	//strcpy(szPath, pFrame->m_szAppPath);
	//strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	//CString strTemp = szPath;
	//strTemp += "PieceText\\";
	//dlg.m_ofn.lpstrInitialDir = strTemp;
	//int iCount = 0;
	//char szTempChar[256];
	//
	//if ( dlg.DoModal() == IDOK )
	//{
	//	FILE *pFile = _fsopen( dlg.GetFileName().GetString(), "wt", _SH_DENYNO );
	//	LPCSTR pPieceName = NULL;
	//	LPCSTR pFrameName = NULL;
	//	std::vector<CString> vecString;
	//	for ( DWORD i=0; i<pPieceManager->GetSize(); ++i )
	//	{
	//		if( pPieceManager->IsGetName( i, pPieceName, pFrameName ) )
	//		{
	//			ZeroMemory( szTempChar, 256 );
	//			CString strInput = pPieceName;

	//			bool bAdd = TRUE;
	//			for( UINT j = 0; j < vecString.size(); j++ )
	//			{
	//				if( strInput == vecString[j] )
	//				{
	//					bAdd = FALSE;
	//					break;
	//				}
	//			}
	//			if( bAdd )
	//			{
	//				sprintf( szTempChar, "%d. PieceName: %s\n", iCount, pPieceName );
	//				fwrite( szTempChar, sizeof(char), strlen(szTempChar), pFile );
	//				iCount++;

	//				
	//				vecString.push_back( strInput );
	//			}
	//		}
	//	}
	//	fclose( pFile );
	//}
}

void CPieceManagerEdit::SelectPieceList( DxPiece* pPiece )
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	int nIndex = pPieceManager->GetIndex( pPiece );
	if( nIndex == -1 )	return;

	// Note : List Ctrl �缳��
	ReSetListCtrl();

	// Note : ����.~!
	//m_rmatPiece = &CWorldEditView::g_pPIECE->m_matWorld;
    m_listReplace.SetSelectionMark( nIndex );							// ���� ���õ�.
	m_listReplace.SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED);	// ȸ������ ǥ�õ�.
	m_listReplace.EnsureVisible( nIndex, FALSE );						// ��ũ�� �ؿ� ������ �Ű���.
	m_listReplace.SetFocus();											// Ǫ�������� ǥ�õ�.

	CWorldEditView::g_pPIECEEDIT = NULL;
	CWorldEditView::g_strPIECENAME = _T("");
}
