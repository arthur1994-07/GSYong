

#ifndef _mrcSqratDefine_h_
#define _mrcSqratDefine_h_


#define SQRAT_ASSERT_MSG mrcScrASSERT_MSG
#define SQRAT_ASSERT mrcScrASSERT


typedef struct SQVM* HSQUIRRELVM;


#define SCRIPT_CALL_IF_FAILED_MSG( X , MSG ) X; mrcScrASSERT_MSG(!isScriptCallErrorValue(),MSG) 

#define SQRAT_ASSERT_ARG_TYPE(TEST,VM,ARG,MSG) ::mrc::setAssertIsSucceedChangeSqObjectToArg(TEST) ? \
	true : ::mrc::setAssertArgTypeMsg(VM,ARG,::mrc::toNString(MSG))

#define SQRAT_TEST_ARG_TYPE() mrcScrASSERT_MSG(getAssertIsSucceedChangeSqObjectToArg(), \
	toNString(getAssertArgType().first) + " arg Index error : " + getAssertArgType().second )

#define SQRAT_ASSERT_ARG_COUNT(VM,COUNT) mrc::setAssertIsAssertArgCount(VM,COUNT)
#define SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN() if( !mrc::getAssertIsAssertArgCount() ) return
#define SQRAT_TEST_ARG_COUNT() mrcScrASSERT_MSG(getAssertIsAssertArgCount(),getAssertArgCountMsg())

#define  SQRAT_ASSERT_OVERLOADING_ARG_TYPE(OVERLOADING_STR) ::mrc::setAssertOverloadingArgType(OVERLOADING_STR)
#define  SQRAT_TEST_ASSERT_OVERLOADING() mrcScrASSERT_MSG(isAssertOverloadingArgType(),getAssertOverloadingArgType());

#define  SQRAT_VAR_GET_CLASS_NAME() mrc::getSqClassNameInspVarGet(vm)
#define  SQRAT_GET_CLASS_NAME(TYPE) mrc::getScriptClassName( typeid(TYPE).name() )

//////////////////////////////////////////////////////////////////////////

#define SQRAT_TEST_ARG() SQRAT_TEST_ARG_COUNT(); SQRAT_TEST_ARG_TYPE(); SQRAT_TEST_ASSERT_OVERLOADING();
#define MIRICALE_SCRIPT_TEST_CLASS_FUNC() SQRAT_TEST_ARG(); mrcScrASSERT(isValid())

#define SqStdString ::mrc::String
#define SqStdStringStream ::mrc::StringStream

// 실수 방지용
#define MAKE_SQ_FUNC(CLASS,FUNC) Func(_mrcT(#FUNC),&CLASS::FUNC)
#define MAKE_SQ_STATIC_FUNC(CLASS,FUNC) StaticFunc(_mrcT(#FUNC),&CLASS::FUNC)



namespace mrc
{
	bool& isScriptCallErrorValue();

	bool setAssertIsSucceedChangeSqObjectToArg(bool isSucceedChangeSqObjectToArg);
	bool getAssertIsSucceedChangeSqObjectToArg();
	bool setAssertArgTypeMsg(HSQUIRRELVM vm,int32 argIndex,const NString& msg);


	bool setAssertIsAssertArgCount(HSQUIRRELVM vm,int32 nativeFuncArgCount);
	bool getAssertIsAssertArgCount();
	NString getAssertArgCountMsg();

	bool isAssertOverloadingArgType();
	String getAssertOverloadingArgType();
	void setAssertOverloadingArgType(const String& errorMsg);

	typedef std::pair<int32,NString> AssertArgAndMsg;
	const AssertArgAndMsg& getAssertArgType();

	String getSqClassNameInspVarGet(HSQUIRRELVM vm);
}



#include "sqrat.h"



#endif