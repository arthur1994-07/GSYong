#include "stdafx.h"

#include "../../InternalCommonLib/Interface/DxInputDeviceInterface.h"

#include "../Define/DxVMDefine.h"

#include "DxViewPortVM.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


DxViewPortVM* DxViewPortVM::g_pInstance = NULL;
DxViewPortVM* DxViewPortVM::GetInstance()
{
	if( g_pInstance )
	{
		return g_pInstance;
	}
	else
	{
		g_pInstance = new DxViewPortVM;
		return g_pInstance;
	}
}

void DxViewPortVM::ReleaseInstance()
{
	SAFE_DELETE( g_pInstance );
}

DxViewPortVM::DxViewPortVM()
    : m_pd3dDevice(NULL)
    , m_fElapsedTime(0.f)
    , m_fFOV(0.f)
    , m_fWidth(0.f)
    , m_fHeight(0.f)
    , m_fNearPlane(0.f)
    , m_fFarPlane(0.f)
{
}

DxViewPortVM::~DxViewPortVM()
{
}

void DxViewPortVM::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd )
{
    m_hWnd = hWnd;
    m_pd3dDevice = pd3dDevice;
}

void DxViewPortVM::SetViewTrans( const D3DXVECTOR3 &vFromPt,const  D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vUpVec )
{
    m_vFromPt = vFromPt;
	m_vLookatPt = vLookatPt;
	m_vUpVec = vUpVec;

	SetViewTrans();
}

void DxViewPortVM::SetViewTrans()
{
    D3DXMatrixLookAtLH( &m_matView, &m_vFromPt, &m_vLookatPt, &m_vUpVec );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
}

void DxViewPortVM::SetProjection( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane )
{
    m_fFOV = fFOV;
    m_fWidth = fWidth;
    m_fHeight = fHeight;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;

	D3DXMatrixPerspectiveFovLH( &m_matProj, fFOV, m_fWidth/m_fHeight, fNearPlane, fFarPlane );
    m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

void DxViewPortVM::SetTransform()
{
    m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

HRESULT DxViewPortVM::SetViewPort()
{
	HRESULT hr;

	//	Note : 백 버퍼를 가져와서 화면의 사이즈를 알아낸다.. 
	//		( 주의 : 반드시 Release()를 해주어야 한다. )
	//
	LPDIRECT3DSURFACEQ pd3dBackBuffer;
	hr = m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dBackBuffer );
	if ( FAILED(hr) )
		return hr;

	D3DSURFACE_DESC surfaceDesc;
	pd3dBackBuffer->GetDesc ( &surfaceDesc );

	m_sViewPort.X = 0;
	m_sViewPort.Y = 0;
	m_sViewPort.Width = surfaceDesc.Width;
	m_sViewPort.Height = surfaceDesc.Height;
	m_sViewPort.MinZ = 0.0f;
	m_sViewPort.MaxZ = 1.0f;

	pd3dBackBuffer->Release ();	// * (중요) 백버퍼의 ref 를 감소시킨다.

	hr = m_pd3dDevice->SetViewport( &m_sViewPort );

    return hr;
}

HRESULT DxViewPortVM::FrameMove( float fElapsedTime )
{
	if ( !m_pd3dDevice )
		return S_FALSE;

    // Command가 선택되어 있는 상태라면 Edit 화면은 움직이지 못하도록 한다.
    if ( NSVISUAL_MATERIAL::g_nSelectCommandVM != -1 )
        return S_OK;

	m_fElapsedTime = fElapsedTime;

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
    BOOL bTARPOSWIN = GetMouseTargetPosWnd( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

    // 카메라 작동
    if ( bTARPOSWIN )
	{
        // Note : 초기화
        int         dx(0), dy(0), dz(0);
        float       fZoom(0.f);
        D3DXVECTOR3 vMove(0.f,0.f,0.f);
        ic::DxInputDeviceInterface::GetInstance()->GetMouseMove ( dx, dy, dz );
	    DWORD dwLButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT );
        DWORD dwRButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_RIGHT );
        DWORD dwMButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_MIDDLE );

        // Zoom In , Zoom Out
        fZoom += dz;

        // Zoom In , Zoom Out
	    if ( (dwLButton&ic::DXKEY_DRAG) && (dwRButton&ic::DXKEY_DRAG) )
	    {
            fZoom -= dy;
	    }
        // Move
        else if ( (dwLButton&ic::DXKEY_DRAG) || (dwRButton&ic::DXKEY_DRAG) )    
	    {
			vMove.x += dx*NSVISUAL_MATERIAL::MOUSE_MOVE_VEL;
            vMove.y += dy*NSVISUAL_MATERIAL::MOUSE_MOVE_VEL;
	    }

		//	Note : 카메라 변경.
		CameraMove( vMove );
		CameraZoom( fZoom );

        // m_matView 정보를 업데이트 한다.
        SetViewTrans();
	}

    // Device에 카메라 셋팅 정보를 적용시킨다.
    SetTransform();

	return S_OK;
}

void DxViewPortVM::CameraMove( const D3DXVECTOR3& vMove )
{
    D3DXVECTOR3 vDir(0.f,0.f,0.f);
    vDir.x = -vMove.x;
    vDir.y = vMove.y;

    m_vFromPt  	+= vDir;
	m_vLookatPt	+= vDir;
}

void DxViewPortVM::CameraZoom( float fZoom )
{
    D3DXVECTOR3 vDir = m_vFromPt - m_vLookatPt;
    float fLength = D3DXVec3Length( &vDir );
    D3DXVec3Normalize( &vDir, &vDir );
    fLength += fZoom;

    if( fLength < 50.f )
        fLength = 50.f;
    if( fLength > 300.f )
        fLength = 300.f;

    m_vFromPt = m_vLookatPt + ( vDir*fLength );
}

BOOL DxViewPortVM::GetMouseTargetPosWnd ( D3DXVECTOR3& vTargetPos )
{
	__gassert_(m_hWnd&&"윈도우핸들이 초기화 되지 않았습니다.");

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
		ComputeBoundBox( vBoundBox );

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

void DxViewPortVM::ComputeBoundBox( D3DXVECTOR3 *pt )
{
	FLOAT dist, t;
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir.x = m_vLookatPt.x - m_vFromPt.x;
	vDir.y = m_vLookatPt.y - m_vFromPt.y;
	vDir.z = m_vLookatPt.z - m_vFromPt.z;

	vUp.x = m_vUpVec.x;
	vUp.y = m_vUpVec.y;
	vUp.z = m_vUpVec.z;

	vPos.x = m_vFromPt.x;
	vPos.y = m_vFromPt.y;
	vPos.z = m_vFromPt.z;

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
		dist = (i & 0x4) ? m_fFarPlane : m_fNearPlane;
		pt[i].x = dist * vDir.x;
		pt[i].y = dist * vDir.y;
		pt[i].z = dist * vDir.z;

		//	(2)
		t = dist * tanf(m_fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i].x += vUp.x * t;
		pt[i].y += vUp.y * t;
		pt[i].z += vUp.z * t;
		
		t = dist * tanf(m_fFOV/2) * m_fWidth/m_fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i].x += vCross.x * t;
		pt[i].y += vCross.y * t;
		pt[i].z += vCross.z * t;
		pt[i].x = vPos.x + pt[i].x;
		pt[i].y = vPos.y + pt[i].y;
		pt[i].z = vPos.z + pt[i].z;
	}
}
