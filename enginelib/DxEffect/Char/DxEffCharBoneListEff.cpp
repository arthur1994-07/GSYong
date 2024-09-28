#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/DxViewPort.h"
#include "../../G-Logic/GLDefine.h"
#include "./DxEffCharBoneListEff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL			g_bOBJECT100;
extern std::string	g_strPICK_BONE;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharBoneListEff::TYPEID			= EMEFFCHAR_BONELISTEFF;
const DWORD	DxEffCharBoneListEff::VERSION			= 0x0101;
char		DxEffCharBoneListEff::NAME[MAX_PATH]	= "4.BoneListEff <Move>"; 

DxEffCharBoneListEff::PARTICLEPOOL*	DxEffCharBoneListEff::m_pParticlePool = NULL;
VERTEXCOLORTEX						DxEffCharBoneListEff::m_pVertices[MAX_FACE*4];

DxEffCharBoneListEff::DxEffCharBoneListEff(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_nPrevKeyTime(-1),
	m_pParticleHEAD(NULL),
	m_fParticleADD(0.f),
	m_fSizeDiff(0.f),
	m_fLifeDiff(0.f),
	m_fRotateDiff(0.f),
	m_fVelocityDiff(0.f),
	m_pBoneTran(NULL)
{
}

DxEffCharBoneListEff::~DxEffCharBoneListEff(void)
{
    CleanUp_Particle();

	CleanUp ();

	m_vecPosition.clear();
	m_listCustomBoneEDIT.clear();
}

void DxEffCharBoneListEff::CleanUp_Particle()
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

DxEffChar* DxEffCharBoneListEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharBoneListEff *pEffChar = new DxEffCharBoneListEff;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	//for ( DWORD i=0; i<m_vecPosition.size(); ++i )
	//{
	//	DXPOSDELTA sData(m_vecPosition[i]);
	//	pEffChar->m_vecPosition.push_back( sData );
	//}
	std::copy( m_vecPosition.begin(), m_vecPosition.end(), std::back_inserter(pEffChar->m_vecPosition) );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharBoneListEff::OnCreateDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    //m_pParticlePool = new sc::ObjectPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharBoneListEff::OnDestroyDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharBoneListEff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_fSizeDiff = m_vSize.y - m_vSize.x;
	m_fLifeDiff = m_vLife.y - m_vLife.x;
	m_fRotateDiff = m_vRotate.y - m_vRotate.x;
	m_fVelocityDiff = m_vVelocity.y - m_vVelocity.x;

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

HRESULT DxEffCharBoneListEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharBoneListEff::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	CleanUp_Particle();

	return S_OK;
}

HRESULT DxEffCharBoneListEff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::FrameMove( float fTime, float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;

	if( !m_rSkeleton )				return S_OK;
	if( m_vecPosition.empty() )		return S_OK;
	if( m_fElapsedTime > 0.033f )	m_fElapsedTime = 0.033f;

	//if( g_bCHAR_EDIT_RUN )
	//{
	//	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	//	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	//	{
	//		if( !(*iter).m_pBoneTran )
	//		{
	//			(*iter).m_pBoneTran = m_rSkeleton->FindBone( (*iter).m_strBoneName.c_str() );

	//			// Note : 본이 없어서 다시 로드 했는데 그래도 로딩이 안됬을 경우 작업안함.
	//			if( !(*iter).m_pBoneTran )	return S_OK;
	//		}
	//	}
	//}

	return S_OK;
}

void DxEffCharBoneListEff::CreateFrameMoveParticle( float fElapsedTime )
{
	if( m_vecPosition.empty() )	
		return;

	if ( !m_rSkeleton )
		return;

	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		if ( !m_vecPosition[i].IsExistBone() )
		{
			// Note : 본이 없어서 다시 로드 했는데 그래도 로딩이 안됬을 경우 작업안함.
			if ( !m_vecPosition[i].SetBoneName( *m_rSkeleton, m_vecPosition[i].GetBoneName() ) )
				return;
		}
	}

	// Note : BoneMatrix가 변한다면 거기에 셋팅된 위치좌표도 변해야 한다.
	UpdateVECPOSION();

	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMoveAbsolute( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// 삭제 없음.
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
			if( pCur->FrameMove( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// 삭제 없음.
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

void DxEffCharBoneListEff::ComputeCreatePos()
{
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : 생명이 다 되었는지도 여기서 체크
		pCur->ComputeCreatePos( m_vecPosition );
		pCur = pCur->m_pNext;
	}
}

void DxEffCharBoneListEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )
{
	PrecalculateBeforeRenderEff( pd3dDevice, pMatrix );
	OnlyRenderEff( pd3dDevice, fScale );
}

void DxEffCharBoneListEff::OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, float fScale )
{
	if( !m_textureRes.Update() )
		return;

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

		vBasePos = pCur->m_vPos;

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos 이넘을 회전 시켜야 정상적으로 나온다.
		m_pVertices[(nCount*4)+1].vPos += vBasePos;
		m_pVertices[(nCount*4)+2].vPos += vBasePos;
		m_pVertices[(nCount*4)+3].vPos += vBasePos;

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;

		// Note : 시퀀스를 사용할 경우와 아닐 경우
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

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBoneListEff::DxCreateParticle( DWORD dwNUM )
{
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	while (dwNUM > 0)
	{
		//DXPARTICLE* pParticle = m_pParticlePool->getMem();
        DXPARTICLE* pParticle = new DXPARTICLE;

        pParticle->m_fLife = m_vLife.x + m_fLifeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
		pParticle->m_fSize = m_vSize.x + m_fSizeDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
		pParticle->m_fRotate = m_vRotate.y * sc::Random::RANDOM_POS(); // 랜덤하게 삽입
		pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff * sc::Random::RANDOM_POS(); // 랜덤하게 삽입

		vDir = D3DXVECTOR3(sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM(), sc::Random::RANDOM_NUM());
		pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// 일정한 거리를 띄어 놓는다.

        pParticle->m_dwFlag = m_dwFlag;                             // Flag 셋팅
		pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
		pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

        if ( m_dwFlag&USE_REMAIN )
        {
		    pParticle->m_fVelocity = m_vVelocity.x + m_fVelocityDiff * sc::Random::RANDOM_POS(); // 절대일 경우만.
		    pParticle->m_fFirstPosRate = sc::Random::RANDOM_POS(); // 절대일 경우만
		    pParticle->m_vFirstPos = D3DXVECTOR3(0.f, 0.f, 0.f); // 절대일 경우만
        }

        if ( m_dwFlag&USE_CREATERANDOM_DONTMOVE )
        {
            pParticle->m_fPos_Rate = sc::Random::RANDOM_POS();
        }

		dwNUM -= 1;		// 값을 빼줌

		pParticle->m_pNext = m_pParticleHEAD;
		m_pParticleHEAD = pParticle;
	}
}

void DxEffCharBoneListEff::UpdateVECPOSION()
{
	if ( !m_rSkeleton )
		return;

	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		if( m_vecPosition[i].m_bVertex )
		{
			const DxBoneTransMaintain* pBoneTran = m_rSkeleton->FindBone_Index( m_vecPosition[i].GetBoneIndex() );
			D3DXVec3TransformCoord( &m_vecPosition[i].m_vPos, &m_vecPosition[i].m_vBase, &pBoneTran->m_matCombined );
		}
		else
		{
			const DxBoneTransMaintain* pBoneTran = m_rSkeleton->FindBone_Index( m_vecPosition[i].GetBoneIndex() );
			m_vecPosition[i].m_vPos.x = pBoneTran->m_matCombined._41;
			m_vecPosition[i].m_vPos.y = pBoneTran->m_matCombined._42;
			m_vecPosition[i].m_vPos.z = pBoneTran->m_matCombined._43;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBoneListEff::DXPARTICLE::FrameMove( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime )
{
	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

    float fPosRate(0.f);
    if ( m_dwFlag&USE_CREATERANDOM_DONTMOVE )
    {
        fPosRate = m_fPos_Rate;
    }
    else
    {
        fPosRate = m_fRate;
    }

	// Note : 위치 셋팅
	for( DWORD i=1; i<vecData.size(); ++i )
	{
		if( (fPosRate >= vecData[i-1].m_fRate) && (fPosRate < vecData[i].m_fRate) )
		{
			float fDisRate = 1.f / (vecData[i].m_fRate - vecData[i-1].m_fRate);
			D3DXVECTOR3 vPos_1 = vecData[i-1].m_vPos*( (vecData[i].m_fRate-fPosRate)*fDisRate );
			D3DXVECTOR3 vPos_2 = vecData[i].m_vPos*( (fPosRate-vecData[i-1].m_fRate)*fDisRate );

			m_vPos = vPos_1 + vPos_2 + m_vPosOffSet;
			break;
		}
	}

	// Note : 변화률이 있을 경우만 사이즈를 바꿈
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

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

BOOL DxEffCharBoneListEff::DXPARTICLE::FrameMoveAbsolute( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime )
{
	// Note : 플레이가 적용 안 되었으면 동작하지 않는다.
	if( !(m_dwFlag&USE_PLAY) )	return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 위치
	m_vPos += m_vDirDiff*m_fVelocity*fElpasedTime;

	// Note : 변화률이 있을 경우만 사이즈를 바꿈
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

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

void DxEffCharBoneListEff::DXPARTICLE::ComputeCreatePos( VECPOSITION& vecData )
{
	if( m_dwFlag&USE_PLAY )	return;

	if( m_dwFlag&USE_POS1 )		// 두번째 걸릴때
	{
		// Note : 두번째 위치를 구한다.
		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		for( DWORD i=1; i<vecData.size(); ++i )
		{
			if( (m_fFirstPosRate >= vecData[i-1].m_fRate) && (m_fFirstPosRate < vecData[i].m_fRate) )
			{
				float fDisRate = 1.f / (vecData[i].m_fRate - vecData[i-1].m_fRate);
				D3DXVECTOR3 vPos_1 = vecData[i-1].m_vPos*( (vecData[i].m_fRate-m_fFirstPosRate)*fDisRate );
				D3DXVECTOR3 vPos_2 = vecData[i].m_vPos*( (m_fFirstPosRate-vecData[i-1].m_fRate)*fDisRate );

				vPos = vPos_1 + vPos_2 + m_vPosOffSet;
			}
		}

		// Note : 2점 사이에서의 새로운 점을 구한다.
		float fRate01 = sc::Random::RANDOM_POS();
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
			if( (m_fFirstPosRate >= vecData[i-1].m_fRate) && (m_fFirstPosRate < vecData[i].m_fRate) )
			{
				float fDisRate = 1.f / (vecData[i].m_fRate - vecData[i-1].m_fRate);
				D3DXVECTOR3 vPos_1 = vecData[i-1].m_vPos*( (vecData[i].m_fRate-m_fFirstPosRate)*fDisRate );
				D3DXVECTOR3 vPos_2 = vecData[i].m_vPos*( (m_fFirstPosRate-vecData[i-1].m_fRate)*fDisRate );

				m_vFirstPos = vPos_1 + vPos_2 + m_vPosOffSet;

				m_dwFlag |= USE_POS1;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBoneListEff::InsertBone( const char* szName )
{
	// Note : 스켈렉톤이 없다면 리턴
	if( !m_rSkeleton )	return;

	DXCUSTOMBONE	sCBone;
	sCBone.m_strBoneName = szName;
	sCBone.m_pBoneTran = m_rSkeleton->FindBone( szName );

	// Note : 본이 없다면 리턴
	if( !sCBone.m_pBoneTran )	return;

	m_listCustomBoneEDIT.push_back( sCBone );
}

BOOL DxEffCharBoneListEff::DeleteBone( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBoneEDIT.size() )	return FALSE;

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBoneEDIT.erase( iter );
			return TRUE;
		}
	}

	return TRUE;
}

int DxEffCharBoneListEff::DataUp( DWORD dwIndex )
{
	if( dwIndex == 0 )	return dwIndex;

	DXCUSTOMBONE vTemp;
	int nCount(0);
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listCustomBoneEDIT.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex -= 1;
	if( dwIndex==0 )
	{
		m_listCustomBoneEDIT.push_front( vTemp );
		return nCount;
	}

	iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBoneEDIT.insert( iter, vTemp );
			return nCount;
		}
	}

	return 0;
}

int DxEffCharBoneListEff::DataDown( DWORD dwIndex )
{
	if( dwIndex >= (m_listCustomBoneEDIT.size()-1) )	return dwIndex;

	DXCUSTOMBONE vTemp;
	int nCount(0);
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listCustomBoneEDIT.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex += 1;
	iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBoneEDIT.insert( iter, vTemp );
			return nCount;
		}
	}

	// Note : 맨 마지막으로 가야함.
	m_listCustomBoneEDIT.push_back( vTemp );

	return nCount;
}

std::string DxEffCharBoneListEff::GetBoneString( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBoneEDIT.size() )	return std::string("");

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			TCHAR szTemp[256] = {0};
			_snprintf( szTemp, 256, "%s, %f, %f, %f", 
					(*iter).m_strBoneName.c_str(),
					(*iter).m_vVertex.x,
					(*iter).m_vVertex.y,
					(*iter).m_vVertex.z );

			return std::string(szTemp);
		}
	}

	return std::string("");
}

void DxEffCharBoneListEff::SetCheckBone( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBoneEDIT.size() )	return;

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )	
		{
			g_strPICK_BONE = (*iter).m_strBoneName.c_str();
			return;
		}
	}
}

BOOL DxEffCharBoneListEff::GetEnableBonePos()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).m_strBoneName.c_str() )	return (*iter).m_bVertex;
	}

	return FALSE;
}

D3DXVECTOR3 DxEffCharBoneListEff::GetInfluencePosition()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).m_strBoneName.c_str() )	return (*iter).m_vVertex;
	}

	return D3DXVECTOR3(0.f,0.f,0.f);
}

D3DXMATRIX DxEffCharBoneListEff::GetInfluenceMatrix()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).m_strBoneName.c_str() )
		{
			if( (*iter).m_pBoneTran )	
			{
				if( (*iter).m_bVertex )
				{
					D3DXVECTOR3  vPos(0.f,0.f,0.f);
					D3DXVec3TransformCoord( &vPos, &(*iter).m_vVertex, &(*iter).m_pBoneTran->m_matCombined );

					D3DXMATRIX matWorld;
					matWorld = (*iter).m_pBoneTran->m_matCombined;
					matWorld._41 = vPos.x;
					matWorld._42 = vPos.y;
					matWorld._43 = vPos.z;

					return matWorld;
				}
				else
				{
					return (*iter).m_pBoneTran->m_matCombined;
				}
			}
		}
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	return matIdentity;
}

D3DXVECTOR3 DxEffCharBoneListEff::SetChangeMatrix( const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3* pLocal(NULL);		// 주소값을 받아서 사용한다.
	const DxBoneTransMaintain* m_pBoneTran(NULL);
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).m_strBoneName.c_str() )
		{
			pLocal = &(*iter).m_vVertex;
			m_pBoneTran = (*iter).m_pBoneTran;
		}
	}

	
	if(	!m_pBoneTran || !pLocal )		return D3DXVECTOR3(0.f,0.f,0.f);

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_pBoneTran->m_matCombined );
	D3DXMatrixMultiply( &matInverse, &matWorld, &matInverse );

	pLocal->x = matInverse._41;
	pLocal->y = matInverse._42;
	pLocal->z = matInverse._43;

	if( (pLocal->x>-0.001f) && (pLocal->x<0.001f) )		pLocal->x = 0.f;
	if( (pLocal->y>-0.001f) && (pLocal->y<0.001f) )		pLocal->y = 0.f;
	if( (pLocal->z>-0.001f) && (pLocal->z<0.001f) )		pLocal->z = 0.f;

	return (*pLocal);
}

void DxEffCharBoneListEff::SetCheckBoneOffSetPos( BOOL bEnable )
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).m_strBoneName.c_str() )
		{
			(*iter).m_bVertex = bEnable;
			return;
		}
	}
}

BOOL DxEffCharBoneListEff::EditApplyResetData()
{
	// 캐릭터 툴에서만 동작.
	if( !g_bCHAR_EDIT_RUN )
	{
		return FALSE;
	}

	// Note : 새로 만들어야 한다.
	m_vecPosition.clear();

	if( m_listCustomBoneEDIT.size() < 2 )
		return FALSE;

	// Note : 스켈렉톤이 없다면 리턴
	if( !m_rSkeleton )
		return FALSE;

	// Note : 일단 복제
	int nCount = 0;
	float fDisADD = 0.f;
	D3DXVECTOR3 vPos_CUR(0.f,0.f,0.f);
	D3DXVECTOR3 vDiff(0.f,0.f,0.f);
	LISTCUSTOMBONE_ITER iterPREV;
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nCount )
	{
		if( nCount==0 )
		{
			DXPOSDELTA sData;
			sData.SetBoneName( *m_rSkeleton, (*iter).m_strBoneName );
			sData.m_vBase = (*iter).m_vVertex;
			sData.m_bVertex = (*iter).m_bVertex;
			sData.m_fRate = 0.f;
			m_vecPosition.push_back( sData );
		}
		else
		{

			const DxBoneTransMaintain* pBoneTran		= m_rSkeleton->FindBone( (*iter).m_strBoneName.c_str() );
			const DxBoneTransMaintain* pBoneTranPrev	= m_rSkeleton->FindBone( (*iterPREV).m_strBoneName.c_str() );
			if ( pBoneTran && pBoneTranPrev )
			{
				D3DXVECTOR3 vPos1, vPos2;
				D3DXVec3TransformCoord( &vPos1, &(*iter).m_vVertex, &pBoneTran->m_matCombined );
				D3DXVec3TransformCoord( &vPos2, &(*iterPREV).m_vVertex, &pBoneTranPrev->m_matCombined );
				vDiff = vPos1 - vPos2;
				float fDis = D3DXVec3Length( &vDiff );
				fDisADD += fDis;

				DXPOSDELTA sData;
				sData.SetBoneName( *m_rSkeleton, (*iter).m_strBoneName );
				sData.m_vBase = (*iter).m_vVertex;
				sData.m_bVertex = (*iter).m_bVertex;
				sData.m_fRate = fDisADD;
				m_vecPosition.push_back( sData );
			}
		}

		iterPREV = iter;
		vPos_CUR = (*iter).m_vVertex;
	}

	// Note : 비율을 맞춘다.
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].m_fRate /= fDisADD;
	}

	return TRUE;
}

HRESULT DxEffCharBoneListEff::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwSize = 0;

	EFFCHAR_PROPERTY_BONELISTEFF_100 pProperty;

	//	Note : 이팩트의 Property 를 저장.
	SFile.ReadBuffer( &pProperty, sizeof(EFFCHAR_PROPERTY_BONELISTEFF_100) );

	m_Property.Assign ( pProperty );

	// Note : Edit를 하기 위한 Data
	SFile >> dwSize;
	m_listCustomBoneEDIT.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXCUSTOMBONE	sData;
		sData.Load( SFile );
		m_listCustomBoneEDIT.push_back( sData );
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

HRESULT DxEffCharBoneListEff::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwSize = 0;

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
	m_listCustomBoneEDIT.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXCUSTOMBONE	sData;
		sData.Load( SFile );
		m_listCustomBoneEDIT.push_back( sData );
	}

	return S_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------------
HRESULT	DxEffCharBoneListEff::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharBoneListEff::SaveFile ( sc::BaseStream &SFile )
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
	SFile << (DWORD)m_listCustomBoneEDIT.size();
	LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
	for( ; iter!=m_listCustomBoneEDIT.end(); ++iter )
	{
		(*iter).Save( SFile );
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharBoneListEff::PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix )
{
	// Note : 캐릭터 Tool일 경우만 사용.
	if( g_bCHAR_EDIT_RUN )
	{
		m_pBoneTran = m_rSkeleton->FindBone( g_strPICK_BONE.c_str() );

		if( !g_bOBJECT100 )
		{
			// 선택된 본을 보여준다;
			if( m_rSkeleton )
				m_rSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );

			// Note : 순서를 표시해 준다.
			int nBoneCount(0);
			LISTCUSTOMBONE_ITER iter = m_listCustomBoneEDIT.begin();
			for( ; iter!=m_listCustomBoneEDIT.end(); ++iter, ++nBoneCount )
			{
				if( !(*iter).m_pBoneTran )
					continue;

				// Note : Render
				(*iter).RenderCount( nBoneCount, (*iter).m_pBoneTran->m_matCombined );
			}
		}
	}

	if( !m_textureRes.Update() )
		return;

	// Note : USE_REMAIN일 때만, 행동이 바뀌었을 경우 동작. 이 녀석은 절대좌표일 경우만 동작한다네.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos();
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : 매트릭스 변화 된 것으로 위치를 얻고 파티클을 움직이고 생성한다.
	CreateFrameMoveParticle( m_fElapsedTime );
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharBoneListEff::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
													  const D3DXMATRIX* pMatrix, 
													  const float fScale )
{
	OnlyRenderEff( pd3dDevice, fScale );
}


//--------------------------------------------------------------------------------------------------------------------------
//									D	X		P	O	S		D	E	L	T	A	< Save, Load >
//--------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBoneListEff::DXPOSDELTA::SetBoneName( const DxSkeletonMaintain &sSkeleton, const TSTRING& strName )
{
	m_strBoneName = strName;

	const DxBoneTransMaintain* pBoneTran = sSkeleton.FindBone( m_strBoneName );
	if ( pBoneTran )
	{
		m_nIndex = pBoneTran->GetIndex();
		return TRUE;
	}

	return FALSE;
}

void DxEffCharBoneListEff::DXPOSDELTA::Save( sc::BaseStream& SFile )
{
	SFile << m_strBoneName;
	SFile << m_bVertex;
	SFile << m_vBase;
	SFile << m_fRate;
}

void DxEffCharBoneListEff::DXPOSDELTA::Load( sc::BaseStream& SFile )
{
	SFile >> m_strBoneName;
	SFile >> m_bVertex;
	SFile >> m_vBase;
	SFile >> m_fRate;
}

//--------------------------------------------------------------------------------------------------------------------------
//								D	X		C	U	S	T	O	M		B	O	N	E	< Save, Load >
//--------------------------------------------------------------------------------------------------------------------------
void DXCUSTOMBONE::RenderCount( DWORD nCount, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3 vSrc;
	D3DXVec3TransformCoord( &vSrc, &m_vVertex, &matWorld );

	D3DXVECTOR3 vOut;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(), &matIdentity );

	CDebugSet::ToPos( vOut.x, vOut.y, "%u", nCount );
}

void DXCUSTOMBONE::Save( sc::BaseStream& SFile )
{
	SFile << m_strBoneName;
	SFile << m_bVertex;
	SFile << m_vVertex;
}

void DXCUSTOMBONE::Load( sc::BaseStream& SFile )
{
	SFile >> m_strBoneName;
	SFile >> m_bVertex;
	SFile >> m_vVertex;
}

