#include "pch.h"
#include "GLTournamentBetting.h"
#include "../../../RanLogic/Msg/GLContrlMatching.h"
#include "../../Character/GLCharAG.h"
#include "../MatchingManager.h"
#include "../../AgentServer/GLAgentServer.h"
#include "../../Database/DBAction/DbActionGameBetting.h"
#include "../../InstanceSystem/LogicAgent/InvokeFunctionAgent.h"

#include "../../Database/DBAction/DbActionLogInstantDungeon.h"

#define MATCHINGPROPERTY_REGISTER( REGESTKEY )								\
	if ( REGESTKEY.IsNil() == true )										\
	{																		\
		sc::writeLogError("RoomManagerBase : ##REGESTKEY is nil");			\
		return;																\
	}		
namespace MatchingSystem
{

	GLTournamentBetting::GLTournamentBetting( MatchingGameBase* parent,sc::LuaInstance* script )
		:m_parent(parent),m_script(script),Winner_Bet_Money_Min(100000),Winner_Bet_Money_Max(10000000)
		,Winner_Bet_Money_Type(0),Winner_forecast_fee(0.1f),IsBettingTime(false)
	{

		// 인던 시스템 함수 등록;
		InstanceSystem::registFunctionCommon(*m_script);
		InstanceSystem::registFunctionAgent(*m_script);

		//변수 찾기
		LuaPlus::LuaObject Winner_Bet_Money_Min = m_script->GetGlobalValue_Lua("Winner_Bet_Money_Min");
		LuaPlus::LuaObject Winner_Bet_Money_Max = m_script->GetGlobalValue_Lua("Winner_Bet_Money_Max");
		LuaPlus::LuaObject Winner_Bet_Money_Type = m_script->GetGlobalValue_Lua("Winner_Bet_Money_Type");
		LuaPlus::LuaObject Winner_forecast_fee = m_script->GetGlobalValue_Lua("Winner_forecast_fee");

		MATCHINGPROPERTY_REGISTER(Winner_Bet_Money_Min);
		MATCHINGPROPERTY_REGISTER(Winner_Bet_Money_Max);
		MATCHINGPROPERTY_REGISTER(Winner_Bet_Money_Type);
		MATCHINGPROPERTY_REGISTER(Winner_forecast_fee);

		this->Winner_Bet_Money_Min=Winner_Bet_Money_Min.GetInteger64();
		this->Winner_Bet_Money_Max=Winner_Bet_Money_Max.GetInteger64();
		this->Winner_Bet_Money_Type=Winner_Bet_Money_Type.GetInteger();
		this->Winner_forecast_fee=Winner_forecast_fee.GetFloat();

		parent->RegistFunctionGameBase(m_script);

	}

	GLTournamentBetting::~GLTournamentBetting()
	{
		m_script->OnDestroy();
		SAFE_DELETE(m_script);
	}

	bool GLTournamentBetting::CanselBetting(DWORD ClientDbid)
	{
        {
            MATCHING_CONTENTS_STATUS status = m_parent->getRoomManager()->GetStatus();

            MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

            // 배팅 신청기간이 아닐 때에는 배팅 취소도 불가능 함;
            if( IsBettingTime == false || status == EMMATCHING_CONTENTS_CLOSED || status == EMMATCHING_CONTENTS_UNKWON)
            {
                GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_OUT_OF_TIME);
                GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
                if ( _pPlayer == 0 )
                    return false;
                gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
                return false;
            }
        }

		{
			std::vector<BettingTable>::iterator _bitor = m_BettingApointTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingApointTable.end();
			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{

					// 이미 신청중
					GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_BUSY);
					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return false;
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
					return false;					
				}
			}
		}


		{
			std::vector<BettingTable>::iterator _bitor = m_BettingTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingTable.end();
			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{
					m_BettingTable.erase(_bitor);
					GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_CANCEL);
					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return false;
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
					MatchingCore::getinstance()->SetPastBettingUser(temp.UserDbid,temp.BettingCount);

					//스크립트에 등록
					if ( m_script->PushFunctionEx("CanselBetting") == true )
					{
						m_script->PushInteger(ClientDbid);
						m_script->PushInteger(temp.bettingGroup);
						m_script->PushInteger((int)temp.BettingCount);
						m_script->CallFunction(3, 0);
					}

					RequestGroupInfo(ClientDbid);

					return true;
				}
			}
		}
		
		return false;		
	}

	void GLTournamentBetting::SetBetting(DWORD ClientDbid,DWORD GroupId, LONGLONG BettingMoney, DWORD LeageID)
	{
		if(m_parent->GetScriptID() != LeageID)
		{
			return;
		}

		std::vector<DWORD> clientlist;
		DWORD instID = m_parent->GetScriptID();

		MATCHING_CONTENTS_STATUS status = m_parent->getRoomManager()->GetStatus();

		MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

		if( IsBettingTime == false || status == EMMATCHING_CONTENTS_CLOSED || status == EMMATCHING_CONTENTS_UNKWON)
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_OUT_OF_TIME);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}


		if( groupmanager->IsRegistCharacter(ClientDbid))
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_JOINED_GAME);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}


		if(BettingMoney <= 0)
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_MONEY_LOW);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}

		if(BettingMoney < Winner_Bet_Money_Min)
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_MONEY_LOW);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}

		if(BettingMoney > Winner_Bet_Money_Max)
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_MONEY_HIGH);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}

		
		{
			std::vector<BettingTable>::iterator _bitor = m_BettingTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingTable.end();
			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{
					// 이미 신청중
					GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_TWICE);
					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return;
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
					return;
				}
			}
		}

		{
			std::vector<BettingTable>::iterator _bitor = m_BettingApointTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingApointTable.end();
			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{
					// 이미 신청중
					GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_BUSY);
					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return;
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
					return;
				}
			}
		}



		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time 가져오기
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time 가져오기

		//	<--0	TimeZone 가져오기
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone 가져오기

		//	<--0	Universal Time과 TimeZone의 합성
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time과 TimeZone의 합성

		BettingTable bettingTable;
		bettingTable.BettingCount = BettingMoney;
		bettingTable.LeageId  = LeageID;
		bettingTable.UserDbid = ClientDbid;
		bettingTable.bettingGroup = GroupId;
		
		bettingTable.bettingRatio = 0;

		m_BettingApointTable.push_back(bettingTable);

		GLMSG::NET_BETTING_MONEY_OK_SPEND_AF ANswer(ClientDbid,BettingMoney);
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOFIELD(_pPlayer->GetChannel(),_pPlayer->GetCurrentFieldServer(), &ANswer);


		DWORD dwTotoTargetChaNum = 0;
		if( groupmanager )
		{
			std::vector<DWORD> groupMember;
			groupmanager->SearchCharacters_MatchingGroupID(groupMember, GroupId);
			
			if( groupMember.size() == 1 )
				dwTotoTargetChaNum = groupMember[0];
		}


		// 토너먼트 로그
		db::LogTournament* pDbAction = new db::LogTournament(9101);

		pDbAction->dwCharDbNum =  ClientDbid;   
		pDbAction->strChaName = _pPlayer->GetChaName();
		pDbAction->dwFactionID =  GroupId;   
		pDbAction->nMoney =  BettingMoney;   		
		pDbAction->dwTotoTargetChaNum = dwTotoTargetChaNum;

		gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
	}

	void GLTournamentBetting::FieldSpendMoneyOK(DWORD ClientDbid,BOOL Isok)
	{
		if(Isok == FALSE)
		{
			GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_NO_MONEY);
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
			if ( _pPlayer == 0 )
				return;
			gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);
			return;
		}
		else
		{
			std::vector<BettingTable>::iterator _bitor = m_BettingApointTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingApointTable.end();

			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{
					
					
					//스크립트에 등록
					if ( m_script->PushFunctionEx("SetBetting") == true )
					{
						m_script->PushInteger(ClientDbid);
						m_script->PushInteger(temp.bettingGroup);
						m_script->PushInteger((int)temp.BettingCount);
						m_script->CallFunction(3, 0);
					}

					

					float bettingRatio = CarcurateBettingRatio(temp.bettingGroup);
					temp.bettingRatio = bettingRatio;

					m_BettingApointTable.erase(_bitor);
					m_BettingTable.push_back(temp);

					bool isFind = false;
					BETTINGINFO_ITER betting_bior = m_BettingRatioTable.begin();
					for(;betting_bior != m_BettingRatioTable.end(); ++betting_bior)
					{
						BettingRatio* bet = &*betting_bior;
						if(bet->bettingGroup == temp.bettingGroup)
						{
							bet->fBettingRatio = bettingRatio;
							isFind = true;
							break;
						}
					}

					if(!isFind)
					{
						BettingRatio bet;
						bet.bettingGroup = temp.bettingGroup;
						bet.fBettingRatio = bettingRatio;
						m_BettingRatioTable.push_back(bet);
					}

					// 클라에게 메시지 전달

					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return;

					// 신청된거 알리고.
					GLMSG::NET_BETTING_ANSWER_AC ANswer(ClientDbid,EM_BETTING_OK);
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&ANswer);

					// 신청된 정보 알려줌.
					GLMSG::NET_BETTING_MY_BETTING_INFO_AC _Info(ClientDbid,temp.bettingGroup,
						temp.BettingCount,temp.LeageId);

					if ( gpAgentServer )
					{
						gpAgentServer->AddGameAdoJob( db::DbActionPtr(
							new db::BettingInsertAction( temp.UserDbid,temp.BettingCount,temp.LeageId,temp.bettingGroup )) );
					}

					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),&_Info);


					return;
				}
			}
		}
	}

	


	void GLTournamentBetting::RequestMyBettingInfo(DWORD ClientDbid)
	{
		// 내 배팅 정보 알려줌
		{
			std::vector<BettingTable>::iterator _bitor = m_BettingTable.begin();
			std::vector<BettingTable>::iterator _eitor = m_BettingTable.end();
			for (;_bitor != _eitor; ++_bitor )
			{
				BettingTable& temp = *_bitor;
				if(temp.UserDbid == ClientDbid)
				{
					// 이미 신청중
					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
					if ( _pPlayer == 0 )
						return;
					GLMSG::NET_BETTING_MY_BETTING_INFO_AC ANswer(ClientDbid,temp.bettingGroup,
						temp.BettingCount, temp.LeageId);
					gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(), &ANswer);
					return;
				}
			}
		}
	}


	float GLTournamentBetting::CarcurateBettingRatio(DWORD Groupid)
	{
		if ( m_script->PushFunctionEx("GetBettingPerRatio") == true )
		{
			m_script->PushInteger(Groupid);
			if ( m_script->CallFunction(1, 1) == true )
				return m_script->PopNumber();
		}
        return 0.0f;
	}

	 

	void GLTournamentBetting::RequestGroupInfo(DWORD ClientDbid)
	{
		// 그룹 정보 전달

		MatchingInstanceBase* matching = m_parent->getMatchingManager();


		std::vector<GLMatChingGroupInfo>::iterator biter = matching->m_vecMatchingGroupInfo.begin();
		std::vector<GLMatChingGroupInfo>::iterator eiter = matching->m_vecMatchingGroupInfo.end();

		BettingTable* myBettingTable = NULL;

		std::vector<BettingTable>::iterator _bitor = m_BettingTable.begin();
		std::vector<BettingTable>::iterator _eitor = m_BettingTable.end();

		for (;_bitor != _eitor; ++_bitor )
		{
			BettingTable* _myBettingTable = NULL;
			_myBettingTable = &*_bitor;
			if(_myBettingTable->UserDbid == ClientDbid)
			{
				
				// 이미 신청중
				GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
				if ( _pPlayer == 0 )
					return;
				myBettingTable = _myBettingTable;
				break;
			}
		}
		
		GLMSG::NET_BETTING_GROUPINFO_AC _MessageNode;
		if(myBettingTable == NULL)
		{
			_MessageNode.bettingGroup	= -1;
			_MessageNode.BettingCount	= 0;
			_MessageNode.BettingRato	= 0;
			_MessageNode.ScriptID		= m_parent->GetScriptID();
		}
		else
		{
			float bettingRatio = CarcurateBettingRatio(myBettingTable->bettingGroup);
			_MessageNode.bettingGroup	= myBettingTable->bettingGroup;
			_MessageNode.BettingCount	= myBettingTable->BettingCount;
			_MessageNode.BettingRato	= bettingRatio;
			_MessageNode.ScriptID		= m_parent->GetScriptID();
			
		}
		std::copy(m_BettingRatioTable.begin(),m_BettingRatioTable.end(), std::back_inserter(_MessageNode.Bettinginfo));
		
		

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, _MessageNode);

		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(ClientDbid));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(),NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, SendBuffer);
	}

	void GLTournamentBetting::SendTournamentBettingRaito(DWORD ClientDbid,DWORD LeageId)
	{

		// 배팅율
		if(LeageId != m_parent->GetScriptID())
			return;

		GLMSG::NET_BETTING_BETTINGINFO_AC _MessageNode(ClientDbid,Winner_Bet_Money_Min,
			Winner_Bet_Money_Max,m_parent->GetScriptID());

		MatchingCore::getinstance()->_sendMessageToClient(ClientDbid,&_MessageNode);
	}

	void GLTournamentBetting::ResualtWinner(DWORD Groupid)
	{
		//이긴사람을 알려줌
		std::vector<BettingTable>::iterator _bitor = m_BettingTable.begin();
		std::vector<BettingTable>::iterator _eitor = m_BettingTable.end();

		for (;_bitor != _eitor; ++_bitor )
		{
			BettingTable& myBettingTable = *_bitor;
			if (myBettingTable.bettingGroup == Groupid)
			{
				//스크립트에 등록
				if ( m_script->PushFunctionEx("ResualtWinner") == true )
				{
					m_script->PushInteger(myBettingTable.UserDbid);
					m_script->PushInteger(Groupid);
					m_script->PushInteger(myBettingTable.BettingCount);
					m_script->CallFunction(3, 0);

					//// 토너먼트 로그
					//db::LogTournament* pDbAction = new db::LogTournament(9103);

					//pDbAction->dwCharDbNum     =  myBettingTable.UserDbid;   
					//pDbAction->dwClass     =  Groupid;   
					//pDbAction->dwSchool     =  myBettingTable.BettingCount;   

					//gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
				}
			}
		}
		m_BettingTable.clear();
	}

	void GLTournamentBetting::SendTournamentGroupInfo(DWORD ClientDbid,DWORD Groupid)
	{
		//토너먼트 그룹 정보.
	}

	void GLTournamentBetting::FieldCheckMoneyOK(DWORD ClientDbid,BOOL Isok)
	{
		//필드에서 돈만체크.. 이런경우는 없다!
	}

	void GLTournamentBetting::PastBettingUserProcess(DWORD outChaDbNum, LONGLONG outBettingCount)
	{
		GLMSG::NET_BETTING_MONEY_OK_RESTORE_AF ANswer(outChaDbNum,outBettingCount);
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(outChaDbNum));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOFIELD(_pPlayer->GetChannel(),_pPlayer->GetCurrentFieldServer(), &ANswer);
	}

	void GLTournamentBetting::registLuaCallBack( sc::LuaInstance* script )
	{
		script->registdirectFunction("SetCanBetting",*this,&GLTournamentBetting::SetCanBetting);
		script->registdirectFunction("GetCanBetting",*this,&GLTournamentBetting::GetCanBetting);
	}

}
