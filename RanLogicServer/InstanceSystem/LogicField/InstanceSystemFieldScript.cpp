#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "../InstanceSystem.h"

#include "./CertificationMachineClubMode.h"
#include "./CertificationMachineCountryMode.h"

#include "InvokeFunctionField.h"
#include "InstanceSystemField.h"

namespace InstanceSystem
{
///////////////////////////////////////////////////////////////////////////////////
// public lua Funtion;
/////////////////////////////////////////////////////////////////////////////////// 
    const bool ManagerField::requestJoin(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const DWORD _dwGateID) const
    { // gate이동이 아닐시 gateID = GAEAID_NULL;
		const GLChar* const _pRequester(gpGaeaServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == NULL )
			return false;

		const bool _bValidInstance(this->pScriptManager->isExist(_keyMapID));
		if ( _bValidInstance == false )
		{ // 스크립트 지정이 되어 있지 않다면 통과;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_JOIN_INSTANCE, _keyMapID);
			gpGaeaServer->SENDTOCLIENT(_pRequester->ClientSlot(), &_errorMessage);
			return false;
		}

		if ( _pRequester->CharDbNum() != _dbNum )
		{ // dbNum 정보가 정확히 일치하지 않다면 실패;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_MISMATCH_INFORMATION, _keyMapID);
			gpGaeaServer->SENDTOCLIENT(_pRequester->ClientSlot(), &_errorMessage);
			return false;
		}		

		if ( this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_USE) == false ||
			this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_VISIBLE) == false )
		{ // 인던이 잠금 상태이거나 보이지 않는 경우 실패;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			gpGaeaServer->SENDTOCLIENT(_pRequester->ClientSlot(), &_errorMessage);
			return false;
		}

        sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(_keyMapID));
        if ( _pLuaInstance == 0 )
		{ // 잘못된 인던 ID;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			gpGaeaServer->SENDTOCLIENT(_pRequester->ClientSlot(), &_errorMessage);
            return false;
		}

		if ( _pLuaInstance->PushFunctionEx("luaFieldRequestJoin") == true )
		{
			const bool _bRequestByGate = ( _dwGateID != GAEAID_NULL );
			_pLuaInstance->PushInteger(_dbNum);
			_pLuaInstance->PushInteger(_bRequestByGate);

			bool _bResult(false);
			_pLuaInstance->CallFunction(2, 1);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaFieldRequestJoin ]" );
#endif

			_bResult = _pLuaInstance->PopBoolean();

			if ( _bResult == false )
			{
				GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_ACCESS_DENIED_INSTANCE, _keyMapID);
				gpGaeaServer->SENDTOCLIENT(_pRequester->ClientSlot(), &_errorMessage);
				return false;
			}
		}

		return true;
    }

///////////////////////////////////////////////////////////////////////////////////
// instance lua Funtion;
///////////////////////////////////////////////////////////////////////////////////
	const bool InstanceField::initiailize(void)
	{
        InstanceSystem::ScriptManager* const pScriptManager = InstanceSystem::ScriptManager::getInstance();
		if ( NULL == pScriptManager )
			return false;

		// Level Field Script 가져오기;
        if ( -1 == pScriptManager->getLevelFieldScript(
			instanceMapID,
			keyMapID,
			luaInstance ) )
            return false;

		// Invoke Function 세팅;
        InstanceSystem::InitializeFieldInstanceInvokeFunction( luaInstance );

		// Callback Exist 확인;
        CheckFunctionExist();

		// Instance Mode 확인;
		CheckInstanceMode();

		// Initialize Callback Function 호출;
        const bool bResult = EventInitialize();
        if ( false == bResult )
        {
            InstanceField::destroyLua();
            return false;
        }
	
		// Timer 세팅;
		eventTimerVector.reserve(10);

        return true;
	}

    void InstanceField::CheckFunctionExist()
    {
        bExistFunc[EM_EVENT_INITIALIZE]                 = this->luaInstance.IsExist("EventInitialize");

        bExistFunc[EM_EVENT_REQUEST_JOIN]               = this->luaInstance.IsExist("EventRequestJoin");
        bExistFunc[EM_EVENT_REQUEST_JOIN_FORCED]        = this->luaInstance.IsExist("EventRequestJoinForced");
        bExistFunc[EM_EVENT_REQUEST_JOIN_GM]            = this->luaInstance.IsExist("EventRequestJoinGM");
        bExistFunc[EM_EVENT_REQUEST_JOIN_OBSERVER]      = this->luaInstance.IsExist("EventRequestJoinObserver");

        bExistFunc[EM_EVENT_REQUEST_MOVE_GATE]          = this->luaInstance.IsExist("EventRequestMoveGate");
        bExistFunc[EM_EVENT_OFFLINE]                    = this->luaInstance.IsExist("EventOffline");
        bExistFunc[EM_EVENT_JOIN]                       = this->luaInstance.IsExist("EventJoin");
        bExistFunc[EM_EVENT_OUT]                        = this->luaInstance.IsExist("EventOut");
        bExistFunc[EM_EVENT_MAP_ENTER]                  = this->luaInstance.IsExist("EventMapEnter");
        bExistFunc[EM_EVENT_PARTY_CHANGE_MASTER]        = this->luaInstance.IsExist("EventPartyChangeMaster");
        bExistFunc[EM_EVENT_PARTY_JOIN]                 = this->luaInstance.IsExist("EventPartyJoin");
        bExistFunc[EM_EVENT_PARTY_OUT]                  = this->luaInstance.IsExist("EventPartyOut");
        bExistFunc[EM_EVENT_TIMER]                      = this->luaInstance.IsExist("EventTimer");
        bExistFunc[EM_EVENT_UPDATE]                     = this->luaInstance.IsExist("EventUpdate");
		bExistFunc[EM_EVENT_RETRY]						= this->luaInstance.IsExist("EventRetry");
		bExistFunc[EM_EVENT_COMPLETE_RETRY]				= this->luaInstance.IsExist("EventCompleteRetry");
		bExistFunc[EM_EVENT_ALL_DIE]					= this->luaInstance.IsExist("EventAllDie");
        bExistFunc[EM_EVENT_DIE]                        = this->luaInstance.IsExist("EventDie");
        bExistFunc[EM_EVENT_RECEIVE_DAMAGE]             = this->luaInstance.IsExist("EventReceiveDamage");
        bExistFunc[EM_EVENT_RECEIVE_HEAL]               = this->luaInstance.IsExist("EventReceiveHeal");
        bExistFunc[EM_EVENT_USE_ITEM]                   = this->luaInstance.IsExist("EventUseItem");
        bExistFunc[EM_EVENT_RESURRECT]                  = this->luaInstance.IsExist("EventResurrect");
        bExistFunc[EM_EVENT_ACTOR_TRIGGER_IN]           = this->luaInstance.IsExist("EventActorTriggerIn");
        bExistFunc[EM_EVENT_ACTOR_TRIGGER_OUT]          = this->luaInstance.IsExist("EventActorTriggerOut");
        bExistFunc[EM_EVENT_CLICK_TRIGGER]              = this->luaInstance.IsExist("EventClickTrigger");
        bExistFunc[EM_EVENT_CUSTOM_MESSAGE]             = this->luaInstance.IsExist("EventCustomMessage");
        bExistFunc[EM_EVENT_CUSTOM_MESSAGE_OUTER]       = this->luaInstance.IsExist("EventCustomMessageOuter");
        bExistFunc[EM_EVENT_CUSTOM_MESSAGE_FROM_AGENT]  = this->luaInstance.IsExist("EventCustomMessageFromAgent");
		bExistFunc[EM_EVENT_SKILL]						= this->luaInstance.IsExist("EventSkill");
        bExistFunc[EM_EVENT_MOTION_FINISH]              = this->luaInstance.IsExist("EventMotionFinish");
		bExistFunc[EM_EVENT_UPDATE_VICTORIOUS_COUNTRY]	= this->luaInstance.IsExist("EventUpdateVictoriousCountry");
		
    }

	void InstanceField::CheckInstanceMode ()
	{
		if ( NULL != m_pMode )
			return;

		// Mode 가 설정되어있는지 확인한다;
		int nMode = 0;
		if ( luaInstance.GetIntegerGlobalValue_Lua( "InstanceFieldMode", nMode ) )
		{
			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( getInstanceMapID() );
			if ( NULL == pLandManager )
				return;

			EMINSTANCE_FIELD_MODE emMode = static_cast< EMINSTANCE_FIELD_MODE >( nMode );

			switch ( emMode )
			{
			case EMINSTANCE_FIELD_MODE_CERTIFICATION_MACHINE_CLUB:
				{
					// 인증기 모드의 세팅데이터를 읽는다;
					SCERTIFICATION_MACHINE_DUNGEON_INFO sDungeonInfo;
					sDungeonInfo.LoadConfig( luaInstance );

					// 클럽 인증기모드를 생성한다;
					m_pMode = new CCertificationMachineClubMode(
						gpGaeaServer,
						pLandManager,
						MapID( keyMapID, SNATIVEID( instanceMapID, 0 ) ),
						gpGaeaServer->GetInstanceManagerField(),
						this,
						&luaInstance,
						sDungeonInfo );
					if ( NULL == m_pMode )
						return;
				}	
				break;

			case EMINSTANCE_FIELD_MODE_CERTIFICATION_MACHINE_COUNTRY:
				{
					// 인증기 모드의 세팅데이터를 읽는다;
					SCERTIFICATION_MACHINE_DUNGEON_INFO sDungeonInfo;
					sDungeonInfo.LoadConfig( luaInstance );

					// 국가 인증기모드를 생성한다;
					m_pMode = new CCertificationMachineCountryMode(
						gpGaeaServer,
						pLandManager,
						MapID( keyMapID, SNATIVEID( instanceMapID, 0 ) ),
						gpGaeaServer->GetInstanceManagerField(),
						this,
						&luaInstance,
						sDungeonInfo );
					if ( NULL == m_pMode )
						return;
				}
				break;
			}
		}
		else
		{
			sc::writeLogInfo ( "[ Instance Log ] [ Don't Exist 'InstanceFieldMode' Global Value, So this Instance Dungeon is only Script Mode. ]" );
		}
	}

    const bool CALLBACK InstanceField::EventInitialize()
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
			return false;

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnInitialize();
		}

        if ( this->bExistFunc[EM_EVENT_INITIALIZE] == true )
        {
            this->luaInstance.PushFunction("EventInitialize");
            this->luaInstance.CallFunction(0, 1);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventInitialize ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			return luaInstance.PopBoolean();
        }

        return bReturn;
    }

    const bool CALLBACK InstanceField::EventUpdate(const float _fElapsedTime)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventUpdate Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnUpdate( _fElapsedTime );
		}

        if ( this->bExistFunc[EM_EVENT_UPDATE] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			//sc::writeLogInfo( "[ Instance Log ] [ Callback - EventUpdate ]" );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventUpdate");
            this->luaInstance.PushNumber(_fElapsedTime);
            this->luaInstance.CallFunction(1, 0);
        }        
        //sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
        return bReturn;
    }

    void CALLBACK InstanceField::EventTimer(const DWORD _timerHandle, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventTimer Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnTimer( _timerHandle, _dwParam1, _dwParam2, _dwParam3, _dwParam4 );
		}

        if ( this->bExistFunc[EM_EVENT_TIMER] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			//sc::writeLogInfo( "[ Instance Log ] [ Callback - EventTimer ]" );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ TimerHandle : %1% ]", _timerHandle ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Timer ID : %1% ]", _fParam1 ) );
#endif

            this->luaInstance.PushFunction("EventTimer");
            this->luaInstance.PushInteger(_timerHandle);  // Timer Handle;                        
            this->luaInstance.PushInteger(_dwParam1);  // Timer Paramater first ;
            this->luaInstance.PushInteger(_dwParam2);  // Timer Paramater second;
            this->luaInstance.PushInteger(_dwParam3);  // Timer Paramater third;
            this->luaInstance.PushInteger(_dwParam4);  // Timer Paramater fourth;
            this->luaInstance.CallFunction(5, 0);
        }
    }

    void CALLBACK InstanceField::EventActorTriggerIn(const InstanceActorTrigger& _actorTrigger, const ACTOR_VECTOR& _innerActorVector)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventActorTriggerIn Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnActorTriggerIn( _actorTrigger, _innerActorVector );
		}

        if ( bExistFunc[EM_EVENT_ACTOR_TRIGGER_IN] == true )
        {
            ACTOR_CITER actorEndIter = _innerActorVector.end();
            for ( ACTOR_CITER actoriter = _innerActorVector.begin(); actoriter != actorEndIter; actoriter++ )
            {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventActorTriggerIn ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventActorTriggerIn");
                this->luaInstance.PushInteger(_actorTrigger.actorID.actorType);
                this->luaInstance.PushInteger(_actorTrigger.actorID.actorID_Num);
                this->luaInstance.PushInteger(actoriter->actorType);
                this->luaInstance.PushInteger(actoriter->actorID_Num);
                this->luaInstance.CallFunction(4,0);
            }
        }
    }

    void CALLBACK InstanceField::EventActorTriggerOut(const InstanceActorTrigger& _actorTrigger, const ACTOR_VECTOR& _outerActorVector)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventActorTriggerOut Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnActorTriggerOut( _actorTrigger, _outerActorVector );
		}

        if ( bExistFunc[EM_EVENT_ACTOR_TRIGGER_OUT] == true )
        {
            ACTOR_CITER actorEndIter = _outerActorVector.end();
            for ( ACTOR_CITER actoriter = _outerActorVector.begin(); actoriter != actorEndIter; actoriter++ )
            {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventActorTriggerOut ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventActorTriggerOut");
                this->luaInstance.PushInteger(_actorTrigger.actorID.actorType);
                this->luaInstance.PushInteger(_actorTrigger.actorID.actorID_Num);
                this->luaInstance.PushInteger(actoriter->actorType);
                this->luaInstance.PushInteger(actoriter->actorID_Num);
                this->luaInstance.CallFunction(4,0);
            }
        }
    }

	const bool InstanceField::EventRequestJoin(const DBNUM _dbNum, const EMAUTHORITY _emAuthority)
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventRequestJoin Function!!!" );
			return false;
		}

		bool bReturn = false;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnRequestJoin( _dbNum, _emAuthority );
		}

        switch ( _emAuthority )
        {
        case EMAUTHORITY_NORMAL:
            {
                if ( bExistFunc[EM_EVENT_REQUEST_JOIN] == false )
                    return bReturn;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRequestJoin ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventRequestJoin");
            }
            break;

        case EMAUTHORITY_NORMAL_FORCED:
            {
                if ( bExistFunc[EM_EVENT_REQUEST_JOIN_FORCED] == false )
                    return bReturn;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRequestJoinForced ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventRequestJoinForced");
            }
            break;

        case EMAUTHORITY_GM:
            {
                if ( bExistFunc[EM_EVENT_REQUEST_JOIN_GM] == false )
                    return bReturn;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRequestJoinGM ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventRequestJoinGM");
            }
            break;

        case EMAUTHORITY_OBSERVER:
            {
                if ( bExistFunc[EM_EVENT_REQUEST_JOIN_OBSERVER] == false )
                    return bReturn;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRequestJoinObserver ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

                this->luaInstance.PushFunction("EventRequestJoinObserver");
            }
            break;
        }

        this->luaInstance.PushInteger(_dbNum);
        this->luaInstance.CallFunction(1, 1);
		return luaInstance.PopBoolean() ? false : true;
	}

    const bool InstanceField::EventRequestMoveGate(const DBNUM _dbNum, const SNATIVEID& _mapIDCurrent, const SNATIVEID& _mapIDTarget, const DWORD _gateID, const DWORD _gateIDTarget)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventRequestMoveGate Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnRequestMoveGate( _dbNum, _mapIDCurrent, _mapIDTarget, _gateID, _gateIDTarget );
		}

        if ( this->bExistFunc[EM_EVENT_REQUEST_MOVE_GATE] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRequestMoveGate ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventRequestMoveGate");
            this->luaInstance.PushInteger(_dbNum);
            this->luaInstance.PushInteger(_mapIDCurrent.wMainID);
            this->luaInstance.PushInteger(_mapIDCurrent.wSubID);
            this->luaInstance.PushInteger(_gateID);
			this->luaInstance.PushInteger(_mapIDTarget.wMainID);
			this->luaInstance.PushInteger(_mapIDTarget.wSubID);
			this->luaInstance.PushInteger(_gateIDTarget);
			
            this->luaInstance.CallFunction(7, 1);
			return luaInstance.PopBoolean();
        }

        return bReturn;
    }

	void InstanceField::EventOnline(const DBNUM _dbNum)
	{		
// 		MemberFieldMapIter _iteratorMember(this->memberMap.find(_dbNum));
// 		if ( _iteratorMember == this->memberMap.end() )
// 			return;
// 		
// 		MemberField& _memberField = _iteratorMember->second;		
// 
// 		const bool _bOnlined(_memberField.isState(InstanceSystem::EMMEMBER_OFFLINE) == false);
// 		_memberField.setState(InstanceSystem::EMMEMBER_OFFLINE, false);
	}

	void InstanceField::EventOffline(const DBNUM _dbNum, const bool _bReserveMember /*= false*/)
	{	
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventOffline Function!!!" );
			return;
		}

		MemberFieldMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return;		

		if ( m_pMode )
		{
			m_pMode->OnOffline( _dbNum, _bReserveMember );
		}

		if ( this->bExistFunc[EM_EVENT_OFFLINE] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventOffline ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			this->luaInstance.PushFunction("EventOffline");
			this->luaInstance.PushInteger(_dbNum);	
			this->luaInstance.CallFunction(1, 0);
		}
        else // 특별히 지정된 오프라인 이벤트가 없다면 인던 퇴장이벤트 자동 호출;
        {
            EventOut(_dbNum);
        }

		this->memberMap.erase(_dbNum);
	}

    const bool InstanceField::EventJoin(const DBNUM _dbNum, GLLandMan* _pLand, IN OUT D3DXVECTOR3& _vAdjustPos)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventJoin Function!!!" );
			return false;
		}

		bool bReturn = false;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnJoin( _dbNum, _pLand, _vAdjustPos );
		}

		if ( this->bExistFunc[EM_EVENT_JOIN] == true )
		{
			this->luaInstance.PushFunction("EventJoin");
			this->luaInstance.PushInteger(_dbNum);
			this->luaInstance.CallFunction(1,2);
			
			float fReturnValue2 = luaInstance.PopNumber();
			float fReturnValue1 = luaInstance.PopNumber();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventJoin ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			// 리턴 값이 없다면 초기 위치 변경 안함;
			if ( sc::LuaInstance::INVAID_POP_FLOAT == fReturnValue1 &&
				sc::LuaInstance::INVAID_POP_FLOAT == fReturnValue2 )
			{
				return false;
			}
			// 리턴 값이 하나일 경우 게이트 번호를 뜻함;
			else if ( sc::LuaInstance::INVAID_POP_FLOAT == fReturnValue2 )
			{
				const int _nGateID = static_cast< const int >( fReturnValue1 );

				if ( _pLand )
				{
					DxLandGateMan* pGateMan = _pLand->GetLandGateMan();
					if ( pGateMan )
					{
						PDXLANDGATE pGate = pGateMan->FindLandGate((DWORD)_nGateID);
						if ( pGate )
						{
							_vAdjustPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
							return true;
						}
					}
				}
			}
			else
			{
				// 리턴 값이 2개일 때는 월드 좌표를 뜻한다.;
				const float _fPositionX = fReturnValue1;
				const float _fPositionZ = fReturnValue2;

				_vAdjustPos.x = _fPositionX;
				_vAdjustPos.z = _fPositionZ;
				return true;
			}
		}

		return bReturn;
    }

    void InstanceField::EventOut(const DBNUM _dbNum)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventOut Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnOut( _dbNum );
		}

        if ( this->bExistFunc[EM_EVENT_OUT] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventOut ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventOut");
            this->luaInstance.PushInteger(_dbNum);
            this->luaInstance.CallFunction(1, 0);
        }
    }

    void InstanceField::EventMapEnter(const DBNUM _dbNum, const SNATIVEID& _baseMapID)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventMapEnter Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnMapEnter( _dbNum, _baseMapID );
		}

        if ( this->bExistFunc[EM_EVENT_MAP_ENTER] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventMapEnter ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventMapEnter");
            this->luaInstance.PushInteger(_dbNum);
            this->luaInstance.PushInteger(_baseMapID.wMainID);
            this->luaInstance.PushInteger(_baseMapID.wSubID);
            this->luaInstance.CallFunction(3, 0);
        }		
    }

	void InstanceField::EventPartyChangeMaster(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _newMasterDbNum, const bool _bInSameInstanceNewMaster, const DBNUM _oldMasterDbNum, const bool _bInSameInstanceOldMaster)
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventPartyChangeMaster Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnPartyChangeMaster( _partyID, _bExpedition, _newMasterDbNum, _bInSameInstanceNewMaster, _oldMasterDbNum, _bInSameInstanceOldMaster );
		}

        if ( this->bExistFunc[EM_EVENT_PARTY_CHANGE_MASTER] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventPartyChangeMaster ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif
            this->luaInstance.PushFunction("EventPartyChangeMaster");
			this->luaInstance.PushInteger(_partyID.GetID());
			this->luaInstance.PushBoolean(_bExpedition);
			this->luaInstance.PushInteger(_newMasterDbNum);
			this->luaInstance.PushBoolean(_bInSameInstanceNewMaster);
			this->luaInstance.PushInteger(_oldMasterDbNum);
			this->luaInstance.PushBoolean(_bInSameInstanceOldMaster);
			this->luaInstance.CallFunction(6, 0);
		}
	}

	void InstanceField::EventPartyJoin(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _joinerDBNum, const bool _bInSameInstance)
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventPartyJoin Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnPartyJoin( _partyID, _bExpedition, _joinerDBNum, _bInSameInstance );
		}

        if ( this->bExistFunc[EM_EVENT_PARTY_JOIN] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventPartyJoin ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventPartyJoin");
			this->luaInstance.PushInteger(_partyID.GetID());
			this->luaInstance.PushBoolean(_bExpedition);
			this->luaInstance.PushInteger(_joinerDBNum);
			this->luaInstance.PushBoolean(_bInSameInstance);
			this->luaInstance.CallFunction(4, 0);
		}
	}
	void InstanceField::EventPartyOut(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _outerDBNum, const bool _bInSameInstance)
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventPartyOut Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnPartyOut( _partyID, _bExpedition, _outerDBNum, _bInSameInstance );
		}

        if ( this->bExistFunc[EM_EVENT_PARTY_OUT] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventPartyOut ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventPartyOut");
			this->luaInstance.PushInteger(_partyID.GetID());
			this->luaInstance.PushBoolean(_bExpedition);
			this->luaInstance.PushInteger(_outerDBNum);
			this->luaInstance.PushBoolean(_bInSameInstance);
			this->luaInstance.CallFunction(4, 0);
		}
	}
	
    void InstanceField::EventClickTrigger(const DWORD _emNpcType, const DWORD _dwNpcID, const DWORD _emClickActorType, const DWORD _dwClickActorID)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventClickTrigger Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnClickTrigger( _emNpcType, _dwNpcID, _emClickActorType, _dwClickActorID );
		}

        if ( this->bExistFunc[EM_EVENT_CLICK_TRIGGER] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventClickTrigger ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventClickTrigger");
            this->luaInstance.PushInteger(_emNpcType);
            this->luaInstance.PushInteger(_dwNpcID);
            this->luaInstance.PushInteger(_emClickActorType);
            this->luaInstance.PushInteger(_dwClickActorID);
            this->luaInstance.CallFunction(4, 0);
        }
    }

	void InstanceField::EventRetryProc (
		const DWORD _dwChaNum,
		bool _bAccept,
		const DWORD _dwResultFlag )
	{
		if ( m_pMode )
		{
			m_pMode->OnRetryProc( _dwChaNum, _bAccept, _dwResultFlag );
		}

		if ( bExistFunc[ EM_EVENT_RETRY ] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventRetry ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			luaInstance.PushFunction( "EventRetry" );
			luaInstance.PushInteger( _dwChaNum );
			luaInstance.PushBoolean( _bAccept );
			luaInstance.PushInteger( _dwResultFlag );

			luaInstance.CallFunction( 3, 0 );
		}
	}

	const bool InstanceField::EventCompleteRetryProc (
		const DWORD _emFactionType,
		const DWORD _dwFactionID,
		bool _bSuccess,
		const DWORD _dwResultFlag )
	{
		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnCompleteRetryProc( _emFactionType, _dwFactionID, _bSuccess, _dwResultFlag );
		}

		if ( bExistFunc[ EM_EVENT_COMPLETE_RETRY ] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventCompleteRetry ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			luaInstance.PushFunction( "EventCompleteRetry" );
			luaInstance.PushInteger( _emFactionType );
			luaInstance.PushInteger( _dwFactionID );
			luaInstance.PushBoolean( _bSuccess );
			luaInstance.PushInteger( _dwResultFlag );

			luaInstance.CallFunction( 4, 1 );

			return luaInstance.PopBoolean();
		}

		return bReturn;
	}

	const bool InstanceField::EventAllDie (
		const DWORD _emFactionType,
		const DWORD _dwFactionID )
	{
		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnAllDie( _emFactionType, _dwFactionID );
		}

		if ( bExistFunc[ EM_EVENT_ALL_DIE ] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventAllDie ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			luaInstance.PushFunction( "EventAllDie" );
			luaInstance.PushInteger( _emFactionType );
			luaInstance.PushInteger( _dwFactionID );

			luaInstance.CallFunction( 2, 1 );

			return luaInstance.PopBoolean();
		}

		return bReturn;
	}

    const bool InstanceField::EventDie(const DWORD _emDie_ActorType, const DWORD _dwDie_ActorID, const DWORD _emKill_ActorType, const DWORD _dwKill_ActorID)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventDie Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnDie( _emDie_ActorType, _dwDie_ActorID, _emKill_ActorType, _dwKill_ActorID );
		}

        if ( this->bExistFunc[EM_EVENT_DIE] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventDie ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventDie");
            this->luaInstance.PushInteger(_emDie_ActorType);
            this->luaInstance.PushInteger(_dwDie_ActorID);
            this->luaInstance.PushInteger(_emKill_ActorType);
            this->luaInstance.PushInteger(_dwKill_ActorID);
            this->luaInstance.CallFunction(4, 1);
			
			return luaInstance.PopBoolean();
        }
        return bReturn;
    }

    // 주의! : _dwDamage 를 reference로 받아서 script에서 실제 가해지는 데미지를 변경할수 있게 함;
    //         _bResult 값이 false라면 기존 데미지 처리를 하지 말라는 뜻;
    const bool InstanceField::EventReceiveDamage(const DWORD _emDamaged_ActorType, const DWORD _dwDamaged_ActorID, const DWORD _emAttack_ActorType, const DWORD _dwAttack_ActorID, DWORD& _dwDamage, const DWORD _dwDamageFlag)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventReceiveDamage Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnReceiveDamage( _emDamaged_ActorType, _dwDamaged_ActorID, _emAttack_ActorType, _dwAttack_ActorID, _dwDamage, _dwDamageFlag );
		}

        if ( this->bExistFunc[EM_EVENT_RECEIVE_DAMAGE] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventReceiveDamage ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			// Lua Function 호출;
            this->luaInstance.PushFunction( "EventReceiveDamage" );
            this->luaInstance.PushInteger( _emDamaged_ActorType );
            this->luaInstance.PushInteger( _dwDamaged_ActorID );
            this->luaInstance.PushInteger( _emAttack_ActorType );
            this->luaInstance.PushInteger( _dwAttack_ActorID );
            this->luaInstance.PushInteger( _dwDamage );
            this->luaInstance.PushInteger( _dwDamageFlag );
            this->luaInstance.CallFunction( 6, 2 );

			// Return 값 받아오기;
			DWORD dwChangeDamage = luaInstance.PopInteger();
			if ( sc::LuaInstance::INVALID_POP_INTEGER == dwChangeDamage )
				dwChangeDamage = 0;
			else
				_dwDamage = dwChangeDamage;

			return luaInstance.PopBoolean();
        }

        return bReturn;
    }

    // 주의! : _dwHeal 를 reference로 받아서 script에서 실제 가해지는 회복량을 변경할수 있게 함;
    //         _bResult 값이 false라면 기존 회복 처리를 하지 말라는 뜻;
    const bool InstanceField::EventReceiveHeal(const DWORD _emReceiveHeal_ActorType, const DWORD _dwReceiveHeal_ActorID, const DWORD _emHealer_ActorType, const DWORD _dwHealer_ActorID, DWORD& _dwHeal, const DWORD _dwHealFlag)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventReceiveHeal Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnReceiveHeal( _emReceiveHeal_ActorType, _dwReceiveHeal_ActorID, _emHealer_ActorType, _dwHealer_ActorID, _dwHeal, _dwHealFlag );
		}

        if ( this->bExistFunc[EM_EVENT_RECEIVE_HEAL] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventReceiveHeal ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventReceiveHeal");
            this->luaInstance.PushInteger(_emReceiveHeal_ActorType);
            this->luaInstance.PushInteger(_dwReceiveHeal_ActorID);
            this->luaInstance.PushInteger(_emHealer_ActorType);
            this->luaInstance.PushInteger(_dwHealer_ActorID);
            this->luaInstance.PushInteger(_dwHeal);
            this->luaInstance.PushInteger(_dwHealFlag);
			this->luaInstance.CallFunction( 6, 2 );

			// Return 값 받아오기;
			DWORD dwChangeHeal = luaInstance.PopInteger();
			if ( sc::LuaInstance::INVALID_POP_INTEGER == dwChangeHeal )
				dwChangeHeal = 0;
			else
				_dwHeal = dwChangeHeal;

			return luaInstance.PopBoolean();
        }
        return bReturn;
    }

    const bool InstanceField::EventUseItem(const DWORD _dwActorID, const DWORD _itemMID, const DWORD _itemSID, const DWORD _nParam, const float _fParam)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventUseItem Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnUseItem( _dwActorID, _itemMID, _itemSID, _nParam, _fParam );
		}

        if ( this->bExistFunc[EM_EVENT_USE_ITEM] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventUseItem ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventUseItem");
            this->luaInstance.PushInteger(_dwActorID);
            this->luaInstance.PushInteger(_itemMID);
            this->luaInstance.PushInteger(_itemSID);
			this->luaInstance.PushInteger(_nParam);
			this->luaInstance.PushNumber(_fParam);
            this->luaInstance.CallFunction(5, 1);

			return luaInstance.PopBoolean();
        }
        return bReturn;
    }

    const bool InstanceField::EventResurrect(const DWORD _emResurrectType, const DWORD _emDie_ActorType, const DWORD _dwDie_ActorID, const DWORD _emKill_ActorType, const DWORD _dwKill_ActorID)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventResurrect Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnResurrect( _emResurrectType, _emDie_ActorType, _dwDie_ActorID, _emKill_ActorType, _dwKill_ActorID );
		}

        if ( this->bExistFunc[EM_EVENT_RESURRECT] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventResurrect ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction("EventResurrect");
            this->luaInstance.PushInteger(_emResurrectType);
            this->luaInstance.PushInteger(_emDie_ActorType);
            this->luaInstance.PushInteger(_dwDie_ActorID);
            this->luaInstance.PushInteger(_emKill_ActorType);
            this->luaInstance.PushInteger(_dwKill_ActorID);
            this->luaInstance.CallFunction(5, 1);

			return luaInstance.PopBoolean();
        }
        return bReturn;
    }

	const bool InstanceField::EventSkill(const DWORD _dwActorType, const DWORD _dwActorID)
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventSkill Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnSkill( _dwActorType, _dwActorID );
		}

		if ( this->bExistFunc[EM_EVENT_SKILL] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventSkill ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			this->luaInstance.PushFunction("EventSkill");
			this->luaInstance.PushInteger(_dwActorType);
			this->luaInstance.PushInteger(_dwActorID);
			this->luaInstance.CallFunction(2, 1);

			return luaInstance.PopBoolean();
		}
		return bReturn;
	}

    void CALLBACK InstanceField::EventMotionFinish(const DWORD _dwActorType, const DWORD _dwActorID, const DWORD _dwMotionMID, const DWORD _dwMotionSID, const float _fRemainSec)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventMotionFinish Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnMotionFinish( _dwActorType, _dwActorID, _dwMotionMID, _dwMotionSID, _fRemainSec );
		}

        if ( this->bExistFunc[EM_EVENT_MOTION_FINISH] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventMotionFinish ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction( "EventMotionFinish" );
            this->luaInstance.PushInteger(_dwActorType);
            this->luaInstance.PushInteger(_dwActorID);
            this->luaInstance.PushInteger(_dwMotionMID);
            this->luaInstance.PushInteger(_dwMotionSID);
            this->luaInstance.PushNumber(_fRemainSec);
            this->luaInstance.CallFunction(5, 0);
        }
    }

    void InstanceField::EventCustomMessage(const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4)
    {
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventCustomMessage Function!!!" );
			return;
		}

		if ( m_pMode )
		{
			m_pMode->OnCustomMessage( _emSrcType, _dwSrcID, _Param1, _Param2, _Param3, _Param4 );
		}

        if ( this->bExistFunc[EM_EVENT_CUSTOM_MESSAGE] == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventCustomMessage ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction( "EventCustomMessage" );
            this->luaInstance.PushInteger((int)_emSrcType);
            this->luaInstance.PushInteger(_dwSrcID);
            this->luaInstance.PushInteger(_Param1);
            this->luaInstance.PushInteger(_Param2);
            this->luaInstance.PushInteger(_Param3);
            this->luaInstance.PushInteger(_Param4);
            this->luaInstance.CallFunction(6, 0);
        }
    }

	bool CALLBACK InstanceField::EventUpdateVictoriousCountry ( DWORD dwContentID, DWORD dwPreCountryID, DWORD dwCurCountryID )
	{
		// Instance Field 삭제 대기 상태라면 아무것도 하지 않는다;
		if ( m_bDeleteInstance )
		{
			sc::writeLogError ( "Deleting Instance Field... Failed Call EventUpdateVictoriousCountry Function!!!" );
			return false;
		}

		bool bReturn = true;
		if ( m_pMode )
		{
			bReturn = m_pMode->OnUpdateVictoriousCountry( dwContentID, dwPreCountryID, dwCurCountryID );
		}

		if ( this->bExistFunc[EM_EVENT_UPDATE_VICTORIOUS_COUNTRY] == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - EventUpdateVictoriousCountry ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			this->luaInstance.PushFunction( "EventUpdateVictoriousCountry" );
			this->luaInstance.PushInteger( dwContentID );
			this->luaInstance.PushInteger( dwPreCountryID );
			this->luaInstance.PushInteger( dwCurCountryID );
			this->luaInstance.CallFunction( 3, 0 );
		}
		
		return bReturn;
	}
}
