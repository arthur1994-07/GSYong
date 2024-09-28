// RanOnlineEdMainFrm.cpp : implementation of the RanOnlineEdFrame class
//

#include "stdafx.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Log/LogMan.h"

#include "../InternalCommonLib/dxstdafx.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxResponseMan.h"
#include "../Enginelib/GUInterface/GameTextControl.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogic/RANPARAM.h"

#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogicServer/DxServerInstance.h"

#include "SplashDlg.h"
#include "LeftView.h"
#include "RightView.h"
#include "RanOnlineEd.h"
#include "RanOnlineEdMainFrm.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "../MfcExLib/Splasher.h"   //스플래쉬

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 스플래쉬 화면을 출력하기위해 추가되었음
#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

class CMySplashWnd : public CSplashWnd
{
public:
    //Constructors / Destructors
    CMySplashWnd()
    {
        //Lets use an almost opaque splash window with some "cut-out" sections in it
        SetDropShadow();
        SetTransparent(RGB(255, 0, 255), LWA_ALPHA, 255);                       
    }

    DECLARE_DYNCREATE(CMySplashWnd);
};
IMPLEMENT_DYNCREATE(CMySplashWnd, CSplashWnd);
//////////////////////////////////////////////////////////////////////////

// RanOnlineEdFrame

IMPLEMENT_DYNCREATE(RanOnlineEdFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(RanOnlineEdFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// RanOnlineEdFrame construction/destruction

RanOnlineEdFrame::RanOnlineEdFrame()
    : m_pLeft(NULL)
    , m_pRight(NULL)
{
	// TODO: add member initialization code here
    RanOnlineEdApp* pApp = (RanOnlineEdApp*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(pApp->GetServiceProvider(), pApp->GetSubPath());
}

RanOnlineEdFrame::~RanOnlineEdFrame()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

int RanOnlineEdFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL RanOnlineEdFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
    // 스플래쉬 화면을 출력하기위해 추가되었음
    CSplashFactory factory;
    if (!factory.Create(RUNTIME_CLASS(CMySplashWnd)))
    {
        AfxMessageBox(_T("Failed to create splash screen"), MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    //////////////////////////////////////////////////////////////////////////


// 	return m_wndSplitter.Create(this,
// 		2, 2,               // TODO: adjust the number of rows, columns
// 		CSize(10, 10),      // TODO: adjust the minimum pane size
// 		pContext);
    
    // create splitter window
    if (!m_wndSplitter.CreateStatic(this, 1, 2))
        return FALSE;

    if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(LeftView), CSize(100, 100), pContext) ||
        !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(RightView), CSize(100, 100), pContext))
    {
        m_wndSplitter.DestroyWindow();
        return FALSE;
    }

    m_pLeft  = (LeftView*)  m_wndSplitter.GetPane(0, 0);
    m_pRight = (RightView*) m_wndSplitter.GetPane(0, 1);

    sc::LogMan::getInstance()->attachEditControl(m_pRight->GetEditCtrl(), NULL);

    //SplashDlg::ShowSplashScreen(NULL);
    GLOGIC::SetGlogicPackFile(FALSE);
    GLOGIC::SetGlogicZipFile(FALSE);
    GLOGIC::SetEngineZipFile(FALSE);

    std::string AppPath = sc::getAppPath();
    //SplashDlg::DisplayMessage(AppPath);

    //SplashDlg::DisplayMessage("DxResponseMan::GetInstance().OneTimeSceneInit");

    m_pEngineDevice->OneTimeSceneInit(
        AppPath.c_str(),
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

    //SplashDlg::DisplayMessage("GMTOOL::Create");
    //GMTOOL::m_CallBack.ProgressCallback = SplashDlg::DisplayMessage;
    RanOnlineEdApp* pApp = (RanOnlineEdApp*) AfxGetApp();
    //GMTOOL::Create(AppPath.c_str(), pApp->GetSubPath(), pApp->GetServiceProvider());
    // 스플래쉬 화면을 출력하기위해 추가되었음
    GMTOOL::Create(AppPath.c_str(), pApp->GetSubPath(), pApp->GetServiceProvider(), NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

    //sc::writeLogInfo(std::string("loading completed"));    


    // 스플래쉬 화면을 출력하기위해 추가되었음
    factory.Close();
    return TRUE;
}

BOOL RanOnlineEdFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// RanOnlineEdFrame diagnostics

#ifdef _DEBUG
void RanOnlineEdFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void RanOnlineEdFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// RanOnlineEdFrame message handlers

BOOL RanOnlineEdFrame::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (SplashDlg::PreTranslateAppMessage(pMsg))
        return TRUE;

    return CFrameWnd::PreTranslateMessage(pMsg);
}

void RanOnlineEdFrame::PostNcDestroy()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    m_pEngineDevice->FinalCleanup();
    GMTOOL::CleanUp();

	GLogicData::GetInstance().ClearData();

    CFrameWnd::PostNcDestroy();
}
