#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "../RanLogicServer/Server/MatchServer.h"

class SUBPATH;

// value
HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

HWND g_hEditBox = NULL;
HWND g_hWindow = NULL;
BOOL g_bConnect = FALSE;
SUBPATH* g_pPath = NULL;

MatchServer* g_pServer = NULL;

// function
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
void setFirstFocus(HWND hDlg);
void cmdStartOrStop(HWND hDlg);
void cmdExit(HWND hDlg);
void initDialog(HWND hDlg);
void cmdShowWindow(HWND hDlg);
void cmdHideWindow(HWND hDlg);
void cmdPopupMenu(HWND hDlg);

#define WM_TRAY_NOTIFICATION WM_APP+1

