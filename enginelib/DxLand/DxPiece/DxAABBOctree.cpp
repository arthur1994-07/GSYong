#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/DxEnvironment.h"
#include "../DxCullingMeshForThread.h"
#include "./OBJOCTree.h"
#include "./NsOCTree.h"

#include "DxAABBOctree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		O	p	t	i	m	i	z	e		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	DxAABBOctree::VERSION = 0x0101;

DxAABBOctree::DxAABBOctree()
    : m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_pOcTree(NULL)
{
    m_emTypeRender = AABBRenderStrategy::TYPE_OCJOCTREE;
}

DxAABBOctree::~DxAABBOctree()
{
	CleanUp();
}

void DxAABBOctree::CleanUp()
{
	SAFE_DELETE ( m_pOcTree );
}

int DxAABBOctree::Create ( const LPDIRECT3DDEVICEQ pd3dDevice, 
						   const DWORD dwFaceNUM, 
						   BYTE* pVertices, 
						   const DWORD dwFVF, 
						   EM_LAND_TYPE emLandType, 
						   BOOL bUserLightMapUV,
						   BOOL bRan_1_0,
						   AABB_BINARY_TREE_TYPE emType,
						   float fCellSize )
{
	// Note : 처음 다 삭제
	CleanUp();

	// Note : 정점 데이터 얻기
	if ( !pVertices )
		return 0;

	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	BYTE* pVertex = new (std::nothrow) BYTE[nVERSIZE*dwFaceNUM*3];
	if ( !pVertex )
		return -1;

	memcpy ( pVertex, pVertices, nVERSIZE*dwFaceNUM*3 );

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
	NSOCTREE::MakeOctree( &m_pOcTree, pVertex, dwFaceNUM, fCellSize, 20000, nVERSIZE );
	SAFE_DELETE_ARRAY( pVertex );

	// Note : OcMesh 만들기
	NSOCTREE::MakeOcMesh( pd3dDevice, m_pOcTree, dwFVF );

	// Note : OcMesh 최적화
	NSOCTREE::MakeOptimizeMesh( pd3dDevice, m_pOcTree );

	// Note : Collision 만들기
	NSOCTREE::MakeCollisionData( pd3dDevice, m_pOcTree );

	// Note : 현 Object의 Type을 설정한다.
	switch( emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		NSOCTREE::MakeVBIBAndDelMesh( pd3dDevice, m_pOcTree );	// OcMesh를 m_pVB, m_pIB, pNormalColor쪽으로 옮기고, 자신은 삭제한다.
		break;

	case EMLR_DAYNIGHT_OFF:
		NSOCTREE::MakeVBIBAndDelMesh( pd3dDevice, m_pOcTree );	// OcMesh를 m_pVB, m_pIB, pNormalColor쪽으로 옮기고, 자신은 삭제한다.
		NSOCTREE::DeleteNormalColor( m_pOcTree );				// 여기서 pNormalColor은 쓰이지 않는다.
		break;

	case EMLR_PIECE:
		NSOCTREE::CovertPieceSetting( pd3dDevice, m_pOcTree, bUserLightMapUV );
		break;
	};

	NSOCTREE::MakeLoadCheckOK( m_pOcTree );					// Load 다 되있다고 설정하자. 안하면 기본 파일이 없어서 로딩완료를 알지 못한다.

	return 1;
}

AABBBinaryTreeBase* DxAABBOctree::GetOBJOCTree()
{ 
    return m_pOcTree; 
}

AABBBinaryTreeBase* DxAABBOctree::GetAABBBinaryBase()
{ 
	return m_pOcTree; 
}

void DxAABBOctree::FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile )
{
	if( !SFile )	return;

	NSOCTREE::FirstLoad ( pd3dDevice, *SFile, m_pOcTree, vMax, vMin );
}

void DxAABBOctree::ThreadLoad( const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
{
	//NSOCTREE::ThreadLoad( m_pOcTree, sCV, listLoadingData, CSLockLoading );
}

void DxAABBOctree::RenderTHREAD_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffFlowUV* pEff )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderOctree_FlowUV( pd3dDevice, sCV, m_pOcTree, pEff );
}

void DxAABBOctree::RenderTHREAD_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffRotate* pEff )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderOctree_Rotate( pd3dDevice, sCV, m_pOcTree, pEff );
}

void DxAABBOctree::RenderTHREAD_VisualMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                                const CLIPVOLUME* pCV, 
                                                DxTexEffBase* pEff, 
												const D3DXMATRIX& matWorld )
{
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderOctree_VisualMaterialColor( pd3dDevice, *pCV, m_pOcTree, pEff, matWorld );
}

void DxAABBOctree::RenderCullingMeshVM( LPDIRECT3DDEVICEQ pd3dDevice, 
										 boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread,
										 DxTexEffBase* pEff, 
										 const D3DXMATRIX& matWorld )
{
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
	spCullingMeshForThread->RenderVisualMaterial
	(
		pd3dDevice,
		pEff, 
		matWorld
	);
}

void DxAABBOctree::DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile )
{
	if( SFile )
	{
		NSOCTREE::DynamicLoad ( pd3dDevice, *SFile, m_pOcTree, sCV );
	}
}

void DxAABBOctree::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pOcTree );
}

void DxAABBOctree::RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	//NSOCTREE::RenderOctree( pd3dDevice, sCV, m_pOcTree );
	spCullingMeshForThread->RenderOctree( pd3dDevice );
}

void DxAABBOctree::RenderPiece ( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderList( pd3dDevice, m_pOcTree );
}

void DxAABBOctree::Render_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffFlowUV* pEff )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderList_FlowUV( pd3dDevice, m_pOcTree, pEff );
}

void DxAABBOctree::Render_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffRotate* pEff )
{
	NSENVIRONMENT::SetReflectClip( pd3dDevice );
	NSOCTREE::RenderList_Rotate( pd3dDevice, m_pOcTree, pEff );
}

void DxAABBOctree::Render_VisualMaterialColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld )
{
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderList_VisualMaterialColor( pd3dDevice, m_pOcTree, pEff, bRealPL, matWorld, 1.f );
}

void DxAABBOctree::RenderPiece_VisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                              DxTexEffBase* pEff, 
                                              BOOL bRealPL, 
                                              const D3DXMATRIX& matWorld, 
                                              const D3DXVECTOR2& vLightMapUV_Offset,
                                              float fAlpha, 
											  LPDIRECT3DTEXTUREQ pTextureDay, 
											  LPDIRECT3DTEXTUREQ pTextureNight )
{
	NSENVIRONMENT::SetReflectClipShader( pd3dDevice );
    NSOCTREE::RenderList_VisualMaterialColor( pd3dDevice, m_pOcTree, pEff, bRealPL, matWorld, fAlpha );
}

void DxAABBOctree::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bAlphaTex )
{
	NSOCTREE::Render_Query( pd3dDevice, sCV, m_pOcTree, bAlphaTex );
}

VOID DxAABBOctree::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	NSOCTREE::CreateQuery( pd3dDevice, m_pOcTree );
}

VOID DxAABBOctree::DestroyQuery()
{
	NSOCTREE::DestroyQuery( m_pOcTree );
}

void DxAABBOctree::ResetOcclusionQuery()
{
	NSOCTREE::ResetOcclusionQuery( m_pOcTree );
}

void DxAABBOctree::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	NSOCTREE::FindOcclusionRenderIndex( pd3dDevice, sCV, m_pOcTree, vecOctreeIndex );
}

//void DxAABBOctree::InsertColorList( LOADINGDATALIST &listColorData )
//{
//	NSOCTREE::InsertColorList( m_pOcTree, listColorData );
//}

BOOL DxAABBOctree::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	return NSOCTREE::IsCollisionLineSimple( m_pOcTree, vStart, vEnd, emCullMode );
}

BOOL DxAABBOctree::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode ) const
{
	BOOL bColl = FALSE;
	float fLength(FLT_MAX);
	NSOCTREE::CollisionLine( m_pOcTree, vStart, vEnd, vColl, vNor, bColl, emCullMode, fLength );

	return bColl;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxAABBOctree::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV )
{
	return NSOCTREE::IsCollisionLinePixel( m_pOcTree, vStart, vEnd, emCullMode, pData16Bit, nIndexUV );
}

void DxAABBOctree::GetCollisionLineInTriListExceptY( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{
	NSOCTREE::GetCollisionLineInTriListExceptY( m_pOcTree, vStart, vEnd, TriList );
}

void DxAABBOctree::GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance )
{
    NSOCTREE::GetClosedPoint( m_pOcTree, vBasePos, vCollisionOut, fDistance );
}

void DxAABBOctree::GetVertexNum( DWORD& dwVertexNum )
{
	NSOCTREE::GetVertexNum( m_pOcTree, dwVertexNum );
}

void DxAABBOctree::GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )
{
    NSOCTREE::GetVertices( m_pOcTree, vecVertices, vecLightMapUV );
}

void DxAABBOctree::GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices )
{
	NSOCTREE::GetVertices( m_pOcTree, vecVertices );
}

void DxAABBOctree::ClipMesh( DxDecal* pDecal )
{
	NSOCTREE::ClipMesh( m_pOcTree, pDecal );
}

void DxAABBOctree::Save ( sc::SerialFile& SFile, BOOL bPiece )
{
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;

		BOOL bExist;
		bExist = m_pOcTree ? TRUE : FALSE;
		SFile << bExist;
		if ( m_pOcTree )	NSOCTREE::SaveOctree( SFile, m_pOcTree, bPiece );
	}
	SFile.EndBlock();
}

void DxAABBOctree::Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			NSOCTREE::LoadOctree( pd3dDevice, SFile, &m_pOcTree, bDynamicLoad, bPiece, ABTT_OCTREE );
		}
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			NSOCTREE::LoadOctree_Prev( pd3dDevice, SFile, &m_pOcTree, bDynamicLoad, bPiece, ABTT_OCTREE );
		}
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}
}

void DxAABBOctree::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, std::string& szName )
{
	CleanUp();

	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> szName;

	BOOL bUse = TRUE;
	SFile >> bUse;
	if ( bUse )
	{
		NSOCTREE::LoadOctreeVER_100( pd3dDevice, SFile, &m_pOcTree );
	}
}

BOOL DxAABBOctree::IsHaveLightMapUV()
{
	BOOL bHaveLightMapUV(FALSE);
	NSOCTREE::HaveLightMapUV( m_pOcTree, bHaveLightMapUV );
	return bHaveLightMapUV;
}