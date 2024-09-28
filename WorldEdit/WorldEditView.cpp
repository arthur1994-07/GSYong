// WorldEditView.cpp : implementation of the CWorldEditView class
//
#include "stdafx.h"
#include "WorldEdit.h"

#include "../EngineLib/GUInterface/Cursor.h"
#include "../EngineLib/Interface/ITextureManager.h"
#include "../EngineLib/Interface/ISerialFile.h"
#include "../EngineLib/DxEffect/DxLoadShader.h"
#include "../EngineLib/DxEffect/Frame/DxEffectFrame.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxCustomTypes.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxLand/DxPrefabMan.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/Common/SubPath.h"

#include "MainFrm.h"
#include "sheetWithTab.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "State/WorldStateMachine.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL					g_bFRAME_LIMIT;


/////////////////////////////////////////////////////////////////////////////
// CWorldEditView

IMPLEMENT_DYNCREATE(CWorldEditView, CView)

BEGIN_MESSAGE_MAP(CWorldEditView, CView)
	//{{AFX_MSG_MAP(CWorldEditView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_TOOLBAR_RELOAD, OnReload)	
    ON_COMMAND(ID_ARCHIVE, OnArchive)
    ON_COMMAND(ID_PSF_SAVE, &CWorldEditView::OnPsfSave)
    ON_COMMAND(ID_PSF_SAVEAS, &CWorldEditView::OnPsfSave)
    ON_COMMAND(ID_ENTER, &CWorldEditView::OnEnter)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView construction/destruction

CWorldEditView::CWorldEditView()
    : CD3DApplication(SP_OFFICE_TEST)
	, m_bCreated(FALSE)
	, m_hCursorDefault(NULL)
	, m_bHeightBaseCamera(TRUE)
	, m_pActiveEffect(NULL)
	, m_p3dPosMesh(NULL)
	, m_vCollisionPos(0,0,0)
	, m_pSwapChainMain(NULL)
	, m_pSwapChainPreView(NULL)
    , m_bFrameMeshWireFrame(FALSE)
	, m_SetLandMan(FALSE)
	, m_SetLandManPrefab(TRUE)
	, m_spDxLandMan(new DxLandMan)
	, m_dwDelayGPU(0)
	, m_dwTimeGetTimePrev(0)
{
	// TODO: add construction code here
	m_bUseDepthBuffer = TRUE;

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    SetServiceProvider(pApp->GetServiceProvider());
    m_pEngineDevice = new DxResponseMan( m_ServiceProvider, pApp->GetSubPath() );
    m_pGlobalStage = new DxGlobalStage( m_pEngineDevice, pApp->GetSubPath(), true );

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

	// Note : �ӵ� ���� ����.
	g_bFRAME_LIMIT = FALSE;

	//// �ӵ� ����
	//g_bFRAME_LIMIT = TRUE;

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc( 206 );
	//_CrtSetBreakAlloc( 141006 );
}

CWorldEditView::~CWorldEditView()
{
    m_pCharacter = NULL;
    SAFE_DELETE(m_pGlobalStage);
    SAFE_DELETE(m_pEngineDevice);
}

BOOL CWorldEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    if (!CView::PreCreateWindow(cs))
        return FALSE;

    //cs.dwExStyle |= WS_EX_CLIENTEDGE;
    //cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
        ::LoadCursor(NULL, IDC_ARROW), 0, NULL);

    return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView drawing

void CWorldEditView::OnDraw(CDC* pDC)
{
	CWorldEditDoc* pDoc = GetDocument();
    Render3DEnvironment();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView printing

BOOL CWorldEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWorldEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWorldEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView diagnostics

#ifdef _DEBUG
void CWorldEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWorldEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWorldEditDoc* CWorldEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWorldEditDoc)));
	return (CWorldEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView message handlers

void CWorldEditView::OnInitialUpdate() 
{
	HRESULT hr;

	CView::OnInitialUpdate();

	//	Note : D3D APP ����.
	//
	hr = CD3DApplication::Create ( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() );
	if ( FAILED(hr) )
	{
		m_bCreated = FALSE;
		return;
	}

	// �ܺ������� �巡�׾� ��� �� �� �ְ� ���ش�.
	DragAcceptFiles();

	m_bCreated = TRUE;
}

void CWorldEditView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ( m_bCreated )	OnSize ( cx, cy );
}

void CWorldEditView::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
    CWorldEditApp *pApp = (CWorldEditApp*) AfxGetApp();
	pApp->ShutDown();

	//	Note : D3D APP �ı�.
	//
	Cleanup3DEnvironment();

	::SetCursor ( NULL );
	::DestroyCursor ( m_hCursorDefault );

	CView::PostNcDestroy();
}

void CWorldEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	CWorldEditApp *pApp = (CWorldEditApp*) AfxGetApp();
	pApp->SetActive ( bActivate );

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

HRESULT CWorldEditView::OpenXFile ( CString FileName )
{
	HRESULT hr;

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	//	Note : ���� �ε�ȰͰ� ������ ������Ʈ �ʱ�ȭ.
	//
	ResetObjects ();

	TSTRING strPath;
	strPath = pPath->AppRoot().c_str();
	strPath += pPath->ObjFileRoot().c_str();

	DxFrameMesh* pFrameMesh = m_SetLandMan.GetFrameMesh ();
	pFrameMesh->SetFile ( strPath.c_str(), FileName.GetString() );

	hr = pFrameMesh->InitDeviceObjects ( m_pd3dDevice );
	if ( FAILED(hr) )	return hr;

	hr = pFrameMesh->RestoreDeviceObjects ( m_pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

void CWorldEditView::SetActiveEffect ( DxEffectBase* pActiveEffect )
{
	if ( !pActiveEffect->IsUseAffineMatrix () ) return;

	//	Note : Matrix ������ �ʿ�ÿ���....
	//
	m_pActiveEffect = pActiveEffect;
	m_CurType = _CUR_MOVE;

	DxFrame *pDxFrame = m_pActiveEffect->GetAdaptFrame();
	if ( !pDxFrame )	return;

	m_pDxEditmat->SetFrameMatrix ( pDxFrame->matCombined );
	m_pDxEditmat->SetAffineMatrix ( (DXAFFINEMATRIX*)pActiveEffect );
	m_sObjectMRS.SetMatrix( pActiveEffect->m_pmatLocal );
}

void CWorldEditView::ResetActiveEffect ()
{
	if( m_pActiveEffect )
	{
		DxFrame *pDxFrame = m_pActiveEffect->GetAdaptFrame();
		if ( pDxFrame )
		{
			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

			DlgEdit.SelectFrameTree ( pDxFrame->szName );
		}
	}

	m_pActiveEffect = NULL;
	m_CurType = _CUR_SELECT;
	m_pDxEditmat->SetAffineMatrix ( NULL );
	m_sObjectMRS.SetMatrix( NULL );
}

void CWorldEditView::SetCurEditType ( DWORD EditType )
{
	m_pDxEditmat->SetType ( (EDITMAT_TYPE)EditType );

	switch( EditType )
	{
	case TRANS_MAT:		m_sObjectMRS.SetMode( OBECTMRS_M );	break;
	case ROTATE_MAT:	m_sObjectMRS.SetMode( OBECTMRS_R );	break;
	case SCALE_MAT:		m_sObjectMRS.SetMode( OBECTMRS_S );	break;
	}
}

void CWorldEditView::ActiveLight ( DXLIGHT *pLight )
{
	switch ( pLight->m_Light.Type )
	{
	case D3DLIGHT_POINT:
			m_CurType = _CUR_MOVE;
			m_sObjectMRS.SetMatrix( &pLight->m_matWorld );
			break;

	case D3DLIGHT_SPOT:
			m_CurType = _CUR_MOVE;
			m_sObjectMRS.SetMatrix( &pLight->m_matWorld );
			break;

	case D3DLIGHT_DIRECTIONAL:
			m_CurType = _CUR_SELECT;
			m_sObjectMRS.SetMatrix( NULL );
			break;
	};
}

void CWorldEditView::ActiveSpotLight( LPD3DXMATRIX pMatrix )
{
	m_CurType = _CUR_MOVE;
	m_sObjectMRS.SetMatrix( pMatrix );
}

void CWorldEditView::DeActiveLight ()
{
	m_CurType = _CUR_SELECT;
	m_pDxEditmat->SetAffineMatrix ( NULL );
	m_sObjectMRS.SetMatrix( NULL );
}

BOOL CWorldEditView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL CWorldEditView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	HCURSOR hCursl = CCursor::GetInstance().GetCurCursor();
	if ( !hCursl )
	{
		if ( !m_hCursorDefault )		m_hCursorDefault = LoadCursor ( NULL, IDC_ARROW );
		hCursl = m_hCursorDefault;
	}

	if ( hCursl )	::SetCursor ( hCursl );

	return TRUE;
}

void CWorldEditView::OnReload()
{
	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;
	int nAtvIndex = pFrame->m_cDlgEdit.GetActiveIndex();
	WorldStateMachine::Reload(nAtvIndex);
}

void CWorldEditView::OnArchive()
{
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;    

    char pszPathname[MAX_PATH];
    BROWSEINFO BrInfo;
    BrInfo.hwndOwner = GetSafeHwnd();
    BrInfo.pidlRoot = 0;
    memset(&BrInfo, 0, sizeof(BrInfo));
    BrInfo.pszDisplayName = pszPathname;
    BrInfo.lpszTitle = _T("������ ���丮�� �����ϼ���");
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // ���̾�α׸� ����
    ITEMIDLIST* pidlBrowse = ::SHBrowseForFolder(&BrInfo);   

    if( pidlBrowse != 0)
    {
        // �н��� ����
        ::SHGetPathFromIDList(pidlBrowse, pszPathname);
        WorldStateMachine::Archive(pszPathname);
    }
}

void CWorldEditView::OnPsfSave()
{
    CString strFilePath;
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    CWorldEditDoc* pDoc = GetDocument();
    if( pDoc )
        strFilePath = pDoc->GetTitleOrig().c_str();

    // FRAME_VIEW, PIECE_REPLACE_VIEW ���� �ڽſ��� ������ �����ϴ�.
    BOOL bMyNameSave(FALSE);
    switch ( CWorldEditView::m_ViewType )
    {
    case FRAME_VIEW:
    case PIECE_REPLACE_VIEW:
    case PIECE_SETPOS_VIEW:
    case EFFECT_SET_VIEW:
    case EFFECT_TILE_VIEW:
    case EFFECT_GRASS_VIEW:
    case WATER_SURF_VIEW:
        bMyNameSave = TRUE;
        break;
    }

    CString strExt = strFilePath.Right( 3 );

    bool bShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

    if( bShift ||                                   // Shift Ű�� ������,
        strExt.CompareNoCase( _T("psf") ) != 0 ||   // Ȯ���ڰ� psf �� �ƴϸ�, 
        !bMyNameSave )                              // ...
    {
        // �ٸ� �̸����� ����
        if( pDoc )
            pDoc->OnMenuitemSettingSave();
    }
    else
    {
        CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
        const SUBPATH* pPath = pApp->GetSubPath();

        char szPath[MAX_PATH]  = {0};
        strcpy(szPath, pFrame->m_szAppPath);
        strcat(szPath, pPath->ObjFileRoot().c_str() ); //SUBPATH::OBJ_FILE_ROOT);
        strcat(szPath, strFilePath.GetString());

        m_SetLandMan.SaveSet( GetD3DDevice(), szPath );
    }
}

void CWorldEditView::OnEnter()
{
    CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    pMainFrame->m_cDlgEdit.KeyBoard_Enter();
}

void CWorldEditView::MakeMiniMap( D3DXVECTOR2& vMin, 
								 D3DXVECTOR2& vMax, 
								 DWORD dwTexSize, 
								 const TCHAR* pName )
{
	m_spDxLandMan->MakeMiniMap
	(
		GetD3DDevice(),
		&m_SetLandManPrefab,
		vMin,
		vMax,
		dwTexSize,
		pName 
	);
}

void CWorldEditView::OnDropFiles(HDROP hDropInfo)
{
	enum EMDROPFILE_MODE
	{
		EMDROPFILE_NONE = 0,
		EMDROPFILE_PREFAB,
		EMDROPFILE_PIECE,
	};

	UINT nCnt = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);

	int nType = EMDROPFILE_NONE;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt ();
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

	D3DXVECTOR3 vColl(0.f,0.f,0.f);
	D3DXVECTOR3 vDirect = vFromPt - vTargetPt;
	D3DXVec3Normalize ( &vDirect, &vDirect );
	vTargetPt = vFromPt - vDirect * 100000.0f;


	BOOL bColl(FALSE);
	bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, NULL, EMCC_CULL_CCW );	// FrameMesh Picking

	DxPiece* pPiece(NULL);
	D3DXVECTOR3 vNor(0.f,1.f,0.f);
	g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, vColl, &pPiece, EMCC_CULL_CCW );

	// �ƹ��͵� �浹���� �ʴ´ٸ�, ��ġ�� Zero �� ���д�.
	if ( !bColl && !pPiece )
	{
		// �Ϲ� Plane �� �浹ó���� �ϵ��� ����.

		vTargetPt = D3DXVECTOR3( 0.f, 0.f, 0.f );
	}


	matIdentity._41 = vTargetPt.x;
	matIdentity._42 = vTargetPt.y;
	matIdentity._43 = vTargetPt.z;

	for(UINT i = 0; i < nCnt; i++)
	{
		TCHAR wzItem[MAX_PATH] = {0};
		int nCount = DragQueryFile(hDropInfo, i, wzItem, MAX_PATH);
		if( nCount == 0 )
			continue;

		TSTRING strName = sc::string::getFileExt( wzItem );
		sc::string::lowerString( strName );
		if ( strName == _T("prefab") )
		{
			nType = EMDROPFILE_PREFAB;
			GetSetLandMan()->GetPrefabMan()->Insert( GetD3DDevice(), wzItem, matIdentity );
		}
		else if ( strName == _T("pie15") )
		{
			//CWorldEditView::g_strPIECENAME = wzItem;
			TSTRING szFileName;
			// Ȯ���ڸ� ������ ���� �̸��� �����ش�;
			sc::string::getFullFileName( wzItem, szFileName );
			GetPieceManager()->SetPiece( GetD3DDevice(), szFileName.c_str(), matIdentity );
			nType = EMDROPFILE_PIECE;
		}
	}

	switch ( nType )
	{
	case EMDROPFILE_PREFAB:
		{
			CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			pMainFrame->m_cDlgEdit.SetActivePagePrefab();
			pMainFrame->m_cDlgEdit.UpdatePage_ListChange();
			break;
		}

	case EMDROPFILE_PIECE:
		{
			CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if ( pMainFrame->m_cDlgEdit.GetActiveIndex() == PIECEPOINTPAGEINDEX )
			{
				pMainFrame->m_cDlgEdit.UpdatePage_List_AddLastItem();
			}
			else
			{
				pMainFrame->m_cDlgEdit.SetActivePagePiecePoint();
			}
			break;
		}
	};

	DragFinish(hDropInfo);

	CView::OnDropFiles(hDropInfo);
}