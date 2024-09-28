


#ifndef _mrcConfig_h_
#define _mrcConfig_h_

#define _CRT_SECURE_NO_WARNINGS

#if (defined(_WIN32) || defined(_WIN64))

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <new>

#include <memory>
#include <limits>
#include <algorithm>
#include <stdlib.h>

#include <string>

#undef  MIRACLE_ARRAYSIZE
#define MIRACLE_ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))


#define BOOST_TYPEOF_SILENT

// BOOST_SP_USE_QUICK_ALLOCATOR 이건 전역적으로 메모리를 할당후 해제를 안함 (계속 재활용) 문제는 crt debug시 메모리릭으로 간주됨
// 프로그래램 종료시 quick_allocator 내부의 static mutex가 가장 나중에 종료 한다고 보장할 방법이 명확히 없어보이기때문에
// quick_allocator 에서 alloc (new) 메모리를 dealloc (delete)를 하지 않아 보인다 ( dealloc 시 뮤택스가 살아 있다 보장할수 없다)
// 따라서 디버그일때는 다른걸 써야함
#ifdef _DEBUG
	#define BOOST_SP_USE_STD_ALLOCATOR
#else
	#define BOOST_SP_USE_QUICK_ALLOCATOR
#endif

#include <boost/foreach.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/static_assert.hpp>
#include <boost/make_shared.hpp>

#include <boost/utility.hpp>
#include <boost/timer.hpp>
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

#define mrcSharedPtr	  boost::shared_ptr
#define mrcWeakenPtr	  boost::weak_ptr
#define mrcAutoPtr		  std::auto_ptr
#define mrcSharedFromThis boost::enable_shared_from_this

#define mrcNonCopyAble boost::noncopyable


// 싱글턴을 쓰려면 반드시 mrcSingleton의 로키 싱글턴으로 수명주기를 맞추자
namespace Loki { template<class T> struct CreateUsingNew; }

namespace mrc
{
}

//	#define MIRACLE_FRIEND_FACTORY template<class TYPE> friend class Factory
//	#define MIRACLE_FRIEND_CLASS_SINGLETON()  template<class T> friend struct Loki::CreateUsingNew
#define MIRACLE_FRIEND_CLASS_SINGLETON(CLASS_NAME)  friend struct Loki::CreateUsingNew<CLASS_NAME>;


#define mrcStaticAssert(X) BOOST_STATIC_ASSERT(X)

//////////////////////////////////////////////////////////////////////////

#define MIRACLE_RENDERER_DX9    (1)
#define MIRACLE_RENDERER_OPENGL (2)
#define MIRACLE_RENDERER_DX11   (3)

#define MIRACLE_RENDERER_DEVICE MIRACLE_RENDERER_DX9

#define MIRACLE_RENDERER_DEVICE_DEBUG 0
//////////////////////////////////////////////////////////////////////////

#define MIRACLE_ASSERT_TYPE_DEFAULT   (1)
#define MIRACLE_ASSERT_TYPE_EXCEPTION (2)

#define MIRACLE_ASSERT_TYPE MIRACLE_ASSERT_TYPE_EXCEPTION

//////////////////////////////////////////////////////////////////////////

#define MIRACLE_DOUBLE_PRECISION 0


//////////////////////////////////////////////////////////////////////////

#define MIRACLE_OBJECT_STD_ALLOCATOR 1
#define MIRACLE_OBJECT_BOOST_POOL_ALLOCATOR 2

#ifdef _DEBUG
// 디버그의 경우 무조건 표준
#define MIRACLE_OBJECT_ALLOCATOR MIRACLE_OBJECT_STD_ALLOCATOR

#else
// 릴리즈의 경우 선택  MIRACLE_OBJECT_STD_ALLOCATOR,MIRACLE_OBJECT_BOOST_POOL_ALLOCATOR
#define MIRACLE_OBJECT_ALLOCATOR MIRACLE_OBJECT_STD_ALLOCATOR

#endif
//////////////////////////////////////////////////////////////////////////
// 2D는 보통 싱글 쓰레드에서 동작하므로 끄면 성능이 향상됨
// mrc 용 객체를 다른 쓰레드로 옮겨서 처리하고 싶다면 반드시 활성화 시켜야함

#define MIRACLE_OBJECT_MULTITHREAD_SAFE_ALLOCATOR 1

//////////////////////////////////////////////////////////////////////////
// #if MIRACLE_OBJECT_MULTITHREAD_SAFE_ALLOCATOR 
// 
// #if MIRACLE_OBJECT_ALLOCATOR == MIRACLE_OBJECT_LOKI_ALLOCATOR
// #define LOKI_CLASS_LEVEL_THREADING
// #endif
// 
// #endif
//////////////////////////////////////////////////////////////////////////


#define MIRACLE_USE_RENDERING_COMMANDER_WITH_MULTITHREAD 0


// 일단 부스트 방법만 추가
#define MIRACLE_CONTAINERS_USE_BOOST_MEMORY_ALLOCATOR 1
#define MIRACLE_CONTAINERS_USE_DEFAULT_MEMORY_ALLOCATOR 2

#define MIRACLE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR MIRACLE_CONTAINERS_USE_BOOST_MEMORY_ALLOCATOR

//////////////////////////////////////////////////////////////////////////

#define MIRACLE_SCRIPT_SQUIRREL 1
#define MIRACLE_SCRIPT_LUA 2

#define MIRACLE_SCRIPT_IMPLEMENT MIRACLE_SCRIPT_SQUIRREL



// #define MIRACLE_DEBUG_BREAK_MESSAGE_PRINT 1
// #define MIRACLE_DEBUG_BREAK_MESSAGE_BOX   2
// #define MIRACLE_DEBUG_BREAK_MESSAGE MIRACLE_DEBUG_BREAK_MESSAGE_PRINT
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////


namespace mrc
{
	// 일단 윈도우것만
#if defined(_WIN32) || defined(_WIN64)
	typedef __int8  int8;
	typedef __int16 int16;
	typedef __int32 int32;
	typedef long long int64;
	
	typedef unsigned __int8    uint8;
	typedef unsigned __int16   uint16;
	typedef unsigned __int32   uint32;
	typedef unsigned long long uint64;


	typedef ULONG_PTR ptr_size;



#else
	#error no support platform, yet
#endif

	mrcStaticAssert(sizeof(void*) == sizeof(ptr_size));



#if MIRACLE_DOUBLE_PRECISION == 1
	typedef double	real;
#else 
	typedef float	real;
#endif
	
	mrcStaticAssert( sizeof( long long ) == sizeof(__int64) );
	mrcStaticAssert( sizeof(bool) == 1 );

	enum { INVALID_SIZE = -1 };

	//////////////////////////////////////////////////////////////////////////
}



//////////////////////////////////////////////////////////////////////////



#if MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_DX9

	struct IDirect3DVertexBuffer9;
	struct IDirect3DIndexBuffer9;
	struct IDirect3DTexture9;
	struct IDirect3DSurface9;

namespace mrc
{
	enum { INVALID_HANDLE = 0 };

	typedef IDirect3DVertexBuffer9* HVertexBuffer; 
	typedef IDirect3DIndexBuffer9*  HFaceBuffer;
	typedef IDirect3DTexture9*		HTexture;
	typedef IDirect3DSurface9*	    HSurface;
}


#elif MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_OPENGL

namespace mrc
{
	typedef GLuint  HVertexBuffer;
	typedef GLuint  HFaceBuffer;
	typedef GLuint	HTexture;
	typedef GLuint	HSurface;

	enum { INVALID_HANDLE = -1 };
}

#else

	#error no support device, yet

#endif

//////////////////////////////////////////////////////////////////////////

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif


namespace mrc
{
	enum ID_SCOPE{ TYPE_SCOPE_INTERNAL = 0 ,TYPE_SCOPE_TRAFT = 10000 ,TYPE_SCOPE_EXTENAL = 20000 };

	inline uint8 makeColor(real color) { return uint8(color*real(255)); }
	inline real makeColor(uint8 color) { return real(color)/real(255); }

	//////////////////////////////////////////////////////////////////////////
	// gcc로 포팅시 HWND WPARAM LPARAM를 제정의 해주자
	//////////////////////////////////////////////////////////////////////////

	struct Message
	{
		Message(HWND hWnd,uint32 msgID,WPARAM wParam,LPARAM lParam) 
		{ mWnd = hWnd; mMsgID = msgID; mwParam = wParam; mlParam = lParam; }
		Message() { mWnd = NULL; mMsgID = 0; mwParam = 0; mlParam = 0; }
		HWND mWnd;
		uint32 mMsgID; 
		WPARAM mwParam; 
		LPARAM mlParam;
	};



	//////////////////////////////////////////////////////////////////////////

	template<class T>
	inline void makeInvalidHandle(T& handle)
	{
		handle = (T)INVALID_HANDLE;
	}

	template<class T>
	inline bool IsInvalidHandle(T& handle)
	{
		return handle == (T)INVALID_HANDLE;
	}

	//////////////////////////////////////////////////////////////////////////

	template<class T, class U> mrcSharedPtr<T> sharedptr_static_cast(mrcSharedPtr<U> const & r)
	{
		return boost::shared_static_cast<T>(r);
	}

	template<class T, class U> mrcSharedPtr<T> sharedptr_dynamic_cast(mrcSharedPtr<U> const & r)
	{
		return boost::shared_dynamic_cast<T>(r);
	}


	template<class T>
	inline mrcSharedPtr<T> SP_New() { return boost::make_shared<T>(); }

	template<class T, class A1>
	inline mrcSharedPtr<T> SP_New( A1 const & a1 ) { return boost::make_shared<T>(a1); }

	template< class T, class A1, class A2 >
	inline mrcSharedPtr<T> SP_New(A1 const & a1, A2 const & a2) { return boost::make_shared<T>(a1,a2); }

}

#define DECLARE_FRIEND_SP_NEW(X) friend mrcSharedPtr<X> boost::make_shared()

#define DECLARE_FRIEND_SP_NEW1 DECLARE_FRIEND_SP_NEW(X); \
	friend mrcSharedPtr<X> boost::make_shared( A1 const & a1 )

#define DECLARE_FRIEND_SP_NEW2 DECLARE_FRIEND_SP_NEW1(X); \
	friend mrcSharedPtr<X> boost::make_shared( A1 const & a1, A2 const & a2 )

//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////

namespace mrc
{


	template<class T> inline T& refPtr(const mrcAutoPtr<T>& t) { return *t; }
	template<class T> inline T& refPtr(mrcAutoPtr<T>& t) { return *t; }

	template<class T> inline T& refPtr(const mrcSharedPtr<T>& t) { return *t; }
	template<class T> inline T& refPtr(mrcSharedPtr<T>& t) { return *t; }

	template<class T> inline T& refPtr(T& t) { return t; }
	template<class T> inline const T& refPtr(const T& t) { return t; }

	template<class T> inline T& refPtr(T* t) { return *t; }
	template<class T> inline const T& refPtr(const T* t) { return *t; }
	
}



//////////////////////////////////////////////////////////////////////////


#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <boost/smart_ptr/detail/atomic_count.hpp>

namespace mrc
{
//	MIRACLE_OBJECT_MULTITHREAD_SAFE_ALLOCATOR
	inline void yield_thread() { boost::this_thread::yield(); }

	typedef boost::mutex mutex;
	typedef mutex::scoped_lock scoped_mutex;

	struct spinlock : public boost::detail::spinlock 
	{
		spinlock(){ v_= 0; }
	};

	typedef spinlock::scoped_lock scoped_spinlock;
	typedef boost::detail::atomic_count atomic_count;
//  쓰레드 safe 하지 않다면 null mutex로 만들기


}	



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 가변 메크로도 생각해보자
//		__MIRACLE_HELPER_VECTOR_INTERFACE__(...)
// 
#if MIRACLE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR == MIRACLE_CONTAINERS_USE_BOOST_MEMORY_ALLOCATOR

#include <boost/pool/pool_alloc.hpp>

//#if MIRACLE_MULTITHREAD_SAFE_ALLOCATOR
#define MIRACLE_ALLOCATOR( T ) boost::pool_allocator< T >
#define MIRACLE_NODE_ALLOCATOR( T ) boost::fast_pool_allocator< T >
#define MIRACLE_NODE_PAIR_ALLOCATOR( T , T1 ) boost::fast_pool_allocator< std::pair<T , T1> >

// 
// #else
// // 이걸 만든 이유는 다른 라이브러리 같은데서 boost 싱글턴객체를 쓸수도 있기때문에 
// // 싱글턴 객체의 혼용을 막기 위함 템플릿이라 가능함
// namespace mrc
// {
// 	struct mrc_user_allocator_new_delete
// 	{
// 		typedef std::size_t size_type;
// 		typedef std::ptrdiff_t difference_type;
// 
// 		static int8* malloc(const size_type bytes)
// 		{ return mrcNEW (std::nothrow) int8[bytes]; }
// 		static void free(int8* const block)
// 		{ mrcDELETE [] block; }
// 	};
// }
// 
// #define MIRACLE_ALLOCATOR(T) boost::pool_allocator< T, mrc_user_allocator_new_delete, boost::details::pool::null_mutex>
// #define MIRACLE_NODE_ALLOCATOR(T) boost::fast_pool_allocator< T, mrc_user_allocator_new_delete , boost::details::pool::null_mutex>
// #define MIRACLE_NODE_PAIR_ALLOCATOR( T , T1 ) boost::fast_pool_allocator< std::pair<T , T1>, mrc_user_allocator_new_delete , boost::details::pool::null_mutex>
// 
// #endif


#else

#define MIRACLE_ALLOCATOR(T) std::allocator< T >
#define MIRACLE_NODE_ALLOCATOR(T) std::allocator< T >
#define MIRACLE_NODE_PAIR_ALLOCATOR( T , T1 ) std::allocator< std::pair<T , T1> >

#endif

//////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
	#if defined(_WIN32) || defined(_WIN64)
		//#define _CRTDBG_MAP_ALLOC
		//#define _CRTDBG_MAP_ALLOC_NEW
		#include <crtdbg.h>

		//#define mrcNEW new

		#define mrcNEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
		#define mrcDELETE delete

	#else
		// No window platform
		#define mrcNEW new
		#define mrcDELETE delete

	#endif
#else 
	// 릴리즈
	#define mrcNEW new
	#define mrcDELETE delete
#endif


//////////////////////////////////////////////////////////////////////////
// xml 선언 

namespace mrc
{
	class TiXmlDocument;
	class TiXmlElement;
	class TiXmlNode;

}

#if _MSC_VER >= 1400
	#define mrcOVERRIDE override
#else
	#define mrcOVERRIDE
#endif


namespace mrc
{
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, MIRACLE_ALLOCATOR(wchar_t) >	WString;
	typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,MIRACLE_ALLOCATOR(wchar_t) > WStringStream;

	typedef std::basic_string<char, std::char_traits<char>, MIRACLE_ALLOCATOR(char) >	NString;
	typedef std::basic_stringstream<char,std::char_traits<char>,MIRACLE_ALLOCATOR(char) > NStringStream;

#ifdef UNICODE
#define _mrcT(X) L ## X
	typedef wchar_t CHAR;

	typedef WString String;
	typedef WStringStream StringStream;

#else
#define _mrcT(X) X
	typedef char CHAR; 

	typedef NString String;
	typedef NStringStream StringStream;

#endif



	typedef String TextureDataID;
	typedef String LayerID;



	typedef int (*F_Print)(const CHAR* , ... );
	void setStandardPrint(F_Print funcPrint);
	
	F_Print getStandardPrint();

	bool isRunningScript();
	bool isShowedAssertMessage();
	void setShowedAssertMessage(bool isShowed);


	String getScriptClassName(const NString& className);


	typedef void (*funcRegisterScriptObject)();

}


//////////////////////////////////////////////////////////////////////////
// 임시 이름만들기
#define MIRACLE_MAKE_TEMP_NAME2(NAME,LINE)   __temp__ ## NAME ## LINE
#define MIRACLE_MAKE_TEMP_NAME1(NAME,LINE)   MIRACLE_MAKE_TEMP_NAME2(NAME,LINE)
#define MIRACLE_MAKE_TEMP_NAME(NAME)		 MIRACLE_MAKE_TEMP_NAME1(NAME,__LINE__)


// 전역 초기화 만들기
#define MIRACLE_MAKE_STATIC_INIT_FUNC() __STATIC_INIT_FUNC__ ## __LINE__ ## __()
#define MIRACLE_STATIC_INIT_FUNC( INIT ) \
	bool MIRACLE_MAKE_STATIC_INIT_FUNC() \
{ \
	INIT \
	return true; \
} \
bool __temp__STATIC_INIT_FUNC__ ## __LINE__ ## __ =  MIRACLE_MAKE_STATIC_INIT_FUNC();
//////////////////////////////////////////////////////////////////////////





#endif //_mrcConfig_h_

