// SpecialToolDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Product/GLProductRecipeMan.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../MfcExLib/Splasher.h"   //���÷���

#include "SheetWithTab.h"
#include "SpecialTool.h"
#include "SpecialToolDlg.h"

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

// CSpecialToolDlg ��ȭ ����

CSpecialToolDlg::CSpecialToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialToolDlg::IDD, pParent)
	, m_pD3D(NULL)
	, m_pD3DDevice(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPage = 0;
    CSpecialToolApp* pApp = (CSpecialToolApp*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CSpecialToolDlg::~CSpecialToolDlg()
{
	if( m_pD3DDevice != NULL )
		m_pD3DDevice->Release();

	if( m_pD3D != NULL )
		m_pD3D->Release();

    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

void CSpecialToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TOOLPAGE, m_cComboPage);
}

BEGIN_MESSAGE_MAP(CSpecialToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_OLDLOAD, OnBnClickedButtonOldload)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_TOOLPAGE, OnCbnSelchangeComboToolpage)
	ON_COMMAND(ID_FILE_NEWFILE, OnFileNewfile)
	ON_COMMAND(ID_FILE_LOADFILE, OnFileLoadfile)
	ON_COMMAND(ID_FILE_LOADFILE_OLD, OnFileLoadfileOld)
	ON_COMMAND(ID_FILE_SAVEFILE, OnFileSavefile)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_BN_CLICKED(IDC_BUTTON_CSVLOAD, OnBnClickedButtonCsvload)
	ON_BN_CLICKED(IDC_BUTTON_CSVSAVE, OnBnClickedButtonCsvsave)
END_MESSAGE_MAP()


// CSpecialToolDlg �޽��� ó����

BOOL CSpecialToolDlg::OnInitDialog()
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

	MakeToolBar();

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

    GLItemMan::GetInstance().OneTimeSceneInit();

	//////////////////////////////////////////////////////////////////////////
	// Effect Load���� Device���� NULL�̶�� Load�� �����ʱ⶧����;
	// �ӽ÷� Device�� �����ؼ� �Ѱ��ش�;
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return FALSE;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, AfxGetMainWnd()->m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_pD3DDevice ) ) )
		return FALSE;

	m_pEngineDevice->InitDeviceObjects(m_pD3DDevice);
	//////////////////////////////////////////////////////////////////////////

    CSpecialToolApp* pApp = (CSpecialToolApp*) AfxGetApp();
	//GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider());
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));


	CRect SheetRect;
	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	LOGFONT logfont = {0};
	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );


	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetPage ( m_nPage );

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	InitCombo();
	UpdateItems();

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    factory.Close();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

LPDIRECT3DDEVICEQ CSpecialToolDlg::GetD3DDevice() 
{
	return m_pEngineDevice->GetDevice(); 
}

void CSpecialToolDlg::MakeToolBar()
{
	if( !m_wndToolBar.Create(this) || 
        !m_wndToolBar.LoadToolBar(IDR_MAIN_MENU) )
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

void CSpecialToolDlg::InitCombo()
{
	SetWin_Combo_Init( this, IDC_COMBO_TOOLPAGE, g_strEditSheet, PAGE_NUM );
}

void CSpecialToolDlg::UpdateItems()
{
	SetWin_Text( this, IDC_EDIT_TOOLTEXT, g_strEditSheet[m_nPage].c_str() );
}

void CSpecialToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpecialToolDlg::OnPaint() 
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
HCURSOR CSpecialToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSpecialToolDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// �����ϸ鼭 ProductRecipeList.sql ���� �����ϵ��� �Ѵ�;
	GLProductRecipeMan::GetInstance().SaveSQLFile();

	GLItemMan::GetInstance().FinalCleanup ();
	m_pEngineDevice->FinalCleanup ();
	GMTOOL::CleanUp ();

	GLogicData::GetInstance().ClearData();

	CDialog::PostNcDestroy();
}

void CSpecialToolDlg::OnBnClickedButtonNew()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->NewFile( m_nPage );
}

void CSpecialToolDlg::OnBnClickedButtonLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->LoadFile( m_nPage );
}

void CSpecialToolDlg::OnBnClickedButtonOldload()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->LoadFileOld( m_nPage );
}

void CSpecialToolDlg::OnBnClickedButtonSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SaveFile( m_nPage );
}

bool CSpecialToolDlg::DoSave()
{
	if ( m_pSheetTab->IsPageEdit( m_nPage ) )
	{
		int nret = MessageBox ( "Press the 'YES' Button to Save.", "Caution", MB_YESNOCANCEL );
		switch ( nret )
		{
		case IDYES:
			if ( !m_pSheetTab->SaveFile( m_nPage ) )	return false;	//	���нÿ��� �������� ����.
			break;

		case IDNO:
			//	�������� �ʰ� ����.
			break;

		case IDCANCEL:
			//	��ҽÿ��� �������� ����.
			return false;
		};
	}

	return true;
}

void CSpecialToolDlg::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if ( !DoSave() ) 
		return;

	CDialog::OnClose();
}

void CSpecialToolDlg::OnCbnSelchangeComboToolpage()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_cComboPage.GetCurSel();
	if ( nIndex == -1 ) return;
	if ( nIndex == m_nPage ) return;

	if ( !DoSave() )
	{
		m_cComboPage.SetCurSel( m_nPage );
		return;
	}

	m_nPage = nIndex;
	m_pSheetTab->SetPage( m_nPage );
	UpdateItems();
}

void CSpecialToolDlg::OnFileNewfile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonNew();
}

void CSpecialToolDlg::OnFileLoadfile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonLoad();
}

void CSpecialToolDlg::OnFileLoadfileOld()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonOldload();
}

void CSpecialToolDlg::OnFileSavefile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonSave();
}

void CSpecialToolDlg::OnFileExit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	SendMessage ( WM_CLOSE );
}

void CSpecialToolDlg::OnBnClickedButtonCsvload()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->CsvLoad( m_nPage );
}

void CSpecialToolDlg::OnBnClickedButtonCsvsave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->CsvSave( m_nPage );
}
