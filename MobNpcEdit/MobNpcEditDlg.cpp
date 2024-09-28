// MobNpcEditDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../Enginelib/GUInterface/GameTextControl.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLStringTable.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "./MobNpcEdit.h"
#include "./Edit/SheetWithTab.h"
#include "./MobNpcEditDlg.h"
#include "../MfcExLib/Splasher.h"
// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 스플래쉬 화면을 출력하기위해 추가되었음
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

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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


// CMobNpcEditDlg 대화 상자

CMobNpcEditDlg::CMobNpcEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMobNpcEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    CMobNpcEditApp* pApp = (CMobNpcEditApp*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CMobNpcEditDlg::~CMobNpcEditDlg()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

void CMobNpcEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMobNpcEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMobNpcEditDlg 메시지 처리기

BOOL CMobNpcEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    // 스플래쉬 화면을 출력하기위해 추가되었음
    CSplashFactory factory;
    if (!factory.Create(RUNTIME_CLASS(CMySplashWnd)))
    {
        AfxMessageBox(_T("Failed to create splash screen"), MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    //////////////////////////////////////////////////////////////////////////

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	CMobNpcEditApp* pApp = (CMobNpcEditApp*) AfxGetApp();

	m_pEngineDevice->OneTimeSceneInit(
        m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

    GLCrowDataMan::GetInstance().OneTimeSceneInit();
	
	//GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider());
    
    // 스플래쉬 화면을 출력하기위해 추가되었음
    GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( CROWTREEPAGE );

	SetReadMode();

//	m_pSheetTab->m_ItemSimpleTreePage.SetComboBoxHandle ( &m_ctrlComboMenu );

    // 스플래쉬 화면을 출력하기위해 추가되었음
    factory.Close();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CMobNpcEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CMobNpcEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CMobNpcEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMobNpcEditDlg::PostNcDestroy()
{
	GLCrowDataMan::GetInstance();
	GLCrowDataMan::GetInstance().SaveFile ( "lastedit.mnsf.bak" );

	GLCrowDataMan::GetInstance().FinalCleanup ();
	m_pEngineDevice->FinalCleanup ();
	GMTOOL::CleanUp ();

	GLogicData::GetInstance().ClearData();

	SAFE_DELETE ( m_pSheetTab );

	CDialog::PostNcDestroy();
}

void CMobNpcEditDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Mob/Npc Setting File (*.mnsf)|*.mnsf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".mnsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CMobNpcEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLCrowDataMan::GetInstance().LoadFile(dlg.GetFileName ().GetString(), TRUE, TRUE);
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File." );
		}

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CGameTextMan::GetInstance().Clear( CGameTextMan::EM_CROW_TEXT );
			if ( !CGameTextMan::GetInstance().LoadText( RANPARAM::strCrowText, CGameTextMan::EM_CROW_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT ) )
				MessageBox ( "Failure to load file." );
		}
		else
		{
			GLStringTable::GetInstance().CLEAR();
			// 아이템 스트링테이블을 로드한다. by 경대
			if( !GLStringTable::GetInstance().LOADFILE( GLCrowDataMan::_STRINGTABLE, GLStringTable::CROW ) )
				MessageBox ( "Failure to load file." );
		}
		
		m_pSheetTab->m_CrowTreePage.UpdateTree ();
	}
}

bool CMobNpcEditDlg::DoSave ()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Mob/Npc Setting File (*.mnsf)|*.mnsf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".mnsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CMobNpcEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLCrowDataMan::GetInstance().SaveFile ( dlg.GetFileName ().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File." );
			return false;
		}

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			if ( !CGameTextMan::GetInstance().SaveXML( RANPARAM::strCrowText, CGameTextMan::EM_CROW_TEXT, TRUE ) )
				MessageBox ( "Failure to Save file." );
		}

		if ( !GLStringTable::GetInstance().SAVEFILE( GLStringTable::CROW ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		return true;
	}

	return false;
}

void CMobNpcEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CMobNpcEditDlg::OnMenuitemExit()
{
	SendMessage ( WM_CLOSE );
}

void CMobNpcEditDlg::OnMenuitemNew()
{
	GLCrowDataMan::GetInstance().FinalCleanup ();
	GLCrowDataMan::GetInstance().OneTimeSceneInit();

	m_pSheetTab->m_CrowTreePage.CleanAllItem ();
}

void CMobNpcEditDlg::MakeToolBar ()
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

void CMobNpcEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
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

void CMobNpcEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}
void CMobNpcEditDlg::OnClose()
{
	if ( GLCrowDataMan::GetInstance().IsModify() )
	{
		int nret = MessageBox ( "Press the 'YES' Button to Save.", "Caution", MB_YESNOCANCEL );
		switch ( nret )
		{
		case IDYES:
			if ( !DoSave () )	return;	//	실패시에는 종료하지 않음.
			break;

		case IDNO:
			//	저장하지 않고 종료.
			break;

		case IDCANCEL:
			//	취소시에는 종료하지 않음.
			return;
		};
	}

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=CROWTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}


void CMobNpcEditDlg::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM
    
	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}