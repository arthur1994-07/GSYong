#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubField.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TriggerSystem/ServerDieMessage.h"
#include "../Move/MoveManagerField.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "./GLChar.h"


void GLChar::Resurrect(const EMRESURRECTION_TYPE _emResurrectionType)  // 플레이어 부활 (유효성 검사, 스크립트 제어);
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    if (IsValidBody() || !IsActState(EM_ACT_DIE))
	{
		sc::writeLogError(
			sc::string::format("IsValidBody: %d, State : %d , RequestResurrect() : 정상적인 사망상태가 아닌 상황에서 부활메시지 처리요청이 발생하였습니다.",
			IsValidBody(), IsActState(EM_ACT_DIE) ) );

		return;
	}

    // 해당 부활 불가능;
    if ( m_sResurrectAbleState.isState( _emResurrectionType ) == false )
        return;

    // 스크립트가 있을 경우 스크립트에서 제어할 수 있도록 함;
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }
        // 만약 Event Resurrect 스크립트가 있다면 스크립트의 처리를 따름;
        if ( _pInstance->EventResurrect(_emResurrectionType, CROW_PC, CharDbNum(), m_sAssault.emCrow, dwID) == false )
        {
            return;
        }
    }

    ResurrectProc(_emResurrectionType);
}

void GLChar::ResurrectImmediatelyForced()
{
	m_fForcedResurrectTime = 0.f;

	Resurrect( EM_RESURRECTION_FORCED );
}

void GLChar::ResurrectProc(const EMRESURRECTION_TYPE _emResurrectionType)  // 실제 플레이어 부활처리;
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ResurrectProc", m_CharDbNum ) );
	}

    switch ( _emResurrectionType )
    {
    case EM_RESURRECTION_NOMAL:  // 일반 부활 ( 경험치를 잃고 시작지점에서 부활 );
        {
            // 일반적인 부활시 경험치 감소.;
            DoDeathPenaltyExp();

            // 시작 지점 부활;
            ResurrectStartMap();
        }
        break;

    case EM_RESURRECTION_SPECIAL:  // 경험치 회복 부활 (게임머니를 사용하여 일정량의 경험치 복구);
        {   
			const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
			if ( pMapNode == NULL )
				return;
			if ( pMapNode->IsRestrt() == true )
				return;

            // 회복할 경험치가 없는가?;
            const __int64 _lnRestorableEXP = GetRestorableExp();
            if (_lnRestorableEXP <= 0)
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOEXP);
                SendToClient( &MsgFB );
                return;
            }

            // 경험치 회복에 필요한 돈이 부족한가?;
            const __int64 _lnRestoreExpCost = GetEXPRestoreCost(_lnRestorableEXP);
            if (_lnRestoreExpCost > GetInvenMoney())
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOMONEY);
                SendToClient( &MsgFB );
                return;
            }

            // 경험치 부활을 사용할 수 없는 상황인가?;
            if ( m_sResurrectAbleState.isState( EM_RESURRECTION_SPECIAL ) == false )
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOTUSE);
                SendToClient( &MsgFB );
                return;
            }

            // 사망시 경험치하락수치를 회복경험치 만큼 경감한 후 부활 시킨다.;
            RestoreDeathPenalty_Exp(_lnRestorableEXP);

            DoDeathPenaltyExp();

            // 경험치 회복 가능량 0으로 초기화;
            m_lnRestorableExp = 0;	
            
            // 경험치 회복 성공 알림;
            GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_OK);
            MsgFB.lnRestorableEXP = _lnRestorableEXP;
            MsgFB.lnRestoreExpCost = _lnRestoreExpCost;
            SendToClient( &MsgFB );

            CheckMoneyUpdate(GetInvenMoney(), _lnRestoreExpCost, FALSE, "Recovery Exp");

            SubtractInvenMoneyServer( _lnRestoreExpCost, SUBTRACT_INVEN_MONEY_EXP_RECOVERY_SPECIAL_REBIRTH );

            // 게임머니 감소 로그를 남긴다.;
			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                -_lnRestoreExpCost, MONEY_RECOVERY_CONSUME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_RESURRECTION );

			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);

            SendInvenMoneyUpdate();

            //	경험치 부활 로그;
            {        
				// 통계 및 로그 수정
                LogCharAction
                    (
                    m_CharDbNum,					//	당사자.
                    gamelog::ACT_RECOVERY,			//	행위.
                    gamelog::ID_CHAR, 0,					//	상대방.
                    _lnRestorableEXP,						// exp
                    0,							// bright
                    0,							// life
                    (int)_lnRestoreExpCost,				// money
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
                    );
            }

            // 현재 자리에서 부활;
            ResurrectCurrentPos();
        }
        break;

    case EM_RESURRECTION_SPECIAL_ITEM:  // 아이템을 사용한 특수 부활 ( 귀혼주 부활 );
        {
			const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
			if ( pMapNode == NULL )
				return;
			if ( pMapNode->IsRestrt() == true )
				return;

	        // 아아템이 있는지?;
	        SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_DECORATION);
	        if ( !pITEM )
	        {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOITEM);
		        SendToClient( &MsgFB );
		        return;
	        }

	        if ( CheckCoolTime ( pITEM->sBasicOp.sNativeID ) )
	        {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_COOLTIME);
		        SendToClient( &MsgFB );
		        return;
	        }

	        // 아이템 속성이 부활기능인지
	        if ( pITEM->sDrugOp.emDrug!=ITEM_DRUG_CALL_REVIVE )
	        {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOITEM);
		        SendToClient( &MsgFB );
		        return;
	        }

            // 귀혼주 부활을 사용할 수 없는 상황인가?;
            if ( m_sResurrectAbleState.isState( EM_RESURRECTION_SPECIAL_ITEM ) == false )
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOTUSE);
                SendToClient( &MsgFB );
                return;
            }

	        // 귀혼주 사용.
	        
	        // 소모된 귀혼주 처리.;
	        ConsumeSlotItem(SLOT_DECORATION);

	        // 귀혼주 사용 부활 성공을 알림
            GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_OK);
	        SendToClient( &MsgFB );

            //// 죽을때 로그 기록.
            // 부활하기 버튼이 나오지 않아 로그를 못남기는 경우가 있어서 귀혼주 사용시 로그를 남긴다.
            {        
			// 통계 및 로그 수정
             LogCharAction(
              m_CharDbNum,					//	당사자.
              gamelog::ACT_RESTORE,			//	행위.
              gamelog::ID_CHAR, 0,			//	상대방.
              m_lnDeathPenaltyEXP,          // exp
              0,							// bright
              0,							// life
              0,							// money
			  ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
              );
            }
            
            // 현재 자리에서 부활;
            ResurrectCurrentPos();	        
        }
        break;

    case EM_RESURRECTION_SKILL:  // 부활스킬에 의한 부활;
        {
			const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
			if ( pMapNode == NULL )
				return;
			if ( pMapNode->IsRebirthBlock() == true )
				return;

            if ( m_fSkillResurrect_Restore == 0.0f ) 
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMSKILL_REVIVE_FB_NOTVALUE);
                SendToClient( &MsgFB );
                return;
            }

            // 무한의재단 스킬 부활시 트리거에게 메시지를 날린다.;
            TriggerSystem::SServerDieMessage msg;
            msg.dwCharDbNum = CharDbNum();
            msg.bIsRevival = true;
            SendTriggerMessage(&msg);

			// 스킬 부활시 경험치 감소시키지 않는다;
            // 현재 자리에서 부활;
            ResurrectCurrentPos(m_fSkillResurrect_Restore);
        }
        break;

    case EM_RESURRECTION_FORCED:  // 시스템에 의한 강제 부활 ( CTF 전용 );
        {
            // 강제 부활 시간 이전에 강제 부활 불가;
            if ( m_fForcedResurrectTime > 0.0f )
			{
                return;
			}

            // 강제 부활시 경험치 감소.;
            DoDeathPenaltyExp();

            // 강제부활지점 부활;
            if ( m_bForcedResurrect_Instance )
                ResurrectMoveMapInstance(m_sForcedResurrect_MapID, m_dwForcedResurrect_GateID, m_vForcedResurrect_position, m_fForcedResurrect_Restore);
            else
			{
				// 강제 부활 기능 수정: land 에 m_GlobPCList 를 바로 접근하기 때문에, 예약하는 방식으로 수정한다.
				if ( m_pGaeaServer )
				{
					m_pGaeaServer->ReserveCTFRebirth( m_CharDbNum, m_sForcedResurrect_MapID, m_dwForcedResurrect_GateID, m_fForcedResurrect_Restore );
				}
                //ResurrectMoveMap(m_sForcedResurrect_MapID, m_dwForcedResurrect_GateID, m_fForcedResurrect_Restore);
			}
          
        }
        break;

    default:
        return;
    }

    // 부활타입 설정을 초기화 후 부활타입 미설정 플래그 켬;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState = EM_RESURRECTION_TYPE_NOT_SET;
}

void GLChar::SetDefaultResurrection()
{
	// RM# 2074 - 대련시 사망하면 따로 처리되고 있다. 부활 처리 필요 없음.
	if( true == m_sCONFTING.IsCONFRONTING() )
		return;

    if (IsCTFPlayer())
    {
        SNATIVEID	idMap	= m_pGaeaServer->GetCTFPvPMapID();
        DWORD		dwGate	= m_pGaeaServer->GetCTFRevivalGate(m_wSchool);
        SetResurrectionForced(false, idMap, dwGate, D3DXVECTOR3(0.0f, 0.0f, 0.0f), GetCTFRebirthTime());
        return;
    }
    // 귀혼주가 있으면 아이템 부활;
    if ( FindRevive() != NULL )
    {
        SetResurrectionItem();
        return;
    }
    SetResurrectionNomal();
}

void GLChar::SetResurrectionNomal()  // 부활 상태 설정 ( 일반 );
{
    // 일반부활, 경험치 부활만 가능;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_NOMAL;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRestrt() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SPECIAL;
	}

    // 클라에게 부활정보를 알림;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionItem()  // 부활 상태 설정 ( 귀혼주 );
{
    // 일반부활, 귀환주 부활만 가능;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_NOMAL;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRestrt() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SPECIAL_ITEM;
	}

    // 클라에게 부활정보를 알림;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionSkill(const float _fResurrectionValue) // 부활 상태 설정 ( 스킬 부활 );
{
    m_sResurrectAbleState -= EM_RESURRECTION_TYPE_NOT_SET;

    // 부활 스킬을 받게 되면 스킬 부활이 가능하게 됨;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRebirthBlock() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SKILL;
	}

    m_fSkillResurrect_Restore = _fResurrectionValue;

    // 클라에게 부활정보를 알림;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    NetMsg.fResurrectRestore = _fResurrectionValue;
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionForced(const bool _bInstance, const SNATIVEID& _sStartMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fForcedResurrectTime, const float _fResurrectionValue /*= 100.0f*/ )  // 부활 상태 설정 ( 강제 부활 );
{
    m_bForcedResurrect_Instance = _bInstance;
    m_sForcedResurrect_MapID = _sStartMapID;
    m_dwForcedResurrect_GateID = _dwGateID;
	m_vForcedResurrect_position = _position;
    m_fForcedResurrectTime = _fForcedResurrectTime;
    m_fForcedResurrect_Restore = _fResurrectionValue;

    // 강제 부활만 가능;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_FORCED;

    // 클라에게 부활정보를 알림;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    NetMsg.fResurrectRestore = _fResurrectionValue;
    NetMsg.fResurrectTime = _fForcedResurrectTime;
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionDisable(const bool _bDisableSkill)
{
    // 부활 불가능 설정;
    m_sResurrectAbleState.reset();
    SetResurrectionDisableSkill(_bDisableSkill);

    // 클라에게 부활정보를 알림;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionDisableSkill(const bool _bDisableSkill)
{
    // 스킬 부활 불가능 설정;
    if ( _bDisableSkill )
        m_sResurrectAbleState += EM_RESURRECTION_DISABLE_SKILL;
    else
        m_sResurrectAbleState -= EM_RESURRECTION_DISABLE_SKILL;
}

void GLChar::Receive_ResurrectSkill(const float _fResurrectionValue)
{
    // 스킬 부활 불가능시에는 스킬 부활 설정을 할 수 없다;
    if ( m_sResurrectAbleState.isState(EM_RESURRECTION_DISABLE_SKILL) )
        return;

    SetResurrectionSkill(_fResurrectionValue);
}

void GLChar::ResurrectStartMap(const float _fValue /*= 100.0f*/)
{
    // 시작지점에서 부활 (귀환장소);
    if ( CheckResurrectStartMap() )
        ResurrectMoveMap(m_sStartMapID, m_dwStartGate, _fValue);
	else
		ResurrectMoveMap(GLCONST_CHAR::nidSTARTMAP[m_wSchool], GLCONST_CHAR::dwSTARTGATE[m_wSchool], _fValue);
}

void GLChar::ResurrectMoveMapInstance(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fRestore /*= 100.0f*/)
{
	/*MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = MapID( _moveMapID, GetCurrentMap().getGaeaMapID() );
	sMoveInfo.dwGateID = _dwGateID;
	sMoveInfo.vPosition = _position;

	MoveSystem::CMoveManagerField::Instance()->Resurrection( sMoveInfo );*/

    // pLand는 인던 복사용 rawData 이므로 건드려선 안된다;
    // pLand의 instanceMapID = NULL; 이다;
    // GetLandGateMan(), FindLandGate()에 모두 const를 붙여야 마땅하나,
    // 시간 관계상 그러지 못한다;
    GLLandMan* const pLandMan = m_pGaeaServer->GetLand(_moveMapID);
	if ( pLandMan == NULL )
		return;

	D3DXVECTOR3 vStartPos(_position);
    if ( _dwGateID != GAEAID_NULL )
	{
		DxLandGateMan* const pGateMan = pLandMan->GetLandGateMan();
		DxLandGate* const pGate = pGateMan->FindLandGate(_dwGateID);
		if ( pGateMan == NULL || pGate == NULL )
		{
			ResurrectStartMap();
			return;
		}
		else
			vStartPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
			
	}    

    InstanceSystem::InformationPosition _reserveSetPosition(
		Faction::GLActorID(CROW_PC, CharDbNum()),
        _moveMapID, Faction::GLActorID(CROW_NULL, GAEAID_NULL), 
		vStartPos.x, vStartPos.z, InstanceSystem::EMMOVE_INSTANCE2INSTANCE);

    // 신규 인던의 경우 이동하려는 대상 Land가 아직 생성 되어 있지 않을 수도 있기 때문에,
    // 반드시 살려놓고 정상적인 이동 루틴을 따라야 하며,
    // 모든 부활 프로세스를 이에 맞추어 변경 할 필요가 있다;
    // 하나의 모듈이 복합적인 기능을 하는 것은 유지 보수에 있어 절대 좋지 않다;
	//SetAlive(_fRestore);

	_reserveSetPosition.bResurrect = true;

    InstanceSystem::ManagerField* const _pManager(m_pGaeaServer->GetInstanceManagerField());
    InstanceSystem::InstanceMapID _instanceMapID(GetCurrentMap().getGaeaMapID().Mid());
    const bool _bSuccess(_pManager->requestMoveToPosition(_instanceMapID, _reserveSetPosition));
}

void GLChar::ResurrectMoveMap(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const float _fRestore /*= 100.0f*/)
{
	if ( FALSE == m_pGaeaServer->IsInstance() )
	{
		if ( SaveLastCall() == S_OK )
		{
			// 직전귀환 위치 DB에 저장;
			AddGameAdoJob(
				db::DbActionPtr(
				new db::CharacterLastCallPosSet(
				m_CharDbNum,
				m_sLastCallMapID.dwID,
				m_vLastCallPos ) ) );
		}
	}

	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = _moveMapID;
	sMoveInfo.dwGateID = _dwGateID;

	MoveSystem::CMoveManagerField::Instance()->Resurrection( sMoveInfo );

    /*// 이 서버에 맵이 없다는 뜻은 부활맵이동으로 인해 필드서버가 바뀌어야 한다는 뜻;
    GLLandMan* const pLand = m_pGaeaServer->GetLand(_moveMapID);
    if ( pLand == NULL || pLand->GetGaeaMapID() == SNATIVEID(false) )
        //if(pMapNode->GetFieldServerNum() != m_pGaeaServer->GetFieldSvrID())
        //if ( bMyField )
    {
        // 에이전트에  맵이동 부활을 요청;
        GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FA NetMsg(CharDbNum(), _moveMapID, _dwGateID, _fRestore);
        SendToAgent(&NetMsg);
    }
    else
    {
        // 이 서버에 존재하는 맵이므로 직접 부활 처리;
        BOOL bOk = m_pGaeaServer->RequestReBirth(m_dwGaeaID, pLand, _dwGateID, D3DXVECTOR3(0.0f,0.0f,0.0f));
        if (!bOk)
        {
            sc::writeLogError(
                sc::string::format(
                "ResurrectMoveMap : ChaDbNum %1% Map %2%/%3% gate %4%/%5%/%6%",
                CharDbNum(),
                pLand->GetBaseMapID().Mid(),
                pLand->GetBaseMapID().Sid(),
                _dwGateID));
            return;
        }
        _sendResurrectMsg(pLand, _fRestore);
    }*/

    if ( IsCTFPlayer() )
        m_pGaeaServer->CTFRebirthPlayerBuff(this);

	/*GLLandMan* const pLand = m_pGaeaServer->GetLand(_moveMapID);
    const SNATIVEID previousMap(GetCurrentMap().getGaeaMapID());
    const SNATIVEID nextMap(pLand == NULL ? _moveMapID : pLand->GetGaeaMapID());
    GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(CharDbNum(), previousMap, GetPosition(), nextMap);
    m_pGaeaServer->SENDTOMYSELF(&_moveMapMessage);*/
}

void GLChar::ResurrectCurrentPos(const float _fValue /*= 100.0f*/)
{
	if ( IsCTFPlayer() )
	{
		MoveSystem::SMOVE_INFO sMoveInfo;
		sMoveInfo.dwChaDBNum = CharDbNum();
		sMoveInfo.sMapID = GetCurrentMap();
		sMoveInfo.vPosition = m_vPosition;

		MoveSystem::CMoveManagerField::Instance()->Resurrection( sMoveInfo );

		m_pGaeaServer->CTFRebirthPlayerBuff(this);

		return;
	}

	// 현재는 무조건 100% 로 회복시킨다;
	if ( false == IsValidBody() )
	{
		if ( FALSE == m_pGaeaServer->IsInstance() )
		{
			if ( SaveLastCall() == S_OK )
			{
				// 직전귀환 위치 DB에 저장;
				AddGameAdoJob(
					db::DbActionPtr(
					new db::CharacterLastCallPosSet(
					m_CharDbNum,
					m_sLastCallMapID.dwID,
					m_vLastCallPos ) ) );
			}
		}

		SetAlive( _fValue );

		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_IDLE;
		NetMsgBrd.dwFLAG	= GetActState();
		SendMsgViewAround(&NetMsgBrd);
	}

	/*MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = GetCurrentMap();
	sMoveInfo.vPosition = m_vPosition;

	MoveSystem::CMoveManagerField::Instance()->Resurrection( sMoveInfo );*/

	/*GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // 부활 map과 부활 gate 지정, 위치는 의미 없는 값.
    BOOL bOk = m_pGaeaServer->RequestReBirth(m_dwGaeaID, pLand, UINT_MAX, m_vPosition);
    if (!bOk)
    {
        sc::writeLogError(
            sc::string::format(
            "ResurrectCurrentPos : ChaDbNum %1% Map %2%/%3% Pos %4%/%5%/%6%",
            m_CharDbNum,
            GetCurrentMap().getBaseMapID().Mid(),
            GetCurrentMap().getBaseMapID().Sid(),
            m_vPosition.x,
            m_vPosition.y,
            m_vPosition.z));
        return;
    }   

    _sendResurrectMsg(pLand, _fValue);*/
}

void GLChar::_sendResurrectMsg(GLLandMan* pLandMan, const float _fValue /*= 100.0f*/)  //플레이어 부활;
{
	// PET
	// 맵이동시 Pet 삭제
	//m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );

	// VEHICLE
	//m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, true );

	// SUMMON
	//SummonAllRemove();

    SetAlive(_fValue);   

    GLMSG::NET_CHAR_RESURRECT_FAFC NetMsg(
		m_mapID,
        GetPosition(),
        m_sHP.nNow,
        m_sMP.nNow,
        m_sSP.nNow,
        pLandMan->IsCullByObjectMap(),
		m_CharDbNum
        );

	SendToAgent(&NetMsg);
}

bool GLChar::IsAlive ()
{
	// 캐릭터가 죽은 상태라면;
	if ( IsActState( EM_ACT_DIE ) )
		return false;

	// 캐릭터 HP가 0이하라면;
	if ( GetNowHP() <= 0 )
		return false;

	return true;
}

void GLChar::SetAlive(const float _fValue /*= 100.0f*/)
{
	if ( IsActState(EM_ACT_DIE))
	{
		ReSetActState(EM_ACT_DIE);
		// 무적 시간 세팅;
		SetMoveDelay(GLCONST_CHAR::fPK_SAFE_TIME);
	}
    // 부활시 액션 상태 복구;
    TurnAction(GLAT_IDLE);

    // 부활시 HP,MP,SP회복 및 초기화;
    GLCHARLOGIC::INIT_RECOVER(_fValue);
    m_fGenAge = 0.0f;
    ResetCP();

	m_sResurrectAbleState.reset();
	m_sResurrectAbleState.setState(EM_RESURRECTION_TYPE_NOT_SET);

	// 이방법뿐인가;
	GLMSG::SNET_ACTION_BRD NetMsgBrd;
	NetMsgBrd.emCrow	= CROW_PC;
	NetMsgBrd.dwID		= m_dwGaeaID;
	NetMsgBrd.emAction	= GLAT_IDLE;
	NetMsgBrd.dwFLAG	= GetActState();
	// 자신의 클라이언트에.
	SendToClient(&NetMsgBrd);
}

void GLChar::DoDeathPenaltyExp ()
{
	if ( m_lnDeathPenaltyEXP==0 )
		return;

	m_lnRestorableExp = GetRestorableExp();

	if ( (m_sExperience.lnNow-m_lnDeathPenaltyEXP) < 0 )
		m_sExperience.lnNow = 0;
	else
		m_sExperience.lnNow -= m_lnDeathPenaltyEXP;

	m_lnLastSendExp = m_sExperience.lnNow;
	m_lnDeathPenaltyEXP = 0;

    // 경험치 변화 클라이언트에 알려줌.
    //
    GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
    NetMsgExp.lnNowExp = m_sExperience.lnNow;
    SendToClient(&NetMsgExp);
}

LONGLONG GLChar::GetRestorableExp ()
{
    LONGLONG nDecExp = 0;
    LONGLONG nReExp = 0;

    nDecExp = m_lnDeathPenaltyEXP;

    if ( nDecExp > m_sExperience.lnNow ) nDecExp = m_sExperience.lnNow;
    if ( nDecExp <= 0 )
        return 0;

    float fRExpRecovery = GLOGICEX::GLDIE_RECOVERYEXP( m_wLevel );
    nReExp = (LONGLONG)(nDecExp * fRExpRecovery);

    if ( nReExp < 0  ) nReExp = 0;	
    if ( nDecExp < nReExp ) nReExp = nDecExp;

    return nReExp;

}

LONGLONG GLChar::GetEXPRestoreCost(LONGLONG nReExp)
{
    LONGLONG nDecMoney = 0;
    if (nReExp > 0)
        nDecMoney = (LONGLONG)(nReExp * GLOGICEX::GLDIE_EXPMONEY(m_wLevel));
    return nDecMoney;
}

BOOL GLChar::CheckResurrectStartMap()
{
    GLLandMan *pLandMan=NULL;
    DWORD dwGenGate = 0;
    pLandMan = m_pGaeaServer->GetLand ( m_sStartMapID );

    if ( pLandMan && pLandMan->m_bClubDeathMatchMap )
    {
        if (GLCONST_CHAR::bPKLESS)
            return FALSE;	

        GLClubDeathMatch* pCDM = m_pGaeaServer->ClubDeathMatchFind(pLandMan->m_dwClubMapID);
        if (pCDM && !pCDM->IsEnterMap(m_ClubDbNum))
            return FALSE;

        std::tr1::shared_ptr<GLClubField> pClub = GetMyClub();
        if (!pClub)
            return FALSE;
        if (pClub->GetAllBattleNum() > 0)
            return FALSE;
        if (!pClub->IsCDMEnter(m_CharDbNum))
            return FALSE;
    }

    //  권한이 모자라서 진입 불가시 시작 지점을 초기화한다.;
    if ( m_dwUserLvl < USER_USER_GM )	
    {
        const SLEVEL_REQUIRE* pRequire = pLandMan->GetLevelRequire ();
        if ( pRequire && ( pRequire->ISCOMPLETE ( this ) != EMREQUIRE_COMPLETE ) )
        {
            m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
            m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
            m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);

            GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(EM_RESURRECTION_NOMAL, EMREQ_RESURRECT_NOMAL_FB_ENTRY_FAIL);
            SendToClient( &MsgFB ); 
            return FALSE;
        }
    }

    if ( pLandMan && pLandMan->GetLandGateMan()->FindLandGate(m_dwStartGate) ) 		
    {
        return TRUE;
    }

    return FALSE;
}

BOOL GLChar::IsSkillEffectFrame()
{
	PGLSKILL pSkill = m_WorkingSkill.GetSkillData();

	if( pSkill == NULL ) return FALSE;

	if( (pSkill->m_sAPPLY.nEffectFrame == 0) ||
		(pSkill->m_sAPPLY.nEffectFrame > 0 && pSkill->m_sAPPLY.nEffectFrame - 1 == m_WorkingSkill.GetCurStep() ) )
		return TRUE;

	return FALSE;
}

const bool GLChar::IsNonTargetable() const
{
	return RF_FEIGN_DEATH( m_EffSkillVarSet ).IsSelectDisable();
}