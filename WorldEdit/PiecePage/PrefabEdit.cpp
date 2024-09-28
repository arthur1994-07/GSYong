// SPGManagerEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../EngineLib/DxLand/DxPrefabMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"

#include "PrefabEdit.h"

// CPrefabEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPrefabEdit, CPropertyPage)

CPrefabEdit::CPrefabEdit()
: CPropertyPage(CPrefabEdit::IDD)
, m_rmatPiece( NULL )
, m_bInitDlg( FALSE )
{

}

CPrefabEdit::~CPrefabEdit()
{
}

void CPrefabEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPLACE, m_listReplace);
}


BEGIN_MESSAGE_MAP(CPrefabEdit, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REPLACE, OnNMClickListReplace)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_INIT, &CPrefabEdit::OnBnClickedButtonRotateInit)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_RANDOM, &CPrefabEdit::OnBnClickedButtonRotateRandom)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_L90, &CPrefabEdit::OnBnClickedButtonRotateL90)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_L15, &CPrefabEdit::OnBnClickedButtonRotateL15)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_R15, &CPrefabEdit::OnBnClickedButtonRotateR15)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_R90, &CPrefabEdit::OnBnClickedButtonRotateR90)


	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_PICK, &CPrefabEdit::OnBnClickedButtonHeightPick)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_UP10, &CPrefabEdit::OnBnClickedButtonHeightUp10)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_UP1, &CPrefabEdit::OnBnClickedButtonHeightUp1)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_DOWN1, &CPrefabEdit::OnBnClickedButtonHeightDown1)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_DOWN10, &CPrefabEdit::OnBnClickedButtonHeightDown10)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_RESET_ALL, &CPrefabEdit::OnBnClickedButtonHeightResetAll)
	ON_BN_CLICKED(IDC_BUTTON_FLOAT_ZERO, &CPrefabEdit::OnBnClickedButtonFloatZero)
	ON_BN_CLICKED(IDC_BUTTON_PREFAB_SCALE, &CPrefabEdit::OnBnClickedButtonScale)
END_MESSAGE_MAP()


// CPrefabEdit �޽��� ó�����Դϴ�.

BOOL CPrefabEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = PREFAB_VIEW;

	// Note : ListControl
	RECT listRect;
	m_listReplace.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[1] = { int(nListWidth*1.1f) };

	m_listReplace.SetExtendedStyle ( m_listReplace.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_listReplace.InsertColumn( 0, _T("Name"),	LVCFMT_CENTER, nColWidth[0] );

	CWnd* pWnd = GetDlgItem( IDC_EDIT_PREFAB_SCALE );
	if( pWnd )
		SetWindowText_gcvt( pWnd, 1.0f, 7 );

	m_bInitDlg = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CPrefabEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = PREFAB_VIEW;
	pView->DeActiveEditMatrix();

	ReSetListCtrl();

	CWnd* pWnd(NULL);
	pWnd = GetDlgItem( IDC_EDIT_POS_X );
	pWnd->SetWindowText( _T("0.f") );
	pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	pWnd->SetWindowText( _T("0.f") );
	pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	pWnd->SetWindowText( _T("0.f") );

	return CPropertyPage::OnSetActive();
}

BOOL CPrefabEdit::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();

	CWorldEditView::g_pPrefab = NULL;
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CPrefabEdit::ReSetListCtrl()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	m_listReplace.DeleteAllItems ();

	TSTRING strName;
	for ( size_t i=0; i<pPrefabMan->GetSize(); ++i )
	{
		if( pPrefabMan->IsGetName( i, strName ) )
		{
			m_listReplace.InsertItem( i, strName.c_str() );
		}
	}
}

// ���λ����� ������ ���� List �� ����
void CPrefabEdit::UpdatePage()
{
	ReSetListCtrl();
}

void CPrefabEdit::UpdateKeyBoardMouse()
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
	}


	if ( DxInputDevice::GetInstance().GetKeyState( DIK_DELETE ) & DXKEY_UP )
	{
		KeyBoard_KeyUp_Delete();
	}
	if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
	{
		UpdatePageClickUP();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD2 ) & DXKEY_UP )
	{
		(*m_rmatPiece)._43 -= 1.f;
		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD4 ) & DXKEY_UP )
	{
		(*m_rmatPiece)._41 -= 1.f;
		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD6 ) & DXKEY_UP )
	{
		(*m_rmatPiece)._41 += 1.f;
		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD8 ) & DXKEY_UP )
	{
		(*m_rmatPiece)._43 += 1.f;
		SetPositionEdit();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_T ) & DXKEY_UP )
	{
		OnBnClickedButtonRotateL15();
	}
	if ( DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
	{
		OnBnClickedButtonRotateR15();
	}
}

void CPrefabEdit::KeyBoard_Enter()
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

void CPrefabEdit::KeyBoard_KeyUp_Delete()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	if ( !m_rmatPiece )	
		return;

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

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		for( INT j=0; j<i; ++j )
		{
			if( nDeleteList[ i ] > nDeleteList[ j ] )
				--nCurIndex;
		}
		pPrefabMan->Delete( nCurIndex );
	}

	i = nDeleteList[ 0 ];
	SAFE_DELETE_ARRAY( nDeleteList );

	ReSetListCtrl();

	CWorldEditView::g_pPrefab = NULL;
	pView->DeActiveEditMatrix();

	// Note : ctrllist �� Ȱ��ȭ ��ġ ����
	nCount = m_listReplace.GetItemCount();
	if( nCount == 0 )	return;
	if( i >= nCount )	i = nCount - 1;

	// ������ ��ġ�� �̵�
	SetFocus( m_listReplace, i );
}

void CPrefabEdit::SetFocus( CListCtrl& listReplace, int nIndex )
{
	listReplace.SetSelectionMark( nIndex );								// ���� ���õ�.
	listReplace.SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED);	// ȸ������ ǥ�õ�.
	listReplace.EnsureVisible( nIndex, FALSE );							// ��ũ�� �ؿ� ������ �Ű���.
	listReplace.SetFocus();												// Ǫ�������� ǥ�õ�.
}

void CPrefabEdit::UpdatePageClickUP()
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();
	pPrefabMan->MakeTree();

	// �̵��� �ߴٸ� �� ��ġ�� �ٽ� �־��ش�.
	SetPositionEdit();
}

void CPrefabEdit::OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	int nItem = m_listReplace.GetNextSelectedItem( pos );
	CString strName = m_listReplace.GetItemText( nItem, 0 );

	m_rmatPiece = pPrefabMan->MoveCamera( (DWORD)nItem );
	if( m_rmatPiece )
	{
		pView->ActiveEditMatrix2( m_rmatPiece );
	}
	CWorldEditView::g_pPrefab = pPrefabMan->GetPrefab( (DWORD)nItem );

	pView->SetCurType( _CUR_SELECT );
}

void CPrefabEdit::OnBnClickedButtonRotateInit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	(*m_rmatPiece)._11 = 1.f;	(*m_rmatPiece)._12 = 0.f;	(*m_rmatPiece)._13 = 0.f;
	(*m_rmatPiece)._21 = 0.f;	(*m_rmatPiece)._22 = 1.f;	(*m_rmatPiece)._23 = 0.f;
	(*m_rmatPiece)._31 = 0.f;	(*m_rmatPiece)._32 = 0.f;	(*m_rmatPiece)._33 = 1.f;
}

void CPrefabEdit::OnBnClickedButtonRotateRandom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*sc::Random::RANDOM_POS() );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPrefabEdit::OnBnClickedButtonRotateL90()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*(90.f/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPrefabEdit::OnBnClickedButtonRotateL15()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*(15.f/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPrefabEdit::OnBnClickedButtonRotateR15()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*(-15.f/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPrefabEdit::OnBnClickedButtonRotateR90()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�..
	if( !m_rmatPiece )	return;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*2.f*(-90.f/360.f) );
	D3DXMatrixMultiply( m_rmatPiece, &matRotate, m_rmatPiece );
}

void CPrefabEdit::SelectPrefabList( DxPrefab* pPrefab )
{
	if( !m_bInitDlg )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	int nIndex = pPrefabMan->GetIndex( pPrefab );
	if( nIndex == -1 )	return;

	// Note : List Ctrl �缳��
	ReSetListCtrl();

	// Note : ����.~!
	m_rmatPiece = &CWorldEditView::g_pPrefab->m_matWorld;
	//SetPositionEdit();

	pView->ActiveEditMatrix2( m_rmatPiece );

	// ������ ��ġ�� �̵�
	SetFocus( m_listReplace, nIndex);
}

void CPrefabEdit::SetPositionEdit()
{
	if ( !m_rmatPiece )
		return ;

	CWnd* pWnd(NULL);
	CString strText;
	CString strTextPrev;

	pWnd = GetDlgItem( IDC_EDIT_POS_X );
	pWnd->GetWindowText( strTextPrev );
	strText.Format ( _T("%3.2f"), (*m_rmatPiece)._41 );
	if ( strTextPrev != strText )
	{
		pWnd->SetWindowText( strText );
	}

	pWnd = GetDlgItem( IDC_EDIT_POS_Y );
	pWnd->GetWindowText( strTextPrev );
	strText.Format ( _T("%3.2f"), (*m_rmatPiece)._42 );
	if ( strTextPrev != strText )
	{
		pWnd->SetWindowText( strText );
	}

	pWnd = GetDlgItem( IDC_EDIT_POS_Z );
	pWnd->GetWindowText( strTextPrev );
	strText.Format ( _T("%3.2f"), (*m_rmatPiece)._43 );
	if ( strTextPrev != strText )
	{
		pWnd->SetWindowText( strText );
	}
}

void CPrefabEdit::OnBnClickedButtonHeightPick()
{
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
}

void CPrefabEdit::OnBnClickedButtonHeightUp10()
{
	if ( !m_rmatPiece )
		return;

	(*m_rmatPiece)._42 += 10.f;
	SetPositionEdit();
}

void CPrefabEdit::OnBnClickedButtonHeightUp1()
{
	if ( !m_rmatPiece )
		return;

	(*m_rmatPiece)._42 += 1.f;
	SetPositionEdit();
}

void CPrefabEdit::OnBnClickedButtonHeightDown1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;
	(*m_rmatPiece)._42 -= 1.f;
	SetPositionEdit();
}

void CPrefabEdit::OnBnClickedButtonHeightDown10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	return;
	(*m_rmatPiece)._42 -= 10.f;
	SetPositionEdit();
}

void CPrefabEdit::OnBnClickedButtonHeightResetAll()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	//D3DXVECTOR3 vStart;
	//D3DXVECTOR3 vEnd;
	//D3DXVECTOR3 vColl;
	//D3DXVECTOR3 vMin;
	//D3DXVECTOR3 vMax;
	//NSHfMain::GetAABB( vMin, vMax );
	//for ( DWORD i=0; i < pPrefabMan->m_vecPrefab.size(); ++i )
	//{
	//	vEnd.x = vStart.x = pPrefabMan->m_vecPrefab[i]->m_matWorld._41;
	//	vEnd.z = vStart.z = pPrefabMan->m_vecPrefab[i]->m_matWorld._43;
	//	vStart.y = vMax.y + 0.0001f;
	//	vEnd.y = vMin.y - 0.0001f;

	//	NSHfMain::IsCollision( vStart, vEnd, FALSE, FALSE );
	//	vColl = vEnd;
	//	pPrefabMan->m_vecPrefab[i]->m_matWorld._42 = vColl.y;
	//}
}

void CPrefabEdit::OnBnClickedButtonFloatZero()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_rmatPiece )	
		return;

	int nIndex(0);

	nIndex = static_cast<int>( (*m_rmatPiece)._41 );
	(*m_rmatPiece)._41 = static_cast<float>( nIndex );

	nIndex = static_cast<int>( (*m_rmatPiece)._42 );
	(*m_rmatPiece)._42 = static_cast<float>( nIndex );

	nIndex = static_cast<int>( (*m_rmatPiece)._43 );
	(*m_rmatPiece)._43 = static_cast<float>( nIndex );

	SetPositionEdit();
}

void CPrefabEdit::OnBnClickedButtonScale()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	INT nCount = m_listReplace.GetSelectedCount();
	if( !nCount )
		return;

	POSITION pos = m_listReplace.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	CWnd* pWnd = GetDlgItem( IDC_EDIT_PREFAB_SCALE );
	if( !pWnd )
		return;

	CString strText;
	pWnd->GetWindowText( strText );
	FLOAT fScale = (FLOAT)_tstof( strText.GetString() );

	D3DXMATRIX matScale;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxPrefabMan* pPrefabMan = pView->GetSetLandMan()->GetPrefabMan();

	while( pos )
	{
		INT nIndex = m_listReplace.GetNextSelectedItem( pos );

		DxPrefab* pPrefab = pPrefabMan->GetPrefab( static_cast< DWORD >( nIndex ) );
		if( pPrefab )
		{
			//  [2/14/2013 gbgim]
			// - ������ ���� �ԷµǸ� ���� ��Ŀ� �����Ǵ� ���Ŀ���;
			//   ���� ����(������ ���� �ѿ�)�� �������� ��ġ�� ��ȯ�ǵ��� ����;
			//   D3DXMatrixDecompose-���� ��Ŀ� ����� ������, ȸ��, ��ġ ������ �������;
			D3DXVECTOR3 vtScale, vtPos;
			D3DXQUATERNION qRotation;
			D3DXMatrixDecompose( &vtScale, &qRotation, &vtPos, &pPrefab->m_matWorld );
			D3DXMatrixScaling( &matScale, fScale * (1.0f/vtScale.x), fScale * (1.0f/vtScale.y), fScale * (1.0f/vtScale.z) );
			D3DXMatrixMultiply( &pPrefab->m_matWorld, &matScale, &pPrefab->m_matWorld );			
		}
	}
}
