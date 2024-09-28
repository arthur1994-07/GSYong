// PieceSingleEffEdit.cpp : 구현 파일입니다.
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

// CPieceSingleEffEdit 대화 상자입니다.

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


// CPieceSingleEffEdit 메시지 처리기입니다.

BOOL CPieceSingleEffEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// Note : 애니메이션을 중지 시킨다. Edit를 위해서.
	g_bPLAY_ANIMATION = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CPieceSingleEffEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// Note : 애니메이션을 중지 시킨다. Edit를 위해서.
	g_bPLAY_ANIMATION = FALSE;

	// Note : List 재설정.
	ReSetListEff();

	return CPropertyPage::OnSetActive();
}


void CPieceSingleEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActiveBackUpPage( PIECESINGLEEFFINDEX );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	// Note : 화살표를 없앰
	pView->DeActiveEditMatrix();
}

void CPieceSingleEffEdit::OnLbnSelchangeListEff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CWnd *pWnd;
	char szEffName[MAX_PATH] = {0};
	pWnd = GetDlgItem( IDC_EDIT_NAME );
	pWnd->GetWindowText( szEffName, MAX_PATH );

	// Note : 파일 보기
	if( szEffName[0] == NULL )
	{
		MessageBox( "Need.. Effect File Set" );
		return;
	}

	// Note : 효과 삽입
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;
	std::string strEditName;
	if( pPieceEdit->SetEff( pView->GetD3DDevice(), g_strPICK_FRAME.c_str(), g_vCLOSED_POINT, szEffName, strEditName ) )
	{
		m_listSingleEff.SelectString( -1,  strEditName.c_str() );

		// Note : List 재설정.
		ReSetListEff();
	}
}

void CPieceSingleEffEdit::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )	return;
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : 효과 삭제
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;
	pPieceEdit->DelEff( StrSelect.GetString() );

	// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
	if( !m_listSingleEff.GetCount() )	{}			// 선택할 것이 없다.
	else if( m_listSingleEff.GetCount() == nIndex )	// 마지막 것을 지웠다.
	{
		m_listSingleEff.SetCurSel( nIndex-1 );
	}
	else
	{
		m_listSingleEff.SetCurSel( nIndex );
	}

	// Note : List 재설정.
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

	// Note : 점프후 화살표 생성
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
		// Note : 화살표를 없앰.
		pView->DeActiveEditMatrix();
		return;
	}
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : 
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		// Note : 화살표를 없앰.
		pView->DeActiveEditMatrix();
		return;
	}

	LPD3DXMATRIX matWorld = pPieceEdit->GetEffMatrix( StrSelect.GetString() );
	if( matWorld )
	{
		D3DXVECTOR3 vPos( matWorld->_41, matWorld->_42, matWorld->_43 );
		DxViewPort::GetInstance().CameraJump ( vPos );
		
		// Note : 화살표를 나오게 함.
		pView->ActiveEditMatrix2( matWorld );
	}
}

void CPieceSingleEffEdit::OnBnClickedCheckPickPos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString StrSelect;
	int nIndex = m_listSingleEff.GetCurSel();
	if ( nIndex==LB_ERR )	return;
	m_listSingleEff.GetText( nIndex, StrSelect );

	// Note : 효과 삭제
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )	return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_PICK_POS );
	pPieceEdit->SetCheckCollPoint( StrSelect.GetString(), pButton->GetCheck() );
}






