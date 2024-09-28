// PageWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicWnd window

#pragma once

#include "../BugTrap/BugTrap.h"
#include "../BugTrap/BTMfcWindow.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxLand/DxLandMan.h"

class DxGlobalStage;
class CBasicApp;
class EngineDeviceMan;

//class CBasicWnd : public CWnd, public CD3DApplication
class CBasicWnd : public BTWindow<CWnd>, public CD3DApplication
{
private:    
    DxGlobalStage* m_pGlobalStage;
    EngineDeviceMan* m_pEngineDevice;

	int			m_nScreenWinX;
	int			m_nScreenWinY;
	int			m_wndSizeX;
	int			m_wndSizeY;

	HCURSOR		m_hCursorDefault;

	BOOL		m_bCreated;
	std::string m_strTEXTURE;

	UINT_PTR	m_nGGTimer;
	UINT_PTR	m_nGGATimer;
	UINT_PTR	m_nGGA12Timer;

	HANDLE		m_hMutex;

	BOOL		m_bForegroundWindow;
	float		m_fEmptyWorkingSetTime;

	bool		m_bLoginProcessLog;

private:
	DWORD			m_dwDelayGPU;
	DWORD			m_dwTimeGetTimePrev;

public:
	static DxGlobalStage* g_pGlobalStage;

	LPDIRECT3DDEVICEQ GetD3dDevice() { return m_pd3dDevice; }
    DxGlobalStage* GetGlobalStage() { return m_pGlobalStage; }
    EngineDeviceMan* GetEngineDevice() { return m_pEngineDevice; }
	HWND			GetHwndApp()		{ return m_hWndApp; }

public:
	void SetForegroundWindow( BOOL bForegroundWindow )	{ m_bForegroundWindow = bForegroundWindow; }

public:
	HRESULT InitDeviceObjects();

private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT CreateObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove( BOOL bNotRendering );
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	HRESULT RenderText();
	HRESULT ReSizeWindow ( int cx, int cy );

	virtual HRESULT Resize3DEnvironment();


public:
	virtual HRESULT FrameMove3DEnvironment();
	virtual HRESULT Render3DEnvironment();

// Construction
public:
	CBasicWnd(EMSERVICE_PROVIDER emServiceProvider, bool bLoginProcessLog);

// Attributes
public:
  CBasicApp* m_pApp;

// Operations
public:
  BOOL Create();
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	virtual ~CBasicWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicWnd)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNetNotify(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
#if _MSC_VER >= 1200
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

private:
    void    CheckPacketSniffer( float fElapsedTime );
};

/////////////////////////////////////////////////////////////////////////////
