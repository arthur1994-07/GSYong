// CharEditView.cpp : implementation of the CCharEditView class
//

#include "stdafx.h"

#include "../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../InternalCommonLib/Interface/DebugSetInterface.h"

#include "../EngineLib/Interface/ISerialFile.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxTools/D3Dfont.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/GammaControl.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxResponseMan.h"

#include "CharEdit.h"
#include "CharEditDoc.h"
#include "CharEditView.h"
#include "MainFrm.h"
#include "State/CharStateMachine.h"
#include "Properties/sheetWithTabChar.h"

#include "../EngineLib/Common/DXInputString.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL				g_bFRAME_LIMIT;


/////////////////////////////////////////////////////////////////////////////
// CCharEditView

IMPLEMENT_DYNCREATE(CCharEditView, CView)

BEGIN_MESSAGE_MAP(CCharEditView, CView)
	//{{AFX_MSG_MAP(CCharEditView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_VIEWFULLSIZE, OnViewfullsize)
	ON_COMMAND(ID_TOOLBAR_RELOAD, OnReload)	
    ON_COMMAND(ID_ARCHIVE, &CCharEditView::OnArchive)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharEditView construction/destruction

CCharEditView::CCharEditView()
	: CD3DApplication(SP_OFFICE_TEST)
	, m_bCreated(FALSE)
	, m_pAttBoneLinkSkeleton(NULL)
	, m_spLandMan(new DxLandMan)
{    
	m_bUseDepthBuffer = TRUE;
    CCharEditApp* pApp = (CCharEditApp*) AfxGetApp();
    m_pEngineDevice = new DxResponseMan(SP_OFFICE_TEST, pApp->GetSubPath());

	//// Note : 속도 제한 없음.
	//g_bFRAME_LIMIT = FALSE;

	// 속도 제한
	g_bFRAME_LIMIT = TRUE;
}

CCharEditView::~CCharEditView()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;    
}

BOOL CCharEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCharEditView drawing

void CCharEditView::OnDraw(CDC* pDC)
{
	CCharEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CCharEditView printing

BOOL CCharEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCharEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CCharEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CCharEditView diagnostics

#ifdef _DEBUG
void CCharEditView::AssertValid() const
{
	CView::AssertValid();
}

void CCharEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCharEditDoc* CCharEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCharEditDoc)));
	return (CCharEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCharEditView message handlers

void CCharEditView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	//	Note : D3D APP 생성.
	//
	CD3DApplication::Create( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() );
	m_bCreated = TRUE;
}

void CCharEditView::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class

	CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
	pApp->ShutDown();

	// 감마 컨트롤 롤백.
	GammaControl::GetInstance().RollBack( m_hWnd );

	//	Note : D3D APP 파괴.
	//
	CD3DApplication::Cleanup3DEnvironment();
	
	CView::PostNcDestroy();
}


void CCharEditView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ( m_bCreated ) OnSize(cx,cy);
}

void CCharEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
	pApp->SetActive ( bActivate );

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CCharEditView::OnViewfullsize()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	DxSkinMesh9* pSkinMesh = m_SkinObject.GetSkinMesh ();
	if ( pSkinMesh )
	{
		DxViewPort::GetInstance().CameraJump ( pSkinMesh->GetCenter() );
			
		float fCameraLength = DxViewPort::GetInstance().GetDistance();

		DxViewPort::GetInstance().CameraZoom ( pSkinMesh->GetRadius()*3.6f-fCameraLength );
	}
}

void CCharEditView::OnReload()
{
	TextureManager::GetInstance().DeleteCacheDataAll();

	CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
	CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;
	int nAtvIndex = pFrame->m_cDlgEdit.GetActiveIndex();
	CharStateMachine::Reload(nAtvIndex);
}


void CCharEditView::OnArchive()
{
    CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
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
        // 패스를 얻어옴
        ::SHGetPathFromIDList(pidlBrowse, pszPathname);
        CharStateMachine::Archive(pszPathname);
    }    
}

DxSkinPiece* CCharEditView::GetSkinPiece(int nParts)
{
#ifdef DEF_SKIN_SYNC
    CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
    CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;

    if (pFrame == 0)
    {
        return 0;
    }

    CsheetWithTabChar* pSheetWithTab = pFrame->m_cDlgEdit.GetSheetChar();;

    if (pSheetWithTab && pSheetWithTab->m_pCurPieceContainer)
    {
        return pSheetWithTab->m_pCurPieceContainer->GetSkinPiece(nParts);        
    }

    return 0;
#else
    return &m_SkinPiece;
#endif
}

void CCharEditView::LoadLandFile( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLandName )
{
	m_spLandMan->LoadFile ( strLandName.c_str(), pd3dDevice );
	m_spLandMan->ActiveMap( m_spLandMan );
}

void CCharEditView::EraseLand()
{
	m_spLandMan->CleanUp();
}

