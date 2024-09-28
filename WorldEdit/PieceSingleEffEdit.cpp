// PieceSingleEffEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/Common/SUBPATH.H"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceEff.h"

#include "PieceSingleEffEdit.h"
#include ".\piecesingleeffedit.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern D3DXVECTOR3			g_vCLOSED_POINT;
extern std::string			g_strPICK_FRAME;
extern BOOL					g_bPLAY_ANIMATION;

// CPieceSingleEffEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPieceSingleEffEdit, CPropertyPage)
CPieceSingleEffEdit::CPieceSingleEffEdit()
	: CPropertyPage(CPieceSingleEffEdit::IDD)
{
}

CPieceSingleEffEdit::~CPieceSingleEffEdit()
{
}

void CPieceSingleEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFF, m_listSingleEff);
}


BEGIN_MESSAGE_MAP(CPieceSingleEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_EFF, OnLbnSelchangeListEff)
	ON_BN_CLICKED(IDC_CHECK_PICK_POS, OnBnClickedCheckPickPos)
	ON_BN_CLICKED(IDC_BUTTON_CUR_FIND, OnBnClickedButtonCurFind)
END_MESSAGE_MAP()


// CPieceSingleEffEdit �޽��� ó�����Դϴ�.

BOOL CPieceSingleEffEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	// Note : �ִϸ��̼��� ���� ��Ų��. Edit�� ���ؼ�.
	g_bPLAY_ANIMATION = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CPieceSingleEffEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// Note : �ִϸ��̼��� ���� ��Ų��. Edit�� ���ؼ�.
	g_bPLAY_ANIMATION = FALSE;

	// Note : List �缳��.
	ReSetListEff();

	return CPropertyPage::OnSetActive();
}


void CPieceSingleEffEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActiveBackUpPage( PIECESINGLEEFFINDEX );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	// Note : ȭ��ǥ�� ����
	pView->DeActiveEditMatrix();
}

void CPieceSingleEffEdit::OnLbnSelchangeListEff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CameraJumpArrowEnable();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )	return;
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : 
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_PICK_POS );
	if( pPieceEdit->GetCheckCollPoint( StrSelect.GetString() ) )	pButton->SetCheck( TRUE );
	else															pButton->SetCheck( FALSE );

	const char* pName = pPieceEdit->GetEffName( StrSelect.GetString() );
	if( pName )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_CUR_NAME );
		pWnd->SetWindowText( pName );
	}
}

void CPieceSingleEffEdit::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Effect File (*.egp)|*.egp";
	CFileDialog dlg ( TRUE, ".egp", NULL, OFN_HIDEREADONLY, szFilter, this );
    
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->EffFileSingle().c_str()); //SUBPATH::EFF_FILE_SINGLE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );
}

void CPieceSingleEffEdit::OnBnClickedButtonCurFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Effect File (*.egp)|*.egp";
	CFileDialog dlg ( TRUE, ".egp", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->EffFileSingle().c_str()); //SUBPATH::EFF_FILE_SINGLE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_CUR_NAME );
		pWnd->SetWindowText ( FilePath );

		CString StrSelect;
		int nIndex = m_listSingleEff.GetCurSel();
		if ( nIndex==LB_ERR )	return;
		m_listSingleEff.GetText( nIndex, StrSelect );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
		if( !pPieceEdit )	return;

		pPieceEdit->ModifyEff( pView->GetD3DDevice(), StrSelect.GetString(), FilePath.GetString() );

		ReSetListEff();
	} // if ( dlg.DoModal() == IDOK )

	pView->SetCurClick( TRUE );
}

void CPieceSingleEffEdit::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CWnd *pWnd;
	char szEffName[MAX_PATH] = {0};
	pWnd = GetDlgItem( IDC_EDIT_NAME );
	pWnd->GetWindowText( szEffName, MAX_PATH );

	// Note : ���� ����
	if( szEffName[0] == NULL )
	{
		MessageBox( "Need.. Effect File Set" );
		return;
	}

	// Note : ȿ�� ����
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;
	std::string strEditName;
	if( pPieceEdit->SetEff( pView->GetD3DDevice(), g_strPICK_FRAME.c_str(), g_vCLOSED_POINT, szEffName, strEditName ) )
	{
		m_listSingleEff.SelectString( -1,  strEditName.c_str() );

		// Note : List �缳��.
		ReSetListEff();
	}
}

void CPieceSingleEffEdit::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )	return;
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : ȿ�� ����
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;
	pPieceEdit->DelEff( StrSelect.GetString() );

	// Note : ������ ������ ������ �ٽ� ���ư���.
	if( !m_listSingleEff.GetCount() )	{}			// ������ ���� ����.
	else if( m_listSingleEff.GetCount() == nIndex )	// ������ ���� ������.
	{
		m_listSingleEff.SetCurSel( nIndex-1 );
	}
	else
	{
		m_listSingleEff.SetCurSel( nIndex );
	}

	// Note : List �缳��.
	ReSetListEff();
}

void CPieceSingleEffEdit::ReSetListEff()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrOldSelect;
	int nPrevIndex = m_listSingleEff.GetCurSel();

	m_listSingleEff.ResetContent();

	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;
	DxPieceEff* pPieceEff = pPieceEdit->GetPieceEffEDIT();
	if( !pPieceEff )	return;
	DxPieceEff::VECLANDEFF_ITER iter = pPieceEff->m_vecLandEff.begin();
	for( ; iter!=pPieceEff->m_vecLandEff.end(); ++iter )
	{
		int nIndex = m_listSingleEff.AddString( (*iter)->m_strEditName.c_str() );
		m_listSingleEff.SetItemData( nIndex, (DWORD)(*iter) );
	}

	if ( nPrevIndex==LB_ERR )	return;
	m_listSingleEff.SetCurSel( nPrevIndex );

	// Note : ������ ȭ��ǥ ����
	CameraJumpArrowEnable();
}

void CPieceSingleEffEdit::CameraJumpArrowEnable()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )
	{
		// Note : ȭ��ǥ�� ����.
		pView->DeActiveEditMatrix();
		return;
	}
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : 
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		// Note : ȭ��ǥ�� ����.
		pView->DeActiveEditMatrix();
		return;
	}

	LPD3DXMATRIX matWorld = pPieceEdit->GetEffMatrix( StrSelect.GetString() );
	if( matWorld )
	{
		D3DXVECTOR3 vPos( matWorld->_41, matWorld->_42, matWorld->_43 );
		DxViewPort::GetInstance().CameraJump ( vPos );
		
		// Note : ȭ��ǥ�� ������ ��.
		pView->ActiveEditMatrix2( matWorld );
	}
}

void CPieceSingleEffEdit::OnBnClickedCheckPickPos()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )	return;
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : ȿ�� ����
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_PICK_POS );
	pPieceEdit->SetCheckCollPoint( StrSelect.GetString(), pButton->GetCheck() );
}






