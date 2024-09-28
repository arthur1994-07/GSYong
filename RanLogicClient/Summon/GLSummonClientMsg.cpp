#include "../pch.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/MSG/GLContrlSkillMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Character/GLCharDefine.h"

#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Effect/GLFactEffect.h"
#include "../GLGaeaClient.h"
#include "../GfxInterfaceBase.h"
#include "../Strike/GLStrikeM.h"
#include "../Char/GLCharClient.h"
#include "./GLSummonClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLSummonClient::MsgUpdateEffect(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_SUMMON_EFFECT* pNetMsg =
		static_cast< GLMSG::SNET_SUMMON_EFFECT* >( nmg );

	if ( pNetMsg )
	{
		SNATIVEID sSkillID = pNetMsg->sSkillID;
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData (
			sSkillID.wMainID, sSkillID.wSubID );
		if ( !pSkill )
			return;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		STARGETID sTARG( GETCROW(), m_dwGaeaID );
		sTARG.vPos = m_pGaeaClient->GetTargetPos( sTARG );

		SK_EFF_TARZONE (
			sTARG,
			SKILL::EMPOS_GROUND,
			pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );
	}
}

void GLSummonClient::MsgUpdateCurSkillPage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_UPDATE_CURSKILLPAGE* pNetMsg =
		static_cast< GLMSG::SNET_SUMMON_UPDATE_CURSKILLPAGE* >( nmg );

	if ( pNetMsg )
	{
		// 초기화;
		m_dwAType = 0;

		// 페이지 변경;
		m_dwCurSkillPage = pNetMsg->dwCurSkillPage;
	}
}

void GLSummonClient::MsgUpdateState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_UPDATE_STATE* pNetMsg = ( GLMSG::SNET_SUMMON_UPDATE_STATE* ) nmg;

	m_fCurSpeed = pNetMsg->fSpeed;
	m_dwNowHP   = pNetMsg->dwHPNow;
	
	// Note : 상태 체크
	DWORD dwActionFlag = pNetMsg->dwActionFlag;

	// Note : 공격 중이면 알아서 서버에서 처리 하게끔한다.
	if ( isAction( GLAT_ATTACK, GLAT_SKILL ) )
		return;

	/*if ( dwActionFlag&EM_SUMMONACT_ATTACK || dwActionFlag&EM_SUMMONACT_SKILL )
	{
		return;
	}*/

	// Note : 생성 중이면
	if ( dwActionFlag&EM_ACT_CREATE )
	{
		if ( !isAction(GLAT_IDLE) || !IsActState(EM_ACT_CREATE) )
		{
			TurnAction(GLAT_IDLE);
			SetActState(EM_ACT_CREATE);
			m_emGestureState = EM_SUMMONGESTURE_NONE;
		}
	}
	// Note : 제스쳐 중이면
	else if ( dwActionFlag&EM_ACT_GESTURE )
	{
		if ( m_emGestureableArray[pNetMsg->emGesture] != EM_SUMMONGESTURE_NONE )
		{
			if ( !isAction(GLAT_IDLE) || m_emGestureState != pNetMsg->emGesture )
			{
				TurnAction(GLAT_IDLE);
				SetActState(EM_ACT_GESTURE);
				m_emGestureState = pNetMsg->emGesture;
			}
		}
	}
}

void GLSummonClient::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	if ( !m_pSummonCrowData ) return;

	GLMSG::SNET_SUMMON_GOTO* pNetMsg = ( GLMSG::SNET_SUMMON_GOTO* ) nmg;

	m_sTargetID    = pNetMsg->sTarID;
	m_fCurSpeed	   = pNetMsg->fSpeed;
	ReSetActState(~pNetMsg->dwFlag);
	SetActState(pNetMsg->dwFlag);	

	ClientActor::GotoLocation(pNetMsg->vTarPos, 10.0f);

	//BOOL bReqRun = pNetMsg->dwFlag&EM_SUMMONACT_RUN;

	ClientActor::SetMaxSpeed ( GetMoveVelo() );

	TurnAction( GLAT_MOVE );
}

void GLSummonClient::MsgAttack ( NET_MSG_GENERIC* nmg )
{
	// 이동중이면 이동을 마친후 공격 모션을 취한다.
	if ( ClientActor::PathIsActive () )
		ClientActor::StopActorMove();

	GLMSG::SNET_SUMMON_ATTACK* pNetMsg = ( GLMSG::SNET_SUMMON_ATTACK* ) nmg;

	m_sTargetID	   = pNetMsg->sTarID;
	m_dwAType	   = pNetMsg->dwAType;

	//ReSetActState(~EM_SUMMONACT_ATTACK);
	//SetActState(EM_SUMMONACT_ATTACK);	
	TurnAction( GLAT_ATTACK );

	const D3DXVECTOR3& vPosition = GetPosition();
	if ( pNetMsg->sTarID.emCrow == CROW_PC )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap ();
		std::tr1::shared_ptr<GLCharClient> pChar = pLandMan->GetChar ( pNetMsg->sTarID.GaeaId );
		D3DXVECTOR3 vDirection;
		if ( pChar ) // 방향 전환			
			vDirection = pChar->GetPosition () - vPosition;				
		else // 다른 PC 가 아니면 플레이어 케릭터이다
			vDirection = m_pGaeaClient->GetCharacter()->GetPosition () - vPosition;

		D3DXVec3Normalize ( &vDirection, &vDirection );
		ClientActor::SetDirect(vDirection);

		m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_MOB )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap ();
		std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pNetMsg->sTarID.GaeaId);
		if (spCrow)
		{
			// 방향 전환
			D3DXVECTOR3 vDirection = spCrow->GetPosition () - vPosition;
			D3DXVec3Normalize(&vDirection, &vDirection);
			ClientActor::SetDirect(vDirection);
			m_pSkinChar->SELECTANI(AN_ATTACK, AN_SUB_NONE);
		}
	}
}

void GLSummonClient::MsgSkill ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_SKILL *pNetMsg = (GLMSG::SNET_SUMMON_SKILL *)nmg;

	m_sTargetID        = pNetMsg->sTarID;
	m_dwAType          = pNetMsg->dwAType;
	m_bUsingEventSkill = pNetMsg->bUsingEventSkill;
	m_idACTIVESKILL    = pNetMsg->skill_id;
	m_wACTIVESKILL_LVL = pNetMsg->skill_lev;
	m_vTARPOS          = pNetMsg->vTARPOS;

	m_wTARNUM = pNetMsg->wTARNUM;
	if( m_wTARNUM > EMTARGET_NET )	
	{
		return;
	}
	else
	{
		memcpy ( m_sTARIDS, pNetMsg->sTARIDS, sizeof(STARID)*m_wTARNUM );
		//ReSetActState(~EM_SUMMONACT_SKILL);
		//SetActState(EM_SUMMONACT_SKILL);

		//////////////////////////////////////////////////////////////////////////
		// GLAT_SKILL_WAIT 를 사용을 안 했다. 시작과 동시에 나오는 Effect 의 경우 아주 미세한 오차가 생길 수 있다.
		// 
		TurnAction ( GLAT_SKILL );
	}
}

void GLSummonClient::MsgSkillEnd(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_SUMMON_END_SKILL* pPacket =
		static_cast< GLMSG::SNET_SUMMON_END_SKILL* >( nmg );

	UPDATE_DATA_END ( pPacket->dwSkillfactIndex );
}

void GLSummonClient::MsgSkillFact ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)nmg;

	if( pNetMsg->emCrow != CROW_SUMMON ) return;

	GLOGICEX::VARIATION ( m_dwNowHP, m_pSummonCrowData->m_dwHP, pNetMsg->nVAR_HP );
	GLOGICEX::VARIATION ( m_wNowMP, m_pSummonCrowData->m_wMP, pNetMsg->nVAR_MP );
	GLOGICEX::VARIATION ( m_wNowSP, m_pSummonCrowData->m_wSP, pNetMsg->nVAR_SP );

	//	데미지 메시지.
	if ( pNetMsg->nVAR_HP < 0 )
	{
		if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_SHOCK )	
		{
			//					SetSTATE ( EM_SUMMONACT_SHOCK );
			TurnAction ( GLAT_SHOCK );
		}
		else					ReceiveSwing ();

		//	Note : 방어스킬의 이펙트가 있을때 발동시킴.
		STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
		sACTOR.vPos = m_pGaeaClient->GetTargetPos ( sACTOR );

		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos,
			-pNetMsg->nVAR_HP, pNetMsg->dwDamageFlag, UI_ATTACK, sACTOR, m_sTargetID );

		SKT_EFF_HOLDOUT ( sACTOR, pNetMsg->dwDamageFlag );

		if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
		{
			// 강한타격 이펙트

			const D3DXVECTOR3& vPosition = GetPosition();

			D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y + 10.0f;
			matTrans._43 = vPosition.z;

			//	Note : 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, FALSE );
		}

		if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CA )
		{	
			// 강한타격 이펙트
			const D3DXVECTOR3& vPosition = GetPosition();
			STARGETID sMyID(CROW_SUMMON,m_dwGaeaID, vPosition);

			D3DXMATRIX matTrans;
			D3DXMatrixTranslation(&matTrans, vPosition.x, vPosition.y, vPosition.z );

			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y + 10.0f;
			matTrans._43 = vPosition.z;

			//	Note : 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCA_EFFECT.c_str(), matTrans, &sMyID, TRUE, FALSE );
		}

		m_pGaeaClient->logDamageRecord(sACTOR, -pNetMsg->nVAR_HP, pNetMsg->sSkillID, pNetMsg->wSkillLevel, pNetMsg->dwDamageFlag);
	}

	//	힐링 메시지.
	//if ( pNetMsg->nVAR_HP > 0 )
	//{
	//	CPlayInterface::GetInstance().InsertText ( GetPosition(), static_cast<WORD>(pNetMsg->nVAR_HP), pNetMsg->bCRITICAL, 1 );
	//}
}

// 스킬 효과 적용;
void GLSummonClient::MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_APPLY_SKILLEFFECT_FC* pNetMsg =
		static_cast< GLMSG::SNET_APPLY_SKILLEFFECT_FC* >( pMsg );
	if ( !pNetMsg )
		return;

	m_bForcedSkillFactsSpecial[ pNetMsg->emSpecAddon ] = pNetMsg->bState;
}

void GLSummonClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	const STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_SUMMON_EFFECT:					MsgUpdateEffect ( nmg );	break;
	case NET_MSG_GCTRL_SUMMON_UPDATE_CUR_SKILLPAGE:		MsgUpdateCurSkillPage ( nmg ); break;
	case NET_MSG_GCTRL_SUMMON_UPDATE_STATE: 		    MsgUpdateState ( nmg );     break;
	case NET_MSG_GCTRL_SUMMON_GOTO:						MsgGoto ( nmg );			break;
	case NET_MSG_GCTRL_SUMMON_ATTACK:				    MsgAttack ( nmg );			break;
	case NET_MSG_GCTRL_SUMMON_SKILL:	                MsgSkill ( nmg );           break;
	case NET_MSG_GCTRL_SUMMON_END_SKILL:				MsgSkillEnd ( nmg );		break;
	case NET_MSG_GCTRL_SKILLFACT_BRD:					MsgSkillFact ( nmg );		break;

	case NET_MSG_GCTRL_SUMMON_ATTACK_BLOCKING:
		{
			GLMSG::SNET_SUMMON_ATTACK_BLOCKING *pNetMsg = (GLMSG::SNET_SUMMON_ATTACK_BLOCKING *) nmg;
			ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor == NULL )
				break;

			pActor->ReceiveBlocking(ActorID);
			m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
		}
		break;

	case NET_MSG_GCTRL_SUMMON_ATTACK_AVOID:
		{
			GLMSG::SNET_SUMMON_ATTACK_AVOID *pNetMsg = (GLMSG::SNET_SUMMON_ATTACK_AVOID *) nmg;
			ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor == NULL )
				break;

			pActor->ReceiveAVoid( ActorID );
			m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
		}
		break;

	case NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE:
		{
			GLMSG::SNET_SUMMON_ATTACK_DAMAGE *pNetMsg = (GLMSG::SNET_SUMMON_ATTACK_DAMAGE *) nmg;

			ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )
				pActor->ReceiveDamage( pNetMsg->nDamage, pNetMsg->dwDamageFlag, pNetMsg->wSchool, ActorID);
			m_pGaeaClient->logDamageRecord(ActorID, pNetMsg->nDamage);

			if ( (pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRITICAL ) || ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) )	
				DxViewPort::GetInstance().CameraQuake ( 0.5f, 1.0f/25.0f, 1.0f );
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)nmg;

			//	지속형 스킬의 경우 스킬 팩터 추가됨.
            if ( pNetMsg->skill_id != SNATIVEID(false) )
            {
				switch( pNetMsg->sEffInfo.eType )
				{
				case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_ATTACK :
					{
						RECEIVE_SKILLCA( pNetMsg );
					}
					break;
				case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET :
					{
						RECEIVE_SKILLFACT ( pNetMsg );
					}
					break;
				default :
					{
						RECEIVE_SKILLFACT ( pNetMsg->skill_id
							, pNetMsg->wLEVEL
							, pNetMsg->wSELSLOT
							, pNetMsg->sID 
							, pNetMsg->sTIME
							);
					}
					break;
				}
				/*
                RECEIVE_SKILLFACT ( pNetMsg->skill_id
                    , pNetMsg->wLEVEL
                    , pNetMsg->wSELSLOT
                    , pNetMsg->sID
                    , pNetMsg->sTIME
                    );
				*/
				FACTEFF::NewSkillFactEffect (
					ActorID, m_pSkinChar, pNetMsg->skill_id, GetTransMatrix(), GetDirect());
			}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_RS_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD *)nmg;

			//	Note : 스킬 fact 들을 종료.
			//		바로 리샛 하지 않고 여기서 시간 조종하여 정상 종료되게 함. ( 이팩트 종료 때문. )
			for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
			{
				if ( pNetMsg->bRESET[i] && m_sSKILLFACT[i].sNATIVEID != NATIVEID_NULL() )
				{
					FACTEFF::DeleteSkillFactEffect (ActorID, m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

					RESETSKEFF(i);
				}
			}
		}
		break;

	case NET_MSG_GCTRL_STATEBLOW_BRD:
		{
			GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)nmg;

			const D3DXVECTOR3& vPosition = GetPosition();

			//	Note : "단독상태이상"일 경우 단독 효과 모두 끄기.
			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
			{
				FACTEFF::DeleteBlowSingleEffect(ActorID, m_pSkinChar, m_sSTATEBLOWS );

				for ( int i=0; i<EMBLOW_MULTI; ++i ) m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
			}

			//	Note : 상태 이상 추가.
			SSTATEBLOW *pSTATEBLOW = NULL;
			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )		pSTATEBLOW = &m_sSTATEBLOWS[0];
			else										pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];
			pSTATEBLOW->emBLOW = pNetMsg->emBLOW;
			pSTATEBLOW->fAGE = pNetMsg->fAGE;
			pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
            pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;
            pSTATEBLOW->wSchool = pNetMsg->wSchool;

			//	Note : 효과 생성.
			FACTEFF::NewBlowEffect (
				ActorID,
				m_pSkinChar, pSTATEBLOW->emBLOW, GetTransMatrix(), GetDirect() );
		}
		break;

	case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
		{
			GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)nmg;

			for ( int i=0; i<EMBLOW_MULTI; ++i )
			{
				EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;
				if ( emBLOW==EMBLOW_NONE )		continue;

				EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
				if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

				DISABLEBLOW(i);
				FACTEFF::DeleteBlowEffect (ActorID, m_pSkinChar, emBLOW );
			}
		}
		break;

	case NET_MSG_GCTRL_PUSHPULL_BRD:
		{
			GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)nmg;
			const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

			//	Note : 밀려날 위치로 이동 시도.
			//
			const BOOL bSucceed = ClientActor::GotoLocation(vMovePos, 5.0f);

			if ( bSucceed )
			{
				//	Note : 밀려나는 엑션 시작.
				//
				m_sTargetID.vPos = vMovePos;
				m_wAniSub = (WORD)pNetMsg->dwAniType;

				TurnAction ( GLAT_PUSHPULL );

				//	Note : 밀리는 속도 설정.
				//
				ClientActor::SetMaxSpeed ( pNetMsg->fPUSH_SPEED );
			}
		}
		break;

	case NET_MSG_GCTRL_ACTION_BRD:
		{
			GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)nmg;
			TurnAction ( pNetMsg->emAction );
		}
		break;

	case NET_MSG_GCTRL_SUMMON_LANDEFFECT_UPDATE:
		{
			GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE* pNetMsg = ( GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE* ) nmg;
			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			if ( pLand == NULL )
				break;
			const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
			if( pLevelEtcFunc == NULL )
				break;

			if ( pNetMsg->bADD )
				ADDLANDEFF( pNetMsg->landEffect );
			else
				DELLANDEFF( pNetMsg->landEffect );
		}
		break;

	case NET_MSG_GCTRL_SUMMON_MAPEFFECT_UPDATE:
		{
			GLMSG::SNET_SUMMON_MAPEFFECT_UPDATE* pNetMsg = ( GLMSG::SNET_SUMMON_MAPEFFECT_UPDATE* ) nmg;
			
			if ( pNetMsg->bADD )
				ADDMAPEFF( pNetMsg->sMapEffect );
			else
				DELMAPEFF( pNetMsg->sMapEffect );
		}
		break;

	default:
		CDebugSet::ToListView ( "[WARNING]GLSummonClient::MsgProcess Illigal Message(%d)", nmg->nType );
		break;
	};
}

