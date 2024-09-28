#include "stdafx.h"
#include "resource.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../=TBB/include/tbb/tbb_allocator.h"
#include "../=TBB/include/tbb/task_scheduler_init.h"
#include "../=TBB/include/tbb/tbbmalloc_proxy.h"

#endif

#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/SigmaCore.h"
#include "../RanLogic/s_NetGlobal.h"
#include "../SigmaCore/gassert.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../ServerController/ServerControllerMsgDefine.h"
#include "../MfcExLib/MinBugTrap.h"
#include "./InstanceServer.h"

//#include "../=VLD/include/vld.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



#define MAX_LOADSTRING 100
extern HWND	g_hwndMainDlg;

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
	
	//open a console for Squirrel script debugging	
	/*
#ifdef _RELEASED
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
#endif
	*/

    // 순수 가상함수 호출 감지 핸들러.
    _set_purecall_handler(smtm_PurecallHandler);
    GLogicData::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall(GLogicData::GetInstance().GetServiceProvider(), std::string(_T("InstanceServer")));	
    g_pPath = new SUBPATH(GLogicData::GetInstance().GetServiceProvider());
	g_hInst = hInstance;
	g_pServer = NULL;
	g_bStopReserve = false;
	SERVER_CONTROLLER::checkCmdParameter(lpCmdLine);

	GLOGIC::SetGlogicPackFile(FALSE);
	GLOGIC::SetGlogicZipFile(TRUE);
	GLOGIC::SetEngineZipFile(TRUE);

	sc::initialize();

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);

	SAFE_DELETE(g_pPath);

	return 0;
}

void setFirstFocus(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_EDITCONSOLE);
	SetFocus(pWnd);
}

void initDialog(HWND hDlg)
{
	g_hwndMainDlg = hDlg;

	std::string strTitle;
	char szTitle[64];
	ZeroMemory(szTitle, sizeof(szTitle));

	GetWindowText(hDlg, szTitle, 64);

	strTitle = std::string(szTitle) + std::string(" - ") + sc::getFileVersion();

	SetWindowText(hDlg, strTitle.c_str());


	HWND pWnd = NULL;
	pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
	EnableWindow(pWnd, TRUE);

	setFirstFocus(hDlg);

	::SetTimer(hDlg, 
		SERVER_CONTROLLER_TIMER_ID,
		SERVER_CONTROLLER_TIMER_MILLISECOND,
		NULL);
}

void cmdStartOrStop(HWND hDlg)
{
	int nRetCode = 0;
	// 정지 예약 상태
	if (g_bStopReserve == true) {
		return;
	}

	// Server is running
	if (g_pServer != NULL && g_pServer->IsRunning())
	{					
		g_bStopReserve = true;
		g_pServer->Stop();
		SetDlgItemText(hDlg, IDC_BTN_STOP, "Waiting...");		

		while (g_pServer->GetReservationServerStop() == true)
		{
			Sleep( 1 );
		}
		g_pServer->FinalStop();
		SAFE_DELETE(g_pServer);

		SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
	}
	// Server is not running
	else
	{				
		g_pServer = new InstanceServer(
            hDlg,
            GetDlgItem(hDlg, IDC_EDITCONSOLE),
            GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO),
            g_pPath,
            GLogicData::GetInstance().GetServiceProvider());
		nRetCode = g_pServer->Start();
		if ( nRetCode == NET_OK )
		{
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");			
		}
		else 
		{
			g_pServer->Stop();
			while (g_pServer->GetReservationServerStop() == true)
			{
				Sleep( 1 );
			}
			g_pServer->FinalStop();
			SAFE_DELETE(g_pServer);
		}
	}
}

void cmdExit(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		g_pServer->Stop();
		while (g_pServer->GetReservationServerStop() == true)
		{
			Sleep( 1 );
		}
		g_pServer->FinalStop();
        delete g_pPath;
        g_pPath = NULL;
	}
	else
	{
		EndDialog(hDlg,0);
	}
}

void cmdFinalExit(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		g_pServer->FinalStop ();
        SAFE_DELETE(g_pServer);
	}
	if (g_bStopReserve == true)
	{
		// Stop
		g_bStopReserve = false;
	}
	else
	{
		// Exit
		EndDialog(hDlg,0);
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
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_INSTANCESERVER));
	lstrcpy(nid.szTip, "InstanceServer");
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

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	g_hWindow = hDlg;
	HWND pWnd = NULL;	
	
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
		break;
	
	case RAN_MSG_START:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_STOP:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_EXIT:
		pWnd = GetDlgItem(hDlg, IDC_CLOSE);
		EnableWindow(pWnd, FALSE);
		cmdExit(hDlg);
		// EndDialog(hDlg, 0);
		break;

	case WM_FINALCLEANUPFINISH :
		cmdFinalExit(hDlg);
		return true;

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
				pWnd = GetDlgItem(hDlg, IDC_CLOSE);
				EnableWindow(pWnd, FALSE);
				cmdExit(hDlg);			
			}
			return TRUE;
		
		case IDC_OPEN :
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

        case IDC_BTN_CLEAR_IP_BLOCK_LIST:
            if (g_pServer && g_pServer->IsRunning())
                g_pServer->ClearBlockIp();
		}
		return FALSE;
	}
	return FALSE;
}