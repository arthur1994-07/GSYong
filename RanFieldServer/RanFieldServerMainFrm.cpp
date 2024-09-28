// RanFieldServerMainFrm.cpp : implementation of the RanFieldServerMainFrame class
//

#include "stdafx.h"
#include "../RanLogicServer/Server/s_CFieldServer.h"
#include "RanFieldServer.h"
#include "RanFieldServerMainFrm.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// RanFieldServerMainFrame

IMPLEMENT_DYNCREATE(RanFieldServerMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(RanFieldServerMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// RanFieldServerMainFrame construction/destruction

RanFieldServerMainFrame::RanFieldServerMainFrame()
    : m_pLeft(NULL)
    , m_pRightBottom(NULL)
    , m_pRightTop(NULL)
    , m_pServer(NULL)
{
	// TODO: add member initialization code here
}

RanFieldServerMainFrame::~RanFieldServerMainFrame()
{
}


int RanFieldServerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	return 0;
}

BOOL RanFieldServerMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
    m_wndSplitter.CreateStatic(
        this,
        1,
        2,
        WS_CHILD | WS_VISIBLE );

    // Left
    m_wndSplitter.CreateView(
        0,
        0,
        RUNTIME_CLASS(LeftView),
        CSize( 300, 10 ),
        pContext );

    // Right
    m_wndSplitterRight.CreateStatic(
        &m_wndSplitter,
        2,
        1,
        WS_CHILD | WS_VISIBLE,
        m_wndSplitter.IdFromRowCol(0, 1) );

    // Right Top
    m_wndSplitterRight.CreateView(
        0,
        0,
        RUNTIME_CLASS(RightTopView),
        CSize( 200, 400 ),
        pContext );

    // Right Bottom
    m_wndSplitterRight.CreateView(
        1,
        0,
        RUNTIME_CLASS(RightBottomView),
        CSize( 200, 200 ),
        pContext );

    // 생성된 pannel 의 포인터를 저장한다.	
    m_pLeft        = (LeftView*)        m_wndSplitter.GetPane( 0, 0 );
    m_pRightTop    = (RightTopView*)    m_wndSplitterRight.GetPane( 0, 0 );
    m_pRightBottom = (RightBottomView*) m_wndSplitterRight.GetPane( 1, 0 );
    
    return TRUE;
}

BOOL RanFieldServerMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// RanFieldServerMainFrame diagnostics

#ifdef _DEBUG
void RanFieldServerMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void RanFieldServerMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// RanFieldServerMainFrame message handlers
bool RanFieldServerMainFrame::StartServer()
{
    if (!m_pServer)
    {
        m_pServer = new CFieldServer(
            m_hWnd,
            m_pRightTop->GetEditCtrl(),
            m_pRightBottom->GetEditCtrl(),
            NULL, // 컴파일 에러 방지용. 나중에 고쳐야함
            SP_OFFICE_TEST); // 컴파일 에러 방지용. 나중에 고쳐야함
    }

    if (m_pServer->IsRunning())
        m_pServer->Stop();
    else
        m_pServer->Start();
    return true;
}

void RanFieldServerMainFrame::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_pServer)
    {
        m_pServer->Stop();
        SAFE_DELETE(m_pServer);
    }

    CFrameWnd::OnClose();
}
