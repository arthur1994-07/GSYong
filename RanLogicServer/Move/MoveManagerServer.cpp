#include "pch.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../AgentServer/GLAgentServer.h"

#include "./MoveManagerServer.h"

namespace MoveSystem
{

	const bool CMoveManagerServer::OnOutServer ( GLMSG::SNETPC_REQ_MOVEMAP_FA* _pMoveMsg, DWORD _dwGaeaID )
	{
		if ( NULL == _pMoveMsg )
			return false;

		sc::writeLogDebug( sc::string::format(
			"[ Move Map ] [ Move Agent Relay : %1% / %2% ]",
			_pMoveMsg->dwChaNum,
			_pMoveMsg->mapIDTo.getGaeaMapID().Mid() ) );

		SMOVE_INFO_AGENT sTempMoveInfo;
		sTempMoveInfo.emMoveType = _pMoveMsg->emMoveType;
		sTempMoveInfo.dwChaDBNum = _pMoveMsg->dwChaNum;
		sTempMoveInfo.sMapID = _pMoveMsg->mapIDTo;
		sTempMoveInfo.dwGateID = _pMoveMsg->dwGATEID;
		sTempMoveInfo.vPosition = _pMoveMsg->vPosition;
		sTempMoveInfo.dwFieldNum = _pMoveMsg->dwFieldServerIDTo;
		sTempMoveInfo.dwChannelNum = _pMoveMsg->dwChannelTo;
		sTempMoveInfo.dwRetryCount = _pMoveMsg->dwRetryCount;

		if ( false == MoveMap( sTempMoveInfo ) )
			return false;

		return true;
	}


	void CMoveManagerServer::OnJoinServer ( GLMSG::SNETPC_FIELDSVR_OUT_FB* pOutMsg )
	{
		if ( NULL == pOutMsg )
			return;

		GLCharAG* pChar = m_pServer->GetCharByDbNum( pOutMsg->dwChaNum );
		if ( NULL == pChar )
			return;

		{
			/// 다른 Field 이동을 할 때에 인던에 알리는 부분이다;
			// Instance System 에 알린다;
			// Instance System 에서도 따로 캐릭터 정보를 관리하기 때문에 알려야한다;
			GLMSG::NET_INSTANCE_MOVE_MAP sInstanceMoveMapMsg(
				pOutMsg->dwChaNum,
				pChar->GetCurrentMap(),
				pChar->GetFieldMapPosition(),
				//pChar->GetCurrentFieldServer(),
				pChar->GetNextMap() );
			_SendToMyself( &sInstanceMoveMapMsg );
		}

		// 현재 필드 서버 및 맵ID 를 지정한다;
		pChar->SetCurrentField( pChar->m_dwNextChannel, pChar->m_dwNextFieldSvr, pChar->GetNextMap(), pChar->m_bIsInstance );
		pChar->SetMapPosition( pOutMsg->positionPrevious, false );

		// 맵에 새로 진입하기 위해서 Field 서버에 메시지를 송신한다;
		NET_GAME_JOIN_FIELDSVR sJoinMsg( pChar->GetCountry() );
		StringCchCopy( sJoinMsg.szUID, DAUM_MAX_UID_LENGTH+1, pChar->m_szUserName );
		sJoinMsg.dwSlotAgentClient = pChar->ClientSlot();
		sJoinMsg.emType	= pOutMsg->emType;
		sJoinMsg.nUserNum = pChar->m_UserDbNum;
		sJoinMsg.dwUserLvl = pChar->m_dwUserLvl;
		sJoinMsg.nChaNum = pChar->m_CharDbNum;
		sJoinMsg.dwGaeaID = pChar->GaeaId();
		sJoinMsg.PartyID = pChar->GetPartyID();
		sJoinMsg.tPREMIUM = pChar->m_tPREMIUM;
		sJoinMsg.tCHATBLOCK = pChar->m_tCHATBLOCK;
		sJoinMsg.sStartMap = pChar->GetNextMap();
		sJoinMsg.dwStartGate = pChar->m_dwNextGateID;
		sJoinMsg.vStartPos = pChar->m_vNextPos;
		sJoinMsg.dwActState	= pChar->m_dwActState;
		sJoinMsg.bUseArmSub	= pChar->m_bUseArmSub;
		sJoinMsg.emIPBonus = pChar->m_emIPBonus;
		sJoinMsg.sChinaTime	= pChar->m_sChinaTime;
		sJoinMsg.sEventTime	= pChar->m_sEventTime;
		sJoinMsg.sVietnamGainSystem	= pChar->m_sVietnamSystem;
		sJoinMsg.bChangeField = pOutMsg->bChangeField;
		sJoinMsg.sSummonable = pOutMsg->sSummonable;
		sJoinMsg.dwCountryID = pChar->GetCountryInfo().dwID;

		if ( pChar->GetCryptKey().guid != GUID_NULL )
		{
			sJoinMsg.ckey = pChar->GetCryptKey();
		}

		//_SendToField( pChar->ClientSlot(), &sJoinMsg );
		_SendToField( pChar->m_dwNextChannel, pChar->m_dwNextFieldSvr, &sJoinMsg );

		pChar->ResetNextFieldSvr();
	}


	void CMoveManagerServer::OnMoveMap_SameServer ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pMoveMsg )
	{
		if ( NULL == pMoveMsg )
			return;

		GLCharAG* pChar = m_pServer->GetCharByDbNum( pMoveMsg->dwChaNum );
		if ( NULL == pChar )
			return;

		// 이동이 성공이라면 캐릭터의 채널, 필드, 맵 정보를 세팅한다;
		if (EMREQ_MOVEMAP_RESULT_AFB_OK == pMoveMsg->emFB)
		{
			pChar->SetCurrentField( pChar->m_nChannel, pChar->m_dwCurFieldSvr, pMoveMsg->sMAPID );
		}

		_SendToClient( pChar->ClientSlot(), pMoveMsg );
	}


	const bool CMoveManagerServer::MoveMap ( const SMOVE_INFO_AGENT& _sMoveInfo )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError( "[ Move Map ] [ Server is NULL ]" );

			return false;
		}

		GLCharAG* pChar = m_pServer->GetCharByDbNum( _sMoveInfo.dwChaDBNum );
		if ( NULL == pChar)
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Char is NULL : %1% ]",
				_sMoveInfo.dwChaDBNum ) );

			return false;
		}

		GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB sNetMsgFB;
		sNetMsgFB.emMoveType = _sMoveInfo.emMoveType;
		sNetMsgFB.dwChaNum = _sMoveInfo.dwChaDBNum;
		sNetMsgFB.sMAPID = _sMoveInfo.sMapID;
		sNetMsgFB.vPOS = _sMoveInfo.vPosition;
		sNetMsgFB.dwGATEID = _sMoveInfo.dwGateID;
		sNetMsgFB.dwRetryCount = _sMoveInfo.dwRetryCount;

		// 친구 이동을 불가능 상태로 바꾼다;
		//pChar->m_bMapMoveFCard = FALSE;

		// 필드서버가 정상인지 검사한다;
		bool bSVR_OK = m_pServer->IsFieldOnline( _sMoveInfo.dwChannelNum, _sMoveInfo.dwFieldNum );
		if ( false == bSVR_OK )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Field is offline : %1% / %2% ]",
				_sMoveInfo.dwFieldNum,
				_sMoveInfo.dwChaDBNum ) );

			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToField( pChar->ClientSlot(), &sNetMsgFB );

			return false;
		}

		// 도착 Land 의 유효성 검사를 한다;
		GLAGLandMan* pNextLand = m_pServer->GetLand( _sMoveInfo.sMapID );
		if ( NULL == pNextLand )
		{	
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Invalid Map ID : %1% / %2% ]",
				_sMoveInfo.sMapID.getBaseMapID().Mid(),
				_sMoveInfo.sMapID.getGaeaMapID().Mid() ) );

			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToField( pChar->ClientSlot(), &sNetMsgFB );

			return false;
		}

		// Instance <-> Instance 이동인데 서로 다른 인던일 경우 막는다;
		SMAPNODE* pMapNode = m_pServer->FindMapNode( pChar->GetCurrentMap() );
		if ( pMapNode && pMapNode->IsNewInstanceMap() && pNextLand->IsNewInstantMap() )
		{
			if ( pChar->GetCurrentMap().getGaeaMapID().Mid() != pNextLand->GetGaeaMapID().Mid() )
			{
				sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_CONDITION;
				_SendToField( pChar->ClientSlot(), &sNetMsgFB );

				return false;
			}
		}

		// 이동 위치 정보의 유효성 검사를 한다;
		if ( ( _sMoveInfo.dwGateID == UINT_MAX ) && 
			( _sMoveInfo.vPosition == D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) ) )
		{
			sc::writeLogError( sc::string::format(
				"[ Move Map ] [ Invalid Gate ID and Position : %1% / %2%, %3%, %4% ]",
				_sMoveInfo.dwGateID,
				_sMoveInfo.vPosition.x,
				_sMoveInfo.vPosition.y,
				_sMoveInfo.vPosition.z) );

			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
			_SendToField( pChar->ClientSlot(), &sNetMsgFB );

			return false;
		}		

		// 필드 서버 변경시 클라이언트 정보 변경 사항을 반영한다;
		m_pServer->ReActionMoveFieldSvr( pChar->ClientSlot(), pChar->GaeaId() );

		// 필드서버에 이동할 것을 알린다;
		GLMSG::SNETPC_MOVEMAP_AF sNetOutMsg;
		sNetOutMsg.emMoveType = _sMoveInfo.emMoveType;
		sNetOutMsg.dwChaNum = pChar->ChaDbNum();
		sNetOutMsg.dwSvrGroupNum = m_pServer->GetMsgServer()->GetServerGroup();
		sNetOutMsg.targetMapID = _sMoveInfo.sMapID;
		sNetOutMsg.bSameFieldServer =
			( pChar->m_nChannel == _sMoveInfo.dwChannelNum ) &&
			( pChar->m_dwCurFieldSvr == _sMoveInfo.dwFieldNum );
		sNetOutMsg.dwGateID = _sMoveInfo.dwGateID;
		sNetOutMsg.vPosition = _sMoveInfo.vPosition;

		switch ( sNetOutMsg.emMoveType )
		{
		case EMMOVE_TYPE_TRIGGERSYSTEM:
			sNetOutMsg.emType = EMJOINTYPE_PVEINDUN;
			break;

		case EMMOVE_TYPE_GATE:
			sNetOutMsg.emType = EMJOINTYPE_GATE;
			break;
		}

		_SendToField( pChar->ClientSlot(), &sNetOutMsg );

		// 이동할 맵ID 를 Agent Server 에 저장한다;
		pChar->SetNextFieldSvr(
			_sMoveInfo.sMapID,
			_sMoveInfo.dwGateID,
			_sMoveInfo.vPosition,
			_sMoveInfo.dwFieldNum,
			_sMoveInfo.dwChannelNum );

		// 친구 이동을 가능 상태로 바꾼다;
		//pChar->m_bMapMoveFCard = TRUE;

		return true;
	}

	void CMoveManagerServer::_SendToClient ( DWORD dwClientID, NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOCLIENT( dwClientID, pMsg );
	}

	void CMoveManagerServer::_SendToField ( DWORD dwClientID, NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOFIELD( dwClientID, pMsg );
	}

	void CMoveManagerServer::_SendToField ( DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOFIELD( dwChannel, dwFieldSvrNum, pMsg );
	}

	void CMoveManagerServer::_SendToAgent ( NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOAGENT( pMsg );
	}

	void CMoveManagerServer::_SendToMyself ( NET_MSG_GENERIC* pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOMYSELF( pMsg );
	}

}