#include "pch.h"

#include "DebugSet.h"
#include "../DxTools/DxInputDevice.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../DxMeshs/FrameMesh/DxFrameMesh.h"

#include "dxviewport.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxViewPort::FrameMoveMAX( float fElapsedTime, const D3DXVECTOR3& vCenter )
{
	if ( !m_pd3dDevice )	return;

	m_fElapsedTime = fElapsedTime;
	
	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
	BOOL bTARPOSWIN = GetMouseTargetPosWnd ( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

	BOOL bCURSOR_SHOW(TRUE);
	BOOL bHandling(FALSE);

	if ( m_bHandling )	bHandling = bTARPOSWIN;

	if ( bHandling && m_dwFlag&DXVP_USER )
	{
		DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		DWORD dwCTRL(DXKEY_IDLE);
		DWORD dwALT(DXKEY_IDLE);
		DWORD dwZ(DXKEY_IDLE);
		DWORD dwKEY_RIGHT(DXKEY_IDLE);
		DWORD dwKEY_LEFT(DXKEY_IDLE);
		DWORD dwKEY_UP(DXKEY_IDLE);
		DWORD dwKEY_DOWN(DXKEY_IDLE);
		DWORD dwKEY_PGUP(DXKEY_IDLE);
		DWORD dwKEY_PGDN(DXKEY_IDLE);

		int dx(0), dy(0), dz(0);
		DWORD dwMOUSEKEY_L(NULL), dwMOUSEKEY_R(NULL), dwMOUSEKEY_M(NULL);

		if ( m_bKeyboard )
		{
			dwCTRL = dxInputDev.GetKeyState(DIK_LCONTROL);
			dwALT = dxInputDev.GetKeyState(DIK_LALT);
			dwZ = dxInputDev.GetKeyState(DIK_Z);

            // Game�� Tool�� ��ư�� ���� ���� ����� �ٸ���.
            if ( m_dwFlag & DXVP_GAME )
		    {
                // Game���� ī�޶� ȸ���� ����Ű�� ��.
			    dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_RIGHT);
			    dwKEY_LEFT = dxInputDev.GetKeyState(DIK_LEFT);
			    dwKEY_UP = dxInputDev.GetKeyState(DIK_UP);
			    dwKEY_DOWN = dxInputDev.GetKeyState(DIK_DOWN);
            }
            else if ( m_dwFlag & DXVP_TOOL )
            {
                // Tool���� �յ��¿츦 WSAD�� ��
                dwKEY_RIGHT = dxInputDev.GetKeyState(DIK_D);
			    dwKEY_LEFT = dxInputDev.GetKeyState(DIK_A);
			    dwKEY_UP = dxInputDev.GetKeyState(DIK_W);
			    dwKEY_DOWN = dxInputDev.GetKeyState(DIK_S);
            }

			dwKEY_PGUP = dxInputDev.GetKeyState(DIK_PGUP);
			dwKEY_PGDN = dxInputDev.GetKeyState(DIK_PGDN);
		}

		dxInputDev.GetMouseMove ( dx, dy, dz );
		dwMOUSEKEY_L = dxInputDev.GetMouseState( DXMOUSE_LEFT );
		dwMOUSEKEY_R = dxInputDev.GetMouseState( DXMOUSE_RIGHT );
		dwMOUSEKEY_M = dxInputDev.GetMouseState( DXMOUSE_MIDDLE );

		float fZoom(0.0f);
		D3DXVECTOR3 vMove(0,0,0);
		D3DXVECTOR3 vRotate(0,0,0);

		if( dwKEY_RIGHT & DXKEY_PRESSED )		vMove.x += fElapsedTime*m_fVELOMOVE;
		if( dwKEY_LEFT & DXKEY_PRESSED )		vMove.x -= fElapsedTime*m_fVELOMOVE;
		
		if( dwKEY_PGUP & DXKEY_PRESSED )		vMove.y += fElapsedTime*m_fVELOMOVE;
		if( dwKEY_PGDN & DXKEY_PRESSED )		vMove.y -= fElapsedTime*m_fVELOMOVE;

		if( dwKEY_UP & DXKEY_PRESSED )			vMove.z += fElapsedTime*m_fVELOMOVE;
		if( dwKEY_DOWN & DXKEY_PRESSED )		vMove.z -= fElapsedTime*m_fVELOMOVE;

		vMove *= m_fDistance*0.02f;	// �ӵ��� ��ȭ

		// Note : ī�޶��� ȸ��
		switch( dwMOUSEKEY_M )
		{
		case DXKEY_DRAG:
			vRotate.x += m_fVELODIR*(dy);
			vRotate.y += m_fVELODIR*(dx);
			
			//vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();
			bCURSOR_SHOW = FALSE;
			break;
		}

		// Note : ī�޶��� �̵�
		switch( dwMOUSEKEY_R )
		{
		case DXKEY_DRAG:
			D3DXVECTOR3 vProjectionX, vProjectionY;
			D3DXVECTOR3 vDir = m_vLookatPt - m_vFromPt;
			D3DXVec3Cross( &vProjectionX, &vDir, &m_vUpVec );
			D3DXVec3Cross( &vProjectionY, &vProjectionX, &vDir );
			D3DXVec3Normalize( &vProjectionX, &vProjectionX );
			D3DXVec3Normalize( &vProjectionY, &vProjectionY );

			vDir = (vProjectionX*(float)dx) + (vProjectionY*(float)dy);
			m_vLookatPt += vDir*m_fDistance*0.005f;
			m_vFromPt += vDir*m_fDistance*0.005f;
			break;
		};

		// Note : Zoom In, Zoom Out
		if( dz != 0 )
		{
			if( (dwCTRL&DXKEY_PRESSED) && (dwALT&DXKEY_PRESSED) )
			{
				fZoom += m_fVELOZOOM*(dz)/1000.0f;
				fZoom = m_damperVeloZoom.Calculate ( fZoom, fElapsedTime );
			}
			else
			{
				fZoom += m_fVELOZOOM*(dz)/1000.0f;
			}

			fZoom *= m_fDistance*0.02f;
		}

		// Note : ��ü ������ Z ������ �� ��ü�� �߽�������
		if( (dwZ&DXKEY_UP) && (dxInputDev.GetKeyState(DIK_LCONTROL)&DXKEY_IDLE) )
		{
			D3DXVECTOR3 vDist = vCenter-m_vLookatPt;
            m_vLookatPt += vDist;
			m_vFromPt += vDist;
		}

		//	Note : ī�޶� ����.
		CameraMove( vMove );
		CameraRotation( vRotate );
		CameraZoom( fZoom );
	}

	CameraLimitCheck();

	//	Note : �� ��Ʈ���� ����.
	//
	D3DXVECTOR3 vUP = ComputeUpVecter ( m_vLookatPt, m_vFromPt );
	D3DXMatrixLookAtLH ( &m_matView, &m_vFromPt, &m_vLookatPt, &vUP );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );

	//	Note : ����ߺ��� ��������
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

	//	Note : �������� Ŭ���� ����.
	//
	ComputeClipVolume ();
}

bool DxViewPort::CameraLimitCheck()
{
	if( m_sCameraControl.bCameraControl )
	{
		BOOL bChange1 = FALSE, bChange2 = FALSE, bChange3 = FALSE, bChange4 = FALSE, bChange5 = FALSE, bChange6 = FALSE;

		D3DXVECTOR3 vCamPos = m_vFromPt - m_vLookatPt;
		D3DXVECTOR3 vMaxPos = m_sCameraControl.GetMaxPos();
		D3DXVECTOR3 vMinPos = m_sCameraControl.GetMinPos();
		if( vCamPos.x > vMaxPos.x ) { vCamPos.x = vMaxPos.x; bChange1 = TRUE; }
		if( vCamPos.y > vMaxPos.y ) { vCamPos.y = vMaxPos.y; bChange2 = TRUE; }
		if( vCamPos.z > vMaxPos.z ) { vCamPos.z = vMaxPos.z; bChange3 = TRUE; }

		if( vCamPos.x < vMinPos.x ) { vCamPos.x = vMinPos.x; bChange4 = TRUE; }
		if( vCamPos.y < vMinPos.y ) { vCamPos.y = vMinPos.y; bChange5 = TRUE; }
		if( vCamPos.z < vMinPos.z ) { vCamPos.z = vMinPos.z; bChange6 = TRUE; }

		if( bChange1 || bChange2 || bChange3 || bChange4 || bChange5 || bChange6 )
		{
//			vCamPos = m_sCameraControl.vBaseCamPos;

			m_vFromPt = vCamPos + m_vLookatPt;
			SetViewTrans( m_vFromPt, m_vLookatPt, m_vUpVec );

			return FALSE;
		}
	}

	return TRUE;
}

void DxViewPort::FrameMoveSpecCam( float fElapsedTime )
{
	if ( !m_pd3dDevice )
		return;

	m_fElapsedTime = fElapsedTime;

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
	BOOL bTARPOSWIN = GetMouseTargetPosWnd ( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

	BOOL bCURSOR_SHOW(TRUE);
	BOOL bHandling(FALSE);

	if ( m_bHandling )	
		bHandling = bTARPOSWIN;

	if ( bHandling && m_dwFlag&DXVP_USER )
	{
		DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		DWORD dwEQALS(DXKEY_IDLE);
		DWORD dwMINUS(DXKEY_IDLE);
		DWORD dwSHIFT(DXKEY_IDLE);
		DWORD dwSPACE(DXKEY_IDLE);
		DWORD dwKEY_W(DXKEY_IDLE);	// ��
		DWORD dwKEY_S(DXKEY_IDLE);	// ��
		DWORD dwKEY_A(DXKEY_IDLE);	// ��
		DWORD dwKEY_D(DXKEY_IDLE);	// ��
		DWORD dwKEY_E(DXKEY_IDLE);	// ��
		DWORD dwKEY_Q(DXKEY_IDLE);	// ��

		int dx(0), dy(0), dz(0);
		DWORD dwMOUSEKEY_R(NULL);

		if ( m_bKeyboard )
		{
			dwEQALS = dxInputDev.GetKeyState(DIK_EQUALS);
			dwMINUS = dxInputDev.GetKeyState(DIK_MINUS);
			dwSPACE = dxInputDev.GetKeyState(DIK_SPACE);
			dwSHIFT = dxInputDev.GetKeyState(DIK_LSHIFT);
			dwSPACE = dxInputDev.GetKeyState(DIK_SPACE);
			dwKEY_W = dxInputDev.GetKeyState(DIK_W);
			dwKEY_S = dxInputDev.GetKeyState(DIK_S);
			dwKEY_A = dxInputDev.GetKeyState(DIK_A);
			dwKEY_D = dxInputDev.GetKeyState(DIK_D);
			dwKEY_E = dxInputDev.GetKeyState(DIK_E);
			dwKEY_Q = dxInputDev.GetKeyState(DIK_Q);
		}

		dxInputDev.GetMouseMove ( dx, dy, dz );
		dwMOUSEKEY_R = dxInputDev.GetMouseState( DXMOUSE_RIGHT );

		float fZoom(0.0f);
		D3DXVECTOR3 vMove(0,0,0);
		D3DXVECTOR3 vRotate(0,0,0);

		float	fAccelator(1.f);
		if( dwSHIFT & DXKEY_PRESSED )	fAccelator = 2.f;
		if( dwEQALS & DXKEY_UP )		m_fSpecCamMove *= 1.2f;
		if( dwMINUS & DXKEY_UP )		m_fSpecCamMove *= 0.9f;

		fAccelator *= m_fSpecCamMove;

		if( dwSPACE & DXKEY_PRESSED )
		{
			if( dwKEY_D & DXKEY_PRESSED )		vMove.x += fElapsedTime*fAccelator;
			if( dwKEY_A & DXKEY_PRESSED )		vMove.x -= fElapsedTime*fAccelator;

			if( dwKEY_E & DXKEY_PRESSED )		vMove.y += fElapsedTime*fAccelator;
			if( dwKEY_Q & DXKEY_PRESSED )		vMove.y -= fElapsedTime*fAccelator;

			if( dwKEY_W & DXKEY_PRESSED )		vMove.z += fElapsedTime*fAccelator;
			if( dwKEY_S & DXKEY_PRESSED )		vMove.z -= fElapsedTime*fAccelator;
		}

		// Note : ī�޶��� ȸ��
		switch( dwMOUSEKEY_R )
		{
		case DXKEY_DRAG:
			vRotate.x -= m_fVELODIR*(dy)*0.2f;
			vRotate.y += m_fVELODIR*(dx)*0.2f;

			//vRotate = m_damperVeloDir.Calculate ( vRotate, fElapsedTime );
			dxInputDev.HoldCursor ();
			bCURSOR_SHOW = FALSE;
			break;
		}

		////	Note : ī�޶� ����.
		CameraMove( vMove, FALSE );
		CameraRotation( vRotate, TRUE );
		//CameraZoom( fZoom );
	}

	CameraLimitCheck();

	//	Note : �� ��Ʈ���� ����.
	//
	D3DXVECTOR3 vUP = ComputeUpVecter ( m_vLookatPt, m_vFromPt );
	D3DXMatrixLookAtLH ( &m_matView, &m_vFromPt, &m_vLookatPt, &vUP );
	m_pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );

	//	Note : ����ߺ��� ��������
	//
	m_vLookDir = m_vLookatPt - m_vFromPt;
	if ( m_fDistance == 0.f )	m_fDistance = 0.001f;
	m_fDistance = D3DXVec3Length ( &m_vLookDir );

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

	//	Note : �������� Ŭ���� ����.
	//
	ComputeClipVolume ();

	if( DxInputDevice::GetInstance().IsMouseInGameScreen( TRUE ) )
	{
		CCursor::GetInstance().SetShowCursor ( FALSE );
	}
	else
	{
		CCursor::GetInstance().SetShowCursor ( TRUE );
	}
}

void DxViewPort::FrameMove_FlyCamera(void)
{
	D3DXVECTOR3 vLengt = m_vLookatPt - m_vFromPt;
	m_fDistance = m_fLength = D3DXVec3Length(&vLengt);
}