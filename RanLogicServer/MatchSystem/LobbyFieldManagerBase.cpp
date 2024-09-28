

#include "pch.h"
#include "MatchingManager.h"
#include "./MatchingDefine.h"
#include "../../SigmaCore/Lua/MinLua.h"

#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "../InstanceSystem/LogicField/InvokeFunctionField.h"

#include "../../RanLogic/GLUseFeatures.h"

#define MATCHINGPROPERTY_REGISTER( REGESTKEY )								\
	LuaPlus::LuaObject REGESTKEY = m_script->lua_getFromName("##REGESTKEY")	\
	if ( REGESTKEY.IsNil() == true )										\
	{																		\
	sc::writeLogError("RoomManagerBase : ##REGESTKEY is nil");			\
	return;																\
	}																		


namespace MatchingSystem
{
	LobbyFieldManagerBase::LobbyFieldManagerBase(MatchingGameBase* parent,sc::LuaInstance* script)
		:m_script(script),m_parent(parent),m_fElpaseTime(0)
	{
		InstanceSystem::InitializeFieldInstanceInvokeFunction(*m_script);

		// 서버에 로그를 남김
		m_script->registdirectFunction("Serverlog",*this,&LobbyFieldManagerBase::Serverlog);

		m_script->registdirectFunction("GetMatchingStatus",*this,&LobbyFieldManagerBase::GetMatchingStatus);
	}

	LobbyFieldManagerBase::~LobbyFieldManagerBase()
	{
		m_script->OnDestroy();
		SAFE_DELETE(m_script);
	}


	void LobbyFieldManagerBase::TournamentRequestJoin( DWORD ClietDBID ) 
	{
		if ( m_script->PushFunctionEx("TournamentRequestJoin") == true )
		{
			m_script->PushInteger(ClietDBID);
			if ( m_script->CallFunction(1, 1) == true )
			{
				bool isJoin = m_script->PopBoolean();
				if(isJoin == true)
				{
					sc::writeLogInfo("LobbyManagerbase : TournamentRequestJoin Success");
				}
			}
		}
		GLMSG::NET_MATCHING_ADMISSION_FA _messge = GLMSG::NET_MATCHING_ADMISSION_FA(ClietDBID,m_parent->GetScriptID());
		MatchingCore::getinstance()->sendMessageFieldToAgent(&_messge);
	}

	void LobbyFieldManagerBase::FieldMatchingStatusChaged( DWORD status ) 
	{
		m_Status = (MATCHING_CONTENTS_STATUS)status;
		return;
	}

	DWORD LobbyFieldManagerBase::GetMatchingStatus()
	{
		return m_Status;
	}

	void LobbyFieldManagerBase::SendPostWinMoneyFC( DWORD dwDbNum, DWORD dwKang, LONGLONG winMoney, EM_MATCHING_POST_TYPE emPostType ) 
	{

		if ( m_script->PushFunctionEx("TournamentPostMessage") == true )
		{
			m_script->PushInteger(dwDbNum);
			m_script->PushInteger(dwKang);
			m_script->PushInteger(winMoney);
			m_script->PushInteger(emPostType);
			m_script->CallFunction(4, 0);
		}
	}

	void LobbyFieldManagerBase::Serverlog(const char* message)
	{
		sc::writeLogInfo(message);
	}
}