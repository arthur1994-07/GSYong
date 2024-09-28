#include "pch.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/DebugSet.h"
#include "../SigmaCore/gassert.h"
#include "../SigmaCore/DebugInclude.h"

#include "./DxTools/NsLoadingLagTester.h"

#include "./ResourceManager.h"

namespace 
{
	class VarResourceVistor : public boost::static_visitor<>
	{
	public:
		VarResourceVistor(ResourceThreadInterface* pResourceMgr) { m_pResourceMgr = pResourceMgr; }
		template <typename T>
		void operator()( T& spData ) const
		{
			// 존재 하지 않는다
			if( spData == NULL )
				return;

			const TSTRING& pathFileName = spData->GetPathFileName(); 
            MIN_ASSERT( !pathFileName.empty() );

            UINT nDataSize = 0;
            CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( pathFileName, nDataSize );

			if( spCTypeRawData == NULL )
			{	
				sc::writeLogWarn( sc::string::format("File Load Error %1%", pathFileName) );
				m_pResourceMgr->DeleteLoadingData( pathFileName );
				spData->m_bFailed = true;
				return;
			}

			SP_RAW_DATA& spRawData = spCTypeRawData.GetSPMemory();
			m_pResourceMgr->PreprocessRawDataWidthBackgroundThread( spRawData, nDataSize, reinterpret_cast<const void*>( &spData->m_addData ) );

			// 이걸 마지막에 해줘야함 이걸로 완료됐는지 판별함으로
            spData->m_rawDataSize = nDataSize;
			spData->m_spRawData = spRawData;
		}

	private:
		ResourceThreadInterface* m_pResourceMgr;
	};
}

ResourceThreadManager& ResourceThreadManager::GetInstance()
{
	static ResourceThreadManager RTM; 
	return RTM;
}

HANDLE ResourceThreadManager::m_hWaitEvent = NULL;

ResourceThreadManager::ResourceThreadManager()
{
    m_bStop = false;
    m_hWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

ResourceThreadManager::~ResourceThreadManager()
{
    if (!m_hWaitEvent)
        CloseHandle(m_hWaitEvent);
}

void ResourceThreadManager::SleepThread(DWORD msTick)
{
    DWORD dwReturn = WaitForSingleObject(m_hWaitEvent, msTick);
    if (WAIT_OBJECT_0 == dwReturn)
        return;
    else if (WAIT_TIMEOUT == dwReturn)
        return;
    else
        return;
}

// TAG_CreateResourceManager_5a
void ResourceThreadManager::Register( ResourceThreadInterface& resourceMgr )
{
	if( m_resourceMgrList.end() != std::find(m_resourceMgrList.begin(),m_resourceMgrList.end(),&resourceMgr) )
		return;

	m_resourceMgrList.push_back(&resourceMgr);
}

void ResourceThreadManager::Run()
{
	// 	std:: vector< ResourceThreadInterface* > validQList;
	// 	BOOST_FOREACH(ResourceThreadInterface* resQ , m_loadingQueueList )
	// 	{
	// 		if( !resQ->IsEmpty() )
	// 			validQList.push_back(resQ);
	// 	}
	// 
	// 	if( validQList.empty() )
	// 	{
	// 		SleepThread();
	// 		return;
	// 	}
	// 	
	// 
	// 	BOOST_FOREACH(ResourceThreadInterface* resQ , validQList )

	// 반드시 리소스 매니저들이 모두 생성된 다음에 쓰레드를 시작하자 , 동기화 문제
	BOOST_FOREACH(ResourceThreadInterface* resQ , m_resourceMgrList )
	{
		ResourceThreadInterface::Data data = resQ->GetReadyFrontData();		
//		ResourceThreadInterface::Data data = m_resourceMgrList[i]->GetReadyFrontData();
		boost::apply_visitor(VarResourceVistor(resQ), data );

	}	

	SleepThread();
}

// TAG_StartResourceThreadManager_2
void ResourceThreadManager::Start()
{
	if( m_thread.joinable() )
	{
		MIN_ASSERT(false);
		return;
	}

	m_thread = boost::thread(WorkerThread(this));	
	m_bStop = false;
}

void ResourceThreadManager::Stop()
{
	m_bStop = true;

	if( m_thread.joinable() )
		m_thread.join();
}

//////////////////////////////////////////////////////////////////////////

void ResourceThreadManager::WorkerThread::operator()()
{
	while( !m_pThreadManager->IsStopped() )
	{
		m_pThreadManager->Run();
	}
}

//////////////////////////////////////////////////////////////////////////

ResourceUpdatorManager& ResourceUpdatorManager::GetInstance()
{
	static ResourceUpdatorManager RUM;
	return RUM;
}

// TAG_CreateResourceManager_5b
void ResourceUpdatorManager::Register( ResourceUpdatorInterface& updator)
{
	if( m_updatorVec.end() != std::find(m_updatorVec.begin(),m_updatorVec.end(),&updator) )
		return;

	m_updatorVec.push_back(&updator);
}

// TAG_UpdateResourceManager_2
void ResourceUpdatorManager::Update()
{
    // 이번 프레임에 5ms 이상 소모하면 다음 프레임으로 넘긴다
	// 또한 UpdateAndPopCompletedVector 에서는 이걸 갯수로 생각해서 완료된 RawData List를 줄이기도 한다.
    DWORD nWorkTime = 5;
    size_t nUpdateIndex = m_nUpdateIndex;

	DWORD dwTick(timeGetTime());

    size_t nManagerCount = m_updatorVec.size();
    for( size_t i=0; i<nManagerCount; ++i )
    {
        if( nUpdateIndex >= nManagerCount )
            nUpdateIndex = 0;

        if( !m_updatorVec[ nUpdateIndex++ ]->Update( dwTick, nWorkTime ) )
		{
            m_nUpdateIndex = nUpdateIndex;
			break;
		}
    }

#ifdef _RELEASED
	if ( NSLoadingLagTester::g_dwResourceUpdatorManager_Update < timeGetTime() - dwTick )
		NSLoadingLagTester::g_dwResourceUpdatorManager_Update = timeGetTime() - dwTick;
#endif
}

void ResourceUpdatorManager::Clear()
{
	BOOST_FOREACH(ResourceUpdatorInterface* pUpdator,m_updatorVec)
	{
		pUpdator->Clear();
	}
}




