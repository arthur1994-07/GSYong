#include "pch.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/GLogicDataMan.h"

#include "./GLScriptedFSM.h"
#include "./GLBasicAIFSM.h"
#include "./GLChannelingAIFSM.h"
#include "./GLDelayedInvokeAIFSM.h"
#include "./GLTrapAIFSM.h"

#include "./GLFSMContainer.h"

namespace AISystem
{
	
	GLFSMContainer::GLFSMContainer ()
		: m_dwMaxFSMType( 0 )
		, m_dwHireSummonLevelInterval( 50 )
		, m_dwSpeedUpValue( 300 )
		, m_dwEventSkillRange( 200 )
		, m_dwOwnerRange( 200 )
	{
		sc::lua_init();
	}

	GLFSMContainer::~GLFSMContainer ()
	{
		sc::lua_destroyState();
	}

	bool GLFSMContainer::LoadScriptFile ()
	{
		// Script 파일의 주소;
		SUBPATH* pPath = GLogicData::GetInstance().GetPath();
		if( !pPath )
		{
			sc::writeLogError( "GLFSMContainer::LoadScriptFile - pPath is NULL." );
			return false;
		}

		// Path;
		std::string strScriptPath( sc::getAppPath() );
		strScriptPath += pPath->GLogicServerScriptsPath();

		if( !sc::lua_doFile(
			std::string( strScriptPath +
			GLogicData::GetInstance().GetFSMContainerFileName() ).c_str() ) )
			return false;

		/// Global Value;
		// AI List;
		LuaPlus::LuaObject basicGroup =
			sc::lua_getGlobalFromName( "List" );
		if ( basicGroup.IsNil() )
		{
			GASSERT( 0 && "Load 'AI FSM List Group' Error!!" );
			return false;
		}

		int nScriptCount = 0;
		for ( LuaPlus::LuaTableIterator iter( basicGroup );
			iter; iter.Next() )
		{
			std::string strTempFileName;
			strTempFileName = sc::GetStringByObject( iter.GetValue() );

			// Script를 쓰지 않을 경우도 지원하기 위함이다;
			GLScriptedFSM* pScript = NULL;
			switch ( nScriptCount )
			{
			case FSMAIIndex::FSM_AI_BASIC:
				pScript = new GLBasicAIFSM();
				break;
			case FSMAIIndex::FSM_AI_CHANNELING:
				pScript = new GLChannelingAIFSM();
				break;
			case FSMAIIndex::FSM_AI_DELAYED_INVOKE:
				pScript = new GLDelayedInvokeAIFSM();
				break;
			case FSMAIIndex::FSM_AI_TRAP:
				pScript = new GLTrapAIFSM();
				break;
			}

			std::tr1::shared_ptr< GLScriptedFSM > spScript( pScript );
			if ( !spScript )
			{
				sc::writeLogError(
					sc::string::format(
					"GLFSMContainer::LoadScriptFile - pScript %1% is NULL.",
					nScriptCount ) );

				GASSERT ( 0 && "Failed Loading FSM Script." );

				continue;
			}

			spScript->Init( std::string( strScriptPath + strTempFileName ).c_str(), nScriptCount );

			m_vecScript.push_back( spScript );

			nScriptCount++;
		}

		m_dwMaxFSMType = nScriptCount;

		// global value list;
		LuaPlus::LuaObject globalGroup =
			sc::lua_getGlobalFromName( "Basic" );
		if ( globalGroup.IsNil() )
		{
			GASSERT( 0 && "Load 'AI FSM Basic Group' Error!!" );
			return false;
		}

		m_dwHireSummonLevelInterval = sc::GetIntegerByName( globalGroup, "hire_summon_level_interval" );
		m_dwSpeedUpValue = sc::GetIntegerByName( globalGroup, "summon_speed_up_value" );
		m_dwEventSkillRange = sc::GetIntegerByName( globalGroup, "summon_event_skill_range" );
		m_dwOwnerRange = sc::GetIntegerByName( globalGroup, "summon_owner_range" );

		return true;
	}

	void GLFSMContainer::Reload ()
	{
		for ( unsigned int i=0; i<m_vecScript.size(); ++i )
		{
			std::tr1::shared_ptr< GLScriptedFSM > tempScriptedFSM =
				m_vecScript.at( i );
			if ( tempScriptedFSM )
				tempScriptedFSM->Reload();
		}
	}

	std::tr1::shared_ptr< GLScriptedFSM > GLFSMContainer::GetFSM( DWORD dwIndex )
	{
		if ( dwIndex >= m_dwMaxFSMType ||
			dwIndex >= m_vecScript.size() )
			return std::tr1::shared_ptr< GLScriptedFSM >( new GLScriptedFSM() );

		return m_vecScript.at( dwIndex );
	}
}