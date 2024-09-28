// EmulatorView.h : iCEmulatorView Ŭ������ �������̽�
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

protected: // serialization������ ��������ϴ�.
	CEmulatorView();
	DECLARE_DYNCREATE(CEmulatorView)

// Ư��
public:
	CEmulatorDoc* GetDocument() const;

// �۾�
public:

// ������
	public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ����
public:
	virtual ~CEmulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
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

#ifndef _DEBUG  // EmulatorView.cpp�� ����� ����
inline CEmulatorDoc* CEmulatorView::GetDocument() const
   { return reinterpret_cast<CEmulatorDoc*>(m_pDocument); }
#endif

