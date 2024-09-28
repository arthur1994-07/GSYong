// LevelEditView.cpp : CLevelEditView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()
//	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CLevelEditView ����/�Ҹ�

CLevelEditView::CLevelEditView()
    : CD3DApplication(SP_OFFICE_TEST)
    , m_bCreated(FALSE)
    , m_p3dPosMesh(NULL)
    , m_pDxEditmat(NULL)
    , m_vCollisionPos(0,0,0)
	, m_spDxLandMan(new DxLandMan)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
    CLevelEditApp* pApp = (CLevelEditApp*) AfxGetApp();
    SetServiceProvider(pApp->GetServiceProvider());
    m_pEngineDevice = new DxResponseMan(m_ServiceProvider, pApp->GetSubPath());
	m_bUseDepthBuffer	= TRUE;

	//// Note : �ӵ� ���� ����.
	//g_bFRAME_LIMIT = FALSE;

	// �ӵ� ����
	g_bFRAME_LIMIT = TRUE;
}

CLevelEditView::~CLevelEditView()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

BOOL CLevelEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CLevelEditView �׸���

void CLevelEditView::OnDraw(CDC* /*pDC*/)
{
	CLevelEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CLevelEditView �μ�

BOOL CLevelEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CLevelEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CLevelEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CLevelEditView ����

#ifdef _DEBUG
void CLevelEditView::AssertValid() const
{
	CView::AssertValid();
}

void CLevelEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLevelEditDoc* CLevelEditView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLevelEditDoc)));
	return (CLevelEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CLevelEditView �޽��� ó����

void CLevelEditView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	//	Note : D3D APP ����.
	//
	CD3DApplication::Create( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() );
	m_bCreated = TRUE;
}

void CLevelEditView::PostNcDestroy()
{
	//	Note : D3D APP �ı�.
	//
	CD3DApplication::Cleanup3DEnvironment();

	__super::PostNcDestroy();
}

void CLevelEditView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if ( m_bCreated ) OnSize(cx,cy);
}

void CLevelEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CLevelEditApp *pApp = (CLevelEditApp*) AfxGetApp();
	pApp->SetActive ( bActivate );

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
