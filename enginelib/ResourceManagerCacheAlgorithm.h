


#ifndef __RESOURCE_MANAGER_CACHE_ALGORITHM_H__
#define __RESOURCE_MANAGER_CACHE_ALGORITHM_H__

#include "./ResourceManager.h"


// TAG_ClassTextureManager_3
// TAG_CreateResourceManager_3
/// 기본적인 시간에 기반한 캐쉬 알고리즘에 쓰일 기본 데이터 정의
template<class COMPLETED_DATA,class ADD_DATA,class CACHE_DATA>
class BaseTimeCacheAlogorithm : public ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>
{
protected:
	/// 시간 설정을 바꾸려면 이걸 이용 현재는 독립적인 타이머를 쓰지만 , 시스템 내부 타이머 값을
	/// 받아 써도 될것이다
	DWORD GetTimeTick() { return ::timeGetTime(); }
public:
	BaseTimeCacheAlogorithm()
	{
		// 메니저가 캐쉬를 업데이트할때 , 나누어서 할것인지 결정
		// 캐쉬를 업데이트할때마다 m_divideDataCountForCacheTime 단위로 나누어 시간 갱신 
		m_divideDataCountForCacheTime = 500;

		// 캐쉬에 남아 있는 시간 5분
		m_deleteCacheTime = 1000 * 60 * 5; 

		// 1초마다 시간 갱신
		m_updateCacheTime = 1000;

		// 캐쉬때 쓰는용
		m_preCacheTime = 0;
	}

	/// @brief  캐쉬에서 지워지는 시간 
	/// @remark 참조하는 객체가 더이상 없을 경우부터 적용된다 
	void SetDeleteCacheTime(DWORD timeTick) {  m_deleteCacheTime = timeTick; }
	/// 캐쉬에서 지워지는 시간 리턴
	DWORD GetDeleteCacheTime() { return m_deleteCacheTime; }

	/// 캐쉬 업데이트 시키는 주기를 설정한다 , 너무 자주해줄 필요는 없기에 디폴트값을 초단위로 설정해도 될것이다
	void SetCacheUpdateTime(DWORD timeTick) { m_updateCacheTime = timeTick; }
	/// 캐쉬 업데이트 시키는 주기 시간 리턴
	DWORD GetCacheUpdateTime() { return m_updateCacheTime; }

	virtual void CacheAlogorithm() = 0;

private:
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
protected:
	/// m_completedDataMap를 부분적으로 나누어서 업데이트할때 쓰이는 키값
	TSTRING m_divideDataBreakPointKeyForCacheTime;
	/// m_completedDataMap를 업데이트할때 나누는 단위 갯수
	UINT m_divideDataCountForCacheTime;


	/// 몇초마다 업데이트
	DWORD m_updateCacheTime;
	/// 전 시간기록
	DWORD m_preCacheTime;

	/// 참조하는게 없을 경우 삭제 시간 설정
	DWORD m_deleteCacheTime;

	// 현재 시간
	DWORD m_currentTime; 
};


typedef DWORD RESOURCE_TIME;

/// 기본 타이머 캐쉬 알고리즘
template<class COMPLETED_DATA,class ADD_DATA = EMPTY_ADD_DATA,class CACHE_DATA = RESOURCE_TIME>
class DefaultTimeCacheAlogorithm : public BaseTimeCacheAlogorithm<COMPLETED_DATA,ADD_DATA,CACHE_DATA>
{
	// TAG_UpdateResourceManager_4a
	/// 시간에 기반해 작성되어짐
	virtual void CacheAlogorithm()
	{
		m_currentTime = GetTimeTick();
		// m_updateCacheTime 마다 한번씩만 해줘도 된다  너무 자주해줄 필요가 없다
		if( m_currentTime - m_preCacheTime < m_updateCacheTime )
			return;

		m_preCacheTime = m_currentTime;

		//////////////////////////////////////////////////////////////////////////
		// 분할 알고리즘은 Map의 특성상 이름으로 정렬되어 있으니 , 
		// m_divideDataCount 갯수 만큼만 업데이트하고 중단된 지점은  
		// m_divideDataKeyCacheBreakPoint에 기록해두었다가
		// 그 다음을 lower_bound로 찾아 다시 시작하자  
		COMPLTED_DATA_MAP::iterator it;

		if( !m_divideDataBreakPointKeyForCacheTime.empty() )
		{
			it = m_completedDataMap.lower_bound(m_divideDataBreakPointKeyForCacheTime);
			if( it == m_completedDataMap.end() )
				it = m_completedDataMap.begin();
			else
			{
				if( it->first < m_divideDataBreakPointKeyForCacheTime )
					++it;
			}
		}
		else
			it = m_completedDataMap.begin();

		UINT updatedCount = 0;
		for ( ;  updatedCount < m_divideDataCountForCacheTime && it != m_completedDataMap.end(); ++updatedCount)
		{
			// 삭제를 대비 
			COMPLTED_DATA_MAP::iterator curIt = it++;
			//////////////////////////////////////////////////////////////////////////
			MANAGED_COMPLETED_DATA& comData = curIt->second;
			if( comData.IsUse() )
			{
				comData.m_cacheData = m_currentTime;
				continue;
			}

			if( m_currentTime - comData.m_cacheData >= m_deleteCacheTime )
				m_completedDataMap.erase(curIt);
		}

		if( updatedCount == m_divideDataCountForCacheTime && it != m_completedDataMap.end() )
			m_divideDataBreakPointKeyForCacheTime = it->first;
		else 
			m_divideDataBreakPointKeyForCacheTime.clear();

		// 완벽히 m_divideDataCountForCacheTime 만큼 돌지 않는 이유는 중복해서 처리될수 있기 때문이고
		// 그것까지 고려하려면 알고리즘이 너무 복잡해짐. 반드시 업데이트를 
		// 꼭 다 해야할 사항도 아니기때문에 오히려 마이너스

	}

private:
	/// 캐쉬가 갱신될때마다 호출
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) {	pManagedData->m_cacheData = GetTimeTick(); }
	/// 캐쉬가 등록될때 호출
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) { CachedData(pManagedData); }
};


// // 
struct OnceRefNTime
{
	OnceRefNTime() { m_time = 0; m_bOnce = false;}
	DWORD m_time;
	bool  m_bOnce;
};

// TAG_ClassTextureManager_2
// TAG_CreateResourceManager_2
/// 현재 게임에서는 기본 베이스 알고리즘을 사용할 경우 
/// 랜더링이 안되어 캐쉬에서 내려가버린후, 참조가 이루어져  택스쳐가 보이지 않는 문제가 발생되어짐
/// 해결방항으로는 백그라운드 로딩시에는 매 프레임마다 로딩 체크용 Update를 호출하거나 
/// OnceRefAndEnableAlgorithm와 같은 알고리즘으로 해결가능한데  캐쉬 알고리즘을 고치는 방법으로 해결
/// 즉 OnceRefAndEnableAlgorithm는 참조가 한번 이루어진후부터 캐쉬에서 제거될수 있는 알고리즘임
/// DefaultTimeCacheAlogorithm 와 차이는 한번 참조된 이후부터 제거되는 거 밖에 없다
template<class COMPLETED_DATA,class ADD_DATA,class CACHE_DATA>
class OnceRefAndEnableAlgorithm : public BaseTimeCacheAlogorithm<COMPLETED_DATA,ADD_DATA,CACHE_DATA>
{
public:
	// TAG_UpdateResourceManager_4a
	virtual void CacheAlogorithm()
	{
		m_currentTime = GetTimeTick();
		// m_updateCacheTime 마다 한번씩만 해줘도 된다  너무 자주해줄 필요가 없다
		if( m_currentTime - m_preCacheTime < m_updateCacheTime )
			return;

		m_preCacheTime = m_currentTime;

		//////////////////////////////////////////////////////////////////////////
		// 분할 알고리즘은 Map의 특성상 이름으로 정렬되어 있으니 , 
		// m_divideDataCount 갯수 만큼만 업데이트하고 중단된 지점은  
		// m_divideDataKeyCacheBreakPoint에 기록해두었다가
		// 그 다음을 lower_bound로 찾아 다시 시작하자  
		COMPLTED_DATA_MAP::iterator it;

		if( !m_divideDataBreakPointKeyForCacheTime.empty() )
		{
			it = m_completedDataMap.lower_bound(m_divideDataBreakPointKeyForCacheTime);
			if( it == m_completedDataMap.end() )
				it = m_completedDataMap.begin();
			else
			{
				if( it->first < m_divideDataBreakPointKeyForCacheTime )
					++it;
			}
		}
		else
			it = m_completedDataMap.begin();

		UINT updatedCount = 0;
		for ( ;  updatedCount < m_divideDataCountForCacheTime && it != m_completedDataMap.end(); ++updatedCount)
		{
			// 삭제를 대비 
			COMPLTED_DATA_MAP::iterator curIt = it++;
			//////////////////////////////////////////////////////////////////////////
			MANAGED_COMPLETED_DATA& comData = curIt->second;
			if( comData.IsUse() )
			{
				comData.m_cacheData.m_time = m_currentTime;
				continue;
			}

			if( m_currentTime - comData.m_cacheData.m_time >= m_deleteCacheTime )
			{
				if(comData.m_cacheData.m_bOnce)
					m_completedDataMap.erase(curIt);
			}
		}

		if( updatedCount == m_divideDataCountForCacheTime && it != m_completedDataMap.end() )
			m_divideDataBreakPointKeyForCacheTime = it->first;
		else 
			m_divideDataBreakPointKeyForCacheTime.clear();

		// 완벽히 m_divideDataCountForCacheTime 만큼 돌지 않는 이유는 중복해서 처리될수 있기 때문이고
		// 그것까지 고려하려면 알고리즘이 너무 복잡해짐. 반드시 업데이트를 
		// 꼭 다 해야할 사항도 아니기때문에 오히려 마이너스

	}

private:

	// 참조가 되면 이함수가 호출됨. m_bOnce 로 참조되는지 확인함.
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) 
	{
		pManagedData->m_cacheData.m_time = GetTimeTick(); 
		pManagedData->m_cacheData.m_bOnce = true;
	}

	// TAG_UpdateResourceManager_4cb
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) 
	{
		pManagedData->m_cacheData.m_time = m_currentTime; 
		pManagedData->m_cacheData.m_bOnce = false;
	}

};




#endif

