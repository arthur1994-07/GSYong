#include "stdafx.h"

#include <vector>
#include "ItemEdit.h"
#include "ItemEditDlg.h"

#include "../EngineLib/G-Logic/GLOGIC.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxBase/RenderDevice.h"
#include "../EngineLib/DxBase/CustomDevice.h"
#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/GUInterface/GameTextLoader.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/DxFontMan.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/Skill/GLSkill.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLStringTable.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../MfcExLib/Splasher.h"   //���÷���

#include "./Dialog/SearchDialog.h"
#include "./Dialog/DlgExcelDetalExport.h"
#include "./Properties/SheetWithTab.h"
#include "./CommonPreview.h"
#include "./LogDialog.h"

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
//#include <boost/function.hpp>

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


// CItemEditDlg ��ȭ ����



CItemEditDlg::CItemEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    CItemEditApp* pApp = (CItemEditApp*) AfxGetApp();
}

CItemEditDlg::~CItemEditDlg()
{
}

void CItemEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
}

BEGIN_MESSAGE_MAP(CItemEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SYNC, OnBnClickedButtonUpdateSync)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_STRTB_SYNC, OnBnClickedButtonStrtbSync)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_IMPORT, OnBnClickedBtnExcelStrTblImport)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_EXPORT, OnBnClickedBtnExcelStrTblExport)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_STRTBL, OnBnClickedButtonReloadStrtbl)
	ON_BN_CLICKED(IDC_BTN_CSV_SAVE, OnBnClickedBtnCsvSave)
	ON_BN_CLICKED(IDC_BTN_CSV_LOAD, OnBnClickedBtnCsvLoad)
	ON_BN_CLICKED(IDC_ITEM_SEARCH_BUTTON, OnBnClickedItemSearchButton)
	ON_BN_CLICKED(IDC_SEARCH_REPORT_BUTTON, OnBnClickedSearchReportButton)
	ON_BN_CLICKED(IDC_BUTTON_STRTB_SEARCH, OnBnClickedButtonStrtbSearch)
    ON_BN_CLICKED(IDC_BTN_SQL_SAVE, &CItemEditDlg::OnBnClickedBtnSqlSave)
	ON_BN_CLICKED(IDC_BUTTON_XMLCONVERT, OnBnClickedBtnXmlConvert)
	ON_BN_CLICKED(IDC_BUTTON_EXCEL_EXPORT, OnBnClickedBtnExcelExport)
END_MESSAGE_MAP()


// CItemEditDlg �޽��� ó����

BOOL CItemEditDlg::OnInitDialog()
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

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	MakeToolBar ();	

	CItemEditApp* pApp = (CItemEditApp*) AfxGetApp();

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);

	CString strAppPath = szPath;
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

	//CCommonPreview::GetInstance().Create( NULL );

    GLItemMan::GetInstance().OneTimeSceneInit();

	GMTOOL::Create(
		m_szAppPath,
		pApp->GetSubPath(),
		pApp->GetServiceProvider(),
		NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1) );

	CWnd* pWnd = GetDlgItem( IDC_STATIC_CONTROL );
	CRect SheetRect;
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	LOGFONT logfont = {0};
	CFont* pFont = GetFont();
	if (pFont)
        pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab(SheetRect, logfont, this);
	m_pSheetTab->Create(this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0);
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_pSheetTab->SetActivePage ( ITEMSUITTREEPAGE );

	CComboBox *pComob = (CComboBox*) GetDlgItem(IDC_ITEM_SEARCH_COMBO);
    pComob->AddString( "MID/SID" );
    pComob->AddString( "Item Name" );
    pComob->AddString( "USE DDS" );
    pComob->AddString( "USE CPS" );
    pComob->AddString( "ITEM TYPE" );
    pComob->SetCurSel ( 0 );

	SetReadMode();

    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    factory.Close();

	SetTimer( 0, 1000 / 60, NULL );

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CItemEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CItemEditDlg::OnPaint() 
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

void CItemEditDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch( nIDEvent )
	{
	case 0:
		CRenderDevice::OnLoop();
		break;
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CItemEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CItemEditDlg::PostNcDestroy()
{
	KillTimer( 0 );

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	bool bValid = GLItemMan::GetInstance().ValidData ();
	if (bValid)
        GLItemMan::GetInstance().SaveFile ( "lastedit.isf.bak" );

	// �����ϸ鼭 itemlist.sql ���� �����ϵ���
	GLItemMan::GetInstance().SaveSqlFile();

	SAFE_DELETE ( m_pSheetTab );

	GLItemMan::GetInstance().FinalCleanup();
	GMTOOL::CleanUp();

	GLogicData::GetInstance().ClearData();

	//CCommonPreview::GetInstance().Destroy();

	CRenderDevice::DetachObject( g_pRI );
    CRenderDevice::DestroyObject( g_pRI );
	SAFE_DELETE( g_pRI );

	CRenderDevice::DetachObject( g_pCustomDevice );
    CRenderDevice::DestroyObject( g_pCustomDevice );
	SAFE_DELETE( g_pCustomDevice );

	CRenderDevice::OnDestroy();

	CDialog::PostNcDestroy();
}

void CItemEditDlg::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
 		HRESULT hr;
 		hr = GLItemMan::GetInstance().LoadFile(dlg.GetFileName().GetString());

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CGameTextMan::GetInstance().Clear( CGameTextMan::EM_ITEM_TEXT );
			if ( CGameTextMan::GetInstance().LoadText( RANPARAM::strItemText.GetString(), CGameTextMan::EM_ITEM_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT ) == FALSE )
			{
				MessageBox ( "Failure to load file." );
			}
		}
		else
		{
			// ������ ��Ʈ�����̺��� �ε��Ѵ�. by ���
			GLStringTable::GetInstance().CLEAR();
			if( !GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM ) )
			{
				MessageBox ( "Failure to load file." );
			}
		}


		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to load file." );
		}
        
		m_pSheetTab->m_ItemSuitTreePage.UpdateTree ();
	}
}

bool CItemEditDlg::DoSave ()
{
	CString szFilter = "Item Setting File (*.isf)|*.isf|";
	
	// ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(
        FALSE,
        ".isf",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
        szFilter,
		(CItemEditDlg*) this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if (dlg.DoModal() == IDOK)
	{
		HRESULT hr;
		hr = GLItemMan::GetInstance().SaveFile(dlg.GetFileName().GetString());
		if (FAILED(hr))
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

 		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 		{
 			CGameTextMan::GetInstance().SaveXML(RANPARAM::strItemText.GetString(), CGameTextMan::EM_ITEM_TEXT, TRUE);
 		}
 		else
 		{
			if (!GLStringTable::GetInstance().SAVEFILE(GLStringTable::ITEM))
			{
				MessageBox("Failure to Save File");
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CItemEditDlg::SaveSearchList()
{
	TCHAR szFullPathFileName[MAX_PATH] = {0};
	StringCchCopy( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );

	CString szFilter = "Search Item List (*.txt)|*.txt|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".txt",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		//StringCchCat ( szFullPathFileName, MAX_PATH, GLItemMan::GetInstance()._SEARCHLIST );
		StringCchCat ( szFullPathFileName, MAX_PATH, dlg.GetFileName().GetString() );

		HRESULT hr;
		hr = GLItemMan::GetInstance().SaveSearchList( szFullPathFileName );

		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save SearchItemList File" );
			return false;
		}

		return true;
	}

	return false;
}

void CItemEditDlg::ExportExcel ( boost::function<void (int)> PercentCallbackFunc )
{
	if ( !m_pSheetTab )
		return;

	m_pSheetTab->ExportExcelDetail( PercentCallbackFunc );
}

void CItemEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CItemEditDlg::OnMenuitemExit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	SendMessage ( WM_CLOSE );
}

void CItemEditDlg::OnMenuitemNew()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.	
	GLItemMan::GetInstance().FinalCleanup ();
	GLItemMan::GetInstance().OneTimeSceneInit();

	m_pSheetTab->m_ItemSuitTreePage.CleanAllItem ();
}

void CItemEditDlg::MakeToolBar ()
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

void CItemEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
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

void CItemEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CItemEditDlg::OnBnClickedButtonUpdateSync()
{
    CItemEditApp* pApp = (CItemEditApp*) AfxGetApp();
	GLItemMan::GetInstance().SyncUpdateData(pApp->GetSubPath());

	CLogDialog LogDlg;
	LogDlg.SetLogFile ( GLItemMan::_LOGFILE );
	LogDlg.DoModal ();
}

void CItemEditDlg::OnClose()
{
	if ( GLItemMan::GetInstance().IsModify() )
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

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=ITEMSUITTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}

void CItemEditDlg::OnBnClickedButtonStrtbSync()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GLItemMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CItemEditDlg::OnBnClickedButtonReloadStrtbl()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().Clear( CGameTextMan::EM_ITEM_TEXT );
		CGameTextMan::GetInstance().LoadText( RANPARAM::strItemText.GetString(), CGameTextMan::EM_ITEM_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
		CGameTextMan::GetInstance().LoadText( RANPARAM::strSkillText.GetString(), CGameTextMan::EM_SKILL_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
		CGameTextMan::GetInstance().LoadText( RANPARAM::strCrowText.GetString(), CGameTextMan::EM_CROW_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	}
	else
	{
		// ������ ��Ʈ�����̺��� �ٽ� �ε��Ѵ�. by ���
		GLStringTable::GetInstance().CLEAR();
		GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM );
		GLStringTable::GetInstance().LOADFILE( GLCrowDataMan::GetInstance()._STRINGTABLE, GLStringTable::CROW );
		GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL );
	}
}

void CItemEditDlg::OnBnClickedBtnCsvSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GLItemMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CItemEditDlg::OnBnClickedBtnCsvLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GLItemMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateSuitTreePage();
	
		OnBnClickedButtonReloadStrtbl();
		m_pSheetTab->m_ItemSuitTreePage.UpdateTree ();

		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemEditDlg::OnBnClickedBtnExcelStrTblImport()
{
	CFileDialog dlg( TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return ;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if ( GLItemMan::GetInstance().LoadStringTableExcel(dlg.GetPathName().GetString()) == S_OK )
		{
			m_pSheetTab->m_ItemSuitTreePage.UpdateTree();
			MessageBox( _T("String Table Excel Load Success"), _T("Success"), MB_OK );
		}
	}
}

void CItemEditDlg::OnBnClickedBtnExcelStrTblExport()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSavePath;
		if ( GLItemMan::GetInstance().SaveStringTableExcel(strSavePath) == S_OK )
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

void CItemEditDlg::OnBnClickedItemSearchButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetDlgItem(IDC_ITEM_SEARCH_EDIT)->GetWindowText( szTempChar, 256 );

	if( strlen(szTempChar) == 0 ) 
	{
		MessageBox( "ã�� ���� ��������!!!!!", "�˸�" );
		return;
	}

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "ã����" );

	int nSearchType = (int)GetDlgItem(IDC_ITEM_SEARCH_COMBO)->SendMessage( CB_GETCURSEL, 0, 0 );	
	m_pSheetTab->SearchItemSelect( nSearchType, szTempChar );
	m_wndSearchDlg.InsertReportItem( m_pSheetTab->GetSearchItemList() );

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "ã��" );
}

void CItemEditDlg::OnBnClickedSearchReportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_wndSearchDlg.DoModal() == IDOK )
	{
		m_pSheetTab->SetSelectIndex( m_wndSearchDlg.GetSelectItem() );
	}
}

void CItemEditDlg::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	SetWin_ShowWindow( this, IDC_BTN_CSV_LOAD, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTB_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTB_SEARCH, FALSE ); // by luxes.

	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}

void CItemEditDlg::OnBnClickedButtonStrtbSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( SaveSearchList() )
		MessageBox( _T("SearchItemList Save Success"), _T("Success"), MB_OK );
}

void CItemEditDlg::OnBnClickedBtnSqlSave()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    std::string SaveFileName = GLItemMan::GetInstance().SaveSqlFile();
    MessageBox(SaveFileName.c_str(), "File saved", MB_OK);
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

void CItemEditDlg::OnBnClickedBtnXmlConvert()
{
	std::string strFullPath = sc::getAppPath();
	strFullPath.append("\\ItemStrTable.xml");

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
	for ( i = 0; i < GLItemMan::MAX_MID; ++i )
	{
		for ( int j = 0; j < GLItemMan::MAX_SID; ++j )
		{
			SITEM* sItem = NULL;
			if ( (sItem = GLItemMan::GetInstance().GetItem(i, j)) != NULL )
			{
				CString strElemName;
				strElemName.Format("IN_%03d_%03d", i, j);
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
					if ( sItem->GetName() != NULL && strlen(sItem->GetName()) != 0)
					{
						_tcscpy(szUTF8, sItem->GetName());
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
				strElemName.Format("ID_%03d_%03d", i, j);
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
					if ( sItem->GetComment() != NULL && strlen(sItem->GetComment()) != 0)
					{
						CString strCheck = sItem->GetComment();
						strCheck.Replace(" ", "");
						if ( strCheck.GetLength() == 0 )
							_tcscpy(szUTF8, "skip");
						else
							_tcscpy(szUTF8, sItem->GetComment());
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

void CItemEditDlg::OnBnClickedBtnExcelExport()
{
	// Excel Export Dialog ȣ��;
	DlgExcelDetailExport Dlg;
	Dlg.DoModal();

	/*m_pSheetTab->ExportExcelDetail(
		boost::bind(
		&DlgExcelDetailExport::SetProgressPercentage, &Dlg, _1 ) );*/
}