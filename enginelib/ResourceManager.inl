


#ifndef __RESOURCE_MANAGER_INL__
#define __RESOURCE_MANAGER_INL__

#include "./DxTools/NsLoadingLagTester.h"
#include "./ResourceManager.h"




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// TAG_LoadResourceManager_3a_2
template<class ADD_DATA>
typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA 
ResourceQueue<ADD_DATA>::PushData( const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bFront )
{
	//////////////////////////////////////////////////////////////////////////
	TSTRING fileName;
	sc::string::getFullFileName(pathFileName,fileName);
	//////////////////////////////////////////////////////////////////////////

	THIS_AUTO_DEFAULT_LOCK;

	QUEUE_INDEX_MAP::iterator gotIt = m_pQueueIndexHash->find(fileName);
	if( gotIt != m_pQueueIndexHash->end() )
	{
		if( !bFront )
			return *gotIt;

		SP_QUEUE_DATA spData = *gotIt;
		spData->m_addData = addData;
		m_pQueueIndexHash->erase(gotIt);
		m_pQueueIndexSeq->push_front(spData);
		return spData;
	}

	//////////////////////////////////////////////////////////////////////////
	// 생성은 오직 여기서만
	SP_QUEUE_DATA spQueueData(new QUEUE_DATA(pathFileName,bMustDirectory,addData));
	//////////////////////////////////////////////////////////////////////////

	if(bFront)
		m_pQueueIndexSeq->push_front(spQueueData);
	else
		m_pQueueIndexSeq->push_back(spQueueData);

	return spQueueData;
}


template<class ADD_DATA>
bool ResourceQueue<ADD_DATA>::Find( const TSTRING& fileName )
{
	THIS_AUTO_DEFAULT_LOCK;
	return m_pQueueIndexHash->end() != m_pQueueIndexHash->find(fileName);
}

template<class ADD_DATA>
void ResourceQueue<ADD_DATA>::Clear()
{
	THIS_AUTO_DEFAULT_LOCK;
	m_multiindexDataQueue.clear();
}


// 
template<class ADD_DATA>
void ResourceQueue<ADD_DATA>::DeleteData( const TSTRING& fileName )
{
	THIS_AUTO_DEFAULT_LOCK;
	QUEUE_INDEX_MAP::iterator it =	m_pQueueIndexHash->find(fileName);
	if( it != m_pQueueIndexHash->end() )
		m_pQueueIndexHash->erase(it);

}

// TAG_UpdateResourceManager_4b
template<class ADD_DATA>
void ResourceQueue<ADD_DATA>::UpdateAndPopCompletedVector( SP_QUEUE_DATA_VECTOR* pDataVec, DWORD nMaxCnt )
{
	THIS_AUTO_DEFAULT_LOCK;

    DWORD nInsertCnt = 0;

	for (QUEUE_INDEX_SEQUENCE::iterator it = m_pQueueIndexSeq->begin(); it != m_pQueueIndexSeq->end(); )
	{
		// 반복자 무효화에 대한 방지
		QUEUE_INDEX_SEQUENCE::iterator curIt = it++;
		if( (*curIt)->IsCompleted() )
		{
			pDataVec->push_back(*curIt);
			m_pQueueIndexSeq->erase(curIt);

            if( ++nInsertCnt >= nMaxCnt )
                break;
		} 
	}
}

template<class ADD_DATA>
void ResourceQueue<ADD_DATA>::Rollback( SP_QUEUE_DATA_VECTOR* pDataVec )
{
	THIS_AUTO_DEFAULT_LOCK;

    BOOST_FOREACH( const SP_QUEUE_DATA& spData, *pDataVec )
    {
        m_pQueueIndexSeq->push_front( spData );
    }
}

template<class ADD_DATA>
typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA ResourceQueue<ADD_DATA>::_GetReadyFrontData()
{
	THIS_AUTO_DEFAULT_LOCK;

	BOOST_FOREACH(const SP_QUEUE_DATA& spData, *m_pQueueIndexSeq)
	{
		if( !spData->IsCompleted() )
			return spData;
	}

	return SP_QUEUE_DATA();
}

template<class ADD_DATA>
bool ResourceQueue<ADD_DATA>::IsEmpty()
{
	THIS_AUTO_DEFAULT_LOCK;
	return m_multiindexDataQueue.empty();
}



template<class ADD_DATA>
ResourceThreadInterface::Data ResourceQueue<ADD_DATA>::GetReadyFrontData()
{
	//////////////////////////////////////////////////////////////////////////
	// 여긴 락이 필요 없다 
	ResourceThreadInterface::Data varData = SP_QUEUE_DATA();
	SP_QUEUE_DATA& data = boost::get< SP_QUEUE_DATA& >(varData);

	data = _GetReadyFrontData();
	return varData;
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
TSTRING ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::GetFileNameFromPathFileName( const TSTRING& pathFileName )
{
	TSTRING fileName; 
	sc::string::getFullFileName(pathFileName,fileName);
	sc::string::lowerString(fileName);
	return fileName;
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::QueryPathFileName( 
	TSTRING* pPathFileName,TSTRING* pFileName )
{
	// 풀패스가 존재하면 그냥 리턴 아님 알아낸다
	TSTRING& lowStringPathFileName = *pPathFileName; 
	sc::string::lowerString( lowStringPathFileName );	

	TSTRING path,fileExt;
	sc::string::getFilePathAndName(lowStringPathFileName,path,*pFileName,fileExt);

	if( path.empty() )
		lowStringPathFileName = QueryPathFileNameHelper(lowStringPathFileName,path,*pFileName);
}



template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
typename ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::Resource 
ResourceManager<COMPLETED_DATA,CACHE_DATA, ADD_DATA>::CacheAndReturnData(const TSTRING& fileName,
															  MANAGED_COMPLETED_DATA& resData )
{
	// 시간을 기록해둔다 캐쉬후보군
	CachedData(&resData);
	return Resource( fileName, this,resData.m_spData );
}

// TAG_LoadResourceManager_3a_1
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
typename ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::SP_QUEUE_DATA
ResourceManager<COMPLETED_DATA,CACHE_DATA, ADD_DATA>::LoadRawData( const TSTRING& pathFileName,bool bMustDirectory,
													   const ADD_DATA& addData,bool bFront /*= false*/ )
{
	return m_loadingQueue.PushData(pathFileName,bMustDirectory,addData,bFront);
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
bool ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::DeleteCacheData( const TSTRING& fileName )
{
	// Ref : 이 함수가 호출되더라도 , 다른 객체가 fileName 리소스를 , 참조하고 있다면 
	// 실패한다 반드시 참조하고 있는 객체가 더이상 없고 단지 캐쉬되고 있는 리소스만 제거가 가능하다
	// return 값의 의미는 캐쉬에 남아 있느냐의 유무 일뿐
	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find(GetFileNameFromPathFileName(fileName));
	if( m_completedDataMap.end() == it )
		return true;

	if( it->second.IsUse() )
		return false;

	m_completedDataMap.erase(it);
	return true;
}


template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::DeleteCacheDataAll()
{
	for( COMPLTED_DATA_MAP::iterator it = m_completedDataMap.begin(); it != m_completedDataMap.end(); )
	{
		COMPLTED_DATA_MAP::iterator curIt = it++;
		if( !curIt->second.IsUse() )
			m_completedDataMap.erase(curIt);
	}
}

// TAG_UpdateResourceManager_4cb_1
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
bool ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::BackUpCache( const TSTRING& fileName )
{
	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find(GetFileNameFromPathFileName(fileName)); 
	if( it != m_completedDataMap.end() )
	{
		m_backupCacheCompletedDataMap.insert(*it);
		return true;
	}

	return false;
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::DeleteData( const TSTRING& fileName )
{
	const TSTRING& lowFileName = GetFileNameFromPathFileName(fileName);
	m_loadingQueue.DeleteData(lowFileName);
	m_completedDataMap.erase(lowFileName);
	m_backupCacheCompletedDataMap.erase(lowFileName);
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
typename ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::Resource 
ResourceManager<COMPLETED_DATA,CACHE_DATA, ADD_DATA>::LoadData(
	const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,
	bool bBackgroundLoading)
{
	//////////////////////////////////////////////////////////////////////////
	/// 넘겨준 Path를 깔끔하게 변환
	TSTRING lowPathFileName = pathFileName;
	TSTRING fileName;
	QueryPathFileName( &lowPathFileName, &fileName );
	//////////////////////////////////////////////////////////////////////////

	return LoadData( lowPathFileName, fileName, bMustDirectory, addData, bBackgroundLoading );
}

// TAG_LoadResourceManager_2
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
typename ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::Resource 
ResourceManager<COMPLETED_DATA,CACHE_DATA, ADD_DATA>::LoadData(
	const TSTRING& pathFileName,const TSTRING& strSettingName,bool bMustDirectory,const ADD_DATA& addData,
	bool bBackgroundLoading)
{
	TSTRING lowPathFileName = pathFileName;

	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find( strSettingName );
	if( m_completedDataMap.end() != it )
		return CacheAndReturnData( strSettingName, it->second );

	if( bBackgroundLoading )
	{
		LoadRawData( lowPathFileName, bMustDirectory, addData );
	}
	else
	{
#ifdef _RELEASED
		DWORD dwTick1 = timeGetTime();
#endif



		UINT nDataSize = 0;
		CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( lowPathFileName, nDataSize );

		if( spCTypeRawData != NULL )
		{


#ifdef _RELEASED
			if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData1 < timeGetTime() - dwTick1 )
			{
				NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData1 = timeGetTime() - dwTick1;
			}
			DWORD dwTick = timeGetTime();
#endif



			SP_RAW_DATA& spRawData = spCTypeRawData.GetSPMemory();
			PreprocessRawDataWidthBackgroundThread( spRawData, nDataSize, reinterpret_cast<const void*>( &addData ) );



#ifdef _RELEASED
			if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData2 < timeGetTime() - dwTick )
			{
				NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData2 = timeGetTime() - dwTick;
			}
			dwTick = timeGetTime();
#endif



			SP_QUEUE_DATA spQueueData( new QUEUE_DATA( lowPathFileName, strSettingName, bMustDirectory, addData ) );
			spQueueData->m_rawDataSize = nDataSize;
			spQueueData->m_spRawData = spRawData;

			ResourceQueue<ADD_DATA>::SP_QUEUE_DATA_VECTOR completedRawDataVec;
			completedRawDataVec.push_back( spQueueData );

			DWORD nWorkTime = ULONG_MAX;
			ProcessCompleteDataAndCache( completedRawDataVec, 0, nWorkTime );



#ifdef _RELEASED
			if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData3 < timeGetTime() - dwTick )
			{
				NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData3 = timeGetTime() - dwTick;
			}
#endif


			it = m_completedDataMap.find( strSettingName );
			if( it != m_completedDataMap.end() )
			{


#ifdef _RELEASED
				if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData4 < timeGetTime() - dwTick1 )
				{
					NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData4 = timeGetTime() - dwTick1;
					NSLoadingLagTester::g_strSkinMeshManager_ProcessCompleteData4 = lowPathFileName;
				}
#endif


				return Resource( strSettingName, this, it->second.m_spData );
			}
		}



#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData4 < timeGetTime() - dwTick1 )
		{
			NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData4 = timeGetTime() - dwTick1;
			NSLoadingLagTester::g_strSkinMeshManager_ProcessCompleteData4 = lowPathFileName;
		}
#endif
	}

	return Resource( strSettingName, this );
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::IsExistLoadingRawData( const TSTRING& fileName )
{
	return m_loadingQueue.Find(GetFileNameFromPathFileName(fileName));
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::DeleteLoadingData( const TSTRING& fileName )
{
	m_loadingQueue.DeleteData(GetFileNameFromPathFileName(fileName));
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
typename ResourceManager<COMPLETED_DATA,CACHE_DATA, ADD_DATA>::SP_COMPLETED_DATA 
ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::GetCompletedDataForUpdate( const TSTRING& fileName )
{
	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find(GetFileNameFromPathFileName(fileName));
	if( m_completedDataMap.end() == it )
		return SP_COMPLETED_DATA();

	//////////////////////////////////////////////////////////////////////////
	CachedData(&it->second);
	return it->second.m_spData;
}
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
COMPLETED_DATA* ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::GetCompletedData( const TSTRING& fileName )
{
	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find(GetFileNameFromPathFileName(fileName));
	if( m_completedDataMap.end() == it )
		return NULL;

	//////////////////////////////////////////////////////////////////////////
	return it->second.m_spData.get();
}


// TAG_UpdateResourceManager_3
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
bool ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::Update( DWORD dwPrevTick, DWORD nWorkTime )
{
	CacheAlogorithm();

	// 캐쉬 작업하고 나니 시간이 다 되었다면 나가도록 한다.
	if ( timeGetTime() - dwPrevTick > nWorkTime )
		return false;

    //// 다른 관리자에서 시간을 다 소모해 버렸다면 캐쉬 처리만 하고 나간다
    //if( nWorkTime <= 0 )
    //    return true;

	ResourceQueue<ADD_DATA>::SP_QUEUE_DATA_VECTOR completedRawDataVec;
	m_loadingQueue.UpdateAndPopCompletedVector( &completedRawDataVec, 10 );	// 10개만 꺼내어 보도록 한다.

    ProcessCompleteDataAndCache( completedRawDataVec, dwPrevTick, nWorkTime );

    return ( timeGetTime() - dwPrevTick > nWorkTime ) ? false : true;
}

// TAG_UpdateResourceManager_4c
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::ProcessCompleteDataAndCache(
    SP_QUEUE_DATA_VECTOR& dataVec,
    DWORD dwPrevTick, 
	DWORD nWorkTime )
{

	NAME_N_SP_COMPLETED_DATA_VECTOR completedDataVec; 
	completedDataVec.reserve( dataVec.size() );
	ProcessCompleteData( dataVec, &completedDataVec, dwPrevTick, nWorkTime );

    if( !dataVec.empty() )
        m_loadingQueue.Rollback( &dataVec );

	BOOST_FOREACH( NAME_N_SP_COMPLETED_DATA& nameNspCompletedData, completedDataVec )
	{
		MANAGED_COMPLETED_DATA managedData( nameNspCompletedData.second );
		InitCacheData( &managedData );
		m_completedDataMap[nameNspCompletedData.first] = managedData;

		// [shhan][2014.09.05] raw 파일을 정리한다. 들고 있을 필요가 없다고 생각한다.
		CFileSystem::getInstance().releaseMemory( nameNspCompletedData.first );
	}

	ProcessCompleteDataBackUpCache( completedDataVec );
}

template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
void ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>::PrintData(const TSTRING& logFile)
{
	FILE* pFile = _tfopen(logFile.c_str(),_T("wb"));
	if( pFile == NULL )
		return;

	fwprintf(pFile,_T("----   %s Comepleted Data List Start ----"),typeid(*this).name());
	
	for (COMPLTED_DATA_MAP::iterator it = m_completedDataMap.begin(); it != m_completedDataMap.end(); ++it )
	{
		fwprintf(pFile,_T("File Name : %s \n") ,it->first.c_str() );
	}

	fwprintf(pFile,_T("----   %s  Data List End   ----"),typeid(*this).name());

	fclose(pFile);
}







#endif



