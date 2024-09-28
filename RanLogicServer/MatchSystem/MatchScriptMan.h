#pragma once
//
//mjeon.instance.match
//
#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"


#define MATCH_GETCONFIG(_type_) SQ_ADDGETCONFIG(m_ConfigTable, _type_)

class MatchScriptMan : public SquirrelMan
{
public:
	MatchScriptMan();
	virtual ~MatchScriptMan();

	virtual void ToDo();

	MATCH_GETCONFIG(UINT);
	MATCH_GETCONFIG(WORD);
	MATCH_GETCONFIG(float);

	DWORD	IsInstanceMap(WORD mid, WORD sid);
	BOOL	IsPvPMap(WORD mid, WORD sid);
	DWORD	GetExpireM(WORD mid, WORD sid);	

public:
	Sqrat::Table m_ConfigTable;
	Sqrat::Function m_IsInstanceMap;
	Sqrat::Function m_IsPvPMap;

	Sqrat::Function m_GetExpireM;
};