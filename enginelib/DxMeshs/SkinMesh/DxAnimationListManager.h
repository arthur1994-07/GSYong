

#pragma once

#include "../../../SigmaCore/CoreCommon.h"

#include "../../ResourceManagerCacheAlgorithm.h"

#include "./DxAnimationAddData.h"
#include "./DxAnimationList.h"


struct DxAnimationListManager : public OnceRefAndEnableAlgorithm<DxAnimationList,ANIMATION_ADD_DATA,OnceRefNTime>
{
public:
	struct Resource : public ResourceManager::Resource
	{
		explicit Resource(const SP_COMPLETED_DATA& spData) { m_spCompletedData = spData; }
		Resource(const ResourceManager::Resource& res) : ResourceManager::Resource(res) {}
		Resource() {}

		DxAnimationList* get() const
		{
			MIN_ASSERT(m_spCompletedData.get());
			return m_spCompletedData.get(); 
		}
	};
public:
	static DxAnimationListManager& GetInstance();

	void						SetPath (const TSTRING& path )	{ m_path = path; }
	const TSTRING& 		GetPath ()							{ return m_path; }

	void SetToolMode(bool bToolMode) { m_bToolMode = bToolMode;}

	void SetDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ m_pd3dDevice = pd3dDevice; }

	void DeleteXFile(const TSTRING& xFileName);
	Resource LoadAnimList_XFile( SANIMCONINFO &sAnimInfo, DxSkeleton* pSkeleton );

	Resource LoadAnimList_BIN(const TSTRING& nameBin,const TSTRING& nameSkeleton,
		bool bBackgroundLoading);

	virtual void DeleteCacheDataAll() OVERRIDE { ResourceManager::DeleteCacheDataAll(); m_xFileAnimList.clear(); }
	virtual void Clear() OVERRIDE { ResourceManager::Clear(); m_xFileAnimList.clear(); }

private:
	virtual void ProcessCompleteData(
        SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );

	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData);

private:
	DxAnimationListManager();
	~DxAnimationListManager();

private:
	typedef std::map<TSTRING,Resource> MAP_ANIMLIST;

	TSTRING			m_path;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	MAP_ANIMLIST			m_xFileAnimList;
	bool					m_bToolMode;
};


typedef DxAnimationListManager::Resource DxAnimationListResource;