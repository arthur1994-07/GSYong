#pragma once
//
//mjeon.groupchat
//
#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"


#define CHAT_GETCONFIG(_type_) SQ_ADDGETCONFIG(m_ConfigTable, _type_)

class ChatScriptMan : public SquirrelMan
{
public:
	ChatScriptMan();
	virtual ~ChatScriptMan();

	virtual void ToDo();

	CHAT_GETCONFIG(UINT);
	CHAT_GETCONFIG(WORD);
	CHAT_GETCONFIG(float);

public:
	Sqrat::Table m_ConfigTable;
};