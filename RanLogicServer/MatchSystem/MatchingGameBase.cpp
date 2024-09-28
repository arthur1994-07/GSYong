#include "pch.h"
#include "../TournamentManager/GLMatchingScriptManager.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"

#include "./InvokeFunctionAgentMatching.h"
#include "./MatchingManager.h"
#include "./MatchingDefine.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"
#include "./LobbyManagerBase.h"

#define MATCHING_CONTENT_TYPE   "Matching_Content_Type"
#define MATCHING_CONTENT_SEASON "Matching_Content_Season"
#define MATCHING_SIZE_PARAM     "MAX_Matching_SIZE"
#define MATCHING_MODE_ID		"Matching_Mode_ID"			// Matching Script 사용하지 않을 때 사용한다;


namespace MatchingSystem
{

	MatchingGameBase::MatchingGameBase(sc::LuaInstance* script, DWORD ScriptID)
		: m_Roominstance(NULL), m_Lobbyinstance(NULL),m_BettingManagerbase(NULL),m_LobbyFieldManagerBase(NULL),
		  m_script(script),m_ScriptID(ScriptID),m_MAX_MATCHING_SIZE(1), m_dwMatchingModeID( 0 )
	{
		m_groupmanager = new MatchingSystem::GLMatchingGroupManager();
        // MATCHING_SIZE_PARAM
        {
            LuaPlus::LuaObject _match_size = script->GetGlobalValue_Lua(MATCHING_SIZE_PARAM);

            if ( _match_size.IsNil() == false)
            {
                if(_match_size.IsNumber() == true)
                {
                    m_MAX_MATCHING_SIZE = _match_size.GetInteger();
                }
            }

            for (DWORD i = 0; i <m_MAX_MATCHING_SIZE ; i ++)
            {
                m_UnUseMatchingID.push_back((DWORD)i);
            }
        }
	}
	MatchingGameBase::~MatchingGameBase()
 	{

		m_script->OnDestroy();
		SAFE_DELETE(m_script);

		SAFE_DELETE(m_Roominstance);
		SAFE_DELETE(m_Lobbyinstance);
		SAFE_DELETE(m_BettingManagerbase);
		SAFE_DELETE(m_LobbyFieldManagerBase);
		SAFE_DELETE(m_groupmanager);

		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		for(; biter != m_matchingLandlist.end(); ++biter)
		{
			MatchingInstanceBase* m_MatchingInstance = NULL;
			m_MatchingInstance = biter->second;
			SAFE_DELETE(m_MatchingInstance);
		}
		m_matchingLandlist.clear();

		
	}



	bool MatchingGameBase::OnLoad()
	{
		ScriptManager* pmananger = MatchingCore::getinstance()->GetScriptManager();

		// AgentServer 이면 처리한다;
		if ( gpGaeaServer == NULL )
		{
			{
				if ( m_script->GetStringGlobalValue_Lua( MATCHING_CONTENT_TYPE, m_strContentType ) )
					bUseCompetitionRating = true;
				else
					bUseCompetitionRating = false;
			}

			{
				int nTempValue = 0;
				if ( m_script->GetIntegerGlobalValue_Lua( MATCHING_MODE_ID, nTempValue ) )
				{
					m_dwMatchingModeID = static_cast< DWORD >( nTempValue );
				}
				else
				{
					m_dwMatchingModeID = 0;
				}

				sc::writeLogInfo(
					sc::string::format(
					"MatchingGameBase: Matching_Mode_ID = %1%",
					m_dwMatchingModeID ) );
			}

			{
				LuaPlus::LuaObject _MatchingTable( m_script->GetGlobalValue_Lua( "Betting_Script" ) );
				if ( _MatchingTable.IsTable() )
				{
					int scriptid = 0;
					for ( LuaPlus::LuaTableIterator _iterator(_MatchingTable); _iterator; _iterator.Next() )
					{
						std::string _MatchingFileName;
						if ( m_script->GetStringGlobalValueObject_Lua( _iterator.GetValue(), _MatchingFileName ) )
						{
							// 해당 인자값의 Lua 읽어들이기;
							scriptid = pmananger->doFileDirect(_MatchingFileName);
						}
					}

					sc::LuaInstance* LuaInstance  = new sc::LuaInstance();
					if ( pmananger->getScriptCOPY( (DWORD)scriptid, LuaInstance ) == -1 )
						ASSERT( L"MatchingCore Script is Not Load : Check MatchingCore" );

					m_BettingManagerbase = new GLTournamentBetting( this, LuaInstance );
				}
				else
				{
					sc::writeLogInfo("MatchingGameBase : Betting_Script not table : %s");
				}
			}
			
		

			{
				LuaPlus::LuaObject _MatchingTable( m_script->GetGlobalValue_Lua("Lobby_Manager") );
				if ( _MatchingTable.IsTable() )
				{
					int scriptid = 0;
					for ( LuaPlus::LuaTableIterator _iterator(_MatchingTable); _iterator; _iterator.Next() )
					{
						std::string _MatchingFileName;
						if ( m_script->GetStringGlobalValueObject_Lua( _iterator.GetValue(), _MatchingFileName ) )
						{
							// 해당 인자값의 Lua 읽어들이기;
							scriptid = pmananger->doFileDirect(_MatchingFileName);
						}
					}

					sc::LuaInstance* LuaInstance  = new sc::LuaInstance();
					if( pmananger->getScriptCOPY(scriptid,LuaInstance) == -1)
						ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");

					m_Lobbyinstance =  new LobbyManagerbase(this,LuaInstance);
				}
				else
				{
					sc::writeLogInfo("MatchingGameBase : Lobby_Manager not table : %s");
					return false;
				}
			}

			{
				LuaPlus::LuaObject _MatchingTable( m_script->GetGlobalValue_Lua("Room_Admin_Script") );
				if ( _MatchingTable.IsTable() )
				{
					int scriptid = 0;
					for ( LuaPlus::LuaTableIterator _iterator(_MatchingTable); _iterator; _iterator.Next() )
					{
						std::string _MatchingFileName;
						if ( m_script->GetStringGlobalValueObject_Lua( _iterator.GetValue(), _MatchingFileName ) )
						{
							// 해당 인자값의 Lua 읽어들이기;
							scriptid = pmananger->doFileDirect(_MatchingFileName);
						}
					}

					sc::LuaInstance* LuaInstance  = new sc::LuaInstance();
					if( pmananger->getScriptCOPY((DWORD)scriptid,LuaInstance) == -1)
						ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");

					m_Roominstance = new RoomManagerBase(this,LuaInstance);
				}
				else
				{
					sc::writeLogInfo("MatchingGameBase : Room_Admin_Script not table : %s");
					return false;
				}
			}

			{
				LuaPlus::LuaObject _MatchingTable( m_script->GetGlobalValue_Lua("Matching_Script") );
				if ( _MatchingTable.IsTable() )
				{
					int scriptid = 0;

					for ( LuaPlus::LuaTableIterator _iterator(_MatchingTable); _iterator; _iterator.Next() )
					{
						std::string _MatchingFileName;
						if ( m_script->GetStringGlobalValueObject_Lua( _iterator.GetValue(), _MatchingFileName ) )
						{
							// 해당 인자값의 Lua 읽어들이기;
							scriptid = pmananger->doFileDirect(_MatchingFileName);

							m_MatchingInstanceID = scriptid;
							PrePareInstance(scriptid);
						}
					}
				}
				else
				{
					sc::writeLogInfo("MatchingGameBase : Matching_Script not table : %s");
					return false;
				}
			}

			// 객체 초기화; (반드시 해야함)
			if ( m_Lobbyinstance )
				m_Lobbyinstance->OnInitialize();

			if ( m_Roominstance )
				m_Roominstance->Initialize();

		}
		else if ( gpAgentServer == NULL )
		{
			LuaPlus::LuaObject _MatchingTable(m_script->GetGlobalValue_Lua("Lobby_Field_Manager"));
			if ( _MatchingTable.IsTable() )
			{
				int scriptid = 0;

				for ( LuaPlus::LuaTableIterator _iterator(_MatchingTable); _iterator; _iterator.Next() )
				{
					std::string _MatchingFileName;
					if ( m_script->GetStringGlobalValueObject_Lua( _iterator.GetValue(), _MatchingFileName ) )
					{
						// 해당 인자값의 Lua 읽어들이기;
						scriptid = pmananger->doFileDirect(_MatchingFileName);
					}
				}

				sc::LuaInstance* LuaInstance  = new sc::LuaInstance();

				if( pmananger->getScriptCOPY((DWORD)scriptid,LuaInstance) == -1)
					ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");

				m_LobbyFieldManagerBase = new LobbyFieldManagerBase(this,LuaInstance);
			}
			else
			{
				return false;;
			}
		}
		else
		{
			ASSERT("MatchingGameBase: Load FAIL");
		}

		

		sc::writeLogInfo("MatchingGameBase: Load Complete");

		return true;
	}

	void MatchingGameBase::ClientMoneySpendCallBack(DWORD ClientDbid,BOOL Ok)
	{
		if(m_BettingManagerbase == NULL)
			return;
		m_Lobbyinstance->ClientMoneySpendCallBack(ClientDbid,Ok);
	}


	void MatchingGameBase::ClientMoneyCheckCallBack(DWORD ClientDbid,BOOL Ok)
	{
		if(m_Lobbyinstance == NULL)
			return;
		m_Lobbyinstance->ClientMoneyCheckCallBack(ClientDbid,Ok);
	}

	void MatchingGameBase::NoticeTournamentinfo(DWORD ClietDBID)
	{
		if(m_Roominstance == NULL)
			return;
		m_Roominstance->NoticeTournamentinfo(ClietDBID);
	}

	void MatchingGameBase::DropOutChar(DWORD ClietDBID)
	{
		if(m_Lobbyinstance == NULL)
			return;

		m_Lobbyinstance->OnDropOutChar(ClietDBID);
	}

	void MatchingGameBase::JoinTournament(DWORD ClietDBID)
	{
		if(m_Lobbyinstance == NULL)
		{
			if(m_LobbyFieldManagerBase != NULL)
			{
				m_LobbyFieldManagerBase->TournamentRequestJoin(ClietDBID);
			}
			else
			{
				GLMSG::NET_MATCHING_ADMISSION_FA _messge(ClietDBID,GetScriptID());
				MatchingCore::getinstance()->sendMessageFieldToAgent(&_messge);
			}
			return;
		}
        m_Lobbyinstance->OnReqJoinMatching(ClietDBID);
	}
	void MatchingGameBase::CancelTournament(DWORD ClietDBID)
	{
		if(m_Lobbyinstance == NULL)
			return;
		m_Lobbyinstance->CancelJoinMatching(ClietDBID);
	}

	void MatchingGameBase::BettingGroupEployeeInfo(DWORD ClietDBID, DWORD dwGroupID)
	{
		std::vector<DWORD> clientlist;
		getGroupManager()->SearchCharacters_MatchingGroupID(clientlist,dwGroupID);
		BOOST_FOREACH(DWORD ClientDBnum,clientlist)
		{
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDBnum));
			if(_pPlayer == NULL)
				continue;
			GLMSG::NET_BETTING_GROUP_EPLOYEE_INFO_AC _message(ClientDBnum,_pPlayer->GetChaLevel(),
				_pPlayer->GetChaName(),_pPlayer->GetClass(),dwGroupID,m_ScriptID
				);
			MatchingCore::getinstance()->_sendMessageToClient(ClietDBID,&_message);
		}
	}

	void MatchingGameBase::OnFrameMove(float fElpaseTime)
	{
		if(m_Roominstance == NULL)
			return;

		if(m_Lobbyinstance == NULL)
			return;

		if(m_Roominstance != NULL)
			m_Roominstance->OnFrameMove(fElpaseTime);

		if(m_Lobbyinstance != NULL)
			m_Lobbyinstance->OnFrameMove(fElpaseTime);

		MatchingInstanceBase* m_MatchingInstance = NULL;

		//한번에 한개씩 지우자!

		DWORD MatchingScriptId = 0;

		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			m_MatchingInstance->OnFrameMove(fElpaseTime);
			if(m_MatchingInstance->CanDestroy())
				MatchingScriptId = m_MatchingInstance->GetMatchingInstanceID();
		}

		DeleteInstance(MatchingScriptId);

		// m_matchingLandlist 는 OnFrameMove 돌지 않고 메시지 기반으로 움직인다.
	}

	DWORD MatchingGameBase::PrePareInstance(DWORD ScriptID)
	{
		ScriptManager* pmananger = MatchingCore::getinstance()->GetScriptManager();

		sc::LuaInstance* pLuaInstance  = new sc::LuaInstance();

		if( pmananger->getScriptCOPY(ScriptID,pLuaInstance) == -1)
			ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");

		DWORD returnvalue = m_UnUseMatchingID.front();
		
		MatchingInstanceBase* m_pClientManager = 
			new MatchingInstanceBase(this,pLuaInstance,ScriptID,returnvalue);
		if ( NULL == m_pClientManager )
			return 0;

		m_UnUseMatchingID.pop_front();


		m_pClientManager->OnInitialize();
		m_pClientManager->OnSetMatchingMode( m_dwMatchingModeID );

		MATCHING_MAP_ITER iter = m_matchingLandlist.find(returnvalue);
		if (iter != m_matchingLandlist.end())
		{
			MatchingInstanceBase* pgroup = iter->second;
			iter = m_matchingLandlist.erase(iter);
			SAFE_DELETE(pgroup);
		}

		m_matchingLandlist.insert(MATCHING_MAP_VALUE(returnvalue, m_pClientManager));
		return returnvalue;
	}

	void MatchingGameBase::DeleteInstance(DWORD _InstanceID)
	{
		MATCHING_MAP_ITER iter = m_matchingLandlist.find(_InstanceID);
		if (iter != m_matchingLandlist.end())
		{
			MatchingInstanceBase* pgroup = iter->second;
			if(pgroup->CanDestroy())
			{
				iter = m_matchingLandlist.erase(iter);
				SAFE_DELETE(pgroup);
				m_UnUseMatchingID.push_back(_InstanceID);
			}
		}
		
	}

	MatchingInstanceBase* MatchingGameBase::getMatchingManager()
	{
		if ( NULL == gpAgentServer )
			return NULL;

		// 다 써버렸으면 새로 하나 만든다;
		if ( m_matchingLandlist.size() == 0 )
		{
			PrePareInstance( m_MatchingInstanceID );
		}

		// 검색해서 안쓰고 있는거 찾는다;
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for ( ; biter != eiter; ++biter )
		{
			MatchingInstanceBase* pTempMatchingInstance = biter->second;

			if ( m_MAX_MATCHING_SIZE == 1 )
				return pTempMatchingInstance;

			if ( pTempMatchingInstance->IsReadyToStart() == true )
			{
				return pTempMatchingInstance;
			}
		}

		// 모두 사용하고 있을 경우 새로 매칭을 하나 만든다;
		MATCHING_MAP_ITER iter = m_matchingLandlist.find(
			PrePareInstance( m_MatchingInstanceID ) );
		if ( iter != m_matchingLandlist.end() )
			return iter->second;

		return NULL;
	}

	bool MatchingGameBase::NoticeMatchedGameFinish
		(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _FactionID, const BOOL _IsWinner)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnNoticeResult(_instanceMapID,_FactionID,_IsWinner))
				return true;
			
		}
		return false;
	}

    bool MatchingGameBase::NoticeMatchedGameCustomMSG(const InstanceSystem::InstanceMapID _instanceMapID, const double _param1, const double _param2, const double _param3, const double _param4)
    {
        MatchingInstanceBase* m_MatchingInstance = NULL;

        // 검색해서 반응 기다리는거 찾는다.
        MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
        MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
        for(; biter != eiter; ++biter)
        {
            m_MatchingInstance = biter->second;
            if(m_MatchingInstance->OnEventCustomMsg(_instanceMapID,_param1, _param2, _param3, _param4))
                return true;

        }
        return false;
    }

	bool MatchingGameBase::NoticeMatchedGameSendStart(const InstanceSystem::InstanceMapID _instanceMapID)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			m_MatchingInstance->OnReceiveDataStart(_instanceMapID);
		}
		return false;
	}

	bool MatchingGameBase::NoticeMatchedGameSendEnd(const InstanceSystem::InstanceMapID _instanceMapID)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReceiveDataEnd(_instanceMapID))
				return true;
		}
		return false;
	}

	bool MatchingGameBase::NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const bool _data)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReceiveData(_instanceMapID,varName,_data))
				return true;
		}
		return false;
	}

	bool MatchingGameBase::NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const char* _data)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReceiveData(_instanceMapID,varName,_data))
				return true;
		}
		return false;
	}

	bool MatchingGameBase::NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const double _data)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReceiveData(_instanceMapID,varName,_data))
				return true;
		}
		return false;
	}

	bool MatchingGameBase::NoticeMatchedGameSendERROR(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD errorCode)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReceiveError(_instanceMapID,errorCode))
				return true;
		}
		return false;
	}

    bool MatchingGameBase::CaptureTheFlag_Result(const GLMSG::NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA& _msg)
    {
        if ( (m_Lobbyinstance == NULL) || (m_Roominstance == NULL) )
            return false;

        std::vector<JoinData*>::iterator biter = m_Roominstance->m_vecInstanceMapids.begin();

        for (;biter != m_Roominstance->m_vecInstanceMapids.end() ; ++biter)
        {
            JoinData* _data = *biter;

            if ( _data->InstanceMapID == _msg.InstanceMapID )
            {   
                sc::LuaInstance* pInstance = m_Lobbyinstance->GetScript();
                if ( pInstance->PushFunctionEx("CaptureTheFlag_Result") == true )
                {
                    lua_State* _pState = pInstance->GetLuaState();
                    
                    const DWORD _redExistPlayerSize = _msg.RedExistPlayer.size();
                    if ( _redExistPlayerSize > 0 )
                    {
                        lua_newtable(_pState);
                        int top = lua_gettop(_pState);

                        for ( unsigned int i = 0; i < _redExistPlayerSize; i++)
                        {
                            lua_pushinteger(_pState, _msg.RedExistPlayer[i]);
                            lua_rawseti(_pState, top, i + 1);
                        }
                    }
                    else
                        lua_pushnil(_pState);

                    const DWORD _blueExistPlayerSize = _msg.BlueExistPlayer.size();
                    if ( _blueExistPlayerSize > 0 )
                    {
                        lua_newtable(_pState);
                        int top = lua_gettop(_pState);

                        for ( unsigned int i = 0; i < _blueExistPlayerSize; i++)
                        {
                            lua_pushinteger(_pState, _msg.BlueExistPlayer[i]);
                            lua_rawseti(_pState, top, i + 1);
                        }
                    }
                    else
                        lua_pushnil(_pState);

                    const DWORD _redRunawayPlayerSize = _msg.RedRunawayPlayer.size();
                    if ( _redRunawayPlayerSize > 0 )
                    {
                        lua_newtable(_pState);
                        int top = lua_gettop(_pState);

                        for ( unsigned int i = 0; i < _redRunawayPlayerSize; i++)
                        {
                            lua_pushinteger(_pState, _msg.RedRunawayPlayer[i]);
                            lua_rawseti(_pState, top, i + 1);
                        }
                    }
                    else
                        lua_pushnil(_pState);

                    const DWORD _blueRunawayPlayerSize = _msg.BlueRunawayPlayer.size();
                    if ( _blueRunawayPlayerSize > 0 )
                    {
                        lua_newtable(_pState);
                        int top = lua_gettop(_pState);

                        for ( unsigned int i = 0; i < _blueRunawayPlayerSize; i++)
                        {
                            lua_pushinteger(_pState, _msg.BlueRunawayPlayer[i]);
                            lua_rawseti(_pState, top, i + 1);
                        }
                    }
                    else
                        lua_pushnil(_pState);

                    lua_pushinteger(_pState, _msg.dwResult);
                    pInstance->CallFunction(5, 0);
                }
                return true;
            }
        }
        return false;
    }

	bool MatchingGameBase::AnswerRUOK(DWORD ClietDBID,DWORD GamescriptID,bool isOk)
	{
		MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
			MatchingSystem::MatchingCallbackTriggerManager::GetInstance();

		MatchingSystem::ReadyTrigger* readytrigger = NULL;

		MatchingSystem::ReadyTriggerVectorIter biter =
			pMCTM->m_ReadyTriggerVector.begin();
		for ( ; biter!= pMCTM->m_ReadyTriggerVector.end(); ++biter )
		{
			readytrigger = *biter;

			if ( readytrigger->_MatchingInstanceid == GamescriptID )
			{
				MatchingSystem::ReadyReceipt receipt;
				receipt.ClietDBID = ClietDBID;
				receipt.isOK = isOk;
				pMCTM->m_ReadyTriggerVector.erase(biter);
				readytrigger->Call(&receipt);
				
				// 사용 한 트리거는 삭제 한다.
				if(readytrigger != NULL)
					SAFE_DELETE(readytrigger);
				break;
			}
		}
		return true;
/*
		MatchingInstanceBase* m_MatchingInstance = NULL;



		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;
			if(m_MatchingInstance->OnReadyAnswer(ClietDBID,GamescriptID,isOk))
				return true;
		}
*/
	}

	void MatchingGameBase::RequireMatchingData(DWORD ClientDBid,DWORD TornanmentID)
	{
		MatchingInstanceBase* m_MatchingInstance = NULL;

		// 검색해서 반응 기다리는거 찾는다.
		MATCHING_MAP_ITER biter = m_matchingLandlist.begin();
		MATCHING_MAP_ITER eiter = m_matchingLandlist.end();
		for(; biter != eiter; ++biter)
		{
			m_MatchingInstance = biter->second;

			//if(m_MatchingInstance->getInstanceID() == GetScriptID())
			{
				m_MatchingInstance->OnUpdateMatchingGroupInfo(ClientDBid);
			}
		}
	}


	
    void MatchingGameBase::RegistFunctionGameBase(sc::LuaInstance* script)
    {
        if ( isUseCompetitionRating() == true )
        {
            script->registdirectFunction("GetRating", *this, &MatchingGameBase::GetRating);
            script->registdirectFunction("SetRating", *this, &MatchingGameBase::SetRating);
            script->registdirectFunction("AddRating", *this, &MatchingGameBase::AddRating);
            script->registdirectFunction("GetWinningStreak", *this, &MatchingGameBase::GetWinningStreak);
            script->registdirectFunction("SetWinningStreak", *this, &MatchingGameBase::SetWinningStreak);
            script->registdirectFunction("GetPlayCount", *this, &MatchingGameBase::GetPlayCount);
            script->registdirectFunction("AddPlayCount", *this, &MatchingGameBase::AddPlayCount);
            script->registFunctionex("GetLastPlayDate", *this, &MatchingGameBase::GetLastPlayDate);
            script->registdirectFunction("GetUserState", *this, &MatchingGameBase::GetUserState);
        }

		// 매칭 시스템 공용 함수 등록;
		registFunction_AgentMatching(*script);

		script->registdirectFunction( "SendToClientMessageBox", *this, &MatchingGameBase::SendToClientMessageBox );
		script->registdirectFunction( "SendToClientMessageBoxALL", *this, &MatchingGameBase::SendToClientMessageBoxALL );
		script->registdirectFunction( "SendToClientMessageBoxGROUP", *this, &MatchingGameBase::SendToClientMessageBoxGROUP );
		script->registdirectFunction( "Serverlog", *this, &MatchingGameBase::Serverlog );

		script->registFunctionex( "SendToClientMessageXML", *this, &MatchingGameBase::SendToClientMessageXML );
		script->registFunctionex( "SendToClientMessageXMLALL", *this, &MatchingGameBase::SendToClientMessageXMLALL );
		script->registFunctionex( "SendToClientMessageXMLGROUP", *this, &MatchingGameBase::SendToClientMessageXMLGROUP );

		if( m_Lobbyinstance != NULL)
		{
			m_Lobbyinstance->RegistCallbackFunction_Lua(script);
		}

		if( m_BettingManagerbase != NULL )
		{
			m_BettingManagerbase->registLuaCallBack(script);
		}
		
    }

    int MatchingGameBase::GetRating(DWORD CharDbNum)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return 0;

        return pInfo->Rating;
    }

    void MatchingGameBase::SetRating(DWORD CharDbNum, int Rating)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return;

        pInfo->Rating = Rating;
    }

    void MatchingGameBase::AddRating(DWORD CharDbNum, int Rating)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return;

        pInfo->Rating += Rating;
    }

    int MatchingGameBase::GetWinningStreak(DWORD CharDbNum)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return 0;

        return pInfo->WinningStreak;
    }

    void MatchingGameBase::SetWinningStreak(DWORD CharDbNum, int WinningStreak)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return;

        pInfo->WinningStreak = WinningStreak;
    }

    int MatchingGameBase::GetPlayCount(DWORD CharDbNum)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return 0;

        return pInfo->PlayCount;
    }

    void MatchingGameBase::AddPlayCount(DWORD CharDbNum)
    {
        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum,getCompetitionContentType());
        if ( pInfo == NULL )
            return;

        pInfo->PlayCount++;
    }

    int MatchingGameBase::GetLastPlayDate(lua_State* pState)
    {
        int currentStack(1);
        DWORD CharDbNum = (DWORD)lua_tonumber(pState, currentStack++);

        CompetitionRatingInfo* pInfo = MatchingCore::getinstance()->getRatingManager()->GetRatingInfo(CharDbNum, getCompetitionContentType());
        if ( pInfo == NULL )
            return 0;

        lua_pushnumber(pState, (lua_Number)pInfo->LastPlayDate);
        return 1;
    }

	void MatchingGameBase::FieldMatchingStatusChaged(DWORD status)
	{
		if(m_LobbyFieldManagerBase != NULL)
		{
			m_LobbyFieldManagerBase->FieldMatchingStatusChaged(status);
		}
	}

	void MatchingGameBase::PlayerPartyChanged(DWORD ClientDbid)
	{
		if(m_Lobbyinstance == NULL)
			return;

		m_Lobbyinstance->OnChangeParty(ClientDbid);
	}
    void MatchingGameBase::EventCustomMessage(DWORD _dwClientDBNum, DWORD _dwParam1, DWORD _dwParam2)
    {
        if(m_Roominstance == NULL)
            return;

        m_Roominstance->EventCustomMessage(_dwClientDBNum,_dwParam1,_dwParam2);
    }

	void MatchingGameBase::SendPostWinMoneyFC( DWORD dwDbNum,DWORD dwKang,LONGLONG winMoney,EM_MATCHING_POST_TYPE emPostType ) 
	{
		if(m_LobbyFieldManagerBase == NULL)
			return;
		m_LobbyFieldManagerBase->SendPostWinMoneyFC(dwDbNum,dwKang,winMoney,emPostType);
	}

	void MatchingGameBase::FieldMatchingObserveOut( const DWORD RequestID, const InstanceSystem::InstanceMapID TornanmentMapID ) 
	{
		m_Roominstance->OutObserve(RequestID,TornanmentMapID);
	}

    int MatchingGameBase::GetUserState(DWORD ClientDbid)
    {
        return (int) MatchingCore::getinstance()->GetJoinUserState(GetScriptID(), ClientDbid);
    }
}