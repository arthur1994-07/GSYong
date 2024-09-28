#include "pch.h"

#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxTools/NsLoadingLagTester.h"

#include "DxAnimationListManager.h"

#include "DxSkeletonManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



DxAnimationListManager& DxAnimationListManager::GetInstance()
{
	static DxAnimationListManager s; return s;
}

DxAnimationListManager::DxAnimationListManager()
{
	m_pd3dDevice = NULL;
	m_bToolMode = false;
	m_deleteCacheTime = 1000 * 60 * 20; 
}

DxAnimationListManager::~DxAnimationListManager()
{
}


DxAnimationListManager::Resource 
DxAnimationListManager::LoadAnimList_XFile( SANIMCONINFO& sAnimInfo,
													  DxSkeleton* pSkeleton )
{
	MAP_ANIMLIST::iterator it = m_xFileAnimList.find(sAnimInfo.m_strXFile);
	if ( it != m_xFileAnimList.end() )
		return it->second;

	// 생성후 로딩한다.
	SP_COMPLETED_DATA spDxAniList( new DxAnimationList );
	if( FAILED( spDxAniList->LoadAnimList_XFile(m_pd3dDevice, sAnimInfo, pSkeleton ) ) )
		return Resource();

	Resource res(spDxAniList);
	m_xFileAnimList.insert( std::make_pair( sAnimInfo.m_strXFile, res ) );
	return res;
}

void DxAnimationListManager::DeleteXFile(const TSTRING& xFileName)
{
	MAP_ANIMLIST::iterator it = m_xFileAnimList.find(xFileName);
	if ( it != m_xFileAnimList.end() )
		m_xFileAnimList.erase(it);
}

DxAnimationListManager::Resource 
DxAnimationListManager::LoadAnimList_BIN(
    const TSTRING& nameBin,
    const TSTRING& nameSkeleton,
    bool bBackgroundLoading )
{
	if(m_bToolMode)
		bBackgroundLoading = false;

	DxSkeleton* pSkeleton = DxSkeletonManager::GetInstance().LoadSkeleton(nameSkeleton,FALSE).get();
	if ( !pSkeleton )
		return DxAnimationListManager::Resource();

	ANIMATION_ADD_DATA sAddData;
	sAddData.m_pSkeleton = pSkeleton;

	Resource res = LoadData(
        GetPath() + nameBin,
        false,
        sAddData,
        bBackgroundLoading );

	//BackUpCache(res);

	return res;
}

// 백그라운드 쓰레드 로우 데이타를 전처리해줄게 있다면 여기서 해주면 된다 단 멀티 쓰레드임을 잊지 말자
void DxAnimationListManager::PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData)
{
	DxAnimationListDummy* pAniListDummy = new DxAnimationListDummy;
	DxAnimationList* pAnimationList = new DxAnimationList;
	pAniListDummy->m_pAnimationList = pAnimationList;
	MIN_ASSERT( S_OK == pAniListDummy->m_pAnimationList->LoadAnimList_BIN( spData.get(), size,reinterpret_cast<const ANIMATION_ADD_DATA*>( pAddData )->m_pSkeleton ) );

	spData.reset();

	spData = SP_RAW_DATA( pAniListDummy );
}

void DxAnimationListManager::ProcessCompleteData(
    SP_QUEUE_DATA_VECTOR& dataVec,
    NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
    SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
    while( it != dataVec.end() )
    {
		DWORD dwTick( timeGetTime() );

        const SP_QUEUE_DATA& spResData = *it;
        ++it;

        MIN_ASSERT(!spResData->m_fileName.empty());
        MIN_ASSERT(spResData->m_spRawData != NULL);
        MIN_ASSERT(m_pd3dDevice);

		DxAnimationListDummy* pAniListDummy = reinterpret_cast<DxAnimationListDummy*>( spResData->m_spRawData.get() );
		DxAnimationList* pAniList = pAniListDummy->m_pAnimationList;

		spResData->m_spRawData.reset();

		SP_COMPLETED_DATA spAniList(pAniList);

        pCompletedDataVec->push_back( std::make_pair(spResData->m_fileName,spAniList) );

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxAnimationListManager_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_DxAnimationListManager_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strDxAnimationListManager_ProcessCompleteData = spResData->m_fileName;
		}
#endif
		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxAnimationListManager bin Load %1% - %2%", spResData->m_fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
    }

    if( !dataVec.empty() )
        dataVec.erase( dataVec.begin(), it );
}