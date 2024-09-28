// PieceMorphEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/Common/SUBPATH.H"

#include "PieceMorphEdit.h"


// CPieceMorphEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPieceMorphEdit, CPropertyPage)
CPieceMorphEdit::CPieceMorphEdit()
	: CPropertyPage(CPieceMorphEdit::IDD)
{
}

CPieceMorphEdit::~CPieceMorphEdit()
{
}

void CPieceMorphEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFF, m_listXFile);
}


BEGIN_MESSAGE_MAP(CPieceMorphEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
END_MESSAGE_MAP()


// CPieceMorphEdit �޽��� ó�����Դϴ�.

BOOL CPieceMorphEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// Note : List �缳��.
	ReSetListEff();

	return CPropertyPage::OnSetActive();
}


void CPieceMorphEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActiveBackUpPage( PIECEMORPHINDEX );
}

void CPieceMorphEdit::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

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

//	Sleep( 100 );
//	DxInputDevice::GetInstance().InitKeyState();
}

void CPieceMorphEdit::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//
//	HRESULT hr = S_OK;
//	CWnd *pWnd;
//
//	//	Note : ���� ���͸� �����Ѵ�.
//	CString szFilter = "Frame Mesh xFile |*.X|";
//	
//	//	Note : ���� ���� ���̾˷α׸� ����.
//	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
//		(CDlgEdit*)this);
//
//	char szPath[MAX_PATH] = {0};
//	strcpy(szPath,pFrame->m_szAppPath);
//	strcat(szPath,SUBPATH::OBJ_FILE_PIECEEDIT);
//
//	dlg.m_ofn.lpstrInitialDir = szPath;
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString sFileName = dlg.GetFileName();
//		char *szFileName = sFileName.GetString(MAX_PATH);
//
//		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
//		DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
//
//		//	Note : ���� ������ ����.
//		pPieceEdit->CleanUp();
//
//		//	Note : ���ο� FrameMesh ����
//		pPieceEdit->NewFrameMesh( pView->GetD3DDevice(), szFileName );
//
//		//	Note : ������ Ʈ�� ���.
//		//
//		SetTreeFrame( pPieceEdit->GetFrameRoot() );
//		
//		CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
//		pWnd->SetWindowText( szFileName );
//
//		strcpy ( szFileName, "" );
//		pWnd = GetDlgItem ( IDC_EDIT_PEF_NAME );
//		pWnd->SetWindowText ( szFileName );
//
//		ResetButton ();		// Button �缳��
//	}
//	return;
//
////e_ERROR:
//	pWnd = GetDlgItem ( IDC_EDIT_XFILE );
//	pWnd->SetWindowText ( "" );
//
//	m_TreeFrame.DeleteAllItems();
//
//	MessageBox ( "xFile Load Failed!" );




	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//CWnd *pWnd = NULL;
	//char szEffName[MAX_PATH] = {0};
	//pWnd = GetDlgItem( IDC_EDIT_NAME );
	//pWnd->GetWindowText( szEffName, MAX_PATH );

	//// Note : ���� ����
	//if( szEffName[0] == NULL )
	//{
	//	MessageBox( "Need.. Effect File Set" );
	//	return;
	//}

	////// Note : ȿ�� ����
	////DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	////if( !pPieceEdit )	return;
	////pPieceEdit->SetEff( pView->GetD3DDevice(), g_strPICK_FRAME.c_str(), g_vCLOSED_POINT, szEffName );

	//// Note : List �缳��.
	//ReSetListEff();
}

void CPieceMorphEdit::ReSetListEff()
{
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//CString StrOldSelect;
	//int nIndex = m_listXFile.GetCurSel();
	//if ( nIndex==LB_ERR )
	//{
	//	m_listXFile.GetDlgItemText( nIndex, StrOldSelect );
	//}
	//m_listXFile.ResetContent();

	//DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	//if( !pPieceEdit )	return;
	//DxPieceEff* pPieceEff = pPieceEdit->GetPieceEffEDIT();
	//if( !pPieceEff )	return;
	//DxPieceEff::VECLANDEFF_ITER iter = pPieceEff->m_vecLandEff.begin();
	//for( ; iter!=pPieceEff->m_vecLandEff.end(); ++iter )
	//{
	//	int nIndex = m_listXFile.AddString( (*iter)->m_strEditName.c_str() );
	//	m_listXFile.SetItemData( nIndex, nIndex );
	//}

	//m_listXFile.SelectString( 0, StrOldSelect );
}

void CPieceMorphEdit::OnBnClickedButtonDelete()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//CString StrSelect;
	//int nIndex = m_listXFile.GetCurSel();
	//if ( nIndex==LB_ERR )	return;
	//m_listXFile.GetText( nIndex, StrSelect );

	//// Note : ȿ�� ����
	//DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	//if( !pPieceEdit )	return;
	//pPieceEdit->DelEff( StrSelect.GetString(MAX_PATH) );

	// Note : List �缳��.
	ReSetListEff();
}
