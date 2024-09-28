// DlgPiecePreView.cpp : 구현 파일입니다.
//
#pragma once
#include "stdafx.h"

#include "../worldedit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../enginelib/DxTools/DxSurfaceTex.h"
#include "../../enginelib/DxTools/TextureManager.h"
#include "../../enginelib/DxTools/DxBackUpRendTarget.h"
#include "../../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../../EngineLib/DxLand/DxPiece/DxPieceContainer.h"

//#include "../enginelib/DxLand/DxSpeedTree/NSSpeedTree.h"
//#include "../enginelib/SpeedTree/SpeedTreeWrapper.h"
#include "../sheetWithTab.h"

#include "DlgPiecePreView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgPiecePreView 대화 상자입니다.

//IMPLEMENT_DYNAMIC(CDlgPiecePreView, CDialog)

CDlgPiecePreView::CDlgPiecePreView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPiecePreView::IDD, pParent)
	, m_pPIECE(NULL)
	, m_fPieceScale(1.0f)
	, m_nPieceAngle(0)
    , m_vRandomScale(1.f,1.f)
{
}
  

CDlgPiecePreView::~CDlgPiecePreView()
{
	//if(d3ddevice) SAFE_RELEASE( d3ddevice );
}

void CDlgPiecePreView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPiecePreView, CDialog)
END_MESSAGE_MAP()


// CDlgPiecePreView 메시지 처리기입니다.

BOOL CDlgPiecePreView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//UpdateLIST();

	return CDialog::PreCreateWindow(cs);
}

void CDlgPiecePreView::PreInitDialog()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::PreInitDialog();
}



BOOL CDlgPiecePreView::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 초기화 시작

	m_nPieceAngle = 0;
	m_fPieceScale = 1.0f;
	return TRUE;

}


void CDlgPiecePreView::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::PostNcDestroy();
}





//D3D Setting


// 렌더 스테이트 설정
HRESULT CDlgPiecePreView::Setup()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	
	D3DXMatrixIdentity( &m_matWorld );

	m_nPieceAngle = 0;
	m_fPieceScale = 1.0f;

	return S_OK;

}



HRESULT CDlgPiecePreView::Cleanup()
{

	return S_OK;
}

LPDIRECT3DDEVICEQ CDlgPiecePreView::GetPieceD3DDevice()
{
	return NULL;
}

HRESULT CDlgPiecePreView::Update(float timeDelta)
{
	return S_OK;
}

// 라이트를 셋팅한다.
void CDlgPiecePreView::SetLight()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();


	D3DLIGHT9				mLight;

	ZeroMemory(&mLight, sizeof(D3DLIGHT9));

	mLight.Type       = D3DLIGHT_DIRECTIONAL;

	mLight.Diffuse.r = 0.5f;
	mLight.Diffuse.g = 0.5f;
	mLight.Diffuse.b = 0.5f;
	mLight.Diffuse.a = 0.5f;

	mLight.Ambient.r = 0.5f;
	mLight.Ambient.g = 0.5f;
	mLight.Ambient.b = 0.5f;
	mLight.Ambient.a = 0.5f;

	D3DXVECTOR3 vecDir;
	D3DXMATRIX matProj;

	pView->GetD3DDevice()->GetTransform( D3DTS_PROJECTION, &matProj );
	D3DXVec3TransformCoord(&vecDir,&m_vDir,&matProj);
	D3DXVec3Normalize( (D3DXVECTOR3*)&mLight.Direction, &vecDir );

	pView->GetD3DDevice()->SetRenderState(D3DRS_AMBIENT,0xFF020202);
	pView->GetD3DDevice()->LightEnable(0,TRUE);
	pView->GetD3DDevice()->SetLight(0,&mLight);
	pView->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING,TRUE);
}

HRESULT CDlgPiecePreView::Render()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//Setup();			// 랜더 스테이트 설정
	//SetLight();

    DxBackupTarget sBackupTarget;
    sBackupTarget.Backup( pView->GetD3DDevice(), DxBackupTarget::ALL );

	D3DCOLOR cColor(D3DCOLOR_XRGB(89,135,179));
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	pView->m_pSwapChainPreView->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pView->GetD3DDevice()->SetRenderTarget( 0, pBackBuffer );
	pView->GetD3DDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, cColor, 1.0f, 0);
	pView->GetD3DDevice()->BeginScene();

	////EDITSHEET
	//switch(CWorldEditView::m_ViewType)
	//{
	//case PIECE_SETPOS_VIEW:
		{
			D3DXMATRIX	matRot;
			D3DXMatrixIdentity(&matRot);
			D3DXMatrixRotationY(&matRot,D3DX_PI/180 * m_nPieceAngle);

			CLIPVOLUME cv;
			if( m_pPIECE )
			{
				pView->GetD3DDevice()->SetTransform( D3DTS_VIEW, &m_matView );
				pView->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);

				m_pPIECE->FrameMove( 0.f, 0.f );
				//m_pPIECE->Render( pView->GetD3DDevice(), cColor, m_matView, m_matProj, m_sCV );

				// BackUp 후 TnL_PS_3_0_DF 으로 동작되도록 하자.
				TnL_MODE emTnL_MODE_BACKUP = RENDERPARAM::g_emGroundQulity;
				RENDERPARAM::g_emGroundQulity = TnL_FIXED;

				m_pPIECE->RenderDeffered( pView->GetD3DDevice(), m_matWorld );
				m_pPIECE->RenderDefferedDecal( pView->GetD3DDevice(), m_matWorld );
				m_pPIECE->RenderDefferedSoftAlpha( pView->GetD3DDevice(), m_matWorld );
				m_pPIECE->RenderDecalPass2( pView->GetD3DDevice(), m_matWorld );

				// 백업된거 되돌림.
				RENDERPARAM::g_emGroundQulity = emTnL_MODE_BACKUP;

				//m_pPIECE->RenderFrame( pView->GetD3DDevice(), cv, matRot, TRUE );
				//m_pPIECE->Render( pView->GetD3DDevice(), cv, matRot);

				//pView->GetD3DDevice()->StretchRect( DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR );
			}
		}
	//	break;	
    //
	//case SPT_SETPOS_VIEW:
	//	{
	//	float m_sfDiffuse[3] = { 1.f, 1.f, 1.f };
	//	float m_sfAmbient[3] = { 0.f, 0.f, 0.f };
	//	NSSpeedTree::OnRenderONE
	//		(
	//		pView->GetD3DDevice(),
	//		m_pSPT.strName.c_str(), 
	//		D3DXVECTOR3(0.0f,0.0f,0.0f),
	//		m_nPieceAngle,
	//		1.0f,
	//		m_vEye,
	//		m_vLookat,
	//		DxViewPort::GetInstance( DXUTGetD3DDevice() )->GetMatProjRH(),
	//		DxViewPort::GetInstance( DXUTGetD3DDevice() )->GetMatView(),
	//		m_sfDiffuse,
	//		m_sfAmbient
	//		);
	//	}
	//	break;
	//}

	pView->GetD3DDevice()->EndScene();
	pView->m_pSwapChainPreView->Present( NULL, NULL, this->m_hWnd, NULL, 0 );

	pBackBuffer->Release();
	
	return S_OK;
}

// 피스 데이타 정보를 바탕으로 카메라를 셋팅한다.
void CDlgPiecePreView::SetCameraEX()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//switch(pFrame->m_dwComboSelected)
	//{
	//case 10:
	//	{
			if( m_pPIECE )
			{
				SetCamera(m_pPIECE->GetCenter(), m_pPIECE->GetLength());
			}
	//	}
	//	break;

	//case 11:
	//	{
	//		SetCamera(m_pSPT.vCenter,m_pSPT.fLength);
	//	}
	//	break;
	//}
}

void CDlgPiecePreView::SetCamera(D3DXVECTOR3 vCenter, float fLenght)
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	float fCameraLength;

	fCameraLength = 1.5f * fLenght;

	m_vLookat = vCenter;
	m_vEye = vCenter + D3DXVECTOR3(fCameraLength,fCameraLength,-fCameraLength);
	m_vUp = D3DXVECTOR3(0.0f,1.0f,0.0f);

	m_vDir = m_vEye - m_vLookat;

	D3DXMatrixIsIdentity(&m_matView);

	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &m_vLookat, &m_vUp );
	pView->GetD3DDevice()->SetTransform( D3DTS_VIEW, &m_matView );

	D3DXMatrixIdentity(&m_matProj);

	//D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI * 0.25f, (float) 150.0f/(float) 150.0f, 1.0f, 10000.0f);
    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI * 0.25f, 1.0f, 1.0f, 10000.0f);
	pView->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);

	m_sCV = DxViewPort::GetInstance().ComputeClipVolumeGameSetting( m_vLookat, m_vEye, m_vUp );

	// 
	NSMaterialManager::FrameMove( m_matView, m_matProj, NULL, NULL, NULL, NULL );
}

// 피스 데이타를 불러온다.
void CDlgPiecePreView::SetPieceData(const TCHAR *szFile )
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	m_pPIECE = DxPieceContainer::GetInstancePSF().LoadPiece( pView->GetD3DDevice(), szFile );
	if( m_pPIECE )
	{
		SetCamera(m_pPIECE->GetCenter(), m_pPIECE->GetLength());
	}
}

void CDlgPiecePreView::SetSPTData(const TCHAR *szFile )
{
	//CSpeedTreeWrapper *cSPTwarp = NSSpeedTree::LoadSpeedTree(szFile);
	//
	//m_pSPT.strName = szFile;
	//m_pSPT.fLength = cSPTwarp->CullRadius();
	//m_pSPT.vCenter = D3DXVECTOR3(0.0f,cSPTwarp->CullRadius()/2,0.0f);
	//
	//SetCamera(m_pSPT.vCenter,m_pSPT.fLength);
}

void CDlgPiecePreView::SetRotateLeft()
{
	if (m_nPieceAngle <= 45)
	{
		m_nPieceAngle = m_nPieceAngle + 360  - 45;
	}
	else
		m_nPieceAngle -= 45;

	//SetCameraEX();
	D3DXMATRIX matLeft;
	D3DXMatrixRotationY(&matLeft,-D3DX_PI/4);
	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matLeft);
}

void CDlgPiecePreView::SetRotateRight()
{
	if (m_nPieceAngle >= 315)
	{
		m_nPieceAngle = m_nPieceAngle - 360  + 45;
	}
	else 
		m_nPieceAngle += 45;

	//SetCameraEX();
	D3DXMATRIX matLeft;
	D3DXMatrixRotationY(&matLeft,D3DX_PI/4);
	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matLeft);
}

void CDlgPiecePreView::SetRotate(int nAngle)
{
	if ( nAngle <= 0) 
	{
		m_nPieceAngle = 0;
	}
	else if ( nAngle >= 360) 
	{
		m_nPieceAngle = 360;
	}
	else
		m_nPieceAngle = nAngle;

	//SetCameraEX();
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);
	
    // 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * nAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);
}

void CDlgPiecePreView::SetRotateRandom()
{
	int nAngle = rand() % 360;

	m_nPieceAngle = nAngle; 

	//SetCameraEX();

	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);

    // 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * nAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);
}

void CDlgPiecePreView::SetDefaultRandom()
{
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);

	// 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * m_nPieceAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);
	//SetCameraEX();
}

void CDlgPiecePreView::SetScaleUp()
{
	m_fPieceScale += 1.0f;

	if(m_fPieceScale >= 30.0f) 
        m_fPieceScale = 30.0f;
	else 
        m_fPieceScale = float(int(m_fPieceScale));

	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);

	// 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * m_nPieceAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);

	//SetCameraEX();
}

void CDlgPiecePreView::SetScaleDown()
{
	m_fPieceScale -= 1.0f;

	if(m_fPieceScale <= 1.0f) 
        m_fPieceScale = 1.0f;
	else 
        m_fPieceScale = float(int(m_fPieceScale));

	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);

	// 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * m_nPieceAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);
	//SetCameraEX();
}

void CDlgPiecePreView::SetScale(float fScale)
{
	if(fScale < 0.0f) m_fPieceScale = 1.0f;
	else m_fPieceScale = fScale;

	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&matRot);

	// 랜덤 스케일 적용함.
    float fMultiplyScale = m_vRandomScale.x + sc::Random::RANDOM_POS() * (m_vRandomScale.y - m_vRandomScale.x);
    m_fFinalScale = m_fPieceScale*fMultiplyScale;

	D3DXMatrixScaling(&m_matWorld,m_fFinalScale,m_fFinalScale,m_fFinalScale);
	D3DXMatrixRotationY(&matRot,D3DX_PI/180 * m_nPieceAngle);

	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&matRot);
	//SetCameraEX();
}

D3DXMATRIX CDlgPiecePreView::GetPieceMatrix()
{
	return m_matWorld;
}

