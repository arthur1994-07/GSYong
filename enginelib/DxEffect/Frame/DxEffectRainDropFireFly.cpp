#include "pch.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../DxTools/DebugSet.h"

#include "../../G-Logic/GLPeriod.h"
#include "../../G-Logic/GLDefine.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxViewPort.h"

#include "./DxEffectRainDrop.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DxEffectRainPoint::D3DVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
sc::ObjectPool<DxEffectRainPoint::FIREFLY>	*DxEffectRainPoint::m_pFireFlyPool = NULL;
DxEffectRainPoint::FIREFLY				*DxEffectRainPoint::m_pFireFlyHead = NULL;
DxEffectRainPoint::D3DVERTEX			*DxEffectRainPoint::m_pFireFlyArray = NULL;
DWORD									DxEffectRainPoint::m_dwFireFlyCount = 0L;
const DWORD								DxEffectRainPoint::MAX_FIREFLY		= 500;

DxEffectRainPoint::FIREFLY::FIREFLY() :
	vPos(0,0,0),
	vDirect(0,0,0),
	fSpeed(0.f),
	fLife(0.f),
	fLifeEnd(0.f),
	bChangeUSE(FALSE),
	fSize(0.f),
	vColor(0,0,0,0),
	pNext(NULL)
{
}

DxEffectRainPoint::FIREFLY::~FIREFLY()
{
}

void DxEffectRainPoint::OnResetDevice_FireFly_STATIC( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 디퓨즈 사용

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_FireFly_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_FireFly );
	}
}

void DxEffectRainPoint::OnLostDevice_FireFly_STATIC( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB_FireFly );
	SAFE_RELEASE( m_pSB_FireFly_SAVE );
}

void DxEffectRainPoint::FrameMoveFireFly( float fElapsedTime )
{
	float		fSize;
	DWORD		dwColor;
	DWORD		dwAlpha;
	D3DXVECTOR3	vLocal;
	D3DXVECTOR3	vDirect( 0.f, 0.f, 0.f );
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXMATRIX	matLocal;
	DxBillboardLookAt( matLocal, &vDirect, DxViewPort::GetInstance().GetMatView() );
	matLocal._41 = 0.f;
	matLocal._42 = 0.f;
	matLocal._43 = 0.f;

	m_dwFireFlyCount = 0;

	//	Update
	FIREFLY* pParticlePrev = NULL;
	FIREFLY* pParticle = m_pFireFlyHead;
	while ( pParticle )
	{
		// Note : 시간 흐름
		pParticle->fLife += fElapsedTime;

		// Note : 삭제 할 것 선택
		if( pParticle->fLife >= pParticle->fLifeEnd )
		{
			FIREFLY* pParticleThis = pParticle;

			//	Note : 리스트에서 분리.
			if ( pParticle == m_pFireFlyHead )
			{
				m_pFireFlyHead = pParticle->pNext;
				pParticlePrev = NULL;

				pParticle = m_pFireFlyHead;
			}
			else
			{
				pParticlePrev->pNext = pParticle->pNext;
				pParticle = pParticlePrev->pNext;
			}

			//	Note : 소멸하는 파티클 Pool에 등록.
			PutSleepFireFly( pParticleThis );

			continue;
		}

		// Note : 움직임.
		pParticle->vPos	+= pParticle->vDirect*pParticle->fSpeed*fElapsedTime;

		float fRate = pParticle->fLife / pParticle->fLifeEnd;

		fRate += sc::Random::RANDOM_NUM()*0.1f;

		if( fRate <= 0.5f )
		{
			if( fRate < 0.f )	fRate = 0.f;

			fRate = fRate*2.f;		// 0 ~ 1

			dwAlpha = (DWORD)(fRate*255);
		}
		else
		{
			if( fRate > 1.f )	fRate = 1.f;

			fRate = fRate*2.f;		// 1 ~ 2
			fRate -= 2.f;			// -1 ~ 0
			fRate = fabsf( fRate );	// 1 ~ 0

			dwAlpha = (DWORD)(fRate*255);

			// Note : 방향의 변경
			if( !pParticle->bChangeUSE )
			{
				pParticle->vDirect += D3DXVECTOR3(sc::Random::RANDOM_POS()-0.5f, sc::Random::RANDOM_POS()-0.5f, sc::Random::RANDOM_POS()-0.5f);
				D3DXVec3Normalize ( &pParticle->vDirect, &pParticle->vDirect );
				pParticle->bChangeUSE = TRUE;
			}
		}

		// Note : 데이터 복제
		fSize = pParticle->fSize;
		matLocal._41 = pParticle->vPos.x;
		matLocal._42 = pParticle->vPos.y;
		matLocal._43 = pParticle->vPos.z;

		dwColor = 0x0033ff33 + (dwAlpha<<24);

		vLocal = D3DXVECTOR3( -fSize, fSize, 0.f );
		D3DXVec3TransformCoord( &m_pFireFlyArray[m_dwFireFlyCount*4+0].vPos, &vLocal, &matLocal );
		vLocal = D3DXVECTOR3( fSize, fSize, 0.f );
		D3DXVec3TransformCoord( &m_pFireFlyArray[m_dwFireFlyCount*4+1].vPos, &vLocal, &matLocal );
		vLocal = D3DXVECTOR3( -fSize, -fSize, 0.f );
		D3DXVec3TransformCoord( &m_pFireFlyArray[m_dwFireFlyCount*4+2].vPos, &vLocal, &matLocal );
		vLocal = D3DXVECTOR3( fSize, -fSize, 0.f );
		D3DXVec3TransformCoord( &m_pFireFlyArray[m_dwFireFlyCount*4+3].vPos, &vLocal, &matLocal );

		m_pFireFlyArray[m_dwFireFlyCount*4+0].Diffuse = dwColor;
		m_pFireFlyArray[m_dwFireFlyCount*4+1].Diffuse = dwColor;
		m_pFireFlyArray[m_dwFireFlyCount*4+2].Diffuse = dwColor;
		m_pFireFlyArray[m_dwFireFlyCount*4+3].Diffuse = dwColor;

		// Note : 다음 작업을 위해서
		++m_dwFireFlyCount;

		if( m_dwFireFlyCount>=MAX_FIREFLY-1 )	return;

		pParticlePrev = pParticle;
		pParticle = pParticle->pNext;
	}
}

void DxEffectRainPoint::RenderFireFly( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_dwFireFlyCount <= 0 )
        return;

    DWORD dwVertexSizeFULL = m_dwFireFlyCount*DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize()*4;
    if( !DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
        return;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

    UINT uiStartVertex = DxDynamicVB::g_sVB_I_24BYTE.CopyToVB(
		m_pFireFlyArray,
		dwVertexSizeFULL,
		m_dwFireFlyCount * 4 );

    // Note : StateBlock Capture
	m_pSB_FireFly_SAVE->Capture();
	m_pSB_FireFly->Apply();	

	pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize() );
	pd3dDevice->SetIndices ( m_pIB );

	pd3dDevice->SetTexture( 0, m_textureResFireFly.GetTexture() );
	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwFireFlyCount*4, 0, m_dwFireFlyCount*2 );

	// Note : StateBlock Apply
	m_pSB_FireFly_SAVE->Apply();	

	CDebugSet::ToView( 11, "%d", m_dwFireFlyCount );
}

void DxEffectRainPoint::PutSleepFireFlys ( FIREFLY* pFireFlys )
{
	if( !pFireFlys )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	if( m_pFireFlyPool ) 
	{
		FIREFLY* pCur = pFireFlys;
		while ( pCur )
		{
			FIREFLY* pRelease = pCur;
			pCur = pCur->pNext;

			pRelease->pNext = NULL;
			m_pFireFlyPool->releaseMem(pRelease);
		}
	}
}

void DxEffectRainPoint::PutSleepFireFly ( FIREFLY* pFireFly )
{
	if( !pFireFly )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	if( m_pFireFlyPool ) 
	{
		pFireFly->pNext = NULL;
		m_pFireFlyPool->releaseMem(pFireFly);
	}
}

void DxEffectRainPoint::SEPERATEOBJ::CreateFireFly( const D3DXVECTOR3& vSrcPos )
{
	float		fRandHeight = (sc::Random::RANDOM_POS()*10.f) + 2.f;
	D3DXVECTOR3 vPos		= D3DXVECTOR3((float)((rand()%100)-50)+vSrcPos.x, vSrcPos.y+fRandHeight, (float)((rand()%100)-50)+vSrcPos.z);
	D3DXVECTOR3 vDirect		= D3DXVECTOR3(sc::Random::RANDOM_POS()-0.5f, sc::Random::RANDOM_POS()-0.5f, sc::Random::RANDOM_POS()-0.5f);

	D3DXVec3Normalize ( &vDirect, &vDirect );

	FIREFLY*		pParticle = NULL;

	//	Note : 새로운 파티클 할당.
	pParticle = m_pFireFlyPool->getMem();

	//	Note : 파티클 활성 리스트에 등록.
	pParticle->pNext = m_pFireFlyHead;
	m_pFireFlyHead = pParticle;

	pParticle->vPos			= vPos;									// 초기 위치 설정
	pParticle->fLife		= 0.f;									// 현재 0.f
	pParticle->fLifeEnd		= 2.f + sc::Random::RANDOM_POS() * 1.f;					// 생명 
	pParticle->bChangeUSE	= FALSE;
	pParticle->fSize		= 2.5f;									// 크기 초기화
	pParticle->fSpeed		= 3.f + sc::Random::RANDOM_POS() * 3.f;					// 속도 정해줌
	pParticle->vColor		= D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f );	// 색은 FrameMove 때 바뀜
	pParticle->vDirect		= vDirect;								// 방향 정해줌
}

void DxEffectRainPoint::SEPERATEOBJ::RenderFireFly( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	//// Note : 낮이라서 나오면 안됨.
	//if( GLPeriod::GetInstance().IsOffLight() )	return;

	//// Note : 비율을 맞춘다.
	//D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	//float fFaceRate = GetLengthRate( vFromPt );		// 0~1 값.

	//for( DWORD i=0; i<dwFaces; ++i )
	//{
	//	if( pFireFlyArray[i].fCurTime >= pFireFlyArray[i].fEndTime )
	//	{
	//		if( (fFaceRate==1.f) || (sc::Random::RANDOM_POS()<fFaceRate) )
	//		{
	//			CreateFireFly( pVertexArray[i] );	// 생성한다.
	//		}

	//		pFireFlyArray[i].fCurTime = 0.f;	// 되돌린다.
	//		continue;
	//	}

	//	pFireFlyArray[i].fCurTime += m_fElapsedTime;
	//}
}
