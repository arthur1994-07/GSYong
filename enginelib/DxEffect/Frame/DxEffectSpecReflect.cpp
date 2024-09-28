// SpecReflect.cpp: implementation of the DxEffectSpecReflect class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxEffect/Frame/DxEffectDefine.h"
#include "../../DxTools/Collision.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxShadowMap.h"

#include "../../DxTools/DxViewPort.h"

#include "./DxEffectSpecReflect.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DxEffectSpecReflect::TYPEID = DEF_EFFECT_SPECREFLECT;
const DWORD	DxEffectSpecReflect::VERSION = 0x00000100;
const char	DxEffectSpecReflect::NAME[] = "[ 스펙큘러-반사 효과 ]";
const DWORD DxEffectSpecReflect::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectSpecReflect::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectSpecReflect::m_pDrawStateBlock	= NULL;

BOOL		DxEffectSpecReflect::m_bReflectUSE			= FALSE;

void DXMATERIAL_SPECREFLECT::SaveFile( sc::SerialFile &SFile )
{
    SFile << bSpecUse;

    SFile << DWORD( 1 );

    SFile.WriteBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.WriteBuffer( szSpecTex, sizeof( char ) * MAX_PATH );
}

void DXMATERIAL_SPECREFLECT::LoadFile( sc::SerialFile &SFile )
{
    SFile >> bSpecUse;

    DWORD nTemp;
    SFile >> nTemp;

    SFile.ReadBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.ReadBuffer( szSpecTex, sizeof( char ) * MAX_PATH );
}

void DxEffectSpecReflect::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SPECREFLECT_PROPERTY);
	dwVer = VERSION;
}

void DxEffectSpecReflect::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SPECREFLECT_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectSpecReflect::DxEffectSpecReflect () :
	m_dwColorOP(D3DTOP_MODULATE),
	m_dwMaterials(0),
	m_dwTempMaterials(0),
	m_pTempMaterials(NULL),
	m_pMaterials(NULL)
{
	StringCchCopy( m_szReflectTex, MAX_PATH, "1d_Lighting.bmp" );
}

DxEffectSpecReflect::~DxEffectSpecReflect ()
{
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials );
}

HRESULT DxEffectSpecReflect::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			DxAnalysis ( pd3dDevice, pmsMeshs );
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectSpecReflect::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectSpecReflect::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	m_bReflectUSE = FALSE;
	if( d3dCaps.TextureCaps&D3DPTEXTURECAPS_PROJECTED )		m_bReflectUSE = TRUE;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_MIRROR );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX,	D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pDrawStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectSpecReflect::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pDrawStateBlock );

	return S_OK;
}

HRESULT DxEffectSpecReflect::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szReflectTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		true,
		FALSE );

	return S_OK;
}

HRESULT DxEffectSpecReflect::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return S_OK;
}

void DxEffectSpecReflect::ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
        m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
            m_pMaterials[i].szSpecTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_EFFECT,
			true,
			FALSE );
	}
}

HRESULT DxEffectSpecReflect::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectSpecReflect::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectSpecReflect::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	if( !m_bReflectUSE )	return S_OK;	// 안 된다면 할 필요 없다.

	PROFILE_BEGIN("DxEffectSpecReflect");

	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				LPDIRECT3DINDEXBUFFERQ	pIB = NULL;
				LPDIRECT3DVERTEXBUFFERQ pVB = NULL;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					
					dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );
				}
				else
				{
					dwFVFSize = pmsMeshs->m_pDxOctreeMesh->m_dwFVF;
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );
				}
				pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

				DWORD					AttribTableSize = 0;
				LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

				if ( pmsMeshs->m_pLocalMesh )
				{	
					pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
				}
				else
				{
					AttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
				}
				
				LPD3DXATTRIBUTERANGE pAttribTable;
				pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

				if ( pmsMeshs->m_pLocalMesh )
				{
					pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );
				}
				else
				{
					memcpy ( pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * AttribTableSize );
				}

				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->m_cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{
					if ( !m_pMaterials[ pAttribTable[i].AttribId ].bSpecUse )	continue;

					//	Note : 메터리얼, 텍스쳐 지정.
					pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ].Power = 30.f;
					pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ].Specular.r = 1.f;
					pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ].Specular.g = 1.f;
					pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ].Specular.b = 1.f;
					pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ].Specular.a = 1.f;
					pd3dDevice->SetMaterial ( &( pmsMeshs->m_rgMaterials[ pAttribTable[i].AttribId ] ) );

					pd3dDevice->SetTexture ( 0, m_pMaterials[ pAttribTable[i].AttribId ].m_textureRes.GetTexture() );
					pd3dDevice->SetTexture ( 1, m_textureRes.GetTexture() );

					m_pSavedStateBlock->Capture();
					m_pDrawStateBlock->Apply();

					DWORD	dwColorOP;
					pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	m_dwColorOP );

					//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
					//
					if ( pmsMeshs->m_pLocalMesh )
					{
						pd3dDevice->SetIndices ( pIB );
						pd3dDevice->SetStreamSource ( 0, pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pLocalMesh->GetFVF () );
					}
					else
					{
						pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );
						pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pDxOctreeMesh->m_dwFVF );
					}

					pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						0,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);

					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

					m_pSavedStateBlock->Apply();

					pd3dDevice->SetTexture ( 1, NULL );
				}

				SAFE_DELETE(pAttribTable);

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
					if(pVB) pVB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render ( pframeChild, pd3dDevice, pFrameMesh, pLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	PROFILE_END("DxEffectSpecReflect");

	return S_OK;
}

HRESULT DxEffectSpecReflect::DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	HRESULT	hr = S_OK;

	m_dwMaterials = pmsMeshs->m_cMaterials;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_SPECREFLECT[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pmsMeshs->m_strTextureFiles[i].GetString() );

		if ( !strlen(m_pMaterials[i].szSpecTex) )
			StringCchCopy( m_pMaterials[i].szSpecTex, MAX_PATH, pmsMeshs->m_strTextureFiles[i].GetString() );

        m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
            m_pMaterials[i].szSpecTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_EFFECT,
			true,
			FALSE );
	}

	if ( pmsMeshs->m_pDxOctreeMesh )
	{
		if ( !m_pTempMaterials )	return S_OK;

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			for ( DWORD j=0; j<m_dwTempMaterials; j++ )
			{
				if ( !strcmp ( m_pMaterials[i].szTexture, m_pTempMaterials[j].szTexture ) )
				{
					m_pMaterials[i].bSpecUse	= m_pTempMaterials[j].bSpecUse;
					StringCchCopy( m_pMaterials[i].szSpecTex, MAX_PATH, m_pTempMaterials[j].szSpecTex );

                    m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
                        m_pMaterials[i].szSpecTex,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_EFFECT,
						true,
						FALSE );
				}
			}
		}
	}

	return hr;
}

void DxEffectSpecReflect::SaveBuffer ( sc::SerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(DXMATERIAL_SPECREFLECT_OLD)*m_dwTempMaterials
			+sizeof(DWORD)+sizeof(DXMATERIAL_SPECREFLECT_OLD)*m_dwMaterials;

	// Material 저장
	SFile << m_dwTempMaterials;
	if ( m_pTempMaterials )
    {
        for( DWORD i=0; i<m_dwTempMaterials; ++i )
            m_pTempMaterials[ i ].SaveFile( SFile );
    }

	SFile << m_dwMaterials;
	if ( m_pMaterials )
    {
        for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].SaveFile( SFile );
    }
}

void DxEffectSpecReflect::LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwTempMaterials;
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	if ( m_dwTempMaterials )
	{
		m_pTempMaterials = new DXMATERIAL_SPECREFLECT[m_dwTempMaterials];
        for( DWORD i=0; i<m_dwTempMaterials; ++i )
            m_pTempMaterials[ i ].LoadFile( SFile );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXMATERIAL_SPECREFLECT[m_dwMaterials];
        for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].LoadFile( SFile );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
                m_pMaterials[i].szSpecTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				true,
				FALSE );
		}
	}
}

void DxEffectSpecReflect::LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwTempMaterials;
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	if ( m_dwTempMaterials )
	{
		m_pTempMaterials = new DXMATERIAL_SPECREFLECT[m_dwTempMaterials];
		for( DWORD i=0; i<m_dwTempMaterials; ++i )
            m_pTempMaterials[ i ].LoadFile( SFile );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXMATERIAL_SPECREFLECT[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].LoadFile( SFile );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
                m_pMaterials[i].szSpecTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				true,
				FALSE );
		}
	}
}

HRESULT DxEffectSpecReflect::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff )
{
	DxEffectSpecReflect* pSrcEff = (DxEffectSpecReflect*) pSrcEffect;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );

	// Material 
	if ( pSrcEff->m_pMaterials )
	{
		//	원본 만들기
		m_dwMaterials = pSrcEff->m_dwMaterials;
		m_pMaterials = new DXMATERIAL_SPECREFLECT[pSrcEff->m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i] = pSrcEff->m_pMaterials[i];

			m_pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
                m_pMaterials[i].szSpecTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				true,
				FALSE );
		}

		//	Frame 만들기
		m_dwTempMaterials = pSrcEff->m_dwMaterials;
		m_pTempMaterials = new DXMATERIAL_SPECREFLECT[m_dwTempMaterials];

		for ( DWORD i=0; i<m_dwTempMaterials; i++ )
		{
            m_pTempMaterials[i] = pSrcEff->m_pMaterials[i];

            m_pTempMaterials[i].m_textureRes.Clear();
		}
	}

	return S_OK;
}