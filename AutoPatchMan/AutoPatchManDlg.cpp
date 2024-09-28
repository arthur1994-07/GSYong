// AutoPatchManDlg.cpp : ���� ����
//
#include "stdafx.h"
#include <afxinet.h>
#include <process.h>

#include <tlhelp32.h>   // CreateToolhelp32Snapshot() �� ����ϱ� ���� �ش�����
#include <Psapi.h>
#include <Msi.h>	// MsiQueryProductState

#include "../SigmaCore/Net/NetUtil.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "../MfcExLib/RegFunction.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"

#include "../RanLogicClient/Network/s_CPatch.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "GlobalVariable.h"
#include "CheckSystem.h"
#include "LogControl.h"
#include "DlgWarningWhql.h"
#include "DlgDownloadArea.h"
#include "VtcGuardLauncher.h"
#include "DlgVtcGuardUpdate.h"
#include "LauncherText.h"

#ifndef USE_MFC7_HTMLVIEW_FEATURES
	#include "Custsite.h"
	#include ".\autopatchmandlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT CAutoPatchManDlg::nLOGIN_CONNECT_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nCHECK_VERSION_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nFTP_CONNECT_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nFORCE_TERMINATE_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::n500_MILISEC = 500;
const UINT CAutoPatchManDlg::n1000_MILISEC = 1000;
const UINT CAutoPatchManDlg::nWEB_BROWSER_START = 1000;

const UINT WM_CUSTOM_CONTROLSITE_MSG  = RegisterWindowMessage(_T("LauncherMsg"));

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	virtual BOOL OnInitDialog();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
private:
	void InitDlgText();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDlgText();
	
	return TRUE;
}
void CAboutDlg::InitDlgText()
{
//	SetDlgItemText(IDD_ABOUT_BOX,ID2LAUNCHERTEXT("IDD_ABOUT_BOX"));
	SetWindowText(ID2LAUNCHERTEXT("IDD_ABOUT_BOX"));
	SetDlgItemText(IDC_VER_STATIC,ID2LAUNCHERTEXT("IDC_VER_STATIC"));
	SetDlgItemText(IDOK,ID2LAUNCHERTEXT("ABOUT_BOX_ID_OK"));	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoPatchManDlg ��ȭ ����


CAutoPatchManDlg::CAutoPatchManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoPatchManDlg::IDD, pParent),
	m_pNetClient( NULL ),
	//m_pFtpPatch( NULL ),
	m_pHttpPatch( NULL ),
	m_bForceTerminate( FALSE ),
	m_emPatchState( E_VERSION_CUR ),
	m_emGameState( E_VERSION_CUR ),
	m_bUseHttp( FALSE ),
	m_pWebBrowserThread(NULL),
	m_pLoginThread(NULL),
	m_pPatchThread(NULL),
	m_pHashThread(NULL),
	m_pMemDC(NULL),
	m_pOldBitmap(NULL),
	m_pBitmapMem(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	GLogicData::GetInstance().LoadCountryFile();
}

void CAutoPatchManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE_IMAGE, m_ctrlTitleImage);
	DDX_Control(pDX, IDC_STATIC_NOTIFY, m_ctrlNotify);
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_ctrlOption);
	DDX_Control(pDX, IDC_BUTTON_START, m_ctrlStart);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_ctrlUpdate);
//#ifdef CH_PARAM
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_CHINA)
	{
		DDX_Control(pDX, IDC_BUTTON_RETRY, m_ctrlRetry);
	}
//#endif
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_ctrlExit);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListBox);
	DDX_Control(pDX, IDC_READYTOSTART, m_ctrlCheckBox);
	DDX_Control(pDX, IDC_BUTTON_ITEM, m_ctrlItem );
	DDX_Control(pDX, IDC_BUTTON_EVENT, m_ctrlManualPatch );
	DDX_Control(pDX, IDC_STATIC_HOMEPAGE, m_ctrlHomePage );
	DDX_Control(pDX, IDC_MNATION_COMBO, m_comboNation );
}

BEGIN_MESSAGE_MAP(CAutoPatchManDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE( IDC_MNATION_COMBO, OnCnbSelChangeNationCombo )
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnBnClickedButtonOption)
	ON_BN_CLICKED(IDC_BUTTON_RETRY, OnBnClickedButtonRetry)
	ON_REGISTERED_MESSAGE( WM_CUSTOM_CONTROLSITE_MSG, OnCustomControlSiteMsg)
	ON_BN_CLICKED(IDC_BUTTON_PATCH, OnBnClickedButtonPatch)
	ON_LBN_DBLCLK(IDC_LIST_LOG, OnLbnDblclkListLog)
	ON_MESSAGE( WM_TRANSPARENT_BK, OnTransaprentBk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_ITEM, OnBnClickedButtonItem)
	ON_BN_CLICKED(IDC_BUTTON_EVENT, OnBnClickedButtonEvent)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonUpdate)
END_MESSAGE_MAP()


// CAutoPatchManDlg �޽��� ó����
BOOL CAutoPatchManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu = ID2LAUNCHERTEXT("IDS_ABOUTBOX", 0 );
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

	m_bLBDown = FALSE;	
	SetWindowText ( NS_GLOBAL_VAR::strTitle.GetString() );
	SetAppPath ();	//	���丮 ��� ����

	
	//	��ư ����
    if (SP_CHINA == GLogicData::GetInstance().GetServiceProvider())
    {
	    ::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_SHOW );
	    ::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_HIDE );
	    ::ShowWindow( m_ctrlUpdate.GetSafeHwnd(), SW_HIDE );
	    m_ctrlRetry.EnableWindow( FALSE );
    }
    else
    {
	    ::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_HIDE );
	    ::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_HIDE );
    }

	m_ctrlStart.EnableWindow( FALSE );
	// m_ctrlOption.EnableWindow( FALSE ); �ӽ�
	m_ctrlUpdate.EnableWindow( FALSE );


	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	GLUseFeatures::GetInstance().LoadFile(GLogicData::GetInstance().GetUseFeaturesFileName(), pPath, FALSE);

	RANPARAM::LOAD(
        NS_GLOBAL_VAR::strAppPath.GetString(),
        pApp->GetServiceProvider(),
        false,
        false);    

//#if defined(RELEASE) || defined(NDEBUG)
    if (SP_INDONESIA == GLogicData::GetInstance().GetServiceProvider())
    {
        DlgVtcGuardUpdate* pVtcGuardDlg = new DlgVtcGuardUpdate(this);
        pVtcGuardDlg->Create(DlgVtcGuardUpdate::IDD);
        pVtcGuardDlg->ShowWindow(SW_SHOW);

        char Msg[MAX_PATH] = {0};
        std::string AppPath(sc::getAppPath());
        vt_Autoupdate( VTC_SERVER_DOMAIN, VTC_SERVER_DOMAIN_SUBFOLDER, VTC_SERVER_PORT, AppPath.c_str(), Msg );

        pVtcGuardDlg->ShowWindow(SW_HIDE);
        pVtcGuardDlg->DestroyWindow();
        delete pVtcGuardDlg;
        pVtcGuardDlg = NULL;
    }
//#endif

	TCHAR szFullPath[MAX_PATH] = {0};

	StringCchCopy(szFullPath, MAX_PATH, NS_GLOBAL_VAR::strAppPath);
	StringCchCat(szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
	
	CLauncherTextMan::GetInstance().SetPath(szFullPath);
	CLauncherTextMan::GetInstance().LoadText(RANPARAM::strLauncherText.GetString(), TRUE );
	
//#ifdef CH_PARAM
    if (SP_CHINA == GLogicData::GetInstance().GetServiceProvider())
    {
	    // �߱� - ��ġ ������ �о ����Ʈ ���̸� �ٿ�ε� ���� ���̾�α׸� ����.
	    if( RANPARAM::nChinaRegion == RANPARAM::MAX_CHINA_REGION )
	    {
		    // ���̾�α� ����
		    CDlgDownloadArea dlg;
		    dlg.m_strAreaName[0] = RANPARAM::China_Region[0].strName;
		    dlg.m_strAreaName[1] = RANPARAM::China_Region[1].strName;
		    dlg.m_strAreaName[2] = RANPARAM::China_Region[2].strName;
    //		dlg.m_strAreaName[3] = RANPARAM::China_Region[3].strName;
    //		dlg.m_strAreaName[4] = RANPARAM::China_Region[4].strName;
    //		dlg.m_strAreaName[5] = RANPARAM::China_Region[5].strName; // �߱����� �߰�

		    dlg.DoModal();
    		
		    RANPARAM::nChinaRegion = dlg.m_nDownloadArea;

		    StringCchCopy( RANPARAM::LoginAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].LoginAddress.GetString() );

		    for( int i=0 ; ; ++i )
		    {
			    if( RANPARAM::China_Region[RANPARAM::nChinaRegion].HttpAddressTable[i].IsEmpty() ) 
				    break;

			    StringCchCopy( RANPARAM::HttpAddressTable[i], STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].HttpAddressTable[i].GetString() );
		    }

		    RANPARAM::strNewsURL = RANPARAM::China_Region[RANPARAM::nChinaRegion].strNewsURL;
		    RANPARAM::strImageURL = RANPARAM::China_Region[RANPARAM::nChinaRegion].strImageURL;		
		    StringCchCopy( RANPARAM::ItemShopAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].strItemShop.GetString() );
		    StringCchCopy( RANPARAM::ItemShopHPAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].strItemShopHP.GetString() );		
		    StringCchCopy( RANPARAM::EventPageAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].strEventPage.GetString() );
			StringCchCopy( RANPARAM::ManualPatchAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].strManualPath.GetString() );
		    StringCchCopy( RANPARAM::HomePageAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].strHomePage.GetString() );

		    RANPARAM::SAVE(GLogicData::GetInstance().GetServiceProvider());

		    dlg.ShowWindow( SW_HIDE );
	    }
    }
//#endif

	m_strNewsURL = RANPARAM::strNewsURL;
	m_strImageURL = RANPARAM::strImageURL;

	InitDlgCtrl();
	InitDlgText();


/*	//		Japan �α��� ��� ����
#ifdef JP_PARAM
	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;

	CString StrCmdLine = pApp->m_lpCmdLine;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "/" );
	STRUTIL::RegisterSeparator ( " " );

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );

	CString strTemp = strCmdArray.GetAt(0);

	CString strVALUE, strKEY;
	int nPos(-1);

	strKEY = "U=";
	nPos = strTemp.Find( strKEY );
	if ( nPos!=-1 )
	{
		strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

		// �Ϻ� ���Ŀ��� Ŀ�ǵ� ���ο� U=01101@nm �� ���
		// NewsURL �ڿ� nm�� �ٿ��� �������� ���� ��Ų��.
		// �Ϻ� �ݸ����� �α��� �Ҷ� �ٸ� �ּҸ� �ٿ��ֱ� �����̴�.
		m_strNewsURL += "?d=";

		nPos = strVALUE.Find( _T('@') );
		if( nPos != -1 )
		{
			m_strNewsURL += strVALUE.Mid( nPos+1, 2 );
		}
	}
#endif
*/
	//	������ ����
	SetTimer( E_WEB_BROWSER, nWEB_BROWSER_START, NULL );
	//BeginThread( E_THREAD_WEB, m_pWebBrowserThread, &m_ctrlNotify, strNewsURL );

	// ����׻��� �ʱ�ȭ
	CDebugSet::OneTimeSceneInit(pApp->GetSubPath(), true);

	//	����!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//	���α׷� ���� ��ΰ� �ʿ��� ��� 'SetAppPath()'�� ȣ��� ���Ŀ�
	//	�ڵ带 �ۼ��ؾ��Ѵ�.
	END_LAUNCHER_PATCH ();

	//////////////////////////////////////////////////////////////////////////////
	// Initilaize Winsock2
    int Ret = sc::net::initializeNetwork();
	if ( Ret == NET_ERROR )
	{
		// ���� ������ ����.	
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 5 ) );
		EndDialog(IDOK);
	}
	//////////////////////////////////////////////////////////////////////////////
	
	// NOTE
	//		�⺻ ����Ÿ ����
	m_pNetClient = new CNetClient( m_hWnd );
	if( NULL == m_pNetClient ) EndDialog(IDOK);
	m_pHttpPatch = new CHttpPatch;
	if( NULL == m_pHttpPatch ) EndDialog(IDOK);
	//m_pFtpPatch = new CPatch;

	//	�������� ��� ����
	//
	//m_pFtpPatch->SetForceTerminateToggle ( &m_bForceTerminate );
	m_pHttpPatch->SetForceTerminateToggle(&m_bForceTerminate);
    mincab::SetCabForceTerminate(&m_bForceTerminate);

	//	�����Ȳ ����
	ULONGLONG* pCurPos = NULL;
	ULONGLONG* pCurEnd = NULL;
	NS_LOG_CONTROL::GetProcessCurPositionAddr ( &pCurPos, &pCurEnd );	
	//m_pFtpPatch->SetProgressValue ( pCurPos, pCurEnd );
	m_pHttpPatch->SetProgressValue ( pCurPos, pCurEnd );
    mincab::SetCabProgressValue(pCurPos, pCurEnd);

	//	Note : �α��� ���� ���� -> �����˻� -> ��ġ
	//
	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void	CAutoPatchManDlg::UPDATE_TIME ( const UINT nElapsedTime )
{
	if ( 0.0f <= m_nLIFE_TIME )	m_nLIFE_TIME -= nElapsedTime;
}

void CAutoPatchManDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAutoPatchManDlg::OnPaint() 
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
		CPaintDC dc(this);
		CRect rtClient;
		GetClientRect(&rtClient);

		if ( m_pMemDC )
		{
			dc.BitBlt( 0,0,rtClient.Width(), rtClient.Height(), m_pMemDC, 0,0, SRCCOPY);
		}
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CAutoPatchManDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoPatchManDlg::PostNcDestroy()
{
	EndThread( m_pWebBrowserThread );
	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	//	FTP ���� ����
	//m_pFtpPatch->DisConnect ();

	SAFE_DELETE ( m_pNetClient );
	SAFE_DELETE ( m_pHttpPatch );
	//SAFE_DELETE ( m_pFtpPatch );


	SAFE_DELETE ( m_ctrlProgressCur );
	SAFE_DELETE ( m_ctrlProgressAll );

	SAFE_DELETE ( m_pMemDC );
	SAFE_DELETE ( m_pBitmapMem );

    sc::net::closeNetwork();

	CDebugSet::FinalCleanup ();

	GLogicData::GetInstance().ClearData();

	//DeleteCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );
	
	CDialog::PostNcDestroy();
}

BOOL CAutoPatchManDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch ( pMsg->wParam )
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return TRUE;
			}
		}
		break;

	case WM_SYSKEYDOWN:
		{
			if ( pMsg->wParam == VK_F4 )
			{
				OnBnClickedButtonExit ();
				return TRUE;
			}
		}
		break;

	case WM_LISTADDSTRING:
		{
			if( pMsg->wParam == 0)
			{
				ListAddString( (PCHAR)pMsg->lParam );
				delete [] (PCHAR)pMsg->lParam; // ���� �޼����� �����͸� �������ش�.
			}
			else
			{
//				ListAddString( static_cast<UINT>(pMsg->wParam) );
				ListAddString( (LPCTSTR) pMsg->wParam );
			}

			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT CAutoPatchManDlg::OnCustomControlSiteMsg(WPARAM wParam, LPARAM lParam)
{
LRESULT hasBeenHandled = FALSE;

	ASSERT((wParam > kCCSN_NoMessage) && (wParam < kCCSN_MessageLimit));

	switch (wParam)
	{
	case kCCSN_CreateSite:

		if (lParam != NULL)
		{
			kCCSN_CreateSiteParams *params = (kCCSN_CreateSiteParams *)lParam;

			if (params->pCtrlCont != NULL)
			{
				params->pSite = new CCustomControlSite(params->pCtrlCont);
				hasBeenHandled = TRUE;
			}
		}

		break;
	}

	return hasBeenHandled;
}

void CAutoPatchManDlg::OnBnClickedButtonPatch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	Note : �α��� ���� ���� -> �����˻� -> ��ġ
	//
	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();
}

void CAutoPatchManDlg::OnLbnDblclkListLog() // ����Ʈ�� �ִ� ������ �����α׷� ���
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	TCHAR szDEBUGDIR[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szDEBUGDIR, CSIDL_PERSONAL, FALSE );
	StringCchCat( szDEBUGDIR, MAX_PATH, pPath->DebugInfoRoot().c_str() ); //SUBPATH::DEBUGINFO_ROOT );

	// Use time structure to build a customized time string.
	time_t ltime;
	struct tm *today;

	time(&ltime);
	today = localtime(&ltime);

	char szFileName[MAX_PATH] = {0};
	// Use strftime to build a customized time string.
	strftime ( szFileName, MAX_PATH, "log.%Y%m%d%H%M.txt", today );	
	
	CString strLogFile;
	strLogFile.Format ( "%s\\%s", szDEBUGDIR, szFileName );

	FILEOUT( strLogFile ); // �α����� ���
}

BOOL CAutoPatchManDlg::CREATEPROCESS_PATCHPRIME()
{
	CString strTemp;
	CString strCmdLine;
	CString strFile( NS_GLOBAL_VAR::g_szLauncherPatchCabFile );
	strFile = strFile.Left( strFile.ReverseFind( '.' ) );

    // ���� �Ķ��Ÿ ����( ������ �� �� �α��ν� ��� ) 
	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;
	strTemp.Format( _T("\"%s%s\""), NS_GLOBAL_VAR::strAppPath, strFile);
	strCmdLine.Format( _T(" %d %d %s"), m_sPatchVer, m_sGameVer, pApp->m_lpCmdLine ); 

	// MEMO : Handles in PROCESS_INFORMATION must be closed with CloseHandle when they are no longer needed.
	if( ! ShellExecute( m_hWnd , "open", strTemp.GetString() ,strCmdLine.GetString(), NULL, SW_SHOW) )
	{
		MessageBox ( "CreateProcess Patch Prime", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

LRESULT CAutoPatchManDlg::OnTransaprentBk( WPARAM wParam, LPARAM lParam)
{
	HDC hdc = ( HDC)wParam;
	HWND hwnd = ( HWND)lParam;
    CTransparentHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}

void CAutoPatchManDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	BuildBkDC();
	Invalidate(FALSE);
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CAutoPatchManDlg::BuildBkDC()
{
	CBitmap bmpbk;
	bmpbk.LoadBitmap(  IDB_BITMAP_BACKIMAGE );
	BITMAP bmpInfo;
	bmpbk.GetBitmap( &bmpInfo);

	CDC* pWndDC = GetDC();
	CDC memdc;
	memdc.CreateCompatibleDC( pWndDC);
	CBitmap* pOld = memdc.SelectObject( &bmpbk);


	CRect rtClient;
	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		m_pMemDC->SelectObject( m_pOldBitmap);
		delete m_pBitmapMem;
		m_pBitmapMem = NULL;
		delete m_pMemDC;
		m_pMemDC = NULL;
	}
	
	if ( m_pMemDC == NULL )
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC( pWndDC);

		m_pBitmapMem = new CBitmap;
		m_pBitmapMem->CreateCompatibleBitmap( pWndDC, rtClient.Width(), rtClient.Height());
		m_pOldBitmap = (CBitmap*)m_pMemDC->SelectObject( m_pBitmapMem);
	}

	m_pMemDC->StretchBlt( 0,0, rtClient.Width(), rtClient.Height(), &memdc, 0,0,bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
    memdc.SelectObject( pOld);
	ReleaseDC( pWndDC);

	//when the parent dialog's background is rebuild, notify the child which has an transparent tag.
	CTransparentHelper::NotifyTransparentChild( GetSafeHwnd());
}

void CAutoPatchManDlg::OnCbnSelchangeCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 73 );
	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, strMsg.GetString() );
}

void CAutoPatchManDlg::GetProcessByFileName( CString strFilename, std::vector< HANDLE >& Handles )
{
	strFilename.MakeUpper();

	HANDLE hProcessSnapshot;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;

	hProcessSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );

	if ( hProcessSnapshot == INVALID_HANDLE_VALUE ) 
		return;

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	Process32First( hProcessSnapshot, &pe32 );

	do
	{
		hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, pe32.th32ProcessID );
		if ( hProcess != NULL )
		{
			CString strTargetName = pe32.szExeFile;
			strTargetName.MakeUpper();
			if ( strFilename == strTargetName )
			{
				Handles.push_back( hProcess );
			}
		}

	} while ( Process32Next( hProcessSnapshot, &pe32 ) );
}

BOOL CAutoPatchManDlg::SafeTerminateProcess( HANDLE hProcess, UINT uExitCode )
{
	DWORD dwTID, dwCode, dwErr = 0;
	HANDLE hProcessDup = INVALID_HANDLE_VALUE;
	HANDLE hRT = NULL;
	HINSTANCE hKernel = GetModuleHandle( "Kernel32" );

	BOOL bSuccess = FALSE;
	BOOL bDup = DuplicateHandle( GetCurrentProcess(), 
		hProcess, 
		GetCurrentProcess(), 
		&hProcessDup, 
		PROCESS_ALL_ACCESS, 
		FALSE, 
		0);
	if ( GetExitCodeProcess(( bDup ) ? hProcessDup : hProcess, &dwCode ) 
		&& ( dwCode == STILL_ACTIVE ) )
	{
		FARPROC pfnExitProc;
		pfnExitProc = GetProcAddress( hKernel, "ExitProcess" );
		hRT = CreateRemoteThread( ( bDup ) ? hProcessDup : hProcess, 
			NULL, 
			0, 
			( LPTHREAD_START_ROUTINE )pfnExitProc,
			( PVOID )uExitCode, 0, &dwTID );
		if ( hRT == NULL ) dwErr = GetLastError();
	}
	else 
	{
		dwErr = ERROR_PROCESS_ABORTED;
	}
	if ( hRT )
	{
		WaitForSingleObject( ( bDup ) ? hProcessDup : hProcess, INFINITE );
		CloseHandle( hRT );
		bSuccess = TRUE;
	}
	if ( bDup )
		CloseHandle( hProcessDup );
	if ( !bSuccess )
		SetLastError( dwErr );

	return bSuccess;
}
