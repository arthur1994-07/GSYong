// AdoTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <string>
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Database/Ado/AdoClass.h"
#include "../SigmaCore/Util/DateTime.h"

#include "AdoTest.h"
#include "AdoTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// AdoTestDlg dialog
AdoTestDlg::AdoTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AdoTestDlg::IDD, pParent)
    , m_ServerIP(_T(""))
    , m_ServerPort(_T(""))
    , m_ServerID(_T(""))
    , m_ServerPassword(_T(""))
    , m_DatabaseName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    sc::LogMan::getInstance();
}

void AdoTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SERVER_NAME, m_ServerIP);
    DDX_Text(pDX, IDC_EDIT_PORT, m_ServerPort);
    DDX_Text(pDX, IDC_EDIT_ID, m_ServerID);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_ServerPassword);
    DDX_Control(pDX, IDC_EDIT_LOG, m_EditLog);
    DDX_Control(pDX, IDC_COMBO_PROVIDER, m_Provider);
    DDX_Text(pDX, IDC_EDIT_DATABASE_NAME, m_DatabaseName);
}

BEGIN_MESSAGE_MAP(AdoTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, &AdoTestDlg::OnBnClickedButtonConnect)
    ON_EN_MAXTEXT(IDC_EDIT_LOG, &AdoTestDlg::OnEnMaxtextEditLog)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_TEST1, &AdoTestDlg::OnBnClickedButtonTest1)
    ON_BN_CLICKED(IDC_BUTTON_TEST2, &AdoTestDlg::OnBnClickedButtonTest2)
    ON_BN_CLICKED(IDC_BUTTON_TEST3, &AdoTestDlg::OnBnClickedButtonTest3)
    ON_BN_CLICKED(IDC_BUTTON_TEST4, &AdoTestDlg::OnBnClickedButtonTest4)
    ON_BN_CLICKED(IDC_BUTTON_TEST5, &AdoTestDlg::OnBnClickedButtonTest5)
    ON_BN_CLICKED(IDC_BUTTON_TEST6, &AdoTestDlg::OnBnClickedButtonTest6)
    ON_BN_CLICKED(IDC_BUTTON_LOG_ITEM_EXCHANGE_INSERT_NEW, &AdoTestDlg::OnBnClickedButtonLogItemExchangeInsertNew)
END_MESSAGE_MAP()

std::string Provider[] = {
    "SQLOLEDB.1",
    "SQLOLEDB",
    "SQLNCLI",
    "SQLNCLI10" };

// AdoTestDlg message handlers

BOOL AdoTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    // korea office test
    //m_ServerIP = "192.168.100.27";
    //m_ServerPort = "1433";
    //m_ServerID = "RanLog";
    //m_ServerPassword = "RanLog@@";
    //m_DatabaseName = "RanLog";

    m_ServerIP = "192.168.100.27";
    m_ServerPort = "1433";
    m_ServerID = "RanLog";
    m_ServerPassword = "RanLog@@";
    m_DatabaseName = "RanLogS3";

    for (size_t i=0; i<_countof(Provider); ++i)
    {
        m_Provider.AddString(Provider[i].c_str());
    }
    m_Provider.SetCurSel(0);
    UpdateData(FALSE);

    sc::LogMan::getInstance()->attachEditControl(m_EditLog.m_hWnd, m_EditLog.m_hWnd);
    m_EditLog.SetLimitText(1024*1024);

    if (FAILED(::CoInitialize(NULL)))
	{	
        AfxMessageBox("::CoInitialized() Failure.", MB_OK);
	}
    else
    {
        AfxMessageBox("::CoInitialized() success.", MB_OK);
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void AdoTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void AdoTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR AdoTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void AdoTestDlg::OnBnClickedButtonConnect()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);

    std::string PrividerName("SQLOLEDB.1");

    int nIndex = m_Provider.GetCurSel();
    int nCount = m_Provider.GetCount();
    if ((nIndex != CB_ERR) && (nCount > 1))
    {
        PrividerName = Provider[nIndex];
    }

    std::string DbConnString(
        sc::string::format(
        "Provider=%1%;Data Source=%2%,%3%;Initial Catalog=%4%;User ID=%5%;Password=%6%;Persist Security Info=True",
        PrividerName,            
        m_ServerIP.GetString(),
        m_ServerPort.GetString(),
        m_DatabaseName.GetString(),
        m_ServerID.GetString(),
        m_ServerPassword.GetString()));

    sc::writeLogInfo(DbConnString);

    m_ConnectionString = DbConnString;
    
    sc::db::CjADO Ado;
    if (Ado.Open(DbConnString))
    {
        std::string Msg("Connection success");
        AfxMessageBox(Msg.c_str(), MB_OK);
        Msg = sc::string::format("Ado Product Version %1%", Ado.GetProductVersion());
        AfxMessageBox(Msg.c_str(), MB_OK);
    }
    else
    {
        std::string Msg(
            sc::string::format(
                "Connection Failed. %1%",
                m_ConnectionString));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }
}

void AdoTestDlg::OnEnMaxtextEditLog()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.    
    m_EditLog.SetWindowText("");
}

void AdoTestDlg::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    sc::LogMan::releaseInstance();
    CDialog::OnClose();
    ::CoUninitialize();
}

void AdoTestDlg::OnBnClickedButtonTest1()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.    
    int m_nCHARID = 1;
    int m_ActionType = 2;
    int m_TargetType = 3;
    int m_TargetId = 4;
    __int64 m_nEXP = 5;
    int m_nBRIGHT_POINT = 6;
    int m_nLIFE_POINT = 7;
    int m_nMONEY = 8;

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_IPARAM_INT  (Ado, "@nChaNum",      m_nCHARID);
    APPEND_IPARAM_INT  (Ado, "@nType",        m_ActionType);
    APPEND_IPARAM_INT  (Ado, "@nTargetNum",   m_TargetId);
    APPEND_IPARAM_INT  (Ado, "@nTargetType",  m_TargetType);
    APPEND_IPARAM_MONEY(Ado, "@nExpPoint",    m_nEXP);
    APPEND_IPARAM_INT  (Ado, "@nBrightPoint", m_nBRIGHT_POINT);
    APPEND_IPARAM_INT  (Ado, "@nLifePoint",   m_nLIFE_POINT);
    APPEND_IPARAM_MONEY(Ado, "@nMoney",       m_nMONEY);

    if (!Ado.Execute4Cmd("dbo.sp_LogAction_Insert", adCmdStoredProc))
        AfxMessageBox("Test #1 failed", MB_OK);
    else
        AfxMessageBox("Test #1 success", MB_OK);
}

void AdoTestDlg::OnBnClickedButtonTest2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int m_nCHARID = 1;
    int m_ActionType = 2;
    int m_TargetType = 3;
    int m_TargetId = 4;
    __int64 m_nEXP = 5;
    int m_nBRIGHT_POINT = 6;
    int m_nLIFE_POINT = 7;
    __int64 m_nMONEY = 8;

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_IPARAM_INT  (Ado, "@nChaNum",      m_nCHARID);
    APPEND_IPARAM_INT  (Ado, "@nType",        m_ActionType);
    APPEND_IPARAM_INT  (Ado, "@nTargetNum",   m_TargetId);
    APPEND_IPARAM_INT  (Ado, "@nTargetType",  m_TargetType);
    APPEND_IPARAM_MONEY(Ado, "@nExpPoint",    m_nEXP);
    APPEND_IPARAM_INT  (Ado, "@nBrightPoint", m_nBRIGHT_POINT);
    APPEND_IPARAM_INT  (Ado, "@nLifePoint",   m_nLIFE_POINT);
    APPEND_IPARAM_MONEY(Ado, "@nMoney",       m_nMONEY);

    if (!Ado.Execute4Cmd("dbo.sp_LogAction_Insert", adCmdStoredProc))
        AfxMessageBox("Test #2 failed", MB_OK);
    else
        AfxMessageBox("Test #2 success", MB_OK);
}

void AdoTestDlg::OnBnClickedButtonTest3()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int m_nCHARID = 1;
    int m_ActionType = 2;
    int m_TargetType = 3;
    int m_TargetId = 4;
    __int64 m_nEXP = 5;
    int m_nBRIGHT_POINT = 6;
    int m_nLIFE_POINT = 7;
    int m_nMONEY = 8;

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_IPARAM_INT  (Ado, "@nChaNum",      m_nCHARID);
    APPEND_IPARAM_INT  (Ado, "@nType",        m_ActionType);
    APPEND_IPARAM_INT  (Ado, "@nTargetNum",   m_TargetId);
    APPEND_IPARAM_INT  (Ado, "@nTargetType",  m_TargetType);
    APPEND_IPARAM_MONEY(Ado, "@nExpPoint",    m_nEXP);
    APPEND_IPARAM_INT  (Ado, "@nBrightPoint", m_nBRIGHT_POINT);
    APPEND_IPARAM_INT  (Ado, "@nLifePoint",   m_nLIFE_POINT);
    APPEND_IPARAM_MONEY(Ado, "@nMoney",       (__int64) m_nMONEY);

    if (!Ado.Execute4Cmd("dbo.sp_LogAction_Insert", adCmdStoredProc))
        AfxMessageBox("Test #3 failed", MB_OK);
    else
        AfxMessageBox("Test #3 success", MB_OK);
}

void AdoTestDlg::OnBnClickedButtonTest4()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int	Mid = 1; // 주번호
    int Sid = 2; // 부번호
    int ServerGroup = 3; // 서버그룹
    int ServerNum = 4; // 서버번호
    int FieldNum = 5; // 필드번호
    int	MakeType = 6; // 생성타입
    LONGLONG lnMakeNum = 7; // 생성번호
    int FromType = 8; // from의 종류 ( 유저, 케릭터, 클럽 )
    int ItemFrom = 9; // 아이템 출발지
    int ToType = 10;  // to의 종류 ( 유저, 케릭터, 클럽 )
    int ItemTo = 11; // 아이템 목적지
    int ExchangeFlag = 12; // 교환 타입
    int CountNum = 13; // 개수    
    int CostumeMid = 14; // 코스툼 변환 정보
    int CostumeSid = 15; // 코스툼 변환 정보    
    short Damage = 16; // 연마 정보
    short Defense = 17; // 연마 정보
    short Fire = 18; // 연마 정보
    short Ice = 19; // 연마 정보
    short Poison = 20; // 연마 정보
    short Electric = 21; // 연마 정보
    short Spirit = 22; // 연마 정보
    LONGLONG TradePrice = 23; // 교환가격

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT  (Ado, "@NIDMain", Mid);
    APPEND_IPARAM_INT  (Ado, "@NIDSub", Sid);
    APPEND_IPARAM_INT  (Ado, "@SGNum", ServerGroup);
    APPEND_IPARAM_INT  (Ado, "@SvrNum", ServerNum);
    APPEND_IPARAM_INT  (Ado, "@FldNum", FieldNum);

    APPEND_IPARAM_INT  (Ado, "@MakeType", MakeType);
    APPEND_IPARAM_MONEY(Ado, "@MakeNum", lnMakeNum);
    APPEND_IPARAM_INT  (Ado, "@ItemAmount", CountNum);
    APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", FromType);
    APPEND_IPARAM_INT  (Ado, "@ItemFrom", ItemFrom);

    APPEND_IPARAM_INT  (Ado, "@ItemToFlag", ToType);
    APPEND_IPARAM_INT  (Ado, "@ItemTo", ItemTo);
    APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", ExchangeFlag);
    APPEND_IPARAM_INT  (Ado, "@Damage", Damage);
    APPEND_IPARAM_INT  (Ado, "@Defense", Defense);

    APPEND_IPARAM_INT  (Ado, "@Fire", Fire);
    APPEND_IPARAM_INT  (Ado, "@Ice", Ice);
    APPEND_IPARAM_INT  (Ado, "@Poison", Poison);
    APPEND_IPARAM_INT  (Ado, "@Electric", Electric);
    APPEND_IPARAM_INT  (Ado, "@Spirit", Spirit);

    APPEND_IPARAM_INT  (Ado, "@CostumeMID", CostumeMid);
    APPEND_IPARAM_INT  (Ado, "@CostumeSID", CostumeSid);

    APPEND_IPARAM_MONEY(Ado, "@TradePrice", TradePrice);

    APPEND_OPARAM_INT  (Ado, "@nReturn");

    if (!Ado.Execute4Cmd("dbo.sp_logitemexchange_insert", adCmdStoredProc))
        AfxMessageBox("Test #4 failed", MB_OK);
    else
        AfxMessageBox("Test #4 success", MB_OK);

    _variant_t varReturn;
    if (Ado.GetParam("@nReturn", varReturn))
    {
        std::string Msg(
            sc::string::format(
                "Test #4 result %1%", varReturn.intVal));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }
}

void AdoTestDlg::OnBnClickedButtonTest5()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int	Mid = 1; // 주번호
    int Sid = 2; // 부번호
    int ServerGroup = 3; // 서버그룹
    int ServerNum = 4; // 서버번호
    int FieldNum = 5; // 필드번호
    int	MakeType = 6; // 생성타입
    LONGLONG lnMakeNum = 7; // 생성번호
    int FromType = 8; // from의 종류 ( 유저, 케릭터, 클럽 )
    int ItemFrom = 9; // 아이템 출발지
    int ToType = 10;  // to의 종류 ( 유저, 케릭터, 클럽 )
    int ItemTo = 11; // 아이템 목적지
    int ExchangeFlag = 12; // 교환 타입
    int CountNum = 13; // 개수    
    int CostumeMid = 14; // 코스툼 변환 정보
    int CostumeSid = 15; // 코스툼 변환 정보    
    int Damage = 16; // 연마 정보
    int Defense = 17; // 연마 정보
    int Fire = 18; // 연마 정보
    int Ice = 19; // 연마 정보
    int Poison = 20; // 연마 정보
    int Electric = 21; // 연마 정보
    int Spirit = 22; // 연마 정보
    LONGLONG TradePrice = 23; // 교환가격

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT  (Ado, "@NIDMain", Mid);
    APPEND_IPARAM_INT  (Ado, "@NIDSub", Sid);
    APPEND_IPARAM_INT  (Ado, "@SGNum", ServerGroup);
    APPEND_IPARAM_INT  (Ado, "@SvrNum", ServerNum);
    APPEND_IPARAM_INT  (Ado, "@FldNum", FieldNum);

    APPEND_IPARAM_INT  (Ado, "@MakeType", MakeType);
    APPEND_IPARAM_MONEY(Ado, "@MakeNum", lnMakeNum);
    APPEND_IPARAM_INT  (Ado, "@ItemAmount", CountNum);
    APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", FromType);
    APPEND_IPARAM_INT  (Ado, "@ItemFrom", ItemFrom);

    APPEND_IPARAM_INT  (Ado, "@ItemToFlag", ToType);
    APPEND_IPARAM_INT  (Ado, "@ItemTo", ItemTo);
    APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", ExchangeFlag);
    APPEND_IPARAM_INT  (Ado, "@Damage", Damage);
    APPEND_IPARAM_INT  (Ado, "@Defense", Defense);

    APPEND_IPARAM_INT  (Ado, "@Fire", Fire);
    APPEND_IPARAM_INT  (Ado, "@Ice", Ice);
    APPEND_IPARAM_INT  (Ado, "@Poison", Poison);
    APPEND_IPARAM_INT  (Ado, "@Electric", Electric);
    APPEND_IPARAM_INT  (Ado, "@Spirit", Spirit);

    APPEND_IPARAM_INT  (Ado, "@CostumeMID", CostumeMid);
    APPEND_IPARAM_INT  (Ado, "@CostumeSID", CostumeSid);

    APPEND_IPARAM_MONEY(Ado, "@TradePrice", TradePrice);

    APPEND_OPARAM_INT  (Ado, "@nReturn");

    if (!Ado.Execute4Cmd("dbo.sp_logitemexchange_insert", adCmdStoredProc))
        AfxMessageBox("Test #5 failed", MB_OK);
    else
        AfxMessageBox("Test #5 success", MB_OK);

    _variant_t varReturn;
    if (Ado.GetParam("@nReturn", varReturn))
    {
        std::string Msg(
            sc::string::format(
                "Test #5 result %1%", varReturn.intVal));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }
}

void AdoTestDlg::OnBnClickedButtonTest6()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int	Mid = 1; // 주번호
    int Sid = 2; // 부번호
    int ServerGroup = 3; // 서버그룹
    int ServerNum = 4; // 서버번호
    int FieldNum = 5; // 필드번호
    int	MakeType = 6; // 생성타입
    LONGLONG lnMakeNum = 7; // 생성번호
    int FromType = 8; // from의 종류 ( 유저, 케릭터, 클럽 )
    int ItemFrom = 9; // 아이템 출발지
    int ToType = 10;  // to의 종류 ( 유저, 케릭터, 클럽 )
    int ItemTo = 11; // 아이템 목적지
    int ExchangeFlag = 12; // 교환 타입
    int CountNum = 13; // 개수    
    int CostumeMid = 14; // 코스툼 변환 정보
    int CostumeSid = 15; // 코스툼 변환 정보    
    short Damage = 16; // 연마 정보
    short Defense = 17; // 연마 정보
    short Fire = 18; // 연마 정보
    short Ice = 19; // 연마 정보
    short Poison = 20; // 연마 정보
    short Electric = 21; // 연마 정보
    short Spirit = 22; // 연마 정보
    LONGLONG TradePrice = 23; // 교환가격

    sc::db::CjADO Ado(m_ConnectionString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT  (Ado, "@NIDMain", Mid);
    APPEND_IPARAM_INT  (Ado, "@NIDSub", Sid);
    APPEND_IPARAM_INT  (Ado, "@SGNum", ServerGroup);
    APPEND_IPARAM_INT  (Ado, "@SvrNum", ServerNum);
    APPEND_IPARAM_INT  (Ado, "@FldNum", FieldNum);

    APPEND_IPARAM_INT  (Ado, "@MakeType", MakeType);
    APPEND_IPARAM_MONEY(Ado, "@MakeNum", lnMakeNum);
    APPEND_IPARAM_INT  (Ado, "@ItemAmount", CountNum);
    APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", FromType);
    APPEND_IPARAM_INT  (Ado, "@ItemFrom", ItemFrom);

    APPEND_IPARAM_INT  (Ado, "@ItemToFlag", ToType);
    APPEND_IPARAM_INT  (Ado, "@ItemTo", ItemTo);
    APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", ExchangeFlag);
    APPEND_IPARAM_INT  (Ado, "@Damage", (int) Damage);
    APPEND_IPARAM_INT  (Ado, "@Defense", (int) Defense);

    APPEND_IPARAM_INT  (Ado, "@Fire", (int) Fire);
    APPEND_IPARAM_INT  (Ado, "@Ice", (int) Ice);
    APPEND_IPARAM_INT  (Ado, "@Poison", (int) Poison);
    APPEND_IPARAM_INT  (Ado, "@Electric", (int) Electric);
    APPEND_IPARAM_INT  (Ado, "@Spirit", (int) Spirit);

    APPEND_IPARAM_INT  (Ado, "@CostumeMID", CostumeMid);
    APPEND_IPARAM_INT  (Ado, "@CostumeSID", CostumeSid);

    APPEND_IPARAM_MONEY(Ado, "@TradePrice", TradePrice);

    APPEND_OPARAM_INT  (Ado, "@nReturn");

    if (!Ado.Execute4Cmd("dbo.sp_logitemexchange_insert", adCmdStoredProc))
        AfxMessageBox("Test #6 failed", MB_OK);
    else
        AfxMessageBox("Test #6 success", MB_OK);

    _variant_t varReturn;
    if (Ado.GetParam("@nReturn", varReturn))
    {
        std::string Msg(
            sc::string::format(
                "Test #6 result %1%", varReturn.intVal));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }
}

void AdoTestDlg::OnBnClickedButtonLogItemExchangeInsertNew()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    try
    {
        int	Mid = 1; // 주번호
        int Sid = 2; // 부번호
        int ServerGroup = 3; // 서버그룹
        int ServerNum = 4; // 서버번호
        int FieldNum = 5; // 필드번호
        int	MakeType = 6; // 생성타입
        LONGLONG lnMakeNum = 7; // 생성번호
        int FromType = 8; // from의 종류 ( 유저, 케릭터, 클럽 )
        int ItemFrom = 9; // 아이템 출발지
        int ToType = 10;  // to의 종류 ( 유저, 케릭터, 클럽 )
        int ItemTo = 11; // 아이템 목적지
        int ExchangeFlag = 12; // 교환 타입
        int CountNum = 13; // 개수    
        int CostumeMid = 14; // 코스툼 변환 정보
        int CostumeSid = 15; // 코스툼 변환 정보    
        short Damage = 16; // 연마 정보
        short Defense = 17; // 연마 정보
        short Fire = 18; // 연마 정보
        short Ice = 19; // 연마 정보
        short Poison = 20; // 연마 정보
        short Electric = 21; // 연마 정보
        short Spirit = 22; // 연마 정보
        LONGLONG TradePrice = 23; // 교환가격

        sc::db::CjADO Ado(m_ConnectionString);
        //APPEND_RPARAM_INT(Ado);

        APPEND_IPARAM_INT  (Ado, "@NIDMain", Mid);
        APPEND_IPARAM_INT  (Ado, "@NIDSub", Sid);
        APPEND_IPARAM_INT  (Ado, "@SGNum", ServerGroup);
        APPEND_IPARAM_INT  (Ado, "@SvrNum", ServerNum);
        APPEND_IPARAM_INT  (Ado, "@FldNum", FieldNum);

        APPEND_IPARAM_INT  (Ado, "@MakeType", MakeType);
        APPEND_IPARAM_MONEY(Ado, "@MakeNum", lnMakeNum);
        APPEND_IPARAM_INT  (Ado, "@ItemAmount", CountNum);
        APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", FromType);
        APPEND_IPARAM_INT  (Ado, "@ItemFrom", ItemFrom);

        APPEND_IPARAM_INT  (Ado, "@ItemToFlag", ToType);
        APPEND_IPARAM_INT  (Ado, "@ItemTo", ItemTo);
        APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", ExchangeFlag);
        APPEND_IPARAM_INT  (Ado, "@Damage", (int) Damage);
        APPEND_IPARAM_INT  (Ado, "@Defense", (int) Defense);

        APPEND_IPARAM_INT  (Ado, "@Fire", (int) Fire);
        APPEND_IPARAM_INT  (Ado, "@Ice", (int) Ice);
        APPEND_IPARAM_INT  (Ado, "@Poison", (int) Poison);
        APPEND_IPARAM_INT  (Ado, "@Electric", (int) Electric);
        APPEND_IPARAM_INT  (Ado, "@Spirit", (int) Spirit);

        APPEND_IPARAM_INT  (Ado, "@CostumeMID", CostumeMid);
        APPEND_IPARAM_INT  (Ado, "@CostumeSID", CostumeSid);

        APPEND_IPARAM_MONEY(Ado, "@TradePrice", TradePrice);

        std::string strDate = sc::time::GetCurrentTimeStr();
        APPEND_IPARAM_VARCHAR(Ado, "@ExchangeDate", strDate.c_str(), strDate.length());

        if (!Ado.Execute4Cmd("dbo.LogItemExchangeInsertNew", adCmdStoredProc))
            AfxMessageBox("LogItemExchangeInsertNew failed", MB_OK);
        else
            AfxMessageBox("LogItemExchangeInsertNew success", MB_OK);
    }
    catch(_com_error &e)
    {        
        _bstr_t bstrDescription(e.Description());      
        CharToOem(bstrDescription, bstrDescription);
        AfxMessageBox(bstrDescription, MB_OK);
    }
}
