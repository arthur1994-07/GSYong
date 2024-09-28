#ifndef FIELD_SERVER_H_
#define FIELD_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "../RanLogicServer/Server/s_CFieldServer.h"

class SUBPATH;

// value
// value
HINSTANCE			g_hInst = NULL;
HWND				g_hWnd = NULL;

HWND				g_hEditBox = NULL;
HWND				g_hWindow = NULL;
BOOL				g_bConnect = FALSE;

bool                g_bStopReserve = false;
bool                g_bRestartReserve = false;

CFieldServer*		g_pServer = NULL;
SUBPATH*            g_pPath = NULL;

// function
void setFirstFocus(HWND hDlg);
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
void initDialog(HWND hDlg);
void cmdStartOrStop(HWND hDlg);
void cmdExit(HWND hDlg);
void cmdFinalExit(HWND hDlg);
void cmdShowWindow(HWND hDlg);
void cmdHideWindow(HWND hDlg);
void cmdPopupMenu(HWND hDlg);

void ServerReStartByCommandLine();

#define WM_TRAY_NOTIFICATION			WM_APP+2

#endif // FIELD_SERVER_H_