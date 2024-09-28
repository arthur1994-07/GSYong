#pragma once
//
//mjeon.CaptureTheField
//
#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

/*
#define CTF_GETCONFIG(_type_)											\
	_type_ GetConfig##_type_(std::string strSlotName)					\
	{																	\
		Sqrat::Object obj = m_ConfigTable.GetSlot(strSlotName.c_str());	\
																		\
		if (obj.IsNull())												\
		{																\
		sc::writeLogError(sc::string::format("CTF_GETCONFIG(%1%) - obj is NULL!", strSlotName));\
			return 0;													\
		}																\
																		\
		return obj.Cast<_type_>();										\
	}
*/

#define CTF_GETCONFIG(_type_) SQ_ADDGETCONFIG(m_ConfigTable, _type_)

class CaptureTheFieldScriptMan : public SquirrelMan
{
public:
	CaptureTheFieldScriptMan();
	virtual ~CaptureTheFieldScriptMan();

	virtual void ToDo();

	CTF_GETCONFIG(UINT);
	CTF_GETCONFIG(WORD);
	CTF_GETCONFIG(float);
	CTF_GETCONFIG(bool);


public:
	Sqrat::Table m_ConfigTable;

private:
};