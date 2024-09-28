// SkillEditDlg.cpp : ���� ����
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
#include "../MfcExLib/Splasher.h"   //���÷���

#include "../RanLogic/Skill/GLSkill.h"

#include "./SkillIconImage.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"
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


// CSkillEditDlg ��ȭ ����

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


// CSkillEditDlg �޽��� ó����

BOOL CSkillEditDlg::OnInitDialog()
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

	//	��ȭ ���ڿ� ���ٸ� ����� ������.
	//
	MakeToolBar ();

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
		boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

	// Note : �������̽� ���� �ε�
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

	//	Note : Sheet 1 �� �����.
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

	// Skill Viewer �߰�
	m_pSkillViewDlg = new SkillViewDlg( this );
	m_pSkillViewDlg->Create( SkillViewDlg::IDD );
	m_pSkillViewDlg->ShowWindow( SW_SHOW );

    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    factory.Close();
	
	m_pSkillIconImage = new SkillIconImage();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSkillEditDlg::OnPaint() 
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
HCURSOR CSkillEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSkillEditDlg::PostNcDestroy()
{
	bool bValid = GLSkillMan::GetInstance().ValidData ();
	if ( bValid )	GLSkillMan::GetInstance().SaveFile ( "lastedit.ssf.bak" );
	
	// �����ϸ鼭 skilllist.sql ���� �����ϵ���
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Skill Set File (*.ssf)|*.ssf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
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
			// ��ų ��Ʈ�����̺��� �ε��Ѵ�. by ���
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Skill Set File (*.ssf)|*.ssf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
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

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=SKILLTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}

void CSkillEditDlg::OnBnClickedButtonReloadStrtbl() // by ���
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ������ ��Ʈ�����̺��� �ٽ� �ε��Ѵ�. by ���
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GLSkillMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CSkillEditDlg::OnBnClickedCsvSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GLSkillMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CSkillEditDlg::OnBnClickedCsvLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	SetWin_ShowWindow( this, IDC_CSV_LOAD, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTBL_SYNC, FALSE );

	EnableMenu ( ID_MENUITEM_NEW, FALSE );
	EnableToobar ( ID_MENUITEM_NEW, FALSE );

#endif
}

// Skill Viewer ����
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
	case BFFM_INITIALIZED:      // ���� ���� ��ȭ���ڸ� �ʱ�ȭ �� ��, �ʱ� ��� ����
		{
			::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData );
		}
		break;

		// BROWSEINFO ����ü�� ulFlags ���� BIF_STATUSTEXT �� ������ ��� ȣ��
		// ��, BIF_NEWDIALOGSTYLE �� �����Ǿ� ���� ��� ȣ����� ����
	case BFFM_SELCHANGED:       // ����ڰ� ������ ������ ��� ��ȭ���ڿ� ���õ� ��� ǥ��
		{
			TCHAR szPath[ MAX_PATH ] = { 0, };

			::SHGetPathFromIDList( (LPCITEMIDLIST)lParam, szPath );
			::SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath );
		}
		break;

		// BROWSEINFO ����ü�� ulFlags ���� BIF_VALIDATE �� ������ ��� ȣ��
		// BIF_EDITBOX �� ���� ������ ��츸 ȣ���
	case BFFM_VALIDATEFAILED:   // ������ ��Ʈ�ѿ��� ���� �̸��� �߸� �Է��� ��� ȣ��
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
			
			//�����α� ����
			CString strFileName("DDS���� ���� ���.txt"), 
					strComment("��ų ������ DDS������ �������� ���� ��ų�Դϴ�.");

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

// ������ ���� ���Ŀ��� ������;
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
		"%1% �� ����Ǿ����ϴ�.",
		strFullPath));
	MessageBox(Msg.c_str(), "Success", MB_OK);
}

void CSkillEditDlg::OnBnClickedBtnExcelExport()
{
	// Excel Export Dialog ȣ��;
	DlgExcelDetailExport Dlg;
	Dlg.DoModal();
}