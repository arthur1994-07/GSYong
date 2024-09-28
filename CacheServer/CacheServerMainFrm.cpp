// CacheServerMainFrm.cpp : implementation of the CacheServerMainFrame class
//

#include "stdafx.h"
#include <Strsafe.h>
#include "../../RanThirdParty/DirectX/Include/d3d9.h"

#ifndef LPDIRECT3DDEVICEQ
#define LPDIRECT3DDEVICEQ LPDIRECT3DDEVICE9
#endif

#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicServer/Server/CacheServer.h"
#include "../RanLogicServer/Net/NetLogDefine.h"

#include "LeftView.h"
#include "RightTopView.h"
#include "RightBottomView.h"
#include "CacheServer.h"
#include "CacheServerMainFrm.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CacheServerMainFrame

extern HWND g_hwndMainDlg;

IMPLEMENT_DYNCREATE(CacheServerMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CacheServerMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_WM_CLOSE()
	ON_MESSAGE(WM_UPDATE_LOG_CONSOLE, OnUpdateLogConsole)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CacheServerMainFrame construction/destruction

CacheServerMainFrame::CacheServerMainFrame()
    : m_pServer(NULL)
{
	// TODO: add member initialization code here
}

CacheServerMainFrame::~CacheServerMainFrame()
{
	if (m_pServer)
	{
		if (m_pServer->IsRunning())
			m_pServer->Stop();
		delete m_pServer;
		m_pServer = NULL;
	}
}


int CacheServerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	g_hwndMainDlg = m_hWnd;

	return 0;
}

BOOL CacheServerMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
    m_wndSplitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE);

    // Left
    m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(LeftView), CSize( 300, 10 ), pContext);

    // Right
    m_wndSplitterRight.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 1));

    // Right Top
    m_wndSplitterRight.CreateView(0, 0, RUNTIME_CLASS(RightTopView), CSize(200, 400), pContext);

    // Right Bottom
    m_wndSplitterRight.CreateView(1, 0, RUNTIME_CLASS(RightBottomView), CSize(200, 200), pContext);

    // 생성된 pannel 의 포인터를 저장한다.	
    m_pLeftView        = (LeftView*)        m_wndSplitter.GetPane(0, 0);
    m_pRightTopView    = (RightTopView*)    m_wndSplitterRight.GetPane(0, 0);
    m_pRightBottomView = (RightBottomView*) m_wndSplitterRight.GetPane(1, 0);
    return TRUE;
}

LRESULT CacheServerMainFrame::OnUpdateLogConsole(WPARAM wParam, LPARAM lParam)
{
	char *pBuffer = (char *)lParam;

	HWND hEditConsole = (HWND)wParam;

	::SendMessage(hEditConsole, WM_SETFOCUS,	(WPARAM) 0, 0);
	::SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX);
	::SendMessage(hEditConsole, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0);
	::SendMessage(hEditConsole, EM_SCROLLCARET,	(WPARAM) 0,				(LPARAM) 0);
	::SendMessage(hEditConsole, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) lParam);
	::SendMessage(hEditConsole, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1);
	::SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX);

	//SAFE_DELETE_ARRAY(pBuffer);

	return 0;
}

BOOL CacheServerMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CacheServerMainFrame diagnostics

#ifdef _DEBUG
void CacheServerMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CacheServerMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CacheServerMainFrame message handlers
void CacheServerMainFrame::ClearLogMessage()
{
    if (m_pRightTopView)
        m_pRightTopView->ClearMessage();
  //if (m_pRightBottomView)
  //    m_pRightBottomView->ClearMessage();
}

bool CacheServerMainFrame::StartServer()
{
    CacheServerApp* pApp = (CacheServerApp*) AfxGetApp();

    if (!m_pServer)
    {
        m_pServer = new CacheServer(
            m_hWnd,
            m_pRightTopView->GetEditCtrl(),
            m_pRightBottomView->GetEditCtrl(),
            pApp->GetSubPath(),
            pApp->GetServiceProvider());
    }

    if (m_pServer->IsRunning())
    {
        if (IDYES == MessageBox("Really want to stop?", "Warning", MB_YESNO))
        {
            m_pServer->Stop();
            delete m_pServer;
            m_pServer = NULL;
        }
    }
    else
    {
        m_pServer->Start();
    }

    if (m_pServer && m_pServer->IsRunning())
    {
        m_pLeftView->SetStartButtonText(std::string("Stop"));
		
		char TitleText[MAX_PATH] = {0};
		pApp->GetMainWnd()->GetWindowText(TitleText, MAX_PATH);

		if ( m_pServer->IsIntegrationServer() )
		{
			std::string strTitle = std::string( "Integration CacheServer") + std::string(" - ") + sc::getFileVersion();
			pApp->GetMainWnd()->SetWindowText( strTitle.c_str() );
		}

		if (GLogicData::GetInstance().GetServiceProvider() == SP_WORLD_BATTLE)
		{
			std::string NewTitle("World Battle ");
			NewTitle.append(TitleText);
			pApp->GetMainWnd()->SetWindowText(NewTitle.c_str());
		}

		if (m_pServer->GetServiceProvider() != GLogicData::GetInstance().GetServiceProvider())
		{
			std::string NewTitle("country.lua and server config xml. service provider mismatch ");
			NewTitle.append(TitleText);
			pApp->GetMainWnd()->SetWindowText(NewTitle.c_str());
		}
    }
    else
    {
        m_pLeftView->SetStartButtonText(std::string("Start"));
    }

    return true;
}

bool CacheServerMainFrame::IsServerRunning()
{
    if (m_pServer && m_pServer->IsRunning())
        return true;
    else
        return false;
}

void CacheServerMainFrame::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_pServer)
    {
		if (m_pServer->IsRunning())
		{
			MessageBox("First stop the server", "Warning", MB_OK);
			return;
		}
		SAFE_DELETE(m_pServer);
    }
    CFrameWnd::OnClose();
}

void CacheServerMainFrame::LogItemExchange()
{
    if (m_pServer)
    {
        LOG_ITEM_EXCHANGE Data;
        NET_LOG_ITEM_EXCHANGE Msg(Data);
        m_pServer->InsertMsg(DBACTION_CLIENT_ID, &Msg);
    }
}

void CacheServerMainFrame::AdoTest1()
{
	if (m_pServer)
        m_pServer->AdoTest1();
}

void CacheServerMainFrame::AdoTest2()
{
    if (m_pServer)
        m_pServer->AdoTest2();
}

void CacheServerMainFrame::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	GLogicData::GetInstance().ClearData();

	BTWindow<CFrameWnd>::PostNcDestroy();
}
