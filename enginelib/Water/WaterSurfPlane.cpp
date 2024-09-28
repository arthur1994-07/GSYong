#include "pch.h"

//#include "../CoreCommon/Math/Random.h"

//#include "../HeightField/NSHfMain.h"

#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Math/Random.h"

#include "../DxTools/RENDERPARAM.h"
#include "../DxLand/DxSetLandMan.h"

#include "./WaterSurfPlane.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
//					S	U	R	F		P	A	R	T		D	A	T	A					
//---------------------------------------------------------------------------------------------
WaterSurfPlane_Part::SURF_PART_DATA::SURF_PART_DATA()
	: m_fTime(0.f)
	, m_bPlay(FALSE)
	, m_fAlpha(0.f)
	, m_fSurfVelocity(3.f)
	, m_fSurfSleepTime(0.f)
{
}

void WaterSurfPlane_Part::SURF_PART_DATA::Create()
{
	m_sTexUV[0] = D3DXVECTOR2(0.f,0.f);
	m_sTexUV[1] = D3DXVECTOR2(1.f,0.f);
	m_sTexUV[2] = D3DXVECTOR2(0.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
	m_sTexUV[3] = D3DXVECTOR2(1.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
	m_sTexUV[4] = D3DXVECTOR2(0.f,0.5f);
	m_sTexUV[5] = D3DXVECTOR2(1.f,0.5f);
}

void WaterSurfPlane_Part::SURF_PART_DATA::Play( float fSurfSleepTime )
{
	m_bPlay				= TRUE;
	m_fTime				= 0.f;
	m_fSurfSleepTime	= fSurfSleepTime;
}

void WaterSurfPlane_Part::SURF_PART_DATA::FrameMove( const WATERSURF_PROPERTY& sProp, 
													float fSurfAcceleration, 
													float fElapsedTime )
{
	if( !m_bPlay )
	{
		return;
	}

	m_fSurfSleepTime -= fElapsedTime;
	if( m_fSurfSleepTime > 0.f )
	{
		return;
	}

	m_fTime += fElapsedTime;

	// Note : 물살 이동속도변화.
	m_fSurfVelocity -= fSurfAcceleration*fElapsedTime;

	// Note : 알파값 변화.
	if( m_fTime > sProp.m_fAlphaSubtractionTime )
	{
		m_fAlpha -= sProp.m_fAlphaSubtraction * fElapsedTime;
		if( m_fAlpha < 0.f )	m_fAlpha = 0.f;
	}
	else
	{
		m_fAlpha += sProp.m_fAlphaAddition * fElapsedTime;
		if( m_fAlpha > 1.f )	m_fAlpha = 1.f;
	}

	// Note : 각 폴리곤들에 작업함.
    float fSurfVelocity = m_fSurfVelocity*fElapsedTime;
	for( int i=0; i<PLANE_IN_VERTEX_NUM; ++i )
		m_sTexUV[i].y += fSurfVelocity;

	if( m_sTexUV[0].y < 0.f || m_sTexUV[PLANE_IN_VERTEX_NUM-2].y < 0.f )
	{
		m_fSurfVelocity	 = sProp.m_fSurfVelocityINIT * 0.01f;

		m_sTexUV[0] = D3DXVECTOR2(0.f,0.f);
		m_sTexUV[1] = D3DXVECTOR2(1.f,0.f);
		m_sTexUV[2] = D3DXVECTOR2(0.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
		m_sTexUV[3] = D3DXVECTOR2(1.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
		m_sTexUV[4] = D3DXVECTOR2(0.f,0.5f);
		m_sTexUV[5] = D3DXVECTOR2(1.f,0.5f);

		m_bPlay = FALSE;
	}
	else if( m_sTexUV[0].y > 1.f || m_sTexUV[PLANE_IN_VERTEX_NUM-2].y > 1.f )
	{
		m_fSurfVelocity	 = 0.f;

		m_sTexUV[0] = D3DXVECTOR2(0.f,0.5f+0.f);
		m_sTexUV[1] = D3DXVECTOR2(1.f,0.5f+0.f);
		m_sTexUV[2] = D3DXVECTOR2(0.f,0.5f+0.25f + 0.25f*VERTEX_MIDDLE_RATE);
		m_sTexUV[3] = D3DXVECTOR2(1.f,0.5f+0.25f + 0.25f*VERTEX_MIDDLE_RATE);
		m_sTexUV[4] = D3DXVECTOR2(0.f,0.5f+0.5f);
		m_sTexUV[5] = D3DXVECTOR2(1.f,0.5f+0.5f);
	}
}

void WaterSurfPlane_Part::SURF_PART_DATA::ResetData( float fSurfVelocityINIT )
{
	m_bPlay				= TRUE;
	m_fTime = 0.f;
	m_fAlpha			= 0.f;
	m_fSurfVelocity		= fSurfVelocityINIT * 0.01f;

	m_sTexUV[0] = D3DXVECTOR2(0.f,0.f);
	m_sTexUV[1] = D3DXVECTOR2(1.f,0.f);
	m_sTexUV[2] = D3DXVECTOR2(0.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
	m_sTexUV[3] = D3DXVECTOR2(1.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
	m_sTexUV[4] = D3DXVECTOR2(0.f,0.5f);
	m_sTexUV[5] = D3DXVECTOR2(1.f,0.5f);
}

//---------------------------------------------------------------------------------------------
//		W	a	t	e	r		S	u	r	f		P	l	a	n	e		P	a	r	t		
//---------------------------------------------------------------------------------------------
WaterSurfPlane_Part::WaterSurfPlane_Part()
	: m_pSurfPartData(NULL)
	, m_nPlaneNUM(0)
	, m_nPlaneStart(0)
	, m_nPlaneEnd(0)
	, m_fSurfAcceleration(1.f)
	, m_fSurfSleepTime(3.f)
{
}

WaterSurfPlane_Part::~WaterSurfPlane_Part()
{
	m_nPlaneNUM = 0;
	SAFE_DELETE_ARRAY( m_pSurfPartData );
}

void WaterSurfPlane_Part::Create( const WATERSURF_PROPERTY& sProp, WATERSURFVERTEX* pVertex, int nPlaneNUM )
{
	m_nPlaneNUM = nPlaneNUM;
	m_nPlaneNUM_Div_3 = m_nPlaneNUM/3 + 1;

	SAFE_DELETE_ARRAY( m_pSurfPartData );
	m_pSurfPartData = new SURF_PART_DATA[ m_nPlaneNUM ];
	for( int i=0; i<m_nPlaneNUM; ++i )
	{
		m_pSurfPartData[i].Create();
	}

	ResetData( sProp, TRUE );
}

void WaterSurfPlane_Part::FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime )
{
	// Note : 물살의 잠복기.
	m_fSurfSleepTime -= fElapsedTime;
	if( m_fSurfSleepTime >= 0.f )
	{
		return;
	}

	// Note : FrameMove
	int nPlayEndCount(0);
	for( int i=m_nPlaneStart; i<m_nPlaneEnd; ++i )
	{
		m_pSurfPartData[i].FrameMove( sProp, m_fSurfAcceleration, fElapsedTime );

		if( !m_pSurfPartData[i].IsPlay() )
		{
			++nPlayEndCount;
		}
	}

	// Note : 모든것이 다 플레이 되었다면 슬립타임을 주고 Play 시키도록 다시 셋팅한다.
	if( nPlayEndCount == (m_nPlaneEnd-m_nPlaneStart) )
	{
		Play( sProp, FALSE );
	}
}

void WaterSurfPlane_Part::Render( WATERSURFVERTEX* pVertex, WATERSURFVERTEX*& pLocked, bool& bFirst )
{
    //if( !RENDERPARAM::bRENDER_SURF )
    //    return;

    D3DXVECTOR2* pTexUV(NULL);
	DWORD dwColor(0L);

    // Note : UV 복제
	for( int i=m_nPlaneStart; i<m_nPlaneEnd; ++i )
	{
        UINT nVertexNoBase = PLANE_IN_VERTEX_NUM * i;

		pTexUV = &m_pSurfPartData[i].m_sTexUV[0];

		dwColor = static_cast<DWORD>( m_pSurfPartData[i].m_fAlpha*255.f );
		dwColor = (dwColor<<16) + (dwColor<<8) + dwColor;

		pVertex[nVertexNoBase+0].dwColor = dwColor;
		pVertex[nVertexNoBase+0].vTex = pTexUV[0];

		pVertex[nVertexNoBase+1].dwColor = dwColor;
		pVertex[nVertexNoBase+1].vTex = pTexUV[1];

		pVertex[nVertexNoBase+2].dwColor = dwColor;
		pVertex[nVertexNoBase+2].vTex = pTexUV[2];

		pVertex[nVertexNoBase+3].dwColor = dwColor;
		pVertex[nVertexNoBase+3].vTex = pTexUV[3];

		pVertex[nVertexNoBase+4].dwColor = 0x00000000;
		pVertex[nVertexNoBase+4].vTex = pTexUV[4];

		pVertex[nVertexNoBase+5].dwColor = 0x00000000;
		pVertex[nVertexNoBase+5].vTex = pTexUV[5];

        if( !bFirst )
        {
            WATERSURFVERTEX* pPrev = pLocked - 1;

            // 한방에 뿌릴려고 렌더링 되지 않는 버텍스 추가 5, 0, 0, 1
            memcpy( pLocked++, pPrev, sizeof( WATERSURFVERTEX ) );
            memcpy( pLocked++, &pVertex[nVertexNoBase+0], sizeof( WATERSURFVERTEX ) );
            memcpy( pLocked++, &pVertex[nVertexNoBase+0], sizeof( WATERSURFVERTEX ) );
            memcpy( pLocked++, &pVertex[nVertexNoBase+1], sizeof( WATERSURFVERTEX ) );
        }
        else
        {
            bFirst = false;
        }

        memcpy( pLocked, &pVertex[nVertexNoBase], sizeof( WATERSURFVERTEX ) * PLANE_IN_VERTEX_NUM );
        pLocked += PLANE_IN_VERTEX_NUM;
	}
}

void WaterSurfPlane_Part::ResetData( const WATERSURF_PROPERTY& sProp, BOOL bInit )
{
	m_fSurfAcceleration	= sProp.m_fEdit_SurfAcceleration * 0.01f;

	if( m_pSurfPartData )
	{
		for( int i=0; i<m_nPlaneNUM; ++i )
		{
			m_pSurfPartData[i].ResetData( sProp.m_fSurfVelocityINIT );
		}
	}

	Play( sProp, bInit );
}

void WaterSurfPlane_Part::Play( const WATERSURF_PROPERTY& sProp, BOOL bInit )
{
	m_nPlaneStart = rand()%m_nPlaneNUM_Div_3;
	m_nPlaneEnd = m_nPlaneNUM - (rand()%m_nPlaneNUM_Div_3);

	if( bInit )
	{
		m_fSurfSleepTime = sc::Random::RANDOM_POS()*10.f;	//sProp.m_fSurfSleepTimeINIT*
	}
	else
	{
		m_fSurfSleepTime = sProp.m_fSurfSleepTimeINIT * sc::Random::RANDOM_POS() * 2.0f;
	}

	int nDiff(0);
	int nHalf = m_nPlaneNUM;
	float fAddSleepTime2(0.f);
	for( int i=m_nPlaneStart; i<m_nPlaneEnd; ++i )
	{
		nDiff = i - nHalf;
		nDiff = abs( nDiff );
		m_pSurfPartData[i].Play( sProp.m_fSideLineInterval*nDiff );	
	}
}

//---------------------------------------------------------------------------------------------
//				W	a	t	e	r		S	u	r	f		P	l	a	n	e				
//---------------------------------------------------------------------------------------------
WaterSurfPlane::WaterSurfPlane()
	: m_nPlaneNUM( 0 )
	, m_pVertex(NULL)
{
	m_pWaterSurtPlane = new WaterSurfPlane_Part;
}

WaterSurfPlane::~WaterSurfPlane()
{
	SAFE_DELETE( m_pWaterSurtPlane );

	CleanUp();
}

void WaterSurfPlane::CleanUp()
{
	SAFE_DELETE_ARRAY( m_pVertex );
}

BOOL WaterSurfPlane::Create( DxSetLandMan* pSetLandMan, const WATERSURF_PROPERTY& sProp, const VEC_VECTOR3& vecWaterSurfPOS )
{
	CleanUp();

	m_nPlaneNUM = static_cast<int>( vecWaterSurfPOS.size() );
	if( m_nPlaneNUM <= 1 )		// 점 1개만 있어서는 성립하지 않는다.
	{
		return FALSE;
	}

	int nPrev(0);
	int nNext(0);
	int nCount(0);
	D3DXVECTOR3 vUp(0.f,1.f,0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);
	D3DXVECTOR3 vCross(0.f,0.f,0.f);
	D3DXVECTOR3 vPlaneUp(0.f,0.f,0.f);

	VEC_VECTOR3 vecPlaneUp;
	VEC_VECTOR3 vecCross;
	
	m_pVertex = new WATERSURFVERTEX[ PLANE_IN_VERTEX_NUM * m_nPlaneNUM ];

	for( int i=0; i<m_nPlaneNUM; ++i )
	{
		nCount = 0;
		nPrev = i-1;
		nNext = i+1;
		vPlaneUp = D3DXVECTOR3(0.f,0.f,0.f);

		if( nPrev < 0 )
		{
			// 아무것도 못한다.
		}
		else
		{
			vDir = vecWaterSurfPOS[i] - vecWaterSurfPOS[nPrev];
			D3DXVec3Cross( &vCross, &vDir, &vUp );
			D3DXVec3Normalize( &vCross, &vCross );

			++nCount;
			vPlaneUp += vCross * sProp.m_fPlaneHeight;
		}

		if( nNext >= m_nPlaneNUM )
		{
			// 아무것도 못한다.
		}
		else
		{
			vDir = vecWaterSurfPOS[nNext] - vecWaterSurfPOS[i];
			D3DXVec3Cross( &vCross, &vDir, &vUp );
			D3DXVec3Normalize( &vCross, &vCross );

			++nCount;
			vPlaneUp += vCross * sProp.m_fPlaneHeight;
		}

		if( sProp.m_bPlaneInverse )		// 역방향
		{
			vPlaneUp = -vPlaneUp;
		}

		if( nCount >= 1 )
		{
			vPlaneUp = vPlaneUp / static_cast<float>(nCount);

			D3DXVec3Cross( &vCross, &vPlaneUp, &vUp );
			D3DXVec3Normalize( &vCross, &vCross );
			vCross *= sProp.m_fPlaneWidth;

			m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vPos = vecWaterSurfPOS[i] + vPlaneUp - vCross;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vPos = vecWaterSurfPOS[i] + vPlaneUp + vCross;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos = vecWaterSurfPOS[i] - vPlaneUp*VERTEX_MIDDLE_RATE - vCross;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos = vecWaterSurfPOS[i] - vPlaneUp*VERTEX_MIDDLE_RATE + vCross;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+4].vPos = vecWaterSurfPOS[i] - vPlaneUp - vCross;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+5].vPos = vecWaterSurfPOS[i] - vPlaneUp + vCross;

			m_pVertex[PLANE_IN_VERTEX_NUM*i+0].dwColor = 0xffffffff;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+1].dwColor = 0xffffffff;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+2].dwColor = 0xffffffff;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+3].dwColor = 0xffffffff;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+4].dwColor = 0xff000000;
			m_pVertex[PLANE_IN_VERTEX_NUM*i+5].dwColor = 0xff000000;
			
			m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vTex = D3DXVECTOR2(0.f,0.f);
			m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vTex = D3DXVECTOR2(1.f,0.f);
			m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vTex = D3DXVECTOR2(0.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
			m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vTex = D3DXVECTOR2(1.f,0.25f + 0.25f*VERTEX_MIDDLE_RATE);
			m_pVertex[PLANE_IN_VERTEX_NUM*i+4].vTex = D3DXVECTOR2(0.f,0.5f);
			m_pVertex[PLANE_IN_VERTEX_NUM*i+5].vTex = D3DXVECTOR2(1.f,0.5f);
		}

		vecPlaneUp.push_back( vPlaneUp );
		vecCross.push_back( vCross );
	}

	// Note : 지형에 높이 맞추기.
	ResetPosition( pSetLandMan, (sProp.m_fPlaneHeight+sProp.m_fPlaneWidth) * 0.5f, vecWaterSurfPOS, vecPlaneUp, vecCross );

	// Note : 생성
	m_pWaterSurtPlane->Create( sProp, m_pVertex, m_nPlaneNUM );

	return TRUE;
}

void WaterSurfPlane::ResetData( const WATERSURF_PROPERTY& sProp )
{
	m_pWaterSurtPlane->ResetData( sProp, FALSE );
}

void WaterSurfPlane::AddHeight( float fHeight )
{
	// Note : 지형에 높이 맞추기.
	D3DXVECTOR3 vNor(0.f,0.f,0.f);
	D3DXVECTOR3 vDown(0.f,0.f,0.f);
	for( int i=0; i<m_nPlaneNUM; ++i )
	{
		for( int j=0; j<PLANE_IN_VERTEX_NUM; ++j )
		{
			m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos.y += fHeight;
		}
	}
}

void WaterSurfPlane::SetObjRotate90( const D3DXMATRIX& matRotate )
{
    for( int i=0; i<m_nPlaneNUM; ++i )
	{
		for( int j=0; j<PLANE_IN_VERTEX_NUM; ++j )
		{
			D3DXVec3TransformCoord( &m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos,
                                    &m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos,
                                    &matRotate );
		}
	}
}

void WaterSurfPlane::CountVertexMax( UINT& nVertexMax )
{
    nVertexMax += static_cast< UINT >( PLANE_IN_VERTEX_NUM * m_nPlaneNUM );

    // 한방에 뿌릴려고 렌더링 되지 않는 버텍스 추가
    nVertexMax += static_cast< UINT >( PLANE_LINK_VERTEX * m_nPlaneNUM );
}

void WaterSurfPlane::CountVertexRender( UINT& nVertexMax )
{
    INT nPlaneNum = m_pWaterSurtPlane->GetPlaneEnd() - m_pWaterSurtPlane->GetPlaneStart();
    nVertexMax += static_cast< UINT >( PLANE_IN_VERTEX_NUM * nPlaneNum );

    // 한방에 뿌릴려고 렌더링 되지 않는 버텍스 추가
    nVertexMax += static_cast< UINT >( PLANE_LINK_VERTEX * nPlaneNum );
}

void WaterSurfPlane::ResetPosition( DxSetLandMan* pSetLandMan, float fHeight, const VEC_VECTOR3& vecWaterSurfPOS, const VEC_VECTOR3& vecPlaneUp, const VEC_VECTOR3& vecCross )
{
	D3DXVECTOR3 vNor;
	D3DXVECTOR3 vUp(0.f,0.f,0.f);
	D3DXVECTOR3 vDown(0.f,0.f,0.f);
	for( int i=0; i<m_nPlaneNUM; ++i )
	{
		for( int j=0; j<PLANE_IN_VERTEX_NUM; ++j )
		{
			if ( j == 0 || j == 2 || j == 4 )
			{
				vUp = m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos + D3DXVECTOR3(0.f,fHeight,0.f);
			}
			else
			{
				vUp = m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos + D3DXVECTOR3(0.f,fHeight,0.f);
			}
			vDown = m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos + D3DXVECTOR3(0.f,0.01f,0.f);
			if ( pSetLandMan->IsCollisionLine_NoneConst( vDown, vUp, vNor, EMCC_CULL_NONE ) )
			{
				m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos = vUp;
			}
			m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos += D3DXVECTOR3(0.f,1.0f,0.f);


			//int nCount(0);
			//BOOL bEnable(FALSE);
			//while ( !bEnable )
			//{
			//	switch ( j )
			//	{
			//	case 0:
			//		vUp = vDown = vecWaterSurfPOS[i] + vecPlaneUp[i] - vecCross[i];
			//		break;
			//	case 1:
			//		vUp = vDown = vecWaterSurfPOS[i] + vecPlaneUp[i] + vecCross[i];
			//		break;
			//	case 2:
			//		vUp = vDown = vecWaterSurfPOS[i] - vecPlaneUp[i]*VERTEX_MIDDLE_RATE - vecCross[i];
			//		break;
			//	case 3:
			//		vUp = vDown = vecWaterSurfPOS[i] - vecPlaneUp[i]*VERTEX_MIDDLE_RATE + vecCross[i];
			//		break;
			//	case 4:
			//		vUp = vDown = vecWaterSurfPOS[i] - vecPlaneUp[i] - vecCross[i];
			//		break;
			//	case 5:
			//		vUp = vDown = vecWaterSurfPOS[i] - vecPlaneUp[i] + vecCross[i];
			//		break;
			//	};

			//	vUp = vUp + D3DXVECTOR3(0.f,fHeight,0.f);
			//	vDown = vDown + D3DXVECTOR3(0.f,0.01f,0.f);
			//	if ( pSetLandMan->IsCollisionLine( vDown, vUp, FALSE ) )
			//	{
			//		bEnable = TRUE;
			//		m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos = vUp;
			//	}
			//	else if ( nCount > 5 )
			//	{
			//		// 계속 보정하는 데도 안되면 강제 셋팅한다.
			//		bEnable = TRUE;
			//	}
			//	else
			//	{
			//		++nCount;
			//		bEnable = FALSE;
			//	}
			//	m_pVertex[PLANE_IN_VERTEX_NUM*i+j].vPos += D3DXVECTOR3(0.f,1.0f,0.f);
			//}
		}

		float fLength;
		fLength = m_pVertex[PLANE_IN_VERTEX_NUM*i+4].vPos.y - m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vPos.y;
		fLength += m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos.y - m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vPos.y;
		m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos.y = m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vPos.y + fLength * 0.2f;

		fLength = m_pVertex[PLANE_IN_VERTEX_NUM*i+5].vPos.y - m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vPos.y;
		fLength += m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos.y - m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vPos.y;
		m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos.y = m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vPos.y + fLength * 0.2f;

		//// Note : 내적을 사용하여 기울기가 있으면 덜 올라가도록 수정한다.
		//PositionDot( m_pVertex[PLANE_IN_VERTEX_NUM*i+0].vPos, m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos );
		//PositionDot( m_pVertex[PLANE_IN_VERTEX_NUM*i+1].vPos, m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos );
		//PositionDot( m_pVertex[PLANE_IN_VERTEX_NUM*i+4].vPos, m_pVertex[PLANE_IN_VERTEX_NUM*i+2].vPos );
		//PositionDot( m_pVertex[PLANE_IN_VERTEX_NUM*i+5].vPos, m_pVertex[PLANE_IN_VERTEX_NUM*i+3].vPos );
	}
}

void WaterSurfPlane::PositionDot( D3DXVECTOR3& vOut, const D3DXVECTOR3& vBase )
{
	float fDot(0.f);
	float fLength(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);
	D3DXVECTOR3 vVector(0.f,0.f,0.f);

	vDir = vOut - vBase;
	vVector = vDir;
	vVector.y = 0.f;
	fLength = D3DXVec3Length( &vVector );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVec3Normalize( &vVector, &vVector );
	fDot = D3DXVec3Dot( &vDir, &vVector );
	vOut = vBase + vDir * fLength * fDot * fDot;	// 제곱을 하여 덜 높게 올라가도록 한다.
}

void WaterSurfPlane::FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime )
{
	m_pWaterSurtPlane->FrameMove( sProp, fElapsedTime );
}

void WaterSurfPlane::Render( WATERSURFVERTEX*& pLocked, bool& bFirst )
{
	if( !m_pVertex )
		return;

    m_pWaterSurtPlane->Render( m_pVertex, pLocked, bFirst );
}

void WaterSurfPlane::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_nPlaneNUM;
		if( m_nPlaneNUM > 0 )
		{
			SFile.WriteBuffer( m_pVertex, sizeof(WATERSURFVERTEX)*PLANE_IN_VERTEX_NUM*m_nPlaneNUM );
		}
	}
	SFile.EndBlock();
}

void WaterSurfPlane::Load( sc::SerialFile &SFile, const WATERSURF_PROPERTY& sProp )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_nPlaneNUM;
	if( m_nPlaneNUM > 0 )
	{
		m_pVertex = new WATERSURFVERTEX[PLANE_IN_VERTEX_NUM*m_nPlaneNUM];
		SFile.ReadBuffer( m_pVertex, sizeof(WATERSURFVERTEX)*PLANE_IN_VERTEX_NUM*m_nPlaneNUM );
	}

	// Note : 생성
	m_pWaterSurtPlane->Create( sProp, m_pVertex, m_nPlaneNUM );
}

void WaterSurfPlane::Import( const WaterSurfPlane* pSrc, const WATERSURF_PROPERTY& sProp )
{
	CleanUp();

	m_nPlaneNUM = pSrc->m_nPlaneNUM;
	if( m_nPlaneNUM > 0 )
	{
		m_pVertex = new WATERSURFVERTEX[PLANE_IN_VERTEX_NUM*m_nPlaneNUM];
		memcpy ( m_pVertex, pSrc->m_pVertex, sizeof(WATERSURFVERTEX)*PLANE_IN_VERTEX_NUM*m_nPlaneNUM );
	}

	// Note : 생성
	m_pWaterSurtPlane->Create( sProp, m_pVertex, m_nPlaneNUM );
}