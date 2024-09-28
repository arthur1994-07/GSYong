// LevelEditView.cpp : CLevelEditView 클래스의 구현
//

#include "stdafx.h"

#include "../Enginelib/DxResponseMan.h"
#include "./LevelEdit.h"
#include "./LevelEditDoc.h"
#include "./LevelEditView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL				g_bFRAME_LIMIT;


// CLevelEditView

IMPLEMENT_DYNCREATE(CLevelEditView, CView)

BEGIN_MESSAGE_MAP(CLevelEditView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()
//	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CLevelEditView 생성/소멸

CLevelEditView::CLevelEditView()
    : CD3DApplication(SP_OFFICE_TEST)
    , m_bCreated(FALSE)
    , m_p3dPosMesh(NULL)
    , m_pDxEditmat(NULL)
    , m_vCollisionPos(0,0,0)
	, m_spDxLandMan(new DxLandMan)
{
	// TODO: 여기에 생성 코드를 추가합니다.
    CLevelEditApp* pApp = (CLevelEditApp*) AfxGetApp();
    SetServiceProvider(pApp->GetServiceProvider());
    m_pEngineDevice = new DxResponseMan(m_ServiceProvider, pApp->GetSubPath());
	m_bUseDepthBuffer	= TRUE;

	//// Note : 속도 제한 없음.
	//g_bFRAME_LIMIT = FALSE;

	// 속도 제한
	g_bFRAME_LIMIT = TRUE;
}

CLevelEditView::~CLevelEditView()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

BOOL CLevelEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CLevelEditView 그리기

void CLevelEditView::OnDraw(CDC* /*pDC*/)
{
	CLevelEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CLevelEditView 인쇄

BOOL CLevelEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CLevelEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CLevelEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CLevelEditView 진단

#ifdef _DEBUG
void CLevelEditView::AssertValid() const
{
	CView::AssertValid();
}

void CLevelEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLevelEditDoc* CLevelEditView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLevelEditDoc)));
	return (CLevelEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CLevelEditView 메시지 처리기

void CLevelEditView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	//	Note : D3D APP 생성.
	//
	CD3DApplication::Create( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() );
	m_bCreated = TRUE;
}

void CLevelEditView::PostNcDestroy()
{
	//	Note : D3D APP 파괴.
	//
	CD3DApplication::Cleanup3DEnvironment();

	__super::PostNcDestroy();
}

void CLevelEditView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( m_bCreated ) OnSize(cx,cy);
}

void CLevelEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CLevelEditApp *pApp = (CLevelEditApp*) AfxGetApp();
	pApp->SetActive ( bActivate );

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
