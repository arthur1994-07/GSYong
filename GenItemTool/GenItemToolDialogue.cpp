// SpecialToolDlg.cpp : ���� ����
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
#include "../MfcExLib/Splasher.h"   //���÷���

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


// CSpecialToolDlg �޽��� ó����

BOOL CGenItemToolDilogue::OnInitDialog()
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

	// ������ ����� �̿� �Ϸ��� �Ʒ� �ּ��� ����;
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

    CGenItemToolApplication* pApp = (CGenItemToolApplication*) AfxGetApp();
	//GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider());
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
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
	

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
    // ���÷��� ȭ���� ����ϱ����� �߰��Ǿ���
    factory.Close();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CGenItemToolDilogue::OnPaint() 
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
		int x = (rect.Width() - cxIcon  - 1) / 2;
		int y = (rect.Height() - cyIcon  - 1) / 2;

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
HCURSOR CGenItemToolDilogue::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGenItemToolDilogue::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if ( !DoSave() ) 
		return;

	CDialog::OnClose();
}

void CGenItemToolDilogue::OnFileExit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	SendMessage ( WM_CLOSE );
}

void CGenItemToolDilogue::OnBnClickedButtonCsvload()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CGenItemToolDilogue::OnBnClickedButtonCsvsave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
