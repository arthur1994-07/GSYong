// DxViewPort.cpp: implementation of the DxViewPort class.
//
//
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../G-Logic/GLDefine.h"
#include "../DxLand/DxOctree/DxOctree.h"
#include "../DxLand/DxLandMan.h"
#include "../DxTools/DxInputDevice.h"
#include "../GUInterface/Cursor.h"
#include "../G-Logic/GLActiveMap.h"
#include "../NaviMesh/navigationmesh.h"

#include "DxViewPort.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//--------------------------------------------------------------------------------------[DxViewPort]

//	Note : 싱글톤 클래스 제어.
//
DxViewPort& DxViewPort::GetInstance()
{
	static DxViewPort Instance;
	return Instance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxViewPort::DxViewPort() :
    m_hWnd(NULL),
    m_pd3dDevice(NULL),
    m_pActiveMap(NULL),

    m_emCamera(CAMERA_UICONTR),
    m_dwFlag(DXVP_USER|DXVP_LANDSCAPEMOVE),
    m_bHandling(true),
    m_bKeyboard(true),

    m_vFromPt(0,0,0),
    m_vLookatPt(0,0,0),
    m_vUpVec(0,0,0),
    m_fLength(0),
    m_fLengthModify(0),

    m_fFOV(0),
    m_fWidth(0),
    m_fHeight(0),
    m_fNearPlane(0),
    m_fFarPlane(0)
    , m_fClip_FarPlane(0.f)
	, m_fBackUp_FarPlane(0.f)

    , m_vLookDir(0,0,0),
    m_vLookDir_Y0(0.f,0.f,0.f),
    m_fDistance(0.f),

	m_fZOOMIN_COLL(15.f)
    , m_fZOOMIN(5.0f)
    , m_fZOOMOUT(1800.0f)
	, m_fZOOMOUT_MapSetting(200.f)
    , m_fHIGHXDIR(D3DX_PI*0.4f),
    m_fLOWXDIR(-D3DX_PI*0.13f),
    //m_fLOWXDIR(0.257f),
    m_fVELOZOOM(80.0f),
    m_fVELOMOVE(80.0f),
    m_fVELODIR(D3DX_PI/360),

    m_damperVeloMove(0.9f,D3DXVECTOR3(0,0,0)),
    m_damperVeloZoom(0.9f,0.0f),
    m_damperVeloDir(0.9f,D3DXVECTOR3(0,0,0)),

    m_bQUAKE_ACTIVE(FALSE),
    m_fQUAKE_LIFE(0),
    m_fQUAKE_TERM(0),
    m_fQUAKE_SCALE(0),
    m_bCamCollTest(FALSE),
    m_pWorldLandMan(NULL),

    m_fQUAKE_TIMER(0),
    m_fQUAKE_TERM_TIMER(0),

    m_vQUAKE_FRMDIS(0,0,0),
    m_vQUAKE_LKTDIS(0,0,0),

    m_fElapsedTime(0.f)

	, m_fSpecCamMove(160.0f)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

DxViewPort::~DxViewPort()
{
}

HRESULT DxViewPort::SetGameCamera ()
{
	m_bHandling = true;
	m_dwFlag = (DXVP_LANDSCAPEMOVE|DXVP_LIMITDIR|DXVP_USER|DXVP_GAME);
	m_fZOOMIN = 30.0f;
	m_fZOOMOUT = m_fZOOMOUT_MapSetting;

	m_fHIGHXDIR = D3DX_PI*0.4f;

	return S_OK;
}

HRESULT DxViewPort::SetLobyCamera ()
{
	m_bHandling = false;
	m_dwFlag = (DXVP_LANDSCAPEMOVE|DXVP_LIMITDIR);
	m_fZOOMIN = 15.0f;
	m_fZOOMOUT = 1800.0f;

	m_fHIGHXDIR = D3DX_PI*0.4f;

	m_sCameraControl.Init();

	return S_OK;
}

HRESULT DxViewPort::SetToolCamera()
{
	m_bHandling = true;
	m_dwFlag = (DXVP_LANDSCAPEMOVE|DXVP_LIMITDIR|DXVP_USER|DXVP_TOOL);
	m_fZOOMIN = 5.0f;
	m_fZOOMOUT = 1800.0f;

	m_fHIGHXDIR = D3DX_PI*0.49f;
	m_fLOWXDIR = -D3DX_PI*0.49f;	

	return S_OK;
}

HRESULT DxViewPort::SetToolCameraMiniMap()
{
	m_bHandling = true;
	m_dwFlag = (DXVP_LIMITDIR|DXVP_USER|DXVP_TOOL|DXVP_MINIMAP);
	m_fZOOMIN = 10.0f;
	m_fZOOMOUT = 1000000.0f;

	m_fHIGHXDIR = D3DX_PI*0.49f;
	m_fLOWXDIR = -D3DX_PI*0.49f;	

	return S_OK;
}

HRESULT	DxViewPort::SetVehicleCamera ()
{
	m_bHandling = true;
	m_dwFlag = (DXVP_LANDSCAPEMOVE|DXVP_LIMITDIR|DXVP_USER|DXVP_GAME);
	m_fZOOMIN = 60.0f;
	m_fZOOMOUT = m_fZOOMOUT_MapSetting;

	m_fHIGHXDIR = D3DX_PI*0.4f;

	return S_OK;
}

void DxViewPort::SetFlyCameraFlag()
{
	m_bHandling = true;
	m_dwFlag = (DXVP_LANDSCAPEMOVE|DXVP_USER|DXVP_GAME);
	m_fZOOMIN = 30.0f;
	m_fZOOMOUT = m_fZOOMOUT_MapSetting;
}

HRESULT DxViewPort::GetViewTrans ( D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vLookatPt, D3DXVECTOR3 &vUpVec )
{
	vFromPt = m_vFromPt;
	vLookatPt = m_vLookatPt;
	vUpVec = m_vUpVec;

	return S_OK;
}

HRESULT DxViewPort::SetViewTrans ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vUpVec )
{
	m_vFromPt = vFromPt;
	m_vLookatPt = vLookatPt;
	m_vUpVec = vUpVec;

	D3DXVECTOR3 vCameraDir = m_vLookatPt - m_vFromPt;
	m_fLengthModify = m_fLength = D3DXVec3Length ( &vCameraDir );

	D3DXMatrixLookAtLH ( &m_matView, &vFromPt, &vLookatPt, &vUpVec );
	return m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
}

HRESULT DxViewPort::SetProjection ( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane )
{
	m_fFOV = fFOV;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fNearPlane = fNearPlane;

    return SetProjection_FarPlane( fFarPlane );
}

HRESULT DxViewPort::SetReflectProjection ( float fNearPlane )
{
	D3DXMATRIX	matProj;

	D3DXMatrixPerspectiveFovLH ( &matProj, m_fFOV, m_fWidth / m_fHeight, fNearPlane, m_fFarPlane );

	return m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );
}

HRESULT DxViewPort::SetProjection_FarPlane ( float fFarPlane )
{
	return SetProjection_FarPlane ( fFarPlane, m_fFOV, m_fWidth / m_fHeight );
}

HRESULT DxViewPort::SetProjection_FarPlane ( float fFarPlane, float fFOV, float fAspect )
{
	if ( !m_pd3dDevice )	
        return S_FALSE;

	SetFarPlane( fFarPlane );

	D3DXMatrixPerspectiveFovLH ( &m_matProj, fFOV, fAspect, m_fNearPlane, m_fFarPlane );

	return m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

HRESULT DxViewPort::SetProjection()
{
	D3DXMatrixPerspectiveFovLH ( &m_matProj, m_fFOV, m_fWidth / m_fHeight, m_fNearPlane, m_fFarPlane );
	return m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

HRESULT DxViewPort::SetProjectionOrthoLH( float fWidth, float fHeight, float fNearPlane, float fFarPlane )
{
	D3DXMatrixOrthoLH( &m_matProj, fWidth, fHeight, fNearPlane, fFarPlane );	// D3DXMatrixPerspectiveFovLH
	return m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

HRESULT DxViewPort::SetViewPort ()
{
	HRESULT hr;

	D3DVIEWPORTQ ViewPort;

	//	Note : 백 버퍼를 가져와서 화면의 사이즈를 알아낸다.. 
	//		( 주의 : 반드시 Release()를 해주어야 한다. )
	//
	LPDIRECT3DSURFACEQ pd3dBackBuffer;
	hr = m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dBackBuffer );
	if ( FAILED(hr) )	return hr;

	D3DSURFACE_DESC surfaceDesc;
	pd3dBackBuffer->GetDesc ( &surfaceDesc );

	ViewPort.X = 0;
	ViewPort.Y = 0;
	ViewPort.Width = surfaceDesc.Width;
	ViewPort.Height = surfaceDesc.Height;
	ViewPort.MinZ = 0.0f;
	ViewPort.MaxZ = 1.0f;

	pd3dBackBuffer->Release ();	// * (중요) 백버퍼의 ref 를 감소시킨다.

	m_ViewPort = ViewPort;
	return m_pd3dDevice->SetViewport ( &m_ViewPort );
}

void DxViewPort::SetFarPlane ( float fFarPlane )
{
	// 건물들 때문에 fFarPlane 수치를 그대로 쓰면 쩔단되는 현상이 보인다.
	// 보완을 위해 1.5f 값을 곱해준다.
	m_fFarPlane			= fFarPlane*1.5f;
	m_fClip_FarPlane	= fFarPlane;
}

D3DXMATRIX g_matProjBACKUP;
void DxViewPort::BackUpFarPlane()
{
	m_fBackUp_FarPlane = m_fClip_FarPlane; 
	g_matProjBACKUP = m_matProj;
}
	
void DxViewPort::RollBackFarPlane( LPDIRECT3DDEVICEQ pd3dDevice )	
{ 
	SetFarPlane( m_fBackUp_FarPlane );
	m_matProj = g_matProjBACKUP;
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
}

void DxViewPort::CameraJump ( D3DXVECTOR3 &vJump )
{
	D3DXVECTOR3 vCameraDir = m_vLookatPt - m_vFromPt;
	float fCameraLength = D3DXVec3Length ( &vCameraDir );
	D3DXVec3Normalize ( &vCameraDir, &vCameraDir );

	m_vLookatPt = vJump;
	m_vFromPt = m_vLookatPt - vCameraDir*fCameraLength;

	//	Note : 카메라 흔들림 효과.
	//
	if ( m_bQUAKE_ACTIVE )
	{
		m_fQUAKE_TIMER += m_fElapsedTime;
		m_fQUAKE_TERM_TIMER += m_fElapsedTime;
		if ( m_fQUAKE_TIMER > m_fQUAKE_LIFE )	m_bQUAKE_ACTIVE = FALSE;

		if ( m_fQUAKE_TERM_TIMER > m_fQUAKE_TERM )
		{
			m_fQUAKE_TERM_TIMER = 0.0f;

			D3DXVECTOR3	vDirect = m_vLookatPt - m_vFromPt;
			D3DXVec3Normalize ( &vDirect, &vDirect );

			float fDX = 1.f - (m_fQUAKE_TIMER/m_fQUAKE_LIFE);
			fDX = powf ( fDX, 3 );
			vDirect = vDirect * m_fQUAKE_SCALE * fDX;

			m_vQUAKE_FRMDIS.x = vDirect.x;
			m_vQUAKE_LKTDIS.x = vDirect.x;

			m_vQUAKE_FRMDIS.y = vDirect.y;
			m_vQUAKE_LKTDIS.y = vDirect.y;

			m_vQUAKE_FRMDIS.z = vDirect.z;
			m_vQUAKE_LKTDIS.z = vDirect.z;
		}
	}
	else
	{
		m_vQUAKE_FRMDIS = D3DXVECTOR3(0,0,0);
		m_vQUAKE_LKTDIS = D3DXVECTOR3(0,0,0);
	}

	D3DXVECTOR3 vFROM = m_vFromPt + m_vQUAKE_FRMDIS;
	D3DXVECTOR3 VLOOK = m_vLookatPt + m_vQUAKE_LKTDIS;
	D3DXVECTOR3 vUP = ComputeUpVecter ( VLOOK, vFROM );

	D3DXMatrixLookAtLH ( &m_matView, &vFROM, &VLOOK, &vUP );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
}

void DxViewPort::CameraMove ( D3DXVECTOR3 &vMove, BOOL bLandScapeMove, BOOL bLookAtMove )
{
	//	Note :	LookAt Matrix
	//	vRight.x	vUp.x	vDir.x
	//	vRight.y	vUp.y	vDir.y
	//	vRight.z	vUp.z	vDir.z
	//
	D3DXVECTOR3 vUp, vDir, vRight;
	vRight.x = m_matView._11;
	vRight.y = m_matView._21;
	vRight.z = m_matView._31;

	vUp.x = m_matView._12;
	vUp.y = m_matView._22;
	vUp.z = m_matView._32;

	vDir.x = m_matView._13;
	vDir.y = m_matView._23;
	vDir.z = m_matView._33;

	D3DXVECTOR3 vToMove = D3DXVECTOR3(0,0,0);	

	if ( (m_dwFlag & DXVP_LANDSCAPEMOVE) && bLandScapeMove )
	{
		vRight.y = 0.0f;
		D3DXVec3Normalize(&vRight,&vRight);

		vDir.y = 0.0f;
		D3DXVec3Normalize(&vDir,&vDir);

		vToMove += vRight*vMove.x;
		vToMove += vUp*vMove.y;
		vToMove += vDir*vMove.z;
	}
	else if ( m_dwFlag & DXVP_MINIMAP )
	{
		vRight.y = 0.0f;
		D3DXVec3Normalize(&vRight,&vRight);

		vUp.y = 0.0f;
		D3DXVec3Normalize(&vUp,&vUp);

		vToMove += vRight*vMove.x;
		vToMove += vUp*vMove.z;
	}
	else
	{
		vToMove += vRight*vMove.x;
		vToMove += vUp*vMove.y;
		vToMove += vDir*vMove.z;
	}

	m_vFromPt += vToMove;
	if ( bLookAtMove == TRUE )
		m_vLookatPt += vToMove;
}

//------------------------------------------
// 리턴 값 : 0 충돌안함, 1 네비와 충돌, 2 그외 면과의 충돌
//------------------------------------------
BYTE DxViewPort::CollisionMap ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vColl )
{
	DxLandMan* pLandMan = NULL;
	if( m_bCamCollTest )
	{
		pLandMan = m_pWorldLandMan;
	}else{
		if ( !m_pActiveMap )	return false;
		pLandMan = m_pActiveMap->getactivemap();
	}


	if ( !pLandMan )
		return false;

	boost::shared_ptr<NavigationMesh> spNaviMesh = pLandMan->GetNaviMesh();
	if ( !spNaviMesh )
		return false;

	D3DXVECTOR3 vPoint1 = vP1;
	D3DXVECTOR3 vPoint2 = vP2;
	DWORD dwCollisionID = 0;
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame = NULL;

	spNaviMesh->IsCollision ( vPoint1, vPoint2, vColl, dwCollisionID, bCollision );
	if( bCollision ) 
		return 1;

	BOOL bOnlyTextureCamColl;

	if( pLandMan->GetCamCollision() )
	{
		bOnlyTextureCamColl = FALSE;
	}
	else
	{
		bOnlyTextureCamColl = TRUE;
	}

	// Note : StaticMesh는 구버전의 맵데이터에선 사용하지 않는다, Texture Camcoll 기능도 사용할수 없다
	//
	if ( pLandMan->IsStaticMesh() || pLandMan->IsStaticMaterialMesh() || bOnlyTextureCamColl )
	{
		pLandMan->IsCollision ( vPoint1, vPoint2, vColl, bCollision, pDxFrame, EMCC_CULL_CCW, bOnlyTextureCamColl );

		if ( bCollision )	
		{
			return 2;
		}
	}

	return 0;
}

void DxViewPort::CameraZoom ( float fZoomOut )
{
	//	Note : 줌 거리 조정.
	m_fLength += fZoomOut;
	if ( m_fLength < m_fZOOMIN )		m_fLength = m_fZOOMIN;
	else if ( m_fLength > m_fZOOMOUT )	m_fLength = m_fZOOMOUT;

	m_fLengthModify = m_fLength;

	//	Note : 현재 방향.
	D3DXVECTOR3 vCameraDir = m_vLookatPt - m_vFromPt;
	D3DXVec3Normalize ( &vCameraDir, &vCameraDir );

	//	Note : 카메라 줌.
	D3DXVECTOR3 vFromPt = m_vLookatPt - vCameraDir*m_fLengthModify;
	
	D3DXVECTOR3 vLookatPtMIN = m_vLookatPt - vCameraDir*m_fZOOMIN_COLL;

	ComputeClipPoint ();

	bool bNear(false);
	float fNear(FLT_MAX);
	D3DXVECTOR3 vNear(0,0,0);
	D3DXVECTOR3 vP1, vP2;
	D3DXVECTOR3 vCollision(0,0,0);
	BYTE bCOLLISION(0);

	//	뷰 중심 선분.
	//
	{
		vP1 = m_vLookatPt;
		vP2 = vLookatPtMIN;

		bCOLLISION = CollisionMap ( vP1, vP2, vCollision );
		if ( bCOLLISION )
		{
			// 최저 유지 카메라 거리에도 물체가 걸린다면 그것은 무시한다.
			m_vFromPt = vLookatPtMIN;
			return;
		}
	}

	//	뷰 중심 선분.
	//
	{
		vP1 = vLookatPtMIN;
		vP2 = vFromPt;

		bCOLLISION = CollisionMap ( vP1, vP2, vCollision );
		if ( bCOLLISION )
		{
			D3DXVECTOR3 vDist = m_vLookatPt - vCollision;
			float fLength = D3DXVec3Length ( &vDist );
			if ( fLength < fNear )
			{
				bNear = true;
				fNear = fLength;
				vNear = vCollision;
			}
		}
	}

	if ( bNear )
	{
		D3DXVECTOR3 v1, v2, n;

		//compute the near plane
		v1 = m_ptViewPort[2] - m_ptViewPort[0];
		v2 = m_ptViewPort[1] - m_ptViewPort[0];

		D3DXVec3Cross ( &n, &v2, &v1 );
		D3DXVec3Normalize ( &n, &n );

		D3DXPLANE pNear;
		pNear.a = n.x;
		pNear.b = n.y;
		pNear.c = n.z;
		pNear.d = - ( n.x * vNear.x + n.y * vNear.y + n.z * vNear.z );

		D3DXVECTOR3 vCOL;
		bool bCOL = COLLISION::CalculatePlane2Line ( pNear, vFromPt, vLookatPtMIN, vCOL ); // by luxes.

		if ( bCOL )
		{
			m_vFromPt = vCOL;
		}

		// 카메라 이동 시킨 후, 카메라가 지면 뚫고 들어가는 것을 방지. by luxes.
		ComputeClipPoint ();

		bNear = false;
		fNear = FLT_MAX;
		vNear = D3DXVECTOR3(0,0,0);
		vCollision = D3DXVECTOR3(0,0,0);
		bCOLLISION = 0;

		//	뷰 프리즘 선분.
		//
		for ( int i=0; i<4; ++i )
		{
			vP1 = vLookatPtMIN;
			vP2 = m_ptViewPort[i];
			vP2.y -= 3.0f; // Memo : 카메라가 지면을 뚫고 들어가는 것을 방지.

			bCOLLISION = CollisionMap ( vP1, vP2, vCollision );
			if ( !bCOLLISION )	continue;

			D3DXVECTOR3 vDist = m_vLookatPt - vCollision;
			float fLength = D3DXVec3Length ( &vDist );
			if ( fLength < fNear )
			{
				bNear = true;
				fNear = fLength;
				vNear = vCollision;
			}
		}
		//

		if ( bNear )
		{
			//compute the near plane
			v1 = m_ptViewPort[2] - m_ptViewPort[0];
			v2 = m_ptViewPort[1] - m_ptViewPort[0];

			D3DXVec3Cross ( &n, &v2, &v1 );
			D3DXVec3Normalize ( &n, &n );


			pNear.a = n.x;
			pNear.b = n.y;
			pNear.c = n.z;
			pNear.d = - ( n.x * vNear.x + n.y * vNear.y + n.z * vNear.z );

			bool bCOL = COLLISION::CalculatePlane2Line ( pNear, m_vFromPt, vLookatPtMIN, vCOL );

			if ( bCOL )
			{
				m_vFromPt = vCOL;
			}
		}
		//
	}
	else
	{
		m_vFromPt = vFromPt;

		// 카메라 이동 시킨 후, 카메라가 지면 뚫고 들어가는 것을 방지. by luxes.
		ComputeClipPoint ();

		bNear = false;
		fNear = FLT_MAX;
		vNear = D3DXVECTOR3(0,0,0);
		vCollision = D3DXVECTOR3(0,0,0);
		bCOLLISION = 0;

		//	뷰 프리즘 선분.
		//
		for ( int i=0; i<4; ++i )
		{
			vP1 = vLookatPtMIN;
			vP2 = m_ptViewPort[i];
			vP2.y -= 3.0f; // Memo : 카메라가 지면을 뚫고 들어가는 것을 방지.

			bCOLLISION = CollisionMap ( vP1, vP2, vCollision );
			if ( !bCOLLISION )	continue;

			D3DXVECTOR3 vDist = m_vLookatPt - vCollision;
			float fLength = D3DXVec3Length ( &vDist );
			if ( fLength < fNear )
			{
				bNear = true;
				fNear = fLength;
				vNear = vCollision;
			}
		}
		//

		if ( bNear )
		{
			D3DXVECTOR3 v1, v2, n;

			//compute the near plane
			v1 = m_ptViewPort[2] - m_ptViewPort[0];
			v2 = m_ptViewPort[1] - m_ptViewPort[0];

			D3DXVec3Cross ( &n, &v2, &v1 );
			D3DXVec3Normalize ( &n, &n );


			D3DXPLANE pNear;
			pNear.a = n.x;
			pNear.b = n.y;
			pNear.c = n.z;
			pNear.d = - ( n.x * vNear.x + n.y * vNear.y + n.z * vNear.z );

			D3DXVECTOR3 vCOL;
			bool bCOL = COLLISION::CalculatePlane2Line ( pNear, m_vFromPt, vLookatPtMIN, vCOL );

			if ( bCOL )
			{
				m_vFromPt = vCOL;
			}
		}
		//
	}
}

void DxViewPort::CameraRotation ( D3DXVECTOR3 &vRotate, BOOL bFrom )
{
	if ( vRotate.x==0.0f && vRotate.y==0.0f && vRotate.z==0.0f )		return;

	//	Note : 머리 회전.
	//
	if ( bFrom )
	{
		D3DXVECTOR3 vCameraDir = m_vLookatPt - m_vFromPt;

		//	Note : 현재의 회전값.
		//
		float Length, LengthXZ;
		float thetaX, thetaY;

		Length = D3DXVec3Length ( &vCameraDir );
		if ( Length == 0 )		Length = 0.001f;

		thetaX = (float) asin ( vCameraDir.y / Length );

		LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );

		//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
		//
		if ( LengthXZ == 0 )	LengthXZ = 0.001f;

		thetaY = (float) acos ( vCameraDir.x / LengthXZ );
		if ( vCameraDir.z>0 )	thetaY = -thetaY;

		//	Note : 회전 적용.
		//
		thetaX += vRotate.x;
		thetaY += vRotate.y;

		//	Note : 회전 제한 값.
		//
		if ( m_dwFlag & DXVP_LIMITDIR )
		{
			if ( m_fHIGHXDIR < thetaX )		thetaX = m_fHIGHXDIR;
			else if ( m_fLOWXDIR > thetaX )	thetaX = m_fLOWXDIR;
		}

		//	Note : 카메라 방향 단위 백터 계산.
		//
		//		Dir = { 1, 0, 0 } 의 초기값에서 회전 시킴.
		//
		D3DXVECTOR3 Dir;
		Dir.y = (float) sin(thetaX);

		Dir.x = 1-Dir.y*Dir.y;
		Dir.x = ( Dir.x < 0 ) ? 0 : (float) sqrt(Dir.x);

		Dir.z = (float) (-Dir.x*sin(thetaY) );
		Dir.x = (float) ( Dir.x*cos(thetaY) );

		m_vLookatPt = m_vFromPt + Dir*Length;
	}
	//	Note : 카메라 회전.
	//
	else
	{
		D3DXVECTOR3 vCameraDir = m_vFromPt - m_vLookatPt;

		//	Note : 현재의 회전값.
		//
		float Length, LengthXZ;
		float thetaX, thetaY;

		Length = D3DXVec3Length ( &vCameraDir );
		if ( Length == 0 )		Length = 0.001f;

		thetaX = (float) asin ( vCameraDir.y / Length );

		//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
		//
		LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
		if ( LengthXZ == 0 )	LengthXZ = 0.001f;

		thetaY = (float) acos ( vCameraDir.x / LengthXZ );
		if ( vCameraDir.z>0 )	thetaY = -thetaY;

		//	Note : 회전 적용.
		//
		thetaX += vRotate.x;
		thetaY += vRotate.y;

		//	Note : 회전 제한 값.
		//
		if ( m_dwFlag & DXVP_LIMITDIR )
		{
			if ( m_fHIGHXDIR < thetaX )		thetaX = m_fHIGHXDIR;
			else if ( m_fLOWXDIR > thetaX )	thetaX = m_fLOWXDIR;
		}
		//	Note : 연산 특성상 "LengthXZ==0" 이 되지 않도록 제한할 필요가 있다.
		//
		else
		{
			const static float fLIMIT = D3DX_PI/2.0f - 0.001f;
			if ( fLIMIT < thetaX )			
				thetaX = fLIMIT;
			else if ( -fLIMIT > thetaX )	
				thetaX = -fLIMIT;
		}

		//	Note : 카메라 방향 단위 백터 계산.
		//
		//		Dir = { 1, 0, 0 } 의 초기값에서 회전 시킴.
		//
		D3DXVECTOR3 Dir;

		//	Note : 피치회전.
		//
		Dir.y = (float) sin(thetaX);

		Dir.x = 1-Dir.y*Dir.y;
		Dir.x = ( Dir.x < 0 ) ? 0 : (float) sqrt(Dir.x);

		Dir.z = (float) (-Dir.x*sin(thetaY) );
		Dir.x = (float) ( Dir.x*cos(thetaY) );

		m_vFromPt = m_vLookatPt + Dir*Length;
	}
}

void DxViewPort::CameraHeight ( float fHeight )
{
	float fdxHeight = fHeight - m_vLookatPt.y;

	m_vFromPt.y += fdxHeight;
	m_vLookatPt.y += fdxHeight;
}

void DxViewPort::SetCameraType ( CAMERA_TYPE toType )
{
	m_emCamera = toType;
}

void DxViewPort::CameraQuake ( float fLife, float fTerm, float fScale )
{
	m_bQUAKE_ACTIVE = TRUE;
	//m_fQUAKE_LIFE = fLife;
	m_fQUAKE_TERM = fTerm;
	//m_fQUAKE_SCALE = fScale;

	m_fQUAKE_LIFE = 1.f;	//	이값으로 바꾸어 주어야 하는데...현 다른곳 체크인으로 못하고 있음.
	m_fQUAKE_SCALE = 1.f;

	m_fQUAKE_TIMER = 0;
	m_fQUAKE_TERM_TIMER = 0;
	m_vQUAKE_FRMDIS = D3DXVECTOR3(0,0,0);
	m_vQUAKE_LKTDIS = D3DXVECTOR3(0,0,0);
}


HRESULT DxViewPort::_FrameMove_20111118 ( float fTime, float fElapsedTime )
{
	if ( !m_pd3dDevice )	return S_FALSE;

	m_fElapsedTime = fElapsedTime;

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
	BOOL bTARPOSWIN = GetMouseTargetPosWnd ( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

	BOOL bCURSOR_SHOW(TRUE);
	bool bHandling(false);
	if ( m_bHandling )
	{
		bHandling = (m_dwFlag&DXVP_GAME) || (m_dwFlag&DXVP_TOOL) || bTARPOSWIN;
	}

	if ( bHandling && m_dwFlag&DXVP_USER )
	{
		DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		DWORD dwCAPSLOCK(DXKEY_IDLE);
		DWORD dwKEY_PGUP(DXKEY_IDLE), dwKEY_PGDN(DXKEY_IDLE);
		DWORD dwKEY_ADD(DXKEY_IDLE), dwKEY_SUBTRACT(DXKEY_IDLE);
		DWORD dwKEY_RIGHT(DXKEY_IDLE), dwKEY_LEFT(DXKEY_IDLE);
		DWORD dwKEY_UP(DXKEY_IDLE), dwKEY_DOWN(DXKEY_IDLE);

		int dx(0), dy(0), dz(0);
		DWORD dwMOUSEKEY_L(NULL), dwMOUSEKEY_R(NULL), dwMOUSEKEY_M(NULL);

		if ( m_bKeyboard )
		{
			// Game과 Tool은 버튼에 따라 동작 방식이 다르다.
			if ( m_dwFlag & DXVP_GAME )
			{
				// Game에서 카메라 회전을 방향키로 함.
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_RIGHT);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_LEFT);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_UP);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_DOWN);
			}
			else if ( m_dwFlag & DXVP_TOOL )
			{
				// Tool에서 앞뒤좌우를 WSAD로 함
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_D);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_A);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_W);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_S);
			}

			if ( (dwKEY_RIGHT|dwKEY_LEFT|dwKEY_UP|dwKEY_DOWN)==DXKEY_IDLE )
			{
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_NUMPAD6);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_NUMPAD4);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_NUMPAD8);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_NUMPAD2);
			}

			dwKEY_PGUP = dxInputDev.GetKeyState(DIK_PGUP);
			dwKEY_PGDN = dxInputDev.GetKeyState(DIK_PGDN);

			dwKEY_ADD = dxInputDev.GetKeyState(DIK_ADD);
			dwKEY_SUBTRACT = dxInputDev.GetKeyState(DIK_SUBTRACT);

			dwCAPSLOCK = dxInputDev.GetKeyState(DIK_CAPSLOCK);
		}

		dxInputDev.GetMouseMove ( dx, dy, dz );
		dwMOUSEKEY_L = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
		dwMOUSEKEY_R = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );
		dwMOUSEKEY_M = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );

		float fZoom(0.0f);
		D3DXVECTOR3 vMove(0,0,0);
		D3DXVECTOR3 vRotate(0,0,0);

		if ( m_dwFlag&DXVP_GAME )
		{
			if ( dwKEY_RIGHT & DXKEY_PRESSED )
			{
				vRotate.y += fElapsedTime*D3DX_PI/2;
			}
			if ( dwKEY_LEFT & DXKEY_PRESSED )
			{
				vRotate.y -= fElapsedTime*D3DX_PI/2;
			}

			if ( dwKEY_UP & DXKEY_PRESSED )
			{
				vRotate.x += fElapsedTime*D3DX_PI/2;
			}
			if ( dwKEY_DOWN & DXKEY_PRESSED )
			{
				vRotate.x -= fElapsedTime*D3DX_PI/2;
			}
		}
		else
		{
			if ( dwKEY_RIGHT & DXKEY_PRESSED )		vMove.x += fElapsedTime*m_fVELOMOVE;
			if ( dwKEY_LEFT & DXKEY_PRESSED )		vMove.x -= fElapsedTime*m_fVELOMOVE;

			if ( dwKEY_PGUP & DXKEY_PRESSED )		vMove.y += fElapsedTime*m_fVELOMOVE;
			if ( dwKEY_PGDN & DXKEY_PRESSED )		vMove.y -= fElapsedTime*m_fVELOMOVE;

			if ( dwKEY_UP & DXKEY_PRESSED )			vMove.z += fElapsedTime*m_fVELOMOVE;
			if ( dwKEY_DOWN & DXKEY_PRESSED )		vMove.z -= fElapsedTime*m_fVELOMOVE;
		}

		if ( dwKEY_ADD & DXKEY_PRESSED )			fZoom -= fElapsedTime*m_fVELOZOOM;
		if ( dwKEY_SUBTRACT & DXKEY_PRESSED )		fZoom += fElapsedTime*m_fVELOZOOM;

		if ( (dwKEY_RIGHT|dwKEY_LEFT|dwKEY_UP|dwKEY_DOWN)!=DXKEY_IDLE )
		{
			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
		}
		//#ifdef CH_PARAM
		//		else if ( dwMOUSEKEY_R & DXKEY_DRAG )
		//#else
		else if ( dwMOUSEKEY_M & DXKEY_DRAG )
			//#endif
		{
			vRotate.x += m_fVELODIR*(dy);
			vRotate.y += m_fVELODIR*(dx);

			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();
			bCURSOR_SHOW = FALSE;
		}
		else if ( dwCAPSLOCK & DXKEY_PRESSED )
		{
			vRotate.x += m_fVELODIR*(dy);
			vRotate.y += m_fVELODIR*(dx);

			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();

			bCURSOR_SHOW = FALSE;
		}
		else
		{
			vRotate = m_damperVeloDir.Calculate ( D3DXVECTOR3(0,0,0), fElapsedTime );
		}

		// 튜토리얼 중일때, 카메라 회전 정보 저장.
		if ( m_bTutorial )
		{
			m_vTutoCamRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_vTutoCamRot.x += vRotate.x;
			m_vTutoCamRot.y += vRotate.y;
		}
		//

		fZoom += m_fVELOZOOM*(dz)/1000.0f;

		fZoom = m_damperVeloZoom.Calculate ( fZoom, fElapsedTime );

		if ( m_bTutorial )
		{
			m_fTutoCamZoom = 0.0f;

			if ( fZoom != 0.0f )
			{
				m_fTutoCamZoom = fZoom;
			}
		}

		if ( m_bTutorial && !m_bTutoCanCamMove )
		{
			// 카메라 제어 불능 상태.
			D3DXVECTOR3 vDefaultCameraPos = m_vTutoCamPos;
			float fDefaultCameraLength = m_fTutoCamLength;
			float fDefaultThetaX = m_fTutoThetaX;
			float fDefaultThetaY = m_fTutoThetaY;

			float fZoom(0.0f);
			D3DXVECTOR3 vMove(0,0,0);
			D3DXVECTOR3 vRotate(0,0,0);
			float fDeltaLength = 30.0f;
			float fDeltaRotate = 1.0f;

			// Note : 카메라 줌을 default와 맞추기. by luxes.
			//
			{
				if ( m_fLength - fDefaultCameraLength > 0.0f )
				{
					fZoom -= ( fElapsedTime * fDeltaLength );
					CameraZoom ( fZoom );

					if (  m_fLength - fDefaultCameraLength < 0.0f )
					{
						float fZoom(0.0f);

						fZoom = fZoom - ( m_fLength - fDefaultCameraLength );
						CameraZoom ( fZoom );
					}
				}
				else
				{
					fZoom += ( fElapsedTime * fDeltaLength );
					CameraZoom ( fZoom );

					if (  m_fLength - fDefaultCameraLength > 0.0f )
					{
						float fZoom(0.0f);

						fZoom = fZoom - ( m_fLength - fDefaultCameraLength );
						CameraZoom ( fZoom );
					}
				}
			}
			//

			// Note : 카메라 회전 정도를 default와 맞추기. by luxes.
			//
			{
				D3DXVECTOR3 vCameraDir = m_vFromPt - m_vLookatPt;

				//	Note : 현재의 회전값.
				//
				float Length, LengthXZ;
				float thetaX, thetaY;

				Length = D3DXVec3Length ( &vCameraDir );
				if ( Length == 0 )		Length = 0.001f;

				thetaX = (float) asin ( vCameraDir.y / Length );

				// Note : X축 회전 정도 맞추기. by luxes.
				//
				if ( thetaX  - fDefaultThetaX > 0.0f )
				{	// 현재의 X축 회전 값과 default 값이 다르다면 회전.
					vRotate.x -= ( fElapsedTime * fDeltaRotate );
					CameraRotation ( vRotate );

					// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
					//
					vCameraDir = m_vFromPt - m_vLookatPt;

					Length = D3DXVec3Length ( &vCameraDir );
					if ( Length == 0 )		Length = 0.001f;

					thetaX = (float) asin ( vCameraDir.y / Length );

					if ( thetaX - fDefaultThetaX < 0.0f )
					{	// default와 X축 회전 정도를 같게 맞춰준다. by luxes.
						D3DXVECTOR3 vRotate(0,0,0);

						vRotate.x = vRotate.x - ( thetaX  - fDefaultThetaX );
						CameraRotation ( vRotate );
					}
				}
				else
				{	// 현재의 X축 회전 값과 default 값이 다르다면 회전.
					vRotate.x += ( fElapsedTime * fDeltaRotate );
					CameraRotation ( vRotate );

					// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
					//
					vCameraDir = m_vFromPt - m_vLookatPt;

					Length = D3DXVec3Length ( &vCameraDir );
					if ( Length == 0 )		Length = 0.001f;

					thetaX = (float) asin ( vCameraDir.y / Length );

					if ( thetaX - fDefaultThetaX > 0.0f )
					{	// default와 X축 회전 정도를 같게 맞춰준다. by luxes.
						D3DXVECTOR3 vRotate(0,0,0);

						vRotate.x = vRotate.x - ( thetaX  - fDefaultThetaX );
						CameraRotation ( vRotate );
					}
				}
				//

				vRotate = D3DXVECTOR3(0,0,0);

				vCameraDir = m_vFromPt - m_vLookatPt;

				//	Note : 현재의 회전값.
				//

				//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
				//
				LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
				if ( LengthXZ == 0 )	LengthXZ = 0.001f;

				thetaY = (float) acos ( vCameraDir.x / LengthXZ );
				if ( vCameraDir.z>0 )	thetaY = -thetaY;

				thetaY = (float) acos ( vCameraDir.x / LengthXZ );
				if ( vCameraDir.z>0 )	thetaY = -thetaY;
				//

				// Note : Y축 회전 정도 맞추기. by luxes.
				//

				if ( thetaY - fDefaultThetaY < 0.0f )
				{
					float fSub_Origin = fDefaultThetaY - thetaY;
					float fSub_Other = abs( D3DX_PI * 2 - fSub_Origin );

					if ( fSub_Other < fSub_Origin )
					{
						thetaY = fDefaultThetaY + fSub_Other;
					}
				}
				else
				{
					float fSub_Origin = thetaY - fDefaultThetaY;
					float fSub_Other = abs( D3DX_PI * 2 - fSub_Origin );

					if ( fSub_Other < fSub_Origin )
					{
						thetaY = fDefaultThetaY - fSub_Other;
					}
				}

				if ( thetaY - fDefaultThetaY > 0.0f )
				{	// 현재의 Y축 회전 값과 default 값이 다르다면 회전.
					vRotate.y -= ( fElapsedTime * fDeltaRotate );
					CameraRotation ( vRotate );

					// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
					//
					vCameraDir = m_vFromPt - m_vLookatPt;

					//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
					//
					LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
					if ( LengthXZ == 0 )	LengthXZ = 0.001f;

					thetaY = (float) acos ( vCameraDir.x / LengthXZ );
					if ( vCameraDir.z>0 )	thetaY = -thetaY;

					thetaY = (float) acos ( vCameraDir.x / LengthXZ );
					if ( vCameraDir.z>0 )	thetaY = -thetaY;
					//

					float fSub_Origin = fDefaultThetaY - thetaY;
					float fSub_Other = abs( D3DX_PI * 2 - fSub_Origin );

					if ( thetaY - fDefaultThetaY < 0.0f && !( fSub_Other < fSub_Origin ) )
					{	// default와 Y축 회전 정도를 같게 맞춰준다. by luxes.
						D3DXVECTOR3 vRotate(0,0,0);

						vRotate.y = vRotate.y - ( thetaY - fDefaultThetaY );
						CameraRotation ( vRotate );
					}
				}
				else
				{	// 현재의 Y축 회전 값과 default 값이 다르다면 회전.
					vRotate.y += ( fElapsedTime * fDeltaRotate );
					CameraRotation ( vRotate );

					// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
					//
					vCameraDir = m_vFromPt - m_vLookatPt;

					//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
					//
					LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
					if ( LengthXZ == 0 )	LengthXZ = 0.001f;

					thetaY = (float) acos ( vCameraDir.x / LengthXZ );
					if ( vCameraDir.z>0 )	thetaY = -thetaY;

					thetaY = (float) acos ( vCameraDir.x / LengthXZ );
					if ( vCameraDir.z>0 )	thetaY = -thetaY;
					//

					float fSub_Origin = thetaY - fDefaultThetaY;
					float fSub_Other = abs( D3DX_PI * 2 - fSub_Origin );

					if ( thetaY - fDefaultThetaY > 0.0f && !( fSub_Other < fSub_Origin ) )
					{	// default와 Y축 회전 정도를 같게 맞춰준다. by luxes.
						D3DXVECTOR3 vRotate(0,0,0);

						vRotate.y = vRotate.y - ( thetaY - fDefaultThetaY );
						CameraRotation ( vRotate );
					}
				}
				//
			}
			//
		}
		else
		{
			//	Note : 카메라 변경.
			//
			/*if ( vMove.x || vMove.y || vMove.z )*/			CameraMove ( vMove );
			/*if ( vRotate.x || vRotate.y || vRotate.z )*/		CameraRotation ( vRotate );


			CameraZoom ( fZoom );
		}
	}

	//else if ( !GLBaseTutorial::GetInstance().IsCanMouseControlInTutorial() )
	//{	// 카메라 제어 불능 상태.
	//	D3DXVECTOR3 vDefaultCameraPos = GLBaseTutorial::GetInstance().GetDefaultCameraPos();
	//	float fDefaultCameraLength = GLBaseTutorial::GetInstance().GetDefaultCameraLength();
	//	float fDefaultThetaX = GLBaseTutorial::GetInstance().GetDefaultCameraThetaX();
	//	float fDefaultThetaY = GLBaseTutorial::GetInstance().GetDefaultCameraThetaY();

	//	float fZoom(0.0f);
	//	D3DXVECTOR3 vMove(0,0,0);
	//	D3DXVECTOR3 vRotate(0,0,0);
	//	float fDeltaLength = 30.0f;
	//	float fDeltaRotate = 1.0f;

	//	// Note : 카메라 줌을 default와 맞추기. by luxes.
	//	//
	//	{
	//		if ( m_fLength - fDefaultCameraLength > 0.0f )
	//		{
	//			fZoom -= ( fElapsedTime * fDeltaLength );
	//			CameraZoom ( fZoom );

	//			if (  m_fLength - fDefaultCameraLength < 0.0f )
	//			{
	//				float fZoom(0.0f);

	//				fZoom = fZoom - ( m_fLength - fDefaultCameraLength );
	//				CameraZoom ( fZoom );
	//			}
	//		}
	//		else
	//		{
	//			fZoom += ( fElapsedTime * fDeltaLength );
	//			CameraZoom ( fZoom );

	//			if (  m_fLength - fDefaultCameraLength > 0.0f )
	//			{
	//				float fZoom(0.0f);

	//				fZoom = fZoom - ( m_fLength - fDefaultCameraLength );
	//				CameraZoom ( fZoom );
	//			}
	//		}
	//	}
	//	//

	//	// Note : 카메라 회전 정도를 default와 맞추기. by luxes.
	//	//
	//	{
	//		D3DXVECTOR3 vCameraDir = m_vFromPt - m_vLookatPt;

	//		//	Note : 현재의 회전값.
	//		//
	//		float Length, LengthXZ;
	//		float thetaX, thetaY;

	//		Length = D3DXVec3Length ( &vCameraDir );
	//		if ( Length == 0 )		Length = 0.001f;

	//		thetaX = (float) asin ( vCameraDir.y / Length );

	//		// Note : X축 회전 정도 맞추기. by luxes.
	//		//
	//		if ( thetaX  - fDefaultThetaX > 0.0f )
	//		{	// 현재의 X축 회전 값과 default 값이 다르다면 회전.
	//			vRotate.x -= ( fElapsedTime * fDeltaRotate );
	//			CameraRotation ( vRotate );

	//			// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
	//			//
	//			vCameraDir = m_vFromPt - m_vLookatPt;

	//			Length = D3DXVec3Length ( &vCameraDir );
	//			if ( Length == 0 )		Length = 0.001f;

	//			thetaX = (float) asin ( vCameraDir.y / Length );

	//			if ( thetaX - fDefaultThetaX < 0.0f )
	//			{	// default와 X축 회전 정도를 같게 맞춰준다. by luxes.
	//				D3DXVECTOR3 vRotate(0,0,0);

	//				vRotate.x = vRotate.x - ( thetaX  - fDefaultThetaX );
	//				CameraRotation ( vRotate );
	//			}
	//		}
	//		else
	//		{	// 현재의 X축 회전 값과 default 값이 다르다면 회전.
	//			vRotate.x += ( fElapsedTime * fDeltaRotate );
	//			CameraRotation ( vRotate );

	//			// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
	//			//
	//			vCameraDir = m_vFromPt - m_vLookatPt;

	//			Length = D3DXVec3Length ( &vCameraDir );
	//			if ( Length == 0 )		Length = 0.001f;

	//			thetaX = (float) asin ( vCameraDir.y / Length );

	//			if ( thetaX - fDefaultThetaX > 0.0f )
	//			{	// default와 X축 회전 정도를 같게 맞춰준다. by luxes.
	//				D3DXVECTOR3 vRotate(0,0,0);

	//				vRotate.x = vRotate.x - ( thetaX  - fDefaultThetaX );
	//				CameraRotation ( vRotate );
	//			}
	//		}
	//		//

	//		vRotate = D3DXVECTOR3(0,0,0);

	//		vCameraDir = m_vFromPt - m_vLookatPt;

	//		//	Note : 현재의 회전값.
	//		//

	//		//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//		//
	//		LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	//		if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	//		thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//		if ( vCameraDir.z>0 )	thetaY = -thetaY;

	//		thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//		if ( vCameraDir.z>0 )	thetaY = -thetaY;
	//		//

	//		// Note : Y축 회전 정도 맞추기. by luxes.
	//		//
	//		if ( thetaY  - fDefaultThetaY > 0.0f )
	//		{	// 현재의 Y축 회전 값과 default 값이 다르다면 회전.
	//			vRotate.y -= ( fElapsedTime * fDeltaRotate );
	//			CameraRotation ( vRotate );

	//			// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
	//			//
	//			vCameraDir = m_vFromPt - m_vLookatPt;

	//			//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//			//
	//			LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	//			if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	//			thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//			if ( vCameraDir.z>0 )	thetaY = -thetaY;

	//			thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//			if ( vCameraDir.z>0 )	thetaY = -thetaY;
	//			//

	//			if ( thetaY - fDefaultThetaY < 0.0f )
	//			{	// default와 Y축 회전 정도를 같게 맞춰준다. by luxes.
	//				D3DXVECTOR3 vRotate(0,0,0);

	//				vRotate.y = vRotate.y - ( thetaY  - fDefaultThetaY );
	//				CameraRotation ( vRotate );
	//			}
	//		}
	//		else
	//		{	// 현재의 Y축 회전 값과 default 값이 다르다면 회전.
	//			vRotate.y += ( fElapsedTime * fDeltaRotate );
	//			CameraRotation ( vRotate );

	//			// Note : 회전 시킨 후 default를 넘어가면, default와 회전 정도를 같게 맞춰준다. by luxes.
	//			//
	//			vCameraDir = m_vFromPt - m_vLookatPt;

	//			//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//			//
	//			LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	//			if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	//			thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//			if ( vCameraDir.z>0 )	thetaY = -thetaY;

	//			thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	//			if ( vCameraDir.z>0 )	thetaY = -thetaY;
	//			//

	//			if ( thetaY - fDefaultThetaY > 0.0f )
	//			{	// default와 Y축 회전 정도를 같게 맞춰준다. by luxes.
	//				D3DXVECTOR3 vRotate(0,0,0);

	//				vRotate.y = vRotate.y - ( thetaY  - fDefaultThetaY );
	//				CameraRotation ( vRotate );
	//			}
	//		}
	//		//
	//	}
	//	//
	//}

	CameraLimitCheck();

	//	Note : 뷰 메트릭스 적용.
	//
	D3DXVECTOR3 vUP = ComputeUpVecter ( m_vLookatPt, m_vFromPt );
	D3DXMatrixLookAtLH ( &m_matView, &m_vFromPt, &m_vLookatPt, &vUP );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );

	//	Note : 계산중복을 막기위함
	//
	m_vLookDir = m_vLookatPt - m_vFromPt;
	m_fDistance = D3DXVec3Length ( &m_vLookDir );
	if ( m_fDistance == 0.f )	m_fDistance = 0.001f;

	D3DXVec3Normalize ( &m_vLookDir, &m_vLookDir );

	if ( m_vLookDir.x==0.f && m_vLookDir.z==0.f )
	{
		m_vLookDir_Y0.x = 1.f;
		m_vLookDir_Y0.z = 0.f;
	}
	else
	{
		m_vLookDir_Y0.x = m_vLookDir.x;
		m_vLookDir_Y0.z = m_vLookDir.z;
		D3DXVec3Normalize ( &m_vLookDir_Y0, &m_vLookDir_Y0 );
	}

	//	Note : 뷰프리즘 클립핑 볼륨.
	//
	ComputeClipVolume ();

	if ( (m_dwFlag & DXVP_GAME) || (m_dwFlag & DXVP_TOOL) )
	{
		CCursor::GetInstance().SetShowCursor ( bCURSOR_SHOW );
	}

	return S_OK;
}

HRESULT DxViewPort::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !m_pd3dDevice )	
		return S_FALSE;

	m_fElapsedTime = fElapsedTime;

	CameraLimitCheck();

	//	Note : 뷰 메트릭스 적용.
	//
	D3DXVECTOR3 vUP = ComputeUpVecter ( m_vLookatPt, m_vFromPt );
	D3DXMatrixLookAtLH ( &m_matView, &m_vFromPt, &m_vLookatPt, &vUP );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );

	//	Note : 계산중복을 막기위함
	//
	m_vLookDir = m_vLookatPt - m_vFromPt;
	m_fDistance = D3DXVec3Length ( &m_vLookDir );
	if ( m_fDistance == 0.f )	m_fDistance = 0.001f;

	D3DXVec3Normalize ( &m_vLookDir, &m_vLookDir );

	if ( m_vLookDir.x==0.f && m_vLookDir.z==0.f )
	{
		m_vLookDir_Y0.x = 1.f;
		m_vLookDir_Y0.z = 0.f;
	}
	else
	{
		m_vLookDir_Y0.x = m_vLookDir.x;
		m_vLookDir_Y0.z = m_vLookDir.z;
		D3DXVec3Normalize ( &m_vLookDir_Y0, &m_vLookDir_Y0 );
	}

	//	Note : 뷰프리즘 클립핑 볼륨.
	//
	ComputeClipVolume ();

	return S_OK;
}

void DxViewPort::FrameMoveRAN ( float fTime, float fElapsedTime )
{
	if ( !m_pd3dDevice )	return;

	m_fElapsedTime = fElapsedTime;

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
	BOOL bTARPOSWIN = GetMouseTargetPosWnd ( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

	BOOL bCURSOR_SHOW(TRUE);
	bool bHandling(true);

	if ( bHandling && m_dwFlag&DXVP_USER )
	{
		DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		DWORD dwCAPSLOCK(DXKEY_IDLE);
		DWORD dwKEY_PGUP(DXKEY_IDLE), dwKEY_PGDN(DXKEY_IDLE);
		DWORD dwKEY_ADD(DXKEY_IDLE), dwKEY_SUBTRACT(DXKEY_IDLE);
		DWORD dwKEY_RIGHT(DXKEY_IDLE), dwKEY_LEFT(DXKEY_IDLE);
		DWORD dwKEY_UP(DXKEY_IDLE), dwKEY_DOWN(DXKEY_IDLE);

		int dx(0), dy(0), dz(0);
		DWORD dwMOUSEKEY_L(NULL), dwMOUSEKEY_R(NULL), dwMOUSEKEY_M(NULL);

		if ( m_bKeyboard )
		{
			// Game과 Tool은 버튼에 따라 동작 방식이 다르다.
			if ( m_dwFlag & DXVP_GAME )
			{
				// Game에서 카메라 회전을 방향키로 함.
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_RIGHT);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_LEFT);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_UP);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_DOWN);
			}
			else if ( m_dwFlag & DXVP_TOOL )
			{
				// Tool에서 앞뒤좌우를 WSAD로 함
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_D);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_A);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_W);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_S);
			}

			if ( (dwKEY_RIGHT|dwKEY_LEFT|dwKEY_UP|dwKEY_DOWN)==DXKEY_IDLE )
			{
				dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_NUMPAD6);
				dwKEY_LEFT = dxInputDev.GetKeyState(DIK_NUMPAD4);

				dwKEY_UP = dxInputDev.GetKeyState(DIK_NUMPAD8);
				dwKEY_DOWN = dxInputDev.GetKeyState(DIK_NUMPAD2);
			}

			dwKEY_PGUP = dxInputDev.GetKeyState(DIK_PGUP);
			dwKEY_PGDN = dxInputDev.GetKeyState(DIK_PGDN);

			dwKEY_ADD = dxInputDev.GetKeyState(DIK_ADD);
			dwKEY_SUBTRACT = dxInputDev.GetKeyState(DIK_SUBTRACT);

			dwCAPSLOCK = dxInputDev.GetKeyState(DIK_CAPSLOCK);
		}

		dxInputDev.GetMouseMove ( dx, dy, dz );
		dwMOUSEKEY_L = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
		dwMOUSEKEY_R = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );
		dwMOUSEKEY_M = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );

		float fZoom(0.0f);
		D3DXVECTOR3 vMove(0,0,0);
		D3DXVECTOR3 vRotate(0,0,0);

		if ( dwKEY_RIGHT & DXKEY_PRESSED )		vMove.x += fElapsedTime*m_fVELOMOVE;
		if ( dwKEY_LEFT & DXKEY_PRESSED )		vMove.x -= fElapsedTime*m_fVELOMOVE;

		if ( dwKEY_PGUP & DXKEY_PRESSED )		vMove.y += fElapsedTime*m_fVELOMOVE;
		if ( dwKEY_PGDN & DXKEY_PRESSED )		vMove.y -= fElapsedTime*m_fVELOMOVE;

		if ( dwKEY_UP & DXKEY_PRESSED )			vMove.z += fElapsedTime*m_fVELOMOVE;
		if ( dwKEY_DOWN & DXKEY_PRESSED )		vMove.z -= fElapsedTime*m_fVELOMOVE;

		if ( dwKEY_ADD & DXKEY_PRESSED )			fZoom -= fElapsedTime*m_fVELOZOOM;
		if ( dwKEY_SUBTRACT & DXKEY_PRESSED )		fZoom += fElapsedTime*m_fVELOZOOM;

		if ( (dwKEY_RIGHT|dwKEY_LEFT|dwKEY_UP|dwKEY_DOWN)!=DXKEY_IDLE )
		{
			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
		}
		//#ifdef CH_PARAM
		//		else if ( dwMOUSEKEY_R & DXKEY_DRAG )
		//#else
		else if ( dwMOUSEKEY_M & DXKEY_DRAG )
			//#endif
		{
			vRotate.x += m_fVELODIR*(dy);
			vRotate.y += m_fVELODIR*(dx);

			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();
			bCURSOR_SHOW = FALSE;
		}
		else if ( dwCAPSLOCK & DXKEY_PRESSED )
		{
			vRotate.x += m_fVELODIR*(dy);
			vRotate.y += m_fVELODIR*(dx);

			vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();

			bCURSOR_SHOW = FALSE;
		}
		else
		{
			vRotate = m_damperVeloDir.Calculate ( D3DXVECTOR3(0,0,0), fElapsedTime );
		}

		fZoom += m_fVELOZOOM*(dz)/1000.0f;

		fZoom = m_damperVeloZoom.Calculate ( fZoom, fElapsedTime );

		if ( m_bTutorial )
		{
			m_fTutoCamZoom = 0.0f;

			if ( fZoom != 0.0f )
			{
				m_fTutoCamZoom = fZoom;
			}
		}

		//	Note : 카메라 변경.
		//
		CameraMove ( vMove );
		CameraRotation ( vRotate );
		CameraZoom ( fZoom );
	}

	CCursor::GetInstance().SetShowCursor ( bCURSOR_SHOW );

	FrameMove(fTime, fElapsedTime);
}

void DxViewPort::ComputeBoundBox ( D3DXVECTOR3 *pt, float fClip_FarPlane )	//m_fClip_FarPlane
{
	FLOAT dist, t;
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 m_vDir, m_vUp, m_vPos;	
	D3DXVECTOR3 m_vCross;

	m_vDir.x = m_vLookatPt.x - m_vFromPt.x;
	m_vDir.y = m_vLookatPt.y - m_vFromPt.y;
	m_vDir.z = m_vLookatPt.z - m_vFromPt.z;

	m_vUp.x = m_vUpVec.x;
	m_vUp.y = m_vUpVec.y;
	m_vUp.z = m_vUpVec.z;

	m_vPos.x = m_vFromPt.x;
	m_vPos.y = m_vFromPt.y;
	m_vPos.z = m_vFromPt.z;

	D3DXVec3Normalize ( &m_vUp, &m_vUp );
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	D3DXVec3Cross ( &m_vCross, &m_vUp, &m_vDir );
	D3DXVec3Normalize ( &m_vCross, &m_vCross );

	D3DXVec3Cross ( &m_vUp, &m_vDir, &m_vCross );
	D3DXVec3Normalize ( &m_vUp, &m_vUp );


	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fClip_FarPlane : m_fNearPlane;
		pt[i].x = dist * m_vDir.x;
		pt[i].y = dist * m_vDir.y;
		pt[i].z = dist * m_vDir.z;

		//	(2)
		t = dist * tanf(m_fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i].x += m_vUp.x * t;
		pt[i].y += m_vUp.y * t;
		pt[i].z += m_vUp.z * t;

		t = dist * tanf(m_fFOV/2) * m_fWidth/m_fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i].x += m_vCross.x * t;
		pt[i].y += m_vCross.y * t;
		pt[i].z += m_vCross.z * t;
		pt[i].x = m_vPos.x + pt[i].x;
		pt[i].y = m_vPos.y + pt[i].y;
		pt[i].z = m_vPos.z + pt[i].z;
	}
}

D3DXVECTOR3 DxViewPort::ComputeUpVecter ( const D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vFromPt )
{
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = m_vLookatPt - m_vFromPt;
	vUp = m_vUpVec;
	vPos = m_vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	return vUp;
}

void DxViewPort::ComputeClipPoint ()
{
	FLOAT dist, t;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = m_vLookatPt - m_vFromPt;
	vUp = m_vUpVec;
	vPos = m_vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? m_fClip_FarPlane : m_fNearPlane;
		m_ptViewPort[i] = dist * vDir;

		//	(2)
		t = dist * tanf(m_fFOV/2);
		t = (i & 0x2) ? t : -t;
		m_ptViewPort[i] += vUp * t;

		t = dist * tanf(m_fFOV/2) * m_fWidth/m_fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		m_ptViewPort[i] += vCross *t;
		m_ptViewPort[i] = vPos + m_ptViewPort[i];
	}
}

void DxViewPort::ComputeClipVolume ()
{
	ComputeClipPoint ();

	CLIPVOLUME cv;
	D3DXVECTOR3 v1, v2, n;

	//compute the near plane
	v1 = m_ptViewPort[2] - m_ptViewPort[0];
	v2 = m_ptViewPort[1] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pNear.a = n.x;
	cv.pNear.b = n.y;
	cv.pNear.c = n.z;
	cv.pNear.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);

	//compute the far plane
	v1 = m_ptViewPort[5] - m_ptViewPort[4];
	v2 = m_ptViewPort[6] - m_ptViewPort[4];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pFar.a = n.x;
	cv.pFar.b = n.y;
	cv.pFar.c = n.z;
	cv.pFar.d = -(n.x * m_ptViewPort[4].x + n.y * m_ptViewPort[4].y + n.z * m_ptViewPort[4].z);

	//compute the top plane
	v1 = m_ptViewPort[6] - m_ptViewPort[2];
	v2 = m_ptViewPort[3] - m_ptViewPort[2];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pTop.a = n.x;
	cv.pTop.b = n.y;
	cv.pTop.c = n.z;
	cv.pTop.d = -(n.x * m_ptViewPort[2].x + n.y * m_ptViewPort[2].y + n.z * m_ptViewPort[2].z);

	//compute the bottom plane
	v1 = m_ptViewPort[1] - m_ptViewPort[0];
	v2 = m_ptViewPort[4] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pBottom.a = n.x;
	cv.pBottom.b = n.y;
	cv.pBottom.c = n.z;
	cv.pBottom.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);

	//compute the left plane
	v1 = m_ptViewPort[3] - m_ptViewPort[1];
	v2 = m_ptViewPort[5] - m_ptViewPort[1];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pLeft.a = n.x;
	cv.pLeft.b = n.y;
	cv.pLeft.c = n.z;
	cv.pLeft.d = -(n.x * m_ptViewPort[1].x + n.y * m_ptViewPort[1].y + n.z * m_ptViewPort[1].z);

	//compute the right plane
	v1 = m_ptViewPort[4] - m_ptViewPort[0];
	v2 = m_ptViewPort[2] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pRight.a = n.x;
	cv.pRight.b = n.y;
	cv.pRight.c = n.z;
	cv.pRight.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);

	// 추가 클리핑
	{
		D3DXVECTOR3 vDir = m_vLookatPt - m_vFromPt;
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vCenter = m_vFromPt + vDir*m_fClip_FarPlane;

		// add plane
		D3DXVECTOR3 vPoint;
		for ( DWORD i=0; i<4; ++i )
		{
			vDir = m_ptViewPort[i+4] - vCenter;
			vPoint = vCenter + vDir * 0.6f;

			vDir = vPoint - m_vFromPt;
			D3DXVec3Normalize( &vDir, &vDir );
			vPoint = m_vFromPt + vDir*m_fClip_FarPlane;

			D3DXPlaneFromPointNormal( &cv.pDetail[i], &vPoint, &vDir );
		}
	}

	m_CV = cv;
}

void DxViewPort::ComputeClipVolumeOrtho( CLIPVOLUME& cv, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, float fWidth, float fHeight )
{
	FLOAT dist, t;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = m_vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? 10000.f : 1.f;
		m_ptViewPort[i] = dist * vDir;

		//	(2)
		t = fHeight*0.5f;
		t = (i & 0x2) ? t : -t;
		m_ptViewPort[i] += vUp * t;

		t = fWidth*0.5f;
		t = (i & 0x1) ? -t : t;
		m_ptViewPort[i] += vCross *t;
		m_ptViewPort[i] = vPos + m_ptViewPort[i];
	}

	D3DXVECTOR3 v1, v2, n;

	//compute the near plane
	v1 = m_ptViewPort[2] - m_ptViewPort[0];
	v2 = m_ptViewPort[1] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pNear.a = n.x;
	cv.pNear.b = n.y;
	cv.pNear.c = n.z;
	cv.pNear.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);

	//compute the far plane
	v1 = m_ptViewPort[5] - m_ptViewPort[4];
	v2 = m_ptViewPort[6] - m_ptViewPort[4];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pFar.a = n.x;
	cv.pFar.b = n.y;
	cv.pFar.c = n.z;
	cv.pFar.d = -(n.x * m_ptViewPort[4].x + n.y * m_ptViewPort[4].y + n.z * m_ptViewPort[4].z);

	//compute the top plane
	v1 = m_ptViewPort[6] - m_ptViewPort[2];
	v2 = m_ptViewPort[3] - m_ptViewPort[2];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pTop.a = n.x;
	cv.pTop.b = n.y;
	cv.pTop.c = n.z;
	cv.pTop.d = -(n.x * m_ptViewPort[2].x + n.y * m_ptViewPort[2].y + n.z * m_ptViewPort[2].z);

	//compute the bottom plane
	v1 = m_ptViewPort[1] - m_ptViewPort[0];
	v2 = m_ptViewPort[4] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pBottom.a = n.x;
	cv.pBottom.b = n.y;
	cv.pBottom.c = n.z;
	cv.pBottom.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);

	//compute the left plane
	v1 = m_ptViewPort[3] - m_ptViewPort[1];
	v2 = m_ptViewPort[5] - m_ptViewPort[1];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pLeft.a = n.x;
	cv.pLeft.b = n.y;
	cv.pLeft.c = n.z;
	cv.pLeft.d = -(n.x * m_ptViewPort[1].x + n.y * m_ptViewPort[1].y + n.z * m_ptViewPort[1].z);

	//compute the right plane
	v1 = m_ptViewPort[4] - m_ptViewPort[0];
	v2 = m_ptViewPort[2] - m_ptViewPort[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pRight.a = n.x;
	cv.pRight.b = n.y;
	cv.pRight.c = n.z;
	cv.pRight.d = -(n.x * m_ptViewPort[0].x + n.y * m_ptViewPort[0].y + n.z * m_ptViewPort[0].z);
}

CLIPVOLUME DxViewPort::ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, float fNearLength, BOOL bFar )
{
	float		fFarPlane	= 500.f;// m_fFarPlane * 0.7f;		// 반사의 부하를 조금이라도 줄이자.
	if ( bFar )	fFarPlane	= 1200.f;
	float		fNearPlane	= m_fNearPlane;
	fNearPlane = ( fNearLength > m_fNearPlane )  ? fNearLength : m_fNearPlane;	// 클리핑.. 위해서		반사용 CV

	return ComputeClipVolume ( vLookatPt, vFromPt, vUpVec, fNearPlane, fFarPlane, m_fWidth, m_fHeight, m_fFOV );
}

CLIPVOLUME DxViewPort::ComputeClipVolumeGameSetting ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec )
{

	return ComputeClipVolume ( vLookatPt, vFromPt, vUpVec, m_fNearPlane, m_fFarPlane, m_fWidth, m_fHeight, m_fFOV );
}

CLIPVOLUME DxViewPort::ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, 
										  float fNearPlane, float fFarPlane, float fWidth, float fHeight, float fFOV )
{
	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;
	CLIPVOLUME	cv;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );


	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;

		t = dist * tanf(fFOV/2) * fWidth/fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}






	//compute the near plane
	v1 = pt[2] - pt[0];
	v2 = pt[1] - pt[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pNear.a = n.x;
	cv.pNear.b = n.y;
	cv.pNear.c = n.z;
	cv.pNear.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);

	//compute the far plane
	v1 = pt[5] - pt[4];
	v2 = pt[6] - pt[4];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pFar.a = n.x;
	cv.pFar.b = n.y;
	cv.pFar.c = n.z;
	cv.pFar.d = -(n.x * pt[4].x + n.y * pt[4].y + n.z * pt[4].z);

	//compute the top plane
	v1 = pt[6] - pt[2];
	v2 = pt[3] - pt[2];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pTop.a = n.x;
	cv.pTop.b = n.y;
	cv.pTop.c = n.z;
	cv.pTop.d = -(n.x * pt[2].x + n.y * pt[2].y + n.z * pt[2].z);

	//compute the bottom plane
	v1 = pt[1] - pt[0];
	v2 = pt[4] - pt[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pBottom.a = n.x;
	cv.pBottom.b = n.y;
	cv.pBottom.c = n.z;
	cv.pBottom.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);

	//compute the left plane
	v1 = pt[3] - pt[1];
	v2 = pt[5] - pt[1];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pLeft.a = n.x;
	cv.pLeft.b = n.y;
	cv.pLeft.c = n.z;
	cv.pLeft.d = -(n.x * pt[1].x + n.y * pt[1].y + n.z * pt[1].z);

	//compute the right plane
	v1 = pt[4] - pt[0];
	v2 = pt[2] - pt[0];

	D3DXVec3Cross ( &n, &v2, &v1 );
	D3DXVec3Normalize ( &n, &n );

	cv.pRight.a = n.x;
	cv.pRight.b = n.y;
	cv.pRight.c = n.z;
	cv.pRight.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);

	// 추가 클리핑
	{
		D3DXVECTOR3 vDir = vLookatPt - vFromPt;
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vCenter = vFromPt + vDir*fFarPlane;

		// add plane
		D3DXVECTOR3 vPoint;
		for ( DWORD i=0; i<4; ++i )
		{
			vDir = pt[i+4] - vCenter;
			vPoint = vCenter + vDir * 0.5f;

			vDir = vPoint - vFromPt;
			D3DXVec3Normalize( &vDir, &vDir );
			vPoint = vFromPt + vDir*fFarPlane;

			D3DXPlaneFromPointNormal( &cv.pDetail[i], &vPoint, &vDir );
		}
	}

	return cv;
}

BOOL DxViewPort::GetMouseTargetPosWnd ( D3DXVECTOR3& vTargetPos )
{
	GASSERT(m_hWnd&&"윈도우핸들이 초기화 되지 않았습니다.");

	RECT rtWindow;
	::GetWindowRect ( m_hWnd, &rtWindow );

	POINT ptMouse;
	::GetCursorPos ( &ptMouse );

	int nWindowWidth = rtWindow.right - rtWindow.left;
	int nWindowHeight = rtWindow.bottom - rtWindow.top;

	if ( rtWindow.left <= ptMouse.x && rtWindow.right >= ptMouse.x
		&& rtWindow.top <= ptMouse.y && rtWindow.bottom >= ptMouse.y )
	{
		int mx, my;
		mx = ptMouse.x - rtWindow.left;
		my = nWindowHeight - (ptMouse.y - rtWindow.top);

		D3DXVECTOR3 vBoundBox[8];
		ComputeBoundBox ( vBoundBox, m_fClip_FarPlane );

		vTargetPos = vBoundBox[5];

		D3DXVECTOR3 vUp = vBoundBox[7] - vBoundBox[5];
		D3DXVECTOR3 vRight = vBoundBox[4] - vBoundBox[5];

		float fHeight = D3DXVec3Length ( &vUp );
		float fWidth = D3DXVec3Length ( &vRight );

		D3DXVec3Normalize ( &vUp, &vUp );
		D3DXVec3Normalize ( &vRight, &vRight );

		vTargetPos += vRight * fWidth * (float)mx / (float)nWindowWidth;
		vTargetPos += vUp * (float)my * fHeight / (float)nWindowHeight;

		return TRUE;
	}

	return FALSE;
}

float DxViewPort::ComputeDistSize ( const float fDist )
{
	FLOAT t;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = m_vLookatPt - m_vFromPt;
	vUp = m_vUpVec;
	vPos = m_vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	D3DXVECTOR3 vLow, vHigh;

	vLow = vDir * fDist;

	t = fDist * tanf(m_fFOV/2);
	vLow += vUp * t;

	t = fDist * tanf(m_fFOV/2) * m_fWidth/m_fHeight;
	vLow += vCross * t;
	vLow += vPos;

	vHigh = vDir * fDist;

	t = - fDist * tanf(m_fFOV/2);
	vHigh += vUp * t;

	t = - fDist * tanf(m_fFOV/2) * m_fWidth/m_fHeight;
	vHigh += vCross * t;
	vHigh += vPos;

	D3DXVECTOR3 vDx = vLow-vHigh;

	return D3DXVec3Length ( &vDx );
}

D3DXVECTOR3 DxViewPort::ComputeVec3Project ( const D3DXVECTOR3* pV, const D3DXMATRIX* pWorld )
{
	D3DXMATRIX matWorld;

	if ( !pWorld )
	{
		D3DXMatrixIdentity ( &matWorld );
		pWorld = &matWorld;
	}

	D3DXVECTOR3 vPosOut;
	D3DXVec3Project ( &vPosOut, pV, &m_ViewPort, &m_matProj, &m_matView, pWorld );

	return vPosOut;
}

void DxViewPort::SetCameraControl( bool bCameraControl, D3DXVECTOR3 vBaseCamPos /*= D3DXVECTOR3( 0.0f, 0.0f, 0.0f )*/, 
								  float fUp /*= 0.0f*/, float fDown /*= 0.0f*/, float fLeft /*= 0.0f*/, float fRight /*= 0.0f*/,
								  float fFar /*= 0.0f*/, float fNear /*= 0.0f*/ )
{
	if( !bCameraControl ) { m_sCameraControl.Init(); return; }

	m_sCameraControl.bCameraControl = TRUE;
	m_sCameraControl.vBaseCamPos    = vBaseCamPos;
	m_sCameraControl.fUp			= fUp;
	m_sCameraControl.fDown			= fDown;
	m_sCameraControl.fLeft			= fLeft;
	m_sCameraControl.fRight			= fRight;
	m_sCameraControl.fFar			= fFar;
	m_sCameraControl.fNear			= fNear;
}

HRESULT DxViewPort::SetTutorialCamera()
{
	D3DXVECTOR3 vDefaultCameraPos = m_vTutoCamPos;
	float fDefaultCameraLength = m_fTutoCamLength;
	float fDefaultThetaX = m_fTutoThetaX;
	float fDefaultThetaY = m_fTutoThetaY;

	float fZoom(0.0f);
	D3DXVECTOR3 vMove(0,0,0);
	D3DXVECTOR3 vRotate(0,0,0);
	float fDeltaLength = 30.0f;
	float fDeltaRotate = 1.0f;

	fZoom = fZoom - ( m_fLength - fDefaultCameraLength );
	CameraZoom ( fZoom );

	D3DXVECTOR3 vCameraDir = m_vFromPt - m_vLookatPt;

	//	Note : 현재의 회전값.
	//
	float Length, LengthXZ;
	float thetaX, thetaY;

	Length = D3DXVec3Length ( &vCameraDir );
	if ( Length == 0 )		Length = 0.001f;

	thetaX = (float) asin ( vCameraDir.y / Length );

	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//
	LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	if ( vCameraDir.z>0 )	thetaY = -thetaY;

	thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	if ( vCameraDir.z>0 )	thetaY = -thetaY;
	//

	vRotate.x = vRotate.x - ( thetaX  - fDefaultThetaX );
	vRotate.y = vRotate.y - ( thetaY  - fDefaultThetaY );
	CameraRotation ( vRotate );

	return S_OK;
}


