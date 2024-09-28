// CameraPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "SheetWithTab.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceManagerWLD.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceManagerPSF.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "CameraPage.h"
#include "State/WorldStateMachine.h"

// CCameraPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraPage, CPropertyPage)
CCameraPage::CCameraPage()
	: CPropertyPage(CCameraPage::IDD)
	, m_bDlgInit(FALSE)
	, m_bNewXFile(FALSE)
{
	m_pFrameMeshForCamera = new DxFrameMesh(TRUE);
}

CCameraPage::~CCameraPage()
{
	SAFE_DELETE ( m_pFrameMeshForCamera );
}

void CCameraPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PAGELINE, m_stPageLine);
	DDX_Control(pDX, IDC_STATIC_ANI_LINE, m_stAniLine);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, m_listAnimation);
	DDX_Control(pDX, IDC_LIST_CAMERA, m_listRegistCamera);
}


BEGIN_MESSAGE_MAP(CCameraPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_TARGET, OnBnClickedButtonTarget)
	ON_BN_CLICKED(IDC_BUTTON_FROM, OnBnClickedButtonFrom)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_RADIO_TARGETCAMERA, OnBnClickedRadioTargetcamera)
	ON_BN_CLICKED(IDC_RADIO_FREECAMERA, OnBnClickedRadioFreecamera)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_XFILE_LOAD, &CCameraPage::OnBnClickedButtonXfileLoad)
	ON_BN_CLICKED(IDC_CHECK_NEW_XFILE, &CCameraPage::OnBnClickedCheckNewXfile)
END_MESSAGE_MAP()


BOOL CCameraPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETCAMERA );
	pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FREECAMERA );
	pButton->SetCheck ( TRUE );

	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SKEY );
	pWnd->SetWindowText ( "0" );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_EKEY );
	pWnd->SetWindowText ( "0" );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETSKEY );
	pWnd->SetWindowText ( "0" );

	ActiveCameraEdit(FALSE);

	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CCameraPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ActiveCameraEdit(FALSE);
	ListCamera ();

	OnBnClickedCheckNewXfile();

	return CPropertyPage::OnSetActive();
}

void CCameraPage::ActiveCameraEdit ( BOOL bShow, BOOL bNew )
{
	int nCameraContrls[] =
	{
		IDC_STATIC_ANI_LINE,
		IDC_EDIT_FINDSTR,
		IDC_BUTTON_SEARCH,
		IDC_LIST_SEARCH_RESULT,
		IDC_BUTTON_TARGET,
		IDC_BUTTON_FROM,
		IDC_STATIC_CARNAME,
		IDC_EDIT_CAMERA_NAME,
		IDC_RADIO_TARGETCAMERA,
		IDC_RADIO_FREECAMERA,
		IDC_EDIT_TARGETPOS_NAME,
		IDC_EDIT_CAMERAPOS_NAME,
		IDC_STATIC_S,
		IDC_CHECK_FREESET,
		IDC_EDIT_ANI_SKEY,
		IDC_EDIT_ANI_SETSKEY,
		IDC_STATIC_E,
		IDC_EDIT_ANI_EKEY,
		IDC_EDIT_ANI_SETEKEY,
		IDC_CHECK_ANILOOP,
		IDC_BUTTON_TEST,
		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD,
		IDC_EDIT_XFILE,
		IDC_BUTTON_XFILE_LOAD,
	};
	int NumContrl = sizeof(nCameraContrls) / sizeof(int);

	CWnd *pWnd;

	for ( int i=0; i<NumContrl; ++i )
	{
		pWnd = GetDlgItem(nCameraContrls[i]);
		pWnd->ShowWindow ( bShow );
	}

	if ( bNew )
	{
		pWnd = GetDlgItem ( IDC_EDIT_CAMERA_NAME );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_EDIT_CAMERA_NAME );
		pWnd->EnableWindow ( FALSE );
	}
}

void CCameraPage::ListCamera ()
{
	DxCamAniMan* pCameraAniMan = GetCameraAni();
	if ( !pCameraAniMan )
		return;

	m_listRegistCamera.ResetContent ();

	DxCameraAni* pCur = pCameraAniMan->GetCameraList ();
	while ( pCur )
	{
		int nIndex = m_listRegistCamera.AddString ( pCur->m_szName );
		m_listRegistCamera.SetItemData ( nIndex, DWORD(pCur) );

		pCur = pCur->m_pNext;
	}
}

void CCameraPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	ActiveCameraEdit(FALSE);
	ListCamera ();
}

//-----------------------------------------[카메라 리스트 관리]-------------------------
void CCameraPage::OnBnClickedButtonNew()
{
	ActiveCameraEdit(TRUE,TRUE);
	m_CameraAni.ClearSet ();
	SetTickCamera ();
}

void CCameraPage::OnBnClickedButtonDel()
{
	DxCamAniMan* pCameraAniMan = GetCameraAni();
	if ( !pCameraAniMan )
		return;

	int nIndex = m_listRegistCamera.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	DWORD dwData = m_listRegistCamera.GetItemData ( nIndex );

	DxCameraAni *pCamera= (DxCameraAni*) dwData;
	if ( !pCamera )	return;
	
	pCameraAniMan->DelCameraAni ( pCamera->m_szName );

    WorldStateMachine::EditAsset(m_CameraAni.m_szName);

	ActiveCameraEdit(FALSE);
	m_CameraAni.ClearSet ();
	ListCamera ();
}

void CCameraPage::OnBnClickedButtonEdit()
{
	int nIndex = m_listRegistCamera.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	DWORD dwData = m_listRegistCamera.GetItemData ( nIndex );
	DxCameraAni *pCamera= (DxCameraAni*) dwData;
	if ( !pCamera )	return;

	m_CameraAni.ClearSet ();
	m_CameraAni.ModifySetCamera ( pCamera );

	ActiveCameraEdit(TRUE,FALSE);
	SetTickCamera ();
}

//-----------------------------------------[카메라 편집]--------------------------------
//
void CCameraPage::OnBnClickedButtonSearch()
{
	DxFrameMesh *pFrameMesh = GetFrameMesh();
	if ( !pFrameMesh )
		return;

	DxAnimation* pAnimationCur = pFrameMesh->GetAnimationList ();

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );

	CString strWndText;
	pWnd->GetWindowText ( strWndText );

	if ( strWndText.GetLength() <= 2 )
	{
		MessageBox ( "검색 단어에 2자 이상을 삽입하십시요." );
		return;
	}

	m_listAnimation.ResetContent ();

	while ( pAnimationCur )
	{
		if ( pAnimationCur->szName && ( strstr ( pAnimationCur->szName, strWndText.GetString() ) != NULL ) )
		{
			int nIndex = m_listAnimation.AddString ( pAnimationCur->szName );
			m_listAnimation.SetItemData ( nIndex, (DWORD)pAnimationCur );
		}

		pAnimationCur = pAnimationCur->pAnimNext;
	}
}

void CCameraPage::OnBnClickedRadioTargetcamera()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;
	
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETCAMERA );
	pButton->SetCheck ( TRUE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FREECAMERA );
	pButton->SetCheck ( FALSE );

	m_CameraAni.m_emCamera = CAMERA_TTARGET;
}

void CCameraPage::OnBnClickedRadioFreecamera()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETCAMERA );
	pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FREECAMERA );
	pButton->SetCheck ( TRUE );

	m_CameraAni.m_emCamera = CAMERA_TFREE;
}

void CCameraPage::OnBnClickedButtonTarget()
{
	int nIndex = m_listAnimation.GetCurSel ();
	if ( nIndex == LB_ERR )	return;

	DWORD dwData = m_listAnimation.GetItemData ( nIndex );
	DxAnimation* pAnimation = (DxAnimation*) dwData;

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_TARGETPOS_NAME );
	pWnd->SetWindowText ( pAnimation->szName );

	float fTime = pAnimation->GetAnimTimeLine();
	if ( m_CameraAni.m_pFromCamera )
	{
		if ( fTime < m_CameraAni.m_pFromCamera->GetAnimTimeLine() )
			fTime = m_CameraAni.m_pFromCamera->GetAnimTimeLine();
	}

	CString Str;
	Str.Format ( "%5.2f", fTime );
	
	pWnd = GetDlgItem ( IDC_EDIT_ANI_EKEY );
	pWnd->SetWindowText ( Str );

	if ( m_CameraAni.m_ETime == 0.0f || m_CameraAni.m_ETime > fTime )	m_CameraAni.m_ETime = fTime;

	Str.Format ( "%5.2f", m_CameraAni.m_ETime );
	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETEKEY );
	pWnd->SetWindowText ( Str );

	//	Note : 카메라 타겟 에니메이션 설정.
	//
	SAFE_DELETE_ARRAY(m_CameraAni.m_szTargetCamera);
	m_CameraAni.m_szTargetCamera = new char[strlen(pAnimation->szName)+1];
	strcpy ( m_CameraAni.m_szTargetCamera, pAnimation->szName );

	m_CameraAni.m_pTargetCamera = pAnimation;
}

void CCameraPage::OnBnClickedButtonFrom()
{
	int nIndex = m_listAnimation.GetCurSel ();
	if ( nIndex == LB_ERR )	return;

	DWORD dwData = m_listAnimation.GetItemData ( nIndex );
	DxAnimation* pAnimation = (DxAnimation*) dwData;

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_CAMERAPOS_NAME );
	pWnd->SetWindowText ( pAnimation->szName );

	float fTime = pAnimation->GetAnimTimeLine();
	if ( m_CameraAni.m_pTargetCamera )
	{
		if ( fTime < m_CameraAni.m_pTargetCamera->GetAnimTimeLine() )
			fTime = m_CameraAni.m_pTargetCamera->GetAnimTimeLine();
	}

	CString Str;
	Str.Format ( "%5.2f", fTime );
	
	pWnd = GetDlgItem ( IDC_EDIT_ANI_EKEY );
	pWnd->SetWindowText ( Str );

	if ( m_CameraAni.m_ETime == 0.0f || m_CameraAni.m_ETime > fTime )	m_CameraAni.m_ETime = fTime;

	Str.Format ( "%5.2f", m_CameraAni.m_ETime );
	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETEKEY );
	pWnd->SetWindowText ( Str );

	//	Note : 카메라 에니메이션 설정.
	//
	SAFE_DELETE_ARRAY(m_CameraAni.m_szFromCamera);
	m_CameraAni.m_szFromCamera = new char[strlen(pAnimation->szName)+1];
	strcpy ( m_CameraAni.m_szFromCamera, pAnimation->szName );

	m_CameraAni.m_pFromCamera = pAnimation;
}

//-----------------------------------------[카메라 등록,취소]--------------------------------
//
BOOL CCameraPage::GetTickCamera ()
{
	float fSTime, fETime;
	CString Str;
	CWnd *pWnd;
	CButton *pButton;

	pWnd = GetDlgItem ( IDC_EDIT_CAMERA_NAME );
	pWnd->GetWindowText ( Str );

	if ( Str.GetLength() == 0 )
	{
		MessageBox ( "카메라의 이름을 지정하여야 합니다." );
		return FALSE;
	}

	SAFE_DELETE(m_CameraAni.m_szName);
	m_CameraAni.m_szName = new char[Str.GetLength()+1];
	strcpy ( m_CameraAni.m_szName, Str.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETSKEY );
	pWnd->GetWindowText ( Str );

	fSTime = (float) atof ( Str.GetString () );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETEKEY );
	pWnd->GetWindowText ( Str );
	
	fETime = (float) atof ( Str.GetString () );

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_FREESET );
	if ( pButton->GetCheck() )
		m_CameraAni.m_dwFlag |= CAMERA_FREESET;
	else
		m_CameraAni.m_dwFlag &= ~CAMERA_FREESET;

	if ( fSTime > fETime )
	{
		MessageBox ( "에니메이션 시작시간이 끝시간보다 작아야 합니다." );
		return FALSE;
	}

	m_CameraAni.m_STime = fSTime;
	m_CameraAni.m_ETime = fETime;

	switch ( m_CameraAni.m_emCamera )
	{
	case CAMERA_TFREE:
		if ( !m_CameraAni.m_pFromCamera || !m_CameraAni.m_szFromCamera )
		{
			MessageBox ( "카메라 에니메이션이 지정되지 않았습니다." );
			return FALSE;
		}
		break;

	case CAMERA_TTARGET:
		if ( !m_CameraAni.m_pFromCamera || !m_CameraAni.m_szFromCamera ||
			!m_CameraAni.m_pTargetCamera || !m_CameraAni.m_szTargetCamera )
		{
			MessageBox ( "카메라 에니메이션이 지정되지 않았습니다." );
			return FALSE;
		}
		break;
	};

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_ANILOOP );
	BOOL bAniLoop = pButton->GetCheck ();
	
	if ( bAniLoop )		m_CameraAni.m_dwFlag |= CAMERA_ANILOOP;
	else				m_CameraAni.m_dwFlag &= ~CAMERA_ANILOOP;

	return TRUE;
}

void CCameraPage::SetTickCamera ()
{
	CString Str;
	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->SetWindowText ( "" );

	m_listAnimation.ResetContent ();

	pWnd = GetDlgItem ( IDC_EDIT_CAMERA_NAME );
	if ( m_CameraAni.m_szName )		pWnd->SetWindowText ( m_CameraAni.m_szName );
	else							pWnd->SetWindowText ( "" );

	CButton *pButton;

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_TARGETCAMERA );
	pButton->SetCheck ( m_CameraAni.m_emCamera==CAMERA_TTARGET );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_FREECAMERA );
	pButton->SetCheck ( m_CameraAni.m_emCamera==CAMERA_TFREE );

	pWnd = GetDlgItem ( IDC_EDIT_TARGETPOS_NAME );
	if ( m_CameraAni.m_szTargetCamera )		pWnd->SetWindowText ( m_CameraAni.m_szTargetCamera );
	else									pWnd->SetWindowText ( "" );

	pWnd = GetDlgItem ( IDC_EDIT_CAMERAPOS_NAME );
	if ( m_CameraAni.m_szFromCamera )		pWnd->SetWindowText ( m_CameraAni.m_szFromCamera );
	else									pWnd->SetWindowText ( "" );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SKEY );
	pWnd->SetWindowText ( "0" );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_EKEY );
	if ( !m_CameraAni.m_pFromCamera && !m_CameraAni.m_pTargetCamera )
	{
		pWnd->SetWindowText ( "0" );
	}
	else if ( !m_CameraAni.m_pTargetCamera )
	{
		Str.Format ( "%5.4f", m_CameraAni.m_pFromCamera->GetAnimTimeLine() );
		pWnd->SetWindowText ( Str );
	}
	else
	{
		float fTime = m_CameraAni.m_pTargetCamera->GetAnimTimeLine();
		if ( fTime > m_CameraAni.m_pFromCamera->GetAnimTimeLine() )		fTime = m_CameraAni.m_pFromCamera->GetAnimTimeLine();

		Str.Format ( "%5.4f", fTime );
		pWnd->SetWindowText ( Str );
	}

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_FREESET );
	pButton->SetCheck ( m_CameraAni.m_dwFlag&CAMERA_FREESET );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETSKEY );
	Str.Format ( "%5.4f", m_CameraAni.m_STime );
	pWnd->SetWindowText ( Str );

	pWnd = GetDlgItem ( IDC_EDIT_ANI_SETEKEY );
	Str.Format ( "%5.4f", m_CameraAni.m_ETime );
	pWnd->SetWindowText ( Str );

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_ANILOOP );
	pButton->SetCheck ( m_CameraAni.m_dwFlag&CAMERA_ANILOOP );
}

void CCameraPage::OnBnClickedButtonAdd()
{
	DxCamAniMan* pCameraAniMan = GetCameraAni();
	if ( !pCameraAniMan )
		return;

	if ( !GetTickCamera() )		return;

	pCameraAniMan->AddCameraAni ( &m_CameraAni );
	pCameraAniMan->ResetTestCameraAni ();

    WorldStateMachine::EditAsset(m_CameraAni.m_szName);

	ActiveCameraEdit(FALSE);
	ListCamera ();    
}

void CCameraPage::OnBnClickedButtonCancel()
{
	DxCamAniMan* pCameraAniMan = GetCameraAni();
	if ( !pCameraAniMan )
		return;

	pCameraAniMan->ResetTestCameraAni ();

	ActiveCameraEdit(FALSE);
}

void CCameraPage::OnBnClickedButtonTest()
{
	DxCamAniMan* pCameraAniMan = GetCameraAni();
	if ( !pCameraAniMan )
		return;

	if ( !GetTickCamera() )		return;

	pCameraAniMan->TestCameraAni ( &m_CameraAni );


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
	if ( pSetLandMan )
	{
		DxPieceManagerPSF* pPieceMng= pSetLandMan->GetPieceManager();
		if ( pPieceMng )
		{
			pPieceMng->ResetAnimationTime_DisableLoopType();
		}
	}

	DxLandMan* pLandMan	= pView->GetLandMan();
	if ( pLandMan )
	{
		DxPieceManagerWLD* pPieceMng= pLandMan->GetPieceManager();
		if ( pPieceMng )
		{
			pPieceMng->ResetAnimationTime_DisableLoopType();
		}
	}
}

void CCameraPage::OnBnClickedButtonXfileLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Frame Mesh xFile |*.X|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CDlgEdit*)this);

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	CWorldEditDoc* pDoc = (CWorldEditDoc *) pFrame->GetActiveDocument ();
	const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileRoot().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		LoadXFileForCamera( sFileName.GetString() );
	}
}

bool CCameraPage::LoadXFileForCamera( const TCHAR* pFileName )
{
	HRESULT hr = S_OK;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	TSTRING strPath;
	strPath = pPath->AppRoot().c_str();
	strPath += pPath->ObjFileRoot().c_str();

	//	Note : 종전 데이터 삭제.
	//
	hr = m_pFrameMeshForCamera->CleanUp ();
	if ( FAILED(hr) )	goto e_ERROR;

	//	Note : 읽을 x파일과 버텍스 구조 정보 ( FVF ) 를 준다.
	//
	m_pFrameMeshForCamera->SetFile ( strPath.c_str(), pFileName );
	m_pFrameMeshForCamera->SetFVF ( VERTEX_WORLD::FVF );

	//	Note : 초기화 한다.
	//
	hr = m_pFrameMeshForCamera->Create ( pd3dDevice );
	if ( FAILED(hr) )	goto e_ERROR;
	m_pFrameMeshForCamera->UpdateFrames();

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->SetWindowText ( pFileName );

	return true;

e_ERROR:
	pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->SetWindowText ( "" );

	MessageBox ( "xFile Load Failed!" );
	return false;
}

DxCamAniMan* CCameraPage::GetCameraAni()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxCamAniMan* pCameraAniMan = pView->GetSetLandMan()->GetCameraAniMan();

	if( pView )
	{
		if ( pView->m_ViewType == OCTREE_VIEW )
		{
			pCameraAniMan = pView->GetLandMan()->GetCamAniMan();
		}
	}

	return pCameraAniMan;
}

DxFrameMesh* CCameraPage::GetFrameMesh()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	// GameMode
	if ( m_bNewXFile )
	{
		pFrameMesh = m_pFrameMeshForCamera;
	}

	return pFrameMesh;
}

void CCameraPage::OnBnClickedCheckNewXfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_NEW_XFILE );
	if ( pButton->GetCheck() )
	{
		m_bNewXFile = TRUE;

		CWnd* pWnd = GetDlgItem ( IDC_EDIT_XFILE );
		pWnd->ShowWindow( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_XFILE_LOAD );
		pWnd->ShowWindow( TRUE );
	}
	else
	{
		m_bNewXFile = FALSE;

		CWnd* pWnd = GetDlgItem ( IDC_EDIT_XFILE );
		pWnd->ShowWindow( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_XFILE_LOAD );
		pWnd->ShowWindow( FALSE );
	}
}
