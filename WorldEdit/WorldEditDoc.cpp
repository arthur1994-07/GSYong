// WorldEditDoc.cpp : implementation of the CWorldEditDoc class
//

#include "stdafx.h"

#include "../MfcExLib/ExLibs/FECFileDialog.h"

#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxLand/ExportXFile/NSExportXFile.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../EngineLib/DxEffect/Mesh/DxStaticMeshLightMap.h"

#include "../EngineLib/Common/SUBPATH.H"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MainFrm.h"
#include "ExportDlg.h"
#include "./ProgressBarDlg.h"
#include "LoadSetLogDlg.h"
#include "CheckIntegrityNaviMeshDlg.h"
#include "CollisionMapDlg.h"
#include ".\worldeditdoc.h"
#include "State/WorldStateMachine.h"
#include "SheetWithTab.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc

IMPLEMENT_DYNCREATE(CWorldEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CWorldEditDoc, CDocument)
	//{{AFX_MSG_MAP(CWorldEditDoc)
	ON_COMMAND(ID_MENUITEM_EXPORT, OnMenuitemExport)
	ON_COMMAND(ID_MENUITEM_EXPORTLOAD, OnMenuitemExportload)
	ON_COMMAND(ID_MENUITEM_EXPORTSAVE, OnMenuitemExportsave)
	ON_COMMAND(ID_MENUITEM_SETTING_SAVE, OnMenuitemSettingSave)
	ON_COMMAND(ID_MENUITEM_SETTING_LOAD, OnMenuitemSettingLoad)
	ON_COMMAND(ID_MENUITEM_PREFAB_SAVE, OnMenuitemPrefabSave)
	ON_COMMAND(ID_MENUITEM_PREFAB_LOAD, OnMenuitemPrefabLoad)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_CHECKINTEGRITY, OnMenuitemCheckintegrity)
	ON_COMMAND(ID_MENUITEM_EXPORT_PIECE, OnMenuitemExportPiece)
	ON_COMMAND(ID_BUTTON_COLLISIONMAP, OnButtonCollisionmap)
	ON_COMMAND(ID_FILE_PIECE_LOAD, OnFilePieceLoad)
	ON_COMMAND(ID_MENUITEM_VRML_LOAD, OnMenuitemVrmlLoad)
	ON_COMMAND(ID_MENUITEM_VRML_SAVE, OnMenuitemVrmlSave)
	ON_COMMAND(ID_EXPORT2XFILE, &CWorldEditDoc::OnExport2xfile)
	ON_COMMAND(ID_EXPORT2NAVIGATIONMESH, OnExport2navigationmesh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc construction/destruction

CWorldEditDoc::CWorldEditDoc()
{
	// TODO: add one-time construction code here
	m_pCheckIntegrityDlg = NULL;
}

CWorldEditDoc::~CWorldEditDoc()
{
}

BOOL CWorldEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	TSTRING strName(_T(""));
	SetTitleAddVersion( strName.c_str() );

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc serialization

void CWorldEditDoc::Serialize(CArchive& ar)
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

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc diagnostics

#ifdef _DEBUG
void CWorldEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWorldEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc commands

void CWorldEditDoc::OnMenuitemExportsave() 
{
	// TODO: Add your command handler code here
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "World ( *.wld ) |*.wld|";
	CFileDialog dlg ( FALSE, ".wld", "world00",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );
	
	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetPathName();

		DxLandMan* pDxLandMan = pView->GetLandMan();
        pDxLandMan->SaveFile ( pView->GetD3DDevice(), WriteName.GetString() );
	}	
}

void CWorldEditDoc::OnMenuitemExportload() 
{
	// TODO: Add your command handler code here
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "World ( *.wld ) |*.wld|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".wld",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

	// [shhan][2015.06.15] ������ �޸𸮸� �����Ѵ�.
	//						�ε� ���� �۾��� �Ͽ��� �������� ū �޸𸮸� �ε��ϴ� ���� �ƴ��� Ȯ���ϴ� �۾��� �Ѵ�.
	HANDLE hProcess	= GetCurrentProcess();
	if (hProcess)
	{
		EmptyWorkingSet( hProcess );
	}

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		LoadExportFile( sFileName.GetString() );
	}

	//CFECFileDialog dlg ( TRUE, _T(".wld"), NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, (CWorldEditView*)pView );

	//CString sFileName;
	//dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();

	//if ( dlg.DoModal() == IDOK )
	//{
	//	CString strPathName;
	//	POSITION pos = dlg.GetStartPosition();
	//	while (pos != NULL)
	//	{
	//		strPathName = dlg.GetNextPathName(pos);
	//		CString strFileName = strPathName;
	//		int nPosition = strFileName.ReverseFind('\\');
	//		strFileName = strFileName.Mid(nPosition+1);

	//		LoadExportFile( strFileName.GetString() );
	//		sFileName.ReleaseBuffer ();
	//	}
	//}
}

void CWorldEditDoc::LoadExportFile(const TCHAR* pFileName)
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

    WorldStateMachine::ClearFileNotifier(FRAMEPAGEINDEX);

	//	Note : ���� �ε�ȰͰ� ������ ������Ʈ �ʱ�ȭ.
	//
	pView->ResetObjects ();

	DxLandMan* pDxLandMan = pView->GetLandMan();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	//	Note : ���� ������ ����.
	pDxLandMan->DeleteDeviceObjects();
	pDxLandMan->LoadFile ( pFileName, pd3dDevice );

	pView->TempCreateChar ();

	CWorldEditView::m_ViewType = OCTREE_VIEW;

	if ( CWorldEditView::g_bOrthoView )
	{
		DxViewPort::GetInstance().SetToolCameraMiniMap();
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.UpdateCamColl();
	SetTitleAddVersion( pFileName );
    
    WorldStateMachine::InsertFileNotifier(FRAMEPAGEINDEX);
    WorldStateMachine::Asset(FRAMEPAGEINDEX);
}

void CWorldEditDoc::OnMenuitemExport() 
{
	// TODO: Add your command handler code here
	CExportDlg	dlg;
	dlg.DoModal();
}

void CWorldEditDoc::OnFilePieceLoad()
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Piece File ( *.pis ) |*.pis|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".pis", NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = DxReplaceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString ReadName = dlg.GetFileName();

		DXREPLACEPIECE* pPiece = pView->GetReplacePiece();
		if ( FAILED ( pPiece->Load ( ReadName.GetString(), pView->GetD3DDevice() ) ) )
		{
			pView->MessageBox ( "���� ������ �б⿡ �����Ͽ����ϴ�." , ReadName.GetString() );
		}
	}
}

void CWorldEditDoc::OnMenuitemExportPiece()
{
	//POSITION pos = GetFirstViewPosition();
	//CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	////	Note : ���� ���͸� �����Ѵ�.
	//CString szFilter = "Piece File ( *.pis ) |*.pis|";

	////	Note : ���� ���� ���̾˷α׸� ����.
	//CFileDialog dlg(TRUE,".pis", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
	//	(CWorldEditView*)pView );

	//dlg.m_ofn.lpstrInitialDir = DxReplaceContainer::GetInstance().GetPath ();
	//if ( dlg.DoModal() == IDOK )
	//{
	//	CString WriteName = dlg.GetPathName();

	//	DxLandMan* pDxLandMan = pView->GetLandMan();
	//	pDxLandMan->SavePieceFile ( WriteName.GetString() );
	//}
}

void CWorldEditDoc::OnMenuitemSettingSave()
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Property Setting Files( *.psf ) |*.psf|";
	CFileDialog dlg ( FALSE, ".psf", "untitled",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );
	
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH]  = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileRoot().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	DxSetLandMan *pSetLandMan = pView->GetSetLandMan ();
	if ( pSetLandMan->IsPrefab() )
	{
		AfxMessageBox( _T("Prefab�� psf ���Ϸ� ������ �� �����ϴ�.") );
		return;
	}

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetPathName();

		pSetLandMan->SaveSet ( pView->GetD3DDevice (), sFileName.GetString() );

        SetTitleAddVersion( dlg.GetFileName().GetString() );
	}
}

void CWorldEditDoc::OnMenuitemSettingLoad()
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Property Setting Files( *.psf ) |*.psf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,"psf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH]  = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileRoot().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		LoadSettingFile( sFileName.GetString() );
	}
}

void CWorldEditDoc::LoadSettingFile(const TCHAR* pFileName)
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

    WorldStateMachine::ClearFileNotifier(FRAMEPAGEINDEX);

	//	Note : ���� �ε�ȰͰ� ������ ������Ʈ �ʱ�ȭ.
	pView->SetLandMan();
	pView->ResetObjects ();

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();		

	DxSetLandMan *pSetLandMan = pView->GetSetLandMan ();
	pSetLandMan->LoadSet ( pd3dDevice, pFileName, pPath );
	pSetLandMan->ActiveMap ();

	//	Note	:	�ε� ����
	//
	CLoadSetLogDlg	Logdlg;
	Logdlg.DoModal();
	
	pFrame->m_cDlgEdit.UpdateContrl ();
	SetTitleAddVersion( pFileName );

	WorldStateMachine::InsertFileNotifier(FRAMEPAGEINDEX);
    WorldStateMachine::Asset(FRAMEPAGEINDEX);
}

void CWorldEditDoc::OnMenuitemPrefabSave()
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	DxSetLandMan *pSetLandMan = pView->GetSetLandMan ();
	if ( !pSetLandMan->IsPrefab() )
	{
		AfxMessageBox( _T("Psf�� Prefab ���Ϸ� ������ �� �����ϴ�.") );
		return;
	}

	const TSTRING& strFileName = pSetLandMan->GetFileName();

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = _T("Prefab Files( *.prefab ) |*.prefab|");
	CFileDialog dlg ( FALSE, _T(".prefab"), strFileName.c_str(),
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH]  = {0};
	StringCchCopy(szPath, MAX_PATH, pFrame->m_szAppPath);
	StringCchCat(szPath, MAX_PATH, pPath->ObjFilePrefab().c_str());

	// ������ ���� ���� �����Ƿ� ������ �̸� �Ѵ�.
	CreateDirectory( szPath, NULL );

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		pSetLandMan->SaveSetPrefab( dlg.GetPathName().GetString(), pView->GetD3DDevice() );

		// psf �� ���õǾ� �ִ� prefab ���� Refresh �Ѵ�.
		pView->RefreshPrefab( pView->GetD3DDevice() );

		//pFrame->SetWindowText( dlg.GetPathName().GetString() );
	}
}

void CWorldEditDoc::OnMenuitemPrefabLoad()
{
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = _T("Prefab Files( *.prefab ) |*.prefab|");

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE, _T("prefab"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH]  = {0};
	StringCchCopy(szPath, MAX_PATH, pFrame->m_szAppPath);
	StringCchCat(szPath, MAX_PATH, pPath->ObjFilePrefab().c_str());

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		//	Note : ���� �ε�ȰͰ� ������ ������Ʈ �ʱ�ȭ.
		pView->SetLandManPrefab();

		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();		

		DxSetLandMan *pSetLandMan = pView->GetSetLandMan ();
		pSetLandMan->LoadSetPrefab( dlg.GetPathName().GetString(), pd3dDevice );
		pSetLandMan->ActiveMap ();

		//	Note	:	�ε� ����
		//
		CLoadSetLogDlg	Logdlg;
		Logdlg.DoModal();

		//pFrame->SetWindowText( dlg.GetPathName().GetString() );
		//pFrame->m_cDlgEdit.UpdateContrl ( FALSE );
	}
}

void CWorldEditDoc::OnMenuitemCheckintegrity()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.	
	if ( m_pCheckIntegrityDlg )
	{
		m_pCheckIntegrityDlg->SetFocus ();
	}
	else
	{
		m_pCheckIntegrityDlg = new CCheckIntegrityNaviMeshDlg;
		m_pCheckIntegrityDlg->Create ( IDD_DIALOG_CHECKINTEGRITY );
	}
}

void CWorldEditDoc::OnButtonCollisionmap()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CCollisionMapDlg	dlg;
	dlg.DoModal ();
}

void CWorldEditDoc::OnMenuitemVrmlLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "WRML( *.WRL ) |*.WRL|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,"WRL",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH]  = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileRoot().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		//CString sFileName = dlg.GetFileName();
		//char *szFileName = sFileName.GetString();
		
		//	Note : ���� �ε�ȰͰ� ������ ������Ʈ �ʱ�ȭ.
		//
		pView->ResetObjects ();

		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();		

		//DxSetLandMan *pSetLandMan = pView->GetSetLandMan ();
		//pSetLandMan->LoadSet ( szFileName, pd3dDevice );
		//pSetLandMan->ActiveMap ();

		////	Note	:	�ε� ����
		////
		//CLoadSetLogDlg	Logdlg;
		//Logdlg.DoModal();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->m_cDlgEdit.UpdateContrl ();
	}
}

void CWorldEditDoc::OnMenuitemVrmlSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CWorldEditDoc::OnExport2xfile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "x ( *.x ) |*.x|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".x", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

	dlg.m_ofn.lpstrInitialDir = DxReplaceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetPathName();

		if ( pView->GetLandMan() )
		{
			// �̸� �� ���� �ε��� ���´�.
			if ( pView->GetLandMan()->GetStaticMesh() )
			{
				D3DXVECTOR3 vMax( 50000.f, 50000.f, 50000.f );
				D3DXVECTOR3 vMin( -50000.f, -50000.f, -50000.f );
				pView->GetLandMan()->GetStaticMesh()->BaseLoad( pView->GetD3DDevice(), vMax, vMin );
			}
			
			CProgressBarDlg sDlg;
			sDlg.Create( WriteName.GetString() );
			sDlg.DoModal();
		}
	}
}

void CWorldEditDoc::OnExport2navigationmesh()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = GetFirstViewPosition();
	CWorldEditView* pView = (CWorldEditView*) GetNextView( pos );

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "navi ( *.navi ) |*.navi|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".navi", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CWorldEditView*)pView );

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName();

		//	Note : ����� ���� ��� ��������
		CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
		const SUBPATH* pPath = pApp->GetSubPath();

		TCHAR szFullPath[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szFullPath, CSIDL_PERSONAL, FALSE );
		StringCchCat( szFullPath, MAX_PATH, pPath->DebugInfoRoot().c_str() ); //SUBPATH::DEBUGINFO_ROOT );

		DxSetLandMan* pDxSetLandMan = pView->GetSetLandMan();
		pDxSetLandMan->SaveSetNavi( pView->GetD3DDevice(), WriteName.GetString(), szFullPath );
	}
}

// Version �� �߰��Ѵ�.
void CWorldEditDoc::SetTitleAddVersion( LPCTSTR lpszTitle )
{
	// ���� �̸��� ������ ���´�.
	// Reload ���� �ʿ���.
	m_strTitleOrig = lpszTitle;

	TSTRING strTitle;
	strTitle = lpszTitle;
	strTitle += _T(" - [ver.");
	strTitle += sc::getFileVersion();
	strTitle += _T("]");
	SetTitle( strTitle.c_str() );
}