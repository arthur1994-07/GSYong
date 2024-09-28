// Collision.cpp: implementation of the CCollision class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"
#include "../GlobalParam.h"

#include "./EDITMESHS.h"
#include "./Collision.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace COLLISION
{
	//D3DXVECTOR3 vColTestStart;
	//D3DXVECTOR3 vColTestEnd;

	//BOOL IsPoint_Inside_Triangle ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
	//							D3DXVECTOR3 *pPoint,
	//							BOOL bX, BOOL bY, BOOL bZ )
	//{
	//	D3DXVECTOR2 v1, v2, v3, vP;

	//	float deltaX=0.0f;
	//	float m1=0.0f, m2=0.0f, m3=0.0f;
	//	float b1=0.0f, b2=0.0f, b3=0.0f;
	//	BOOL b12Vect=FALSE, b23Vect=FALSE, b31Vect=FALSE;

	//	//	Note : 단 하나만 TRUE 되어 있어야 정상 동작한다.
	//	//
	//	if ( ((bX==FALSE)&&(bY==FALSE)) ||
	//		((bX==FALSE)&&(bZ==FALSE)) ||
	//		((bY==FALSE)&&(bZ==FALSE)) ||
	//		((bX==FALSE)&&(bY==FALSE)&&(bZ==FALSE)) )
	//	{
	//		return FALSE;
	//	}

	//	//	Note : x 축 투영.
	//	if ( bX==FALSE )
	//	{
	//		v1.x = pTri0->y;
	//		v1.y = pTri0->z;
	//		v2.x = pTri1->y;
	//		v2.y = pTri1->z;
	//		v3.x = pTri2->y;
	//		v3.y = pTri2->z;
	//		
	//		vP.x = pPoint->y;
	//		vP.y = pPoint->z;
	//	}
	//	//	Note : y축 투영.
	//	else if ( bY==FALSE )
	//	{
	//		v1.x = pTri0->x;
	//		v1.y = pTri0->z;
	//		v2.x = pTri1->x;
	//		v2.y = pTri1->z;
	//		v3.x = pTri2->x;
	//		v3.y = pTri2->z;
	//		
	//		vP.x = pPoint->x;
	//		vP.y = pPoint->z;
	//	}
	//	//	Note : z축 투영.
	//	else if ( bZ==FALSE )
	//	{
	//		v1.x = pTri0->x;
	//		v1.y = pTri0->y;
	//		v2.x = pTri1->x;
	//		v2.y = pTri1->y;
	//		v3.x = pTri2->x;
	//		v3.y = pTri2->y;
	//		
	//		vP.x = pPoint->x;
	//		vP.y = pPoint->y;
	//	}


	//	//	Note : y=mx+b 세가지 선분 방정식 계산.
	//	//
	//	deltaX = (v2.x-v1.x);
	//	if ( deltaX!=0 )
	//	{
	//		m1 = (v2.y-v1.y) / deltaX;	//	v1 -> v2
	//		b1 = v1.y - (m1*v1.x);		//	b=y-mx
	//	}
	//	else //	deltaX==0
	//	{
	//		b12Vect = TRUE;
	//	}

	//	deltaX = (v3.x-v2.x);
	//	if ( deltaX!=0 )
	//	{
	//		m2 = (v3.y-v2.y) / deltaX;	//	v2 -> v3
	//		b2 = v2.y - (m2*v2.x);		//	b=y-mx
	//	}
	//	else //	deltaX==0
	//	{
	//		b23Vect = TRUE;
	//	}

	//	deltaX = (v1.x-v3.x);
	//	if ( deltaX!=0 )
	//	{
	//		m3 = (v1.y-v3.y) / deltaX;	//	v3 -> v1
	//		b3 = v3.y - (m3*v3.x);		//	b=y-mx
	//	}
	//	else //	deltaX==0
	//	{
	//		b31Vect = TRUE;
	//	}
	//	
	//	//	Note : 삼각형이 아니라 직선에 가까워서 확인 작업 중단.
	//	if ( IsfEqual(m1,m2) && IsfEqual(m1,m3) )	return FALSE;
	//	////  Modify : 가까운 2개의 점이 있을 경우 문제가 발생했다.
	//	////			밑 부분에서 선같은 경우는 체크를 못하기 때문에 여기서 막았다.
	//	//if ( IsfEqual(m1,m2) || IsfEqual(m1,m3) || IsfEqual(m2,m3) )	return FALSE;

	//	//	Note : 중점 찾기.
	//	//		- 트라이엥글 안에 있는 점 구하기.
	//	//
	//	D3DXVECTOR2 vCenter;
	//	vCenter.x = (v1.x+v2.x+v3.x) / 3;
	//	vCenter.y = (v1.y+v2.y+v3.y) / 3;

	//	//	Note : 선분을 기준으로 중점이 어디에 위치하는지 구하기.
	//	//
	//	BOOL bUP1, bUP2, bUP3;

	//	//	Note : v1 -> v2
	//	if ( (m1*vCenter.x+b1) >= vCenter.y )
	//		bUP1 = TRUE;
	//	else
	//		bUP1 = FALSE;

	//	//	Note : v2 -> v3
	//	if ( (m2*vCenter.x+b2) >= vCenter.y )
	//		bUP2 = TRUE;
	//	else
	//		bUP2 = FALSE;

	//	//	Note : v3 -> v1
	//	if ( (m3*vCenter.x+b3) >= vCenter.y )
	//		bUP3 = TRUE;
	//	else
	//		bUP3 = FALSE;


	//	//	Note : 삼각형 내부에 있는지 판단.
	//	//
	//	int nInside = 0;

	//	//	Note : v1 -> v2
	//	if ( b12Vect==TRUE )
	//	{
	//		if ( (v1.x<vP.x) && (v1.x<vCenter.x) )
	//			nInside++;
	//		else if ( (v1.x>vP.x) && (v1.x>vCenter.x) )
	//			nInside++;
	//	}
	//	else
	//	{
	//		if ( bUP1 )
	//		{
	//			if ( vP.y <= (m1*vP.x)+b1 )
	//				nInside++;
	//		}
	//		else
	//		{
	//			if ( vP.y >= (m1*vP.x)+b1 )
	//				nInside++;
	//		}
	//	}

	//	//	Note : v2 -> v3
	//	if ( b23Vect==TRUE )
	//	{
	//		if ( (v2.x<vP.x) && (v2.x<vCenter.x) )
	//			nInside++;
	//		else if ( (v2.x>vP.x) && (v2.x>vCenter.x) )
	//			nInside++;
	//	}
	//	else
	//	{
	//		if ( bUP2 )
	//		{
	//			if ( vP.y <= (m2*vP.x)+b2 )
	//				nInside++;
	//		}
	//		else
	//		{
	//			if ( vP.y >= (m2*vP.x)+b2 )
	//				nInside++;
	//		}
	//	}

	//	//	Note : v3 -> v1
	//	if ( b31Vect==TRUE )
	//	{
	//		if ( (v3.x<vP.x) && (v3.x<vCenter.x) )
	//			nInside++;
	//		else if ( (v3.x>vP.x) && (v3.x>vCenter.x) )
	//			nInside++;
	//	}
	//	else
	//	{
	//		if ( bUP3 )
	//		{
	//			if ( vP.y <= (m3*vP.x)+b3 )
	//				nInside++;
	//		}
	//		else
	//		{
	//			if ( vP.y >= (m3*vP.x)+b3 )
	//				nInside++;
	//		}
	//	}

	//	if ( nInside == 3 )		return TRUE;

	//	return FALSE;
	//}

	BOOL IsCollision2DLineToLine( const D3DXVECTOR2& AP1, const D3DXVECTOR2& AP2, const D3DXVECTOR2& BP1, const D3DXVECTOR2& BP2, D3DXVECTOR2& vOut ) //주병진 추가 메소드
	{
		double t;
		double s;	
		double under = (BP2.y-BP1.y)*(AP2.x-AP1.x)-(BP2.x-BP1.x)*(AP2.y-AP1.y);

		if(under==0) 
			return FALSE;

		double _t = (BP2.x-BP1.x)*(AP1.y-BP1.y) - (BP2.y-BP1.y)*(AP1.x-BP1.x);
		double _s = (AP2.x-AP1.x)*(AP1.y-BP1.y) - (AP2.y-AP1.y)*(AP1.x-BP1.x);	
		t = _t/under;
		s = _s/under;	

		if(t<0.0 || t>1.0 || s<0.0 || s>1.0) 
			return FALSE;	

		if(_t==0 &&    _s==0) 
			return FALSE;	

		vOut.x = static_cast<FLOAT>(AP1.x + t * (double)(AP2.x-AP1.x));
		vOut.y = static_cast<FLOAT>(AP1.y + t * (double)(AP2.y-AP1.y));

		return TRUE;
	}

	BOOL IsCollision2DTriangleToSquare( D3DXVECTOR2 sLineTriangle[], D3DXVECTOR2 sCross[], D3DXVECTOR2 sLineSquare[] )
	{
		D3DXVECTOR2 vOut;

		// 삼각형의 1번 라인 검사 ( Line 과 Line 검사 )
		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[1], sLineSquare[0], sLineSquare[1], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[1], sLineSquare[1], sLineSquare[2], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[1], sLineSquare[2], sLineSquare[3], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[1], sLineSquare[3], sLineSquare[0], vOut ) )
			return TRUE;


		// 삼각형의 2번 라인 검사 ( Line 과 Line 검사 )
		if ( IsCollision2DLineToLine( sLineTriangle[2], sLineTriangle[1], sLineSquare[0], sLineSquare[1], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[2], sLineTriangle[1], sLineSquare[1], sLineSquare[2], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[2], sLineTriangle[1], sLineSquare[2], sLineSquare[3], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[2], sLineTriangle[1], sLineSquare[3], sLineSquare[0], vOut ) )
			return TRUE;


		// 삼각형의 3번 라인 검사 ( Line 과 Line 검사 )
		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[2], sLineSquare[0], sLineSquare[1], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[2], sLineSquare[1], sLineSquare[2], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[2], sLineSquare[2], sLineSquare[3], vOut ) )
			return TRUE;

		if ( IsCollision2DLineToLine( sLineTriangle[0], sLineTriangle[2], sLineSquare[3], sLineSquare[0], vOut ) )
			return TRUE;


		// 사각형에 삼각형의 한 점이라도 들어있으면 충돌됨.
		if ( sLineSquare[0].x <= sLineTriangle[0].x && 
			sLineSquare[2].x >= sLineTriangle[0].x &&
			sLineSquare[0].y >= sLineTriangle[0].y &&
			sLineSquare[2].y >= sLineTriangle[0].y )
			return TRUE;

		// 사각형에 삼각형의 한 점이라도 들어있으면 충돌됨.
		if ( sLineSquare[0].x <= sLineTriangle[1].x && 
			sLineSquare[2].x >= sLineTriangle[1].x &&
			sLineSquare[0].y >= sLineTriangle[1].y &&
			sLineSquare[2].y >= sLineTriangle[1].y )
			return TRUE;

		// 사각형에 삼각형의 한 점이라도 들어있으면 충돌됨.
		if ( sLineSquare[0].x <= sLineTriangle[1].x && 
			sLineSquare[2].x >= sLineTriangle[1].x &&
			sLineSquare[0].y >= sLineTriangle[1].y &&
			sLineSquare[2].y >= sLineTriangle[1].y )
			return TRUE;


		// 삼각형에 사각형의 한 점이라도 들어있으면 충돌됨.
		D3DXVECTOR2 vDir;
		for ( DWORD i=0; i<4; ++i )
		{
			vDir = sLineSquare[i] - sLineTriangle[0];
			if ( (D3DXVec2Dot( &vDir, &sCross[0] ) >= 0.f) )
			{
				vDir = sLineSquare[i] - sLineTriangle[1];
				if ( (D3DXVec2Dot( &vDir, &sCross[1] ) >= 0.f) )
				{
					vDir = sLineSquare[i] - sLineTriangle[2];
					if ( (D3DXVec2Dot( &vDir, &sCross[2] ) >= 0.f) )
					{
						return TRUE;
					}
				}
			}
		}

		return FALSE;

	}

	BOOL IsLineTriangleCollision(
		const D3DXVECTOR3 *pTri0,
		const D3DXVECTOR3 *pTri1,
		const D3DXVECTOR3 *pTri2,
		const D3DXVECTOR3 *pPoint1,
		const D3DXVECTOR3 *pPoint2,
		D3DXVECTOR3 *pCollision,
		float& t, 
		float& u, 
		float& v,
		D3DXVECTOR3 *pNormal,
		EMCOLLMODE emCullMode,
		BOOL bSmallHeight )
	{
        D3DXVECTOR3 vDir( *pPoint2 - *pPoint1 );
        float fLength = D3DXVec3Length( &vDir );

		D3DXVECTOR3 vEdge1, vEdge2;
		if( IntersectTriangle( *pPoint1, vDir, *pTri0, *pTri1, *pTri2, &t, &u, &v, 0.000001f, emCullMode ) )
		{
			if( t>=0.f && t<1.f )
			{
				vEdge1 = *pTri1 - *pTri0;
				vEdge2 = *pTri2 - *pTri0;
				*pCollision = *pTri0 + (vEdge1*u) + (vEdge2*v);

                //// 충돌위치가 더 멀면 충돌 된 것이 아니다.
                //D3DXVECTOR3 vDir2 = *pCollision - *pPoint1;
                //float fLength2 = D3DXVec3Length( &vDir2 );
                //if( fLength < fLength2 )
                //{
                //    return FALSE;
                //}

                //	Note : 노말 값 구하기.
                //
                D3DXVECTOR3 vNormal;
                D3DXVec3Cross ( &vNormal, &vEdge1, &vEdge2 );
                D3DXVec3Normalize ( &vNormal, &vNormal );

                //	Note : 충돌 값을 반환.
	            if( bSmallHeight )
	            {
	            	*pCollision += vNormal * 0.001f;
	            }
	            else
	            {
	            	*pCollision += vNormal * 0.1f;	// 약간 앞부분을 충돌지점으로 하자.
	            									// 문제가 생긴다면 지워도 되지만
	            									// 캐릭터의 이동시 DxPiece::CollBSP_Move 쪽에서 수정이 필요할 것임.
	            }

                if( pNormal )
                {
                     *pNormal = vNormal;
                }

				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL IsLineTriangleCollision(
		const D3DXVECTOR3 *pTri0,
		const D3DXVECTOR3 *pTri1,
		const D3DXVECTOR3 *pTri2,
		const D3DXVECTOR3 *pPoint1,
		const D3DXVECTOR3 *pPoint2,
		D3DXVECTOR3 *pCollision,
		D3DXVECTOR3 *pNormal,
		EMCOLLMODE emCullMode,
		BOOL bSmallHeight )
	{
		float t,u,v;
		return IsLineTriangleCollision( pTri0, pTri1, pTri2, pPoint1, pPoint2, pCollision, t, u, v, pNormal, emCullMode, bSmallHeight );
	}

    BOOL IntersectTriangle(
		const D3DXVECTOR3& orig,
		const D3DXVECTOR3& dir,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2,
		FLOAT* t,
		FLOAT* u,
		FLOAT* v,
		const FLOAT fMinColl,
		EMCOLLMODE emCullMode )
	{
		// Find vectors for two edges sharing vert0
		D3DXVECTOR3 edge1 = v1 - v0;
		D3DXVECTOR3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		D3DXVECTOR3 pvec;
		D3DXVec3Cross( &pvec, &dir, &edge2 );

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = D3DXVec3Dot( &edge1, &pvec );

		// 충돌면 검사. 앞, 뒤, 양면
		D3DXVECTOR3 tvec;
		if ( emCullMode & EMCC_CULL_NONE ) // 앞/뒤 모두 검사;
		{
			if( det > 0 )
			{
				tvec = orig - v0;
			}
			else
			{
				tvec = v0 - orig;
				det = -det;
			}
		}
		else if ( emCullMode & D3DCULL_CW )	// 뒷면만 검사하겠다;
		{
			tvec = v0 - orig;
			det = -det;
		}
		else // 앞면만 검사하겠다;
		{
			tvec = orig - v0;
		}

		if( det < fMinColl )
			return FALSE;

		// Calculate U parameter and test bounds
		*u = D3DXVec3Dot( &tvec, &pvec );
		if( *u < 0.0f || *u > det )
			return FALSE;

		// Prepare to test V parameter
		D3DXVECTOR3 qvec;
		D3DXVec3Cross( &qvec, &tvec, &edge1 );

		// Calculate V parameter and test bounds
		*v = D3DXVec3Dot( &dir, &qvec );
		if( *v < 0.0f || *u + *v > det )
			return FALSE;

		// Calculate t, scale parameters, ray intersects triangle
		*t = D3DXVec3Dot( &edge2, &qvec );
		FLOAT fInvDet = 1.0f / det;
		*t *= fInvDet;
		*u *= fInvDet;
		*v *= fInvDet;

		return TRUE;
	}

	void IntersectTriangle_GetUV(
		const D3DXVECTOR3& orig,
		const D3DXVECTOR3& dir,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2,
		FLOAT* u,
		FLOAT* v )
	{
		// Find vectors for two edges sharing vert0
		D3DXVECTOR3 edge1 = v1 - v0;
		D3DXVECTOR3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		D3DXVECTOR3 pvec;
		D3DXVec3Cross( &pvec, &dir, &edge2 );

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = D3DXVec3Dot( &edge1, &pvec );

		D3DXVECTOR3 tvec;
		if( det > 0 )
		{
			tvec = orig - v0;
		}
		else if ( det == 0.f )
		{
			*u = 0.f;
			*v = 0.f;
		}
		else
		{
			tvec = v0 - orig;
			det = -det;
		}

		// Calculate U parameter and test bounds
		*u = D3DXVec3Dot( &tvec, &pvec );

		// Prepare to test V parameter
		D3DXVECTOR3 qvec;
		D3DXVec3Cross( &qvec, &tvec, &edge1 );

		// Calculate V parameter and test bounds
		*v = D3DXVec3Dot( &dir, &qvec );

		// Calculate t, scale parameters, ray intersects triangle
		FLOAT fInvDet = 1.0f / det;
		*u *= fInvDet;
		*v *= fInvDet;
	}

	BOOL IntersectTriangle(
		const D3DXVECTOR3& orig,
		const D3DXVECTOR3& dir,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2,
		FLOAT* t,
		const FLOAT fMinColl,
		EMCOLLMODE emCullMode )
	{
		// Find vectors for two edges sharing vert0
		D3DXVECTOR3 edge1(v1 - v0);
		D3DXVECTOR3 edge2(v2 - v0);

		// Begin calculating determinant - also used to calculate U parameter
		D3DXVECTOR3 pvec;
		D3DXVec3Cross( &pvec, &dir, &edge2 );

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = D3DXVec3Dot( &edge1, &pvec );

		// 충돌면 검사. 앞, 뒤, 양면
		D3DXVECTOR3 tvec;		
		if ( emCullMode & EMCC_CULL_NONE ) // 앞/뒤 모두 검사;
		{
			if( det > 0 )
			{
				tvec = orig - v0;
			}
			else
			{
				tvec = v0 - orig;
				det = -det;
			}
		}
		else if ( emCullMode & D3DCULL_CW )	// 뒷면만 검사하겠다;
		{
			tvec = v0 - orig;
			det = -det;
		}
		else // 앞면만 검사하겠다;
		{
			tvec = orig - v0;
		}

		if( det < fMinColl )
			return FALSE;

		// Calculate U parameter and test bounds
		FLOAT u = D3DXVec3Dot( &tvec, &pvec );
		if( u < 0.0f || u > det )
			return FALSE;

		// Prepare to test V parameter
		D3DXVECTOR3 qvec;
		D3DXVec3Cross( &qvec, &tvec, &edge1 );

		// Calculate V parameter and test bounds
		FLOAT v = D3DXVec3Dot( &dir, &qvec );
		if( v < 0.0f || u + v > det )
			return FALSE;

		// Calculate t, scale parameters, ray intersects triangle
		*t = D3DXVec3Dot( &edge2, &qvec );
		FLOAT fInvDet = 1.0f / det;
		*t *= fInvDet;

		return TRUE;
	}

	BOOL IsCollisionVolume(
		const CLIPVOLUME& cv,
		const D3DXVECTOR3 &vCenter,
		float fLength )
	{
		float fLENGTH_NEW(0.f);

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pNear, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pFar, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pLeft, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pRight, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pTop, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		fLENGTH_NEW = D3DXPlaneDotCoord( &cv.pBottom, &vCenter );
		if( fLENGTH_NEW > fLength  )		return FALSE;

		return TRUE;
	}

	//BOOL g_bCheckAddPlane(FALSE);
	//BOOL bCollisionCheck = TRUE;
	BOOL IsCollisionVolume ( const CLIPVOLUME& cv, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
	{
		//if( !bCollisionCheck ) return TRUE;

		//D3DXVECTOR3 vNearMin(	cv.pNear.a * vMin.x,	cv.pNear.b * vMin.y,	cv.pNear.c * vMin.z );
		//D3DXVECTOR3 vNearMax(	cv.pNear.a * vMax.x,	cv.pNear.b * vMax.y,	cv.pNear.c * vMax.z );

		//if( ((vNearMin.x + vNearMax.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
		//	((vNearMin.x + vNearMax.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
		//	((vNearMax.x + vNearMax.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
		//	((vNearMax.x + vNearMax.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		//	((vNearMin.x + vNearMin.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		//	((vNearMin.x + vNearMin.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		//	((vNearMax.x + vNearMin.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		//	((vNearMax.x + vNearMin.y + vNearMin.z + cv.pNear.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
		//	return FALSE;

		D3DXVECTOR3 vFarMin(	cv.pFar.a * vMin.x,		cv.pFar.b * vMin.y,		cv.pFar.c * vMin.z );
		D3DXVECTOR3 vFarMax(	cv.pFar.a * vMax.x,		cv.pFar.b * vMax.y,		cv.pFar.c * vMax.z );

		if( ((vFarMin.x + vFarMax.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vFarMin.x + vFarMax.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vFarMax.x + vFarMax.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vFarMax.x + vFarMax.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vFarMin.x + vFarMin.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vFarMin.x + vFarMin.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vFarMax.x + vFarMin.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vFarMax.x + vFarMin.y + vFarMin.z + cv.pFar.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		D3DXVECTOR3 vLeftMin(	cv.pLeft.a * vMin.x,	cv.pLeft.b * vMin.y,	cv.pLeft.c * vMin.z );
		D3DXVECTOR3 vLeftMax(	cv.pLeft.a * vMax.x,	cv.pLeft.b * vMax.y,	cv.pLeft.c * vMax.z );
		D3DXVECTOR3 vRightMin(	cv.pRight.a * vMin.x,	cv.pRight.b * vMin.y,	cv.pRight.c * vMin.z );
		D3DXVECTOR3 vRightMax(	cv.pRight.a * vMax.x,	cv.pRight.b * vMax.y,	cv.pRight.c * vMax.z );

		if( ((vLeftMin.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vLeftMin.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vLeftMin.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vLeftMin.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vRightMin.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vRightMin.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vRightMax.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vRightMax.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vRightMin.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vRightMin.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vRightMax.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vRightMax.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		D3DXVECTOR3 vTopMin(	cv.pTop.a * vMin.x,		cv.pTop.b * vMin.y,		cv.pTop.c * vMin.z );
		D3DXVECTOR3 vTopMax(	cv.pTop.a * vMax.x,		cv.pTop.b * vMax.y,		cv.pTop.c * vMax.z );
		D3DXVECTOR3 vBottomMin(	cv.pBottom.a * vMin.x,	cv.pBottom.b * vMin.y,	cv.pBottom.c * vMin.z );
		D3DXVECTOR3 vBottomMax(	cv.pBottom.a * vMax.x,	cv.pBottom.b * vMax.y,	cv.pBottom.c * vMax.z );

		if( ((vTopMin.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vTopMin.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vTopMax.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vTopMax.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vTopMin.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vTopMin.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vTopMax.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vTopMax.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vBottomMin.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vBottomMin.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vBottomMax.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vBottomMax.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vBottomMin.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vBottomMin.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vBottomMax.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vBottomMax.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		//if ( g_bCheckAddPlane )
		//{
		//	for ( DWORD i=0; i<4; ++i )
		//	{
		//		D3DXVECTOR3 vDTMin(	cv.pDetail[i].a * vMin.x, cv.pDetail[i].b * vMin.y, cv.pDetail[i].c * vMin.z );
		//		D3DXVECTOR3 vDTMax(	cv.pDetail[i].a * vMax.x, cv.pDetail[i].b * vMax.y, cv.pDetail[i].c * vMax.z );

		//		if( ((vDTMin.x + vDTMax.y + vDTMin.z + cv.pDetail[i].d) > -0.01f) &&	
		//			((vDTMin.x + vDTMax.y + vDTMax.z + cv.pDetail[i].d) > -0.01f) &&	
		//			((vDTMax.x + vDTMax.y + vDTMax.z + cv.pDetail[i].d) > -0.01f) &&	
		//			((vDTMax.x + vDTMax.y + vDTMin.z + cv.pDetail[i].d) > -0.01f) &&	

		//			((vDTMin.x + vDTMin.y + vDTMin.z + cv.pDetail[i].d) > -0.01f) &&	
		//			((vDTMin.x + vDTMin.y + vDTMax.z + cv.pDetail[i].d) > -0.01f) &&
		//			((vDTMax.x + vDTMin.y + vDTMax.z + cv.pDetail[i].d) > -0.01f) &&
		//			((vDTMax.x + vDTMin.y + vDTMin.z + cv.pDetail[i].d) > -0.01f) )
		//			return FALSE;
		//	}
		//}

		return TRUE;
	}

	BOOL IsCollisionVolumeOrtho( const CLIPVOLUME& cv, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		//if( !bCollisionCheck ) return TRUE;

		D3DXVECTOR3 vLeftMin(	cv.pLeft.a * vMin.x,	cv.pLeft.b * vMin.y,	cv.pLeft.c * vMin.z );
		D3DXVECTOR3 vLeftMax(	cv.pLeft.a * vMax.x,	cv.pLeft.b * vMax.y,	cv.pLeft.c * vMax.z );
		D3DXVECTOR3 vRightMin(	cv.pRight.a * vMin.x,	cv.pRight.b * vMin.y,	cv.pRight.c * vMin.z );
		D3DXVECTOR3 vRightMax(	cv.pRight.a * vMax.x,	cv.pRight.b * vMax.y,	cv.pRight.c * vMax.z );

		if( ((vLeftMin.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vLeftMin.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vLeftMin.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vLeftMin.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vRightMin.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vRightMin.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vRightMax.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vRightMax.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vRightMin.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vRightMin.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vRightMax.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vRightMax.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		D3DXVECTOR3 vTopMin(	cv.pTop.a * vMin.x,		cv.pTop.b * vMin.y,		cv.pTop.c * vMin.z );
		D3DXVECTOR3 vTopMax(	cv.pTop.a * vMax.x,		cv.pTop.b * vMax.y,		cv.pTop.c * vMax.z );
		D3DXVECTOR3 vBottomMin(	cv.pBottom.a * vMin.x,	cv.pBottom.b * vMin.y,	cv.pBottom.c * vMin.z );
		D3DXVECTOR3 vBottomMax(	cv.pBottom.a * vMax.x,	cv.pBottom.b * vMax.y,	cv.pBottom.c * vMax.z );

		if( ((vTopMin.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vTopMin.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vTopMax.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vTopMax.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vTopMin.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vTopMin.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vTopMax.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vTopMax.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vBottomMin.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vBottomMin.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vBottomMax.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vBottomMax.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vBottomMin.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vBottomMin.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vBottomMax.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vBottomMax.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		return TRUE;
	}

	BOOL IsCollisionAABBToAABB(
        const D3DXVECTOR3 &vAMax, const D3DXVECTOR3 &vAMin,
		const D3DXVECTOR3 &vBMax, const D3DXVECTOR3 &vBMin )
	{
		if ( vAMax.x<vBMin.x && vAMin.x<vBMin.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.x>vBMax.x && vAMin.x>vBMax.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.y<vBMin.y && vAMin.y<vBMin.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.y>vBMax.y && vAMin.y>vBMax.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.z<vBMin.z && vAMin.z<vBMin.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.z>vBMax.z && vAMin.z>vBMax.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		return TRUE;
	}

	BOOL IsCollisionLineToAABB(
        const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd,
		const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin)
	{
		D3DXVECTOR3 vP1(vStart);
		D3DXVECTOR3 vP2(vEnd);
		float fRatio;
		D3DXVECTOR3 vCullPos;

		//	Note : X,min
		//		N(-1,0,0) : -X + X,min = 0
		//		X < X,min is front
		//
		if ( vP1.x<vMin.x && vP2.x<vMin.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.x<vMin.x || vP2.x<vMin.x )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.x + vMin.x ) / ( -vP1.x + vP2.x );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.x < vMin.x )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : X,max
		//		N(1,0,0) : X - X,max = 0
		//		X > X,max is front
		//
		if ( vP1.x>vMax.x && vP2.x>vMax.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.x>vMax.x || vP2.x>vMax.x )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( vP1.x - vMax.x ) / ( vP1.x - vP2.x );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.x > vMax.x )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Y,min
		//		N(0,-1,0) : -Y + Y,min = 0
		//		Y < Y,min is front
		//
		if ( vP1.y<vMin.y && vP2.y<vMin.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.y<vMin.y || vP2.y<vMin.y )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.y + vMin.y ) / ( -vP1.y + vP2.y );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.y < vMin.y )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Y,max
		//		N(0,1,0) : Y - Y,max = 0
		//		Y > Y,max is front
		//
		if ( vP1.y>vMax.y && vP2.y>vMax.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.y>vMax.y || vP2.y>vMax.y )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( vP1.y - vMax.y ) / ( vP1.y - vP2.y );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.y > vMax.y )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Z,min
		//		N(0,0,-1) : -Z + Z,min = 0
		//		Z < Z,min is front
		//
		if ( vP1.z<vMin.z && vP2.z<vMin.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.z<vMin.z || vP2.z<vMin.z )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.z + vMin.z ) / ( -vP1.z + vP2.z );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.z < vMin.z )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Z,max
		//		N(0,0,-1) : Z - Z,max = 0
		//		Z > Z,max is front
		//
		if ( vP1.z > vMax.z && vP2.z > vMax.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		//	Note : 충돌 되었음.
		return TRUE;
	}

	BOOL IsCollisionLineToSphere( 
		const D3DXVECTOR3& vStart, 
		const D3DXVECTOR3& vEnd, 
		const D3DXVECTOR3& vCenter, 
		const float fLength, 
		D3DXVECTOR3* pCollision )
	{
		// Note : 삼각형을 만들기 위한 3번째의 점을 찾는다.
		D3DXVECTOR3 vThree;
		D3DXVECTOR3 vLine1 = vStart - vEnd;
		const D3DXVECTOR3 vLine2 = vStart - vCenter;

		if( ((vLine1.x + 0.01f) >= vLine2.x) && (vLine1.x <= (vLine2.x + 0.01f)) && 
			((vLine1.y + 0.01f) >= vLine2.y) && (vLine1.y <= (vLine2.y + 0.01f)) && 
			((vLine1.z + 0.01f) >= vLine2.z) && (vLine1.z <= (vLine2.z + 0.01f)) )
		{
			return TRUE;
		}

		// 거리로 1차적인 체크를 한다.
		float fLength1 = D3DXVec3Length( &vLine1 );
		float fLength2 = D3DXVec3Length( &vLine2 );
		if( fLength1 < (fLength2-fLength) )
		{
			return FALSE;
		}
	
		//D3DXVec3Cross( &vThree, &vLine1, &vLine2 );
		//vThree += vEnd;
		vThree = vStart + D3DXVECTOR3(0.f,1.f,0.f);

		// Note : 삼각형을 만든다.
		D3DXPLANE sPlane;
		D3DXPlaneFromPoints( &sPlane, &vStart, &vEnd, &vThree );

		// Note : 원점과의 거리를 구한다.
		float fTempDis = (sPlane.a*vCenter.x) + (sPlane.b*vCenter.y) + (sPlane.c*vCenter.z) + sPlane.d;
		fTempDis = fabsf( fTempDis );

		// Note : 체크를 한다.
		if( fTempDis > fLength )
		{
			return FALSE;
		}
		else
		{
			if( pCollision )
			{
				D3DXVec3Normalize( &vLine1, &vLine1 );

				float fLengthTEMP;
				D3DXVECTOR3 vDir;
				D3DXVECTOR3 vP1 = vCenter + D3DXVECTOR3(sPlane.a,sPlane.b,sPlane.b)*fTempDis;
				D3DXVECTOR3 vP2 = vP1 + vLine1*fLength;
				for( DWORD i=0; i<5; ++i )
				{
					*pCollision = (vP1 + vP2) * 0.5f;
					vDir = *pCollision - vCenter;
					fLengthTEMP = D3DXVec3Length( &vDir );

					// 기준치보다 클경우, 작을경우
					if( fLengthTEMP > fLength )
					{
						vP2 = *pCollision;
					}
					else
					{
						vP1 = *pCollision;
					}
				}
				*pCollision = vP2 + vLine1*0.03125f;
			}
			return TRUE;
		}
	}

    BOOL IsCollisionLineToSphere_Limit( 
		const D3DXVECTOR3& vStart, 
		const D3DXVECTOR3& vEnd, 
		const D3DXVECTOR3& vCenter, 
		const float fLength )
	{
		// Note : 삼각형을 만들기 위한 3번째의 점을 찾는다.
		D3DXVECTOR3 vThree;
		D3DXVECTOR3 vLine1 = vStart - vEnd;
		D3DXVECTOR3 vLine2 = vStart - vCenter;

		if( ((vLine1.x + 0.01f) >= vLine2.x) && (vLine1.x <= (vLine2.x + 0.01f)) && 
			((vLine1.y + 0.01f) >= vLine2.y) && (vLine1.y <= (vLine2.y + 0.01f)) && 
			((vLine1.z + 0.01f) >= vLine2.z) && (vLine1.z <= (vLine2.z + 0.01f)) )
		{
			return TRUE;
		}

		D3DXPLANE sPlane;
		{
			vThree = vStart + D3DXVECTOR3(0.f,1.f,0.f);

			// Note : 삼각형을 만든다.
			D3DXPlaneFromPoints( &sPlane, &vStart, &vEnd, &vThree );

			// Note : 원점과의 거리를 구한다.
			float fTempDis = (sPlane.a*vCenter.x) + (sPlane.b*vCenter.y) + (sPlane.c*vCenter.z) + sPlane.d;
			fTempDis = fabsf( fTempDis );

			// Note : 체크를 한다.
			if( fTempDis > fLength )
			{
				return FALSE;
			}
		}

		{
			vThree = vStart + D3DXVECTOR3(0.f,0.f,1.f);

			D3DXPlaneFromPoints( &sPlane, &vStart, &vEnd, &vThree );

			// Note : 원점과의 거리를 구한다.
			float fTempDis = (sPlane.a*vCenter.x) + (sPlane.b*vCenter.y) + (sPlane.c*vCenter.z) + sPlane.d;
			fTempDis = fabsf( fTempDis );

			// Note : 체크를 한다.
			if( fTempDis > fLength )
			{
				return FALSE;
			}
		}

		// Note : vEnd를 넘어갔다.
		//vLine1 = vStart - vEnd;
		vLine2 = vCenter - vEnd;
		float fDot = D3DXVec3Dot( &vLine1, &vLine2 );
		if( fDot < 0.f )
		{
			return FALSE;
		}

		// Note : vStart를 넘어갔다.
		vLine1 = vEnd - vStart;
		vLine2 = vCenter - vStart;
		fDot = D3DXVec3Dot( &vLine1, &vLine2 );
		if( fDot < 0.f )
		{
			return FALSE;
		}

		return TRUE;
	}

	D3DXVECTOR3 ClosestPointOnLine ( D3DXVECTOR3 *pvA, D3DXVECTOR3 *pvB, D3DXVECTOR3 vPoint )
	{
		D3DXVECTOR3 vVecter1(vPoint - *pvA);
		D3DXVECTOR3 vVecter2(*pvB - *pvA);
		float fDistant = D3DXVec3Length ( &vVecter2 );
		D3DXVec3Normalize ( &vVecter2, &vVecter2 );

		float fDot = D3DXVec3Dot ( &vVecter2, &vVecter1 );

		if ( fDot <= 0 )			return *pvA;
		if ( fDot >= fDistant )		return *pvB;

		D3DXVECTOR3 vVecter3(vVecter2 * fDot);

		return *pvA + vVecter3;
	}

	BOOL IsSpherePointCollision ( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCenter, const float fRadius )
	{
		return D3DXVec3Length ( &D3DXVECTOR3(vPos-vCenter) ) < fRadius;
	}

	//BOOL IsSpherePlolygonCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
	//							const D3DXVECTOR3 &vCenter, const float fRadius )
	//{
	//	//	Note : 노말 값 구하기.
	//	//
	//	D3DXVECTOR3 v1(*pTri1 - *pTri0);
 //       D3DXVECTOR3 v2(*pTri2 - *pTri0);
 //       
 //       D3DXVECTOR3 vNormal;
	//	D3DXVec3Cross ( &vNormal, &v1, &v2 );
	//	D3DXVec3Normalize ( &vNormal, &vNormal );

	//	float fDistance = 0.0f;


	//	//	Note : 평면 방정식.
	//	//
	//	D3DXPLANE Plane(
 //           vNormal.x,
 //           vNormal.y,
 //           vNormal.z,
 //           -(vNormal.x * pTri0->x + vNormal.y * pTri0->y + vNormal.z * pTri0->z));

	//	//	Note : 평면과 구 사이의 거리 계산.
	//	//		Ax + Bx + Cz + d = 0  :  x,y,z (pTri0)
	//	//
	//	fDistance = Plane.a * vCenter.x + Plane.b * vCenter.y + Plane.c * vCenter.z + Plane.d;

	//	//	Note : 충돌.
	//	//
	//	if ( fDistance < fRadius && fDistance > -fRadius )
	//	{
	//		//	Note : vCenter와 평면간에 가장 가까운 지점을 찾음.
	//		//
	//		D3DXVECTOR3 vPosition(vCenter - vNormal*fDistance);
	//		
	//		//	Note : 삼각형 영역 내에 있는지 검사.
	//		//
	//		BOOL bInside = FALSE;
	//		
	//		//	Note : X가 최대인 경우.
	//		if ( (fabs(Plane.a)>=fabs(Plane.b)) && (fabs(Plane.a)>=fabs(Plane.c)) )
	//			bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, FALSE, TRUE, TRUE );

	//		//	Note : Y가 최대인 경우.
	//		if ( (fabs(Plane.b)>=fabs(Plane.a)) && (fabs(Plane.b)>=fabs(Plane.c)) )
	//			bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, TRUE, FALSE, TRUE );
	//		
	//		//	Note : Z가 최대인 경우.
	//		if ( (fabs(Plane.c)>=fabs(Plane.a)) && (fabs(Plane.c)>=fabs(Plane.b)) )
	//			bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, TRUE, TRUE, FALSE );

	//		return bInside;
	//	}
	//	//	Note : Back 에 있음.
	//	//
	//	else if ( fDistance < fRadius )
	//	{
	//		//D3DXVECTOR3 vClosestP = ClosestPointOnLine ( pTri0, pTri1, vCenter );
	//		//vClosestP = vClosestP-vCenter;
 //           D3DXVECTOR3 vClosestP(ClosestPointOnLine(pTri0, pTri1, vCenter) - vCenter);
	//		float fDist = D3DXVec3Length ( &vClosestP );
	//		if ( fDist < fRadius )
 //               return TRUE;
	//	}

	//	return FALSE;
	//}

	HRESULT CalculateBoundingBox ( const D3DXMATRIX *const pmatComb, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin,
								PBYTE pbPoints, UINT cVertices, DWORD dwFVF )
	{
		GASSERT(pbPoints);

		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		PBYTE pbCur;
		D3DXVECTOR3 vDist;
		D3DXVECTOR3 *pvCur;
		DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );

		UINT iPoint;
		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
		{
			pvCur = (D3DXVECTOR3*)pbCur;
			D3DXVec3TransformCoord ( &vDist, pvCur, pmatComb );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;
		}

		return S_OK;
	}

	bool CalculatePlane2Line ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, D3DXVECTOR3 &vCol )
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

	void TransformAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXMATRIX &matTrans )
	{
		D3DXVECTOR3 VertBox[8];

		VertBox[0] = D3DXVECTOR3(vMin.x,vMax.y,vMin.z);
		VertBox[1] = D3DXVECTOR3(vMin.x,vMax.y,vMax.z);
		VertBox[2] = D3DXVECTOR3(vMax.x,vMax.y,vMax.z);
		VertBox[3] = D3DXVECTOR3(vMax.x,vMax.y,vMin.z);

		VertBox[4] = D3DXVECTOR3(vMin.x,vMin.y,vMin.z);
		VertBox[5] = D3DXVECTOR3(vMin.x,vMin.y,vMax.z);
		VertBox[6] = D3DXVECTOR3(vMax.x,vMin.y,vMax.z);
		VertBox[7] = D3DXVECTOR3(vMax.x,vMin.y,vMin.z);

		CalculateBoundingBox ( &matTrans, vMax, vMin, (PBYTE)VertBox, 8, D3DFVF_XYZ );
	}

	//DWORD dwLeafCount = 0;
	//DWORD dwNodeCount = 0;
	void MakeAABBTree ( const POBJAABB *const ppArray, const DWORD dwArraySize,
		const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin, POBJAABBNODE &pTreeNode, BOOL bQUAD )
	{
		if ( dwArraySize == 1 )
		{
			D3DXVECTOR3 vObjMax, vObjMin;
			ppArray[0]->GetAABBSize ( vObjMax, vObjMin );

			pTreeNode->vMax = vObjMax;
			pTreeNode->vMin = vObjMin;

			//dwLeafCount++;
			pTreeNode->pObject = ppArray[0];
			return;
		}

		float SizeX = vMax.x - vMin.x;
		float SizeY = vMax.y - vMin.y;
		float SizeZ = vMax.z - vMin.z;

		D3DXVECTOR3 vDivMax(vMax);
		D3DXVECTOR3 vDivMin(vMin);

		if ( bQUAD )
		{
			if ( SizeX >= SizeZ )		vDivMin.x = vMin.x + SizeX/2;
			else						vDivMin.z = vMin.z + SizeZ/2;
		}
		else
		{
			if ( SizeX > SizeY && SizeX > SizeZ )	vDivMin.x = vMin.x + SizeX/2;
			else if ( SizeY > SizeZ )				vDivMin.y = vMin.y + SizeY/2;
			else									vDivMin.z = vMin.z + SizeZ/2;
		}

		DWORD dwLeftCount = 0, dwRightCount = 0;
		POBJAABB* pLeftArray = new POBJAABB[dwArraySize];
		POBJAABB* pRightArray = new POBJAABB[dwArraySize];

		//	Note : 분할된 공간으로 요소 분리.
		DWORD i;
		for ( i=0; i<dwArraySize; i++ )
		{
			if ( ppArray[i]->IsDetectDivision (vDivMax,vDivMin) )
				pLeftArray[dwLeftCount++] = ppArray[i];
			else
				pRightArray[dwRightCount++] = ppArray[i];
		}

		//	Note : 무작위로 요소 분리.
		if ( dwLeftCount == 0 || dwRightCount == 0 )
		{
			DWORD dwDivCount = dwArraySize / 2;
		
			dwLeftCount = 0;
			dwRightCount = 0;

			for ( i=0; i<dwDivCount; i++ )
				pLeftArray[dwLeftCount++] = ppArray[i];

			for ( i=dwDivCount; i<dwArraySize; i++ )
				pRightArray[dwRightCount++] = ppArray[i];
		}

		D3DXVECTOR3 vNodeMax, vNodeMin;

		//	Note : 왼쪽 자식 노드 생성.
		//
		D3DXVECTOR3 vCMax(D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX));
        D3DXVECTOR3 vCMin(D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX));		

		for ( i=0; i<dwLeftCount; ++i )
		{
			pLeftArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
		}

		pTreeNode->pLeftChild = new OBJAABBNode;
		pTreeNode->pLeftChild->vMax = vCMax;
		pTreeNode->pLeftChild->vMin = vCMin;
		MakeAABBTree ( pLeftArray, dwLeftCount, vCMax, vCMin, pTreeNode->pLeftChild, bQUAD );

		//	Note : 오른쪽 자식 노드 생성.
		//
		vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( i=0; i<dwRightCount; i++ )
		{
			pRightArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
		}

		pTreeNode->pRightChild = new OBJAABBNode;
		pTreeNode->pRightChild->vMax = vCMax;
		pTreeNode->pRightChild->vMin = vCMin;
		MakeAABBTree ( pRightArray, dwRightCount, vCMax, vCMin, pTreeNode->pRightChild, bQUAD );

		SAFE_DELETE_ARRAY(pRightArray);
		SAFE_DELETE_ARRAY(pLeftArray);
	}

	void MakeAABBTree ( POBJAABBNODE &pTreeHead, POBJAABB pListHead, BOOL bQUAD )
	{
		SAFE_DELETE(pTreeHead);

		//dwLeafCount = 0;
		DWORD dwNodeCount = 0;

		// 갯수 파악.
        POBJAABB pNodeCur = pListHead;
		while ( pNodeCur )
		{
			++dwNodeCount;
			pNodeCur = pNodeCur->GetNext();
		}

		if ( dwNodeCount == 0 )
			return;

		//	Note : 포인터 배열에 저장후 정렬수행.
		//
		POBJAABB *ppArray = new POBJAABB[dwNodeCount];

		dwNodeCount = 0;
		pNodeCur = pListHead;
		while ( pNodeCur )
		{
			ppArray[dwNodeCount++] = pNodeCur;
			pNodeCur = pNodeCur->GetNext();
		}

		D3DXVECTOR3 vMax(D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX));
		D3DXVECTOR3 vMin(D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX));

		D3DXVECTOR3 vNodeMax, vNodeMin;
		for ( DWORD i=0; i<dwNodeCount; i++ )
		{
			ppArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vMax.x < vNodeMax.x )	vMax.x = vNodeMax.x;
			if ( vMax.y < vNodeMax.y )	vMax.y = vNodeMax.y;
			if ( vMax.z < vNodeMax.z )	vMax.z = vNodeMax.z;

			if ( vMin.x > vNodeMin.x )	vMin.x = vNodeMin.x;
			if ( vMin.y > vNodeMin.y )	vMin.y = vNodeMin.y;
			if ( vMin.z > vNodeMin.z )	vMin.z = vNodeMin.z;
		}

		pTreeHead = new OBJAABBNode;
		pTreeHead->vMax = vMax;
		pTreeHead->vMin = vMin;
		MakeAABBTree ( ppArray, dwNodeCount, vMax, vMin, pTreeHead, bQUAD );

		SAFE_DELETE_ARRAY(ppArray);
	}

//	DWORD dwRendAABB = 0;
	HRESULT RenderAABBTree ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB )
	{
		HRESULT hr;
		if ( !pNode )
            return S_OK;

         if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		    return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )
            return S_OK;

        if ( bRendAABB )
            EDITMESHS::RENDERAABB ( pd3dDevice, pNode->vMax, pNode->vMin );	//	테스트용.

		if ( pNode->pObject )
		{
//			dwRendAABB++;
			hr = pNode->pObject->Render ( pd3dDevice, pCV );
			return hr;
		}

		hr = RenderAABBTree ( pd3dDevice, pCV, pNode->pLeftChild, bRendAABB );
		if ( FAILED(hr) )
            return E_FAIL;

		hr = RenderAABBTree ( pd3dDevice, pCV, pNode->pRightChild, bRendAABB );
		if ( FAILED(hr) )
            return E_FAIL;

		return S_OK;
	}

	HRESULT RenderAABBTreeReflect ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, float fReflectHeight )
	{
		HRESULT hr;
		if ( !pNode )
			return S_OK;

		if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
			return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )
			return S_OK;

		if ( pNode->pObject )
		{
//			dwRendAABB++;
			hr = pNode->pObject->Render ( pd3dDevice, pCV, TRUE, fReflectHeight );
			return hr;
		}

		hr = RenderAABBTreeReflect ( pd3dDevice, pCV, pNode->pLeftChild, fReflectHeight );
		if ( FAILED(hr) )
			return E_FAIL;

		hr = RenderAABBTreeReflect ( pd3dDevice, pCV, pNode->pRightChild, fReflectHeight );
		if ( FAILED(hr) )
			return E_FAIL;

		return S_OK;
	}

	HRESULT RenderAABBTreeFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB, DxLandMan* pLandMan )
	{
		HRESULT hr;
		if ( !pNode )
            return S_OK;

         if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		    return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )
            return S_OK;

        if ( bRendAABB )
            EDITMESHS::RENDERAABB ( pd3dDevice, pNode->vMax, pNode->vMin );	//	테스트용.

		if ( pNode->pObject )
		{
//			dwRendAABB++;
			return pNode->pObject->Render ( NULL, pd3dDevice, NULL, pLandMan );
		}

		hr = RenderAABBTreeFrame ( pd3dDevice, pCV, pNode->pLeftChild, bRendAABB, pLandMan );
		if ( FAILED(hr) )
            return E_FAIL;

		hr = RenderAABBTreeFrame ( pd3dDevice, pCV, pNode->pRightChild, bRendAABB, pLandMan );
		if ( FAILED(hr) )
            return E_FAIL;

		return S_OK;
	}

	POBJAABB IsCollisonPointToOBJAABBTree ( POBJAABBNODE &pTreeNode, const D3DXVECTOR3 &vPos )
	{
		if ( IsWithInPoint ( pTreeNode->vMax, pTreeNode->vMin, vPos ) )
		{
			if ( pTreeNode->pObject )
                return pTreeNode->pObject;

			if ( pTreeNode->pLeftChild )
			{
				POBJAABB pObjAABB = IsCollisonPointToOBJAABBTree ( pTreeNode->pLeftChild, vPos );
				if ( pObjAABB )
                    return pObjAABB;
			}
			
			if ( pTreeNode->pRightChild )
			{
				POBJAABB pObjAABB = IsCollisonPointToOBJAABBTree ( pTreeNode->pRightChild, vPos );
				if ( pObjAABB )
                    return pObjAABB;
			}
		}

		return NULL;
	}

	void MINDETECTAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const float fRate, const float fMinLeng )
	{
		float fLength(0);
		fLength = vMax.x - vMin.x;
		vMax.x += ( fLength*fRate - fLength ) / 2.0f;
		vMin.x -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.x - vMin.x;
		if ( fLength < fMinLeng )
		{
			vMax.x += fMinLeng/2.0f;
			vMin.x -= fMinLeng/2.0f;
		}

		fLength = vMax.y - vMin.y;
		vMax.y += ( fLength*fRate - fLength ) / 2.0f;
		vMin.y -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.y - vMin.y;
		if ( fLength < fMinLeng )
		{
			vMax.y += fMinLeng/2.0f;
			vMin.y -= fMinLeng/2.0f;
		}

		fLength = vMax.z - vMin.z;
		vMax.z += ( fLength*fRate - fLength ) / 2.0f;
		vMin.z -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.z - vMin.z;
		if ( fLength < fMinLeng )
		{
			vMax.z += fMinLeng/2.0f;
			vMin.z -= fMinLeng/2.0f;
		}
	}

	// 같은 포지션인지 확인하도록 한다.
	BOOL IsSamePosition( const D3DXVECTOR3& sV1, const D3DXVECTOR3& sV2 )
	{
		if ( sV1.x+0.001f < sV2.x )		return FALSE;
		if ( sV1.y+0.001f < sV2.y )		return FALSE;
		if ( sV1.z+0.001f < sV2.z )		return FALSE;
		if ( sV1.x > sV2.x+0.001f )		return FALSE;
		if ( sV1.y > sV2.y+0.001f )		return FALSE;
		if ( sV1.z > sV2.z+0.001f )		return FALSE;

		return TRUE;
	}
}

void DxAABBNode::Import( DWORD dwFaceID, const D3DXVECTOR3& vPos1, const D3DXVECTOR3& vPos2, const D3DXVECTOR3& vPos3 )
{
	m_dwFaceID = dwFaceID;
	m_vPosition[0] = vPos1;
	m_vPosition[1] = vPos2;
	m_vPosition[2] = vPos3;
}

DWORD DxAABBNode::GetFaceID() const
{
	return m_dwFaceID;
}

BOOL DxAABBNode::IsEnableFace() const
{
	return m_dwFaceID != AABB_NONINDEX ? TRUE : FALSE;
}

const D3DXVECTOR3* DxAABBNode::GetPosition( DWORD dwIndex ) const
{
	GASSERT( dwIndex<TRIANGLE_VERTEX_NUM );

	return &m_vPosition[dwIndex];
}

BOOL DxAABBNode::IsCollision ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 ) const 
{
	float fRatio;
	D3DXVECTOR3 vCollision;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<fMinX && vP2.x<fMinX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<fMinX || vP2.x<fMinX )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + fMinX ) / ( -vP1.x + vP2.x );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < fMinX )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>fMaxX && vP2.x>fMaxX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>fMaxX || vP2.x>fMaxX )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - fMaxX ) / ( vP1.x - vP2.x );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > fMaxX )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<fMinY && vP2.y<fMinY )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<fMinY || vP2.y<fMinY )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + fMinY ) / ( -vP1.y + vP2.y );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < fMinY )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>fMaxY && vP2.y>fMaxY )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>fMaxY || vP2.y>fMaxY )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - fMaxY ) / ( vP1.y - vP2.y );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > fMaxY )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<fMinZ && vP2.z<fMinZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<fMinZ || vP2.z<fMinZ )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + fMinZ ) / ( -vP1.z + vP2.z );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < fMinZ )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > fMaxZ && vP2.z > fMaxZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	//	Note : 충돌 발생.
	return TRUE;
}

BOOL DxAABBNode::IsCollisionExceptY ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 ) const  // 주병진 추가 메소드
{
	D3DXVECTOR3 vCollision;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<fMinX && vP2.x<fMinX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.


	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>fMaxX && vP2.x>fMaxX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.


	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<fMinZ && vP2.z<fMinZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > fMaxZ && vP2.z > fMaxZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.


	//	Note : 충돌 발생.
	return TRUE;
}

BOOL DxAABBNode::IsCollisionVolume ( CLIPVOLUME & cv ) const
{
	return COLLISION::IsCollisionVolume ( cv, vMax, vMin );

	//if (((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//if (((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//if (((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//else if (((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//if (((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//else if (((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
	//	((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
	//	((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
	//	((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

	//	((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
	//	((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
	//	((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
	//	((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
	//	return FALSE;

	//return TRUE;
}

BOOL DxAABBNode::SaveFile ( sc::SerialFile &SFile )
{	
	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();

	SFile << fMaxX;
	SFile << fMaxY;
	SFile << fMaxZ;

	SFile << fMinX;
	SFile << fMinY;
	SFile << fMinZ;

	SFile << m_dwFaceID;

	SFile << m_vPosition[0];
	SFile << m_vPosition[1];
	SFile << m_vPosition[2];

	if ( pLeftChild )
	{
		SFile << BOOL ( TRUE );
		pLeftChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pRightChild )
	{
		SFile << BOOL ( TRUE );
		pRightChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	SFile.EndBlock();

	return TRUE;
}

BOOL DxAABBNode::LoadFile ( sc::SerialFile &SFile )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	BOOL	bExist = FALSE;

	SFile >> fMaxX;
	SFile >> fMaxY;
	SFile >> fMaxZ;

	SFile >> fMinX;
	SFile >> fMinY;
	SFile >> fMinZ;

	// [shhan][2014.03.17]
	// 유격이 필요한데 사라졌다.
	// 그래서 추가됨.
	{
		fMaxX += 0.0001f;
		fMaxY += 0.0001f;
		fMaxZ += 0.0001f;

		fMinX -= 0.0001f;
		fMinY -= 0.0001f;
		fMinZ -= 0.0001f;
	}

	SFile >> m_dwFaceID;

	SFile >> m_vPosition[0];
	SFile >> m_vPosition[1];
	SFile >> m_vPosition[2];

	SFile >> bExist;
	if ( bExist )
	{
		pLeftChild = new DxAABBNode();
		pLeftChild->LoadFile ( SFile );
	}	

	SFile >> bExist;
	if ( bExist )
	{
		pRightChild = new DxAABBNode();
		pRightChild->LoadFile ( SFile );
	}

	return TRUE;
}

BOOL DxAABBNode::LoadFile_PREV ( sc::SerialFile &SFile )
{
	BOOL	bExist = FALSE;

	SFile >> fMaxX;
	SFile >> fMaxY;
	SFile >> fMaxZ;

	SFile >> fMinX;
	SFile >> fMinY;
	SFile >> fMinZ;

	// [shhan][2014.03.17]
	// 유격이 필요한데 사라졌다.
	// 그래서 추가됨.
	{
		fMaxX += 0.0001f;
		fMaxY += 0.0001f;
		fMaxZ += 0.0001f;

		fMinX -= 0.0001f;
		fMinY -= 0.0001f;
		fMinZ -= 0.0001f;
	}

	SFile >> m_dwFaceID;

	SFile >> bExist;
	if ( bExist )
	{
		pLeftChild = new DxAABBNode();
		pLeftChild->LoadFile_PREV ( SFile );
	}	

	SFile >> bExist;
	if ( bExist )
	{
		pRightChild = new DxAABBNode();
		pRightChild->LoadFile_PREV ( SFile );
	}

	return TRUE;
}
