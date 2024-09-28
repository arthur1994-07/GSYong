#include "pch.h"

//#include "../../EngineBaseLib/Define/DxLandDef.h"
//#include "../../EngineBaseLib/Common/stlfunctions.h"

//#include "../DxLand/DxOctree/DxLandMan.h"
#include "../../NaviMesh/navigationmesh.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxLand/DxLandMan.h"

#include "./DxFootColl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const size_t CONST_COLL_MAX = 4;

	D3DXVECTOR3 g_vPointOrig[ CONST_COLL_MAX ];
	D3DXVECTOR3 g_vPointColl[ CONST_COLL_MAX ];
}

//------------------------------------------------------------------------------------------------------------
//								D	x		F	o	o	t		C	o	l	l
//------------------------------------------------------------------------------------------------------------
DxFootColl::DxFootColl()
	: m_emMode(MODE_NONE)
{
	// 2발 자전거....
	m_vecFootColl_2.push_back( D3DXVECTOR3( 0.f, 0.f, -1.f ) );
	m_vecFootColl_2.push_back( D3DXVECTOR3( 0.f, 0.f, 1.f ) );

	// 4발 짐승
	m_vecFootColl_4.push_back( D3DXVECTOR3( 1.f, 0.f, -1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( -1.f, 0.f, -1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( 1.f, 0.f, 1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( -1.f, 0.f, 1.f ) );
}

DxFootColl::~DxFootColl()
{
	CleanUp();
}

void DxFootColl::CleanUp()
{
	m_vecFootColl_2.clear();
	m_vecFootColl_4.clear();

	// 2발 자전거....
	m_vecFootColl_2.push_back( D3DXVECTOR3( 0.f, 0.f, -1.f ) );
	m_vecFootColl_2.push_back( D3DXVECTOR3( 0.f, 0.f, 1.f ) );

	// 4발 짐승
	m_vecFootColl_4.push_back( D3DXVECTOR3( 1.f, 0.f, -1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( -1.f, 0.f, -1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( 1.f, 0.f, 1.f ) );
	m_vecFootColl_4.push_back( D3DXVECTOR3( -1.f, 0.f, 1.f ) );
}

BOOL DxFootColl::GetMatrix( D3DXMATRIX &matWorldInOut, DxLandMan *pLandMan, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vDirOrig, NavigationMesh* pNavigation ) const
{
	if( m_emMode == MODE_NONE )
		return FALSE;

	if( !pLandMan )
		return FALSE;

	if ( !pLandMan->GetNaviMesh() )
		return FALSE;

	if( m_emMode == MODE_POINT4 )
	{
		D3DXMATRIX matRot;
		float fTheta = DXGetThetaYFromDirection( vDir, vDirOrig );
		D3DXMatrixRotationY( &matRot, fTheta );
		if ( !CalculateCollisionY( m_vecFootColl_4, pLandMan, vPos, matRot ) )
			return FALSE;

		if ( pNavigation == NULL )
		{
			D3DXVECTOR3 vFront	= (g_vPointColl[ 0 ] + g_vPointColl[ 1 ]) * 0.5f;
			D3DXVECTOR3 vBack	= (g_vPointColl[ 2 ] + g_vPointColl[ 3 ]) * 0.5f;
			D3DXVECTOR3 vLeft	= (g_vPointColl[ 0 ] + g_vPointColl[ 2 ]) * 0.5f;
			D3DXVECTOR3 vRight	= (g_vPointColl[ 1 ] + g_vPointColl[ 3 ]) * 0.5f;

			D3DXVECTOR3 vDir1 = vFront - vBack;
			D3DXVECTOR3 vDir2 = vLeft - vRight;

			D3DXVec3Normalize( &vDir1, &vDir1 );
			D3DXVec3Normalize( &vDir2, &vDir2 );

			FLOAT fAngle = vDir1.y;	// * D3DX_PI * 0.5f;
			D3DXMATRIX matRotX;
			fAngle = asinf( fAngle );
			D3DXMatrixRotationX( &matRotX, fAngle );

			fAngle = vDir2.y;	// * D3DX_PI * 0.5f;
			D3DXMATRIX matRotZ;
			fAngle = asinf( fAngle );
			D3DXMatrixRotationZ( &matRotZ, fAngle );

			D3DXMatrixMultiply( &matRotX, &matRotX, &matRotZ );
			D3DXMatrixMultiply( &matRot, &matRotX, &matRot );

			matWorldInOut = matRot;
			matWorldInOut._41 = vPos.x;
			matWorldInOut._42 = vPos.y;
			matWorldInOut._43 = vPos.z;
			return TRUE;
		}

		/* [0][1] */
		/* [2][3] */
		// 바퀴별 공중에 떳는지 판별;
		// 무조건 한바퀴는 떳다고 생각한다, 경우에 따라 3개까지 뜬다;
		bool	bAir[4] = {false,};
		float	fGap[4] = {0.0f,};
		short	sHeavy = 0;			// 제일 격차가 심한 바퀴;
		short	sAirCount = 0;

		const DWORD dwCurrentCell = pNavigation->FindClosestCell(vPos);
		if ( !pNavigation->GetCell( dwCurrentCell ) )
			return FALSE;

		int i;
		for ( i = 0; i < 4; ++i )
		{
			// 비율;
// 			fGap[i] = D3DXVec3Length( &(vPos - g_vPointColl[i]) )/D3DXVec3Length( &(vPos - g_vPointOrig[i]) );
// 			sHeavy = (fGap[i] > fGap[sHeavy]) ? sHeavy : static_cast<short>(i);
// 			if ( fGap[i] > 1.15f )	// 원래 크기의 1.2배;
// 			{
// 				bAir[i] = true;
// 				++sAirCount;
// 			}
			D3DXVECTOR3 vDest;
			DWORD dwCollisionID = 0;
			BOOL bCollision = FALSE;
			pNavigation->IsCollision( g_vPointColl[i] + D3DXVECTOR3(0.0f, 10.0f, 0.0f)
				, g_vPointColl[i]- D3DXVECTOR3(0.0f, 10.0f, 0.0f)
				, vDest, dwCollisionID, bCollision );
			if ( bCollision == FALSE || pNavigation->LineOfSightTest(dwCurrentCell, vPos, dwCollisionID, vDest ) == FALSE )
			{
				bAir[i] = true;
				++sAirCount;
			}
		}

		// 바퀴하나는 무조건 떳다고 설정;
		if ( sAirCount == 0 )
		{
			sAirCount = 1;
			bAir[sHeavy] = true;
		}

		float fAngleX = 0.0f, fAngleZ = 0.0f;
		switch ( sAirCount )
		{
		case 1:
			{
				// 세로 가로 판단;
				const bool bLeft = ((bAir[0] | bAir[2]) == false);
				const bool bRear = ((bAir[2] | bAir[3]) == false);

				D3DXMATRIX matRotX, matRotZ;
				D3DXMatrixIdentity( &matRotX );
				D3DXMatrixIdentity( &matRotZ );
				D3DXVECTOR3 vDir_1, vDir_2;
				fAngleX = D3DXVec3Normalize( &vDir_1, &(bLeft ? g_vPointColl[0] - g_vPointColl[2] : g_vPointColl[1] - g_vPointColl[3]) )->y;
				fAngleZ = D3DXVec3Normalize( &vDir_2, &(bRear ? g_vPointColl[2] - g_vPointColl[3] : g_vPointColl[0] - g_vPointColl[1]) )->y;
			}
			break;
		case 2:
			{	// 대각선 방향의 바퀴가 2개 붙을 일은없다;
				const bool bLeft = ((bAir[0] | bAir[2]) == false);
				const bool bRight = ((bAir[1] | bAir[3]) == false);
				const bool bFront = ((bAir[0] | bAir[1]) == false);
				const bool bRear = ((bAir[2] | bAir[3]) == false);

				D3DXVECTOR3 vDir_1, vDir_2;
				if ( bLeft )
				{					
					const D3DXVECTOR3 vLeft	= (g_vPointColl[ 0 ] + g_vPointColl[ 2 ]) * 0.5f;
					fAngleX = D3DXVec3Normalize( &vDir_1, &(g_vPointColl[0] - g_vPointColl[2]) )->y;
					fAngleZ = D3DXVec3Normalize( &vDir_2, &(vLeft - vPos))->y;
				}
				else if ( bRight )
				{
					const D3DXVECTOR3 vRight = (g_vPointColl[ 1 ] + g_vPointColl[ 3 ]) * 0.5f;
					fAngleX = D3DXVec3Normalize( &vDir_1, &(g_vPointColl[1] - g_vPointColl[3]) )->y;
					fAngleZ = D3DXVec3Normalize( &vDir_2, &(vPos - vRight))->y;
				}
				else if ( bFront )
				{
					const D3DXVECTOR3 vFront = (g_vPointColl[ 0 ] + g_vPointColl[ 1 ]) * 0.5f;
					fAngleX = D3DXVec3Normalize( &vDir_1, &(vPos - vFront))->y;
					fAngleZ = D3DXVec3Normalize( &vDir_2, &(g_vPointColl[0] - g_vPointColl[1]) )->y;
				}
				else if ( bRear )
				{
					const D3DXVECTOR3 vBack = (g_vPointColl[ 2 ] + g_vPointColl[ 3 ]) * 0.5f;
					fAngleX = D3DXVec3Normalize( &vDir_1, &(vPos - vBack))->y;
					fAngleZ = D3DXVec3Normalize( &vDir_2, &(g_vPointColl[2] - g_vPointColl[3]) )->y;
				}
				else
				{
					int dummycode;
					dummycode=0;
				}
			}
			break;
		case 3:
			{
				matWorldInOut = matRot;
			}
			break;
		default:
			{
				matWorldInOut = matRot;
				//MessageBox(NULL, "GetMatrix, MODE_POINT4", "ERROR", MB_OK);;
			}
			break;
		}

		D3DXMATRIX matRotX, matRotZ;
		D3DXMatrixRotationX( &matRotX, fAngleX );
		D3DXMatrixRotationZ( &matRotZ, fAngleZ );
		D3DXMatrixMultiply( &matRotX, &matRotX, &matRotZ );
		D3DXMatrixMultiply( &matRot, &matRotX, &matRot );
		matWorldInOut = matRot;
		matWorldInOut._41 = vPos.x;
		matWorldInOut._42 = vPos.y;// + fOffset;
		matWorldInOut._43 = vPos.z;

	}
	else if( m_emMode == MODE_POINT2 )
	{
        D3DXMATRIX matRot;
		float fTheta = DXGetThetaYFromDirection( vDir, vDirOrig );
		D3DXMatrixRotationY( &matRot, fTheta );

		if( !CalculateCollisionY( m_vecFootColl_2, pLandMan, vPos, matRot ) )
			return FALSE;

		D3DXVECTOR3 vNormal = g_vPointColl[ 0 ] - g_vPointColl[ 1 ];
		D3DXVec3Normalize( &vNormal, &vNormal );

		float fAngle = vNormal.y;	// * D3DX_PI * 0.5f;
		fAngle = asinf( fAngle );
		D3DXMATRIX matRotX;
		D3DXMatrixRotationX( &matRotX, fAngle );

		D3DXMatrixMultiply( &matRot, &matRotX, &matRot );

		matWorldInOut = matRot;
		matWorldInOut._41 = vPos.x;
		matWorldInOut._42 = vPos.y;
		matWorldInOut._43 = vPos.z;
	}

    return TRUE;
}

// RK 랑은 다르게 FALSE 는 나오지 않도록 작업함.
BOOL DxFootColl::CalculateCollisionY( const VEC_VECTOR3& vecFootColl, DxLandMan *pLandMan, const D3DXVECTOR3 &vPos, const D3DXMATRIX &matRotate ) const
{
	size_t dwSize = vecFootColl.size();
	for( size_t i=0; i<dwSize; ++i )
	{
		const D3DXVECTOR3& refPoint = vecFootColl[ i ];

        g_vPointOrig[ i ] = refPoint;
        D3DXVec3TransformCoord( &g_vPointOrig[ i ], &g_vPointOrig[ i ], &matRotate );
        g_vPointOrig[ i ] += vPos;

		// 네비게이션으로 검사를 해보고 안된다면 일반 Mesh 또한 검사해보자.
		BOOL bCollision(FALSE);
		DWORD dwCollisionID(-1);
		pLandMan->GetNaviMesh()->IsCollision( g_vPointOrig[ i ] + D3DXVECTOR3( 0, 100.f, 0 ),
												g_vPointOrig[ i ] - D3DXVECTOR3( 0, 100.f, 0 ),
												g_vPointColl[ i ],
												dwCollisionID,
												bCollision );

		// 충돌이 안되고 있다.
		// Mesh 로 충돌처리하자. Mesh 로 하면 문제가 생길 수도 있어서 범위를 10 ~ -10 으로 좁게 잡았다.
		if ( !bCollision )
		{
			pLandMan->IsCollisionNEW(
				g_vPointOrig[ i ] + D3DXVECTOR3( 0, 10.f, 0 ),
				g_vPointOrig[ i ] - D3DXVECTOR3( 0, 10.f, 0 ),
				g_vPointColl[ i ],
				bCollision,
				EMCC_CULL_CW,
				TRUE,
				FALSE );

			if ( bCollision == 0 )
			{
				// 충돌이 안된다면 기본 높이로 셋팅하도록 하자.
				g_vPointColl[ i ] = g_vPointOrig[ i ];
			}
		}
	}

	return TRUE;
}

void DxFootColl::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << static_cast<DWORD>( m_emMode );

		DWORD dwSize = (DWORD)m_vecFootColl_2.size();
		SFile << dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile << m_vecFootColl_2[i];
		}

		dwSize = (DWORD)m_vecFootColl_4.size();
		SFile << dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile << m_vecFootColl_4[i];
		}
	}
	SFile.EndBlock();
}

void DxFootColl::Load( sc::BaseStream& SFile )
{
	m_vecFootColl_2.clear();
	m_vecFootColl_4.clear();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		DWORD dwTemp;
		SFile >> dwTemp;
		m_emMode = static_cast<MODE>( dwTemp );

		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> vPos;
			m_vecFootColl_2.push_back( vPos );
		}

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> vPos;
			m_vecFootColl_4.push_back( vPos );
		}
	}
}