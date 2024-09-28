#include "pch.h"
#include "./GLGaeaServer.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../Move/MoveManagerField.h"
#include "../Database/DBAction/DbActionLogInstantDungeon.h"
#include "../Database/DBAction/DbActionGame.h"
#include <boost/timer.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const HRESULT _CheckPhysicalMemory ( void )
{
	const unsigned int nAvailablePhysicalMemory =
		static_cast< unsigned int >( sc::getMemoryPhysicalAvail() / 1048576 ); // 1048576 = 1024 * 1024;
	const unsigned int nMemoryUsagePercent =
		static_cast< unsigned int >( sc::getMemoryUsagePercent() );
	const unsigned int nMemoryWorkingSet =
		static_cast< unsigned int >( sc::getMemoryWorkingSet() );

	if ( MAX_ALLOW_MEMORY_CREATE_INSTANTMAP < nMemoryWorkingSet )
	{
		sc::writeLogWarn( sc::string::format(
			"[ Instance Log ] [ Cannot create an instantMap: WorkingSet size(%1% MB) is too big !! ]",
			nMemoryWorkingSet ) );

		return E_OUTOFMEMORY;
	}
	else
	{
		// 여유 물리 메모리가 100 MB 이하일 경우 인던 생성에 실패한다;
		if ( nAvailablePhysicalMemory <= 100 )
		{
			sc::writeLogWarn( sc::string::format(
				"[ Instance Log ] [ Cannot create an instantMap: More memory is required: only %1% (MB) is available !! ]",
				nAvailablePhysicalMemory ) );

			return E_FAIL;
		}

		// 물리 메모리 사용량이 95% 이상일 경우 인던 생성에 실패한다;
		if ( 95 <= nMemoryUsagePercent )
		{
			sc::writeLogWarn( sc::string::format(
				"[ Instance Log ] [ Cannot create an instantMap: More memory is required: %1%%% of physical memory is in use !! ]",
				nMemoryUsagePercent ) );

			return E_FAIL;
		}
	}

	return S_OK;
}

void GLGaeaServer::CreateAndEnterInstantMaps()
{
    CreateOldInstanceMaps();	
    CreatePVEInstanceMaps(0.1f);
    CreateNewInstanceMaps();
}

void GLGaeaServer::reserveCreateInstance(const InstanceMapInformation& _instanceMapInformation)
{
	m_queInstanceMapToCreate.push(_instanceMapInformation);
}
const bool GLGaeaServer::reserveDestroyInstance(const DWORD _instanceMapID)
{	
    GLLandMan* const _pLandMan(GetLand(_instanceMapID));
	if ( _pLandMan == NULL )
    {
		return false;
    }
	m_queDestroyInstanceMapID.push(_instanceMapID);
	return true;
}

void GLGaeaServer::CreateNewInstanceMaps(void)
{
	//	GASSERT(IsField());
	boost::timer _timer;	

	InstanceMapInformation _instanceMapInformation;

#ifdef _USE_TBB
	while ( m_queInstanceMapToCreate.try_pop( _instanceMapInformation ) )
	{
#else
	while ( m_queInstanceMapToCreate.empty() == false )
	{
		_instanceMapInformation = m_queInstanceMapToCreate.front();
		m_queInstanceMapToCreate.pop();
#endif

		const SNATIVEID& _baseMapID(_instanceMapInformation.baseMapID);
		const SNATIVEID& _keyMapID(_instanceMapInformation.keyMapID);
        const InstanceSystem::InstanceMapID _instanceMapID(_instanceMapInformation.instanceMapID);
		const InstanceSystem::EMINSTANCE_TYPE _instanceType(InstanceSystem::EMINSTANCE_TYPE(_instanceMapInformation.instanceType));

		const DWORD _baseMapIDMain(_baseMapID.Mid());
		const DWORD _baseMapIDSub(_baseMapID.Sid());
		
		HRESULT hr = _CheckPhysicalMemory();
		if ( FAILED( hr ) )
		{
            sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - out of memory; (instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));

			if ( E_OUTOFMEMORY == hr )
			{
				// 서버 재시작한다.
				//AddGameAdoJob( db::DbActionPtr( new db::ReserveServerStop() ) );
				// 위치 옮김
			}

			continue;
		}		

		SMAPNODE* _pMapNode(FindMapNode(_baseMapID));
		// 기존 코드는 인던으로 사용할 맵은 따로 관리 했으나,
		// 신규 인던의 경우 모든 맵을 인던으로 활용 할 수 있게 한다;
		// 기존에 인던으로(구인던) 사용하던 맵은 사용 할 수 없다;
        GLLandMan* _pBaseLand(GetLand(_baseMapID)); 
        if ( _pMapNode == NULL || _pBaseLand == NULL )
		{
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - invalid baseMapID; (instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
			continue;
		}
		
		if ( (_instanceMapID >= MAX_LAND_MID) || (_instanceMapID == _baseMapIDMain) )
		{			
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - invalid instanceMapID; (instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
			GASSERT(_instanceMapID < MAX_LAND_MID);
			continue;
		}

        const bool _bExistInstance(m_pLandManager[_instanceMapID] != 0);
        GLLandManager* const _pInstanceLandMan(NEW_GLLANDMANAGER(_instanceMapID, _baseMapID.Mid()));
        if ( _pInstanceLandMan == NULL )
        {
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to NEW_GLLANDMAN(); (instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
			continue;
		}
		
        bool _bFirstMake(false);
        InstanceSystem::InstanceField* pScriptInstance(m_pInstanceManager->createInstance(_instanceMapID, _keyMapID, _instanceType, _bFirstMake));
        if ( pScriptInstance == 0 )
		{			
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to create Script.(instanceMapID:%1% keyMapID:%2%/%3%)", _instanceMapID, _keyMapID.Mid(), _keyMapID.Sid()));
            RELEASE_GLLANDMANAGER(_pInstanceLandMan);
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
			continue;
		}
		_pInstanceLandMan->setScriptInstance(pScriptInstance);
        if ( _pInstanceLandMan->addLand(_pBaseLand) == false )
		{			
			sc::writeLogError(sc::string::format("GLGaeaServer::Create() - failed to addLand()! (instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
			RELEASE_GLLANDMANAGER ( _pInstanceLandMan );
			GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
			continue;
		}

		const int _sizeUsedMemory(_pBaseLand->GetUsedMemory());
		_pInstanceLandMan->addUsedMemory(_sizeUsedMemory);
		m_pLandManager[_instanceMapID] = _pInstanceLandMan;

        if ( _bFirstMake == true )
        { // 최초로 생성 하였을 때만 초기화 함수를 호출 해 준다;
            // 생성자 내에서 알아서 해주는게(LandMan, ScriptMan 모두 생성) 좋지만,
            // 기존의 LandManager 생성 방식을 전부 고칠 시간이 없고 동기화 문제를 피하기 위해 따로 처리 하도록 한다;
            if ( pScriptInstance->initiailize() == false )
            {
				sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to initialize Script.(instanceMapID:%1% keyMapID:%2%/%3%)", _instanceMapID, _keyMapID.Mid(), _keyMapID.Sid()));
                RELEASE_GLLANDMANAGER(_pInstanceLandMan);
                GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));
                continue;
            }

            const std::string* pstrInstanceName = m_pInstanceManager->getScriptMan()->getInstanceName(_keyMapID);

            db::LogInstantNewInstance* pDbAction = new db::LogInstantNewInstance(
                db::LogInstantNewInstance::CREATE,
                m_pLandManager[_instanceMapID]->getGUID());

            if ( pstrInstanceName ) 
                pDbAction->strInstanceName = (*pstrInstanceName);

            pDbAction->wKeyMapMID = _keyMapID.wMainID;
            pDbAction->wKeyMapSID = _keyMapID.wSubID;
            pDbAction->dwBasMapID = m_pLandManager[_instanceMapID]->getBaseMapMainID();
            pDbAction->dwInstanceMapID = _instanceMapID;
            pDbAction->tCreateDateTime = sc::time::GetCurrentTime();

            // New Instance Map Destroy Log
            AddLogAdoJob( db::DbActionPtr(pDbAction) );
        }

        if ( _pInstanceLandMan->isCopied() == FALSE )
        {
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to copy LandMan.(instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
            RELEASE_GLLANDMANAGER(_pInstanceLandMan);
            GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, false));	
            continue;
        }

        if ( _bExistInstance == false )
        {			
            m_landManagerMap.push_back( LandManagerPair(_instanceMapID, _pInstanceLandMan) );
            m_setInstantMapIDs.insert(_instanceMapID);
        }

		ReportFieldServerStateToAgent(_sizeUsedMemory);
		
		GLGaeaServer::SENDTOAGENT(&GLMSG::NET_INSTANCE_CREATE_COMPLETE(_instanceMapID, _bFirstMake, true));

		if ( _bFirstMake == true )
			sc::writeLogInfo(sc::string::format("Created a New Instant-Map.(instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));
		else
			sc::writeLogInfo(sc::string::format("Add a child-Map.(instanceMapID:%1% baseMapID:%2%/%3%)", _instanceMapID, _baseMapIDMain, _baseMapIDSub));

        // 인던 생성에 대한 생성 요청이 과해지는 경우 다음 프레임에 생성 하도록 함;
        if ( _timer.elapsed() > 0.1 )
            break;

		DWORD dwPhysicalMemAvailMB		= static_cast<DWORD> (sc::getMemoryPhysicalAvail() / 1048576); // 1048576 = 1024 * 1024
		DWORD dwPhysicalMemUsePercent	= sc::getMemoryUsagePercent();

		sc::writeLogInfo ( "------------------ Crow Pool Log ------------------" );
		sc::writeLogInfo ( sc::string::format( "Crow Data Size : %1%", sizeof( GLCrow ) ) );
		sc::writeLogInfo ( sc::string::format( "Crow Count : %1%", GetCrowPool_MaxSize() ) );
		sc::writeLogInfo ( sc::string::format( "Available Physical Memory is: %1% (MB), %2%%%", dwPhysicalMemAvailMB, ( 100 - dwPhysicalMemUsePercent ) ) );
		sc::writeLogInfo ( "---------------------------------------------------" );
	}
}

void GLGaeaServer::CreateOldInstanceMaps()
{
	GASSERT( IsField() );

	SENTRYINSTMAPINFO sInstanceMapInfo;		

	while ( !m_queInstantMapToCreate.empty() )
	{
		sInstanceMapInfo = m_queInstantMapToCreate.front();
		m_queInstantMapToCreate.pop();

		// Instance Map 정보;
		const DWORD dwInstanceMapID = sInstanceMapInfo.instanceMapID;
		const DWORD dwMainMapID = sInstanceMapInfo.sBaseMapID.Mid();
		const DWORD dwSubMapID = sInstanceMapInfo.sBaseMapID.Sid();
		const MapID sMapID = MapID( sInstanceMapInfo.sBaseMapID, SNATIVEID( dwInstanceMapID, dwSubMapID ) );

		GLChar* pChar = GetChar( sInstanceMapInfo.dwGaeaID );
		if ( NULL == pChar )
		{
			GLMSG::SNET_CREATE_INSTANT_MAP_DEL sNetMsg;
			sNetMsg.instanceMapID = sInstanceMapInfo.instanceMapID;

			SENDTOAGENT( &sNetMsg );

			sc::writeLogError( sc::string::format(
				"GLGaeaServer::CreateOldInstanceMaps() - Owner char does not exist. instantMapID:%1%  baseMapID:%2%/%3%",
				dwInstanceMapID,
				dwMainMapID,
				dwSubMapID ) );

			continue;
		}

		/// 출발 맵 정보를 확인한다;
		GLLandMan* const pLand = pChar->GetLandMan();
		if ( NULL == pLand )
		{
			continue;
		}

		DxLandGateMan* pOutGateMan = pLand->GetLandGateMan();
		if ( NULL == pOutGateMan )
		{
			continue;
		}

		PDXLANDGATE pOutGate = pOutGateMan->FindLandGate( sInstanceMapInfo.dwGateID );
		if ( NULL == pOutGate )
		{
			sc::writeLogError( sc::string::format(
				"GLGaeaServer::CreateOldInstanceMaps() - Invalud Gate ID. instantMapID:%1% baseMapID:%2%/%3%",
				dwInstanceMapID,
				dwMainMapID,
				dwSubMapID ) );

			continue;
		}

		/// 인던 맵을 생성한다;
		if ( sInstanceMapInfo.bCreate )
		{
            HRESULT hr = CreateInstantMap( sInstanceMapInfo );
			if ( hr != S_OK )
			{
				GLMSG::SNET_CREATE_INSTANT_MAP_FB sNetMsgFB;
				sNetMsgFB.dwChaNum = pChar->CharDbNum();
                if ( hr == E_OUTOFMEMORY )
                    sNetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX;
                else
				    sNetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE;
                sNetMsgFB.mapID = sMapID;

				SENDTOAGENT( &sNetMsgFB );

				sc::writeLogError( sc::string::format(
					"GLGaeaServer::CreateOldInstanceMaps() - Could not create the land. instantMapID:%1% baseMapID:%2%/%3%",
					dwInstanceMapID,
					dwMainMapID,
					dwSubMapID ) );

				continue;
			}
		}
        
        GLLandMan* pArriveLand = GetLand( sMapID );
		if ( NULL == pArriveLand )
		{
			sc::writeLogError( sc::string::format(
				"GLGaeaServer::CreateOldInstanceMaps() - Could not find the land. instantMapID:%1%  baseMapID:%2%/%3%",
				dwInstanceMapID,
				dwMainMapID,
				dwSubMapID ) );

			continue;
		}

		GLChar* pHostChar = NULL;
		if ( false == pArriveLand->IsPartyInstantMap() )
		{
			pHostChar = GetChar( pArriveLand->GetInstantMapHostID() );
		}

		/// 이동 정보를 세팅한다;
		MoveSystem::SMOVE_INFO sMoveInfo;
		sMoveInfo.dwChaDBNum = pChar->CharDbNum();
		sMoveInfo.sMapID = sMapID;
		sMoveInfo.dwGateID = pOutGate->GetToGateID( sInstanceMapInfo.dwGateToID );

		/// 이동한다;
		bool bSuccess = MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

		//BOOL bSuccess = EnterLand( instInfo.dwGaeaID, instInfo.dwGateID, instInfo.dwGateToID, gaeaMapID );

		if ( false == bSuccess )
		{
			sc::writeLogError( sc::string::format(
				"GLGaeaServer::CreateOldInstanceMaps() - Could not enter the land. instantMapID:%1%  baseMapID:%2%/%3%",
				dwInstanceMapID,
				dwMainMapID,
				dwSubMapID ) );

			GLMSG::SNET_CREATE_INSTANT_MAP_FB sNetMsgFB;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
			sNetMsgFB.mapID = sMapID;
			SENDTOAGENT( &sNetMsgFB );

			continue;
		}
		
		/// 인던인 경우 성공을 에이전트 서버에 알린다;
		if ( pChar && pArriveLand->isMapInstant() )
		{
			GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsgFB;	//FA
			NetMsgFB.dwChaNum = pChar->CharDbNum();
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_OK;
			NetMsgFB.bCullByObjectMap = pArriveLand->IsCullByObjectMap();
			NetMsgFB.mapID = pArriveLand->GetMapID();
			NetMsgFB.vPos = pChar->GetPosition();

			SENDTOAGENT(&NetMsgFB);
		}

		if ( sInstanceMapInfo.bCreate )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Instance System ] [ Created an Old Instance Map : instanceMapID : %1%, MapID : %2%/%3% ]",
				dwInstanceMapID,
				dwMainMapID,
				dwSubMapID ) );
		}

		/// Crow Pool Log 를 찍는다;
		DWORD dwPhysicalMemAvailMB = static_cast< DWORD >( sc::getMemoryPhysicalAvail() / 1048576 ); // 1048576 = 1024 * 1024
		DWORD dwPhysicalMemUsePercent = sc::getMemoryUsagePercent();

		sc::writeLogInfo ( "------------------ Crow Pool Log ------------------" );
		sc::writeLogInfo ( sc::string::format( "Crow Data Size : %1%", sizeof( GLCrow ) ) );
		sc::writeLogInfo ( sc::string::format( "Crow Count : %1%", GetCrowPool_MaxSize() ) );
		sc::writeLogInfo ( sc::string::format( "Available Physical Memory is: %1% (MB), %2%%%", dwPhysicalMemAvailMB, ( 100 - dwPhysicalMemUsePercent ) ) );
		sc::writeLogInfo ( "---------------------------------------------------" );
	}
}

void GLGaeaServer::CreatePVEInstanceMaps(double dTimer)
{
	SPVEENTRYINSTMAPINFO instPVEInfo;

	boost::timer timer;

	while(!m_quePVEInstantMapToCreate.empty())
	{
		if (timer.elapsed() >= dTimer)
		{
			break;
		}

		instPVEInfo = m_quePVEInstantMapToCreate.front();
		m_quePVEInstantMapToCreate.pop();

		SENTRYINSTMAPINFO instInfo;
		instInfo.bCreate = TRUE;
		instInfo.dwGaeaID = 0;
		instInfo.PartyID = instPVEInfo.PartyID;
		instInfo.sBaseMapID = instPVEInfo.sBaseMapID;
		instInfo.instanceMapID = instPVEInfo.instanceMapID;
		instInfo.dwChaNum = instPVEInfo.sInDunID.dwCharDbNum;
		instInfo.dwGateID = 0;
		instInfo.bCreateTest = FALSE;
		instInfo.dwReqTime = ::GetTickCount();

		GLMSG::NET_PVE_CREATE_INSTANT_MAP_ACK_FA NetMsg;

		NetMsg.bResult = (CreateInstantMap(instInfo) == S_OK);

		if (NetMsg.bResult)
		{
			GLLandMan* pLandMan = GetLand(instInfo.instanceMapID);

			if (pLandMan)
			{
				pLandMan->SetPVEIndunID(instPVEInfo.sInDunID);
				pLandMan->StartTriggerFinger(instPVEInfo.nStage, instPVEInfo.sInDunID.nLevel, instPVEInfo.strGuid);
			}
		}

		NetMsg.dwInstanceMapID = SNATIVEID(WORD(instPVEInfo.instanceMapID), WORD(instPVEInfo.sBaseMapID.wSubID)).Id();
		NetMsg.sInDunID = instPVEInfo.sInDunID;		

		msgpack::sbuffer buf;
		msgpack::pack(buf, NetMsg);
		SENDTOAGENT(NET_MSG_PVE_CREATE_INSTANT_MAP_ACK_FA, buf);

		DWORD dwPhysicalMemAvailMB		= static_cast<DWORD> (sc::getMemoryPhysicalAvail() / 1048576); // 1048576 = 1024 * 1024
		DWORD dwPhysicalMemUsePercent	= sc::getMemoryUsagePercent();

		sc::writeLogInfo ( "------------------ Crow Pool Log ------------------" );
		sc::writeLogInfo ( sc::string::format( "Crow Data Size : %1%", sizeof( GLCrow ) ) );
		sc::writeLogInfo ( sc::string::format( "Crow Count : %1%", GetCrowPool_MaxSize() ) );
		sc::writeLogInfo ( sc::string::format( "Available Physical Memory is: %1% (MB), %2%%%", dwPhysicalMemAvailMB, ( 100 - dwPhysicalMemUsePercent ) ) );
		sc::writeLogInfo ( "---------------------------------------------------" );
	}
}

void GLGaeaServer::DeleteInstantMaps(float fElapsedTime)
{
	GASSERT(IsField());	

    //::EnterCriticalSection(&m_CS_queDestroyInstanceMapID);
	boost::timer _timer;
	DWORD _instanceMapID(GAEAID_NULL);	
#ifdef _USE_TBB
	while ( m_queDestroyInstanceMapID.try_pop( _instanceMapID ) )
	{
#else
	while ( m_queDestroyInstanceMapID.empty() == false )
	{
		_instanceMapID = m_queDestroyInstanceMapID.front();
		m_queDestroyInstanceMapID.pop();
#endif

		if ( _instanceMapID >= MAX_LAND_MID )
		{
			sc::writeLogInfo(sc::string::format("DeleteInstantMaps() _ invalid instanceMapID.(instanceMapID:%1%)", _instanceMapID));
			GASSERT(_instanceMapID < MAX_LAND_MID);
			continue;
		}
		
		m_pInstanceManager->_destroy(_instanceMapID);

        GLLandManager* _pLandManager(NULL);
        ITER_CVECLANDMANAGER _iteratorLandManager = m_landManagerMap.begin();
        for( ; _iteratorLandManager != m_landManagerMap.end(); _iteratorLandManager++ )
        {
            if ( _iteratorLandManager->LandBaseID == _instanceMapID )
            {
                _pLandManager = _iteratorLandManager->pLandManager;
                break;
            }
        }

        if ( _pLandManager == NULL )
		{
			sc::writeLogInfo(sc::string::format("DeleteInstantMaps() _ not Exist Map.(instanceMapID:%1%)", _instanceMapID));
            continue;
		}

		const DWORD _baseMapID(_pLandManager->getBaseMapMainID());
		const DWORD _instanceMapIDLandManager(_pLandManager->getInstanceMapMainID());
		const DWORD _nChildMap(_pLandManager->getNStage());

		if ( m_pLandManager[_instanceMapID] == NULL )
		{
			sc::writeLogError(sc::string::format("DeleteInstantMap() - m_pLandManager[%1%] is NULL!!!!.(instanceMapID:%2% baseMapID:%3%)", _instanceMapID, _instanceMapID, _baseMapID));
		}

		if ( _baseMapID == _instanceMapIDLandManager )
		{
			sc::writeLogError(sc::string::format("DeleteInstantMap() - Can't remove normal-Map!!!!.(instanceMapID:%1% baseMapID:%2%)", _instanceMapID, _baseMapID));
			continue;
		}

		if ( _instanceMapIDLandManager == GAEAID_NULL )
		{
			sc::writeLogError(sc::string::format("DeleteInstantMap() - Can't remove raw-Data-Map!!!.(instanceMapID:%1% baseMapID:%2%)", _instanceMapID, _baseMapID));
			continue;
		}		

		if ( _pLandManager->isOldInstantMap() == true )
		{
			sc::writeLogInfo(sc::string::format("Delete an old-Instant Map.(instanceMapID:%1% baseMapID:%2% nChild:%3%)", _instanceMapID, _baseMapID, _nChildMap));

			GLMSG::SNET_CREATE_INSTANT_MAP_DEL NetMsg;
			NetMsg.instanceMapID = _instanceMapID;
			SENDTOAGENT(&NetMsg);
		}
		else
		{		
			sc::writeLogInfo(sc::string::format("Delete a new-Instant Map.(instanceMapID:%1% nChild:%2%)", _instanceMapID, _nChildMap));
			for ( DWORD _index(MAX_LAND_SID); _index--; )
			{
				GLLandMan* const pLand(_pLandManager->getLand(_index));
				if ( pLand == NULL )
					continue;

				const DWORD _mainMapID(pLand->GetBaseMapID().Mid());
				const DWORD _subMapID(pLand->GetBaseMapID().Sid());
				sc::writeLogInfo(sc::string::format("--Delete Child Map - stage:%1% baseMapID:%2%/%3%", _index, _mainMapID, _subMapID));
			}

			InstanceSystem::InstanceField* const _pInstanceField(_pLandManager->GetScriptInstance());
			if ( _pInstanceField != NULL )
			{
				const SNATIVEID& _keyMapID(_pInstanceField->getKeyMapID());
				db::LogInstantNewInstance* pDbAction(new db::LogInstantNewInstance(db::LogInstantNewInstance::DESTROY, _pLandManager->getGUID()));

				pDbAction->wKeyMapMID = _keyMapID.Mid();
				pDbAction->wKeyMapSID = _keyMapID.Sid();
				pDbAction->dwBasMapID = _pLandManager->getBaseMapMainID();
				pDbAction->dwInstanceMapID = _instanceMapID;
				pDbAction->tDestroyDateTime = sc::time::GetCurrentTime();

				// New Instance Map Destroy Log
				AddLogAdoJob( db::DbActionPtr(pDbAction) );
			}			
		}	

		m_setInstantMapIDs.erase(_instanceMapID);
#ifdef _USE_TBB
        CVECLANDMANAGER _templandManagerMap;
        size_t tSize = m_landManagerMap.size();
        for( size_t i=0; i<tSize; ++i )
        {
            if ( m_landManagerMap[i].LandBaseID != _instanceMapID )
                _templandManagerMap.push_back(m_landManagerMap[i]);
        }
        m_landManagerMap = _templandManagerMap;
#else
		m_landManagerMap.erase(_iteratorLandManager);
#endif
		m_pLandManager[_instanceMapID] = NULL;
		RELEASE_GLLANDMANAGER(_pLandManager);		

		ReportFieldServerStateToAgent(-_pLandManager->getUsedMemory());

		sc::writeLogInfo(std::string("Deleted Instant-Map OK;"));	
    }
    //::LeaveCriticalSection(&m_CS_queDestroyInstanceMapID);
}

HRESULT GLGaeaServer::CreateInstantMap(const SENTRYINSTMAPINFO& createInstMapInfo)
{
	GASSERT(IsField());

	HRESULT hr = _CheckPhysicalMemory();
	if ( FAILED( hr ) )
	{
		if ( E_OUTOFMEMORY == hr )
		{
			// 서버 재시작한다.
			//AddGameAdoJob( db::DbActionPtr( new db::ReserveServerStop() ) );
			// 위치 옮김
		}
		return hr;
	}

    const SNATIVEID& _mapID(createInstMapInfo.sBaseMapID);  
    SMAPNODE* pMapNode(FindMapNode(_mapID));
    if ( pMapNode == NULL )
		return E_FAIL;

	GLLandMan* const pBaseLandMan(GetLand(_mapID));
    if ( pBaseLandMan == NULL )
        return E_FAIL;

	const DWORD nMainMapID(_mapID.Mid());
	const DWORD nSubMapID(_mapID.Sid());	

	const DWORD instanceMapID(createInstMapInfo.instanceMapID);
	if ( (instanceMapID >= MAX_LAND_MID) || (instanceMapID == nMainMapID) )
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - invalid instanceMapID; (instanceMapID:%1%, baseMapID:%2%/%3%)", instanceMapID, nMainMapID, nSubMapID));
		return E_FAIL;	
	}

    GLLandManager* const pTargetLandMan(NEW_GLLANDMANAGER(instanceMapID, nMainMapID));
    if ( pTargetLandMan == NULL )
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to NEW_GLLANDMAN(); (instanceMapID:%1%, baseMapID:%2%/%3%)", instanceMapID, nMainMapID, nSubMapID));
        return E_FAIL;	
	}

	//
	//mjeon
	//InstantMap could be made more than one. Therefore BaseMap is required.
	//Each InstantMap is created by copying from BaseMap.
	//	
    pTargetLandMan->addLand(nSubMapID, pMapNode->IsPeaceZone(), pMapNode->IsPkZone(), pBaseLandMan);
    pTargetLandMan->setInstantMapData(createInstMapInfo.dwGaeaID, createInstMapInfo.PartyID);

	if(!pTargetLandMan->isCopied())
	{
		RELEASE_GLLANDMANAGER(pTargetLandMan);

		return E_FAIL;
	}

	m_setInstantMapIDs.insert(instanceMapID);

	InsertMap(pTargetLandMan);

	GLLandMan* const _pLand(pTargetLandMan->getLand(nSubMapID));
	if ( _pLand != NULL )
	{
		const int _sizeUsedMemory(_pLand->GetUsedMemory());
		pTargetLandMan->addUsedMemory(_sizeUsedMemory);
		ReportFieldServerStateToAgent(_sizeUsedMemory);
	}

	sc::writeLogInfo(sc::string::format("Created an Instant Map.(instanceMapID:%1% baseMapID:%2%/%3%)", instanceMapID, nMainMapID, nSubMapID));

	return S_OK;
}
