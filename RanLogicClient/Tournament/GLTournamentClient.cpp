#include "pch.h"


#include "../GLGaeaClient.h"
#include "./GLTournamentClient.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../../RanLogicClient/Tournament/MatchingJoinManagerClient.h"

using namespace GLMSG;

namespace MatchingSystem
{
	GLTouranmentClient::GLTouranmentClient(GLGaeaClient* pGaeaClent)
	:	m_pGaeaClient(pGaeaClent)
	,   m_portal(false)
	,   m_Pageupdate(false)
	,   m_Dashboardupdate(false)
	,   m_TOTOupdate(false)
	,   m_MoreInfoupdate(false)
	,	m_nThisStage(0)
	,	m_AskedMatchingInstance(0)
	,	m_peventHandler(0)
	,	m_fElapsedTime(0)
	,	m_dw1FactionScroe(0)
	,	m_dw2FactionScroe(0)
	,	m_GroupInfoOutofDate(false)
	,	m_dwObserveType(MatchingSystem::TOURNAMENT_UI_NON)
	,	m_ClientType(MatchingSystem::TOURNAMENT_OUT)
	,	m_ObserveJoinTime(-1)
	{//
/*
		TournamentSystem::Tournament_UserInfo_BASIC info;
		info.dwLevel = 0;
		info.SetName("Test");
		info.wClass = GLCI_EXTREME_M;

		m_UserInfo.push_back(info);
		m_UserInfo.push_back(info);
		m_UserInfo.push_back(info);
		m_UserInfo.push_back(info);
		m_UserInfo.push_back(info);
		m_UserInfo.push_back(info);
		*/
	}

	GLTouranmentClient::~GLTouranmentClient()
	{
		SAFE_DELETE(m_peventHandler);

		std::vector<MatchingSystem::MatchingClientInfo*>::iterator biter  = m_ClientMatchingStatus.begin();
		
		while (biter != m_ClientMatchingStatus.end() )
		{
			MatchingSystem::MatchingClientInfo* _Node = NULL;
			_Node = *biter;
			biter = m_ClientMatchingStatus.erase(biter);
			SAFE_DELETE(_Node);
		}

		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		while(_bitor != m_VecMatchingInfo.end())
		{
			GLMatchingInfo* pMatchinginfo= NULL;
			pMatchinginfo = *_bitor;
			_bitor = m_VecMatchingInfo.erase(_bitor);
			SAFE_DELETE(pMatchinginfo);
		}

	}

	//
	void GLTouranmentClient::Init()
	{
		//젤먼저 정보 얻어와보자 
		GetTournamentInfomation();
	}

	void GLTouranmentClient::GetTournamentInfomation()
	{
		/*
		GLMatchingInfo* pMatchinginfo;
		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for(;_bitor != _eitor;)
		{
			pMatchinginfo = *_bitor;
			SAFE_DELETE(pMatchinginfo);
			_bitor = m_VecMatchingInfo.erase(_bitor);
		}
		*/
		NET_MATCHING_MESSAGE_INFO_REQ_CA message = NET_MATCHING_MESSAGE_INFO_REQ_CA(m_pGaeaClient->GETMYDBNUM());
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::JoinTournament(DWORD ScriptID)
	{
		GLMatchingInfo* pMatchinginfo= NULL;
		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for(;_bitor != _eitor;++_bitor)
		{
			pMatchinginfo = *_bitor;

			if(pMatchinginfo->m_ScriptID == ScriptID)
			{
				NET_MATCHING_ADMISSION_CF message = NET_MATCHING_ADMISSION_CF(m_pGaeaClient->GETMYDBNUM(),ScriptID);
				m_pGaeaClient->NETSENDTOFIELD(&message);
			}
		}
	}

	void GLTouranmentClient::CanselJoinTournament()
	{
		//Playing 중에는 나갈수가 없다!
		if(m_ClientMatchingStatus_Select.ClientStatus == EMSTATUS_GROUP_IN)
		{
			GLMSG::NET_MATCHING_ADMISSION_CF message = GLMSG::NET_MATCHING_ADMISSION_CF(m_pGaeaClient->GETMYDBNUM(),m_ClientMatchingStatus_Select.ScriptID);
			m_pGaeaClient->NETSENDTOFIELD(&message);
		}	
	}

	void GLTouranmentClient::FrameMove(float fElapsedTime)
	{

		//!! SF_TODO
		/*if(m_pInterfaceBase->UiIsVisibleGroup(COMPETITION_TOURNAMENTENT_INDICATOR))
		{
			m_fElapsedTime -= fElapsedTime;
			int _sec = (int)m_fElapsedTime;

			while(true)
			{
				bool isBlink = false;
				if(_sec < 0)
					break;

				if(_sec < 10)
					isBlink = true;

				if(_sec == 0)
					isBlink = false;

				if(m_pInterfaceBase->GetIndicatorTime() == _sec)
					break;
				else
					m_pInterfaceBase->SetIndicatorTime(_sec,isBlink,true);

				break;
			}
		}*/

		if(m_ObserveJoinTime > 0)
		{
			m_ObserveJoinTime -= fElapsedTime;
		}

		
	}

	void GLTouranmentClient::ObserveTournament(DWORD ScriptID, const CString& _nameCharacterTargetObserve)
	{
		if ( m_ObserveJoinTime >= 0 )
		{
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT",10));
			return;
		}

		// dbNum 혹은 해당 dbNum의 케릭터가 속한 instanceID를 알면 좋으나,
		// 얻어 오기가 힘들다;
		GLMSG::NET_INSTANCE_REQUEST_OBSERVE _requestObserve;
		::StringCchCopy(_requestObserve.namePlayerTargetObserve, CHAR_SZNAME - 1, _nameCharacterTargetObserve.GetString());
		m_pGaeaClient->NETSENDTOAGENT(&_requestObserve);
	}

// 	void GLTouranmentClient::ObserveTournament(DWORD ScriptID,DWORD GroupID)
// 	{
// 		if(m_ObserveJoinTime < 0)
// 		{
// 			GLMatchingInfo* pMatchinginfo= NULL;
// 			VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
// 			VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
// 			for(;_bitor != _eitor;++_bitor)
// 			{
// 				pMatchinginfo = *_bitor;
// 
// 				if(pMatchinginfo->m_ScriptID == ScriptID)
// 				{
// 					NET_MATCHING_OBSERVE_ADMISSION_CA message = NET_MATCHING_OBSERVE_ADMISSION_CA(ScriptID,GroupID,m_pGaeaClient->GETMYDBNUM());
// 					m_pGaeaClient->NETSENDTOAGENT(&message);
// 					m_ObserveJoinTime = 10;
// 					return;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			m_pInterfaceBase->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT",10));
// 		}
// 	}

	void GLTouranmentClient::OutObserveTournament()
	{
		GLCharacter* const _pCharacter(m_pGaeaClient->GetCharacter());
		if ( _pCharacter->IsActState(EM_ACT_OBSERVE) == false )
			return;

		GLLandManClient* const pLand(m_pGaeaClient->GetActiveMap());
		if ( pLand == NULL )
			return;

		const InstanceSystem::InstanceMapID _instanceMapID(pLand->GetGaeaMapID().Mid());

		GLMSG::NET_INSTANCE_REQUEST_OUT _messageRequestOut(_instanceMapID, _pCharacter->CharDbNum());
		m_pGaeaClient->NETSENDTOFIELD(&_messageRequestOut);
		
		return;
	}

	void GLTouranmentClient::AskJoinAnswer(bool _isok)
	{
		GLMSG::NET_MATCHING_NOTIFY_RUOK_CA _CommandMessage(
			m_pGaeaClient->GETMYDBNUM(),
			m_pGaeaClient->GetTournamentClient()->GetAskedMatchingInstance(),
			_isok);
		m_pGaeaClient->NETSENDTOAGENT(&_CommandMessage);
	}

	void GLTouranmentClient::GetMyBettingInformation()
	{
		NET_BETTING_MY_BETTING_INFO_CA message = NET_BETTING_MY_BETTING_INFO_CA(m_pGaeaClient->GETMYDBNUM());
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::GetBettingGroupInfo()
	{
		NET_BETTING_GROUPINFO_CA message = NET_BETTING_GROUPINFO_CA(m_pGaeaClient->GETMYDBNUM());
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::GetBettingInfoEployee(DWORD GroupNum)
	{
		m_MyBettingEployeeInfo.clear();
		NET_BETTING_GROUP_EPLOYEE_INFO_CA message = NET_BETTING_GROUP_EPLOYEE_INFO_CA(m_pGaeaClient->GETMYDBNUM(),GroupNum);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::GetBettingInfo(DWORD LeageId)
	{
		NET_BETTING_BETTINGINFO_CA message = NET_BETTING_BETTINGINFO_CA(m_pGaeaClient->GETMYDBNUM(),LeageId);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::JoinBetting(DWORD bettingGroup,LONGLONG BettingCount,DWORD LeageId)
	{
		NET_BETTING_APPLY_CA message = NET_BETTING_APPLY_CA(m_pGaeaClient->GETMYDBNUM(),bettingGroup,BettingCount,LeageId);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::CanselBetting(DWORD LeageId)
	{
		NET_BETTING_CANSEL_CA message = NET_BETTING_CANSEL_CA(m_pGaeaClient->GETMYDBNUM(),LeageId);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	void GLTouranmentClient::SetBettingMoney(LONGLONG BettingCount)
	{	
		NET_BETTING_APPLY_CA message = NET_BETTING_APPLY_CA(m_pGaeaClient->GETMYDBNUM(),m_bettingGroup,BettingCount,m_SelectScriptID);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}

	MatchingSystem::BettingRatio* GLTouranmentClient::GetBettingTable(DWORD GroupID,DWORD ScriptID)
	{
		std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = m_MyBettingNormalInfo.begin();



		for (; _bitor != m_MyBettingNormalInfo.end(); ++_bitor)
		{
			MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
			_pMatchinginfo = *_bitor;
			if(_pMatchinginfo->ScriptID == ScriptID)
			{

				std::vector<MatchingSystem::BettingRatio>::iterator b_bitor = _pMatchinginfo->m_bettingTable.begin();
				for(;b_bitor != _pMatchinginfo->m_bettingTable.end();b_bitor++)
				{
					MatchingSystem::BettingRatio* bettingtable = &*b_bitor;
					if(bettingtable->bettingGroup == GroupID)
					{
						return bettingtable;
					}
				}
				break;
			}
		}

		return NULL;
	}

	MatchingSystem::GLMatChingGroupInfo* GLTouranmentClient::GetBettingNormalInfo(int _Number,DWORD ScriptID)
	{
		GLMatchingInfo* pMatchinginfo= NULL;
		for(int i = m_VecMatchingInfo.size(); i > 0 ; i--)
		{
			pMatchinginfo = m_VecMatchingInfo[i-1];
			if(pMatchinginfo->m_ScriptID == ScriptID)
			{
				if((int)pMatchinginfo->m_GroupListInfo.size() <= 0 ||  _Number < 0)
					return NULL;

				std::vector<MatchingSystem::GLMatChingGroupInfo>::iterator b_bitor = pMatchinginfo->m_GroupListInfo.begin();
				for(;b_bitor != pMatchinginfo->m_GroupListInfo.end();b_bitor++)
				{
					MatchingSystem::GLMatChingGroupInfo* bettingtable = &*b_bitor;
					if(_Number == bettingtable->Index)
					{
						return bettingtable;
					}
				}
			}			
		}
		return NULL;
		
	}

	MatchingSystem::GLMatChingGroupInfo* GLTouranmentClient::GetGroupInfobyIndex(DWORD index,DWORD ScriptID)
	{

		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLMatchingInfo* pMatchinginfo= NULL;
		for(int i = m_VecMatchingInfo.size(); i > 0 ; i--)
		{
			pMatchinginfo = m_VecMatchingInfo[i-1];
			if(pMatchinginfo->m_ScriptID == ScriptID)
			{
				if((int)pMatchinginfo->m_GroupListInfo.size() <= 0)
					return NULL;


				std::vector<MatchingSystem::GLMatChingGroupInfo>::iterator b_bitor = pMatchinginfo->m_GroupListInfo.begin();
				for(;b_bitor != pMatchinginfo->m_GroupListInfo.end();b_bitor++)
				{
					MatchingSystem::GLMatChingGroupInfo* bettingtable = &*b_bitor;
					if(index == bettingtable->Index)
					{
						return bettingtable;
					}
				}
			}			
		}
		return NULL;
	}

	MatchingSystem::GLMatChingGroupInfo* GLTouranmentClient::GetGroupInfo(DWORD GroupID,DWORD ScriptID)
	{

		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLMatchingInfo* pMatchinginfo= NULL;
		for(int i = m_VecMatchingInfo.size(); i > 0 ; i--)
		{
			pMatchinginfo = m_VecMatchingInfo[i-1];
			if(pMatchinginfo->m_ScriptID == ScriptID)
			{
				if((int)pMatchinginfo->m_GroupListInfo.size() <= 0)
					return NULL;


				std::vector<MatchingSystem::GLMatChingGroupInfo>::iterator b_bitor = pMatchinginfo->m_GroupListInfo.begin();
				for(;b_bitor != pMatchinginfo->m_GroupListInfo.end();b_bitor++)
				{
					MatchingSystem::GLMatChingGroupInfo* bettingtable = &*b_bitor;
					if(GroupID == bettingtable->dwGROUPID)
					{
						return bettingtable;
					}
				}
			}			
		}
		return NULL;
	}

	void GLTouranmentClient::SetInstancePopupEvent(InstanceSystem::InstancePopUpTrigger* eventHandler)
	{
		SAFE_DELETE(m_peventHandler);
		m_peventHandler = eventHandler;
	}

	MatchingSystem::TournamentGameInfo* GLTouranmentClient::GetNowTeamInfo()
	{
		// 토너먼트 정보 를 가져온다.
		std::vector<MatchingSystem::TournamentGameInfo*>::iterator _bitor = m_TournamentInfo.begin();

		// 게임 정보를 찾는다.
		MatchingSystem::TournamentGameInfo* pTeamInfo = NULL;
		for (; _bitor != m_TournamentInfo.end();++_bitor)
		{
			MatchingSystem::TournamentGameInfo* pTeamInfoFind = NULL;
			pTeamInfoFind= *_bitor;
			//찾는 키 정보는 MAP ID이다.
			if(pTeamInfoFind->TornanmentMapID == NowTornanmentMapID)
			{
				pTeamInfo = pTeamInfoFind;
				break;
			}
		}
		return pTeamInfo;
	}

    void GLTouranmentClient::SendCustomMessage(DWORD _dwScriptID, DWORD _dwParam1, DWORD _dwParam2)
    {
        GLMSG::NET_MATCHING_CUSTOM_MSG_CA netMsg(_dwScriptID, _dwParam1, _dwParam2);
        m_pGaeaClient->NETSENDTOAGENT(&netMsg);
    }

	const bool GLTouranmentClient::_messagePackProcedure(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
		GLMSG::NET_MATCHING_MSGPACK_MESSAGE _pMatchingMessage;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _pMatchingMessage) == false)
			return false;

		switch ( _pMatchingMessage.emType )
		{
        case MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_AT_FIRST_AC:
            return GLTouranmentClient::_messageProcedure_Notify_at_first(_pMessage);		
		case MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_START_AC:
			return GLTouranmentClient::_messageProcedure_Matching_Group_Start(_pMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_ADD_AC:
			return GLTouranmentClient::_messageProcedure_Matching_Group_Add(_pMessage);
		case MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Groupinfo(_pMessage);
        case MatchingSystem::EMMESSAGE_TOURNAMENT_INDICATOR_UI_INFO_FC:
            return GLTouranmentClient::_messageProcedure_Tournament_Indicator_UI_Info(_pMessage);
		}
		return false;

	}

	const bool CALLBACK GLTouranmentClient::OnMsgProcedure ( const GLMSG::NET_MATCHING_MESSAGE_HEADER* _pMsg )
	{
		switch ( _pMsg->Type() )
		{
		case NET_MSG_GCTRL_MATCHING_UPDATE_JOIN_COUNT_AC:
			return _MsgUpdateJoinCount( static_cast< const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* >( _pMsg ) );
		}
	}

	const bool CALLBACK GLTouranmentClient::_MsgUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg )
	{
		CMatchingJoinManagerClient::Instance()->OnUpdateJoinCount( _pMsg );

		return true;
	}

	const bool GLTouranmentClient::_messageProcedure(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_MATCHING_MESSAGE* const _pMatchingMessage((const GLMSG::NET_MATCHING_MESSAGE* const)_pMessage);
		switch ( _pMatchingMessage->emType )
		{
		case MatchingSystem::EMMESSAGE_ERROR_AC:
			return GLTouranmentClient::_messageProcedure_error(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_MESSAGEBOX_AC:
			return GLTouranmentClient::_messageProcedure_messagebox(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_INFO_UPDATE_AC:
			return GLTouranmentClient::_messageProcedure_info_update(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_RUOK_AC:
			return GLTouranmentClient::_messageProcedure_join_ask(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_NODE_AC:
			return GLTouranmentClient::_messageProcedure_Matching_Group_info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_MY_BETTING_INFO_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Groupinfo(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_MORE_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Group_More_info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_ANSWER_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Answer(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_BETTINGINFO_AC:
			return GLTouranmentClient::_messageProcedure_Betting_BettingInfo(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_AC:
			return GLTouranmentClient::_messageProcedure_Betting_Group_Eployee_Info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_OBSERVE_ADMISSION_AC:
			return GLTouranmentClient::_messageProcedure_ObserveStart(_pMatchingMessage);

		case MatchingSystem::EMMESSAGE_TOURNAMENT_IN_TOURNAMENT_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_in_Tournament(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_OUT_TOURNAMENT_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_Out_Tournament(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_ROLE_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_GameInformation_Role(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_GameInformation_Team_Info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_WIN_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_Win(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_LOSE_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_Lose(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_TIMELIMIT_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_timelimit(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_FACTION_INFO_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_faction_info(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_POPUP_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_game_popup(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_INFO_CLIENT_AC:
			return GLTouranmentClient::_messageProcedure_Tournament_ClientINFO(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_REFRASH_FIRST_CA:
			return GLTouranmentClient::_messageProcedure_Tournament_Refrash(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_OUTOFDATE_AC:
			return GLTouranmentClient::_messageProcedure_Tournament_Outofdate(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_TOURNAMENT_FIGHT_COUNTING_FC:
			return GLTouranmentClient::_messageProcedure_Tournament_Counting(_pMatchingMessage);
		case MatchingSystem::EMMESSAGE_MATCHING_INFO_UPDATE:
			return GLTouranmentClient::_messageProcedure_InfoUpdate(_pMatchingMessage);
		}
		return false;
	}


	const bool CALLBACK GLTouranmentClient::_messageProcedure_InfoUpdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_INFO_UPDATE* const _pInfoUpdate((const GLMSG::NET_MATCHING_INFO_UPDATE* const)_pMessage);

		const MATCHING_CONTENTS_STATUS _mapStatus(MATCHING_CONTENTS_STATUS(_pInfoUpdate->MapStatus));
		const DWORD _scriptID(_pInfoUpdate->ScriptID);

		GLMatchingInfo* pMatchinginfo(NULL);
		for ( GLTouranmentClient::VEC_MATCHINFO_ITOR _iteratorMatchingInfo(m_VecMatchingInfo.begin()); _iteratorMatchingInfo != m_VecMatchingInfo.end(); ++_iteratorMatchingInfo )
		{
			GLMatchingInfo* pTemp(*_iteratorMatchingInfo);
			if ( pTemp->m_ScriptID != _scriptID )
				continue;

			pMatchinginfo = pTemp;
		}
		if ( pMatchinginfo == NULL )
			return false;

		if ( pMatchinginfo->m_ContentsStatus == _mapStatus )
			return true;

        // 기존에 있던 열려있지 않았던 컨텐츠가 Open되면 UI에 Open상태를 알림창으로 알림;
        if ( _mapStatus == EMMATCHING_CONTENTS_OPENED )
        {
            const char* szName = ID2GAMEINTEXT(pMatchinginfo->m_strContentsSubType.c_str());
			//!! SF_TODO
            //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_scriptID, sc::string::format( ID2GAMEWORD("COMEPTITION_NOTIFY_BUTTON", 0), szName).c_str());
        }
        else
        {
			//!! SF_TODO
            //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_scriptID, NULL);
        }

		if( _mapStatus == EMMATCHING_CONTENTS_UNKWON)
			pMatchinginfo->m_ContentsStatus = EMMATCHING_CONTENTS_CLOSED;
		else
			pMatchinginfo->m_ContentsStatus = _mapStatus;

		NET_MATCHING_GROUPTABLEINFO_REQ_CA message(m_pGaeaClient->GETMYDBNUM(), _scriptID);
		m_pGaeaClient->NETSENDTOAGENT(&message);

		m_Pageupdate = true;
		m_Dashboardupdate = true;
		m_TOTOupdate = true;
		m_MoreInfoupdate = true;		

		return true;
	}
	
	const bool GLTouranmentClient::_messageProcedure_Tournament_Counting(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_FIGHT_COUNTING_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_FIGHT_COUNTING_FC* const)_pMessage);
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
			"-n", int( _pStartMessage->Counting ) );

		return m_GroupInfoOutofDate = true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_Outofdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_GROUPTABLEINFO_OUTOFDATE* const _pStartMessage((const GLMSG::NET_MATCHING_GROUPTABLEINFO_OUTOFDATE* const)_pMessage);
		m_OutofDateID = _pStartMessage->LegeNum;

		return m_GroupInfoOutofDate = true;
	}

	void GLTouranmentClient::UpdateDashboard(DWORD matchingID)
	{
		NET_MATCHING_GROUPTABLEINFO_REQ_CA message = NET_MATCHING_GROUPTABLEINFO_REQ_CA(m_pGaeaClient->GETMYDBNUM(),matchingID);
		m_pGaeaClient->NETSENDTOAGENT(&message);
	}
	const bool GLTouranmentClient::_messageProcedure_Tournament_Refrash(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		
		GLMatchingInfo* pMatchinginfo= NULL;
		
		VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();

		for(int i = m_VecMatchingInfo.size(); i > 0 ; i--)
		{
			pMatchinginfo = *_bitor;
			SAFE_DELETE(pMatchinginfo);
			_bitor = m_VecMatchingInfo.erase(_bitor);
		}

		m_ClientMatchingStatus_Select.ClientStatus = 0;
		m_ClientMatchingStatus_Select.ScriptID = -1;

		m_ClientMatchingStatus_Select.ClientStatus = EMSTATUS_GROUP_OUT;
		//m_ClientMatchingStatus.ScriptID = 0;

		m_GroupInfoOutofDate = true;
		m_Dashboardupdate = true;
	

		GetTournamentInfomation();
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_ClientINFO(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_INFO_USERDATA_AC* const _pStartMessage((const GLMSG::NET_MATCHING_INFO_USERDATA_AC* const)_pMessage);
		m_ClientMatchingStatus_Select.ClientStatus = _pStartMessage->MapStatus;
		m_ClientMatchingStatus_Select.ScriptID = _pStartMessage->ScriptID;
				


        //////////////////////// 기존에 정보가 존재한다면 정보 갱신///////////////////////////////////////
		// 나중에 여러개가 필요할때 아래의 코드를 사용하세요
		std::vector<MatchingSystem::MatchingClientInfo*>::iterator biter  = m_ClientMatchingStatus.begin();
		MatchingSystem::MatchingClientInfo* _Node = NULL;
		for (;biter != m_ClientMatchingStatus.end(); ++biter )
		{
			_Node = *biter;
			if(_Node->ScriptID == _pStartMessage->ScriptID)
			{
                if ( _pStartMessage->MapStatus == MatchingSystem::EMSTATUS_GROUP_IN )
                {
                    if ( _Node->ClientStatus != _pStartMessage->MapStatus )
                    {
                        std::vector<MatchingSystem::GLMatchingInfo*>::iterator _infoIter  = m_VecMatchingInfo.begin();
                        for (;_infoIter != m_VecMatchingInfo.end(); ++_infoIter )
                        {
                            MatchingSystem::GLMatchingInfo* _info = *_infoIter;
                            if ( _info->m_ScriptID == _pStartMessage->ScriptID )
                            {
                                // 신청시 입장대기 상태에 대한 정보를 UI에 알림창으로 띄움;
                                const char* szName = ID2GAMEINTEXT(_info->m_strContentsSubType.c_str());
								//!! SF_TODO
                                //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pStartMessage->ScriptID, sc::string::format( ID2GAMEWORD("COMEPTITION_NOTIFY_BUTTON", 1), szName).c_str());
                                break;
                            }
                        }
                    }
                }
                else
                {
					//!! SF_TODO
                    //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pStartMessage->ScriptID, NULL);
                }
				_Node->ClientStatus = _pStartMessage->MapStatus;
				return false;
			}
		}

        //////////////////////// 기존에 정보가 존재하지 않는다면 새로 만듬 ///////////////////////////////////////
		MatchingSystem::MatchingClientInfo* newMatching = new MatchingSystem::MatchingClientInfo();

        if ( _pStartMessage->MapStatus == MatchingSystem::EMSTATUS_GROUP_IN )
        {
            std::vector<MatchingSystem::GLMatchingInfo*>::iterator _infoIter  = m_VecMatchingInfo.begin();
            for (;_infoIter != m_VecMatchingInfo.end(); ++_infoIter )
            {
                MatchingSystem::GLMatchingInfo* _info = *_infoIter;
                if ( _info->m_ScriptID == _pStartMessage->ScriptID )
                {
                    // 신청시 입장대기 상태에 대한 정보를 UI에 알림창으로 띄움;
                    const char* szName = ID2GAMEINTEXT(_info->m_strContentsSubType.c_str());
					//!! SF_TODO
                    //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pStartMessage->ScriptID, sc::string::format( ID2GAMEWORD("COMEPTITION_NOTIFY_BUTTON", 1), szName).c_str());
                    break;
                }
            }
        }
        else
        {
			//!! SF_TODO
            //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pStartMessage->ScriptID, NULL);
        }
		newMatching->ClientStatus = _pStartMessage->MapStatus;
		newMatching->ScriptID = _pStartMessage->ScriptID;
		m_ClientMatchingStatus.push_back(newMatching);
		return true;
	}

	void GLTouranmentClient::InvokeInstancePopupCloseEvent()
	{
		NET_TOURNAMENT_GAME_POPUP_CF message = NET_TOURNAMENT_GAME_POPUP_CF(m_pGaeaClient->GETMYDBNUM());
		m_pGaeaClient->NETSENDTOFIELD(&message);
	}



	const bool GLTouranmentClient::_messageProcedure_Tournament_game_popup(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAME_POPUP_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAME_POPUP_FC* const)_pMessage);

		const bool _bShow(_pStartMessage->bShow);

		GLCharacter* const _pCharacter(m_pGaeaClient->GetCharacter());
		if ( _bShow == true )
			_pCharacter->SetActState(EM_ACT_OBSERVE);
		else
			_pCharacter->ReSetActState(EM_ACT_OBSERVE);

		InstancePopUpReceipt Receipt;
		Receipt.dwTime = 1;
		Receipt.isRevers = false;
		Receipt.isShow = _pStartMessage->bShow;
		//!! SF_TODO
		//m_pInterfaceBase->SetInstancePopUp(&Receipt);

		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_faction_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAME_FACTION_INFO_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAME_FACTION_INFO_FC* const)_pMessage);

		m_dwObserveType = _pStartMessage->dwViewType;
		m_dw1FactionID = _pStartMessage->dw1Faction;
		m_dw2FactionID = _pStartMessage->dw2Faction;

		m_bTournamentUIview = true;
		TornanmentUIMapID = _pStartMessage->TornanmentMapID;

		//!! SF_TODO
		//m_pInterfaceBase->SetTournamentIndicatorType(m_dwObserveType,m_dw1FactionID,m_dw2FactionID);
		
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_timelimit(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAME_TIMELIMIT_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAME_TIMELIMIT_FC* const)_pMessage);
		//!! SF_TODO
		//m_pInterfaceBase->SetIndicatorTime(_pStartMessage->nSec,_pStartMessage->bBlank,_pStartMessage->bShow);
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_Win(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_WIN_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_WIN_FC* const)_pMessage);
		//m_pInterfaceBase->OpenTournamentWinLose(true);

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
			"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_WIN );

		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_Lose(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC* const)_pMessage);
		//m_pInterfaceBase->OpenTournamentWinLose(false);

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
			"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_LOSE );
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_in_Tournament(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
// 		const GLMSG::NET_TOURNAMENT_IN_TOURNAMENT_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_IN_TOURNAMENT_FC* const)_pMessage);
// 		m_pInterfaceBase->OpenTournamentUI_HUD(true);
// 		m_ClientType = (TOURNAMENT_MY_ROLE_TYPE)_pStartMessage->dwClientType;
// 		NowTornanmentMapID = _pStartMessage->TornanmentMapID;
// 
// 		GLCharacter* const _pChar(m_pGaeaClient->GetCharacter());
// 		if(m_ClientType == MatchingSystem::TOURNAMENT_OBSERVER)
// 		{
// 			//CString strTemp;
// 			//strTemp.Format("YOUR WATCHING OBSERVE (Mapid:%d)", _pStartMessage->TornanmentMapID);
// 			//m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::AQUA, strTemp.GetString());
// 			HideLeftTOPControlGroup();
// 
// 			//메시지 박스 보이게 하기.
// 			InstancePopUpReceipt Receipt;
// 			Receipt.dwTime = 0;
// 			Receipt.isRevers = true;
// 			Receipt.TimerName = "OBSERVE_TIMER_TEXT";
// 			Receipt.isShow = true;
// 			m_pInterfaceBase->SetInstancePopUp(&Receipt);
// 		}
// 
// 
// 
// 
// 		m_dw1FactionID = 0;
// 		m_dw2FactionID = 0;
// 		m_dw1FactionScroe = 0;
// 		m_dw2FactionScroe = 0;
// 		m_bTournamentUIview = false;
// 
// 		m_ObserveJoinTime = -1;

		

		return true;
	}
	void GLTouranmentClient::HideLeftTOPControlGroup()
	{
		//!! SF_TODO
		//UI 다 숨기자
		//m_pInterfaceBase->UiHideGroup(ADMIN_MESSAGE_DISPLAY);
		//m_pInterfaceBase->UiHideGroup(LEFTTOP_CONTROL_GROUP);
		//m_pInterfaceBase->UiHideGroup(BASIC_INFO_VIEW);
		//m_pInterfaceBase->UiHideGroup(QUICK_SKILL_TRAY_TAB_WINDOW);
		//m_pGaeaClient->SetChaseCam(0,true);
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_Out_Tournament(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC* const)_pMessage);
		//!! SF_TODO
		//m_pInterfaceBase->OpenTournamentUI_HUD(false);

		// 토너먼트 정보 를 가져온다.
		std::vector<MatchingSystem::TournamentGameInfo*>::iterator _bitor = m_TournamentInfo.begin();

		// 게임 정보를 찾는다.
		MatchingSystem::TournamentGameInfo* pTeamInfo = NULL;
		for (; _bitor != m_TournamentInfo.end();)
		{
			MatchingSystem::TournamentGameInfo* pTeamInfoFind = NULL;
			pTeamInfoFind= *_bitor;
			//찾는 키 정보는 MAP ID이다.
			if(pTeamInfoFind->TornanmentMapID == _pStartMessage->TornanmentMapID)
			{
				_bitor = m_TournamentInfo.erase(_bitor);
				// 찾았으면 지운다.
				break;
			}
			else
			{
				// 계속 검색때리
				_bitor++;
			}
		}
//UiHideGroup

		if(m_ClientType == MatchingSystem::TOURNAMENT_OBSERVER)
		{
			//!! SF_TODO
			//UI 다 숨기자
			//m_pInterfaceBase->UiShowGroupFocus(ADMIN_MESSAGE_DISPLAY);
			//m_pInterfaceBase->UiShowGroupFocus(LEFTTOP_CONTROL_GROUP);
			//m_pInterfaceBase->UiShowGroupFocus(BASIC_INFO_VIEW);
			//m_pInterfaceBase->UiShowGroupFocus(QUICK_SKILL_TRAY_TAB_WINDOW);
			// m_pGaeaClient->SetChaseCam(0,false);
			m_pGaeaClient->SetChaseCam(STARGETID(CROW_PC, m_pGaeaClient->GetCharacter()->GetGaeaID()), true);
		}

		m_dw1FactionScroe = 0;

		m_dw2FactionScroe = 0;

		//!! SF_TODO
		//m_pInterfaceBase->HideAllTournamentWindow();

		m_ClientType = MatchingSystem::TOURNAMENT_OUT;
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_GameInformation_Role(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC* const)_pMessage);
		
		std::vector<MatchingSystem::TournamentGameInfo*>::iterator _bitor = m_TournamentInfo.begin();

		MatchingSystem::TournamentGameInfo* pTeamInfo = NULL;
		for (; _bitor != m_TournamentInfo.end(); )
		{
			pTeamInfo= *_bitor;
			if(pTeamInfo->TornanmentMapID == _pStartMessage->TornanmentMapID)
			{
				pTeamInfo->GameWinConditionScore = _pStartMessage->GameWinConditionScore;
				pTeamInfo->leftTime = _pStartMessage->leftTime;
				return true;
			}
			else
			{
				_bitor++;
			}
		}

		if(pTeamInfo != NULL)
			return false;

		MatchingSystem::TournamentGameInfo* Enode = new MatchingSystem::TournamentGameInfo();
		Enode->GameWinConditionScore = _pStartMessage->GameWinConditionScore;
		Enode->leftTime = _pStartMessage->leftTime;
		Enode->TornanmentMapID = _pStartMessage->TornanmentMapID;
		m_TournamentInfo.push_back(Enode);
		
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Tournament_GameInformation_Team_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC* const _pStartMessage((const GLMSG::NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC* const)_pMessage);

		std::vector<MatchingSystem::TournamentGameInfo*>::iterator _bitor = m_TournamentInfo.begin();

		if(_pStartMessage->TeamFactionID == 0)
			m_dw1FactionScroe = _pStartMessage->TeamScore;

		if(_pStartMessage->TeamFactionID == 1)
			m_dw2FactionScroe = _pStartMessage->TeamScore;

		MatchingSystem::TournamentGameInfo* pTeamInfo = NULL;
		for (; _bitor != m_TournamentInfo.end();++_bitor )
		{
			MatchingSystem::TournamentGameInfo* ptemp = *_bitor;
			
			if(ptemp->TornanmentMapID == _pStartMessage->TornanmentMapID)
			{
				pTeamInfo= *_bitor;
				break;
			}
		}

		if(pTeamInfo == NULL)
		{
			MatchingSystem::TournamentGameInfo* Enode = new MatchingSystem::TournamentGameInfo();
			Enode->GameWinConditionScore = 1; // 기본값
			Enode->leftTime = 60; // 기본값
			Enode->TornanmentMapID = _pStartMessage->TornanmentMapID;
			m_TournamentInfo.push_back(Enode);

			pTeamInfo = Enode;
		}

		std::vector<MatchingSystem::TournamentTeamInfo*>::iterator _Infobitor  = pTeamInfo->Team.begin();
		MatchingSystem::TournamentTeamInfo* InfoNode = NULL;

		for (; _Infobitor != pTeamInfo->Team.end(); ++_Infobitor)
		{
			MatchingSystem::TournamentTeamInfo* ptemp = *_Infobitor;
			if(ptemp->TeamFactionID == _pStartMessage->TeamFactionID)
			{
				ptemp->TeamScore = _pStartMessage->TeamScore;
				InfoNode = ptemp;
				return true;
			}
		}
		



		if(InfoNode != NULL)
			return true;

		MatchingSystem::TournamentTeamInfo* TeamInfonode 
			= new MatchingSystem::TournamentTeamInfo(_pStartMessage->TeamScore,_pStartMessage->TeamFactionID);
		pTeamInfo->Team.push_back(TeamInfonode);

		return true;

	}


	const bool GLTouranmentClient::_messageProcedure_ObserveStart(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		//const GLMSG::NET_MATCHING_OBSERVE_ADMISSION_AC* const _pStartMessage((const GLMSG::NET_MATCHING_OBSERVE_ADMISSION_AC* const)_pMessage);
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Betting_Group_Eployee_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_BETTING_GROUP_EPLOYEE_INFO_AC* const _pStartMessage((const GLMSG::NET_BETTING_GROUP_EPLOYEE_INFO_AC* const)_pMessage);
		

		std::vector<MatchingSystem::GroupInforEployee*>::iterator _bitor = m_MyBettingEployeeInfo.begin();

		MatchingSystem::GroupInforEployee* pMatchinginfo= NULL;

		for (; _bitor != m_MyBettingEployeeInfo.end(); ++_bitor )
		{
			MatchingSystem::GroupInforEployee* _pMatchinginfo;
			_pMatchinginfo = *_bitor;
			if(_pMatchinginfo->LeageId == _pStartMessage->LeageId)
			{
				if(_pMatchinginfo->GroupID == _pStartMessage->GroupID)
				{
					pMatchinginfo = _pMatchinginfo;
					break;
				}
			}
		}

		if(pMatchinginfo == NULL)
		{
			MatchingSystem::GroupInforEployee* info = new MatchingSystem::GroupInforEployee();
			info->dwLevel = _pStartMessage->dwLevel;
			info->SetName(_pStartMessage->szSetName);
			info->LeageId = _pStartMessage->LeageId;
			info->GroupID = _pStartMessage->GroupID;
			info->wClass = _pStartMessage->wClass;

			m_MyBettingEployeeInfo.push_back(info);
		}
		else
		{
			pMatchinginfo->dwLevel = _pStartMessage->dwLevel;
			pMatchinginfo->SetName(_pStartMessage->szSetName);
			pMatchinginfo->LeageId = _pStartMessage->LeageId;
			pMatchinginfo->GroupID = _pStartMessage->GroupID;
			pMatchinginfo->wClass = _pStartMessage->wClass;
		}


		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_Betting_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_BETTING_MY_BETTING_INFO_AC* const _pStartMessage((const GLMSG::NET_BETTING_MY_BETTING_INFO_AC* const)_pMessage);
		
/*
		std::vector<TournamentSystem::Groupinfo*>::iterator _bitor = m_MyBettingInfo.begin();

		m_TOTOupdate = true;

		TournamentSystem::Groupinfo* pMatchinginfo= NULL;

		for (; _bitor != m_MyBettingInfo.end(); )
		{
			TournamentSystem::Groupinfo* _Matchinginfo;
			_Matchinginfo = *_bitor;

			if(_Matchinginfo->MatchingInstanceID == _pStartMessage->LeageId)
			{
				if(_Matchinginfo->GroupID == _pStartMessage->bettingGroup)
				{
					pMatchinginfo = _Matchinginfo;
					break;
				}
				else
				{
					_bitor++;
				}
			}
			else
			{
				pMatchinginfo = NULL;
				++_bitor;
			}
		}

		if(pMatchinginfo == NULL)
		{
			//TournamentSystem::Groupinfo* _node;
			pMatchinginfo = new TournamentSystem::Groupinfo();
			pMatchinginfo->BettingCount = _pStartMessage->BettingCount;
			pMatchinginfo->GroupID = _pStartMessage->bettingGroup;
			pMatchinginfo->MatchingInstanceID = _pStartMessage->LeageId;

			m_MyBettingInfo.push_back(pMatchinginfo);
		}
		else
		{
			pMatchinginfo->BettingCount = _pStartMessage->BettingCount;
			pMatchinginfo->GroupID = _pStartMessage->bettingGroup;
			pMatchinginfo->MatchingInstanceID = _pStartMessage->LeageId;
		}*/
		return true;
	}


	const bool GLTouranmentClient::_messageProcedure_Betting_Group_More_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_BETTING_GROUPINFO_MORE_AC* const _pStartMessage((const GLMSG::NET_BETTING_GROUPINFO_MORE_AC* const)_pMessage);
/*
		std::vector<TournamentSystem::Groupinfo*>::iterator _bitor = m_MyBettingInfo.begin();

		m_TOTOupdate = true;
		TournamentSystem::Groupinfo* pMatchinginfo = NULL;

		for (; _bitor != m_MyBettingInfo.end(); )
		{
			TournamentSystem::Groupinfo* _Matchinginfo = *_bitor;

			if(_Matchinginfo->MatchingInstanceID == _pStartMessage->LeageId)
			{
				if(_Matchinginfo->GroupID == _pStartMessage->GroupID)
				{
					pMatchinginfo = _Matchinginfo;
					break;
				}
				else
				{
					_bitor++;
				}
			}
			else
			{
				pMatchinginfo = NULL;
				++_bitor;
			}
		}

		if(pMatchinginfo == NULL)
		{

			//TournamentSystem::Groupinfo* _node;
			pMatchinginfo = new TournamentSystem::Groupinfo();
			pMatchinginfo->GroupID = _pStartMessage->GroupID;
			pMatchinginfo->MatchingInstanceID = _pStartMessage->LeageId;
			pMatchinginfo->ScriptID = _pStartMessage->MatchingInstanceNum;
			pMatchinginfo->GroupPopulation = _pStartMessage->Groupsize;
			pMatchinginfo->GroupMaxPopulation = _pStartMessage->GroupMaxSize;
			pMatchinginfo->SetClubName(_pStartMessage->ClubName);
			m_MyBettingInfo.push_back(pMatchinginfo);
		}
		else
		{
			pMatchinginfo->ScriptID = _pStartMessage->MatchingInstanceNum;
			pMatchinginfo->GroupPopulation = _pStartMessage->Groupsize;
			pMatchinginfo->GroupMaxPopulation = _pStartMessage->GroupMaxSize;
			pMatchinginfo->SetClubName(_pStartMessage->ClubName);
			//m_MyBettingInfo.push_back(pMatchinginfo);
		}
*/

		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_Betting_Groupinfo(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
		GLMSG::NET_BETTING_GROUPINFO_AC _pStartMessage;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _pStartMessage) == false)
			return false;


		std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = m_MyBettingNormalInfo.begin();

		MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

		for (; _bitor != m_MyBettingNormalInfo.end(); ++_bitor)
		{
			MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
			_pMatchinginfo = *_bitor;
			if(_pMatchinginfo->ScriptID == _pStartMessage.ScriptID)
			{
				pMatchinginfo = _pMatchinginfo;
				break;
			}
		}

		if(pMatchinginfo == NULL)
		{
			MatchingSystem::Bettinginfo* info = new MatchingSystem::Bettinginfo();
			info->ScriptID = _pStartMessage.ScriptID;
			m_MyBettingNormalInfo.push_back(info);
			pMatchinginfo = info;
		}

		pMatchinginfo->MyBetting.BettingCount = _pStartMessage.BettingCount;
		pMatchinginfo->MyBetting.bettingGroup = _pStartMessage.bettingGroup;
		pMatchinginfo->MyBetting.bettingRatio = _pStartMessage.BettingRato;
		pMatchinginfo->MyBetting.LeageId = _pStartMessage.ScriptID;
		pMatchinginfo->MyBetting.UserDbid = 0;

		

		pMatchinginfo->m_bettingTable.clear();

		if(_pStartMessage.Bettinginfo.size() == 0)
			return true;

		std::copy(_pStartMessage.Bettinginfo.begin(),_pStartMessage.Bettinginfo.end(), std::back_inserter(pMatchinginfo->m_bettingTable));
		m_TOTOupdate = true;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Tournament_BettingGroupInfo, 
		"-dw, -dw, -l, -dw, -f", 
		pMatchinginfo->MyBetting.UserDbid, 
		pMatchinginfo->MyBetting.bettingGroup,
		pMatchinginfo->MyBetting.BettingCount,
		pMatchinginfo->MyBetting.LeageId,
		pMatchinginfo->MyBetting.bettingRatio
		);
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Betting_Answer(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_BETTING_ANSWER_AC* const _pStartMessage((const GLMSG::NET_BETTING_ANSWER_AC* const)_pMessage);


		

		switch(_pStartMessage->emReseon)
		{
		case EM_BETTING_JOINED_GAME:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_JOINED_GAME"));
			break;
		case EM_BETTING_OK:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_OKEM_BETTING_OK"));
			break;
		case EM_BETTING_TWICE:
			{
				CString strRATO;

				strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_MESSAGEBOX_TWICE")); 

				std::string MessageBoxName(strRATO.GetString());

				//!! SF_TODO
				/*m_pInterfaceBase->DoModal(
					MessageBoxName,
					UI::MODAL_INFOMATION,
					UI::OK,
					UI::MODAL_DO_NOTING );*/
			
				this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_TWICE"));
			}
			break;
		case EM_BETTING_NO_MONEY:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_NO_MONEY"));
			break;
		case EM_BETTING_MONEY_LOW:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_MONEY_LOW"));
			break;
		case EM_BETTING_MONEY_HIGH:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_MONEY_HIGH"));
			break;
		case EM_BETTING_OUT_OF_TIME:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_OUT_OF_TIME"));
			break;
		case EM_BETTING_CANCEL:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_CANCEL"));
			break;
		case EM_BETTING_NO_DATA:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_NO_DATA"));
			break;
		case EM_BETTING_BUSY:
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_BUSY"));
			break;
		}
			

		GetBettingGroupInfo();

		//배팅 완료
		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_Betting_BettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_BETTING_BETTINGINFO_AC* const _pStartMessage((const GLMSG::NET_BETTING_BETTINGINFO_AC* const)_pMessage);


		std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = m_MyBettingNormalInfo.begin();

		MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

		for (; _bitor != m_MyBettingNormalInfo.end(); ++_bitor)
		{
			MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
			_pMatchinginfo = *_bitor;
			if(_pMatchinginfo->ScriptID == _pStartMessage->ScriptID)
			{
				pMatchinginfo = _pMatchinginfo;
				break;
			}
		}

		if(pMatchinginfo == NULL)
		{
			MatchingSystem::Bettinginfo* info = new MatchingSystem::Bettinginfo();
			info->MinimumBettingSize = _pStartMessage->MinimumBettingSize;
			info->MaximumBettingSize = _pStartMessage->MaximumBettingSize;
			info->ScriptID = _pStartMessage->ScriptID;

			//info->m_bettingTable

			m_MyBettingNormalInfo.push_back(info);

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Tournament_BettingInfo, 
				"-l, -l, -dw", 
				info->MinimumBettingSize, 
				info->MaximumBettingSize,
				info->ScriptID
				);
		}
		else
		{
			pMatchinginfo->MinimumBettingSize = _pStartMessage->MinimumBettingSize;
			pMatchinginfo->MaximumBettingSize = _pStartMessage->MaximumBettingSize;
			pMatchinginfo->ScriptID = _pStartMessage->ScriptID;

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Tournament_BettingInfo, 
				"-l, -l, -dw", 
				pMatchinginfo->MinimumBettingSize, 
				pMatchinginfo->MaximumBettingSize,
				pMatchinginfo->ScriptID
				);
		}

		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_Matching_Group_Add(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
		GLMSG::NET_MATCHING_GROUPTABLEINFO_ADD _pStartMessage;

		if ( CastNetMsg(_pMessage, _pStartMessage) == false )
			return false;

		GLMatchingInfo* pMatchinginfo= NULL;

		bool isSame = false;
		GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for (; _bitor != _eitor; ++_bitor)
		{
			pMatchinginfo = *_bitor;
			if(pMatchinginfo->m_ScriptID == _pStartMessage.ScriptID)
			{
				pMatchinginfo->m_dwMatchingSize = _pStartMessage.TourNumentSize;
				std::copy(_pStartMessage.matchingInfo.begin(), _pStartMessage.matchingInfo.end(), std::back_inserter(pMatchinginfo->m_GroupListInfo));
			}
		}
		m_Dashboardupdate = true;
		return true;
	}


	const bool GLTouranmentClient::_messageProcedure_Matching_Group_Start(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
		GLMSG::NET_MATCHING_GROUPTABLEINFO_START _pStartMessage;

		if ( CastNetMsg(_pMessage, _pStartMessage) == false )
			return false;

		GLMatchingInfo* pMatchinginfo= NULL;

		bool isSame = false;
		GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for (; _bitor != _eitor; ++_bitor)
		{
			pMatchinginfo = *_bitor;
			if(pMatchinginfo->m_ScriptID == _pStartMessage.ScriptID)
			{
				pMatchinginfo->m_dwMatchingSize = _pStartMessage.TourNumentSize;
				pMatchinginfo->m_dwCurrentRound = _pStartMessage.TourNumCurrentRound;
				pMatchinginfo->m_GroupListInfo.clear();
				std::copy(_pStartMessage.matchingInfo.begin(), _pStartMessage.matchingInfo.end(), std::back_inserter(pMatchinginfo->m_GroupListInfo));
			}
		}
		m_Dashboardupdate = true;
		return true;

	}

	const bool GLTouranmentClient::_messageProcedure_Matching_Group_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_GROUPTABLEINFO_NODE* const _pNodeMessage((const GLMSG::NET_MATCHING_GROUPTABLEINFO_NODE* const)_pMessage);
	/*	GLMatchingInfo* pMatchinginfo= NULL;

		bool isSame = false;
		GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for (; _bitor != _eitor; ++_bitor)
		{
			pMatchinginfo = *_bitor;
			if(pMatchinginfo->m_ScriptID == _pNodeMessage->MatchingInstanceNum)
			{
				/ *
				BOOST_FOREACH( MatchingSystem::GLMatChingGroupInfo _node , pMatchinginfo->m_GroupListInfo )
				{
					if(_node.dwGROUPID == _pNodeMessage->dwGROUPID && _pNodeMessage->Position == _node.Position )
						return false;
				}* /

				GLMatChingGroupInfo data(
					_pNodeMessage->dwGROUPID,
					std::string(_pNodeMessage->GroupName),
					"",
					_pNodeMessage->Position,
					_pNodeMessage->MatchingInstanceNum ,
					_pNodeMessage->LegeNum,
					0,
					0,
					0) ;
				pMatchinginfo->m_GroupListInfo.push_back(data);
			}
		}
*/
		m_Pageupdate = true;
		m_Dashboardupdate = true;
		m_TOTOupdate = true;
		m_MoreInfoupdate = true;

		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_join_ask(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_NOTIFY_RUOK_AC* const _pJoinMessage((const GLMSG::NET_MATCHING_NOTIFY_RUOK_AC* const)_pMessage);
		m_AskedMatchingInstance = _pJoinMessage->dwGameScriptID;

		// 한명이라도 취소한 경우이다;
		if ( _pJoinMessage->bComplete )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MATCHING_CANCEL_JOIN_TEXT" ) );
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Mathing_ConfirmJoin,
				"-b, -n",
				false, 0 );
		}
		// 입장할지말지 질의하는 부분이다;
		else
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Mathing_ConfirmJoin,
				"-b, -n",
				true, static_cast<int>( _pJoinMessage->fTime ) );
		}

		return true;
	}

	const bool GLTouranmentClient::_messageSetPVEStageState(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_STAGE_STATE_FC* const _pJoinMessage((const GLMSG::NET_MATCHING_STAGE_STATE_FC* const)_pMessage);

		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_error(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_ERROR_AC* const _pErrorMessage((const GLMSG::NET_MATCHING_ERROR_AC* const)_pMessage);
		return true;
	}
	
	const bool GLTouranmentClient::_messageProcedure_messagebox(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_NOTIFY_MESSAGEBOX_AC* const _pErrorMessage((const GLMSG::NET_MATCHING_NOTIFY_MESSAGEBOX_AC* const)_pMessage);
		if(_pErrorMessage->isXml)
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, ID2GAMEINTEXT(_pErrorMessage->szCharName));
		else
			this->m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::TOMATO, _pErrorMessage->szCharName);
		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_Notify_at_first(const NET_MSG_GENERIC* const _pMessage)
	{
        GLMSG::NET_MATCHING_NOTIFY_AT_FIRST_AC _pInfoMessage;

        if ( CastNetMsg(_pMessage, _pInfoMessage) == false )
            return false;

		GLMatchingInfo* pMatchinginfo= NULL;

		bool isSame = false;
        bool bChangeOpen = false;
		//머시랏! MAP ID가 같다면 그친구만 바꾸자
		GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_VecMatchingInfo.begin();
		GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_VecMatchingInfo.end();
		for (; _bitor != _eitor; ++_bitor)
		{
			pMatchinginfo = *_bitor;
            if(pMatchinginfo->m_ScriptID == _pInfoMessage.dwScriptID)
			{
				isSame = true;
				break;
			}
		}

		// 새친구다 새로 소개 시켜주자.
		if(isSame == false)
		{
			pMatchinginfo = new GLMatchingInfo();
		}

        pMatchinginfo->m_ScriptID = _pInfoMessage.dwScriptID;
        pMatchinginfo->m_ContentsType = (MATCHING_CONTENTS_TYPE)_pInfoMessage.wContentsType;
        
        if ( isSame && ((MATCHING_CONTENTS_STATUS)_pInfoMessage.wContentsStatus != pMatchinginfo->m_ContentsStatus) )
        {
            // 기존에 있던 열려있지 않았던 컨텐츠가 Open되면 UI에 Open상태를 알림창으로 알림;
            if ( (MATCHING_CONTENTS_STATUS)_pInfoMessage.wContentsStatus == EMMATCHING_CONTENTS_OPENED )
            {
                const char* szName = ID2GAMEINTEXT(_pInfoMessage.strContentsSubType.c_str());
				//!! SF_TODO
                //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pInfoMessage.dwScriptID, sc::string::format( ID2GAMEWORD("COMEPTITION_NOTIFY_BUTTON", 0), szName).c_str());
            }
            else
            {
				//!! SF_TODO
                //m_pInterfaceBase->CompetitionNotifyButtonSetAlarm(_pInfoMessage.dwScriptID, NULL);
            }
        }        

		if( (MATCHING_CONTENTS_STATUS)_pInfoMessage.wContentsStatus == EMMATCHING_CONTENTS_UNKWON)
			pMatchinginfo->m_ContentsStatus = EMMATCHING_CONTENTS_CLOSED;
		else
			pMatchinginfo->m_ContentsStatus = (MATCHING_CONTENTS_STATUS)_pInfoMessage.wContentsStatus;

        pMatchinginfo->m_dwContentsMapID = _pInfoMessage.dwContentsMapID;
        pMatchinginfo->m_strContentsMainType = stringformat( "%1%", _pInfoMessage.strContentsMainType);
        pMatchinginfo->m_strContentsSubType = stringformat( "%1%", _pInfoMessage.strContentsSubType);
        pMatchinginfo->m_strContentsName =  stringformat( "%1%", _pInfoMessage.strContentsName);
        pMatchinginfo->m_strMissionInfo =  stringformat( "%1%", _pInfoMessage.strContentsMissionInfo);
        pMatchinginfo->m_strDetailInfo =  stringformat( "%1%", _pInfoMessage.strContentsDetailInfo);
        pMatchinginfo->m_strRewardInfo =  stringformat( "%1%", _pInfoMessage.strContentsRewardInfo);
        pMatchinginfo->m_ContentsDateType = (MATCHING_TIME_TYPE)_pInfoMessage.wContentsDateType;

        pMatchinginfo->m_vecTimeInfo = _pInfoMessage.vecDateTimeInfo;

        pMatchinginfo->m_bCanObserve = _pInfoMessage.bCanObserve;
        pMatchinginfo->m_bCanTOTO = _pInfoMessage.bCanTOTO;
        pMatchinginfo->m_bEnableInfoButton = _pInfoMessage.bEnableInfoButton;
		pMatchinginfo->m_bJoinInstanceDungeon = _pInfoMessage.bJoinInstanceDungeon;
		pMatchinginfo->m_dwMaxJoinCount = _pInfoMessage.dwMaxJoinCount;

		NET_MATCHING_GROUPTABLEINFO_REQ_CA message = NET_MATCHING_GROUPTABLEINFO_REQ_CA(m_pGaeaClient->GETMYDBNUM(),pMatchinginfo->m_ScriptID);
		m_pGaeaClient->NETSENDTOAGENT(&message);		

		if(isSame == false)
		{
			m_VecMatchingInfo.push_back(pMatchinginfo);
		}

		m_Pageupdate = true;
		m_Dashboardupdate = true;
		m_TOTOupdate = true;
		m_MoreInfoupdate = true;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Competition_UpdatePVP );

		return true;
	}

	const bool GLTouranmentClient::_messageProcedure_info_update(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_INFO_UPDATE_AC* const _pErrorMessage((const GLMSG::NET_MATCHING_INFO_UPDATE_AC* const)_pMessage);

		BOOST_FOREACH(GLMatchingInfo* _info,m_VecMatchingInfo)
		{
			if(_info->m_ScriptID == _pErrorMessage->ScriptID)
			{
				_info->m_ContentsStatus = (MATCHING_CONTENTS_STATUS)_pErrorMessage->MapStatus;
				if(_info->m_ContentsStatus == EMMATCHING_CONTENTS_OPENED)
				{	
					_info->m_GroupListInfo.clear();
				}
				return true;
			}
		}

		//이곳으로 오면 정보가 먼가 어긋났다는 의미다.
		GetTournamentInfomation();
        return false;
	}


	const bool GLTouranmentClient::_messageProcedure_admission(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_ADMISSION_FA* const _pErrorMessage((const GLMSG::NET_MATCHING_ADMISSION_FA* const)_pMessage);
		return false;
	}

	const bool GLTouranmentClient::_messageProcedure_cancel(const NET_MATCHING_MESSAGE* const _pMessage)
	{
		const GLMSG::NET_MATCHING_CANCEL_CA* const _pErrorMessage((const GLMSG::NET_MATCHING_CANCEL_CA* const)_pMessage);
		return false;
	}

    const bool GLTouranmentClient::_messageProcedure_Tournament_Indicator_UI_Info(const NET_MSG_GENERIC* const _pMessage)
    {
        NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
        GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC sNetMsg;
        if( msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), sNetMsg ) == false )
            return false;

		switch( sNetMsg.emInfoType )
		{
		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Time :
			{
				m_fIndicatorTime = sNetMsg.fFloatData;

				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetTime,
					"-f", m_fIndicatorTime );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftName :
			{
				m_strLeftTeamName = sNetMsg.strText;
				m_dwLeftTeamNameColor = sNetMsg.dwFontColor;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetName,
					"-b, -s, -dw", true, m_strLeftTeamName.c_str(), m_dwLeftTeamNameColor );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightName :
			{
				m_strRightTeamName = sNetMsg.strText;
				m_dwRightTeamNameColor = sNetMsg.dwFontColor;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetName,
					"-b, -s, -dw", false, m_strRightTeamName.c_str(), m_dwRightTeamNameColor );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftIcon :
			{
				m_strLeftTeamIconXmlKeyword = sNetMsg.strText;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetIcon,
					"-b, -s", true, m_strLeftTeamIconXmlKeyword.c_str() );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightIcon :
			{
				m_strRightTeamIconXmlKeyword = sNetMsg.strText;
				
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetIcon,
					"-b, -s", false, m_strRightTeamIconXmlKeyword.c_str() );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftScore :
			{
				m_fLeftTeamScore = sNetMsg.fFloatData;
				m_dwLeftTeamScoreColor = sNetMsg.dwFontColor;

				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetScore,
					"-b, -n, -dw", true, (int) m_fLeftTeamScore, m_dwLeftTeamScoreColor );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightScore :
			{
				m_fRightTeamScore = sNetMsg.fFloatData;
				m_dwRightTeamScoreColor = sNetMsg.dwFontColor;

				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetScore,
					"-b, -n, -dw", false, (int) m_fRightTeamScore, m_dwRightTeamScoreColor );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftImage :
			{
				m_strLeftTeamImageXmlKeyword = sNetMsg.strText;
				m_bLeftTeamImageBlink = sNetMsg.bBlink;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetImage,
					"-b, -s, -b",
					true,
					m_strLeftTeamImageXmlKeyword.c_str(),
					m_bLeftTeamImageBlink );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightImage :
			{
				m_strRightTeamImageXmlKeyword = sNetMsg.strText;
				m_bRightTeamImageBlink = sNetMsg.bBlink;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetImage,
					"-b, -s, -b", 
					false, 
					m_strRightTeamImageXmlKeyword.c_str(),
					m_bRightTeamImageBlink );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Object :
			{
				m_strTeamObjectText = sNetMsg.strText;
				m_dwTeamObjectTextColor = sNetMsg.dwFontColor;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_SetObjectText,
					"-s, -dw", m_strTeamObjectText.c_str(), sNetMsg.dwFontColor );
			}
			break;

		case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Show :
			{
				bool bFlag = false;
				if( 1.0f == sNetMsg.fFloatData )
					bFlag = true;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTFlag_Open,
					"-b", bFlag );
			}
			break;
		}

        return true;
    }
}
