// EffectToolDoc.cpp : implementation of the CEffectToolDoc class
//

#include "stdafx.h"
#include "EffectTool.h"
#include "MainFrm.h"

#include "../MfcExLib/ExLibs/FECFileDialog.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "EffectToolDoc.h"
#include "EffectToolView.h"

#include "./Properties/EffGroupPage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc

IMPLEMENT_DYNCREATE(CEffectToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectToolDoc, CDocument)
	//{{AFX_MSG_MAP(CEffectToolDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc construction/destruction

CEffectToolDoc::CEffectToolDoc()
{
}

CEffectToolDoc::~CEffectToolDoc()
{
}

BOOL CEffectToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	TSTRING strName(_T(""));
	SetTitleAddVersion( strName.c_str() );

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc serialization

void CEffectToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc diagnostics

#ifdef _DEBUG
void CEffectToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffectToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc commands

void CEffectToolDoc::OnFileSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );	// Ŀ�� Ŭ���� ���δ�.
			

	////	Note : ���� ������ ���� �־� �ش�.
	//CEffGroupPage::m_EffPropGroup.m_vMax = pView->m_EditBox.m_vMax;
	//CEffGroupPage::m_EffPropGroup.m_vMin = pView->m_EditBox.m_vMin;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg( FALSE,".egp", CEffGroupPage::m_EffPropGroup.m_strFileName.c_str(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pView );
	
	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CEffGroupPage::m_EffPropGroup.SaveFile ( dlg.GetPathName().GetString() );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CEffectToolDoc::OnFileOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = GetFirstViewPosition();
	CEffectToolView* pView = (CEffectToolView*) GetNextView( pos );
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pView->SetCurClick( FALSE );	// Ŀ�� Ŭ���� ���δ�.

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();
		SetTitleAddVersion ( WriteName );

		//	Note : Sound Log
#ifdef	_DEBUG
		CDebugSet::ToFile ( "UseSoundList.txt", "" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[-------------------------------------------------------------]" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[!.%s]", WriteName.GetString() );
	
		//////////////////////////////////////////////
		//	<--	���� �ð� Ȯ��
		char	szTime[128];
		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time ��������
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time ��������

		//	<--0	TimeZone ��������
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone ��������

		//	<--0	Universal Time�� TimeZone�� �ռ�
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time�� TimeZone�� �ռ�

		sprintf ( szTime, "%d-%02d-%02dT%02d:%02d", LocalTime.wYear, LocalTime.wMonth,
			LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
		//	-->	���� �ð� Ȯ��
		////////////////////////////////////////////////

		CDebugSet::ToFile ( "UseSoundList.txt", "[$.%s]", szTime );	
#endif	//	_DEBUG

		CEffGroupPage* pEffGroupPage = pFrame->GetEffGroupPage ();
		pEffGroupPage->LoadEffPropGroup ( WriteName );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.














	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CEffectToolView* pView = (CEffectToolView *) pFrame->GetActiveView ();

	//CString szFilter = "Effect Group ( *.egp ) |*.egp";
	//CFECFileDialog dlg ( TRUE, _T(".egp"), NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, pView );

	//dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();

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

	//		CEffGroupPage* pEffGroupPage = pFrame->GetEffGroupPage ();
	//		pEffGroupPage->LoadEffPropGroup ( strFileName.GetString() );
	//	}
	//}
}

void CEffectToolDoc::OnFileNew()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CEffGroupPage::m_EffPropGroup.CleanUp();
	SetTitleAddVersion ( "���� ����" );
}

// Version �� �߰��Ѵ�.
void CEffectToolDoc::SetTitleAddVersion( LPCTSTR lpszTitle )
{
	//////////////////////////////////////////////////////////////////////////
	// WorldEdit ���� �۾��Ǿ��� ��.
	// ���� �̸��� ������ ���´�.
	// Reload ���� �ʿ���.
	// m_strTitleOrig = lpszTitle;

	TSTRING strTitle;
	strTitle = lpszTitle;
	strTitle += _T(" - [ver.");
	strTitle += sc::getFileVersion();
	strTitle += _T("]");
	SetTitle( strTitle.c_str() );
}