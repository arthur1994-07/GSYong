// SkinObjPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"

#include "SkinObjPage.h"


// CSkinObjPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSkinObjPage, CPropertyPage)
CSkinObjPage::CSkinObjPage()
	: CPropertyPage(CSkinObjPage::IDD),
	m_pSheetTabAnim(NULL)
{
}

CSkinObjPage::~CSkinObjPage()
{
}

void CSkinObjPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSkinObjPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SKINFILE, OnBnClickedButtonSkinfile)
	ON_BN_CLICKED(IDC_BUTTON_BONEFILE, OnBnClickedButtonBonefile)
	ON_BN_CLICKED(IDC_BUTTON_MAKEOBJ, OnBnClickedButtonMakeobj)
END_MESSAGE_MAP()


// CSkinObjPage �޽��� ó�����Դϴ�.
BOOL CSkinObjPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	//	Note : CsheetWithTabAnim �� �����.
	//
	CWnd *pWnd;
	CRect SheetRect;

	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP2 );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );
	
	m_pSheetTabAnim = new CsheetWithTabAnim ( SheetRect, this );
	m_pSheetTabAnim->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );


	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->SetWindowText ( "untitle.sob" );

	pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
	pWnd->SetWindowText ( "char_bone.X" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSkinObjPage::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	SAFE_DELETE(m_pSheetTabAnim);

	CPropertyPage::PostNcDestroy();
}

void CSkinObjPage::OnBnClickedButtonNew()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject();

	pSkinObject->ClearAll ();

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
	pWnd->SetWindowText ( "" );

	pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
	pWnd->SetWindowText ( "char_bone.X" );


	m_pSheetTabAnim->ReSetPages ();
}

BOOL CSkinObjPage::LoadSkinObject( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	//	Note : ��Ų������Ʈ �ε�.
	//
	BOOL bResult = pSkinObject->LoadObject( pszFileName, pd3dDevice, FALSE );
	if ( bResult )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
		pWnd->SetWindowText ( pSkinObject->GetSkinMeshName() );

		pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
		pWnd->SetWindowText ( pSkinObject->GetSkeletonName() );

		m_pSheetTabAnim->SetPages ( NULL, pView->GetSkinObject() );
	}
	else
	{
		MessageBox ( "Error reading skin object file." );
		return FALSE;
	}
	return TRUE;
}

void CSkinObjPage::OnBnClickedButtonOpen()
{
	

	CString szFilter = "Skin Obj File (*.sob)|*.sob|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".sob",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		LoadSkinObject( sFileName.GetString() );

		
	}
}

void CSkinObjPage::OnBnClickedButtonSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString StrFileName;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->GetWindowText ( StrFileName );

	CString szFilter = "Skin Obj File (*.sob)|*.sob|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".sob",StrFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		//	Note : ��Ų������Ʈ ���̺�.
		//
		pSkinObject->SaveObject ( sFileName.GetString() );
	}
}

void CSkinObjPage::OnBnClickedButtonSkinfile()
{
	CString szFilter = "Skin xFile (*.x,*.mxf)|*.X;*.mxf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����. Create a file open dialog
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinMeshManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
		pWnd->SetWindowText ( sFileName.GetString() );
	}
}

void CSkinObjPage::OnBnClickedButtonBonefile()
{
	CString szFilter = "Bone xFile (*.x,*.mxf)|*.X;*.mxf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����. Create a file open dialog.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkeletonManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
		pWnd->SetWindowText ( sFileName.GetString() );
	}
}

void CSkinObjPage::OnBnClickedButtonMakeobj()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject();

	CString sSkinMesh, sSkinBone;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
	pWnd->GetWindowText ( sSkinMesh );
	pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
	pWnd->GetWindowText ( sSkinBone );

	if ( sSkinBone.IsEmpty() || sSkinMesh.IsEmpty () )
	{
		MessageBox ( "Skin, no bone file specified." );
		return;
	}

	if ( !pView->GetD3dDevice() )	return;
	if ( !pSkinObject->CreateSkinMesh( sSkinMesh.GetString(), sSkinBone.GetString(), pView->GetD3dDevice(), FALSE ) )
	{
		MessageBox ( "Failed to create skin/bone." );
	}
}
