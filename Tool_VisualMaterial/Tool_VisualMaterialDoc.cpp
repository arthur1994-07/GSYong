// Project3Doc.cpp : implementation of the CTool_VisualMaterialDoc class
//

#include "stdafx.h"

#include "../VisualMaterialLib/Preview/NSVM_PREVIEW.h"
#include "../VisualMaterialLib/Manager/DxVMManager.h"
#include "../VisualMaterialLib/DxVisualMaterial.h"

#include "./MainFrm.h"
#include "Tool_VisualMaterial.h"
#include "Tool_VisualMaterialView.h"

#include "Tool_VisualMaterialDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTool_VisualMaterialDoc

IMPLEMENT_DYNCREATE(CTool_VisualMaterialDoc, CDocument)

BEGIN_MESSAGE_MAP(CTool_VisualMaterialDoc, CDocument)
    ON_COMMAND(ID_FILE_NEW, &CTool_VisualMaterialDoc::OnFileNew)
    ON_COMMAND(ID_FILE_SAVE, &CTool_VisualMaterialDoc::OnFileSave)
    ON_COMMAND(ID_FILE_OPEN, &CTool_VisualMaterialDoc::OnFileOpen)
END_MESSAGE_MAP()


// CTool_VisualMaterialDoc construction/destruction

CTool_VisualMaterialDoc::CTool_VisualMaterialDoc()
{
	// TODO: add one-time construction code here

}

CTool_VisualMaterialDoc::~CTool_VisualMaterialDoc()
{
}

BOOL CTool_VisualMaterialDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CTool_VisualMaterialDoc serialization

void CTool_VisualMaterialDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CTool_VisualMaterialDoc diagnostics

#ifdef _DEBUG
void CTool_VisualMaterialDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTool_VisualMaterialDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTool_VisualMaterialDoc commands

void CTool_VisualMaterialDoc::OnFileNew()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();

    if( IDYES == AfxMessageBox( _T("기존데이터는 삭제됩니다. 진행하시겠습니까?"), MB_YESNO ) )
	{
        NSVM_PREVIEW::Reset();
        pView->m_pVisualMaterial->CleanUp();
        pView->m_pVisualMaterial->Create( pView->GetD3DDevice() );
        pFrame->SelectCommand( NULL );
        SetTitle( _T("New") );
	}
}

void CTool_VisualMaterialDoc::OnFileSave()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
//	pView->SetCurClick( FALSE );	// 커서 클릭을 죽인다.

    // 폴더가 없을 수도 있으므로 생성을 미리 한다.
    CreateDirectory( NS_VM_MANAGER::g_strFullPath.c_str(), NULL );

	//	Note : 파일 필터를 정의한다.
	CString szFilter = _T("Visual Material ( *.vm ) |*.vm");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg( FALSE,_T(".vm"), GetTitle().GetString(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );
	
    dlg.m_ofn.lpstrInitialDir = NS_VM_MANAGER::g_strFullPath.c_str();
	if ( dlg.DoModal() == IDOK )
	{
        pView->m_pVisualMaterial->Save( dlg.GetPathName().GetString(), dlg.GetFileName().GetString() );
        SetTitle( dlg.GetFileName().GetString() );
	}

//	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CTool_VisualMaterialDoc::OnFileOpen()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
//	pView->SetCurClick( FALSE );	// 커서 클릭을 죽인다.
			
	//	Note : 파일 필터를 정의한다.
	CString szFilter = _T("Visual Material ( *.vm ) |*.vm");

	//	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog dlg( TRUE,_T(".vm"), GetTitle().GetString(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );
	
    dlg.m_ofn.lpstrInitialDir = NS_VM_MANAGER::g_strFullPath.c_str();
	if ( dlg.DoModal() == IDOK )
	{
        pView->m_pVisualMaterial->Load( pView->GetD3DDevice(), dlg.GetFileName().GetString() );
        pFrame->SelectCommand( NULL );
        SetTitle( dlg.GetFileName().GetString() );
	}

//	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}
