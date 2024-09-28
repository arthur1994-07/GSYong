
#ifndef _mrcMemoryMgr_h_
#define _mrcMemoryMgr_h_

#include "mrcConfig.h"



namespace mrc
{
	//////////////////////////////////////////////////////////////////////////
#if MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_BOOST_POOL_ALLOCATOR
	class AllocatedObject
	{
	public:
		explicit AllocatedObject()
		{ }

		void* operator new(size_t sz)
		{
			return allocateBytes(sz);
		}

		/// placement operator new
		void* operator new(size_t sz, void* ptr)
		{
			//			mrcASSERT(false);
			(void) sz;
			return ptr;
		}

		void* operator new[] ( size_t sz )
		{
			return allocateBytes(sz);
		}

		void operator delete( void* ptr, void* )
		{
			ptr;
			mrcASSERT(false);		
		}

		void operator delete(void* ptr)
		{
			deallocateBytes(ptr);
		}

		void operator delete[] ( void* ptr )
		{
			deallocateBytes(ptr);
		}

		////////////////////////////////////////////////////////////////////////
		/// operator new, with debug line info
		void* operator new(size_t sz,int32 block, const char*  fileName,int32 fileLine)
		{
			return allocateBytes(sz,block,fileName,fileLine);
		}

		/// array operator new, with debug line info
		void* operator new[] (size_t sz,int32 block, const char* fileName,int32 fileLine)
		{
			return allocateBytes(sz,block,fileName,fileLine);
		}

		void operator delete( void* ptr, int32 block, const char* ,int32 )
		{
			deallocateBytes(ptr,block);
		}

		void operator delete[] ( void* ptr, int32 block, const char* ,int32 )
		{
			deallocateBytes(ptr,block);
		}

	private:
		static void* allocateBytes( size_t size );
		static void* allocateBytes(size_t size,int32 block, const char*  fileName,int32 fileLine);

		static void deallocateBytes( void* ptr );
		static void deallocateBytes( void* ptr ,int32 block);

		//		static void deallocateBytes( void* ptr, int32 block, const char* ,int32 );

	};
#elif MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_STD_ALLOCATOR
	class AllocatedObject {};
#endif

}



#endif