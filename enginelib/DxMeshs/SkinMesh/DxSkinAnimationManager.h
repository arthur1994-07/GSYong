#ifndef _DXSKIN_ANI_MAN_H_
#define _DXSKIN_ANI_MAN_H_

#pragma once

#include <hash_map>
#include <queue>

#include "../../../SigmaCore/CoreCommon.h"

//#include <string>
#include "../../../SigmaCore/gassert.h"
//#include "../../CoreCommon/Lock.h"
//#include "./DxSkinAniControl.h"
#include "../DxAniKeys.h"
#include "./SAnimationDefine.h"

#include "../../ResourceManagerCacheAlgorithm.h"

#define	E_DIFFERENT_BONEFILE MAKE_HRESULT(1,27,100)

struct DxSkinAnimation;

#ifndef LPDIRECT3DDEVICEQ
#define LPDIRECT3DDEVICEQ			LPDIRECT3DDEVICE9
#endif


class DxSkinAnimationManager : public OnceRefAndEnableAlgorithm<DxSkinAnimation,EMPTY_ADD_DATA,OnceRefNTime>
{
public:
	struct Resource : public ResourceManager::Resource
	{
		Resource(const ResourceManager::Resource& res) : ResourceManager::Resource(res) {}
		Resource() {}

		DxSkinAnimation* get() const
		{
			MIN_ASSERT(m_spCompletedData.get());
			return m_spCompletedData.get(); 
		}
	};

public:
	static DxSkinAnimationManager& GetInstance();

	void						SetPath (const TSTRING& path )	{ m_path = path; }
	const TSTRING& 		GetPath ()							{ return m_path; }

	void SetDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ m_pd3dDevice = pd3dDevice; }


	BOOL SaveAnimConInfo( const TSTRING& pNameXFile, const TSTRING& pNameCFG );

	Resource LoadAnimationCFG(const TSTRING& nameCFG );

	Resource LoadAnimContainerFromTool(
		const TSTRING& nameCFG,
		const TSTRING& nameXFile,
		const TSTRING& nameSkeleton,
		BOOL bRefesh);

	//// [shhan][2014.12.08] 이전에는 팔벌리는 문제로 List 를 등록 시켰는데 지금은 애니메이션이 로딩 안되면 렌더 조차 안하기 때문에
	////						Preload 를 할 필요가 없다.
	//BOOL BackUpList(const TCHAR* szAniContList );
	//void ReloadBackUpList();

	virtual void DeleteCacheDataAll() OVERRIDE;

private:
	virtual void ProcessCompleteData(
		SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

private:
	DxSkinAnimationManager(void);
    virtual ~DxSkinAnimationManager(void);

private:
	TSTRING			m_path;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

	TSTRING			m_backupListFileName;
};

typedef	DxSkinAnimationManager::Resource DxSkinAnimationRes;


namespace COMMENT
{
	extern std::string ANI_MAINTYPE[AN_TYPE_SIZE];
	extern std::string ANI_ATTBONE_MAINTYPE[AN_ATTBONE_SIZE];
	extern std::string ANI_VEHICLE_MAINTYPE[AN_VEHICLE_SIZE];
	extern std::string ANI_SUBTYPE[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_00[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_01[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_02[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_03[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_FLY[AN_SUB_FLY_SIZE]; // 주병진

	extern std::string STRIKE_EFF[EMSF_SIZE];
	
	CString GetAnimMainType(int nType);
	CString GetAnimSubType(int nMType, int nSType);
}



#endif // _DXSKIN_ANI_MAN_H_