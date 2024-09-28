
#include "stdafx.h"

#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../State/WorldStateMachine.h"

#include "PiecePointEdit.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
extern DxPiece*			g_pPIECE_SNAP;			// ������ Piece?
extern D3DXVECTOR3		g_vSNAP_POINT;
extern D3DXVECTOR3		g_vCLOSED_POINT;
extern TSTRING			g_strPICK_FRAME;

// CPiecePointEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPiecePointEdit, CPropertyPage)
CPiecePointEdit::CPiecePointEdit()
	: CPropertyPage(CPiecePointEdit::IDD),
	m_bInitDlg(FALSE),
	m_bFrameSelect(TRUE),
	m_rmatPiece(NULL)
{
}

CPiecePointEdit::~CPiecePointEdit()
{
}

void CPiecePointEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPLACE, m_listReplace);
}


BEGIN_MESSAGE_MAP(CPiecePointEdit, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REPLACE, OnNMClickListReplace)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_INIT, &CPiecePointEdit::OnBnClickedButtonRotateInit)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_RANDOM, &CPiecePointEdit::OnBnClickedButtonRotateRandom)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_PICK, &CPiecePointEdit::OnBnClickedButtonHeightPick)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_UP10, &CPiecePointEdit::OnBnClickedButtonHeightUp10)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_UP1, &CPiecePointEdit::OnBnClickedButtonHeightUp1)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_DOWN1, &CPiecePointEdit::OnBnClickedButtonHeightDown1)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_DOWN10, &CPiecePointEdit::OnBnClickedButtonHeightDown10)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPiecePointEdit::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_RESET_ALL, &CPiecePointEdit::OnBnClickedButtonHeightResetAll)
	ON_BN_CLICKED(IDC_BUTTON_TIME_APPLY, &CPiecePointEdit::OnBnClickedButtonTimeApply)
	ON_BN_CLICKED(IDC_RADIO_PUSH, &CPiecePointEdit::OnBnClickedRadioPush)
	ON_BN_CLICKED(IDC_RADIO_DONTMOVE, &CPiecePointEdit::OnBnClickedRadioDontmove)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_MODE, &CPiecePointEdit::OnBnClickedButtonEditMode)
	ON_BN_CLICKED(IDC_BUTTON_FLOAT_ZERO, &CPiecePointEdit::OnBnClickedButtonFloatZero)
	ON_BN_CLICKED(IDC_BUTTON_SLOPE, &CPiecePointEdit::OnBnClickedButtonSlope)
	ON_BN_CLICKED(IDC_CHECK_MY_ROOM_TEST, &CPiecePointEdit::OnBnClickedCheckMyRoomTest)
    ON_BN_CLICKED(IDC_BUTTON_SNAP_BASE, &CPiecePointEdit::OnBnClickedButtonSnapBase)
    ON_BN_CLICKED(IDC_BUTTON_SNAP_MOVE, &CPiecePointEdit::OnBnClickedButtonSnapMove)
    ON_BN_CLICKED(IDC_BUTTON_REPLACE, &CPiecePointEdit::OnBnClickedButtonReplace)
    ON_BN_CLICKED(IDC_BUTTON_SCALE_APPLY, &CPiecePointEdit::OnBnClickedButtonScaleApply)
	ON_BN_CLICKED(IDC_BUTTON_CUR_POS1, &CPiecePointEdit::OnBnClickedButtonCurPos1)
	ON_BN_CLICKED(IDC_BUTTON_CUR_POS2, &CPiecePointEdit::OnBnClickedButtonCurPos2)
	ON_BN_CLICKED(IDC_BUTTON_RESET_FADE, &CPiecePointEdit::OnBnClickedButtonResetFade)
	ON_EN_CHANGE(IDC_EDIT_VISIBLE_RADIUS, &CPiecePointEdit::OnEnChangeEditVisibleRadius)
	ON_EN_CHANGE(IDC_EDIT_INVISIBLE_RADIUS, &CPiecePointEdit::OnEnChangeEditInvisibleRadius)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PIE15, &CPiecePointEdit::OnBnClickedButtonChangePie15)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_L5, &CPiecePointEdit::OnBnClickedButtonRotateL5)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_R5, &CPiecePointEdit::OnBnClickedButtonRotateR5)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPiecePointEdit::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_CHECK_SHADER_3_0, &CPiecePointEdit::OnBnClickedButtonShader30)
END_MESSAGE_MAP()


// CPiecePointEdit �޽��� ó�����Դϴ�.

BOOL CPiecePointEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = PIECE_SETPOS_VIEW;

	// Note : ListControl
	RECT listRect;
	m_listReplace.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.70f), int(nListWidth*0.15f), int(nListWidth*0.15f) };

	m_listReplace.SetExtendedStyle ( m_listReplace.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_listReplace.InsertColumn( 0, _T("Piece"),	LVCFMT_CENTER, nColWidth[0] );
	m_listReplace.InsertColumn( 1, _T("Time"),	LVCFMT_CENTER, nColWidth[1] );
	m_listReplace.InsertColumn( 2, _T("Push"),	LVCFMT_CENTER, nColWidth[2] );

	m_bInitDlg = TRUE;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
	pWnd->SetWindowText( _T("5") );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CPiecePointEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	pView->m_ViewType = PIECE_SETPOS_VIEW;

	ReSetListCtrl();

	CButton* pButton = (CButton*)GetDlgItem( IDC_RADIO_PUSH );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_DONTMOVE );
	pButton->SetCheck( FALSE );

	pFrame->m_cDlgEditPIECE.ShowWindow(SW_SHOW);
	pFrame->m_cDlgEditPIECE.UpdateLIST();
	
	DxPieceManagerPSF::g_emEDITMODE = DxPieceManagerPSF::MODE_EDIT;

    CWnd* pWnd(NULL);
    pWnd = GetDlgItem( IDC_EDIT_POS_X );
	pWnd->SetWindowText( _T("0.f") );
    pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	pWnd->SetWindowText( _T("0.f") );
    pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	pWnd->SetWindowText( _T("0.f") );
    pWnd = GetDlgItem( IDC_EDIT_PIECE_SCALE );
	pWnd->SetWindowText( _T("1.f") );

	pWnd = GetDlgItem( IDC_EDIT_VISIBLE_RADIUS );
	pWnd->SetWindowText( _T("0.f") );
	pWnd = GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS );
	pWnd->SetWindowText( _T("0.f") );

	pWnd = GetDlgItem( IDC_EDIT_SLOPE_RATE );
	pWnd->SetWindowText( _T("1.f") );

	//pButton = (CButton*)GetDlgItem( IDC_CHECK_MY_ROOM_TEST );
	//pButton->SetCheck( FALSE );
	//DxPieceManager::g_bMYROOM_MODE = FALSE;

	return CPropertyPage::OnSetActive();
}

BOOL CPiecePointEdit::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pFrame->m_cDlgEditPIECE.ShowWindow(SW_HIDE);

	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CPiecePointEdit::OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}
	
	int nItem = m_listReplace.GetNextSelectedItem( pos );
	CString strName = m_listReplace.GetItemText( nItem, 0 );

	//if( strName.GetLength() >= 1 )
	//{
	//	// Note : ��ġ �̵�
	//	m_rmatPiece = pPieceManager->MoveCamera( pView->GetFrameMesh()->GetFrameRoot(), strName.GetString() );
	//	if( m_rmatPiece )	pView->ActiveEditMatrix2( m_rmatPiece );
	//	CWorldEditView::g_pPIECE = pPieceManager->GetPiece( (DWORD)nItem );
	//	//CWorldEditView::g_pPIECE = NULL;
	//}
	//else
	//{
		m_rmatPiece = pPieceManager->MoveCamera( (DWORD)nItem );
		if( m_rmatPiece )	
        {
            pView->ActiveEditMatrix2( m_rmatPiece );
            SetPositionEdit();
        }
		CWorldEditView::g_pPIECE = pPieceManager->GetPiece( (DWORD)nItem );
	//}

	//// Note : �ִϸ��̼��� �ִ� ��ü�� �ʱ� Ÿ��.
	//DxPieceManagerPSF::MATRIXPISNAME* pMATRIXPISNAME = pPieceManager->GetMATRIXPISNAME( nItem );
	//if( pMATRIXPISNAME )
	//{
 //       CString Str;
	//    Str.Format ( "%3.2f", pMATRIXPISNAME->m_fStartAniTime );
	//    GetDlgItem ( IDC_EDIT_TIME ) -> SetWindowText ( Str );
	//}
	//else
	//{
 //       CString Str;
	//    Str.Format ( "%3.2f", 0.f );
 //       GetDlgItem ( IDC_EDIT_TIME ) -> SetWindowText ( Str );
	//}

	//if( pMATRIXPISNAME->m_bColl_Push )
	//{
	//	CButton* pButton = (CButton*)GetDlgItem( IDC_RADIO_PUSH );
	//	pButton->SetCheck( TRUE );
	//	pButton = (CButton*)GetDlgItem( IDC_RADIO_DONTMOVE );
	//	pButton->SetCheck( FALSE );
	//}
	//else
	//{
	//	CButton* pButton = (CButton*)GetDlgItem( IDC_RADIO_PUSH );
	//	pButton->SetCheck( FALSE );
	//	pButton = (CButton*)GetDlgItem( IDC_RADIO_DONTMOVE );
	//	pButton->SetCheck( TRUE );
	//}

	{
		CString Str;

		Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

		Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SHADER_3_0 );
	pButton->SetCheck( CWorldEditView::g_pPIECE->m_bExceptLowOption );
	

	// Note : ���� �޽����� ǥ�ø� �ؾ��Ѵ�.
	DxFrameMesh* pFrameMesh = pView->GetSetLandMan()->GetFrameMesh();
	DxFrame* pFrame = pFrameMesh->FindFrame( strName.GetString() );
	pFrameMesh->SetCollisionDetectedFrame( pFrame );

	CWorldEditView::g_pPIECEEDIT = NULL;
	CWorldEditView::g_strPIECENAME = _T("");

	DxPieceManagerPSF::g_emEDITMODE = DxPieceManagerPSF::MODE_EDIT;

	pView->SetCurType( _CUR_SELECT );
}

void CPiecePointEdit::ReSetListCtrl()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	m_listReplace.DeleteAllItems ();

	//char szValue[256];
	//WCHAR szValueW[256];
	LPCTSTR pPieceName = NULL;
	LPCTSTR pFrameName = NULL;
    for ( DWORD i=0; i<pPieceManager->m_vecPointPis.size(); ++i )
	{
		if( pPieceManager->IsGetName( i, pPieceName, pFrameName ) )
		{
			m_listReplace.InsertItem( i, pPieceName );

			//// Note : Ÿ�̹�.
			//DxPieceManagerPSF::MATRIXPISNAME* pMATRIXPISNAME = pPieceManager->GetMATRIXPISNAME( i );
			//_gcvt( pMATRIXPISNAME->m_fStartAniTime, 7, szValue );
			//MultiByteToWideChar( CP_ACP, 0, szValue, -1, szValueW, 256 );
			//szValueW[256-1] = 0;
			//m_listReplace.SetItemText( i, 1, szValueW );

			//if( pMATRIXPISNAME->m_bColl_Push )
			//{
			//	m_listReplace.SetItemText( i, 2, _T("0") );
			//}
			//else
			//{
			//	m_listReplace.SetItemText( i, 2, _T("X") );
			//}
		}
		else
		{
			MessageBox( _T("Code ERROR. Please. Call Me to Sung-Hwan Han") );
			return;
		}
	}
}

void CPiecePointEdit::SetFocus( CListCtrl& listReplace, int nIndex )
{
	listReplace.SetSelectionMark( nIndex );								// ���� ���õ�.
	listReplace.SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED);	// ȸ������ ǥ�õ�.
	listReplace.EnsureVisible( nIndex, FALSE );							// ��ũ�� �ؿ� ������ �Ű���.
	listReplace.SetFocus();												// Ǫ�������� ǥ�õ�.
}

void CPiecePointEdit::SelectPieceList( DxPiece* pPiece, int nCurType )
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	int nIndex = pPieceManager->GetIndex( pPiece );
	if( nIndex == -1 )	return;

	// Note : List Ctrl �缳��
	//ReSetListCtrl();

    // Note : ����.~!
    m_rmatPiece = &CWorldEditView::g_pPIECE->m_matWorld;
    SetPositionEdit();

    _CUR_TYPE emCurType = static_cast<_CUR_TYPE>( nCurType );
    switch( emCurType )
    {
    case _CUR_SELECT:
	    pView->ActiveEditMatrix2( m_rmatPiece );
        break;

    case _CUR_SELECT_POINT:
        pView->DeActiveEditMatrix();
        break;
    };

	// ������ ���õǾ� �ִ°� ȸ������
	int nCount = m_listReplace.GetItemCount();
	for ( int i=0; i<nCount; ++i )
	{
		if ( m_listReplace.GetItemState( i, LVIS_SELECTED ) != 0 )
		{
			m_listReplace.SetItemState( i, 0, LVIS_SELECTED );	// ȸ������
		}
	}

	// ������ ��ġ�� �̵�
	SetFocus( m_listReplace, nIndex);

	// SetFocus �� �� �Ŀ� �۾��� �Ǿ�� �Ѵ�.
	{
		CString Str;

		Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

		Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SHADER_3_0 );
	pButton->SetCheck( CWorldEditView::g_pPIECE->m_bExceptLowOption );

	CWorldEditView::g_pPIECEEDIT = NULL;
	CWorldEditView::g_strPIECENAME = _T("");

	pView->SetCurType( emCurType );
}

void CPiecePointEdit::InsertPiece()
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();
	D3DXMATRIX matWorld;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply(&matWorld,&matWorld,&pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.GetPieceMatrix());

	matWorld._41 = g_vCLOSED_POINT.x;
	matWorld._42 = g_vCLOSED_POINT.y;
	matWorld._43 = g_vCLOSED_POINT.z;
	if( pPieceManager->SetPiece( pView->GetD3DDevice(), 
                                CWorldEditView::g_strPIECENAME.c_str(), 
                                matWorld,
								pView->GetSetLandMan(),
								pFrame->m_cDlgEditPIECE.m_bSlope,
								pFrame->m_cDlgEditPIECE.m_fSlopeRate) )
	{
		UpdatePage_List_AddLastItem2(); 
	}

	WorldStateMachine::EditAsset(0);
	//WorldStateMachine::ClearFileNotifier();
	//WorldStateMachine::InsertFileNotifier();

	// CMainFrame�� Focus �� �ش�.
	pFrame->SetFocus();
}

void CPiecePointEdit::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	INT nCount = m_listReplace.GetSelectedCount();
	if( !nCount )
		return;

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_listReplace.GetNextSelectedItem( pos );
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();

	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		for( INT j=0; j<i; ++j )
		{
			if( nDeleteList[ i ] > nDeleteList[ j ] )
				--nCurIndex;
		}
		pPieceManager->DeletePiece( nCurIndex );        
	}

	WorldStateMachine::EditAsset(0);
	//WorldStateMachine::ClearFileNotifier();
	//WorldStateMachine::InsertFileNotifier();

	i = nDeleteList[ 0 ];
	SAFE_DELETE_ARRAY( nDeleteList );

	ReSetListCtrl();

	m_rmatPiece = NULL;
    g_pPIECE_SNAP = NULL;
	CWorldEditView::g_pPIECE = NULL;

	// Note : ctrllist �� Ȱ��ȭ ��ġ ����
	nCount = m_listReplace.GetItemCount();
	if( nCount == 0 )	return;
	if( i >= nCount )	i = nCount - 1;

	// ������ ��ġ�� �̵�
	SetFocus( m_listReplace, i );
}

void CPiecePointEdit::KeyBoard_Delete()
{
	if( !m_bInitDlg )	return;

	OnBnClickedButtonDelete();
}

// Note : Clone�� �ٿ�.
//			List�� ������Ʈ �� �� ���� ��ġ�� �ٲٰ�, �״��� List ������Ʈ �Ͽ��� ������ �Ѵ�.
void CPiecePointEdit::UpdatePage_List_AddLastItem()
{
	if ( !UpdatePage_List_AddLastItem2() )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	m_rmatPiece = &pPieceManager->m_vecPointPis[ pPieceManager->m_vecPointPis.size()-1 ]->m_pPiece->m_matWorld;
}

// ������ �����͸� list �� �ø���.
BOOL CPiecePointEdit::UpdatePage_List_AddLastItem2()
{
	if( !m_bInitDlg )	
		return FALSE;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	// �߰��� �ӵ��� ������ �� �� �ְ� ����.
	int nCount = m_listReplace.GetItemCount();
	for ( int i=0; i<nCount; ++i )
	{
		if ( m_listReplace.GetItemState( i, LVIS_SELECTED ) != 0 )
		{
			m_listReplace.SetItemState( i, 0, LVIS_SELECTED );	// ȸ������
		}
	}

	m_listReplace.InsertItem( nCount, pPieceManager->m_vecPointPis[ pPieceManager->m_vecPointPis.size()-1 ]->m_pPisName );
	m_listReplace.SetItemState( nCount, LVIS_SELECTED, LVIS_SELECTED);	// ȸ������ ǥ�õ�.

	SetFocus( m_listReplace, pPieceManager->m_vecPointPis.size()-1 );

	return TRUE;
}

void CPiecePointEdit::UpdatePageClickUP()
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();
	pPieceManager->MakeTree();

    // �̵��� �ߴٸ� �� ��ġ�� �ٽ� �־��ش�.
    SetPositionEdit();
}

//--------------------------------------------------------------------
//							E	D	I	T
void CPiecePointEdit::OnBnClickedButtonRotateInit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	(*m_rmatPiece)._11 = 1.f;	(*m_rmatPiece)._12 = 0.f;	(*m_rmatPiece)._13 = 0.f;
	(*m_rmatPiece)._21 = 0.f;	(*m_rmatPiece)._22 = 1.f;	(*m_rmatPiece)._23 = 0.f;
	(*m_rmatPiece)._31 = 0.f;	(*m_rmatPiece)._32 = 0.f;	(*m_rmatPiece)._33 = 1.f;
}

void CPiecePointEdit::OnBnClickedButtonRotateRandom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*sc::Random::RANDOM_POS() );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPiecePointEdit::OnBnClickedButtonRotateL5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )
		return;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
	pWnd->GetWindowText( strText );

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*((float)_tstof ( strText.GetString() )/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );

	// Edit Focus �� ���� ��찡 �־ ���� Focus �� ���ش�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->SetFocus();
}

void CPiecePointEdit::OnBnClickedButtonRotateR5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )
		return;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
	pWnd->GetWindowText( strText );

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*(-(float)_tstof ( strText.GetString() )/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );

	// Edit Focus �� ���� ��찡 �־ ���� Focus �� ���ش�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->SetFocus();
}

void CPiecePointEdit::OnBnClickedButtonHeightPick()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
    DxFrameMesh* pFrameMesh = pView->GetFrameMesh();
    if ( !pFrameMesh )
        return;
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();
    if ( !pPieceManager )
        return;

    D3DXVECTOR3 vStart;
	D3DXVECTOR3 vEnd;
    vEnd.x = vStart.x = (*m_rmatPiece)._41;
	vEnd.z = vStart.z = (*m_rmatPiece)._43;
	vStart.y = pFrameMesh->GetTreeMax().y + 1.f;
	vEnd.y = pFrameMesh->GetTreeMin().y - 1.f;

    if ( pFrameMesh->IsCollision_NoneConst( vStart, vEnd, NULL, NULL, EMCC_CULL_CCW ) )
    {
        m_rmatPiece->_42 = vEnd.y;
    }

	//D3DXVECTOR3 vStart;
	//D3DXVECTOR3 vEnd;
	//D3DXVECTOR3 vColl;
	//D3DXVECTOR3 vMin;
	//D3DXVECTOR3 vMax;

	//NSHfMain::GetAABB( vMin, vMax );

	//vEnd.x = vStart.x = (*m_rmatPiece)._41;
	//vEnd.z = vStart.z = (*m_rmatPiece)._43;
	//vStart.y = vMax.y + 0.0001f;
	//vEnd.y = vMin.y - 0.0001f;

	//NSHfMain::IsCollision( vStart, vEnd, FALSE, FALSE );
	//vColl = vEnd;
	//m_rmatPiece->_42 = vColl.y;
}

void CPiecePointEdit::OnBnClickedButtonHeightUp10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

	(*m_rmatPiece)._42 += 10.f;
    SetPositionEdit();
}

void CPiecePointEdit::OnBnClickedButtonHeightUp1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

	(*m_rmatPiece)._42 += 1.f;
    SetPositionEdit();
}

void CPiecePointEdit::OnBnClickedButtonHeightDown1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

	(*m_rmatPiece)._42 -= 1.f;
    SetPositionEdit();
}

void CPiecePointEdit::OnBnClickedButtonHeightDown10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

	(*m_rmatPiece)._42 -= 10.f;
    SetPositionEdit();
}

void CPiecePointEdit::OnBnClickedButtonHeightResetAll()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//DxPieceManager* pPieceManager = pView->GetPieceManager();

	//D3DXVECTOR3 vStart;
	//D3DXVECTOR3 vEnd;
	//D3DXVECTOR3 vColl;
	//D3DXVECTOR3 vMin;
	//D3DXVECTOR3 vMax;
	//NSHfMain::GetAABB( vMin, vMax );
	//for ( DWORD i=0; i<pPieceManager->m_vecPointPis.size(); ++i )
	//{
	//	vEnd.x = vStart.x = pPieceManager->m_vecPointPis[i]->m_pPiece->m_matWorld._41;
	//	vEnd.z = vStart.z = pPieceManager->m_vecPointPis[i]->m_pPiece->m_matWorld._43;
	//	vStart.y = vMax.y + 0.0001f;
	//	vEnd.y = vMin.y - 0.0001f;

	//	NSHfMain::IsCollision( vStart, vEnd, FALSE, FALSE );
	//	vColl = vEnd;
	//	pPieceManager->m_vecPointPis[i]->m_pPiece->m_matWorld._42 = vColl.y;
	//}
}

void CPiecePointEdit::OnBnClickedButtonTimeApply()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//DxPieceManager* pPieceManager = pView->GetPieceManager();

	//POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	//if( pos==NULL )
	//{
	//	MessageBox( _T("Please. Check Point") );
	//	return;
	//}
	//
	//int nItem = m_listReplace.GetNextSelectedItem( pos );

	//DxPiece* pPiece = pPieceManager->GetPiece( nItem );
	//DxPieceManagerPSF::MATRIXPISNAME* pMATRIXPISNAME = pPieceManager->GetMATRIXPISNAME( nItem );

	//CWnd* pWnd = GetDlgItem( IDC_EDIT_TIME );
	//CString strText;
	//pWnd->GetWindowText( strText );
	//pMATRIXPISNAME->m_fStartAniTime = (float)_tstof ( strText.GetString() );
	//pPiece->m_fStartAniTime = pMATRIXPISNAME->m_fStartAniTime;

	//// Note : Ÿ�̹�.
	//char szValue[256];
	//WCHAR szValueW[256];
	//_gcvt( pMATRIXPISNAME->m_fStartAniTime, 7, szValue );
	//MultiByteToWideChar( CP_ACP, 0, szValue, -1, szValueW, 256 );
	//szValueW[256-1] = 0;
	//m_listReplace.SetItemText( nItem, 1, szValueW );
	////ReSetListCtrl();
}

void CPiecePointEdit::OnBnClickedRadioPush()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//DxPieceManager* pPieceManager = pView->GetPieceManager();

	//POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	//if( pos==NULL )
	//{
	//	MessageBox( _T("Please. Check Point") );
	//	return;
	//}
	//
	//int nItem = m_listReplace.GetNextSelectedItem( pos );

	//DxPiece* pPiece = pPieceManager->GetPiece( nItem );
	//DxPieceManagerPSF::MATRIXPISNAME* pMATRIXPISNAME = pPieceManager->GetMATRIXPISNAME( nItem );
	//pMATRIXPISNAME->m_bColl_Push = TRUE;

	//ReSetListCtrl();

	//// ������ ��ġ�� �̵�
	//SetFocus( m_listReplace, nItem );
}

void CPiecePointEdit::OnBnClickedRadioDontmove()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//DxPieceManager* pPieceManager = pView->GetPieceManager();

	//POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	//if( pos==NULL )
	//{
	//	MessageBox( _T("Please. Check Point") );
	//	return;
	//}
	//
	//int nItem = m_listReplace.GetNextSelectedItem( pos );

	//DxPiece* pPiece = pPieceManager->GetPiece( nItem );
	//DxPieceManagerPSF::MATRIXPISNAME* pMATRIXPISNAME = pPieceManager->GetMATRIXPISNAME( nItem );
	//pMATRIXPISNAME->m_bColl_Push = FALSE;

	//ReSetListCtrl();

	//// ������ ��ġ�� �̵�
	//SetFocus( m_listReplace, nItem );
}

void CPiecePointEdit::OnBnClickedButtonEditMode()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	DxPieceManagerPSF::g_emEDITMODE = DxPieceManagerPSF::MODE_EDIT;
}

void CPiecePointEdit::SetHalfDecimal( float& fResult )
{
    float fValue(0);
    float fValue2 = modf( fResult, &fValue );
    if ( fValue2 > 0.f )        // +
    {
        if ( fValue2 >= 0.5f )
            fValue += 1;
    }
    else if ( fValue2 < 0.f )   // -
    {
        if ( fValue2 < -0.5f )
            fValue -= 1;
    }

    fResult = fValue;
}

void CPiecePointEdit::OnBnClickedButtonFloatZero()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
        return;

    SetHalfDecimal( (*m_rmatPiece)._41 );
    SetHalfDecimal( (*m_rmatPiece)._42 );
    SetHalfDecimal( (*m_rmatPiece)._43 );

    SetPositionEdit();
}


void CPiecePointEdit::OnBnClickedCheckMyRoomTest()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_MY_ROOM_TEST );
	//if( pButton->GetCheck() )
	//{
	//	DxPieceManager::g_bMYROOM_MODE = TRUE;

	//	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//	NSMyRoom::Init( pView->GetD3DDevice(), NULL );
	//}
	//else
	//{
	//	DxPieceManager::g_bMYROOM_MODE = FALSE;
	//}
}

void CPiecePointEdit::OnBnClickedButtonSnapBase()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    g_pPIECE_SNAP = CWorldEditView::g_pPIECE;
    g_vSNAP_POINT = g_vCLOSED_POINT;
}

void CPiecePointEdit::OnBnClickedButtonSnapMove()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    if ( !g_pPIECE_SNAP )
        return;

    D3DXVECTOR3 vDir = g_vCLOSED_POINT - g_vSNAP_POINT;
    g_pPIECE_SNAP->m_matWorld._41 += vDir.x;
    g_pPIECE_SNAP->m_matWorld._42 += vDir.y;
    g_pPIECE_SNAP->m_matWorld._43 += vDir.z;

    D3DXMatrixInverse( &g_pPIECE_SNAP->m_matInverse, NULL, &g_pPIECE_SNAP->m_matWorld );

    g_vSNAP_POINT = g_vCLOSED_POINT;

    SetPositionEdit();
}

void CPiecePointEdit::OnBnClickedButtonReplace()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CPiecePointEdit::UpdateKeyBoardMouse()
{
	if ( (DxInputDevice::GetInstance().GetKeyState( DIK_RETURN ) & DXKEY_UP) ||
		(DxInputDevice::GetInstance().GetKeyState( DIK_NUMPADENTER ) & DXKEY_UP) )
	{
		KeyBoard_Enter();
	}

	// EditBox �� Focus �� �ִٸ� ����Ű�� �����ϵ��� �Ѵ�.
	{
		CWnd* pWndFocus = GetFocus();

		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_POS_X );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_POS_Y );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_POS_Z );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_TIME );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_PIECE_SCALE );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_VISIBLE_RADIUS );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_SLOPE_RATE );
		if ( pWnd == pWndFocus )
			return;

		pWnd = GetDlgItem( IDC_EDIT_ROTATION_Y );
		if ( pWnd == pWndFocus )
		{
			if ( DxInputDevice::GetInstance().GetKeyState( DIK_T ) & DXKEY_UP )
			{
				OnBnClickedButtonRotateL5();
			}
			if ( DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
			{
				OnBnClickedButtonRotateR5();
			}
			return;
		}
	}


    if ( DxInputDevice::GetInstance().GetKeyState( DIK_DELETE ) & DXKEY_UP )
    {
        KeyBoard_Delete();
    }
    if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
    {
        UpdatePageClickUP();
    }
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD2 ) & DXKEY_UP )
	{
		if ( m_rmatPiece )
			(*m_rmatPiece)._43 -= 1.f;

		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD4 ) & DXKEY_UP )
	{
		if ( m_rmatPiece )
			(*m_rmatPiece)._41 -= 1.f;

		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD6 ) & DXKEY_UP )
	{
		if ( m_rmatPiece )
			(*m_rmatPiece)._41 += 1.f;

		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD8 ) & DXKEY_UP )
	{
		if ( m_rmatPiece )
			(*m_rmatPiece)._43 += 1.f;

		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_T ) & DXKEY_UP )
	{
		OnBnClickedButtonRotateL5();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
	{
		OnBnClickedButtonRotateR5();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_F ) & DXKEY_UP )
	{
		OnBnClickedButtonHeightPick();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_ESCAPE ) & DXKEY_UP )
	{
		OnBnClickedButtonEditMode();
	}
}

void CPiecePointEdit::KeyBoard_Enter()
{
    if( !m_rmatPiece )	
        return;

    CWnd* pWnd(NULL);
    CString strText;

    pWnd = GetDlgItem( IDC_EDIT_POS_X );
	pWnd->GetWindowText( strText );
    (*m_rmatPiece)._41 = (float)_tstof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	pWnd->GetWindowText( strText );
    (*m_rmatPiece)._42 = (float)_tstof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	pWnd->GetWindowText( strText );
    (*m_rmatPiece)._43 = (float)_tstof ( strText.GetString() );

    UpdatePageClickUP();
}

void CPiecePointEdit::SetPositionEdit()
{
    if( !m_rmatPiece )	
        return;

    CWnd* pWnd(NULL);
    CString strText;
    CString strTextPrev;

    pWnd = GetDlgItem( IDC_EDIT_POS_X );
    pWnd->GetWindowText( strTextPrev );
    strText.Format ( "%3.2f", (*m_rmatPiece)._41 );
    if ( strTextPrev != strText )
    {
	    pWnd->SetWindowText( strText );
    }

    pWnd = GetDlgItem( IDC_EDIT_POS_Y );
    pWnd->GetWindowText( strTextPrev );
    strText.Format ( "%3.2f", (*m_rmatPiece)._42 );
	if ( strTextPrev != strText )
    {
	    pWnd->SetWindowText( strText );
    }

    pWnd = GetDlgItem( IDC_EDIT_POS_Z );
    pWnd->GetWindowText( strTextPrev );
    strText.Format ( "%3.2f", (*m_rmatPiece)._43 );
	if ( strTextPrev != strText )
    {
	    pWnd->SetWindowText( strText );
    }
}

void CPiecePointEdit::OnBnClickedButtonScaleApply()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

    if( !m_rmatPiece )	return;

    CString strText;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_PIECE_SCALE );
	pWnd->GetWindowText( strText );
    float fScale = (float)_tstof ( strText.GetString() );

	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, fScale, fScale, fScale );
	D3DXMatrixMultiply( m_rmatPiece, &matScale, m_rmatPiece );
}

void CPiecePointEdit::OnBnClickedButtonCurPos1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	INT nCount = m_listReplace.GetSelectedCount();
	if( nCount > 1 )
	{
		AfxMessageBox( _T("���߼����� �Ұ����մϴ�.") );
		return;
	}

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	int nItem = m_listReplace.GetNextSelectedItem( pos );

	DxPiece* pPiece = pPieceManager->GetPiece( nItem );
	if ( !pPiece )
		return;

	if ( !CWorldEditView::g_pPIECE )
		return;

	D3DXVECTOR3 LookAtPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vDir = pPiece->m_vCenter - LookAtPt; // ��ü�� ���������� ����� �ش�.
	float fLengthLookat = D3DXVec3Length( &vDir ); // ī�޶� ������ �Ÿ�
	fLengthLookat -= pPiece->m_fRadius; // ��ü�� ���������� ����� �ش�.

	CWorldEditView::g_pPIECE->m_fVisibleRadius = fLengthLookat;

	CString Str;
	Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fVisibleRadius );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );
}

void CPiecePointEdit::OnBnClickedButtonCurPos2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	INT nCount = m_listReplace.GetSelectedCount();
	if( nCount > 1 )
	{
		AfxMessageBox( _T("���߼����� �Ұ����մϴ�.") );
		return;
	}

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	int nItem = m_listReplace.GetNextSelectedItem( pos );

	DxPiece* pPiece = pPieceManager->GetPiece( nItem );
	if ( !pPiece )
		return;

	if ( !CWorldEditView::g_pPIECE )
		return;

	D3DXVECTOR3 LookAtPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vDir = pPiece->m_vCenter - LookAtPt; // ��ü�� ���������� ����� �ش�.
	float fLengthLookat = D3DXVec3Length( &vDir ); // ī�޶� ������ �Ÿ�
	fLengthLookat -= pPiece->m_fRadius; // ��ü�� ���������� ����� �ش�.

	CWorldEditView::g_pPIECE->m_fInvisibleRadius = fLengthLookat;

	CString Str;
	Str.Format( _T("%3.2f"), CWorldEditView::g_pPIECE->m_fInvisibleRadius );
	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
}

void CPiecePointEdit::OnBnClickedButtonResetFade()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPiece* pPiece = pPieceManager->GetPiece( static_cast< DWORD >( nIndex ) );
		if( pPiece )
		{
			pPiece->m_fVisibleRadius = DEFAULT_VISIBLE_RADIUS;
			pPiece->m_fInvisibleRadius = DEFAULT_INVISIBLE_RADIUS;
		}
	}

	CString Str;

	Str.Format( _T("%3.2f"), DEFAULT_VISIBLE_RADIUS );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

	Str.Format( _T("%3.2f"), DEFAULT_INVISIBLE_RADIUS );
	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
}

void CPiecePointEdit::OnEnChangeEditVisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;

	CString Str;
	GetDlgItem(IDC_EDIT_VISIBLE_RADIUS)->GetWindowText( Str );

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPiece* pPiece = pPieceManager->GetPiece( static_cast< DWORD >( nIndex ) );
		if( pPiece )
		{
			pPiece->m_fVisibleRadius = (float)_tstof( Str.GetString() );
		}
	}
}

void CPiecePointEdit::OnEnChangeEditInvisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;

	CString Str;
	GetDlgItem(IDC_EDIT_INVISIBLE_RADIUS)->GetWindowText( Str );

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPiece* pPiece = pPieceManager->GetPiece( static_cast< DWORD >( nIndex ) );
		if( pPiece )
		{
			pPiece->m_fInvisibleRadius = (float)_tstof( Str.GetString() );
		}
	}
}

void CPiecePointEdit::OnBnClickedButtonChangePie15()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
	pSetLandMan->ChangeNamePIE15();
}

void CPiecePointEdit::OnBnClickedButtonSlope()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	INT nCount = m_listReplace.GetSelectedCount();
	if( !nCount )
		return;

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_SLOPE_RATE );
	pWnd->GetWindowText( strText );
	float fSlopeRate = (float)_tstof ( strText.GetString() );

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPiece* pPiece = pPieceManager->GetPiece( static_cast< DWORD >( nIndex ) );
		if( pPiece )
		{
			pPiece->CalculateSlopeMatrix( pView->GetSetLandMan(), fSlopeRate );
		}
	}
}

void CPiecePointEdit::OnBnClickedButtonRefresh()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();
	pPieceManager->SortPointPis();

	ReSetListCtrl();
}

void CPiecePointEdit::OnBnClickedButtonShader30()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPieceManagerPSF* pPieceManager = pView->GetPieceManager();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SHADER_3_0 );
	BOOL bCheck = pButton->GetCheck();

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPiece* pPiece = pPieceManager->GetPiece( static_cast< DWORD >( nIndex ) );
		if( pPiece )
		{
			pPiece->m_bExceptLowOption = bCheck;
		}
	}
}
