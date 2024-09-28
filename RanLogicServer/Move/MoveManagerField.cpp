#include "pch.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../FieldServer/GLGaeaServer.h"

#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TriggerManager/GLTriggerManagerFD.h"

#include "./MoveManagerField.h"

namespace MoveSystem
{

	void CMoveManagerField::OnFrameMove ( const float _fElapsedTime )
	{
		SMOVE_INFO sTempMoveMapInfo;

		// 이동 큐가 큰 의미가 없어서 제거한다;
/*#ifdef _USE_TBB
		while ( m_queueMoveMap.try_pop( sTempMoveMapInfo ) )
		{
#else
		while ( false == m_queueMoveMap.empty() )
		{
			sTempMoveMapInfo = m_queueMoveMap.front();
			m_queueMoveMap.pop();
#endif
			// Move Map 이 가능한지 확인한다;
			if ( EMREQ_MOVEMAP_RESULT_AFB_OK != IsCanMoveMap( sTempMoveMapInfo ) )
			{
				// 이 부분에서 실패가 일어나면 재시도를 하지 않는다;
				// MoveMap Function 을 실행할 때 검사하기 때문에 한번 더 확인하는 정도의 의미를 가진다;
				continue;
			}

			// 맵 이동을 진행한다;
			if ( false == _MoveMapProc( sTempMoveMapInfo ) )
			{
				/// _MoveMapProc 가 실패하는 경우는 같은 Field 내에서 이동하는 경우로 한정된다;
				/// 다른 Field 로 이동하는 것을 실패했을 경우에는 OnFailToMove Callback 이 호출된다;

				/// 이 시점에서 실패를 알아도 현재는 사용된 아이템이 복구되지 않는다;
				/// 아이템을 복구하는 코드를 넣거나 아이템 처리 로직을 변경해야 한다;

				// 재시도 컨테이너에 추가한다;
				if ( false == _RetryMoveMapProc( sTempMoveMapInfo ) )
				{
					// 맵 이동 실패 로그;
					sc::writeLogError( sc::string::format(
						"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
						sTempMoveMapInfo.dwChaDBNum,
						sTempMoveMapInfo.sMapID.getGaeaMapID().Mid() ) );
				}
			}
		}
*/
		// 재시도 한다;
		for ( UINT i=0; i<m_vecMoveMap.size(); ++i )
		{
			SMOVE_INFO& sMoveInfo = m_vecMoveMap.at( i );

			// 재시도 로그;
			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ Retry Move Map : %1% / %2% ]",
				sMoveInfo.dwChaDBNum,
				sMoveInfo.sMapID.getGaeaMapID().Mid() ) );

			MoveMap( sMoveInfo );
		}

		// 모두 다시 재시도 했으므로 초기화한다;
		m_vecMoveMap.clear();
	}


	void CMoveManagerField::OnJoinServer (
		GLChar* const _pChar,
		GLLandMan* const _pLand )
	{
		/*sc::writeLogDebug( sc::string::format(
			"[ Move Map ] [ OnJoinServer : %1% / %2% ]",
			_pChar->CharDbNum(),
			_pLand->GetMapID().getGaeaMapID().Mid() ) );*/

		// Land 에 입장한다;
		_EnterLand( _pChar, _pLand );
	}


	const bool CMoveManagerField::OnJoinClient (
		const DWORD _dwChaDBNum,
		const MapID& _sPreviousMapID,
		InstanceSystem::EMAUTHORITY _emAuthority,
		EMGAME_JOINTYPE _emGameJoinType )
	{
		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( pLand )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Move Map ] [ OnJoinClient : %1% / %2% ]",
				_dwChaDBNum,
				pLand->GetMapID().getGaeaMapID().Mid() ) );

			// 출발맵과 도착맵 중에 하나가 인던일 경우에 처리한다;
			const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sPreviousMapID );
			const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
			if ( pMapNode && pArriveMapNode )
			{
				// 이동이 이루어진 후에 Instance System 에 알린다;
				if ( pMapNode->IsNewInstanceMap() || pArriveMapNode->IsNewInstanceMap() )
				{
					GLMSG::NET_INSTANCE_MOVE_MAP sInstanceMoveMapMsg(
						_dwChaDBNum,
						_sPreviousMapID,
						pChar->GetPosition(),
						//m_pServer->GetFieldSvrID(),
						pLand->GetMapID() );
					_SendToMyself( &sInstanceMoveMapMsg );

					// 인던 입장 이벤트 발생;
					// 도착맵이 인던일 경우에만 들어온다;
					if ( ( false == pMapNode->IsNewInstanceMap() ) && ( pArriveMapNode->IsNewInstanceMap() ) )
					{
						D3DXVECTOR3 vAdsjustPosition = pChar->GetPosition();
						InstanceSystem::InstanceField* const pInstance = pLand->GetScriptInstance();
						if ( pInstance )
						{
							if ( pInstance->EventJoin( _dwChaDBNum, pLand, vAdsjustPosition ) )
							{
								// 입장시 위치 조정이 있다면 해당하는 위치로 바꾼다.;
								pChar->SetPosition( vAdsjustPosition );
							}
						}
					}
				}
			}
		}	

		/// 현재 관전자의 개념이 토너먼트에 한정되어 있으므로;
		/// 토너먼트가 켜져 있을 경우에만 동작해야 한다;
		if ( GLUseFeatures::GetInstance().IsUsingTournament() )
		{
			if ( _emAuthority == InstanceSystem::EMAUTHORITY_OBSERVER )
			{
				pChar->SetActState( EM_ACT_OBSERVE );
				pChar->m_sSummonable.Clear();

				GLMSG::NET_TOURNAMENT_GAME_POPUP_FC sTournamentMsg( true );
				_SendToClient( pChar->ClientSlot(), &sTournamentMsg);
			}
			else
			{
				pChar->ReSetActState( EM_ACT_OBSERVE );

				GLMSG::NET_TOURNAMENT_GAME_POPUP_FC sTournamentMsg( false );
				_SendToClient( pChar->ClientSlot(), &sTournamentMsg);
			}
		}

		// 접속 타입에 따라 다른 처리를 한다;
		switch ( _emGameJoinType )
		{
		case EMJOINTYPE_FIRST:
			{
				// 캐릭터의 첫번째 접속일 경우 처리한다;
				// 캐릭터의 생성 정보를 전송한다;
				pChar->MsgGameJoin();

				// 캐릭터 최초 접속 트리거를 발생시킨다;
				triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_Connect_To_Field().Do( pChar );
			}
			break;

		case EMJOINTYPE_TRANSFER:	// Instance Server 이동으로 현재는 사용하지 않는다 ( Instance Server 개발하다 말았다 );
		case EMJOINTYPE_MOVEMAP:
			{
				if ( pLand )
				{
					GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
					sNetMsgFB.emMoveType = MoveSystem::EMMOVE_TYPE_DEFAULT;
					sNetMsgFB.dwChaNum = pChar->CharDbNum();
					sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
					sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
					sNetMsgFB.sMAPID = pChar->GetCurrentMap();
					sNetMsgFB.vPOS = pChar->GetPosition();
					_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

					pChar->MsgFieldMoveReset ();
				}
			}
			break;

		case EMJOINTYPE_GATE:	
			{	
				// 이동일 경우 처리한다;
				if ( pLand )
				{
					// 게이트 이동 확인 메시지;
					GLMSG::SNETREQ_GATEOUT_FB sNetMsgGateOutFB;
					sNetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
					sNetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
					sNetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
					sNetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
					sNetMsgGateOutFB.vPos = pChar->GetPosition();
					_SendToAgent( &sNetMsgGateOutFB );

					pChar->MsgFieldMoveReset ();
				}			
			}
			break;

		/*case EMJOINTYPE_REBIRTH:
			{		
				// 부활일 경우 처리한다;
				if ( pLand )
				{
					// 부활 확인 메시지;
					GLMSG::NET_CHAR_RESURRECT_FAFC sNetResurrectMsg(
						pChar->GetCurrentMap(),
						pChar->GetPosition(),
						pChar->GETHP(),
						pChar->GETMP(),
						pChar->GETSP(),
						pLand ? pLand->IsCullByObjectMap() : true,
						pChar->GetCharID()
						);

					_SendToAgent( &sNetResurrectMsg );

					pChar->MsgFieldMoveReset ();
				}
			}
			break;*/

		case EMJOINTYPE_PVEINDUN:
			{
				// PVE 인던일 경우 처리한다;
				if ( pLand )
				{
					// 게이트 이동 확인 메시지;
					GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
					sNetMsgFB.emMoveType = MoveSystem::EMMOVE_TYPE_TRIGGERSYSTEM;
					sNetMsgFB.dwChaNum = pChar->CharDbNum();
					sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
					sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
					sNetMsgFB.sMAPID = pChar->GetCurrentMap();
					sNetMsgFB.vPOS = pChar->GetPosition();
					_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

					/*GLMSG::SNETREQ_GATEOUT_FB sNetMsgGateOutFB;
					sNetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
					sNetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
					sNetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
					sNetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
					sNetMsgGateOutFB.vPos = pChar->GetPosition();
					_SendToAgent( &sNetMsgGateOutFB );*/

					// PVE 인던 이동 확인 메시지;
					GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA sNetEntranceMsg;
					sNetEntranceMsg.dwCharDbNum = pChar->CharDbNum();
					sNetEntranceMsg.sInDunID = pLand->GetPVEIndunID();
					sNetEntranceMsg.bResult = true;
					msgpack::sbuffer buf;
					msgpack::pack( buf, sNetEntranceMsg );
					m_pServer->SENDTOAGENT( NET_MSG_PVE_ENTRANCE_INSTANT_MAP_ACK_FA, buf );

					pChar->MsgFieldMoveReset();
				}
			}
			break;
		};

		pChar->Online();

		return true;
	}


	const bool CMoveManagerField::OnMoveMap ( GLMSG::SNETPC_MOVEMAP_AF* pOutMsg )
	{
		if ( NULL == pOutMsg )
			return false;

		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( pOutMsg->dwChaNum );
		if ( NULL == pChar )
			return false;

		GLLandMan* const pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return false;

		// 맵 이동시 대련을 취소한다;
		const SCONFTING& sCONFTING = pChar->GETCONFRONTING();
		if ( sCONFTING.IsCONFRONTING() )
		{
			GASSERT( m_pServer->IsField() );

			switch ( sCONFTING.emTYPE )
			{
			case EMCONFT_ONE:
				{
					GLChar* pTAR = m_pServer->GetChar( sCONFTING.dwTAR_ID );
					if ( pTAR )
						pTAR->ResetConfront( EMCONFRONT_END_MOVEMAP );

					pChar->ResetConfront( EMCONFRONT_END_MOVEMAP );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;

			case EMCONFT_PARTY:
				{
					pChar->ResetConfront( EMCONFRONT_END_NOTWIN );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;

			case EMCONFT_GUILD:
				{
					pChar->ResetConfront( EMCONFRONT_END_NOTWIN );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;
			};
		}		

		// 매크로가 켜져있다면 맵이동시 끈다;
		if ( pChar->IsActState( EM_ACT_MACROMODE ) )
		{
			pChar->MacroOnOff( false );

			GLMSG::SNET_MACROMODE_CLOSE_FC sMacroCloseMsg;
			_SendToClient( pChar->ClientSlot(), &sMacroCloseMsg );
		}

		/// 이동하기 전에 캐릭터가 죽어있으면 살린다;
		// 현재는 무조건 100% 로 회복시킨다;
		if ( false == IsValidCharacter( pChar->CharDbNum() ) )
		{
			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ Resurrect Character : %1% / %2% ]",
				pChar->CharDbNum(),
				pLand->GetMapID().getGaeaMapID().Mid() ) );

			pChar->SetAlive( 100.f );
		}

		// 맵 이동시 Pet 을 삭제한다;
		m_pServer->DropOutPET( pChar->m_dwPetGUID, true, true );

		// 맵 이동시 탈것 정보를 저장한다;
		if ( pOutMsg->emMoveType == MoveSystem::EMMOVE_TYPE_GATE )
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, true );
		else
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, false );

		// 맵 이동시 Summon 을 모두 삭제한다;
		m_pServer->DropOutAllSummon( pChar );

		// 출발맵이나 도착맵이 신규인던인 경우 이동이 이루어지기 전에 Instance System 에 알린다;
		{
			const SMAPNODE* pMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
			const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( pOutMsg->targetMapID );
			if ( pMapNode && pArriveMapNode )
			{
				if ( pMapNode->IsNewInstanceMap() || pArriveMapNode->IsNewInstanceMap() )
				{
					GLMSG::NET_INSTANCE_MOVE_MAP sInstanceMoveMapMsg(
						pChar->CharDbNum(),
						pLand->GetMapID(),
						pChar->GetPosition(),
						//m_pServer->GetFieldSvrID(),
						pOutMsg->targetMapID,
						false );
					_SendToMyself( &sInstanceMoveMapMsg );

					// 인던 퇴장 이벤트 발생;
					// 출발맵이 인던이고 도착맵이 인던이 아닐 경우에만 들어온다;
					// Instance -> Field;
					if ( ( pMapNode->IsNewInstanceMap() ) && ( false == pArriveMapNode->IsNewInstanceMap() ) )
					{
						InstanceSystem::InstanceField* const pInstance = pLand->GetScriptInstance();
						if ( pInstance )
						{
							pInstance->EventOut( pChar->CharDbNum() );
						}
					}
				}
			}
		}

		/// Move Map 을 수행한다;

		// 다른 Field 로의 이동이거나 신규인던인 경우에 처리한다;
		// 신규인던의 경우는 무조건 새로 만들어야 한다;
		//const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( pOutMsg->targetMapID );
		//if ( ( FALSE == pOutMsg->bSameFieldServer ) || ( pArriveMapNode && pArriveMapNode->IsNewInstanceMap() ) )
		{
			/// Agent 에 전달되는 Field Out Message;
			// Field 를 나가게 되면 등록된 다음 Field 로 이동된다;
			GLMSG::SNETPC_FIELDSVR_OUT_FB sMsgOutFB;
			sMsgOutFB.dwSvrGroupNum		= pChar->SvrGroupNum();
			sMsgOutFB.dwChaNum			= pChar->CharDbNum();
			sMsgOutFB.positionPrevious	= pChar->GetPosition();
			sMsgOutFB.bTransfer			= pOutMsg->bTransfer;
			sMsgOutFB.bToInstance		= pOutMsg->bToInstance;
			sMsgOutFB.emType	        = pOutMsg->emType;	
			sMsgOutFB.bChangeField		= true;
			sMsgOutFB.sSummonable		= pChar->m_sSummonable;

			m_pServer->GetMsgServer()->ResetGaeaID( pChar->ClientSlot() );

			m_pServer->ReserveDropOutPC(
				pChar->GetGaeaID(),
				db::DbActionPtr( new db::CharacterDbToAgentMessage( pChar->ClientSlot(), &sMsgOutFB ) ) );

			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ OnMoveMap : %1% ]",
				pOutMsg->dwChaNum ) );
		}
		/*// 같은 Field 로의 이동이거나 신규인던이 아닌 경우에 처리한다;
		else
		{
			SMOVE_INFO sMoveInfo;
			sMoveInfo.emMoveType = pOutMsg->emMoveType;
			sMoveInfo.dwChaDBNum = pChar->CharDbNum();
			sMoveInfo.sMapID = pOutMsg->targetMapID;
			sMoveInfo.dwGateID = pOutMsg->dwGateID;
			sMoveInfo.vPosition = pOutMsg->vPosition;

			GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
			sNetMsgFB.emMoveType = sMoveInfo.emMoveType;

			// _RequestMoveMap_ThisFieldServer 함수 호출 이후에는 케릭터의 현재 mapID 가 이동하는 mapID 로 변경된다;
			const MapID sCurMapID = pChar->GetCurrentMap();

			const SMAPNODE* pMapNode = m_pServer->FindMapNode( sMoveInfo.sMapID );
			if ( NULL == pMapNode )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

				return false;
			}

			BOOL bOK = _RequestMoveMap_ThisFieldServer( sMoveInfo );
			if ( FALSE == bOK )
				return false;

			// 출발지와 목적지가 다를 경우 버프와 상태이상을 제거한다;
			if( sCurMapID != sMoveInfo.sMapID )
			{
				for ( int i = 0; i < EMBLOW_MULTI; ++i )
					pChar->DISABLEBLOW( i );
				for ( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
					pChar->RESETSKEFF( i );

				// CP 초기화 맵인지 확인한다;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action 을 리셋한다;
			pChar->ResetAction();

			// 이동 성공을 알린다;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToAgent( &sNetMsgFB );

			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ OnMoveMap : %1% ]",
				pOutMsg->dwChaNum ) );
		}*/

		// 캐릭터 이동 추적;
		/*if ( pChar->m_bTracingUser )
		{
			GLMSG::NET_LOG_UPDATE_TRACINGCHAR sTracingMsg;
			sTracingMsg.nUserNum  = pChar->GetUserID();
			sTracingMsg.SetAccount( pChar->m_szUID );

			CString strTemp;
			if ( pLand )
			{
				int nPosX = 0;
				int nPosY = 0;
				const D3DXVECTOR3& vPosition = pChar->GetPosition();
				pLand->GetMapAxisInfo().Convert2MapPos( vPosition.x, vPosition.z, nPosX, nPosY );

				strTemp.Format(
					"[ Move Map ] [ Move Map, [%s][%s], MAP:mid[%d]sid[%d], StartPos:[%d][%d], Money:[%I64d] ]",
					pChar->m_szUID,
					pChar->m_szName,
					pChar->GetCurrentMap().getGaeaMapID().Mid(),
					pChar->GetCurrentMap().getGaeaMapID().Sid(),
					nPosX,
					nPosY,
					pChar->GetInvenMoney());
			}
			else
			{
				strTemp.Format(
					"[ Move Map ] [ Move Map, [%s][%s], MAP:mid[%d]sid[%d], Money:[%I64d] ]",
					pChar->m_szUID,
					pChar->m_szName,
					pChar->GetCurrentMap().getGaeaMapID().Mid(),
					pChar->GetCurrentMap().getGaeaMapID().Sid(),				
					pChar->GetInvenMoney());
			}


			sTracingMsg.SetLog( strTemp.GetString() );
			_SendToAgent( &sTracingMsg );
		}*/

		return true;
	}


	void CMoveManagerField::OnFailToMove ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pStateMsg )
	{
		if ( NULL == pStateMsg )
			return;

		sc::writeLogDebug( sc::string::format(
			"[ Move Map ] [ OnFailToMove : %1% / %2% ]",
			pStateMsg->dwChaNum,
			pStateMsg->sMAPID.getGaeaMapID().Mid() ) );

		// 필드 서버 이동 실패 시에 들어온다;
		// 재시도 처리가 필요하며, 재시도 하지 못할 경우 Client 에 알려야 한다;

		/// 이 시점에서 실패를 알아도 현재는 사용된 아이템이 복구되지 않는다;
		/// 아이템을 복구하는 코드를 넣거나 아이템 처리 로직을 변경해야 한다;

		// 재시도 컨테이너에 추가한다;
		SMOVE_INFO sTempMoveMapInfo;
		sTempMoveMapInfo.emMoveType = pStateMsg->emMoveType;
		sTempMoveMapInfo.dwChaDBNum = pStateMsg->dwChaNum;
		sTempMoveMapInfo.sMapID = pStateMsg->sMAPID;
		sTempMoveMapInfo.vPosition = pStateMsg->vPOS;
		sTempMoveMapInfo.dwGateID = pStateMsg->dwGATEID;
		sTempMoveMapInfo.dwRetryCount = pStateMsg->dwRetryCount;

		if ( false == _RetryMoveMapProc( sTempMoveMapInfo ) )
		{
			// 맵 이동 실패 로그;
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
				sTempMoveMapInfo.dwChaDBNum,
				sTempMoveMapInfo.sMapID.getGaeaMapID().Mid() ) );
		}
	}


	const bool CMoveManagerField::OnCompleteMove ( GLMSG::SNETREQ_LANDIN* pCompleteMsg )
	{
		if ( NULL == pCompleteMsg )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( pCompleteMsg->dwChaNum );
		if ( NULL == pChar )
			return false;

		// 맵 이동시 대련을 취소한다; // OnMoveMap 에 있던 함수. OnMoveMap이 현재 호출되지 않아 CompleMove로 이동
		const SCONFTING& sCONFTING = pChar->GETCONFRONTING();
		if ( sCONFTING.IsCONFRONTING() )
		{
			GASSERT( m_pServer->IsField() );

			switch ( sCONFTING.emTYPE )
			{
			case EMCONFT_ONE:
				{
					GLChar* pTAR = m_pServer->GetChar( sCONFTING.dwTAR_ID );
					if ( pTAR )
						pTAR->ResetConfront( EMCONFRONT_END_MOVEMAP );

					pChar->ResetConfront( EMCONFRONT_END_MOVEMAP );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;

			case EMCONFT_PARTY:
				{
					pChar->ResetConfront( EMCONFRONT_END_NOTWIN );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;

			case EMCONFT_GUILD:
				{
					pChar->ResetConfront( EMCONFRONT_END_NOTWIN );
					pChar->ReceiveLivingPoint( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				}
				break;
			};
		}	

		if ( false == pChar->IsActState( EM_GETVA_AFTER ) )
		{
			// 캐릭터 주변의 정보를 인지한다 ( View Around );
			pChar->GetViewAround ();

			//
			// mjeon.activity
			//
			// When a character moves into other FieldServer, it may take some time to load activity. 
			// At the time, this ActivityCheckMap() can be failed because ActivityLoad() did not completed yet. 
			// But in this case, CheckAfterLoad() will call CheckMap() inside it. Just ignore this failure.
			//
			pChar->ActivityCheckMap();

			// 이동이 완료되는 시점에 캐릭터가 처리해야 할 부분;
			pChar->ToDoRightAfterRecallDone();

			pChar->m_mapIDPrevious = pChar->GetCurrentMap();
			pChar->m_tLoginTime = pCompleteMsg->tLoginTime;

			// Instance System 에 이동 성공을 알림;		
			GLMSG::NET_INSTANCE_MOVE_DONE sCompleteMsg(
				pChar->GetCurrentMap(),
				pChar->CharDbNum() );
			_SendToAgent( &sCompleteMsg );

		}
		else
		{
			sc::writeLogError(
				sc::string::format(
				"[ Move Map ] [ OnCompleteMove Call this function after receive view around message already. ChaName %1% ChaDbNum %2% DoActWait Prev %3%, Cur %4% ]",
				pChar->m_szName,
				pChar->CharDbNum(),
				pCompleteMsg->m_nDoActWaitTypePrev,
				pCompleteMsg->m_nDoActWaitType ) );
		}

		sc::writeLogInfo( sc::string::format(
			"[ Move Map ] [ OnCompleteMove : %1% / %2% ]",
			pCompleteMsg->dwChaNum,
			pChar->GetCurrentMap().getGaeaMapID().Mid() ) );

		return true;
	}


	const EMREQ_MOVEMAP_RESULT_AFB CMoveManagerField::IsUseMoveItem ( const DWORD _dwChaDBNum, const MapID _sMapID, SINVENITEM* const _pINVENITEM )
	{
		if ( NULL == m_pServer )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		// 아이템 정보 가져옴;
		const SITEM* pItem = GLogicData::GetInstance().GetItem( _pINVENITEM->sItemCustom.GetNativeID() );
		if ( !pItem )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		}

		/// 출발 맵이 아이템 사용 가능 지역인지 확인한다;
		const SMAPNODE* pCurMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
		if ( ( NULL == pCurMapNode ) ||
			( pCurMapNode->IsBlockDrugItem( pItem->BasicType() ) ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		/// 도착 맵이 아이템 사용 가능 지역인지 확인한다;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMapID );
		if ( ( NULL == pMapNode ) ||
			( pMapNode->IsBlockDrugItem( pItem->BasicType() ) ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		}

		// 아이템의 쿨타임을 체크한다;
		if ( pChar->CheckCoolTime( _pINVENITEM->sItemCustom.GetNativeID() ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		}

		// 아이템을 사용할 수 없는 상황일 경우 처리한다;
		if ( pChar->IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		// 현재 Vehicle 상태에 따라 처리한다;
		const SITEM* pVehicleItem = pChar->GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
		if ( pVehicleItem )
		{
			if ( EMVEHICLE_DRIVER == pChar->VehicleState() &&
				false == vehicle::CanUseTeleportItem( pVehicleItem->VehicleType() ) )
			{
				return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
			}
			else if ( EMVEHICLE_PASSENGER == pChar->VehicleState() )
			{
				return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
			}
		}

		// 대련 도중 일 경우 처리한다;
		if ( pChar->m_sCONFTING.IsCONFRONTING() ||
			pChar->IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		return EMREQ_MOVEMAP_RESULT_AFB_OK;
	}


	const bool CMoveManagerField::IsValidCharacter ( const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 캐릭터 상태가 정상이 아닐 경우에 처리한다;
		if ( false == pChar->IsValidBody() )
			return false;

		return true;
	}


	const EMREQ_MOVEMAP_RESULT_AFB CMoveManagerField::IsCanMoveMap ( const SMOVE_INFO& _sMoveInfo )
	{
		if ( NULL == m_pServer )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;

		/// 출발맵과 도착맵이 둘다 인던인데 서로 다른 인던일 경우 이동을 막는다;
		if ( ( true == pLand->IsNewInstantMap() ) && ( false == _sMoveInfo.sMapID.isSameID() ) )
		{
			if ( pLand->GetGaeaMapID().Mid() != _sMoveInfo.sMapID.getGaeaMapID().Mid() )
				return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		/// 일반 처리;

		// 거래중일 경우 처리한다;
		if ( pChar->m_sTrade.Valid() )
		{
			// 거래를 취소한다;
			pChar->m_sTrade.Reset();

			GLMSG::SNET_TRADE_CANCEL_TAR sNetMsg;
			sNetMsg.dwChaNum = pChar->CharDbNum();
			m_pServer->SENDTOAGENT( &sNetMsg );

			// 거래 타겟과의 거래도 취소한다;
			GLChar* pTargetChar = m_pServer->GetChar( pChar->m_sTrade.GetTarget() );		
			if ( pTargetChar )
			{
				pTargetChar->m_sTrade.Reset();

				sNetMsg.dwChaNum = pTargetChar->CharDbNum();
				m_pServer->SENDTOAGENT( &sNetMsg );
			}
		}


		/// 도착 Map 관련 제한사항 확인;

		// 가야할 Map 정보가 올바른지 확인한다;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
		if ( NULL == pMapNode)
		{
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		}

		// GM 이 아닐경우 진입 조건을 검사한다;
		if ( pChar->UserLevel() < USER_USER_GM )
		{
			EMREQFAIL emReqFail = EMREQUIRE_COMPLETE;
			const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
			emReqFail = pRequire->ISCOMPLETE( pChar );
			if ( emReqFail != EMREQUIRE_COMPLETE )
			{
				return EMREQ_MOVEMAP_RESULT_AFB_JOINCON;
			}
		}

		return EMREQ_MOVEMAP_RESULT_AFB_OK;
	}


	const bool CMoveManagerField::MoveMap ( SMOVE_INFO& _sMoveInfo )
	{
		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;

		if ( NULL == m_pServer )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map, Server is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map, This Character is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		// 체크 타입일 경우 캐릭터가 유효한지 체크한다;
		
		if ( _sMoveInfo.emMoveType == EMMOVE_TYPE_CHECK ||
			_sMoveInfo.emMoveType == EMMOVE_TYPE_GATE )
		{
			if ( false == IsValidCharacter( _sMoveInfo.dwChaDBNum ) )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
				_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Check to Move Map, This Character is Invalid : %1%, Error Code : %2% ]",
					_sMoveInfo.dwChaDBNum,
					sNetMsgFB.emFB ) );

				return false;
			}
		}

		// 이동 가능 기본 체크한다;
		sNetMsgFB.emFB = IsCanMoveMap( _sMoveInfo );
		if ( EMREQ_MOVEMAP_RESULT_AFB_OK != sNetMsgFB.emFB )
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map, IsCanMoveMap : %1%, Error Code : %2% ]",
				_sMoveInfo.dwChaDBNum,
				sNetMsgFB.emFB ) );

			return false;
		}

		// 맵 이동을 진행한다;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// 재시도 컨테이너에 추가한다;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// 맵 이동 실패 로그;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// 이동 큐에 넣는다;
		//m_queueMoveMap.push( _sMoveInfo );

		return true;
	}

	const bool CMoveManagerField::MoveMap_UseItem ( SMOVE_INFO& _sMoveInfo, SINVENITEM* _pINVENITEM )
	{
		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;

		if ( NULL == m_pServer )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map Use Item, Server is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map Use Item, This Character is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		// 아이템 사용 가능한지 체크한다;
		sNetMsgFB.emFB = MoveSystem::CMoveManagerField::Instance()->IsUseMoveItem(
			_sMoveInfo.dwChaDBNum,
			_sMoveInfo.sMapID,
			_pINVENITEM );
		if ( EMREQ_MOVEMAP_RESULT_AFB_OK != sNetMsgFB.emFB )
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map Use Item, IsUseMoveItem : %1%, Error Code : %2% ]",
				_sMoveInfo.dwChaDBNum,
				sNetMsgFB.emFB ) );

			return false;
		}

		// 캐릭터가 유효한지 체크한다;

		if ( false == IsValidCharacter( _sMoveInfo.dwChaDBNum ) )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map Use Item, This Character is Invalid : %1%, Error Code : %2% ]",
				_sMoveInfo.dwChaDBNum,
				sNetMsgFB.emFB ) );

			return false;
		}

		// 이동 가능 기본 체크한다;
		sNetMsgFB.emFB = IsCanMoveMap( _sMoveInfo );
		if ( EMREQ_MOVEMAP_RESULT_AFB_OK != sNetMsgFB.emFB )
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Move Map Use Item, IsCanMoveMap : %1%, Error Code : %2% ]",
				_sMoveInfo.dwChaDBNum,
				sNetMsgFB.emFB ) );

			return false;
		}

		// 맵 이동을 진행한다;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// 재시도 컨테이너에 추가한다;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// 맵 이동 실패 로그;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// 이동 큐에 넣는다;
		//m_queueMoveMap.push( _sMoveInfo );
	}


	const bool CMoveManagerField::Resurrection ( SMOVE_INFO& _sMoveInfo )
	{
		_sMoveInfo.emMoveType = EMMOVE_TYPE_RESURRECT;

		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;

		if ( NULL == m_pServer )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Resurrection, Server is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );
			return false;
		}

		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Resurrection, This Character is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		sNetMsgFB.emFB = IsCanMoveMap( _sMoveInfo );
		if ( EMREQ_MOVEMAP_RESULT_AFB_OK != sNetMsgFB.emFB )
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Failed Check to Resurrection, IsCanMoveMap : %1%, Error Code : %2% ]",
				_sMoveInfo.dwChaDBNum,
				sNetMsgFB.emFB ) );

			return false;
		}

		// 맵 이동을 진행한다;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// 재시도 컨테이너에 추가한다;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// 맵 이동 실패 로그;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// 이동 큐에 넣는다;
		//m_queueMoveMap.push( _sMoveInfo );
	}


	const bool CMoveManagerField::_RetryMoveMapProc ( SMOVE_INFO _sMoveInfo )
	{
		// 재시도 횟수가 최대 재시도 횟수를 넘을 경우 Move Map 을 실패시킨다;
		//if ( _sMoveInfo.dwRetryCount >= MAX_RETRY_COUNT )
		{
			GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
			if ( NULL == pChar )
				return false;

			// Client 에 실패를 알린다;
			GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			return false;
		}

		/*// 재시도 횟수를 추가한다;
		_sMoveInfo.dwRetryCount++;

		// 다시한번 재시도한다;
		m_vecMoveMap.push_back( _sMoveInfo );*/

		return true;
	}


	const bool CMoveManagerField::_MoveMapProc ( const SMOVE_INFO& _sMoveInfo )
	{
		if ( NULL == m_pServer )
			return false;

		// 캐릭터 정보를 받아온다;
		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return false;

		/// 맵 이동을 하기 전의 준비작업을 한다;
		// 매크로가 켜져있다면 맵이동시 끈다;
		if ( pChar->IsActState( EM_ACT_MACROMODE ) )
		{
			pChar->MacroOnOff( false );

			GLMSG::SNET_MACROMODE_CLOSE_FC sMacroCloseMsg;
			_SendToClient( pChar->ClientSlot(), &sMacroCloseMsg );
		}

		/// Move Map Logic 컨트롤 메시지;
		// 모두 Agent 로 보내어 순서가 보장되도록 해야함에 주의한다;
		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;
		sNetMsgFB.dwChaNum = pChar->CharDbNum();

		/// 클라이언트에 로딩을 시작하라고 알린다;
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_LOADING;
		sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
		sNetMsgFB.vPOS = pChar->GetPosition();

		// 에뮬레이터의 경우 클라이언트로 바로 보낸다;
		if (true == m_pServer->IsEmulatorMode())
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );
		}
		else
		{
			_SendToAgent( &sNetMsgFB );
		}

		/// 맵 정보를 파악한다;
		// 출발 맵 정보를 파악한다;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
		if ( NULL == pMapNode )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToAgent( &sNetMsgFB );

			return false;
		}

		// 도착 맵 정보를 파악한다;
		const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
		if ( NULL == pArriveMapNode )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToAgent( &sNetMsgFB );

			return false;
		}
		
		/// 맵이동을 수행한다;

		/// 0. Emulator;
		// -> 에뮬레이터일 경우에는 Agent 가 사용되지 않으므로 이곳에서 이동처리를 해준다;
		if ( m_pServer->IsEmulatorMode() )
		{
			/// 이동하기 전에 캐릭터가 죽어있으면 살린다;
			// 현재는 무조건 100% 로 회복시킨다;
			bool bResurrect = false;
			if ( false == IsValidCharacter( pChar->CharDbNum() ) )
			{
				sc::writeLogDebug( sc::string::format(
					"[ Move Map ] [ Resurrect Character : %1% / %2% ]",
					pChar->CharDbNum(),
					pLand->GetMapID().getGaeaMapID().Mid() ) );

				pChar->SetAlive( 100.f );

				bResurrect = true;
			}

			// _RequestMoveMap_ThisFieldServer 함수 호출 이후에는 케릭터의 현재 mapID 가 이동하는 mapID 로 변경된다;
			const MapID sCurMapID = pChar->GetCurrentMap();

			const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
			if ( NULL == pMapNode )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

				return false;
			}

			BOOL bOK = _RequestMoveMap_ThisFieldServer( _sMoveInfo );
			if ( FALSE == bOK )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

				return false;
			}

			// 출발지와 목적지가 다를 경우 버프와 상태이상을 제거한다;
			if( sCurMapID != _sMoveInfo.sMapID )
			{
				for ( int i = 0; i < EMBLOW_MULTI; ++i )
					pChar->DISABLEBLOW( i );
				for ( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pChar->m_sSKILLFACT[i].sNATIVEID );
					if ( NULL == pSkill )
						continue;

					SKILL::EMACTION_TYPE emActionType = pSkill->m_sBASIC.emACTION_TYPE;
					if ( SKILL::EMACTION_TYPE_NORMAL != emActionType &&
						SKILL::EMACTION_TYPE_BUFF != emActionType )
						pChar->RESETSKEFF( i );
				}

				// CP 초기화 맵인지 확인한다;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action 을 리셋한다;
			pChar->ResetAction();

			// 이동 성공을 알린다;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			// 부활을 알린다;
			if ( bResurrect )
			{
				GLMSG::NET_CHAR_RESURRECT_FAFC sNetMsg(
					pChar->GetCurrentMap(),
					pChar->GetPosition(),
					pChar->GetNowHP(),
					pChar->GetNowMP(),
					pChar->GetNowSP(),
					pLand->IsCullByObjectMap(),
					_sMoveInfo.dwChaDBNum
					);

				_SendToClient( pChar->ClientSlot(), &sNetMsg );
			}
		}

		/// 1. Instance -> Field, Instance -> Instance;
		// -> 인던 이동은 인던시스템을 거쳐서 와야한다;
		// -> 인던시스템 처리가 완료되지 않았을 경우에는 이곳으로 들어온다;
		if ( ( pMapNode->IsNewInstanceMap() ) && ( false == _sMoveInfo.bCompleteProc_InstanceSystem ) )
		{
			// 이전 맵의 스크립트에 게이트 이동에 관한 제어를 묻는다.;
			//! 주의: Gate 0번으로 오는것이 꼭 실제 게이트 이동뿐만이 아니라 맵 입장조건이 안맞게 되어서 팅겨질때도 Gate0을 사용한다;
			InstanceSystem::InstanceField* const pInstance( pLand->GetScriptInstance() );
			if ( NULL != pInstance )
			{
				InstanceSystem::ManagerField* const pInstanceMan = m_pServer->GetInstanceManagerField();
				if ( pInstanceMan )
				{
					EMCROW emGateType = CROW_NULL;
					if ( _sMoveInfo.dwGateID != UINT_MAX )
					{
						emGateType = CROW_GATE;
					}

					InstanceSystem::InformationPosition sInformationPosition(
						Faction::GLActorID( CROW_PC, pChar->GetCharID() ),
						_sMoveInfo.sMapID.getBaseMapID(),
						Faction::GLActorID( emGateType, _sMoveInfo.dwGateID ),
						_sMoveInfo.vPosition.x,
						_sMoveInfo.vPosition.z,
						pArriveMapNode->IsNewInstanceMap() ? InstanceSystem::EMMOVE_INSTANCE2INSTANCE : InstanceSystem::EMMOVE_INSTANCE2FIELD );
					sInformationPosition.bResurrect = !IsValidCharacter( _sMoveInfo.dwChaDBNum );

					pInstanceMan->requestMoveToPosition(
						pLand->GetMapID().getGaeaMapID().Mid(),
						sInformationPosition );
				}

				return true;
			}
		}

		/// 2. Instance -> Instance;
		// -> 인던시스템의 처리가 완료되었을 경우 이곳으로 들어온다;
		// -> Instance -> Field 의 경우에는 OnMoveMap 콜백으로 가게되므로 그곳을 참조한다;
		if ( pMapNode->IsNewInstanceMap() && pArriveMapNode->IsNewInstanceMap() && _sMoveInfo.bCompleteProc_InstanceSystem )
		{
			/// 이동하기 전에 캐릭터가 죽어있으면 살린다;
			// 현재는 무조건 100% 로 회복시킨다;
			bool bResurrect = false;
			if ( false == IsValidCharacter( pChar->CharDbNum() ) )
			{
				sc::writeLogDebug( sc::string::format(
					"[ Move Map ] [ Resurrect Character : %1% / %2% ]",
					pChar->CharDbNum(),
					pLand->GetMapID().getGaeaMapID().Mid() ) );

				pChar->SetAlive( 100.f );

				bResurrect = true;
			}

			// _RequestMoveMap_ThisFieldServer 함수 호출 이후에는 케릭터의 현재 mapID 가 이동하는 mapID 로 변경된다;
			const MapID sCurMapID = pChar->GetCurrentMap();

			BOOL bOK = _RequestMoveMap_ThisFieldServer( _sMoveInfo );
			if ( FALSE == bOK )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToAgent( &sNetMsgFB );

				return false;
			}

			// 출발지와 목적지가 다를 경우 버프와 상태이상을 제거한다;
			if( sCurMapID != _sMoveInfo.sMapID )
			{
				for ( int i = 0; i < EMBLOW_MULTI; ++i )
					pChar->DISABLEBLOW( i );
				for ( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pChar->m_sSKILLFACT[i].sNATIVEID );
					if ( NULL == pSkill )
						continue;

					SKILL::EMACTION_TYPE emActionType = pSkill->m_sBASIC.emACTION_TYPE;
					if ( SKILL::EMACTION_TYPE_NORMAL != emActionType &&
						SKILL::EMACTION_TYPE_BUFF != emActionType )
						pChar->RESETSKEFF( i );
				}

				// CP 초기화 맵인지 확인한다;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action 을 리셋한다;
			pChar->ResetAction();

			// 이동 성공을 알린다;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToAgent( &sNetMsgFB );

			// 부활을 알린다;
			if ( bResurrect )
			{
				GLMSG::NET_CHAR_RESURRECT_FAFC sNetMsg(
					pChar->GetCurrentMap(),
					pChar->GetPosition(),
					pChar->GetNowHP(),
					pChar->GetNowMP(),
					pChar->GetNowSP(),
					pLand->IsCullByObjectMap(),
					_sMoveInfo.dwChaDBNum
					);

				_SendToClient( pChar->ClientSlot(), &sNetMsg );
			}

			return true;
		}


		/// 3. Field -> Field;
		// -> 다른 필드의 경우 Agent 로 전달한다;
		// -> 같은 필드의 경우 이동시킨다;
		if ( pArriveMapNode->GetFieldServerNum() != m_pServer->GetFieldSvrID() )
		{
			GLMSG::SNETPC_REQ_MOVEMAP_FA NetMsgAg;
			NetMsgAg.emMoveType = _sMoveInfo.emMoveType;
			NetMsgAg.dwChaNum = pChar->CharDbNum();
			NetMsgAg.dwFieldServerIDTo = pArriveMapNode->GetFieldServerNum();
			NetMsgAg.dwChannelTo = pChar->GetServerChannel();
			NetMsgAg.mapIDTo = _sMoveInfo.sMapID;
			NetMsgAg.dwGATEID = _sMoveInfo.dwGateID;
			NetMsgAg.vPosition = _sMoveInfo.vPosition;
			NetMsgAg.dwRetryCount = _sMoveInfo.dwRetryCount;

			_SendToAgent( &NetMsgAg );

			return true;
		}
		else
		{
			/// 이동하기 전에 캐릭터가 죽어있으면 살린다;
			// 현재는 무조건 100% 로 회복시킨다;
			bool bResurrect = false;
			if ( false == IsValidCharacter( pChar->CharDbNum() ) )
			{
				sc::writeLogDebug( sc::string::format(
					"[ Move Map ] [ Resurrect Character : %1% / %2% ]",
					pChar->CharDbNum(),
					pLand->GetMapID().getGaeaMapID().Mid() ) );

				pChar->SetAlive( 100.f );

				bResurrect = true;
			}

			// _RequestMoveMap_ThisFieldServer 함수 호출 이후에는 케릭터의 현재 mapID 가 이동하는 mapID 로 변경된다;
			const MapID sCurMapID = pChar->GetCurrentMap();

			BOOL bOK = _RequestMoveMap_ThisFieldServer( _sMoveInfo );
			if ( FALSE == bOK )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToAgent( &sNetMsgFB );

				return false;
			}

			// 출발지와 목적지가 다를 경우 버프와 상태이상을 제거한다;
			if( sCurMapID != _sMoveInfo.sMapID )
			{
				for ( int i = 0; i < EMBLOW_MULTI; ++i )
					pChar->DISABLEBLOW( i );
				for ( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pChar->m_sSKILLFACT[i].sNATIVEID );
					if ( NULL == pSkill )
						continue;

					SKILL::EMACTION_TYPE emActionType = pSkill->m_sBASIC.emACTION_TYPE;
					if ( SKILL::EMACTION_TYPE_NORMAL != emActionType &&
						SKILL::EMACTION_TYPE_BUFF != emActionType )
						pChar->RESETSKEFF( i );
				}

				// CP 초기화 맵인지 확인한다;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action 을 리셋한다;
			pChar->ResetAction();

			// 이동 성공을 알린다;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToAgent( &sNetMsgFB );

			// 부활을 알린다;
			if ( bResurrect )
			{
				GLMSG::NET_CHAR_RESURRECT_FAFC sNetMsg(
					pChar->GetCurrentMap(),
					pChar->GetPosition(),
					pChar->GetNowHP(),
					pChar->GetNowMP(),
					pChar->GetNowSP(),
					pLand->IsCullByObjectMap(),
					_sMoveInfo.dwChaDBNum
					);

				_SendToClient( pChar->ClientSlot(), &sNetMsg );
			}

			return true;
		}


		/// 4. Field -> Instance;
		// -> Agent 에서 처리되어 Field 의 OnMoveMap Callback 에서 처리된다;
		// -> 이곳에서는 아무런 처리도 하지 않는다;
		


		// 이동처리가 되지 않아야만 이곳으로 올 수 있다;
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		_SendToAgent( &sNetMsgFB );

		return false;
	}


	const bool CMoveManagerField::_RequestMoveMap_ThisFieldServer ( const SMOVE_INFO& _sMoveInfo )
	{
		if ( NULL == m_pServer )
			return false;

		// 캐릭터 정보를 받아온다;
		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 맵 이동시 Pet 을 삭제한다;
		m_pServer->DropOutPET( pChar->m_dwPetGUID, false, true );

		// 맵 이동시 탈것 정보를 저장한다;
		if ( _sMoveInfo.emMoveType == MoveSystem::EMMOVE_TYPE_GATE )
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, true );
		else
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, false );

		// 맵 이동시 Summon 을 모두 삭제한다;
		m_pServer->DropOutAllSummon( pChar );

		const MapID sCurMapID = pChar->GetCurrentMap();

		// 이전 맵 정보;
		const SNATIVEID& previousMapID = sCurMapID.getGaeaMapID();

		// 도착지의 Land 정보를 얻어와서 제대로된 도착지인지 확인한다;
		GLLandMan* pLandMan = m_pServer->GetLand( _sMoveInfo.sMapID );
		if ( NULL == pLandMan )
			return false;	

		if ( SNATIVEID( false ) == pLandMan->GetGaeaMapID() )
			return false;

		// 도착지 Gate 가 존재하면 Gate 주변으로 위치시키고 아니라면 도착위치에 위치시킨다;
		D3DXVECTOR3 vStartPos;
		if ( UINT_MAX != _sMoveInfo.dwGateID )
		{
			bool bInvalidGate = true;
			DxLandGateMan* pGateMan = pLandMan->GetLandGateMan ();
			if ( pGateMan )
			{
				PDXLANDGATE pGate = pGateMan->FindLandGate ( _sMoveInfo.dwGateID );
				if ( pGate )
				{
					vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );

					bInvalidGate = false;
				}
			}

			// 게이트가 유효하지 않으면 에러를 출력한다;
			if ( bInvalidGate )
			{
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Invalid Gate ID : %1%/%2% ) ]", 
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.dwGateID ) );
			}
		}
		else
		{
			vStartPos = _sMoveInfo.vPosition;
		}

		// 자신이 본 주변 셀을 정리한다;
		pChar->ResetViewAround();

		// 현재 활성화된 스킬 클리어;
		pChar->ActiveSkillClear();

		// 이전 맵에 있던 관리 노드를 삭제한다;
		GLLandMan* pCurrentLandMan( pChar->GetLandMan() );
		if ( pCurrentLandMan )
		{
			const MapID& sLandMapID = pCurrentLandMan->GetMapID();
			if ( false == pCurrentLandMan->AddGlobPcDelList( pChar->m_pCharNodeInLand ) )
			{
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Invalid pPC->m_pCharNodeInLand, LandMapID( %1%/%2%, %3%/%4% ) ]", 
					sLandMapID.getBaseMapID().Mid(),
					sLandMapID.getBaseMapID().Sid(),
					sLandMapID.getGaeaMapID().Mid(),
					sLandMapID.getGaeaMapID().Sid() ) );
			}

			// 캐릭터가 속한 쿼드노드에서 제거한다;
			if ( pChar->m_pQuadNode && pChar->m_pQuadNode->pData )
			{
				if ( false == pChar->m_pQuadNode->pData->m_PCList.DELNODE( pChar->m_pCellNode ) )
				{
					sc::writeLogError( sc::string::format(
						"[ Move Map ] [ Invalid pPC->m_pCellNode, LandMapID( %1%/%2%, %3%/%4% ) ]",
						sLandMapID.getBaseMapID().Mid(),
						sLandMapID.getBaseMapID().Sid(),
						sLandMapID.getGaeaMapID().Mid(),
						sLandMapID.getGaeaMapID().Sid() ) );
				}
			}
			else
			{
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Invalid pPC->m_pQuadNode, LandMapID( %1%/%2%, %3%/%4% ) ]", 
					sLandMapID.getBaseMapID().Mid(),
					sLandMapID.getBaseMapID().Sid(),
					sLandMapID.getGaeaMapID().Mid(),
					sLandMapID.getGaeaMapID().Sid() ) );		
			}
		}

		// 새로운 맵 위치로 내비게이션 초기화한다;
		pChar->SetNavi( pLandMan->GetNavi(), vStartPos );

		// Land 에 입장한다;
		_EnterLand( pChar, pLandMan );

		// Action 을 초기화한다;
		pChar->ResetAction();

		/// 구인던이 모두 제거되면 삭제한다;
		/// 신규인던이 아닌데 인던인 경우 에이전트 서버에 알린다;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
		if ( pMapNode )
		{
			if ( pChar && pLandMan->isMapInstant() && ( false == pMapNode->IsNewInstanceMap() ) )
			{
				GLMSG::SNET_CREATE_INSTANT_MAP_FB sNetMsgFB;
				sNetMsgFB.dwChaNum = pChar->CharDbNum();
				sNetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_OK;
				sNetMsgFB.bCullByObjectMap = pLandMan->IsCullByObjectMap();
				sNetMsgFB.mapID = pLandMan->GetMapID();
				sNetMsgFB.vPos = pChar->GetPosition();

				_SendToAgent( &sNetMsgFB );
			}
		}

		return true;
	}


	void CMoveManagerField::_EnterLand ( GLChar* const _pChar, GLLandMan* const _pLand )
	{
		if ( NULL == m_pServer )
			return;

		/*sc::writeLogDebug( sc::string::format(
			"[ Move Map ] [ EnterLand : %1% / %2% ]",
			_pChar->CharDbNum(),
			_pLand->GetMapID().getGaeaMapID().Mid() ) );*/

		// 도착 Land 에 입장한다;
		_pLand->EnterLand( _pChar );

		_pChar->m_dwCeID = 0;
		_pChar->m_pQuadNode = NULL;
		_pChar->m_pCellNode = NULL;
	}


	void CMoveManagerField::_SendToClient ( DWORD dwClientID, NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOCLIENT( dwClientID, pMsg );
	}

	void CMoveManagerField::_SendToAgent ( NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOAGENT( pMsg );
	}

	void CMoveManagerField::_SendToMyself ( NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOMYSELF( pMsg );
	}

}