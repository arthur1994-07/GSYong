#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubField.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TriggerSystem/ServerDieMessage.h"
#include "../Move/MoveManagerField.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "./GLChar.h"


void GLChar::Resurrect(const EMRESURRECTION_TYPE _emResurrectionType)  // �÷��̾� ��Ȱ (��ȿ�� �˻�, ��ũ��Ʈ ����);
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    if (IsValidBody() || !IsActState(EM_ACT_DIE))
	{
		sc::writeLogError(
			sc::string::format("IsValidBody: %d, State : %d , RequestResurrect() : �������� ������°� �ƴ� ��Ȳ���� ��Ȱ�޽��� ó����û�� �߻��Ͽ����ϴ�.",
			IsValidBody(), IsActState(EM_ACT_DIE) ) );

		return;
	}

    // �ش� ��Ȱ �Ұ���;
    if ( m_sResurrectAbleState.isState( _emResurrectionType ) == false )
        return;

    // ��ũ��Ʈ�� ���� ��� ��ũ��Ʈ���� ������ �� �ֵ��� ��;
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // �δ��� �ƴ� ��� NULL;
    {
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }
        // ���� Event Resurrect ��ũ��Ʈ�� �ִٸ� ��ũ��Ʈ�� ó���� ����;
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

void GLChar::ResurrectProc(const EMRESURRECTION_TYPE _emResurrectionType)  // ���� �÷��̾� ��Ȱó��;
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ResurrectProc", m_CharDbNum ) );
	}

    switch ( _emResurrectionType )
    {
    case EM_RESURRECTION_NOMAL:  // �Ϲ� ��Ȱ ( ����ġ�� �Ұ� ������������ ��Ȱ );
        {
            // �Ϲ����� ��Ȱ�� ����ġ ����.;
            DoDeathPenaltyExp();

            // ���� ���� ��Ȱ;
            ResurrectStartMap();
        }
        break;

    case EM_RESURRECTION_SPECIAL:  // ����ġ ȸ�� ��Ȱ (���ӸӴϸ� ����Ͽ� �������� ����ġ ����);
        {   
			const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
			if ( pMapNode == NULL )
				return;
			if ( pMapNode->IsRestrt() == true )
				return;

            // ȸ���� ����ġ�� ���°�?;
            const __int64 _lnRestorableEXP = GetRestorableExp();
            if (_lnRestorableEXP <= 0)
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOEXP);
                SendToClient( &MsgFB );
                return;
            }

            // ����ġ ȸ���� �ʿ��� ���� �����Ѱ�?;
            const __int64 _lnRestoreExpCost = GetEXPRestoreCost(_lnRestorableEXP);
            if (_lnRestoreExpCost > GetInvenMoney())
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOMONEY);
                SendToClient( &MsgFB );
                return;
            }

            // ����ġ ��Ȱ�� ����� �� ���� ��Ȳ�ΰ�?;
            if ( m_sResurrectAbleState.isState( EM_RESURRECTION_SPECIAL ) == false )
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_NOTUSE);
                SendToClient( &MsgFB );
                return;
            }

            // ����� ����ġ�϶���ġ�� ȸ������ġ ��ŭ �氨�� �� ��Ȱ ��Ų��.;
            RestoreDeathPenalty_Exp(_lnRestorableEXP);

            DoDeathPenaltyExp();

            // ����ġ ȸ�� ���ɷ� 0���� �ʱ�ȭ;
            m_lnRestorableExp = 0;	
            
            // ����ġ ȸ�� ���� �˸�;
            GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_FB_OK);
            MsgFB.lnRestorableEXP = _lnRestorableEXP;
            MsgFB.lnRestoreExpCost = _lnRestoreExpCost;
            SendToClient( &MsgFB );

            CheckMoneyUpdate(GetInvenMoney(), _lnRestoreExpCost, FALSE, "Recovery Exp");

            SubtractInvenMoneyServer( _lnRestoreExpCost, SUBTRACT_INVEN_MONEY_EXP_RECOVERY_SPECIAL_REBIRTH );

            // ���ӸӴ� ���� �α׸� �����.;
			// ��� �� �α� ����
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                -_lnRestoreExpCost, MONEY_RECOVERY_CONSUME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_RESURRECTION );

			// ��� �� �α� ����
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);

            SendInvenMoneyUpdate();

            //	����ġ ��Ȱ �α�;
            {        
				// ��� �� �α� ����
                LogCharAction
                    (
                    m_CharDbNum,					//	�����.
                    gamelog::ACT_RECOVERY,			//	����.
                    gamelog::ID_CHAR, 0,					//	����.
                    _lnRestorableEXP,						// exp
                    0,							// bright
                    0,							// life
                    (int)_lnRestoreExpCost,				// money
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
                    );
            }

            // ���� �ڸ����� ��Ȱ;
            ResurrectCurrentPos();
        }
        break;

    case EM_RESURRECTION_SPECIAL_ITEM:  // �������� ����� Ư�� ��Ȱ ( ��ȥ�� ��Ȱ );
        {
			const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
			if ( pMapNode == NULL )
				return;
			if ( pMapNode->IsRestrt() == true )
				return;

	        // �ƾ����� �ִ���?;
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

	        // ������ �Ӽ��� ��Ȱ�������
	        if ( pITEM->sDrugOp.emDrug!=ITEM_DRUG_CALL_REVIVE )
	        {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOITEM);
		        SendToClient( &MsgFB );
		        return;
	        }

            // ��ȥ�� ��Ȱ�� ����� �� ���� ��Ȳ�ΰ�?;
            if ( m_sResurrectAbleState.isState( EM_RESURRECTION_SPECIAL_ITEM ) == false )
            {
                GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOTUSE);
                SendToClient( &MsgFB );
                return;
            }

	        // ��ȥ�� ���.
	        
	        // �Ҹ�� ��ȥ�� ó��.;
	        ConsumeSlotItem(SLOT_DECORATION);

	        // ��ȥ�� ��� ��Ȱ ������ �˸�
            GLMSG::NET_CHAR_REQ_RESURRECT_FB MsgFB(_emResurrectionType, EMREQ_RESURRECT_SPECIAL_ITEM_FB_OK);
	        SendToClient( &MsgFB );

            //// ������ �α� ���.
            // ��Ȱ�ϱ� ��ư�� ������ �ʾ� �α׸� ������� ��찡 �־ ��ȥ�� ���� �α׸� �����.
            {        
			// ��� �� �α� ����
             LogCharAction(
              m_CharDbNum,					//	�����.
              gamelog::ACT_RESTORE,			//	����.
              gamelog::ID_CHAR, 0,			//	����.
              m_lnDeathPenaltyEXP,          // exp
              0,							// bright
              0,							// life
              0,							// money
			  ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
              );
            }
            
            // ���� �ڸ����� ��Ȱ;
            ResurrectCurrentPos();	        
        }
        break;

    case EM_RESURRECTION_SKILL:  // ��Ȱ��ų�� ���� ��Ȱ;
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

            // ��������� ��ų ��Ȱ�� Ʈ���ſ��� �޽����� ������.;
            TriggerSystem::SServerDieMessage msg;
            msg.dwCharDbNum = CharDbNum();
            msg.bIsRevival = true;
            SendTriggerMessage(&msg);

			// ��ų ��Ȱ�� ����ġ ���ҽ�Ű�� �ʴ´�;
            // ���� �ڸ����� ��Ȱ;
            ResurrectCurrentPos(m_fSkillResurrect_Restore);
        }
        break;

    case EM_RESURRECTION_FORCED:  // �ý��ۿ� ���� ���� ��Ȱ ( CTF ���� );
        {
            // ���� ��Ȱ �ð� ������ ���� ��Ȱ �Ұ�;
            if ( m_fForcedResurrectTime > 0.0f )
			{
                return;
			}

            // ���� ��Ȱ�� ����ġ ����.;
            DoDeathPenaltyExp();

            // ������Ȱ���� ��Ȱ;
            if ( m_bForcedResurrect_Instance )
                ResurrectMoveMapInstance(m_sForcedResurrect_MapID, m_dwForcedResurrect_GateID, m_vForcedResurrect_position, m_fForcedResurrect_Restore);
            else
			{
				// ���� ��Ȱ ��� ����: land �� m_GlobPCList �� �ٷ� �����ϱ� ������, �����ϴ� ������� �����Ѵ�.
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

    // ��ȰŸ�� ������ �ʱ�ȭ �� ��ȰŸ�� �̼��� �÷��� ��;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState = EM_RESURRECTION_TYPE_NOT_SET;
}

void GLChar::SetDefaultResurrection()
{
	// RM# 2074 - ��ý� ����ϸ� ���� ó���ǰ� �ִ�. ��Ȱ ó�� �ʿ� ����.
	if( true == m_sCONFTING.IsCONFRONTING() )
		return;

    if (IsCTFPlayer())
    {
        SNATIVEID	idMap	= m_pGaeaServer->GetCTFPvPMapID();
        DWORD		dwGate	= m_pGaeaServer->GetCTFRevivalGate(m_wSchool);
        SetResurrectionForced(false, idMap, dwGate, D3DXVECTOR3(0.0f, 0.0f, 0.0f), GetCTFRebirthTime());
        return;
    }
    // ��ȥ�ְ� ������ ������ ��Ȱ;
    if ( FindRevive() != NULL )
    {
        SetResurrectionItem();
        return;
    }
    SetResurrectionNomal();
}

void GLChar::SetResurrectionNomal()  // ��Ȱ ���� ���� ( �Ϲ� );
{
    // �Ϲݺ�Ȱ, ����ġ ��Ȱ�� ����;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_NOMAL;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRestrt() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SPECIAL;
	}

    // Ŭ�󿡰� ��Ȱ������ �˸�;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionItem()  // ��Ȱ ���� ���� ( ��ȥ�� );
{
    // �Ϲݺ�Ȱ, ��ȯ�� ��Ȱ�� ����;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_NOMAL;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRestrt() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SPECIAL_ITEM;
	}

    // Ŭ�󿡰� ��Ȱ������ �˸�;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionSkill(const float _fResurrectionValue) // ��Ȱ ���� ���� ( ��ų ��Ȱ );
{
    m_sResurrectAbleState -= EM_RESURRECTION_TYPE_NOT_SET;

    // ��Ȱ ��ų�� �ް� �Ǹ� ��ų ��Ȱ�� �����ϰ� ��;
	const SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap()));
	if ( pMapNode != NULL )
	{
		if ( pMapNode->IsRebirthBlock() == false )
			m_sResurrectAbleState += EM_RESURRECTION_SKILL;
	}

    m_fSkillResurrect_Restore = _fResurrectionValue;

    // Ŭ�󿡰� ��Ȱ������ �˸�;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    NetMsg.fResurrectRestore = _fResurrectionValue;
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionForced(const bool _bInstance, const SNATIVEID& _sStartMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fForcedResurrectTime, const float _fResurrectionValue /*= 100.0f*/ )  // ��Ȱ ���� ���� ( ���� ��Ȱ );
{
    m_bForcedResurrect_Instance = _bInstance;
    m_sForcedResurrect_MapID = _sStartMapID;
    m_dwForcedResurrect_GateID = _dwGateID;
	m_vForcedResurrect_position = _position;
    m_fForcedResurrectTime = _fForcedResurrectTime;
    m_fForcedResurrect_Restore = _fResurrectionValue;

    // ���� ��Ȱ�� ����;
    m_sResurrectAbleState.reset();
    m_sResurrectAbleState += EM_RESURRECTION_FORCED;

    // Ŭ�󿡰� ��Ȱ������ �˸�;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    NetMsg.fResurrectRestore = _fResurrectionValue;
    NetMsg.fResurrectTime = _fForcedResurrectTime;
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionDisable(const bool _bDisableSkill)
{
    // ��Ȱ �Ұ��� ����;
    m_sResurrectAbleState.reset();
    SetResurrectionDisableSkill(_bDisableSkill);

    // Ŭ�󿡰� ��Ȱ������ �˸�;
    GLMSG::NET_CHAR_RESURRECT_INFO_FC NetMsg( m_sResurrectAbleState.nState);
    SendToClient(ClientSlot(), &NetMsg);
}

void GLChar::SetResurrectionDisableSkill(const bool _bDisableSkill)
{
    // ��ų ��Ȱ �Ұ��� ����;
    if ( _bDisableSkill )
        m_sResurrectAbleState += EM_RESURRECTION_DISABLE_SKILL;
    else
        m_sResurrectAbleState -= EM_RESURRECTION_DISABLE_SKILL;
}

void GLChar::Receive_ResurrectSkill(const float _fResurrectionValue)
{
    // ��ų ��Ȱ �Ұ��ɽÿ��� ��ų ��Ȱ ������ �� �� ����;
    if ( m_sResurrectAbleState.isState(EM_RESURRECTION_DISABLE_SKILL) )
        return;

    SetResurrectionSkill(_fResurrectionValue);
}

void GLChar::ResurrectStartMap(const float _fValue /*= 100.0f*/)
{
    // ������������ ��Ȱ (��ȯ���);
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

    // pLand�� �δ� ����� rawData �̹Ƿ� �ǵ���� �ȵȴ�;
    // pLand�� instanceMapID = NULL; �̴�;
    // GetLandGateMan(), FindLandGate()�� ��� const�� �ٿ��� �����ϳ�,
    // �ð� ����� �׷��� ���Ѵ�;
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

    // �ű� �δ��� ��� �̵��Ϸ��� ��� Land�� ���� ���� �Ǿ� ���� ���� ���� �ֱ� ������,
    // �ݵ�� ������� �������� �̵� ��ƾ�� ����� �ϸ�,
    // ��� ��Ȱ ���μ����� �̿� ���߾� ���� �� �ʿ䰡 �ִ�;
    // �ϳ��� ����� �������� ����� �ϴ� ���� ���� ������ �־� ���� ���� �ʴ�;
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
			// ������ȯ ��ġ DB�� ����;
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

    /*// �� ������ ���� ���ٴ� ���� ��Ȱ���̵����� ���� �ʵ弭���� �ٲ��� �Ѵٴ� ��;
    GLLandMan* const pLand = m_pGaeaServer->GetLand(_moveMapID);
    if ( pLand == NULL || pLand->GetGaeaMapID() == SNATIVEID(false) )
        //if(pMapNode->GetFieldServerNum() != m_pGaeaServer->GetFieldSvrID())
        //if ( bMyField )
    {
        // ������Ʈ��  ���̵� ��Ȱ�� ��û;
        GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FA NetMsg(CharDbNum(), _moveMapID, _dwGateID, _fRestore);
        SendToAgent(&NetMsg);
    }
    else
    {
        // �� ������ �����ϴ� ���̹Ƿ� ���� ��Ȱ ó��;
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

	// ����� ������ 100% �� ȸ����Ų��;
	if ( false == IsValidBody() )
	{
		if ( FALSE == m_pGaeaServer->IsInstance() )
		{
			if ( SaveLastCall() == S_OK )
			{
				// ������ȯ ��ġ DB�� ����;
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

    // ��Ȱ map�� ��Ȱ gate ����, ��ġ�� �ǹ� ���� ��.
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

void GLChar::_sendResurrectMsg(GLLandMan* pLandMan, const float _fValue /*= 100.0f*/)  //�÷��̾� ��Ȱ;
{
	// PET
	// ���̵��� Pet ����
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
	// ĳ���Ͱ� ���� ���¶��;
	if ( IsActState( EM_ACT_DIE ) )
		return false;

	// ĳ���� HP�� 0���϶��;
	if ( GetNowHP() <= 0 )
		return false;

	return true;
}

void GLChar::SetAlive(const float _fValue /*= 100.0f*/)
{
	if ( IsActState(EM_ACT_DIE))
	{
		ReSetActState(EM_ACT_DIE);
		// ���� �ð� ����;
		SetMoveDelay(GLCONST_CHAR::fPK_SAFE_TIME);
	}
    // ��Ȱ�� �׼� ���� ����;
    TurnAction(GLAT_IDLE);

    // ��Ȱ�� HP,MP,SPȸ�� �� �ʱ�ȭ;
    GLCHARLOGIC::INIT_RECOVER(_fValue);
    m_fGenAge = 0.0f;
    ResetCP();

	m_sResurrectAbleState.reset();
	m_sResurrectAbleState.setState(EM_RESURRECTION_TYPE_NOT_SET);

	// �̹�����ΰ�;
	GLMSG::SNET_ACTION_BRD NetMsgBrd;
	NetMsgBrd.emCrow	= CROW_PC;
	NetMsgBrd.dwID		= m_dwGaeaID;
	NetMsgBrd.emAction	= GLAT_IDLE;
	NetMsgBrd.dwFLAG	= GetActState();
	// �ڽ��� Ŭ���̾�Ʈ��.
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

    // ����ġ ��ȭ Ŭ���̾�Ʈ�� �˷���.
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

    //  ������ ���ڶ� ���� �Ұ��� ���� ������ �ʱ�ȭ�Ѵ�.;
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