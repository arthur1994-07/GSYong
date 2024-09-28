// DxStaticMeshLightMap.h: interface for the CCollision class.
//
//	class DxStaticMeshLightMap
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxLand/NSLandThreadDef.h"
#include "./DxStaticMeshCollBase.h"

struct CLIPVOLUME;
class DxDecal;
class DxStaticMesh;
class DxTextureEffNewFile;

namespace sc
{
	class SerialFile;
};

namespace COLLISION
{	
	struct TRIANGLE;
};


class DxStaticMeshLightMap : public DxStaticMeshCollBase
{
private:
	enum
    {
        VERSION = 0x0100,           // �⺻���� Version
    };

private:
	typedef std::vector<std::tr1::shared_ptr<DxStaticMesh>>	VEC_DXSTATICMESH;
	typedef VEC_DXSTATICMESH::iterator						VEC_DXSTATICMESH_ITER;

	VEC_DXSTATICMESH	m_vecStaticMesh;

	//sc::SerialFile		m_SFile;		// Thread Loading �� �ʿ���.
	//DxStaticMesh_THREAD	m_sThread;			// Loading�� ���� Thread
	//LOADINGDATALIST		m_listLoadingData;	// �ּ� ����Ʈ
	//CRITICAL_SECTION	m_CSLockLoading;	// m_listLoadingData�� ��ȭ�� �ߺ����� �ʵ��� �ϱ� ����.

//private:
//	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
//	void EndThread();

public:
	void Insert( std::tr1::shared_ptr<DxStaticMesh> pStaticMesh );
	void FrameMove( float fElapsedTime );

	// Thread Loading Render
	void Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV );
	void Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV );
	void Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV );

	virtual void Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

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

	// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	void GetCollisionLineInTriListExceptY( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	BOOL IsLightMap();

	void ClipStaticMesh( DxDecal* pDecal );

	void LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

	void GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffNewFile& sTexEffNewFile );
	void SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffNewFile& sTexEffNewFile );

	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	// Thread Loading �� �ǹ���.
	void Save( const char* szName, boost::function<void (const TCHAR*)> pArchive = 0 );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading �� �ǹ���.
	void Save( sc::SerialFile& SFile );

	// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
	void SetNSLandThread();

private:
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void CleanUp();

public:
	DxStaticMeshLightMap();
	virtual ~DxStaticMeshLightMap();
};
