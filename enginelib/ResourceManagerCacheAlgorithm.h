


#ifndef __RESOURCE_MANAGER_CACHE_ALGORITHM_H__
#define __RESOURCE_MANAGER_CACHE_ALGORITHM_H__

#include "./ResourceManager.h"


// TAG_ClassTextureManager_3
// TAG_CreateResourceManager_3
/// �⺻���� �ð��� ����� ĳ�� �˰��� ���� �⺻ ������ ����
template<class COMPLETED_DATA,class ADD_DATA,class CACHE_DATA>
class BaseTimeCacheAlogorithm : public ResourceManager<COMPLETED_DATA,CACHE_DATA,ADD_DATA>
{
protected:
	/// �ð� ������ �ٲٷ��� �̰� �̿� ����� �������� Ÿ�̸Ӹ� ������ , �ý��� ���� Ÿ�̸� ����
	/// �޾� �ᵵ �ɰ��̴�
	DWORD GetTimeTick() { return ::timeGetTime(); }
public:
	BaseTimeCacheAlogorithm()
	{
		// �޴����� ĳ���� ������Ʈ�Ҷ� , ����� �Ұ����� ����
		// ĳ���� ������Ʈ�Ҷ����� m_divideDataCountForCacheTime ������ ������ �ð� ���� 
		m_divideDataCountForCacheTime = 500;

		// ĳ���� ���� �ִ� �ð� 5��
		m_deleteCacheTime = 1000 * 60 * 5; 

		// 1�ʸ��� �ð� ����
		m_updateCacheTime = 1000;

		// ĳ���� ���¿�
		m_preCacheTime = 0;
	}

	/// @brief  ĳ������ �������� �ð� 
	/// @remark �����ϴ� ��ü�� ���̻� ���� ������ ����ȴ� 
	void SetDeleteCacheTime(DWORD timeTick) {  m_deleteCacheTime = timeTick; }
	/// ĳ������ �������� �ð� ����
	DWORD GetDeleteCacheTime() { return m_deleteCacheTime; }

	/// ĳ�� ������Ʈ ��Ű�� �ֱ⸦ �����Ѵ� , �ʹ� �������� �ʿ�� ���⿡ ����Ʈ���� �ʴ����� �����ص� �ɰ��̴�
	void SetCacheUpdateTime(DWORD timeTick) { m_updateCacheTime = timeTick; }
	/// ĳ�� ������Ʈ ��Ű�� �ֱ� �ð� ����
	DWORD GetCacheUpdateTime() { return m_updateCacheTime; }

	virtual void CacheAlogorithm() = 0;

private:
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
protected:
	/// m_completedDataMap�� �κ������� ����� ������Ʈ�Ҷ� ���̴� Ű��
	TSTRING m_divideDataBreakPointKeyForCacheTime;
	/// m_completedDataMap�� ������Ʈ�Ҷ� ������ ���� ����
	UINT m_divideDataCountForCacheTime;


	/// ���ʸ��� ������Ʈ
	DWORD m_updateCacheTime;
	/// �� �ð����
	DWORD m_preCacheTime;

	/// �����ϴ°� ���� ��� ���� �ð� ����
	DWORD m_deleteCacheTime;

	// ���� �ð�
	DWORD m_currentTime; 
};


typedef DWORD RESOURCE_TIME;

/// �⺻ Ÿ�̸� ĳ�� �˰���
template<class COMPLETED_DATA,class ADD_DATA = EMPTY_ADD_DATA,class CACHE_DATA = RESOURCE_TIME>
class DefaultTimeCacheAlogorithm : public BaseTimeCacheAlogorithm<COMPLETED_DATA,ADD_DATA,CACHE_DATA>
{
	// TAG_UpdateResourceManager_4a
	/// �ð��� ����� �ۼ��Ǿ���
	virtual void CacheAlogorithm()
	{
		m_currentTime = GetTimeTick();
		// m_updateCacheTime ���� �ѹ����� ���൵ �ȴ�  �ʹ� �������� �ʿ䰡 ����
		if( m_currentTime - m_preCacheTime < m_updateCacheTime )
			return;

		m_preCacheTime = m_currentTime;

		//////////////////////////////////////////////////////////////////////////
		// ���� �˰����� Map�� Ư���� �̸����� ���ĵǾ� ������ , 
		// m_divideDataCount ���� ��ŭ�� ������Ʈ�ϰ� �ߴܵ� ������  
		// m_divideDataKeyCacheBreakPoint�� ����صξ��ٰ�
		// �� ������ lower_bound�� ã�� �ٽ� ��������  
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
			// ������ ��� 
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

		// �Ϻ��� m_divideDataCountForCacheTime ��ŭ ���� �ʴ� ������ �ߺ��ؼ� ó���ɼ� �ֱ� �����̰�
		// �װͱ��� ����Ϸ��� �˰����� �ʹ� ��������. �ݵ�� ������Ʈ�� 
		// �� �� �ؾ��� ���׵� �ƴϱ⶧���� ������ ���̳ʽ�

	}

private:
	/// ĳ���� ���ŵɶ����� ȣ��
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) {	pManagedData->m_cacheData = GetTimeTick(); }
	/// ĳ���� ��ϵɶ� ȣ��
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
/// ���� ���ӿ����� �⺻ ���̽� �˰����� ����� ��� 
/// �������� �ȵǾ� ĳ������ ������������, ������ �̷����  �ý��İ� ������ �ʴ� ������ �߻��Ǿ���
/// �ذ�������δ� ��׶��� �ε��ÿ��� �� �����Ӹ��� �ε� üũ�� Update�� ȣ���ϰų� 
/// OnceRefAndEnableAlgorithm�� ���� �˰������� �ذᰡ���ѵ�  ĳ�� �˰����� ��ġ�� ������� �ذ�
/// �� OnceRefAndEnableAlgorithm�� ������ �ѹ� �̷�����ĺ��� ĳ������ ���ŵɼ� �ִ� �˰�����
/// DefaultTimeCacheAlogorithm �� ���̴� �ѹ� ������ ���ĺ��� ���ŵǴ� �� �ۿ� ����
template<class COMPLETED_DATA,class ADD_DATA,class CACHE_DATA>
class OnceRefAndEnableAlgorithm : public BaseTimeCacheAlogorithm<COMPLETED_DATA,ADD_DATA,CACHE_DATA>
{
public:
	// TAG_UpdateResourceManager_4a
	virtual void CacheAlogorithm()
	{
		m_currentTime = GetTimeTick();
		// m_updateCacheTime ���� �ѹ����� ���൵ �ȴ�  �ʹ� �������� �ʿ䰡 ����
		if( m_currentTime - m_preCacheTime < m_updateCacheTime )
			return;

		m_preCacheTime = m_currentTime;

		//////////////////////////////////////////////////////////////////////////
		// ���� �˰����� Map�� Ư���� �̸����� ���ĵǾ� ������ , 
		// m_divideDataCount ���� ��ŭ�� ������Ʈ�ϰ� �ߴܵ� ������  
		// m_divideDataKeyCacheBreakPoint�� ����صξ��ٰ�
		// �� ������ lower_bound�� ã�� �ٽ� ��������  
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
			// ������ ��� 
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

		// �Ϻ��� m_divideDataCountForCacheTime ��ŭ ���� �ʴ� ������ �ߺ��ؼ� ó���ɼ� �ֱ� �����̰�
		// �װͱ��� ����Ϸ��� �˰����� �ʹ� ��������. �ݵ�� ������Ʈ�� 
		// �� �� �ؾ��� ���׵� �ƴϱ⶧���� ������ ���̳ʽ�

	}

private:

	// ������ �Ǹ� ���Լ��� ȣ���. m_bOnce �� �����Ǵ��� Ȯ����.
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

