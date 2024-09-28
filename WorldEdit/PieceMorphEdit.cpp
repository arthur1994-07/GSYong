// PieceMorphEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/Common/SUBPATH.H"

#include "PieceMorphEdit.h"


// CPieceMorphEdit 대화 상자입니다.

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


// CPieceMorphEdit 메시지 처리기입니다.

BOOL CPieceMorphEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// Note : List 재설정.
	ReSetListEff();

	return CPropertyPage::OnSetActive();
}


void CPieceMorphEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActiveBackUpPage( PIECEMORPHINDEX );
}

void CPieceMorphEdit::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//
//	HRESULT hr = S_OK;
//	CWnd *pWnd;
//
//	//	Note : 파일 필터를 정의한다.
//	CString szFilter = "Frame Mesh xFile |*.X|";
//	
//	//	Note : 파일 오픈 다이알로그를 만듬.
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
//		//	Note : 종전 데이터 삭제.
//		pPieceEdit->CleanUp();
//
//		//	Note : 새로운 FrameMesh 생성
//		pPieceEdit->NewFrameMesh( pView->GetD3DDevice(), szFileName );
//
//		//	Note : 프레임 트리 등록.
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
//		ResetButton ();		// Button 재설정
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

	//// Note : 파일 보기
	//if( szEffName[0] == NULL )
	//{
	//	MessageBox( "Need.. Effect File Set" );
	//	return;
	//}

	////// Note : 효과 삽입
	////DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	////if( !pPieceEdit )	return;
	////pPieceEdit->SetEff( pView->GetD3DDevice(), g_strPICK_FRAME.c_str(), g_vCLOSED_POINT, szEffName );

	//// Note : List 재설정.
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
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//CString StrSelect;
	//int nIndex = m_listXFile.GetCurSel();
	//if ( nIndex==LB_ERR )	return;
	//m_listXFile.GetText( nIndex, StrSelect );

	//// Note : 효과 삭제
	//DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	//if( !pPieceEdit )	return;
	//pPieceEdit->DelEff( StrSelect.GetString(MAX_PATH) );

	// Note : List 재설정.
	ReSetListEff();
}
