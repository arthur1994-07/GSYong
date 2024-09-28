

#ifndef _SKINMESH_MANAGER_H_
#define _SKINMESH_MANAGER_H_

#include "../../../SigmaCore/File/FileFind.h"

#include "./_new/DxSkinMesh9.h"

#include "../../ResourceManagerCacheAlgorithm.h"

#include "./DxSkinMeshAddData.h"


class DxSkinMeshManager : public OnceRefAndEnableAlgorithm<DxSkinMesh9,SKINMESH_ADD_DATA,OnceRefNTime>
{
	DxSkinMeshManager();
public:
	struct Resource : public ResourceManager::Resource
	{
		explicit Resource(DxSkinMesh9* pSkinMesh) {	m_spCompletedData = SP_COMPLETED_DATA( pSkinMesh ); }
		Resource(const ResourceManager::Resource& res) : ResourceManager::Resource(res) {}
		Resource() {}

		DxSkinMesh9* get()  { return Update() ? m_spCompletedData.get() : NULL; }
	};

public:
	static DxSkinMeshManager& GetInstance();	

	void				SetDevice( LPDIRECT3DDEVICEQ pd3dDevice ) { m_pd3dDevice = pd3dDevice; }
	void				SetPath(const TSTRING& path);
    const TSTRING& GetPath()							{ return m_path; }

	Resource LoadDxSkinMesh(const TSTRING& szSkinMesh, const TSTRING& szSkeleton,
		BOOL bMeshThread, BOOL bTexThread );

	bool ReLoadDxSkinMesh(const TSTRING& szSkinMesh, const TSTRING& szSkeleton, Resource& res, DxSkeletonResource& sres);
	bool ReLoadData(const TSTRING& szSkinMesh, bool bMustDirectory, const SKINMESH_ADD_DATA& addData, Resource& res, DxSkeletonResource& sres);

	BOOL ExistFile( const TSTRING& strName );

	void OnFindInverseScale();
	//void FindInverseScale( const TCHAR* pNameCPS );

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

private:
	virtual void ProcessCompleteData(
		SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING				m_path;
	sc::CFileFindTree	m_sFileTree;
};

typedef DxSkinMeshManager::Resource DxSkinMeshResource;

#endif
