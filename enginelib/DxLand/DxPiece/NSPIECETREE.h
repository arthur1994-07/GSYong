#pragma once

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/Material/MaterialDef.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "../LightMap/LightMapDef.h"
#include "../LightMap/LightMapBoxColor.h"
#include "../Entity.h"
#include "./DxPieceDef.h"

class DxPieceEdit;
class DxPieceEff;
class DxStaticAniFrame;
class DxPieceTexEff;
class DxPieceManager;
class DxPieceManagerPSF;
class DxStaticMesh;
class DxStaticMeshLightMap;
class DxFrameMesh;
class LightMapBoxColor;
class DxSetLandMan;
class DxLandMan;
class DxStaticMeshCollBase;
class DxPieceInstancing;
struct DxPieceNode;
struct CLIPVOLUME;

namespace sc
{	
    class SerialFile;
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//											N	S		P	I	E	C	E		T	R	E	E
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSPIECETREE
{
	//void MakeTree( PDXPIECENODE& pTree, DxPiece* pPiece );	// Tree 를 만든다. pPiece 는 Tree의 Leaf 속으로 흡수 된다. List 사용불가.
	void MakeTree ( PDXPIECENODE& pTree, const VEC_PIECE& vecPiece, float fCellSize );

	void OnResetDevice( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( PDXPIECENODE& pTree );

	void InitInstancing( PDXPIECENODE& pTree, VEC_PIECE& vecInstancingExceptPiece );
	void CreateVB_ForInstancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice );
	void GetVecPiece( PDXPIECENODE& pTree, VEC_PIECE& vecPiece );
	//void CreateUnderShader_2_0( PDXPIECENODE& pTree );
	//void FrameMove_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, VEC_PIECE& vecInstancingExceptPiece );
	void RenderOpaque_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderHardAlpha_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderEffFrame_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer );
	void RenderDecal_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderDecalPass2_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderWaterCustomSoftAlpha_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderEff_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap );

	void InsertQuickSortALL( DxPieceNode* pTree,
							const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending, BOOL bTimeBlend );

	void InsertQuickSortForShadow( DxPieceNode* pTree, const CLIPVOLUME &sCV );

	//void InsertQuickSortForInstancing( DxPieceNode* pTree,
	//									const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending, BOOL bTimeBlend );

	//void InsertQuickSortForUnderShader_2_0( DxPieceNode* pTree,
	//										const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending );

	void CollisionLine( DxPieceNode* pTree, 
						const CLIPVOLUME &sCV, 
						const D3DXVECTOR3& vStart, 
						D3DXVECTOR3& vEnd, 
						D3DXVECTOR3* pNor, 
						D3DXVECTOR3& vCollision, 
						BOOL& bColl, 
						EMCOLLMODE emCullMode, 
						BOOL bOnlyCamColl );

    void CollisionLine2( DxPieceNode* pTree, 
						const D3DXVECTOR3& vStart, 
						D3DXVECTOR3& vEnd, 
						D3DXVECTOR3* pNor, 
						D3DXVECTOR3& vCollision, 
						BOOL& bColl, 
						PDXPIECE& rPiece,
						EMCOLLMODE emCullMode, 
						BOOL bOnlyCamColl );

	BOOL IsCollisionLineSimple( DxPieceNode* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode );

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( DxPieceNode* pTree, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	void GetLightMapColor( DxPieceNode* pTree, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, BOOL& bColl );

	//void ExportXFile( DxPieceNode* pTree, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot );

	void RenderCascadeShadowOpaque( DxPieceNode* pTree, 
									LPDIRECT3DDEVICEQ pd3dDevice, 
									const CLIPVOLUME &sCV, 
									std::vector<DxPiece*>& vecPieceForFrameAniHardAlpha, 
									std::vector<DxPiece*>& vecPieceForReplace );

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const CLIPVOLUME &sCV, BOOL bPSF );

    //void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const TSTRING& strLightMapName );
    //void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree );

	void Save( DxPieceNode* pNode, sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, const VEC_PIECE& vecPiece );
    void Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece );
	void Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece );

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
}

