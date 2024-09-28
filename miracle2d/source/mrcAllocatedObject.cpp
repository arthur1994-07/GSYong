

#include "mrcPrecompiled.h"
#include "mrcAllocatedObject.h"
#include "mrcUtil/mrcSingleton.h"



#if defined(_WIN32) || defined(_WIN64) 
void enableLowFragmentationHeap()
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	static bool once = false;
	if( once == false )
	{
		HANDLE heaps[1025];
		DWORD nheaps = GetProcessHeaps(1024, heaps);

		for (DWORD i = 0; i < nheaps; i++) {
			ULONG  HeapFragValue = 2;
			HeapSetInformation(heaps[i],
				HeapCompatibilityInformation,
				&HeapFragValue,
				sizeof(HeapFragValue));
		}

		once = true;
	}
}
#else
void enableLowFragmentationHeap()
{
}
#endif

MIRACLE_STATIC_INIT_FUNC
(
	enableLowFragmentationHeap();
)


#if MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_BOOST_POOL_ALLOCATOR
#include <boost/pool/pool.hpp>
#endif

namespace mrc
{

#if MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_BOOST_POOL_ALLOCATOR
	namespace private_pool_alloc
	{
		class MemoryPool 
		{
		public:
			explicit MemoryPool(size_t size) : mPool(size) 
			{
			}

			void* malloc() 
			{
				scoped_spinlock lock(mMutex);
				return mPool.malloc();
			}
			void free(void* ptr)
			{
				scoped_spinlock lock(mMutex);
				mPool.free(ptr);
			}

			size_t getSize() const { return mPool.get_requested_size(); }

		private:
			spinlock mMutex;
			boost::pool<> mPool;
		};
		
		static const int32 POOL_COUNT = 15;
//		static const int16 FLAG = 9999;
		static const int16 MAX_POOL_SIZE = 256;
		class MemoryPoolMgr 
		{
			MIRACLE_FRIEND_CLASS_SINGLETON(MemoryPoolMgr);

			struct PoolSizeCmp
			{
				bool operator()(const MemoryPool* pool,const int32 aSize) const 
				{
					return (int32)(*pool).getSize() < aSize;
				}

				bool operator()(const int32 aSize,const MemoryPool* pool) const
				{
					return aSize < (int32)(*pool).getSize();
				}

				bool operator()(const MemoryPool* pool1,const MemoryPool* pool2) const
				{
					return (*pool1).getSize() < (*pool2).getSize();
				}
			};
			//////////////////////////////////////////////////////////////////////////
			MemoryPoolMgr() : 
				mPoolArray0(4),
				mPoolArray1(8),
				mPoolArray2(16),
				mPoolArray3(24),
				mPoolArray4(32),
				mPoolArray5(40),
				mPoolArray6(48),
				mPoolArray7(64),
				mPoolArray8(80),
				mPoolArray9(96),
				mPoolArray10(128),
				mPoolArray11(160),
				mPoolArray12(192),
				mPoolArray13(224),
				mPoolArray14(MAX_POOL_SIZE)

			{
				mPoolArray[0] = &mPoolArray0;
				mPoolArray[1] = &mPoolArray1;
				mPoolArray[2] = &mPoolArray2;
				mPoolArray[3] = &mPoolArray3;
				mPoolArray[4] = &mPoolArray4;
				mPoolArray[5] = &mPoolArray5;
				mPoolArray[6] = &mPoolArray6;
				mPoolArray[7] = &mPoolArray7;
				mPoolArray[8] = &mPoolArray8;
				mPoolArray[9] = &mPoolArray9;
				mPoolArray[10] = &mPoolArray10;
				mPoolArray[11] = &mPoolArray11;
				mPoolArray[12] = &mPoolArray12;
				mPoolArray[13] = &mPoolArray13;
				mPoolArray[14] = &mPoolArray14;


			}

		public:
			~MemoryPoolMgr(){}
			

			static MemoryPoolMgr& getInstance() 
			{
				typedef Loki::SingletonHolder<MemoryPoolMgr,Loki::CreateUsingNew,
					Loki::LongevityLifetime::DieLast > MemoryPoolMgrS;

				return MemoryPoolMgrS::Instance();
			}

			void* malloc(size_t sz,int32 block, const char* fileName ,int32 fileLine)
			{
				// 디버깅 정보인데 아직  미처리
				block;
				fileName;
				fileLine;
				return malloc(sz);
			}

			void* malloc(size_t sz)
			{
				size_t total = sz + sizeof(size_t);
				MemoryPool** ppPool = findMemoryPool(total);
				
				size_t* pHeader = NULL;
				if( ppPool != endIter())
				{
					mrcCRITIAL_ASSERT( (*ppPool)->getSize() >= total );
					pHeader = (size_t*)(*ppPool)->malloc();
				}
				else
				{
					pHeader = (size_t*)::malloc(total);	
				}
				
				pHeader[0] = (size_t)sz;
				return &pHeader[1];
			}

			void free(void* ptr)
			{
				if( NULL == ptr )
					return;

				size_t* pHeader = ((size_t*)ptr)-1;
				size_t sz = pHeader[0];
				size_t total = sz + sizeof(size_t);
				MemoryPool** ppPool = findMemoryPool(total);
				if( ppPool != endIter() )
				{
					mrcCRITIAL_ASSERT( (*ppPool)->getSize() >= total );
					return (*ppPool)->free(pHeader);
				}


				return ::free(pHeader);
			}
			
		private:
			MemoryPool** findMemoryPool(size_t sz)
			{
				// sz - 1를 해줘야 딱 맞는게 왔을때 찾을수 있다
				return std::upper_bound(beginIter(),endIter(),int32(sz)-1,PoolSizeCmp());
			}

			MemoryPool** beginIter() { return &mPoolArray[0]; }
			MemoryPool** endIter() { return &mPoolArray[POOL_COUNT]; }
		private:
			MemoryPool mPoolArray0;
			MemoryPool mPoolArray1;
			MemoryPool mPoolArray2;
			MemoryPool mPoolArray3;
			MemoryPool mPoolArray4;
			MemoryPool mPoolArray5;
			MemoryPool mPoolArray6;
			MemoryPool mPoolArray7;
			MemoryPool mPoolArray8;
			MemoryPool mPoolArray9;
			MemoryPool mPoolArray10;
			MemoryPool mPoolArray11;
			MemoryPool mPoolArray12;
			MemoryPool mPoolArray13;
			MemoryPool mPoolArray14;

			MemoryPool* mPoolArray[POOL_COUNT];
		};
	}

	void* AllocatedObject::allocateBytes( size_t size )
	{
		return private_pool_alloc::MemoryPoolMgr::getInstance().malloc(size);
	}

	void* AllocatedObject::allocateBytes( size_t size,int32 block, const char* fileName ,int32 fileLine )
	{
		return private_pool_alloc::MemoryPoolMgr::getInstance().malloc(size,block,fileName,fileLine);
	}

	void AllocatedObject::deallocateBytes( void* ptr )
	{
		private_pool_alloc::MemoryPoolMgr::getInstance().free(ptr);
	}

	void AllocatedObject::deallocateBytes( void* ptr,int32 block )
	{
		private_pool_alloc::MemoryPoolMgr::getInstance().free(ptr);
	}

#elif MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_STD_ALLOCATOR

// 	void* AllocatedObject::allocateBytes( size_t size )
// 	{
// 		return ::malloc(size);
// 	}
// 
// 	void* AllocatedObject::allocateBytes( size_t size,int32 block, const char* fileName ,int32 fileLine )
// 	{
// 		return ::malloc(size);
// 	}
// 
// 	void AllocatedObject::deallocateBytes( void* ptr )
// 	{
// 		::free(ptr);
// 	}
// 
// 	void AllocatedObject::deallocateBytes( void* ptr,int32 block )
// 	{
// 		::free(ptr);
// 	}

#else
	#error not support
#endif


}

