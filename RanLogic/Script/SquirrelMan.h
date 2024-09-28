#pragma once
//
//mjeon.attendance
//

#include "./ScriptMan.h"

#undef V	// RanLogic/stdafx.h에 V 매크로 정의되어 있음
#include "../../=Squirrel/include/sqrat.h"
#include "../../=Squirrel/include/sqstdsystem.h"
#include "../../=Squirrel/include/sqstdmath.h"


#define SQ_DEFAULT_STACK_SIZE	1024 // number of objects

//
// Squirrel Manager. MUST be constructed only once.
//
class SquirrelMan: public IScriptMan
{
public:
	SquirrelMan();
	virtual ~SquirrelMan();
	
	void LoadScript(const char *szFilePath);
	virtual void ToDo() = 0;

	virtual BOOL IsLoaded() {return m_bLoaded;}

protected:
	HSQUIRRELVM	m_vm;	
};


//
// Print Message Handlers
//
void		SqPrintCompileError(HSQUIRRELVM, const SQChar * desc, const SQChar * source, SQInteger line, SQInteger column);
void		SqPrintMsg(HSQUIRRELVM v, const SQChar *s, ...);
SQInteger	SqPrintErrorMsg(HSQUIRRELVM v);


//
//mjeon.각종 type의 환경변수값 등을 획득해오기 위한 용도로 Squirrel을 사용하는 부분이 있다면 아래의 macro를 사용하도록 하자.
//
#define SQ_ADDGETCONFIG(_table_, _type_)								\
	_type_ GetConfig##_type_(std::string strSlotName)					\
	{																	\
		Sqrat::Object obj = _table_.GetSlot(strSlotName.c_str());		\
																		\
		if (obj.IsNull())												\
		{																\
		sc::writeLogError(sc::string::format("SQ_GETCONFIG(%1%) - obj is NULL!", strSlotName));\
			return 0;													\
		}																\
																		\
		return obj.Cast<_type_>();										\
	}