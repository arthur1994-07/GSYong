#include "stdafx.h"

#include "../../InternalCommonLib/Interface/DxInputDeviceInterface.h"

#include "../Define/DxVMDefine.h"

#include "DxViewPortPreview.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


DxViewPortPreview::DxViewPortPreview()
    : m_pd3dDevice(NULL)
    , m_fElapsedTime(0.f)
    , m_fFOV(0.f)
    , m_fWidth(0.f)
    , m_fHeight(0.f)
    , m_fNearPlane(0.f)
    , m_fFarPlane(0.f)
{
}

DxViewPortPreview::~DxViewPortPreview()
{
}

void DxViewPortPreview::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;
}

void DxViewPortPreview::SetHwnd( HWND hWnd )
{
    m_hWnd = hWnd;
}

void DxViewPortPreview::SetViewTrans( const D3DXVECTOR3 &vFromPt,const  D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vUpVec )
{
    m_vFromPt = vFromPt;
	m_vLookatPt = vLookatPt;
	m_vUpVec = vUpVec;

	SetViewTrans();
}

void DxViewPortPreview::SetViewTrans()
{
    D3DXMatrixLookAtLH( &m_matView, &m_vFromPt, &m_vLookatPt, &m_vUpVec );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
}

void DxViewPortPreview::SetProjection( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane )
{
    if ( !m_pd3dDevice )
        return;

    m_fFOV = fFOV;
    m_fWidth = fWidth;
    m_fHeight = fHeight;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;

	D3DXMatrixPerspectiveFovLH( &m_matProj, fFOV, m_fWidth/m_fHeight, fNearPlane, fFarPlane );
    m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

void DxViewPortPreview::SetTransform()
{
    m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
}

HRESULT DxViewPortPreview::SetViewPort()
{
	HRESULT hr;

    if ( !m_pd3dDevice )
        return S_OK;

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

HRESULT DxViewPortPreview::FrameMove( float fElapsedTime )
{
	if ( !m_pd3dDevice )
		return S_FALSE;

	m_fElapsedTime = fElapsedTime;

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
    BOOL bTARPOSWIN = GetMouseTargetPosWnd( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

    // 카메라 작동
    if ( bTARPOSWIN )
	{
        // L 키를 누른 상태에서는 카메라 이동이 작동되지 않는다.
        if( ic::DxInputDeviceInterface::GetInstance()->GetKeyState(DIK_L) & ic::DXKEY_DOWNED )
	    {
		    return S_OK;
	    }

        // Note : 초기화
        int         dx(0), dy(0), dz(0);
        float       fZoom(0.f);
        D3DXVECTOR3 vRotate(0.f,0.f,0.f);
        ic::DxInputDeviceInterface::GetInstance()->GetMouseMove ( dx, dy, dz );
        DWORD dwLButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT );
        DWORD dwRButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_RIGHT );
        DWORD dwMButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_MIDDLE );

        // Zoom In , Zoom Out
        fZoom += dz * 0.1f;

        // Zoom In , Zoom Out
	    if ( (dwLButton&ic::DXKEY_DRAG) && (dwRButton&ic::DXKEY_DRAG) )
	    {
            fZoom -= dy * 0.5f;
	    }
        // Rotate
        else if ( (dwLButton&ic::DXKEY_DRAG) || (dwRButton&ic::DXKEY_DRAG) )    
	    {
            vRotate.x += 0.01f*(dy);
			vRotate.y += 0.01f*(dx);
	    }

		//	Note : 카메라 변경.
		CameraRotation( vRotate );
		CameraZoom( fZoom );

        // m_matView 정보를 업데이트 한다.
        SetViewTrans();
	}

	return S_OK;
}

void DxViewPortPreview::CameraRotation( const D3DXVECTOR3& vRotate )
{
   D3DXVECTOR3 vCameraDir = m_vFromPt - m_vLookatPt;

	//	Note : 현재의 회전값.
	//
	float Length, LengthXZ;
	float thetaX, thetaY;

	Length = D3DXVec3Length ( &vCameraDir );
	if ( Length == 0 )
		Length = 0.001f;

	thetaX = (float) asinf ( vCameraDir.y / Length );

	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//
	LengthXZ = (float) sqrtf ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	if ( LengthXZ == 0 )
		LengthXZ = 0.001f;

	thetaY = (float) acosf ( vCameraDir.x / LengthXZ );
	if ( vCameraDir.z>0 )
		thetaY = -thetaY;

	//	Note : 회전 적용.
	//
	thetaX += vRotate.x;
	thetaY += vRotate.y;

	////	Note : 회전 제한 값.
	////
	//if ( m_dwFlag & DXVP_LIMITDIR )
	//{
	//	if ( m_fHIGHXDIR < thetaX )
	//		thetaX = m_fHIGHXDIR;
	//	else if ( m_fLOWXDIR > thetaX )
	//		thetaX = m_fLOWXDIR;
	//}
	////	Note : 연산 특성상 "LengthXZ==0" 이 되지 않도록 제한할 필요가 있다.
	////
	//else
	//{
		const static float fLIMIT = D3DX_PI/2.0f - 0.001f;
		if ( fLIMIT < thetaX )
			thetaX = fLIMIT;
		else if ( -fLIMIT > thetaX )
			thetaX = -fLIMIT;
	//}

	//	Note : 카메라 방향 단위 백터 계산.
	//
	//		Dir = { 1, 0, 0 } 의 초기값에서 회전 시킴.
	//
	D3DXVECTOR3 Dir;

	//	Note : 피치회전.
	//
	Dir.y = sinf(thetaX);

	Dir.x = 1-Dir.y*Dir.y;
	Dir.x = ( Dir.x < 0 ) ? 0 : sqrtf(Dir.x);

	Dir.z = (float) (-Dir.x*sinf(thetaY) );
	Dir.x = (float) ( Dir.x*cosf(thetaY) );

	m_vFromPt = m_vLookatPt + Dir*Length;
}

void DxViewPortPreview::CameraZoom( float fZoom )
{
    D3DXVECTOR3 vDir = m_vFromPt - m_vLookatPt;
    float fLength = D3DXVec3Length( &vDir );
    D3DXVec3Normalize( &vDir, &vDir );
    fLength += fZoom;

    if( fLength < 25.f )
        fLength = 25.f;
    if( fLength > 300.f )
        fLength = 300.f;

    m_vFromPt = m_vLookatPt + ( vDir*fLength );
}

BOOL DxViewPortPreview::GetMouseTargetPosWnd ( D3DXVECTOR3& vTargetPos )
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

void DxViewPortPreview::ComputeBoundBox( D3DXVECTOR3 *pt )
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
