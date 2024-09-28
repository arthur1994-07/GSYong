#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/TexEff/DxTexEffSpecular.h"
#include "../../DxEffect/TexEff/DxTexEffDiffuse.h"
#include "../../DxEffect/TexEff/DxTexEffRotate.h"
#include "../../DxEffect/TexEff/DxTexEffFlowUV.h"
#include "../../DxEffect/TexEff/DxTexEffMan.h"
#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxTools/TriangleLinkLight/DxTriangleLinkLight_LightMap.h"
#include "../../DxTools/Material/DxSingleMaterialMesh.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxViewPort.h"
#include "../../GlobalParam.h"
#include "../../Common/profile.h"

#include "../ExportXFile/NSExportXFile.h"
#include "../LightMap/LightMapRenderMan.h"
#include "../LightMap/LightMapObjectVEC.h"
#include "../LightMap/LightMapObject.h"
#include "../LightMap/NSLightMapFX.h"
#include "../LightMap/NSLightMap.h"
#include "../DxCullingMeshForThread.h"
#include "../NSLandThread.h"

#include "./DxAABBOctree.h"
#include "./DxSingleTexMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	i	n	g	l	e	T	e	x		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
#define MIPMAPLEVEL 3

DxSingleTexMesh::DxSingleTexMesh()
    : m_pAABBRenderStrategy(NULL)
   // , m_pTexture32BIT(NULL)
	, m_bCheckTex(FALSE)
	, m_pTexEffList(NULL)
	, m_dwEffectFlag(0L)
	, m_dwFlags(0L)
	, m_bDefaultFX_EnableLightMap(TRUE)
	, m_wwLightID(WORDWORD(-1,-1))
	, m_pNext(NULL)		// m_pNext �� �ı� �� ���� ����.... Octree �� �ı��϶�.
{
	m_strTexName = "";
	m_strMaterialName = _T("");
};

DxSingleTexMesh::~DxSingleTexMesh()
{
	CleanUp();

	m_pNext = NULL;
}

void DxSingleTexMesh::CleanUp()
{
	m_bCheckTex = FALSE;
    SAFE_DELETE( m_pAABBRenderStrategy );

	SAFE_DELETE( m_pTexEffList );

	m_textureRes.Clear();
	m_strTexName = "";
}

int DxSingleTexMesh::Create ( const LPDIRECT3DDEVICEQ pd3dDevice, 
							  const TCHAR* szMaterialName, 
							  const TSTRING& strTextureName,
							  const DWORD dwFaceNUM, 
							  BYTE* pVertices, 
							  const DWORD dwFVF, 
							  BOOL bPiece, 
							  DWORD dwFlags, 
							  const MaterialValue& sMaterialValue,
							  BOOL bUserLightMapUV,
							  float fCellSize )
{
	CleanUp();

	// Note : Object�� Ÿ���� �����Ѵ�.
	EM_LAND_TYPE emLandType(EMLR_PIECE);
	if( !bPiece )
	{
		if( DxLightMan::GetInstance()->IsNightAndDay() )	emLandType = EMLR_DAYNIGHT_ON;
		else												emLandType = EMLR_DAYNIGHT_OFF;		
	}

    SAFE_DELETE( m_pAABBRenderStrategy );
    m_pAABBRenderStrategy = AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_OCJOCTREE );
	int nResult = m_pAABBRenderStrategy->Create( pd3dDevice, dwFaceNUM, pVertices, dwFVF, emLandType, bUserLightMapUV, TRUE, ABTT_OCTREE, fCellSize );
	if ( nResult != 1 )
	{
		SAFE_DELETE ( m_pAABBRenderStrategy );
		return nResult;
	}

	m_dwFlags = dwFlags;
	m_sMaterialValue = sMaterialValue;
	m_strMaterialName = szMaterialName;
	m_strTexName = strTextureName;
	m_bDefaultFX_EnableLightMap = bUserLightMapUV;

	return 1;
}

int DxSingleTexMesh::CreateLightMap( const LPDIRECT3DDEVICEQ pd3dDevice, 
									 const TCHAR* szMaterialName, 
									 const TSTRING& strTextureName,
                                     const DWORD dwFaceNUM, 
                                     BYTE* pVertices,
                                     DWORD dwFlags,
									 const MaterialValue& sMaterialValue,
                                     DWORD dwFVF,
									 const TSTRING& strTextureCombined,
									 const TSTRING& strTextureDirect_Shadow_1,
									 const TSTRING& strTextureDirect_Shadow_2,
									 const TSTRING& strTextureIndirect,
									 BOOL bUserLightMapUV,
									 BOOL bRan_1_0,
									 float fCellSize )
{
	CleanUp();

    SAFE_DELETE( m_pAABBRenderStrategy );
    m_pAABBRenderStrategy = AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP );
    int nResult = m_pAABBRenderStrategy->Create( pd3dDevice, dwFaceNUM, pVertices, dwFVF, EMLR_PIECE, bUserLightMapUV, bRan_1_0, ABTT_LIGHTMAP, fCellSize );
	if ( nResult != 1 )
	{
		SAFE_DELETE ( m_pAABBRenderStrategy );
		return nResult;
	}

    m_pAABBRenderStrategy->SetTextureName( strTextureCombined, strTextureDirect_Shadow_1, strTextureDirect_Shadow_2, strTextureIndirect );

	m_dwFlags = dwFlags;
	m_sMaterialValue = sMaterialValue;
	m_strMaterialName = szMaterialName;
	m_strTexName = strTextureName;
	m_bDefaultFX_EnableLightMap = bUserLightMapUV;

	return 1;
}

int DxSingleTexMesh::CreateLightMapForTriangleLinkLight( const LPDIRECT3DDEVICEQ pd3dDevice, 
														const TCHAR* szMaterialName, 
														const TSTRING& strTextureName,
														const DWORD dwFaceNUM, 
														BYTE* pVertices,
														DWORD dwFlags,
														const MaterialValue& sMaterialValue,
														DWORD dwFVF,
														const TSTRING& strTextureCombined,
														const TSTRING& strTextureDirect_Shadow_1,
														const TSTRING& strTextureDirect_Shadow_2,
														const TSTRING& strTextureIndirect,
														BOOL bUserLightMapUV,
														const std::vector<WORDWORD>& vecTriangle_wwLightID,
														float fCellSize )
{
	CleanUp();

	SAFE_DELETE( m_pAABBRenderStrategy );
	m_pAABBRenderStrategy = AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_TRIANGLE_LINK_LIGHT_LIGHTMAP );
	DxTriangleLinkLight_LightMap* pTriangleLinkLight = dynamic_cast<DxTriangleLinkLight_LightMap*>(m_pAABBRenderStrategy);
	if ( !pTriangleLinkLight )
		return 0;

	pTriangleLinkLight->Create
	( 
		pd3dDevice, 
		szMaterialName,
		strTextureName,
		dwFaceNUM, 
		pVertices, 
		dwFlags,
		sMaterialValue,
		dwFVF, 
		strTextureCombined,
		strTextureDirect_Shadow_1,
		strTextureDirect_Shadow_2,
		strTextureIndirect,
		bUserLightMapUV, 
		vecTriangle_wwLightID,
		fCellSize
	);

	m_dwFlags = dwFlags;
	m_sMaterialValue = sMaterialValue;
	m_strMaterialName = szMaterialName;
	m_strTexName = strTextureName;
	m_bDefaultFX_EnableLightMap = bUserLightMapUV;

	return 1;
}

void DxSingleTexMesh::SetLightID( const WORDWORD& wwLightID )
{
	m_wwLightID = wwLightID;
}

// DxStaticMaterialMesh ���� mmapMesh �� ������� ���ȴ�. 
void DxSingleTexMesh::Create_DxSingleMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mmapMesh, float fCellSize ) const
{
	DxTriangleLinkLight_LightMap* pTriangleLinkLight = dynamic_cast<DxTriangleLinkLight_LightMap*>(m_pAABBRenderStrategy);
	if ( pTriangleLinkLight )
	{
		pTriangleLinkLight->Create_DxSingleMaterialMesh( pd3dDevice, mmapMesh, fCellSize );
	}
	else
	{
		DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
		pNew->Import( pd3dDevice, this, fCellSize );

		mmapMesh.insert( std::make_pair ( m_strMaterialName, pNew ) );
	}
}

void DxSingleTexMesh::LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ��ܿ����� �̰� LightMapRenderMan, DxTriangleLinkLight_LightMap, DxAABBOctree ���� �� �� �����鼭 �� LoadLightMap �� ȣ���Ѵ�.
	// 
	LightMapRenderMan* pLightMapRenderMan = dynamic_cast<LightMapRenderMan*>(m_pAABBRenderStrategy);
	if ( pLightMapRenderMan )
	{
		pLightMapRenderMan->LoadTexture( pd3dDevice );
	}
}
//
//void DxSingleTexMesh::MakeLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
//                                        const TSTRING& strLightMapName, 
//                                        DxStaticMesh* pStaticMesh, 
//                                        DxPieceManager* pPieceManager, 
//                                        const D3DXMATRIX& matWorld, 
//                                        SVEC_LIGHTMAPOBJ* psvecLightMapObj,
//                                        DWORD& dwStartTextureSize,
//                                        float fLightMapDetail,
//                                        BOOL bDontMakeShadowLM )
//{
//    std::vector<VERTEXNORCOLORTEX4> vecVertices;
//    m_pAABBRenderStrategy->GetVertices( vecVertices );
//
//    if ( vecVertices.empty() )
//        return;
//
//    // ���ؽ� ������ ��´�.
//    VERTEX* pVerticesVERTEX = new VERTEX[vecVertices.size()];
//    for ( DWORD i=0; i<vecVertices.size(); ++i )
//    {
//        pVerticesVERTEX[i].vPos = vecVertices[i].vPos;
//		pVerticesVERTEX[i].vNor = vecVertices[i].vNor;
//		pVerticesVERTEX[i].vTex = vecVertices[i].vTex[0];
//    }
//
//    psvecLightMapObj->m_dwLightMapFaceNUM = static_cast<DWORD> (vecVertices.size()/3);
//
//
//    // LightMap ������ ������ Color �� ����� 
//    {
//        NSLightMap::CreateLightMap( pd3dDevice,
//                                    strLightMapName, 
//                                    _T("_P"), 
//                                    FALSE,
//                                    TRUE,
//                                    psvecLightMapObj->m_vecLightMapObject,
//                                    psvecLightMapObj->m_dwLightMapFaceNUM, 
//                                    reinterpret_cast<BYTE*>(pVerticesVERTEX),
//                                    VERTEX::FVF,
//                                    pStaticMesh,
//                                    pPieceManager, 
//                                    matWorld,
//                                    dwStartTextureSize,
//                                    fLightMapDetail,
//                                    NSLIGHTMAPDEF::LMT_UV,
//                                    bDontMakeShadowLM );
//    }
//    
//
//    SAFE_DELETE_ARRAY( pVerticesVERTEX );
//}

void DxSingleTexMesh::GetVertexNum( DWORD& dwVertexNum )
{
	m_pAABBRenderStrategy->GetVertexNum( dwVertexNum );
}

void DxSingleTexMesh::GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV, std::vector<BOOL>& vecReceiveShadowLM )
{
	// �޸� ����ȭ�� ���ϱ� ���� �۾�.
	DWORD dwVertexNum(0);
	m_pAABBRenderStrategy->GetVertexNum( dwVertexNum );
	if ( (dwVertexNum+vecVertices.size()) > vecVertices.capacity() )
	{
		vecVertices.reserve( vecVertices.size() + dwVertexNum );
	}
	if ( (dwVertexNum+vecLightMapUV.size()) > vecLightMapUV.capacity() )
	{
		vecLightMapUV.reserve( vecLightMapUV.size() + dwVertexNum );
	}
	if ( (dwVertexNum+vecReceiveShadowLM.size()) > vecReceiveShadowLM.capacity() )
	{
		vecReceiveShadowLM.reserve( vecReceiveShadowLM.size() + dwVertexNum );
	}

	size_t nFaceNUM = vecVertices.size()/3;
	m_pAABBRenderStrategy->GetVertices( vecVertices, vecLightMapUV );

	BOOL bDontReceive(FALSE);
	if ( m_dwFlags & ENSTM_DONT_RECEIVE_SHADOW_LM )
	{
		bDontReceive = TRUE;
	}

	size_t nFaceNUM_NEW = vecVertices.size()/3;
	for ( size_t i=nFaceNUM; i<nFaceNUM_NEW; ++i )
	{
		vecReceiveShadowLM.push_back( bDontReceive );
	}
}
//
//void DxSingleTexMesh::MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
//											  const TSTRING& strLightMapName, 
//											  DxStaticMesh* pStaticMesh, 
//											  DxPieceManager* pPieceManager, 
//											  const D3DXMATRIX& matWorld, 
//											  SVEC_LIGHTMAPOBJ* psvecLightMapObj,
//											  DWORD& dwStartTextureSize,
//											  float fLightMapDetail,
//											  BOOL bDontMakeShadowLM,
//											  WORDWORD wwTexelSize )
//{
//	std::vector<VERTEXNORTEX2>	vecVertices;
//    m_pAABBRenderStrategy->GetVertices( vecVertices );
//
//    if ( vecVertices.empty() )
//        return;
//
//    // ���ؽ� ������ ��´�.
//    VERTEXNORTEX2* pVerticesVERTEX = new VERTEXNORTEX2[vecVertices.size()];
//    for ( DWORD i=0; i<vecVertices.size(); ++i )
//    {
//        pVerticesVERTEX[i] = vecVertices[i];
//    }
//
//    psvecLightMapObj->m_dwLightMapFaceNUM = vecVertices.size()/3;
//
//
//    // LightMap ������ ������ Color �� ����� 
//    {
//        NSLightMap::CreatePieceLightMapUserUV( pd3dDevice,
//										strLightMapName, 
//										_T("_P"), 
//										FALSE,
//										TRUE,
//										psvecLightMapObj->m_vecLightMapObject,
//										psvecLightMapObj->m_dwLightMapFaceNUM, 
//										reinterpret_cast<BYTE*>(pVerticesVERTEX),
//										VERTEXNORTEX2::FVF,
//										pStaticMesh,
//										pPieceManager, 
//										matWorld,
//										dwStartTextureSize,
//										fLightMapDetail,
//										NSLIGHTMAPDEF::LMT_GROUND,
//										bDontMakeShadowLM,
//										wwTexelSize );
//    }
//    
//
//    SAFE_DELETE_ARRAY( pVerticesVERTEX );
//}
//
//void DxSingleTexMesh::CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
//                                          const TSTRING& strLightMapName,
//                                          TSTRING& strTextureDay, 
//                                          TSTRING& strTextureNight,
//                                          TSTRING& strTextureAvgDay,
//                                          TSTRING& strTextureAvgNight,
//                                          SVEC_LIGHTMAPOBJ* psvecLightMapObj,
//                                          WORDWORD wwBaseTexelXY )
//{
//    // Texture ����� ����.
//    // UV ��ǥ ����
//    // DxSingleTexMesh �� ����.
//    {
//		// NSLightMap �ؽ���(m_vecTextureData) �� �����Ѵ�.
//        // ������ �����۾�.
//        NSLightMap::CreateLightMap2_PIECE( psvecLightMapObj->m_vecLightMapObject, 
//											strLightMapName, 
//											_T("_P"), 
//											FALSE,
//											TRUE,
//											strTextureDay, 
//											strTextureNight, 
//											strTextureAvgDay, 
//											strTextureAvgNight,
//											wwBaseTexelXY,
//											TRUE,
//											FALSE );
//
//        // DxSingleTexMesh - LightMap ������ �۾��ϸ� �� �� ����.
//        // LightMap �� ������ ���´�.
//        {
//            // Vertex ������
//            DWORD dwFaceStartNum(0);
//            VERTEXNORTEX2* pVertices = new VERTEXNORTEX2[ psvecLightMapObj->m_dwLightMapFaceNUM*3 ];
//            for ( DWORD j=0; j<psvecLightMapObj->m_vecLightMapObject.size(); ++j )
//            {
//                psvecLightMapObj->m_vecLightMapObject[j]->GetVerticesData( pVertices, dwFaceStartNum, dwFaceStartNum );
//            }
//
//            TSTRING strTexName = m_strTexName;
//            CreateLightMap
//            ( 
//                pd3dDevice, 
//                strTexName.c_str(), 
//                psvecLightMapObj->m_dwLightMapFaceNUM, 
//                reinterpret_cast<BYTE*>( pVertices ), // �۾�����.
//                m_dwFlags & ENSTM_CAMCOLL,
//                VERTEXNORTEX2::FVF,
//                strTextureDay,
//                strTextureNight,
//                strTextureAvgDay,
//                strTextureAvgNight,
//				FALSE
//            );
//
//            SAFE_DELETE_ARRAY( pVertices );
//        }
//    }
//}

void DxSingleTexMesh::CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
											LightMapObject* pLightMapObject,
											TSTRING& strTextureCombined, 
											TSTRING& strTextureDirect_Shadow_1,
											TSTRING& strTextureDirect_Shadow_2,
											TSTRING& strTextureIndirect,
											WORDWORD wwBaseTexelXY,
											BOOL bRan_1_0,
											float fCellSize )
{
	// Texture ����� ����.
	// UV ��ǥ ����
	// DxSingleTexMesh �� ����.
	{
		// DxSingleTexMesh - LightMap ������ �۾��ϸ� �� �� ����.
		// LightMap �� ������ ���´�.
		{
			// �޸� ����ȭ�� ���ϱ� ���� �۾�.
			std::vector<VERTEXNORCOLORTEX4> vecVertices;
			std::vector<D3DXVECTOR2>		vecLightMapUV;
			DWORD dwVertexNum(0);
			m_pAABBRenderStrategy->GetVertexNum( dwVertexNum );
			vecVertices.reserve( dwVertexNum );
			vecLightMapUV.reserve( dwVertexNum );

			// Vertex ������
			m_pAABBRenderStrategy->GetVertices( vecVertices, vecLightMapUV );
			{
				//------- UV ��ǥ�� �����Ѵ�.
				{
					pLightMapObject->CaleUV( vecLightMapUV, 0, 0 );
					pLightMapObject->SubUV( vecLightMapUV, wwBaseTexelXY );
				}
			}

			VERTEXNORCOLORTEX4* pVertices = new VERTEXNORCOLORTEX4[ vecVertices.size() ];
			for ( DWORD i=0; i<vecVertices.size(); ++i )
			{
				pVertices[i] = vecVertices[i];
			}

			TSTRING strTexName = m_strTexName;
			CreateLightMap
			( 
				pd3dDevice, 
				strTexName.c_str(),
				strTexName.c_str(),
				static_cast<DWORD> (vecVertices.size()/3), 
				reinterpret_cast<BYTE*>( pVertices ), // �۾�����.
				m_dwFlags,
				m_sMaterialValue,
				VERTEXNORCOLORTEX4::FVF,
				strTextureCombined, 
				strTextureDirect_Shadow_1,
				strTextureDirect_Shadow_2,
				strTextureIndirect,
				TRUE,
				bRan_1_0,
				fCellSize
			);

			SAFE_DELETE_ARRAY( pVertices );
		}
	}
}

void DxSingleTexMesh::GetAABBSize ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{ 
	if ( m_pAABBRenderStrategy )
    {
        m_pAABBRenderStrategy->GetAABBSize( vMax, vMin );
    }
	else
	{
		vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	}
}

void DxSingleTexMesh::ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff )
{
	// Note : ����
	SAFE_DELETE( m_pTexEffList );
	m_dwEffectFlag = 0L;

	// Note : 
	DxTextureEffMan::MAPTEXEFF* pmapTexEff = pTexEff->GetMapTexEff();
	DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->begin();
	for( ; iter!=pmapTexEff->end(); ++iter )
	{
		if( (*iter).first.c_str()!=m_strTexName )	continue;

		PBYTE pProp;
		DxTexEffBase* pNew;
		DxTexEffBase* pTexEff = (*iter).second;
		while( pTexEff )
		{
			DWORD dwTypeID = pTexEff->GetTypeID();

			pTexEff->GetProperty( pProp );
			pNew = DxTexEffBase::NEW_TEXEFF_STATIC( dwTypeID );
			pNew->SetProperty( pd3dDevice, pProp );

			pNew->m_pNext = m_pTexEffList;
			m_pTexEffList = pNew;

			m_dwEffectFlag |= dwTypeID;

			pTexEff = pTexEff->m_pNext;
		}
	}
}

void DxSingleTexMesh::FrameMove( const float fElpasedTime )
{
	DxTexEffBase* pCur = m_pTexEffList;
	while( pCur )
	{
		pCur->FrameMove( fElpasedTime );	// ������ DEF_TEXEFF_DIFFUSE	< switch �Ἥ �� >
		pCur = pCur->m_pNext;
	}
}

HRESULT DxSingleTexMesh::RenderTHREAD ( const LPDIRECT3DDEVICEQ pd3dDevice,
									   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									   const CLIPVOLUME &sCV,
									   BOOL bMPShading )
{
	if ( !m_pAABBRenderStrategy )
        return S_OK;

    // ThreadLoading�� ���� �۾�.
    //m_pAABBRenderStrategy->ThreadLoad( sCV, listLoadingData, CSLockLoading );

    // Note : Texture Eff
    BOOL bVisualMaterial = FALSE;
    if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
    {
	    DxTexEffBase* pCur = m_pTexEffList;
	    while( pCur )
	    {
		    switch( pCur->GetTypeID() )
		    {
		    case DEF_TEXEFF_VISUAL_MATERIAL:
                {
                    // World
                    D3DXMATRIX matIdentity;
	                D3DXMatrixIdentity( &matIdentity );
                    bVisualMaterial = TRUE;
			        //m_pAABBRenderStrategy->RenderTHREAD_VisualMaterial( pd3dDevice, &sCV, pCur, matIdentity );
					m_pAABBRenderStrategy->RenderCullingMeshVM
					( 
						pd3dDevice, 
						m_spCullingMeshSwap[emCullingRenderType], 
						pCur, 
						matIdentity
					);
                }
			    break;
		    };

		    pCur = pCur->m_pNext;
	    } 
    }

    if ( !bVisualMaterial )
    {
	    // Note : Texture Loading
	    if( !m_textureRes.Update() )
	    {
		    if( !m_bCheckTex )
		    {
			    if ( m_strTexName.size() )
			    {
                    m_textureRes = TextureManager::GetInstance().LoadTexture(
                        m_strTexName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_LAND,
						true,
						FALSE );
			    }
			    m_bCheckTex = TRUE;
		    }

            //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
            //if ( m_textureRes.GetTexture() )
            //{
            //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
            //    if ( emTexType == EMTT_ALPHA_HARD )
            //    {
            //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
            //        m_textureRes.GetTexture()->SetLOD( dwLOD );
            //    }
            //}
	    }
    	
	    // Note : Render
		D3DXVECTOR2 vOffset(0.f,0.f);
	    pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
        NSLightMapFX::SetBaseTexture( m_textureRes.GetTexture() );
		NSLightMapFX::SetLightMapUV_Offset( vOffset );
        NSVERTEXNORTEX_FX::SetBaseTexture( m_textureRes.GetTexture() );



		DWORD dwZWriteEnable(FALSE);
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
		if ( m_dwFlags & ENSTM_DECAL_ALPHABLEND )
		{
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		}

	    //m_pAABBRenderStrategy->Render( pd3dDevice, sCV );
		m_pAABBRenderStrategy->RenderCullingMesh( pd3dDevice, m_spCullingMeshSwap[emCullingRenderType] );

		if ( m_dwFlags & ENSTM_DECAL_ALPHABLEND )
		{
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, dwZWriteEnable );
		}



	    // Note : Texture Eff
	    DxTexEffBase* pCur = m_pTexEffList;
	    while( pCur )
	    {
		    switch( pCur->GetTypeID() )
		    {
		    case DEF_TEXEFF_DIFFUSE:
			    ((DxTexEffDiffuse*)pCur)->SetRenderBegin( pd3dDevice );
			    //m_pAABBRenderStrategy->Render( pd3dDevice, sCV );
				m_pAABBRenderStrategy->RenderCullingMesh( pd3dDevice, m_spCullingMeshSwap[emCullingRenderType] );
			    ((DxTexEffDiffuse*)pCur)->SetRenderEnd( pd3dDevice );
			    break;

		    case DEF_TEXEFF_FLOWUV:
				{
					DxAABBOctree* pAABBOctree = dynamic_cast<DxAABBOctree*>(m_pAABBRenderStrategy);
					if ( pAABBOctree )
					{
						((DxTexEffFlowUV*)pCur)->SetRenderBegin( pd3dDevice );
						//m_pAABBRenderStrategy->RenderTHREAD_FlowUV( pd3dDevice, sCV, (DxTexEffFlowUV*)pCur );
						m_spCullingMeshSwap[emCullingRenderType]->RenderOctreeEff( pd3dDevice, pCur );
						((DxTexEffFlowUV*)pCur)->SetRenderEnd( pd3dDevice );
					}
				}
			    break;

		    case DEF_TEXEFF_ROTATE:
				{
					DxAABBOctree* pAABBOctree = dynamic_cast<DxAABBOctree*>(m_pAABBRenderStrategy);
					if ( pAABBOctree )
					{
						((DxTexEffRotate*)pCur)->SetRenderBegin( pd3dDevice );
						//m_pAABBRenderStrategy->RenderTHREAD_Rotate( pd3dDevice, sCV, (DxTexEffRotate*)pCur );
						m_spCullingMeshSwap[emCullingRenderType]->RenderOctreeEff( pd3dDevice, pCur );
						((DxTexEffRotate*)pCur)->SetRenderEnd( pd3dDevice );
					}
				}
			    break;

		    case DEF_TEXEFF_SPECULAR:
			    ((DxTexEffSpecular*)pCur)->SetRenderBegin( pd3dDevice );
			    //m_pAABBRenderStrategy->Render( pd3dDevice, sCV );
				m_pAABBRenderStrategy->RenderCullingMesh( pd3dDevice, m_spCullingMeshSwap[emCullingRenderType] );
			    ((DxTexEffSpecular*)pCur)->SetRenderEnd( pd3dDevice );
			    break;
		    };

		    pCur = pCur->m_pNext;
	    }
    }
	
	return S_OK;
}

HRESULT DxSingleTexMesh::RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, 
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
										const CLIPVOLUME &sCV, 
										sc::SerialFile* const SFile )
{
	if ( !m_pAABBRenderStrategy )	return S_OK;

    // DynaminLoad
    m_pAABBRenderStrategy->DynamicLoad( pd3dDevice, sCV, SFile );

    // Note : Texture Eff
    BOOL bVisualMaterial = FALSE;
    if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
    {
	    DxTexEffBase* pCur = m_pTexEffList;
	    while( pCur )
	    {
		    switch( pCur->GetTypeID() )
		    {
		    case DEF_TEXEFF_VISUAL_MATERIAL:
                {
                    // World, View, Projection
                    D3DXMATRIX matIdentity;
	                D3DXMatrixIdentity( &matIdentity );
                    bVisualMaterial = TRUE;
			        //m_pAABBRenderStrategy->RenderTHREAD_VisualMaterial( pd3dDevice, &sCV, pCur, matIdentity );
					m_pAABBRenderStrategy->RenderCullingMeshVM
					( 
						pd3dDevice, 
						m_spCullingMeshSwap[emCullingRenderType], 
						pCur, 
						matIdentity
					);
                }
			    break;
		    };

		    pCur = pCur->m_pNext;
	    } 
    }

    if ( !bVisualMaterial )
    {
	    // Note : Texture Loading
	    if( !m_textureRes.IsValid() )
	    {
		    if( !m_bCheckTex )
		    {
			    if ( m_strTexName.size() )
			    {
                    m_textureRes = TextureManager::GetInstance().LoadTexture(
                        m_strTexName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_LAND,
						true,
						FALSE );
			    }
			    m_bCheckTex = TRUE;
		    }

            //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
            //if ( m_textureRes.GetTexture() )
            //{
            //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
            //    if ( emTexType == EMTT_ALPHA_HARD )
            //    {
            //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
            //        m_textureRes.GetTexture()->SetLOD( dwLOD );
            //    }
            //}
	    }

	    pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
        NSLightMapFX::SetBaseTexture( m_textureRes.GetTexture() );
        NSVERTEXNORTEX_FX::SetBaseTexture( m_textureRes.GetTexture() );
	    //m_pAABBRenderStrategy->Render( pd3dDevice, sCV );
		m_pAABBRenderStrategy->RenderCullingMesh( pd3dDevice, m_spCullingMeshSwap[emCullingRenderType] );
    }
	return S_OK;
}

HRESULT DxSingleTexMesh::FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile )
{
	if ( !m_pAABBRenderStrategy )	return S_OK;

	// Note : Texture Loading
	if( !m_textureRes.Update() )
	{
		if( !m_bCheckTex )
		{
			if ( m_strTexName.size() )
			{
                m_textureRes = TextureManager::GetInstance().LoadTexture(
                    m_strTexName,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    0,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_NONE,
                    TEXTURE_LAND,
					false,
					FALSE );

                //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
                //if ( m_textureRes.GetTexture() )
                //{
                //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
                //    if ( emTexType == EMTT_ALPHA_HARD )
                //    {
                //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
                //        m_textureRes.GetTexture()->SetLOD( dwLOD );
                //    }
                //}
			}
			m_bCheckTex = TRUE;
		}
	}

	//pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
	//m_pAABBRenderStrategy->FirstLoad( pd3dDevice, vMax, vMin, SFile );
	
	return S_OK;
}

HRESULT DxSingleTexMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV )
{
	if ( !m_pAABBRenderStrategy )	return S_OK;

	PROFILE_BEGIN("DxSingleTexMesh::Render_2");
	{
		// Note : Texture Loading
		if( !m_textureRes.Update() )
		{
			if( !m_bCheckTex )
			{
				if ( m_strTexName.size() )
				{
                    m_textureRes = TextureManager::GetInstance().LoadTexture(
                        m_strTexName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_LAND,
						true,
						FALSE );
				}
				m_bCheckTex = TRUE;
			}

            //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
            //if ( m_textureRes.GetTexture() )
            //{
            //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
            //    if ( emTexType == EMTT_ALPHA_HARD )
            //    {
            //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
            //        m_textureRes.GetTexture()->SetLOD( dwLOD );
            //    }
            //}
		}

        NSLightMapFX::SetBaseTexture( m_textureRes.GetTexture() );
        NSVERTEXNORTEX_FX::SetBaseTexture( m_textureRes.GetTexture() );

		//m_pAABBRenderStrategy->Render( pd3dDevice, sCV );
		m_pAABBRenderStrategy->RenderCullingMesh( pd3dDevice, m_spCullingMeshSwap[emCullingRenderType] );
	}
	PROFILE_END("DxSingleTexMesh::Render_2");
	
	return S_OK;
}

void DxSingleTexMesh::RenderPiece( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                    DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                                    const D3DXMATRIX& matWorld, 
                                    BOOL bRealPL, 
                                    const D3DXVECTOR2& vLightMapUV_Offset,
                                    float fAlpha, 
									LPDIRECT3DTEXTUREQ pTextureDay, 
									LPDIRECT3DTEXTUREQ pTextureNight )
{
	if ( !m_pAABBRenderStrategy )	return;

    // Note : Texture Eff
    BOOL bVisualMaterial = FALSE;
    if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
    {
        if ( pmapTexEff )
        {
	        DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->find( m_strTexName.c_str() );
	        if( iter!=pmapTexEff->end() )
	        {
		        DxTexEffBase* pCur = (*iter).second;
		        while( pCur )
		        {
			        switch( pCur->GetTypeID() )
			        {
			        case DEF_TEXEFF_VISUAL_MATERIAL:
                        bVisualMaterial = TRUE;
				        m_pAABBRenderStrategy->RenderPiece_VisualMaterial( pd3dDevice, pCur, bRealPL, matWorld, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );

				        break;
			        };

			        pCur = pCur->m_pNext;
		        } 
            }
        }
    }

    if ( !bVisualMaterial )
    {
	    // Note : Texture Loading
	    if( !m_textureRes.Update() )
	    {
		    if( !m_bCheckTex )
		    {
			    if ( m_strTexName.size() )
			    {
                    m_textureRes = TextureManager::GetInstance().LoadTexture(
                        m_strTexName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_LAND,
						true,
						FALSE );
			    }
			    m_bCheckTex = TRUE;
		    }

            //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
            //if ( m_textureRes.GetTexture() )
            //{
            //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
            //    if ( emTexType == EMTT_ALPHA_HARD )
            //    {
            //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
            //        m_textureRes.GetTexture()->SetLOD( dwLOD );
            //    }
            //}
	    }

	    pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
        NSLightMapFX::SetBaseTexture( m_textureRes.GetTexture() );
        NSLightMapFX::SetLightMapUV_Offset( vLightMapUV_Offset );
        NSVERTEXNORTEX_FX::SetBaseTexture( m_textureRes.GetTexture() );
        {
	        m_pAABBRenderStrategy->RenderPiece( pd3dDevice, pTextureDay, pTextureNight );
        }
        NSLightMapFX::SetLightMapUV_Offset( D3DXVECTOR2(0.f,0.f) );

	    if( !pmapTexEff )					return;	// TexEff ��ü�� ���ٸ� ����.
	    if( !(m_dwEffectFlag&DEF_TEXEFF_ALL) )	return;	// ���õ� ȿ���� ���ٸ� ����.

	    // Note : Texture Eff
	    DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->find( m_strTexName.c_str() );
	    if( iter!=pmapTexEff->end() )
	    {
		    DxTexEffBase* pCur = (*iter).second;
		    while( pCur )
		    {
			    switch( pCur->GetTypeID() )
			    {
			    case DEF_TEXEFF_DIFFUSE:
				    ((DxTexEffDiffuse*)pCur)->SetRenderBegin( pd3dDevice );
				    m_pAABBRenderStrategy->RenderPiece( pd3dDevice, pTextureDay, pTextureNight );
				    ((DxTexEffDiffuse*)pCur)->SetRenderEnd( pd3dDevice );
				    break;
			    case DEF_TEXEFF_FLOWUV:
				    ((DxTexEffFlowUV*)pCur)->SetRenderBegin( pd3dDevice );
				    m_pAABBRenderStrategy->Render_FlowUV( pd3dDevice, (DxTexEffFlowUV*)pCur );
				    ((DxTexEffFlowUV*)pCur)->SetRenderEnd( pd3dDevice );
				    break;
			    case DEF_TEXEFF_ROTATE:
				    ((DxTexEffRotate*)pCur)->SetRenderBegin( pd3dDevice );
				    m_pAABBRenderStrategy->Render_Rotate( pd3dDevice, (DxTexEffRotate*)pCur );
				    ((DxTexEffRotate*)pCur)->SetRenderEnd( pd3dDevice );
				    break;
			    case DEF_TEXEFF_SPECULAR:
				    ((DxTexEffSpecular*)pCur)->SetRenderBegin( pd3dDevice );
				    m_pAABBRenderStrategy->RenderPiece( pd3dDevice, pTextureDay, pTextureNight );
				    ((DxTexEffSpecular*)pCur)->SetRenderEnd( pd3dDevice );
				    break;
			    };

			    pCur = pCur->m_pNext;
		    } 
	    }
    }
}

HRESULT DxSingleTexMesh::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pAABBRenderStrategy )
		return S_OK;

	m_pAABBRenderStrategy->Render_Query( pd3dDevice, sCV, FALSE );
	return S_OK;
}

VOID DxSingleTexMesh::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pAABBRenderStrategy )
		return;

	m_pAABBRenderStrategy->CreateQuery( pd3dDevice );
}

VOID DxSingleTexMesh::DestroyQuery()
{
	if ( !m_pAABBRenderStrategy )
		return;

	m_pAABBRenderStrategy->DestroyQuery();
}

void DxSingleTexMesh::ResetOcclusionQuery()
{
	if ( !m_pAABBRenderStrategy )
		return;

	m_pAABBRenderStrategy->ResetOcclusionQuery();
}

void DxSingleTexMesh::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	if ( !m_pAABBRenderStrategy )
		return;

	m_pAABBRenderStrategy->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
}

// TexEff �� �����´�.
BOOL DxSingleTexMesh::GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, VEC_TEXEFF& vecTexEff )
{
    BOOL bEnable(FALSE);
    PBYTE pProp;
    DxTexEffBase* pCur = m_pTexEffList;
	while( pCur )
	{
        bEnable = TRUE;

        std::tr1::shared_ptr<DxTexEffBase> spNew = std::tr1::shared_ptr<DxTexEffBase>( DxTexEffBase::NEW_TEXEFF_STATIC(pCur->GetTypeID()) );
        pCur->GetProperty( pProp );
        spNew->SetProperty( pd3dDevice, pProp );

        vecTexEff.push_back( spNew );

		pCur = pCur->m_pNext;
	}

    return bEnable;
}

// TexEff�� ����Ѵ�.
void DxSingleTexMesh::SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const VEC_TEXEFF& vecTexEff )
{
    // ���� �����ʹ� ������.
    SAFE_DELETE( m_pTexEffList );

    // ���ο� ������ �����.
    DxTexEffBase*	pTexEffBase;
	PBYTE           pProp;
	for( DWORD i=0; i<vecTexEff.size(); ++i )
    {
		// Note : ��������.
		pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( vecTexEff[i]->GetTypeID(), pd3dDevice, NULL );

		// Note : 
        vecTexEff[i]->GetProperty( pProp );
        pTexEffBase->SetProperty( pd3dDevice, pProp );

		// Note : ����
		pTexEffBase->m_pNext = m_pTexEffList;
		m_pTexEffList = pTexEffBase;
	}
}

//void DxSingleTexMesh::InsertColorList(LOADINGDATALIST& listColorData)
//{
//	if (m_pAABBRenderStrategy)
//	    m_pAABBRenderStrategy->InsertColorList(listColorData);
//}

BOOL DxSingleTexMesh::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if ( !m_pAABBRenderStrategy )
		return FALSE;

	if ( m_pAABBRenderStrategy->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
	{
		return TRUE;
	}
	else
		return FALSE;
}

BOOL DxSingleTexMesh::IsCollisionLine(
    const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
    LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if ( !m_pAABBRenderStrategy )	return FALSE;

	if ( bOnlyCamColl )
	{
		if ( !(m_dwFlags&ENSTM_CAMCOLL) ) 
			return FALSE;
	}

	if ( m_pAABBRenderStrategy->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode ) )
	{
		szName = m_strTexName.c_str();
		return TRUE;
	}
	else	return FALSE;
}

BOOL DxSingleTexMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if ( !m_pAABBRenderStrategy )	return FALSE;
	
	if ( bOnlyCamColl )
	{
		if ( !(m_dwFlags&ENSTM_CAMCOLL) ) 
			return FALSE;
	}

	if ( m_pAABBRenderStrategy->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode ) )	return TRUE;
	else																			        return FALSE;
}

// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
BOOL DxSingleTexMesh::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
    if ( !m_pAABBRenderStrategy )	
        return FALSE;

	//// Matrerial �� �ִٸ� �װͿ� ������. �׿ܴ� Tool ���� ������ ���� ������.
	//int nCastShadow = NSMaterialManager::IsCastShadowLightMap( m_strTexName );
	//if ( nCastShadow == 0 )
	//{
	//	return FALSE;
	//}
	//else if ( nCastShadow == -1 )
	//{
	//	if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
	//		return FALSE;
	//	if ( m_dwFlags & ENSTM_DECAL_FULL )
	//		return FALSE;
	//}

	// ����� Tool ���� ������ ���� �״�� �������� �Ѵ�.
	{
		if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
			return FALSE;
		if ( m_dwFlags & ENSTM_DECAL_FULL )
			return FALSE;
	}
	
	NSTexture16Bit::DATA* pData16Bit(NULL);
	if ( m_textureRes.GetAlphaType() != EMTT_NORMAL )
	{
		// Note : UV�� ��ǥ�� ���Ѵ�.

		if ( m_textureRes.GetTexture_const() )
		{
			pData16Bit = NSTexture16Bit::Insert( pd3dDevice, m_textureRes.GetFileName(), m_textureRes.GetTexture() );
		}
	}

	if ( m_pAABBRenderStrategy->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )	return TRUE;
	else																												return FALSE;
}

void DxSingleTexMesh::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // �ֺ��� �߰� �޼ҵ�
{
	if ( !m_pAABBRenderStrategy ) return;

	m_pAABBRenderStrategy->GetCollisionLineInTriListExceptY(vStart, vEnd, TriList);
}

BOOL DxSingleTexMesh::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	if ( !m_pAABBRenderStrategy ) return FALSE;

	//LightMapRenderMan* pLightMapRenderMan = dynamic_cast<LightMapRenderMan*>( m_pAABBRenderStrategy );
	//if ( pLightMapRenderMan )
	//{
	//	return pLightMapRenderMan->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor );
	//}

	return FALSE;
}

BOOL DxSingleTexMesh::IsLightMap()
{
	if ( !m_pAABBRenderStrategy ) 
		return FALSE;

	LightMapRenderMan* pLightMapRenderMan = dynamic_cast<LightMapRenderMan*>( m_pAABBRenderStrategy );
	if ( pLightMapRenderMan )
	{
		return TRUE;
	}

	return FALSE;
}

void DxSingleTexMesh::Save( sc::SerialFile& SFile, BOOL bPiece )
{
	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << m_strTexName;
		SFile << m_dwEffectFlag;
		SFile << m_dwFlags;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize=0;
			DxTexEffBase* pCur = m_pTexEffList;
			while( pCur )
			{
				++dwSize;
				pCur = pCur->m_pNext;
			}
			SFile << dwSize;

			pCur = m_pTexEffList;
			while( pCur )
			{
				SFile << pCur->GetTypeID();

				pCur->SavePSF( SFile );
				pCur = pCur->m_pNext;
			}
		}
	}
	SFile.EndBlock();

	BOOL bExist;
	bExist = m_pAABBRenderStrategy ? TRUE : FALSE;
	SFile << bExist;
	if ( m_pAABBRenderStrategy )
    {
        SFile << static_cast<DWORD>( m_pAABBRenderStrategy->GetTypeRender() );
        m_pAABBRenderStrategy->Save( SFile, bPiece );
    }
}

void DxSingleTexMesh::Load ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece, BOOL bDummy )
{
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_strTexName; 
		SFile >> m_dwEffectFlag;
		SFile >> m_dwFlags;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : ��������.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : ����
					pTexEffBase->m_pNext = m_pTexEffList;
					m_pTexEffList = pTexEffBase;
				}
				else
				{
					SAFE_DELETE( pTexEffBase );
				}
			}
		}
	}
	else if( dwVer==0x0105 )
	{
		DWORD bCamColl(FALSE);
		DWORD bDontCastShadowLM(FALSE);

		SFile >> m_strTexName; 
		SFile >> m_dwEffectFlag;
		SFile >> bCamColl;
		SFile >> bDontCastShadowLM;

		// Note : ī�޶� �浹���θ� �����Ѵ�.
		if ( bCamColl )
		{
			m_dwFlags |= ENSTM_CAMCOLL;
		}
		else
		{
			m_dwFlags &= ~ENSTM_CAMCOLL;
		}

		// LightMap �׸��ڸ� �����ϴ� ��ü���� ���ܽ�Ű�� TRUE
		if ( bDontCastShadowLM )
		{
			m_dwFlags |= ENSTM_DONT_CAST_SHADOW_LM;
		}
		else
		{
			m_dwFlags &= ~ENSTM_DONT_CAST_SHADOW_LM;
		}

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : ��������.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : ����
					pTexEffBase->m_pNext = m_pTexEffList;
					m_pTexEffList = pTexEffBase;
				}
				else
				{
					SAFE_DELETE( pTexEffBase );
				}
			}
		}
	}
	else if( dwVer==0x0104 || dwVer==0x0103 )	//	������ ���� ���� ����
	{
		DWORD bCamColl(FALSE);

		SFile >> m_strTexName; 
		SFile >> m_dwEffectFlag;
		SFile >> bCamColl;

		// Note : ī�޶� �浹���θ� �����Ѵ�.
		if ( bCamColl )
		{
			m_dwFlags |= ENSTM_CAMCOLL;
		}
		else
		{
			m_dwFlags &= ~ENSTM_CAMCOLL;
		}

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : ��������.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : ����
					pTexEffBase->m_pNext = m_pTexEffList;
					m_pTexEffList = pTexEffBase;
				}
				else
				{
					SAFE_DELETE( pTexEffBase );
				}
			}
		}
	}
	else if ( dwVer==0x0102 )
	{
		SFile >> m_strTexName;
		SFile >> m_dwEffectFlag;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : ��������.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : ����
					pTexEffBase->m_pNext = m_pTexEffList;
					m_pTexEffList = pTexEffBase;
				}
				else
				{
					SAFE_DELETE( pTexEffBase );
				}
			}
		}
	
	}
	else if( dwVer==0x0101 )
	{
		SFile >> m_dwEffectFlag;
		SFile >> m_strTexName;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : ��������.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : ����
					pTexEffBase->m_pNext = m_pTexEffList;
					m_pTexEffList = pTexEffBase;
				}
				else
				{
					SAFE_DELETE( pTexEffBase );
				}
			}
		}
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_strTexName;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

    if( dwVer > 0x0103 )
	{
		// Note : m_pAABBRenderStrategy���� Version üũ�� �Ͼ��.
	    BOOL bExist;
	    SFile >> bExist;
	    if ( bExist )
	    {
            DWORD dwTemp;
            SFile >> dwTemp;    
            AABBRenderStrategy::TYPE_RENDER emTypeRender = static_cast<AABBRenderStrategy::TYPE_RENDER>( dwTemp );

		    m_pAABBRenderStrategy = AABBRenderStrategy::CreateFactory( emTypeRender );
		    m_pAABBRenderStrategy->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
	    }
	}
    else
    {
        // Note : m_pAABBRenderStrategy���� Version üũ�� �Ͼ��.
	    BOOL bExist;
	    SFile >> bExist;
	    if ( bExist )
	    {
		    m_pAABBRenderStrategy = new DxAABBOctree;
		    m_pAABBRenderStrategy->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
	    }
    }

	//// Field�� ������ �ѹ��� �ؾ��Ѵ�. Piece �� �� �ʿ� ����.
	//// ���� �����ϰ��� �ϸ� �ȵȴ�.
	//if ( !bPiece && !bDummy )
	//{
	//	// Thread �� DxCullingMeshForThread ������ �Ѱ��ֵ��� �Ѵ�.
	//	CreateDxCullingMeshSwap();
	//}

	// [shhan][2014.11.11] �׷���Ƽ�� ��� ZWrite �� �ϸ� �ȵǾ�����.
	//						���� Render Model �� ��� ������ ������ ���� ���� �̷��� ������.
	//						1city_sportzone.wld(��) �� �׷���Ƽ���� ������ �Ǿ�����.
	if ( m_strTexName == _T("city1_road_graffity02.dds") ||
		m_strTexName == _T("city1_road_graffity03.dds") ||
		m_strTexName == _T("city1_road_graffity04.dds") ||
		m_strTexName == _T("city1_brock_graffity01.dds") ||
		m_strTexName == _T("city1_brock_graffity02.dds") ||
		m_strTexName == _T("city1_brock_graffity04.dds") ||
		m_strTexName == _T("city1_brock_graffity05.dds") )
	{
		m_dwFlags |= ENSTM_DECAL_ALPHABLEND;
	}
}

void DxSingleTexMesh::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDummy )
{
	CleanUp();

	std::string szName;

	m_pAABBRenderStrategy = new DxAABBOctree;
	m_pAABBRenderStrategy->Load_VER100 ( pd3dDevice, SFile, szName );

	m_strTexName = szName.c_str();

	
	//// �����ϰ��� �ϸ� �ȵȴ�.
	//if ( !bDummy )
	//{
	//	// Thread �� DxCullingMeshForThread ������ �Ѱ��ֵ��� �Ѵ�.
	//	CreateDxCullingMeshSwap();
	//}
}

void DxSingleTexMesh::CreateDxCullingMeshSwap()
{
	// Thread �� DxCullingMeshForThread ������ �Ѱ��ֵ��� �Ѵ�.
	if ( m_pAABBRenderStrategy )
	{
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			m_spCullingMeshSwap[i] = boost::shared_ptr<DxCullingMeshSwap>(new DxCullingMeshSwap(m_pAABBRenderStrategy->GetAABBBinaryBase()));
			NSLandThread::AddCullingMesh( static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), m_spCullingMeshSwap[i] );
		}
	}
}

BOOL DxSingleTexMesh::IsLightMapPiece()
{
    if ( m_pAABBRenderStrategy && (m_pAABBRenderStrategy->GetTypeRender() == AABBRenderStrategy::TYPE_LIGHTMAP) )
    {
        return TRUE;
    }

    return FALSE;
}

void DxSingleTexMesh::SaveLightMapPiece( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION_LM );

    SFile.BeginBlock();
    {
        BOOL bExist(FALSE);
        if ( m_pAABBRenderStrategy && (m_pAABBRenderStrategy->GetTypeRender() == AABBRenderStrategy::TYPE_LIGHTMAP) )
        {
            bExist = TRUE;
        }

        SFile << bExist;
        if ( m_pAABBRenderStrategy )
        {
            SFile << static_cast<DWORD>( m_pAABBRenderStrategy->GetTypeRender() );
            m_pAABBRenderStrategy->Save( SFile, FALSE );
        }
    }
    SFile.EndBlock();
}

void DxSingleTexMesh::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

    BOOL bExist;
    SFile >> bExist;
    if ( bExist )
    {
        DWORD dwTemp;
        SFile >> dwTemp;    
        AABBRenderStrategy::TYPE_RENDER emTypeRender = static_cast<AABBRenderStrategy::TYPE_RENDER>( dwTemp );

	    m_pAABBRenderStrategy = AABBRenderStrategy::CreateFactory( emTypeRender );
	    m_pAABBRenderStrategy->Load( pd3dDevice, SFile, FALSE, TRUE );
    }
}

void DxSingleTexMesh::FlipLightMapRenderMan( DxSingleTexMesh* pSingleTexMesh )
{
    if ( pSingleTexMesh->m_pAABBRenderStrategy )
    {
        SAFE_DELETE( m_pAABBRenderStrategy );
        m_pAABBRenderStrategy = pSingleTexMesh->m_pAABBRenderStrategy;
        pSingleTexMesh->m_pAABBRenderStrategy = NULL;
    }
}

void DxSingleTexMesh::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
    // ������ �ε���Ų��.
    if ( !m_textureRes.Update() )
    {
        if ( m_strTexName.size() )
	    {
            m_textureRes = TextureManager::GetInstance().LoadTexture(
                m_strTexName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_LAND,
				false,
				FALSE );

            //// AlphaHard �� ���� �Ӹ� ������ ���δ�.
            //if ( m_textureRes.GetTexture() )
            //{
            //    TEXTURE_ALPHA emTexType = m_textureRes.GetAlphaType();
            //    if ( emTexType == EMTT_ALPHA_HARD )
            //    {
            //        DWORD dwLOD = new_min( MIPMAPLEVEL, m_textureRes.GetTexture()->GetLOD() );
            //        m_textureRes.GetTexture()->SetLOD( dwLOD );
            //    }
            //}
	    }
	    m_bCheckTex = TRUE;
    }

    //if ( m_textureRes.GetTexture() )
    //{
    //    // Note : �̷��� ���ִ� ���� �� ��ȯ �ð��� ���� ���� �ɸ��� �����̴�.
    //    if( !m_pTexture32BIT )
    //    {
	   //     // Note : 32 bit �� ��ȯ��Ų��.
	   //     D3DSURFACE_DESC sDesc;
	   //     m_textureRes.GetTexture()->GetLevelDesc( 0, &sDesc );

	   //     LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
	   //     NSTexCompress::Compress
	   //     (
		  //      pd3dDevice,
		  //      sDesc.Width,
		  //      sDesc.Height,
		  //      0,
		  //      1,
		  //      D3DFMT_A8R8G8B8,
		  //      m_textureRes.GetTexture(),
		  //      pTexNew
	   //     );

	   //     // Note : �� ������ �ٲ��־���. ���߿� ť����̳� �׷� ��� ������ ����� �ִ�.
	   //     m_pTexture32BIT = static_cast<LPDIRECT3DTEXTURE9>( pTexNew );
    //    }
    //}
}

void DxSingleTexMesh::GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance )
{
    if ( !m_pAABBRenderStrategy )	
	{
		return;
	}

	m_pAABBRenderStrategy->GetClosedPoint( vBasePos, vCollisionOut, fDistance );
}

void DxSingleTexMesh::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	// Mesh ������ ���´�.
	std::vector<VERTEXNORCOLORTEX4>	vecVertices;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	std::vector<BOOL>				vecReceiveShadowLM;
	GetVertices( vecVertices, vecLightMapUV, vecReceiveShadowLM );

	NSExportXFile::ExportDxStaticMesh_SingleTexMesh
	( 
		pxofsave, 
		pDataObjectRoot, 
		vecVertices,
		strPieceName, 
		m_strTexName,
		matWorld 
	);
}

void RenderRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
					 DxLandMan* pLandMan,
					 LPDIRECT3DTEXTUREQ pTextureSystem, 
					 const TSTRING& strLightMapName,
					 int nRadiosityPassCUR,				// ���� �������� Pass
					 float fRadiosityReflectance,
					 float fRadiosityFogBlend,
					 DWORD dwRadiosityFogLength )
{

}