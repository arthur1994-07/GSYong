#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <boost/scoped_ptr.hpp> 
#include <vector>
#include <map>
#include <string>

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/DxCustomTypes.h"
#include "./DxPieceDef.h"

struct DxFrame;
class DxFrameMesh;
class DxStaticMeshLightMap;
class DxPiece;
struct DxPieceNode;
class DxPieceEdit;
class DxPieceManager;
class DxPieceInstancingForShadow;
class DxLandMan;
class DxStaticMeshCollBase;
class DxStaticMesh;
class DxPieceManagerPSF;
class DxStaticAniFrame;
struct PIECE_INSTANCING_FIRST;

template <typename T, typename T2>
class PieceInstancingThread;

namespace sc
{
	class SerialFile;
}


// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		A	A	B	B
// -----------------------------------------------------------------------------------------------------------------------------------------
class DxPieceAABB
{
private:
    enum
    {
        VERSION = 0x102,
    };

protected:
	VEC_PIECE			m_vecPiece;			// Piece 데이터 // Save, Load도 한다.

	DxPieceNode*		m_pOldMapAndCollisionPieceTree;	// AABB Tree < View >	충돌처리 및 Instancing 을 사용하지 않을 경우에 쓰인다.
														//						1.1 이하버젼.. IsExceptLowOption() 이면 렌더링에서 제외된다.
														//						충돌처리.. IsExceptLowOption() 이면 충돌처리에서 제외된다.

	DxPieceNode*		m_pAllPieceTree;				// AABB Tree < View >	모든 Piece

	//std::vector<DxPiece*> m_vecPieceForFrameAniHardAlpha;	// 참조값
	//std::vector<DxPiece*> m_vecPieceForReplace;				// 참조값

	int				m_nFrameMoveStep;


public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
	void SetData_Force_TnL_PS_3_0_DF();


public:
	VEC_PIECE&	GetVecPiece()	{ return m_vecPiece; }
	void SetPiece( DxPiece* pPiece );

	void AddPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );

	// 이걸 사용하면 MakeTree 를 해야 DxPieceNode 가 생성된다.
	void AddPieceAABB_DeleteSrc( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceAABB* pSrc );

	//void AddPieceAABB( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceAABB* pSrc );

	//void AddPieceAABB( LPDIRECT3DDEVICEQ pd3dDevice, 
	//					DxPieceAABB* pSrc, 
	//					BOOL bLightMap, 
	//					const TSTRING& strLightMapName, 
	//					DxStaticMesh* pStaticMesh, 
	//					DxPieceManagerPSF* pPieceManager,
	//					DWORD& dwStartTextureSize,
	//					float fLightMapDetail );

	void MakeTree();

	//void MakeTree( BOOL bWLD, 
	//				float fInstancingCellSize, 
	//				BOOL bInstancing, 
	//				boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingBase,
	//				boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>& spPieceInstancingReflection );

	void MakeTree( const D3DXMATRIX& matWorld );

	void ResetPieceParameter();

	void InitInstancing( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//////////////////////////////////////////////////////////////////////////
	// 전체 다 호출된다.
	void InsertQuickSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect );
	void InsertQuickSortForShadow( const CLIPVOLUME &sCV );
	void InsertQuickSortPSF( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend );
	// 전체 다 호출된다.
	//////////////////////////////////////////////////////////////////////////

	void FrameMove( const float fTime, const float fElapsedTime );
	//void FrameMoveAll( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	//void FrameMoveStep( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	//void FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

	//void RenderOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void RenderHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void RenderEffFrame_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer );
	//void RenderDecal_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void RenderDecalPass2_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void RenderWaterCustomSoftAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap );

	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	void CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	void DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderPieceIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecPieceIndex );

	// Edit
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

    BOOL IsCollisionLine2( const D3DXVECTOR3& vStart, 
							D3DXVECTOR3& vEnd, 
							D3DXVECTOR3* vNor, 
							D3DXVECTOR3& vCollision, 
							DxPiece** rrPiece,
                            EMCOLLMODE emCullMode, 
							BOOL bOnlyCamColl ) const;				// 형상 - <Edit> Piece를 얻기위함.

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0, float fCellSize );
    void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReleaseLightMap();
	void ResetAnimationTime_DisableLoopType();
    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	//BOOL IsUserLightMapUV();	// User가 만든 LightMapUV를 사용하는가~?

	BOOL Clone_MouseShift( LPD3DXMATRIX& pMatrix );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot );

	void SetPrefabMatrix( const D3DXMATRIX& matPrefab );

	// Radiosity
	void BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
		DxLandMan* pLandMan, 
		LPDIRECT3DTEXTUREQ pTextureSystem, 
		float fReflectance, 
		float fFogBlend, 
		DWORD dwFogLength,
		DWORD dwRadiosityIndex );

	void AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength );

	// TriangleLinkLight
public:
	// Piece의 TriangleLinkLight 를 생성하도록 한다.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									BOOL bDirectionLight );

	//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// 한 삼각형에서 Light를 3개 이상 쓰고 있는 것을 찾는다.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const;

	// 특정 Light를 줄여주도록 한다.
	void EraseLightID( DWORD dwLightID );

	// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다.
	void SetType_PS_3_0();

public:
	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bWLD );
    void Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName, BOOL bWLD );
	void Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName, BOOL bWLD );

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

public:
	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
                    DxPieceAABB* pSrc, 
                    BOOL bLightMap, 
                    const TSTRING& strLightMapName, 
                    DxStaticMesh* pStaticMesh, 
                    DxPieceManagerPSF* pPieceManager,
                    DWORD& dwStartTextureSize,
					BOOL& bEnoughUsedPixelLightMap,
					float fCellSize,
					BOOL bWLD );

	void SetPiece_NULL();		// DxPieceManagerPSF에서만 쓰인다.
	void CleanUp();

	void Archive( boost::function<void (const TCHAR*)> fun );

	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice,
					DxPieceAABB* pSrc,
					float fCellSize,
					BOOL bWLD );

	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece, float fCellSize );
	BOOL IsSupportRAD();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	DxPieceAABB();
	~DxPieceAABB();
};
