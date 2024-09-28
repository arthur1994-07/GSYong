

#ifndef  _mrcAssert_h_
#define  _mrcAssert_h_

#include "mrcConfig.h"
#include "mrcString.h"


//////////////////////////////////////////////////////////////////////////
namespace mrc
{


	class Exception
	{
	public:
		Exception(const NString& msg) : mMsg(msg) {}
		virtual ~Exception() {}

		const NString& getMessage() const { return mMsg; } 

	private:
		NString mMsg;
	};

	class ExceptionAssert : public Exception
	{
	public:
		ExceptionAssert(const NString& msg) : Exception(msg) {}
	};

	class ExceptionScript : public Exception
	{
	public:
		ExceptionScript(const NString& msg) : Exception(msg) {}
	};


//////////////////////////////////////////////////////////////////////////
	// 스크립트가 동작할때 콜스택 보여주기
	NString getSqError();
//////////////////////////////////////////////////////////////////////////



	inline void DEBUG_ABORT()
	{
#if (defined(_WIN32) || defined(_WIN64))
		DebugBreak();
#else
		abort();
#endif
	}

	inline void ShowMessageBox(const NString& msg)
	{
		setShowedAssertMessage(true);
#if defined(_WIN32) || defined(_WIN64)
		MessageBoxA(NULL,msg.c_str(),NULL,MB_APPLMODAL);
#else
		printf("MessageBox : %s \n",msg.c_str());
		printf("typing : OK \n");
		CHAR test[256]={0,};
		while( _tcsicmp(test,_mrcT("OK")) != 0 )
		{
			_fgetts(test, 1024, stdin); 
		}
#endif
		setShowedAssertMessage(false);
	}

	inline void ShowMessage(const NString& msg)
	{
		F_Print funcPrint =	getStandardPrint();
		if( funcPrint )	
			funcPrint(toString(msg).c_str());

		ShowMessageBox(msg);
	}

	template<class T>
	bool DEBUG_BREAK(const NString& fileName,int32 line,const NString& funcName,const NString& msg,int assertType)
	{
		NString errorMsg = fileName + "(line : " + 
			toNString(line) + ") \nfunction Name : " + funcName + "\n" + msg;

		if( isRunningScript() )
		{
			errorMsg += "\n -> Script Call Stack :  " + getSqError() + "\n"; 
		}

		if( assertType == MIRACLE_ASSERT_TYPE_DEFAULT )
		{
			ShowMessage(errorMsg);
			DEBUG_ABORT();
		}
		else if( assertType == MIRACLE_ASSERT_TYPE_EXCEPTION )
		{
			throw T( errorMsg );			
		}
		else
		{
			ShowMessage(errorMsg);
			DEBUG_ABORT();
		}
		
		return false;
	}


}

// 릴리즈	(expr)? ((void)0): ::
#define mrcASSERT_MSG(expr, msg) ((void) ((expr) || \
	::mrc::DEBUG_BREAK<mrc::ExceptionAssert>( __FILE__,__LINE__,__FUNCTION__,::mrc::toNString(#expr) + ::mrc::toNString(" => ") + ::mrc::toNString(msg),MIRACLE_ASSERT_TYPE_DEFAULT )))  

#define mrcASSERT(expr) mrcASSERT_MSG(expr,"Normal ASSERT")

//////////////////////////////////////////////////////////////////////////
// 반드시 중단되어야함
#define mrcCRITIAL_ASSERT_MSG(expr, msg) ((void) ((expr) || \
	::mrc::DEBUG_BREAK<mrc::ExceptionAssert>( __FILE__,__LINE__,__FUNCTION__,::mrc::toNString(#expr) + ::mrc::toNString(" => ") + ::mrc::toNString(msg),MIRACLE_ASSERT_TYPE_DEFAULT)))  

#define mrcCRITIAL_ASSERT(expr) mrcASSERT_MSG(expr,"Critical ASSERT")


// 스크립트용은 항상 ExceptionScript 예외를 던진다
// #define mrcScrASSERT_MSG(expr, msg) ((void) ((expr) || \
// 	::mrc::DEBUG_BREAK<mrc::ExceptionScript>( __FILE__,__LINE__, \
// 	::mrc::toNString(#expr) + ::mrc::toNString(" => ") + ::mrc::toNString(msg) + "\n"\
// 	" -> Script Binder Error :  " + ::mrc::getSqError(),MIRACLE_ASSERT_TYPE_EXCEPTION )))  
// 
// #define mrcScrASSERT(expr) mrcScrASSERT_MSG(expr,"normal script ASSERT")
#define mrcScrASSERT_MSG(expr, msg) ((void) ((expr) || \
	::mrc::DEBUG_BREAK<mrc::ExceptionScript>( __FILE__,__LINE__,::mrc::toNString(::mrc::getScriptClassName(::mrc::toNString(__FUNCTION__))),::mrc::toNString(#expr) + ::mrc::toNString(" => ") + ::mrc::toNString(msg),MIRACLE_ASSERT_TYPE )))  

#define mrcScrASSERT(expr) mrcScrASSERT_MSG(expr,"normal script ASSERT")


namespace mrc
{
	template<class T> 
	inline T& refPtr(const mrcWeakenPtr<T>& t) 
	{
		mrcASSERT(!t.expired());
		return *(t.lock()); 
	}

	template<class T> 
	inline T& refPtr(mrcWeakenPtr<T>& t) 
	{
		mrcASSERT(!t.expired());
		return *(t.lock()); 
	}
}


#endif