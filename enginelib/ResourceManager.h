
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

/**

@file ResourceManager.h
@brief  ���ҽ��� ���Ϸκ��� �ε带 �� ���ҽ��� �����ϴ� å���� ���� ���� Ŭ����
@author ������
@date 2010-7-19 

*/


#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

#include <boost/foreach.hpp>
#include <boost/utility.hpp>
#include <boost/tr1/memory.hpp>


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
//#include <boost/multi_index/member.hpp>
#include <boost/functional/hash.hpp>

#include <assert.h>

typedef unsigned long DWORD;
typedef unsigned char BYTE;

#include "../SigmaCore/Util/Lock.h"
#include "../SigmaCore/Memory/SmartPtr.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../InternalCommonLib/dxstdafx.h"
#include "./Common/FileSystem.h"

#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////
// #######################################################################

// �ε��Ҷ� �ʿ��Ѱ� ������ ���⿡ �߰��Ϲ� �׸� , ���� ��Ȱ�� Ŭ������ �������
// ResourceThreadInterface::Data��   ResourceRawData �� �θ� Ŭ������ ����� �������� ó���ϸ� ������
// ĳ������ ����������  �ڵ尡 ���� �� ����������  ��� ������ ���ҽ� �Ŵ��������� ����
// ���ð� ���� �ʱ⿡ �̷��� ó���� , �̺κ��� �����ϸ� �Ϻ��� ���ҽ� �޴����� �������� ����� �ȴ�

// �ý��Ŀ�

#include "../InternalCommonLib/Define/TextureAddData.h"

// �ִϸ��̼ǿ�
#include "./DxMeshs/SkinMesh/DxAnimationAddData.h"

// ��Ų�޽���
#include "./DxMeshs/SkinMesh/DxSkinMeshAddData.h"

// SimpleMesh��
#include "./DxMeshs/DxSimpleMeshAddData.h"

// SingleEffect��
#include "./DxEffect/Single/DxEffSinglePropGAddData.h"

//////////////////////////////////////////////////////////////////////////
// �⺻ �ƹ��͵� ���� 
class  EMPTY_ADD_DATA{};

// #######################################################################
// ���� �͵��� ResourceThreadInterface::Data�� �߰� ���ָ� �ڿ������� ���� �ְ� �ȴ�



typedef std::tr1::shared_ptr<void> SP_RAW_DATA;
//////////////////////////////////////////////////////////////////////////

///@struct    ���Ͻý������κ��� �ε��� �����͸� �����ϴ� ������ (���� �޸�����)
template<class ADD_DATA>
struct ResourceRawData : private boost::noncopyable
{

	/// @brief �Ҹ��ڷν� �Ҹ��Ҷ� ������ m_spRawData�� �����Ͱ� �ö�� �ִٸ� �����ش�
	~ResourceRawData() 
	{
		if( IsCompleted() )
            CFileSystem::getInstance().releaseMemory( GetPathFileName() );
	}

	/// @brief �����ڷν� ó������ �� �����͸� ������ ���������� ������ �޴�������
	///        m_spRawData�� �����͸� ä���ش�
	ResourceRawData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData = ADD_DATA() )
	{
		MIN_ASSERT(!pathFileName.empty());
		m_pathFileName = pathFileName;
		m_bMustDirectory = bMustDirectory;
		sc::string::getFullFileName(pathFileName,m_fileName);
		m_addData = addData;
		m_rawDataSize = 0;	
		m_bFailed = false;
	}

    /// @brief �����ڷν� ó������ �� �����͸� ������ ���������� ������ �޴�������
    ///        m_spRawData�� �����͸� ä���ش�
    ResourceRawData(const TSTRING& pathFileName,const TSTRING& strSettingName,bool bMustDirectory,const ADD_DATA& addData = ADD_DATA() )
    {
        MIN_ASSERT(!pathFileName.empty());
        m_pathFileName = pathFileName;
        m_bMustDirectory = bMustDirectory;
        m_fileName=strSettingName;
        m_addData = addData;
        m_rawDataSize = 0;	
        m_bFailed = false;
    }


	/// @brief �Ϸ� �Ǿ����� ���� üũ 
	bool IsCompleted() const { return m_spRawData != NULL; }
	bool IsFailed() const { return m_bFailed; }

	const TSTRING& GetFileName() const { return m_fileName; }
	const TSTRING& GetPathFileName() { return m_pathFileName; }


	/// �ε�Ǿ��� ���� �̸�
	TSTRING m_fileName;
	/// Ǯ�н����� �̸�
	TSTRING m_pathFileName;

	///   ���� �ε� ���� ����
	bool  m_bFailed;
	///   �ݵ�� ���丮�κ��� ������
	bool  m_bMustDirectory;
	/// ���� �ý������κ��� �о��� ������ �޸� ���
	/// ���ҽ� �޴����� PreprocessRawDataWidthBackgroundThread���� ��ó���ĵ� �����Ͱ� �ɼ��� �ִ�
	SP_RAW_DATA m_spRawData;

	/// m_spRawData �޸� ũ��
	UINT           m_rawDataSize;
	/// m_spRawData�� �ε�Ǿ� ���� �ʿ��� �����ͷ� �����ϱ� ���� �߰���������
	ADD_DATA	   m_addData;
};

//////////////////////////////////////////////////////////////////////////
/// ������ �ٸ� ���������� , �߰� �ɰ� ������ ���⿡ �߰� ��Ű�� �ȴ�
/// ����Ǵ°� �ؽ��� , �޽� , ���̷���(��) ���
/// �׸��� �⺻���� ������ �׻� ����Ʈ�����ͷ� �̷������
typedef std::tr1::shared_ptr< ResourceRawData<EMPTY_ADD_DATA> > SP_RESOURCE_RAW_DATA_EMPTY; 
typedef	std::tr1::shared_ptr< ResourceRawData<TEXTURE_ADD_DATA> > SP_RESOURCE_RAW_DATA_TEXTURE;
typedef	std::tr1::shared_ptr< ResourceRawData<TSTRING> > SP_RESOURCE_RAW_DATA_STRING;
typedef	std::tr1::shared_ptr< ResourceRawData<ANIMATION_ADD_DATA> > SP_RESOURCE_RAW_DATA_ANIMATION;
typedef	std::tr1::shared_ptr< ResourceRawData<SKINMESH_ADD_DATA> > SP_RESOURCE_RAW_DATA_SKINMESH;
typedef	std::tr1::shared_ptr< ResourceRawData<SIMPLEMESH_ADD_DATA> > SP_RESOURCE_RAW_DATA_SIMPLEMESH;
typedef	std::tr1::shared_ptr< ResourceRawData<EFF_PROPGROUP_ADD_DATA> > SP_RESOURCE_RAW_DATA_EFF_PROPGROUP;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// @class ������ �޴����� �ϰ������� ���ҽ� �޴����� ������ ���� ���ѿ�
// TAG_ClassTextureManager_5b
class ResourceThreadInterface
{
public:

	//////////////////////////////////////////////////////////////////////////
	/// ���ҽ� �޴��� ������ �ð� �μ������� �־����� ADD_DATA ������ �þ�ٸ� �ؿ� �߰�������Ѵ�
	/// ���� �׷������� �ʿ������ SP_RESOURCE_RAW_DATA_EMPTY �̰� ����Ѵ�
	/// @see boost::variant
	typedef	boost::variant
		<
		SP_RESOURCE_RAW_DATA_EMPTY,
		SP_RESOURCE_RAW_DATA_TEXTURE,
		SP_RESOURCE_RAW_DATA_STRING,
		SP_RESOURCE_RAW_DATA_ANIMATION,
		SP_RESOURCE_RAW_DATA_SKINMESH,
		SP_RESOURCE_RAW_DATA_SIMPLEMESH,
		SP_RESOURCE_RAW_DATA_EFF_PROPGROUP
		> Data;
	//////////////////////////////////////////////////////////////////////////


	virtual ~ResourceThreadInterface() {}

	/// ������ �޴����� ť�κ��� �����͸� ������ ����
	virtual Data GetReadyFrontData() = 0;
	/// ������� üũ
	virtual bool IsEmpty() = 0;

	///@brief �����忡�� �����͸� ��� �ε� �Ѵ��� �ʿ��� ����?ó���� ������ �� ���ش�
	///       �������� �ʴ� �ڿ��̶�� ���ξ����忡���� �������� ���⼭ �ٿ��ִ°��� ���� 
	///@param spData �ε�� �ο�޸� �޸�,  ��ó���� �ȴٸ� ������ ������ �ٲ�� �ִ�
	///@param size   �ε�� �ο�޸� ũ��,    ��ó���� �ȴٸ� ������ ũ�Ⱑ �ٲ�� �ִ�
	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData) = 0;
	/// ������ �ε��� ������ ��ü�� ����
	virtual void DeleteLoadingData(const TSTRING& fileName) = 0;


};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// @class ResourceRawData�� �����ϴ� Ŭ���� , �̸��״�� ���Ͻý������ν� �������� ����
///       Ȥ�� ������ ����Ÿ�� ť�� ��Ƴ��� ��Ȱ�� �Ѵ�

/// @remark �����忡�� ť�� ������ �о�⿡ lock�� �ɷ��ִ�
template<class ADD_DATA>
class ResourceQueue : public sc::DefaultLock , private boost::noncopyable
{
public:
	ResourceQueue() 
	{
		m_pQueueIndexSeq  = &m_multiindexDataQueue.get<0>(); 	
		m_pQueueIndexHash = &m_multiindexDataQueue.get<1>(); 
	}

	typedef ResourceRawData<ADD_DATA> QUEUE_DATA;
	typedef std::tr1::shared_ptr< QUEUE_DATA > SP_QUEUE_DATA;
	typedef std::vector< SP_QUEUE_DATA > SP_QUEUE_DATA_VECTOR;

	/// @brief  ���� �����͸� ť���ִ� ��Ȱ ť�� ������ ������ �޴������� ����Ÿ�� �д´�
	/// @return ���� ������ �ʴ´�
	/// @remark ���ξ����忡�� ���
	SP_QUEUE_DATA PushData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bFront = false);

	//////////////////////////////////////////////////////////////////////////
	/// @brief  �����忡�� �б⿡ ������ ���� ť���� ���ŵǾ�� �Ѵ� (��ȿ ���� ���� �����̹Ƿ�)
	/// @see    VarResourceVistor::operator() ���� 
	void DeleteData(const TSTRING& fileName);

	/// @brief  ť�� �����ϴ��� Ȯ��
	/// @remark ���ξ����忡�� ���
	bool Find(const TSTRING& fileName);

	/// @brief  �ε��� ���� �����͸� Ȯ���� ť���� ������ ��Ȱ
	/// @param  pCompletedDataVec �� �ε��� ���� �����Ͱ� ���
	/// @remark ���ξ����忡�� ���
	void UpdateAndPopCompletedVector( SP_QUEUE_DATA_VECTOR* pCompletedDataVec, DWORD nMaxCnt );

    /// @brief  UpdateAndPopCompletedVector �Լ��� �����Դµ� �� ó���� �� �ߴ�
    /// @param  pCompletedDataVec �� ó������ ���� �����Ͱ� ���
    /// @remark ���ξ����忡�� ���
    void Rollback( SP_QUEUE_DATA_VECTOR* pCompletedDataVec );

	/// @brief  ť�� ����� ��� ���ҽ� ����
	/// @remark ���ξ����忡�� ���
	void Clear();
	//////////////////////////////////////////////////////////////////////////

	/// @brief  �����忡�� �ε��� �����͸� ť�κ��� �����͸� �����ذ��� �Լ�
	/// @remark ������ �޴����� �����忡�� ���
	ResourceThreadInterface::Data GetReadyFrontData();

	/// @briefť�� ��� �ִ��� üũ
	bool IsEmpty();

private:


	/// @breif ���ø� ���ڷ� ���� , ���� Ÿ���� ���⼭ �������ְ� GetReadyFrontData������
	///        boost::variant Ÿ������ ��ȯ�� ����
	/// @see   GetReadyFrontData
	SP_QUEUE_DATA _GetReadyFrontData();


private:

	/// �� �ؽ��� �� unsigned int�� ���� �����ָ� ������ �߱⿡ ���� �ؽ��� ����� �־���� -_-;  
	struct string_hash : public boost::hash<TSTRING>
	{ 
		unsigned int operator()(const TSTRING& str ) const
		{
			return (unsigned int)boost::hash<TSTRING>::operator()(str);
		}
	};

	/// ��Ƽ�ε����� ������ �˻��� ���� ���ÿ� ���ϱ�
	typedef boost::multi_index_container<
		SP_QUEUE_DATA,
		boost::multi_index::indexed_by<
		boost::multi_index::sequenced<>,
		boost::multi_index::hashed_unique< 
		BOOST_MULTI_INDEX_CONST_MEM_FUN(QUEUE_DATA,const TSTRING&,GetFileName),string_hash >
		>
	> MULTI_INDEX_QUEUE;


	//	BOOST_STATIC_ASSERT( sizeof(size_t) == sizeof(unsigned int) );

	typedef typename MULTI_INDEX_QUEUE::nth_index<0>::type  QUEUE_INDEX_SEQUENCE;
	typedef typename MULTI_INDEX_QUEUE::nth_index<1>::type  QUEUE_INDEX_MAP;


	/// ResourceRawData�� ����� �ִ� ť
	MULTI_INDEX_QUEUE     m_multiindexDataQueue;
	QUEUE_INDEX_SEQUENCE* m_pQueueIndexSeq;
	QUEUE_INDEX_MAP*      m_pQueueIndexHash;

};




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// @class  ResourceManager���� �ϰ������� Update �� Clear���ֱ� ���ѿ�
// TAG_ClassTextureManager_5a
class ResourceUpdatorInterface
{
public:
	virtual ~ResourceUpdatorInterface() {}

	virtual bool Update( DWORD dwPrevTick, DWORD nWorkTime ) = 0;
	virtual void Clear() = 0;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/// @class  ���ҽ� �޴����� �ؾ����ϵ��� ���س��� �� 
///         ��� ���ҽ� �޴����� �� Ŭ������ ��ӹ޾�ó���ϸ�
///         �ڵ����� ������ �ε��� �����ް� �ǰ� 
///         ĳ���� �ڵ����� ����ϰ� �ȴ�
///         ############################################################################################
///			�̸� ���� ���� �ٽ������� �����ؾ� �ϴ� �Լ����� �ִµ�
///			���ҽ����� ����ó��(Raw �޸𸮿��� ���� ��������  ����ó������ �޸� ��ȯ)�� 
///			��ӹ��� Ŭ�������� ProcessCompleteData ���� �����ν� �����ϰ� �ȴ�
///		    �� �Լ��� ���Ϸκ��� �ε��� ���� �����͸� ResourceQueue�κ��� �޾�
///         ��ӹ��� ���ҽ� �޴������� �ʿ��� �����ͷ� �������ִ� ��Ȱ�� �ϴ� �Լ��ν�
///         �̰͸� �����ϸ�  ������ �����͵� �� Ŭ������ �����ϰ� �ȴ�
///         #############################################################################################
///			ĳ�� ��å�� ���������� ���� �Ҽ� �ֱ⿡ ĳ�� ��å�� ��ӹ��� ���� Ŭ�������� 
///			CacheAlogorithm , CachedData , InitCacheData �Լ��� �����Ͽ� �����Ѵ� 
///			�⺻���� �������δ� �ð��� ����� ĳ�� ��å���ν� ResourceManagerCacheAlgorithm.h��
///			���ǵǾ� �ִ�.  �׿� �������� ����� �ְ����� �⺻ ���� �������δ� �ð��� ���������
///			Ŭ������ �����Ѵ�
// TAG_ClassTextureManager_4
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
class ResourceManager : public ResourceUpdatorInterface , 
	public ResourceThreadInterface,
	private boost::noncopyable
{

protected:
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// �Ϸ�� �����ʹ� 
	typedef std::tr1::shared_ptr< COMPLETED_DATA > SP_COMPLETED_DATA;
	//-------------------------------------------------------------------------------------------------------

	TSTRING GetFileNameFromPathFileName(const TSTRING& pathFileName);


	/// ������ �����͸� ĳ���ؼ� ������ �����ϴ¿�
	template<class T,class CACHE_DATA>
	struct MANAGED_COMPLETED_DATA_
	{
		MANAGED_COMPLETED_DATA_(const T& spData = T(),
			const CACHE_DATA& cache = CACHE_DATA()) { m_spData = spData; m_cacheData = cache; }

		bool IsUse() { return !m_spData.unique(); }

		CACHE_DATA m_cacheData;
		T m_spData;
	};

	typedef MANAGED_COMPLETED_DATA_<SP_COMPLETED_DATA,CACHE_DATA> MANAGED_COMPLETED_DATA;
	

	/// @class  ������ �����͸� ����?�� ����ϵ��� ����ڿ��� �����ϴ� ĸ��Ŭ����
	///         ������ ����ϰ� �ȴ�
    class Resource
	{
		friend class ResourceManager;

	protected:
		Resource() { m_pMgr = NULL; }
		Resource(const TSTRING& fileName,ResourceManager* pMgr,SP_COMPLETED_DATA spData = SP_COMPLETED_DATA()) 
		{
			MIN_ASSERT(!fileName.empty());
			MIN_ASSERT(pMgr);
			m_fileName = fileName;
			m_pMgr = pMgr;
			m_spCompletedData = spData;
		}

	public:
		/// ����  ���̻� ���� ���� �ʴ´�
		void Clear() { m_fileName.clear(); m_spCompletedData.reset(); m_pMgr = NULL; }
		/// ��ȿ�� üũ	
		virtual bool IsValid() const { return m_spCompletedData.get() != NULL; }

		/// �̸� ����
		const TSTRING& GetFileName() const { return m_fileName; }

		/// �ε������� üũ
		bool IsLoading() const { return m_pMgr ? m_pMgr->IsLoading(m_fileName) : false; }

		/// ������ �ε��� ��� Update�� ���� ������ �����͸� üũ�� ���� �޵����Ѵ�
		virtual bool Update()
		{
			if( m_pMgr )
			{
				if( !m_spCompletedData.get() ) 
					m_spCompletedData = m_pMgr->GetCompletedDataForUpdate(m_fileName);
			}

			return IsValid(); 
		}

		BOOL IsCompletedData() const
		{
			return (m_pMgr && m_spCompletedData.get()) ? TRUE : FALSE;
		}

	protected:
		/// ���� �̸�
		TSTRING      m_fileName;
		/// �Ϸ�� ������ 
		SP_COMPLETED_DATA m_spCompletedData;
		/// �ش� ������ �޴��� ������
		ResourceManager*  m_pMgr;
	};

	typedef typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA SP_QUEUE_DATA; 
	typedef typename ResourceQueue<ADD_DATA>::QUEUE_DATA QUEUE_DATA;

	typedef typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA_VECTOR SP_QUEUE_DATA_VECTOR; 

	typedef std::pair<TSTRING,SP_COMPLETED_DATA> NAME_N_SP_COMPLETED_DATA; 
	typedef std::vector<NAME_N_SP_COMPLETED_DATA> NAME_N_SP_COMPLETED_DATA_VECTOR;

	typedef std::map<TSTRING,MANAGED_COMPLETED_DATA> COMPLTED_DATA_MAP;


protected:
	// TAG_CreateResourceManager_4
	ResourceManager(void) 
	{
		// �ڵ����� ������ �޴��� �� ������Ʈ �޴����� ��ϵȴ�
		ResourceThreadManager::GetInstance().Register(*this); 
		ResourceUpdatorManager::GetInstance().Register(*this);
	}

	virtual ~ResourceManager(void) 
	{
		// �Ҹ��Ҷ��� �ݵ�� SP_COMPLETED_DATA�� �����ɶ� �ʿ��� 
		// �ڿ��޴����� �����ؾ� �ϹǷ� �ش� �ڿ��޴����� �����Ǳ����� Clear�Լ��� ȣ������
	}

public:


	/// fileName�� ���� �ε��ǰ� �ִ°� üũ
    bool IsLoading(const TSTRING& fileName) { return m_loadingQueue.Find( GetFileNameFromPathFileName(fileName) ); }
	/// fileName���ε� ������ �����Ͱ� �����ϴ��� üũ
	bool IsExistCompletedData(const TSTRING& fileName) { return GetCompletedData(GetFileNameFromPathFileName(fileName)) != NULL;  }

	/// ������ �ش� ���ҽ��� ��������, �����ϴ� ��ü�� �����ϴ��� , ���̻� �޴��������� �ν��Ҽ� ����
	void DeleteData(const TSTRING& fileName);
	/// @see DeleteData 
	void DeleteData(const Resource& res) { DeleteData(res.GetFileName());  }
	/// ������ ���� �ϰ� ��� ���������� , �����ϴ� ��ü�� �����ϴ��� , ���̻� �޴��������� �ν��Ҽ� ����
	virtual void Clear() { m_loadingQueue.Clear(); m_completedDataMap.clear(); ClearBackupCache();}

	/// @brief   ĳ������ �����
	/// @return  true : ���̻� ĳ���� �������� ���� , false : �����ϴ� ��ü�� �����ϱ⿡ ����� ����
	bool DeleteCacheData(const TSTRING& fileName);

	/// @brief   ����� �ִ� ���ҽ��� ĳ������ ���� �����
	/// @see     DeleteCacheData
	virtual void DeleteCacheDataAll();

	/// @brief  ��� ĳ���� �����ϱ�
	/// @return �����ߴ��� ���� , ��ϵǾ� ���� �ʰų� �ε����̸� �����ϰ� �ȴ�
	///         ���� �ε����̶�� �ݵ�� �Ϸ�ɶ����� ��ٸ��簡 �ؾ���
	/// @remark ������� �����ϰ� �����ν� DeleteCacheDataAll �Լ��� �������� �ʰ� ���� �ȴ�
	bool BackUpCache(const TSTRING& fileName);

	/// @see BackUpCache(const Resource& res) helper
	void BackUpCache(const Resource& res) { BackUpCache(res.GetFileName()); }

	/// ����� ĳ�� ��� ����������
	void ClearBackupCache() { m_backupCacheCompletedDataMap.clear(); }
protected:

	/// @brief �����͸� �ε���
	/// @param pathFileName �ε��� �����̸� 
	/// @param bMustDirectory  ���������� �ƴ϶� �ݵ�� ���丮�� ��� �ִ� ���� ��� ����
	/// @param addData�� ProcessCompleteData ó������ �ɼ� ��Ȱ�ξ��δ� , ��ӹ޴� ���ҽ� �޴������� �ٸ���
	/// @param bBackgroundLoading �񵿱�ó���� �Ұ����� ����
	/// @remark  protected�� ������ ������  ��ӹ޴� Ŭ�������� ������ ���� ������ �ϱ� ����
	Resource LoadData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bBackgroundLoading = false);
	Resource LoadData(const TSTRING& pathFileName,const TSTRING& strSettingName,bool bMustDirectory,const ADD_DATA& addData,bool bBackgroundLoading = false);

	/// ������ �ε����� ����� Resource�� ������ ����Ÿ�� ���⶧���� , Update�� �Ź�
	/// ������ �����͸� ���캸�� �������� �ȴ� ���� ������ �����Ͱ� ���� �غ� �ȵ����� SP_COMPLETED_DATA�� ����������� 
	SP_COMPLETED_DATA GetCompletedDataForUpdate(const TSTRING& fileName);

	/// ������ ������ �����͸� �ܼ��� �����ؾ� �Ҷ�
	COMPLETED_DATA*   GetCompletedData(const TSTRING& fileName);


private:
	/// �н��� ������ �ҹ��� ��ȯ �� �н��� ������ �н��� ã�� ��Ȱ ,���ϸ� ã����
	void QueryPathFileName(TSTRING* pPathFileName,TSTRING* pFileName);
	/// �ʿ��ϴٸ� ��ӹ��� Ŭ�������� ����
	virtual TSTRING QueryPathFileNameHelper(const TSTRING& pathFileName,
		const TSTRING& path,const TSTRING& fileName) { return pathFileName; }


	/// �ܼ��� �ε�ť�� ����ϰ� �� ����Ʈ �����͸� ����
	SP_QUEUE_DATA LoadRawData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bFront = false);
	/// �ܼ��� �ε�ť�� �޾Ƴ��Ҵ��� üũ 
	void IsExistLoadingRawData(const TSTRING& fileName);


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// �ο쵥���ͷκ��� ������ �����ͷ� ��ȯ�ϴ� �ϰ� ĳ���� �����ϴ� ��Ȱ�� �Ѵ�
    /// m_loadingQueue ���� �޸� �����͸� �����ͼ� ó�� �Ѵ�
	virtual bool Update( DWORD dwPrevTick, DWORD nWorkTime );

    /// �ο쵥���ͷκ��� ������ �����ͷ� ��ȯ�ϴ� �ϰ� ĳ���� �����ϴ� ��Ȱ�� �Ѵ�
    /// ���� dataVec �� �־ ȣ�����ָ� �Ϸᰡ �ȴ�
    virtual void ProcessCompleteDataAndCache( SP_QUEUE_DATA_VECTOR& dataVec, DWORD dwPrevTick, DWORD nWorkTime );

	/// ĳ���˰��� ��ӹ޾Ƽ� �����ȴ�
	virtual void CacheAlogorithm() = 0;

	/// ĳ���� ��ϵɶ� ó��
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// �����Ҷ� ����
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	
	/// Load�Ǿ��ų� ���ŵ� �����ʹ�  ĳ�� �ð��� ������Ʈ���ִµ� , �� ��Ȱ
	Resource CacheAndReturnData(const TSTRING& fileName,MANAGED_COMPLETED_DATA& resData);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// ���ҽ� �������� �������̽��� ����

	/// ���ҽ� �����忡�� ť�� �����͸� �����ϱ� ����
	virtual Data GetReadyFrontData() { return m_loadingQueue.GetReadyFrontData(); }

	/// �ε�ť�� ������� Ȯ��
	virtual bool IsEmpty() { return m_loadingQueue.IsEmpty(); }

	/// ��׶��� �����忡�� �ο� ����Ÿ�� �ε��ѵڿ� �� �����忡�� ��ó�� ������ ���ټ� ������ ���ش�
	/// �⺻�����δ� �ƹ� �۾��� �������� �޴������� ���ټ� �ִ� �۾��� �����Ҽ� ����
	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData) {}
	/// @see ResourceQueue::DeleteData
	virtual void DeleteLoadingData(const TSTRING& fileName);

	//////////////////////////////////////////////////////////////////////////

	/// @brief  m_loadingQueue�κ��� �Ϸ�� �ο� ������(�޸𸮵��)�� �޾� �ش� ���ҽ��޴������� ����
	///		    �ʿ��� ������ �����ͷ� ����� ��Ȱ
	/// @param  dataVec m_loadingQueue���� �Ϸ�� �����͹���
	/// @param  pCompletedDataVec ����ó���� �Ϸ�� ������ ����
	/// @remark ��� ���ҽ� �޴����� �� �Լ��� �ݵ�� �����ؾ� �ϰ� pCompletedDataVec�� �Ϸ�� �����͸�
	///         �����ؾ��ϸ� �Ѵ�
	virtual void ProcessCompleteData(
		typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
        DWORD nWorkTime ) = 0;

	virtual void ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec ) {}

public:
	void PrintData(const TSTRING& logFile);
protected:
	/// �ο쵥���͸� �ε� �ϴµ� ���̴� ť
	/// @remark �� ť�� ��Ƽ������� ����
	ResourceQueue<ADD_DATA> m_loadingQueue;

	/// @brief ProcessCompleteData���� ���� �Ϸ�� �����ʹ� �� �ʿ� ��ϵ�
	/// @see   ProcessCompleteData
	COMPLTED_DATA_MAP m_completedDataMap;

	/// @brief ��������ν� ClearBackupCache�� ȣ��Ǳ������� ĳ���� �����ϴ°� �����ϱ�����
	COMPLTED_DATA_MAP m_backupCacheCompletedDataMap;
};


/// @class ���ҽ� �޴������� ��Ƽ������ �ε��� �������ִ� Ŭ����
class ResourceThreadManager
{
	//ResourceThreadManager() { m_bStop = false; }
    //~ResourceThreadManager() {}
    ResourceThreadManager();
    ~ResourceThreadManager();

public:
	/// ���ӵ� CPU �Ҹ� ���̱� ���ѿ�
	//static void SleepThread(DWORD msTick=1) { ::Sleep(msTick); }//boost::this_thread::sleep(boost::posix_time::millisec(msTick)); }
    static HANDLE m_hWaitEvent;
    static void SleepThread(DWORD msTick=16);

public:
	/// �̱���
	static ResourceThreadManager& GetInstance();

	/// @brief  ���ҽ� �޴����� ��� 
	/// @remark ���ҽ� �޴����� ResourceThreadInterface�� �����ϰ� �ִ�
	void Register(ResourceThreadInterface& resourceMgr );

	/// ������ ����
	void Start();

	/// @brief   ������ ����
	/// @remark  �����尡 �����ϰ� ������ ���⶧���� ���
	void Stop();
private:

	friend class WorkerThread;
	/// ������ ���� �÷��װ� ���� �ִ°�?
	bool IsStopped() { return m_bStop; }

	/// ��ϵ� ���ҽ� �޴����� �ε�ť(m_loadingQueue)���� �ε��� ���ϴ� �����͸� ������
	/// �����͸� �־��ִ� ��Ȱ 
	void Run();
private:

	/// ���� �۾��ϴ� ������ Ŭ����
	class WorkerThread
	{
	public:
		WorkerThread(ResourceThreadManager* pThreadManager) { m_pThreadManager = pThreadManager; }
		void operator()();

	private:
		ResourceThreadManager* m_pThreadManager;
	};

private:
	/// �����尡 ���� �� ��� �� �÷��װ� ������ , �׸��� �۾������忡���� �� �÷��׸� ���� �����ϰԵ�
	bool m_bStop;

	// ������� �̿��Ϸ� �ߴ��� wait �Ҷ� �ɸ���...  
	// ���ҽ������� ������ ������� ������ ������ �ְ� ���� wait�� �ϸ� ������ 
	// �ϳ��� ������ �̹Ƿ� ����ϳ�
	//	boost::condition_variable m_condition;

	/// ������ ��ü
	boost::thread m_thread;

	/// ��ϵ� ���ҽ� �޴�����
	std:: vector< ResourceThreadInterface* > m_resourceMgrList;
};


/// ���ǿ� Ŭ����  ���ҽ� �޴������� �ϸ��� Update ȣ���ϴ°��� ��������� �ְ� , �����ϱ⿡
/// �ϰ������� �� Ŭ������ ��ϵǾ� ���Ǿ�����
class ResourceUpdatorManager
{
    ResourceUpdatorManager()
        : m_nUpdateIndex( 0 )
    {
    }
    ~ResourceUpdatorManager()
    {
    }

public:
	static ResourceUpdatorManager& GetInstance();

	/// ���ҽ� �޴��� ���
	void Register(ResourceUpdatorInterface& updator);
	
	/// ���ҽ��޴��� �ϰ� ������Ʈ
	void Update();

    /// ���ҽ� �޴��� �ϰ� ������ ����
	void Clear();

private:
	/// ���ҽ� �޴��� ������ �����̳�
	std::vector<ResourceUpdatorInterface*> m_updatorVec;

    /// Update ������ ����
    size_t m_nUpdateIndex;
};





//////////////////////////////////////////////////////////////////////////
#include "./ResourceManager.inl"

//////////////////////////////////////////////////////////////////////////


#endif
