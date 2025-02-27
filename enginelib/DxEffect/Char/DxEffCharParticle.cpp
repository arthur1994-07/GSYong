#include "pch.h"

#include <string>

#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/TextureManager.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/DxDynamicVB.h"

#include "../../G-Logic/GLDefine.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/DxViewPort.h"

#include "./DxEffCharParticle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL		g_bOBJECT100;
extern BOOL		g_bCHAR_EDIT_RUN;

DWORD		DxEffCharParticle::TYPEID			= EMEFFCHAR_PARTICLE;
const DWORD	DxEffCharParticle::VERSION			= 0x0102;
char		DxEffCharParticle::NAME[MAX_PATH]	= "4.Static_Item_Particle"; 

DxEffCharParticle::PARTICLEPOOL*	DxEffCharParticle::m_pParticlePool = NULL;
VERTEXCOLORTEX						DxEffCharParticle::m_pVertices[MAX_FACE*4];

DxEffCharParticle::DxEffCharParticle(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_nPrevKeyTime(-1),
	m_dwCheckindex(0),
	m_fTimeADD(0.f),
	m_fParticleADD(0.f),
	m_fSizeDiff(0.f),
	m_fLifeDiff(0.f),
	m_fRotateDiff(0.f),
	m_fVelocityDiff(0.f),
	m_pParticleHEAD(NULL)
{
}

DxEffCharParticle::~DxEffCharParticle(void)
{
    CleanUp_Particle();

	CleanUp ();

	m_vecPosition.clear();
	m_listPosition.clear();
}

void DxEffCharParticle::CleanUp_Particle()
{
    //if ( !m_pParticlePool )
    //{
    //    m_pParticleHEAD = NULL;
    //    return;
    //}

	DXPARTICLE* pDel = NULL;
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pNext;

        //m_pParticlePool->releaseMem( pDel );
        SAFE_DELETE( pDel );
	}

    m_pParticleHEAD = NULL;
}

DxEffChar* DxEffCharParticle::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharParticle *pEffCharSingle = new DxEffCharParticle;
	pEffCharSingle->SetFileName ( GetFileName() );
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

	//m_vecPosition.clear();
	std::copy( m_vecPosition.begin(), m_vecPosition.end(), std::back_inserter(pEffCharSingle->m_vecPosition) );

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

HRESULT DxEffCharParticle::OnCreateDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    //m_pParticlePool = new sc::ObjectPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharParticle::OnDestroyDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharParticle::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharParticle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_fSizeDiff = m_vSize.y - m_vSize.x;
	m_fLifeDiff = m_fLifeMax - m_fLifeMin;
	m_fRotateDiff = m_vRotate.y - m_vRotate.x;

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	return S_OK;
}

HRESULT DxEffCharParticle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharParticle::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharParticle::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	CleanUp_Particle();

	return S_OK;
}

HRESULT DxEffCharParticle::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharParticle::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fElapsedTime = fElapsedTime;
	if( m_vecPosition.empty() )	return S_OK;
	if( m_fElapsedTime > 0.033f )	m_fElapsedTime = 0.033f;

	return S_OK;
}

void DxEffCharParticle::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )
{
	if ( !m_textureRes.Update() )
		return;

	if( m_vecPosition.empty() )	
		return;

	if( g_bCHAR_EDIT_RUN )	RenderTestViewPoint( pd3dDevice );

	// Note : USE_REMAIN일 때만, 행동이 바뀌었을 경우 동작. 이 녀석은 절대좌표일 경우만 동작한다네.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos( pMatrix );
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : 스케일을 체크한다.
	float		fLength(1.f);
	D3DXVECTOR3 vLocal(0.f,0.f,0.f);
	D3DXVECTOR3	vUp(0.f,1.f,0.f);
	if( pMatrix )
	{
		D3DXVec3TransformCoord( &vUp, &vUp, pMatrix );
		D3DXVec3TransformCoord( &vLocal, &vLocal, pMatrix );

		vUp -= vLocal;
		fLength = D3DXVec3Length( &vUp );
	}

	// Note : 매트릭스 변화 된 것으로 위치를 얻고 파티클을 움직이고 생성한다.
	CreateFrameMoveParticle( m_fElapsedTime, fLength );

	int			nCount(0);
	float		fSize(0.f);

	// Note : 시퀀스인지 체크한다.
	D3DXVECTOR2* pTexUV(NULL);
	D3DXVECTOR2 vTexUV00(0.f,0.f);
	D3DXVECTOR2 vTexUV10(0.f,0.f);
	D3DXVECTOR2 vTexUV01(0.f,0.f);
	D3DXVECTOR2 vTexUV11(0.f,0.f);
	float		fOffsetWidth(0.f);
	float		fOffsetHeight(0.f);
	if( m_dwFlag & USE_SEQUENCE )
	{
		fOffsetWidth = m_sSequenceUV.GetOffsetWidth();
		fOffsetHeight = m_sSequenceUV.GetOffsetHeight();
	}

	// Note : 빌보드를 체크한다.
	D3DXVECTOR3 vBasePos(0.f,0.f,0.f);
	D3DXMATRIX	matLocal;
	D3DXMatrixIdentity( &matLocal );
	DxBillboardLookAt( matLocal, &vBasePos, DxViewPort::GetInstance().GetMatView() );

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;

	// Note : 만든 후 뿌린다.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )
			break;

		// Note : 회전을 사용할 경우와 아닐 경우
		if( m_dwFlag & USE_ROTATE )
		{
			// Note : 회전값 넣어줌
			pCur->m_fRotate += pCur->m_fRotateVel*m_fElapsedTime;

			// Note : 사이즈 변화를 줌
			fSize = pCur->m_fSize*fScale;

			// Note : 점들의 적용
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate),				fSize*cosf(pCur->m_fRotate), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+fHalfPI),		fSize*cosf(pCur->m_fRotate+fHalfPI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+fOneHalfPI),	fSize*cosf(pCur->m_fRotate+fOneHalfPI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+D3DX_PI),		fSize*cosf(pCur->m_fRotate+D3DX_PI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+3].vPos, &vLocal, &matLocal );
		}
		else
		{
			// Note : 사이즈 변화를 줌
			fSize = pCur->m_fSize*fLength;

			// Note : 점들의 적용
			vLocal = D3DXVECTOR3( -fSize, fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize, fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -fSize, -fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize, -fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+3].vPos, &vLocal, &matLocal );
		}
		
		if( pMatrix )
		{
			if( m_dwFlag&USE_REMAIN )	vBasePos = pCur->m_vPos;
			else						D3DXVec3TransformCoord( &vBasePos, &pCur->m_vPos, pMatrix );
		}
		else
		{
			vBasePos = pCur->m_vPos;
		}

		if( m_dwFlag & USE_RANDOMRANGE )
		{
			vBasePos += pCur->m_vRandomRange*fLength;		// Note : 일정한 범위를 이동 시킨다.
		}

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos 이넘을 회전 시켜야 정상적으로 나온다.
		m_pVertices[(nCount*4)+1].vPos += vBasePos;
		m_pVertices[(nCount*4)+2].vPos += vBasePos;
		m_pVertices[(nCount*4)+3].vPos += vBasePos;

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;

		if( m_dwFlag & USE_SEQUENCE )
		{
			pTexUV = m_sSequenceUV.GetTexUV( m_sSequenceUV.GetTexCount(pCur->m_fRate) );
			if( !pTexUV )
			{
				pCur = pCur->m_pNext;	// 문제가 있으므로 생성하지 않는다.
				continue;
			}
			vTexUV00 = vTexUV10 = vTexUV01 = vTexUV11 = *pTexUV;

			vTexUV10.x += fOffsetWidth;
			vTexUV01.y += fOffsetHeight;
			vTexUV11.x += fOffsetWidth;
			vTexUV11.y += fOffsetHeight;

			m_pVertices[(nCount*4)+0].vTex = vTexUV00;
			m_pVertices[(nCount*4)+1].vTex = vTexUV10;
			m_pVertices[(nCount*4)+2].vTex = vTexUV01;
			m_pVertices[(nCount*4)+3].vTex = vTexUV11;
		}
		else
		{
			m_pVertices[(nCount*4)+0].vTex = D3DXVECTOR2( 0.f, 0.f );
			m_pVertices[(nCount*4)+1].vTex = D3DXVECTOR2( 1.f, 0.f );
			m_pVertices[(nCount*4)+2].vTex = D3DXVECTOR2( 0.f, 1.f );
			m_pVertices[(nCount*4)+3].vTex = D3DXVECTOR2( 1.f, 1.f );
		}

		pCur = pCur->m_pNext;
		++nCount;
	}

	// Note : nCount 가 파티클 갯수를 나타낸다.
    if( nCount > 0 )
	{
		DWORD dwVertexSizeFULL = nCount*DxDynamicVB::g_sVB_PDT.GetFVFSize()*4;
        if( !DxDynamicVB::g_sVB_PDT.IsValidSize( dwVertexSizeFULL ) )
            return;

        if( !DxDynamicVB::g_sVB_PDT.IsValidCopy( dwVertexSizeFULL ) )
		    OPTMManager::GetInstance().Render();

	    UINT nVertexCount = DxDynamicVB::g_sVB_PDT.CopyToVB(
		    m_pVertices,
		    dwVertexSizeFULL,
		    nCount * 4 );

		// Note : 데이터를 넣는다.
        OPTMCharParticle::DATA* pData = OPTMManager::GetInstance().m_sCharParticle.GetData();
		pData->pTex = m_textureRes.GetTexture();
        pData->nVertexCount = nVertexCount;
		pData->nFaceCount = nCount;

		// Note : 나중에 뿌리도록 셋팅 해 놓는다.
		OPTMManager::GetInstance().m_sCharParticle.InsertData( pData );
	}
}

void DxEffCharParticle::RenderTestViewPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( g_bOBJECT100 )	return;

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	int nCount(0);
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==m_dwCheckindex )
		{
			EDITMESHS::RENDERSPHERE( pd3dDevice, (*iter), 0.02f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );
			return;
		}
	}
}

void DxEffCharParticle::CreateFrameMoveParticle( float fElapsedTime, const float fScale )
{
	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMoveAbsolute( m_Property, m_vecPosition, m_fElapsedTime, m_sPlayData, fScale ) )	// 삭제 없음.
			{
				pPrev = pCur;	// 이전 위치 백업
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// 중간 녀석을 삭제	// 백업 필요 없다. pPrev이기 때문에.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
			else				// 처음 녀석을 삭제 // 백업 필요 없음.
			{
				pDel = pCur;
				m_pParticleHEAD = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
		}
	}
	else
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMove( m_Property, m_vecPosition, m_fElapsedTime, m_sPlayData ) )	// 삭제 없음.
			{
				pPrev = pCur;	// 이전 위치 백업
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// 중간 녀석을 삭제	// 백업 필요 없다. pPrev이기 때문에.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
			else				// 처음 녀석을 삭제 // 백업 필요 없음.
			{
				pDel = pCur;
				m_pParticleHEAD = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
		}
	}

	m_fParticleADD += m_dwParticlePec*m_fElapsedTime;
	DWORD dwParticleCreate = (DWORD)m_fParticleADD;
	m_fParticleADD -= dwParticleCreate;

	DxCreateParticle( dwParticleCreate );
}

void DxEffCharParticle::ComputeCreatePos( const D3DXMATRIX* pMatrix )
{
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : 생명이 다 되었는지도 여기서 체크
		pCur->ComputeCreatePos( pMatrix, m_vecPosition );
		pCur = pCur->m_pNext;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharParticle::DXPARTICLE::FrameMove( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, const float fElpasedTime, DXDATA& sData )
{
	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )
		return FALSE;

	// Note : 위치 셋팅
	m_fRate = m_fTime / m_fLife;
	for( DWORD i=1; i<vecData.size(); ++i )
	{
		if( (m_fRate >= vecData[i-1].fRate) && (m_fRate < vecData[i].fRate) )
		{
			float fDisRate = 1.f/(vecData[i].fRate-vecData[i-1].fRate);
			D3DXVECTOR3 vPos_1 = vecData[i-1].vPos*( (vecData[i].fRate-m_fRate)*fDisRate );
			D3DXVECTOR3 vPos_2 = vecData[i].vPos*( (m_fRate-vecData[i-1].fRate)*fDisRate );

			m_vPos = vPos_1 + vPos_2;
			break;
		}
	}

	// Note : 변화률이 있을 경우만 사이즈, 속도를 바꿈
	if( sProp.m_vSize.z != 1.f )
		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : 디퓨즈 셋팅.
	if( m_fRate < 0.2f )
	{
		const float fAlpha = m_fRate/0.2f;

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else if( m_fRate > 0.8f )
	{
		const float fAlpha = 1.f - ((m_fRate-0.8f)/0.2f);

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else
	{
		m_dwColor = sData.dwColorBASE;
	}

	return TRUE;
}

BOOL DxEffCharParticle::DXPARTICLE::FrameMoveAbsolute( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, 
														const float fElpasedTime, DXDATA& sData, const float fScale ) 
{
	// Note : 플레이가 적용 안 되었으면 동작하지 않는다.
	if( !(m_dwFlag&USE_PLAY) )
		return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )
		return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 위치
	m_vPos += m_vDirDiff*m_fVelocity*fElpasedTime*fScale;

	// Note : 변화률이 있을 경우만 사이즈, 속도를 바꿈
	if( sProp.m_vSize.z != 1.f )
		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : 디퓨즈 셋팅.
	if( m_fRate < 0.2f )
	{
		const float fAlpha = m_fRate/0.2f;

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else if( m_fRate > 0.8f )
	{
		const float fAlpha = 1.f - ((m_fRate-0.8f)/0.2f);

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else
	{
		m_dwColor = sData.dwColorBASE;
	}

	return TRUE;
}

void DxEffCharParticle::DXPARTICLE::ComputeCreatePos( const D3DXMATRIX* pMatrix, VECPOSITION& vecData )
{
	if( m_dwFlag&USE_PLAY )	return;

	D3DXVECTOR3 vPos_1(0.f,0.f,0.f);
	D3DXVECTOR3 vPos_2(0.f,0.f,0.f);

	if( m_dwFlag&USE_POS1 )		// 두번째 걸릴때
	{
		// Note : 두번째 위치를 구한다.
		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		for( DWORD i=1; i<vecData.size(); ++i )
		{
			if( (m_fFirstPosRate >= vecData[i-1].fRate) && (m_fFirstPosRate < vecData[i].fRate) )
			{
				float fDisRate = 1.f / (vecData[i].fRate - vecData[i-1].fRate);

				D3DXVec3TransformCoord( &vPos_1, &vecData[i-1].vPos, pMatrix );
				D3DXVec3TransformCoord( &vPos_2, &vecData[i].vPos, pMatrix );
				vPos_1 = vPos_1*( (vecData[i].fRate-m_fFirstPosRate)*fDisRate );
				vPos_2 = vPos_2*( (m_fFirstPosRate-vecData[i-1].fRate)*fDisRate );

				vPos = vPos_1 + vPos_2;
			}
		}

		// Note : 2점 사이에서의 새로운 점을 구한다.
		float fRate01 = RANDOM_01;
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// 처음 걸릴때
	{
		// Note : 처음 위치를 구한다.
		for( DWORD i=1; i<vecData.size(); ++i )
		{
			if( (m_fFirstPosRate >= vecData[i-1].fRate) && (m_fFirstPosRate < vecData[i].fRate) )
			{
				float fDisRate = 1.f / (vecData[i].fRate - vecData[i-1].fRate);
				
				D3DXVec3TransformCoord( &vPos_1, &vecData[i-1].vPos,  pMatrix );
				D3DXVec3TransformCoord( &vPos_2, &vecData[i].vPos,  pMatrix );
				vPos_1 = vPos_1*( (vecData[i].fRate-m_fFirstPosRate)*fDisRate );
				vPos_2 = vPos_2*( (m_fFirstPosRate-vecData[i-1].fRate)*fDisRate );

				m_vFirstPos = vPos_1 + vPos_2;

				m_dwFlag |= USE_POS1;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharParticle::DxCreateParticle( DWORD dwNUM )
{
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	if( m_dwFlag&USE_REMAIN )
	{
		while( dwNUM > 0 )
		{
			//DXPARTICLE* pParticle = m_pParticlePool->getMem();
            DXPARTICLE* pParticle = new DXPARTICLE;

            pParticle->m_vPos = m_vecPosition[0].vPos;
			pParticle->m_dwColor = 0x00000000;
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_01;				// 랜덤하게 삽입
			pParticle->m_fLife = m_fLifeMin + m_fLifeDiff*RANDOM_01;			// 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * RANDOM_01;						// 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_01;	// 랜덤하게 삽입

			pParticle->m_fSizeBase = pParticle->m_fSize;									// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

			if( m_dwFlag & USE_RANDOMRANGE )	// Note : 범위를 랜덤하게 해 놓을 경우.
			{
				pParticle->m_vRandomRange = D3DXVECTOR3( RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange );
			}

			pParticle->m_dwFlag = 0L;											// 절대일 경우만.
			pParticle->m_fVelocity = m_vVelocity.x + m_fVelocityDiff*RANDOM_01;	// 절대일 경우만.
			pParticle->m_fFirstPosRate = RANDOM_01;								// 절대일 경우만
			pParticle->m_vFirstPos = D3DXVECTOR3( 0.f, 0.f, 0.f );				// 절대일 경우만

			dwNUM -= 1;		// 값을 빼줌

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
	else
	{
		while( dwNUM > 0 )
		{
			//DXPARTICLE* pParticle = m_pParticlePool->getMem();
            DXPARTICLE* pParticle = new DXPARTICLE;

            pParticle->m_vPos = m_vecPosition[0].vPos;
			pParticle->m_dwColor = 0x00000000;
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_01;				// 랜덤하게 삽입
			pParticle->m_fLife = m_fLifeMin + m_fLifeDiff*RANDOM_01;			// 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * RANDOM_01;						// 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_01;	// 랜덤하게 삽입

			pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

			if( m_dwFlag & USE_RANDOMRANGE )	// Note : 범위를 랜덤하게 해 놓을 경우.
			{
				pParticle->m_vRandomRange = D3DXVECTOR3( RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange );
			}

			dwNUM -= 1;		// 값을 빼줌

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharParticle::InsertPoint( const D3DXVECTOR3& vPos )
{
	m_listPosition.push_back( vPos );
}

BOOL DxEffCharParticle::DeletePoint( const DWORD dwIndex )
{
	if( dwIndex >= m_listPosition.size() )	return FALSE;

	int nCount = 0;
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listPosition.erase( iter );
			return TRUE;
		}
	}

	return TRUE;
}

int DxEffCharParticle::DataUp( DWORD dwIndex )
{
	if( dwIndex == 0 )	return dwIndex;

	D3DXVECTOR3 vTemp;
	int nCount(0);
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listPosition.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex -= 1;
	if( dwIndex==0 )
	{
		m_listPosition.push_front( vTemp );
		return nCount;
	}

	iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listPosition.insert( iter, vTemp );
			return nCount;
		}
	}

	return 0;
}

int DxEffCharParticle::DataDown( DWORD dwIndex )
{
	if( dwIndex >= (m_listPosition.size()-1) )	return dwIndex;

	D3DXVECTOR3 vTemp;
	int nCount(0);
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listPosition.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex += 1;
	iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listPosition.insert( iter, vTemp );
			return nCount;
		}
	}

	// Note : 맨 마지막으로 가야함.
	m_listPosition.push_back( vTemp );

	return nCount;
}

std::string DxEffCharParticle::GetPositionString( const DWORD dwIndex )
{
	if( dwIndex >= m_listPosition.size() )	return std::string("");

	int nCount = 0;
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			TCHAR szTemp[128] = {0};
			_snprintf( szTemp, 128, "%f, %f, %f", (*iter).x, (*iter).y, (*iter).z );

			return std::string(szTemp);
		}
	}

	return std::string("");
}

void DxEffCharParticle::SetPoistionEditPos( const DWORD dwIndex )
{
	m_dwCheckindex = dwIndex;

	DWORD nCount(0L);
	D3DXVECTOR3 vTemp;
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==m_dwCheckindex )
		{
			vTemp = (*iter);
			break;
		}
	}

	DxViewPort::GetInstance().CameraJump( vTemp );
}

D3DXVECTOR3 DxEffCharParticle::GetInfluencePosition()
{
	DWORD dwCount(0L);
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++dwCount )
	{
		if( m_dwCheckindex == dwCount )	return (*iter);
	}

	return D3DXVECTOR3(0.f,0.f,0.f);
}

D3DXVECTOR3 DxEffCharParticle::SetMatrixToMakePos( const D3DXMATRIX& matWorld )
{
	DWORD dwCount(0L);
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++dwCount )
	{
		if( m_dwCheckindex == dwCount )
		{
			(*iter).x = matWorld._41;
			(*iter).y = matWorld._42;
			(*iter).z = matWorld._43;

			return (*iter);
		}
	}

	return D3DXVECTOR3(0.f,0.f,0.f);
}

BOOL DxEffCharParticle::EditApplyResetData()
{
	// Note : 새로 만들어야 한다.
	m_vecPosition.clear();

	if( m_listPosition.size() < 2 )	return FALSE;

	// Note : 일단 복제
	int nCount = 0;
	float fDisADD = 0.f;
	D3DXVECTOR3 vPos_CUR(0.f,0.f,0.f);
	D3DXVECTOR3 vDiff(0.f,0.f,0.f);
	LISTPOSITION_ITER iterPREV;
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter, ++nCount )
	{
		if( nCount==0 )
		{
			DXPOSDELTA sData;
			sData.vPos = (*iter);
			sData.fRate = 0.f;
			m_vecPosition.push_back( sData );
		}
		else
		{
			vDiff = (*iter) - (*iterPREV);
			float fDis = D3DXVec3Length( &vDiff );
			fDisADD += fDis;

			DXPOSDELTA sData;
			sData.vPos = (*iter);
			sData.fRate = fDisADD;
			m_vecPosition.push_back( sData );
		}

		iterPREV = iter;
		vPos_CUR = (*iter);
	}


	// Note : 비율을 맞춘다.
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].fRate /= fDisADD;
	}

	return TRUE;
}


HRESULT DxEffCharParticle::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwSize = 0;
	D3DXVECTOR3 vPos(0.f,0.f,0.f);

	//	Note : 이팩트의 Property 를 저장.
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	// Note : Edit를 하기 위한 Data
	SFile >> dwSize;
	m_listPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> vPos;
		m_listPosition.push_back( vPos );
	}

	// Note : 실제 사용하기 위한 Data
	SFile >> dwSize;
	m_vecPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXPOSDELTA sPosDelta;
		sPosDelta.Load( SFile );
		m_vecPosition.push_back( sPosDelta );
	}


	return S_OK;
}

HRESULT DxEffCharParticle::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwSize = 0;
	D3DXVECTOR3 vPos(0.f,0.f,0.f);

	//	Note : 이팩트의 Property 를 저장.
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );


	// Note : 실제 사용하기 위한 Data
	SFile >> dwSize;
	m_vecPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXPOSDELTA sPosDelta;
		sPosDelta.Load( SFile );
		m_vecPosition.push_back( sPosDelta );
	}

	// Note : Edit를 하기 위한 Data
	SFile >> dwSize;
	m_listPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> vPos;
		m_listPosition.push_back( vPos );
	}

	return S_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------------
HRESULT	DxEffCharParticle::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0102( SFile, pd3dDevice );	break;
	default:	// Ver.100은 안함.
		{
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
			return E_FAIL;
		}
		break;
	};

	//	Note : Device 자원을 생성한다.
	hr = Create( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharParticle::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// Note : 실제 사용하기 위한 Data
	SFile << (DWORD)m_vecPosition.size();
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].Save( SFile );
	}

	// Note : Edit를 하기 위한 Data
	SFile << (DWORD)m_listPosition.size();
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter )
	{
		SFile << (*iter);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.

// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
void DxEffCharParticle::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
												   const D3DXMATRIX* pMatrix,		// Skin 아닌 무기를 위한 Matrix 좌표.
												   const float fScale )
{
	RenderEff( pd3dDevice, pMatrix, 1.f );
}

//--------------------------------------------------------------------------------------------------------------------------
//									D	X		P	O	S		D	E	L	T	A	< Save, Load >
//--------------------------------------------------------------------------------------------------------------------------
void DxEffCharParticle::DXPOSDELTA::Save( sc::BaseStream& SFile )
{
	SFile << vPos;
	SFile << fRate;
}

void DxEffCharParticle::DXPOSDELTA::Load( sc::BaseStream& SFile )
{
	SFile >> vPos;
	SFile >> fRate;
}

