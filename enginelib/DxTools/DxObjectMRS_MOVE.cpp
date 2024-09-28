#include "pch.h"

#include "../DxTools/DxInputDevice.h"
#include "../DxTools/DxViewPort.h"
#include "DxViewPort.h"

#include "DxObjectMRS.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define MOVE_VALUE	0.00002f
#define MOVE_VALUE2	0.0005f

//------------------------------------------------------------------------------------------------------------------
// Note :							화		살		표		생		성
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CreateArrow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matTemp;
	matTemp = matWorld;
	D3DXVECTOR3 vPos( matTemp._41, matTemp._42, matTemp._43 );
	matTemp._41 = 0.f;
	matTemp._42 = 0.f;
	matTemp._43 = 0.f;

	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*0.13f;

	D3DXVECTOR3 vCenter( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vTargetX = D3DXVECTOR3( fDistance, 0.f, 0.f );
	D3DXVECTOR3 vTargetY = D3DXVECTOR3( 0.f, fDistance, 0.f );
	D3DXVECTOR3 vTargetZ = D3DXVECTOR3( 0.f, 0.f, fDistance );

	D3DXVec3TransformCoord( &m_vLineX[0], &vCenter, &matWorld );
	D3DXVec3TransformCoord( &m_vLineY[0], &vCenter, &matWorld );
	D3DXVec3TransformCoord( &m_vLineZ[0], &vCenter, &matWorld );

	D3DXVec3TransformCoord( &vTargetX, &vTargetX, &matTemp );
	D3DXVec3TransformCoord( &vTargetY, &vTargetY, &matTemp );
	D3DXVec3TransformCoord( &vTargetZ, &vTargetZ, &matTemp );

	D3DXVec3Normalize( &vTargetX, &vTargetX );
	D3DXVec3Normalize( &vTargetY, &vTargetY );
	D3DXVec3Normalize( &vTargetZ, &vTargetZ );
	m_vLineX[1] = vTargetX*fDistance;
	m_vLineY[1] = vTargetY*fDistance;
	m_vLineZ[1] = vTargetZ*fDistance;

	m_vLineX[1] += vPos;
	m_vLineY[1] += vPos;
	m_vLineZ[1] += vPos;
}

//------------------------------------------------------------------------------------------------------------------
// Note :							사		각		형		생		성
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CreateSqrt( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matTemp;
	matTemp = matWorld;
	D3DXVECTOR3 vPos( matTemp._41, matTemp._42, matTemp._43 );
	matTemp._41 = 0.f;
	matTemp._42 = 0.f;
	matTemp._43 = 0.f;

	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*0.03f;
	float fDistance2 = sqrtf( (fDistance*fDistance) + (fDistance*fDistance) );

	D3DXVECTOR3 vCenter( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vTargetX = D3DXVECTOR3( fDistance, 0.f, 0.f );
	D3DXVECTOR3 vTargetY = D3DXVECTOR3( 0.f, fDistance, 0.f );
	D3DXVECTOR3 vTargetZ = D3DXVECTOR3( 0.f, 0.f, fDistance );

	D3DXVECTOR3 vTargetXY = D3DXVECTOR3( fDistance, fDistance, 0.f );
	D3DXVECTOR3 vTargetXZ = D3DXVECTOR3( fDistance, 0.f, fDistance );
	D3DXVECTOR3 vTargetYZ = D3DXVECTOR3( 0.f, fDistance, fDistance );
	
	D3DXVec3TransformCoord( &vCenter,	&vCenter,	&matWorld );
	D3DXVec3TransformCoord( &vTargetX,	&vTargetX,	&matTemp );
	D3DXVec3TransformCoord( &vTargetY,	&vTargetY,	&matTemp );
	D3DXVec3TransformCoord( &vTargetZ,	&vTargetZ,	&matTemp );
	D3DXVec3TransformCoord( &vTargetXY, &vTargetXY, &matTemp );
	D3DXVec3TransformCoord( &vTargetXZ, &vTargetXZ, &matTemp );
	D3DXVec3TransformCoord( &vTargetYZ, &vTargetYZ, &matTemp );

	D3DXVec3Normalize( &vTargetX, &vTargetX );
	D3DXVec3Normalize( &vTargetY, &vTargetY );
	D3DXVec3Normalize( &vTargetZ, &vTargetZ );
	D3DXVec3Normalize( &vTargetXY, &vTargetXY );
	D3DXVec3Normalize( &vTargetXZ, &vTargetXZ );
	D3DXVec3Normalize( &vTargetYZ, &vTargetYZ );
	vTargetX *= fDistance;
	vTargetY *= fDistance;
	vTargetZ *= fDistance;
	vTargetXY *= fDistance2;
	vTargetXZ *= fDistance2;
	vTargetYZ *= fDistance2;

	vTargetX += vPos;
	vTargetY += vPos;
	vTargetZ += vPos;
	vTargetXY += vPos;
	vTargetXZ += vPos;
	vTargetYZ += vPos;

	m_vSqrtXY[0] = vTargetX;	m_vSqrtXY[1] = vCenter;		m_vSqrtXY[2] = vTargetXY;	m_vSqrtXY[3] = vTargetY;
	m_vSqrtXZ[0] = vTargetX;	m_vSqrtXZ[1] = vCenter;		m_vSqrtXZ[2] = vTargetXZ;	m_vSqrtXZ[3] = vTargetZ;
	m_vSqrtYZ[0] = vTargetY;	m_vSqrtYZ[1] = vCenter;		m_vSqrtYZ[2] = vTargetYZ;	m_vSqrtYZ[3] = vTargetZ;
}

//------------------------------------------------------------------------------------------------------------------
// Note :							C	h	e	c	k		R	o	o	t
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CheckMoveMode( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_nMoveMode_Check = EM_MOVE_0;		// Default

	if( DxInputDevice::GetInstance().IsMouseInGameScreen( TRUE ) )
	{
		CheckMoveMode_Arrow( pd3dDevice );
		CheckMoveMode_Sqrt( pd3dDevice );
	}

	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_DOWN )
	{
		m_nMoveMode_Click = m_nMoveMode_Check;

		D3DXPLANE sPlane;
		if( m_nMoveMode_Click==EM_MOVE_X )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vLineX[0], &m_vLineX[1], &m_vLineY[1] );
		}
		else if( m_nMoveMode_Click==EM_MOVE_Y )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vLineY[0], &m_vLineY[1], &m_vLineX[1] );
		}
		else if( m_nMoveMode_Click==EM_MOVE_Z )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vLineZ[0], &m_vLineZ[1], &m_vLineY[1] );
		}
		else if( m_nMoveMode_Click==EM_MOVE_XY )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vSqrtXY[0], &m_vSqrtXY[1], &m_vSqrtXY[3] );
		}
		else if( m_nMoveMode_Click==EM_MOVE_XZ )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vSqrtXZ[0], &m_vSqrtXZ[1], &m_vSqrtXZ[3] );
		}
		else if( m_nMoveMode_Click==EM_MOVE_YZ )
		{
			D3DXPlaneFromPoints( &sPlane, &m_vSqrtYZ[0], &m_vSqrtYZ[1], &m_vSqrtYZ[3] );
		}

		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		{
			D3DXVECTOR3 vTargetPt, vFromPt;
			vFromPt = DxViewPort::GetInstance().GetFromPt ();
			BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
			if ( bTargetted )
			{
				D3DXVECTOR3 vColl;
				D3DXPlaneIntersectLine( &vColl, &sPlane, &vFromPt, &vTargetPt );
				D3DXVECTOR3 vCenter(m_matBackUp._41,m_matBackUp._42,m_matBackUp._43);
				m_vMoveOffset = vCenter - vColl;
			}
		}
	}
	if( dwLButton&DXKEY_UP )
	{
		m_nMoveMode_Click = m_nMoveMode_Check;
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :							화		살		표		체		크
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CheckMoveMode_Arrow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR2 vMousePt;
	vMousePt.x = (float)DxInputDevice::GetInstance().GetMouseLocateX();
	vMousePt.y = (float)DxInputDevice::GetInstance().GetMouseLocateY();

	if( IsCollisionLine( vMousePt, m_vLineX[0], m_vLineX[1], 5.f ) )		m_nMoveMode_Check = EM_MOVE_X;
	else if( IsCollisionLine( vMousePt, m_vLineY[0], m_vLineY[1], 5.f ) )	m_nMoveMode_Check = EM_MOVE_Y;
	else if( IsCollisionLine( vMousePt, m_vLineZ[0], m_vLineZ[1], 5.f ) )	m_nMoveMode_Check = EM_MOVE_Z;
}

//------------------------------------------------------------------------------------------------------------------
// Note :							사		각		형		체		크
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::CheckMoveMode_Sqrt( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR2 vMousePt;
	vMousePt.x = (float)DxInputDevice::GetInstance().GetMouseLocateX();
	vMousePt.y = (float)DxInputDevice::GetInstance().GetMouseLocateY();

	// Note :
	if( IsCollisionTriangle( vMousePt, m_vSqrtXY[0], m_vSqrtXY[1], m_vSqrtXY[2] ) )			m_nMoveMode_Check = EM_MOVE_XY;
	else if( IsCollisionTriangle( vMousePt, m_vSqrtXY[1], m_vSqrtXY[2], m_vSqrtXY[3] ) )	m_nMoveMode_Check = EM_MOVE_XY;

	// Note :
	if( IsCollisionTriangle( vMousePt, m_vSqrtYZ[0], m_vSqrtYZ[1], m_vSqrtYZ[2] ) )			m_nMoveMode_Check = EM_MOVE_YZ;
	else if( IsCollisionTriangle( vMousePt, m_vSqrtYZ[1], m_vSqrtYZ[2], m_vSqrtYZ[3] ) )	m_nMoveMode_Check = EM_MOVE_YZ;

	// Note :
	if( IsCollisionTriangle( vMousePt, m_vSqrtXZ[0], m_vSqrtXZ[1], m_vSqrtXZ[2] ) )			m_nMoveMode_Check = EM_MOVE_XZ;
	else if( IsCollisionTriangle( vMousePt, m_vSqrtXZ[1], m_vSqrtXZ[2], m_vSqrtXZ[3] ) )	m_nMoveMode_Check = EM_MOVE_XZ;
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Move_World( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
	matIdentity._41 = m_pmatWorld->_41;
	matIdentity._42 = m_pmatWorld->_42;
	matIdentity._43 = m_pmatWorld->_43;

	// Note : 아이템들의 생성
	CreateArrow( pd3dDevice, matIdentity );
	CreateSqrt( pd3dDevice, matIdentity );

	// Note : 체크
	CheckMoveMode( pd3dDevice );

	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Move_Object( pd3dDevice, m_nMoveMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nMoveMode_Click != EM_MOVE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Move_Object();
		Render_Move_Object( pd3dDevice, m_nMoveMode_Click );
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Move_Local( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// Note : 아이템들의 생성
	CreateArrow( pd3dDevice, *m_pmatWorld );
	CreateSqrt( pd3dDevice, *m_pmatWorld );

	// Note : 체크
	CheckMoveMode( pd3dDevice );

	DWORD dwLShift = DxInputDevice::GetInstance().GetKeyState(DIK_LSHIFT);
	DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
	if( dwLButton&DXKEY_IDLE || dwLButton&DXKEY_DOWN || dwLButton&DXKEY_UP )
	{
		Render_Move_Object( pd3dDevice, m_nMoveMode_Check );

		if( (dwLShift&DXKEY_DOWNED) && (dwLButton&DXKEY_DOWN) )
		{
			if( m_nMoveMode_Click != EM_MOVE_0 )	m_bClone = TRUE;
		}
	}
	else
	{
		Move_Object();
		Render_Move_Object( pd3dDevice, m_nMoveMode_Click );
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Move_Object()
{
	// Note : 화면을 넘어가면 안 움직이게 함
	D3DXVECTOR3 vTargetPt;
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
	if ( !bTargetted )	return;

	BYTE btUpdate = 0;

	D3DXPLANE sPlane;
	if( m_nMoveMode_Click==EM_MOVE_X )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vLineX[0], &m_vLineX[1], &m_vLineY[1] );
		btUpdate += MoveObject( m_vLineX[1]-m_vLineX[0], sPlane );
	}
	else if( m_nMoveMode_Click==EM_MOVE_Y )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vLineY[0], &m_vLineY[1], &m_vLineX[1] );
		btUpdate += MoveObject( m_vLineY[1]-m_vLineY[0], sPlane );
	}
	else if( m_nMoveMode_Click==EM_MOVE_Z )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vLineZ[0], &m_vLineZ[1], &m_vLineY[1] );
		btUpdate += MoveObject( m_vLineZ[1]-m_vLineZ[0], sPlane );
	}
	else if( m_nMoveMode_Click==EM_MOVE_XY )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vSqrtXY[0], &m_vSqrtXY[1], &m_vSqrtXY[3] );
		btUpdate += MoveObject2( m_vSqrtXY[0]-m_vSqrtXY[1], m_vSqrtXY[3]-m_vSqrtXY[1], sPlane );
	}
	else if( m_nMoveMode_Click==EM_MOVE_XZ )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vSqrtXZ[0], &m_vSqrtXZ[1], &m_vSqrtXZ[3] );
		btUpdate += MoveObject2( m_vSqrtXZ[0]-m_vSqrtXZ[1], m_vSqrtXZ[3]-m_vSqrtXZ[1], sPlane );
	}
	else if( m_nMoveMode_Click==EM_MOVE_YZ )
	{
		D3DXPlaneFromPoints( &sPlane, &m_vSqrtYZ[0], &m_vSqrtYZ[1], &m_vSqrtYZ[3] );
		btUpdate += MoveObject2( m_vSqrtYZ[0]-m_vSqrtYZ[1], m_vSqrtYZ[3]-m_vSqrtYZ[1], sPlane );
	}

	//Note : 이동 거리의 표시
	CDebugSet::ToView( 9, "Object Trans : [%f] [%f] [%f]", m_pmatWorld->_41 - m_matBackUp._41, 
															m_pmatWorld->_42 - m_matBackUp._42, 
															m_pmatWorld->_43 - m_matBackUp._43 );
}

//------------------------------------------------------------------------------------------------------------------
// Note :
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Move_Object( LPDIRECT3DDEVICEQ pd3dDevice, const int& nMode )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vCenter_PROJ, vTargetX_PROJ, vTargetY_PROJ, vTargetZ_PROJ;
	D3DXVec3Project( &vCenter_PROJ, &m_vLineX[0], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetX_PROJ, &m_vLineX[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetY_PROJ, &m_vLineY[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetZ_PROJ, &m_vLineZ[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	// Note : X
	if( nMode==EM_MOVE_X )
	{
		Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], OBECTMRS_RED );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffff0000 );
		Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );
	}

	// Note : Y
	if( nMode==EM_MOVE_Y )
	{
		Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], OBECTMRS_GREEN );
		pd3dDevice->SetFVF( D3DFVF_XYZRHW );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff00ff00 );
		Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );
	}

	// Note : Z
	if( nMode==EM_MOVE_Z )
	{
		Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_YELLOW );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffff00 );
		Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], OBECTMRS_BLUE );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff0000ff );
		Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );
	}

	//Note : EM_MOVE_XY
	if( nMode==EM_MOVE_XY )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff888800 );
		Render_Sqrt( pd3dDevice, m_vSqrtXY );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vSqrtXY[0], m_vSqrtXY[2], OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vSqrtXY[3], m_vSqrtXY[2], OBECTMRS_GREEN );
	}

	//Note : EM_MOVE_XZ
	if( nMode==EM_MOVE_XZ )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff888800 );
		Render_Sqrt( pd3dDevice, m_vSqrtXZ );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vSqrtXZ[0], m_vSqrtXZ[2], OBECTMRS_RED );
		Render_Arrow( pd3dDevice, m_vSqrtXZ[3], m_vSqrtXZ[2], OBECTMRS_BLUE );
	}

	//Note : EM_MOVE_YZ
	if( nMode==EM_MOVE_YZ )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff888800 );
		Render_Sqrt( pd3dDevice, m_vSqrtYZ );
	}
	else
	{
		Render_Arrow( pd3dDevice, m_vSqrtYZ[0], m_vSqrtYZ[2], OBECTMRS_GREEN );
		Render_Arrow( pd3dDevice, m_vSqrtYZ[3], m_vSqrtYZ[2], OBECTMRS_BLUE );
	}
}

//------------------------------------------------------------------------------------------------------------------
// Note :							화		살		표		출		력
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Arrow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vTarget, int nType )
{
	DWORD dwColor1;
	DWORD dwColor2;
	switch( nType )
	{
	case OBECTMRS_YELLOW:
		dwColor1 = 0xff666600;
		dwColor2 = 0xffffff00;
		break;
	case OBECTMRS_RED:
		dwColor1 = 0xff440000;
		dwColor2 = 0xffff0000;
		break;
	case OBECTMRS_GREEN:
		dwColor1 = 0xff004400;
		dwColor2 = 0xff00ff00;
		break;
	case OBECTMRS_BLUE:
		dwColor1 = 0xff000044;
		dwColor2 = 0xff0000ff;
		break;
	case OBECTMRS_GREY1:
		dwColor1 = 0xff444444;
		dwColor2 = 0xff000000;
		break;
	case OBECTMRS_GREY2:
		dwColor1 = 0xffaaaaaa;
		dwColor2 = 0xff555555;
		break;
	};

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	D3DXVECTOR3 vPos[2];
	vPos[0] = vCenter;
	vPos[1] = vTarget;
	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	m_pSavedSB->Apply();

	m_pADDSavedSB->Capture();
	m_pADDDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor2 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vPos, sizeof(D3DXVECTOR3) );

	m_pADDSavedSB->Apply();
}

//------------------------------------------------------------------------------------------------------------------
// Note :							사		각		형		출		력
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Sqrt( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 Sqrt[] )
{
	m_pAlphaSavedSB->Capture();
	m_pAlphaDrawSB->Apply();

	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &Sqrt[0], sizeof(D3DXVECTOR3) );

	m_pAlphaSavedSB->Apply();
}

//-------------------------------------------------------------------------------------
// Note :				물		체		의		이		동
//-------------------------------------------------------------------------------------
BYTE DxObjectMRS::MoveObject( const D3DXVECTOR3& vDir, const D3DXPLANE& sPlane )
{
	// 이동된 위치를 찾는다.
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
		if ( bTargetted )
		{
			D3DXVECTOR3 vColl;
			D3DXPlaneIntersectLine( &vColl, &sPlane, &vFromPt, &vTargetPt );
			vPos = vColl + m_vMoveOffset;
		}
		else
		{
			return 0;
		}
	}

	D3DXVECTOR3 vMoveDir(0.f,0.f,0.f);
	D3DXVECTOR3 vNormalizeDir;
	D3DXVECTOR3 vDirTemp(vPos.x-m_matBackUp._41,vPos.y-m_matBackUp._42,vPos.z-m_matBackUp._43);

	// 1번째 방향.
	D3DXVec3Normalize( &vNormalizeDir, &vDir );
	float fDot = D3DXVec3Dot( &vDirTemp, &vNormalizeDir );
	vMoveDir += vNormalizeDir * fDot;

	vPos = vMoveDir;
	vPos.x += m_matBackUp._41;
	vPos.y += m_matBackUp._42;
	vPos.z += m_matBackUp._43;

	if ( vPos.x == m_pmatWorld->_41 && vPos.y == m_pmatWorld->_42 && vPos.z == m_pmatWorld->_43 )
	{
		return 0;
	}

	m_pmatWorld->_41 = vPos.x;
	m_pmatWorld->_42 = vPos.y;
	m_pmatWorld->_43 = vPos.z;

	if ( m_bSnapMode )
	{
		m_vSnapMove.x = m_pmatWorld->_41 - m_matBackUp._41;
		m_vSnapMove.y = m_pmatWorld->_42 - m_matBackUp._42;
		m_vSnapMove.z = m_pmatWorld->_43 - m_matBackUp._43;

		MoveObject_Snap( vDir );
	}

	return 1;
}

BYTE DxObjectMRS::MoveObject2( const D3DXVECTOR3& vDir, const D3DXVECTOR3& vDir2, const D3DXPLANE& sPlane )
{
	// 이동된 위치를 찾는다.
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
		if ( bTargetted )
		{
			D3DXVECTOR3 vColl;
			D3DXPlaneIntersectLine( &vColl, &sPlane, &vFromPt, &vTargetPt );
			vPos = vColl + m_vMoveOffset;
		}
		else
		{
			return 0;
		}
	}

	D3DXVECTOR3 vMoveDir(0.f,0.f,0.f);
	D3DXVECTOR3 vNormalizeDir;
	D3DXVECTOR3 vDirTemp(vPos.x-m_matBackUp._41,vPos.y-m_matBackUp._42,vPos.z-m_matBackUp._43);

	// 1번째 방향.
	D3DXVec3Normalize( &vNormalizeDir, &vDir );
	float fDot = D3DXVec3Dot( &vDirTemp, &vNormalizeDir );
	vMoveDir += vNormalizeDir * fDot;

	// 2번째 방향.
	D3DXVec3Normalize( &vNormalizeDir, &vDir2 );
	fDot = D3DXVec3Dot( &vDirTemp, &vNormalizeDir );
	vMoveDir += vNormalizeDir * fDot;


	vPos = vMoveDir;
	vPos.x += m_matBackUp._41;
	vPos.y += m_matBackUp._42;
	vPos.z += m_matBackUp._43;

	if ( vPos.x == m_pmatWorld->_41 && vPos.y == m_pmatWorld->_42 && vPos.z == m_pmatWorld->_43 )
	{
		return 0;
	}

	m_pmatWorld->_41 = vPos.x;
	m_pmatWorld->_42 = vPos.y;
	m_pmatWorld->_43 = vPos.z;

	if ( m_bSnapMode )
	{
		m_vSnapMove.x = m_pmatWorld->_41 - m_matBackUp._41;
		m_vSnapMove.y = m_pmatWorld->_42 - m_matBackUp._42;
		m_vSnapMove.z = m_pmatWorld->_43 - m_matBackUp._43;

		MoveObject_Snap( vDir );
	}

	return 1;
}

void DxObjectMRS::MoveObject_Snap( const D3DXVECTOR3& vDir )
{
	D3DXVECTOR3 vSnapCount = (m_vSnapMove / m_fSnapGridSize) + D3DXVECTOR3(0.5f,0.5f,0.5f);

	int nPosX = (int)floor(vSnapCount.x);
	int nPosY = (int)floor(vSnapCount.y);
	int nPosZ = (int)floor(vSnapCount.z);

	m_pmatWorld->_41 = m_matBackUp._41 + (nPosX*m_fSnapGridSize);
	m_pmatWorld->_42 = m_matBackUp._42 + (nPosY*m_fSnapGridSize);
	m_pmatWorld->_43 = m_matBackUp._43 + (nPosZ*m_fSnapGridSize);
}

//------------------------------------------------------------------------------------------------------------------
// Note :								임		시		표		시
//------------------------------------------------------------------------------------------------------------------
void DxObjectMRS::Render_Move_Object_Temp( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXVECTOR3 vCenter_PROJ, vTargetX_PROJ, vTargetY_PROJ, vTargetZ_PROJ;
	D3DXVec3Project( &vCenter_PROJ, &m_vLineX[0], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetX_PROJ, &m_vLineX[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetY_PROJ, &m_vLineY[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	D3DXVec3Project( &vTargetZ_PROJ, &m_vLineZ[1], &DxViewPort::GetInstance().GetViewPort(), 
						&DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(),
						&matIdentity );

	// Note : X
	Render_Arrow( pd3dDevice, m_vLineX[0], m_vLineX[1], dwColor );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffff0000 );
	Render_STR_X( pd3dDevice, vTargetX_PROJ.x-8.f, vTargetX_PROJ.y+8.f );

	// Note : Y
	Render_Arrow( pd3dDevice, m_vLineY[0], m_vLineY[1], dwColor );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff00ff00 );
	Render_STR_Y( pd3dDevice, vTargetY_PROJ.x-8.f, vTargetY_PROJ.y+8.f );

	// Note : Z
	Render_Arrow( pd3dDevice, m_vLineZ[0], m_vLineZ[1], dwColor );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xff0000ff );
	Render_STR_Z( pd3dDevice, vTargetZ_PROJ.x-8.f, vTargetZ_PROJ.y+8.f );
}

