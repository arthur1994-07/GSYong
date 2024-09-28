#include "pch.h"

#include "./LuaMan.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../=LuaPlus/src/LuaPlus/LuaPlus.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


using namespace LuaPlus;


LuaMan::LuaMan()
:m_pLuaState(NULL)
{
	try 
	{
		m_pLuaState = LuaState::Create(true);

		if (!m_pLuaState)
		{
			sc::writeLogError(sc::string::format("LuaState::Create() Failure.(%1%)", LuaStackObject(m_pLuaState, -1).GetString()));
		}

	}
	catch( const LuaException& e )
	{
		sc::writeLogError(sc::string::format("LuaMan::LuaMan() - %1%", e.GetErrorMessage()));

	}
}


LuaMan::~LuaMan()
{
	if( m_pLuaState )
    {
        LuaPlus::LuaState::Destroy(m_pLuaState);
        m_pLuaState = NULL;
    }
}


void LuaMan::LoadScript(const char *szFilePath)
{
	if (!szFilePath)
	{
		sc::writeLogError(std::string("LuaMan::LoadScript() - Script FileName is NULL."));
		return;
	}

	try
	{
		if (m_pLuaState->DoFile(szFilePath) != 0)
		{
			sc::writeLogError(sc::string::format("LuaState::DoFile() Failure.(%1%)", LuaStackObject(m_pLuaState, -1).GetString()));
		}

		ToDo();

		m_bLoaded = TRUE;
	}
	catch( const LuaException& e )
	{
		sc::writeLogError(sc::string::format("LuaMan::LoadScript() - %1%", e.GetErrorMessage()));
	}
}
