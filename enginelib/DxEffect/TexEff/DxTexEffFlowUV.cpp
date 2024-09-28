
#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../G-Logic/GLDefine.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/TextureManager.h"

#include "./DxTexEffFlowUV.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DxTexEffFlowUV::TYPEID = DEF_TEXEFF_FLOWUV;
const DWORD	DxTexEffFlowUV::VERSION = 0x00000101;
const char DxTexEffFlowUV::NAME[] = "[ Flow UV ]";

// Note : TEXEFF_FLOWUV_PROPERTY
TEXEFF_FLOWUV_PROPERTY::TEXEFF_FLOWUV_PROPERTY() :
	dwFlag(0L),
	fScale(1.f),
	fAlpha(1.f),
	vSpeed(0.f,1.f),
	fSpeed(1.f),
	vDirect(0.f,1.f)
{
}

TEXEFF_FLOWUV_PROPERTY::~TEXEFF_FLOWUV_PROPERTY()
{
}

void TEXEFF_FLOWUV_PROPERTY::Save( sc::SerialFile& SFile )
{
	SFile << dwFlag;
	SFile << strAlphaTex;
	SFile << strFlowTex;
	SFile << fScale;
	SFile << vSpeed.x;
	SFile << vSpeed.y;
	SFile << fSpeed;
	SFile << fAlpha;
	SFile << vDirect.x;
	SFile << vDirect.y;
}

void TEXEFF_FLOWUV_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> dwFlag;
	SFile >> strAlphaTex;
	SFile >> strFlowTex;
	SFile >> fScale;
	SFile >> vSpeed.x;
	SFile >> vSpeed.y;
	SFile >> fSpeed;
	SFile >> fAlpha;
	SFile >> vDirect.x;
	SFile >> vDirect.y;

    m_textureResAlpha = TextureManager::GetInstance().LoadTexture(
        strAlphaTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

    m_textureResFlow = TextureManager::GetInstance().LoadTexture(
        strFlowTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );
}

void TEXEFF_FLOWUV_PROPERTY::Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> dwFlag;
	SFile >> fScale;
	SFile >> fAlpha;
	SFile >> vSpeed.x;
	SFile >> vSpeed.y;
	SFile >> fSpeed;
	SFile >> vDirect.x;
	SFile >> vDirect.y;
	SFile >> strAlphaTex;
	SFile >> strFlowTex;

	m_textureResAlpha = TextureManager::GetInstance().LoadTexture(
        strAlphaTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

    m_textureResFlow = TextureManager::GetInstance().LoadTexture(
        strFlowTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );
}

// Note : DxTexEffFlowUV
LPDIRECT3DSTATEBLOCK9	DxTexEffFlowUV::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffFlowUV::m_pDrawSB = NULL;

DxTexEffFlowUV::DxTexEffFlowUV() :
	m_bUp(TRUE),
	m_vLastSpeed(0.f,0.f),
	m_fTime(-D3DX_PI),
	m_fRandSpeed(1.f),
	m_dwColor(0L)
{
}

DxTexEffFlowUV::~DxTexEffFlowUV()
{
}

void DxTexEffFlowUV::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TEXEFF_FLOWUV_PROPERTY* pTemp = (TEXEFF_FLOWUV_PROPERTY*)pProp;
	m_sProp.dwFlag		= pTemp->dwFlag;
	m_sProp.fScale		= pTemp->fScale;
	m_sProp.fAlpha		= pTemp->fAlpha;
	m_sProp.vSpeed		= pTemp->vSpeed;
	m_sProp.strAlphaTex	= pTemp->strAlphaTex.c_str();
	m_sProp.strFlowTex	= pTemp->strFlowTex.c_str();

	m_sProp.m_textureResAlpha = TextureManager::GetInstance().LoadTexture(
        m_sProp.strAlphaTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

    m_sProp.m_textureResFlow = TextureManager::GetInstance().LoadTexture(
        m_sProp.strFlowTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

	// Note : 변환..
	m_sProp.fSpeed		= sqrtf( (m_sProp.vSpeed.x*m_sProp.vSpeed.x) + (m_sProp.vSpeed.y*m_sProp.vSpeed.y) );
	if( m_sProp.fSpeed > 0.f )
	{
		m_sProp.vDirect.x	= m_sProp.vSpeed.x / m_sProp.fSpeed;
		m_sProp.vDirect.y	= m_sProp.vSpeed.y / m_sProp.fSpeed;
	}
}

void DxTexEffFlowUV::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//// Anisotropic Filter 확인
	//DWORD dwMinFilter;
	//if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	//else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	//else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		//{
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		//	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, dwMinFilter );
		//	pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		//}

		// Note : SetRenderState()	 변해도 상관없음.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		// Note : SetRenderState()
		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffFlowUV::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

//void DxTextureEffMan::RenderMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : GetTexture
//	if( !pProp->pMultiBaseTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiBaseTex.c_str(), pProp->pMultiBaseTex );
//		if( !pProp->pMultiBaseTex )	return;
//	}
//	if( !pProp->pMultiFlowTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiFlowTex.c_str(), pProp->pMultiFlowTex );
//		if( !pProp->pMultiFlowTex )	return;
//	}
//
//	// Note : 텍스쳐를 흘린다.
//	if( pProp->bfMultiTexNEWUV )
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pProp->fMultiTexUVScale, pProp->fMultiTexAddUV, pMesh );
//	}
//	else
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pMesh );
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pMultiBaseTex );
//	pd3dDevice->SetTexture( 1, pProp->pMultiFlowTex  );
//
//	
//	m_pMultiTexSavedSB->Capture();
//	m_pMultiTexDrawSB->Apply();
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_dwMultiTexSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, LPD3DXMESH pMesh )
//{
//	float fSpeed = fBaseSpeed * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2 += D3DXVECTOR2( fSpeed, fSpeed );
//	}
//	pMesh->UnlockVertexBuffer();
//}

void DxTexEffFlowUV::FrameMove( const float fElapsedTime )
{
	if (m_sProp.dwFlag&TEXEFFFLOWUV_NEWUV)
    {
    }

	if (m_sProp.dwFlag&TEXEFFFLOWUV_RANDOMSPEED)
	{
		// Note : fTime을 더해준다.
		m_fTime += m_fRandSpeed * fElapsedTime;

		// Note : Random Speed Setting
		if( m_fTime > 0.f && m_bUp )		
		{
			m_bUp = FALSE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}
		else if( m_fTime>D3DX_PI && !m_bUp )
		{
			m_fTime -= D3DX_PI*2.f;	// 값을 빼준다.
			m_bUp = TRUE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}

		// Note : 오르락 내리락
		float fSpeed = cosf( m_fTime );
		fSpeed += 2.f;
		fSpeed *= 0.33f;

		m_vLastSpeed += m_sProp.vDirect * (fSpeed*m_sProp.fSpeed) * fElapsedTime;
	}
	else
	{
		m_vLastSpeed += m_sProp.vSpeed * fElapsedTime;
	}
}

void DxTexEffFlowUV::FrameMoveMultiTex( LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVert, VERTEX* pVertSrc )
{
	struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; float fTex3; };
	VERTEXTEX2 sBackUp;
	VERTEXTEX2* pVert;
	HRESULT hr = pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	if( FAILED(hr) )	return;
	for( DWORD i=0; i<dwVert; ++i )
	{
		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vTex1 = pVertSrc[i].vTex;
		sBackUp.vTex2 = pVertSrc[i].vTex + m_vLastSpeed;
		sBackUp.fTex3 = 1.f;

		pVert[i] = sBackUp;
	}
	pVB->Unlock();
}

void DxTexEffFlowUV::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
							LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert )
{
	FrameMoveMultiTex( pVB, dwVert, pVertSrc );

	SetRenderBegin( pd3dDevice );

	pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(VERTEX) );
	pd3dDevice->SetIndices( pIB );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
									0,
									pAttrib[dwAttrib].VertexStart,
									pAttrib[dwAttrib].VertexCount,
									pAttrib[dwAttrib].FaceStart*3,
									pAttrib[dwAttrib].FaceCount );

	SetRenderEnd( pd3dDevice );
}

void DxTexEffFlowUV::Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc )
{
	DWORD dwVertexSizeFULL = dwVert*DxDynamicVB::g_sVB_PT2I.GetFVFSize();
    if( !DxDynamicVB::g_sVB_PT2I.IsValidSize( dwVertexSizeFULL ) )
        return;

    VOID* pLocked = DxDynamicVB::g_sVB_PT2I.LockVB( dwVertexSizeFULL );
    if( !pLocked )
        return;

	struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; };
	VERTEXTEX2 sBackUp;
	VERTEXTEX2* pVertices = (VERTEXTEX2*)pLocked;

    for( DWORD i=0; i<dwVert; ++i )
	{
		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vTex1 = pVertSrc[i].vTex;
		sBackUp.vTex2 = pVertSrc[i].vTex + m_vLastSpeed;

		pVertices[i] = sBackUp;
	}

    UINT uiStartVertex = DxDynamicVB::g_sVB_PT2I.UnlockVB( dwVertexSizeFULL, dwVert );

	//pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );	// SetRenderBegin에서 부른다.
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_PT2I.pVB, 0, DxDynamicVB::g_sVB_PT2I.GetFVFSize() );
	//pd3dDevice->SetIndices ( m_pIBuffer );		// 위쪽에서 IB를 부르게 한다.

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, dwVert, 0, dwFaces );
}

void DxTexEffFlowUV::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetTexture( 0, m_sProp.m_textureResAlpha.GetTexture() );
	pd3dDevice->SetTexture( 1, m_sProp.m_textureResFlow.GetTexture() );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PT2I.dwFVF );

	if( m_sProp.dwFlag&TEXEFFFLOWUV_MODULATE2X )
	{
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
	}
}

void DxTexEffFlowUV::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedSB->Apply();
	pd3dDevice->SetTexture( 1, NULL );
}

void DxTexEffFlowUV::SavePSF( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	m_sProp.Save( SFile );
	SFile.EndBlock();
}

BOOL DxTexEffFlowUV::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}
	else if( dwVer==0x00000100 )
	{
		m_sProp.Load_100( pd3dDevice, SFile );
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}

	return TRUE;
}