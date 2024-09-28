#include "pch.h"
#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../G-Logic/GLDefine.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/DxViewPort.h"

#include "./DxEffCharBonePosEff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern std::string	g_strPICK_BONE;
extern BOOL			g_bOBJECT100;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharBonePosEff::TYPEID			= EMEFFCHAR_BONEPOSEFF;
const DWORD	DxEffCharBonePosEff::VERSION		= 0x0101;
char		DxEffCharBonePosEff::NAME[MAX_PATH]	= "4.BonePosEff <Move>"; 

DxEffCharBonePosEff::PARTICLEPOOL*	DxEffCharBonePosEff::m_pParticlePool = NULL;
VERTEXCOLORTEX						DxEffCharBonePosEff::m_pVertices[MAX_FACE*4];

DxEffCharBonePosEff::DxEffCharBonePosEff(void)
	: DxEffChar()
	, m_fElapsedTime(0.f)
	, m_nPrevKeyTime(-1)
	, m_pParticleHEAD(NULL)
	, m_fParticleADD(0.f)
	, m_fSizeDiff(0.f)
	, m_fLifeDiff(0.f)
	, m_fVelocityDiff(0.f)
	, m_fRotateDiff(0.f)
	, m_rBoneTranMaintain(NULL)
	, m_nBoneIndex(-1)
{
}

DxEffCharBonePosEff::~DxEffCharBonePosEff(void)
{
    CleanUp_Particle();

	CleanUp ();
}

void DxEffCharBonePosEff::CleanUp_Particle()
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

DxEffChar* DxEffCharBonePosEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharBonePosEff *pEffCharSingle = new DxEffCharBonePosEff;
	pEffCharSingle->SetFileName ( GetFileName() );
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

HRESULT DxEffCharBonePosEff::OnCreateDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    //m_pParticlePool = new sc::ObjectPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharBonePosEff::OnDestroyDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_fSizeDiff = m_vSize.y - m_vSize.x;
	m_fLifeDiff = m_vLife.y - m_vLife.x;
	m_fVelocityDiff = m_vVelocity.y - m_vVelocity.x;
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

HRESULT DxEffCharBonePosEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	CleanUp_Particle();

	return S_OK;
}

HRESULT DxEffCharBonePosEff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fElapsedTime = fElapsedTime;
	if( !m_rBoneTranMaintain )				
		return S_OK;

	if( m_fElapsedTime > 0.033f )	
		m_fElapsedTime = 0.033f;

	return S_OK;
}

void DxEffCharBonePosEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )
{
	// // Note : 캐릭터 Tool일 경우만 사용. 본 선택시;
	if( g_bCHAR_EDIT_RUN )
	{
		if( m_rSkeleton && !g_bOBJECT100 )	
		{
			m_rSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );
		}

		// 캐릭터 툴은 매번 체크하게 된다.
		m_rBoneTranMaintain = m_rSkeleton->FindBone( m_szBoneName );
		if ( m_rBoneTranMaintain )
		{
			m_nBoneIndex = m_rBoneTranMaintain->GetIndex();
		}
	}
	else
	{
		// 게임상에서는 한번 정해지면 바뀌지 않는다.
		if ( m_nBoneIndex == -1 )
		{
			m_rBoneTranMaintain = m_rSkeleton->FindBone( m_szBoneName );
			if ( m_rBoneTranMaintain )
			{
				m_nBoneIndex = m_rBoneTranMaintain->GetIndex();
			}
		}
		else
		{
			m_rBoneTranMaintain = m_rSkeleton->FindBone_Index( m_nBoneIndex );
		}
	}

	if( !m_textureRes.Update() )
		return;

	if( !m_rBoneTranMaintain )	
		return;

	// Note : 일정 거리를 만든다.
	D3DXVECTOR3 vOffSetPos(0.f,0.f,0.f);
	if( m_dwFlag & USE_BONEOFFSETPOS )	vOffSetPos = m_vLocalPos;

	// Note : USE_REMAIN일 때만, 행동이 바뀌었을 경우 동작. 이 녀석은 절대좌표일 경우만 동작한다네.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos( vOffSetPos );
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : 매트릭스 변화 된 것으로 위치를 얻고 파티클을 움직이고 생성한다.
	CreateFrameMoveParticle( m_fElapsedTime );

	int			nCount(0);
	float		fSize(0.f);
	D3DXVECTOR3 vLocal(0.f,0.f,0.f);
	D3DXVECTOR3	vUp(0.f,1.f,0.f);

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
	DxBillboardLookAt( matLocal, &vBasePos, DxViewPort::GetInstance().GetMatView() );

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;

	// Note : 만든 후 뿌린다.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )	break;

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
			fSize = pCur->m_fSize*fScale;

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

		if( m_dwFlag&USE_REMAIN )
		{
			vBasePos = pCur->m_vPos;
		}
		else
		{
			vBasePos = pCur->m_vPos + vOffSetPos;
			D3DXVec3TransformCoord( &vBasePos, &vBasePos, &m_rBoneTranMaintain->m_matCombined );
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

void DxEffCharBonePosEff::ComputeCreatePos( const D3DXVECTOR3& vOffSet )
{
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : 생명이 다 되었는지도 여기서 체크
		pCur->ComputeCreatePos( m_rBoneTranMaintain->m_matCombined, vOffSet );
		pCur = pCur->m_pNext;
	}
}

void DxEffCharBonePosEff::CreateFrameMoveParticle( float fElapsedTime )
{
	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMoveAbsolute( m_Property, m_sPlayData, m_fElapsedTime ) )	// 삭제 없음.
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
			if( pCur->FrameMove( m_Property, m_sPlayData, m_fElapsedTime ))	// 삭제 없음.
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

	m_fParticleADD += m_dwParticlePec*fElapsedTime;
	DWORD dwParticleCreate = (DWORD)m_fParticleADD;
	m_fParticleADD -= dwParticleCreate;

	DxCreateParticle( dwParticleCreate );
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBonePosEff::DxCreateParticle( DWORD dwNUM )
{
	if( m_dwFlag&USE_REMAIN )
	{
		while( dwNUM > 0 )
		{
			//DXPARTICLE* pParticle = m_pParticlePool->getMem();
            DXPARTICLE* pParticle = new DXPARTICLE;

            pParticle->m_vDir = D3DXVECTOR3( sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM() );
			pParticle->m_vPos = pParticle->m_vDir * m_fCreateLength;
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fVel = m_vVelocity.x + m_fVelocityDiff * sc::Random::RANDOM_POS();	// 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입

			pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
			pParticle->m_fVelBase = pParticle->m_fVel;					// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해
			pParticle->m_fVelDiff = (pParticle->m_fVel*m_vVelocity.z) - pParticle->m_fVel;	// 값의 변화를 위해

			pParticle->m_dwFlag = 0L;							   // 절대일 경우만.
			pParticle->m_fFirstPosRate = sc::Random::RANDOM_POS(); // 절대일 경우만
			pParticle->m_vFirstPos = D3DXVECTOR3(0.f, 0.f, 0.f); // 절대일 경우만

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

            pParticle->m_vDir = D3DXVECTOR3( sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM() );
			pParticle->m_vPos = pParticle->m_vDir * m_fCreateLength;
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fVel = m_vVelocity.x + m_fVelocityDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입

			pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
			pParticle->m_fVelBase = pParticle->m_fVel;					// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해
			pParticle->m_fVelDiff = (pParticle->m_fVel*m_vVelocity.z) - pParticle->m_fVel;	// 값의 변화를 위해

			dwNUM -= 1;		// 값을 빼줌

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBonePosEff::DXPARTICLE::FrameMove( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime )
{
	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 변화률이 있을 경우만 사이즈, 속도를 바꿈
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;
	if( sProp.m_vVelocity.z != 1.f )	m_fVel = m_fVelBase + m_fVelDiff*m_fRate;

	// Note : 위치를 셋팅
	m_vPos += m_vDir*m_fVel*fElpasedTime;

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

BOOL DxEffCharBonePosEff::DXPARTICLE::FrameMoveAbsolute( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime ) 
{
	// Note : 플레이가 적용 안 되었으면 동작하지 않는다.
	if( !(m_dwFlag&USE_PLAY) )	return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 위치
	m_vPos += m_vDirDiff*m_fVel*fElpasedTime;

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

void DxEffCharBonePosEff::DXPARTICLE::ComputeCreatePos( const D3DXMATRIX& matMatrix, const D3DXVECTOR3& vOffSet )
{
	if( m_dwFlag&USE_PLAY )	return;

	D3DXVECTOR3 vPos_1(0.f,0.f,0.f);
	D3DXVECTOR3 vPos_2(0.f,0.f,0.f);

	if( m_dwFlag&USE_POS1 )		// 두번째 걸릴때
	{
		// Note : 두번째 위치를 구한다.
		D3DXVECTOR3 vPos = m_vPos + vOffSet;
		D3DXVec3TransformCoord( &vPos, &vPos, &matMatrix );

		// Note : 2점 사이에서의 새로운 점을 구한다.
		float fRate01 = sc::Random::RANDOM_POS();
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// 처음 걸릴때
	{
		D3DXVECTOR3 vPos = m_vPos + vOffSet;
		D3DXVec3TransformCoord( &m_vFirstPos, &vPos, &matMatrix );

		m_dwFlag |= USE_POS1;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------
D3DXMATRIX DxEffCharBonePosEff::GetInfluenceMatrix()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	if( !m_rSkeleton )	return matWorld;
	if( !m_rBoneTranMaintain )
	{
		D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
		m_rBoneTranMaintain = m_rSkeleton->FindBone( m_szBoneName );

		if( !m_rBoneTranMaintain )	return matWorld;
	}

	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	D3DXVec3TransformCoord( &vPos, &m_vLocalPos, &m_rBoneTranMaintain->m_matCombined );

	matWorld = m_rBoneTranMaintain->m_matCombined;
	matWorld._41 = vPos.x;
	matWorld._42 = vPos.y;
	matWorld._43 = vPos.z;

	return matWorld;
}

D3DXVECTOR3 DxEffCharBonePosEff::SetChangeMatrix( const D3DXMATRIX& matWorld )
{
	if(	!m_rBoneTranMaintain )	return D3DXVECTOR3(0.f,0.f,0.f);

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_rBoneTranMaintain->m_matCombined );
	D3DXMatrixMultiply( &matInverse, &matWorld, &matInverse );

	m_vLocalPos.x = matInverse._41;
	m_vLocalPos.y = matInverse._42;
	m_vLocalPos.z = matInverse._43;

	if( (m_vLocalPos.x>-0.001f) && (m_vLocalPos.x<0.001f) )		m_vLocalPos.x = 0.f;
	if( (m_vLocalPos.y>-0.001f) && (m_vLocalPos.y<0.001f) )		m_vLocalPos.y = 0.f;
	if( (m_vLocalPos.z>-0.001f) && (m_vLocalPos.z<0.001f) )		m_vLocalPos.z = 0.f;

	return m_vLocalPos;
}

HRESULT DxEffCharBonePosEff::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_BONEPOSEFF_100 pProperty;

	//	Note : 이팩트의 Property 를 저장.
	SFile.ReadBuffer( &pProperty, sizeof(EFFCHAR_PROPERTY_BONEPOSEFF_100) );
	m_Property.Assign( pProperty );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharBonePosEff::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0101( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			SFile.SetOffSet( SFile.GetfTell()+dwSize );
			
			return E_FAIL;
		}
		break;
	};



	//	Note : Device 자원을 생성한다.
	hr = Create( pd3dDevice );
	if ( FAILED(hr) )	return hr;
	
	
	return S_OK;
}

HRESULT	DxEffCharBonePosEff::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharBonePosEff::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
													 const D3DXMATRIX* pMatrix, 
													 const float fScale )
{
	RenderEff( pd3dDevice, pMatrix, fScale );
}