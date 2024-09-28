#include "pch.h"

#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../enginelib/DxLand/DxLandDef.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/Msg/GLContrlMatching.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Msg/GLContrlMatching.h"
#include "../../InstantMap/InstantMapTypes.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "InvokeFunctionField.h"
#include "InstanceSystemField.h"

namespace InstanceSystem
{
	GLGaeaServer* gpGaeaServer = 0;

    InstanceActorTrigger::InstanceActorTrigger(const InstanceMapID& _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius)
        : instanceMapID(_instanceMapID)
        , actorID(_actorID)
        , triggerType(_triggerType)
        , fRadiusSqrt(_fRadius * _fRadius)
        , rangeType(EM_TRIGGER_SPHERE)
        , fZoneX(0.0f)
        , fZoneZ(0.0f)
        , fZoneY(0.0f)
    {   
    }
    InstanceActorTrigger::InstanceActorTrigger(const InstanceMapID& _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY)
        : instanceMapID(_instanceMapID)
        , actorID(_actorID)
        , triggerType(_triggerType)
        , fRadiusSqrt(0.0f)
        , rangeType(EM_TRIGGER_CUBE)
        , fZoneX(_fZoneX)
        , fZoneZ(_fZoneZ)
        , fZoneY(_fZoneY)
    {   
    }

    const bool InstanceActorTrigger::updateAround(OUT ACTOR_VECTOR& _inActor, OUT ACTOR_VECTOR& _outActor)
    {
		if ( ( innerActorSet.begin() != innerActorSet.end() ) && ( innerActorSet.empty() ) )
		{
			GASSERT(0&&"wtf!!!");
		}
        std::set<Faction::GLActorID> _actorSet;

        GLLandManager* pLandMan = gpGaeaServer->GetLandManager(instanceMapID);
        if ( pLandMan == NULL )
            return false;

        switch ( actorID.actorType )
        {
        case CROW_PC:
            {
                GLChar* const pChar(gpGaeaServer->GetCharByDbNum(actorID.actorID_Num));
                if ( pChar == NULL )
                    return false;

                GLLandMan* pLand = pChar->GetLandMan();
                if ( pLand == NULL )
                    return false;
                
                GLChar::VIEW_CROW_MAP_CITER crowBeginCIter = pChar->GetViewCrowBegin();
                GLChar::VIEW_CROW_MAP_CITER crowEndCIter = pChar->GetViewCrowEnd();
                for ( GLChar::VIEW_CROW_MAP_CITER crowCIter = crowBeginCIter; crowCIter != crowEndCIter; crowCIter++ )
                {
                    const GLChar::CROW_PAIR &crowPair = crowCIter->first;
                    ServerActor* _pActor = gpGaeaServer->GetTarget(pLand, STARGETID(crowPair.first, crowPair.second));
                    if ( _pActor == NULL )
                        continue;

                    switch ( _pActor->GetCrow() )
                    {
                        case CROW_PC:
                            if (!(triggerType & EM_TRIGGER_EVENT_PC))
                                continue;
                            break;
                        case CROW_MOB:
                            if (!(triggerType & EM_TRIGGER_EVENT_MOB))
                                continue;
                            break;
                        case CROW_NPC:
                            if (!(triggerType & EM_TRIGGER_EVENT_NPC))
                                continue;
                            break;
                        case CROW_SUMMON:
                            if (!(triggerType & EM_TRIGGER_EVENT_SUMMON))
                                continue;
                            break;
                    }

                    // Ʈ���� �ݰ濡 ���Դ��� �˻�;
                    if ( isInside(pChar, _pActor) )
                    {
                        //! ����: GetView������ PC�� GaeaID���� ActorSet�� �������� Dbnum���� �����Ѵ�.
                        //! (��ũ��Ʈ������ Pc�� ��� DbNum�� ����ϱ� ����;
                        _actorSet.insert(Faction::GLActorID( _pActor->GetCrow(), _pActor->GetCharID()));
                    }
                }
            }
            break;
        case CROW_NPC:
        case CROW_MOB:
            {
                GLCrow* pCrow = pLandMan->getCrow(this->actorID.actorID_Num);
                if (pCrow == NULL)
                    return false;

                GLLandMan* pLand = pCrow->GetLandMan();
                if ( pLand == NULL )
                    return false;

                if ( triggerType & EM_TRIGGER_EVENT_PC )
                {
                    GLCrow::VECPC_CITER pcCIter = pCrow->getViewPcBegin();
                    GLCrow::VECPC_CITER pcEndCIter = pCrow->getViewPcEnd();       
                    const DWORD dwPcAmount = pCrow->getViewPcAmount();
                    for ( DWORD i = 0; (pcCIter != pcEndCIter) && (i < dwPcAmount); pcCIter++, i++ )
                    {
                        ServerActor* _pActor = gpGaeaServer->GetTarget(pLand, STARGETID(CROW_PC, *pcCIter));
                        if ( _pActor == NULL )
                            continue;

                        // Ʈ���� �ݰ濡 ���Դ��� �˻�;
                        if ( isInside(pCrow, _pActor) )
                        {
                            //! ����: GetView������ PC�� GaeaID���� ActorSet�� �������� Dbnum���� �����Ѵ�.
                            //! (��ũ��Ʈ������ Pc�� ��� DbNum�� ����ϱ� ����;
                            _actorSet.insert(Faction::GLActorID( _pActor->GetCrow(), _pActor->GetCharID()));
                        }
                    }
                }

                if ( triggerType & (EM_TRIGGER_EVENT_MOB | EM_TRIGGER_EVENT_NPC) )
                {
                    if ( (actorID.actorType == CROW_NPC) && (triggerType & EM_TRIGGER_EVENT_NPC) )
                        break;
                    if ( (actorID.actorType == CROW_MOB) && (triggerType & EM_TRIGGER_EVENT_MOB) )
                        break;

                    GLCrow::VECPC_CITER mobCIter = pCrow->getViewMobBegin();
                    GLCrow::VECPC_CITER mobEndCIter = pCrow->getViewMobEnd();       
                    const DWORD dwMobAmount = pCrow->getViewMobAmount();
                    for ( DWORD i = 0; (mobCIter != mobEndCIter) && (i < dwMobAmount); mobCIter++, i++ )
                    {
                        GLCrow* pInnerCrow = pLandMan->getCrow(*mobCIter);
                        if ( pInnerCrow == NULL )
                            continue;

                        switch ( pInnerCrow->GetCrow() )
                        {
                        case CROW_NPC:
                            if (!(triggerType & EM_TRIGGER_EVENT_NPC))
                                continue;
                            break;
                        case CROW_MOB:
                            if (!(triggerType & EM_TRIGGER_EVENT_MOB))
                                continue;
                            break;
                        }

                        // Ʈ���� �ݰ濡 ���Դ��� �˻�;
                        if ( isInside(pCrow, pInnerCrow) )
                        {
                            //! ����: GetView������ PC�� GaeaID���� ActorSet�� �������� Dbnum���� �����Ѵ�.
                            //! (��ũ��Ʈ������ Pc�� ��� DbNum�� ����ϱ� ����;
                            _actorSet.insert(Faction::GLActorID( pInnerCrow->GetCrow(), pInnerCrow->GetCharID()));
                        }
                    }
                }

                if ( triggerType & EM_TRIGGER_EVENT_SUMMON )
                {
                    GLCrow::VECPC_CITER summonCIter = pCrow->getViewSummonBegin();
                    GLCrow::VECPC_CITER summonEndCIter = pCrow->getViewSummonEnd();       
                    const DWORD dwSummonAmount = pCrow->getViewSummonAmount();
                    for ( DWORD i = 0; (summonCIter != summonEndCIter) && (i < dwSummonAmount); summonCIter++, i++ )
                    {
                        ServerActor* _pActor = gpGaeaServer->GetTarget(pLand, STARGETID(CROW_SUMMON, *summonCIter));
                        if ( _pActor == NULL )
                            continue;

                        // Ʈ���� �ݰ濡 ���Դ��� �˻�;
                        if ( isInside(pCrow, _pActor) )
                        {
                            //! ����: GetView������ PC�� GaeaID���� ActorSet�� �������� Dbnum���� �����Ѵ�.
                            //! (��ũ��Ʈ������ Pc�� ��� DbNum�� ����ϱ� ����;
                            _actorSet.insert(Faction::GLActorID( _pActor->GetCrow(), _pActor->GetCharID()));
                        }
                    }
                }
            }
            break;
        }

        bool bChange(false);

		// ���� ������ ���� �Ѵ� ���Ͱ� �������� �ʴ´ٸ� ���� ���ŷӰ� �˻����� ����;
		if ( _actorSet.empty() && innerActorSet.empty() )
			return false;
		
		//���� �ݰ濡 �ִ� ���͵�� �� �˻�;
		for ( INNER_ACTOR_CITER _actorSetIter = _actorSet.begin(); _actorSetIter != _actorSet.end(); _actorSetIter++)
		{
			// ������ ���� �¿� �������� �ʴ� ������ ��� ( ���� �ݰ濡 ���� ���� );
			if ( innerActorSet.erase(*_actorSetIter) == 0 )
			{
				bChange = true;
				_inActor.push_back(*_actorSetIter);
			}
		}

		// �������� ��� ���� ����;
		for ( INNER_ACTOR_CITER innerIter = innerActorSet.begin(); innerIter != innerActorSet.end(); innerIter++)
		{
			bChange = true; 
			_outActor.push_back(*innerIter);
		}

        // ���� �˻縦 ���� ���� �ݰ�ȿ� ����ִ� ���ͼ� ����;
		innerActorSet = _actorSet;
        return bChange;
    }

    const bool InstanceActorTrigger::isInside(const ServerActor* _triggerActor, const ServerActor* _inActor)
    {
        // Ʈ���� �ݰ��� ������� �˻�;
        switch( rangeType )
        {
        case EM_TRIGGER_SPHERE:
            {
                D3DXVECTOR3 _actorGapVec(_triggerActor->GetPosition() - _inActor->GetPosition());
                const float _actorDistance(D3DXVec3LengthSq(&_actorGapVec));

                if ( _actorDistance > fRadiusSqrt )
                    return false;
            }
            break;            

        case EM_TRIGGER_CUBE:
            {
                const D3DXVECTOR3& _triggerPos(_triggerActor->GetPosition());
                const D3DXVECTOR3& _actorPos(_inActor->GetPosition());

                if ( abs(_triggerPos.x - _actorPos.x) > fZoneX )
                    return false;

                if ( abs(_triggerPos.z - _actorPos.z) > fZoneZ )
                    return false;

                if ( abs(_triggerPos.y - _actorPos.y) > fZoneY )
                    return false;
            }
            break;

        default:
            return false;
        }
        return true;
    }





    MemberField::MemberField(const DBNUM _dbNum)
		: dbNum(_dbNum)
	{
	}
	MemberField::MemberField(const MemberField& _rhs)
		: dbNum(_rhs.dbNum)
	{
	}
	MemberField::~MemberField(void)
	{
	}
	void MemberField::frameMove(const float _fTime, const float _fElapsedTime)
	{
	}






	InstanceField::InstanceField(const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType)
        : instanceMapID(_instanceMapID)
        , keyMapID(_keyMapID)
		, emInstanceType(_emInstanceType)
        , maxTimer(0)
		, m_bDeleteInstance ( false )
		, m_pMode ( NULL )
	{
		this->childInstanceIDVector.reserve(MAX_LAND_SID);
		for ( unsigned int _i(MAX_LAND_SID); _i--; )
			this->childInstanceIDVector.push_back(SNATIVEID(false));
	}
	InstanceField::InstanceField(const InstanceField& _rhs)
		: instanceMapID(_rhs.instanceMapID)
		, childInstanceIDVector(_rhs.childInstanceIDVector)
		, instanceTimerMap(_rhs.instanceTimerMap)
		//, timerHandleQueue(_rhs.timerHandleQueue)
        , maxTimer(_rhs.maxTimer)
		, keyMapID(_rhs.keyMapID)
		, emInstanceType(_rhs.emInstanceType)
		, luaInstance(_rhs.luaInstance)
		, memberMap(_rhs.memberMap)
        , fUpdateTimer(0.0f)
		, m_bDeleteInstance ( false )
		, m_pMode ( NULL )
	{
	}
	InstanceField::~InstanceField(void)
	{
	}
    
	void InstanceField::destroyLua(void)
	{
		if ( m_bDeleteInstance )
		{
			SAFE_DELETE( m_pMode );

			luaInstance.OnDestroy();

			return;
		}

		// ����� �� ����� ���� �� ����;
		sc::writeLogError ( "[ Instance Log ] [ Failed Destroy Lua!!!!! ]" );
	}

	struct Obj
	{
		std::set<int> a;
		int b;
	};

	GLLandMan* InstanceField::GetLand()
	{
		GLLandManager* pLandManager = gpGaeaServer->GetLandManager( getInstanceMapID() );
		if ( NULL == pLandManager )
			return NULL;

		GLLandMan* pLand = pLandManager->getLand( getKeyMapID() );
		if ( NULL == pLand )
			return NULL;

		return pLand;
	}

	void InstanceField::frameMove(const float _fTime, const float _fElapsedTime)
	{
		// Instance Field ���� ��� ���¶�� �ƹ��͵� ���� �ʴ´�;
		if ( m_bDeleteInstance )
			return;

		/// �絵�� ó��;
		for ( int i=0; i<m_vecFactionRetry.size(); ++i )
		{
			SRETRY_INFO& sRetry = m_vecFactionRetry.at( i );
			if ( false == sRetry.bRetry )
				continue;

			sRetry.fTime += _fElapsedTime;
			if ( sRetry.fTime >= sRetry.fRetryTime )
			{
				sRetry.fTime = 0.f;
				sRetry.bRetry = false;
				sRetry.bSuccess = false;
				sRetry.emResult = SRETRY_INFO::EMRETRY_RESULT_TIMEOVER;

				EventRetryProc(
					0,
					false,
					SRETRY_INFO::EMRETRY_RESULT_TIMEOVER );

				// �絵�� �ݹ� ����;
				// �ð��� �ٵǾ��ٴ� ���� �����ߴٴ� ���̴�;
				EventCompleteRetryProc(
					sRetry.sFactionID.factionType,
					sRetry.sFactionID.factionID_Num,
					false,
					SRETRY_INFO::EMRETRY_RESULT_TIMEOVER );
			}
		}


		/// Ÿ�̸� ó��;
        this->fUpdateTimer += _fElapsedTime;
        if ( this->fUpdateTimer > DEFINSTANCE_UPDATE_TIMER )
        {
            for ( MemberFieldMapIter _iterator(this->memberMap.begin()); _iterator != this->memberMap.end(); ++_iterator )
            {
                MemberField& _member= _iterator->second;
                _member.frameMove(_fTime, this->fUpdateTimer);
            }
            InstanceField::EventUpdate(this->fUpdateTimer);

            this->fUpdateTimer = 0.0f;
        }
        InstanceField::updateTimer(_fElapsedTime);


		/// Ʈ���� ó��;
		while( ActorTriggerReserveQueue.empty() == false )
		{
			const InstanceActorTriggerReserveInfo& _reserveAction = ActorTriggerReserveQueue.front();
			switch( _reserveAction.emReserveAction )
			{
			case InstanceActorTriggerReserveInfo::EM_RESERVE_REGIST_NOMAL:
				{
					registActorTrigger(_reserveAction._actorID, _reserveAction._triggerType, _reserveAction._fRadius);
				}
				break;
			case InstanceActorTriggerReserveInfo::EM_RESERVE_REGIST_ZONE:
				{
					registActorTrigger(_reserveAction._actorID, _reserveAction._triggerType, _reserveAction._fZoneX, _reserveAction._fZoneZ, _reserveAction._fZoneY);
				}
				break;
			case InstanceActorTriggerReserveInfo::EM_RESERVE_UNREGIST:
				{
					removeActorTrigger(_reserveAction._actorID);
				}
				break;
			}
			ActorTriggerReserveQueue.pop();
		}

        InstanceActorTriggerVectorIter iter = instanceActorTriggerVector.begin();
        for( ; iter != instanceActorTriggerVector.end(); iter++ )
        {
            ACTOR_VECTOR inActor;
            ACTOR_VECTOR outActor;
            InstanceActorTrigger& actorTrigger = (*iter);

            // �����ų� ���� �÷����̰� ����;
            if ( actorTrigger.updateAround(inActor, outActor) )
            {
                EventActorTriggerIn(actorTrigger, inActor);
                EventActorTriggerOut(actorTrigger, outActor);
            }
        }
	}

	void InstanceField::ReserveDestroy ()
	{
		m_bDeleteInstance = true;
	}

	void InstanceField::CancelReserveDestroy ()
	{
		m_bDeleteInstance = false;
	}

    void InstanceField::updateTimer(const float _fElapsedTime)
    {
		// Instance Field ���� ��� ���¶�� �ƹ��͵� ���� �ʴ´�;
		if ( m_bDeleteInstance )
			return;


		// EventTimer�� eventTimerVector�� ����Ͽ� ȣ�� �ϴ� ������
		// instanceTimerMap for���ȿ��� EventTimerȣ�� �� ��ũ��Ʈ����
		// RemoveTimer�� �Ͽ����� Map�� Ư���� ������ ������ �Ͼ�� ���� Iterater�� ������ �Ǵµ�
		// ��ũ��Ʈ���� ȣ��� �Լ��� erase���� ����� Iter�� �������� ���� ���� ����;
		// ���� regist,remove�� Queueing�� �ϴ��� EventTimer�� Queueing�� �ϴ��� �ؾ��߱� ����;

		// ID�� ������ �Ǵ� Timer Map�� Ÿ�̸ӵ��� �ð��� ��� �Ǿ����� �˻�;
        for ( InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.begin()); _iteratorTimer != this->instanceTimerMap.end(); )
        {
            InstanceTimer& _instanceTimer = _iteratorTimer->second;
            if ( _instanceTimer.updateTimer(_fElapsedTime) == false )
            {
                ++_iteratorTimer;
                continue;
            }

            const TimerHandle _hTimer(_iteratorTimer->first);
			eventTimerVector.push_back( _iteratorTimer->second );

            // ���� LoopTimer�� ���� ��� ����ǹǷ� ������ �ʴ´�.( 0��¥�� �ν��Ͻ� Ÿ�̸Ӵ� Loop������ (����ó��) );
            if ( _instanceTimer.bLoop && _instanceTimer.fLifeTime != 0.0f)
            {
                _instanceTimer.fElapsedTime = _instanceTimer.fLifeTime;
				++_iteratorTimer;
            }
            else
            {
                //Ÿ�̸Ӵ� �ѹ� ���ǰ� ���� ������;
                //this->timerHandleQueue.push(_hTimer);
                _iteratorTimer = this->instanceTimerMap.erase(_iteratorTimer);

				// ������ Timer�� ������ �����ش�;
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				sc::writeLogInfo( "[ Instance Log ] [ Automatic Erase Timer ( InstanceField::updateTimer ) ]" );
				sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ TimerHandle : %1% ]", _hTimer ) );
#endif
            }
        }

		if ( eventTimerVector.empty() == false )
		{
			for( std::vector< InstanceTimer >::iterator iter = eventTimerVector.begin(); iter != eventTimerVector.end(); iter++ )
			{
				InstanceTimer& _instanceTimer = *iter;
				EventTimer(_instanceTimer.hTimerHandle, _instanceTimer.m_dwParam0, _instanceTimer.m_dwParam1, _instanceTimer.m_dwParam2, _instanceTimer.m_dwParam3);
			}
			eventTimerVector.clear();
		}
    }


	void InstanceField::RetryFaction ( SRETRY_INFO sRetryInfo )
	{
		GLLandMan* pLand = GetLand();
		if ( NULL == pLand )
			return;

		// ���� ��Ƽ�� ���� Faction�� ��� ���� ���¶�� ȣ���Ѵ�;
		Faction::ManagerField* pFactionManager = pLand->GetFactionManager();
		if ( NULL == pFactionManager )
			return;

		// ���� ������ ��� ��ο��� ������;
		GLMSG::NET_RETRY_FACTION_FB NetMsgBRD( getInstanceMapID(), getKeyMapID() );
		NetMsgBRD.fTime = sRetryInfo.fRetryTime;
		NetMsgBRD.dwMoney = sRetryInfo.dwMoney;

		pFactionManager->sendMessageToFaction( sRetryInfo.sFactionID, &NetMsgBRD );

		// �絵�� �߰�;
		for ( int i=0; i<m_vecFactionRetry.size(); ++i )
		{
			SRETRY_INFO& sTempRetryInfo = m_vecFactionRetry.at( i );
			if ( sTempRetryInfo.sFactionID == sRetryInfo.sFactionID )
			{
				sTempRetryInfo = sRetryInfo;

				return;
			}
		}
		
		m_vecFactionRetry.push_back( sRetryInfo );
	}

	void InstanceField::ShowTimeUI ( const DWORD _dwChaNum, const float _fSec, const bool bShow )
	{
		GLChar* pChar = gpGaeaServer->GetCharByDbNum( _dwChaNum );
		if ( NULL == pChar )
			return;

		GLMSG::NET_SHOW_TIME_UI_FB sMsg( getInstanceMapID(), getKeyMapID() );
		sMsg.bShow = bShow;
		sMsg.fSec = _fSec;

		gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );
	}

	void InstanceField::ShowEntranceStateUI (
		const Faction::FactionID _sFactionID,
		const DWORD _dwCurCount,
		const DWORD _dwMaxCount,
		const float _fSec,
		const bool _bShow )
	{
		GLLandMan* pLand = GetLand();
		if ( NULL == pLand )
			return;

		// Faction Manager;
		Faction::ManagerField* pFactionManager = pLand->GetFactionManager();
		if ( NULL == pFactionManager )
			return;

		// ���� Faction ��ο��� ������;
		GLMSG::NET_SHOW_ENTRANCE_STATE_UI_FB sMsg( getInstanceMapID(), getKeyMapID() );
		sMsg.dwCurCount = _dwCurCount;
		sMsg.dwMaxCount = _dwMaxCount;
		sMsg.bShow = _bShow;
		sMsg.fSec = _fSec;

		pFactionManager->sendMessageToFaction( _sFactionID, &sMsg );
	}
	
	const bool InstanceField::OnMessageProc (
		GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage,
		const DWORD _dwGaeaID )
	{
		switch ( _pMessage->Type() )
		{
		case NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_CF:
			return MsgRetryFactionCF(
				static_cast< GLMSG::NET_RETRY_FACTION_CF* >( _pMessage ),
				_dwGaeaID );
		}

		return false;
	}

	const bool InstanceField::MsgRetryFactionCF (
		GLMSG::NET_RETRY_FACTION_CF* _pMsg,
		const DWORD _dwGaeaID )
	{
		if ( NULL == _pMsg )
			return false;

		GLLandMan* pLand = GetLand();
		if ( NULL == pLand )
			return false;

		Faction::ManagerField* pFactionMan = pLand->GetFactionManager();
		if ( NULL == pFactionMan )
			return false;

		GLChar* pChar = gpGaeaServer->GetChar( _dwGaeaID );
		if ( NULL == pChar )
			return false;
		
		// Faction ID �� �޾ƿ´�;
		const Faction::FactionID* pFactionID = pFactionMan->getFactionID( pChar->GetActorID() );
		if ( NULL == pFactionID )
			return false;

		// Faction �� ������� �޾ƿ´�;
		const int nMember = pFactionMan->getNMember( *pFactionID );

		// ��ϵ� Faction �� �ִٸ� ó���Ѵ�;
		for ( int i=0; i<m_vecFactionRetry.size(); ++i )
		{
			SRETRY_INFO& sRetryInfo = m_vecFactionRetry.at( i );
			if ( sRetryInfo.sFactionID == *pFactionID )
			{
				// �絵���� ��������� ���ų� �̹� ����Ǿ���;
				if ( false == sRetryInfo.bRetry )
					return true;

				// �絵���� ���� ��� ���н�Ų��;
				if ( false == _pMsg->bOK )
				{
					sRetryInfo.bSuccess = false;
					sRetryInfo.bRetry = false;
					sRetryInfo.fTime = 0.f;
					sRetryInfo.emResult = SRETRY_INFO::EMRETRY_RESULT_FAIL;

					const Faction::FactionMemberVector* vecFactionMember =
						pFactionMan->getFactionMembers( sRetryInfo.sFactionID );
					if ( NULL == vecFactionMember )
					{
						sc::writeLogError( "[ Instance Log ] [ Retry Faction Error!!! Invalid Faction Container!! ]" );

						return false;
					}

					for ( int i=0; i<vecFactionMember->size(); ++i )
					{
						Faction::GLActorID sActorID = vecFactionMember->at( i );
						GLChar* pMember = pLand->GetCharByDbNum( sActorID.actorID_Num );
						if ( NULL == pMember )
							continue;

						GLMSG::NET_RETRY_FACTION_FB NetMsgBRD( getInstanceMapID(), getKeyMapID() );
						NetMsgBRD.fTime = 0.f;
						NetMsgBRD.dwMoney = 0;
						NetMsgBRD.bShow = false;
						gpGaeaServer->SENDTOCLIENT( pMember->GETCLIENTID(), &NetMsgBRD );
					}

					EventRetryProc(
						pChar->CharDbNum(),
						false,
						SRETRY_INFO::EMRETRY_RESULT_FAIL );

					EventCompleteRetryProc(
						pFactionID->factionType,
						pFactionID->factionID_Num,
						false,
						SRETRY_INFO::EMRETRY_RESULT_FAIL );

					return true;
				}

				// ���� ������ ��� ���н�Ų��;
				bool bFailed = false;
				const Faction::FactionMemberVector* vecFactionMember =
					pFactionMan->getFactionMembers( sRetryInfo.sFactionID );
				if ( NULL == vecFactionMember )
				{
					sc::writeLogError( "[ Instance Log ] [ Retry Faction Error!!! Invalid Faction Container!! ]" );

					return false;
				}

				// �� Ȯ��;
				if ( sRetryInfo.dwMoney > pChar->GetInvenMoney() )
				{
					bFailed = true;

				}

				if ( bFailed )
				{
					sRetryInfo.bSuccess = false;
					sRetryInfo.bRetry = false;
					sRetryInfo.fTime = 0.f;
					sRetryInfo.emResult = SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY;

					const Faction::FactionMemberVector* vecFactionMember =
						pFactionMan->getFactionMembers( sRetryInfo.sFactionID );
					if ( NULL == vecFactionMember )
					{
						sc::writeLogError( "[ Instance Log ] [ Retry Faction Error!!! Invalid Faction Container!! ]" );

						return false;
					}

					for ( int i=0; i<vecFactionMember->size(); ++i )
					{
						Faction::GLActorID sActorID = vecFactionMember->at( i );
						GLChar* pMember = pLand->GetCharByDbNum( sActorID.actorID_Num );
						if ( NULL == pMember )
							continue;

						GLMSG::NET_RETRY_FACTION_FB NetMsgBRD( getInstanceMapID(), getKeyMapID() );
						NetMsgBRD.fTime = 0.f;
						NetMsgBRD.dwMoney = 0;
						NetMsgBRD.bShow = false;
						gpGaeaServer->SENDTOCLIENT( pMember->GETCLIENTID(), &NetMsgBRD );
					}

					EventRetryProc(
						pChar->CharDbNum(),
						false,
						SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY );

					EventCompleteRetryProc(
						pFactionID->factionType,
						pFactionID->factionID_Num,
						false,
						SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY );

					return true;
				}

				// �絵���� ���� ��� �߰��Ѵ�;
				sRetryInfo.dwCountOK++;

				EventRetryProc(
					pChar->CharDbNum(),
					true,
					SRETRY_INFO::EMRETRY_RESULT_OK );

				// ��� ����� ����ϸ� ������Ų��;
				if ( sRetryInfo.dwCountOK >= nMember )
				{
					const Faction::FactionMemberVector* vecFactionMember =
						pFactionMan->getFactionMembers( sRetryInfo.sFactionID );
					if ( NULL == vecFactionMember )
					{
						sc::writeLogError( "[ Instance Log ] [ Retry Faction Error!!! Invalid Faction Container!! ]" );

						return false;
					}

					/// �ٽ� �ѹ� ���� Ȯ���Ѵ�;
					for ( int i=0; i<vecFactionMember->size(); ++i )
					{
						Faction::GLActorID sActorID = vecFactionMember->at( i );
						GLChar* pMember = pLand->GetCharByDbNum( sActorID.actorID_Num );
						if ( NULL == pMember )
							continue;

						// �� Ȯ��;
						if ( sRetryInfo.dwMoney > pMember->GetInvenMoney() )
						{
							sRetryInfo.bSuccess = false;
							sRetryInfo.bRetry = false;
							sRetryInfo.fTime = 0.f;
							sRetryInfo.emResult = SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY;

							EventRetryProc(
								pMember->CharDbNum(),
								false,
								SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY );

							EventCompleteRetryProc(
								pFactionID->factionType,
								pFactionID->factionID_Num,
								false,
								SRETRY_INFO::EMRETRY_RESULT_INVALID_MONEY );

							return true;
						}
					}

					for ( int i=0; i<vecFactionMember->size(); ++i )
					{
						Faction::GLActorID sActorID = vecFactionMember->at( i );
						GLChar* pMember = pLand->GetCharByDbNum( sActorID.actorID_Num );
						if ( NULL == pMember )
							continue;

						pMember->SubtractInvenMoneyServer(
							static_cast<LONGLONG>(
							sRetryInfo.dwMoney ), SUBTRACT_INVEN_MONEY_INSTANCE_DUNGEON_RETRY_COST );
						pMember->SendInvenMoneyUpdate();
					}

					sRetryInfo.bSuccess = true;
					sRetryInfo.bRetry = false;
					sRetryInfo.fTime = 0.f;
					sRetryInfo.emResult = SRETRY_INFO::EMRETRY_RESULT_OK;

					EventCompleteRetryProc(
						pFactionID->factionType,
						pFactionID->factionID_Num,
						true,
						SRETRY_INFO::EMRETRY_RESULT_OK );

					return true;
				}
			}
		}

		// ��ϵ� �絵���� ������ ����;
		return false;
	}




	ManagerField::ManagerField(GLGaeaServer* const _pGaeaServer, const unsigned int _nServerChannel)
		: pScriptManager(InstanceSystem::ScriptManager::getInstance())
		, nServerChannel(_nServerChannel)
	{
		gpGaeaServer = _pGaeaServer;
        InitializeFieldPublicInvokeFunction();
	}
	ManagerField::~ManagerField(void)
	{
		for ( InstanceFieldMapIter _iterator(this->instanceMap.begin()); _iterator != this->instanceMap.end(); ++_iterator )
		{
			InstanceField& _instance = _iterator->second;
			_instance.ReserveDestroy();
			_instance.destroyLua();
		}
	}
	void CALLBACK ManagerField::frameMove(const float _fTime, const float _fElapsedTime)
	{
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
		{
			//sc::writeLogError( "[ Instance Log ] [ UseFeatures is none. ]" );
			return;
		}

        //this->pScriptManager->pushCallFunction("luaFieldEventUpdate");

        for ( InstanceFieldMapIter _iterator(this->instanceMap.begin()); _iterator != this->instanceMap.end(); ++_iterator )
        {
            InstanceField& _instance = _iterator->second;
			if ( _instance.IsReserveDestroy() )
				continue;

            _instance.frameMove(_fTime, _fElapsedTime);
        }
	}

	bool CALLBACK ManagerField::EventUpdateVictoriousCountry (
		DWORD dwContentID,
		DWORD dwPreCountryID,
		DWORD dwCurCountryID )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingNewInstance() )
		{
			sc::writeLogError( "[ Instance Log ] [ UseFeatures is none. ]" );

			return true;
		}

		for ( InstanceFieldMapIter _iterator( this->instanceMap.begin() );
			_iterator != this->instanceMap.end(); ++_iterator )
		{
			InstanceField& _instance = _iterator->second;
			if ( _instance.IsReserveDestroy() )
				continue;

			_instance.EventUpdateVictoriousCountry(
				dwContentID, dwPreCountryID, dwCurCountryID );
		}

		return true;
	}

    const InstanceSystem::ScriptManager* const ManagerField::getScriptMan() const
    {
        return pScriptManager;
    }

	InstanceField* CALLBACK ManagerField::createInstance(const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType, bool& _bFirstMake)
	{
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
		{
			//sc::writeLogError( "[ Instance Log ] [ UseFeatures is none. ]" );
			return 0;
		}

        InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
        { 
            const InstanceField _instanceField(_instanceMapID, _keyMapID, _emInstanceType);
            this->instanceMap.insert(InstanceFieldValue(_instanceMapID, _instanceField));

            _bFirstMake = true;
			InstanceFieldMapIter _iteratorNewInstance(this->instanceMap.find(_instanceMapID));
			InstanceField& _newInstanceField = _iteratorNewInstance->second;                
			return &_newInstanceField;
        }

        _bFirstMake = false;
        InstanceField& _instanceField(_iteratorInstance->second);
        return &_instanceField;
    }	

	const bool CALLBACK ManagerField::_destroy(const InstanceMapID& _instanceMapID)
	{
        InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::_destroy ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField = _iteratorInstance->second;
		// lua�� �ı��ϰ� �ʿ��� ���� ��Ų��;
		// lua��ü�� ��� �������簡 �ȵǼ� �Ҹ��ڸ� �̿��� �� ����;
		// ���� ����� ��;
        _instanceField.destroyLua();
        this->instanceMap.erase(_iteratorInstance);

		sc::writeLogInfo ( sc::string::format( "[ Instance Log ] [ Delete Instance %1% ]", _instanceMapID ) );

        return true;
    }

    const bool ManagerField::offline(const DBNUM _dbNum)
    {
        for ( InstanceFieldMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
        {
            InstanceField& _instanceField = _iteratorInstance->second;
			if ( _instanceField.IsReserveDestroy() )
			{
				sc::writeLogInfo ( sc::string::format(
					"[ Instance Log ] [ Destroing Instance Field : %1% ]",
					_instanceField.getInstanceMapID() ) );

				continue;
			}

            _instanceField.EventOffline(_dbNum);
        }

        return true;
    }

    const bool ManagerField::requestMoveToPosition(const InstanceMapID _instanceMapID, const InformationPosition& _informationSetPosition) const
    {		
        const GLLandManager* const _pLandManager(gpGaeaServer->GetLandManager(_instanceMapID));
        if ( _pLandManager == 0 )
		{
			sc::writeLogError( "[ Instance Log ] [ landManager is NULL. ( ManagerField::requestMoveToPosition ) ]" );
            return false;
		}

        const Faction::GLActorID& _actorID = _informationSetPosition.actorID;
		const ServerActor* const pActor(_pLandManager->getActor(_actorID));
        if ( pActor == NULL )
		{
			sc::writeLogError( "[ Instance Log ] [ Actor is NULL. ( ManagerField::requestMoveToPosition ) ]" );
            return false;
		}

        GLMSG::NET_INSTANCE_SET_POSITION _setPositionMessage(_instanceMapID, _informationSetPosition);
        gpGaeaServer->SENDTOAGENT(&_setPositionMessage);

        return true;
    }
	const bool ManagerField::requestDestroy(const InstanceMapID _instanceMapID)
	{
		InstanceFieldMapIter iter = instanceMap.find( _instanceMapID );
		if ( iter == instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::requestDestroy ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return false;
		}

		// �δ� �ı� ����;
		//InstanceField& refInstanceField = iter->second;
		//refInstanceField.ReserveDestroy();
		
		// �δ� �ı� �޽��� ������ ( Agent );
		GLMSG::NET_INSTANCE_DESTROY _NetMsg(_instanceMapID);
		gpGaeaServer->SENDTOAGENT(&_NetMsg);

		return true;
	}

    const bool ManagerField::reserveRegistActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius)
    {
        InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
        if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::reserveRegistActorTrigger ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}

        return _instanceField.reserveRegistActorTrigger(_actorID, _triggerType, _fRadius);
    }

    const bool ManagerField::reserveRegistActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY)
    {
        InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
        if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::reserveRegistActorTrigger ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField = _iterator->second;
        return _instanceField.reserveRegistActorTrigger(_actorID, _triggerType, _fZoneX, _fZoneZ, _fZoneY);
    }

    const bool ManagerField::reserveRemoveActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID)
    {
        InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
        if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::reserveRemoveActorTrigger ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}

        return _instanceField.reserveRemoveActorTrigger(_actorID);
    }
    
    const TimerHandle ManagerField::registTimer(const InstanceMapID _instanceMapID, const float _nTimeSec, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
    {
        InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
        if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::registTimer ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return -1;
		}

        InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return -1;
		}

        return _instanceField.registTimer(_nTimeSec, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);
    }
	const int ManagerField::removeTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer)
	{
		InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
		if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::removeTimer ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return -1;
		}

		InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return -1;
		}

		return _instanceField.removeTimer(_hTimer);
	}
	const int ManagerField::suspendTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer)
	{
		InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
		if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::suspendTimer ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return -1;
		}

		InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return -1;
		}

		return _instanceField.suspendTimer(_hTimer);
	}
	const int ManagerField::resumeTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer)
	{
		InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
		if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::resumeTimer ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return -1;
		}

		InstanceField& _instanceField = _iterator->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return -1;
		}

		return _instanceField.resumeTimer(_hTimer);
	}

    const int ManagerField::isRegistTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer)
    {
        InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
        if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::isRegistTimer ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return -1;
		}

        InstanceField& _instance = _iterator->second;
		if ( _instance.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instance.getInstanceMapID() ) );

			return -1;
		}

        return _instance.isRegistTimer(_hTimer);
    }
	const int ManagerField::getLeftTime(const InstanceMapID _instanceMapID, const TimerHandle _hTimer)
	{
		InstanceFieldMapIter _iterator = this->instanceMap.find(_instanceMapID);
		if ( _iterator == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::getLeftTime ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return -1;
		}

		InstanceField& _instance = _iterator->second;
		if ( _instance.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instance.getInstanceMapID() ) );

			return -1;
		}

		return _instance.getLeftTime(_hTimer);
	}

	const bool ManagerField::_notifyInstanceInformation(void) const
	{
		GLMSG::SNETLOBBY_INSTANCE_LIST _instanceListMessage;
		_instanceListMessage.dwChaNum = GAEAID_NULL;

		const InstanceSystem::MapIDVector& _keyMapIDVector(this->pScriptManager->getKeyMapIDVector());
		for ( InstanceSystem::MapIDVectorCIter _iteratorKeyMapID(_keyMapIDVector.begin()); _iteratorKeyMapID != _keyMapIDVector.end(); ++_iteratorKeyMapID )
		{
			const SNATIVEID& _keyMapID(*_iteratorKeyMapID);

			InstanceSystem::InstanceInformationClient _instanceInformation;
			pScriptManager->getInstanceInformation(_keyMapID, _instanceInformation);
			if ( _instanceListMessage.IsAddable() == false )
			{
				gpGaeaServer->SENDTOALLCLIENT(&_instanceListMessage);
				_instanceListMessage.reset();
			}

			if ( false == _instanceListMessage.add(_instanceInformation) )
			{
				sc::writeLogError( "Failed Load Instance List Key MapID!!!!!" );
			}
		}

		if ( _instanceListMessage.nInstanceInformation != 0 )
		{
			gpGaeaServer->SENDTOALLCLIENT(&_instanceListMessage);
		}

		return true;
	}
    const bool ManagerField::_joinLock(const InstanceMapID _instanceMapID, const bool _bLock)
    {
        InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::_joinLock ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField(_iteratorInstance->second);
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}

        _instanceField.setState(InstanceSystem::EMINSTANCE_JOIN_LOCK, _bLock);
        return true;
    }

	const bool ManagerField::_joinLock(const SNATIVEID& _keyMapID, const bool _bLock)
	{
		if ( this->pScriptManager->isExist(_keyMapID) == false )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid keyMapID ( ManagerField::_joinLock ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ keyMapID : %1% / %2% ]", _keyMapID.Mid(), _keyMapID.Sid() ) );
			return false;
		}

		this->pScriptManager->setState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_JOIN_LOCK, _bLock);
		return true;
	}

	const bool ManagerField::_joinSuspend(const InstanceMapID _instanceMapID, const bool _bSuspend)
	{
		InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::_joinSuspend ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
			return false;
		}

		InstanceField& _instanceField = _iteratorInstance->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}

		_instanceField.setState(InstanceSystem::EMINSTANCE_JOIN_SUSPEND, _bSuspend);
		return true;
	}

    const bool ManagerField::_addMember(const DBNUM _dbNum, const SNATIVEID& _mapID)
    {
        const InstanceMapID _instanceMapID(_mapID.wMainID);
        InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
		{
			sc::writeLogError( "[ Instance Log ] [ Invalid instanceMapID ( ManagerField::_addMember ) ]" );
			sc::writeLogError( sc::string::format("[ Instance Log ] -> [ InstnaceMapID : %1% ]", _instanceMapID ) );
            return false;
		}

        InstanceField& _instanceField(_iteratorInstance->second);
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}

        _instanceField.addMember(_dbNum);

        { // ������ �ִ� ����� ���� ���ش�;
            MemberInstanceFieldMapIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
            if ( _iteratorMember != this->memberInstanceMap.end() )
                this->memberInstanceMap.erase(_iteratorMember);
        }

        // ���ο� ������ ����;		
        this->memberInstanceMap.insert(MemberInstanceFieldValue(_dbNum, _mapID));

        return true;
    }
    const bool ManagerField::_removeMember(const DBNUM _dbNum)
    {
        MemberInstanceFieldMapIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
        if ( _iteratorMember != this->memberInstanceMap.end() )
        {
            const SNATIVEID& _mapID(_iteratorMember->second);
            InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_mapID.wMainID));
            if ( _iteratorInstance != this->instanceMap.end() )
            {
                InstanceField& _instanceField = _iteratorInstance->second;
				if ( _instanceField.IsReserveDestroy() )
				{
					sc::writeLogInfo ( sc::string::format(
						"[ Instance Log ] [ Destroing Instance Field : %1% ]",
						_instanceField.getInstanceMapID() ) );

					return false;
				}

                _instanceField.removeMember(_dbNum);
            }
            this->memberInstanceMap.erase(_iteratorMember);
        }		

        return true;
    }

	const bool ManagerField::_reload(void)
	{
		const bool _bSuccess(this->pScriptManager->reload() == 0);
		ManagerField::_notifyInstanceInformation();
        InitializeFieldPublicInvokeFunction();
		return _bSuccess;
	}
}

