#include "pch.h"
#include "MatchingManager.h"
#include "./MatchingDefine.h"
#include "../../SigmaCore/Lua/MinLua.h"

#include "../TournamentManager/MatchingJoinManager.h"

#include "../InstanceSystem/InstanceSystem.h"
#include "../InstanceSystem/LogicAgent/InvokeFunctionAgent.h"

#define MATCHINGPROPERTY_REGISTER( REGESTKEY )								\
	LuaPlus::LuaObject REGESTKEY = m_script->lua_getFromName("##REGESTKEY")	\
	if ( REGESTKEY.IsNil() == true )										\
	{																		\
		sc::writeLogError("RoomManagerBase : ##REGESTKEY is nil");			\
		return;																\
	}																		


namespace MatchingSystem
{
	RoomManagerBase::RoomManagerBase(MatchingGameBase* parent,sc::LuaInstance* script)
		:m_script(script),m_parent(parent),m_fElpaseTime(0),MatchingOpenIndex(-1),m_RetryTime(0)
	{
	}

	void RoomManagerBase::Initialize()
	{
        // 인던 시스템 함수 등록;
        InstanceSystem::registFunctionCommon(*m_script);
        InstanceSystem::registFunctionAgent(*m_script);

		m_parent->RegistFunctionGameBase(m_script);

		//지역 함수 설정.
		m_script->registdirectFunction("MatchingInfo",*this,&RoomManagerBase::MatchingInfo);

		LuaPlus::LuaObject bCanObserve = m_script->GetGlobalValue_Lua("TournamentCanObserve");
		LuaPlus::LuaObject bCanTOTO = m_script->GetGlobalValue_Lua("TournamentCanToTo");

        /*LuaPlus::LuaObject ContentsMainType = m_script->GetGlobalValue_Lua("ContentsMainType");
        LuaPlus::LuaObject ContentsSubType = m_script->GetGlobalValue_Lua("ContentsSubType");
        LuaPlus::LuaObject ContentsName = m_script->GetGlobalValue_Lua("ContentsName");
        LuaPlus::LuaObject ContentsMissionInfo = m_script->GetGlobalValue_Lua("ContentsMissionInfo");
        LuaPlus::LuaObject ContentsDetailInfo = m_script->GetGlobalValue_Lua("ContentsDetailInfo");
        LuaPlus::LuaObject ContentsRewardInfo = m_script->GetGlobalValue_Lua("ContentsRewardInfo");*/
        LuaPlus::LuaObject ContentsType = m_script->GetGlobalValue_Lua("ContentsType");
        LuaPlus::LuaObject ContentsDateType = m_script->GetGlobalValue_Lua("ContentsDateType");
        LuaPlus::LuaObject ContentsMapID = m_script->GetGlobalValue_Lua("ContentsMapID");
        LuaPlus::LuaObject ContentsDate = m_script->GetGlobalValue_Lua("ContentsDate");

		m_script->GetStringGlobalValue_Lua( "ContentsMainType", m_GLMatching.m_strContentsMainType );
		m_script->GetStringGlobalValue_Lua( "ContentsSubType", m_GLMatching.m_strContentsSubType );
		m_script->GetStringGlobalValue_Lua( "ContentsName", m_GLMatching.m_strContentsName );
		m_script->GetStringGlobalValue_Lua( "ContentsMissionInfo", m_GLMatching.m_strMissionInfo );
		m_script->GetStringGlobalValue_Lua( "ContentsDetailInfo", m_GLMatching.m_strDetailInfo );
		m_script->GetStringGlobalValue_Lua( "ContentsRewardInfo", m_GLMatching.m_strRewardInfo );

        LuaPlus::LuaObject ContentsUseInfoButton = m_script->GetGlobalValue_Lua("ContentsUseInfoButton");
		LuaPlus::LuaObject ContentsMaxJoincount = m_script->GetGlobalValue_Lua("ContentsMaxJoinCount");
        
#pragma region nil 체크

		if( bCanObserve.IsNil() == true)
			sc::writeLogInfo("RoomManagerBase : bCanObserve Not exsist (defalt false)");
		else
		{
			const bool _bCanObserve(bCanObserve.GetBoolean());
			m_GLMatching.m_bCanObserve = _bCanObserve;
		}

		if( bCanTOTO.IsNil()  == true)
			sc::writeLogInfo("RoomManagerBase : bCanTOTO Not exsist (defalt false)");
		else
		{
			const bool _bCanTOTO(bCanTOTO.GetBoolean());
			m_GLMatching.m_bCanTOTO = _bCanTOTO;
		}

//-------------------------------------------------------------------------------------------------------
       // 공용 입장 UI 작업
//-------------------------------------------------------------------------------------------------------
		/*if ( ContentsMainType.IsString() == false )
			sc::writeLogError("RoomManagerBase : ContentsMainType Error (mus be String)");

        if( ContentsMainType.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsMainType Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsMainType.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strContentsMainType = _strigndData;
        }

        if( ContentsSubType.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsSubType Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsSubType.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strContentsSubType = _strigndData;
        }

        if( ContentsName.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsName Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsName.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strContentsName = _strigndData;
        }

        if( ContentsMissionInfo.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsMissionInfo Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsMissionInfo.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strMissionInfo = _strigndData;
        }

        if( ContentsDetailInfo.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsDetailInfo Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsDetailInfo.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strDetailInfo = _strigndData;
        }

        if( ContentsRewardInfo.IsWString() == false)
            sc::writeLogError("RoomManagerBase : ContentsRewardInfo Error (must be WString)");
        else
        {
            const std::wstring _wStringData((const wchar_t*)ContentsRewardInfo.GetWString());
            const std::string _strigndData(sc::string::unicodeToAnsi(_wStringData));
            m_GLMatching.m_strRewardInfo = _strigndData;
        }*/

        if( ContentsType.IsInteger() == false)
            sc::writeLogError("RoomManagerBase : ContentsType Error");
        else
            m_GLMatching.m_ContentsType = (MATCHING_CONTENTS_TYPE)ContentsType.GetInteger();

        if( ContentsMapID.IsInteger() == false)
            sc::writeLogError("RoomManagerBase : ContentsMapID Error");
        else
            m_GLMatching.m_dwContentsMapID = ContentsMapID.GetInteger();

        if( ContentsDateType.IsInteger() == false)
        {
            sc::writeLogError("RoomManagerBase : ContentsDateType Error");
            m_GLMatching.m_ContentsDateType = EMMATCHING_TIME_EVENTUAL;
        }
        else
            m_GLMatching.m_ContentsDateType = (MATCHING_TIME_TYPE)ContentsDateType.GetInteger();

        if ( ContentsUseInfoButton.IsBoolean() == false)
            m_GLMatching.m_bEnableInfoButton = true;
        else
            m_GLMatching.m_bEnableInfoButton = ContentsUseInfoButton.GetBoolean();

		if ( false == ContentsMaxJoincount.IsInteger() )
		{
			sc::writeLogInfo( "[ Matching Log ] [ Value ContentsMaxJoincount don't exist ( default : 0 ) ]" );

			m_GLMatching.m_dwMaxJoinCount = 0;
		}
		else
		{
			m_GLMatching.m_dwMaxJoinCount = static_cast< DWORD >( ContentsMaxJoincount.GetInteger() );
		}

        // 상시 진행 방식의 경우 기본적으로 신청가능 상태;
        // 이벤트식 진행 방식의 경우 기본적으로 신청불가 상태;
        // 정시 진행 방식의 경우에만 진행 시간 정보를 읽는다;
        switch ( m_GLMatching.m_ContentsDateType )
        {
        case EMMATCHING_TIME_ALWAYS:
            Refresh(EMMATCHING_CONTENTS_OPENED);
            break;
        case EMMATCHING_TIME_PERIODIC:
            {
                if( ContentsDate.IsTable() == false)
                {
                    sc::writeLogError("RoomManagerBase : ContentsDate Error");
                    m_GLMatching.m_ContentsDateType = EMMATCHING_TIME_EVENTUAL;
                    Refresh(EMMATCHING_CONTENTS_CLOSED);
                }
                else
                {
                    for ( LuaPlus::LuaTableIterator _iterator(ContentsDate); _iterator; _iterator.Next() )
                    {
                        LuaPlus::LuaObject ContentsDateObj = _iterator.GetValue();
                        if ( ContentsDateObj.IsTable() )
                        {
                            GLMatching_TimeInfo _timeInfo;
                            LuaPlus::LuaTableIterator _ObjIterator(ContentsDateObj);

                            if ( _ObjIterator.IsValid() == false )
                                break;
                            _timeInfo.wWeekDay = _ObjIterator.GetValue().GetInteger();
                            _ObjIterator.Next();

                            if ( _ObjIterator.IsValid() == false )
                                break;
                            _timeInfo.wStartHour = _ObjIterator.GetValue().GetInteger();
                            _ObjIterator.Next();

                            if ( _ObjIterator.IsValid() == false )
                                break;
                            _timeInfo.wStartMin = _ObjIterator.GetValue().GetInteger();
                            _ObjIterator.Next();

                            if ( _ObjIterator.IsValid() == false )
                                break;
                            _timeInfo.dwDurationMin = _ObjIterator.GetValue().GetInteger();

                            m_GLMatching.m_vecTimeInfo.push_back(_timeInfo);
                        }
                    }
                }
            }
            break;
        case EMMATCHING_TIME_EVENTUAL:
            Refresh(EMMATCHING_CONTENTS_CLOSED);
            break;
        }

#pragma endregion nil 체크		
	}

	RoomManagerBase::~RoomManagerBase()
	{
		m_script->OnDestroy();
		SAFE_DELETE(m_script);

		std::vector<RoomMultyTrigger*>::iterator bitor = m_FaildTrigger.begin() ;
		for(;bitor != m_FaildTrigger.end();)
		{
			RoomMultyTrigger* FaildTrigger = *bitor;
			bitor = m_FaildTrigger.erase(bitor);
			SAFE_DELETE(FaildTrigger);
		}

		std::vector<JoinData*> ::iterator _bitor = m_vecInstanceMapids.begin();

		for(;_bitor != m_vecInstanceMapids.end();)
		{
			JoinData* JoinData_ = *_bitor;
			_bitor = m_vecInstanceMapids.erase(_bitor);
			SAFE_DELETE(JoinData_);
		}

		//SAFE_DELETE(m_parent);
	}


	void RoomManagerBase::OnFrameMove(float fElpaseTime)
	{
		//m_fElpaseTime += fElpaseTime;

		m_RetryTime += fElpaseTime;
		if(m_RetryTime > 10)
		{
			
			
			//맵 생성에 실패한 트리거를 모아둠
			std::vector<RoomMultyTrigger*>::iterator bitor = m_FaildTrigger.begin() ;
			for(;bitor != m_FaildTrigger.end();)
			{
				RoomMultyTrigger* FaildTrigger = *bitor;

				FaildTrigger->m_frameMove -= m_RetryTime;

				if(FaildTrigger->m_frameMove  > 0)
				{
					++bitor;
					continue;
				}

				float mapFrameMove = FaildTrigger->m_frameMove;
				FaildTrigger->m_frameMove = 0;
				//인던 시스템에 넣어줌.
				InstanceSystem::ManagerAgent* pmanager = MatchingCore::getInstanceManager();
				//날려버림.

				//for( unsigned int i = 0; i < _ptrigger->_MapNeedSize; i ++)

				bool isScsess = pmanager->createInstanceByTrigger(FaildTrigger);

				if(isScsess == false)
				{
					FaildTrigger->m_frameMove = 1;
					for(DWORD i = 0 ; i < FaildTrigger->m_faildCounter;i++)
					{
						FaildTrigger->m_frameMove *= 5;
					}
					

					/*
					if(FaildTrigger->m_faildCounter > 5)
					{

						CString strTemp;
						strTemp.Format("MapCreate Fail :: Please Check Map ID (%d/%d)",
							FaildTrigger->_instanceID.wMainID,FaildTrigger->_instanceID.wSubID );
						sc::writeLogError(strTemp.GetString());
						MatchingSystem::RoomMultyReceipt receipt;
						bitor = m_FaildTrigger.erase(bitor);
						FaildTrigger->Call(&receipt);
						SAFE_DELETE(FaildTrigger);
						continue;
					}
					else*/
					{
						FaildTrigger->m_faildCounter += 1;
						CString strTemp;
						strTemp.Format("MapCreate Fail :: Retry Create Map (%d) (retry to %.0fs)", FaildTrigger->m_faildCounter ,FaildTrigger->m_frameMove);
						sc::writeLogError(strTemp.GetString());

					}
				}
				else
				{
					bitor = m_FaildTrigger.erase(bitor);
					continue;
				}
				++bitor;

			}

			m_RetryTime = 0;
		}
		

        // 정기식 진행일 경우 시간경과에 따라 진행상태를 자동으로 바꾼다;
        if ( m_GLMatching.m_ContentsDateType == EMMATCHING_TIME_PERIODIC )
        {
            CTime cCurTime = CTime::GetCurrentTime();
            int nDayOfWeek = cCurTime.GetDayOfWeek ();
            int nDay = cCurTime.GetDay ();
            int nHour = cCurTime.GetHour();
            int nMinute = cCurTime.GetMinute ();

			//int _MatchingOpenIndex =-1;

			switch (m_GLMatching.m_ContentsType)
			{

			/*case EMMATCHING_CONTENTS_TOURNAMENT:
				
				// [shhan][2014.10.13] 루아 쪽에서 EMMATCHING_CONTENTS_IN_PROGRESS, EMMATCHING_CONTENTS_CLOSED 로 변경하는 부분이 있다.
				//						일단 건들지 않도록 한다.
				//
				//시간을 측정한다.
				BOOST_FOREACH(GLMatching_TimeInfo _timeInfo,m_GLMatching.m_vecTimeInfo)
				{
					_MatchingOpenIndex++;
					if ( ( _timeInfo.wWeekDay != 0 ) && ( _timeInfo.wWeekDay != nDayOfWeek ) )
						continue;

					const int nHourInterval = nHour - _timeInfo.wStartHour;
					const int nMinInterval = nMinute - _timeInfo.wStartMin;

					if ( nHourInterval < 0 )
					{
						SetStatus(EMMATCHING_CONTENTS_CLOSED);
						continue;
					}

					int nInterval = nHourInterval * 60;
					nInterval += nMinInterval;

					if ( nInterval >= 0 )
					{
						if ( nInterval <= (int)_timeInfo.dwDurationMin )
						{
							//단한번만 Open으로 바꾼다.
							if(MatchingOpenIndex != _MatchingOpenIndex)
							{
								MatchingOpenIndex = _MatchingOpenIndex;
								SetStatus(EMMATCHING_CONTENTS_OPENED);
							}
							return;
						}
					}
				}
				MatchingOpenIndex= -1;
				break;*/


			case EMMATCHING_CONTENTS_NOMAL:
			case EMMATCHING_CONTENTS_NOMAL_REJOINALBE:
			default:

				// [shhan][2014.10.13] 하루에 두번 CMD이 열리면서, 매 Frame, EMMATCHING_CONTENTS_CLOSED 과 EMMATCHING_CONTENTS_OPENED 를 반복함.
				//						엄청난 부하를 유발함.
				//						하루에 한번 셋팅이라면 정상작동되었었음.
				//
				////시간을 측정한다.
				//BOOST_FOREACH(GLMatching_TimeInfo _timeInfo,m_GLMatching.m_vecTimeInfo)
				//{
				//	if ( ( _timeInfo.wWeekDay != 0 ) && ( _timeInfo.wWeekDay != nDayOfWeek ) )
				//		continue;

				//	const int nHourInterval = nHour - _timeInfo.wStartHour;
				//	const int nMinInterval = nMinute - _timeInfo.wStartMin;

				//	if ( nHourInterval < 0 )
				//	{
				//		SetStatus(EMMATCHING_CONTENTS_CLOSED);
				//		continue;
				//	}

				//	int nInterval = nHourInterval * 60;
				//	nInterval += nMinInterval;

				//	if ( nInterval >= 0 )
				//	{
				//		if ( nInterval <= (int)_timeInfo.dwDurationMin )
				//		{
				//			SetStatus(EMMATCHING_CONTENTS_OPENED); // 참여 가능
				//			return;
				//		}
				//		else
				//		{
				//			SetStatus(EMMATCHING_CONTENTS_CLOSED);
				//		}
				//	}

				//	//시간 지났음 참가 불가능합니당.
				//	//
				//}
				//SetStatus(EMMATCHING_CONTENTS_CLOSED); 



				//시간을 측정한다.
				BOOST_FOREACH(GLMatching_TimeInfo _timeInfo,m_GLMatching.m_vecTimeInfo)
				{
					if ( ( _timeInfo.wWeekDay != 0 ) && ( _timeInfo.wWeekDay != nDayOfWeek ) )
						continue;

					int nNowMinute = nHour * 60 + nMinute;
					int nSettingMinute = _timeInfo.wStartHour * 60 + _timeInfo.wStartMin;

					// 셋팅시간보다 5분 지났으면 5가 나옴.
					// 셋팅시간보다 5분 이전이면 -5 가 나옴.
					int nInterval = nNowMinute - nSettingMinute;

					// [shhan][2014.10.13] 참여가 가능한 시간이 되면 참여된다고 셋팅한다.
					//						SetStatus 내부에서 같은 셋팅값이면 추가적인 작업은 하지 않는다.
					//						return 이 꼭 필요하다.
					if ( nInterval >= 0 && nInterval <= (int)_timeInfo.dwDurationMin )
					{
						// 바꾸고 싶은 부분이다. 
						// 다른곳에서 다른셋팅값을 넣으면, 문제 또 발생됨.
						// 현재는 루아나 소스상에서 변경되지는 않고 있다.
						if ( SetStatus(EMMATCHING_CONTENTS_OPENED) ) // 참여 가능	
						{
							sc::writeLogInfo( sc::string::format(
								"[ Matching Log ] [ Open Matching, Interval : %1% / %2%, CurTime : %3% Day, %4%:%5% ( Day of week %6% ) ]",
								nInterval,
								(int)_timeInfo.dwDurationMin,
								nDay,
								nHour,
								nMinute,
								nDayOfWeek ) );
						}

						// 조건을 만족한다면 return 필수.
						return;
					}
				}

				// [shhan][2014.10.13] 매칭을 위한 모든시간조건을 검사했는데, 참여가 불가능이면 당연 EMMATCHING_CONTENTS_CLOSED 해준다.
				//						SetStatus 내부에서 같은 셋팅값이면 추가적인 작업은 하지 않는다. 
				//						바꾸고 싶은 부분이다.	 
				//						다른곳에서 다른셋팅값을 넣으면, 문제 또 발생됨.
				//						현재는 루아나 소스상에서 변경되지는 않고 있다.
				if ( SetStatus(EMMATCHING_CONTENTS_CLOSED) )
				{
					sc::writeLogInfo( sc::string::format(
						"[ Matching Log ] [ Close Matching, CurTime : %1% Day, %2%:%3% ( Day of week %4% ) ]",
						nDay,
						nHour,
						nMinute,
						nDayOfWeek ) );
				}



				break;
			}

             
        }
	}



	bool RoomManagerBase::NoticeTournamentinfo(DWORD ClietDBID)
	{
		if ( m_script->PushFunctionEx("NoticeTournamentinfo") == true )
		{
			m_script->PushInteger(ClietDBID);
			if ( m_script->CallFunction(1, 1) == true )
				return m_script->PopBoolean();
		}
		MatchingInfo(ClietDBID);
		return false;
	}

	void RoomManagerBase::MatchingInfo ( DWORD _dwChaDBNum )
	{
		GLCharAG* const pChar = gpAgentServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return;

		// 유저가 입장중인 Matching Instance Dungeon 인지 확인한다;
		bool bInstanceDugeonMember = ( GetMatchingBaseID_JoinGame( _dwChaDBNum ) == m_parent->m_MatchingInstanceID );

		// 재입장 가능한 Matching 인지 확인한다;
		bool bRejoin = CanRejoin( _dwChaDBNum );
		bRejoin = ( bRejoin && bInstanceDugeonMember );

		// Matching 의 상태를 확인한다;
		const MATCHING_CONTENTS_STATUS emStatus( m_GLMatching.m_ContentsStatus );
		
		// Matching 갱신 정보 메시지를 생성한다;
		GLMSG::NET_MATCHING_NOTIFY_AT_FIRST_AC sNetMsg(
			_dwChaDBNum,
			m_parent->GetScriptID(),
			static_cast< WORD >( m_GLMatching.m_ContentsType ),
			bRejoin ? static_cast< WORD >( EMMATCHING_CONTENTS_IN_PROGRESS ) : static_cast< WORD >( emStatus ),
			m_GLMatching.m_dwContentsMapID,
			m_GLMatching.m_strContentsMainType,
			m_GLMatching.m_strContentsSubType,
			m_GLMatching.m_strContentsName,
			m_GLMatching.m_strDetailInfo,
			m_GLMatching.m_strMissionInfo,
			m_GLMatching.m_strRewardInfo,
			static_cast< WORD >( m_GLMatching.m_ContentsDateType ),
			m_GLMatching.m_vecTimeInfo,
			m_GLMatching.m_bCanObserve,
			m_GLMatching.m_bCanTOTO,
			m_GLMatching.m_bEnableInfoButton,
			bRejoin,
			m_GLMatching.m_dwMaxJoinCount );

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		gpAgentServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, SendBuffer );

		// 클라이언트의 Matching UI 정보를 갱신시킨다;
		GLMSG::NET_MATCHING_INFO_UPDATE_AC sUpdateNetMsg( m_parent->GetScriptID(), emStatus );
		MatchingCore::getinstance()->_sendMessageToClient( _dwChaDBNum, &sUpdateNetMsg );
	}

	//

	const bool RoomManagerBase::SetStatus(DWORD _status)
	{
		if(_status == m_GLMatching.m_ContentsStatus)
			return false;

		m_GLMatching.m_ContentsStatus = (MATCHING_CONTENTS_STATUS)_status;

		GLMSG::NET_MATCHING_INFO_UPDATE _message(m_parent->GetScriptID(),_status);
		gpAgentServer->SENDTOALLCLIENT(&_message);
		gpAgentServer->SENDTOALLCHANNEL(&_message);

		return true;
	}
	void RoomManagerBase::Refresh(DWORD _status)
	{

		if(_status == m_GLMatching.m_ContentsStatus)
			return;
		m_GLMatching.m_ContentsStatus = (MATCHING_CONTENTS_STATUS)_status;


        GLMSG::NET_MATCHING_NOTIFY_AT_FIRST_AC _messageAC(0,
            m_parent->GetScriptID(),
            (WORD)m_GLMatching.m_ContentsType,
            (WORD)m_GLMatching.m_ContentsStatus,
            m_GLMatching.m_dwContentsMapID,
            m_GLMatching.m_strContentsMainType,
            m_GLMatching.m_strContentsSubType,
            m_GLMatching.m_strContentsName,
            m_GLMatching.m_strDetailInfo,
            m_GLMatching.m_strMissionInfo,
            m_GLMatching.m_strRewardInfo,
            (WORD)m_GLMatching.m_ContentsDateType,
            m_GLMatching.m_vecTimeInfo,
            m_GLMatching.m_bCanObserve,
            m_GLMatching.m_bCanTOTO,
            m_GLMatching.m_bEnableInfoButton,
			false,
			m_GLMatching.m_dwMaxJoinCount
            );

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, _messageAC);
        gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, SendBuffer);

		GLMSG::NET_MATCHING_INFO_UPDATE _messageAF(m_parent->GetScriptID(),_status);

		MatchingCore::getinstance()->_sendMessageAgentToALLField(&_messageAF);
	}

	void RoomManagerBase::CreateNewField(RoomTrigger* _ptrigger)
	{
		//인던 시스템에 넣어줌.
		InstanceSystem::ManagerAgent* pmanager = MatchingCore::getInstanceManager();
		//날려버림.
		pmanager->createInstanceByTrigger(_ptrigger);
	}

	void RoomManagerBase::CreateNewField(RoomMultyTrigger* _ptrigger)
	{
		//인던 시스템에 넣어줌.
		InstanceSystem::ManagerAgent* pmanager = MatchingCore::getInstanceManager();
		//날려버림.

		//for( unsigned int i = 0; i < _ptrigger->_MapNeedSize; i ++)
		bool isScsess = pmanager->createInstanceByTrigger(_ptrigger);

		if(isScsess == false)
		{
			sc::writeLogError("MapCreate Fail :: Retry Create Map");
			_ptrigger->m_faildCounter = 1;
			m_FaildTrigger.push_back(_ptrigger);
		}



	}

	int RoomManagerBase::DestroyComplete(DestroyReceipt* _RoomReceipt,bool NotOut )
	{
		std::vector< DWORD > vecTempMember;

		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();

		bool isRoomExsist = false;
		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;

			if(_data->InstanceMapID == _RoomReceipt->_instanceMapID)
			{
				if(NotOut == false)
				{
                    BOOST_FOREACH(DWORD p, _data->m_VecGroupID)
                    {
                        m_parent->getLobbyManager()->DeleteMatchingGroup(p);
                    }

					//BOOST_FOREACH(DWORD p, _data->m_VecClientID)
					//{
						//MatchingCore::getinstance()->OutMatchingSystem(m_parent->GetScriptID(),p);
					//}
				}

				BOOST_FOREACH(DWORD _nDbNum, _data->m_VecClientID)
				{
					GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC sendserverinfo(_nDbNum,_RoomReceipt->_instanceMapID);
					MatchingCore::getinstance()->_sendMessageToClient(_nDbNum,&sendserverinfo);

					vecTempMember.push_back( _nDbNum );
				}
				SAFE_DELETE(_data);
				biter = m_vecInstanceMapids.erase(biter);
				break;
			}
		}

		// 파괴되는 인던의 멤버들이거나 이었던 유저들의 Matching 정보를 갱신시킨다;
		for ( UINT i=0; i<vecTempMember.size(); ++i )
		{
			MatchingInfo( vecTempMember.at( i ) );
		}

		// Matching 이 성공적으로 파괴되었음을 알린다;
		if ( m_script->PushFunctionEx("DestroyComplete") == true )
		{
			m_script->PushInteger(_RoomReceipt->_instanceMapID);
			m_script->CallFunction(1, 0);
		}
		return false;
	}	

	bool RoomManagerBase::IsMyMap( const InstanceSystem::InstanceMapID TornanmentMapID , DWORD MatchingInstanceID )
	{
		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();

		MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;

			if( _data->MatchingInstacneScriptID != MatchingInstanceID)
				continue;

			if ( _data->InstanceMapID == TornanmentMapID)
				return true;
		}
		return false;
	}

	DWORD RoomManagerBase::GetChaNumOnGroup( const InstanceSystem::InstanceMapID nTornanmentMapID, DWORD nMatchingInstanceID, DWORD nFactionID )
	{
		DWORD dwChaNum = 0;
		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();
		MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;

			if( _data->MatchingInstacneScriptID != nMatchingInstanceID)
				continue;

			if ( _data->InstanceMapID == nTornanmentMapID )
			{
				if( _data->m_VecClientID.size() <= nFactionID  )
				{
					dwChaNum = 0;
				}
				else
				{
					dwChaNum = _data->m_VecClientID[nFactionID];
				}
				return dwChaNum;
			}
		}

		return 0;
	}

	bool RoomManagerBase::HasMyMap( DWORD MatchingInstanceID )
	{
		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();

		MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;

			if( _data->MatchingInstacneScriptID != MatchingInstanceID)
				return true;
		}
		return false;
	}


	const DWORD RoomManagerBase::GetInstanceMapID_JoinGame ( const DWORD _dwChaDBNum )
	{
		std::vector< JoinData* >::iterator beginJoinIter =
			m_vecInstanceMapids.begin();
		for ( ; beginJoinIter != m_vecInstanceMapids.end(); ++beginJoinIter )
		{
			JoinData* pJoinData = *beginJoinIter;
			if ( NULL == pJoinData )
				continue;

			std::vector<DWORD>::iterator beginClientIter =
				pJoinData->m_VecClientID.begin();
			for ( ; beginClientIter != pJoinData->m_VecClientID.end(); ++beginClientIter )
			{
				if ( *beginClientIter == _dwChaDBNum )
				{
					return pJoinData->InstanceMapID;
				}
			}
		}

		return MAXDWORD;
	}


	const DWORD RoomManagerBase::GetMatchingBaseID_JoinGame ( const DWORD _dwChaDBNum )
	{
		std::vector< JoinData* >::iterator beginJoinIter =
			m_vecInstanceMapids.begin();
		for ( ; beginJoinIter != m_vecInstanceMapids.end(); ++beginJoinIter )
		{
			JoinData* pJoinData = *beginJoinIter;
			if ( NULL == pJoinData )
				continue;

			std::vector<DWORD>::iterator beginClientIter =
				pJoinData->m_VecClientID.begin();
			for ( ; beginClientIter != pJoinData->m_VecClientID.end(); ++beginClientIter )
			{
				if ( *beginClientIter == _dwChaDBNum )
				{
					return pJoinData->dwMatchingBaseID;
				}
			}
		}

		return MAXDWORD;
	}


	const DWORD RoomManagerBase::GetMatchingInstanceID_JoinGame ( const DWORD _dwChaDBNum )
	{
		std::vector< JoinData* >::iterator beginJoinIter =
			m_vecInstanceMapids.begin();
		for ( ; beginJoinIter != m_vecInstanceMapids.end(); ++beginJoinIter )
		{
			JoinData* pJoinData = *beginJoinIter;
			if ( NULL == pJoinData )
				continue;

			std::vector<DWORD>::iterator beginClientIter =
				pJoinData->m_VecClientID.begin();
			for ( ; beginClientIter != pJoinData->m_VecClientID.end(); ++beginClientIter )
			{
				if ( *beginClientIter == _dwChaDBNum )
				{
					return pJoinData->MatchingInstacneScriptID;
				}
			}
		}

		return MAXDWORD;
	}


	const bool RoomManagerBase::CanRejoin ( const DWORD _dwChaDBNum )
	{
		// 재입장 가능한 방인지 확인;
		if ( GetMatchingInfo().m_ContentsType != EMMATCHING_CONTENTS_NOMAL_REJOINALBE )
			return false;

		LobbyManagerbase* pLobby = m_parent->getLobbyManager();
		if ( NULL == pLobby )
			return false;

		return pLobby->CanRejoin( _dwChaDBNum );
	}


	bool RoomManagerBase::RequestObserve(DWORD GroupID, DWORD ReQuester,bool UseCashItem)
	{
		// 유저는 저장할필요없다.
		// 이유는 인던 스크립트에서 관전자를 정리하고 사용하기 때문이다.
		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();

		MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();

		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;

			BOOST_FOREACH ( DWORD _GroupID , _data->m_VecGroupID)
			{
				if(_GroupID == GroupID)
				{
					bool ScrpitAnser = true;

					//스크립트에 정의 되지 않다고 해도 괜찮다.
					if ( m_script->PushFunctionEx("RequestObserve") == true )
					{
						m_script->PushInteger(ReQuester);
						m_script->PushInteger(GroupID);
						m_script->PushInteger(_data->InstanceMapID);
						if ( m_script->CallFunction(3, 1) == true )
							ScrpitAnser = m_script->PopBoolean(); // 막을순있다!.
					}

					//케쉬 아이템 쓰면 걍 입장이다.
					if(UseCashItem == true)
					{
						ScrpitAnser = true;
					}

					if(ScrpitAnser == true)
					{
						JoinTrigger * ptrigger = new JoinTrigger(_data->InstanceMapID);
						ptrigger->_ClientDBID = ReQuester;
						ptrigger->_instanceMapID = _data->InstanceMapID;
						ptrigger->RegistCallback(*this,&RoomManagerBase::ObserveJoinComplete);

						InstanceSystem::InformationPosition _informationPosition(
							Faction::GLActorID(CROW_PC, ReQuester),
							SNATIVEID(_data->InstanceMapID, 0),
							Faction::GLActorID(),
							0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

						InstanceSystem::ManagerAgent* pmanager = MatchingCore::getInstanceManager();
						pmanager->DoJoin(ReQuester, _informationPosition, InstanceSystem::EMAUTHORITY_OBSERVER,ptrigger);
						//sc::writeLogInfo(sc::string::format("RequestObserve: ClientDBID:%d, InstanceMapID:%d",ptrigger->_ClientDBID, ptrigger->_instanceMapID));
						//pmanager->_messageRequestOut()
						GLMSG::NET_MATCHING_OBSERVE_ADMISSION_AC __Message(m_parent->GetScriptID(),GroupID,ReQuester);
						MatchingCore::getinstance()->_sendMessageToClient(ReQuester,&__Message);

						//pmanager->DoJoin()
						return true;
					}
					else
					{
						//sc::writeLogInfo("RequestObserve : : ScrpitAnser FAIL");
						return false;
					}
					
				}
				
			}
		}
		//sc::writeLogInfo("RequestObserve : : GroupID Not Exsist FAIL");
		return false;
	}

	int	RoomManagerBase::ObserveJoinComplete(JoinReceipt* _RoomReceipt)
	{
		if(_RoomReceipt->isFail == true)
		{
			sc::writeLogInfo("RoomManagerBase : : Observe FAIL");
		}
		else
		{
			sc::writeLogInfo("RoomManagerBase : : Observe Sucess");
		}
		return 0;
	}

	int	RoomManagerBase::JoinComplete(DWORD GroupID, DWORD _instanceMapID)
	{
		sc::writeLogInfo ( "[ Matching Log ] [ Complete Join!! ]" );

		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();
		
		bool isRoomExsist = false;
		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;
			if( _data->InstanceMapID == _instanceMapID)
			{
				isRoomExsist = true;
				bool isExsist = false;
				BOOST_FOREACH ( DWORD _GroupID , _data->m_VecGroupID)
				{
					if(_GroupID == GroupID)
						isExsist = true;
				}
				if(isExsist == false)
				{
					std::vector<DWORD> clientlist;
					MatchingSystem::GLMatchingGroupManager* groupmanager = m_parent->getGroupManager();
					groupmanager->SearchCharacters_MatchingGroupID(clientlist,GroupID);

					BOOST_FOREACH(DWORD p, clientlist)
					{
						_data->m_VecClientID.push_back(p);

						CMatchingJoinManager::Instance()->JoinMatching_DBNum(
							m_parent->GetScriptID(),
							p );
					}

					_data->m_VecGroupID.push_back(GroupID);
				}
			}
		}

		if(isRoomExsist == false)
		{
			JoinData* dataTable = new JoinData();
			dataTable->InstanceMapID = _instanceMapID;
			dataTable->m_VecGroupID.push_back( GroupID );	
			m_vecInstanceMapids.push_back(dataTable);
		}


		if ( m_script->PushFunctionEx("JoinComplete") == true )
		{
			m_script->PushInteger(GroupID);
			m_script->PushInteger(_instanceMapID);
			if ( m_script->CallFunction(1, 1) == true )
				return m_script->PopBoolean();
		}
		return false;
	}

	int RoomManagerBase::CreateComplete(RoomReceipt* _RoomReceipt)
	{

		std::vector<JoinData*>::iterator biter = m_vecInstanceMapids.begin();

		bool isRoomExsist = false;
		for (;biter != m_vecInstanceMapids.end() ; ++biter)
		{
			JoinData* _data = *biter;
			if( _data->InstanceMapID == _RoomReceipt->_instanceMapID)
			{
				isRoomExsist = true;
			}
		}

		if(isRoomExsist == false)
		{
			JoinData* dataTable = new JoinData();
			dataTable->InstanceMapID = _RoomReceipt->_instanceMapID;
			dataTable->MatchingInstacneScriptID = _RoomReceipt->_MatchingInstanceID;
			dataTable->dwMatchingBaseID = _RoomReceipt->dwMatchingBaseID;
			m_vecInstanceMapids.push_back(dataTable);
		}

		if ( m_script->PushFunctionEx("CreateComplete") == true )
		{
			m_script->PushInteger(_RoomReceipt->_instanceMapID);
			if ( m_script->CallFunction(1, 1) == true )
				return m_script->PopBoolean();
		}
		return false;
	}

    void RoomManagerBase::EventCustomMessage(DWORD _dwClientDBNum, DWORD _dwParam1, DWORD _dwParam2)
    {
        if ( m_script->PushFunctionEx("EventCustomMessage") == true )
        {
            m_script->PushInteger(_dwClientDBNum);
            m_script->PushInteger(_dwParam1);
            m_script->PushInteger(_dwParam2);
            m_script->CallFunction(3, 0);
        }
    }

	void RoomManagerBase::OutObserve( const DWORD RequestID, const InstanceSystem::InstanceMapID TornanmentMapID ) 
	{
		//sc::writeLogInfo(sc::string::format("OutObserve: ClientDBID:%d, InstanceMapID:%d",RequestID, OutObserve));
		//pmanager->_messageRequestOut()

		if ( m_script->PushFunctionEx("OutObserve") == true )
		{
			m_script->PushInteger(RequestID);
			m_script->PushInteger(TornanmentMapID);;
			m_script->CallFunction(2, 0);
		}

		GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC sendserverinfo(RequestID,TornanmentMapID);
		MatchingCore::getinstance()->_sendMessageToClient(RequestID,&sendserverinfo);

		GLMSG::NET_INSTANCE_REQUEST_OUT _outMessage(TornanmentMapID, RequestID);
		gpAgentServer->SENDTOMYSELF(&_outMessage);

		//InstanceSystem::ManagerAgent* pmanager = MatchingCore::getInstanceManager();
		//pmanager->DoOut(RequestID,TornanmentMapID);
	}
}
