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
	VEC_PIECE			m_vecPiece;			// Piece ������ // Save, Load�� �Ѵ�.

	DxPieceNode*		m_pOldMapAndCollisionPieceTree;	// AABB Tree < View >	�浹ó�� �� Instancing �� ������� ���� ��쿡 ���δ�.
														//						1.1 ���Ϲ���.. IsExceptLowOption() �̸� ���������� ���ܵȴ�.
														//						�浹ó��.. IsExceptLowOption() �̸� �浹ó������ ���ܵȴ�.

	DxPieceNode*		m_pAllPieceTree;				// AABB Tree < View >	��� Piece

	//std::vector<DxPiece*> m_vecPieceForFrameAniHardAlpha;	// ������
	//std::vector<DxPiece*> m_vecPieceForReplace;				// ������

	int				m_nFrameMoveStep;


public:
	// Material �� ���Ѵ�.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
	void SetData_Force_TnL_PS_3_0_DF();


public:
	VEC_PIECE&	GetVecPiece()	{ return m_vecPiece; }
	void SetPiece( DxPiece* pPiece );

	void AddPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );

	// �̰� ����ϸ� MakeTree �� �ؾ� DxPieceNode �� �����ȴ�.
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
	// ��ü �� ȣ��ȴ�.
	void InsertQuickSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect );
	void InsertQuickSortForShadow( const CLIPVOLUME &sCV );
	void InsertQuickSortPSF( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend );
	// ��ü �� ȣ��ȴ�.
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
							BOOL bOnlyCamColl ) const;				// ���� - <Edit> Piece�� �������.

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0, float fCellSize );
    void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReleaseLightMap();
	void ResetAnimationTime_DisableLoopType();
    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	//BOOL IsUserLightMapUV();	// User�� ���� LightMapUV�� ����ϴ°�~?

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
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									BOOL bDirectionLight );

	//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// �� �ﰢ������ Light�� 3�� �̻� ���� �ִ� ���� ã�´�.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const;

	// Ư�� Light�� �ٿ��ֵ��� �Ѵ�.
	void EraseLightID( DWORD dwLightID );

	// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�.
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

	void SetPiece_NULL();		// DxPieceManagerPSF������ ���δ�.
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
