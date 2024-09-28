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

	// Note : USE_REMAIN�� ����, �ൿ�� �ٲ���� ��� ����. �� �༮�� ������ǥ�� ��츸 �����Ѵٳ�.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos( pMatrix );
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : �������� üũ�Ѵ�.
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

	// Note : ��Ʈ���� ��ȭ �� ������ ��ġ�� ��� ��ƼŬ�� �����̰� �����Ѵ�.
	CreateFrameMoveParticle( m_fElapsedTime, fLength );

	int			nCount(0);
	float		fSize(0.f);

	// Note : ���������� üũ�Ѵ�.
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

	// Note : �����带 üũ�Ѵ�.
	D3DXVECTOR3 vBasePos(0.f,0.f,0.f);
	D3DXMATRIX	matLocal;
	D3DXMatrixIdentity( &matLocal );
	DxBillboardLookAt( matLocal, &vBasePos, DxViewPort::GetInstance().GetMatView() );

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;

	// Note : ���� �� �Ѹ���.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )
			break;

		// Note : ȸ���� ����� ���� �ƴ� ���
		if( m_dwFlag & USE_ROTATE )
		{
			// Note : ȸ���� �־���
			pCur->m_fRotate += pCur->m_fRotateVel*m_fElapsedTime;

			// Note : ������ ��ȭ�� ��
			fSize = pCur->m_fSize*fScale;

			// Note : ������ ����
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
			// Note : ������ ��ȭ�� ��
			fSize = pCur->m_fSize*fLength;

			// Note : ������ ����
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
			vBasePos += pCur->m_vRandomRange*fLength;		// Note : ������ ������ �̵� ��Ų��.
		}

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos �̳��� ȸ�� ���Ѿ� ���������� ���´�.
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
				pCur = pCur->m_pNext;	// ������ �����Ƿ� �������� �ʴ´�.
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

	// Note : nCount �� ��ƼŬ ������ ��Ÿ����.
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

		// Note : �����͸� �ִ´�.
        OPTMCharParticle::DATA* pData = OPTMManager::GetInstance().m_sCharParticle.GetData();
		pData->pTex = m_textureRes.GetTexture();
        pData->nVertexCount = nVertexCount;
		pData->nFaceCount = nCount;

		// Note : ���߿� �Ѹ����� ���� �� ���´�.
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
			// Note : ������ �� �Ǿ������� ���⼭ üũ
			if( pCur->FrameMoveAbsolute( m_Property, m_vecPosition, m_fElapsedTime, m_sPlayData, fScale ) )	// ���� ����.
			{
				pPrev = pCur;	// ���� ��ġ ���
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// �߰� �༮�� ����	// ��� �ʿ� ����. pPrev�̱� ������.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
			else				// ó�� �༮�� ���� // ��� �ʿ� ����.
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
			// Note : ������ �� �Ǿ������� ���⼭ üũ
			if( pCur->FrameMove( m_Property, m_vecPosition, m_fElapsedTime, m_sPlayData ) )	// ���� ����.
			{
				pPrev = pCur;	// ���� ��ġ ���
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// �߰� �༮�� ����	// ��� �ʿ� ����. pPrev�̱� ������.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;
				pCur = pCur->m_pNext;

                //m_pParticlePool->releaseMem( pDel );
                SAFE_DELETE( pDel );
			}
			else				// ó�� �༮�� ���� // ��� �ʿ� ����.
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
		// Note : ������ �� �Ǿ������� ���⼭ üũ
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

	// Note : ��ġ ����
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

	// Note : ��ȭ���� ���� ��츸 ������, �ӵ��� �ٲ�
	if( sProp.m_vSize.z != 1.f )
		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : ��ǻ�� ����.
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
	// Note : �÷��̰� ���� �� �Ǿ����� �������� �ʴ´�.
	if( !(m_dwFlag&USE_PLAY) )
		return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )
		return FALSE;

	// Note : ������ �˾Ƴ�.
	m_fRate = m_fTime / m_fLife;

	// Note : ��ġ
	m_vPos += m_vDirDiff*m_fVelocity*fElpasedTime*fScale;

	// Note : ��ȭ���� ���� ��츸 ������, �ӵ��� �ٲ�
	if( sProp.m_vSize.z != 1.f )
		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : ��ǻ�� ����.
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

	if( m_dwFlag&USE_POS1 )		// �ι�° �ɸ���
	{
		// Note : �ι�° ��ġ�� ���Ѵ�.
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

		// Note : 2�� ���̿����� ���ο� ���� ���Ѵ�.
		float fRate01 = RANDOM_01;
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// ó�� �ɸ���
	{
		// Note : ó�� ��ġ�� ���Ѵ�.
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
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_01;				// �����ϰ� ����
			pParticle->m_fLife = m_fLifeMin + m_fLifeDiff*RANDOM_01;			// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_01;						// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_01;	// �����ϰ� ����

			pParticle->m_fSizeBase = pParticle->m_fSize;									// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����

			if( m_dwFlag & USE_RANDOMRANGE )	// Note : ������ �����ϰ� �� ���� ���.
			{
				pParticle->m_vRandomRange = D3DXVECTOR3( RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange );
			}

			pParticle->m_dwFlag = 0L;											// ������ ��츸.
			pParticle->m_fVelocity = m_vVelocity.x + m_fVelocityDiff*RANDOM_01;	// ������ ��츸.
			pParticle->m_fFirstPosRate = RANDOM_01;								// ������ ��츸
			pParticle->m_vFirstPos = D3DXVECTOR3( 0.f, 0.f, 0.f );				// ������ ��츸

			dwNUM -= 1;		// ���� ����

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
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_01;				// �����ϰ� ����
			pParticle->m_fLife = m_fLifeMin + m_fLifeDiff*RANDOM_01;			// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_01;						// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_01;	// �����ϰ� ����

			pParticle->m_fSizeBase = pParticle->m_fSize;				// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����

			if( m_dwFlag & USE_RANDOMRANGE )	// Note : ������ �����ϰ� �� ���� ���.
			{
				pParticle->m_vRandomRange = D3DXVECTOR3( RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange, RANDOM_11*m_fRandomRange );
			}

			dwNUM -= 1;		// ���� ����

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

	// Note : �� ���������� ������.
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
	// Note : ���� ������ �Ѵ�.
	m_vecPosition.clear();

	if( m_listPosition.size() < 2 )	return FALSE;

	// Note : �ϴ� ����
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


	// Note : ������ �����.
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

	//	Note : ����Ʈ�� Property �� ����.
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	// Note : Edit�� �ϱ� ���� Data
	SFile >> dwSize;
	m_listPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> vPos;
		m_listPosition.push_back( vPos );
	}

	// Note : ���� ����ϱ� ���� Data
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

	//	Note : ����Ʈ�� Property �� ����.
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );


	// Note : ���� ����ϱ� ���� Data
	SFile >> dwSize;
	m_vecPosition.clear();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXPOSDELTA sPosDelta;
		sPosDelta.Load( SFile );
		m_vecPosition.push_back( sPosDelta );
	}

	// Note : Edit�� �ϱ� ���� Data
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
	default:	// Ver.100�� ����.
		{
			//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
			return E_FAIL;
		}
		break;
	};

	//	Note : Device �ڿ��� �����Ѵ�.
	hr = Create( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharParticle::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// Note : ���� ����ϱ� ���� Data
	SFile << (DWORD)m_vecPosition.size();
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].Save( SFile );
	}

	// Note : Edit�� �ϱ� ���� Data
	SFile << (DWORD)m_listPosition.size();
	LISTPOSITION_ITER iter = m_listPosition.begin();
	for( ; iter!=m_listPosition.end(); ++iter )
	{
		SFile << (*iter);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System �� ���� ȿ������ ���̱� ���� �۾�.

// PrecalculateForMaterialSystem�� 2�� �� ���� �̸� ����� ���°Ŷ��, �̰����� ������ �ȴ�.
void DxEffCharParticle::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
												   const D3DXMATRIX* pMatrix,		// Skin �ƴ� ���⸦ ���� Matrix ��ǥ.
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

