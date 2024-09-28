// DxStaticMaterialMeshLightMap.h: interface for the CCollision class.
//
//	class DxStaticMaterialMeshLightMap
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxLand/NSLandThreadDef.h"

struct CLIPVOLUME;
class DxDecal;
class DxStaticMaterialMesh;
class DxTextureEffNewFile;


namespace sc
{
	class SerialFile;
};

namespace COLLISION
{	
	struct TRIANGLE;
};


class DxStaticMaterialMeshLightMap
{
private:
	enum
	{
		VERSION = 0x0100,           // 기본적인 Version
	};

private:
	typedef std::vector<std::tr1::shared_ptr<DxStaticMaterialMesh>>	VEC_DXSTATICMATERIALMESH;
	typedef VEC_DXSTATICMATERIALMESH::iterator						VEC_DXSTATICMATERIALMESH_ITER;

	// LightMap 별로 DxStaticMaterialMesh 를 가지고 있다.
	VEC_DXSTATICMATERIALMESH	m_vecStaticMesh;

	sc::SerialFile		m_SFile;		// Thread Loading 에 필요함.
	LOADINGDATALIST		m_listLoadingData;
	CRITICAL_SECTION	m_CSLockLoading;

	float				m_fRealTimeWaterHeight;	// 실시간 물 높이.

public:
	void Insert_SMMLM( std::tr1::shared_ptr<DxStaticMaterialMesh> pStaticMesh );
	void FrameMove( float fElapsedTime );

	void RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld );
	void RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderSoftAlpha_Deffered( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld );
	void RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld );
	void RenderWater_CustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void RenderCascadeShadow( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	// Tool 에서만 사용됨.
	void Render_CharShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, 
		D3DXVECTOR3& vEnd, 
		D3DXVECTOR3 &vColl,
		D3DXVECTOR3 &vNor, 
		LPCSTR& szName, 
		EMCOLLMODE emCullMode,
		BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, 
		D3DXVECTOR3& vEnd, 
		D3DXVECTOR3 &vColl,
		D3DXVECTOR3 &vNor, 
		EMCOLLMODE emCullMode,
		BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

	void GetCollisionLineInTriListExceptY( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList );

	// 실시간 반사 위치를 구한다.
	// 값이 FLT_MAX 일 경우는 작동되지 않는다.
	float GetRealTimeWaterHeight();

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	void ClipStaticMesh( DxDecal* pDecal );

	void LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice );
	void RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	// Thread Loading 을 의미함.
	void Save( const char* szName, boost::function<void (const TCHAR*)> pArchive = 0 );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading 을 의미함.
	void Save( sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void CleanUp();



public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
	void SetNSLandThread();



public:
	DxStaticMaterialMeshLightMap();
	~DxStaticMaterialMeshLightMap();
};
