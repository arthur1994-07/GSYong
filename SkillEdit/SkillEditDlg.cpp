// SkillEditDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/GameTextLoader.h"
#include "../EngineLib/DxBase/RenderDevice.h"
#include "../EngineLib/DxBase/CustomDevice.h"
#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/DxFontMan.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLStringTable.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"

#include "../RanLogicServer/DxServerInstance.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "./SkillEdit.h"
#include "./SkillEditDlg.h"
#include "./SkillViewDlg.h"
#include "./DlgExcelDetailExport.h"
#include "./Edit/SheetWithTab.h"
#include "./LogDialog.h"
#include "../MfcExLib/Splasher.h"   //스플래쉬

#include "../RanLogic/Skill/GLSkill.h"

#include "./SkillIconImage.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"
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


// CSkillEditDlg 대화 상자

CSkillEditDlg::CSkillEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillEditDlg::IDD, pParent)
	, m_pEngineDevice( NULL )
	, m_pSheetTab( NULL )
	, m_pSkillViewDlg( NULL )
	, m_pSkillIconImage( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    CSkillEditApp* pApp = (CSkillEditApp*) AfxGetApp();

    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CSkillEditDlg::~CSkillEditDlg()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
	
	m_pSkillIconImage->DetachWnd();
	SAFE_DELETE( m_pSkillIconImage );
}

void CSkillEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSkillEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SYNC, OnBnClickedButtonUpdateSync)
//	ON_WM_NCDESTROY()
//	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_STRTBL, OnBnClickedButtonReloadStrtbl)
	ON_BN_CLICKED(IDC_BUTTON_STRTBL_SYNC, OnBnClickedButtonStrtblSync)
	ON_BN_CLICKED(IDC_CSV_SAVE, OnBnClickedCsvSave)
	ON_BN_CLICKED(IDC_CSV_LOAD, OnBnClickedCsvLoad)
	ON_BN_CLICKED(IDC_ICON_PASER, &CSkillEditDlg::OnBnClickedIconPaser)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_IMPORT, OnBnClickedBtnExcelStrTblImport)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_EXPORT, OnBnClickedBtnExcelStrTblExport)
	ON_BN_CLICKED(IDC_BUTTON_XMLCONVERT, OnBnClickedBtnXmlConvert)
	ON_BN_CLICKED(IDC_BUTTON_EXCEL_EXPORT, OnBnClickedBtnExcelExport)
END_MESSAGE_MAP()


// CSkillEditDlg 메시지 처리기

BOOL CSkillEditDlg::OnInitDialog()
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

	//	대화 상자에 툴바를 만들어 붙힌다.
	//
	MakeToolBar ();

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	m_pEngineDevice->OneTimeSceneInit(
        m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

    GLCrowDataMan::GetInstance().OneTimeSceneInit();
	GLSkillMan::GetInstance().OneTimeSceneInit();

    CSkillEditApp* pApp = (CSkillEditApp*) AfxGetApp();

	HWND hWnd = GetSafeHwnd();
	CRenderDevice::OnCreate( hWnd, hWnd, hWnd, false, false );

	g_pCustomDevice = new CCustomDevice(
		pApp->GetServiceProvider(),
		pApp->GetSubPath(),
		m_strAppPath,
		this,
		RANPARAM::strFontType,
		RENDERPARAM::emLangSet,
		RANPARAM::strGDIFont );
	CRenderDevice::CreateObject( g_pCustomDevice );
	CRenderDevice::AttachObject( g_pCustomDevice, CRenderDevice::EM_INSERT_FRONT );

	g_pRI = new CRenderInterface;
	CRenderDevice::CreateObject( g_pRI );
	CRenderDevice::AttachObject( g_pRI, CRenderDevice::EM_INSERT_FRONT );

    //GMTOOL::Create ( m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider() );
    // 스플래쉬 화면을 출력하기위해 추가되었음
    GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
		boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

	// Note : 인터페이스 워드 로딩
	char szFullPath[ MAX_PATH ] = { 0 };
	StringCchCopy ( szFullPath, MAX_PATH, m_szAppPath );
	StringCchCat  ( szFullPath, MAX_PATH
		, pApp->GetSubPath()->GuiFileRoot().c_str());
	CGameTextMan::GetInstance().SetPath( szFullPath );
	CGameTextMan::GetInstance().LoadText( RANPARAM::strGameWord.GetString(), CGameTextMan::EM_GAME_WORD, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	CGameTextMan::GetInstance().LoadText( RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	// CGameTextMan::GetInstance().LoadText( RANPARAM::strSkillText.GetString(), CGameTextMan::EM_SKILL_TEXT, TRUE , RANPARAM::strCountry, FALSE, TRUE );

	
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
	
	m_pSheetTab->SetActivePage ( SKILLTREEPAGE );

	SetReadMode();

	// Skill Viewer 추가
	m_pSkillViewDlg = new SkillViewDlg( this );
	m_pSkillViewDlg->Create( SkillViewDlg::IDD );
	m_pSkillViewDlg->ShowWindow( SW_SHOW );

    // 스플래쉬 화면을 출력하기위해 추가되었음
    factory.Close();
	
	m_pSkillIconImage = new SkillIconImage();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CSkillEditDlg::MakeToolBar ()
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

void CSkillEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSkillEditDlg::OnPaint() 
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
HCURSOR CSkillEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSkillEditDlg::PostNcDestroy()
{
	bool bValid = GLSkillMan::GetInstance().ValidData ();
	if ( bValid )	GLSkillMan::GetInstance().SaveFile ( "lastedit.ssf.bak" );
	
	// 종료하면서 skilllist.sql 파일 생성하도록
	GLSkillMan::GetInstance().SaveSqlFile();

	SAFE_DELETE ( m_pSheetTab );

	if( m_pSkillViewDlg )
	{
		m_pSkillViewDlg->ShowWindow( SW_HIDE );
		m_pSkillViewDlg->DestroyWindow();
		SAFE_DELETE( m_pSkillViewDlg );
	}

	GLSkillMan::GetInstance().FinalCleanup ();
	CDebugSet::FinalCleanup ();

	GMTOOL::CleanUp ();

	m_pEngineDevice->FinalCleanup();

	CRenderDevice::DetachObject( g_pRI );
	CRenderDevice::DestroyObject( g_pRI );
	SAFE_DELETE( g_pRI );

	CRenderDevice::DetachObject( g_pCustomDevice );
	CRenderDevice::DestroyObject( g_pCustomDevice );
	SAFE_DELETE( g_pCustomDevice );

	CRenderDevice::OnDestroy();

	CDialog::PostNcDestroy();
}

void CSkillEditDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Skill Set File (*.ssf)|*.ssf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ssf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLSkillMan::GetInstance().LoadFile ( dlg.GetFileName().GetString(), TRUE );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File." );
		}

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CGameTextMan::GetInstance().Clear( CGameTextMan::EM_SKILL_TEXT );
			if ( FALSE == CGameTextMan::GetInstance().LoadText( RANPARAM::strSkillText.GetString(), CGameTextMan::EM_SKILL_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT ) )
			{
				MessageBox ( "Failure to load file." );
			}
		}
		else
		{
			GLStringTable::GetInstance().CLEAR();
			// 스킬 스트링테이블을 로드한다. by 경대
			if( !GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL ) )
			{
				MessageBox ( "Failure to load file." );
			}
		}
		
		m_pSheetTab->m_SkillTreePage.UpdateTreeItem ();
	}
}

bool CSkillEditDlg::DoSave ()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Skill Set File (*.ssf)|*.ssf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".ssf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLSkillMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File." );
			return false;
		}

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			if ( FALSE == CGameTextMan::GetInstance().SaveXML(RANPARAM::strSkillText.GetString(), CGameTextMan::EM_SKILL_TEXT, TRUE) )
			{
				MessageBox ( "Failure to Save File" );
				return false;
			}
		}
		else
		{
			if ( !GLStringTable::GetInstance().SAVEFILE( GLStringTable::SKILL ) )
			{
				MessageBox ( "Failure to Save File" );
				return false;
			}
		}

		return true;
	}

	return false;
}

void CSkillEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CSkillEditDlg::OnMenuitemExit()
{
	SendMessage ( WM_CLOSE );
}

void CSkillEditDlg::OnMenuitemNew()
{
	GLSkillMan::GetInstance().FinalCleanup ();
	GLSkillMan::GetInstance().OneTimeSceneInit();

	m_pSheetTab->m_SkillTreePage.CleanAllItem ();
}

void CSkillEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
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

void CSkillEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CSkillEditDlg::OnBnClickedButtonUpdateSync()
{
    CSkillEditApp* pApp = (CSkillEditApp*) AfxGetApp();
	GLSkillMan::GetInstance().SyncUpdateData(pApp->GetSubPath());

	CLogDialog LogDlg;
	LogDlg.SetLogFile ( GLSkillMan::_LOGFILE );
	LogDlg.DoModal ();
}

void CSkillEditDlg::OnClose()
{
	if ( GLSkillMan::GetInstance().IsModify() )
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

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=SKILLTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}

void CSkillEditDlg::OnBnClickedButtonReloadStrtbl() // by 경대
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 아이템 스트링테이블을 다시 로드한다. by 경대
	//
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().Clear( CGameTextMan::EM_SKILL_TEXT );
		CGameTextMan::GetInstance().LoadText(RANPARAM::strSkillText, CGameTextMan::EM_SKILL_TEXT, language::strProvideLanguage[RANPARAM::emProvideLangFlag], CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	}
	else
	{
		GLStringTable::GetInstance().CLEAR();
		GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL );
	}
}

void CSkillEditDlg::OnBnClickedButtonStrtblSync()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GLSkillMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CSkillEditDlg::OnBnClickedCsvSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLSkillMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CSkillEditDlg::OnBnClickedCsvLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLSkillMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateTreePage();
		OnBnClickedButtonReloadStrtbl();
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CSkillEditDlg::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	SetWin_ShowWindow( this, IDC_CSV_LOAD, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTBL_SYNC, FALSE );

	EnableMenu ( ID_MENUITEM_NEW, FALSE );
	EnableToobar ( ID_MENUITEM_NEW, FALSE );

#endif
}

// Skill Viewer 세팅
void CSkillEditDlg::SetSkillView( GLSKILL* pSkill )
{
	if( pSkill && m_pSkillViewDlg )
	{
		m_pSkillViewDlg->SetSkill( pSkill );
		m_pSkillViewDlg->ShowWindow( SW_SHOW );
	}
}

void CSkillEditDlg::ExportExcel ( boost::function<void (int)> PercentCallbackFunc )
{
	if ( !m_pSheetTab )
		return;

	m_pSheetTab->ExportExcelDetail( PercentCallbackFunc );
}

bool CSkillEditDlg::IsSkillLevelChecked ( WORD wLevel )
{
	static int LevelCheckBoxID[ 9 ] = 
	{
		IDC_CHECK_EXEXCEL_LV_1,
		IDC_CHECK_EXEXCEL_LV_2,
		IDC_CHECK_EXEXCEL_LV_3,
		IDC_CHECK_EXEXCEL_LV_4,
		IDC_CHECK_EXEXCEL_LV_5,
		IDC_CHECK_EXEXCEL_LV_6,
		IDC_CHECK_EXEXCEL_LV_7,
		IDC_CHECK_EXEXCEL_LV_8,
		IDC_CHECK_EXEXCEL_LV_9
	};

	if ( wLevel >= 0 && wLevel < 9 )
		return IsDlgButtonChecked( LevelCheckBoxID[ wLevel ] );
	else
		return false;
}

int CALLBACK CSkillEditDlg::BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	switch( uMsg )
	{
	case BFFM_INITIALIZED:      // 폴더 선택 대화상자를 초기화 할 때, 초기 경로 설정
		{
			::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData );
		}
		break;

		// BROWSEINFO 구조체의 ulFlags 값에 BIF_STATUSTEXT 가 설정된 경우 호출
		// 단, BIF_NEWDIALOGSTYLE 가 설정되어 있을 경우 호출되지 않음
	case BFFM_SELCHANGED:       // 사용자가 폴더를 선택할 경우 대화상자에 선택된 경로 표시
		{
			TCHAR szPath[ MAX_PATH ] = { 0, };

			::SHGetPathFromIDList( (LPCITEMIDLIST)lParam, szPath );
			::SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath );
		}
		break;

		// BROWSEINFO 구조체의 ulFlags 값에 BIF_VALIDATE 가 설정된 경우 호출
		// BIF_EDITBOX 와 같이 설정된 경우만 호출됨
	case BFFM_VALIDATEFAILED:   // 에디터 콘트롤에서 폴더 이름을 잘못 입력한 경우 호출
		{
			::MessageBox( hWnd, _T( "Could not find folder." ), _T( "Error" ),
				MB_ICONERROR | MB_OK );
		}
		break;
	}
	return 0;
}


void CSkillEditDlg::OnBnClickedIconPaser()
{
	if( m_pSkillIconImage )
	{
		TCHAR szFolderPath[MAX_PATH] = {0};
		LPITEMIDLIST pidlSelected =NULL;
		
		//SetRoot
		//LPITEMIDLIST pidRoot = NULL;
		//SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &pidRoot);
		
		//SelectFolder
		TCHAR szDefaultPath[MAX_PATH] = {0,};
		SHGetSpecialFolderPath(m_hWnd, szDefaultPath, CSIDL_DRIVES, false);

		BROWSEINFO bi;
		memset(&bi,0,sizeof(bi));
		bi.hwndOwner	= GetSafeHwnd();
		bi.ulFlags		= BIF_USENEWUI | BIF_VALIDATE;
		bi.lParam		= (LPARAM)(LPCTSTR)szDefaultPath;//GLOGIC::GetPath();
		bi.lpfn         = BrowseCallbackProc;
		bi.lpszTitle	= NULL;
		//bi.pidlRoot	= pidRoot;

		pidlSelected = ::SHBrowseForFolder(&bi);

		if( pidlSelected != NULL )
		{
			::SHGetPathFromIDList( pidlSelected, szFolderPath );

			CString strPath( szFolderPath );
			strPath += "\\";
			m_pSkillIconImage->SaveIconToFile( strPath );
			
			//에러로그 저장
			CString strFileName("DDS파일 누락 목록.txt"), 
					strComment("스킬 아이콘 DDS파일을 포함하지 않은 스킬입니다.");

			m_pSkillIconImage->SaveIconErrorListToFile( strPath, strFileName, strComment, false);
		}
		
	}
}

void CSkillEditDlg::OnBnClickedBtnExcelStrTblImport()
{
	CFileDialog dlg( TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return ;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if ( GLSkillMan::GetInstance().LoadStrTblExcelFile(dlg.GetPathName().GetString()) == S_OK )
		{
			m_pSheetTab->m_SkillTreePage.UpdateTreeItem();
			MessageBox( _T("String Table CSV Load Success"), _T("Success"), MB_OK );
		}
	}
}

void CSkillEditDlg::OnBnClickedBtnExcelStrTblExport()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSavePath;
		if ( GLSkillMan::GetInstance().SaveStrTblExcelFile(strSavePath) == S_OK )
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

// 데이터 통합 이후에는 지우자;
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

void CSkillEditDlg::OnBnClickedBtnXmlConvert()
{
	std::string strFullPath = sc::getAppPath();
	strFullPath.append("\\SkillStrTable.xml");

	// 컨버팅;
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

	int i, j, iLangFlag = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);
	for ( i = 0; i < EMSKILLCLASS_NSIZE; ++i )
	{
		for ( j = 0; j < GLSkillMan::MAX_CLASSSKILL; ++j )
		{
			GLSKILL* pSKILL = NULL;
			if ( (pSKILL = GLSkillMan::GetInstance().GetData(i, j)) != NULL )
			{
				CString strElemName;
				strElemName.Format("SN_%03d_%03d", i, j);
				// Name;
				{
					TIXMLOLD::TiXmlElement* pElem, *pChildElem;
					if ( !GetChildElement(&pElem, pRootElem, "Id", strElemName.GetString()) )
					{
						pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
						pElem->SetAttribute("Ver", 1);
						pElem->SetAttribute("Id", strElemName.GetString());
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

					char	szUTF8[1024] = {0};
					wchar_t	wsUTF8[512] = {0};
					if ( pSKILL->GetName() != NULL && strlen(pSKILL->GetName()) != 0)
					{
						_tcscpy(szUTF8, pSKILL->GetName());
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

					TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(szUTF8);
					pChildElem->LinkEndChild(pXmlText);
				}
				strElemName.Format("SD_%03d_%03d", i, j);
				// Desc;
				{
					TIXMLOLD::TiXmlElement* pElem, *pChildElem;
					if ( !GetChildElement(&pElem, pRootElem, "Id", strElemName.GetString()) )
					{
						pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
						pElem->SetAttribute("Ver", 1);
						pElem->SetAttribute("Id", strElemName.GetString());
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

					char	szUTF8[1024] = {0};
					wchar_t	wsUTF8[512] = {0};
					if ( pSKILL->GetDesc() != NULL && strlen(pSKILL->GetDesc()) != 0)
					{
						CString strCheck = pSKILL->GetDesc();
						strCheck.Replace(" ", "");
						if ( strCheck.GetLength() == 0 )
							_tcscpy(szUTF8, "skip");
						else
							_tcscpy(szUTF8, pSKILL->GetDesc());
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

					TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(szUTF8);
					pChildElem->LinkEndChild(pXmlText);
				}
			}
		}
	}
	XmlDoc.SaveFile(strFullPath);

	std::string Msg(
		sc::string::format(
		"%1% 에 저장되었습니다.",
		strFullPath));
	MessageBox(Msg.c_str(), "Success", MB_OK);
}

void CSkillEditDlg::OnBnClickedBtnExcelExport()
{
	// Excel Export Dialog 호출;
	DlgExcelDetailExport Dlg;
	Dlg.DoModal();
}