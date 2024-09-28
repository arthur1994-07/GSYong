#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/GLColorTable.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/Macro/MacroDefine.h"

#include "../TriggerSystem/ServerEntryMessage.h"
#include "../Club/GLClubServer.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Database/DBAction/DbActionGameVehicle.h"
#include "../Database/DBAction/DbActionGamePet.h"
#include "../Database/DBAction/DbActionMacro.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Party/GLPartyField.h"
#include "../PK/GLSchoolFreePK.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../Util/GLItemLimit.h"
#include "../Vehicle/GLVehicleField.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define MAX_MOVEDIST				40000.0f

void GLChar::MsgSendPeriod()
{
	m_fPERIOD_TIMER = 0.0f;

	// Period Sync
	//
	GLMSG::SNET_PERIOD NetMsgPeriod;
	NetMsgPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod();

	SendToClient(&NetMsgPeriod);
}

void GLChar::MsgSendUpdateState ()
{
	//	[�ڽſ���] ȸ������ ����.
	GLMSG::SNETPC_UPDATE_STATE NetMsg;
	NetMsg.sHP = GLCHARLOGIC::m_sHP;
	NetMsg.sMP = GLCHARLOGIC::m_sMP;
	NetMsg.sSP = GLCHARLOGIC::m_sSP;

	//NetMsg.ENCODE ( m_dwGaeaID );
	SendToClient( &NetMsg );

	//	����ڿ��� ü�� ��ȭ �˸�.
	GLMSG::SNETPC_UPDATE_STATE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.sHP = GLCHARLOGIC::m_sHP;

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				GLChar* pCHAR = GetChar(m_sCONFTING.dwTAR_ID);
				if (pCHAR)
					pCHAR->SendToClient(&NetMsgBrd);
			}
			break;
		case EMCONFT_PARTY:
			{
				GLPartyField* const pConftParty = GetParty(m_sCONFTING.dwTAR_ID);
				if ( pConftParty )
				{
					PartyMemberGaeaIDs arPartyMemberGaeaIDs;
					const DWORD nMember = pConftParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);
					for ( DWORD _i = 0; _i < nMember; ++_i )
					{
						const DWORD dwGaeaID = arPartyMemberGaeaIDs[_i];
						if ( !pConftParty->isConfront(dwGaeaID) )
							continue;

						GLChar* pTAR = GetChar(dwGaeaID);
						if ( pTAR == NULL )
                            continue;

						pTAR->SendToClient(&NetMsgBrd);
					}
				}
			}
			break;
		case EMCONFT_GUILD:
			{
				std::tr1::shared_ptr<GLClubField> pMY_CLUB = GetMyClub();
				if ( pMY_CLUB )
				{
                    CLUBMEMBERS& Members = pMY_CLUB->GetMembers();
					CLUBMEMBERS_ITER pos = Members.begin();
					CLUBMEMBERS_ITER end = Members.end();
					for ( ; pos!=end; ++pos)
					{
						GLChar* pTAR = GetCharByDbNum(pos->first);
						if (pTAR && pTAR->m_sCONFTING.IsCONFRONTING())
						    pTAR->SendToClient(&NetMsgBrd);
					}
				}

				std::tr1::shared_ptr<GLClubField> pTAR_CLUB = GetClub(m_sCONFTING.dwTAR_ID);
				if ( pTAR_CLUB )
				{
                    CLUBMEMBERS& Members = pTAR_CLUB->GetMembers();
					CLUBMEMBERS_ITER pos = Members.begin();
					CLUBMEMBERS_ITER end = Members.end();
					for ( ; pos!=end; ++pos)
					{
						GLChar* pTAR = GetCharByDbNum(pos->first);
						if (pTAR && pTAR->m_sCONFTING.IsCONFRONTING())
						    pTAR->SendToClient(&NetMsgBrd);
					}
				}
			}
			break;
		};
	}

	// ���� �������� Ÿ�̸� ���� �� ����.
	{
		GLChar* pTAR = NULL;
		for ( MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin(); pos != m_mapPlayHostile.end(); ++pos )
		{
			pTAR = GetCharByDbNum( pos->first );
			if (pTAR)
				pTAR->SendToClient(&NetMsgBrd);

			if (!pos->second)
                sc::writeLogError(std::string("GLChar::MsgSendUpdateState, pos->second = NULL"));
		}
	}
	GLLandMan* const pLand( GetLandMan() );
	/*
	bool bGuidBattleMap = pLand == NULL ? false : pLand->m_bGuidBattleMap;
	bool bClubDeathMatch = pLand == NULL ? false : pLand->m_bClubDeathMatchMap;
	bool bPKMap = pLand == NULL ? false : pLand->IsPKMap();
    bool bCTF = IsCTFPlayer() ? true : false;
	bool bSCHOOL_FREEPK = SchoolFreePkOn();
	bool bBRIGHTEVENT   = IsBRIGHTEVENT();
	bool bPKServer   = IsPKServer();	
	bool bHostileTargetExist(((int)m_mapPlayHostile.size())>0);
	// bHostileTargetExist;
	// �������� ��밡 �����ϸ� HP ���� �޼����� ������;
	// Ŭ���̾�Ʈ������ HP == 0?���� ó���ϴ� ������ ������, pkŸ�� pk���������ƴҶ� �����̾ȵǼ� ���������ϴ� �κ��� �����;
	// ex RM# 1674 - GLCharClient::UPDATE_DATA������ �ҷ�̳��̼� ���� ���Ž� hp�� 0�����ƴ����� �˻��Ѵ�;

    // HP ��ȭ���¸� �ֺ��� ���������� ���� ���� üũ:
    // �Ϲ����� ��Ȳ : HPǥ�� �÷��� üũ;
    // ����ó�� : ������, (��)CDM, �п��� FreePKŸ��, �����̺�Ʈ, PK����, PK��, �����;
    // if ( bGuidBattleMap || bClubDeathMatch || bSCHOOL_FREEPK || bBRIGHTEVENT || bPKServer || bPKMap || bCTF || bHostileTargetExist )
	*/
	//  [7/13/2016 gbgim] RM #3571
	// - ������ �� �ִ� ��Ȳ�� �ƴ϶�� �ٸ� Ŭ���� HP������ �����ʴ´�;
	//   (�������� ���� ���������� ������谡 �ƴ϶�� ������ Ǯ�Ƿ� ������);
	// - Ÿ������ ������ ���� ���� ���� �� ��Ȳ�� �����ϰԵǸ� ���ŵ����ʴ´�;
	// - �̹� PK Server�� ����ϴ� ���� �ְ�, ������ ���� �����ϴ� ���̸鼭 ;
	//   PK��Ȳ�� �ƴѰ��� �п� ������ ���ζ�� �����ϰ� ��ȭ������ üũ�ϵ��� ������;
	const bool bPeaceZone(pLand && pLand->IsPeaceZone());
	if ( bPeaceZone == false )
    {
        SendMsgViewAround(&NetMsgBrd);
    }
    else
    {
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand != NULL )
		{
			Faction::ManagerField* pFactionMan = pLand->GetFactionManager();
			if ( pFactionMan )
			{
				const Faction::EMRELATION_FLAG_TYPE emHpVisibleRealtionType = pFactionMan->getHPVisible();

				switch ( emHpVisibleRealtionType )
				{
				case Faction::EMRELATION_FLAG_ERROR:
				case Faction::EM_RELATION_FLAG_NONE:
					break;

				case Faction::EM_RELATION_FLAG_ALL:
					SendMsgViewAround(&NetMsgBrd);
					break;

				default:
					SendMsgViewAroundRelation(emHpVisibleRealtionType, &NetMsgBrd);
					break;
				}
			}
		}
    }
}

void GLChar::MsgSendUpdateHireSummonState( DWORD dwHireSummonGaeaID, SNATIVEID sBasicSummonID )
{
	GLMSG::SNETPC_UPDATE_HIRE_SUMMON_STATE NetMsg;
	NetMsg.dwHireSummonGaeaID = dwHireSummonGaeaID;
	NetMsg.sBasicSummonID = sBasicSummonID;

	SendToClient( &NetMsg );
}

void GLChar::MsgSendUpdateCharacterSlotState ( void )
{
	GLMSG::SNETPC_UPDATE_CHARSLOT_FC sNetMsg;
	sNetMsg.dwChaNum = CharDbNum();
	sNetMsg.wCharSlotNum = m_wSlotIndex;
	sNetMsg.nOpenCharSlot = GLCharSlotMan::Instance()->GetOpenSlotCount();
	
	memcpy( sNetMsg.arrSlotLockState, m_arrLockSlotState, sizeof(bool)*EMCHAR_SLOT_DATA_SIZE );

	SendToClient( &sNetMsg );
}

HRESULT GLChar::MsgReady (NET_MSG_GENERIC* nmg)
{
	// Ȱ��ȭ ��Ŵ.
	//
	ReSetActState(EM_ACT_WAITING);

	// ���� �޽��� ����
	TriggerSystem::SServerEntryMessage msg;
	msg.dwCharDbNum = GetCharID();
	SendTriggerMessage(&msg);

	return S_OK;
}

HRESULT GLChar::MsgMoveState (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_MOVESTATE *pNetMsg = reinterpret_cast<GLMSG::SNETPC_MOVESTATE*> ( nmg );

	const DWORD dwOldActState = GetActState();

	if ( m_dwUserLvl >= USER_USER_GM )
	{
		if ( pNetMsg->dwActState & EM_REQ_VISIBLENONE )	
			SetActState( EM_REQ_VISIBLENONE );
		else											
			ReSetActState( EM_REQ_VISIBLENONE );
		
		if ( pNetMsg->dwActState & EM_REQ_VISIBLEOFF )	
			SetActState( EM_REQ_VISIBLEOFF );
		else											
			ReSetActState( EM_REQ_VISIBLEOFF );
	} //if

	if( pNetMsg->dwActState & EM_ACT_RUN )
    {
		SetActState( EM_ACT_RUN );
		if ( GLHIDESET::CheckHideSet(EMHIDE_TURN_RUN) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}
    else 
    {
		ReSetActState( EM_ACT_RUN );
		if ( GLHIDESET::CheckHideSet(EMHIDE_TURN_WALK) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}	

	if( pNetMsg->dwActState & EM_ACT_PEACEMODE )	
		SetActState( EM_ACT_PEACEMODE );
	else											
		ReSetActState( EM_ACT_PEACEMODE );

	const DWORD& dwCurActState = GetActState();
	if( dwOldActState != dwCurActState )
	{
		// �����̸� �ݿ��� �̵� �ӵ� ����.
		m_actorMove.SetMaxSpeed( GetMoveVelo() );

		// �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
		GLMSG::SNETPC_MOVESTATE_BRD NetMsgFB( m_dwGaeaID, dwCurActState );
		SendMsgViewAround( &NetMsgFB );
	} //if

	return S_OK;
}

void GLChar::SendJumpMsg( const D3DXVECTOR3& vTarPos, bool bGate, DWORD dwFlags )
{
	if( !(dwFlags & MOVE_OPTION_NOT_TURN_IDLE) ) 
		TurnAction( GLAT_IDLE );

	if( bGate )
	{
		GLMSG::SNET_GM_MOVE2GATE_FB		NetMsgFB;
		NetMsgFB.vPOS		= vTarPos;
		NetMsgFB.dwFlags	= dwFlags;
		SendToClient( &NetMsgFB );
	}

	// Ŭ���̾�Ʈ�鿡�� ��ġ ���� ����.
	GLMSG::SNETPC_JUMP_POS_BRD	NetMsgJump( m_dwGaeaID, vTarPos, dwFlags );
	SendMsgViewAround( &NetMsgJump );
}

void GLChar::SendGoingMsg(const D3DXVECTOR3& vTarPos)
{	
	m_TargetID.vPos = vTarPos;
	TurnAction( GLAT_MOVE );

	BOOL bSucceed = m_actorMove.GotoLocation(
		m_TargetID.vPos + D3DXVECTOR3( 0, 10, 0 ),
		m_TargetID.vPos - D3DXVECTOR3( 0, 10, 0 ) );

	if ( m_emVehicle == EMVEHICLE_DRIVER )
	{
		for ( DWORD _i = MAX_PASSENGER_COUNT; _i; --_i )
		{
			const DWORD dwGaeaID = VehiclePassengerId(_i - 1);
			if ( dwGaeaID == m_dwGaeaID )
				continue;

			GLChar* const pPassenger = m_pGaeaServer->GetChar(dwGaeaID);
			if ( pPassenger == NULL )
				continue;

			pPassenger->SendGoingMsg(vTarPos);
		}
	}	

	if( bSucceed )
	{
		m_actorMove.SetMaxSpeed( GetMoveVelo() );

		if ( m_emVehicle != EMVEHICLE_PASSENGER )
		{			
			// �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
			GLMSG::SNETPC_GOTO_BRD NetMsgFB;
			NetMsgFB.dwGaeaID	= m_dwGaeaID;
			NetMsgFB.dwActState	= GetActState();
			NetMsgFB.vCurPos	= m_vPosition;
			NetMsgFB.vTarPos	= m_TargetID.vPos;
			NetMsgFB.fDelay		= 0.0f;
			SendMsgViewAround( &NetMsgFB );
		}	
	}
}

bool GLChar::CheckStateToGo()
{
	// ������ ������ ���¿��� �̵��� �� ����.
	if (m_pPrivateMarket->IsOpen())
        return false;

	// �����°� DEAD �̸� DEAD Animation ó��
	if (IsActState(EM_ACT_DIE))
	{
		// �������� Ŭ���̾�Ʈ �鿡�� �˸�.
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_FALLING;
        
        // �ڽ��� Ŭ���̾�Ʈ��
		SendToClient(&NetMsgBrd);
		return false;
	}

	if ( m_sDOMINATE.emController == EMSPEC_DOMINATE_TYPE_FIX )
		return false;

	return IsMovement();
}

//! �̵� ó��
HRESULT GLChar::MsgGoto(NET_MSG_GENERIC* nmg)
{
	if( m_pGaeaServer->m_bEmptyMsg )
        return S_OK;

	if( !CheckStateToGo() )
		return E_FAIL;

	GLMSG::SNETPC_GOTO* pPacket = reinterpret_cast<GLMSG::SNETPC_GOTO*>( nmg );

	BOOL	bRun	= IsActState( EM_ACT_RUN );				// �޸��� ��������
	BOOL	bToRun	= pPacket->dwActState & EM_ACT_RUN;		// �޷��� �ϴ���

	// ���� ����
	if( bRun != bToRun )
	{
		if ( bToRun )
		{
            SetActState( EM_ACT_RUN );
			if ( GLHIDESET::CheckHideSet(EMHIDE_TURN_RUN) )
			{
				RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				RestoreActStateByInvisible();
			}
		}
		else
		{
            ReSetActState( EM_ACT_RUN );
			if ( GLHIDESET::CheckHideSet(EMHIDE_TURN_WALK) )
			{
				RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				RestoreActStateByInvisible();
			}
		}
	} //if

	// ���ο� ��ġ �̵��� ������ �� ���� Client �� Server ��ġ ���̿� �浹 Object �� ���� ���, 
	// Client �� ��ġ�� ������ġ�� ����.
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand )
	{
		if ( pLand->IsLayerSimpleCollision( m_vPosition, pPacket->vCurPos, 5.f ) )
		{
			SendJumpMsg( m_vPosition, true, 0 );
			return S_OK;
		}
	}

	D3DXVECTOR3 vDist = m_vPosition - pPacket->vCurPos;
	float fDist = D3DXVec3LengthSq( &vDist );

	if( fDist > MAX_MOVEDIST )
	{		
		// ������ ���̸� ������ ������ ( ���� �����̵� ���� )
		if( isAction(GLAT_TALK, GLAT_GATHERING) )
			return E_FAIL;

        //sc::writeLogWarn(sc::string::format("MsgGoto %1% > %2%", fDist, MAX_MOVEDIST));
		SendJumpMsg( m_vPosition, true, 0 );
	}
	else
	{
		SendGoingMsg( pPacket->vTarPos );
	} //if..else

	return S_OK;
}

HRESULT GLChar::MsgDominateMoveTo(NET_MSG_GENERIC* nmg)
{
	if( m_pGaeaServer->m_bEmptyMsg )
		return S_OK;
	
	GLMSG::SNETPC_DOMINATE_MOVETO_CF* pNetMsg = reinterpret_cast<GLMSG::SNETPC_DOMINATE_MOVETO_CF*>( nmg );

	ServerActor* pACTOR = GetTarget(pNetMsg->sTargetID);	

	if ( pACTOR == NULL )
	{
		GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
		NetMsg.emFB = EMDOMINATE_MOVETO_FB_FAIL;
		SendToClient( &NetMsg );
		m_sDOMINATE.bDominate = false;
		return E_FAIL;
	}

	if ( pACTOR->IsValidBody() == false )
	{
		GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
		NetMsg.emFB = EMDOMINATE_MOVETO_FB_FAIL;
		SendToClient( &NetMsg );
		m_sDOMINATE.bDominate = false;
		return E_FAIL;
	}

	switch ( pACTOR->GetCrow() )
	{
	case CROW_PC:
		{
			
			GLChar* pChar = m_pGaeaServer->GetChar(pACTOR->GetGaeaID());
			if ( pChar->m_sDOMINATE.emController == EMSPEC_DOMINATE_TYPE_CASTER )
			{				
				GLMSG::SNETPC_DOMINATE_MOVETO_FC NetMsg;
				NetMsg.fTime = pNetMsg->fTime;	
				NetMsg.vFromPt = pNetMsg->vFromPt;
				NetMsg.vTargetPt = pNetMsg->vTargetPt;
				pChar->SendToClient( &NetMsg );
			}
			else
			{
				GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
				NetMsg.emFB = EMDOMINATE_MOVETO_FB_NOT_DOMINATED;
				SendToClient( &NetMsg );
				NetMsg.emFB = EMDOMINATE_MOVETO_FB_CANCEL;

				pChar->SendToClient( &NetMsg );
				m_sDOMINATE.bDominate = false;
			}
		}
		break;		
	case CROW_MOB:
		{
			if ( ((GLCrow*)pACTOR)->m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER )
			{
				BOOL bColl=FALSE;
				DWORD dwCollID = 0;
				D3DXVECTOR3 vCollPos = ( pNetMsg->vFromPt + pNetMsg->vTargetPt ) / 2.0f;

				if ( pACTOR->IsCollision(pNetMsg->vFromPt, pNetMsg->vTargetPt, vCollPos, dwCollID, bColl) == false )
					return E_FAIL;

				if ( bColl == FALSE )
					return E_FAIL;

				pACTOR->MoveTo(&vCollPos, false, 0);			
			}
			else
			{
				GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
				NetMsg.emFB = EMDOMINATE_MOVETO_FB_NOT_DOMINATED;
				SendToClient( &NetMsg );
				m_sDOMINATE.bDominate = false;
			}
		}
		break;
	case CROW_SUMMON:
		{
			if ( ((GLSummonField*)pACTOR)->m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER )
			{
				BOOL bColl=FALSE;
				DWORD dwCollID = 0;
				D3DXVECTOR3 vCollPos = ( pNetMsg->vFromPt + pNetMsg->vTargetPt ) / 2.0f;

				if ( pACTOR->IsCollision(pNetMsg->vFromPt, pNetMsg->vTargetPt, vCollPos, dwCollID, bColl) == false )
					return E_FAIL;

				if ( bColl == FALSE )
					return E_FAIL;

				((GLSummonField*)pACTOR)->ReSetActState( EM_ACT_ALL );
				((GLSummonField*)pACTOR)->SetActState( EM_ACT_RUN );
				((GLSummonField*)pACTOR)->Move( vCollPos, true, false );			
			}
			else
			{
				GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
				NetMsg.emFB = EMDOMINATE_MOVETO_FB_NOT_DOMINATED;
				SendToClient( &NetMsg );
				m_sDOMINATE.bDominate = false;
			}
		}
		break;
	}
	
	return S_OK;
}

// AnimationMove �� �� ȸ���� �ٸ� Ŭ�� ������.
void GLChar::MsgAniMoveStartCF( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ANIMOVE_START_CF* pNetMsg = (GLMSG::SNETPC_ANIMOVE_START_CF*) nmg;

	// AnimationMove Start

	// �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
	GLMSG::SNETPC_ANIMOVE_START_BRD NetMsgFB;
	NetMsgFB.dwGaeaID	= m_dwGaeaID;
	NetMsgFB.m_fAngleY	= pNetMsg->m_fAngleY;
	SendMsgViewAround( &NetMsgFB );
}

// AnimationMove�� �̵��� �ڽ��� ������ ��ġ�� ������ �����Ѵ�.
void GLChar::MsgAniMoveEndCF( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ANIMOVE_END_CF* pNetMsg = (GLMSG::SNETPC_ANIMOVE_END_CF*) nmg;

	// AnimationMove End

	// 1. ������ġ�� �� �� �ִ��� Ȯ���ϰ� �� �� �ִٸ� �̵��Ѵ�.
	//		�� �� ���ٸ� Client �� Rollback �����ش�. ( Character �� Crow �� �ٸ� )
	// 2. �׸��� ���� ��ġ�� �� �������� �̵��϶�� �����ش�. ( Character �� Crow �� �ٸ� )
	//
	// ���� �۾��� MsgGoto ���� �� �۾��ȴ�.

	GLMSG::SNETPC_GOTO sMagGoto;
	sMagGoto.dwGaeaID	= m_dwGaeaID;
	sMagGoto.dwActState = GetActState();
	sMagGoto.vCurPos	= m_vPosition;
	sMagGoto.vTarPos	= pNetMsg->m_vCurPos;
	MsgGoto( &sMagGoto );
}

HRESULT GLChar::MsgAttack (NET_MSG_GENERIC* nmg)
{
	if (m_pGaeaServer->m_bEmptyMsg)
		return S_OK;

	if (!IsValidBody())
		return E_FAIL;

	if (IsCtrlBlockBody())
		return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
		return E_FAIL;

    GLMSG::SNETPC_ATTACK *pNetMsg = reinterpret_cast<GLMSG::SNETPC_ATTACK*> ( nmg );    

    //�������̶�� �ٸ� Ÿ���� �����Ҽ� ����.
    if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        if ( pNetMsg->dwTarID != m_sTaunt.sTauntTargetID.GaeaId )
        {
            return E_FAIL;
        }
    }   

    m_dwANISUBSELECT	= pNetMsg->dwAniSel;
    m_TargetID.emCrow	= pNetMsg->emTarCrow;
    m_TargetID.GaeaId	= pNetMsg->dwTarID;

    ServerActor* pTARGET = GetTarget(STARGETID(pNetMsg->emTarCrow, pNetMsg->dwTarID));
    if (!pTARGET)
        return E_FAIL;

    // Ÿ�ٰ��� �Ÿ� �˻�
    D3DXVECTOR3 vTarPos = pTARGET->GetPosition();

    int nAttackAbleDistance = 
        pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 9 + static_cast<int>(GETSUM_RANGE_PSY_LONG());

	if ( nAttackAbleDistance < 0 )
		nAttackAbleDistance = pTARGET->GetBodyRadius() + GETBODYRADIUS() + 9;

	nAttackAbleDistance *= GLCONST_CHAR::fATTACK_RANGE_ADJUST_RATE;
    if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( nAttackAbleDistance, m_vPosition, vTarPos, 5.f ) )
    {
        // �ڽſ���.
        GLMSG::SNETPC_ATTACK_AVOID NetMsg;
        NetMsg.emTarCrow	= m_TargetID.emCrow;
        NetMsg.dwTarID		= m_TargetID.GaeaId;
        SendToClient((NET_MSG_GENERIC*) &NetMsg );

        // �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
        //
        GLMSG::SNETPC_ATTACK_AVOID_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID	= m_dwGaeaID;
        NetMsgBrd.emTarCrow	= m_TargetID.emCrow;
        NetMsgBrd.dwTarID	= m_TargetID.GaeaId;
        SendMsgViewAround(&NetMsgBrd);

        if ( m_TargetID.emCrow == CROW_PC ){
            GLChar* pChar = GetChar(m_TargetID.GaeaId);
            if ( pChar )
			{
                if ( GLHIDESET::CheckHideSet(EMHIDE_AVOID) )
                {
                    pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
                    pChar->RestoreActStateByInvisible();
                }
            }			
        }	

        return E_FAIL;
    }

    // ������ ������ Ÿ�������� �˻�;
    // ���� �߸����� ����� ���� ���� �����Ѱ����� ����;
    const EMREACTIONABLE emREACTIONALBE  = IsReActionable( pTARGET, EMIMPACT_SIDE_ENEMY );
    switch ( emREACTIONALBE )
    {
    case EMREACTIONABLE_FALSE:
        return E_FAIL;

    case EMREACTIONABLE_TRUE:
        break;

    case EMREACTIONABLE_FORCED_TRUE:
        // ������ �����ѵ� �߸�������̶� ���� �������� �����Ǿ��� ���;
        // 
        // �� Ÿ�� �ܿ� Ÿ���� ���� ���� �Ұ��ϴ�.;
        // PC�� ��� Client���� Ÿ�ټ����� ó���Ͽ� ������ ������ �����µ�
        // ��Ÿ�ٿܿ� �������� Ÿ���� �ִٴ� ���� ��Ŷ ������ ���ɼ��� �ִ�.;
        // 
        ReActionByForcedAction(pTARGET, EMIMPACT_SIDE_ENEMY);
        break;

    default:
        return E_FAIL;
    }

    EMSLOT emRHand = GetCurRHand();
    EMSLOT emLHand = GetCurLHand();

	BOOL bDistinguish = RF_DISGUISE( m_EffSkillVarSet ).IsOn();

	GLDistinguishSkinManager::sTransAni* pAniSet = NULL;

	if( bDistinguish ) {
		DWORD dwIndex = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

		SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
		SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
		if ( pItem )
		{
			// Table ID �� ���� ��쿡�� Index�� �����Ѵ�;
			// �����͸� �ø��� �ʱ� ���Ͽ� Pet ������ �̿��Ѵ�;
			if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
				dwIndex = pItem->sPet.sPetID.Sid();
		}

		pAniSet = DISTINGUISH_INST.GetAniSet( m_wSex, dwIndex );
	}

    EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );

    GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK &vecAniAttack = ( pAniSet != NULL ) ? pAniSet->m_ANIMATION[AN_ATTACK][emANISUBTYPE] : ConstCharClass.m_ANIMATION[AN_ATTACK][emANISUBTYPE];

    if (vecAniAttack.empty())
        return E_FAIL;

    if (vecAniAttack.size() <= m_dwANISUBSELECT)
        m_dwANISUBSELECT = 0;


    const SANIATTACK &sAniAttack = vecAniAttack[m_dwANISUBSELECT];
    WORD wStrikeNum = sAniAttack.m_wDivCount;

    m_TargetID.vPos = pTARGET->GetPosition();

    // ������ �������� �˻�.
    //
    EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);

    if (emBeginFB != EMBEGINA_OK && emBeginFB != EMBEGINA_SP)
        return E_FAIL;


    // �Ҹ� ������ ��ȿ�� �˻���, ������� �޽��� �߻�.
    CheckInstanceItem();

    // ���� ����ġ ���.
    SNATIVEID				idSkill( false );
    SCHARSKILL				sSkillLevel;
    GetSkillLevel( idSkill, sSkillLevel );
    m_WorkingSkill.Start( sSkillLevel, this, emBeginFB == EMBEGINA_SP, &m_TargetID, m_wTARNUM, m_sTARIDS, false );

    // ���� ����.
    //
    TurnAction(GLAT_ATTACK);

    if ( GLHIDESET::CheckHideSet(EMHIDE_START_ATTACK) )
    {
        RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
        RestoreActStateByInvisible();
    }

    // ���� ������ - �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
    //
    GLMSG::SNETPC_ATTACK_BRD NetMsgFB;
    NetMsgFB.dwGaeaID = m_dwGaeaID;
    NetMsgFB.emTarCrow = pNetMsg->emTarCrow;
    NetMsgFB.dwTarID = pNetMsg->dwTarID;
    NetMsgFB.dwAniSel = m_dwANISUBSELECT;

    SendMsgViewAround(&NetMsgFB);


    // ���� �����ϸ� ���� ���ݸ���� ����
    if ( pTARGET->GetCrow() == CROW_MOB )
    {
        GLPetField* pMyPet = GetMyPet();
        if ( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
        {
            D3DXVECTOR3 vDist;
            float fDist;
            vDist = pMyPet->m_vPos - m_vPosition;
            fDist = D3DXVec3Length(&vDist);

            // �����Ÿ� �ȿ� ������ 
            if ( fDist <= GLCONST_PET::fWalkArea )
            {
                GLMSG::SNETPET_ATTACK NetMsg(m_TargetID);
                SendToClient( &NetMsg );

                GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(m_TargetID);
                NetMsgBRD.dwGUID = m_dwPetGUID;
                SendMsgViewAround( &NetMsgBRD );
            }
        }
    }

    return S_OK;
}

HRESULT GLChar::MsgSetTarget(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_SELECT_TARGET* pNetMsg = (GLMSG::SNETPC_SELECT_TARGET*)nmg;
	m_TargetID.emCrow = pNetMsg->emTarCrow;
	m_TargetID.GaeaId = pNetMsg->dwTarID;

	return S_OK;
}

HRESULT GLChar::MsgAttackCancel (NET_MSG_GENERIC* nmg)
{
	// �׼� ���.
	if ( isAction(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );

	// �ڽ��� �ֺ� Char���� �ڽ��� Msg�� ����.
	//
	GLMSG::SNETPC_ATTACK_CANCEL_BRD NetMsgFB;
	NetMsgFB.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround(&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgGatheringCancel (NET_MSG_GENERIC* nmg)
{
	// �׼� ���.
	if ( isAction(GLAT_GATHERING) )	TurnAction ( GLAT_IDLE );
	return S_OK;
}

void GLChar::ReSetStartMap()
{
	// ����Ʈ�� �� ã���� ���� ���� ��ġ�� �̵��Ѵ�.
	m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
	m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
	m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);

	// ������ġ ���� (���п��� ���۸�����)
	GLMSG::SNETPC_UPDATE_STARTCALL NetMsg;
	NetMsg.dwChaNum		 = CharDbNum();
	NetMsg.sStartMapID   = m_sStartMapID;
	NetMsg.dwStartGateID = m_dwStartGate;
	NetMsg.vStartPos	 = m_vStartPos;

	// Ŭ���̾�Ʈ�� �˸�
	SendToClient( &NetMsg );

	// Agent�� �˸�
	SendToAgent(&NetMsg);

	return;
}

//! ȸ���� ����ġ���� �˷��ش�. NPC
HRESULT GLChar::MsgReqGetExpRecoveryNpc (NET_MSG_GENERIC* nmg)
{
    EMSERVICE_PROVIDER ServiceProvider = GetServiceProvider();
    // ����ġȸ��_����_Npc
    {
	    GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC *pNetMsg = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC *)nmg;
	    GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB MsgFB;

	    LONGLONG nReExp = 0;
	    LONGLONG nDecMoney = 0;

	    nReExp = m_lnRestorableExp;
	    nDecMoney = GetEXPRestoreCost ( nReExp );	

	    MsgFB.nReExp = nReExp;
	    MsgFB.nDecMoney = nDecMoney;
	    MsgFB.dwNpcGlobID = pNetMsg->dwNpcGlobID;
	    SendToClient( &MsgFB );	
    }
	return S_OK;
}

//	����ġ�� ȸ���Ѵ�.
HRESULT GLChar::MsgReqRecoveryNpc (NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqRecoveryNpc", m_CharDbNum ) );
	}

    EMSERVICE_PROVIDER ServiceProvider = GetServiceProvider();

    // ����ġȸ��_����_Npc
    {
	    GLMSG::SNETPC_REQ_RECOVERY_NPC *pNetMsg = (GLMSG::SNETPC_REQ_RECOVERY_NPC *)nmg;
	    GLMSG::SNETPC_REQ_RECOVERY_NPC_FB MsgFB;

	    LONGLONG nReExp = 0;
	    LONGLONG nDecMoney = 0;

	    nReExp = m_lnRestorableExp;
	    nDecMoney = GetEXPRestoreCost ( nReExp );

	    if ( nDecMoney > GetInvenMoney() )
	    {
		    MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOMONEY;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }

	    if ( nReExp <= 0  )
	    {
		    MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOREEXP;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }

	    // NPC üũ 
	    PGLCROW pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	    if ( !pCrow )
	    {
		    MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_FAIL;
		    SendToClient( &MsgFB );
		    return E_FAIL;
	    }

        if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_RECOVERY_EXP ) )
        {
            MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOTUSE;
            SendToClient( &MsgFB );
            return E_FAIL;
        }

	    CheckMoneyUpdate( GetInvenMoney(), nDecMoney, FALSE, "Recovery Exp To Npc" );

	    m_sExperience.lnNow += m_lnRestorableExp;
		SubtractInvenMoneyServer( nDecMoney, SUBTRACT_INVEN_MONEY_EXP_RECOVERY_NPC );
	    m_lnRestorableExp = 0;	// ����ġ ȸ�� ���ɷ� 0���� �ʱ�ȭ		
    	
	    // ����ġ ȸ�� ���� �˸�
	    MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_OK;
	    MsgFB.nReExp = nReExp;
	    SendToClient( &MsgFB );

	    SendInvenMoneyUpdate();
    	
	    //	����ġ ��Ȱ �α�
	    {   
			// ��� �� �α� ����
		    LogCharAction
			    (
			    m_CharDbNum,					//	�����.
			    gamelog::ACT_RECOVERY_NPC,		//	����.
			    gamelog::ID_CHAR, 0,					//	����.
			    nReExp,						// exp
			    0,							// bright
			    0,							// life
			    (int)nDecMoney,				// money
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
			    );
	    }
    }
	return S_OK;
}

// *****************************************************
// Desc: ģ���̵� ó�� (Agent->Field)
// *****************************************************
HRESULT GLChar::MsgReqFriendFld (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_2_FRIEND_FLD *pNetMsg = (GLMSG::SNETPC_2_FRIEND_FLD *)nmg;
	GLMSG::SNETPC_2_FRIEND_FB MsgFB;

	// ������ ���� ���� �Ǵ�.
	SINVENITEM *pINVENITEM = m_cInventory.GetItem( pNetMsg->wItemPosX, pNetMsg->wItemPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )	return S_FALSE;

	const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM_DATA )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	if ( pITEM_DATA->sBasicOp.emItemType != ITEM_2FRIEND )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	//	�ɸ��� ���� ���°� �ƴ� ���.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	��� ������ ���.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	�ŷ����� ���.
	if ( m_sTrade.Valid() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	SMAPNODE *pMapNode = m_pGaeaServer->FindMapNode ( pNetMsg->sFriendMapID.getBaseMapID() );
	if ( !pMapNode )
	{
		MsgFB.emFB = EM2FRIEND_FB_FAIL;
		SendToClient( &MsgFB );
		return FALSE;
	}

	if (pMapNode->IsLeaderClubZone())
	{
		MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	if (pMapNode->IsClubDeathMatchZone())
	{
		MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	// �ش� ������ ���� ���� �˻�.
	const SLEVEL_REQUIRE* pLEVEL_REQUIRE = m_pGaeaServer->GetLevelRequire(pNetMsg->sFriendMapID.getBaseMapID());
	if ( !pLEVEL_REQUIRE )
	{
		MsgFB.emFB = EM2FRIEND_FB_FAIL;
		SendToClient( &MsgFB );
		return FALSE;
	}

	if ( m_dwUserLvl < USER_USER_GM )
	{
		if ( pLEVEL_REQUIRE->ISCOMPLETE(this)!=EMREQUIRE_COMPLETE )
		{
			MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
			SendToClient( &MsgFB );
			return FALSE;
		}
	}

	// pk ����� ������ ��� �̻��� ��� ��ȯ ī���� ����� ���´�.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		MsgFB.emFB = EM2FRIEND_FB_PK_CONDITION;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// ������ �Ҹ�.
	ConsumeInvenItem( pNetMsg->wItemPosX, pNetMsg->wItemPosY, true );

	MapID sMAPID = pNetMsg->sFriendMapID;
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS = pNetMsg->vFriendPos;


	/// �̵�;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;
	sMoveInfo.vPosition = vPOS;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	/*// PET
	// ���̵��� Pet ����
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	
	//VEHICLE
	m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, true );
	
	// SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	// �ٸ� �ʵ� ������ ���.
	if ( pMapNode->GetFieldServerNum() != GetFieldSvrID() )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
		NetMsgAg.mapIDTo = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPosition = vPOS;
		SendToAgent(&NetMsgAg);
	}
	// ���� �ʵ� ������ ���.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ� const MapID sCurMapID(GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(GetCurrentMap());

		BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SendToClient( &NetMsgFB );
			return FALSE;
		}

		// ������ �����̻� ����
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				RESETSKEFF ( i );

			// CP �ʱ�ȭ ������ Ȯ��
			if (pMapNode->IsCombatPointReset())
				ResetCP();
		}

		ResetAction();		

		// �� �̵� ������ �˸�.
		//
		GLLandMan* const pLand( GetLandMan() );
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pLand == NULL ? TRUE : pLand->IsCullByObjectMap();
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = m_vPosition;
		SendToAgent(&NetMsgFB);
	}*/

	return S_OK;
}

HRESULT GLChar::MsgReqMove2CharFld (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_GM_MOVE2CHAR_FLD* pNetMsg =
		static_cast< GLMSG::SNETPC_GM_MOVE2CHAR_FLD* >( nmg );
	//GLMSG::SNETPC_GM_MOVE2CHAR_FB MsgFB;

	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = pNetMsg->mapIDTo;
	sMoveInfo.vPosition = pNetMsg->vToPos;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	return S_OK;

	/*//	�ɸ��� ���� ���°� �ƴ� ���.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	��� ������ ���.
	if ( m_sCONFTING.IsCONFRONTING()  )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	�ŷ����� ���.
	if ( m_sTrade.Valid() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	const MapID _mapIDTo(pNetMsg->mapIDTo);
	const DWORD dwFieldServerIDTo(pNetMsg->dwFieldServerIDTo);
	const D3DXVECTOR3& vPosition(pNetMsg->vToPos);
	DWORD dwGATEID(UINT_MAX);	

	SMAPNODE *pMapNode(m_pGaeaServer->FindMapNode(pNetMsg->mapIDTo));
	if ( pMapNode == NULL )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_FAIL;
		SendToClient( &MsgFB );
		return FALSE;
	}


	// PET
	// ���̵��� Pet ����
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	
	//VEHICLE
	m_pGaeaServer->SetActiveVehicle( m_ClientSlot, m_dwGaeaID, false );

	// SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	// ��ũ�ΰ� �����ִٸ� ���̵��� ����;
	if ( IsActState(EM_ACT_MACROMODE) )
	{
		MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		m_pGaeaServer->SENDTOCLIENT ( ClientSlot(), &msg );
	}
	
	GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(m_CharDbNum, GetCurrentMap(), GetPosition(), GetFieldSvrID(), pNetMsg->mapIDTo);	
	m_pGaeaServer->SENDTOMYSELF(&_moveMapMessage);

	// �ٸ� �ʵ� ������ ���.
	if ( dwFieldServerIDTo != GetFieldSvrID() )
	{ // �δ��� ��� pMapNode�� fieldServerID�� ���� �� �� ����;
		// �̵��� ���� �δ��� ��� �α׸� ����		

		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = CharDbNum();
		NetMsgAg.dwFieldServerIDTo = dwFieldServerIDTo;
		NetMsgAg.mapIDTo = _mapIDTo;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPosition = vPosition;
		SendToAgent(&NetMsgAg);
	}
	// ���� �ʵ� ������ ���.
	else
	{

		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ� const MapID sCurMapID(GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(GetCurrentMap());

        BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, _mapIDTo, dwGATEID, vPosition );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SendToClient( &NetMsgFB );
            return FALSE;
        }

        // ������ �����̻� ����
        if ( sCurMapID != _mapIDTo )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                RESETSKEFF ( i );

            // CP �ʱ�ȭ ������ Ȯ��
            if (pMapNode->IsCombatPointReset())
                ResetCP();
        }

		ResetAction();

		// �� �̵� ������ �˸�.
		//
		GLLandMan* const pLand( GetLandMan() );
		NetMsgFB.bCullByObjectMap = pLand == NULL ? TRUE : pLand->IsCullByObjectMap();
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = _mapIDTo;
		NetMsgFB.vPOS = m_vPosition;
		SendToAgent(&NetMsgFB);
	}*/

	return S_OK;
}

HRESULT GLChar::MsgReqLevelUp (NET_MSG_GENERIC* nmg)
{
	if (m_pGaeaServer->m_bEmptyMsg)
        return S_OK;	

	if (VALID_LEVELUP())
        return ReqLevelUpValid();
    else
        return ReqLevelUpInvalid();
}

HRESULT GLChar::ReqLevelUpValid() 
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ReqLevelUpValid", m_CharDbNum ) );
	}

    bool bInitNowLevel = TRUE;
    if (GLCONST_CHAR::ExcessExpProcessType == 1) //m_pGLGaeaServer->GetExcessExpProcessType() == 1 )
    {
        if (m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W)
            bInitNowLevel = FALSE;
    }
    else if (GLCONST_CHAR::ExcessExpProcessType == 2 || m_bVietnamLevelUp)
    {
        bInitNowLevel = FALSE;
    }

    // ������ ó��
    LEVLEUP(bInitNowLevel, false, true);

	//������ �ϱ� ����Ʈ
	DoQuestLevelUp();

    // ������ ���� ����Ʈ �ڵ� ����
    QuestStartFromGetLEVEL ( m_wLevel );

    // ����Ʈ ���� ���� ���� ����.
    //
    DoQuestLevel ();


    //mjeon.activity
    ActivityCheckLevel();


    //	[�ڽſ���]
    GLMSG::SNETPC_REQ_LEVELUP_FB NetMsg;
    NetMsg.wLevel		 = m_wLevel;
    NetMsg.wStatsPoint	 = m_wStatsPoint;
    NetMsg.dwSkillPoint  = m_dwSkillPoint;
    NetMsg.bInitNowLevel = bInitNowLevel;
	NetMsg.bJumping = false;
	NetMsg.bCalExp = true;
    NetMsg.sMapID		 = GetCurrentMap().getBaseMapID();
    SendToClient( &NetMsg );

    //	[��ο���]
    GLMSG::SNETPC_REQ_LEVELUP_BRD NetMsgBrd( m_wLevel );
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    SendMsgViewAround(&NetMsgBrd);

	if ( isValidParty() && (GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false) )
    {
		GLMSG::SNET_PARTY_MBR_LEVEL_UP NetPartyMemeberLevelUp(m_dwGaeaID,m_wLevel);		
		SendToParty(&NetPartyMemeberLevelUp);
    }


    //	[��Ƽ,�����] hp ��ȭ �˸�.
    MsgSendUpdateState();


    // ������ �α� ���.(���� 20�̻󿡼�)
    if (m_wLevel > 20)
    {   
		// ��� �� �α� ����
        LogCharAction(
            m_CharDbNum,			//	�����
            gamelog::ACT_LEV_UP,	//	����
            gamelog::ID_CHAR,
            m_wLevel,				//	ĳ���� ����
            m_sExperience.lnNow,	//	exp
            0,						//	bright
            0,						//	life
            0,
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());					//	money
    }

    // Agent �� �˸�
    GLMSG::NET_CHAR_LEVEL_UP_FA MsgFA(m_CharDbNum, m_wLevel);
    SendToAgent(&MsgFA);

    // Ŭ���� LevelUp �ݿ�    
    m_pGaeaServer->ClubMemberLevelUp(m_ClubDbNum, m_CharDbNum, m_wLevel);

	/*if (m_bTracingUser)
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);					

		CString strTemp;
		strTemp.Format( "LevelUp!!, [%s][%s], Level[%d]", m_szUID, m_szName, m_wLevel );

		TracingMsg.SetLog(strTemp.GetString());
		SendToAgent(&TracingMsg);
	}*/

    //** Add EventTime

    SEventState sEventState = m_pGaeaServer->m_sEventState;
    if( sEventState.bEventStart )
    {
        if( m_bEventApply == TRUE )
        {
            if( sEventState.MinEventLevel > GETLEVEL() || sEventState.MaxEventLevel < GETLEVEL() )
            {
                m_bEventApply = FALSE;
            }
        }
        else
        {
            if( sEventState.MinEventLevel <= GETLEVEL() && sEventState.MaxEventLevel >= GETLEVEL() )
            {
                CTime	  crtTime     = CTime::GetCurrentTime();
                m_sEventTime.loginTime = crtTime.GetTime();

                GLMSG::SNET_GM_LIMIT_EVENT_RESTART NetMsg;
                NetMsg.dwChaNum = CharDbNum();
                NetMsg.restartTime = crtTime.GetTime();
                SendToAgent(&NetMsg);
                m_bEventApply = TRUE;
            }
        }
    }

    if( !bInitNowLevel )
    {
        if ( m_sExperience.ISOVER() )
        {
            if ( GLHIDESET::CheckHideSet(EMHIDE_LEVELUP) )
           {
                RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);
                RestoreActStateByInvisible();
           } 

            // ���� ����. ( ���� �޽��� �߻�. )
            //
            GLMSG::SNETPC_REQ_LEVELUP NetMsg;
            MsgReqLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

            m_lnLastSendExp = m_sExperience.lnNow;
        }
    }
    m_bVietnamLevelUp = FALSE;
    return S_OK;
}

HRESULT GLChar::ReqLevelUpInvalid() 
{
    if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
    {
       if ( m_wLevel > GLCONST_CHAR::wMAX_EXTREME_LEVEL )
	{
		sc::writeLogWarn(sc::string::format(
            "Level up Failed!!, Account[%1%], ID[%2%], Level %3%, NowExp %4%, MaxExp %5%", 
		m_szUID,
            m_szName,
            m_wLevel,
            m_sExperience.lnNow,
            m_sExperience.lnMax ) );	
		m_sExperience.lnMax = GLOGICEX::GLNEEDEXP2(GLCONST_CHAR::wMAX_EXTREME_LEVEL);
	}
    }
    else
    {
       if ( m_wLevel > GLCONST_CHAR::wMAX_LEVEL )	
      {
		sc::writeLogWarn(sc::string::format(
            "Level up Failed!!, Account[%1%], ID[%2%], Level %3%, NowExp %4%, MaxExp %5%", 
		m_szUID,
            m_szName,
            m_wLevel,
            m_sExperience.lnNow,
            m_sExperience.lnMax) );
		m_sExperience.lnMax = GLOGICEX::GLNEEDEXP(GLCONST_CHAR::wMAX_LEVEL);
	}
    }
    return E_FAIL;
}

HRESULT GLChar::MsgReqJumpingLevelUp (NET_MSG_GENERIC* nmg)
{
	if (m_pGaeaServer->m_bEmptyMsg)
        return S_OK;	

	if (VALID_LEVELUP())
        ReqJumpingLevelUpValid();
    //else
        //ReqLevelUpInvalid();

	 return S_OK;	
}

HRESULT GLChar::MsgReqJumpingLevelUpCA (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_JUMPINGLEVELUP_CA *pNetMsg = (GLMSG::SNETPC_REQ_JUMPINGLEVELUP_CA *)nmg;
	GLMSG::SNETPC_REQ_JUMPINGLEVELUP_FB MsgFB;

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if( !pChar )
		return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;
	
	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_EXP_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )	return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_EXP_CARD )
	{
		MsgFB.emFB = EMINVEN_EXP_FB_ITEMTYPE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// ���� �۾��� �븸���̹Ƿ� ������ 190������ ����
	LONGLONG nExp = GLOGICEX::GLNEEDACCUMULATEDEXP(m_wLevel, 190);
	m_sExperience.lnNow = ( nExp == 0 ) ? m_sExperience.lnNow : nExp;

    // ���� ����. ( ���� �޽��� �߻�. )
    //
    GLMSG::SNETPC_REQ_JUMPINGLEVELUP NetMsg;
    MsgReqJumpingLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

    m_lnLastSendExp = m_sExperience.lnNow;

	//if (VALID_LEVELUP())
 //       ReqJumpingLevelUpValid();
 //   else
 //       ReqLevelUpInvalid();

		// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.emFB = EMINVEN_EXPE_FB_OK;
	SendToClient( &MsgFB );

	 return S_OK;	
}

HRESULT GLChar::MsgReqJumpingLevelUpByExp (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_JUMPING_LEVLEUP_BYEXP *pNetMsg = (GLMSG::SNETPC_JUMPING_LEVLEUP_BYEXP *)nmg;

	if (m_pGaeaServer->m_bEmptyMsg)
        return S_OK;	

	if( pNetMsg->m_nExp >= 0 )
		m_sExperience.lnNow = pNetMsg->m_nExp;

	if (VALID_LEVELUP())
        return ReqJumpingLevelUpValid();
    //else
      //  return ReqLevelUpInvalid();

	return S_OK;	
}

HRESULT GLChar::ReqJumpingLevelUpValid() 
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ReqJumpingLevelUpValid", m_CharDbNum ) );
	}

    bool bInitNowLevel = false;
	bool bJumping = true;

    // ������ ó��
    LEVLEUP(bInitNowLevel, bJumping, true);

	//������ �ϱ� ����Ʈ
	DoQuestLevelUp();

    // ������ ���� ����Ʈ �ڵ� ����
    QuestStartFromGetLEVEL ( m_wLevel );

    // ����Ʈ ���� ���� ���� ����.
    //
    DoQuestLevel ();


    //mjeon.activity
    ActivityCheckLevel();

	//	[�ڽſ���]
	// ������ �����Ҷ� ���� ����ġ %���� ���� %�� ������� ������ �ȵǱ⶧���� ������ ���� �޼����� ����
	GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
	NetMsgExp.lnNowExp = m_sExperience.lnNow;
	NetMsgExp.bJumping = TRUE;
	SendToClient(&NetMsgExp);

    //	[�ڽſ���]
    GLMSG::SNETPC_REQ_LEVELUP_FB NetMsg;
    NetMsg.wLevel		 = m_wLevel;
    NetMsg.wStatsPoint	 = m_wStatsPoint;
    NetMsg.dwSkillPoint  = m_dwSkillPoint;
    NetMsg.bInitNowLevel = bInitNowLevel;
	NetMsg.bJumping = true;
	NetMsg.bCalExp = false;
    NetMsg.sMapID		 = GetCurrentMap().getBaseMapID();
    SendToClient( &NetMsg );

    //	[��ο���]
    GLMSG::SNETPC_REQ_LEVELUP_BRD NetMsgBrd( m_wLevel );
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    SendMsgViewAround(&NetMsgBrd);

	if ( isValidParty() && (GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false) )
    {
		GLMSG::SNET_PARTY_MBR_LEVEL_UP NetPartyMemeberLevelUp(m_dwGaeaID,m_wLevel);		
		SendToParty(&NetPartyMemeberLevelUp);
    }


    //	[��Ƽ,�����] hp ��ȭ �˸�.
    MsgSendUpdateState();


    // ������ �α� ���.(���� 20�̻󿡼�)
		// ��� �� �α� ����
    LogCharAction(
        m_CharDbNum,			//	�����
        gamelog::ACT_JUMPING_LEV_UP,	//	����
        gamelog::ID_CHAR,
        m_wLevel,				//	ĳ���� ����
        m_sExperience.lnNow,	//	exp
        0,						//	bright
        0,						//	life
        0,
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());					//	money

    // Agent �� �˸�
    GLMSG::NET_CHAR_LEVEL_UP_FA MsgFA(m_CharDbNum, m_wLevel);
    SendToAgent(&MsgFA);

    // Ŭ���� LevelUp �ݿ�    
    m_pGaeaServer->ClubMemberLevelUp(m_ClubDbNum, m_CharDbNum, m_wLevel);

	/*if (m_bTracingUser)
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);					

		CString strTemp;
		strTemp.Format( "Jumping LevelUp!!, [%s][%s], Level[%d]", m_szUID, m_szName, m_wLevel );

		TracingMsg.SetLog(strTemp.GetString());
		SendToAgent(&TracingMsg);
	}*/

    //** Add EventTime

    SEventState sEventState = m_pGaeaServer->m_sEventState;
    if( sEventState.bEventStart )
    {
        if( m_bEventApply == TRUE )
        {
            if( sEventState.MinEventLevel > GETLEVEL() || sEventState.MaxEventLevel < GETLEVEL() )
            {
                m_bEventApply = FALSE;
            }
        }
        else
        {
            if( sEventState.MinEventLevel <= GETLEVEL() && sEventState.MaxEventLevel >= GETLEVEL() )
            {
                CTime	  crtTime     = CTime::GetCurrentTime();
                m_sEventTime.loginTime = crtTime.GetTime();

                GLMSG::SNET_GM_LIMIT_EVENT_RESTART NetMsg;
                NetMsg.dwChaNum = CharDbNum();
                NetMsg.restartTime = crtTime.GetTime();
                SendToAgent(&NetMsg);
                m_bEventApply = TRUE;
            }
        }
    }

    if( !bInitNowLevel )
    {
        if ( m_sExperience.ISOVER() )
        {
            if ( GLHIDESET::CheckHideSet(EMHIDE_LEVELUP) )
           {
                RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);
                RestoreActStateByInvisible();
           } 

            // ���� ����. ( ���� �޽��� �߻�. )
            //
            GLMSG::SNETPC_REQ_JUMPINGLEVELUP NetMsg;
            MsgReqJumpingLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

            m_lnLastSendExp = m_sExperience.lnNow;
        }
    }
    m_bVietnamLevelUp = FALSE;

    return S_OK;
}

HRESULT GLChar::MsgReqStatsUp (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_STATSUP *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP *) nmg;
	if ( m_wStatsPoint == 0 )	return S_FALSE;

	//	������ STATE�� ������Ŵ.
	STATSUP(pNetMsg->emStats);
	INIT_DATA(FALSE,FALSE);

	//	ó�� ��� ����.
	GLMSG::SNETPC_REQ_STATSUP_FB NetMsg;
	NetMsg.emStats = pNetMsg->emStats;
	NetMsg.wStatsPoint = m_wStatsPoint;
	SendToClient( &NetMsg );

	//	[��Ƽ,�����] hp ��ȭ �˸�.
	MsgSendUpdateState();

	return S_OK;
}

HRESULT GLChar::MsgReqStatsUpEx(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_STATSUP_EX *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP_EX*) nmg;

	/// 0 ���� ���� ���� ������ �ȵȴ�
	DWORD sum = 0; 
	INT nStatsPoint(m_wStatsPoint);
	for(int i=0; i < EMSIZE; ++i )
	{
		if( pNetMsg->emStatsArray[i] < 0 ) // always false
			return S_FALSE;

		sum += WORD(pNetMsg->emStatsArray[i]); 
		nStatsPoint -= abs(pNetMsg->emStatsArray[i]);
		if ( nStatsPoint < 0 )
		{
			sc::writeLogError(
				sc::string::format(
				"[CRITICAL HACKING] Overstat UserID %1%, ChaDBNum %2%, ChaName %3%", m_dwUserID, m_CharDbNum, m_szName ));
			return S_FALSE;
		}
	}

	 // �䱸 ��ġ�� �ڱ��� ����Ʈ ������ ũ�� �ȵȴ�
	if( sum > m_wStatsPoint )
	{
		sc::writeLogError(
			sc::string::format(
			"[CRITICAL HACKING] Overstat UserID %1%, ChaDBNum %2%, ChaName %3%", m_dwUserID, m_CharDbNum, m_szName ));
		return S_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////

	for(int i = 0; i < EMSIZE; ++i)
	{
		for(int k=0; k < pNetMsg->emStatsArray[i]; ++k)
			STATSUP(EMSTATS(i));
	}

	INIT_DATA(FALSE,FALSE);
	//	������ STATE�� ������Ŵ.
//	STATSUP(pNetMsg->emStats);

	//	ó�� ��� ����.
	GLMSG::SNETPC_REQ_STATSUP_EX_FB NetMsg;
	std::copy(pNetMsg->emStatsArray,pNetMsg->emStatsArray+EMSIZE,NetMsg.emStatsArray);
	NetMsg.wStatsPoint = m_wStatsPoint;
	SendToClient( &NetMsg );

	//	[��Ƽ,�����] hp ��ȭ �˸�.
	MsgSendUpdateState();

	return S_OK;
}


HRESULT GLChar::MsgReqActionQSet (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_ACTIONQUICK_SET *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_SET *) nmg;

	WORD wSLOT = pNetMsg->wSLOT;
	if ( EMACTIONQUICK_SIZE <= wSLOT )
        return E_FAIL;

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );

	if ( !pHoldInvenItem )
        return S_FALSE;

	const SITEMCUSTOM& sITEM = pHoldInvenItem->CustomItem();

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( sITEM.bVietnamGainItem )
			return E_FAIL;
	}
//#endif

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( !pITEM )
		return S_FALSE;

	if ( pITEM->sBasicOp.emItemType != ITEM_EFFECT_RELEASE_CURE &&  
		 pITEM->sBasicOp.emItemType != ITEM_CURE && 
		 pITEM->sBasicOp.emItemType != ITEM_RECALL &&
		 pITEM->sBasicOp.emItemType != ITEM_TELEPORT_CARD &&
		 pITEM->sBasicOp.emItemType != ITEM_PET_CARD )
		return S_FALSE;

	// ���� �ൿ ���� ����.
	//
	m_sACTIONQUICK[wSLOT].wACT = pNetMsg->wACT;
	m_sACTIONQUICK[wSLOT].sNID = sITEM.GetNativeID();

	// Ŭ���̾�Ʈ�� ����.
	//
	GLMSG::SNETPC_REQ_ACTIONQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = wSLOT;
	NetMsgFB.sACT = m_sACTIONQUICK[wSLOT];
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgReqActionQReSet (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_ACTIONQUICK_RESET *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_RESET *) nmg;

	WORD wSLOT = pNetMsg->wSLOT;
	if ( EMACTIONQUICK_SIZE <= wSLOT )		return E_FAIL;

	m_sACTIONQUICK[wSLOT].RESET ();

	// Ŭ���̾�Ʈ�� ����.
	//
	GLMSG::SNETPC_REQ_ACTIONQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = wSLOT;
	NetMsgFB.sACT = m_sACTIONQUICK[wSLOT];
	SendToClient( &NetMsgFB );

	return S_OK;
}

// *****************************************************
// Desc: ��Ȱ��ġ��û ó��
// *****************************************************
HRESULT GLChar::MsgReqReGenGate (NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_REGEN_GATE *pNetMsg = (GLMSG::SNETPC_REQ_REGEN_GATE *)nmg;

	GLMSG::SNETPC_REQ_REGEN_GATE_FB	MsgFB(EMREGEN_GATE_FAIL, SNATIVEID(), 0);
	MsgFB.dwChaNum = CharDbNum();
	
	PGLCROW pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pCrow )
    {
        goto _REQ_FAIL;
    }

    if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_STARTPOINT ) )
    {
        goto _REQ_FAIL;
    }

	GLMobSchedule* pMOBSCH = pCrow->GetMobSchedule();
	if ( !pMOBSCH )
		goto _REQ_FAIL;

	if ( pMOBSCH->m_dwPC_REGEN_GATEID == UINT_MAX )
		goto _REQ_FAIL;

	DxLandGateMan* pLandGateMan = pLand->GetLandGateMan();
	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( pMOBSCH->m_dwPC_REGEN_GATEID );
	if ( !pLandGate )
		goto _REQ_FAIL;

	if ( !(pLandGate->GetFlags()&DxLandGate::GATE_IN) )
		goto _REQ_FAIL;

	// ��Ȱ ��ġ ����.
	//
	m_sStartMapID = GetCurrentMap().getBaseMapID();
	m_dwStartGate = pLandGate->GetGateID();

	// ��Ȱ ��ġ ������ �����Ͽ����� �˸�.
	//
	MsgFB.emFB     = EMREGEN_GATE_SUCCEED;
	MsgFB.sMapID   = m_sStartMapID;
	MsgFB.dwGateID = m_dwStartGate;
	SendToAgent(&MsgFB);

	return S_OK;

_REQ_FAIL:
	// ��Ȱ ��ġ ������ �����Ͽ����� �˸�.
	//
	SendToClient( &MsgFB );
	return E_FAIL;
}

HRESULT GLChar::MsgReqCure (NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_CURE *pNetMsg = (GLMSG::SNETPC_REQ_CURE *)nmg;

	GLMSG::SNETPC_REQ_CURE_FB MsgFB(EMREGEN_CURE_FAIL, NULL, m_sHP, m_sMP, m_sSP );

	GLCrow* pCrow = pLand->GetCrow(pNetMsg->dwNpcGlobID);	
	if ( !pCrow )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

    if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_CURE ) )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	// ġ�ᰡ���� NPC���� �˻� �ʿ�.
	bool bCURE = pCrow->IsBasicTalk(SNpcTalk::EM_CURE);
	if (!bCURE)
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	// ġ�ᰡ���� �������� üũ
	std::tr1::shared_ptr<SNpcTalk> spTalk = pCrow->GetTalk(pNetMsg->dwTalkID);
	if (!spTalk)
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

    GLCHARLOGIC* pCharLogic = dynamic_cast<GLCHARLOGIC*>(this);
	if ( pCharLogic )
	{
        DWORD PartyNum = 0;
        GLPartyField* const pParty = GetMyParty();
        if ( pParty )
            PartyNum = pParty->GetNMember();

		CTime cCurSvrTime = CTime::GetCurrentTime();

		if ( !spTalk->DoTEST(pCharLogic, PartyNum, cCurSvrTime.GetHour()) )
		{
            SendToClient( &MsgFB );
            return S_OK;
		}
	}

	// �Ÿ� ���� �׽�Ʈ
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPosition-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if (fDist > fTalkableDis)
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	// ġ���Ŵ
	m_sHP.TO_FULL();
    m_sMP.TO_FULL();
    m_sSP.TO_FULL();
	CURE_STATEBLOW ( DIS_ALL );

	// ��� ����
	MsgFB.emFB = EMREGEN_CURE_SUCCEED;
	MsgFB.sHP = m_sHP;
    MsgFB.sMP = m_sMP;
    MsgFB.sSP = m_sSP;
	MsgFB.dwCUREFLAG = DIS_ALL;

	SendToClient( &MsgFB );
	return S_OK;
// 
// _REQ_FAIL:
// 	SendToClient( &MsgFB );
// 	return S_OK;
}

HRESULT GLChar::MsgReqCharReset (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_CHARRESET *pNetMsg = (GLMSG::SNETPC_REQ_CHARRESET *)nmg;

	GLMSG::SNETPC_REQ_CHARRESET_FB	MsgFB(EMREGEN_CHARRESET_FAIL, 0, 0);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		goto _REQ_FAIL;

	PGLCROW pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );

	if ( !pCrow )
        goto _REQ_FAIL;

    if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), SNpcTalk::EM_CHAR_RESET ) )
    {
        goto _REQ_FAIL;
    }

	// ġ�ᰡ���� NPC���� �˻� �ʿ�
	bool bRESET = pCrow->IsBasicTalk(SNpcTalk::EM_CHAR_RESET);
	if (!bRESET)
        goto _REQ_FAIL;

	// char reset ������ ��.
	//
	WORD wPosX(0), wPosY(0);
	bool bITEM = m_cInventory.GetCharResetItem ( wPosX, wPosY );
	if ( !bITEM )
	{
		MsgFB.emFB = EMREGEN_CHARRESET_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );
	
	// ���� ����. ( stats, skill )
	RESET_STATS_SKILL();

	// ���� SKILL ������̶�� ��� ����.
	//
	if ( isAction(GLAT_SKILL) )
	{
		GLMSG::SNETPC_SKILL_CANCEL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround(&NetMsgBrd);

		TurnAction(GLAT_IDLE);
	}

	// ��� ����.
	//
	MsgFB.emFB = EMREGEN_CHARRESET_SUCCEED;
	MsgFB.dwSKILL_P = m_dwSkillPoint;
	MsgFB.dwSTATS_P = m_wStatsPoint;

	SendToClient( &MsgFB );

	return S_OK;

_REQ_FAIL:

	// ���� ���� �����Ͽ����� �˸�.
	//
	SendToClient( &MsgFB );

	return S_OK;
}

void GLChar::DoConftStateBackup ()
{
	// ü�� ������ ��ȭ.
	//
	m_sCONFTING.sBACKUP_HP = m_sHP;
	m_sCONFTING.sBACKUP_MP = m_sMP;
	m_sCONFTING.sBACKUP_SP = m_sSP;

	if ( !m_mapPlayHostile.empty() )
	{
		m_sCONFTING.sOption.fHP_RATE = 1.0f;
	}

	// ü���� �ִ� ��ġ ���.
	UPDATE_MAX_POINT ( m_sCONFTING.sOption.fHP_RATE );

	// ��ÿ� ���� ���� ����.
	m_sHP.TO_FULL();
	m_sMP.TO_FULL();
	m_sSP.TO_FULL();

	// ü�� ��ȭ�� �˸�. [�ڽ�,��Ƽ,�����]
	//
	MsgSendUpdateState();
}

void GLChar::DoConftStateRestore ()
{
	if ( !m_sCONFTING.IsCONFRONTING() )		return;

	// ü�� ȯ��.
	//
	m_sHP = m_sCONFTING.sBACKUP_HP;
	m_sMP = m_sCONFTING.sBACKUP_MP;
	m_sSP = m_sCONFTING.sBACKUP_SP;

	// ü���� �ִ� ��ġ ���.
	UPDATE_MAX_POINT ( 1.0f, false );

	m_sHP.LIMIT();
	m_sMP.LIMIT();
	m_sSP.LIMIT();

	// ü�� ��ȭ�� �˸�. [�ڽ�,��Ƽ,�����]
	//
	MsgSendUpdateState();

	// �ൿ�� ����Ѵ�.
	//
	TurnAction(GLAT_IDLE);
}

HRESULT GLChar::MsgReqConFrontStart (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_CONFRONT_START2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONT_START2_FLD *)nmg;
	
	m_sCONFTING.RESET();

	m_cDamageLog.clear ();

	GLChar* pTAR = GetChar(pNetMsg->dwTARID);
	if ( !pTAR )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, m_dwGaeaID, EMCONFRONT_FAIL, 0.0f);
		NetMsgFB.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgFB);
		return E_FAIL;
	}

	// ������ ���� ����
	if( pTAR->IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, m_dwGaeaID, EMCONFRONT_DIE_YOU, 0.0f);
		NetMsgFB.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgFB);
		return E_FAIL;
	}

	// ���� ���� ����
	if( IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, m_dwGaeaID, EMCONFRONT_DIE_ME, 0.0f);
		NetMsgFB.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgFB);
		return E_FAIL;
	}

	D3DXVECTOR3 vPosition = ( m_vPosition + pTAR->m_vPosition ) / 2.0f;

	// ���������� ����.
	//
	RemoveAllPlayHostiles();

	// ��� ���� ����.
	//
	m_sCONFTING.emTYPE = pNetMsg->emTYPE;
	m_sCONFTING.dwTAR_ID = pNetMsg->dwTARID;
	m_sCONFTING.vPosition = vPosition;
	m_sCONFTING.sOption = pNetMsg->sOption;

	// ��� ���� Ŭ���̾�Ʈ�� �˸�.
	GLMSG::SNETPC_CONFRONT_START2_CLT NetMsgClt(pNetMsg->emTYPE, pNetMsg->dwTARID, vPosition, pNetMsg->sOption, pTAR->GetName() );
	SendToClient(&NetMsgClt);


	// ��� ���۽� ��ȯ�� ����
	//SummonAllRemove();
	KillAllChildCrow();

	// ü�� ���.
	//
	DoConftStateBackup();

	if ( GLHIDESET::CheckHideSet(EMHIDE_BEING_SELF) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqConFrontEnd (NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pGaeaServer->IsField());

	GLMSG::SNETPC_CONFRONT_END2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_FLD *)nmg;

	EMCONFRONT_END emEND = pNetMsg->emEND;

    // Ŭ���̾�Ʈ�� ��� ��Ҹ� �뺸.
	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgClt(emEND);
	SendToClient(&NetMsgClt);

	// ��� ���� �޽���.
	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd(emEND);
	NetMsgTarBrd.dwGaeaID = m_dwGaeaID;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			//	��Ƽ�� �ִ� '���' ���������� ����.
			GLPartyField* const pParty = GetMyParty();
			if ( pParty )
			{
				//	��� ����Ʈ���� ����.
				pParty->MBR_CONFRONT_LEAVE(m_dwGaeaID);

				//	��Ƽ������ ��ÿ��� ���� ������ �˸�.
				SendToParty(&NetMsgTarBrd);
			}
		}
		break;

	case EMCONFT_GUILD:
		{			
			std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
			if ( pCLUB )
			{
				pCLUB->DELCONFT_MEMBER ( m_CharDbNum );

				//	Ŭ�������� ��ÿ��� ���������� �˸�.
				SendToClub(&NetMsgTarBrd);
			}
		}
		break;
	};

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			GLPartyField* const pParty = GetMyParty();
			if( !pParty )
				return E_FAIL;

			GLChar* const pMaster = GetChar(pParty->GetMasterGaeaID());
			if ( pMaster == NULL )
				return E_FAIL;

			//  ��� ���� ȿ�� ó��
            EndConfting();
            
			if( m_mapID == pMaster->GetCurrentMap() )
			{	
				// �ֺ� ���ֵ鿡 ������� �뺸.
				if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
				{
					SendMsgViewAround(&NetMsgTarBrd);
				}
				// ��� ����, ���� Ÿ�� ���� (�ڽ� & ��)
				m_sCONFTING.SETPOWERFULTIME ( 10.0f );
			}

			//
			//mjeon.attendance
			//
			//AttendanceCheckSparingParty();
		}
		break;
	case EMCONFT_GUILD:
		{			
			std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
			DWORD dwMasterID = pCLUB->MasterCharDbNum();
			GLChar* pMASTER = GetCharByDbNum(dwMasterID);

			//  ��� ���� ȿ�� ó��
            EndConfting();
           
			if( m_mapID == pMASTER->GetCurrentMap() )
			{
				// �ֺ� ���ֵ鿡 ������� �뺸.
				if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
				{
					SendMsgViewAround(&NetMsgTarBrd);
				}

				m_sCONFTING.SETPOWERFULTIME ( 10.0f );
			}
		}
		break;
	default:
		{
			// �ֺ� ���ֵ鿡 ������� �뺸.
			if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
			{
				SendMsgViewAround(&NetMsgTarBrd);
			}

			//  ��� ���� ȿ�� ó��
            EndConfting();
            
			m_sCONFTING.SETPOWERFULTIME ( 10.0f );

			//
			//mjeon.attendance
			//
			//AttendanceCheckSparingSolo();
		}
		break;
	}
	// �ൿ�� ����Ѵ�.
	TurnAction ( GLAT_IDLE );

	return S_OK;
}

HRESULT GLChar::MsgActState (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_ACTSTATE *pNetMsg = (GLMSG::SNETPC_ACTSTATE *)nmg;

	const DWORD dwOldActState = GetActState();

	// ��Ʋ�ξ� �������� ���������� Visible ��� ���������ϵ��� üũ�׸� �߰�.

	if ( m_dwUserLvl >= USER_USER_GM || GLCONST_CHAR::bBATTLEROYAL )
	{
		if ( pNetMsg->dwActState & EM_REQ_VISIBLENONE )
			SetActState ( EM_REQ_VISIBLENONE );
		else
			ReSetActState ( EM_REQ_VISIBLENONE );
		
		if ( pNetMsg->dwActState & EM_REQ_VISIBLEOFF )
			SetActState ( EM_REQ_VISIBLEOFF );
		else
			ReSetActState ( EM_REQ_VISIBLEOFF );
	}

	if ( pNetMsg->dwActState & EM_ACT_PEACEMODE )
		SetActState ( EM_ACT_PEACEMODE );
	else
		ReSetActState ( EM_ACT_PEACEMODE );

	const DWORD& dwCurActState = GetActState();
	if ( dwOldActState != dwCurActState || GLCONST_CHAR::bBATTLEROYAL )
	{
		GLMSG::SNETPC_MOVESTATE_BRD NetMsgBrd(m_dwGaeaID, dwCurActState);		
		SendMsgViewAround(&NetMsgBrd);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqGesture (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_GESTURE *pNetMsg = (GLMSG::SNETPC_REQ_GESTURE *)nmg;

    GLLandMan* pLand = GetLandMan();
    if ( pLand == NULL )
        return S_FALSE;

    // �������¿����� ���Ұ�;
    if ( !pLand->IsPeaceZone() && !IsActState(EM_ACT_PEACEMODE) )
    {
        return S_FALSE;
    }
    // Ż�� ž�����϶� �Ұ�; // ����������� ��� �Ұ�; // ��� ���°� �ƴҰ�� �Ұ�;
    if ( ( m_emVehicle ) || ( !IsValidBody() ) || ( isAction( GLAT_IDLE ) == false ) )
    {
        return S_FALSE;
    }

	// ������ �׼�
	TurnAction ( GLAT_TALK );
	m_dwANISUBSELECT = pNetMsg->dwID;

	// ������ ������ ��� �ֺ� ����鿡�� ����.
	GLMSG::SNETPC_REQ_GESTURE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = pNetMsg->dwID;

	SendMsgViewAround(&NetMsgBrd);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// ��������
// 2005-12-27 Jgkim
// �߱�, ���������ƿ��� NpcTalk ������ �����Ͽ� �ҹ����������� ��ȯ����	
//
// pNetMsg->dwA_NID; // A:npc���� �� ������
// pNetMsg->dwB_NID; // b:npc���� ���� ������
// pNetMsg->dwNpcID; // ��û�� �޴� NPC ID
//
// 1. ��û�� NPC �� �ִ��� Ȯ��
// 2. A �������� ĳ���Ͱ� ������ �ִ��� Ȯ��
// 3. NPC �� B �������� �� �� �ִ��� Ȯ��
// 4. NPC �� B �������� �� �� �ִٸ�, ĳ������ A �������� �Ҹ��� B �������� ����
///////////////////////////////////////////////////////////////////////////
// PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pNetMsg->dwNpcID);
// pCrowData->m_sNpcTalkDlg
///////////////////////////////////////////////////////////////////////////
HRESULT GLChar::MsgReqNpcItemTrade( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_NPC_ITEM_TRADE *pNetMsg = ( GLMSG::SNETPC_REQ_NPC_ITEM_TRADE* ) nmg;

	GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsgUpdate;
	NetMsgUpdate.dwChaNum = CharDbNum();

	GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB	MsgFB( EMNPC_ITEM_TRADE_FAIL );

	CTime cTIME;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		SendToClient( &MsgFB );
		return S_OK;
	}

	GLCrow* pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pCrow )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

    if ( !pCrow->IsNpcInteractionAble( m_vPosition, GETBODYRADIUS(), SNpcTalk::EM_ITEM_TRADE ) )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	if ( !pCrow->IsBasicTalk( SNpcTalk::EM_ITEM_TRADE ) )	
    {
        SendToClient( &MsgFB );
        return S_OK;
    }
	
	std::tr1::shared_ptr< SNpcTalk > spTalk = pCrow->GetTalk( pNetMsg->dwTalkID );
	if ( !spTalk )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }
     
	if ( !spTalk->IsUscCondition() )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	if ( spTalk->m_dwACTION_NO != SNpcTalk::EM_ITEM_TRADE )
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	if ( !spTalk->m_vecNeedItem.size() )
	{
		MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
        SendToClient( &MsgFB );
        return S_OK;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqNpcItemTrade ", m_CharDbNum ) );
	}

	SNpcTalk::VEC_ITEM_NUM& vecNeedItem = spTalk->m_vecNeedItem;
	SNpcTalk::VEC_ITEM_NUM& vecTradeItem = spTalk->m_vecTradeItem;

	// Need Add New Item Con.
	BOOL bOK[MAX_NEEDITEM_COUNT] = { TRUE, TRUE, TRUE, TRUE, TRUE };

	int NeedItemSize =  static_cast< int >( vecNeedItem.size() );
	for ( int i = 0; i < NeedItemSize; ++i ) 
	{
		bOK[i] = ISHAVEITEM ( vecNeedItem[i].sNativeID, vecNeedItem[i].dwNum );
	}

	for ( int i = 0; i < MAX_NEEDITEM_COUNT; ++i ) 
	{
		if ( !bOK[i] ) 
		{
			MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
            SendToClient( &MsgFB );
            return S_OK;
		}
	}

	//	������� �����ϰ� ������ �ִ��� üũ�Ѵ�.
	//	���� ����� ��������� �ѹ��� üũ�ϴ°� ������ ����.
	GLInventory cInventory;
	cInventory.Assign( m_cInventory );

	//	�ϴ� �ʿ��Ѿ������� ã�Ƽ� �κ��丮���� ���� �Ѵ�.
	for ( int i = 0; i < NeedItemSize; ++i ) 
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( vecNeedItem[i].sNativeID );
		if ( !pItem )
        {
            SendToClient( &MsgFB );
            return S_OK;
        }

		int nNum = vecNeedItem[i].dwNum * pItem->GETAPPLYNUM();
		bool bCheck = true;
		while ( bCheck )
		{
			SINVENITEM* pInvenItem = cInventory.FindItem ( vecNeedItem[i].sNativeID );
			if ( !pInvenItem )
			{
				MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
                SendToClient( &MsgFB );
                return S_OK;
			}

			if ( nNum < pInvenItem->sItemCustom.wTurnNum ) 
			{
				pInvenItem->sItemCustom.wTurnNum -= nNum;
				nNum = 0;
			}
			else
			{
				nNum -= pInvenItem->sItemCustom.wTurnNum;
				cInventory.DeleteItem( pInvenItem->wPosX, pInvenItem->wPosY );
			}

			if ( nNum <= 0 )
                bCheck = false;
		}
	}

	//	���� �������� �־��. �κ��丮�� ������...
	int TradeItemSize =  static_cast< int >( vecTradeItem.size() );
	for ( int i = 0; i < TradeItemSize; ++i ) 
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( vecTradeItem[i].sNativeID );
		if ( !pItem )
        {
            SendToClient( &MsgFB );
            return S_OK;
        }
		
		if ( pItem->ISPILE() ) 
		{
			//	��ħ �������� ���.
			SITEMCUSTOM sITEM_NEW( pItem->sBasicOp.sNativeID );
			sITEM_NEW.wTurnNum = static_cast< WORD >( vecTradeItem[i].dwNum * pItem->GETAPPLYNUM() );	

			//	�κ��� ���������� ���� ��� ��ҵ�.
			if ( !cInventory.InsertItemEx( sITEM_NEW ) )
			{
				MsgFB.emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
                SendToClient( &MsgFB );
                return S_OK;
			}
		}
		else
		{
			for ( int j = 0; j < static_cast< int >( vecTradeItem[i].dwNum ); ++j ) 
			{
				SITEMCUSTOM sITEM_NEW( pItem->sBasicOp.sNativeID ); 
				if ( !cInventory.InsertItem( sITEM_NEW ) ) 
				{
					MsgFB.emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
                    SendToClient( &MsgFB );
                    return S_OK;
				}				
			}			
		}
	}

	//	������ �Ϸ�Ǿ���. ���۾��� �����ؾ� �Ѵ�.
	//	�ϴ� �ʿ��Ѿ������� ã�Ƽ� �κ��丮���� ���� �Ѵ�.
	for ( int i = 0; i < NeedItemSize; ++i ) 
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( vecNeedItem[i].sNativeID );
		if ( !pItem )
        {
            SendToClient( &MsgFB );
            return S_OK;
        }

		int nNum = vecNeedItem[i].dwNum * pItem->GETAPPLYNUM();
		bool bCheck = true;
		while ( bCheck )
		{
			SINVENITEM* pInvenItem = m_cInventory.FindItem ( vecNeedItem[i].sNativeID );
			if ( !pInvenItem )
			{
				//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.				
				sc::writeLogError( std::string( "NPC ITEM TRADE Critical Error" ) );
				MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
                SendToClient( &MsgFB );
                return S_OK;
			}

			if ( nNum < pInvenItem->sItemCustom.wTurnNum ) 
			{
				pInvenItem->sItemCustom.wTurnNum -= nNum;
				nNum = 0;

				// Ŭ���̾�Ʈ�� ���� ������ �˸�.
				NetMsgUpdate.wPosX = pInvenItem->wPosX;
				NetMsgUpdate.wPosY = pInvenItem->wPosY;
				NetMsgUpdate.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
				SendToClient( &NetMsgUpdate );
			}
			else
			{
				nNum -= pInvenItem->sItemCustom.wTurnNum;

				// �������� ���� ���� ��� ���.				
				// ��� �� �α� ����
				LogItemExchange(
                    pInvenItem->sItemCustom,
                    gamelog::ID_CHAR, m_CharDbNum,
                    gamelog::ID_CHAR, 0,
                    ITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_NPC_TRADE,
					false );
				
                InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 8009, true );
			}

			if ( nNum <= 0 )
                bCheck = false;
		}
	}

	cTIME = CTime::GetCurrentTime();
	__time64_t curTime = cTIME.GetTime();
	int nChannel = GetServerChannel();
	int nFieldID = GetFieldSvrID();

	// ��������� �ִ´�.
	for ( int i = 0; i < TradeItemSize; ++i ) 
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( vecTradeItem[i].sNativeID );
		if ( !pItem )
        {
            SendToClient( &MsgFB );
            return S_OK;
        }

        SITEMCUSTOM sITEM_NEW( vecTradeItem[i].sNativeID );
		sITEM_NEW.tBORNTIME = curTime;
		sITEM_NEW.cGenType = EMGEN_NPC;

		if ( pItem->ISPILE() ) 
		{
			SITEMCUSTOM sITEM_NEW2 = sITEM_NEW;			
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW2.guid );
			sITEM_NEW2.GenerateBasicStat( false );
			sITEM_NEW2.GenerateLinkSkill();
			sITEM_NEW2.GenerateAddOption();

			LogItemBasicStatOption( sITEM_NEW2 );
			LogItemLinkSkillOption( sITEM_NEW2 );
			LogItemAddonOption( sITEM_NEW2 );

			sITEM_NEW2.wTurnNum = static_cast< WORD >( vecTradeItem[i].dwNum * pItem->GETAPPLYNUM() );

			// #item
			// ��ħ���� �����ۿ� �����ɼ��� �־��ְ� �ִ�. �������� �ʵ��� ��
			//if ( sITEM_NEW2.GENERATE_RANDOM_OPT( false ) )
			//	LogItemRandomOption( sITEM_NEW2 );

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW2;

			//	�κ��� ���������� ���� ��� ��ҵ�.
			if ( !InsertToInven ( &cDropItem, true, true, false, false, true ) )
			{
				//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.
				sc::writeLogError( sc::string::format( "NPC ITEM TRADE Critical Error %hd/%hd",
									sITEM_NEW2.GetNativeID().wMainID, sITEM_NEW2.GetNativeID().wSubID ) );
				MsgFB.emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
                SendToClient( &MsgFB );
                return S_OK;
			}
		}
		else
		{            
			for ( int j = 0; j < static_cast< int >( vecTradeItem[i].dwNum ); ++j ) 
			{
				//	���� �ɼǶ����� �����Ѵ�.
				SITEMCUSTOM sITEM_NEW2 = sITEM_NEW;
				sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW2.guid );
				sITEM_NEW2.GenerateBasicStat( false );
				sITEM_NEW2.GenerateLinkSkill();
				sITEM_NEW2.GenerateAddOption();

				LogItemBasicStatOption( sITEM_NEW2 );
				LogItemLinkSkillOption( sITEM_NEW2 );
				LogItemAddonOption( sITEM_NEW2 );

				sITEM_NEW2.wTurnNum = 1;

				if ( sITEM_NEW2.GENERATE_RANDOM_OPT( false ) )
                    LogItemRandomOption( sITEM_NEW2 );

				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW2;

				if ( !InsertToInven ( &cDropItem, true, true, false, false, true ) ) 
				{
					sc::writeLogError( sc::string::format( "NPC ITEM TRADE Critical Error %hd/%hd",
												sITEM_NEW2.GetNativeID().wMainID, sITEM_NEW2.GetNativeID().wSubID ) );
					MsgFB.emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
                    SendToClient( &MsgFB );
                    return S_OK;
				}				
			}			
		}
	}

	MsgFB.emFB = EMNPC_ITEM_TRADE_SUCCEED;
	SendToClient( &MsgFB );

	return S_OK;
}
/*
HRESULT GLChar::MsgReqPhoneNumber(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PHONE_NUMBER *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER*) nmg;

	// DB�� �����û
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterPhoneNumberSet(m_ClientSlot, m_CharDbNum, pNetMsg->szPhoneNumber)));    
	return S_OK;
}
*/
/*
// ����ȣ ���� ��� ó��
HRESULT GLChar::MsgPhoneNumber(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *)nmg;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	GLMSG::SNETPC_PHONE_NUMBER_FB MsgFB;

	if ( pNetMsg->emFB == EMSMS_PHONE_NUMBER_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMSMS_PHONE_NUMBER_FROM_DB_FAIL;
		StringCchCopy ( MsgFB.szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
		SendToClient( &MsgFB );
		return S_FALSE;
	}

	StringCchCopy ( MsgFB.szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
	MsgFB.emFB = EMSMS_PHONE_NUMBER_FB_OK;
	SendToClient( &MsgFB );

	// ����ȣ ������ ģ���ɹ��鿡�� �˸�
	GLMSG::SNETPC_PHONE_NUMBER_AGTBRD NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();
	NetMsgAgt.dwID = m_dwGaeaID;
	StringCchCopy ( NetMsgAgt.szName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgAgt.szNewPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
	SendToAgent(&NetMsgAgt);

	m_pGLGaeaServer->ChangeNameMap( this, pNetMsg->szPhoneNumber );

	return S_OK;
}
*/
HRESULT	GLChar::MsgPetReviveInfo (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_PETREVIVEINFO* pNetMsg = ( GLMSG::SNETPET_REQ_PETREVIVEINFO* ) nmg;
	AddGameAdoJob(
        db::DbActionPtr(
            new db::PetListRestoreGet(m_CharDbNum, m_ClientSlot)));	
	return S_OK;
}

HRESULT GLChar::MsgReqGetRightOfItem (NET_MSG_GENERIC* nmg)
{
	return S_OK;
}


void GLChar::ActiveVehicle(bool bActive, bool bMoveMap)
{    
	if (bActive)
    {
        ActiveVehicleTrue(); // Ȱ��ȭ	
		if ( GLHIDESET::CheckHideSet(EMHIDE_GETON_VEHICLE) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}
    else 
    {
        ActiveVehicleFalse(bMoveMap); // ��Ȱ��ȭ
		if ( GLHIDESET::CheckHideSet(EMHIDE_GETOFF_VEHICLE) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}
}


void GLChar::ActiveVehicleTrue() 
{
    GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC NetMsgFB;

    if (m_emVehicle != EMVEHICLE_OFF)
    {
        // sc::writeLogError ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
        NetMsgFB.bActive = true;
        NetMsgFB.emFB = EMVEHICLE_SET_FB_RESET;
        SendToClient( &NetMsgFB );
        return;
    }

	if (m_emVehicle != EMVEHICLE_PASSENGER)
		m_emVehicle = EMVEHICLE_DRIVER;

    int emType = m_pVehicle->m_emTYPE;	
    m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;

    SetActState(EM_ACT_PEACEMODE);

    // Ż�� Ż�� ��ȯ�� ����
    //SummonAllRemove();
	KillAllChildCrow();

    GLCHARLOGIC::INIT_DATA( FALSE, FALSE );
    ReSelectAnimation();

    m_pVehicle->PassengerIdSet(0, m_dwGaeaID);

    m_pVehicle->BoostReset();

    NetMsgFB.bActive = true;
    NetMsgFB.emFB = EMVEHICLE_SET_FB_OK;
    NetMsgFB.dwClientID = m_dwGaeaID;
    SendToClient( &NetMsgFB );

    // �ֺ��� �˸�
    GLMSG::SNETPC_ACTIVE_VEHICLE_BRD NetMsgBRD;

    NetMsgBRD.bActive = true;
    NetMsgBRD.dwGaeaID = m_dwGaeaID;
    for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
    {
        NetMsgBRD.dwPassengerID[i] = m_pVehicle->PassengerId(i);
    }
    SendMsgViewAround( &NetMsgBRD );
}

void GLChar::ActiveVehicleFalse(bool bMoveMap) 
{
    GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC NetMsgFB;

    if (m_emVehicle == EMVEHICLE_OFF) 
    {
        // sc::writeLogError ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
        NetMsgFB.bActive = false;
        NetMsgFB.emFB = EMVEHICLE_SET_FB_RESET;
        SendToClient( &NetMsgFB );
        return;
    }
    
    EMSLOT emRHand = GetCurRHand();
    EMSLOT emLHand = GetCurLHand();
    m_emANISUBTYPE = CHECK_ANISUB( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );   
    
	if ( m_emVehicle == EMVEHICLE_DRIVER )
	{
		for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			const DWORD dwGaeaID = m_pVehicle->PassengerId(i);
			if ( dwGaeaID == m_dwGaeaID )
				continue;

			GLChar* const pPassenger = m_pGaeaServer->GetChar(dwGaeaID);
			if ( pPassenger != NULL )
				pPassenger->ActiveVehicleFalse(bMoveMap);

			m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
		}
	}
	else
	{
		for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
			m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
	}
	m_emVehicle = EMVEHICLE_OFF;	
	GLCHARLOGIC::INIT_DATA(FALSE, FALSE);
	ReSelectAnimation();

    m_pVehicle->BoostReset();

    NetMsgFB.bActive = false;
    NetMsgFB.emFB = EMVEHICLE_SET_FB_OK;
    NetMsgFB.bLeaveFieldServer = bMoveMap;
    SendToClient( &NetMsgFB );

    // �ֺ��� �˸��� �����ڸ� ��;
	if ( m_emVehicle != EMVEHICLE_PASSENGER )
	{
		GLMSG::SNETPC_ACTIVE_VEHICLE_BRD NetMsgBRD;

		NetMsgBRD.bActive = false;
		NetMsgBRD.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround( &NetMsgBRD );
	}
}

HRESULT GLChar::ActiveWithVehicle ( bool bActive, bool bMoveMap )
{
	// Ȱ��ȭ
	if ( bActive )
	{
		GLMSG::SNET_ACTIVE_WITH_VEHICLE_FB NetMsgFB;

		if ( m_emVehicle ) 
		{
			// sc::writeLogError ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
			return false;
		}

		m_pVehicle->SetActiveValue( true );
		m_emVehicle = EMVEHICLE_PASSENGER;
		int emType = m_pVehicle->m_emTYPE ;	
		m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;

		SetActState ( EM_ACT_PEACEMODE );

		// Ż�� Ż�� ��ȯ�� ����
		//SummonAllRemove();
		KillAllChildCrow();

		GLCHARLOGIC::INIT_DATA( FALSE, FALSE );
		ReSelectAnimation();

		for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			NetMsgFB.dwPassengerID[i] = m_pVehicle->PassengerId(i);
		}

		NetMsgFB.bActive = true;
		SendToClient( &NetMsgFB );



		// �ֺ��� �˸�
		GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD NetMsgBRD;

		NetMsgBRD.bActive = true;
		NetMsgBRD.dwGaeaID = m_dwGaeaID;
		NetMsgBRD.emVehicle = m_emVehicle;
		for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			NetMsgBRD.dwPassengerID[ i ] = m_pVehicle->PassengerId(i);
		}
		SendMsgViewAround( &NetMsgBRD );


	}
	// ��Ȱ��ȭ
	else 
	{
		GLMSG::SNET_ACTIVE_WITH_VEHICLE_FB NetMsgFB;

		if ( !m_emVehicle ) 
		{
			// sc::writeLogError ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
			return false;
		}

		SITEMCUSTOM sCUSTOM = m_PutOnItems[SLOT_VEHICLE];
		if ( sCUSTOM.GetNativeID() == NATIVEID_NULL() )
		{
			m_pVehicle->SetActiveValue( false );
		}
		else
		{
			m_pVehicle->SetActiveValue( true );
		}
		
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();
		m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );		

		if ( m_emVehicle == EMVEHICLE_DRIVER )
		{
			for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
			{
				const DWORD dwGaeaID = m_pVehicle->PassengerId(i);
				if ( dwGaeaID == m_dwGaeaID )
					continue;

				GLChar* const pPassenger = m_pGaeaServer->GetChar(dwGaeaID);
				if ( pPassenger != NULL )
					pPassenger->ActiveWithVehicle(false, false);
				m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
			}
		}
		else
		{
			for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
				m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
		}
		m_emVehicle = EMVEHICLE_OFF;	
		GLCHARLOGIC::INIT_DATA( FALSE, FALSE );
		ReSelectAnimation();

		NetMsgFB.bActive = false;
		SendToClient( &NetMsgFB );

		// �ֺ��� �˸��� �����ڸ� ��;
		if ( m_emVehicle != EMVEHICLE_PASSENGER )
		{
			GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD NetMsgBRD;

			NetMsgBRD.bActive = false;
			NetMsgBRD.dwGaeaID = m_dwGaeaID;
			NetMsgBRD.emVehicle = m_emVehicle;
			for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
			{
				NetMsgBRD.dwPassengerID[ i ] = m_pVehicle->PassengerId(i);
			}

			SendMsgViewAround( &NetMsgBRD );
		}

	}

	return S_OK;
}

HRESULT GLChar::MsgVehicleInvenExSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_INVEN_EX_SLOT* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_INVEN_EX_SLOT* )nmg;
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB NetMsgFB;

	// �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.

	DWORD VehicleNum = VehicleDbNum();
	if ( GAEAID_NULL == VehicleNum )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

    SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pInvenItem )
    {
        // �Ϲݿ���
        NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
        SendToClient( &NetMsgFB );

        return E_FAIL;
    }

	if ( !m_pVehicle || !m_pVehicle->IsValidSlot( pNetMsg->emSuit ) )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	SITEMCUSTOM sHoldItemCustom = pInvenItem->CustomItem();
	SITEMCUSTOM	sSlotItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );

	if ( !pHoldItem || !pSlotItem ) 
	{
		// �Ϲݿ���
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
		return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_F )
	{
		// Ż�Ϳ� �������� �ƴҶ�
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ���� �ٸ� Ÿ���϶�
	if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		SendToClient( &NetMsgFB );
		return E_FAIL; 
	}

	if ( !m_pVehicle->CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) )
		return E_FAIL;

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem( m_pVehicle->m_sVehicleID );
	if ( !pVehicleItem )
	{
		return E_FAIL;
	}
	if ( pVehicleItem->sVehicle.bNotUseParts )
	{
		return E_FAIL;
	}

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory�� �ִ� ������ ���.

    WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
    WORD wPosY = sInven_BackUp.wPosY;

	SINVENITEM* pInsertedItem = NULL;

	// #item
	// vehicle puton(Inven) <-> inven(Hold)
	// vehicle puton ������(Inven) pNetMsg->emSuit, sSlotItemCustom
	// inven ������(Hold) wPosX, wPosY, sInven_BackUp, sHoldItemCustom

	{
		if ( !m_pVehicle->ReSetSlotItem( pNetMsg->emSuit ) )
		{
			sc::writeLogError( sc::string::format( "critical error MsgVehicleInvenExSlot, ReSetSlotItem fail, chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sSlotItemCustom.GetGuid() ).c_str() ) );
			return S_FALSE;
		}

		if ( !m_cInventory.DeleteItem( wPosX, wPosY ) )
		{
			sc::writeLogError( sc::string::format( "critical error MsgVehicleInvenExSlot, DeleteItem fail, chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sHoldItemCustom.GetGuid() ).c_str() ) );
			return S_FALSE;
		}

		if ( !m_pVehicle->SetSlotItem( pNetMsg->emSuit, sHoldItemCustom ) )
		{
			sc::writeLogError( sc::string::format( "critical error MsgVehicleInvenExSlot, SetSlotItem fail, chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sHoldItemCustom.GetGuid() ).c_str() ) );
			return S_FALSE;
		}

		pInsertedItem = m_cInventory.InsertItemNew( sSlotItemCustom, wPosX, wPosY );
		if ( !pInsertedItem )
		{
			sc::writeLogError( sc::string::format( "critical error MsgVehicleInvenExSlot, InsertItemNew fail, chanum: %1% guid: %2%", m_CharDbNum, sc::string::uuidToString( sSlotItemCustom.GetGuid() ).c_str() ) );
			return S_FALSE;
		}
	}

	// Ż�� ����â ��� ó��
	VehiclePutOnItemUpdate( pNetMsg->emSuit, true );

	// �κ� ������ ��� ������Ʈ
	InvenUpdateItem( pInsertedItem, true );

	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;			//	������ ������.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pInsertedItem;				//	���ԵǴ� ������.

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, NetMsg_Del_Insert );
	SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	SITEMCUSTOM VehiclePutonItem = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );

	// �ڽſ��� �˸�
	NetMsgFB.emFB                   = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
    NetMsgFB.sHoldtoSlotItemCustom  = VehiclePutonItem;
	NetMsgFB.emSuit                 = pNetMsg->emSuit;
	SendToClient( &NetMsgFB );

	// �ٸ� Ŭ���̾�Ʈ ȭ�鿡 �������� �ٲ� ���尡 ��µǾ�� �Ѵ�
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = VehiclePutonItem;
	SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

HRESULT GLChar::MsgVehicleInvenToSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_INVEN_TO_SLOT* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_INVEN_TO_SLOT* )nmg;
	GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB NetMsgFB;

    SINVENITEM* pInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pInvenItem )
    {
        // �Ϲݿ���
        NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
        SendToClient( &NetMsgFB );

        return E_FAIL;
    }

	if ( !m_pVehicle || !m_pVehicle->IsValidSlot( pNetMsg->emSuit ) )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );

		return E_FAIL;
	}

	SITEMCUSTOM sHoldItemCustom = pInvenItem->CustomItem();

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );

	if ( !pHoldItem ) 
	{
		// �Ϲݿ���
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
		return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_F )
	{
		// Ż�Ϳ� �������� �ƴҶ�
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// ���� �ٸ� Ÿ���϶�
	if ( pHoldItem->sSuitOp.emSuit != pNetMsg->emSuit )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		SendToClient( &NetMsgFB );
		return E_FAIL; 
	}

	if ( !m_pVehicle->CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) ) 
		return E_FAIL;

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem( m_pVehicle->m_sVehicleID );
	if ( !pVehicleItem )
	{
		return E_FAIL;
	}
	if ( pVehicleItem->sVehicle.bNotUseParts )
	{
		return E_FAIL;
	}

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory�� �ִ� ������ ���.

    WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
    WORD wPosY = sInven_BackUp.wPosY;

	// ������ġ�� ����Ǳ� ������ ��񿡼��� �������� �ʴ´�.
    if ( !InvenDeleteItem( wPosX, wPosY, true, sHoldItemCustom.bVietnamGainItem, 1, false ) )
        return E_FAIL;

	// �տ��� �������� �ֿ��� ������Ű��
	m_pVehicle->SetSlotItem( pNetMsg->emSuit, sHoldItemCustom );

	// ��� ó��
	VehiclePutOnItemUpdate( pNetMsg->emSuit, true );

	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	// �ڽſ��� �˸�
	NetMsgFB.emFB		 = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );
	SendToClient( &NetMsgFB );

	// �ٸ� Ŭ���̾�Ʈ ȭ�鿡 �������� �ٲ� ���� ��µǾ�� �Ѵ�
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );
	SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

HRESULT GLChar::MsgVehicleSlotToInven( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_SLOT_TO_INVEN* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_TO_INVEN* )nmg;
	GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB NetMsgFB;

	if ( !m_pVehicle || !m_pVehicle->IsValidSlot( pNetMsg->emSuit ) )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	SITEMCUSTOM	sSlotItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// �Ϲݿ���
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem( m_pVehicle->m_sVehicleID );
	if ( !pVehicleItem )
	{
		return E_FAIL;
	}
	if ( pVehicleItem->sVehicle.bNotUseParts )
	{
		return E_FAIL;
	}

    // ������ �������� �κ��丮��.
    if ( !InvenInsertItem( sSlotItemCustom, pNetMsg->wPosX, pNetMsg->wPosY, true, 1, false ) )
        return E_FAIL;

	// ��� ������Ʈ, InvenInsertItem ���� ��� ������Ʈ �ص� ������ ���� ��ġ�� ����Ǿ�� �Ǳ� ������ �̰ŷ� ó���Ѵ�.
	InvenUpdateItem( m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY ), true );

	// ������ �������� �����Ѵ�. ����� ��� �۾� �ʿ� ����.
	m_pVehicle->ReSetSlotItem( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

    GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB NetMsgRemoveFB;
    NetMsgRemoveFB.emSuit = pNetMsg->emSuit;
    SendToClient( &NetMsgRemoveFB );

	// �ٸ� Ŭ���̾�Ʈ ȭ�鿡 �������� �ٲ� ���� ��µǾ�� �Ѵ�
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = SITEMCUSTOM ( NATIVEID_NULL() );
	SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

// Ż�� ����â ����Ŭ���ϸ� ����� ���µ�
HRESULT GLChar::MsgVehicleRemoveSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM* )nmg;

	if ( !m_pVehicle || !m_pVehicle->IsValidSlot( pNetMsg->emSuit ) )
	{
		return E_FAIL;
	}

	SITEMCUSTOM	sSlotItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// �Ϲ� ����
		return E_FAIL;
	}

	WORD wPosX( 0 ), wPosY( 0 );
	BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		return E_FAIL;
	}

	// �κ��� �ֱ�
    if ( !InvenInsertItem( sSlotItemCustom, wPosX, wPosY, true, 1, false ) )
        return E_FAIL;

	// ��� ������Ʈ, InvenInsertItem ���� ��� ������Ʈ �ص� ������ ���� ��ġ�� ����Ǿ�� �Ǳ� ������ �̰ŷ� ó���Ѵ�.
	InvenUpdateItem( m_cInventory.GetItem( wPosX, wPosY ), true );

	// ���Ծ����� ����, ����� ��� �۾� �ʿ� ����.
	m_pVehicle->ReSetSlotItem ( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB NetMsgFB;
	NetMsgFB.emSuit = pNetMsg->emSuit;
	SendToClient( &NetMsgFB );

	// ���Ծ����� ���Ÿ� �˸�
	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	NetMsgBRD.emSuit = pNetMsg->emSuit;
	SendMsgViewAround( &NetMsgBRD );

	return S_OK;
}

HRESULT GLChar::MsgVehicleGiveBattery( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY* ) nmg;
	
	GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
	SNATIVEID sNativeID;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

	// Ż�� ���� üũ
	if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
	if ( !pHold ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY;
		return E_FAIL;
	}

	// ���͸� ���� üũ
	if ( pHold->sBasicOp.emItemType != ITEM_VEHICLE_OIL )	
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY;
		return E_FAIL;
	}

	if( pItem->sVehicle.emVehicleType != pHold->sVehicle.emVehicleType )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_DIFFERENT;
		return E_FAIL;
	}

	sNativeID = pHold->sBasicOp.sNativeID;

	// pHoldInvenItem �������� ����Ǵ� ���� ���� �Լ������� ������ ���� �����ؼ� ��ǥ�� ����Ѵ�.
    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	switch ( pHold->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		{
			if (pInvenItem->sItemCustom.m_VehicleDbNum == 0)
                return E_FAIL;
            AddGameAdoJob(
                db::DbActionPtr(
                    new CGetVehicleBattery(
                        m_ClientSlot,
                        m_CharDbNum,
                        pInvenItem->sItemCustom.m_VehicleDbNum,
                        pInvenItem->sItemCustom.GetNativeID(),
                        pHold->sDrugOp.dwCureVolume, 
                        pHold->sDrugOp.bRatio,
                        sNativeID,
                        pHold->sVehicle.emVehicleType,
                        wHoldPosX,
                        wHoldPosY ) ) );            
		}
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT	GLChar::MsgGetVehicleFullFromDB(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB* pIntMsg = ( GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB* ) nmg;
	switch ( pIntMsg->emFB )
	{
	case EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL:
		{
			GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
			NetMsgFB.emFB = EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL;
			SendToClient( &NetMsgFB );
		}
		break;

	case EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK:
		{
			//	�տ� �� ������ Ȯ���Ѵ�.
			//	�������� �ٲ�� ��찡 �ִ�.
            SINVENITEM* pHoldInvenItem = m_cInventory.GetItem( pIntMsg->wHoldPosX, pIntMsg->wHoldPosY );
            if ( !pHoldInvenItem )
                return E_FAIL;

            SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );
			if ( !pHold ) 
			{
				sc::writeLogError( sc::string::format( "Vehicle Give Full Error CharID : %d", m_CharDbNum ) );
				return E_FAIL;
			}

			if (  pHold->sBasicOp.emItemType != ITEM_VEHICLE_OIL || pHold->sBasicOp.sNativeID != pIntMsg->sBatteryID ) 
			{
				sc::writeLogError( sc::string::format( "Vehicle Give Full Error CharID : %d, Item %d/%d",
									m_CharDbNum, pHold->sBasicOp.sNativeID.wMainID, pHold->sBasicOp.sNativeID.wSubID ) );
				return E_FAIL;
			}

			// ���� ������ ���� �α�
			LogVehicleAction(
                pIntMsg->m_VehicleDbNum, 
                pIntMsg->sBatteryID,
                EMVEHICLE_ACTION_BATTERY_BEFORE, 
                pIntMsg->nFull );

			int nFull = pIntMsg->nFull;
			int nMaxFull = GLCONST_VEHICLE::pGLVEHICLE[pIntMsg->emType]->m_nFull;
			if ( nFull >= nMaxFull ) break;

			if ( pIntMsg->bRatio )
			{
				nFull += ( nMaxFull * pIntMsg->wCureVolume ) / 100;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}
			else
			{
				nFull += pIntMsg->wCureVolume;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}

			AddGameAdoJob(
                db::DbActionPtr(
                    new CSetVehicleBattery( m_ClientSlot, m_CharDbNum, pIntMsg->m_VehicleDbNum, nFull ) ) );

			// pHoldInvenItem �������� ����Ǵ� ���� ���� �Լ������� ������ ���� �����ؼ� ��ǥ�� ����ϰ� �ִ°� ������
            //SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	�տ� �� ������ ���.
            //WORD wHoldPosX = sHold_BackUp.wPosX;		//	�տ� �� ������ ��ġ.
            //WORD wHoldPosY = sHold_BackUp.wPosY;

			//ConsumeInvenItem( wHoldPosX, wHoldPosY );
            ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

			// �α�
			LogVehicleAction(
                pIntMsg->m_VehicleDbNum,
                pIntMsg->sBatteryID,
                EMVEHICLE_ACTION_BATTERY_AFTER,
                nFull );

			// ���� ������ ���� Ż�Ͱ� �����ѰŶ�� ������ �������ش�.
			// ������ ��Ȱ��ȭ �Ǿ������� ���߿� �ٷ� ����ϱ� ���� ����
			if ( m_pVehicle->m_DbNum == pIntMsg->m_VehicleDbNum )	
				m_pVehicle->IncreaseFull( pIntMsg->wCureVolume, pIntMsg->bRatio );

			GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
			NetMsgFB.emFB	   = EMVEHICLE_REQ_GIVE_BATTERY_FB_OK;
			NetMsgFB.dwVehicleID = pIntMsg->m_VehicleDbNum;
			NetMsgFB.sItemID	= pIntMsg->sItemID;
			NetMsgFB.sBatteryID = pIntMsg->sBatteryID;
			NetMsgFB.nFull	   = nFull;
			SendToClient( &NetMsgFB );
		}
	}

	return S_OK;
}

bool	GLChar::CheckCostumColorRestrict( int nColor )
{
	if ( nColor < RESTRICT_MIN )
		return false;
	else if ( nColor > RESTRICT_MAX )
		return false;

	return true;
}

bool	GLChar::CheckCostumColorChange_Old( DWORD dwMainColor, DWORD dwSubColor )
{
	DWORD dwColor = dwMainColor;

	INT nR, nG, nB;
	nR = (dwColor&0xff0000)>>16;
	nG = (dwColor&0xff00)>>8;
	nB = dwColor&0xff;

	bool bRCheck = CheckCostumColorRestrict( nR );
	bool bGCheck = CheckCostumColorRestrict( nG );
	bool bBCheck = CheckCostumColorRestrict( nB );

	if ( !bRCheck || !bGCheck || !bBCheck )
		return false;

	dwColor = dwSubColor;

	nR = (dwColor&0xff0000)>>16;
	nG = (dwColor&0xff00)>>8;
	nB = dwColor&0xff;

	bRCheck = CheckCostumColorRestrict( nR );
	bGCheck = CheckCostumColorRestrict( nG );
	bBCheck = CheckCostumColorRestrict( nB );

	if ( !bRCheck || !bGCheck || !bBCheck )
		return false;

	return true;
}

bool	GLChar::CheckCostumColorChange( DWORD dwMainColor, DWORD dwSubColor )
{
    std::vector<DWORD>::iterator iterBegin = GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT.begin();
    std::vector<DWORD>::iterator iterEnd   = GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT.end();

    if ( std::find( iterBegin, iterEnd, dwMainColor ) == iterEnd )
    {
        return false;
    }

    if ( std::find( iterBegin, iterEnd, dwSubColor ) == iterEnd )
    {
        return false;
    }

    return true;
}

HRESULT GLChar::MsgReqInvenCostumColorChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );

	if ( !pITEM )
	{
		MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( pITEM->sBasicOp.emItemType != ITEM_COSTUM_COLOR_CH_CD )
	{
		MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// �ǻ� ������ ��ȭ ��Ŵ.
	//
	EMSLOT emSlot = pNetMsg->emSlot;
	if ( emSlot < 0 || emSlot >= SLOT_TSIZE )
	{
		return E_FAIL;
	}

	SITEM* pSlotItem = GET_SLOT_ITEMDATA( emSlot );
	if ( !pSlotItem )
	{
		MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( !( m_PutOnItems[ emSlot ].IsDisguise() != NULL ) )
	{
		if ( m_PutOnItems[ emSlot ].nidDISGUISE != NATIVEID_NULL() )
		{
			const SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emSlot ].nidDISGUISE );

			if ( pDisguiseItem )
			{
				if ( !( m_PutOnItems[ emSlot ].IsDisguise() != NULL ) )
				{
					//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
					MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
					SendToClient( &MsgFB );
					return E_FAIL;
				}
			}
		}
		else
		{
			//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
			MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	if ( !pSlotItem->IsColorChange() )
	{
		if ( m_PutOnItems[ emSlot ].nidDISGUISE != NATIVEID_NULL() )
		{
			const SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emSlot ].nidDISGUISE );

			if ( pDisguiseItem )
			{
				if ( !pDisguiseItem->IsColorChange() )
				{
					MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
					SendToClient( &MsgFB );
					return E_FAIL;
				}
			}
		}
		else
		{
			MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	if ( pINVENITEM->sItemCustom.IsRestrict() != NULL )
	{
		// Memo : ����� ���ѵ� �������� ���.
		if ( !CheckCostumColorChange( pNetMsg->dwMainColor, pNetMsg->dwSubColor ) )
		{	// Memo : ���� ���� ���� ������ ����ϴ�.
			MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_RESTRICT;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	m_PutOnItems[ emSlot ].dwMainColor = pNetMsg->dwMainColor;
	m_PutOnItems[ emSlot ].dwSubColor = pNetMsg->dwSubColor;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.emSlot = emSlot;

	MsgFB.dwMainColor = m_PutOnItems[ emSlot ].dwMainColor;
	MsgFB.dwSubColor = m_PutOnItems[ emSlot ].dwSubColor;

	MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;

	NetMsgBrd.emSlot = emSlot;

	NetMsgBrd.dwMainColor = m_PutOnItems[ emSlot ].dwMainColor;
	NetMsgBrd.dwSubColor = m_PutOnItems[ emSlot ].dwSubColor;

	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenVehicleColorChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE* pNetMsg = ( GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE* )nmg;

	GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_VEHICLE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_VEHICLE_CHANGE_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	SITEM* pVehicle = GET_SLOT_ITEMDATA ( SLOT_VEHICLE );	
	if ( !pVehicle )
		return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pVehicle->VehicleType() != pITEM->sVehicle.emVehicleType )
	{
		return E_FAIL;
	}

	if ( pVehicle->IsColorChange() == false )
	{
		return E_FAIL;		
	}

	if ( pITEM->sBasicOp.emItemType != ITEM_VEHICLE_COLOR )
	{
		MsgFB.emFB = EMINVEN_VEHICLE_CHANGE_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// �� ��ȭ ��Ŵ.
	//
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		m_pVehicle->m_sColor[ i ].dwMainColor = pNetMsg->dwMainColor[ i ];
		m_pVehicle->m_sColor[ i ].dwSubColor = pNetMsg->dwSubColor[ i ];
	}

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		MsgFB.dwMainColor[ i ] = m_pVehicle->m_sColor[ i ].dwMainColor;
		MsgFB.dwSubColor[ i ] = m_pVehicle->m_sColor[ i ].dwSubColor;
	}

	MsgFB.emFB = EMINVEN_VEHICLE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		NetMsgBrd.dwMainColor[ i ] = m_pVehicle->m_sColor[ i ].dwMainColor;
		NetMsgBrd.dwSubColor[ i ] = m_pVehicle->m_sColor[ i ].dwSubColor;
	}

	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
    AddGameAdoJob(
        db::DbActionPtr(
            new CSetVehicleColor( m_ClientSlot, m_CharDbNum, m_pVehicle->m_DbNum, m_pVehicle ) ) );	
	return S_OK;
}

HRESULT GLChar::MsgReqVehicleGetOff (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GET_OFF_WITH_VEHICLE *pNetMsg = (GLMSG::SNET_GET_OFF_WITH_VEHICLE *)nmg;

	if( m_emVehicle == EMVEHICLE_OFF )
	{
		return E_FAIL;
	}

	else if( m_emVehicle == EMVEHICLE_DRIVER )
	{
		for( INT i = 1; i < MAX_PASSENGER_COUNT; i++ )
		{
			if( m_pVehicle->PassengerId(i) == GAEAID_NULL )
				continue;

			GLChar* pCHAR = GetChar(m_pVehicle->PassengerId(i));
			if ( pCHAR )
			{
				pCHAR->ActiveWithVehicle( false, false );
			}

			m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
		}
	}
	else if( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		INT nIndex = -1;
		for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			if( m_pVehicle->PassengerId(i) == m_dwGaeaID )
			{
				nIndex = i;
				break;
			}
		}

		if ( nIndex > 0 && nIndex < MAX_PASSENGER_COUNT ) 
		{
			for( INT j = 0; j < MAX_PASSENGER_COUNT; j++ )
			{
				if( m_pVehicle->PassengerId(j) == m_dwGaeaID || m_pVehicle->PassengerId(j) == GAEAID_NULL )
					continue;

				GLChar* pCHARSnd = GetChar(m_pVehicle->PassengerId(j));
				if( !pCHARSnd )
					continue;

				pCHARSnd->m_pVehicle->PassengerIdSet(nIndex, GAEAID_NULL);
			}
		}
		ActiveWithVehicle( false, false );	

	}
	return S_OK;
}

HRESULT GLChar::MsgReqLearnBikeBoost( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_BIKE_BOOST* pNetMsg = ( GLMSG::SNETPC_REQ_BIKE_BOOST* )nmg;

	GLMSG::SNETPC_REQ_BIKE_BOOST_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_LEARN_BIKE_BOOST_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		MsgFB.emFB = EMINVEN_LEARN_BIKE_BOOST_FB_COOLTIME;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );

	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_BIKE_BOOST_CARD )
	{
		MsgFB.emFB = EMINVEN_LEARN_BIKE_BOOST_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
	if ( !pVehicleItem )
		return E_FAIL;

	if( pVehicleItem->sVehicle.emVehicleType != pITEM->sVehicle.emVehicleType )
		return E_FAIL;

	if( pVehicleItem->sVehicle.emPartsType != pITEM->sVehicle.emPartsType )
		return E_FAIL;

	m_pVehicle->m_dwBoosterID = TRUE;

	// Ŭ���̾�Ʈ�� ó�� ��� ����.
	MsgFB.dwBoosterID = m_pVehicle->m_dwBoosterID;
	MsgFB.emFB = EMINVEN_LEARN_BIKE_BOOST_FB_OK;
	SendToClient( &MsgFB );

	// ��� ������ �ٸ� ����鿡�� �˸�.
	GLMSG::SNETPC_REQ_BIKE_BOOST_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwBoosterID = m_pVehicle->m_dwBoosterID;
	SendMsgViewAround( &NetMsgBrd );

	//	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB�� ����
    AddGameAdoJob(
        db::DbActionPtr(
            new CSetVehicleBooster( m_ClientSlot, m_CharDbNum, m_pVehicle->m_DbNum, m_pVehicle->m_dwBoosterID ) ) );	
	return S_OK;
}

HRESULT GLChar::MsgReqUseBikeBoost (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_USE_BIKE_BOOST *pNetMsg = (GLMSG::SNETPC_USE_BIKE_BOOST *)nmg;
	GLMSG::SNETPC_USE_BIKE_BOOST_FB MsgFB;
    
	const SITEM* pItem = GLogicData::GetInstance().GetItem( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
	if( !pItem )
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_NOITEM;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if( pItem->VehicleType() == VEHICLE_TYPE_BOARD )
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_NOBIKE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if( m_emVehicle != EMVEHICLE_DRIVER )
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_NODRIVER;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if( !m_pVehicle->m_dwBoosterID )
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_NOT_BOOSTERID;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if( m_pVehicle->m_fBoostDelay > 0.0f )
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_DELAY;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if( m_pVehicle->m_bUseBoost )	
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_USING;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( m_pVehicle->IsNotEnoughFull() ) 
	{
		MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_NOTFULL;
		SendToClient( &MsgFB );
		return E_FAIL;

	}

	m_pVehicle->m_bUseBoost = TRUE;
	m_pVehicle->m_fBoostTime = 0.0f;	

	MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_FB_OK;
	SendToClient( &MsgFB );

	GLMSG::SNETPC_REQ_USE_BIKE_BOOST_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround(&NetMsgBrd);

	m_pVehicle->m_nFull -= 2;
	if ( m_pVehicle->m_nFull < 0 )
        m_pVehicle->m_nFull = 0;
	// Ŭ���̾�Ʈ ������ ����
	GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY NetMsg;
	NetMsg.nFull = m_pVehicle->m_nFull;
	SendToClient( &NetMsg );

	return S_OK;
}

HRESULT GLChar::MsgReqVietnamGainType (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_VIETNAM_TIME_REQ_FB NetMsgFB;
	NetMsgFB.gameTime  = m_sVietnamSystem.gameTime;
	NetMsgFB.loginTime = m_sVietnamSystem.loginTime;
	SendToClient( &NetMsgFB );

	GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
	NetMsg.dwGainType = (BYTE)m_dwVietnamGainType;
	SendToClient( &NetMsg );
	return S_OK;
}

HRESULT GLChar::MsgItemShopOpen(NET_MSG_GENERIC* nmg) // ItemShopOpen
{
//#if defined ( JP_PARAM ) || defined(_RELEASED)	// JAPAN Item Shop
	if (m_ServiceProvider == SP_JAPAN || m_ServiceProvider == SP_OFFICE_TEST)
	{	
		GLMSG::SNETPC_OPEN_ITEMSHOP* NetMsg = ( GLMSG::SNETPC_OPEN_ITEMSHOP* ) nmg;

		if ( m_bItemShopOpen == NetMsg->bOpen ) return E_FAIL;

		m_bItemShopOpen = NetMsg->bOpen;

		GLMSG::SNETPC_OPEN_ITEMSHOP_BRD NetMsgBrd;
		NetMsgBrd.bOpen = m_bItemShopOpen;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		SendMsgViewAround(&NetMsgBrd);
	}
//#endif
	
	return S_OK;
}

HRESULT	GLChar::MsgGathering(NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_REQ_GATHERING *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_GATHERING*> ( nmg );

	PGLMATERIAL pMaterial = pLand->GetMaterial ( pNetMsg->dwTargetID );

	if ( !pMaterial )
	{
		//	�������� ��������ϴ�.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	if ( pMaterial->GetCrow() != CROW_MATERIAL )
	{
		//	�������� ��������ϴ�.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOTTYPE;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	if ( !pMaterial->IsValidBody()  )
	{
		//	�������� ��������ϴ�.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	// ���Ȯ��
	if ( !IsValidBody() )
		return E_FAIL;

	if ( isAction( GLAT_GATHERING ) )
	{
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_GATHERING;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	//	�Ÿ� üũ
	D3DXVECTOR3 vPos;
	vPos = m_vPosition;

	const D3DXVECTOR3 &vTarPos = pMaterial->GetPosition();

	D3DXVECTOR3 vDistance = vPos - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	WORD wTarBodyRadius = 4;
	WORD wGatherRange = wTarBodyRadius + GETBODYRADIUS() + 2;
	WORD wGatherAbleDis = wGatherRange + 15;

	if ( fDistance>wGatherAbleDis )
	{
		//	�Ÿ��� �ٴϴ�.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_DISTANCE;
		SendToClient( &NetMsg );

		return E_FAIL;
	}


	// �̹� �ٸ������ ������̶��???

	if ( pMaterial->m_dwGatherCharID != GAEAID_NULL )
	{
		//	�Ÿ��� �ٴϴ�.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_USE;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	int nTime = sc::Random::RandomNumber( pMaterial->m_pCrowData->m_wGatherTimeLow, pMaterial->m_pCrowData->m_wGatherTimeHigh );
	// crow�� ����
	pMaterial->m_dwGatherCharID = m_dwGaeaID;
	pMaterial->m_fGatherTime = (float)nTime;

	// random Ÿ�� �����Ŀ� �ð� üũ

	//	User�� ����
	TurnAction ( GLAT_GATHERING );

	m_dwANISUBSELECT = pMaterial->m_pCrowData->m_wGatherAnimation;

	//	�Ÿ��� �ٴϴ�.
	GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
	NetMsg.emFB = EMREQ_GATHER_FB_OK;
	NetMsg.dwTime = nTime; // pCrow->dwTime;
	NetMsg.dwAniSel = m_dwANISUBSELECT;
	NetMsg.dwGaeaID = pMaterial->GetGaeaID();
	SendToClient( &NetMsg );

	GLMSG::SNETPC_REQ_GATHERING_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwAniSel = m_dwANISUBSELECT;
	SendMsgViewAround(&NetMsgBrd);

	return S_OK;
}

HRESULT GLChar::MsgReqDialogueWindowOpen(NET_MSG_GENERIC* nmg){
	GLMSG::SNETPC_DIALOGUEWINDOW_OPEN* pNetMsg = (GLMSG::SNETPC_DIALOGUEWINDOW_OPEN*)nmg;
	GLMSG::SNETPC_DIALOGUEWINDOW_OPEN_FB NetMsg_fb;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_FAIL;
		SendToClient( &NetMsg_fb );
		return E_FAIL;
	}

	switch( pNetMsg->emCrow )
	{
	case CROW_NPC :
		{
			PGLCROW pCrow = pLand->GetCrow(pNetMsg->dwGaeaID);

			if ( !pCrow )
			{
				NetMsg_fb.emFB   = EMREQ_NPCTALK_FB_FAIL;
				NetMsg_fb.emCrow = CROW_NPC;
				SendToClient(&NetMsg_fb);
				return E_FAIL;
			}
		}
		break;
	case CROW_SUMMON :
		{
			PGLSUMMONFIELD pCrow = pLand->GetSummon(pNetMsg->dwGaeaID);

			if ( !pCrow )
			{
				NetMsg_fb.emFB   = EMREQ_NPCTALK_FB_FAIL;
				NetMsg_fb.emCrow = CROW_SUMMON;
				SendToClient(&NetMsg_fb);
				return E_FAIL;
			}

			if( pCrow->GetSummonType() != SUMMON_TYPE_NPC )
			{
				NetMsg_fb.emFB   = EMREQ_NPCTALK_FB_FAIL;
				NetMsg_fb.emCrow = CROW_SUMMON;
				SendToClient(&NetMsg_fb);
				return E_FAIL;
			}
		}
		break;
	default:
		{
			NetMsg_fb.emFB   = EMREQ_NPCTALK_FB_FAIL;
			NetMsg_fb.emCrow = CROW_MOB;
			SendToClient(&NetMsg_fb);
			return E_FAIL;
		}
		break;
	}
	
	NetMsg_fb.emFB   = EMREQ_NPCTALK_FB_OK;
	NetMsg_fb.sNPCID = pNetMsg->sNPCID;
	NetMsg_fb.emCrow = pNetMsg->emCrow;
	SendToClient(&NetMsg_fb );

	if ( GLHIDESET::CheckHideSet(EMHIDE_TALK_START) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqNpcTalkBasic(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_NPCTALK_BASIC *pNetMsg = (GLMSG::SNETPC_REQ_NPCTALK_BASIC *) nmg;
	GLMSG::SNETPC_REQ_NPCTALK_BASIC_FB NetMsg_fb;

	NetMsg_fb.nAction = pNetMsg->nAction;
	NetMsg_fb.dwNpcGlobID = pNetMsg->dwNpcGlobID;
	NetMsg_fb.dwTalkID = pNetMsg->dwTalkID;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_NPC;
		SendToClient(&NetMsg_fb );
		return E_FAIL;
	}

	PGLCROW pCrow = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pCrow )
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_NPC;
		SendToClient(&NetMsg_fb );
		return E_FAIL;
	}

    if ( !pCrow->IsNpcInteractionAble( GetPosition(), GETBODYRADIUS(), pNetMsg->nAction ) )
    {
        NetMsg_fb.emFB = EMREQ_NPCTALK_FB_DISTANCE;
        SendToClient(&NetMsg_fb );
        return E_FAIL;
    }

	// NpcTalk ��ȿ�� üũ
	if (!pCrow->IsBasicTalk(pNetMsg->nAction))
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_TYPE;
		SendToClient(&NetMsg_fb );
		return E_FAIL;
	}

	std::tr1::shared_ptr<SNpcTalk> spTalk = pCrow->GetTalk(pNetMsg->dwTalkID);
	if ( !spTalk || !spTalk->IsUscCondition()) 
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_TALK;
		SendToClient(&NetMsg_fb );
		return E_FAIL;
	}

	if ( spTalk->m_nACTION != SNpcTalk::EM_BASIC || spTalk->m_dwACTION_NO != pNetMsg->nAction )
	{
		NetMsg_fb.emFB = EMREQ_NPCTALK_FB_TYPE;
		SendToClient(&NetMsg_fb );
		return E_FAIL;
	}

	NetMsg_fb.emFB = EMREQ_NPCTALK_FB_OK;
	SendToClient(&NetMsg_fb );

	return S_OK;
}

//
//mjeon.activity
//
HRESULT GLChar::MsgActivityChangeBadge(NET_MSG_GENERIC* nmg)
{
	//validate the activity-done request.

	GLMSG::SNET_ACTIVITY_CHANGE_BADGE *msg = (GLMSG::SNET_ACTIVITY_CHANGE_BADGE *)nmg;

	int idx = msg->nIndex;

	BOOL bChanged = m_pTitleMan->Select(idx);
	
	//
	//prepare the response message.
	//
	GLMSG::SNET_ACTIVITY_CHANGE_BADGE_FB msgFB;
	msgFB.bDone = bChanged;
	
	SendToClient( &msgFB);

	if (bChanged)
	{
		GLMSG::SNET_TITLE_NOTIFY_CHANGE msgNotify;	//viewaround

		TITLEINFO ti;
		m_pTitleMan->GetCurrentTitle(ti);

		msgNotify.dwGaeaID = m_dwGaeaID;
		StringCchCopy(msgNotify.szNewTitle, EM_TITLEID_MAXLENGTH, ti.strTitle.c_str());
		msgNotify.nClass = ti.nClass;

		SendMsgViewAround(&msgNotify);

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}


HRESULT GLChar::MsgActivityCompletedFF(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ACTIVITY_COMPLETED_FF *msg = (GLMSG::SNET_ACTIVITY_COMPLETED_FF *)nmg;
	
	m_pActivityMan->CompletedActivity(msg->dwActivityID, (ENUM_ACTIVITY_CLASS)msg->dwClass, msg->dwRewardPoint, std::string(msg->szRewardTitleID));

	return S_OK;
}


HRESULT GLChar::MsgActivityLoadCompletedFF(NET_MSG_GENERIC* nmg)
{
	m_pActivityMan->SetReady(TRUE);
	m_pActivityMan->CheckAfterLoad();

	return S_OK;
}


//
//mjeon.attendance
//
HRESULT GLChar::MsgAttendanceSyncFF(NET_MSG_GENERIC* nmg)
{
	//GLMSG::SNET_ATTENDANCE_SYNC_FF *msg = (GLMSG::SNET_ATTENDANCE_SYNC_FF *)nmg;
	
	m_pAttendanceMan->Sync();

	return S_OK;
}


HRESULT GLChar::MsgAttendanceResetFF(NET_MSG_GENERIC* nmg)
{
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();

	m_pAttendanceMan->Reset(ctCurrentTime);
	
	return S_OK;	
}


HRESULT GLChar::MsgAttendanceUpgradeFF(NET_MSG_GENERIC* nmg)
{	
	m_pAttendanceMan->Upgrade();
	
	return S_OK;	
}


HRESULT GLChar::MsgAttendanceTakeRewardCF(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ATTENDANCE_TAKEREWARD_CF *msg = (GLMSG::SNET_ATTENDANCE_TAKEREWARD_CF *)nmg;

	int idx = msg->idxTask;

	if ( m_pAttendanceMan->TakeReward(idx) )
		return S_OK;
	else
		return E_FAIL;
}


HRESULT GLChar::MsgAttendanceDonePointItemCF(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_CF *msg = (GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_CF *)nmg;

	int idx = msg->idx;

	m_pAttendanceMan->CheckTaskPointItem();

	GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_FB msgFB;
	msgFB.idx = idx;
	SendToClient(&msgFB);

	return S_OK;
}


HRESULT GLChar::MsgAttendanceWhisperDoneAF(NET_MSG_GENERIC* nmg)
{
	m_pAttendanceMan->CheckTaskWhisper();

	return S_OK;
}

HRESULT GLChar::MsgChaExtraInfoHF(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *pmsg = (GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *)nmg;

	m_nContributionPoint = pmsg->nContributionPoint;
	//SendToClient(pmsg);
	m_pGaeaServer->SENDTOAGENT(nmg);

	return S_OK;
}


HRESULT GLChar::MsgReqItemSort( NET_MSG_GENERIC* nmg )
{
	if ( E_FAIL == MsgReqInvenItemSort( nmg ) )
	{
		GLMSG::SNET_ITEM_SORT_FB NetMsg( E_FAIL );
		SendToClient( &NetMsg );
	}
	return S_OK;
}

HRESULT GLChar::MsgReqItemSortStart( NET_MSG_GENERIC* nmg )
{
	int nRet = MsgReqInvenItemSortStart( nmg );

	if ( 1 != nRet )
	{
		GLMSG::SNET_ITEM_SORT_FB NetMsg(nRet);
		SendToClient( &NetMsg );
	}
	return S_OK;
}

HRESULT GLChar::MsgReqItemSortData( NET_MSG_GENERIC* nmg )
{
	int nRet = MsgReqInvenItemSortData( nmg );

	if ( 1 != nRet )
	{
		GLMSG::SNET_ITEM_SORT_FB NetMsg(nRet);
		SendToClient( &NetMsg );
	}
	return S_OK;
}

HRESULT GLChar::MsgReqItemSortEnd( NET_MSG_GENERIC* nmg )
{
	int nRet = MsgReqInvenItemSortEnd( nmg );

	if ( 1 != nRet )
	{
		GLMSG::SNET_ITEM_SORT_FB NetMsg(nRet);
		SendToClient( &NetMsg );
	}
	return S_OK;
}

HRESULT GLChar::MsgTransDB2WorldBattle( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE *pNetMsg = (GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE *) nmg;

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if( !pChar )
		return E_FAIL;

	AddGameAdoJob( db::DbActionPtr( new db::TransDBDataToWorldBattle(pChar, m_CharDbNum) ) );

	return S_OK;
}

HRESULT GLChar::MsgTransDB2WorldBattleAns( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB *pNetMsg = (GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB *) nmg;
	//GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB NetMsgAns;

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if( !pChar )
		return E_FAIL;

	SendToClient(pNetMsg);

	return S_OK;
}

int GLChar::MsgReqExpCompressor( NET_MSG_GENERIC* nmg, int &nExpPercent, SNATIVEID &sCapsuleItem )
{
	GLMSG::SNETPC_EXP_COMPRESSOR_REQ *pNetMsg = ( GLMSG::SNETPC_EXP_COMPRESSOR_REQ * )nmg;
	GLMSG::SNETPC_EXP_COMPRESSOR_ANS MsgFB;
	int nRet = 0;
	LONGLONG llNeedExp = 0;
	LONGLONG llOverExp = 0;

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if ( !pChar )
		return nRet;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return nRet;
	
	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_NOTEXIST;
		SendToClient( &MsgFB );
		return nRet;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )	return nRet;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_EXP_COMPERSSOR )
	{
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_ITEMTYPE;
		SendToClient( &MsgFB );
		return nRet;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return nRet;
	}

	if ( pITEM->sDrugOp.emDrug != ITEM_DRUG_EXP_COMPERSSOR_TYPE1 && pITEM->sDrugOp.emDrug != ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
	{
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_ITEMTYPE;
		SendToClient( &MsgFB );
		return nRet;
	}

	if ( pITEM->sBox.vecItems.size() != 1 )
	{
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_NOTVALUE;
		SendToClient( &MsgFB );
		return nRet;
	}
	else
	{
		sCapsuleItem = pITEM->sBox.vecItems[0].nidITEM;
	}
	// ����ϴ� ����⿡ ���� ������ ĸ�� ���� ����
	// ����� ������ �����ϱ⶧���� Ÿ�Կ� ���� ĸ���� ���������� ���� ������ �������� ����ٸ� ���⼭ ����ؼ� ĸ�� Ÿ���� �����������
	// 1�� ����ġ�� �����̹Ƿ� ������ Ÿ�Կ� ���� ������ ���
	switch( pITEM->sDrugOp.emDrug )
	{
	case ITEM_DRUG_EXP_COMPERSSOR_TYPE1:
		nExpPercent = pITEM->sDrugOp.GetCompressPrecent();
		nRet = 1;
		llNeedExp = 10000000000;
		break;
	case ITEM_DRUG_EXP_COMPERSSOR_TYPE2:
		nExpPercent = pITEM->sDrugOp.GetCompressPrecent() * 10;
		nRet = 2;
		llNeedExp = 100000000000;
		break;
	}


	if( m_sExperience.lnNow < llNeedExp || m_sExperience.lnMax < llNeedExp )
	{
		nRet = 0;
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_NOTENOUGH;
		SendToClient( &MsgFB );
		return nRet;
	}
	else
	{
		if( m_sExperience.lnNow >= m_sExperience.lnMax )
		{
			llOverExp = m_sExperience.lnNow - m_sExperience.lnMax;
			m_sExperience.lnNow = m_sExperience.lnMax;
		}
		m_sExperience.lnNow -= llNeedExp;
	}
	
	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
	NetMsgExp.lnNowExp = m_sExperience.lnNow;
	SendToClient( &NetMsgExp );

	AddGameAdoJob( db::DbActionPtr( new db::CalculateExpCompressor(m_CharDbNum, m_sExperience.lnNow, llNeedExp + llOverExp ) ) );

    LogCharAction
	    (
	    m_CharDbNum,					//	�����.
	    gamelog::ACT_EXP_COMPRESSOR,	//	����.
	    gamelog::ID_CHAR, 0,			//	����.
	    llNeedExp,						// exp
	    0,								// bright
	    0,								// life
	    0,								// money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
	    );

	 return nRet;	
}


HRESULT GLChar::MsgReqExpCapsule(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_EXP_CAPSULE_REQ *pNetMsg = (GLMSG::SNETPC_EXP_CAPSULE_REQ *)nmg;
	GLMSG::SNETPC_EXP_CAPSULE_ANS MsgFB;

	GLChar* pChar = GetCharByDbNum( m_CharDbNum );
	if( !pChar )
		return E_FAIL;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;
	
	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );

	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_EXP_CAPSULE_NOTEXIST;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	LONGLONG llRatioExp = (LONGLONG)pINVENITEM->sItemCustom.GetDurability();

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )	return E_FAIL;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_EXP_CAPSULE )
	{
		MsgFB.emFB = EMINVEN_EXP_CAPSULE_ITEMTYPE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( pITEM->sDrugOp.emDrug != ITEM_DRUG_EXP_CAPSULE_TYPE1 && pITEM->sDrugOp.emDrug != ITEM_DRUG_EXP_CAPSULE_TYPE2 )
	{
		MsgFB.emFB = EMINVEN_EXP_CAPSULE_ITEMTYPE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	if ( llRatioExp == 0 )
	{
		MsgFB.emFB = EMINVEN_EXP_CAPSULE_NOTVALUE;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	//	[�ڽſ���] Inventory ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	LONGLONG llExp = 100000000;
	llExp *= llRatioExp;

	m_sExperience.lnNow += ( llExp <= 0 ) ? 0 : llExp;
	
    // ���� ����. ( ���� �޽��� �߻�. )
    //
	if( m_sExperience.lnNow >= m_sExperience.lnMax && m_wLevel < GLCONST_CHAR::wMAX_LEVEL )
	{
		GLMSG::SNETPC_REQ_JUMPINGLEVELUP NetMsg;
		MsgReqJumpingLevelUp ( (NET_MSG_GENERIC*) &NetMsg );
	}
	// ������ �ƴϸ� ����ġ ����
	else
	{
		GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
		NetMsgExp.lnNowExp = m_sExperience.lnNow;
		SendToClient(&NetMsgExp);
	}

    m_lnLastSendExp = m_sExperience.lnNow;

	MsgFB.emFB = EMINVEN_EXP_CAPSULE_OK;
	SendToClient( &MsgFB );

	 return S_OK;	
}

void GLChar::MacroOnOff(bool bOn)
{
	if ( bOn == true )
	{
		SetActState(EM_ACT_MACROMODE);

		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::MacroLogInsert( m_CharDbNum, MACRO::MACROLOG_STATE_START )));
	}
	else
	{
		ReSetActState(EM_ACT_MACROMODE);
		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::MacroLogInsert( m_CharDbNum, MACRO::MACROLOG_STATE_END )));
	}
}

HRESULT GLChar::MsgReqMacroModeOnoffCF(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_MACROMODE_ONOFF_CF* pMsg = static_cast<GLMSG::SNET_MACROMODE_ONOFF_CF*>(nmg);
	GLMSG::SNET_MACROMODE_ONOFF_FB msgFB;

	if ( pMsg->bOn )
	{
		msgFB.dwMsg = GLMSG::SNET_MACROMODE_ONOFF_FB::MACROON_OK;
		if ( GLUseFeatures::GetInstance().IsUsingMacro_LimitTime() && m_fRemainMacroTime <= 0.0f )
		{
			msgFB.dwMsg = GLMSG::SNET_MACROMODE_ONOFF_FB::MACROON_ERROR_LACKTIME;
			SendToClient(&msgFB);
			return E_FAIL;
		}
		MacroOnOff(true);
	}
	else
	{
		msgFB.dwMsg = GLMSG::SNET_MACROMODE_ONOFF_FB::MACROOFF_OK;
		MacroOnOff(false);
	}
	SendToClient(&msgFB);
	return S_OK;
}
