// SpecialToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../enginelib/G-Logic/GLogic.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Item/GLItemMan.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../MfcExLib/Splasher.h"   //스플래쉬

#include "SheetWithTab.h"

#include "./GenItemTool.h"
#include "./GenItemRevise.h"
#include "./GenItemToolDialogue.h"
#include "./ExcelExportGenItem.h"
#include "./DataSelect.h"

#include "../RanLogic/GenItemTable/GenItemManager.h"

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

// CSpecialToolDlg 대화 상자

CGenItemToolDilogue::CGenItemToolDilogue(CWnd* pParent /*=NULL*/)
	: CDialog(CGenItemToolDilogue::IDD, pParent)
	, m_pD3D(NULL)
	, m_pD3DDevice(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPage = 0;
    CGenItemToolApplication* pApp = (CGenItemToolApplication*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CGenItemToolDilogue::~CGenItemToolDilogue()
{
	if( m_pD3DDevice != NULL )
		m_pD3DDevice->Release();

	if( m_pD3D != NULL )
		m_pD3D->Release();

    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

void CGenItemToolDilogue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
}

BEGIN_MESSAGE_MAP(CGenItemToolDilogue, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)	
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_TO_CSV, OnBnClickedButtonCsvsave)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOSTDATA, &CGenItemToolDilogue::OnBnClickedButtonLinklostdata)
	ON_BN_CLICKED(IDC_BUTTON_SELEC_RELOAD, &CGenItemToolDilogue::OnBnClickedButtonSelecReload)
	ON_BN_CLICKED(IDC_BUTTON_FILEEDIT, &CGenItemToolDilogue::OnBnClickedButtonFileedit)
END_MESSAGE_MAP()


// CSpecialToolDlg 메시지 처리기

BOOL CGenItemToolDilogue::OnInitDialog()
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

	// 툴바의 기능을 이용 하려면 아래 주석을 해제;
//	MakeToolBar();

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
	// Effect Load에서 Device값이 NULL이라면 Load를 하지않기때문에;
	// 임시로 Device를 생성해서 넘겨준다;
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

    CGenItemToolApplication* pApp = (CGenItemToolApplication*) AfxGetApp();
	//GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider());
    // 스플래쉬 화면을 출력하기위해 추가되었음
	GLOGIC::SetGlogicZipFile(TRUE);
    GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

	GenItem::GenItemManager::GetInstance().LoadData(GenItem::ALL_DATA);

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
	

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
    // 스플래쉬 화면을 출력하기위해 추가되었음
    factory.Close();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

LPDIRECT3DDEVICEQ CGenItemToolDilogue::GetD3DDevice() 
{
	return m_pEngineDevice->GetDevice(); 
}


void CGenItemToolDilogue::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGenItemToolDilogue::OnPaint() 
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
		int x = (rect.Width() - cxIcon  - 1) / 2;
		int y = (rect.Height() - cyIcon  - 1) / 2;

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
HCURSOR CGenItemToolDilogue::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGenItemToolDilogue::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GLItemMan::GetInstance().FinalCleanup ();
	m_pEngineDevice->FinalCleanup ();
	GMTOOL::CleanUp ();

	GLogicData::GetInstance().ClearData();

	CDialog::PostNcDestroy();
}



bool CGenItemToolDilogue::DoSave()
{	

	return true;
}

void CGenItemToolDilogue::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( !DoSave() ) 
		return;

	CDialog::OnClose();
}

void CGenItemToolDilogue::OnFileExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SendMessage ( WM_CLOSE );
}

void CGenItemToolDilogue::OnBnClickedButtonCsvload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CGenItemToolDilogue::OnBnClickedButtonCsvsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GenItem::GenItemManager::GetInstance().SaveExcelFile(this);	
	MessageBox("Save Complete", "Excel Export", MB_OK);
}

void CGenItemToolDilogue::OnBnClickedButtonLinklostdata()
{
	if(m_LinkLostData.GetSafeHwnd() == NULL)
	{
		m_LinkLostData.Create(IDD_DIALOG_LINKLOST);
	}

	m_LinkLostData.ShowWindow(SW_SHOW);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CGenItemToolDilogue::OnBnClickedButtonSelecReload()
{	
	GenItem::GenItemManager::GetInstance().LoadSelectFile();
}

void CGenItemToolDilogue::OnBnClickedButtonFileedit()
{
	DataSelect Dlg;
	Dlg.SetCase(GenItem::GENITEMFILE_NAME_WORD);	
	Dlg.DoModal();

	std::string _CompleteWord = Dlg.GetCompleteStr();

	if(_CompleteWord == "" || _CompleteWord == ".genitem")
		return;
	
	char* _StrTemp = const_cast<char*>(_CompleteWord.c_str());
	char* _ItemName(NULL);
	_ItemName = strtok(_StrTemp, ";");	
	_CompleteWord = _ItemName;	

	GenItemRevise::GetInstance().Clear();
	GenItemRevise::GetInstance().SetFileName(_CompleteWord);

	if(GenItemRevise::GetInstance().GetSafeHwnd() == NULL)
	{
		GenItemRevise::GetInstance().Create(IDD_DIALOG_REVISE);
	}	
	else
	{
		GenItemRevise::GetInstance().DestroyWindow();
		GenItemRevise::GetInstance().Create(IDD_DIALOG_REVISE);
	}
	GenItemRevise::GetInstance().ShowWindow(SW_SHOW);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
