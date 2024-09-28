#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <map>
#include <string>

#include "../../DxTools/DxCustomTypes.h"
#include "../NSLandThreadDef.h"

#include "./DxPieceManager.h"


struct DxFrame;
class DxFrameMesh;
class DxStaticMeshLightMap;
class DxPiece;
class DxPieceAABB;
struct DxPieceNode;
class DxPieceEdit;
class DxPieceManagerPSF;
class DxFogLocalMan;
class DxPrefabMan;
class DxLandMan;
class DxPieceInstancing;
class DxPieceInstancingForShadow;
class EntityRenderListSwap;
class DxStaticAniFrame;
struct PIECE_INSTANCING_FIRST;

template <typename T, typename T2>
class PieceInstancingThread;

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		M	a	n	a	g	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
class DxPieceManagerWLD : public DxPieceManager
{
private:
	static const DWORD VERSION_WLD;

protected:
	float m_fTime;
	float m_fElapsedTime;

protected:
	//// PieceName, PieceAABB - PSF 에서 그대로 가져온 정보
	//typedef std::tr1::unordered_map<std::string, DxPieceAABB*> MAPNAMEPIECE;
	//typedef MAPNAMEPIECE::iterator				               MAPNAMEPIECE_ITER;
	//typedef MAPNAMEPIECE::value_type			               MAPNAMEPIECE_VALUE;
	//MAPNAMEPIECE	m_mapNamePiece;

	// 렌더링시 사용되는 정보
	// 원본 Src 들이 여기에 다 있다.
	boost::shared_ptr<DxPieceAABB>	m_spPieceAABB;

	// Instancing 정보. Thread 와 공유한다.
	boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_spInstancingMeshLM;										// Map 에 first 값은 Mesh + LM 정보로. (Base)
	boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_spInstancingMeshReflection;								// Map 에 first 값은 Mesh + LM 정보로. (Reflection)
	boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>	m_spInstancingPureMesh[NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE];	// Map 에 first 값은 Mesh로만

protected:
	BOOL		m_bInstancingON;	// Instancing 이 되는가~?
	BOOL		m_bUseLightMap;		// LightMap 으로 작동되는가~?
	BOOL		m_bLodBlending;		// Lod Blending 지원하는가~?

public:
	DxPieceAABB*					GetPieceAABB()		{ return m_spPieceAABB.get(); }
	boost::shared_ptr<DxPieceAABB>	GetPieceAABB_SP()	{ return m_spPieceAABB; }

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

    BOOL IsCollisionLine2( const D3DXVECTOR3& vStart, 
						      D3DXVECTOR3& vEnd, 
						      D3DXVECTOR3& vCollision, 
						      DxPiece** rrPiece,
                              EMCOLLMODE emCullMode ) const;						// 형상 - <Edit> Piece를 얻기위함.

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

	void ResetAnimationTime_DisableLoopType();

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot );

private:
    friend class CPieceManagerEdit;
    friend class CPiecePointEdit;
    friend class CWorldEditView;

public:
	void FrameMove( const float fTime, const float fElapsedTime, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );

public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
	void SetData_Force_TnL_PS_3_0_DF();


public:
	//////////////////////////////////////////////////////////////////////////
	// 전체 다 호출된다.
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
						//const D3DXVECTOR3& vFromPt, 
						//const D3DXVECTOR3& vLookatPt,
						const VEC_DWORD* pvecDWORD, 
						BOOL bMPShading,
						BOOL bUsedMaterialSystem,
						BOOL bReflect,
						float fReflectHeight,
						boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );

	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							float fLightMapDetail, 
							BOOL bMPShading, 
							BOOL bUsedMaterialSystem, 
							BOOL bReflect,
							boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );

	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, 
					DxStaticMeshLightMap* pStaticMeshLightMap, 
					DxFrameMesh* pFrameMesh, 
					float fLightMapDetail, 
					BOOL bUsedMaterialSystem,
					boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	// 전체 다 호출된다.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_1_1 이하에서만 호출 된다.
	void RenderEffFrame( boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice,
							float fLightMapDetail, 
							BOOL bMPShading, 
							BOOL bUsedMaterialSystem,
							boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );

	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshLightMap* pStaticMeshLightMap, 
							DxFrameMesh* pFrameMesh,
							BOOL bReflect,
							float fReflectHeight,
							boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );

	//void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	
	// ENSTM_DECAL_AFTER 는 렌더링 작업이 된다.
	void RenderDecal( boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );	
	// TnL_PS_1_1 이하에서만 호출 된다.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered 중,상,최상 옵션	
	void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflection, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflection, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap, BOOL bReflect, float fReflectHeight );
	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	// Deffered 중,상,최상 옵션	
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_3_0_CSM 이상
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	// TnL_PS_3_0_CSM 이상
	//////////////////////////////////////////////////////////////////////////



public:
	void PieceSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt );

	void CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	void DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderPieceIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecPieceIndex );

public:
	void SaveWLD( sc::SerialFile& SFile, float fInstancingCellSize );
	void LoadWLD( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
    void LoadWLD_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	void ClonePSFtoWLD( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxStaticMesh* pStaticMesh, 
						DxPieceManagerPSF* pSrc, 
						DxFogLocalMan* pFogLocalMan, 
						BOOL bRan_1_0,
						BOOL bLightMap, 
						const TSTRING& strLightMapName,
						TSTRING& strErrorMsg,
						float fCellSize );

	void RenderRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxLandMan* pLandMan,
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							const TSTRING& strLightMapName,
							int nRadiosityPassCUR,				// 현재 진행중인 Pass
							float fRadiosityReflectance,
							float fRadiosityFogBlend,
							DWORD dwRadiosityFogLength );

	void RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void CleanUp();

	// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
	void SetNSLandThread( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

public:
	DxPieceManagerWLD();
	virtual ~DxPieceManagerWLD();
};