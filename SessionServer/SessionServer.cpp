// GameServer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"

#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/s_NetGlobal.h"
#include "../RanLogic/GLogicDataMan.h"

#include "SessionServer.h"
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
extern HWND  g_hwndMainDlg;
std::string g_WindowsTitle;

int APIENTRY WinMain(HINSTANCE hInstance,
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

	_set_purecall_handler( smtm_PurecallHandler );

    GLogicData::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall(GLogicData::GetInstance().GetServiceProvider(), std::string(_T("SessionServer")),true,true,true);
    g_pPath = new SUBPATH(GLogicData::GetInstance().GetServiceProvider());
	g_hInst = hInstance;
	g_pServer = NULL;
	SERVER_CONTROLLER::checkCmdParameter(lpCmdLine);

	sc::initialize();

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);

	return 0;
}

void cmdStartOrStop(HWND hDlg)
{
	int nRetCode = 0;
	// Server is running
	if (g_pServer != NULL && g_pServer->IsRunning())
	{					
		nRetCode = g_pServer->Stop();
		if ( nRetCode == NET_OK )
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
		delete g_pServer;
		g_pServer = NULL;		
	}
	// Server is not running
	else
	{				
		g_pServer = new CSessionServer(
            hDlg,
            GetDlgItem(hDlg, IDC_EDITCONSOLE),
            GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO),
            g_pPath,
            GLogicData::GetInstance().GetServiceProvider());
		nRetCode = g_pServer->Start();
		if ( nRetCode == NET_OK )
		{
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");
			std::string NewTitle;
			if (GLogicData::GetInstance().GetServiceProvider() == SP_WORLD_BATTLE)
				NewTitle = "World Battle ";
			NewTitle.append(g_WindowsTitle);
			SetWindowText(hDlg, NewTitle.c_str());
		}
		else 
		{
			g_pServer->Stop();
			delete g_pServer;
			g_pServer = NULL;			
		}
	}
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

void initDialog(HWND hDlg)
{
	g_hwndMainDlg = hDlg;

	char szTitle[64];
	ZeroMemory(szTitle, sizeof(szTitle));

	GetWindowText(hDlg, szTitle, 64);

	g_WindowsTitle = std::string(szTitle) + std::string(" - ") + sc::getFileVersion();

	SetWindowText(hDlg, g_WindowsTitle.c_str());
	
	setFirstFocus(hDlg);
}

void setFirstFocus(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_EDITCONSOLE);
	SetFocus(pWnd);
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
	return;
}

void cmdHideWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_SESSIONSERVER));
	lstrcpy(nid.szTip, "SessionServer");
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(hDlg, SW_HIDE);
	return;
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
	return;
}

void DeleteAndEndDialog(HWND hDlg)
{
	GLogicData::GetInstance().ClearData();
	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();

	EndDialog(hDlg,0);

	SAFE_DELETE ( g_pPath );
}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	g_hWindow = hDlg;

	switch(iMessage)
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

	case RAN_MSG_EXIT:
		cmdExit(hDlg);
		DeleteAndEndDialog(hDlg);
		break;

	case RAN_MSG_AUTH_FAIL:
		cmdStartOrStop(hDlg);
		//cmdExit(hDlg);
		DeleteAndEndDialog(hDlg);
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
			
		case IDC_CLOSE :
			if( MessageBox(hDlg,"Do you really want to exit?", "Warning", MB_YESNO | MB_ICONWARNING ) == IDYES){
				cmdExit(hDlg);			
				DeleteAndEndDialog(hDlg);
			}
			return TRUE;

		case IDC_OPEN : 
			cmdShowWindow(hDlg);
			return TRUE;

		case IDC_EDITCONSOLE :
			switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;
			break;
		
        case IDC_EDITCONSOLE_INFO :
            switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;
			break;		
        }
		return FALSE;
	}
	return FALSE;
}


