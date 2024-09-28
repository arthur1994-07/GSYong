#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../Common/StlFunctions.h"
#include "../DxPiece/NsOCTree.h"
#include "../DxCullingMeshForThread.h"
#include "./LightMapRenderMat.h"
#include "./LightMapRender.h"
#include "./NSLightMapFX.h"

#include "LightMapRenderMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


LightMapRenderMan::LightMapRenderMan()
	: m_dwFVF(0L)
    , m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
    , m_strTextureDay(_T(""))
	, m_strTextureNight(_T(""))
	, m_pLightMapTree(NULL)
{
    m_emTypeRender = AABBRenderStrategy::TYPE_LIGHTMAP;
}

LightMapRenderMan::~LightMapRenderMan()
{
	CleanUp();
}

void LightMapRenderMan::CleanUp()
{
    m_textureResDay.Clear();
    m_textureResNight.Clear();

	NSTexture16Bit::CleanUp();

    SAFE_DELETE ( m_pLightMapTree );
}

AABBBinaryTreeBase* LightMapRenderMan::GetAABBBinaryBase()
{
	return m_pLightMapTree;
}

int LightMapRenderMan::Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
                               const DWORD dwFaceNUM, 
                               BYTE* pVertices, 
                               const DWORD dwFVF, 
                               EM_LAND_TYPE emLandType, 
							   BOOL bUserLightMapUV,
							   BOOL bRan_1_0,
							   AABB_BINARY_TREE_TYPE emType,
							   float fCellSize )
{
    

    // 데이터 복제
    {
        // Note : 처음 다 삭제
	    CleanUp();

	    // Note : 정점 데이터 얻기
	    if ( !pVertices )	
            return 0;

        m_dwFVF = dwFVF;

	    UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	    for ( DWORD i=0; i<dwFaceNUM*3; ++i )
	    {
		    D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertices + nVERSIZE*i );

		    if ( m_vMax.x < pVecter->x )	m_vMax.x = pVecter->x;
		    if ( m_vMax.y < pVecter->y )	m_vMax.y = pVecter->y;
		    if ( m_vMax.z < pVecter->z )	m_vMax.z = pVecter->z;

		    if ( m_vMin.x > pVecter->x )	m_vMin.x = pVecter->x;
		    if ( m_vMin.y > pVecter->y )	m_vMin.y = pVecter->y;
		    if ( m_vMin.z > pVecter->z )	m_vMin.z = pVecter->z;
	    }

	    // Note : Octree 형식으로 분할
		switch ( emType )
		{
		case ABTT_LIGHTMAP:
			{
				LightMapRender*	pLightMapTree;
				NSOCTREE::MakeOctree( &pLightMapTree, pVertices, dwFaceNUM, fCellSize, 20000, nVERSIZE );	// Fog 거리에 2배 정도가 적당하다.
				m_pLightMapTree = pLightMapTree;
			}
			break;
		case ABTT_MAT:
			{
				LightMapRenderMat*	pLightMapTree;
				NSOCTREE::MakeOctree( &pLightMapTree, pVertices, dwFaceNUM, fCellSize, 20000, nVERSIZE );	// Fog 거리에 2배 정도가 적당하다.
				m_pLightMapTree = pLightMapTree;
			}
			break;
		};

	    // Note : OcMesh 만들기
	    NSOCTREE::MakeOcMesh( pd3dDevice, m_pLightMapTree, m_dwFVF );

		// Note : Ran_1_0 일 경우만, 원하는 FVF 로 변경
		if ( bRan_1_0 )
		{
			if ( m_dwFVF & D3DFVF_DIFFUSE )
			{
				NSOCTREE::SetFVF( pd3dDevice, m_pLightMapTree, VERTEXNORCOLORTEX2::FVF );
			}
			else
			{
				NSOCTREE::SetFVF( pd3dDevice, m_pLightMapTree, VERTEXNORTEX2::FVF );
			}
		}
		else
		{
			NSOCTREE::SetFVF( pd3dDevice, m_pLightMapTree, m_dwFVF );
		}

	    // Note : OcMesh 최적화
	    NSOCTREE::MakeOptimizeMesh( pd3dDevice, m_pLightMapTree, TRUE );

		// Note : Ran_1_0 이 아닐 경우만, MaterialMesh 를 만든다.
		if ( !bRan_1_0 && emType == ABTT_MAT )
		{
			NSOCTREE::MakeMaterialMesh( pd3dDevice, m_pLightMapTree );
		}

	    // Note : Collision 만들기
	    NSOCTREE::MakeCollisionData( pd3dDevice, m_pLightMapTree );

        // Load 다 되있다고 설정하자. 안하면 기본 파일이 없어서 로딩완료를 알지 못한다.
	    NSOCTREE::MakeLoadCheckOK( m_pLightMapTree );
    }

	return 1;
}

void LightMapRenderMan::SetTextureName( const TSTRING& strTextureCombined,
									   const TSTRING& strTextureDirect_Shadow_1,
									   const TSTRING& strTextureDirect_Shadow_2,
									   const TSTRING& strTextureIndirect )
{
    // 텍스쳐 셋팅과
    m_strTextureDay = strTextureCombined;
    m_strTextureNight = strTextureDirect_Shadow_1;
    //m_strTextureIndirect = strTextureIndirect;
}

void LightMapRenderMan::LoadTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_textureResDay = TextureManager::GetInstance().LoadTexture(
        m_strTextureDay,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_LAND,
		false,
		FALSE );

    m_textureResNight = TextureManager::GetInstance().LoadTexture(
        m_strTextureNight,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_LAND,
		false,
		FALSE );
}

//void LightMapRenderMan::ReleaseLightMap()
//{
//	TextureManager::GetInstance().DeleteData( m_strTextureDay );
//	TextureManager::GetInstance().DeleteData( m_strTextureNight );
//}

void LightMapRenderMan::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax; vMin = m_vMin;
}

void LightMapRenderMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( m_dwFVF );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	
    if ( GLPeriod::GetInstance().GetBlendFact() > 0.5f )
    {
        pd3dDevice->SetTexture( 1, m_textureResDay.GetTexture() );
    }
    else
    {
        pd3dDevice->SetTexture( 1, m_textureResNight.GetTexture() );
    }
    NSLightMapFX::SetWorld( matIdentity, 1.f );
    NSLightMapFX::SetLightMapTexture( m_textureResDay.GetTexture(), m_textureResNight.GetTexture() );

    DWORD dwColorOP_0(0L);
    DWORD dwAlphaOP_0(0L);
    pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,    &dwColorOP_0 );
    pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,    &dwAlphaOP_0 );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,  D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_MODULATE2X );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,  D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_SELECTARG2 );

    //pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pLightMapTree );

	pd3dDevice->SetTexture( 1, NULL );

    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    dwColorOP_0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    dwAlphaOP_0 );

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
}

void LightMapRenderMan::RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread )
{
	DWORD dwFVF(0L);
	pd3dDevice->GetFVF( &dwFVF );
	pd3dDevice->SetFVF( m_dwFVF );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	if ( GLPeriod::GetInstance().GetBlendFact() > 0.5f )
	{
		pd3dDevice->SetTexture( 1, m_textureResDay.GetTexture() );
	}
	else
	{
		pd3dDevice->SetTexture( 1, m_textureResNight.GetTexture() );
	}
	NSLightMapFX::SetWorld( matIdentity, 1.f );
	NSLightMapFX::SetLightMapTexture( m_textureResDay.GetTexture(), m_textureResNight.GetTexture() );

	DWORD dwColorOP_0(0L);
	DWORD dwAlphaOP_0(0L);
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,    &dwColorOP_0 );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,    &dwAlphaOP_0 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,  D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_MODULATE2X );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,  D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_SELECTARG2 );

	//pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
	//NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pLightMapTree );
	spCullingMeshForThread->RenderOctree( pd3dDevice );

	pd3dDevice->SetTexture( 1, NULL );

	pd3dDevice->SetFVF( dwFVF );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    dwColorOP_0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    dwAlphaOP_0 );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
}

// Piece 쪽이다.
void LightMapRenderMan::RenderPiece( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( m_dwFVF );

    if ( GLPeriod::GetInstance().GetBlendFact() > 0.5f )
    {
        pd3dDevice->SetTexture( 1, pTextureDay );
    }
    else
    {
        pd3dDevice->SetTexture( 1, pTextureNight );
    }
    NSLightMapFX::SetLightMapTexture( pTextureDay, pTextureNight );

    DWORD dwColorOP_0(0L);
    pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,    &dwColorOP_0 );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,  D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_MODULATE2X );

	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderList( pd3dDevice, m_pLightMapTree );

	pd3dDevice->SetTexture( 1, NULL );

    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    dwColorOP_0 );

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,    D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
}

void LightMapRenderMan::RenderTHREAD_VisualMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                                    const CLIPVOLUME* pCV, 
                                                    DxTexEffBase* pEff, 
                                                    const D3DXMATRIX& matWorld )
{
	LPDIRECT3DTEXTUREQ pTextureDay = m_textureResDay.GetTexture();
	LPDIRECT3DTEXTUREQ pTextureNight = m_textureResNight.GetTexture();

	MATERIAL_DATA sMaterial_Data;
    NSOCTREE::InitRenderOctree
    ( 
        pd3dDevice, 
        NULL, 
        &matWorld, 
        pEff, 
		sMaterial_Data,
		pTextureDay, 
		pTextureNight,
        D3DXVECTOR2(0.f,0.f),
        1.f,
		FALSE
    );
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderOctree_VisualMaterial( m_pLightMapTree );
}

void LightMapRenderMan::RenderCullingMeshVM( const LPDIRECT3DDEVICEQ pd3dDevice, 
											boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread,
											DxTexEffBase* pEff, 
											const D3DXMATRIX& matWorld )
{
	LPDIRECT3DTEXTUREQ pTextureDay = m_textureResDay.GetTexture();
	LPDIRECT3DTEXTUREQ pTextureNight = m_textureResNight.GetTexture();

	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
	spCullingMeshForThread->RenderVisualMaterial
	(
		pd3dDevice,
		pEff, 
		matWorld,
		D3DXVECTOR2(0.f,0.f),
		1.f,
		pTextureDay,
		pTextureNight
	);
}

void LightMapRenderMan::RenderPiece_VisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                                    DxTexEffBase* pEff, 
                                                    BOOL bRealPL, 
                                                    const D3DXMATRIX& matWorld, 
                                                    const D3DXVECTOR2& vLightMapUV_Offset,
													float fAlpha, 
													LPDIRECT3DTEXTUREQ pTextureDay, 
													LPDIRECT3DTEXTUREQ pTextureNight )
{
	MATERIAL_DATA sMaterial_Data;
    NSOCTREE::InitRenderOctree
    ( 
        pd3dDevice, 
        NULL, 
        &matWorld, 
        pEff, 
		sMaterial_Data,
		pTextureDay, 
		pTextureNight,
        vLightMapUV_Offset,
		fAlpha,
		FALSE
    );
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderOctree_VisualMaterial_NoneClipVolume( m_pLightMapTree );
}

void LightMapRenderMan::RenderMaterialTnL( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	NSOCTREE::RenderMaterial( pd3dDevice, pCV, m_pLightMapTree );

	// 여기서 Tree 가 되어진 것을 렌더링 해야한다.
}

void LightMapRenderMan::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, BOOL bAlphaTex )
{
	NSOCTREE::RenderCascadeShadow( pd3dDevice, pCV, m_pLightMapTree, bAlphaTex );
}

void LightMapRenderMan::RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pLightMapTree )
		return;

	NSOCTREE::RenderCascadeShadow_Instancing( pd3dDevice, m_pLightMapTree, bAlphaTex );
}

void LightMapRenderMan::RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
									   const CLIPVOLUME* pCV, 
									   const MATERIAL_DATA& sMaterial_Data )	// Material 은 외부의 LightMap 정보를 읽어서 사용한다.
{
	// 유효성 검사는 더 상단에서 해주도록 하자.
	//if ( !sMaterial_Data.m_rMaterial->m_pFX )
	//	return;

	NSOCTREE::InitRenderOctree
	( 
		pd3dDevice, 
		pCV, 
		NULL, 
		NULL, 
		sMaterial_Data,
		NULL, 
		NULL,
		D3DXVECTOR2(0.f,0.f),
		1.f,
		FALSE
	);

	NSOCTREE::RenderMaterial( m_pLightMapTree );
}

void LightMapRenderMan::RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice,
														ID3DXEffect* pFX,
														const MATERIAL_DATA& sMaterial_Data )	// Material 은 외부의 LightMap 정보를 읽어서 사용한다.
{
	if ( !m_pLightMapTree )
		return;

	NSOCTREE::RenderPieceMaterial_Instancing( m_pLightMapTree, pd3dDevice, pFX, sMaterial_Data );
}

void LightMapRenderMan::RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice,
											   const CLIPVOLUME* pCV, 
											   const MATERIAL_DATA& sMaterial_Data,
											   float& fHeight,
											   float& fNearLength )
{
	NSOCTREE::InitRenderOctree
	( 
		pd3dDevice, 
		pCV, 
		NULL, 
		NULL, 
		sMaterial_Data,
		NULL, 
		NULL,
		D3DXVECTOR2(0.f,0.f),
		1.f,
		FALSE
	);

	NSOCTREE::RenderMaterialWater( m_pLightMapTree, fHeight, fNearLength );
}

void LightMapRenderMan::RenderMaterial_OtherFX( LPDIRECT3DDEVICEQ pd3dDevice,
											   const MATERIAL_DATA& sMaterial_Data )	// Material 은 외부의 LightMap 정보를 읽어서 사용한다.
{
	NSOCTREE::RenderMaterial( pd3dDevice, m_pLightMapTree );
}

void LightMapRenderMan::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bAlphaTex )
{
	NSOCTREE::Render_Query( pd3dDevice, sCV, m_pLightMapTree, bAlphaTex );
}

VOID LightMapRenderMan::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	NSOCTREE::CreateQuery( pd3dDevice, m_pLightMapTree );
}

VOID LightMapRenderMan::DestroyQuery()
{
	NSOCTREE::DestroyQuery( m_pLightMapTree );
}

VOID LightMapRenderMan::ResetOcclusionQuery()
{
	NSOCTREE::ResetOcclusionQuery( m_pLightMapTree );
}

void LightMapRenderMan::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	NSOCTREE::FindOcclusionRenderIndex( pd3dDevice, sCV, m_pLightMapTree, vecOctreeIndex );
}

BOOL LightMapRenderMan::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode ) const
{
	BOOL bColl = FALSE;
	float fLength(FLT_MAX);
	NSOCTREE::CollisionLine( m_pLightMapTree, vStart, vEnd, vColl, vNor, bColl, emCullMode, fLength );

	return bColl;
}

BOOL LightMapRenderMan::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if ( NSOCTREE::IsCollisionLineSimple( m_pLightMapTree, vStart, vEnd, emCullMode ) )
		return TRUE;

	return FALSE;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL LightMapRenderMan::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV )
{
    return NSOCTREE::IsCollisionLinePixel( m_pLightMapTree, vStart, vEnd, emCullMode, pData16Bit, nIndexUV );
}

BOOL LightMapRenderMan::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay )
{
	//// Note : UV로 좌표를 구한다.
	//NSTexture16Bit::DATA* pDataDay(NULL);
	//if ( m_textureResDay.GetTexture() )
	//{
	//	pDataDay = NSTexture16Bit::Insert( pd3dDevice, m_textureResDay.GetFileName(), m_textureResDay.GetTexture() );
	//}

	D3DXVECTOR2 vUV;
	if ( NSOCTREE::GetLightMapColor( m_pLightMapTree, vStart, vEnd, vUV ) )
	{
		// 낮 용 컬러를 계산하자.
		D3DXVECTOR4 vColorDay(0.f,0.f,0.f,0.f);
		if ( GLPeriod::GetInstance().GetBlendFact() != 0.f && pDataDay )
		{
			vColorDay = NSTexture16Bit::GetColor( pDataDay, vUV );
		}

		vColorDay *= 2.f;
		if ( vColorDay.x > 255.f )	vColorDay.x = 255.f;
		if ( vColorDay.y > 255.f )	vColorDay.y = 255.f;
		if ( vColorDay.z > 255.f )	vColorDay.z = 255.f;

		dwColor = 0xff000000;
		dwColor += (static_cast<DWORD>(vColorDay.x) << 16);
		dwColor += (static_cast<DWORD>(vColorDay.y) << 8);
		dwColor += static_cast<DWORD>(vColorDay.z);
		
		return TRUE;
	}
	return FALSE;
}

void LightMapRenderMan::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	NSOCTREE::SetLightMapUV_Rate( vLightMapUV_Rate, m_pLightMapTree );
}

void LightMapRenderMan::GetVertexNum( DWORD& dwVertexNum )
{
	NSOCTREE::GetVertexNum( m_pLightMapTree, dwVertexNum );
}

void LightMapRenderMan::GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )
{
    NSOCTREE::GetVertices( m_pLightMapTree, vecVertices, vecLightMapUV );
}

void LightMapRenderMan::GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices )
{
	NSOCTREE::GetVertices( m_pLightMapTree, vecVertices );
}

void LightMapRenderMan::ClipMesh( DxDecal* pDecal )
{
	NSOCTREE::ClipMesh( m_pLightMapTree, pDecal );
}

void LightMapRenderMan::Save( sc::SerialFile& SFile, BOOL bPiece )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		if( m_pLightMapTree )
		{
			SFile << (BOOL)TRUE;

            SFile << m_dwFVF;
            SFile << m_vMax;
		    SFile << m_vMin;

			SFile << m_strTextureDay;
			SFile << m_strTextureNight;

			LightMapRender* pLightMapRender = dynamic_cast<LightMapRender*>( m_pLightMapTree );
			if ( pLightMapRender )
			{
				SFile << (BOOL)TRUE;
				NSOCTREE::SaveOctree ( SFile, m_pLightMapTree, FALSE );
			}
			else
			{
				SFile << (BOOL)FALSE;
			}

			LightMapRenderMat* pLightMapRenderMat = dynamic_cast<LightMapRenderMat*>( m_pLightMapTree );
			if ( pLightMapRenderMat )
			{
				SFile << (BOOL)TRUE;
				NSOCTREE::SaveOctree ( SFile, m_pLightMapTree, FALSE );
			}
			else
			{
				SFile << (BOOL)FALSE;
			}
		}
		else
		{
			SFile << (BOOL)FALSE;
		}
	}
	SFile.EndBlock();
}

void LightMapRenderMan::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	DWORD dwVer(0);
	DWORD dwBufferSize(0);

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == 0x0104 )
	{
		BOOL bUse(FALSE);

		SFile >> bUse;
		if( bUse )
		{
			SFile >> m_dwFVF;
			SFile >> m_vMax;
			SFile >> m_vMin;

			SFile >> m_strTextureDay;
			SFile >> m_strTextureNight;

			SFile >> bUse;
			if( bUse )
			{
				LightMapRender*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_LIGHTMAP );
				m_pLightMapTree = pLightMapTree;
			}

			SFile >> bUse;
			if( bUse )
			{
				LightMapRenderMat*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_MAT );
				m_pLightMapTree = pLightMapTree;
			}

			LoadTexture( pd3dDevice );
		}
	}
	else if ( dwVer == 0x0103 )
	{
		BOOL bUse(FALSE);

		SFile >> bUse;
		if( bUse )
		{
			SFile >> m_dwFVF;
			SFile >> m_vMax;
			SFile >> m_vMin;

			SFile >> m_strTextureDay;
			SFile >> m_strTextureNight;

			TSTRING strTemp;
			SFile >> strTemp;
			SFile >> strTemp;

			SFile >> bUse;
			if( bUse )
			{
				LightMapRender*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_LIGHTMAP );
				m_pLightMapTree = pLightMapTree;
			}

			SFile >> bUse;
			if( bUse )
			{
				LightMapRenderMat*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_MAT );
				m_pLightMapTree = pLightMapTree;
			}

			LoadTexture( pd3dDevice );
		}
	}
    else if ( dwVer == 0x0102 )
    {
        BOOL bUse(FALSE);

	    SFile >> bUse;
	    if( bUse )
        {
            SFile >> m_dwFVF;
            SFile >> m_vMax;
		    SFile >> m_vMin;

		    SFile >> m_strTextureDay;
		    SFile >> m_strTextureNight;
            
			TSTRING strTemp;
			SFile >> strTemp;
			SFile >> strTemp;

			SFile >> bUse;
			if( bUse )
			{
				LightMapRender*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_LIGHTMAP );
				m_pLightMapTree = pLightMapTree;

				// Note : Collision 만들기
				NSOCTREE::MakeCollisionData( pd3dDevice, m_pLightMapTree );
			}

			SFile >> bUse;
			if( bUse )
			{
				LightMapRenderMat*	pLightMapTree;
				NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_MAT );
				m_pLightMapTree = pLightMapTree;
			}

		    LoadTexture( pd3dDevice );
	    }
    }
	else if ( dwVer == 0x0101 )
	{
		BOOL bUse(FALSE);

		SFile >> bUse;
		if( bUse )
		{
			SFile >> m_dwFVF;
			SFile >> m_vMax;
			SFile >> m_vMin;

			SFile >> m_strTextureDay;
			SFile >> m_strTextureNight;
			
			TSTRING strTemp;
			SFile >> strTemp;
			SFile >> strTemp;

			LightMapRender*	pLightMapTree;
			NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_LIGHTMAP );
			m_pLightMapTree = pLightMapTree;

			// Note : Collision 만들기
			NSOCTREE::MakeCollisionData( pd3dDevice, m_pLightMapTree );

			LoadTexture( pd3dDevice );
		}
	}
    else if (dwVer == 0x0100 )
    {
        BOOL bUse(FALSE);

	    SFile >> bUse;
	    if( bUse )
        {
            SFile >> m_vMax;
		    SFile >> m_vMin;

		    SFile >> m_strTextureDay;
		    SFile >> m_strTextureNight;
            
			TSTRING strTemp;
			SFile >> strTemp;
			SFile >> strTemp;

			LightMapRender*	pLightMapTree;
            NSOCTREE::LoadOctree( pd3dDevice, SFile, &pLightMapTree, bDynamicLoad, bPiece, ABTT_LIGHTMAP );
			m_pLightMapTree = pLightMapTree;

			// Note : Collision 만들기
			NSOCTREE::MakeCollisionData( pd3dDevice, m_pLightMapTree );

		    LoadTexture( pd3dDevice );
	    }
    }
    else
    {
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
    }
}

void LightMapRenderMan::Import( LPDIRECT3DDEVICEQ pd3dDevice, LightMapRenderMan* pSrc, float fCellSize, bool &bEnableVertexColor )
{
	CleanUp();

	m_dwFVF					= pSrc->m_dwFVF;
	m_vMax					= pSrc->m_vMax;
	m_vMin					= pSrc->m_vMin;
	m_strTextureDay			= pSrc->m_strTextureDay;
	m_strTextureNight		= pSrc->m_strTextureNight;

	//LoadTexture( pd3dDevice );

	std::vector<VERTEXNORCOLORTEX5>	vecVertices;
	NSOCTREE::GetVertices( pSrc->m_pLightMapTree, vecVertices );

	VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[ vecVertices.size() ];
	for ( DWORD i=0; i<vecVertices.size(); ++i )
	{
		pVertices[i] = vecVertices[i];

		if ( vecVertices[i].dwColor != 0xffffffff )
		{
			bEnableVertexColor = true;
		}
	}

	// mat 종류로 사용하도록 한다.
	Create( pd3dDevice, vecVertices.size()/3, reinterpret_cast<BYTE*>( pVertices ), m_dwFVF, EMLR_DAYNIGHT_ON, TRUE, FALSE, ABTT_MAT, fCellSize );

	SAFE_DELETE_ARRAY ( pVertices );
}
