// EffectToolView.cpp : implementation of the CEffectToolView class
//

#include "stdafx.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../enginelib/Archive/Archive.h"

#include "./Properties/EffGroupPage.h"
#include "EffectTool.h"
#include "MainFrm.h"
#include "EffectToolDoc.h"
#include "EffectToolView.h"


extern BOOL				g_bFRAME_LIMIT;
extern BOOL				g_bImportMobile;


/////////////////////////////////////////////////////////////////////////////
// CEffectToolView

IMPLEMENT_DYNCREATE(CEffectToolView, CView)

BEGIN_MESSAGE_MAP(CEffectToolView, CView)
	//{{AFX_MSG_MAP(CEffectToolView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_ARCHIVE, &CEffectToolView::OnArchive)
	ON_COMMAND(ID_ARCHIVE_MOBILE, &CEffectToolView::OnArchiveMobile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView construction/destruction

CEffectToolView::CEffectToolView()
    : CD3DApplication(SP_OFFICE_TEST)
    , m_pGlobalStage(NULL)
    , m_pCharacter(NULL)
    , m_bCreated(FALSE)
	, m_spDxLandMan(new DxLandMan)
{
    m_bUseDepthBuffer = TRUE;
    CEffectToolApp* pApp = (CEffectToolApp*) AfxGetApp();
    SetServiceProvider(pApp->GetServiceProvider());
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
    m_pGlobalStage = new DxGlobalStage(pEngineDevice, pApp->GetSubPath(), true);
    
    if ( !m_pGlobalStage )
    {
        ASSERT(m_pGlobalStage != NULL);
        return;
    }

    GLGaeaClient* pGaeaClient = m_pGlobalStage->GetGaeaClient();

    if ( !pGaeaClient )
    {
        ASSERT(pGaeaClient != NULL);
        return;
    }

    m_pCharacter = pGaeaClient->GetCharacter();

    if ( !m_pCharacter )
    {
        ASSERT(m_pCharacter != NULL);
        return;
    }

    //m_dwMinDepthBits = 24;
    //m_dwMinStencilBits = 8;

	//// Note : 속도 제한 없음.
	//g_bFRAME_LIMIT = FALSE;

	// Note : 속도 제한 없음.
	g_bFRAME_LIMIT = FALSE;
}

CEffectToolView::~CEffectToolView()
{
    m_pCharacter = NULL;
    SAFE_DELETE ( m_pGlobalStage );
}

BOOL CEffectToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView drawing

void CEffectToolView::OnDraw(CDC* pDC)
{
	CEffectToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView printing

BOOL CEffectToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEffectToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CEffectToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView diagnostics

#ifdef _DEBUG
void CEffectToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEffectToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEffectToolDoc* CEffectToolView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectToolDoc)));
	return (CEffectToolDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView message handlers

void CEffectToolView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ( m_bCreated )	ReSizeWindow ( cx, cy );
}

void CEffectToolView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	if ( FAILED( CD3DApplication::Create ( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() ) ) )
	{
		m_bCreated = FALSE;
		return;
	}
	
	m_bCreated = TRUE;
}

void CEffectToolView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	CEffectToolApp *pApp = (CEffectToolApp*) AfxGetApp();
	pApp->SetActive ( bActivate );
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CEffectToolView::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//	Note : D3D APP 파괴.
	//
	Cleanup3DEnvironment();

	__super::PostNcDestroy();
}

void CEffectToolView::OnArchive()
{
	CEffectToolApp *pApp = (CEffectToolApp*) AfxGetApp();
	CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;    

	char pszPathname[MAX_PATH];
	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = 0;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("복사할 디렉토리를 선택하세요");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그를 띄우기
	ITEMIDLIST* pidlBrowse = ::SHBrowseForFolder(&BrInfo);   

	if( pidlBrowse != 0)
	{
		g_bImportMobile = FALSE;

		// 패스를 얻어옴
		::SHGetPathFromIDList(pidlBrowse, pszPathname);
		//EffectStateMachine::Archive(pszPathname);

		CMainFrame* pFrame			= (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CEffectToolView* pView		= (CEffectToolView*)pFrame->GetActiveView();
		LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();   

		const TCHAR* pFolder = 0;
		//kParam >> pFolder;
		pFolder = pszPathname;

		Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
		Archive::Archive( &CEffGroupPage::m_EffPropGroup );
	}    
}

void CEffectToolView::OnArchiveMobile()
{
	CEffectToolApp *pApp = (CEffectToolApp*) AfxGetApp();
	CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;    

	char pszPathname[MAX_PATH];
	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = 0;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("복사할 디렉토리를 선택하세요");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그를 띄우기
	ITEMIDLIST* pidlBrowse = ::SHBrowseForFolder(&BrInfo);   

	if( pidlBrowse != 0)
	{
		g_bImportMobile = TRUE;

		// 패스를 얻어옴
		::SHGetPathFromIDList(pidlBrowse, pszPathname);
		//CharStateMachine::Archive(pszPathname);

		CMainFrame* pFrame			= (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CEffectToolView* pView		= (CEffectToolView*)pFrame->GetActiveView();
		LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();   

		const TCHAR* pFolder = 0;
		//kParam >> pFolder;
		pFolder = pszPathname;

		Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
		Archive::Archive( &CEffGroupPage::m_EffPropGroup );
	}    
}