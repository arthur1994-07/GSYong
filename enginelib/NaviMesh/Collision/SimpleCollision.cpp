#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "SimpleCollision.h"



//////////////////////////////////////////////////////////////////////////
//								계산
//////////////////////////////////////////////////////////////////////////
// engine 에 있지만 include 하기 싫어서 복사해 놓음.
bool CalculatePlane2Line_RanLogic ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, D3DXVECTOR3 &vCol )
{
	D3DXVECTOR3 vNormal(plane.a,plane.b,plane.c);
	D3DXVECTOR3 vDist(vLine2 - vLine1);

	float fNormal4Line1 = D3DXVec3Dot ( &vNormal, &vLine1 );
	float fNormal4Dist = D3DXVec3Dot ( &vNormal, &vDist );

	float t = ( - plane.d - fNormal4Line1 ) / fNormal4Dist;

	//	t의 값은 ( 0 <= t <= 1 ) 으로 제한된다.
	//
	if ( 0 > t )	return false;
	if ( t > 1 )	return false;

	vCol = vLine1 + t * vDist;

	return true;
}

BOOL CalculatePlane2Line_LimitX( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, const D3DXVECTOR2 vMinMax, const D3DXMATRIX& matInverseRotationY, float fOffset )
{
	D3DXVECTOR3 vCol(0.f,0.f,0.f);
	if ( !CalculatePlane2Line_RanLogic( plane, vLine1, vLine2, vCol ) )
		return FALSE;

	D3DXVec3TransformCoord( &vCol, &vCol, &matInverseRotationY );

	if ( vCol.x < (vMinMax.x-fOffset-0.001f) )
		return FALSE;

	if ( vCol.x > (vMinMax.y+fOffset+0.001f) )
		return FALSE;

	return TRUE;
}

BOOL CalculatePlane2Line_LimitZ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, const D3DXVECTOR2 vMinMax, const D3DXMATRIX& matInverseRotationY, float fOffset )
{
	D3DXVECTOR3 vCol(0.f,0.f,0.f);
	if ( !CalculatePlane2Line_RanLogic( plane, vLine1, vLine2, vCol ) )
		return FALSE;

	D3DXVec3TransformCoord( &vCol, &vCol, &matInverseRotationY );

	if ( vCol.z < (vMinMax.x-fOffset-0.001f) )
		return FALSE;

	if ( vCol.z > (vMinMax.y+fOffset+0.001f) )
		return FALSE;

	return TRUE;
}

BOOL IsCollisionChangePosistion( const D3DXPLANE &sPlane, const D3DXVECTOR3& vStart, float fOffset )
{
	float fDot = D3DXPlaneDotCoord( &sPlane, &vStart );

	// 시작점이 평면과 가까이에 있다.
	if ( fDot >= -fOffset && fDot <= fOffset )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsCollisionCircle( const D3DXVECTOR2& vStart, const D3DXVECTOR2& vEnd, const D3DXVECTOR2& vCenter, float fRadius )
{
	D3DXVECTOR2 vLine1 = vStart - vEnd;
	D3DXVECTOR2 vLine2 = vStart - vCenter;

	float fDistance = D3DXVec2Length( &vLine1 );
	if ( fDistance <= 0.f )
		return FALSE;

	// 평면이 원과 닿는지 확인한다.
	// 안닿이면 충돌되지 않음.
	//D3DXPLANE sPlane;
	{
		// ax + by + c = 0
		D3DXVECTOR2 vNor( vLine1.y, -vLine1.x );
		D3DXVec2Normalize( &vNor, &vNor );
		float fC = -D3DXVec2Dot( &vNor, &vStart );

		// Note : 원점과의 거리를 구한다.
		float fTempDis = (vNor.x*vCenter.x) + (vNor.y*vCenter.y) + fC;
		fTempDis = fabsf( fTempDis );

		// Note : 체크를 한다.
		if( fTempDis > fRadius )
		{
			return FALSE;
		}
	}


	// 길이 구함.
	float fDistance2 = D3DXVec2Length( &vLine2 );

	D3DXVECTOR2 vLine3 = vEnd - vCenter;
	float fDistance3 = D3DXVec2Length( &vLine3 );


	// 큰원쪽
	// 두점다 단방향 외부로 나갔는지 확인한다. 나갔으면 충돌되지 않음.
	if ( fDistance2 > fRadius && fDistance3 > fRadius )
	{
		float fDot1 = D3DXVec2Dot( &vLine2, &vLine1 );
		float fDot2 = D3DXVec2Dot( &vLine3, &vLine1 );

		if ( fDot1 > 0.f && fDot2 > 0.f )
			return FALSE;

		if ( fDot1 < 0.f && fDot2 < 0.f )
			return FALSE;
	}

	return TRUE;
}

BOOL IsCollisionLineLineLoose( const D3DXVECTOR2& vStart1, const D3DXVECTOR2& vEnd1, const D3DXVECTOR2& vStart2, const D3DXVECTOR2& vEnd2, float fOffset )
{
	D3DXVECTOR2 vCenter1 = (vStart1 + vEnd1) * 0.5f;
	D3DXVECTOR2 vCenter2 = (vStart2 + vEnd2) * 0.5f;

	D3DXVECTOR2 vDir = vCenter2 - vCenter1;
	float fCenterLength = D3DXVec2Length( &vDir );

	vDir = (vStart1 - vEnd1) * 0.5f;
	float fNew = D3DXVec2Length( &vDir );

	vDir = (vStart2 - vEnd2) * 0.5f;
	fNew += D3DXVec2Length( &vDir );
	fNew += fOffset;

	if ( fNew >= fCenterLength )
		return TRUE;

	return FALSE;
}



//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionBase
//////////////////////////////////////////////////////////////////////////
SimpleCollisionBase::SimpleCollisionBase( SIMPLE_COLLISION_TYPE emType, const char* pName )
	: m_emType(emType)
{
	StringCchCopy( m_szName, NAME_MAXSZ, pName );
}

SimpleCollisionBase::~SimpleCollisionBase()
{

}

void SimpleCollisionBase::SetObjRotate( float fAddAngle )
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, fAddAngle );
	D3DXVec3TransformCoord( &m_vPosition, &m_vPosition, &matRotate );
}

void SimpleCollisionBase::Save( sc::SerialFile &SFile )
{
	SFile << BASE_VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( m_szName, NAME_MAXSZ );
		SFile << m_vPosition;
	}
	SFile.EndBlock();
}

void SimpleCollisionBase::Load( sc::BaseStream &SFile )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile.ReadBuffer( m_szName, NAME_MAXSZ );
	SFile >> m_vPosition;
}



//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionPlane
//////////////////////////////////////////////////////////////////////////
SimpleCollisionPlane::SimpleCollisionPlane()
	: SimpleCollisionBase( EMSCT_PLANE, _T("PLANE") )
	, m_vMinMaxX(-50.f,50.f)
	, m_fRotationY(0.f)
{
	InitData();
}

SimpleCollisionPlane::~SimpleCollisionPlane()
{

}

void SimpleCollisionPlane::InitData()
{
	D3DXVECTOR3 vPos[4];
	vPos[0] = D3DXVECTOR3 ( m_vMinMaxX.x, 10000.f, 0.f );
	vPos[1] = D3DXVECTOR3 ( m_vMinMaxX.y, 10000.f, 0.f );
	vPos[2] = D3DXVECTOR3 ( m_vMinMaxX.x, -10000.f, 0.f );
	vPos[3] = D3DXVECTOR3 ( m_vMinMaxX.y, -10000.f, 0.f );

	D3DXMATRIX matRotateY;
	D3DXMatrixRotationY( &matRotateY, D3DX_PI*m_fRotationY/180 );
	matRotateY._41 = m_vPosition.x;
	matRotateY._42 = m_vPosition.y;
	matRotateY._43 = m_vPosition.z;
	//D3DXMatrixInverse( &m_matInverseRotationY, NULL, &matRotateY );

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
	}

	//D3DXPlaneFromPoints( &m_sPlane, &vPos[0], &vPos[1], &vPos[2] );

	m_sLine.SetEndPointA( D3DXVECTOR2( vPos[0].x, vPos[0].z ) );
	m_sLine.SetEndPointB( D3DXVECTOR2( vPos[1].x, vPos[1].z ) );
}

void SimpleCollisionPlane::SetObjRotate( float fAddAngle )
{
	m_fRotationY += fAddAngle;
	SimpleCollisionBase::SetObjRotate( fAddAngle );
}

void SimpleCollisionPlane::SetPosisionRotation( const D3DXVECTOR3& vPosition, float fRotationY )
{
	m_vPosition = vPosition;
	m_fRotationY = fRotationY;
	InitData();
}

SIMPLE_COLLISION_RESULT SimpleCollisionPlane::IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset )
{
	D3DXVECTOR2 vNewStart( vStart.x, vStart.z );
	D3DXVECTOR2 vNewEnd( vEnd.x, vEnd.z );

	D3DXVECTOR2 vLine1 = vNewEnd - vNewStart;
	float fDistance = D3DXVec2Length( &vLine1 );
	if ( fDistance <= 0.f )
		return COLL_FALSE;

	// 큰원으로 검사를 하도록 한다.
	if ( !IsCollisionLineLineLoose( vNewStart, vNewEnd, m_sLine.EndPointA(), m_sLine.EndPointB(), fOffset ) )
	{
		return COLL_FALSE;
	}

	// 시작점이 선의 fOffset 내부에 있다.
	if ( IsCollisionCircle( m_sLine.EndPointA(), m_sLine.EndPointB(), vNewStart, fOffset ) )
	{
		return COLL_TRUE_START_IN;
	}

	Line2D sLine2D;
	sLine2D.SetEndPointA( vNewStart );
	sLine2D.SetEndPointB( vNewEnd );

	Line2D::LINE_CLASSIFICATION emResult = m_sLine.Intersection( sLine2D, NULL );
	if ( emResult == Line2D::SEGMENTS_INTERSECT )
	{
		return COLL_TRUE;
	}
	else if ( emResult == Line2D::B_BISECTS_A )	// sLine2D 가 쭉 이어질 경우 m_sLine 에 닿는다.
	{
		if ( m_sLine.ClassifyPoint( vNewEnd, fOffset ) == Line2D::ON_LINE )	// 도착점이 선의 fOffset 내부에 있다.
		{
			return COLL_TRUE;
		}
	}

	// 틈을 이용해서 내부로 들어갈 수 있는 부분을 없앤다.
	if ( IsCollisionCircle( vNewStart, vNewEnd, m_sLine.EndPointA(), fOffset ) )
	{
		return COLL_TRUE;
	}
	if ( IsCollisionCircle( vNewStart, vNewEnd, m_sLine.EndPointB(), fOffset ) )
	{
		return COLL_TRUE;
	}
	if ( IsCollisionCircle( m_sLine.EndPointA(), m_sLine.EndPointB(), vNewEnd, fOffset ) )
	{
		return COLL_TRUE;
	}

	return COLL_FALSE;


	//D3DXVECTOR3 vDir = vEnd - vStart;
	//float fDistance = D3DXVec3Length( &vDir );
	//fDistance += fOffset;
	//D3DXVec3Normalize( &vDir, &vDir );

	//D3DXVECTOR3 vNewEnd = vStart + (vDir*fDistance);

	//if ( CalculatePlane2Line_LimitX( m_sPlane, vStart, vNewEnd, m_vMinMaxX, m_matInverseRotationY, fOffset ) )
	//{
	//	if ( IsCollisionChangePosistion( m_sPlane, vStart, fOffset ) )
	//	{
	//		return COLL_TRUE_START_IN;
	//	}
	//	return COLL_TRUE;
	//}

	//return COLL_FALSE;
}

float SimpleCollisionPlane::GetMinusX() const
{
	return m_vMinMaxX.x;
}

float SimpleCollisionPlane::GetPlusX() const
{
	return m_vMinMaxX.y;
}

float SimpleCollisionPlane::GetRotationY() const
{
	return m_fRotationY;
}

void SimpleCollisionPlane::Save( sc::SerialFile &SFile )
{
	SimpleCollisionBase::Save( SFile );

	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_vMinMaxX, sizeof(D3DXVECTOR2) );
		SFile << m_fRotationY;
	}
	SFile.EndBlock();
}

void SimpleCollisionPlane::Load( sc::BaseStream &SFile )
{
	SimpleCollisionBase::Load( SFile );

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile.ReadBuffer( &m_vMinMaxX, sizeof(D3DXVECTOR2) );
	SFile >> m_fRotationY;

	InitData();
}




//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionBox
//////////////////////////////////////////////////////////////////////////
SimpleCollisionBox::SimpleCollisionBox()
	: SimpleCollisionBase( EMSCT_BOX, _T("BOX"))
	, m_vMinMaxX(-50.f,50.f)
	, m_vMinMaxZ(-50.f,50.f)
	, m_fRotationY(0.f)
{
	InitData();
}

SimpleCollisionBox::~SimpleCollisionBox()
{

}

void SimpleCollisionBox::InitData()
{
	D3DXVECTOR3 vPos[4];

	//////////////////////////////////////////////////////////////////////////
	vPos[0] = D3DXVECTOR3 ( m_vMinMaxX.x, 10000.f, m_vMinMaxZ.x );
	vPos[1] = D3DXVECTOR3 ( m_vMinMaxX.y, 10000.f, m_vMinMaxZ.x );
	vPos[2] = D3DXVECTOR3 ( m_vMinMaxX.x, -10000.f, m_vMinMaxZ.x );
	vPos[3] = D3DXVECTOR3 ( m_vMinMaxX.y, -10000.f, m_vMinMaxZ.x );

	D3DXMATRIX matRotateY;
	D3DXMatrixRotationY( &matRotateY, D3DX_PI*m_fRotationY/180 );
	matRotateY._41 = m_vPosition.x;
	matRotateY._42 = m_vPosition.y;
	matRotateY._43 = m_vPosition.z;
	D3DXMatrixInverse( &m_matInverseRotationY, NULL, &matRotateY );

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
	}

	D3DXPlaneFromPoints( &m_sPlane[0], &vPos[0], &vPos[1], &vPos[2] );

	//////////////////////////////////////////////////////////////////////////
	vPos[0] = D3DXVECTOR3 ( m_vMinMaxX.x, 10000.f, m_vMinMaxZ.y );
	vPos[1] = D3DXVECTOR3 ( m_vMinMaxX.y, 10000.f, m_vMinMaxZ.y );
	vPos[2] = D3DXVECTOR3 ( m_vMinMaxX.x, -10000.f, m_vMinMaxZ.y );
	vPos[3] = D3DXVECTOR3 ( m_vMinMaxX.y, -10000.f, m_vMinMaxZ.y );

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
	}

	D3DXPlaneFromPoints( &m_sPlane[1], &vPos[0], &vPos[1], &vPos[2] );

	//////////////////////////////////////////////////////////////////////////
	vPos[0] = D3DXVECTOR3 ( m_vMinMaxX.x, 10000.f, m_vMinMaxZ.x );
	vPos[1] = D3DXVECTOR3 ( m_vMinMaxX.x, 10000.f, m_vMinMaxZ.y );
	vPos[2] = D3DXVECTOR3 ( m_vMinMaxX.x, -10000.f, m_vMinMaxZ.x );
	vPos[3] = D3DXVECTOR3 ( m_vMinMaxX.x, -10000.f, m_vMinMaxZ.y );

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
	}

	D3DXPlaneFromPoints( &m_sPlane[2], &vPos[0], &vPos[1], &vPos[2] );

	//////////////////////////////////////////////////////////////////////////
	vPos[0] = D3DXVECTOR3 ( m_vMinMaxX.y, 10000.f, m_vMinMaxZ.x );
	vPos[1] = D3DXVECTOR3 ( m_vMinMaxX.y, 10000.f, m_vMinMaxZ.y );
	vPos[2] = D3DXVECTOR3 ( m_vMinMaxX.y, -10000.f, m_vMinMaxZ.x );
	vPos[3] = D3DXVECTOR3 ( m_vMinMaxX.y, -10000.f, m_vMinMaxZ.y );

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
	}

	D3DXPlaneFromPoints( &m_sPlane[3], &vPos[0], &vPos[1], &vPos[2] );
}

void SimpleCollisionBox::SetObjRotate( float fAddAngle )
{
	m_fRotationY += fAddAngle;
	SimpleCollisionBase::SetObjRotate( fAddAngle );
}

void SimpleCollisionBox::SetPosisionRotation( const D3DXVECTOR3& vPosition, float fRotationY )
{
	m_vPosition = vPosition;
	m_fRotationY = fRotationY;
	InitData();
}

SIMPLE_COLLISION_RESULT SimpleCollisionBox::IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset )
{
	// 2d 개념으로 충돌처리 필요함.

	// 구멍생김.
	//D3DXVECTOR3 vDir = vEnd - vStart;
	//float fDistance = D3DXVec3Length( &vDir );
	//fDistance += fOffset;
	//D3DXVec3Normalize( &vDir, &vDir );

	//D3DXVECTOR3 vNewEnd = vStart + (vDir*fDistance);

	//if ( CalculatePlane2Line_LimitX( m_sPlane[0], vStart, vNewEnd, m_vMinMaxX, m_matInverseRotationY, fOffset ) )
	//{
	//	if ( IsCollisionChangePosistion( m_sPlane[0], vStart, fOffset ) )
	//	{
	//		return COLL_TRUE_START_IN;
	//	}
	//	return COLL_TRUE;
	//}

	//if ( CalculatePlane2Line_LimitX( m_sPlane[1], vStart, vNewEnd, m_vMinMaxX, m_matInverseRotationY, fOffset ) )
	//{
	//	if ( IsCollisionChangePosistion( m_sPlane[1], vStart, fOffset ) )
	//	{
	//		return COLL_TRUE_START_IN;
	//	}
	//	return COLL_TRUE;
	//}

	//if ( CalculatePlane2Line_LimitZ( m_sPlane[2], vStart, vNewEnd, m_vMinMaxZ, m_matInverseRotationY, fOffset ) )
	//{
	//	if ( IsCollisionChangePosistion( m_sPlane[2], vStart, fOffset ) )
	//	{
	//		return COLL_TRUE_START_IN;
	//	}
	//	return COLL_TRUE;
	//}

	//if ( CalculatePlane2Line_LimitZ( m_sPlane[3], vStart, vNewEnd, m_vMinMaxZ, m_matInverseRotationY, fOffset ) )
	//{
	//	if ( IsCollisionChangePosistion( m_sPlane[3], vStart, fOffset ) )
	//	{
	//		return COLL_TRUE_START_IN;
	//	}
	//	return COLL_TRUE;
	//}

	return COLL_FALSE;
}

float SimpleCollisionBox::GetMinusX() const
{
	return m_vMinMaxX.x;
}

float SimpleCollisionBox::GetMinusZ() const
{
	return m_vMinMaxZ.x;
}

float SimpleCollisionBox::GetPlusX() const
{
	return m_vMinMaxX.y;
}

float SimpleCollisionBox::GetPlusZ() const
{
	return m_vMinMaxZ.y;
}

float SimpleCollisionBox::GetRotationY() const
{
	return m_fRotationY;
}

void SimpleCollisionBox::Save( sc::SerialFile &SFile )
{
	SimpleCollisionBase::Save( SFile );

	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_vMinMaxX, sizeof(D3DXVECTOR2) );
		SFile.WriteBuffer( &m_vMinMaxZ, sizeof(D3DXVECTOR2) );
		SFile << m_fRotationY;
	}
	SFile.EndBlock();
}

void SimpleCollisionBox::Load( sc::BaseStream &SFile )
{
	SimpleCollisionBase::Load( SFile );

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile.ReadBuffer( &m_vMinMaxX, sizeof(D3DXVECTOR2) );
	SFile.ReadBuffer( &m_vMinMaxZ, sizeof(D3DXVECTOR2) );
	SFile >> m_fRotationY;

	InitData();
}





//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionCylinder
//////////////////////////////////////////////////////////////////////////
SimpleCollisionCylinder::SimpleCollisionCylinder()
	: SimpleCollisionBase( EMSCT_CYLINDER, _T("CYLINDER") )
	, m_fRadius(50.f)
{

}

SimpleCollisionCylinder::~SimpleCollisionCylinder()
{

}

SIMPLE_COLLISION_RESULT SimpleCollisionCylinder::IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset )
{
	D3DXVECTOR2 vCenter(m_vPosition.x,m_vPosition.z);
	D3DXVECTOR2 vStart_2(vStart.x,vStart.z);
	D3DXVECTOR2 vEnd_2(vEnd.x,vEnd.z);

	D3DXVECTOR2 vLine1 = vStart_2 - vEnd_2;
	D3DXVECTOR2 vLine2 = vStart_2 - vCenter;

	float fDistance = D3DXVec2Length( &vLine1 );
	if ( fDistance <= 0.f )
		return COLL_FALSE;

	float fRadiusMin = (m_fRadius-fOffset);
	float fRadiusMax = (m_fRadius+fOffset);

	// 평면이 원과 닿이는지 확인한다.
	// 안닿이면 충돌되지 않음.
	{
		// ax + by + c = 0
		D3DXVECTOR2 vNor( vLine1.y, -vLine1.x );
		D3DXVec2Normalize( &vNor, &vNor );
		float fC = -D3DXVec2Dot( &vNor, &vStart_2 );

		// Note : 원점과의 거리를 구한다.
		float fTempDis = (vNor.x*vCenter.x) + (vNor.y*vCenter.y) + fC;
		fTempDis = fabsf( fTempDis );

		// Note : 체크를 한다.
		if( fTempDis > fRadiusMax )
		{
			return COLL_FALSE;
		}
	}


	// 길이 구함.
	float fDistance2 = D3DXVec2Length( &vLine2 );

	D3DXVECTOR2 vLine3 = vEnd_2 - vCenter;
	float fDistance3 = D3DXVec2Length( &vLine3 );


	// 큰원쪽
	// 두점다 단방향 외부로 나갔는지 확인한다. 나갔으면 충돌되지 않음.
	if ( fDistance2 > fRadiusMax && fDistance3 > fRadiusMax )
	{
		float fDot1 = D3DXVec2Dot( &vLine2, &vLine1 );
		float fDot2 = D3DXVec2Dot( &vLine3, &vLine1 );

		if ( fDot1 > 0.f && fDot2 > 0.f )
			return COLL_FALSE;

		if ( fDot1 < 0.f && fDot2 < 0.f )
			return COLL_FALSE;
	}


	// 작은 원쪽
	// 두점다 내부에 있는지 확인한다. 내부에 있다면 충돌되지 않음.
	if ( fDistance2 < fRadiusMin && fDistance3 < fRadiusMin )
	{
		return COLL_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 여기까지 왔다면 일단 충돌처리가 되었다는 것임.

	// 시작점이 내부에 있는지 체크한다. 있다면 위치를 변경시켜준다.
	if ( fDistance2 <= fRadiusMax && fDistance2 >= fRadiusMin )
	{
		return COLL_TRUE_START_IN;
	}
	return COLL_TRUE;
}

float SimpleCollisionCylinder::GetRadius() const
{
	return m_fRadius;
}

void SimpleCollisionCylinder::Save( sc::SerialFile &SFile )
{
	SimpleCollisionBase::Save( SFile );

	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << m_fRadius;
	}
	SFile.EndBlock();
}

void SimpleCollisionCylinder::Load( sc::BaseStream &SFile )
{
	SimpleCollisionBase::Load( SFile );

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_fRadius;
}