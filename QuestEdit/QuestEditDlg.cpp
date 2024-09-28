// QuestEditDlg.cpp : ���� ����
//
#include "stdafx.h"
#include <fstream>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"

#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/DxFontMan.h"

#include "../RanLogic/Quest/GLQuestMan.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"

#include "./Edit/SheetWithTab.h"
#include "./QuestEdit.h"
#include "./DlgNpcAction.h"
#include "./QuestEditDlg.h"

#include "../MfcExLib/Splasher.h"
// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

class CMySplashWnd : public CSplashWnd
{
public:
    //Constructors / Destructors
    CMySplashWnd()
    {
        //Lets use an almost opaque splash window with some "cut-out" sections in it
        SetDropShadow();
        SetTransparent(RGB(255, 0, 255), LWA_ALPHA, 255);                       
    }

    DECLARE_DYNCREATE(CMySplashWnd);
};
IMPLEMENT_DYNCREATE(CMySplashWnd, CSplashWnd);
//////////////////////////////////////////////////////////////////////////

GLQuestMan::SNODE CQuestEditDlg::m_sQuestNode;

GLMapList CQuestEditDlg::m_sMapList;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQuestEditDlg ��ȭ ����

CQuestEditDlg::CQuestEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestEditDlg::IDD, pParent)
    , m_pDlgNpcAction(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    CQuestEditApp* pApp = (CQuestEditApp*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CQuestEditDlg::~CQuestEditDlg()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

void CQuestEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQuestEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_CSV_LO, OnMenuitemCsvLO)
	ON_COMMAND(ID_MENUITEM_CSV_SA, OnMenuitemCsvSA )
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENUITEM_TEXT_EXPORT, OnMenuitemTextExport)
	ON_COMMAND(ID_MENUITEM_TEXT_IMPORT, OnMenuitemTextImport)
	ON_COMMAND(ID_MENUITEM_EXCELTXT_IMPORT, OnMenuitemExcelTextImport)
	ON_COMMAND(ID_MENUITEM_EXCELTXT_EXPORT, OnMenuitemExcelTextExport)
	ON_COMMAND(ID_MENUITEM_XMLCONVERT, OnMenuitemXmlConvert)
END_MESSAGE_MAP()


// CQuestEditDlg �޽��� ó����

BOOL CQuestEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    CSplashFactory factory;
    if (!factory.Create(RUNTIME_CLASS(CMySplashWnd)))
    {
        AfxMessageBox(_T("Failed to create splash screen"), MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    //////////////////////////////////////////////////////////////////////////

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

		
	MakeToolBar ();

	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );
			
			if ( !m_strAppPath.IsEmpty() )
			if ( m_strAppPath.GetAt(0) == '"' )
				m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

			strcpy ( m_szAppPath, m_strAppPath.GetString() );
		}
	}

	m_sQuestNode.pQUEST = new GLQUEST;

	m_pEngineDevice->OneTimeSceneInit(
        m_szAppPath,
        this,
		RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont);

  	// GLOGIC init
    CQuestEditApp* pApp = (CQuestEditApp*) AfxGetApp();
	//GMTOOL::Create(m_strAppPath.GetString(), pApp->GetSubPath(), pApp->GetServiceProvider());
    
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    GMTOOL::Create(
        m_szAppPath,
        pApp->GetSubPath(), 
        pApp->GetServiceProvider(),
        NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));
	
    m_sMapList.LoadMapList(); // "mapslist.luc" 

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );


	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if (pFont)
        pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( QUESTTREE_PAGE );

	SetReadMode();

    m_pDlgNpcAction = new DlgNpcAction(this);
    m_pDlgNpcAction->Create(DlgNpcAction::IDD);
    m_pDlgNpcAction->ShowWindow(SW_SHOW);

    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    factory.Close();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CQuestEditDlg::PostNcDestroy()
{
	SAFE_DELETE(m_pSheetTab);
	SAFE_DELETE(m_sQuestNode.pQUEST);

    if (m_pDlgNpcAction)
    {
        m_pDlgNpcAction->ShowWindow(SW_HIDE);
        m_pDlgNpcAction->DestroyWindow();
        delete m_pDlgNpcAction;
        m_pDlgNpcAction = NULL;
    }

	GLQuestMan::GetInstance().SaveSqlFile();
	GLQuestMan::GetInstance().Clean();
	m_pEngineDevice->FinalCleanup();
	GMTOOL::CleanUp ();

	GLogicData::GetInstance().ClearData();

	CDialog::PostNcDestroy();
}

void CQuestEditDlg::MakeToolBar ()
{
	if( !m_wndToolBar.Create(this) || 
        !m_wndToolBar.LoadToolBar(IDR_TOOLBAR1) )
    {
        TRACE("Failed to Create Toolbar\n");
        EndDialog(IDCANCEL);
    }

    CRect rcClientStart;
    CRect rcClientNow;
    GetClientRect(&rcClientStart);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
                   0, reposQuery, rcClientNow);

    CPoint ptOffset(rcClientNow.left - rcClientStart.left,
                    rcClientNow.top  - rcClientStart.top);

    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while( pwndChild )
    {
        pwndChild->GetWindowRect(rcChild);
        ScreenToClient(rcChild);
        rcChild.OffsetRect(ptOffset);
        pwndChild->MoveWindow(rcChild, FALSE);
        pwndChild = pwndChild->GetNextWindow();
    }

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
    rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();

    MoveWindow(rcWindow, FALSE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, 
                   AFX_IDW_CONTROLBAR_LAST, 0);
}

void CQuestEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CQuestEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CQuestEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQuestEditDlg::OnMenuitemNew()
{
	int nret = MessageBox ( "Press the 'YES' Button to Save.", "Caution", MB_YESNOCANCEL );
	switch ( nret )
	{
	case IDYES:
		if ( !DoSave () )	return;	//	���нÿ��� �������� ����.
		break;

	case IDNO:
		//	�������� �ʰ� ����.
		break;

	case IDCANCEL:
		//	��ҽÿ��� �������� ����.
		return;
	};

	SetWindowText( "QuestEdit" );

	//	Note : ���� ����.
	//
	GLQuestMan::GetInstance().Clean();

	m_pSheetTab->m_cQuestTreePage.CleanAllQuest ();
}

void CQuestEditDlg::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".qst",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,this);

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		GLQuestMan::GetInstance().Clean();
		hr = GLQuestMan::GetInstance().LoadFile(dlg.GetFileName().GetString(), TRUE);


		if ( FAILED ( hr ) ) 
		{
			MessageBox ( "Failure to Load File." );
		}

		//	Note : ���� ����.
		//
		m_pSheetTab->m_cQuestTreePage.UpdateTree();
	}
}

bool CQuestEditDlg::DoSave ()
{
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".qst",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLQuestMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		return true;
	}

	return false;
}

void CQuestEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CQuestEditDlg::OnMenuitemExit()
{
	SendMessage ( WM_CLOSE );
}

void CQuestEditDlg::OnMenuitemCsvLO()
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
		return;

	sc::CStringFile StrFile( 65535 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	CStringArray StrArrayProgress;

	CString strError;
	int MID = 0;

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	StrFile.GetNextLine( strLine ); // data

	while( StrFile.GetNextLine( strLine ) ) // head
	{
		STRUTIL::StringSeparate( strLine, StrArray );

		MID = atoi( StrArray[1] );

		
		iter_pos = m_mapQuestMap.find( MID );

		if ( iter_pos == m_mapQuestMap.end() ) 
		{
			strError.Format("%d ID not Found",MID);
			MessageBox(strError,_T("Error"), MB_OK );

			//StrFile.GetNextLine( strLine ); // progress Head
			//StrFile.GetNextLine( strLine ); // progress Data
			continue;
		}
		else
		{
			//StrFile.GetNextLine( strLine ); // progress Head
			//StrFile.GetNextLine( strLine ); // progress Data
			STRUTIL::StringSeparate( strLine, StrArrayProgress );

			sNode = (GLQuestMan::SNODE) iter_pos->second;
			sNode.pQUEST->LoadCsv( StrArray, StrArrayProgress );
			sNode.pQUEST->SAVE( sNode.strFILE.c_str() );
		}
	}

	MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );

	m_pSheetTab->UpdateQuestTreePage();
}


void CQuestEditDlg::OnMenuitemCsvSA()
{

	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
	{
		return;
	}

	std::fstream streamFILE;
	streamFILE.open( dlg.GetPathName().GetString(), std::ios_base::out );


	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	bool bHeadWrite = false;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (GLQuestMan::SNODE) iter_pos->second;

		if( !bHeadWrite )
		{
			sNode.pQUEST->SaveCsvHead(streamFILE, sNode.strFILE.c_str());
			bHeadWrite = true;
		}

		sNode.pQUEST->SaveCsv( streamFILE, sNode.strFILE.c_str() );
	}

	MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CQuestEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(0);
	
	if ( bEnable )
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_ENABLED );
	}
	else
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
}

void CQuestEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CQuestEditDlg::OnClose()
{
	//if ( GLQuestMan::GetInstance().IsModify() )
	{
		int nret = MessageBox ( "Press the 'YES' Button to save.", "Caution", MB_YESNOCANCEL );
		switch ( nret )
		{
		case IDYES:
			if ( !DoSave () )	return;	//	���нÿ��� �������� ����.
			break;

		case IDNO:
			//	�������� �ʰ� ����.
			break;

		case IDCANCEL:
			//	��ҽÿ��� �������� ����.
			return;
		};
	}

	m_pEngineDevice->FinalCleanup();

	CDialog::OnClose();
}

void CQuestEditDlg::OnMenuitemTextExport()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	
	CFileDialog dlg(
        FALSE,
        ".txt",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"text file (*.txt)|*.txt|",
        this);

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	std::fstream streamFILE;
	streamFILE.open( dlg.GetPathName().GetString(), std::ios_base::out );


	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (GLQuestMan::SNODE) iter_pos->second;
		sNode.pQUEST->ExportText( streamFILE );
		streamFILE << std::endl;
	}

	MessageBox( _T("Text Export Success"), _T("Success"), MB_OK );

}

void CQuestEditDlg::OnMenuitemTextImport()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog dlg( TRUE, ".txt", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					"txt file (*.txt)|*.txt|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
		return;

	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return;

	CString strLine;
	CString strFlag;
	CString strError;
	int MID = 0;

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	while( StrFile.GetNextLine( strLine ) ) // <begin>
	{
		if ( strLine != "<BEGIN>") continue;

		StrFile.GetNextLine( strLine ); // <QUEST_ID>

		MID = GetID( strLine );

		iter_pos = m_mapQuestMap.find( MID );

		if ( iter_pos == m_mapQuestMap.end() ) 
		{
			strError.Format("%d ID not Found",MID);
			MessageBox(strError,_T("Error"), MB_OK );

			continue;
		}
		else
		{
			sNode = (GLQuestMan::SNODE) iter_pos->second;
			sNode.pQUEST->ImportText( StrFile );
			sNode.pQUEST->SAVE( sNode.strFILE.c_str() );
		}
	}

	MessageBox( _T("Text Import Success"), _T("Success"), MB_OK );
	
	m_pSheetTab->UpdateQuestTreePage ();
}

int CQuestEditDlg::GetID( CString& strID )
{
	int begin=0;

	CString strTemp = strID.Tokenize("<>",begin);

	if ( strTemp != "QUEST_ID" ) return -1;

	strTemp = strID.Tokenize("<>",begin);

	return atoi( strTemp );
}

void CQuestEditDlg::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

    EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableMenu( ID_MENUITEM_CSV_LO, FALSE );
	
	EnableToobar( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_CSV_SA, FALSE );
	EnableToobar( ID_MENUITEM_CSV_LO, FALSE );

#endif

}

void CQuestEditDlg::SetNpcAction(GLQUEST* pQuest)
{
    if (pQuest && m_pDlgNpcAction)
    {
        m_pDlgNpcAction->SetQuest(pQuest);
        m_pDlgNpcAction->ShowWindow(SW_SHOW);
    }
}

void CQuestEditDlg::OnMenuitemExcelTextImport()
{
	CFileDialog dlg( TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return ;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if ( GLQuestMan::GetInstance().LoadStringTableExcel(dlg.GetPathName().GetString()) == S_OK )
		{
			m_pSheetTab->UpdateQuestTreePage();
			MessageBox( _T("String Table Excel Load Success"), _T("Success"), MB_OK );
		}
	}
}

void CQuestEditDlg::OnMenuitemExcelTextExport()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSavePath;
		if ( GLQuestMan::GetInstance().SaveStringTableExcel(strSavePath) == S_OK )
		{
			std::string OkMsg(
				sc::string::format("\"%1%\" Save Success!", strSavePath.c_str() ) );
			MessageBox( OkMsg.c_str(), _T("Success"), MB_OK );
		}
		else
		{
			MessageBox( _T("Save Fail"), _T("Fail"), MB_OK );
		}
	}
}

// ������ ���� ���Ŀ��� ������;
void ConvertUTF8( const char* szString, std::string& strResult )
{
	const int iLangFlag = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);

	// UTF-8 ��ȯ;
	char	szUTF8[1024] = {0};
	wchar_t	wsUTF8[512] = {0};
	if ( szString != NULL && strlen(szString) != 0)
	{
		_tcscpy(szUTF8, szString);
	}
	else
	{
		_tcscpy(szUTF8, "skip");
	}

	MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag],
		0,szUTF8, 
		strlen(szUTF8)
		,wsUTF8,sizeof(wsUTF8)/sizeof(wsUTF8[0]));			
	WideCharToMultiByte(CP_UTF8, 0, wsUTF8, -1, szUTF8,
		sizeof(szUTF8)/sizeof(szUTF8[0]), NULL,NULL );

	strResult = szUTF8;
}

const bool GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
						   TIXMLOLD::TiXmlElement* rootElem,
						   const char* atrbName,
						   const char* atrbData)
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue, strAtrbData;

	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return false;

	TIXMLOLD::TiXmlElement* nodeElem = rootElem; 
	TIXMLOLD::TiXmlElement* nodeElemChild = rootElem->FirstChildElement(); 	

	while ( nodeElemChild )
	{
		TIXMLOLD::TiXmlAttribute* atrb = nodeElemChild->FirstAttribute();
		while ( atrb )
		{
			if ( strcmp( atrb->Name(),  atrbName ) == 0 && 
				strcmp( atrb->Value(), atrbData ) == 0)
			{
				(*outXmlElem) = nodeElemChild;
				return true;
			}

			strAtrbData = atrb->Value();
			atrb = atrb->Next();
		}
		if ( nodeElemChild == nodeElemChild->NextSiblingElement() )
			break;

		nodeElemChild = nodeElemChild->NextSiblingElement();
	}
	return false;
}

void CQuestEditDlg::OnMenuitemXmlConvert()
{
	std::string strFullPath = sc::getAppPath();
	strFullPath.append("\\QuestStrTable.xml");

	// ������;
	TIXMLOLD::TiXmlDocument XmlDoc;
	TIXMLOLD::TiXmlElement* pRootElem = NULL;
	if ( XmlDoc.LoadFile(strFullPath.c_str(), TIXMLOLD::TIXML_ENCODING_UTF8) )
	{
		pRootElem = XmlDoc.RootElement();
	}
	else
	{
		TIXMLOLD::TiXmlDeclaration*	XmlDecl = new TIXMLOLD::TiXmlDeclaration( "1.0", "utf-8", "");
		XmlDoc.LinkEndChild( XmlDecl );

		pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
		XmlDoc.LinkEndChild(pRootElem);
	}

	const int iLangFlag = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);
	
	GLQuestMan::MAPQUEST_ITER iter_pos = GLQuestMan::GetInstance().GetMap().begin()
		, iter_end = GLQuestMan::GetInstance().GetMap().end();
	GLQuestMan::SNODE sNode;

	for ( ; iter_pos != iter_end; iter_pos++ )
	{
		sNode = (GLQuestMan::SNODE) iter_pos->second;
		const SNATIVEID sNID = sNode.pQUEST->m_sNID;

		if ( sNode.pQUEST )
		{
			// Title;
			{
				CString strNameKey;
				strNameKey.Format("QN_%03d_%03d", sNID.Mid(), sNID.Sid());

				TIXMLOLD::TiXmlElement* pElem, *pChildElem;
				if ( !GetChildElement(&pElem, pRootElem, "Id", strNameKey.GetString()) )
				{
					pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
					pElem->SetAttribute("Ver", 1);
					pElem->SetAttribute("Id", strNameKey.GetString());
					pRootElem->LinkEndChild(pElem);
				}
				if ( !GetChildElement(&pChildElem, pElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
				{
					pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
					pChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
					pElem->LinkEndChild(pChildElem);
				}
				else
				{
					pChildElem->RemoveChild(pChildElem->FirstChild());
				}

				std::string strUTF8;
				ConvertUTF8(sNode.pQUEST->GetTITLE(), strUTF8);
				TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strUTF8.c_str());
				pChildElem->LinkEndChild(pXmlText);	
			}

			// Comment
			{
				CString strNameKey;
				strNameKey.Format("QD_%03d_%03d", sNID.Mid(), sNID.Sid());

				TIXMLOLD::TiXmlElement* pElem, *pChildElem;
				if ( !GetChildElement(&pElem, pRootElem, "Id", strNameKey.GetString()) )
				{
					pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
					pElem->SetAttribute("Ver", 1);
					pElem->SetAttribute("Id", strNameKey.GetString());
					pRootElem->LinkEndChild(pElem);
				}
				if ( !GetChildElement(&pChildElem, pElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
				{
					pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
					pChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
					pElem->LinkEndChild(pChildElem);
				}
				else
				{
					pChildElem->RemoveChild(pChildElem->FirstChild());
				}

				// UTF-8 ��ȯ;
				std::string strUTF8;
				ConvertUTF8(sNode.pQUEST->GetCOMMENT(), strUTF8);
				TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strUTF8.c_str());
				pChildElem->LinkEndChild(pXmlText);
			}

			// Quest Step
			size_t i;
			for ( i = 0; i < sNode.pQUEST->m_vecProgStep.size(); ++i )
			{
				const GLQUEST_STEP& itrStep = sNode.pQUEST->m_vecProgStep[i];
				// Step Title;
				{
					CString strStepTitle;
					strStepTitle.Format("QN_%03d_%03d_P_%d", sNID.Mid(), sNID.Sid(), i);
					TIXMLOLD::TiXmlElement* pElem, *pChildElem;
					if ( !GetChildElement(&pElem, pRootElem, "Id", strStepTitle.GetString()) )
					{
						pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
						pElem->SetAttribute("Ver", 1);
						pElem->SetAttribute("Id", strStepTitle.GetString());
						pRootElem->LinkEndChild(pElem);
					}
					if ( !GetChildElement(&pChildElem, pElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
					{
						pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
						pChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
						pElem->LinkEndChild(pChildElem);
					}
					else
					{
						pChildElem->RemoveChild(pChildElem->FirstChild());
					}

					// UTF-8 ��ȯ;
					std::string strUTF8;
					ConvertUTF8(itrStep.GetTITLE(), strUTF8);
					TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strUTF8.c_str());
					pChildElem->LinkEndChild(pXmlText);
				}
				// Step Comment;
				{
					CString strStepTitle;
					strStepTitle.Format("QD_%03d_%03d_D_%d", sNID.Mid(), sNID.Sid(), i);
					TIXMLOLD::TiXmlElement* pElem, *pChildElem;
					if ( !GetChildElement(&pElem, pRootElem, "Id", strStepTitle.GetString()) )
					{
						pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
						pElem->SetAttribute("Ver", 1);
						pElem->SetAttribute("Id", strStepTitle.GetString());
						pRootElem->LinkEndChild(pElem);
					}
					if ( !GetChildElement(&pChildElem, pElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
					{
						pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
						pChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
						pElem->LinkEndChild(pChildElem);
					}
					else
					{
						pChildElem->RemoveChild(pChildElem->FirstChild());
					}

					// UTF-8 ��ȯ;
					std::string strUTF8;
					ConvertUTF8(itrStep.GetCOMMENT(), strUTF8);
					TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strUTF8.c_str());
					pChildElem->LinkEndChild(pXmlText);
				}
			}
		}
	}
	XmlDoc.SaveFile(strFullPath);

	std::string Msg(
		sc::string::format(
		"%1% �� ����Ǿ����ϴ�.",
		strFullPath));
	MessageBox(Msg.c_str(), "Success", MB_OK);
}