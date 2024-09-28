#define STRICT
#include "stdafx.h"

#include "../EngineLib/GUInterface/Cursor.h"
#include "../EngineLib/DxTools/DxCursor.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxResponseMan.h"

#include "../RanLogic/GLogicData.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/Tool/DxParamSet.h"
#include "../RanLogicClient/NetWork/s_NetClient.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../RanLogic/RANPARAM.h"
#include "Basic.h"
#include "BasicWnd.h"
#include "../MfcExLib/BlockProg.h"
#include "../MfcExLib/rol_clipboard.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//CBLOCK_PROG	g_BLOCK_PROG;
HWND g_hWnd(NULL);
DxGlobalStage* CBasicWnd::g_pGlobalStage = NULL;

/////////////////////////////////////////////////////////////////////////////
// CBasicWnd

CBasicWnd::CBasicWnd(EMSERVICE_PROVIDER emServiceProvider, bool bLoginProcessLog)
    : CD3DApplication(emServiceProvider)
    , m_bCreated(FALSE)
    , m_wndSizeX(800)
    , m_wndSizeY(600)
    , m_hCursorDefault(FALSE)
    , m_nGGTimer(0)
    , m_nGGATimer(0)
    , m_nGGA12Timer(0)
    , m_hMutex(NULL)
	, m_dwDelayGPU(0)
	, m_dwTimeGetTimePrev(0)
	, m_bForegroundWindow(TRUE)
	, m_fEmptyWorkingSetTime(0.f)
	, m_bLoginProcessLog(bLoginProcessLog)
{
	m_bUseDepthBuffer = TRUE;
	m_pApp = (CBasicApp*) AfxGetApp();

	GLOGIC::SetGlogicPackFile(FALSE);
	GLOGIC::SetGlogicZipFile(TRUE);
	GLOGIC::SetEngineZipFile(TRUE);    
    m_pEngineDevice = new DxResponseMan(m_pApp->GetServiceProvider(), m_pApp->GetSubPath());
    m_pGlobalStage = new DxGlobalStage(m_pEngineDevice, m_pApp->GetSubPath(), false, m_bLoginProcessLog);
	CBasicWnd::g_pGlobalStage = m_pGlobalStage;
}

CBasicWnd::~CBasicWnd()
{
    delete m_pGlobalStage;
    m_pGlobalStage = NULL;

    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

BEGIN_MESSAGE_MAP(CBasicWnd, CWnd)
//{{AFX_MSG_MAP(CBasicWnd)
	ON_WM_ACTIVATEAPP()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_MESSAGE(NET_NOTIFY,  OnNetNotify)
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
	ON_WM_MOUSEMOVE()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCACTIVATE()
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicWnd message handlers

BOOL CBasicWnd::Create() 
{
	CBasicApp *pBasicApp = (CBasicApp *) AfxGetApp();

	CRect rectWnd;
	LONG nWindowStyle = WS_POPUP | WS_VISIBLE;
	
	if ( RANPARAM::bScrWindowed )
	{
		m_nScreenWinX = GetSystemMetrics(SM_CXSCREEN);
		m_nScreenWinY = GetSystemMetrics(SM_CYSCREEN);

		m_wndSizeX = RANPARAM::dwScrWidth;
		m_wndSizeY = RANPARAM::dwScrHeight;

		if ( m_wndSizeX>m_nScreenWinX || m_wndSizeY>m_nScreenWinY )
		{
			m_wndSizeX = m_nScreenWinX;
			m_wndSizeY = m_nScreenWinY;
		}

		if ( RANPARAM::bScrWndFullSize )
		{
			m_wndSizeX = m_nScreenWinX;
			m_wndSizeY = m_nScreenWinY;

			//	Note : ���̵� ��ũ�� ũ�⸦ �Ѿ ���
			//			2 ��ũ������ �ν�.
			float fScreen = m_nScreenWinX / float(m_nScreenWinY);
			if ( fScreen > (16.0f/9.0f) )
			{
				m_wndSizeX = m_nScreenWinX/2;
			}

			rectWnd = CRect( 0, 0, m_wndSizeX, m_wndSizeY );
		}
		else
		{
			if ( m_wndSizeX>=(m_nScreenWinX) || m_wndSizeY>=(m_nScreenWinY) )
			{
				m_wndSizeX = 1024;
				m_wndSizeY = 768;

				if ( m_wndSizeX>=m_nScreenWinX || m_wndSizeY>=m_nScreenWinY )
				{
					m_wndSizeX = m_nScreenWinX-40;
					m_wndSizeY = m_nScreenWinY-60;
				}
			}

			if( RANPARAM::dwScrWidth < 1024 || RANPARAM::dwScrHeight < 768 )
			{
				nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | 
					WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
			}
			else
			{
				nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | 
					WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
			}

			RECT rt = {0, 0, m_wndSizeX, m_wndSizeY};
			AdjustWindowRectEx(&rt, nWindowStyle, FALSE, WS_EX_APPWINDOW);

			m_wndSizeX = rt.right - rt.left;
			m_wndSizeY = rt.bottom - rt.top;
			INT nX = (m_nScreenWinX - m_wndSizeX) / 2;
			INT nY = (m_nScreenWinY - m_wndSizeY) / 2;

			rectWnd = CRect( nX, nY, m_wndSizeX, m_wndSizeY );
		}
	}
	else
	{
		m_nScreenWinX = GetSystemMetrics(SM_CXSCREEN);
		m_nScreenWinY = GetSystemMetrics(SM_CYSCREEN);

		m_wndSizeX = m_nScreenWinX;
		m_wndSizeY = m_nScreenWinY;

		rectWnd = CRect( 0, 0, m_wndSizeX, m_wndSizeY );
	}

	EMSERVICE_PROVIDER ServiceProvider = pBasicApp->GetServiceProvider();
//#if defined(NDEBUG)	&& !defined(_RELEASED)
	// �ߺ� ���� ����
	if (/*ServiceProvider == SP_TAIWAN || */ServiceProvider == SP_HONGKONG || ServiceProvider == SP_PHILIPPINES || ServiceProvider == SP_MALAYSIA ||  ServiceProvider == SP_MALAYSIA_EN)
		m_hMutex = CreateMutex(NULL, TRUE, "Global\\RAN-ONLINE");

	if (ServiceProvider != SP_WORLD_BATTLE && GetLastError() == ERROR_ALREADY_EXISTS) 
		return FALSE;
//#endif

	LPCSTR strClass = AfxRegisterWndClass(
							CS_DBLCLKS,
							::LoadCursor(NULL, IDC_ARROW),
							(HBRUSH)GetStockObject(BLACK_BRUSH),
							AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	if (!strClass)
	{
		AfxMessageBox( _T( "Class Registration Failed\n" ) );
		return FALSE;
	}

	if ( CreateEx ( WS_EX_APPWINDOW, strClass, AfxGetAppName(), nWindowStyle,
		rectWnd.left, rectWnd.top, rectWnd.right, rectWnd.bottom, NULL, NULL, 0 ) == FALSE )
	{
		return FALSE;
	}

	//#if ( !defined(DAUMPARAM) && !defined(DEBUG) && !defined(_RELEASED) )
	//{
		//rol_clipboard::InitClipboard();
		//bool bCHECK = rol_clipboard::RolHeadCheck(m_hWnd);
		//if ( !bCHECK )
		//{
		//	::MessageBox ( NULL, "This Exe don't execute independent.", "error", MB_OK );
		//	return FALSE;
		//}
	//}
	//#endif // !DAUMPARAM && NDEBUG

//#ifdef CH_PARAM
    if (ServiceProvider == SP_CHINA)
	    SetWindowText("���ǻ�԰");
//#elif TW_PARAM
    else if (ServiceProvider == SP_TAIWAN)
	    SetWindowText( "Ran Online");
//#elif HK_PARAM
    else if (ServiceProvider == SP_HONGKONG)
	    SetWindowText("Ran Online");
    else if (ServiceProvider == SP_OFFICE_TEST )
        SetWindowText(sc::string::format("[PID:%1%] %2%",GetCurrentProcessId(), "Ran Online").c_str());
	else if ( ServiceProvider == SP_GS )
	{
		SetWindowText( "Ran Online" );
	}
    else
	    SetWindowText("Ran Online");
//#endif

	UpdateWindow();

	//	Note : �� ���α׷� ������.
	//
	//bool bBLOCK_LOAD = g_BLOCK_PROG.LOAD ( pBasicApp->m_szAppPath, "form.set" );
	//if ( !bBLOCK_LOAD )		return E_FAIL;

	//	Note : ������ ����.
	//g_BLOCK_PROG.DoSTARTTHREAD ();

	CD3DApplication::SetScreen( RANPARAM::dwScrWidth, 
								RANPARAM::dwScrHeight, 
								RANPARAM::emScrFormat, 
								RANPARAM::uScrRefreshHz, 
								RANPARAM::bScrWindowed );

	if ( FAILED( CD3DApplication::Create ( m_hWnd, m_hWnd, AfxGetInstanceHandle() ) ) )
		return FALSE;
	
	m_bCreated = TRUE;
	g_hWnd = m_hWnd;


    // Prevent to use WPE program.
    HMODULE hModule = GetModuleHandle( "WpeSpy.dll" );

    if ( hModule )
        FreeLibrary( hModule );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CBasicWnd::PostNcDestroy() 
{
	::UnregisterClass( AfxGetAppName(), AfxGetInstanceHandle() ); 

	//	Note : ������ ����.
	//g_BLOCK_PROG.DoENDTHREAD ();

	// Note : NP Timer End;
//#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) //|| defined ( VN_PARAM ) || defined(TH_PARAM) || defined ( HK_PARAM ) || defined(TW_PARAM) // Nprotect ����
    if (m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_EU  ) // _NP30
    {
    	KillTimer( m_nGGTimer );
	    KillTimer( m_nGGATimer );
	    KillTimer( m_nGGA12Timer );
    }
//#endif

	//	Note : ��� ���Ӱ��� ��ü ����.
	//
	Cleanup3DEnvironment();
    
    if (m_hMutex)
    {
        ReleaseMutex(m_hMutex);
    	CloseHandle(m_hMutex);
    	m_hMutex = NULL;
    }

	CWnd::PostNcDestroy();
	m_pApp->ShutDown();

	if ( RANPARAM::bGameCursor )
	{
		::SetCursor ( NULL );
		if ( m_hCursorDefault )		::DestroyCursor ( m_hCursorDefault );
	}

	HWND hTrayWnd = ::FindWindow("Shell_TrayWnd", NULL);
	if ( RANPARAM::bAllMinimum ) // �ּ�ȭ �ɼ� ���� �������� ��� â �ּ�ȭ �ǵ���. by luxes.
		::SendMessage( hTrayWnd, WM_COMMAND, 416, 0 );

//#ifdef CH_PARAM
    if (m_ServiceProvider == SP_CHINA)
    {
	    CString strTemp;
	    strTemp.Format( _T("\"explorer\" %s"), RANPARAM::strConnectionURL );

	    STARTUPINFO si;
	    PROCESS_INFORMATION pi;

	    SecureZeroMemory( &si, sizeof(si) );
	    si.cb = sizeof(si);
	    SecureZeroMemory( &pi, sizeof(pi) );

	    if( !CreateProcess( NULL, strTemp.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
	    {
		    MessageBox ( "CreateProcess explorer", "ERROR", MB_OK|MB_ICONEXCLAMATION );
	    }
	    else
	    {
		    CloseHandle( pi.hProcess );
		    CloseHandle( pi.hThread );
	    }
        strTemp.ReleaseBuffer();
    }
//#endif

    delete this;
}

/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
	void CBasicWnd::OnActivateApp(BOOL bActive, DWORD hTask)
#else
	void CBasicWnd::OnActivateApp(BOOL bActive, HTASK hTask)
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	//m_pApp->SetActive(bActive);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CBasicWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if ( RANPARAM::bGameCursor )
	{
		HCURSOR hCursl = CCursor::GetInstance().GetCurCursor();
		if ( hCursl )
		{
			CCursor::GetInstance().SetCursorNow ();
			return TRUE;
		}
	}

	return CWnd::OnSetCursor ( pWnd, nHitTest, message );
}

void CBasicWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( RANPARAM::bGameCursor )
	{
		HCURSOR hCursl = CCursor::GetInstance().GetCurCursor();
		if ( hCursl )
		{
			CCursor::GetInstance().SetCursorNow ();
			return;
		}
	}

	__super::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CBasicWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_ESCAPE:
		//PostMessage(WM_CLOSE, 0, 0);
		break;
	};

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////

// ��ž˸� �޽���
LRESULT CBasicWnd::OnNetNotify(WPARAM wParam, LPARAM lParam)
{
	if( m_pGlobalStage )
		m_pGlobalStage->OnNetNotify(wParam, lParam);

	return NULL;
}

void CBasicWnd::CheckPacketSniffer( float fElapsedTime )
{
    static float fTimer = 0;
    fTimer += m_fElapsedTime;

    // �ð��� �Ǹ� �˻� �Ѵ�
    if( fTimer < 3 )
    {
        return;
    }

#ifndef _NP30
    if(GetModuleHandle("WpeSpy.DLL"))
    {
        MessageBox ( "Hacking Detected!", "ERROR", MB_OK );
        PostMessage( WM_QUIT );
    }
#endif

	// ��Ƽ���̷��� ���α׷� (Bitdefender) �� �浹�ǰ� ��� ��Ƽ���̶� ���α׷��� ������ �׽�Ʈ�� �� ���� GameGuard�� ����� ������ ��κ��̹Ƿ� �ּ� ó��
	/*
    HMODULE hSocketDll = GetModuleHandle("ws2_32.dll");
    if(!hSocketDll)
    {
        hSocketDll = LoadLibrary("ws2_32.dll");
        if(!hSocketDll) return;
    }
    LPVOID recvApi = (LPVOID)GetProcAddress(hSocketDll, "recv");
    if(recvApi)
    {
        // ��ɾ�.
        // DLL�� ��ŷ�ؼ� �ش� DLL�� �ֻ�ܿ� �Ʒ��� ���� ��ɾ �����ϴ°�� Ŭ���̾�Ʈ�� �����Ų��.
        //
        // 0xCC - INT 3 (�극��ũ ����Ʈ)
        //  VS�� ���Ͼȿ��� �Լ������� ������ ä��� ���� �� ����� ����Ѵ�.
        // 0xE9 - JMP
        //
        // 0xE8 - CALL
        // 
        // 0xEB - JMP
        //  ��� ��ġ���� ����� ũ�� ��ŭ �����϶�� ���
        // 0xCD - INT 
        //  ���� ��� binary�� E9 37 ... ���� �����ϸ� E9�� CD�� ��ü�Ͽ� INT 0x37�̶� ������� �ٲ� ��, INT 0x37 IDT entry�� ��ŷ�ϴ� ����̴�.
        //
        BYTE FirstByte = *(BYTE *)recvApi;
        if(FirstByte == (BYTE)0xCC || FirstByte == (BYTE)0xE9 || FirstByte == (BYTE)0xE8 || FirstByte == (BYTE)0xEB || FirstByte == (BYTE)0xCD)
        {
            MessageBox ( "Hacking Detected!", "ERROR", MB_OK );
            PostMessage( WM_QUIT );
        }
    }
    LPVOID sendApi = (LPVOID)GetProcAddress(hSocketDll, "send");
    if(sendApi)
    {
        BYTE FirstByte = *(BYTE *)sendApi;
        if(FirstByte == (BYTE)0xCC || FirstByte == (BYTE)0xE9 || FirstByte == (BYTE)0xE8 || FirstByte == (BYTE)0xEB || FirstByte == (BYTE)0xCD)
        {
            MessageBox ( "Hacking Detected!", "ERROR", MB_OK );
            PostMessage( WM_QUIT );
        }
    }
	*/
    fTimer = 0;
}
