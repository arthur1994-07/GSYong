
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

/**

@file ResourceManager.h
@brief  리소스를 파일로부터 로드를 해 리소스를 관리하는 책임을 지는 관리 클래스
@author 조해진
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

// 로딩할때 필요한게 있으면 여기에 추가하믄 그만 , 완전 재활용 클래스로 만드려면
// ResourceThreadInterface::Data를   ResourceRawData 의 부모 클래스를 만들어 동적으로 처리하면 되지만
// 캐스팅이 귀찬아지고  코드가 조금 더 복잡해진다  사실 각각의 리소스 매니저용으로 많이
// 나올거 같지 않기에 이렇게 처리함 , 이부분을 제외하면 완벽히 리소스 메니저는 독립적인 모듈이 된다

// 택스쳐용

#include "../InternalCommonLib/Define/TextureAddData.h"

// 애니메이션용
#include "./DxMeshs/SkinMesh/DxAnimationAddData.h"

// 스킨메쉬용
#include "./DxMeshs/SkinMesh/DxSkinMeshAddData.h"

// SimpleMesh용
#include "./DxMeshs/DxSimpleMeshAddData.h"

// SingleEffect용
#include "./DxEffect/Single/DxEffSinglePropGAddData.h"

//////////////////////////////////////////////////////////////////////////
// 기본 아무것도 없음 
class  EMPTY_ADD_DATA{};

// #######################################################################
// 위에 것들은 ResourceThreadInterface::Data에 추가 해주면 자연스럽게 쓸수 있게 된다



typedef std::tr1::shared_ptr<void> SP_RAW_DATA;
//////////////////////////////////////////////////////////////////////////

///@struct    파일시스템으로부터 로딩해 데이터를 저장하는 데이터 (순수 메모리정보)
template<class ADD_DATA>
struct ResourceRawData : private boost::noncopyable
{

	/// @brief 소멸자로써 소멸할때 실제로 m_spRawData에 데이터가 올라와 있다면 돌려준다
	~ResourceRawData() 
	{
		if( IsCompleted() )
            CFileSystem::getInstance().releaseMemory( GetPathFileName() );
	}

	/// @brief 생성자로써 처음에는 빈 데이터만 가지고 시작하지만 쓰레드 메니저에서
	///        m_spRawData에 데이터를 채워준다
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

    /// @brief 생성자로써 처음에는 빈 데이터만 가지고 시작하지만 쓰레드 메니저에서
    ///        m_spRawData에 데이터를 채워준다
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


	/// @brief 완료 되었는지 유무 체크 
	bool IsCompleted() const { return m_spRawData != NULL; }
	bool IsFailed() const { return m_bFailed; }

	const TSTRING& GetFileName() const { return m_fileName; }
	const TSTRING& GetPathFileName() { return m_pathFileName; }


	/// 로드되어질 파일 이름
	TSTRING m_fileName;
	/// 풀패스파일 이름
	TSTRING m_pathFileName;

	///   파일 로딩 실패 여부
	bool  m_bFailed;
	///   반드시 디렉토리로부터 가져옴
	bool  m_bMustDirectory;
	/// 파일 시스템으로부터 읽어질 데이터 메모리 덩어리
	/// 리소스 메니저의 PreprocessRawDataWidthBackgroundThread에서 전처리후된 데이터가 될수도 있다
	SP_RAW_DATA m_spRawData;

	/// m_spRawData 메모리 크기
	UINT           m_rawDataSize;
	/// m_spRawData가 로드되어 실제 필요한 데이터로 가공하기 위한 추가적인정보
	ADD_DATA	   m_addData;
};

//////////////////////////////////////////////////////////////////////////
/// 앞으로 다른 데이터형이 , 추가 될게 있으면 여기에 추가 시키면 된다
/// 예상되는거 텍스쳐 , 메쉬 , 스켈레톤(뼈) 등등
/// 그리고 기본적인 관리는 항상 스마트포인터로 이루어진다
typedef std::tr1::shared_ptr< ResourceRawData<EMPTY_ADD_DATA> > SP_RESOURCE_RAW_DATA_EMPTY; 
typedef	std::tr1::shared_ptr< ResourceRawData<TEXTURE_ADD_DATA> > SP_RESOURCE_RAW_DATA_TEXTURE;
typedef	std::tr1::shared_ptr< ResourceRawData<TSTRING> > SP_RESOURCE_RAW_DATA_STRING;
typedef	std::tr1::shared_ptr< ResourceRawData<ANIMATION_ADD_DATA> > SP_RESOURCE_RAW_DATA_ANIMATION;
typedef	std::tr1::shared_ptr< ResourceRawData<SKINMESH_ADD_DATA> > SP_RESOURCE_RAW_DATA_SKINMESH;
typedef	std::tr1::shared_ptr< ResourceRawData<SIMPLEMESH_ADD_DATA> > SP_RESOURCE_RAW_DATA_SIMPLEMESH;
typedef	std::tr1::shared_ptr< ResourceRawData<EFF_PROPGROUP_ADD_DATA> > SP_RESOURCE_RAW_DATA_EFF_PROPGROUP;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// @class 쓰레드 메니저가 일괄적으로 리소스 메니저를 관리해 쓰기 위한용
// TAG_ClassTextureManager_5b
class ResourceThreadInterface
{
public:

	//////////////////////////////////////////////////////////////////////////
	/// 리소스 메니저 종류가 늘고 부수적으로 주어지는 ADD_DATA 성격이 늘어난다면 밑에 추가해줘야한다
	/// 굳이 그런정보가 필요없으면 SP_RESOURCE_RAW_DATA_EMPTY 이걸 사용한다
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

	/// 쓰레드 메니저에 큐로부터 데이터를 빼낼때 쓰임
	virtual Data GetReadyFrontData() = 0;
	/// 비었는지 체크
	virtual bool IsEmpty() = 0;

	///@brief 쓰레드에서 데이터를 모두 로드 한다음 필요한 가공?처리가 있으면 더 해준다
	///       공유되지 않는 자원이라면 메인쓰레드에서의 할일일을 여기서 줄여주는것이 좋다 
	///@param spData 로드된 로우메모리 메모리,  전처리가 된다면 데이터 내용이 바뀔수 있다
	///@param size   로드된 로우메모리 크기,    전처리가 된다면 데이터 크기가 바뀔수 있다
	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData) = 0;
	/// 쓰레드 로딩에 실패한 객체는 제거
	virtual void DeleteLoadingData(const TSTRING& fileName) = 0;


};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// @class ResourceRawData를 관리하는 클래스 , 이름그대로 파일시스템으로써 읽혀지기 위해
///       혹은 읽혀진 데이타를 큐에 담아놓는 역활을 한다

/// @remark 쓰레드에서 큐의 내용을 읽어가기에 lock이 걸려있다
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

	/// @brief  읽을 데이터를 큐에넣는 역활 큐에 넣으면 쓰레드 메니저에서 데이타를 읽는다
	/// @return 현재 쓰이지 않는다
	/// @remark 메인쓰레드에서 사용
	SP_QUEUE_DATA PushData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bFront = false);

	//////////////////////////////////////////////////////////////////////////
	/// @brief  쓰레드에서 읽기에 실패한 넘은 큐에서 제거되어야 한다 (유효 하지 않은 파일이므로)
	/// @see    VarResourceVistor::operator() 참조 
	void DeleteData(const TSTRING& fileName);

	/// @brief  큐에 존재하는지 확인
	/// @remark 메인쓰레드에서 사용
	bool Find(const TSTRING& fileName);

	/// @brief  로딩이 끝난 데이터를 확인해 큐에서 빼내는 역활
	/// @param  pCompletedDataVec 에 로딩이 끝난 데이터가 담김
	/// @remark 메인쓰레드에서 사용
	void UpdateAndPopCompletedVector( SP_QUEUE_DATA_VECTOR* pCompletedDataVec, DWORD nMaxCnt );

    /// @brief  UpdateAndPopCompletedVector 함수로 꺼내왔는데 다 처리를 못 했다
    /// @param  pCompletedDataVec 에 처리하지 못한 데이터가 담김
    /// @remark 메인쓰레드에서 사용
    void Rollback( SP_QUEUE_DATA_VECTOR* pCompletedDataVec );

	/// @brief  큐에 담겨진 모든 리소스 삭제
	/// @remark 메인쓰레드에서 사용
	void Clear();
	//////////////////////////////////////////////////////////////////////////

	/// @brief  쓰레드에서 로드할 데이터를 큐로부터 데이터를 참조해가는 함수
	/// @remark 쓰레드 메니저의 쓰레드에서 사용
	ResourceThreadInterface::Data GetReadyFrontData();

	/// @brief큐가 비어 있는지 체크
	bool IsEmpty();

private:


	/// @breif 템플릿 인자로 인해 , 실제 타입은 여기서 리턴해주고 GetReadyFrontData에서는
	///        boost::variant 타입으로 변환해 리턴
	/// @see   GetReadyFrontData
	SP_QUEUE_DATA _GetReadyFrontData();


private:

	/// 이 해쉬를 써 unsigned int를 리턴 안해주면 워닝이 뜨기에 직접 해쉬를 만들어 넣어줬다 -_-;  
	struct string_hash : public boost::hash<TSTRING>
	{ 
		unsigned int operator()(const TSTRING& str ) const
		{
			return (unsigned int)boost::hash<TSTRING>::operator()(str);
		}
	};

	/// 멀티인덱스로 순차와 검색의 장점 동시에 취하기
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


	/// ResourceRawData가 담겨져 있는 큐
	MULTI_INDEX_QUEUE     m_multiindexDataQueue;
	QUEUE_INDEX_SEQUENCE* m_pQueueIndexSeq;
	QUEUE_INDEX_MAP*      m_pQueueIndexHash;

};




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// @class  ResourceManager들을 일괄적으로 Update 나 Clear해주기 위한용
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





/// @class  리소스 메니저가 해야할일들을 정해놓은 곳 
///         모든 리소스 메니저가 이 클래스를 상속받아처리하면
///         자동으로 쓰레드 로딩을 지원받게 되고 
///         캐쉬를 자동으로 사용하게 된다
///         ############################################################################################
///			이를 위해 이중 핵심적으로 구현해야 하는 함수들이 있는데
///			리소스마다 가공처리(Raw 메모리에서 실제 쓰기위한  가공처리가된 메모리 변환)는 
///			상속받은 클래스에서 ProcessCompleteData 구현 함으로써 수행하게 된다
///		    이 함수는 파일로부터 로딩이 끝난 데이터를 ResourceQueue로부터 받아
///         상속받은 리소스 메니저에서 필요한 데이터로 가공해주는 역활을 하는 함수로써
///         이것만 구현하면  가공된 데이터도 이 클래스에 관리하게 된다
///         #############################################################################################
///			캐쉬 정책은 여러가지가 존재 할수 있기에 캐쉬 정책은 상속받은 하위 클래스에서 
///			CacheAlogorithm , CachedData , InitCacheData 함수를 구현하여 결정한다 
///			기본적인 지원으로는 시간에 기반한 캐쉬 정책으로써 ResourceManagerCacheAlgorithm.h에
///			정의되어 있다.  그외 여러가지 방법이 있겠으나 기본 정의 구현으로는 시간을 기반으로한
///			클래스만 존재한다
// TAG_ClassTextureManager_4
template<class COMPLETED_DATA,class CACHE_DATA,class ADD_DATA>
class ResourceManager : public ResourceUpdatorInterface , 
	public ResourceThreadInterface,
	private boost::noncopyable
{

protected:
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 완료된 데이터는 
	typedef std::tr1::shared_ptr< COMPLETED_DATA > SP_COMPLETED_DATA;
	//-------------------------------------------------------------------------------------------------------

	TSTRING GetFileNameFromPathFileName(const TSTRING& pathFileName);


	/// 가공된 데이터를 캐쉬해서 저장해 관리하는용
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
	

	/// @class  가공된 데이터를 포장?해 사용하도록 사용자에게 제공하는 캡슐클래스
	///         복사해 사용하게 된다
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
		/// 제거  더이상 참조 하지 않는다
		void Clear() { m_fileName.clear(); m_spCompletedData.reset(); m_pMgr = NULL; }
		/// 유효성 체크	
		virtual bool IsValid() const { return m_spCompletedData.get() != NULL; }

		/// 이름 리턴
		const TSTRING& GetFileName() const { return m_fileName; }

		/// 로딩중인지 체크
		bool IsLoading() const { return m_pMgr ? m_pMgr->IsLoading(m_fileName) : false; }

		/// 쓰레드 로딩일 경우 Update를 통해 가공된 데이터를 체크해 리턴 받도록한다
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
		/// 파일 이름
		TSTRING      m_fileName;
		/// 완료된 데이터 
		SP_COMPLETED_DATA m_spCompletedData;
		/// 해당 관리자 메니저 포인터
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
		// 자동으로 쓰레드 메니저 및 업데이트 메니저에 등록된다
		ResourceThreadManager::GetInstance().Register(*this); 
		ResourceUpdatorManager::GetInstance().Register(*this);
	}

	virtual ~ResourceManager(void) 
	{
		// 소멸할때는 반드시 SP_COMPLETED_DATA가 해제될때 필요한 
		// 자원메니저가 존재해야 하므로 해당 자원메니저가 해제되기전에 Clear함수를 호출하자
	}

public:


	/// fileName이 현재 로딩되고 있는가 체크
    bool IsLoading(const TSTRING& fileName) { return m_loadingQueue.Find( GetFileNameFromPathFileName(fileName) ); }
	/// fileName으로된 가공된 데이터가 존재하는지 체크
	bool IsExistCompletedData(const TSTRING& fileName) { return GetCompletedData(GetFileNameFromPathFileName(fileName)) != NULL;  }

	/// 무조건 해당 리소스를 날려버림, 참조하는 객체가 존재하더라도 , 더이상 메니저에서는 인식할수 없다
	void DeleteData(const TSTRING& fileName);
	/// @see DeleteData 
	void DeleteData(const Resource& res) { DeleteData(res.GetFileName());  }
	/// 이유를 막론 하고 모두 날려버린다 , 참조하는 객체가 존재하더라도 , 더이상 메니저에서는 인식할수 없다
	virtual void Clear() { m_loadingQueue.Clear(); m_completedDataMap.clear(); ClearBackupCache();}

	/// @brief   캐쉬에서 지우기
	/// @return  true : 더이상 캐쉬에 존재하지 않음 , false : 참조하는 객체가 존재하기에 지울수 없음
	bool DeleteCacheData(const TSTRING& fileName);

	/// @brief   지울수 있는 리소스를 캐쉬에서 전부 지우기
	/// @see     DeleteCacheData
	virtual void DeleteCacheDataAll();

	/// @brief  백업 캐쉬에 보관하기
	/// @return 성공했는지 유무 , 등록되어 있지 않거나 로딩중이면 실패하게 된다
	///         따라서 로딩중이라면 반드시 완료될때까지 기다리든가 해야함
	/// @remark 백업으로 보관하게 됨으로써 DeleteCacheDataAll 함수때 지워지지 않고 남게 된다
	bool BackUpCache(const TSTRING& fileName);

	/// @see BackUpCache(const Resource& res) helper
	void BackUpCache(const Resource& res) { BackUpCache(res.GetFileName()); }

	/// 백업된 캐쉬 모두 날려버리기
	void ClearBackupCache() { m_backupCacheCompletedDataMap.clear(); }
protected:

	/// @brief 데이터를 로딩함
	/// @param pathFileName 로딩할 파일이름 
	/// @param bMustDirectory  압축파일이 아니라 반드시 디렉토리에 들어 있는 파일 사용 여부
	/// @param addData는 ProcessCompleteData 처리에서 옵션 역활로쓰인다 , 상속받는 리소스 메니저마다 다를것
	/// @param bBackgroundLoading 비동기처리로 할것인지 여부
	/// @remark  protected로 설정된 이유는  상속받는 클래스에서 가져다 쓰는 용으로 하기 위해
	Resource LoadData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bBackgroundLoading = false);
	Resource LoadData(const TSTRING& pathFileName,const TSTRING& strSettingName,bool bMustDirectory,const ADD_DATA& addData,bool bBackgroundLoading = false);

	/// 쓰레드 로딩으로 등록한 Resource는 실제로 데이타가 없기때문에 , Update때 매번
	/// 가공된 데이터를 살펴보고 가져오게 된다 물론 가공된 데이터가 아직 준비 안됐으면 SP_COMPLETED_DATA는 비어있을것임 
	SP_COMPLETED_DATA GetCompletedDataForUpdate(const TSTRING& fileName);

	/// 가공된 데이터 포인터를 단순히 참조해야 할때
	COMPLETED_DATA*   GetCompletedData(const TSTRING& fileName);


private:
	/// 패스를 받으면 소문자 변환 및 패스가 없으면 패스를 찾는 역활 ,파일명도 찾아줌
	void QueryPathFileName(TSTRING* pPathFileName,TSTRING* pFileName);
	/// 필요하다면 상속받은 클래스에서 구현
	virtual TSTRING QueryPathFileNameHelper(const TSTRING& pathFileName,
		const TSTRING& path,const TSTRING& fileName) { return pathFileName; }


	/// 단순히 로딩큐에 등록하고 그 스마트 포인터를 리턴
	SP_QUEUE_DATA LoadRawData(const TSTRING& pathFileName,bool bMustDirectory,const ADD_DATA& addData,bool bFront = false);
	/// 단순히 로딩큐에 달아놓았는지 체크 
	void IsExistLoadingRawData(const TSTRING& fileName);


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// 로우데이터로부터 가공된 데이터로 변환하는 일과 캐쉬를 갱신하는 역활을 한다
    /// m_loadingQueue 에서 메모리 데이터를 가져와서 처리 한다
	virtual bool Update( DWORD dwPrevTick, DWORD nWorkTime );

    /// 로우데이터로부터 가공된 데이터로 변환하는 일과 캐쉬를 갱신하는 역활을 한다
    /// 직접 dataVec 에 넣어서 호출해주면 완료가 된다
    virtual void ProcessCompleteDataAndCache( SP_QUEUE_DATA_VECTOR& dataVec, DWORD dwPrevTick, DWORD nWorkTime );

	/// 캐쉬알고리즘 상속받아서 구현된다
	virtual void CacheAlogorithm() = 0;

	/// 캐쉬가 등록될때 처리
	virtual void InitCacheData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 갱신할때 사용됨
	virtual void CachedData(MANAGED_COMPLETED_DATA* pManagedData) = 0;
	
	/// Load되었거나 갱신된 데이터는  캐쉬 시간을 업데이트해주는데 , 그 역활
	Resource CacheAndReturnData(const TSTRING& fileName,MANAGED_COMPLETED_DATA& resData);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// 리소스 쓰레드의 인터페이스를 구현

	/// 리소스 쓰레드에서 큐의 데이터를 참조하기 위해
	virtual Data GetReadyFrontData() { return m_loadingQueue.GetReadyFrontData(); }

	/// 로딩큐가 비었는지 확인
	virtual bool IsEmpty() { return m_loadingQueue.IsEmpty(); }

	/// 백그라운드 쓰레드에서 로우 데이타를 로딩한뒤에 그 쓰레드에서 전처리 과정을 해줄수 있으면 해준다
	/// 기본적으로는 아무 작업을 안하지만 메니저마다 해줄수 있는 작업이 존재할수 있음
	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData) {}
	/// @see ResourceQueue::DeleteData
	virtual void DeleteLoadingData(const TSTRING& fileName);

	//////////////////////////////////////////////////////////////////////////

	/// @brief  m_loadingQueue로부터 완료된 로우 데이터(메모리덩어리)를 받아 해당 리소스메니저에서 실제
	///		    필요한 가공된 데이터로 만드는 역활
	/// @param  dataVec m_loadingQueue에서 완료된 데이터백터
	/// @param  pCompletedDataVec 가공처리가 완료된 데이터 리턴
	/// @remark 모든 리소스 메니저는 이 함수를 반드시 구현해야 하고 pCompletedDataVec에 완료된 데이터를
	///         리턴해야하만 한다
	virtual void ProcessCompleteData(
		typename ResourceQueue<ADD_DATA>::SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
        DWORD nWorkTime ) = 0;

	virtual void ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec ) {}

public:
	void PrintData(const TSTRING& logFile);
protected:
	/// 로우데이터를 로딩 하는데 쓰이는 큐
	/// @remark 이 큐는 멀티쓰레드로 동작
	ResourceQueue<ADD_DATA> m_loadingQueue;

	/// @brief ProcessCompleteData에서 가공 완료된 데이터는 이 맵에 등록됨
	/// @see   ProcessCompleteData
	COMPLTED_DATA_MAP m_completedDataMap;

	/// @brief 백업용으로써 ClearBackupCache가 호출되기전까지 캐쉬에 존재하는걸 보장하기위해
	COMPLTED_DATA_MAP m_backupCacheCompletedDataMap;
};


/// @class 리소스 메니저들의 멀티쓰레드 로딩을 지원해주는 클래스
class ResourceThreadManager
{
	//ResourceThreadManager() { m_bStop = false; }
    //~ResourceThreadManager() {}
    ResourceThreadManager();
    ~ResourceThreadManager();

public:
	/// 지속된 CPU 소모를 줄이긴 위한용
	//static void SleepThread(DWORD msTick=1) { ::Sleep(msTick); }//boost::this_thread::sleep(boost::posix_time::millisec(msTick)); }
    static HANDLE m_hWaitEvent;
    static void SleepThread(DWORD msTick=16);

public:
	/// 싱글톤
	static ResourceThreadManager& GetInstance();

	/// @brief  리소스 메니저를 등록 
	/// @remark 리소스 메니저는 ResourceThreadInterface를 구현하고 있다
	void Register(ResourceThreadInterface& resourceMgr );

	/// 쓰레드 시작
	void Start();

	/// @brief   쓰레드 중지
	/// @remark  쓰레드가 동작하고 있으면 멈출때까지 대기
	void Stop();
private:

	friend class WorkerThread;
	/// 쓰레드 중지 플래그가 켜져 있는가?
	bool IsStopped() { return m_bStop; }

	/// 등록된 리소스 메니저의 로딩큐(m_loadingQueue)에서 로딩을 원하는 데이터를 가져와
	/// 데이터를 넣어주는 역활 
	void Run();
private:

	/// 실제 작업하는 쓰레드 클래스
	class WorkerThread
	{
	public:
		WorkerThread(ResourceThreadManager* pThreadManager) { m_pThreadManager = pThreadManager; }
		void operator()();

	private:
		ResourceThreadManager* m_pThreadManager;
	};

private:
	/// 쓰레드가 중지 될 경우 이 플레그가 켜진다 , 그리고 작업쓰레드에서는 이 플래그를 보고 종료하게됨
	bool m_bStop;

	// 컨디션을 이용하려 했더니 wait 할때 걸리네...  
	// 리소스마다의 각각의 쓰레드면 각각의 락으로 넣고 뺄때 wait를 하면 되지만 
	// 하나의 쓰레드 이므로 곤란하네
	//	boost::condition_variable m_condition;

	/// 쓰레드 객체
	boost::thread m_thread;

	/// 등록된 리소스 메니저들
	std:: vector< ResourceThreadInterface* > m_resourceMgrList;
};


/// 편의용 클래스  리소스 메니저들을 일리리 Update 호출하는것을 까먹을수도 있고 , 불편하기에
/// 일관적으로 이 클래스에 등록되어 사용되어진다
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

	/// 리소스 메니저 등록
	void Register(ResourceUpdatorInterface& updator);
	
	/// 리소스메니저 일괄 업데이트
	void Update();

    /// 리소스 메니저 일괄 데이터 삭제
	void Clear();

private:
	/// 리소스 메니저 포인터 컨테이너
	std::vector<ResourceUpdatorInterface*> m_updatorVec;

    /// Update 관리자 순서
    size_t m_nUpdateIndex;
};





//////////////////////////////////////////////////////////////////////////
#include "./ResourceManager.inl"

//////////////////////////////////////////////////////////////////////////


#endif
