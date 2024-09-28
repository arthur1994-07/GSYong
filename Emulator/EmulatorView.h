// EmulatorView.h : iCEmulatorView 클래스의 인터페이스
//


#pragma once

#include "../EngineLib/DxCommon/D3DApp.h"
#include "../RanLogicServer/EmulServer.h"
#include "ServerDialogue.h"
#include "SettingIni.h"

class CAgentServer;

class EngineDeviceMan;
class DxGlobalStage;
class DxDirectMsgServer;
class GLGaeaServer;
class GLChar;

class CEmulatorView : public CView, public CD3DApplication, public CSettingIni
{
private:
	BOOL		m_bCreated;
	CString		m_strCharSetFile;
	CString		m_strMapList;
	bool		m_bEnterGmLevel;
	HCURSOR		m_hCursorDefault;
    bool        m_bAuto;

protected:
	CServerDialogue m_serverDialogue;
	CAgentServer* m_pAgentServer;
    DxGlobalStage* m_pGlobalStage;
    DxDirectMsgServer* m_pMsgServer;
    EmulServerField* m_pEmulServerField;
    GLGaeaServer* m_pGaeaServer;
	GLChar* m_pServerChar;
    EngineDeviceMan* m_pEngineDevice;

private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT CreateObjects();
	HRESULT InitDeviceObjects();	
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove( BOOL bNotRendering );
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	HRESULT RenderText();
	HRESULT ReSizeWindow ( int cx, int cy )
	{
		if ( cx < 10 )	cx = 10;
		if ( cy < 10 )	cy = 10;

		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

public:
	void SetActive ( BOOL bActive );

public:
	virtual HRESULT FrameMove3DEnvironment();
	virtual HRESULT Render3DEnvironment();

protected: // serialization에서만 만들어집니다.
	CEmulatorView();
	DECLARE_DYNCREATE(CEmulatorView)

// 특성
public:
	CEmulatorDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 구현
public:
	virtual ~CEmulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:	
	afx_msg void OnAppPlay();
    afx_msg void OnUpdateMenuAuto(CCmdUI *pCmdUI);
    afx_msg void OnMenuAuto();
};

#ifndef _DEBUG  // EmulatorView.cpp의 디버그 버전
inline CEmulatorDoc* CEmulatorView::GetDocument() const
   { return reinterpret_cast<CEmulatorDoc*>(m_pDocument); }
#endif

