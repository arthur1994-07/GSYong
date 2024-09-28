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
	//// PieceName, PieceAABB - PSF ���� �״�� ������ ����
	//typedef std::tr1::unordered_map<std::string, DxPieceAABB*> MAPNAMEPIECE;
	//typedef MAPNAMEPIECE::iterator				               MAPNAMEPIECE_ITER;
	//typedef MAPNAMEPIECE::value_type			               MAPNAMEPIECE_VALUE;
	//MAPNAMEPIECE	m_mapNamePiece;

	// �������� ���Ǵ� ����
	// ���� Src ���� ���⿡ �� �ִ�.
	boost::shared_ptr<DxPieceAABB>	m_spPieceAABB;

	// Instancing ����. Thread �� �����Ѵ�.
	boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_spInstancingMeshLM;										// Map �� first ���� Mesh + LM ������. (Base)
	boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_spInstancingMeshReflection;								// Map �� first ���� Mesh + LM ������. (Reflection)
	boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>	m_spInstancingPureMesh[NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE];	// Map �� first ���� Mesh�θ�

protected:
	BOOL		m_bInstancingON;	// Instancing �� �Ǵ°�~?
	BOOL		m_bUseLightMap;		// LightMap ���� �۵��Ǵ°�~?
	BOOL		m_bLodBlending;		// Lod Blending �����ϴ°�~?

public:
	DxPieceAABB*					GetPieceAABB()		{ return m_spPieceAABB.get(); }
	boost::shared_ptr<DxPieceAABB>	GetPieceAABB_SP()	{ return m_spPieceAABB; }

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

    BOOL IsCollisionLine2( const D3DXVECTOR3& vStart, 
						      D3DXVECTOR3& vEnd, 
						      D3DXVECTOR3& vCollision, 
						      DxPiece** rrPiece,
                              EMCOLLMODE emCullMode ) const;						// ���� - <Edit> Piece�� �������.

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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
	// Material �� ���Ѵ�.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
	void SetData_Force_TnL_PS_3_0_DF();


public:
	//////////////////////////////////////////////////////////////////////////
	// ��ü �� ȣ��ȴ�.
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
	// ��ü �� ȣ��ȴ�.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
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
	
	// ENSTM_DECAL_AFTER �� ������ �۾��� �ȴ�.
	void RenderDecal( boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );	
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered ��,��,�ֻ� �ɼ�	
	void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflection, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflection, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap, BOOL bReflect, float fReflectHeight );
	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	// Deffered ��,��,�ֻ� �ɼ�	
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_3_0_CSM �̻�
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap );
	// TnL_PS_3_0_CSM �̻�
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
							int nRadiosityPassCUR,				// ���� �������� Pass
							float fRadiosityReflectance,
							float fRadiosityFogBlend,
							DWORD dwRadiosityFogLength );

	void RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void CleanUp();

	// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
	void SetNSLandThread( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

public:
	DxPieceManagerWLD();
	virtual ~DxPieceManagerWLD();
};