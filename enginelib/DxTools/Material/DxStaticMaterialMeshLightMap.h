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
		VERSION = 0x0100,           // �⺻���� Version
	};

private:
	typedef std::vector<std::tr1::shared_ptr<DxStaticMaterialMesh>>	VEC_DXSTATICMATERIALMESH;
	typedef VEC_DXSTATICMATERIALMESH::iterator						VEC_DXSTATICMATERIALMESH_ITER;

	// LightMap ���� DxStaticMaterialMesh �� ������ �ִ�.
	VEC_DXSTATICMATERIALMESH	m_vecStaticMesh;

	sc::SerialFile		m_SFile;		// Thread Loading �� �ʿ���.
	LOADINGDATALIST		m_listLoadingData;
	CRITICAL_SECTION	m_CSLockLoading;

	float				m_fRealTimeWaterHeight;	// �ǽð� �� ����.

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

	// Tool ������ ����.
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

	// �ǽð� �ݻ� ��ġ�� ���Ѵ�.
	// ���� FLT_MAX �� ���� �۵����� �ʴ´�.
	float GetRealTimeWaterHeight();

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	void ClipStaticMesh( DxDecal* pDecal );

	void LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice );
	void RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	// Thread Loading �� �ǹ���.
	void Save( const char* szName, boost::function<void (const TCHAR*)> pArchive = 0 );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading �� �ǹ���.
	void Save( sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void CleanUp();



public:
	// Material �� ���Ѵ�.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
	void SetNSLandThread();



public:
	DxStaticMaterialMeshLightMap();
	~DxStaticMaterialMeshLightMap();
};
