#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "../AutoPatchMan/GlobalVariable.h"

class CLPatchThread;
class CHttpPatch;
class CPatch;

const int WM_TRAY_NOTIFICATION	= WM_APP+2;
const int TRAYICON_ANIMATION_FRAME	= 4;
class CPreDownloadManDlg : public CDialog
{
public:
	CPreDownloadManDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_PRE_DOWNLOAD_MAN_DIALOG };


private:
	enum //	타이머 ID
	{
		E_PRE_TIMER_START_PATCH		= 1,
		E_PRE_TIMER_TRAYICON_ANIM	= 2,
	};

private:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

	void SetAppPath ();

	// 쓰레드 생성 함수;
	typedef CLPatchThread* LPCLPATCHTHREAD;

	void BEGIN_PRE_DOWNLOAD ();
	void BeginThread( int LoginPort, const CString& strParam = "" );
	void EndThread( LPCLPATCHTHREAD & pThread );
	void ForceThreadKill( LPCLPATCHTHREAD & pThread );

	// 트레이 아이콘 변경;
	void SetTrayIconImage( HICON hIcon );
	void TrayIconAnimation();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnTimer(UINT nIDEvent);

	LRESULT OnTrayNotification( WPARAM wParam, LPARAM lParam );

	void OnMenuStop();
	void OnMenuExit();

private:
	CLPatchThread * m_pPatchThread;

	NOTIFYICONDATA	m_nid;
	CHttpPatch*	m_pHttpPatch;

	HICON m_hIcon;
	HICON m_hTrayIcon[TRAYICON_ANIMATION_FRAME];
	BYTE m_nTrayIconFrame;
};