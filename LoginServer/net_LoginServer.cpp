// GameServer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"

#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/s_NetGlobal.h"
#include "../RanLogic/GLogicDataMan.h"
#include "CheckOrder.h"
#include "net_LoginServer.h"
#include "../ServerController/ServerControllerMsgDefine.h"
#include "../MfcExLib/MinBugTrap.h"

//#include "../=VLD/include/vld.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../=TBB/include/tbb/tbbmalloc_proxy.h"

#include "../=TBB/include/tbb/task_scheduler_init.h"

#endif

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define MAX_LOADSTRING 100
#define CHECK_MAINTENANCE_TIMER_ID 2

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

HWND g_hEditBox = NULL;
HWND g_hWindow = NULL;
bool g_bConnect = false;
SUBPATH* g_pPath = NULL;

CLoginServer* g_pServer = NULL;
extern HWND	g_hwndMainDlg;
std::string g_TitleText;

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
	::AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	//
	//mjeon.tbb
	// all threads that uses tbb MUST init the scheduler.
	//
#ifdef _USE_TBB
	tbb::task_scheduler_init init;	
#endif

	g_hInst=hInstance;
	g_pServer = NULL;
	SERVER_CONTROLLER::checkCmdParameter(lpCmdLine);
    GLogicData::GetInstance().LoadCountryFile();
	BUG_TRAP::BugTrapInstall(GLogicData::GetInstance().GetServiceProvider(), std::string(_T("LoginServer")),true,true,true);
    g_pPath = new SUBPATH(GLogicData::GetInstance().GetServiceProvider());

    sc::initialize();

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);
	return 0;
}

void setFirstFocus(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_EDITCONSOLE);
	SetFocus(pWnd);
}

void cmdStartOrStop(HWND hDlg)
{
	int nRetCode = 0;
	if (g_pServer != NULL && g_pServer->IsRunning())
	{					
		nRetCode = g_pServer->Stop();
		if ( nRetCode == NET_OK )
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
		delete g_pServer;
		g_pServer = NULL;

		HWND pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
		EnableWindow( pWnd, FALSE );
	}			
	else // Server is not running
	{				
		g_pServer = new CLoginServer(
            hDlg,
            GetDlgItem(hDlg, IDC_EDITCONSOLE),
            GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO),
            g_pPath,
            GLogicData::GetInstance().GetServiceProvider());
		nRetCode = g_pServer->Start();

		if ( nRetCode == NET_OK )
		{
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");			
			
			CString strTemp;
			
			strTemp.Format(_T("%d"), g_pServer->GetPatchVersion());
			SetDlgItemText(hDlg, IDC_EDIT_GAMEVERSION, strTemp);
			
			strTemp.Format(_T("%d"), g_pServer->GetLauncherVersion());
			SetDlgItemText(hDlg, IDC_EDIT_PATCHVERSION, strTemp);

            strTemp.Format(_T("%d"), g_pServer->GetPatchCutVersion());
            SetDlgItemText(hDlg, IDC_EDIT_VERSIONGAP, strTemp);

			HWND pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
			EnableWindow( pWnd, TRUE );
			switch( GLogicData::GetInstance().GetServiceProvider())
			{
				case EMSERVICE_PROVIDER::SP_OFFICE_TEST:
				case EMSERVICE_PROVIDER::SP_KOREA:
				case EMSERVICE_PROVIDER::SP_KOREA_TEST:
				case EMSERVICE_PROVIDER::SP_GS:
				case EMSERVICE_PROVIDER::SP_GLOBAL:
					{
						CheckDlgButton(hDlg, IDC_CHECK_MAINTENANCE, BST_CHECKED);				
						g_pServer->SetAllowIP(TRUE);		
						break;
					}
				case EMSERVICE_PROVIDER::SP_TAIWAN:
				case EMSERVICE_PROVIDER::SP_CHINA:
				case EMSERVICE_PROVIDER::SP_JAPAN:
				case EMSERVICE_PROVIDER::SP_PHILIPPINES:
				case EMSERVICE_PROVIDER::SP_THAILAND:			
				case EMSERVICE_PROVIDER::SP_MALAYSIA:			
				case EMSERVICE_PROVIDER::SP_INDONESIA:
				case EMSERVICE_PROVIDER::SP_MALAYSIA_EN:
				case EMSERVICE_PROVIDER::SP_VIETNAM:
				case EMSERVICE_PROVIDER::SP_HONGKONG:			
				case EMSERVICE_PROVIDER::SP_WORLD_BATTLE:
				case EMSERVICE_PROVIDER::SP_EU:
				case EMSERVICE_PROVIDER::SP_US:
				case EMSERVICE_PROVIDER::SP_TOTAL_NUM:
					{
						break;
					}			
			}		

			if (GLogicData::GetInstance().GetServiceProvider() == SP_WORLD_BATTLE)
			{				
				std::string NewTitle("World Battle ");
				NewTitle.append(g_TitleText);
				SetWindowText(hDlg, NewTitle.c_str());
			}

			if (g_pServer->GetServiceProvider() != GLogicData::GetInstance().GetServiceProvider())
			{
				std::string NewTitle("country.lua and server config xml. service provider mismatch ");
				NewTitle.append(g_TitleText);
				SetWindowText(hDlg, NewTitle.c_str());
			}
		}
		else 
		{
			g_pServer->Stop();
			delete g_pServer;
			g_pServer = NULL;			

			HWND pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
			EnableWindow( pWnd, FALSE );
		}				
	}
}

void initDialog(HWND hDlg)
{
	g_hwndMainDlg = hDlg;

	std::string strTitle;
	char szTitle[64];
	ZeroMemory(szTitle, sizeof(szTitle));

	GetWindowText(hDlg, szTitle, 64);

	g_TitleText = std::string(szTitle) + std::string(" - ") + sc::getFileVersion();

	SetWindowText(hDlg, g_TitleText.c_str());


	HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
	EnableWindow(pWnd, TRUE);
	
	setFirstFocus(hDlg);
    
	//	check order control
	order_ctrl::init(hDlg);

	::SetTimer(hDlg,
		CHECK_MAINTENANCE_TIMER_ID,
		1000,
		NULL );
}

void cmdExit(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		g_pServer->Stop();
		delete g_pServer;
		g_pServer = NULL;
        delete g_pPath;
        g_pPath = NULL;
	}
}

void cmdCheck(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		if (g_pServer->GetInspection()) // 점검중이면
		{
			g_pServer->SetInspecting(FALSE); // 정상상태로 변환
			SetDlgItemText(hDlg, IDC_BTN_CHECK, _CHECK_TEXT );
		}
		else // 정상이면
		{
			g_pServer->SetInspecting(TRUE); // 점검상태로 변환
			SetDlgItemText(hDlg, IDC_BTN_CHECK, _CHECK_END_TEXT );
		}
	}
}

void cmdVersion(HWND hDlg)
{
	TCHAR szPatchVersion[10] = {0};
	TCHAR szLauncherVersion[10] = {0};
    TCHAR szVersionGap[10] = {0};

	if (g_pServer != NULL)
	{
		GetDlgItemText(hDlg, IDC_EDIT_GAMEVERSION,  szPatchVersion,  10);
		GetDlgItemText(hDlg, IDC_EDIT_PATCHVERSION, szLauncherVersion, 10);
        GetDlgItemText(hDlg, IDC_EDIT_VERSIONGAP, szVersionGap, 10);

		g_pServer->SetVersion(atoi(szPatchVersion), atoi(szLauncherVersion), atoi(szVersionGap));
	}
}

void cmdShowWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;
	Shell_NotifyIcon(NIM_DELETE, &nid);			
	ShowWindow(hDlg, SW_SHOW);
}

void cmdHideWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_NET_LOGINSERVER));
	lstrcpy(nid.szTip, "LoginServer");
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(hDlg, SW_HIDE);
}

void cmdPopupMenu(HWND hDlg)
{
	HMENU hMenu, hPopupMenu;
	POINT pt;

	hMenu = LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_POPMENU));
	hPopupMenu = GetSubMenu(hMenu,0);
	GetCursorPos(&pt);
	SetForegroundWindow(hDlg);
	TrackPopupMenu(hPopupMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
					pt.x, pt.y, 0, hDlg, NULL);
	SetForegroundWindow(hDlg);
	DestroyMenu(hPopupMenu);
	DestroyMenu(hMenu);
}

void DeleteAndEndDialog(HWND hDlg)
{
	GLogicData::GetInstance().ClearData();
	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();

	EndDialog(hDlg,0);

	SAFE_DELETE( g_pPath );
}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	g_hWindow = hDlg;

	switch (iMessage)
	{	
	case WM_UPDATE_LOG_CONSOLE:
	{
		char *pBuffer = (char *)lParam;

		if(!pBuffer)
			return FALSE;

		HWND hEditConsole = (HWND)wParam;

		SendMessage(hEditConsole, WM_SETFOCUS,		(WPARAM) 0, 0);
		SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX);
		SendMessage(hEditConsole, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0);
		SendMessage(hEditConsole, EM_SCROLLCARET,	(WPARAM) 0,				(LPARAM) 0);
		SendMessage(hEditConsole, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) lParam);
		SendMessage(hEditConsole, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1);
		SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX);

		//SAFE_DELETE_ARRAY(pBuffer);
		break;
	}
	case WM_INITDIALOG :
		initDialog(hDlg);		
        if( SERVER_CONTROLLER::bStart )
            ::PostMessage( g_hWindow, RAN_MSG_START, 0, 0 );

		break;
	
	case RAN_MSG_START:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_STOP:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_AUTH_FAIL:
		cmdStartOrStop(hDlg);
		//cmdExit(hDlg);
		DeleteAndEndDialog(hDlg);
		break;

	case RAN_MSG_EXIT:
		cmdExit(hDlg);
		DeleteAndEndDialog(hDlg);
		break;

	case WM_TIMER:
		{
			switch ( wParam )
			{
			case order_ctrl::CO_TIMER:
				//	check order control
				order_ctrl::timer(hDlg);
				break;
			
			case CHECK_MAINTENANCE_TIMER_ID:
				if ( g_pServer && g_pServer->IsRunning() )
				{
					bool bOK = g_pServer->GetAllowIP();

					if( bOK )
					{
						::SendMessage(GetDlgItem(hDlg,IDC_CHECK_MAINTENANCE), BM_SETCHECK, true, 0);
					}
					else
					{
						::SendMessage(GetDlgItem(hDlg,IDC_CHECK_MAINTENANCE), BM_SETCHECK, false, 0);
					}
				}
				break;
			};
		}
		break;
	case WM_INITMENUPOPUP:
		if (g_pServer != NULL && g_pServer->IsRunning())
		{
			ModifyMenu(HMENU(wParam),IDC_BTN_STOP,MF_BYCOMMAND | MF_STRING, IDC_BTN_STOP, "Stop");
		}
		else
		{
			ModifyMenu(HMENU(wParam),IDC_BTN_STOP,MF_BYCOMMAND | MF_STRING, IDC_BTN_STOP, "Start");			
		}
		break;
	case WM_SYSCOMMAND:
		switch(LOWORD(wParam))
		{
		case SC_CLOSE:
			cmdHideWindow(hDlg);
			return TRUE;
		}
		break;
	case WM_TRAY_NOTIFICATION:
		switch(lParam)
		{
		case WM_LBUTTONUP:
			cmdShowWindow(hDlg);				
			return TRUE;
		case WM_RBUTTONUP:
			cmdPopupMenu(hDlg);
			return TRUE;
		}
		return FALSE;
	case WM_COMMAND :
		switch (LOWORD(wParam))
		{
		case IDC_BTN_STOP :
			cmdStartOrStop(hDlg);
			setFirstFocus(hDlg);
			return TRUE;

		case IDC_BTN_CHECK : // 점검중
			cmdCheck(hDlg);			
			return TRUE;

		case IDC_BTN_SETVERSION : // 버전 세팅
			cmdVersion(hDlg);
			return TRUE;		

		case IDC_CLOSE :
			if( MessageBox(hDlg,"Do you really want to exit?", "Warning", MB_YESNO | MB_ICONWARNING ) == IDYES){
				cmdExit(hDlg);			
				DeleteAndEndDialog(hDlg);
			}
			return TRUE;

		case IDC_OPEN:
			cmdShowWindow(hDlg);
			return TRUE;
		
		// Session 서버와 연결이 끊어졌을때 다시 연결하기 위해서 추가함.
		case IDC_BTN_CONNECT_SESSION:
			if (g_pServer != NULL && g_pServer->IsRunning())
			{
				// g_pServer->SessionConnectSndSvrInfo( true );
				g_pServer->SessionConnectSndSvrInfo();
			}
			return TRUE;

		case IDC_EDITCONSOLE :
			switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND) lParam, _T(""));
				break;
			}
			return TRUE;
			break;

        case IDC_EDITCONSOLE_INFO :
            switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND) lParam, _T(""));
				break;
			}
			return TRUE;
			break;

		case IDC_CHECK_CHECKORDER:
			{
				//	check order control
				order_ctrl::command(hDlg);
			}
			break;

		case IDC_CHECK_MAINTENANCE:
			{
				if (g_pServer && g_pServer->IsRunning())
				{
					bool bOK = g_pServer->GetAllowIP() ? false : true;
					g_pServer->SetAllowIP(bOK);
				}
			}
            break;

        case IDC_BTN_CLEAR_IP_BLOCK_LIST:
            if (g_pServer && g_pServer->IsRunning())
                g_pServer->ClearBlockIp();
            break;
		}
		return FALSE;
	}
	return FALSE;
}
