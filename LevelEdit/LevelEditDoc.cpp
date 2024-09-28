// LevelEditDoc.cpp : CLevelEditDoc Ŭ������ ����
//
#include "stdafx.h"

#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./LevelEdit.h"
#include "./LevelMainFrm.h"
#include "./LevelEditDoc.h"
#include "./LevelEditView.h"
#include "./Edit/LevelSheetWithTab.h"
#include "./Edit/MobSetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelEditDoc

IMPLEMENT_DYNCREATE(CLevelEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CLevelEditDoc, CDocument)
	ON_COMMAND(ID_IMPORT_LAYER, OnFileImport)
	ON_COMMAND(ID_IMPORT_LAYER_EXCEPT_EFFECT, OnFileImportExceptEffect)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_CSVSAVE, OnFileCsvsave)
	ON_COMMAND(ID_FILE_ALL_CSVSAVE, OnFileAllCsvsave)
	ON_COMMAND(ID_FILE_ALL_CSVLOAD, OnFileAllCsvload)
	ON_COMMAND(ID_FILE_CSVLOAD, OnFileCsvload)

	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)

END_MESSAGE_MAP()


// CLevelEditDoc ����/�Ҹ�

CLevelEditDoc::CLevelEditDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
}

CLevelEditDoc::~CLevelEditDoc()
{
}

BOOL CLevelEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CLevelEditDoc serialization

void CLevelEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CLevelEditDoc ����

#ifdef _DEBUG
void CLevelEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLevelEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CLevelEditDoc::OnFileImport()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	GLGaeaServer* pGLGaeaServer = pFrame->GetGLGaeaServer();	

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Effect Group ( *.Lev ) |*.Lev";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg( TRUE,".Lev", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString fileName = dlg.GetFileName ();
		const char *szWriteName = fileName.GetString();

		//	editing ���� ���� ���� ����.
		CString WriteName(fileName + ".bak");
		pView->SaveGLLandMan ( WriteName.GetString() );

		pView->ImportGLLandMan(szWriteName);
	}
}

void CLevelEditDoc::OnFileImportExceptEffect()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	GLGaeaServer* pGLGaeaServer = pFrame->GetGLGaeaServer();	

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Effect Group ( *.Lev ) |*.Lev";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg( TRUE,".Lev", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString fileName = dlg.GetFileName ();
		const char *szWriteName = fileName.GetString();

		//	editing ���� ���� ���� ����.
		CString WriteName(fileName + ".bak");
		pView->SaveGLLandMan ( WriteName.GetString() );

		pView->ImportGLLandMan(szWriteName, TRUE);
	}
}


// CLevelEditDoc ���
void CLevelEditDoc::OnFileOpen()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

    GLGaeaServer* pGLGaeaServer = pFrame->GetGLGaeaServer();

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Effect Group ( *.Lev ) |*.Lev";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg( TRUE,".Lev", "untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );
	
	std::string strInitPath = sc::getAppPath();
	strInitPath.append(SubPath::GLOGIC_LEVEL_FILE);
	dlg.m_ofn.lpstrInitialDir = strInitPath.c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();
		const char *szWriteName = WriteName.GetString();

		pGLGaeaServer->GetRootMap()->InvalidateDeviceObjects ();
		pGLGaeaServer->GetRootMap()->DeleteDeviceObjects ();

		pGLGaeaServer->CleanUp ();

		pGLGaeaServer->Create4Level ( pView->GetD3DDevice() );
		pGLGaeaServer->SetUpdate ( FALSE );

		//	���� ����.
		pView->LoadGLLandMan ( szWriteName );

		//	editing ���� ���� ���� ����.
		WriteName = WriteName + ".bak";
		pView->SaveGLLandMan ( WriteName.GetString() );

		pFrame->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CLevelEditDoc::OnFileSave()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Effect Group ( *.Lev ) |*.Lev";
	CFileDialog dlg ( FALSE, ".Lev", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY|OFN_OVERWRITEPROMPT, szFilter, pView );

	std::string strInitPath = sc::getAppPath();
	strInitPath.append(SubPath::GLOGIC_LEVEL_FILE);
	dlg.m_ofn.lpstrInitialDir = strInitPath.c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();
		const char *szWriteName = WriteName.GetString();

		pView->SaveGLLandMan ( szWriteName );

		pFrame->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CLevelEditDoc::OnEditCopy()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd
	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == MOBSETPAGEINDEX )
	{
		// �� ������ ����,���� �������� �������� ���� ���� ����Ű �۵��ϵ��� ����;
		if ( CMobSetPage::m_emEditType == CMobSetPage::EM_EDIT_NONE )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.CopyMobSch();
		}
	}
	else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTINDEX )
	{
		pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffect.CopyMobSch();
	}
	else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTLAYERINDEX )
	{
		pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffectLayer.CopyMobSch();
	}
}
void CLevelEditDoc::OnEditCut()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd
	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == MOBSETPAGEINDEX )
	{
		// �� ������ ����,���� �������� �������� ���� ���� ����Ű �۵��ϵ��� ����;
		if ( CMobSetPage::m_emEditType == CMobSetPage::EM_EDIT_NONE )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.CutMobSch();
		}
	}
	//else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTINDEX )
	//{
	//	pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffect.CutMobSch();
	//}
	//else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTLAYERINDEX )
	//{
	//	pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffectLayer.CutMobSch();
	//}
}
void CLevelEditDoc::OnEditPaste()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd
	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == MOBSETPAGEINDEX )
	{
		// �� ������ ����,���� �������� �������� ���� ���� ����Ű �۵��ϵ��� ����;
		if ( CMobSetPage::m_emEditType == CMobSetPage::EM_EDIT_NONE )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.PasteMobSch();			
		}
	}
	else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTINDEX )
	{
		pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffect.PasteMobSch();
	}
	else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTLAYERINDEX )
	{
		pFrame->m_DlgLevelEdit.m_pSheetTab->m_SingleEffectLayer.PasteMobSch();
	}
}

void CLevelEditDoc::OnCloseDocument()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
	GLGaeaServer* pGLGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGLGaeaServer->GetRootMap();

	//	�ڵ����� ������ ���� ����.
	CString strFileName = pGLLandMan->GetFileName();
	if ( strFileName.GetLength()==0 )	strFileName = "untitled.lev";
	strFileName = strFileName + ".last";
	pView->SaveGLLandMan ( strFileName.GetString() );

	CDocument::OnCloseDocument();
}


void CLevelEditDoc::OnFileAllCsvsave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	GLMapList sMapList;
    HRESULT hr = sMapList.LoadMapList();
	if (hr != S_OK)
	{
        std::string ErrorMsg(GLogicData::GetInstance().GetMapListCompileFileName());
        ErrorMsg.append(" open failed.");
		MessageBox(NULL, ErrorMsg.c_str(), "ERROR", MB_OK);
		return;
	}


	std::fstream streamFILE;
	char szPathName[MAX_PATH] = {0};
	StringCchCopy(szPathName,MAX_PATH,GLOGIC::GetPath ());
	StringCchCat(szPathName,MAX_PATH,"level.csv");

	streamFILE.open ( szPathName, std::ios_base::out );

	GLMapList::FIELDMAP MapsList = sMapList.GetMapList();
	GLMapList::FIELDMAP_ITER iter = MapsList.begin();

	pFrame->SetWindowText( "Level Edit - All Csv Saving...." );

	GLGaeaServer* pGaeaServer(pFrame->GetGLGaeaServer());
	GLLandManager* const pNewLandMan = pGaeaServer->NEW_GLLANDMANAGER(0, 0);
	if ( pNewLandMan == NULL )
		return;

	if ( pNewLandMan->addLand(0, false, false) == false )
		return;
	
	for( ;iter != MapsList.end(); ++iter )
	{		
		SMAPNODE mapNode = iter->second;		
        
		if ( pNewLandMan->loadLevelFile(0, mapNode.m_LevelFile.c_str(), NULL) == FALSE )
			continue;

		GLLandMan* pLand(pNewLandMan->getLand());		
		pLand->SaveCsvFile(streamFILE, TRUE);
		pNewLandMan->removeLand(0);		
	}

	pFrame->SetWindowText( "Level Edit" );


	streamFILE.close();

    AfxMessageBox( "All Csv Save Success!", MB_OK | MB_ICONINFORMATION );

}

void CLevelEditDoc::OnFileAllCsvload()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	GLLevelFileServer levelFile;
	pFrame->SetWindowText( "Level Edit - All Csv Loading...." );

	levelFile.LoadAllCsvFile(pView->GetD3DDevice(), NULL);

	pFrame->SetWindowText( "Level Edit" );

    AfxMessageBox( "All Csv Load Success!", MB_OK | MB_ICONINFORMATION );
}

void CLevelEditDoc::OnFileCsvsave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", AfxGetApp()->m_pMainWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;
	
	pView->SaveCsvFile( dlg.GetPathName().GetString() );

	
}	


void CLevelEditDoc::OnFileCsvload()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
    GLGaeaServer* pGLGaeaServer = pFrame->GetGLGaeaServer();
	CFileDialog dlg(
        TRUE,
        ".csv",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|",
        AfxGetApp()->m_pMainWnd);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;

	pGLGaeaServer->SetUpdate ( FALSE );    
	pView->LoadCsvFile( dlg.GetPathName().GetString() );

}
