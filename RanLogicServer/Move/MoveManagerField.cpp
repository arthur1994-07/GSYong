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

		// �̵� ť�� ū �ǹ̰� ��� �����Ѵ�;
/*#ifdef _USE_TBB
		while ( m_queueMoveMap.try_pop( sTempMoveMapInfo ) )
		{
#else
		while ( false == m_queueMoveMap.empty() )
		{
			sTempMoveMapInfo = m_queueMoveMap.front();
			m_queueMoveMap.pop();
#endif
			// Move Map �� �������� Ȯ���Ѵ�;
			if ( EMREQ_MOVEMAP_RESULT_AFB_OK != IsCanMoveMap( sTempMoveMapInfo ) )
			{
				// �� �κп��� ���а� �Ͼ�� ��õ��� ���� �ʴ´�;
				// MoveMap Function �� ������ �� �˻��ϱ� ������ �ѹ� �� Ȯ���ϴ� ������ �ǹ̸� ������;
				continue;
			}

			// �� �̵��� �����Ѵ�;
			if ( false == _MoveMapProc( sTempMoveMapInfo ) )
			{
				/// _MoveMapProc �� �����ϴ� ���� ���� Field ������ �̵��ϴ� ���� �����ȴ�;
				/// �ٸ� Field �� �̵��ϴ� ���� �������� ��쿡�� OnFailToMove Callback �� ȣ��ȴ�;

				/// �� �������� ���и� �˾Ƶ� ����� ���� �������� �������� �ʴ´�;
				/// �������� �����ϴ� �ڵ带 �ְų� ������ ó�� ������ �����ؾ� �Ѵ�;

				// ��õ� �����̳ʿ� �߰��Ѵ�;
				if ( false == _RetryMoveMapProc( sTempMoveMapInfo ) )
				{
					// �� �̵� ���� �α�;
					sc::writeLogError( sc::string::format(
						"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
						sTempMoveMapInfo.dwChaDBNum,
						sTempMoveMapInfo.sMapID.getGaeaMapID().Mid() ) );
				}
			}
		}
*/
		// ��õ� �Ѵ�;
		for ( UINT i=0; i<m_vecMoveMap.size(); ++i )
		{
			SMOVE_INFO& sMoveInfo = m_vecMoveMap.at( i );

			// ��õ� �α�;
			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ Retry Move Map : %1% / %2% ]",
				sMoveInfo.dwChaDBNum,
				sMoveInfo.sMapID.getGaeaMapID().Mid() ) );

			MoveMap( sMoveInfo );
		}

		// ��� �ٽ� ��õ� �����Ƿ� �ʱ�ȭ�Ѵ�;
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

		// Land �� �����Ѵ�;
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

			// ��߸ʰ� ������ �߿� �ϳ��� �δ��� ��쿡 ó���Ѵ�;
			const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sPreviousMapID );
			const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
			if ( pMapNode && pArriveMapNode )
			{
				// �̵��� �̷���� �Ŀ� Instance System �� �˸���;
				if ( pMapNode->IsNewInstanceMap() || pArriveMapNode->IsNewInstanceMap() )
				{
					GLMSG::NET_INSTANCE_MOVE_MAP sInstanceMoveMapMsg(
						_dwChaDBNum,
						_sPreviousMapID,
						pChar->GetPosition(),
						//m_pServer->GetFieldSvrID(),
						pLand->GetMapID() );
					_SendToMyself( &sInstanceMoveMapMsg );

					// �δ� ���� �̺�Ʈ �߻�;
					// �������� �δ��� ��쿡�� ���´�;
					if ( ( false == pMapNode->IsNewInstanceMap() ) && ( pArriveMapNode->IsNewInstanceMap() ) )
					{
						D3DXVECTOR3 vAdsjustPosition = pChar->GetPosition();
						InstanceSystem::InstanceField* const pInstance = pLand->GetScriptInstance();
						if ( pInstance )
						{
							if ( pInstance->EventJoin( _dwChaDBNum, pLand, vAdsjustPosition ) )
							{
								// ����� ��ġ ������ �ִٸ� �ش��ϴ� ��ġ�� �ٲ۴�.;
								pChar->SetPosition( vAdsjustPosition );
							}
						}
					}
				}
			}
		}	

		/// ���� �������� ������ ��ʸ�Ʈ�� �����Ǿ� �����Ƿ�;
		/// ��ʸ�Ʈ�� ���� ���� ��쿡�� �����ؾ� �Ѵ�;
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

		// ���� Ÿ�Կ� ���� �ٸ� ó���� �Ѵ�;
		switch ( _emGameJoinType )
		{
		case EMJOINTYPE_FIRST:
			{
				// ĳ������ ù��° ������ ��� ó���Ѵ�;
				// ĳ������ ���� ������ �����Ѵ�;
				pChar->MsgGameJoin();

				// ĳ���� ���� ���� Ʈ���Ÿ� �߻���Ų��;
				triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_Connect_To_Field().Do( pChar );
			}
			break;

		case EMJOINTYPE_TRANSFER:	// Instance Server �̵����� ����� ������� �ʴ´� ( Instance Server �����ϴ� ���Ҵ� );
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
				// �̵��� ��� ó���Ѵ�;
				if ( pLand )
				{
					// ����Ʈ �̵� Ȯ�� �޽���;
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
				// ��Ȱ�� ��� ó���Ѵ�;
				if ( pLand )
				{
					// ��Ȱ Ȯ�� �޽���;
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
				// PVE �δ��� ��� ó���Ѵ�;
				if ( pLand )
				{
					// ����Ʈ �̵� Ȯ�� �޽���;
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

					// PVE �δ� �̵� Ȯ�� �޽���;
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

		// �� �̵��� ����� ����Ѵ�;
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

		// ��ũ�ΰ� �����ִٸ� ���̵��� ����;
		if ( pChar->IsActState( EM_ACT_MACROMODE ) )
		{
			pChar->MacroOnOff( false );

			GLMSG::SNET_MACROMODE_CLOSE_FC sMacroCloseMsg;
			_SendToClient( pChar->ClientSlot(), &sMacroCloseMsg );
		}

		/// �̵��ϱ� ���� ĳ���Ͱ� �׾������� �츰��;
		// ����� ������ 100% �� ȸ����Ų��;
		if ( false == IsValidCharacter( pChar->CharDbNum() ) )
		{
			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ Resurrect Character : %1% / %2% ]",
				pChar->CharDbNum(),
				pLand->GetMapID().getGaeaMapID().Mid() ) );

			pChar->SetAlive( 100.f );
		}

		// �� �̵��� Pet �� �����Ѵ�;
		m_pServer->DropOutPET( pChar->m_dwPetGUID, true, true );

		// �� �̵��� Ż�� ������ �����Ѵ�;
		if ( pOutMsg->emMoveType == MoveSystem::EMMOVE_TYPE_GATE )
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, true );
		else
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, false );

		// �� �̵��� Summon �� ��� �����Ѵ�;
		m_pServer->DropOutAllSummon( pChar );

		// ��߸��̳� �������� �ű��δ��� ��� �̵��� �̷������ ���� Instance System �� �˸���;
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

					// �δ� ���� �̺�Ʈ �߻�;
					// ��߸��� �δ��̰� �������� �δ��� �ƴ� ��쿡�� ���´�;
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

		/// Move Map �� �����Ѵ�;

		// �ٸ� Field ���� �̵��̰ų� �ű��δ��� ��쿡 ó���Ѵ�;
		// �ű��δ��� ���� ������ ���� ������ �Ѵ�;
		//const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( pOutMsg->targetMapID );
		//if ( ( FALSE == pOutMsg->bSameFieldServer ) || ( pArriveMapNode && pArriveMapNode->IsNewInstanceMap() ) )
		{
			/// Agent �� ���޵Ǵ� Field Out Message;
			// Field �� ������ �Ǹ� ��ϵ� ���� Field �� �̵��ȴ�;
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
		/*// ���� Field ���� �̵��̰ų� �ű��δ��� �ƴ� ��쿡 ó���Ѵ�;
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

			// _RequestMoveMap_ThisFieldServer �Լ� ȣ�� ���Ŀ��� �ɸ����� ���� mapID �� �̵��ϴ� mapID �� ����ȴ�;
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

			// ������� �������� �ٸ� ��� ������ �����̻��� �����Ѵ�;
			if( sCurMapID != sMoveInfo.sMapID )
			{
				for ( int i = 0; i < EMBLOW_MULTI; ++i )
					pChar->DISABLEBLOW( i );
				for ( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
					pChar->RESETSKEFF( i );

				// CP �ʱ�ȭ ������ Ȯ���Ѵ�;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action �� �����Ѵ�;
			pChar->ResetAction();

			// �̵� ������ �˸���;
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

		// ĳ���� �̵� ����;
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

		// �ʵ� ���� �̵� ���� �ÿ� ���´�;
		// ��õ� ó���� �ʿ��ϸ�, ��õ� ���� ���� ��� Client �� �˷��� �Ѵ�;

		/// �� �������� ���и� �˾Ƶ� ����� ���� �������� �������� �ʴ´�;
		/// �������� �����ϴ� �ڵ带 �ְų� ������ ó�� ������ �����ؾ� �Ѵ�;

		// ��õ� �����̳ʿ� �߰��Ѵ�;
		SMOVE_INFO sTempMoveMapInfo;
		sTempMoveMapInfo.emMoveType = pStateMsg->emMoveType;
		sTempMoveMapInfo.dwChaDBNum = pStateMsg->dwChaNum;
		sTempMoveMapInfo.sMapID = pStateMsg->sMAPID;
		sTempMoveMapInfo.vPosition = pStateMsg->vPOS;
		sTempMoveMapInfo.dwGateID = pStateMsg->dwGATEID;
		sTempMoveMapInfo.dwRetryCount = pStateMsg->dwRetryCount;

		if ( false == _RetryMoveMapProc( sTempMoveMapInfo ) )
		{
			// �� �̵� ���� �α�;
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

		// �� �̵��� ����� ����Ѵ�; // OnMoveMap �� �ִ� �Լ�. OnMoveMap�� ���� ȣ����� �ʾ� CompleMove�� �̵�
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
			// ĳ���� �ֺ��� ������ �����Ѵ� ( View Around );
			pChar->GetViewAround ();

			//
			// mjeon.activity
			//
			// When a character moves into other FieldServer, it may take some time to load activity. 
			// At the time, this ActivityCheckMap() can be failed because ActivityLoad() did not completed yet. 
			// But in this case, CheckAfterLoad() will call CheckMap() inside it. Just ignore this failure.
			//
			pChar->ActivityCheckMap();

			// �̵��� �Ϸ�Ǵ� ������ ĳ���Ͱ� ó���ؾ� �� �κ�;
			pChar->ToDoRightAfterRecallDone();

			pChar->m_mapIDPrevious = pChar->GetCurrentMap();
			pChar->m_tLoginTime = pCompleteMsg->tLoginTime;

			// Instance System �� �̵� ������ �˸�;		
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

		// ������ ���� ������;
		const SITEM* pItem = GLogicData::GetInstance().GetItem( _pINVENITEM->sItemCustom.GetNativeID() );
		if ( !pItem )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		}

		/// ��� ���� ������ ��� ���� �������� Ȯ���Ѵ�;
		const SMAPNODE* pCurMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
		if ( ( NULL == pCurMapNode ) ||
			( pCurMapNode->IsBlockDrugItem( pItem->BasicType() ) ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		/// ���� ���� ������ ��� ���� �������� Ȯ���Ѵ�;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMapID );
		if ( ( NULL == pMapNode ) ||
			( pMapNode->IsBlockDrugItem( pItem->BasicType() ) ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		}

		// �������� ��Ÿ���� üũ�Ѵ�;
		if ( pChar->CheckCoolTime( _pINVENITEM->sItemCustom.GetNativeID() ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		}

		// �������� ����� �� ���� ��Ȳ�� ��� ó���Ѵ�;
		if ( pChar->IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
		{
			return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		// ���� Vehicle ���¿� ���� ó���Ѵ�;
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

		// ��� ���� �� ��� ó���Ѵ�;
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

		// ĳ���� ���°� ������ �ƴ� ��쿡 ó���Ѵ�;
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

		/// ��߸ʰ� �������� �Ѵ� �δ��ε� ���� �ٸ� �δ��� ��� �̵��� ���´�;
		if ( ( true == pLand->IsNewInstantMap() ) && ( false == _sMoveInfo.sMapID.isSameID() ) )
		{
			if ( pLand->GetGaeaMapID().Mid() != _sMoveInfo.sMapID.getGaeaMapID().Mid() )
				return EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
		}

		/// �Ϲ� ó��;

		// �ŷ����� ��� ó���Ѵ�;
		if ( pChar->m_sTrade.Valid() )
		{
			// �ŷ��� ����Ѵ�;
			pChar->m_sTrade.Reset();

			GLMSG::SNET_TRADE_CANCEL_TAR sNetMsg;
			sNetMsg.dwChaNum = pChar->CharDbNum();
			m_pServer->SENDTOAGENT( &sNetMsg );

			// �ŷ� Ÿ�ٰ��� �ŷ��� ����Ѵ�;
			GLChar* pTargetChar = m_pServer->GetChar( pChar->m_sTrade.GetTarget() );		
			if ( pTargetChar )
			{
				pTargetChar->m_sTrade.Reset();

				sNetMsg.dwChaNum = pTargetChar->CharDbNum();
				m_pServer->SENDTOAGENT( &sNetMsg );
			}
		}


		/// ���� Map ���� ���ѻ��� Ȯ��;

		// ������ Map ������ �ùٸ��� Ȯ���Ѵ�;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
		if ( NULL == pMapNode)
		{
			return EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		}

		// GM �� �ƴҰ�� ���� ������ �˻��Ѵ�;
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

		// üũ Ÿ���� ��� ĳ���Ͱ� ��ȿ���� üũ�Ѵ�;
		
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

		// �̵� ���� �⺻ üũ�Ѵ�;
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

		// �� �̵��� �����Ѵ�;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// ��õ� �����̳ʿ� �߰��Ѵ�;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// �� �̵� ���� �α�;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// �̵� ť�� �ִ´�;
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

		// ������ ��� �������� üũ�Ѵ�;
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

		// ĳ���Ͱ� ��ȿ���� üũ�Ѵ�;

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

		// �̵� ���� �⺻ üũ�Ѵ�;
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

		// �� �̵��� �����Ѵ�;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// ��õ� �����̳ʿ� �߰��Ѵ�;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// �� �̵� ���� �α�;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// �̵� ť�� �ִ´�;
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

		// �� �̵��� �����Ѵ�;
		if ( false == _MoveMapProc( _sMoveInfo ) )
		{
			// ��õ� �����̳ʿ� �߰��Ѵ�;
			if ( false == _RetryMoveMapProc( _sMoveInfo ) )
			{
				// �� �̵� ���� �α�;
				sc::writeLogError( sc::string::format(
					"[ Move Map ] [ Failed Move Map : %1% / %2% ]",
					_sMoveInfo.dwChaDBNum,
					_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );
			}
		}

		// �̵� ť�� �ִ´�;
		//m_queueMoveMap.push( _sMoveInfo );
	}


	const bool CMoveManagerField::_RetryMoveMapProc ( SMOVE_INFO _sMoveInfo )
	{
		// ��õ� Ƚ���� �ִ� ��õ� Ƚ���� ���� ��� Move Map �� ���н�Ų��;
		//if ( _sMoveInfo.dwRetryCount >= MAX_RETRY_COUNT )
		{
			GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
			if ( NULL == pChar )
				return false;

			// Client �� ���и� �˸���;
			GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			return false;
		}

		/*// ��õ� Ƚ���� �߰��Ѵ�;
		_sMoveInfo.dwRetryCount++;

		// �ٽ��ѹ� ��õ��Ѵ�;
		m_vecMoveMap.push_back( _sMoveInfo );*/

		return true;
	}


	const bool CMoveManagerField::_MoveMapProc ( const SMOVE_INFO& _sMoveInfo )
	{
		if ( NULL == m_pServer )
			return false;

		// ĳ���� ������ �޾ƿ´�;
		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return false;

		/// �� �̵��� �ϱ� ���� �غ��۾��� �Ѵ�;
		// ��ũ�ΰ� �����ִٸ� ���̵��� ����;
		if ( pChar->IsActState( EM_ACT_MACROMODE ) )
		{
			pChar->MacroOnOff( false );

			GLMSG::SNET_MACROMODE_CLOSE_FC sMacroCloseMsg;
			_SendToClient( pChar->ClientSlot(), &sMacroCloseMsg );
		}

		/// Move Map Logic ��Ʈ�� �޽���;
		// ��� Agent �� ������ ������ ����ǵ��� �ؾ��Կ� �����Ѵ�;
		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;
		sNetMsgFB.dwChaNum = pChar->CharDbNum();

		/// Ŭ���̾�Ʈ�� �ε��� �����϶�� �˸���;
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_LOADING;
		sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
		sNetMsgFB.vPOS = pChar->GetPosition();

		// ���ķ������� ��� Ŭ���̾�Ʈ�� �ٷ� ������;
		if (true == m_pServer->IsEmulatorMode())
		{
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );
		}
		else
		{
			_SendToAgent( &sNetMsgFB );
		}

		/// �� ������ �ľ��Ѵ�;
		// ��� �� ������ �ľ��Ѵ�;
		const SMAPNODE* pMapNode = m_pServer->FindMapNode( pLand->GetMapID() );
		if ( NULL == pMapNode )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToAgent( &sNetMsgFB );

			return false;
		}

		// ���� �� ������ �ľ��Ѵ�;
		const SMAPNODE* pArriveMapNode = m_pServer->FindMapNode( _sMoveInfo.sMapID );
		if ( NULL == pArriveMapNode )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToAgent( &sNetMsgFB );

			return false;
		}
		
		/// ���̵��� �����Ѵ�;

		/// 0. Emulator;
		// -> ���ķ������� ��쿡�� Agent �� ������ �����Ƿ� �̰����� �̵�ó���� ���ش�;
		if ( m_pServer->IsEmulatorMode() )
		{
			/// �̵��ϱ� ���� ĳ���Ͱ� �׾������� �츰��;
			// ����� ������ 100% �� ȸ����Ų��;
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

			// _RequestMoveMap_ThisFieldServer �Լ� ȣ�� ���Ŀ��� �ɸ����� ���� mapID �� �̵��ϴ� mapID �� ����ȴ�;
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

			// ������� �������� �ٸ� ��� ������ �����̻��� �����Ѵ�;
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

				// CP �ʱ�ȭ ������ Ȯ���Ѵ�;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action �� �����Ѵ�;
			pChar->ResetAction();

			// �̵� ������ �˸���;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToClient( pChar->ClientSlot(), &sNetMsgFB );

			// ��Ȱ�� �˸���;
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
		// -> �δ� �̵��� �δ��ý����� ���ļ� �;��Ѵ�;
		// -> �δ��ý��� ó���� �Ϸ���� �ʾ��� ��쿡�� �̰����� ���´�;
		if ( ( pMapNode->IsNewInstanceMap() ) && ( false == _sMoveInfo.bCompleteProc_InstanceSystem ) )
		{
			// ���� ���� ��ũ��Ʈ�� ����Ʈ �̵��� ���� ��� ���´�.;
			//! ����: Gate 0������ ���°��� �� ���� ����Ʈ �̵��Ӹ��� �ƴ϶� �� ���������� �ȸ°� �Ǿ �ð������� Gate0�� ����Ѵ�;
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
		// -> �δ��ý����� ó���� �Ϸ�Ǿ��� ��� �̰����� ���´�;
		// -> Instance -> Field �� ��쿡�� OnMoveMap �ݹ����� ���ԵǹǷ� �װ��� �����Ѵ�;
		if ( pMapNode->IsNewInstanceMap() && pArriveMapNode->IsNewInstanceMap() && _sMoveInfo.bCompleteProc_InstanceSystem )
		{
			/// �̵��ϱ� ���� ĳ���Ͱ� �׾������� �츰��;
			// ����� ������ 100% �� ȸ����Ų��;
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

			// _RequestMoveMap_ThisFieldServer �Լ� ȣ�� ���Ŀ��� �ɸ����� ���� mapID �� �̵��ϴ� mapID �� ����ȴ�;
			const MapID sCurMapID = pChar->GetCurrentMap();

			BOOL bOK = _RequestMoveMap_ThisFieldServer( _sMoveInfo );
			if ( FALSE == bOK )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToAgent( &sNetMsgFB );

				return false;
			}

			// ������� �������� �ٸ� ��� ������ �����̻��� �����Ѵ�;
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

				// CP �ʱ�ȭ ������ Ȯ���Ѵ�;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action �� �����Ѵ�;
			pChar->ResetAction();

			// �̵� ������ �˸���;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToAgent( &sNetMsgFB );

			// ��Ȱ�� �˸���;
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
		// -> �ٸ� �ʵ��� ��� Agent �� �����Ѵ�;
		// -> ���� �ʵ��� ��� �̵���Ų��;
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
			/// �̵��ϱ� ���� ĳ���Ͱ� �׾������� �츰��;
			// ����� ������ 100% �� ȸ����Ų��;
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

			// _RequestMoveMap_ThisFieldServer �Լ� ȣ�� ���Ŀ��� �ɸ����� ���� mapID �� �̵��ϴ� mapID �� ����ȴ�;
			const MapID sCurMapID = pChar->GetCurrentMap();

			BOOL bOK = _RequestMoveMap_ThisFieldServer( _sMoveInfo );
			if ( FALSE == bOK )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
				_SendToAgent( &sNetMsgFB );

				return false;
			}

			// ������� �������� �ٸ� ��� ������ �����̻��� �����Ѵ�;
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

				// CP �ʱ�ȭ ������ Ȯ���Ѵ�;
				if ( pMapNode->IsCombatPointReset() )
					pChar->ResetCP();
			}

			// Action �� �����Ѵ�;
			pChar->ResetAction();

			// �̵� ������ �˸���;
			sNetMsgFB.dwChaNum = pChar->CharDbNum();
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_OK;
			sNetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
			sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
			sNetMsgFB.vPOS = pChar->GetPosition();
			_SendToAgent( &sNetMsgFB );

			// ��Ȱ�� �˸���;
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
		// -> Agent ���� ó���Ǿ� Field �� OnMoveMap Callback ���� ó���ȴ�;
		// -> �̰������� �ƹ��� ó���� ���� �ʴ´�;
		


		// �̵�ó���� ���� �ʾƾ߸� �̰����� �� �� �ִ�;
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		_SendToAgent( &sNetMsgFB );

		return false;
	}


	const bool CMoveManagerField::_RequestMoveMap_ThisFieldServer ( const SMOVE_INFO& _sMoveInfo )
	{
		if ( NULL == m_pServer )
			return false;

		// ĳ���� ������ �޾ƿ´�;
		GLChar* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// �� �̵��� Pet �� �����Ѵ�;
		m_pServer->DropOutPET( pChar->m_dwPetGUID, false, true );

		// �� �̵��� Ż�� ������ �����Ѵ�;
		if ( _sMoveInfo.emMoveType == MoveSystem::EMMOVE_TYPE_GATE )
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, true );
		else
			m_pServer->SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false, false );

		// �� �̵��� Summon �� ��� �����Ѵ�;
		m_pServer->DropOutAllSummon( pChar );

		const MapID sCurMapID = pChar->GetCurrentMap();

		// ���� �� ����;
		const SNATIVEID& previousMapID = sCurMapID.getGaeaMapID();

		// �������� Land ������ ���ͼ� ����ε� ���������� Ȯ���Ѵ�;
		GLLandMan* pLandMan = m_pServer->GetLand( _sMoveInfo.sMapID );
		if ( NULL == pLandMan )
			return false;	

		if ( SNATIVEID( false ) == pLandMan->GetGaeaMapID() )
			return false;

		// ������ Gate �� �����ϸ� Gate �ֺ����� ��ġ��Ű�� �ƴ϶�� ������ġ�� ��ġ��Ų��;
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

			// ����Ʈ�� ��ȿ���� ������ ������ ����Ѵ�;
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

		// �ڽ��� �� �ֺ� ���� �����Ѵ�;
		pChar->ResetViewAround();

		// ���� Ȱ��ȭ�� ��ų Ŭ����;
		pChar->ActiveSkillClear();

		// ���� �ʿ� �ִ� ���� ��带 �����Ѵ�;
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

			// ĳ���Ͱ� ���� �����忡�� �����Ѵ�;
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

		// ���ο� �� ��ġ�� ������̼� �ʱ�ȭ�Ѵ�;
		pChar->SetNavi( pLandMan->GetNavi(), vStartPos );

		// Land �� �����Ѵ�;
		_EnterLand( pChar, pLandMan );

		// Action �� �ʱ�ȭ�Ѵ�;
		pChar->ResetAction();

		/// ���δ��� ��� ���ŵǸ� �����Ѵ�;
		/// �ű��δ��� �ƴѵ� �δ��� ��� ������Ʈ ������ �˸���;
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

		// ���� Land �� �����Ѵ�;
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