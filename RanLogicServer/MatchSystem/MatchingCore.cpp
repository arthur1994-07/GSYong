#include "pch.h"
#include "MatchingManager.h"
#include "./MatchingDefine.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"
#include "../Database/DBAction/DbActionGameCompetition.h"
#include "../Database/DBAction/DbActionGameBetting.h"
#include "../../RanLogicServer/Character/GLChar.h"




namespace MatchingSystem
{
	GLAgentServer* gpAgentServer= NULL;
	GLGaeaServer* gpGaeaServer = NULL;
	MatchingCore* MatchingCore::m_instance;

	MatchingCore::MatchingCore(GLAgentServer* const _pAgentServer)	
		: m_FileName("MatchingScript.lua"),pmananger(NULL)
        , m_pRatingManager(NULL)
		, packetCounter(unsigned int(MatchingSystem::EMMESSAGE_MATCHING_NSIZE))
	{	
		gpGaeaServer= NULL;
		gpAgentServer =_pAgentServer;
		m_instance = this;	

		
	}

	MatchingCore::MatchingCore(GLGaeaServer* const _pGaeaServer)	
		: m_FileName("MatchingScript.lua"),pmananger(NULL)
		, m_pRatingManager(NULL)
		, packetCounter(unsigned int(MatchingSystem::EMMESSAGE_MATCHING_NSIZE))
	{	
		gpAgentServer = NULL;
		gpGaeaServer =_pGaeaServer;
		m_instance = this;	


	}

	MatchingCore::~MatchingCore()
	{

		VEC_P_MATCHING_LAND_LIST_ITOR biter = m_matchingLandlist.begin();
		VEC_P_MATCHING_LAND_LIST_ITOR eiter = m_matchingLandlist.end();

		for(;biter != eiter;++biter)
		{
			MatchingGameBase* land = *biter;
			SAFE_DELETE(land);
		}
		m_matchingLandlist.clear();
		m_ClientParticipate.clear();

		if(pmananger != NULL)
			SAFE_DELETE(pmananger);
		if(m_pRatingManager != NULL)
			SAFE_DELETE(m_pRatingManager);

	}

	bool MatchingCore::OnLoad()
	{
		if ( pmananger != NULL )
			SAFE_DELETE( pmananger );

		if ( GLUseFeatures::GetInstance().IsUsingTournament() == false )
			return true;

		pmananger = new ScriptManager();
		if ( NULL == pmananger )
			return false;

		sc::writeLogInfo( "[ Matching Log ] [ Initialize Matching System ]" );

		// 전장 Script List 읽어오기;
		pmananger->doFile(GetFileName(),"MatchingLuaList");

		// 전장 점수 시즌 읽어오기;
		{
			LuaPlus::LuaObject _SeasonTable(sc::lua_getGlobalFromName("Matching_Content_TypeList"));
			if ( _SeasonTable.IsNil() == true )
			{
				sc::writeLogInfo( sc::string::format(
					"MatchingSystem : ScriptManager Script param dose not Exsist : %s",
					"Matching_Content_TypeList" ) );
			}
			else
			{
				if ( _SeasonTable.IsTable() == false )
				{
					sc::writeLogInfo( sc::string::format(
						"MatchingSystem : Table Not Exsist : %s",
						"Matching_Content_TypeList" ) );
				}
				else
				{
					for ( LuaPlus::LuaTableIterator CompetitionTypeIter( _SeasonTable ); CompetitionTypeIter; CompetitionTypeIter.Next() )
					{
						if ( CompetitionTypeIter.GetValue().IsTable() )
						{
							LuaPlus::LuaTableIterator CompetitionSeasonIter( CompetitionTypeIter.GetValue() );

							std::string _strContentType;
							int _nCurrentSeason = 0;
							int _nBaseRating = 0;
							LuaPlus::LuaObject& _objContentType = CompetitionSeasonIter.GetValue();
							if ( _objContentType.IsString() == false )
								continue;
							_strContentType = _objContentType.GetString();
							CompetitionSeasonIter.Next();

							LuaPlus::LuaObject& _objSeasonNUm = CompetitionSeasonIter.GetValue();
							if ( _objSeasonNUm.IsInteger() == false )
								continue;
							_nCurrentSeason = _objSeasonNUm.GetInteger();
							CompetitionSeasonIter.Next();

							LuaPlus::LuaObject& _objBaseRating = CompetitionSeasonIter.GetValue();
							if ( _objBaseRating.IsInteger() == false )
								continue;
							_nBaseRating = _objBaseRating.GetInteger();
							CompetitionSeasonIter.Next();

							UpdateCompetitionSeason(_strContentType, _nCurrentSeason, _nBaseRating);
						}
					}
				}
			}
		}

		// 읽어온 전장 Script List 파일 읽기;
		int Matchingsize = pmananger->GetScriptFileStackSize();
		sc::writeLogInfo(sc::string::format("MatchingCore: OnLoad Script List Count = %d",Matchingsize));

		// 스크립트 읽어오기.
		for(int i = 0; Matchingsize > i; i++)
		{
			sc::LuaInstance* LuaInstance  = new sc::LuaInstance();

			int scriptNum = pmananger->getScriptCOPY(i,LuaInstance);

			if(scriptNum == -1)
			{
				sc::writeLogError("MatchingCore Script is Not Load : MatchingLuaList -> fils");
				return false;
			}

			MatchingGameBase* land = new MatchingGameBase(LuaInstance,(DWORD)i);
			m_matchingLandlist.push_back(land);
			
			land->OnLoad();
		}

        m_pRatingManager = new CompetitionRatingManager();

		return true;
	}

	bool MatchingCore::OnReload()
	{		

		if ( GLUseFeatures::GetInstance().IsUsingTournament() == false )
			return true;

		

		
		m_ClientParticipate.clear();

		

		/*
		GLGroupSystem::GLGroupManager* groupmanager = GLGroupSystem::GLGroupManager::getInstance();
		groupmanager->ResetGroup();
		*/

		if(pmananger != NULL)
		{
			SAFE_DELETE(pmananger);
		}

		pmananger = new ScriptManager();
		if ( NULL == pmananger )
			return false;

		// 전장 Script List 읽어오기;
		pmananger->doFile(GetFileName(),"MatchingLuaList");

		// 전장 점수 시즌 읽어오기;
		{
			LuaPlus::LuaObject _SeasonTable(sc::lua_getGlobalFromName("Matching_Content_TypeList"));
			if ( _SeasonTable.IsNil() == true )
			{
				sc::writeLogInfo( sc::string::format(
					"MatchingSystem : ScriptManager Script param dose not Exsist : %s",
					"Matching_Content_TypeList" ) );
			}
			else
			{
				if ( _SeasonTable.IsTable() == false )
				{
					sc::writeLogInfo( sc::string::format(
						"MatchingSystem : Table Not Exsist : %s",
						"Matching_Content_TypeList" ) );
				}
				else
				{
					for ( LuaPlus::LuaTableIterator CompetitionTypeIter( _SeasonTable ); CompetitionTypeIter; CompetitionTypeIter.Next() )
					{
						if ( CompetitionTypeIter.GetValue().IsTable() )
						{
							LuaPlus::LuaTableIterator CompetitionSeasonIter( CompetitionTypeIter.GetValue() );

							std::string _strContentType;
							int _nCurrentSeason = 0;
							int _nBaseRating = 0;
							LuaPlus::LuaObject& _objContentType = CompetitionSeasonIter.GetValue();
							if ( _objContentType.IsString() == false )
								continue;
							_strContentType = _objContentType.GetString();
							CompetitionSeasonIter.Next();

							LuaPlus::LuaObject& _objSeasonNUm = CompetitionSeasonIter.GetValue();
							if ( _objSeasonNUm.IsInteger() == false )
								continue;
							_nCurrentSeason = _objSeasonNUm.GetInteger();
							CompetitionSeasonIter.Next();

							LuaPlus::LuaObject& _objBaseRating = CompetitionSeasonIter.GetValue();
							if ( _objBaseRating.IsInteger() == false )
								continue;
							_nBaseRating = _objBaseRating.GetInteger();
							CompetitionSeasonIter.Next();

							UpdateCompetitionSeason(_strContentType, _nCurrentSeason, _nBaseRating);
						}
					}
				}
			}
		}

		// 읽어온 전장 Script List 파일 읽기;
		int Matchingsize = pmananger->GetScriptFileStackSize();

		VEC_P_MATCHING_LAND_LIST_ITOR biter = m_matchingLandlist.begin();
		VEC_P_MATCHING_LAND_LIST_ITOR eiter = m_matchingLandlist.end();

		for(;biter != eiter;++biter)
		{
			MatchingGameBase* land = *biter;
			SAFE_DELETE(land);
		}
		m_matchingLandlist.clear();


		// 스크립트 읽어오기.
		for(int i = 0; Matchingsize > i; i++)
		{
			sc::LuaInstance* LuaInstance  = new sc::LuaInstance();

			int scriptNum = pmananger->getScriptCOPY(i,LuaInstance);

			if(scriptNum == -1)
			{
				sc::writeLogError("MatchingCore Script is Not Load : MatchingLuaList -> fils");
				return false;
			}

			MatchingGameBase* land = new MatchingGameBase(LuaInstance,(DWORD)i);
			m_matchingLandlist.push_back(land);

			land->OnLoad();
		}

        SAFE_DELETE(m_pRatingManager);
        m_pRatingManager = new CompetitionRatingManager();

		if ( gpAgentServer )
		{
			GLMSG::NET_MATCHING_REFRASH_FIRST_CA _MessageBox;
			gpAgentServer->SENDTOALLCLIENT(&_MessageBox);
		}
		
		///MatchingCore::getinstance()->_sendMessageToClient(dbnum,&_MessageBox);


		return true;
	}


	bool MatchingCore::OnReload(const DWORD MatchNum) // 특정 매칭 스크립트 재설정;
	{
		OutMatchingSystem(MatchNum);
		sc::LuaInstance* LuaInstance  = new sc::LuaInstance();

		int scriptNum = pmananger->getScriptCOPY(MatchNum,LuaInstance);

		if(scriptNum == -1)
		{
			sc::writeLogError("MatchingCore Script is Not Load : MatchingLuaList -> fils");
			return false;
		}
		
		MatchingGameBase* land = new MatchingGameBase(LuaInstance, MatchNum);
		MatchingGameBase * oldGame = m_matchingLandlist[MatchNum];
		SAFE_DELETE(oldGame);
		m_matchingLandlist[MatchNum] = land;
		land->OnLoad();
		return true;
	}


	void MatchingCore::OnFrameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime)
	{

		if ( GLUseFeatures::GetInstance().IsUsingTournament() == false )
			return ;

		if(m_MatchingCopyCommand.size() != 0)
		{
			VEC_P_COPYCOMMAND_LIST_ITOR biter = m_MatchingCopyCommand.begin();
			VEC_P_COPYCOMMAND_LIST_ITOR eiter = m_MatchingCopyCommand.end();

			for (;biter!=eiter ; ++biter)
			{

				CopyTrigger* rvalue = *biter;
				
				sc::LuaInstance* _LuaInstance  = new sc::LuaInstance();


				if( pmananger->getScriptCOPY(rvalue->ScriptID,_LuaInstance) == -1 )
				{
					ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");
					return ;
				}

				CopyScript pCopy;
				pCopy.Resualt = _LuaInstance;

				rvalue->Call(&pCopy);

				m_MatchingCopyCommand.erase(biter);
				SAFE_DELETE(rvalue);
				return;
			}
		}

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->OnFrameMove(fElapsedTime);
		}

		// 인스턴스 매니저 업데이트;
		//m_pInstanceManager->frameMove(fTime, fElapsedTime, CurrentTime);
		
	}

	bool MatchingCore::SetcopyScript(CopyTrigger* rvalue)
	{
 		m_MatchingCopyCommand.push_back(rvalue);
		return true;
	}

	sc::LuaInstance* MatchingCore::SetcopyScript(DWORD id)
	{
		sc::LuaInstance* _LuaInstance  = new sc::LuaInstance();

		if( pmananger->getScriptCOPY(id,_LuaInstance) == -1)
			ASSERT(L"MatchingCore Script is Not Load : Check MatchingCore");

		return _LuaInstance;
	}

	bool MatchingCore::MatchingInfoamtionSend(DWORD ClietDBID)
	{
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->NoticeTournamentinfo(ClietDBID);
			SendClientUserMatchingInfo(ClietDBID,land->GetScriptID());
		}


		return true;
	}

	void MatchingCore::SendClientUserMatchingInfo(DWORD ClientDBID,DWORD ScriptID)
	{
		
		EMCUSTOMER_STATUS_TYPE Status = EMCUSTOMER_STATUS_TYPE(EMSTATUS_GROUP_OUT);

		ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDBID);
		if (iter != m_ClientParticipate.end())
		{
			TOURNAMENTID_VEC &MatchVec = iter->second;

			BOOST_FOREACH( CLINET_TOURNAMENT_INFO& _info , MatchVec )
			{
				if( _info.dwTournamentID == ScriptID)
				{
                    Status = (EMCUSTOMER_STATUS_TYPE)_info.emTournamentState;
					break;
				}
			}
			
		}
		else
		{
			Status = EMSTATUS_GROUP_OUT;
		}
		GLMSG::NET_MATCHING_INFO_USERDATA_AC _Message(ScriptID,Status);

		MatchingCore::getinstance()->_sendMessageToClient(ClientDBID,&_Message);
	}


	void MatchingCore::UserMatchingStatusUpdate(DWORD ClientDBID, DWORD ScriptID, EMCUSTOMER_STATUS_TYPE GameStatus)
	{
		
		GLMSG::NET_MATCHING_INFO_USERDATA_AC _Message(ScriptID,(DWORD)GameStatus);

		ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDBID);
		if (iter != m_ClientParticipate.end())
		{
			TOURNAMENTID_VEC &MatchVec = iter->second;
			BOOST_FOREACH( CLINET_TOURNAMENT_INFO& _info , MatchVec )
			{
				if( _info.dwTournamentID == ScriptID)
				{
                    _info.emTournamentState = (DWORD)GameStatus;
					break;
				}
			}
		}
		else
		{
			GameStatus = EMSTATUS_GROUP_OUT;
		}

		_Message.MapStatus = (DWORD)GameStatus;

		MatchingCore::getinstance()->_sendMessageToClient(ClientDBID,&_Message);
		//GameStatus
	}

	void MatchingCore::DropOutChar(DWORD ClietDBID)
	{
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->DropOutChar(ClietDBID);
		}

		//UserMatchingStatusUpdate(ClietDBID,ScriptID,EMCUSTOMER_STATUS_TYPE::EMSTATUS_GROUP_OUT);

		return;
	}

	// 메칭 시스템에 클라이언트 입장
	bool MatchingCore::JoinMatchingSystem(DWORD MatchNum,DWORD ClientDbid)
	{
		ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDbid);
		if (iter != m_ClientParticipate.end())
		{
			bool isfind = false;

			TOURNAMENTID_VEC &MatchVec = iter->second;
			TOURNAMENTID_VEC_ITER biter = MatchVec.begin();
			for(;biter != MatchVec.end();)
			{
				if((*biter).dwTournamentID == MatchNum)
				{
					isfind = true;
					break;
				}
				else
				{
					biter++;
				}
			}

			if( isfind == false)
            {
                CLINET_TOURNAMENT_INFO _info;
                _info.dwTournamentID = MatchNum;
                _info.emTournamentState = (DWORD)EMSTATUS_GROUP_IN;
				MatchVec.push_back(_info);
            }
		}
		else
		{
			TOURNAMENTID_VEC MatchVec;
            CLINET_TOURNAMENT_INFO _info;
            _info.dwTournamentID = MatchNum;
            _info.emTournamentState = (DWORD)EMSTATUS_GROUP_IN;
			MatchVec.push_back(_info);
			m_ClientParticipate.insert(ClientCoreMAP_VALUE(ClientDbid,MatchVec));
		}

		//메시지 던지자.
		MatchingInfoamtionSend(ClientDbid);

		UserMatchingStatusUpdate(ClientDbid,MatchNum,EMSTATUS_GROUP_IN);
		return true;
	}

	bool MatchingCore::OutMatchingSystem(DWORD MatchNum)
	{
		ClientCoreMAP_ITER iter = m_ClientParticipate.begin();
		for (;iter != m_ClientParticipate.end();)
		{
			TOURNAMENTID_VEC &MatchVec = iter->second;
			DWORD ClientDbid = iter->first;
			TOURNAMENTID_VEC_ITER biter = MatchVec.begin();
			for(;biter != MatchVec.end();)
			{
				if((*biter).dwTournamentID == MatchNum)
				{
					biter = MatchVec.erase(biter);
					UserMatchingStatusUpdate(ClientDbid,MatchNum,EMSTATUS_GROUP_OUT);
				}
				else
				{
					biter++;
				}
			}
			iter++;
		}

		{
			ClientCoreMAP_ITER iterMap = m_ClientParticipate.begin();
			for (;iterMap != m_ClientParticipate.end();)
			{
				TOURNAMENTID_VEC &MatchVec = iterMap->second;
				if(MatchVec.size() == 0)
					iterMap = m_ClientParticipate.erase(iterMap);
				else
					iterMap++;
			}
		}

		

		//MatchingInfoamtionSend(ClientDbid);

		return true;
	}

	bool MatchingCore::OutMatchingSystem(DWORD MatchNum,DWORD ClientDbid)
	{
		ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDbid);
		if (iter != m_ClientParticipate.end())
		{
			TOURNAMENTID_VEC &MatchVec = iter->second;
			TOURNAMENTID_VEC_ITER biter = MatchVec.begin();
			for(;biter != MatchVec.end();)
			{
				if((*biter).dwTournamentID == MatchNum)
				{
					biter = MatchVec.erase(biter);
				}
				else
				{
					biter++;
				}
			}
			
		}

		{
			ClientCoreMAP_ITER iterMap = m_ClientParticipate.begin();
			for (;iterMap != m_ClientParticipate.end();)
			{
				TOURNAMENTID_VEC &MatchVec = iterMap->second;
				if(MatchVec.size() == 0)
					iterMap = m_ClientParticipate.erase(iterMap);
				else
					iterMap++;
			}
		}

		UserMatchingStatusUpdate(ClientDbid,MatchNum,EMSTATUS_GROUP_OUT);

		MatchingInfoamtionSend(ClientDbid);

		return true;
	}

	bool MatchingCore::IsAreadyJoin(DWORD ClientDbid)
	{
		ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDbid);
		if (iter != m_ClientParticipate.end())
		{
			TOURNAMENTID_VEC &MatchVec = iter->second;
			if(MatchVec.size() == 0)
				return false;
			return true;
		}
		return false;
	}

    EMCUSTOMER_STATUS_TYPE MatchingCore::GetJoinUserState(DWORD MatchNum, DWORD ClientDbid)
    {
        ClientCoreMAP_ITER iter = m_ClientParticipate.find(ClientDbid);
        if (iter != m_ClientParticipate.end())
        {
            TOURNAMENTID_VEC &MatchVec = iter->second;
            TOURNAMENTID_VEC::iterator _matchIter = MatchVec.begin();
            for ( ; _matchIter != MatchVec.end(); )
            {
                if ( _matchIter->dwTournamentID == MatchNum )
                {
                    return (EMCUSTOMER_STATUS_TYPE)_matchIter->emTournamentState;
                }
                else
                {
                    _matchIter++;
                }
            }
        }
        return EMSTATUS_GROUP_OUT;
    }

	void MatchingCore::_sendMessageToClient(const DWORD _dbNum, NET_MSG_GENERIC* _pMessage)
	{

		if(gpAgentServer== NULL)
		{
			if(gpGaeaServer== NULL)
			{
				GLChar* const _pPlayer(gpGaeaServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
					return
				gpGaeaServer->SENDTOCLIENT(_pPlayer->ClientSlot(),_pMessage);
				return;
			}
			return;
		}
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(), _pMessage);
	}

	void MatchingCore::sendMessageFieldToAgent( NET_MSG_GENERIC* _pMessage )
	{
		if(gpGaeaServer== NULL)
		{
			ASSERT("M002 :This is Not Field Server!");
			return;
		}

		gpGaeaServer->SENDTOAGENT(_pMessage);
	}

	void MatchingCore::_sendMessageToALLClient( NET_MSG_GENERIC* _pMessage)
	{
		if(gpAgentServer== NULL)
		{
			ASSERT("M001 :This is Not Agent Server!");
			return;
		}

		gpAgentServer->SENDTOALLCLIENT(_pMessage);
	}

	void MatchingCore::_sendMessageAgentToALLField( NET_MSG_GENERIC* _MessageBox )
	{
		gpAgentServer->SENDTOALLCHANNEL(_MessageBox);
	}

    void MatchingCore::UpdateCompetitionSeason(const std::string& _strContentType, const int _nSeason, const int _nBaseRating)
    {

        if ( gpAgentServer )
        {
            gpAgentServer->AddGameAdoJob( db::DbActionPtr(
                new db::DbAction_CompetitionSeasonUpdate( _strContentType, _nSeason, _nBaseRating )) );
        }
    }

	

	void MatchingCore::SetPastBettingUser( DWORD outChaDbNum, LONGLONG outBettingCount ) 
	{
		GLMSG::NET_BETTING_MONEY_OK_RESTORE_AF ANswer(outChaDbNum,outBettingCount);
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(outChaDbNum));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOFIELD(_pPlayer->GetChannel(),_pPlayer->GetCurrentFieldServer(), &ANswer);
	}

	void MatchingCore::GetPastBettingUser( DWORD ChaDbNum , DWORD BettingStatus )
	{
		if ( gpAgentServer )
		{
			gpAgentServer->AddGameAdoJob( db::DbActionPtr(
				new db::BettingSelectAction( ChaDbNum, BettingStatus )) );
		}
	}

	const bool MatchingCore::_messageRestoreBetting( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID )
	{
		const GLMSG::NET_BETTING_MONEY_OK_RESTORE_FA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_RESTORE_FA* const)_pMessage);
		if ( gpAgentServer )
		{
			gpAgentServer->AddGameAdoJob( db::DbActionPtr(
				new db::BettingDeleteAction( _pMatchedGameMessage->dwDbNum )) );
		}
		return true;
	}
    
    

	void MatchingCore::OnForceStart( DWORD Param1 )
	{
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if (land->GetScriptID() == Param1)
			{
				land->getMatchingManager()->StartMatching();
				return;
			}
		}
	}

	void MatchingCore::PartyMemberChaged(DWORD dwGaeaID,DWORD ParyID )
	{
		if(gpAgentServer == NULL)
			return;
		DWORD dwDbnum = -1;
		GLCharAG* _char = gpAgentServer->GetCharByGaeaID(dwGaeaID);
		if(_char != NULL)
			dwDbnum = _char->ChaDbNum();

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(!land->getLobbyManager()->IsJoinGame(dwDbnum))
				continue;

			land->getLobbyManager()->OnChangePartyMember(dwDbnum,ParyID);

		}
	}

	void MatchingCore::PartyMemberRemoved(DWORD dwGaeaID,DWORD ParyID)
	{
		if(gpAgentServer == NULL)
			return;
		DWORD dwDbnum = -1;
		GLCharAG* _char = gpAgentServer->GetCharByGaeaID(dwGaeaID);
		if(_char != NULL)
			dwDbnum = _char->ChaDbNum();

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(!land->getLobbyManager()->IsJoinGame(dwDbnum))
				continue;

			land->getLobbyManager()->OnDropOutPartyMember(dwDbnum,ParyID);

		}
	}

	void MatchingCore::PartyMasterChaged(DWORD olddwGaeaID,DWORD NewdwGaeaID,DWORD ParyID)
	{
		if(gpAgentServer == NULL)
			return;
		DWORD olddwDbnum = -1;
		GLCharAG* _oldchar = gpAgentServer->GetCharByGaeaID(olddwGaeaID);
		if(_oldchar != NULL)
			olddwDbnum = _oldchar->ChaDbNum();

		DWORD newDbnum = -1;
		GLCharAG* _newchar = gpAgentServer->GetCharByGaeaID(NewdwGaeaID);
		if(_newchar != NULL)
			newDbnum = _newchar->ChaDbNum();

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(!land->getLobbyManager()->IsJoinGame(olddwDbnum))
				continue;

			land->getLobbyManager()->OnChangePartyMaster(olddwDbnum,newDbnum,ParyID);

		}
	}

	void MatchingCore::PartyDestroyed(DWORD ParyID)
	{
		//DO Nothing

		//PartyMemberRemoved를 모든 멤버에대해 호출 해주기 때문에 따로 필요가 없다.
		//특이하게 필요하다면 아래에 적당한 코드를 작성하세요
// 		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
// 		{
// 			land->getLobbyManager()->PartyDestroyed(ParyID)
// 		}
	}

	void MatchingCore::ForcCancel(DWORD dwDBNum )
	{
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->CancelTournament(dwDBNum);
		}
	}
}

