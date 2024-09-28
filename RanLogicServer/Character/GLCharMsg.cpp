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
	//	[자신에게] 회복상태 갱신.
	GLMSG::SNETPC_UPDATE_STATE NetMsg;
	NetMsg.sHP = GLCHARLOGIC::m_sHP;
	NetMsg.sMP = GLCHARLOGIC::m_sMP;
	NetMsg.sSP = GLCHARLOGIC::m_sSP;

	//NetMsg.ENCODE ( m_dwGaeaID );
	SendToClient( &NetMsg );

	//	대련자에게 체력 변화 알림.
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

	// 적대 행위자의 타이머 갱신 및 관리.
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
	// 적대적인 상대가 존재하면 HP 갱신 메세지를 날린다;
	// 클라이언트내에서 HP == 0?으로 처리하는 구분이 많은데, pk타임 pk컨텐츠가아닐땐 갱신이안되서 비정상동작하는 부분이 생긴다;
	// ex RM# 1674 - GLCharClient::UPDATE_DATA내에서 할루미네이션 정보 갱신시 hp가 0인지아닌지로 검사한다;

    // HP 변화상태를 주변에 전송할지에 대한 조건 체크:
    // 일반적인 상황 : HP표시 플래그 체크;
    // 예외처리 : 선도전, (구)CDM, 학원간 FreePK타임, 성향이벤트, PK서버, PK맵, 깃발전;
    // if ( bGuidBattleMap || bClubDeathMatch || bSCHOOL_FREEPK || bBRIGHTEVENT || bPKServer || bPKMap || bCTF || bHostileTargetExist )
	*/
	//  [7/13/2016 gbgim] RM #3571
	// - 공격할 수 있는 상황이 아니라면 다른 클라의 HP갱신이 되지않는다;
	//   (스케일폼 이하 버전에서는 적대관계가 아니라면 무조건 풀피로 보여줌);
	// - 타겟으로 설정한 유저 정보 또한 위 상황에 만족하게되면 갱신되지않는다;
	// - 이미 PK Server로 사용하는 곳도 있고, 유저가 많이 존재하는 곳이면서 ;
	//   PK상황이 아닌경우는 학원 광장이 전부라고 생각하고 평화지역만 체크하도록 변경함;
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
	// 활성화 시킴.
	//
	ReSetActState(EM_ACT_WAITING);

	// 진입 메시지 전송
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
		// 딜래이를 반영한 이동 속도 측정.
		m_actorMove.SetMaxSpeed( GetMoveVelo() );

		// 자신의 주변 Char에게 자신의 Msg를 전달.
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

	// 클라이언트들에게 위치 강제 설정.
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
			// 자신의 주변 Char에게 자신의 Msg를 전달.
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
	// 상점이 개설된 상태에서 이동할 수 없다.
	if (m_pPrivateMarket->IsOpen())
        return false;

	// 내상태가 DEAD 이면 DEAD Animation 처리
	if (IsActState(EM_ACT_DIE))
	{
		// 쓰러짐을 클라이언트 들에게 알림.
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_FALLING;
        
        // 자신의 클라이언트에
		SendToClient(&NetMsgBrd);
		return false;
	}

	if ( m_sDOMINATE.emController == EMSPEC_DOMINATE_TYPE_FIX )
		return false;

	return IsMovement();
}

//! 이동 처리
HRESULT GLChar::MsgGoto(NET_MSG_GENERIC* nmg)
{
	if( m_pGaeaServer->m_bEmptyMsg )
        return S_OK;

	if( !CheckStateToGo() )
		return E_FAIL;

	GLMSG::SNETPC_GOTO* pPacket = reinterpret_cast<GLMSG::SNETPC_GOTO*>( nmg );

	BOOL	bRun	= IsActState( EM_ACT_RUN );				// 달리는 상태인지
	BOOL	bToRun	= pPacket->dwActState & EM_ACT_RUN;		// 달려야 하는지

	// 상태 변경
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

	// 새로운 위치 이동이 들어왔을 때 현재 Client 와 Server 위치 사이에 충돌 Object 가 있을 경우, 
	// Client 상 위치를 서버위치로 변경.
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
		// 제스쳐 중이면 제스쳐 끝내고 ( 댄스중 순간이동 방지 )
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

// AnimationMove 시 몸 회전을 다른 클라에 보낸다.
void GLChar::MsgAniMoveStartCF( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ANIMOVE_START_CF* pNetMsg = (GLMSG::SNETPC_ANIMOVE_START_CF*) nmg;

	// AnimationMove Start

	// 자신의 주변 Char에게 자신의 Msg를 전달.
	GLMSG::SNETPC_ANIMOVE_START_BRD NetMsgFB;
	NetMsgFB.dwGaeaID	= m_dwGaeaID;
	NetMsgFB.m_fAngleY	= pNetMsg->m_fAngleY;
	SendMsgViewAround( &NetMsgFB );
}

// AnimationMove로 이동된 자신의 마지막 위치를 서버로 전송한다.
void GLChar::MsgAniMoveEndCF( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ANIMOVE_END_CF* pNetMsg = (GLMSG::SNETPC_ANIMOVE_END_CF*) nmg;

	// AnimationMove End

	// 1. 실제위치로 갈 수 있는지 확인하고 갈 수 있다면 이동한다.
	//		갈 수 없다면 Client 에 Rollback 시켜준다. ( Character 랑 Crow 랑 다름 )
	// 2. 그리고 현재 위치를 전 유저에서 이동하라고 보내준다. ( Character 랑 Crow 랑 다름 )
	//
	// 위의 작업이 MsgGoto 에서 다 작업된다.

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

    //도발중이라면 다른 타겟은 공격할수 없다.
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

    // 타겟과의 거리 검사
    D3DXVECTOR3 vTarPos = pTARGET->GetPosition();

    int nAttackAbleDistance = 
        pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 9 + static_cast<int>(GETSUM_RANGE_PSY_LONG());

	if ( nAttackAbleDistance < 0 )
		nAttackAbleDistance = pTARGET->GetBodyRadius() + GETBODYRADIUS() + 9;

	nAttackAbleDistance *= GLCONST_CHAR::fATTACK_RANGE_ADJUST_RATE;
    if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( nAttackAbleDistance, m_vPosition, vTarPos, 5.f ) )
    {
        // 자신에게.
        GLMSG::SNETPC_ATTACK_AVOID NetMsg;
        NetMsg.emTarCrow	= m_TargetID.emCrow;
        NetMsg.dwTarID		= m_TargetID.GaeaId;
        SendToClient((NET_MSG_GENERIC*) &NetMsg );

        // 주변 클라이언트들에게 메세지 전송.
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

    // 공격이 가능한 타켓인지를 검사;
    // 만약 중립적인 대상인 경우는 강제 공격한격으로 판정;
    const EMREACTIONABLE emREACTIONALBE  = IsReActionable( pTARGET, EMIMPACT_SIDE_ENEMY );
    switch ( emREACTIONALBE )
    {
    case EMREACTIONABLE_FALSE:
        return E_FAIL;

    case EMREACTIONABLE_TRUE:
        break;

    case EMREACTIONABLE_FORCED_TRUE:
        // 공격이 가능한데 중립적대상이라서 강제 공격으로 판정되었을 경우;
        // 
        // 주 타겟 외에 타겟은 강제 공격 불가하다.;
        // PC의 경우 Client에서 타겟선별을 처리하여 서버로 정보를 보내는데
        // 주타겟외에 강제공격 타겟이 있다는 말은 패킷 조작일 가능성이 있다.;
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
			// Table ID 가 같을 경우에만 Index를 변경한다;
			// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
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

    // 공격이 가능한지 검사.
    //
    EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);

    if (emBeginFB != EMBEGINA_OK && emBeginFB != EMBEGINA_SP)
        return E_FAIL;


    // 소모성 아이탬 유효성 검사후, 사라질때 메시지 발생.
    CheckInstanceItem();

    // 사전 명중치 계산.
    SNATIVEID				idSkill( false );
    SCHARSKILL				sSkillLevel;
    GetSkillLevel( idSkill, sSkillLevel );
    m_WorkingSkill.Start( sSkillLevel, this, emBeginFB == EMBEGINA_SP, &m_TargetID, m_wTARNUM, m_sTARIDS, false );

    // 공격 수행.
    //
    TurnAction(GLAT_ATTACK);

    if ( GLHIDESET::CheckHideSet(EMHIDE_START_ATTACK) )
    {
        RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
        RestoreActStateByInvisible();
    }

    // 공격 성공시 - 자신의 주변 Char에게 자신의 Msg를 전달.
    //
    GLMSG::SNETPC_ATTACK_BRD NetMsgFB;
    NetMsgFB.dwGaeaID = m_dwGaeaID;
    NetMsgFB.emTarCrow = pNetMsg->emTarCrow;
    NetMsgFB.dwTarID = pNetMsg->dwTarID;
    NetMsgFB.dwAniSel = m_dwANISUBSELECT;

    SendMsgViewAround(&NetMsgFB);


    // 몹을 공격하면 팻이 공격모션을 취함
    if ( pTARGET->GetCrow() == CROW_MOB )
    {
        GLPetField* pMyPet = GetMyPet();
        if ( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
        {
            D3DXVECTOR3 vDist;
            float fDist;
            vDist = pMyPet->m_vPos - m_vPosition;
            fDist = D3DXVec3Length(&vDist);

            // 일정거리 안에 있으면 
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
	// 액션 취소.
	if ( isAction(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );

	// 자신의 주변 Char에게 자신의 Msg를 전달.
	//
	GLMSG::SNETPC_ATTACK_CANCEL_BRD NetMsgFB;
	NetMsgFB.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround(&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgGatheringCancel (NET_MSG_GENERIC* nmg)
{
	// 액션 취소.
	if ( isAction(GLAT_GATHERING) )	TurnAction ( GLAT_IDLE );
	return S_OK;
}

void GLChar::ReSetStartMap()
{
	// 게이트가 못 찾으면 최초 시작 위치로 이동한다.
	m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
	m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
	m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);

	// 시작위치 리셋 (각학원별 시작맵으로)
	GLMSG::SNETPC_UPDATE_STARTCALL NetMsg;
	NetMsg.dwChaNum		 = CharDbNum();
	NetMsg.sStartMapID   = m_sStartMapID;
	NetMsg.dwStartGateID = m_dwStartGate;
	NetMsg.vStartPos	 = m_vStartPos;

	// 클라이언트에 알림
	SendToClient( &NetMsg );

	// Agent에 알림
	SendToAgent(&NetMsg);

	return;
}

//! 회복할 경험치량을 알려준다. NPC
HRESULT GLChar::MsgReqGetExpRecoveryNpc (NET_MSG_GENERIC* nmg)
{
    EMSERVICE_PROVIDER ServiceProvider = GetServiceProvider();
    // 경험치회복_정의_Npc
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

//	경험치를 회복한다.
HRESULT GLChar::MsgReqRecoveryNpc (NET_MSG_GENERIC* nmg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqRecoveryNpc", m_CharDbNum ) );
	}

    EMSERVICE_PROVIDER ServiceProvider = GetServiceProvider();

    // 경험치회복_정의_Npc
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

	    // NPC 체크 
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
	    m_lnRestorableExp = 0;	// 경험치 회복 가능량 0으로 초기화		
    	
	    // 경험치 회복 성공 알림
	    MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_OK;
	    MsgFB.nReExp = nReExp;
	    SendToClient( &MsgFB );

	    SendInvenMoneyUpdate();
    	
	    //	경험치 부활 로그
	    {   
			// 통계 및 로그 수정
		    LogCharAction
			    (
			    m_CharDbNum,					//	당사자.
			    gamelog::ACT_RECOVERY_NPC,		//	행위.
			    gamelog::ID_CHAR, 0,					//	상대방.
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
// Desc: 친구이동 처리 (Agent->Field)
// *****************************************************
HRESULT GLChar::MsgReqFriendFld (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_2_FRIEND_FLD *pNetMsg = (GLMSG::SNETPC_2_FRIEND_FLD *)nmg;
	GLMSG::SNETPC_2_FRIEND_FB MsgFB;

	// 아이템 존재 유무 판단.
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

	//	케릭이 정상 상태가 아닐 경우.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	거래중일 경우.
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

	// 해당 레벨의 진입 조건 검사.
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

	// pk 등급이 살인자 등급 이상일 경우 귀환 카드의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		MsgFB.emFB = EM2FRIEND_FB_PK_CONDITION;
		SendToClient( &MsgFB );
		return E_FAIL;
	}

	// 아이템 소모.
	ConsumeInvenItem( pNetMsg->wItemPosX, pNetMsg->wItemPosY, true );

	MapID sMAPID = pNetMsg->sFriendMapID;
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS = pNetMsg->vFriendPos;


	/// 이동;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;
	sMoveInfo.vPosition = vPOS;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	/*// PET
	// 맵이동시 Pet 삭제
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	
	//VEHICLE
	m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, true );
	
	// SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	// 다른 필드 서버일 경우.
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
	// 같은 필드 서버일 경우.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다 const MapID sCurMapID(GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(GetCurrentMap());

		BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SendToClient( &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				RESETSKEFF ( i );

			// CP 초기화 맵인지 확인
			if (pMapNode->IsCombatPointReset())
				ResetCP();
		}

		ResetAction();		

		// 멥 이동 성공을 알림.
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

	/*//	케릭이 정상 상태가 아닐 경우.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( m_sCONFTING.IsCONFRONTING()  )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		SendToClient( &MsgFB );
		return FALSE;
	}

	//	거래중일 경우.
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
	// 맵이동시 Pet 삭제
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	
	//VEHICLE
	m_pGaeaServer->SetActiveVehicle( m_ClientSlot, m_dwGaeaID, false );

	// SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( IsActState(EM_ACT_MACROMODE) )
	{
		MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		m_pGaeaServer->SENDTOCLIENT ( ClientSlot(), &msg );
	}
	
	GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(m_CharDbNum, GetCurrentMap(), GetPosition(), GetFieldSvrID(), pNetMsg->mapIDTo);	
	m_pGaeaServer->SENDTOMYSELF(&_moveMapMessage);

	// 다른 필드 서버일 경우.
	if ( dwFieldServerIDTo != GetFieldSvrID() )
	{ // 인던의 경우 pMapNode의 fieldServerID로 구분 할 수 없다;
		// 이동할 맵이 인던인 경우 로그를 남김		

		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = CharDbNum();
		NetMsgAg.dwFieldServerIDTo = dwFieldServerIDTo;
		NetMsgAg.mapIDTo = _mapIDTo;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPosition = vPosition;
		SendToAgent(&NetMsgAg);
	}
	// 같은 필드 서버일 경우.
	else
	{

		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다 const MapID sCurMapID(GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(GetCurrentMap());

        BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, _mapIDTo, dwGATEID, vPosition );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SendToClient( &NetMsgFB );
            return FALSE;
        }

        // 버프와 상태이상 제거
        if ( sCurMapID != _mapIDTo )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                RESETSKEFF ( i );

            // CP 초기화 맵인지 확인
            if (pMapNode->IsCombatPointReset())
                ResetCP();
        }

		ResetAction();

		// 멥 이동 성공을 알림.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

    // 레벨업 처리
    LEVLEUP(bInitNowLevel, false, true);

	//레벨업 하기 퀘스트
	DoQuestLevelUp();

    // 렙업에 따른 퀘스트 자동 시작
    QuestStartFromGetLEVEL ( m_wLevel );

    // 퀘스트 진행 조건 도달 여부.
    //
    DoQuestLevel ();


    //mjeon.activity
    ActivityCheckLevel();


    //	[자신에게]
    GLMSG::SNETPC_REQ_LEVELUP_FB NetMsg;
    NetMsg.wLevel		 = m_wLevel;
    NetMsg.wStatsPoint	 = m_wStatsPoint;
    NetMsg.dwSkillPoint  = m_dwSkillPoint;
    NetMsg.bInitNowLevel = bInitNowLevel;
	NetMsg.bJumping = false;
	NetMsg.bCalExp = true;
    NetMsg.sMapID		 = GetCurrentMap().getBaseMapID();
    SendToClient( &NetMsg );

    //	[모두에게]
    GLMSG::SNETPC_REQ_LEVELUP_BRD NetMsgBrd( m_wLevel );
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    SendMsgViewAround(&NetMsgBrd);

	if ( isValidParty() && (GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false) )
    {
		GLMSG::SNET_PARTY_MBR_LEVEL_UP NetPartyMemeberLevelUp(m_dwGaeaID,m_wLevel);		
		SendToParty(&NetPartyMemeberLevelUp);
    }


    //	[파티,대련자] hp 변화 알림.
    MsgSendUpdateState();


    // 레벨업 로그 기록.(레벨 20이상에서)
    if (m_wLevel > 20)
    {   
		// 통계 및 로그 수정
        LogCharAction(
            m_CharDbNum,			//	당사자
            gamelog::ACT_LEV_UP,	//	행위
            gamelog::ID_CHAR,
            m_wLevel,				//	캐릭터 레벨
            m_sExperience.lnNow,	//	exp
            0,						//	bright
            0,						//	life
            0,
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());					//	money
    }

    // Agent 에 알림
    GLMSG::NET_CHAR_LEVEL_UP_FA MsgFA(m_CharDbNum, m_wLevel);
    SendToAgent(&MsgFA);

    // 클럽에 LevelUp 반영    
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

            // 랩업 수행. ( 로컬 메시지 발생. )
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// 최초 작업이 대만용이므로 무조건 190랩으로 변경
	LONGLONG nExp = GLOGICEX::GLNEEDACCUMULATEDEXP(m_wLevel, 190);
	m_sExperience.lnNow = ( nExp == 0 ) ? m_sExperience.lnNow : nExp;

    // 랩업 수행. ( 로컬 메시지 발생. )
    //
    GLMSG::SNETPC_REQ_JUMPINGLEVELUP NetMsg;
    MsgReqJumpingLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

    m_lnLastSendExp = m_sExperience.lnNow;

	//if (VALID_LEVELUP())
 //       ReqJumpingLevelUpValid();
 //   else
 //       ReqLevelUpInvalid();

		// 클라이언트에 처리 결과 전송.
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ReqJumpingLevelUpValid", m_CharDbNum ) );
	}

    bool bInitNowLevel = false;
	bool bJumping = true;

    // 레벨업 처리
    LEVLEUP(bInitNowLevel, bJumping, true);

	//레벨업 하기 퀘스트
	DoQuestLevelUp();

    // 렙업에 따른 퀘스트 자동 시작
    QuestStartFromGetLEVEL ( m_wLevel );

    // 퀘스트 진행 조건 도달 여부.
    //
    DoQuestLevel ();


    //mjeon.activity
    ActivityCheckLevel();

	//	[자신에게]
	// 강제로 랩업할때 기존 경험치 %보다 현재 %가 낮은경우 갱신이 안되기때문에 강제로 갱신 메세지를 전송
	GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
	NetMsgExp.lnNowExp = m_sExperience.lnNow;
	NetMsgExp.bJumping = TRUE;
	SendToClient(&NetMsgExp);

    //	[자신에게]
    GLMSG::SNETPC_REQ_LEVELUP_FB NetMsg;
    NetMsg.wLevel		 = m_wLevel;
    NetMsg.wStatsPoint	 = m_wStatsPoint;
    NetMsg.dwSkillPoint  = m_dwSkillPoint;
    NetMsg.bInitNowLevel = bInitNowLevel;
	NetMsg.bJumping = true;
	NetMsg.bCalExp = false;
    NetMsg.sMapID		 = GetCurrentMap().getBaseMapID();
    SendToClient( &NetMsg );

    //	[모두에게]
    GLMSG::SNETPC_REQ_LEVELUP_BRD NetMsgBrd( m_wLevel );
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    SendMsgViewAround(&NetMsgBrd);

	if ( isValidParty() && (GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false) )
    {
		GLMSG::SNET_PARTY_MBR_LEVEL_UP NetPartyMemeberLevelUp(m_dwGaeaID,m_wLevel);		
		SendToParty(&NetPartyMemeberLevelUp);
    }


    //	[파티,대련자] hp 변화 알림.
    MsgSendUpdateState();


    // 레벨업 로그 기록.(레벨 20이상에서)
		// 통계 및 로그 수정
    LogCharAction(
        m_CharDbNum,			//	당사자
        gamelog::ACT_JUMPING_LEV_UP,	//	행위
        gamelog::ID_CHAR,
        m_wLevel,				//	캐릭터 레벨
        m_sExperience.lnNow,	//	exp
        0,						//	bright
        0,						//	life
        0,
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());					//	money

    // Agent 에 알림
    GLMSG::NET_CHAR_LEVEL_UP_FA MsgFA(m_CharDbNum, m_wLevel);
    SendToAgent(&MsgFA);

    // 클럽에 LevelUp 반영    
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

            // 랩업 수행. ( 로컬 메시지 발생. )
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

	//	선택한 STATE를 증가시킴.
	STATSUP(pNetMsg->emStats);
	INIT_DATA(FALSE,FALSE);

	//	처리 결과 응답.
	GLMSG::SNETPC_REQ_STATSUP_FB NetMsg;
	NetMsg.emStats = pNetMsg->emStats;
	NetMsg.wStatsPoint = m_wStatsPoint;
	SendToClient( &NetMsg );

	//	[파티,대련자] hp 변화 알림.
	MsgSendUpdateState();

	return S_OK;
}

HRESULT GLChar::MsgReqStatsUpEx(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_STATSUP_EX *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP_EX*) nmg;

	/// 0 보다 작은 값을 보내면 안된다
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

	 // 요구 수치가 자기의 포인트 값보다 크면 안된다
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
	//	선택한 STATE를 증가시킴.
//	STATSUP(pNetMsg->emStats);

	//	처리 결과 응답.
	GLMSG::SNETPC_REQ_STATSUP_EX_FB NetMsg;
	std::copy(pNetMsg->emStatsArray,pNetMsg->emStatsArray+EMSIZE,NetMsg.emStatsArray);
	NetMsg.wStatsPoint = m_wStatsPoint;
	SendToClient( &NetMsg );

	//	[파티,대련자] hp 변화 알림.
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

	// 슬롯 행동 정보 설정.
	//
	m_sACTIONQUICK[wSLOT].wACT = pNetMsg->wACT;
	m_sACTIONQUICK[wSLOT].sNID = sITEM.GetNativeID();

	// 클라이언트에 통지.
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

	// 클라이언트에 통지.
	//
	GLMSG::SNETPC_REQ_ACTIONQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = wSLOT;
	NetMsgFB.sACT = m_sACTIONQUICK[wSLOT];
	SendToClient( &NetMsgFB );

	return S_OK;
}

// *****************************************************
// Desc: 부활위치요청 처리
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

	// 부활 위치 지정.
	//
	m_sStartMapID = GetCurrentMap().getBaseMapID();
	m_dwStartGate = pLandGate->GetGateID();

	// 부활 위치 지정에 성공하였음을 알림.
	//
	MsgFB.emFB     = EMREGEN_GATE_SUCCEED;
	MsgFB.sMapID   = m_sStartMapID;
	MsgFB.dwGateID = m_dwStartGate;
	SendToAgent(&MsgFB);

	return S_OK;

_REQ_FAIL:
	// 부활 위치 지정에 실패하였음을 알림.
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

	// 치료가능한 NPC인지 검사 필요.
	bool bCURE = pCrow->IsBasicTalk(SNpcTalk::EM_CURE);
	if (!bCURE)
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	// 치료가능한 상태인지 체크
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

	// 거리 제한 테스트
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPosition-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if (fDist > fTalkableDis)
    {
        SendToClient( &MsgFB );
        return S_OK;
    }

	// 치료시킴
	m_sHP.TO_FULL();
    m_sMP.TO_FULL();
    m_sSP.TO_FULL();
	CURE_STATEBLOW ( DIS_ALL );

	// 결과 전송
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

	// 치료가능한 NPC인지 검사 필요
	bool bRESET = pCrow->IsBasicTalk(SNpcTalk::EM_CHAR_RESET);
	if (!bRESET)
        goto _REQ_FAIL;

	// char reset 아이텡 수.
	//
	WORD wPosX(0), wPosY(0);
	bool bITEM = m_cInventory.GetCharResetItem ( wPosX, wPosY );
	if ( !bITEM )
	{
		MsgFB.emFB = EMREGEN_CHARRESET_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );
	
	// 정보 리셋. ( stats, skill )
	RESET_STATS_SKILL();

	// 만약 SKILL 사용중이라면 모두 리셋.
	//
	if ( isAction(GLAT_SKILL) )
	{
		GLMSG::SNETPC_SKILL_CANCEL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround(&NetMsgBrd);

		TurnAction(GLAT_IDLE);
	}

	// 결과 전송.
	//
	MsgFB.emFB = EMREGEN_CHARRESET_SUCCEED;
	MsgFB.dwSKILL_P = m_dwSkillPoint;
	MsgFB.dwSTATS_P = m_wStatsPoint;

	SendToClient( &MsgFB );

	return S_OK;

_REQ_FAIL:

	// 정보 리셋 실패하였음을 알림.
	//
	SendToClient( &MsgFB );

	return S_OK;
}

void GLChar::DoConftStateBackup ()
{
	// 체력 스케일 변화.
	//
	m_sCONFTING.sBACKUP_HP = m_sHP;
	m_sCONFTING.sBACKUP_MP = m_sMP;
	m_sCONFTING.sBACKUP_SP = m_sSP;

	if ( !m_mapPlayHostile.empty() )
	{
		m_sCONFTING.sOption.fHP_RATE = 1.0f;
	}

	// 체력의 최대 수치 계산.
	UPDATE_MAX_POINT ( m_sCONFTING.sOption.fHP_RATE );

	// 대련에 따른 배율 조정.
	m_sHP.TO_FULL();
	m_sMP.TO_FULL();
	m_sSP.TO_FULL();

	// 체력 변화를 알림. [자신,파티,대련자]
	//
	MsgSendUpdateState();
}

void GLChar::DoConftStateRestore ()
{
	if ( !m_sCONFTING.IsCONFRONTING() )		return;

	// 체력 환원.
	//
	m_sHP = m_sCONFTING.sBACKUP_HP;
	m_sMP = m_sCONFTING.sBACKUP_MP;
	m_sSP = m_sCONFTING.sBACKUP_SP;

	// 체력의 최대 수치 계산.
	UPDATE_MAX_POINT ( 1.0f, false );

	m_sHP.LIMIT();
	m_sMP.LIMIT();
	m_sSP.LIMIT();

	// 체력 변화를 알림. [자신,파티,대련자]
	//
	MsgSendUpdateState();

	// 행동을 취소한다.
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

	// 상대방이 죽은 상태
	if( pTAR->IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, m_dwGaeaID, EMCONFRONT_DIE_YOU, 0.0f);
		NetMsgFB.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgFB);
		return E_FAIL;
	}

	// 내가 죽은 상태
	if( IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, m_dwGaeaID, EMCONFRONT_DIE_ME, 0.0f);
		NetMsgFB.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgFB);
		return E_FAIL;
	}

	D3DXVECTOR3 vPosition = ( m_vPosition + pTAR->m_vPosition ) / 2.0f;

	// 적대행위자 삭제.
	//
	RemoveAllPlayHostiles();

	// 대련 정보 설정.
	//
	m_sCONFTING.emTYPE = pNetMsg->emTYPE;
	m_sCONFTING.dwTAR_ID = pNetMsg->dwTARID;
	m_sCONFTING.vPosition = vPosition;
	m_sCONFTING.sOption = pNetMsg->sOption;

	// 대련 시작 클라이언트에 알림.
	GLMSG::SNETPC_CONFRONT_START2_CLT NetMsgClt(pNetMsg->emTYPE, pNetMsg->dwTARID, vPosition, pNetMsg->sOption, pTAR->GetName() );
	SendToClient(&NetMsgClt);


	// 대련 시작시 소환수 제거
	//SummonAllRemove();
	KillAllChildCrow();

	// 체력 백업.
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

    // 클라이언트에 대련 취소를 통보.
	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgClt(emEND);
	SendToClient(&NetMsgClt);

	// 대련 종료 메시지.
	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd(emEND);
	NetMsgTarBrd.dwGaeaID = m_dwGaeaID;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			//	파티에 있는 '대련' 참가정보를 리셋.
			GLPartyField* const pParty = GetMyParty();
			if ( pParty )
			{
				//	대련 리스트에서 제거.
				pParty->MBR_CONFRONT_LEAVE(m_dwGaeaID);

				//	파티원에게 대련에서 빠져 나감을 알림.
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

				//	클럽원에게 대련에서 빠져나감을 알림.
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

			//  대련 종료 효과 처리
            EndConfting();
            
			if( m_mapID == pMaster->GetCurrentMap() )
			{	
				// 주변 유닛들에 대련종료 통보.
				if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
				{
					SendMsgViewAround(&NetMsgTarBrd);
				}
				// 대련 종료, 무적 타임 설정 (자신 & 적)
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

			//  대련 종료 효과 처리
            EndConfting();
           
			if( m_mapID == pMASTER->GetCurrentMap() )
			{
				// 주변 유닛들에 대련종료 통보.
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
			// 주변 유닛들에 대련종료 통보.
			if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
			{
				SendMsgViewAround(&NetMsgTarBrd);
			}

			//  대련 종료 효과 처리
            EndConfting();
            
			m_sCONFTING.SETPOWERFULTIME ( 10.0f );

			//
			//mjeon.attendance
			//
			//AttendanceCheckSparingSolo();
		}
		break;
	}
	// 행동을 취소한다.
	TurnAction ( GLAT_IDLE );

	return S_OK;
}

HRESULT GLChar::MsgActState (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_ACTSTATE *pNetMsg = (GLMSG::SNETPC_ACTSTATE *)nmg;

	const DWORD dwOldActState = GetActState();

	// 배틀로얄 서버에서 강제적으로 Visible 모드 수정가능하도록 체크항목 추가.

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

    // 전투상태에서는 사용불가;
    if ( !pLand->IsPeaceZone() && !IsActState(EM_ACT_PEACEMODE) )
    {
        return S_FALSE;
    }
    // 탈것 탑승중일때 불가; // 살아있지않을 경우 불가; // 대기 상태가 아닐경우 불가;
    if ( ( m_emVehicle ) || ( !IsValidBody() ) || ( isAction( GLAT_IDLE ) == false ) )
    {
        return S_FALSE;
    }

	// 제스쳐 액션
	TurnAction ( GLAT_TALK );
	m_dwANISUBSELECT = pNetMsg->dwID;

	// 제스쳐 행위를 모든 주변 사람들에게 보냄.
	GLMSG::SNETPC_REQ_GESTURE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = pNetMsg->dwID;

	SendMsgViewAround(&NetMsgBrd);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// 수정사항
// 2005-12-27 Jgkim
// 중국, 말레이지아에서 NpcTalk 파일을 개조하여 불법아이템으로 교환받음	
//
// pNetMsg->dwA_NID; // A:npc에게 줄 아이템
// pNetMsg->dwB_NID; // b:npc에게 받을 아이템
// pNetMsg->dwNpcID; // 요청을 받는 NPC ID
//
// 1. 요청한 NPC 가 있는지 확인
// 2. A 아이템을 캐릭터가 가지고 있는지 확인
// 3. NPC 가 B 아이템을 줄 수 있는지 확인
// 4. NPC 가 B 아이템을 줄 수 있다면, 캐릭터의 A 아이템을 소멸후 B 아이템을 지급
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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

	//	백업으로 복사하고 넣을수 있는지 체크한다.
	//	복사 비용이 상당하지만 한번더 체크하는게 나을것 같다.
	GLInventory cInventory;
	cInventory.Assign( m_cInventory );

	//	일단 필요한아이템을 찾아서 인벤토리에서 삭제 한다.
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

	//	보상 아이템을 넣어본다. 인벤토리에 들어가는지...
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
			//	겹침 아이템일 경우.
			SITEMCUSTOM sITEM_NEW( pItem->sBasicOp.sNativeID );
			sITEM_NEW.wTurnNum = static_cast< WORD >( vecTradeItem[i].dwNum * pItem->GETAPPLYNUM() );	

			//	인벤에 여유공간이 없을 경우 취소됨.
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

	//	점검이 완료되었다. 실작업을 수행해야 한다.
	//	일단 필요한아이템을 찾아서 인벤토리에서 삭제 한다.
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
				//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.				
				sc::writeLogError( std::string( "NPC ITEM TRADE Critical Error" ) );
				MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
                SendToClient( &MsgFB );
                return S_OK;
			}

			if ( nNum < pInvenItem->sItemCustom.wTurnNum ) 
			{
				pInvenItem->sItemCustom.wTurnNum -= nNum;
				nNum = 0;

				// 클라이언트에 갯수 변경을 알림.
				NetMsgUpdate.wPosX = pInvenItem->wPosX;
				NetMsgUpdate.wPosY = pInvenItem->wPosY;
				NetMsgUpdate.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
				SendToClient( &NetMsgUpdate );
			}
			else
			{
				nNum -= pInvenItem->sItemCustom.wTurnNum;

				// 아이템의 소유 이전 경로 기록.				
				// 통계 및 로그 수정
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

	// 보상아이템 넣는다.
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
			// 겹침가능 아이템에 랜덤옵션을 넣어주고 있다. 동작하지 않도록 함
			//if ( sITEM_NEW2.GENERATE_RANDOM_OPT( false ) )
			//	LogItemRandomOption( sITEM_NEW2 );

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW2;

			//	인벤에 여유공간이 없을 경우 취소됨.
			if ( !InsertToInven ( &cDropItem, true, true, false, false, true ) )
			{
				//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
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
				//	랜덤 옵션때문에 복사한다.
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

	// DB에 저장요청
	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterPhoneNumberSet(m_ClientSlot, m_CharDbNum, pNetMsg->szPhoneNumber)));    
	return S_OK;
}
*/
/*
// 폰번호 변경 결과 처리
HRESULT GLChar::MsgPhoneNumber(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *)nmg;

	// 클라이언트에 처리 결과 전송.
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

	// 폰번호 변경을 친구맴버들에게 알림
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
        ActiveVehicleTrue(); // 활성화	
		if ( GLHIDESET::CheckHideSet(EMHIDE_GETON_VEHICLE) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}
    else 
    {
        ActiveVehicleFalse(bMoveMap); // 비활성화
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

    // 탈것 탈때 소환수 제거
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

    // 주변에 알림
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

    // 주변에 알림은 운전자만 함;
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
	// 활성화
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

		// 탈것 탈때 소환수 제거
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



		// 주변에 알림
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
	// 비활성화
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

		// 주변에 알림은 운전자만 함;
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

	// 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.

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
        // 일반오류
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
		// 일반오류
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
		return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_F )
	{
		// 탈것용 아이템이 아닐때
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 서로 다른 타입일때
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

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.

    WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
    WORD wPosY = sInven_BackUp.wPosY;

	SINVENITEM* pInsertedItem = NULL;

	// #item
	// vehicle puton(Inven) <-> inven(Hold)
	// vehicle puton 아이템(Inven) pNetMsg->emSuit, sSlotItemCustom
	// inven 아이템(Hold) wPosX, wPosY, sInven_BackUp, sHoldItemCustom

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

	// 탈것 장착창 디비 처리
	VehiclePutOnItemUpdate( pNetMsg->emSuit, true );

	// 인벤 아이템 디비 업데이트
	InvenUpdateItem( pInsertedItem, true );

	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;			//	삭제될 아이템.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pInsertedItem;				//	삽입되는 아이템.

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, NetMsg_Del_Insert );
	SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	SITEMCUSTOM VehiclePutonItem = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );

	// 자신에게 알림
	NetMsgFB.emFB                   = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
    NetMsgFB.sHoldtoSlotItemCustom  = VehiclePutonItem;
	NetMsgFB.emSuit                 = pNetMsg->emSuit;
	SendToClient( &NetMsgFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 보드가 출력되어야 한다
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
        // 일반오류
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
		// 일반오류
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
		return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_F )
	{
		// 탈것용 아이템이 아닐때
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	// 서로 다른 타입일때
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

    SINVENITEM sInven_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.

    WORD wPosX = sInven_BackUp.wPosX;			//	아이템 위치.
    WORD wPosY = sInven_BackUp.wPosY;

	// 저장위치만 변경되기 때문에 디비에서는 삭제하지 않는다.
    if ( !InvenDeleteItem( wPosX, wPosY, true, sHoldItemCustom.bVietnamGainItem, 1, false ) )
        return E_FAIL;

	// 손에든 아이템을 팻에게 장착시키고
	m_pVehicle->SetSlotItem( pNetMsg->emSuit, sHoldItemCustom );

	// 디비 처리
	VehiclePutOnItemUpdate( pNetMsg->emSuit, true );

	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = m_pVehicle->GetSlotitembySuittype( pNetMsg->emSuit );
	SendToClient( &NetMsgFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
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
		// 일반오류
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

    // 장착된 아이템을 인벤토리로.
    if ( !InvenInsertItem( sSlotItemCustom, pNetMsg->wPosX, pNetMsg->wPosY, true, 1, false ) )
        return E_FAIL;

	// 디비 업데이트, InvenInsertItem 에서 디비 업데이트 해도 되지만 저장 위치만 변경되어야 되기 때문에 이거로 처리한다.
	InvenUpdateItem( m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY ), true );

	// 장착된 아이템을 제거한다. 여기는 디비 작업 필요 없다.
	m_pVehicle->ReSetSlotItem( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

    GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB NetMsgRemoveFB;
    NetMsgRemoveFB.emSuit = pNetMsg->emSuit;
    SendToClient( &NetMsgRemoveFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = SITEMCUSTOM ( NATIVEID_NULL() );
	SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

// 탈것 장착창 오른클릭하면 여기로 오는듯
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
		// 일반 오류
		return E_FAIL;
	}

	WORD wPosX( 0 ), wPosY( 0 );
	BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		return E_FAIL;
	}

	// 인벤에 넣기
    if ( !InvenInsertItem( sSlotItemCustom, wPosX, wPosY, true, 1, false ) )
        return E_FAIL;

	// 디비 업데이트, InvenInsertItem 에서 디비 업데이트 해도 되지만 저장 위치만 변경되어야 되기 때문에 이거로 처리한다.
	InvenUpdateItem( m_cInventory.GetItem( wPosX, wPosY ), true );

	// 슬롯아이템 제거, 여기는 디비 작업 필요 없다.
	m_pVehicle->ReSetSlotItem ( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_pVehicle->GetSpeedRate();
	m_fVehicleSpeedVol = m_pVehicle->GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB NetMsgFB;
	NetMsgFB.emSuit = pNetMsg->emSuit;
	SendToClient( &NetMsgFB );

	// 슬롯아이템 제거를 알림
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

	// 탈것 여부 체크
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

	// 배터리 여부 체크
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

	// pHoldInvenItem 아이템이 변경되는 것이 없는 함수에서도 저렇게 굳이 복사해서 좌표만 사용한다.
    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
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
			//	손에 든 아이템 확인한다.
			//	아이템이 바뀌는 경우가 있다.
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

			// 보드 포만감 이전 로그
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

			// pHoldInvenItem 아이템이 변경되는 것이 없는 함수에서도 저렇게 굳이 복사해서 좌표만 사용하고 있는거 수정함
            //SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
            //WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
            //WORD wHoldPosY = sHold_BackUp.wPosY;

			//ConsumeInvenItem( wHoldPosX, wHoldPosY );
            ConsumeInvenItem( pHoldInvenItem->wPosX, pHoldInvenItem->wPosY );

			// 로그
			LogVehicleAction(
                pIntMsg->m_VehicleDbNum,
                pIntMsg->sBatteryID,
                EMVEHICLE_ACTION_BATTERY_AFTER,
                nFull );

			// 현재 로직에 가진 탈것과 동일한거라면 로직을 수정해준다.
			// 정보가 비활성화 되어있지만 나중에 바로 사용하기 위해 갱신
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

	// 의상 색상을 변화 시킴.
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
					//	Memo : 컬러 변경을 할 수 없는 아이템 입니다. by luxes.
					MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM;
					SendToClient( &MsgFB );
					return E_FAIL;
				}
			}
		}
		else
		{
			//	Memo : 컬러 변경을 할 수 없는 아이템 입니다. by luxes.
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
		// Memo : 기능이 제한된 아이템일 경우.
		if ( !CheckCostumColorChange( pNetMsg->dwMainColor, pNetMsg->dwSubColor ) )
		{	// Memo : 색상 값이 제한 범위를 벗어납니다.
			MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_RESTRICT;
			SendToClient( &MsgFB );
			return E_FAIL;
		}
	}

	m_PutOnItems[ emSlot ].dwMainColor = pNetMsg->dwMainColor;
	m_PutOnItems[ emSlot ].dwSubColor = pNetMsg->dwSubColor;

	// 클라이언트에 처리 결과 전송.
	MsgFB.emSlot = emSlot;

	MsgFB.dwMainColor = m_PutOnItems[ emSlot ].dwMainColor;
	MsgFB.dwSubColor = m_PutOnItems[ emSlot ].dwSubColor;

	MsgFB.emFB = EMINVEN_COSTUM_COLOR_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;

	NetMsgBrd.emSlot = emSlot;

	NetMsgBrd.dwMainColor = m_PutOnItems[ emSlot ].dwMainColor;
	NetMsgBrd.dwSubColor = m_PutOnItems[ emSlot ].dwSubColor;

	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
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

	// Note : 레벨 조건 확인
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	// 헤어를 변화 시킴.
	//
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		m_pVehicle->m_sColor[ i ].dwMainColor = pNetMsg->dwMainColor[ i ];
		m_pVehicle->m_sColor[ i ].dwSubColor = pNetMsg->dwSubColor[ i ];
	}

	// 클라이언트에 처리 결과 전송.
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		MsgFB.dwMainColor[ i ] = m_pVehicle->m_sColor[ i ].dwMainColor;
		MsgFB.dwSubColor[ i ] = m_pVehicle->m_sColor[ i ].dwSubColor;
	}

	MsgFB.emFB = EMINVEN_VEHICLE_CHANGE_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		NetMsgBrd.dwMainColor[ i ] = m_pVehicle->m_sColor[ i ].dwMainColor;
		NetMsgBrd.dwSubColor[ i ] = m_pVehicle->m_sColor[ i ].dwSubColor;
	}

	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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

	// Note : 레벨 조건 확인
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

	// 클라이언트에 처리 결과 전송.
	MsgFB.dwBoosterID = m_pVehicle->m_dwBoosterID;
	MsgFB.emFB = EMINVEN_LEARN_BIKE_BOOST_FB_OK;
	SendToClient( &MsgFB );

	// 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_REQ_BIKE_BOOST_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwBoosterID = m_pVehicle->m_dwBoosterID;
	SendMsgViewAround( &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
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
	// 클라이언트 포만도 갱신
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
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	if ( pMaterial->GetCrow() != CROW_MATERIAL )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOTTYPE;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	if ( !pMaterial->IsValidBody()  )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	// 사망확인
	if ( !IsValidBody() )
		return E_FAIL;

	if ( isAction( GLAT_GATHERING ) )
	{
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_GATHERING;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	//	거리 체크
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
		//	거리가 멉니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_DISTANCE;
		SendToClient( &NetMsg );

		return E_FAIL;
	}


	// 이미 다른사람이 사용중이라면???

	if ( pMaterial->m_dwGatherCharID != GAEAID_NULL )
	{
		//	거리가 멉니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_USE;
		SendToClient( &NetMsg );
		return E_FAIL;
	}

	int nTime = sc::Random::RandomNumber( pMaterial->m_pCrowData->m_wGatherTimeLow, pMaterial->m_pCrowData->m_wGatherTimeHigh );
	// crow에 셋팅
	pMaterial->m_dwGatherCharID = m_dwGaeaID;
	pMaterial->m_fGatherTime = (float)nTime;

	// random 타입 구현후에 시간 체크

	//	User에 셋팅
	TurnAction ( GLAT_GATHERING );

	m_dwANISUBSELECT = pMaterial->m_pCrowData->m_wGatherAnimation;

	//	거리가 멉니다.
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

	// NpcTalk 유효성 체크
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

	// Note : 레벨 조건 확인
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
	// 사용하는 압축기에 따라 생성될 캡슐 종류 결정
	// 현재는 비율이 동일하기때문에 타입에 따라 캡슐이 고정이지만 만약 비율이 여러가지 생긴다면 여기서 계산해서 캡슐 타입을 결정해줘야함
	// 1억 경험치가 기준이므로 아이템 타입에 따라서 배율을 계산
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
	
	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
	NetMsgExp.lnNowExp = m_sExperience.lnNow;
	SendToClient( &NetMsgExp );

	AddGameAdoJob( db::DbActionPtr( new db::CalculateExpCompressor(m_CharDbNum, m_sExperience.lnNow, llNeedExp + llOverExp ) ) );

    LogCharAction
	    (
	    m_CharDbNum,					//	당사자.
	    gamelog::ACT_EXP_COMPRESSOR,	//	행위.
	    gamelog::ID_CHAR, 0,			//	상대방.
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

	// Note : 레벨 조건 확인
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

	//	[자신에게] Inventory 아이템 소모시킴.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	LONGLONG llExp = 100000000;
	llExp *= llRatioExp;

	m_sExperience.lnNow += ( llExp <= 0 ) ? 0 : llExp;
	
    // 랩업 수행. ( 로컬 메시지 발생. )
    //
	if( m_sExperience.lnNow >= m_sExperience.lnMax && m_wLevel < GLCONST_CHAR::wMAX_LEVEL )
	{
		GLMSG::SNETPC_REQ_JUMPINGLEVELUP NetMsg;
		MsgReqJumpingLevelUp ( (NET_MSG_GENERIC*) &NetMsg );
	}
	// 랩업이 아니면 경험치 갱신
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
