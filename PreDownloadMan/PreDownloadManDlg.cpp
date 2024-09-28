#include "stdAfx.h"
#include <afxinet.h>
#include <process.h>

#include "../SigmaCore/Net/NetUtil.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/DebugSet.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"

#include "../RanLogicClient/Network/s_CPatch.h"

#include "PreDownloadMan.h"
#include "PreDownloadManDlg.h"
#include "PreDownloadDefine.h"
#include "PreDownloadThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPreDownloadManDlg::CPreDownloadManDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPreDownloadManDlg::IDD, pParent),
m_pHttpPatch(NULL),
m_pPatchThread(NULL),
m_nTrayIconFrame(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	m_hTrayIcon[0] = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1) );
	m_hTrayIcon[1] = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON2) );
	m_hTrayIcon[2] = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON3) );
	m_hTrayIcon[3] = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON4) );

	GLogicData::GetInstance().LoadCountryFile();
}

BEGIN_MESSAGE_MAP(CPreDownloadManDlg, CDialog)
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_COMMAND(ID_PRE_DOWNLODER_STOP, OnMenuStop)
	ON_COMMAND(ID_PRE_DOWNLODER_EXIT, OnMenuExit)
END_MESSAGE_MAP()

BOOL CPreDownloadManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetAppPath ();

	CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();

	RANPARAM::LOAD( PreDownloadDefine::g_strAppPath.GetString(), pApp->GetServiceProvider(), false, false );
	CDebugSet::OneTimeSceneInit(pApp->GetSubPath(), true);

	m_pHttpPatch = new CHttpPatch;
	if( NULL == m_pHttpPatch ) EndDialog(IDOK);

	BEGIN_PRE_DOWNLOAD();

	return TRUE;
}

void CPreDownloadManDlg::SetAppPath ()
{
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );

		if ( !strAppPath.IsEmpty() )
			if ( strAppPath.GetAt(0) == '"' )
				strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

		strAppPath += '\\';
		PreDownloadDefine::g_strAppPath = strAppPath;		
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );	
	PreDownloadDefine::g_strProFile = szPROFILE;
}

void CPreDownloadManDlg::BEGIN_PRE_DOWNLOAD()
{
	BeginThread( RANPARAM::LoginServerPort, RANPARAM::LoginAddress );
	SetTimer( E_PRE_TIMER_START_PATCH, 30, NULL );
	SetTimer( E_PRE_TIMER_TRAYICON_ANIM, 100, NULL );
}

void CPreDownloadManDlg::OnTimer(UINT nIDEvent)
{
	switch ( nIDEvent )
	{
	case E_PRE_TIMER_START_PATCH:
		{
			if( m_pPatchThread == NULL )
				break;
			
			if( m_pPatchThread->IsComplete() || m_pPatchThread->IsFail() )
				OnMenuExit();
		}
		break;

	case E_PRE_TIMER_TRAYICON_ANIM:
		{
			TrayIconAnimation();
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CPreDownloadManDlg::BeginThread( int LoginPort, const CString& strParam )
{
	if( m_pPatchThread != NULL )
	{
		GASSERT( 0 && "BeginThread()" );
		return;
	}

	m_pPatchThread = new CPreDownloadThread( m_pHttpPatch, AfxGetThread()->m_nThreadID );

	if( m_pPatchThread == NULL )
		return;

	ASSERT_VALID(pThread);
	m_pPatchThread->m_pThreadParams = NULL;

	if ( !m_pPatchThread->CreateThread(CREATE_SUSPENDED) )
	{
		delete m_pPatchThread;
		return;
	}

	VERIFY(m_pPatchThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	m_pPatchThread->ResumeThread();
}

void CPreDownloadManDlg::SetTrayIconImage( HICON hIcon )
{
	ZeroMemory(&m_nid, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = 0;
	m_nid.hWnd = m_hWnd;
	m_nid.hIcon = hIcon;

	int nTotalCount = 0;
	int nDownedCount = 0;

	if( m_pPatchThread != NULL )
	{
		nTotalCount = m_pPatchThread->m_nDownloadFileCount;
		nDownedCount = m_pPatchThread->m_nDownloadedFileCount;
	}
	
	CString	strDownloadProgress = sc::string::format("%1%/%2%", nDownedCount, nTotalCount).c_str();

	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

	if( nDownedCount ==  nTotalCount )
		lstrcpy(m_nid.szTip, "PreDownload");
	else
		lstrcpy(m_nid.szTip, strDownloadProgress);

	m_nid.uCallbackMessage = WM_TRAY_NOTIFICATION;

	Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

void CPreDownloadManDlg::TrayIconAnimation()
{
	++m_nTrayIconFrame;
	if( m_nTrayIconFrame > TRAYICON_ANIMATION_FRAME - 1 )
		m_nTrayIconFrame = 0;
	SetTrayIconImage( m_hTrayIcon[m_nTrayIconFrame] );
}

void CPreDownloadManDlg::PostNcDestroy()
{
	EndThread( m_pPatchThread );

	SAFE_DELETE ( m_pHttpPatch );

	Shell_NotifyIcon(NIM_DELETE, &m_nid);

	for( int i=0; i< TRAYICON_ANIMATION_FRAME; ++i )
		DestroyIcon( m_hTrayIcon[i] );

	DestroyIcon( m_hIcon );

	sc::net::closeNetwork();
	CDebugSet::FinalCleanup ();
	CDialog::PostNcDestroy();
}

void CPreDownloadManDlg::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
	// 윈도우 켜짐과 동시에 숨기기;
	lpwndpos->flags &= ~SWP_SHOWWINDOW;

	CDialog::OnWindowPosChanging(lpwndpos);

	ZeroMemory(&m_nid, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = 0;
	m_nid.hWnd = m_hWnd;
	m_nid.hIcon = m_hTrayIcon[0];

	int nTotalCount = 0;
	int nDownedCount = 0;

	if( m_pPatchThread != NULL )
	{
		nTotalCount = m_pPatchThread->m_nDownloadFileCount;
		nDownedCount = m_pPatchThread->m_nDownloadedFileCount;
	}

	CString strDownloadProgress = sc::string::format("%1%/%2%", nDownedCount, nTotalCount).c_str();
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	if( nDownedCount ==  nTotalCount )
		lstrcpy(m_nid.szTip, "PreDownload");
	else
		lstrcpy(m_nid.szTip, strDownloadProgress);
	m_nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	Shell_NotifyIcon(NIM_ADD, &m_nid);
}

LRESULT CPreDownloadManDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	int nTotalCount = 0;
	int nDownedCount = 0;

	switch(lParam)
	{
	case WM_RBUTTONDOWN:
		{
			::SetForegroundWindow(m_hWnd);
			::PostMessage(m_hWnd, WM_NULL, 0, 0);
			CPoint ptMouse;
			::GetCursorPos(&ptMouse);
			CMenu menu;
			menu.LoadMenu(IDR_MENU1);
			CMenu *pMenu = menu.GetSubMenu(0);
			if( m_pPatchThread->IsPause() )
				pMenu->ModifyMenu( ID_PRE_DOWNLODER_STOP, MF_BYCOMMAND, ID_PRE_DOWNLODER_STOP, PreDownloadDefine::g_strMenuResume );
			else
				pMenu->ModifyMenu( ID_PRE_DOWNLODER_STOP, MF_BYCOMMAND, ID_PRE_DOWNLODER_STOP, PreDownloadDefine::g_strMenuPause );
			pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, AfxGetMainWnd() );
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			Shell_NotifyIcon(NIM_MODIFY , &m_nid);
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( m_pPatchThread != NULL )
			{
				nTotalCount = m_pPatchThread->m_nDownloadFileCount;
				nDownedCount = m_pPatchThread->m_nDownloadedFileCount;
			}

			CString strDownloadProgress = sc::string::format("%1%/%2%", nDownedCount, nTotalCount).c_str();
			m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			if( nDownedCount ==  nTotalCount )
				lstrcpy(m_nid.szTip, "PreDownload");
			else
				lstrcpy(m_nid.szTip, strDownloadProgress);
			m_nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
			Shell_NotifyIcon(NIM_MODIFY, &m_nid);
		}
		break;
	}
	return 1;
}

void CPreDownloadManDlg::EndThread( LPCLPATCHTHREAD & pThread )
{
	if( pThread != NULL )
	{
		pThread->KillThread();
		pThread = NULL;
	}
}

void CPreDownloadManDlg::ForceThreadKill( LPCLPATCHTHREAD & pThread )
{
	if( pThread != NULL )
	{
		pThread->ForceKillThread();
		pThread = NULL;
	}
}

void CPreDownloadManDlg::OnMenuStop()
{
	if( m_pPatchThread == NULL )
		return;

	if( m_pPatchThread->IsPause() )
	{
		m_pPatchThread->Resume();

		SetTimer( E_PRE_TIMER_TRAYICON_ANIM, 1000, NULL );
	}
	else
	{
		m_pPatchThread->Pause();

		m_nTrayIconFrame = 0;
		SetTrayIconImage( m_hTrayIcon[m_nTrayIconFrame] );
		KillTimer( E_PRE_TIMER_TRAYICON_ANIM );
	}
}

void CPreDownloadManDlg::OnMenuExit()
{
	EndThread( m_pPatchThread );
	CDialog::EndDialog(0);
}