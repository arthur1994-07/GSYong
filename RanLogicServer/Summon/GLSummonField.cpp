#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystem.h"

#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubField.h"
#include "../Club/GLClubManField.h"
#include "../Guidance/GLGuidance.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Land/GLLandMan.h"
#include "../Party/GLPartyField.h"
#include "../PK/GLSchoolFreePK.h"
#include "../Util/GLItemLimit.h"
#include "../Util/StlFuncServer.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "./GLSummonField.h"

#include "../AI/CAICompField.h"
#include "../AI/GLFSMContainer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSummonField::GLSummonField(GLGaeaServer* pGaeaServer) 
: ServerActor(pGaeaServer, CROW_SUMMON)
, m_pSkinChar(NULL)
, m_vBackupPosition(D3DXVECTOR3(0,0,0))
, m_vTarPos(D3DXVECTOR3(0,0,0))

, m_pQuadNode(NULL)
, m_pCellNode(NULL)

, m_dwAtypeAfterTracing(SCROWDATA::EMMAXATTACK)
, m_wRangeTracing(0)

, m_wAniSub(0)

, m_fattTIMER(0.0f)
, m_dwReactCount(0)

//, m_pAttackProp(NULL)

, m_vStartPos(0,0,0)

, m_fIdleTime(0.0f)

, m_ScheduledAction()

, m_bRunAway(false)

, m_dwEventSkillIdx( 0 )
, m_bEventSkill(false)
, m_bDelay(false)
, m_fDelayTime(10.f)
, m_bUnlimit(false)
, m_bSpeedUp(false)
, m_nSkillFramCount( 0 )
, m_bHire( false )
{

	m_ARoundSlot.SetSummon ( this );
	m_TargetID.RESET();
	m_sHITARRAY.clear();

	// Note : 지형효과 초기화.
	m_vecLandEffect.clear();
	m_vecMapEffect.clear();

	for (int i = 0; i < CROW_SKILLFACT_SIZE; i++ )
		RESETSKEFF( i );
}

GLSummonField::~GLSummonField(void)
{
	SAFE_DELETE(m_pSkinChar);
}

HRESULT GLSummonField::Create(
    LPDIRECT3DDEVICEQ pd3dDevice,
    GLLandMan* pLandMan,
    PGLSUMMON pSummonData,
    ServerActor* pOwner,
    D3DXVECTOR3& vPosition,
    const STARGETID& sTarget)
{
	if (!pSummonData || !pLandMan || !pOwner)
        return E_FAIL;

#ifdef _USE_SUMMON_COMPONET_

	// Summon Component 세팅;
	SummonSystem::CSummonCompField* pSummonComp =
		new SummonSystem::CSummonCompField( this );
	if ( pSummonComp )
		InsertComponent( pSummonComp );

#endif

	ASSIGN(*pSummonData);

	m_pd3dDevice = pd3dDevice;

	m_pSummonCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pSummonData->m_sSummonID );
	if( !m_pSummonCrowData )
	{
        sc::writeLogError(
            sc::string::format(
                "GLSummonField::Create. CrowID %1%/%2%",
                m_sSummonID.wMainID,
                m_sSummonID.wSubID));
		return E_FAIL;
	}

	// 있을 수 없는 타입이 들어올 경우 Normal 타입으로 소환한다;	
	if ( m_emTYPE == SUMMON_TYPE_NONE || m_emTYPE >= SUMMON_TYPE_SIZE )
	{
		m_emTYPE = SUMMON_TYPE_NORMAL;
	}

	// AI 생성 ( Script로 관리되는 타입일 경우에만 );
	if ( m_emTYPE == SUMMON_TYPE_NORMAL )
	{
		AISystem::CAICompField* pAI = new AISystem::CAICompField( this );
		if ( pAI )
			InsertComponent( pAI );
	}

	//	몹 로직 데이터 초기화.
	GLSUMMON::INIT_DATA();

	if ( pSummonData->m_fRemainedTime == 0.f )
		m_bUnlimit = true;

	ReSetActState(EM_ACT_ALL);

	// 위치
	m_vPosition = vPosition;

	// 방향
	D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();

	m_vDirect = m_vPosition - vOwnerPos;
	D3DXVec3Normalize ( &m_vDirect, &m_vDirect );

	// 속도
	m_fCurSpeed = m_pSummonCrowData->m_sAction.m_fRunVelo;

	// 타겟ID
	m_TargetID.RESET();
	m_TargetID = sTarget;

	m_dwOwnerGaeaID = pOwner->GetGaeaID();
	m_mapID  = pOwner->GetCurrentMap();

	CreateActorMove(pLandMan->GetNavi (), m_vPosition);
	StopActorMove();

	m_sHITARRAY.clear();

	m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
	m_wRangeTracing       = 0;

	m_fattTIMER    = 0.0f;
	m_dwReactCount = 0;

	m_fIdleTime         = 0.0f;
	m_fGestureStartTime = 0.0f;

	// Note : 스케쥴 리셋
	m_ScheduledAction.Reset();

	// 주변 케릭터 인식.
	//
	UpdateViewAround ();

	// Note : 지형효과 초기화.
	m_vecLandEffect.clear();

	m_vecMapEffect.clear();

	// Note : 스킨 데이터 ( 실제 게임내에선 사용 안함 )
	if ( m_pd3dDevice )
	{
		DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pSummonCrowData->GetSkinObjFile(), m_pd3dDevice );
		if ( !pCharData )
            return E_FAIL;

		SAFE_DELETE(m_pSkinChar);
		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData ( pCharData, m_pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	}

	// Note : 상태를 생성중으로 바꾼다.
	//	
	m_fIdleTime = 0;
	

	VECANIATTACK& cVecCreateAnimation = m_pSummonCrowData->GetAnimation ( AN_CREATE, AN_SUB_00 );

	// Note :  애니메이션이 있을 경우만
	//
	if ( !cVecCreateAnimation.empty() )
	{
		SetActState( EM_ACT_CREATE );
	}

	// Note : 사용가능한 제스쳐를 체크한다.
	//
	m_emGestureState = EM_SUMMONGESTURE_NONE;
	m_dwGestureNum   = 0;

	for ( int i=0; i<EM_SUMMONGESTURE_MAX; ++i )
	{
		VECANIATTACK& cVecGestureAnimation = m_pSummonCrowData->GetAnimation ( AN_GESTURE, EMANI_SUBTYPE(i) );
	
		if ( cVecGestureAnimation.empty() )
		{
			continue;
		}

		m_emGestureableArray[m_dwGestureNum] = EMSUMMONGESTURESTATE(i);
		++m_dwGestureNum;
	}

	return S_OK;
}

void GLSummonField::RESET_DATA ()
{
	m_pd3dDevice = NULL;
	SAFE_DELETE(m_pSkinChar);

	m_vTarPos  = D3DXVECTOR3(0,0,0);
	m_vBackupPosition = D3DXVECTOR3(0,0,0);

	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_vecLandEffect.clear();

	m_vecMapEffect.clear();

	m_sHITARRAY.clear();

	m_actorMove.Stop();
	m_actorMove.Release();

	m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
	m_wRangeTracing       = 0;

	m_wAniSub	   = 0;
	m_fattTIMER    = 0.0f;
	m_dwReactCount = 0;
	

	// 소환수 삭제
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand )
	{
		ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
		if (pOwner)
		{
			pOwner->RemoveChildCrow( Faction::GLActorID( CROW_SUMMON, m_dwGaeaID ) );
		}
	}

	m_dwGlobalID = GAEAID_NULL;

	m_sAttackProp = SCROWATTACK();

	m_vStartPos  = D3DXVECTOR3(0,0,0);

	m_TargetID.RESET();

	m_fattTIMER    = 0.0f;
	m_fIdleTime    = 0.0f;

	m_wTARNUM = 0;
	for ( DWORD i=0; i<EMTARGET_NET; ++i )
		m_sTARIDS[i] = STARID(CROW_PC, GAEAID_NULL);
	m_vTARPOS = D3DXVECTOR3(0,0,0);

	// Note : 스케쥴 리셋
	m_ScheduledAction.Reset();

	// 소환수 데이터 리셋
	GLSUMMON::RESET_DATA();
	GLSUMMON::RESET ();
	ReSetActState(EM_ACT_ALL);

	m_ARoundSlot.Reset();
}

HRESULT GLSummonField::SetPosition(GLLandMan* pLandMan)
{
	if (!pLandMan)
        return E_FAIL;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLandMan, m_sOwnerID );
    if (!pOwner)
    {
        sc::writeLogError(std::string("GLSummonField::SetPosition pOwner NULL"));
        return E_FAIL;
    }

	// 주인 주변	
	D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();
	D3DXVECTOR3 vRandPos  = GetRandomPosition();	
	m_vPosition = vRandPos + vOwnerPos;

	// 방향
	m_vDirect = m_vPosition - vOwnerPos;
	D3DXVec3Normalize(&m_vDirect, &m_vDirect);

	m_mapID  = pOwner->GetCurrentMap();

	CreateActorMove(pLandMan->GetNavi (), m_vPosition);
	StopActorMove();

	return S_OK;
}

void GLSummonField::TurnAction(const EMACTIONTYPE toAction, const bool bMsg)
{
	const EMACTIONTYPE& emAction = GetAction();
	// 이전 액션 취소.
	//
	switch ( emAction )
	{
	case GLAT_IDLE:
		break;

	case GLAT_MOVE:
		// 도망 상태를 해지한다;
		if ( toAction != GLAT_MOVE )
			m_bRunAway = false;
		break;

	case GLAT_ATTACK:
		// 공격수행이 끝날때 딜레이 등록.
		//ReSetActState(EM_SUMMONACT_ATTACK);

		if ( m_sAttackProp.bUsed )
		{
			m_fACTIONDELAY[m_dwAType] = m_sAttackProp.fDelay + 0.3f;
		}
		break;

	case GLAT_SKILL:
		{
			//ReSetActState(EM_SUMMONACT_SKILL);

			if ( toAction != GLAT_SKILL )
			{
				m_idACTIVESKILL = NATIVEID_NULL();
				m_wTARNUM       = 0;

				// 이벤트 스킬 초기화;
				/*if ( m_bUsingEventSkill )
				{
					//sc::writeLogInfo( sc::string::format(
					//	"Event Skill Count %1%!", m_nSkillFramCount ) );
					//m_nSkillFramCount = 0;
					m_bUsingEventSkill = false;
					//sc::writeLogInfo( "End Event Skill Mode." );
				}*/
			}

			// Note : 이게 필요한건지 모르겠다
			/*
			if ( m_pAttackProp )
			{
				SNATIVEID sRunSkill = m_pAttackProp->skill_id;
				WORD      wLevel    = m_pAttackProp->skill_lev;

				// 스킬 정보 가져옴.
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sRunSkill );
				if ( !pSkill )								break;
				if ( wLevel >= SKILL::MAX_LEVEL )			break;

				float fSkillDelay = pSkill->m_sAPPLY.sDATA_LVL[wLevel].fDELAYTIME;
				m_fACTIONDELAY[m_dwAType] = fSkillDelay + 0.1f;
			}
			*/
		}
		break;

	case GLAT_FALLING:
		{
			if ( toAction != GLAT_DIE )
			{
				return;
			}
		}
		break;

	case GLAT_DIE:
		{
			return;
		}
		break;

	default:
		break;
	};

	// 액션 초기화.
	//
	SetAction(toAction);

	// Note : 예약된 스케쥴 검사
	//
	if ( toAction == GLAT_IDLE )
	{
		if ( UpdateScheduledAction() )
		{
			return;
		}
	}

	// AI;
	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );

	switch ( toAction )
	{
	case GLAT_IDLE:
		{
			// 초기화;
			ReSetActState( EM_ACT_ALL );
			m_idACTIVESKILL = NATIVEID_NULL();
			m_wTARNUM = 0;
			//m_TargetID.RESET();
			m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			m_wRangeTracing = 0;
			m_sAttackProp = SCROWATTACK();

			m_fIdleTime = 0.0f;

			if ( !bMsg )
				break;

			GLMSG::SNET_ACTION_BRD NetMsg;
			NetMsg.emCrow	= CROW_SUMMON;
			NetMsg.dwID		= m_dwGaeaID;
			NetMsg.emAction	= toAction;

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
		}
		break;

	case GLAT_MOVE:
		{
			m_fIdleTime = 0.0f;

			if ( !bMsg )
				break;

			if ( m_actorMove.PathIsActive() )
			{
				m_vStartPos = m_vPosition;

				// 이동 메세지 발생.
				//
				GLMSG::SNET_SUMMON_GOTO NetMsg(m_dwGaeaID,m_TargetID,m_actorMove.Position(),m_TargetID.vPos,GetMoveVelo(), GetActState());
				SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		break;

	case GLAT_ATTACK:
		{
			ReSetActState(EM_ACT_ALL);
			//SetActState(EM_SUMMONACT_ATTACK);

			m_fIdleTime = 0.0f;
			m_fattTIMER = 0.0f;
			StartStrikeProc ( FALSE, FALSE );
			StartAttackProc ();

			SendToChildCrowAttack();

			if ( m_emTYPE == SUMMON_TYPE_NORMAL )
			{
				if ( pAI )
					pAI->Attack();
			}
		}
		break; // TurnAction (GLAT_ATTACK) 끝

	case GLAT_SKILL:
		{
			ReSetActState(EM_ACT_ALL);
			//SetActState(EM_SUMMONACT_SKILL);

			m_fIdleTime = 0.0f;
			m_fattTIMER = 0.0f;
			StartStrikeProc ( TRUE, FALSE );
			StartSkillProc ();

			SendToChildCrowAttack();

			if ( m_emTYPE == SUMMON_TYPE_NORMAL )
			{
				if ( pAI )
					pAI->Attack();
			}
		}
		break; // TurnAction (GLAT_SKILL) 끝

	case GLAT_SHOCK:
		if ( m_pSummonCrowData->IsIgnoreShock() == FALSE )
			m_fIdleTime = 0.0f;
		break;

	case GLAT_PUSHPULL:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_FALLING:
		{
			ReSetActState(EM_ACT_ALL);
			//SetActState( EM_SUMMONACT_STOP );

			m_TargetID      = STARGETID();
			m_fIdleTime     = 0.0f;
			m_fRemainedTime = 0.0f;
			m_dwNowHP       = 0;

			if ( !bMsg ) break;

			// 주변 Char에게 쓰러짐을 알림.
			GLMSG::SNET_ACTION_BRD NetMsg;
			NetMsg.emCrow	= CROW_SUMMON;
			NetMsg.dwID		= m_dwGaeaID;
			NetMsg.emAction	= toAction;

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
		}
		break;

	default:
		break;
	};

	if ( m_actorMove.PathIsActive() )
	{
		if ( !isAction(GLAT_MOVE) && !isAction(GLAT_PUSHPULL) )
			m_actorMove.Stop();
	}
}

void GLSummonField::TurnScheduledAction ( SSCHEDULEDACTION& sSchedule )
{
	if ( m_ScheduledAction.emAction <= sSchedule.emAction )
	{
		m_ScheduledAction.Assign(sSchedule);
	}
}

void GLSummonField::SetScheduledAction  ( SSCHEDULEDACTION& sSchedule )
{
	m_ScheduledAction.Assign(sSchedule);
}

void GLSummonField::RunSchedule ( float fTime, float fElapsedTime )
{
	// 유효하지 않다면
	if ( !IsValidBody() )
	{
		return;
	}

	// 생성 중이라면
	if ( IsActState(EM_ACT_CREATE) )
	{
		return;
	}

	if ( m_bSTATE_STUN )
		return;

	if ( m_sDOMINATION.bDomination )
		return;

	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::RunSchedule - pLand is NULL." );
		return;
	}

	// 딜레이 적용;
	static float fAllTime = 0.f;
	if ( m_bDelay )
	{
		fAllTime += fElapsedTime;
		if ( fAllTime >= m_fDelayTime )
		{
			m_bDelay = false;
			fAllTime = 0.f;
		}
	}

	// 소환수 기본 행동 지침 
	switch ( m_emTYPE )
	{
	default:
	case SUMMON_TYPE_NORMAL:
		// 일반 ( Script로 관리됨 );
		{
			AISystem::CAICompField* pAI =
				static_cast< AISystem::CAICompField* >(
				GetComponent( ComponentSystem::EMFAMILY_AI ) );

			if ( pAI )
			{
				pAI->FrameMove( fElapsedTime );
			}
		}
		break;

	case SUMMON_TYPE_ATTACK: 
		// 공격 소환수
		{
			// Note : 대상이 없으면 죽는다
			if ( m_TargetID.GaeaId == EMTARGET_NULL )
			{
				Order_Die();
				break;
			}

			// Note : 대상이 없으면 죽는다
			ServerActor *pTARGET = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
			if ( !pTARGET || !pTARGET->IsValidBody() )
			{
				Order_Die();
				break;
			}

			// Note : 대상이 반응 할수없는 대상이면 죽는다
			if ( IsReActionable(pTARGET, EMIMPACT_SIDE_ENEMY) == EMREACTIONABLE_FALSE)
			{
				Order_Die();
				break;
			}

			/*
			// Note : 대상이 PC고 적대관계가 아니면 죽는다
			if ( pTARGET->GetCrow() == CROW_PC && !m_pOwner->IS_PLAYHOSTILE(pTARGET->GetCharID()) && !m_pOwner->IsConflictTarget ( pTARGET ) )
			{
				Order_Die();
				break;
			}
			*/

			// Note : 리액션중이 아니면
			if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
			{
				switch ( SetReactionTarget( m_TargetID ) )
				{
				case EMRETURN_SETREACTION_SUCCESS:
					break;

					// Note : 리액션 설정이 실패하면 특정 경우에만 추적한다.
				case EMRETURN_SETREACTION_FAILED_COLLISION:
				case EMRETURN_SETREACTION_FAILED_RANGE:
					Move_TargetTracing( m_TargetID, m_wRangeTracing );
					break;

				case EMRETURN_SETREACTION_FAILED_SKILL:
					// Note : 쿨타임이 부족하면 해당목표의 임시 거리 만큼 유지
					Move_TargetTracing( m_TargetID, 10 );
					break;

				default:
					break;
				}
			}
		}
		break;

	case SUMMON_TYPE_DISPOSABLE:
		// 자폭 소환수
		{
			// Note : 대상이 없으면 죽는다
			if ( m_TargetID.GaeaId == EMTARGET_NULL )
			{
				Order_Die();
				break;
			}

			// Note : 대상이 없으면 죽는다
			ServerActor *pTARGET = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
			if ( !pTARGET || !pTARGET->IsValidBody() )
			{
				Order_Die();
				break;
			}

			// Note : 대상이 반응 할수없는 대상이면 죽는다
			if ( IsReActionable(pTARGET, EMIMPACT_SIDE_ENEMY) == EMREACTIONABLE_FALSE )
			{
				Order_Die();
				break;
			}

			/*
			// Note : 대상이 PC고 적대관계가 아니면 죽는다
			if ( pTARGET->GetCrow() == CROW_PC && !m_pOwner->IS_PLAYHOSTILE(pTARGET->GetCharID()) && !m_pOwner->IsConflictTarget ( pTARGET ) )
			{
				Order_Die();
				break;
			}
			*/

			// Note : 리액션중이 아니면
			if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
			{
				if ( m_dwReactCount ) // 리액션을 한번 시전했으면
				{
					Order_Die();
					break;
				}
				else
				{
					switch ( SetReactionTarget( m_TargetID ) )
					{
					case EMRETURN_SETREACTION_SUCCESS:
						break;

					case EMRETURN_SETREACTION_FAILED_COLLISION:
					case EMRETURN_SETREACTION_FAILED_RANGE:
						// Note : 리액션 설정이 실패하면 특정 경우에만 추적한다.
						Move_TargetTracing( m_TargetID, m_wRangeTracing );
						break;

					case EMRETURN_SETREACTION_FAILED_SKILL:
						// Note : 쿨타임이 부족하면 해당목표의 임시 거리 만큼 유지
						Move_TargetTracing( m_TargetID, 10 );
						break;;

					default:
						break;
					}
				}
			}
		}
		break;

	case SUMMON_TYPE_BUFF:
		// 버프 소환수
		{
			// 주인이 없으면
			ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
			if (!pOwner)
				break;

			D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();

			// Note : 리액션중이 아니면
			if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
			{
				STARGETID sTarget;

				// Note : 대상을 검색한후 리액션 설정
				DetectReactionableTarget( sTarget );

				// Note : 타겟이 다르면 추적 상태를 멈춘다
				if ( m_TargetID != sTarget )
				{
					m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
					m_wRangeTracing       = 0;
					//ReSetActState(EM_SUMMONACT_TRACING);
				}

				switch ( SetReactionTarget( sTarget ) )
				{
					case EMRETURN_SETREACTION_SUCCESS:
						break;

					case EMRETURN_SETREACTION_FAILED_COLLISION:
					case EMRETURN_SETREACTION_FAILED_RANGE:
						// Note : 리액션 설정이 실패시 특정 경우 타겟 추적.
						Move_TargetTracing( sTarget, m_wRangeTracing );
						break;

					default:
						// Note : 실패한 후 특정 경우가 아니라면 주인을 추적.
						m_TargetID.emCrow = pOwner->GetCrow();
						m_TargetID.GaeaId = pOwner->GetGaeaID();
						Move_TargetTracing(m_TargetID, (WORD)m_fOwnerDistance, true);
						break;
				}
			}

			/*
			// Note : 추적중이나 리액션중이 아니면
			if ( !IsSTATE(EM_SUMMONACT_ATTACK | EM_SUMMONACT_SKILL | EM_SUMMONACT_TRACING ) )
			{
				// Note : 리액션 실패후 주인에게 이동 처리
				if ( IsSTATE ( EM_SUMMONACT_MOVE ) )
				{
					if ( m_actorMove.PathIsActive() )
					{
						D3DXVECTOR3 vTargetPosition = m_actorMove.GetTargetPosition();
						D3DXVECTOR3 vToOwner        = vOwnerPos - vTargetPosition;
						FLOAT       fTargetToOwner  = D3DXVec3Length( &vToOwner );

						// Note : 목적지에서 주인과의 거리가 최대 유지거리 이상이라면 이동&메세지를 보낸다
						if ( m_fMoveRefreshDistance < fTargetToOwner )
						{
							m_TargetID.emCrow = m_pOwner->GetCrow();
							m_TargetID.dwID   = m_pOwner->GetGaeaID();

							FLOAT fMinDistance = (FLOAT)m_pOwner->GetBodyRadius() + (FLOAT)this->GETBODYRADIUS();

							Move_TargetAround_Random ( m_TargetID, fMinDistance, m_fOwnerDistance, true );
						}
					}
				}
				else
				{
					D3DXVECTOR3 vToOwner         = vOwnerPos - m_vPos;
					FLOAT       fToOwnerDistance = D3DXVec3Length( &vToOwner );

					// Note : 현재 위치에서 주인과의 거리가 최소 유지거리 이상이라면 이동&메세지를 보낸다
					if ( m_fOwnerDistance < fToOwnerDistance )
					{
						m_TargetID.emCrow = m_pOwner->GetCrow();
						m_TargetID.dwID   = m_pOwner->GetGaeaID();

						FLOAT fMinDistance = (FLOAT)m_pOwner->GetBodyRadius() + (FLOAT)this->GETBODYRADIUS();

						Move_TargetAround_Random ( m_TargetID, fMinDistance, m_fOwnerDistance, true );
					}
				}
			}
			*/
		}
		break;
	case SUMMON_TYPE_WAY_1 :
		{
			GLLandMan* const pLand = GetLandMan();
			if ( NULL == pLand )
			{
				break;
			}

			ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
			if ( NULL == pOwner )
			{
				break;
			}

			D3DXVECTOR3 vLength = pOwner->GetPosition() - GetPosition();
			float fOwnerLength = D3DXVec3Length( &vLength );

			if ( fOwnerLength >= m_fOwnerDistance )
			{
				Order_Die();
				break;
			}

			if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
			{
				PGLSKILL  pSkill    = NULL;
				WORD      wLevel    = 0;

				SCROWATTACK sAttackProp = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType];

				pSkill = GLSkillMan::GetInstance().GetData ( sAttackProp.skill_id );

				if( !pSkill ) break;

				wLevel = ((WORD)pSkill->m_sBASIC.dwMAXLEVEL < m_wOwnerSkillLevel) ? (WORD)pSkill->m_sBASIC.dwMAXLEVEL : (WORD)m_wOwnerSkillLevel;

				if( !SelfZoneInCrow( m_TargetID, pSkill, wLevel ) ) break;
				if( !IsOtherWay( pSkill, wLevel )) break;

				switch ( SetReactionTarget( m_TargetID ) )
				{
				case EMRETURN_SETREACTION_SUCCESS:
					break;

					// Note : 리액션 설정이 실패하면 특정 경우에만 추적한다.
				case EMRETURN_SETREACTION_FAILED_COLLISION:
				case EMRETURN_SETREACTION_FAILED_RANGE:
					break;

				case EMRETURN_SETREACTION_FAILED_SKILL:
					// Note : 쿨타임이 부족하면 해당목표의 임시 거리 만큼 유지
					break;
				default:
					break;
				}
			}
		}
		break;
	case SUMMON_TYPE_WAY_2 :
		{
			GLLandMan* const pLand = GetLandMan();
			if ( NULL == pLand )
			{
				break;
			}

			ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
			if ( NULL == pOwner )
			{
				break;
			}

			D3DXVECTOR3 vLength = pOwner->GetPosition() - GetPosition();
			float fOwnerLength = D3DXVec3Length( &vLength );

			if ( fOwnerLength >= m_fOwnerDistance )
			{
				Order_Die();
				break;
			}

			if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
			{
				PGLSKILL  pSkill    = NULL;
				WORD      wLevel    = 0;

				SCROWATTACK sAttackProp = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType];

				pSkill = GLSkillMan::GetInstance().GetData ( sAttackProp.skill_id );

				if( !pSkill ) break;

				wLevel = ((WORD)pSkill->m_sBASIC.dwMAXLEVEL < m_wOwnerSkillLevel) ? (WORD)pSkill->m_sBASIC.dwMAXLEVEL : (WORD)m_wOwnerSkillLevel;

				if( !SelfZoneInCrow( m_TargetID, pSkill, wLevel ) ) break;
				if( !IsOtherWay( pSkill, wLevel )) break;
				
				switch ( SetReactionTarget( m_TargetID ) )
				{
				case EMRETURN_SETREACTION_SUCCESS:
					break;

					// Note : 리액션 설정이 실패하면 특정 경우에만 추적한다.
				case EMRETURN_SETREACTION_FAILED_COLLISION:
				case EMRETURN_SETREACTION_FAILED_RANGE:
					break;

				case EMRETURN_SETREACTION_FAILED_SKILL:
					// Note : 쿨타임이 부족하면 해당목표의 임시 거리 만큼 유지
					break;
				default:
					break;
				}
			}
		}
		break;
	case SUMMON_TYPE_NPC   :
		{

		}
		break;

	}
}

HRESULT GLSummonField::UpdateDurDamage ( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	//	지속 데미지 처리( 데미지 준다. ) 
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].VAILD() ) continue;

		m_sDurDamage[i].m_fTime -= fElapsedTime;			
	
		if ( m_sDurDamage[i].m_fTime < 0.0f ) 
		{
			//	데미지 준다.
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;

			//	데미지 계산

			// 전체저항값 = 상대의 저항값 / 100 * 속성저항치 적용비율
			int nRESIST = m_sSUMRESIST.GetElement( (EMELEMENT)i );
			float fResistTotal = (float) ((float) nRESIST * 0.01f * GLCONST_CHAR::fRESIST_G);
			fResistTotal = fResistTotal > GLCONST_CHAR::fREGIST_MAX_G ? GLCONST_CHAR::fREGIST_MAX_G : fResistTotal;

			// 최소/최대 데미지 = 데미지-(데미지*전체저항값)
			int nDamage = int(m_sDurDamage[i].m_fDamage);
			nDamage -= int(m_sDurDamage[i].m_fDamage  * fResistTotal);

            ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_sDurDamage[i].m_sID );

            if ( !pACTOR )
            {
                continue;
            }

            int nActualDamageHP = -nDamage;
            int nActualDamageMP(0);
            int nActualDamageSP(0);
            //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
            VAR_BODY_POINT( m_sDurDamage[i].m_sID.emCrow, m_sDurDamage[i].m_sID.GaeaId, DAMAGE_TYPE_DOT, nActualDamageHP, nActualDamageMP, nActualDamageSP );

			GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
			NetMsgBRD.emCrow = GetCrow();
			NetMsgBRD.dwID   = m_dwGaeaID;

			//	스킬을 사용한 행위자 마킹.
			NetMsgBRD.sACTOR.emCrow = m_sDurDamage[i].m_sID.emCrow;
			NetMsgBRD.sACTOR.dwID   = m_sDurDamage[i].m_sID.GaeaId;

			NetMsgBRD.dwDamageFlag |= DAMAGE_TYPE_DOT;
			NetMsgBRD.nVAR_HP = nActualDamageHP;
			NetMsgBRD.nVAR_MP = nActualDamageMP;
			NetMsgBRD.nVAR_SP = nActualDamageSP;

            NetMsgBRD.wSchool = pACTOR->GetSchool();

			SendMsgViewAround(&NetMsgBRD);
		}
	}  

	return S_OK;
}

HRESULT GLSummonField::UpdateTurnAction( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	// 액션별 처리작업.
	//	
	switch ( m_sDOMINATION.emController )
	{
	case EMSPEC_DOMINATE_TYPE_RANDOM:		
		if ( isAction(GLAT_FALLING, GLAT_DIE) == false )
		{
			const DWORD dwCurTimer = ::clock();
			if ( dwCurTimer - m_sDOMINATION.dwTimer > 1000 )
			{
				m_sDOMINATION.dwTimer = dwCurTimer;
				const float fDirectAngle = sc::Random::getInstance().RANDOM_NUM() * 6.28f;
				const float fDistance = sc::Random::getInstance().RANDOM_POS() * m_sDOMINATION.fRadiusMoveable;
				const D3DXVECTOR3 vRandomPosition(m_vPosition.x + cos(fDirectAngle) * fDistance, m_vPosition.y, m_vPosition.z + sin(fDirectAngle) * fDistance);		
				MoveTo(&vRandomPosition, false, 0);
			}			
			TurnAction(GLAT_MOVE);
		}
		break;
	case EMSPEC_DOMINATE_TYPE_FIX:
		if ( isAction(GLAT_FALLING, GLAT_DIE) == false )
			TurnAction(GLAT_IDLE);
		break;
	}	

	HRESULT hr(S_OK);

	// Note : 딜레이 감소
	DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();

	for ( DWORD i=0; i<dwSize; ++i )
	{
		if ( m_fACTIONDELAY[i] > 0.0f )
			m_fACTIONDELAY[i] -= fElapsedTime;
	}
	
	const EMACTIONTYPE& emAction = GetAction();
	// Note : 행동 처리
	switch ( emAction )
	{
	case GLAT_IDLE:
		IdleProcess ( fElapsedTime );
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		{
			// Note : 속도 관련
			float fMoveVelo = GetMoveVelo();

			// Note : 이속이 틀려지면 속도 갱신을 한다.
			if ( m_bOwnerSpeed && (m_fCurSpeed != fMoveVelo) )
			{
				// Note : 주변의 클라이언트에게 상태변화를 알림
				GLMSG::SNET_SUMMON_UPDATE_STATE	NetMsgBrd(GetGaeaID(), GetActState(), m_dwNowHP, m_emGestureState, GetMoveVelo());
				SendMsgViewAround(&NetMsgBrd);
			}

			m_fCurSpeed = fMoveVelo;

			// 케릭의 이동 업데이트.
			//
			m_actorMove.SetMaxSpeed ( fMoveVelo );
			hr = m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );
			if ( FAILED(hr) )
                return E_FAIL;

			m_vPosition = m_actorMove.Position();

			if ( !m_actorMove.PathIsActive() )
			{			
				TurnAction ( GLAT_IDLE, false );

				m_actorMove.Stop ();
				//SetActState ( EM_SUMMONACT_STOP );
			}

			// 방향 업데이트
			D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
			if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection = vMovement - m_vPosition;
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					m_vDirect = vDirection;
				}
			}			
		}
		break;

	case GLAT_ATTACK:
		{
			if ( !AttackProcess ( fElapsedTime ) )
			{
				//SetActState( EM_SUMMONACT_STOP );
				TurnAction( GLAT_IDLE, false );
			}

			// 공격 방향으로 회전.
			//
			ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
			if ( pACTOR )
			{
				D3DXVECTOR3 vDirection = pACTOR->GetPosition() - m_vPosition;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDirect = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			//m_nSkillFramCount++;

			if ( !SkillProcess ( fElapsedTime ) )
			{
				//SetActState( EM_SUMMONACT_STOP );
				TurnAction ( GLAT_IDLE, false );
			}
		}
		break;

	case GLAT_SHOCK:
		//	한프레임만 적용되게 수정
//		if ( !ShockProcess ( fElapsedTime ) )	
		{
			TurnAction( GLAT_IDLE );
			//SetActState( EM_SUMMONACT_STOP );
		}
		break;

	case GLAT_FALLING:
		m_fIdleTime += fElapsedTime;
		if ( m_fIdleTime > 4.0f )	TurnAction ( GLAT_DIE );
		break;

	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction( GLAT_IDLE );
				//SetActState( EM_SUMMONACT_STOP );
			}

			// Mob의 현제 위치 업데이트.
			//
			m_vPosition = m_actorMove.Position();
		}
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	return S_OK;
}

const BOOL GLSummonField::UpdateScheduledAction ()
{	
	if ( isAction(GLAT_IDLE) == false )
		return FALSE;

	if ( IsActState(EM_ACT_CREATE) )
		return FALSE;

	// Note : 사용한 스케쥴을 소멸시킨다.
	SSCHEDULEDACTION sSchedule;
	sSchedule.Assign( m_ScheduledAction );

	m_ScheduledAction.Reset();

	switch ( sSchedule.emAction )
	{
	default:
	case EMSCHEDULETYPE_NONE:
		break;

	case EMSCHEDULETYPE_DIE:
		{
			m_fRemainedTime = 0.0f;
			m_dwNowHP       = 0;
			TurnAction(GLAT_FALLING);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void GLSummonField::UpdateMapEffect()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	//	선도전맵에선 다른 지형효과가 적용된다.
	if ( pLand->m_bGuidBattleMap ) 
	{        
		GLGuidance* pGuid = m_pGaeaServer->GuidanceFind(pLand->m_dwClubMapID);
		if (!pGuid || !pGuid->m_bMapEffectUse)
            return;

		//	선도전 종료
		if ( !pGuid->IsBattle() ) 
		{
			DISABLEALLMAPEFF();
			return;
		}

		ServerActor* pOwnerBasic = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
        GLChar* pOwner = NULL;
        if (!pOwnerBasic)
            return;

		if ( pOwnerBasic->GetCrow() == CROW_PC )
		{
			pOwner = static_cast< GLChar* >( pOwnerBasic );
			if ( !pOwner )
				return;
		}
		else
			return;

		std::tr1::shared_ptr<GLClubField> pClub = m_pGaeaServer->GetClub(pOwner->m_ClubDbNum );
		std::tr1::shared_ptr<GLClubField> pGuidClub = m_pGaeaServer->GetClub(pGuid->m_dwGuidanceClub);
		
		if ( pClub && pGuidClub ) 
		{
			//	인증 클럽 및 동맹에게 효과 추가한다. 
			//	인증이 바뀔경우 맵에서 다 튕기므로 삭제되는 경우는 없다.
			//	다른 형태의 맵 효과가 추가되면 삭제 코드도 만들어줘야 합니다.
			
			//	같은 클럽이거나 동맹인지 체크
			if ( pClub->IsAllianceGuild_COMMON( pGuidClub->m_DbNum, pGuidClub->m_dwAlliance ) )
			{
				int nMax = pGuid->m_nMapEffectIndex;
				int nSize = (int)m_vecMapEffect.size();

				for ( int i = nSize; i <= nMax; ++i ) 
				{
					ADDMAPEFF( pGuid->m_vecMapEffect[i].sMapEffect );

					GLMSG::SNET_SUMMON_MAPEFFECT_UPDATE NetMsg(m_dwGaeaID,pGuid->m_vecMapEffect[i].sMapEffect,true);	
					SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsg );
				}

				return;
			}
		}
	}
	
	//	여기까지 오면 해당사항 없는것 이다.
	DISABLEALLMAPEFF();
	return;
}

void GLSummonField::UpdateLandEffect()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_LANDEFFECT) ) 
	{
		DISABLEALLLANDEFF();
		return;
	}

	if ( m_vPosition == m_vBackupPosition )
		return;

	m_vBackupPosition = m_vPosition;			
	
	const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();	
	for( DWORD i = 0; i < pLevelEtcFunc->m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT landEffect = pLevelEtcFunc->m_vecLandEffect[i];
		bool bApply = landEffect.IsApplyEffect( m_vPosition );

		SLANDEFFECT_VALUE landEffectValue(landEffect,i);

		VEC_LANDEFF_VALUE_ITER pos = std::find( m_vecLandEffect.begin(), m_vecLandEffect.end(), landEffectValue );

		if ( bApply ) 
		{
			if ( pos == m_vecLandEffect.end() ) 
			{
				// 추가해줘야 된다.
				ADDLANDEFF( landEffectValue );

				// 클라이언트 갱신

				GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE NetMsg(m_dwGaeaID,landEffectValue,true);
				SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		else
		{
			if ( pos != m_vecLandEffect.end() ) 
			{

				//	삭제해줘야 한다.
				m_vecLandEffect.erase( pos );

				// 클라이언트 갱신
				GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE NetMsg(m_dwGaeaID,landEffectValue,false);
				SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
	}

	// 모든 버프를 취소해야할 경우의 처리
	for( DWORD i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			DisableSkillFact();
			break;
		}
	}
}

HRESULT GLSummonField::UpdateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	float fAnimation = fElapsedTime;

	EMANI_MAINTYPE MType = m_pSkinChar->GETCURMTYPE();
	EMANI_SUBTYPE SType  = m_pSkinChar->GETCURSTYPE();
	float fLIFETIME      = m_pSkinChar->GETCURANITIME();

	const EMACTIONTYPE& emAction = GetAction();
	switch ( emAction )
	{
	case GLAT_IDLE:
		if ( m_sHALLUCINATE.bEmoticon )
		{
			if ( !m_pSkinChar->IsANI_SELECTANI( AN_GESTURE, m_sHALLUCINATE.emAni_SubType ) )
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
		else
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		break;

	case GLAT_TALK:
		m_pSkinChar->SELECTANI ( AN_TALK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsActState(EM_ACT_RUN) )				m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else									m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
		//		if ( !ShockProcess ( fElapsedTime ) )
		//			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		fAnimation = 0.0f;
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;
	};

	// Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( emAction )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLSUMMON::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GETATTVELO();
		break;
	};

	m_pSkinChar->SetPosition ( m_vPosition );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, FALSE, m_matTrans );

	return S_OK;
}

HRESULT GLSummonField::FrameMove( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	HRESULT hr(S_OK);
	//static float fTimer(0.0f);

	// 랜드에서 등록된 셀을 업데이트 하기 위해서 호출함
	pLand->MoveSummon(m_dwGaeaID, m_vPosition);

	UpdateViewAround();

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
	if ( !pOwner )
		return E_FAIL;

	while (IsValidBody())
	{
		// Note : 체력 점검
		//
		if ( m_dwNowHP == 0 )
		{
			Order_Die();
			break;
		}

		// 무제한 모드가 아닐 경우 유지 시간 감소
		//
		if ( !m_bUnlimit )
		{
			m_fRemainedTime -= fElapsedTime;

			if ( m_fRemainedTime <= 0.0f )
			{
				Order_Die();
				break;
			}
		}

		// 주인과 다른 맵에 있다면
		if ( m_mapID != pOwner->GetCurrentMap() )
		{
			Order_Die();
			break;
		}	

		// 만약 주인이 없거나 죽었다면, 주인이 Vehicle을 타고있다면
		//
		if (!pOwner || !pOwner->IsValidBody() )
		{
			if ( pOwner->GetCrow() == CROW_PC )
			{
				if ( static_cast< GLChar* >( pOwner )->VehicleState() )
				{
					SetAction ( GLAT_IDLE );
					Order_Die();
				}
			}
			else
			{
				SetAction ( GLAT_IDLE );
				Order_Die();
			}
			break;
		}

		// 주인이 캐릭터일 경우 소환시 사용한 무기를 차고 있는지 점검
		if ( pOwner->GetCrow() == CROW_PC )
		{
			GLChar* pChar = static_cast< GLChar* >( pOwner );
			if ( pChar )
			{
				EMSLOT emRHand    = pChar->GetCurRHand();	
				SITEM* pRightItem = pChar->GET_SLOT_ITEMDATA( emRHand );		

				// 여러상황을 테스트 해볼수 없으므로..스킬사용으로 들어오는 값과 그렇지 않은값을 모두 호환되도록
				// 맵이동시에는 m_emNeedItemAtt값을 세팅할기 어렵고, 기타 다른상황이 있을수 있으니.
				bool bCheck = m_emNeedItemAtt == SKILL::SKILLATT_NOTHING ? false : true;

				if ( pRightItem )
				{					
					if( !bCheck && pRightItem->sSuitOp.emAttack != m_emOwnerItemAtt )
						Order_Die();
					else
					if( bCheck && !CHECHSKILL_ITEM(m_emNeedItemAtt, pChar->CONVERT_ITEMATT( pRightItem->sSuitOp.emAttack ), false ) )
						Order_Die();
				}
				else
				{
					if ( !bCheck && m_emOwnerItemAtt != ITEMATT_NOTHING )
						Order_Die();
					if( bCheck && !CHECHSKILL_ITEM(m_emNeedItemAtt, pChar->CONVERT_ITEMATT( ITEMATT_NOTHING ), false ) )
						Order_Die();
				}
			}
		}
		

		m_ARoundSlot.Update ( pLand );

		// Summon 의 LandEffect를 Update 한다
		//UpdateLandEffect();

		//UpdateMapEffect();

		// Note : Summon 의 현재 상태값들을 Update 한다
		//
		GLSUMMON::UPDATE_DATA ( fTime, fElapsedTime );
		UPDATE_DATA_Additional( fElapsedTime );
		UpdateDurDamage ( fTime, fElapsedTime );
		break;
	}
	
	// 소환수 행동 지침
	//
	RunSchedule ( fTime, fElapsedTime );

	// 액션 업데이트
	//
	UpdateTurnAction ( fElapsedTime );

	// Note : 예약된 액션 업데이트
	UpdateScheduledAction ();

	// 레벨에딧에서 Summon 이 나오진 않는다.
	//// m_pd3dDevice 가 유효할 경우만 아랫 코드가 실행된다. ( Level Editor 에서 태스트용. )
	////
	if ( m_pd3dDevice )
	{
		// 애니메이션, 스킨 업데이트;
		UpdateAnimation( fTime, fElapsedTime );
	}

	return S_OK;
}

void GLSummonField::IdleProcess ( float fElapsedTime )
{
	m_fIdleTime += fElapsedTime;

	// Note : 생성 중이라면
	if ( IsActState(EM_ACT_CREATE) )
	{
		VECANIATTACK &VecAni = m_pSummonCrowData->GetAnimation ( AN_CREATE, AN_SUB_00 );

		if ( VecAni.empty() )
		{
			m_fIdleTime = 0.0f;
			ReSetActState(EM_ACT_CREATE);
			return;
		}

		SANIATTACK &sAniAttack = VecAni[0];

		int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
		int wThisKey   = int(m_fIdleTime*UNITANIKEY_PERSEC);

		// Note : 임시로 N Frame만큼 더 뒤에 메세지를 보낸다.
		if ( wThisKey >= wTotalKeys + 100 )
		{
			m_fIdleTime = 0.0f;
			ReSetActState(EM_ACT_CREATE);
			return;
		}
	}
	// Note : 제스쳐 중이라면
	else if ( IsActState(EM_ACT_GESTURE) )
	{
		if ( m_emGestureState == EM_SUMMONGESTURE_NONE || m_emGestureState >= EM_SUMMONGESTURE_MAX )
		{
			m_emGestureState = EM_SUMMONGESTURE_NONE;
			m_fIdleTime      = 0.0f;
			ReSetActState(EM_ACT_GESTURE);
			return;
		}

		VECANIATTACK &VecAni = m_pSummonCrowData->GetAnimation ( AN_GESTURE, EMANI_SUBTYPE(m_emGestureState) );

		if ( VecAni.empty() )
		{
			m_emGestureState = EM_SUMMONGESTURE_NONE;
			m_fIdleTime      = 0.0f;
			ReSetActState(EM_ACT_GESTURE);
			return;
		}

		SANIATTACK &sAniAttack = VecAni[0];

		int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
		int wThisKey   = int(m_fIdleTime*UNITANIKEY_PERSEC);

		if ( wThisKey >= wTotalKeys )
		{
			m_emGestureState = EM_SUMMONGESTURE_NONE;
			m_fIdleTime      = 0.0f;
			ReSetActState(EM_ACT_GESTURE);
			return;
		}
	}
	// Note : 보통 상태라면
	else
	{
		if ( m_dwGestureNum != 0 )
		{
			if ( m_fGestureStartTime <= 0.0f )
			{
				m_fGestureStartTime = (FLOAT)((rand()%GLCONST_SUMMON::dwIdleGestureTimeMax[m_emTYPE]) + GLCONST_SUMMON::dwIdleGestureTimeMin[m_emTYPE]); // 15 ~  5
			}

			// Note : 제스쳐를 설정해준다.
			if ( m_fIdleTime >= m_fGestureStartTime )
			{
				m_emGestureState = m_emGestureableArray[(rand()%m_dwGestureNum)];

				if ( m_emGestureState != EM_SUMMONGESTURE_NONE && m_emGestureState < EM_SUMMONGESTURE_MAX )
				{
					m_fIdleTime = 0.0f;
					SetActState(EM_ACT_GESTURE);

					// Note : 주변의 클라이언트에게 상태변화를 알림
					GLMSG::SNET_SUMMON_UPDATE_STATE	NetMsgBrd(GetGaeaID(), GetActState(), m_dwNowHP, m_emGestureState, GetMoveVelo());
					SendMsgViewAround( (NET_MSG_GENERIC *)&NetMsgBrd );
				}

				m_fGestureStartTime = 0.0f;
			}
		}
	}
}

BOOL GLSummonField::AttackProcess ( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	//GASSERT(m_pAttackProp);
	if ( !m_sAttackProp.bUsed )
		return TRUE;

	m_fattTIMER += fElapsedTime/* * GETATTVELO()*/;

	SANIATTACK &sAniAttack = m_sAttackProp.sAniAttack;
	int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int wThisKey   = int(m_fattTIMER*UNITANIKEY_PERSEC);

	ServerActor* const pTargetActor = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if ( pTargetActor == NULL )
	{
		TurnAction ( GLAT_IDLE );
		m_TargetID.RESET();
		return FALSE;
	}

	if ( GetAttackRangeType()==EMATT_SHORT )
	{
		WORD wAttackRange = pTargetActor->GetBodyRadius() + GETSUMMON_BODYRADIUS() + GETSUMMON_ATTACKRANGE() + 20;
		wAttackRange *= GLCONST_CHAR::fATTACK_RANGE_ADJUST_RATE;

		if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( wAttackRange, m_vPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			TurnAction ( GLAT_IDLE );
			return FALSE;
		}
	}

	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( wThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
			{
				int nDamage = 0;
				DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
				dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, pLand );
				
				if ( CalcBlocking(pTargetActor, nDamage) == true )
					BlockProc( m_TargetID );
				else 
					DamageProc ( nDamage, dwDamageFlag );
			}
			else
				AvoidProc ();

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	if ( wThisKey >= wTotalKeys )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();

				if ( sStrike.bHit )
				{
					int nDamage = 0;
					DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
					dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, pLand );

					if ( CalcBlocking(pTargetActor, nDamage) == true )
						BlockProc( m_TargetID );
					else 
						DamageProc ( nDamage, dwDamageFlag );
				}
				else
					AvoidProc ();

				//	큐 데이타가 처리된 경우 삭제한다.
				m_sHITARRAY.pop_front ();
			}
		}

		return FALSE;
	}

	return TRUE;
}

DWORD GLSummonField::ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	ServerActor* pACTOR_TAR = m_pGaeaServer->GetTarget ( pLand, sTargetID );
	if (!pACTOR_TAR)
        return 0;

	// PC에게 damage를 주려면 주인이 대련자여야 가능.
	//		대련 종료 직후 damage 유입 차단을 위해서.
	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return 0;

	if ( IsReActionable(pACTOR_TAR, EMIMPACT_SIDE_ENEMY) == EMREACTIONABLE_FALSE )
        return 0;
    else
	    return pACTOR_TAR->ReceiveDamage(GLSUMMON::GETCROW(), m_dwGaeaID, nDAMAGE, dwDamageFlag);
}

void GLSummonField::BlockProc ( const STARGETID& sTargetID, const SNATIVEID& SkillID /* = NATIVEID_NULL() */, const WORD wSkillLevel /* = 0*/)
{
	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNET_SUMMON_ATTACK_BLOCKING NetMsg(m_dwGaeaID, sTargetID.emCrow, sTargetID.GaeaId, SkillID, wSkillLevel);
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
}

void GLSummonField::AvoidProc(const SNATIVEID& SkillID /* = NATIVEID_NULL() */, const WORD wSkillLevel /* = 0*/)
{					 
	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNET_SUMMON_ATTACK_AVOID NetMsg(m_dwGaeaID,m_TargetID.emCrow,m_TargetID.GaeaId, SkillID, wSkillLevel);
	SendMsgViewAround(&NetMsg);
}

void GLSummonField::DamageProc ( const int nDAMAGE, DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	int nRESULTDAMAGE = nDAMAGE;

	// PK 데미지 감소 수치 적용
	if ( m_TargetID.emCrow==CROW_PC || m_TargetID.emCrow==CROW_SUMMON )
	{
		nRESULTDAMAGE = int(nRESULTDAMAGE*GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if ( nRESULTDAMAGE==0 ) nRESULTDAMAGE = 1;
	}

	// 대상자에게 대미지값 적용.
	const DWORD dwActualDamage = ToDamage ( m_TargetID, nRESULTDAMAGE, dwDamageFlag );

	// 주변 클라이언트들에게 메세지 전송.
	GLMSG::SNET_SUMMON_ATTACK_DAMAGE NetMsg(m_dwGaeaID,m_TargetID.emCrow,m_TargetID.GaeaId,dwActualDamage,dwDamageFlag, GetSchool());
	SendMsgViewAround(&NetMsg);

	ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );

	// 상태 이상 유발.
	//
	if ( pACTOR && m_sOwnerBlow.emTYPE != EMBLOW_NONE )
	{
		// 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(m_sOwnerBlow.emTYPE) );
		if ( nBLOWRESIST>99 )
			nBLOWRESIST = 99;

		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( m_sOwnerBlow.emTYPE, dwWeather, pLand->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(m_sOwnerBlow.emTYPE)) )
		{
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( m_sOwnerBlow.fRATE*fPOWER, GLSUMMON::GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );
		}

		if ( bBLOW )
		{
			// 상태이상 발생.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = m_sOwnerBlow.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW      = m_sOwnerBlow.emTYPE;
			sSTATEBLOW.fAGE        = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = m_sOwnerBlow.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = m_sOwnerBlow.fVAR2;

			if ( pACTOR->STATEBLOW ( sSTATEBLOW ) )
			{
				// 상태이상 발생 Msg.
				//
				GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
				NetMsgState.emCrow = m_TargetID.emCrow;
				NetMsgState.dwID = m_TargetID.GaeaId;
				NetMsgState.emBLOW = sSTATEBLOW.emBLOW;
				NetMsgState.fAGE = fLIFE;
				NetMsgState.fSTATE_VAR1 = sSTATEBLOW.fSTATE_VAR1;
				NetMsgState.fSTATE_VAR2 = sSTATEBLOW.fSTATE_VAR2;
                NetMsgState.wSchool     = GetSchool();

				// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				//
				pACTOR->SendMsgViewAround(&NetMsgState);

				// 스킬 대상에게 메세지 전송.
				if ( m_TargetID.emCrow==CROW_PC )
                    m_pGaeaServer->SENDTOCLIENT ( pACTOR->GETCLIENTID (), &NetMsgState );
			}
		}
	}
}

BOOL GLSummonField::IsCtrlBlockBody () const
{
	if ( isAction(GLAT_SHOCK) )
		return TRUE;
	if ( isAction(GLAT_PUSHPULL) )
		return TRUE;

	return FALSE;
}


bool GLSummonField::IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const
{
	return GLSUMMON::IsCheckedSkillFlagSpecial( dwFlag );
}


void GLSummonField::StartStrikeProc ( BOOL bSkill, BOOL bLowSP )
{
	if ( !m_sAttackProp.bUsed )
		return;

	SANIATTACK &sAniAttack = m_sAttackProp.sAniAttack;

	//m_sHITARRAY.clear ();

	STARGETID sTargetID;
	BOOL bCheckHit = FALSE;

	if ( !bSkill )
	{
		sTargetID = m_TargetID;
		bCheckHit = TRUE;
	}
	else
	{
		// 스킬 정보 가져옴.
		//
		PGLSKILL pSkill = NULL;
		pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
		if ( !pSkill )
			return;

		if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
		{
			// 스킬 타겟을 하나만 유효하게 확인을 하고 있음.
			//	NEED : 다중 타겟일 경우 다시 확인해야함.
			sTargetID = STARGETID(m_sTARIDS[0].emCrow, m_sTARIDS[0].dwID);
			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.GaeaId = EMTARGET_NULL;
		}
	}

	//	큐 초기화
	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bHit = true;
		if ( bCheckHit && (sTargetID.GaeaId!=EMTARGET_NULL) )
		{
			if (!CheckHit(sTargetID))
                bHit = false;
		}

		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bHit) );
	}
}

void GLSummonField::StartAttackProc()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	/*if ( m_OwnerDbNum == GAEAID_NULL )
        return;*/

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return;

    ServerActor*  pTARGET = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
    if ( !pTARGET )
        return;

    const EMREACTIONABLE _emReActionable = IsReActionable(pTARGET, EMIMPACT_SIDE_ENEMY);
    switch ( _emReActionable )
    {
    case EMREACTIONABLE_FALSE:
        return;

    case EMREACTIONABLE_TRUE:
        break;

    case EMREACTIONABLE_FORCED_TRUE:
		if ( m_sOwnerID.actorType == CROW_PC )
			static_cast< GLChar* >( pOwner )->ReActionByForcedAction(pTARGET, EMIMPACT_SIDE_ENEMY);
        break;
    }

    // 공격 메세지 발생.;
	GLMSG::SNET_SUMMON_ATTACK NetMsg(m_dwGaeaID,m_TargetID,m_dwAType);
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );

	return;
}

void GLSummonField::StartSkillProc ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	/*if ( m_OwnerDbNum == GAEAID_NULL)
        return;*/

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return;

	// 스킬 정보 가져옴.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )
		return;

	const EMIMPACT_SIDE emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;

    WORD i = 0;
	WORD j = 0;

	for ( i=0, j=0; i<m_wTARNUM && j<EMTARGET_NET; ++i )
	{
		// 유효성 검사.
		//
		STARGETID sTARID(m_sTARIDS[i].GETCROW(),m_sTARIDS[i].GETID());
		ServerActor*  pTARGET = m_pGaeaServer->GetTarget ( pLand, sTARID );
		if ( !pTARGET )
			continue;

        const EMREACTIONABLE _emReActionable = IsReActionable(pTARGET, emTargetSide);
        switch ( _emReActionable )
        {
        case EMREACTIONABLE_FALSE:
            continue;

        case EMREACTIONABLE_TRUE:
            break;

        case EMREACTIONABLE_FORCED_TRUE:
            // 주 타겟 외에 타겟은 강제 공격 불가하다.;
            // PC의 경우 Client에서 타겟선별을 처리하여 서버로 정보를 보내지만;
            // Summon의 경우 Server에서 타겟을 선별하므로 처리를 해줌;
            if ( m_TargetID != sTARID )
                continue;

			if ( m_sOwnerID.actorType == CROW_PC )
				static_cast< GLChar* >( pOwner )->ReActionByForcedAction(pTARGET, emTargetSide);
            break;
        }

		// 타겟을 공격 가능한지 검사.
		//		( 상대방이 대련시에 자신과 대련이 아닐 경우 공격 불가. )
		m_sTARIDS[j++] = m_sTARIDS[i];
	}

	m_wTARNUM = j;

	if ( m_wTARNUM == 0 )
	{
		return;
	}

	// 스킬사용 소모 수치 감산.
	ACCOUNTSKILL ( 1 );

	// 스킬 메시지 발생.
	//
	GLMSG::SNET_SUMMON_SKILL NetMsg(m_dwGaeaID,m_TargetID,m_dwAType,m_bUsingEventSkill,m_idACTIVESKILL,m_wACTIVESKILL_LVL,m_vTARPOS);

	for ( WORD i=0; i<m_wTARNUM; ++i )	
		NetMsg.ADDTARGET ( m_sTARIDS[i] );

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );

	return;
}

bool GLSummonField::GetReactionTarget( STARGETID sTargetID ) 
{ 
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		m_TargetID.RESET();
		return TRUE;
	}

	if( m_TargetID.GaeaId != EMTARGET_NULL ) 
	{
		if( m_pGaeaServer->GetTarget ( pLand, m_TargetID ) == NULL )
		{
			m_TargetID.RESET();
			return TRUE;
		}

		if( m_TargetID == sTargetID )
			return TRUE;
		else
			return FALSE;
	}

	return TRUE; 
}

bool GLSummonField::MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags )
{
	//m_TargetID.vPos = vTarPos;
	//TurnAction ( GLAT_MOVE );

	//return S_OK;

	return true;
}

ServerActor* GLSummonField::GetTarget ( const STARGETID& sTargetID )
{
	GLLandMan* const pLand ( GetLandMan() );
	if ( pLand == NULL )
		return NULL;

	return m_pGaeaServer->GetTarget ( pLand, sTargetID );
}

// 피격 데미지 함수 통합 [9/26/2013 hsshin];
const DWORD GLSummonField::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    return ReceiveDamage_Procedure(emACrow, dwAID, dwDamage, dwDamageFlag);
}
const DWORD GLSummonField::ReceiveDamage_Procedure( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	if ( !IsValidBody() )
		return 0;
	if ( m_dwNowHP == 0 )
		return 0;

	m_sAssault.emCrow = emACrow;
	m_sAssault.GaeaId   = dwAID;

	// 인던 Script 에서의 Receive Damage 처리;
	DWORD dwActualDamage = dwDamage;
	InstanceSystem::InstanceField* const pInstance =
		pLand->GetScriptInstance();
	if ( NULL != pInstance )  // 인던이 아닐 경우 NULL;
	{
		// dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
		DWORD dwID = m_sAssault.GaeaId;
		if ( m_sAssault.emCrow == CROW_PC )
		{
			GLChar* pChar = m_pGaeaServer->GetChar( m_sAssault.GaeaId );
			if ( pChar )
				dwID = pChar->CharDbNum();
		}

		if ( CROW_SUMMON == m_sAssault.emCrow )
		{
			GLSummonField* pSummon =
				pLand->GetSummon( m_sAssault.GaeaId );
			if ( pSummon )
			{
				ServerActor* pOwner = pSummon->GetOwner();
				if ( pOwner )
				{
					GLChar* pChar = static_cast< GLChar* >( pOwner );
					if ( pChar )
						dwID = pChar->CharDbNum();
				}
			}
		}

		// 주의! : dwDamage는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 데미지값 변경을 할 수 있게 하기 위함 );
		if ( false == pInstance->EventReceiveDamage(
			CROW_SUMMON,
			GetGaeaID(),
			emACrow,
			dwID,
			dwActualDamage,
			dwDamageFlag ) )  // return값이 false일 경우 기존의 데미지 처리를 하지 않겠다는 뜻;
		{
			return dwActualDamage;
		}
	}

    // Damage 적용.        
    const DWORD dwDxHp = RECEIVE_DAMAGE ( (WORD)dwDamage );

    // Damage 값이 있을때. 0 일때는 miss 발생 ( 타겟 지정을 위해 호출됨. )
    if ( dwDxHp > 0 )
    {
        //	Damage 기록.
        if ( emACrow == CROW_PC )
        {
            PGLCHAR	pChar = m_pGaeaServer->GetChar ( dwAID );

            if (pChar)
            {
                //AddDamageLog      ( m_cDamageLog, dwAID, pChar->GetUserID(), dwDxHp );
                //AddDamageLogParty ( m_cDamageLogParty, pChar->GetPartyID(), dwDxHp );
                if ( pChar->IsCTFPlayer() && dwDamageFlag != DAMAGE_TYPE_GATHER )
                {
                    pChar->CTFDamageTo(dwDxHp);
                }
                pChar->ToAllDamage_PVE(dwDxHp);
            }
        }
        // Note : 소환수 공격일때 m_cSummonDamageLog에 기록한다
        else if ( emACrow == CROW_SUMMON )
        {
			if ( pLand != NULL )
			{
				PGLSUMMONFIELD pSummon = pLand->GetSummon(dwAID);			
				if (pSummon)
				{
					if ( pSummon->m_sOwnerID.actorType == CROW_PC )
					{
						GLChar* pSummonOwner = static_cast< GLChar* >(
							m_pGaeaServer->GetActor(
							pLand,
							pSummon->m_sOwnerID ) );
						if (pSummonOwner)
						{
							//AddDamageLog(m_cSummonDamageLog, pSummonOwner->GetGaeaID(), pSummonOwner->GetUserID(), dwDxHp);
							//AddDamageLogParty(m_cDamageLogParty, pSummonOwner->GetPartyID(), dwDxHp);
							if ( pSummonOwner->IsCTFPlayer() && dwDamageFlag != DAMAGE_TYPE_GATHER )
							{
								pSummonOwner->CTFDamageTo(dwDxHp);
							}
							pSummonOwner->ToAllDamage_PVE(dwDxHp);
						}
					}
				}
			}
        }

        // 충격 받음.
        if( !m_pSummonCrowData->IsIgnoreShock() )
        {
            if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	
            {
                TurnAction ( GLAT_SHOCK );
            }
        }
    }

	// 캐릭터가 살아있을 경우에만 보낸다;
	if ( m_dwNowHP > 0 )
	{
		Update_AccumulateHit( dwDxHp, emACrow, dwAID );

		Faction::GLActorID sAttackerID( emACrow, dwAID );
		ServerActor* pAttacker =
			m_pGaeaServer->GetActor( pLand, sAttackerID );
		if ( pAttacker )
			SendToChildCrowAttacked( pAttacker );

		// Note : 일반타입일때 공격받으면 반격한다
		//
		if ( m_emTYPE == SUMMON_TYPE_NORMAL )
		{
			AISystem::CAICompField* pAI =
				static_cast< AISystem::CAICompField* >(
				GetComponent( ComponentSystem::EMFAMILY_AI ) );
			if ( pAI && pAttacker )
			{
				pAI->Attacked( pAttacker );
			}

			/*STARGETID sTargetID = STARGETID ( emACrow, dwAID );
			if( !isAction( GLAT_ATTACK, GLAT_SKILL ) &&
				GetReactionTarget( sTargetID ) )
			{
				SetReactionTarget( sTargetID );
			}*/
		}
	}

	/*if ( emACrow == CROW_PC )
	{
		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwAID );
		AddDamageLog      ( TurnAction, dwAID, pChar->GetUserID(), wDxHP );
		AddDamageLogParty ( m_cDamageLogParty, pChar->m_dwPartyID, wDxHP );
	}*/

	return dwDxHp;
}

// 회복 함수 통합 [9/26/2013 hsshin];
const DWORD GLSummonField::ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    DWORD dwActualHeal(dwHeal);
    InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        // dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }

        // 주의! : dwHeal는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 회복값 변경을 할 수 있게 하기 위함 );
        if ( _pInstance->EventReceiveHeal(emACrow, dwID, CROW_SUMMON, GetGaeaID(), dwActualHeal, dwHealFlag) == false )  // return값이 false일 경우 기존의 회복 처리를 하지 않겠다는 뜻;
            return dwActualHeal;
    }

    return ReceiveHeal_Procedure(emACrow, dwAID, dwActualHeal, dwHealFlag);
}
const DWORD GLSummonField::ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	m_sHeal.emCrow = emACrow;
	m_sHeal.GaeaId = dwAID;

    const DWORD wDxHP = GLSUMMON::RECEIVE_HEAL( (WORD) dwHeal );	

    if ( emACrow == CROW_PC )
    {
        GLChar* pChar = m_pGaeaServer->GetChar(dwAID);
        //
        //mjeon.CaptureTheField
        //
        if ( dwHealFlag != HEAL_TYPE_GATHER && pChar->IsCTFPlayer() )
        {
            pChar->CTFHealTo(wDxHP);
        }
        pChar->ToAllHeal_PVE(wDxHP);
    }
    else if ( emACrow == CROW_SUMMON )
    {        
        // Note : 주인이 친걸로 등록
        //
        PGLSUMMONFIELD pSummon = pLand->GetSummon(dwAID);
        if (pSummon)
        {
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				GLChar* pSummonOwner = static_cast< GLChar* >(
					m_pGaeaServer->GetActor(
					pLand,
					pSummon->m_sOwnerID ) );
				if (pSummonOwner)
				{
					//
					//mjeon.CaptureTheField
					//
					if ( dwHealFlag != HEAL_TYPE_GATHER && pSummonOwner->IsCTFPlayer() )
					{
						pSummonOwner->CTFHealTo(wDxHP);
					}
					pSummonOwner->ToAllHeal_PVE(wDxHP);
				}
			}
        }
    }

	return wDxHP;
}

BOOL GLSummonField::ShockProcess ( float fElapsedTime )
{
	VECANIATTACK &vecShock = m_pSummonCrowData->GetAnimation(AN_SHOCK,AN_SUB_NONE);
	if ( vecShock.empty() )	return FALSE;

	m_fIdleTime += fElapsedTime * GETATTVELO();

	SANIATTACK &sAniShock = vecShock[0];

	int wTotalKeys = int(sAniShock.m_dwETime) - int(sAniShock.m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);

	if ( wThisKey >= wTotalKeys )	return FALSE;

	return TRUE;
}

BOOL GLSummonField::IsOwnerShortDistance( D3DXVECTOR3 vCheckPos )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return FALSE;

	D3DXVECTOR3 vLeng = pOwner->GetPosition() - vCheckPos;
	float fTarLeng1 = D3DXVec3Length(&vLeng);

	vLeng  = m_vPosition - vCheckPos;
	float fTarLeng2 = D3DXVec3Length(&vLeng);

	if (fTarLeng1 > fTarLeng2)
		return TRUE;
    else
	    return FALSE;
}

void GLSummonField::SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor /*= true*/ )
{
	if ( !IsValidBody () )		return;

	// 밀려날 위치로 이동 시도.
	//
	BOOL bSucceed = m_actorMove.GotoLocation
		(
		D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
		D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
		);

	if ( bSucceed )
	{
		// 밀려나는 엑션 시작.
		//
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		// 밀리는 속도 설정.
		//
		m_actorMove.SetMaxSpeed ( fPUSH_SPEED );

		// 메시지 내용 설정.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow    = GETCROW();
		NetMsgBRD.dwID      = m_dwGaeaID;
		NetMsgBRD.vMovePos  = vMovePos;
		NetMsgBRD.dwAniType = dwAniType;

		// [자신의 주변 Char] 에게 Msg를 전달.
		//
		SendMsgViewAround(&NetMsgBRD);
	}
}

void GLSummonField::SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag )
{
    GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;

    if ( dwRemoveFlag >= SKILL::EMACTION_TYPE_NSIZE )
        return;

    //일반적인 스킬의 효과만 제거한다. (몹의 경우는 도시락,시스템 버프가 없다.);
    for ( size_t i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
    {
        if ( i >= CROW_SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
        if ( !pSkill )
            continue;

        //제거 불가 옵션;
        if ( pSkill->m_sBASIC.bNonEffectRemove )
            continue;

        //해당하는 스킬타입인지?
        if ( pSkill->m_sBASIC.emACTION_TYPE != (SKILL::EMACTION_TYPE)dwRemoveFlag )
            continue;

        //활성화 여부;
        if ( !m_sSKILLFACT[i].bEnable )
            continue;

        //스킬효과 종료;
		UPDATE_DATA_END(i);

        NetMsgSkillBrd.bRESET[i] = true;
    }
    //스킬효과 종료에 대해 알 수 있도록 패킷전송;
    NetMsgSkillBrd.dwID = m_dwGaeaID;
    NetMsgSkillBrd.emCrow = CROW_SUMMON;
    SendMsgViewAround(&NetMsgSkillBrd);	
}

void GLSummonField::DO_STUN_ACTION ()
{
	// 쓰러짐을 클라이언트 들에게 알림.
	GLMSG::SNET_ACTION_BRD NetMsgBrd;
	NetMsgBrd.emCrow	= GETCROW();
	NetMsgBrd.dwID		= m_dwGaeaID;
	NetMsgBrd.emAction	= GLAT_IDLE;

	// Msg를 전달.
	//
	SendMsgViewAround(&NetMsgBrd);

	TurnAction ( GLAT_IDLE );
	//SetActState( EM_SUMMONACT_STOP );		
}

void GLSummonField::DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sACTOR );

    if ( !pACTOR )
    {
        return;
    }

    int nActualDamageHP = (int)-nDAMAGE;
    int nActualDamageMP(0);
    int nActualDamageSP(0);
    //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
    VAR_BODY_POINT( sACTOR.emCrow, sACTOR.GaeaId, DAMAGE_TYPE_CURSE, nActualDamageHP, nActualDamageMP, nActualDamageSP );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
	NetMsgBRD.emCrow = GetCrow();
	NetMsgBRD.dwID   = m_dwGaeaID;

	//	스킬을 사용한 행위자 마킹.
	NetMsgBRD.sACTOR.emCrow = sACTOR.emCrow;
	NetMsgBRD.sACTOR.dwID   = sACTOR.GaeaId;

	NetMsgBRD.dwDamageFlag = DAMAGE_TYPE_CURSE;
	NetMsgBRD.nVAR_HP = nActualDamageHP;
	NetMsgBRD.nVAR_MP = nActualDamageMP;
	NetMsgBRD.nVAR_SP = nActualDamageSP;

    NetMsgBRD.wSchool = pACTOR->GetSchool();

	SendMsgViewAround(&NetMsgBRD);
}

const bool GLSummonField::IsImmuneToSkill ( BYTE iIndex ) const
{
	if ( RF_IMMUNE( m_EffSkillVarSet ).IsOn() )
	{
		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iIndex ) )
		{
			return true;
		}
	}

	return false;
}

void GLSummonField::DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR)
{
	// 대상자에게 대미지값 적용.
	const DWORD dwActualDamage = ToDamage ( sACTOR, nDAMAGE, 0 );

	// 주변 클라이언트들에게 메세지 전송.
	GLMSG::SNETCROW_DAMAGE NetMsg(
        m_dwGaeaID,
        sACTOR.emCrow,
        sACTOR.GaeaId,
        dwActualDamage,
        m_dwNowHP,
        DAMAGE_TYPE_NONE);
	SendMsgViewAround(&NetMsg);
}

namespace
{
	typedef std::pair<int,int> PRIOR_PAIR;
	class prior_sort
	{
	public:
		bool operator () ( const PRIOR_PAIR &lvalue, const PRIOR_PAIR &rvalue )
		{
			return lvalue.first > rvalue.first;
		}
	};

	// float g_fMINNEAR(15.0f);
	float g_fMINNEAR(21.0f);
	float g_fMAXNEAR(35.0f);
};

void GLSummonField::Move ( const D3DXVECTOR3& vPos, bool bMsg, bool bSpeedUp )
{
	D3DXVECTOR3 vDist = m_vPosition - vPos;
	float fDist = D3DXVec3Length ( &vDist );
	if ( fDist < 1.0f )
	{
		return;
	}

	BOOL bSucceed = m_actorMove.GotoLocation
		(
			vPos+D3DXVECTOR3(0,+10,0),
			vPos+D3DXVECTOR3(0,-10,0)
		);

	if ( bSucceed )
	{
		m_bSpeedUp = bSpeedUp;

		//SetActState ( EM_SUMMONACT_MOVE );

		m_TargetID.vPos = vPos;

		m_actorMove.SetMaxSpeed ( GetMoveVelo() );

		TurnAction( GLAT_MOVE, bMsg );
	}
}

void GLSummonField::Move_TargetTracing ( const STARGETID& sTarget, const WORD wReactionRange, bool IsOwnerTracing, bool bSpeedUp )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ReSetActState(EM_ACT_ALL);
	//SetActState ( EM_SUMMONACT_TRACING );
	SetActState ( EM_ACT_RUN );

	ServerActor *pTARGET = m_pGaeaServer->GetTarget ( pLand, sTarget );
	if ( !pTARGET || !pTARGET->IsValidBody() )
		return;

	//WORD wActionAble    = wTarBodyRadius + GETBODYRADIUS() + wReactionRange + 3;
	WORD wTarBodyRadius = pTARGET->GetBodyRadius();
	WORD wActionRangeMin = static_cast<WORD> ( (wTarBodyRadius+GETBODYRADIUS()) ); //*0.4f
	WORD wActionRangeMax = wActionRangeMin + wReactionRange; // wTarBodyRadius + GETBODYRADIUS() + wReactionRange;
	WORD wTracingRange   = wActionRangeMax; // WORD ( wActionRangeMax * 1.2f );

	D3DXVECTOR3 vEndDist;
	float       fEndDist;

	if ( m_actorMove.PathIsActive() )
	{
		vEndDist = m_actorMove.GetTargetPosition() - pTARGET->GetPosition();
		fEndDist = D3DXVec3Length ( &vEndDist );
	}
	else
	{
		vEndDist = m_vPosition - pTARGET->GetPosition();
		fEndDist = D3DXVec3Length ( &vEndDist );
	}

	//	이동이 끝낫거나 추적 위치랑 타겟위치가 너무 떨어졌을 경우 다시 이동한다
	//
	//m_actorMove.PathIsActive()
	//	IsSTATE GLATMOVE
	if ( fEndDist>wTracingRange || fEndDist < ((float)wActionRangeMin-D3DX_16F_EPSILON) )
	{
		D3DXVECTOR3 vRandPosition;

		if ( !GetRandomTargetPos ( vRandPosition, pTARGET, wActionRangeMin, wActionRangeMax, IsOwnerTracing ) )
		{
			return;
		}

		m_TargetID = sTarget;

		Move ( vRandPosition, true, bSpeedUp );	
	}
}

const BOOL GLSummonField::GetRandomTargetPos ( D3DXVECTOR3& vOutPos, const ServerActor* pTarget, const float fDistanceMin, const float fDistanceMax, bool IsOwnerTracing )
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	if ( pTarget == NULL )
		return FALSE;

	boost::shared_ptr<NavigationMesh> spNaviMesh = pLand->GetNavi();

	if ( spNaviMesh == NULL )
		return FALSE;

	D3DXVECTOR3 vDir(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 vTargetPos = pTarget->GetPosition();
	FLOAT       fRadian    = 0.0f;

	// Note : IsOwnerTracing는 주인 추적 여부다
	//
	if ( IsOwnerTracing )
	{
		vDir    = pTarget->GetDirect();
		vDir.x  = -vDir.x;
		vDir.z  = -vDir.z;

		fRadian = (m_fOwnerDegree/360.0f) * (D3DX_PI*2.0f); //D3DX_PI * (((FLOAT)(rand()%21) - 10.0f) / 100.0f);
	}
	else
	{
		fRadian = (D3DX_PI*2.0f) * (((FLOAT)(rand()%100)/100.0f));
	}

	FLOAT fDistanceRate = ((FLOAT)(rand()%101)/100.0f);
	FLOAT fDistance     = (fDistanceMax * fDistanceRate) + (fDistanceMin * (1.0f - fDistanceRate));

	D3DXMATRIX matRotY;
	D3DXMatrixRotationY(&matRotY, fRadian);
	D3DXVec3TransformCoord ( &vDir, &vDir, &matRotY );
	vOutPos = vTargetPos + (vDir * fDistance);

	// Note : 유효한 위치인지 점검
	//

	BOOL        bNaviCo(FALSE);
	DWORD       dwCoID(0);
	D3DXVECTOR3 vNaviCo(0,0,0);
	D3DXVECTOR3 vPos1(vOutPos.x,vOutPos.y+5.0f,vOutPos.z);
	D3DXVECTOR3 vPos2(vOutPos.x,vOutPos.y-5.0f,vOutPos.z);

	spNaviMesh->IsCollision ( vPos1, vPos2, vNaviCo, dwCoID, bNaviCo );

	if ( !bNaviCo )
	{
		vOutPos = vTargetPos;
		return TRUE;
	}

	D3DXVECTOR3 vSuitable (vOutPos.x,    vOutPos.y+15.0f,    vOutPos.z);
	D3DXVECTOR3 vTarget   (vTargetPos.x, vTargetPos.y+15.0f, vTargetPos.z);

	BOOL bCollision = pLand->IsCollision( vSuitable, vTarget, EMCC_CULL_NONE );
	if ( bCollision )
	{
		vOutPos = vTargetPos;
		return TRUE;
	}

	return TRUE;
}

GLSummonField::EMRETURN_SETREACTION GLSummonField::SetReactionTarget( STARGETID sTargetID, bool bEventSkill )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return EMRETURN_SETREACTION_FAILED;

	EMREACTION emSelectReaction = EMREACT_NONE;

	if (!IsValidBody())
        return EMRETURN_SETREACTION_FAILED;

	ServerActor *pTARGET = m_pGaeaServer->GetTarget ( pLand, sTargetID );
	if( !pTARGET )
	{
		sc::writeLogInfo (
			sc::string::format( "Invalid Target '%1% %2%'."
			, sTargetID.emCrow
			, sTargetID.GaeaId ) );

		m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
		m_wRangeTracing       = 0;
		return EMRETURN_SETREACTION_FAILED;	
	}

	D3DXVECTOR3 vDist = pTARGET->GetPosition () - m_vPosition;
	float		fDist = D3DXVec3Length( &vDist );

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return EMRETURN_SETREACTION_FAILED;	

	D3DXVECTOR3 vOwnerDist = pOwner->GetPosition() - pTARGET->GetPosition();
	float       fOnwerDist = D3DXVec3Length( &vDist );

	if ( !m_bUsingEventSkill )
	{
		DWORD dwFoundAType = m_dwAtypeAfterTracing;
		
		// Note : 추적 후 AType이 등록되어 있으면 찾지 않는다 .
		if ( dwFoundAType == SCROWDATA::EMMAXATTACK )
		{
			dwFoundAType = FindOptReactionType ( sTargetID, true );
		}

		if ( dwFoundAType >= m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size() ||
			dwFoundAType < 0 )
		{
			//m_TargetID.RESET();
			//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			//m_wRangeTracing       = 0;
			//m_pAttackProp = NULL;
			TurnAction( GLAT_IDLE );
			//SetAction( GLAT_IDLE );
			return EMRETURN_SETREACTION_FAILED_SKILL;
		}

		// Note : 공격 타입과 사정거리를 설정한다
		//
		m_dwAType       = m_dwAtypeAfterTracing = dwFoundAType;
		m_sAttackProp   = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType];
	}
	else
	{

		CROWATTACK_VEC vecTemp = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage );
		if ( m_dwEventSkillIdx < 0 || m_dwEventSkillIdx >= vecTemp.size() )
		{
			sc::writeLogInfo( "Invalid EventSkill Index." );
			return EMRETURN_SETREACTION_FAILED;
		}
		m_dwAType       = m_dwAtypeAfterTracing = m_dwEventSkillIdx;
		m_sAttackProp = vecTemp.at( m_dwEventSkillIdx );

		//sc::writeLogInfo( "Setting Event Skill AttackProp." );
	}

	if ( !m_sAttackProp.bUsed )
		return EMRETURN_SETREACTION_FAILED;
	
	if ( m_sAttackProp.skill_id==SNATIVEID(false) )
	{
		m_wRangeTracing = m_sAttackProp.wRange;
	}
	else
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sAttackProp.skill_id );

		m_wRangeTracing = pSkill->m_sBASIC.wTARRANGE;
	}

	// 
	if( fDist >= GetBodyRadius() || !m_pSummonCrowData->m_bOverlapAttack )
	{
		// Note : 0일시 즉시 사용할수 있는 스킬이다
		//
		if ( m_wRangeTracing != 0 && m_emTYPE != SUMMON_TYPE_WAY_1 && m_emTYPE != SUMMON_TYPE_WAY_2 )
		{
			// Note : 액션 가능거리 조사
			//
			WORD wRange          = m_wRangeTracing;
			WORD wActionRangeMin = static_cast<WORD> (pTARGET->GetBodyRadius() + GETBODYRADIUS());
			WORD wActionRangeMax = wActionRangeMin + wRange;

			// 스킬사거리보다 거리가 길면 실패;
			if ( wActionRangeMax < fDist  )
			{
				SetAction( GLAT_MOVE );

				return EMRETURN_SETREACTION_FAILED_RANGE;	
			}

			// 최소사거리보다 짧으면;
			if ( fDist < ((float)wActionRangeMin-D3DX_16F_EPSILON) )
			{
				// 겹침 가능의 경우 실패하지 않는다;
				if ( !m_pSummonCrowData->m_bOverlapAttack )
				{
					SetAction( GLAT_MOVE );

					return EMRETURN_SETREACTION_FAILED_RANGE;
				}
			}

			// 시야가 확보되는가
			//
			D3DXVECTOR3 vFinderPos(m_vPosition.x,m_vPosition.y+15.0f,m_vPosition.z);
			D3DXVECTOR3 vTargetPos(pTARGET->GetPosition());
			vTargetPos.y += 15.0f;

			BOOL bCollision = pLand->IsCollision( vFinderPos, vTargetPos, EMCC_CULL_NONE );
			if ( bCollision )
			{
				SetAction( GLAT_MOVE );

				return EMRETURN_SETREACTION_FAILED_COLLISION;
			}
		}
	}

	// Note : 스킬인지 여부 조사
	//
	if ( m_sAttackProp.skill_id==SNATIVEID(false) )
	{
		// 공격이다
		emSelectReaction = EMREACT_ATTACK;
	}
	else
	{
		// 스킬이다
		if ( EMSKILL_OK != GLSUMMON::CHECHSKILL(m_dwAType, false, m_bUsingEventSkill ) )
		{
			m_TargetID.RESET();
			//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			//m_wRangeTracing       = 0;
			//m_pAttackProp = NULL;
			//SetAction( GLAT_IDLE );
			TurnAction( GLAT_IDLE );
			return EMRETURN_SETREACTION_FAILED_SKILL;
		}


		PGLSKILL  pSkill    = GLSkillMan::GetInstance().GetData ( m_sAttackProp.skill_id );
		SNATIVEID sRunSkill = m_sAttackProp.skill_id;
		WORD      wLevel    = ((WORD)pSkill->m_sBASIC.dwMAXLEVEL < m_wOwnerSkillLevel) ? (WORD)pSkill->m_sBASIC.dwMAXLEVEL : (WORD)m_wOwnerSkillLevel; // Note : 해당 스킬의 최대 레벨을 가져온다.

		D3DXVECTOR3 vTarPos(0,0,0);
		BOOL bOK = UpdateSkillTarget ( sTargetID, &vTarPos, sRunSkill, wLevel );
		if ( !bOK )
		{
			m_TargetID.RESET();
			//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			//m_wRangeTracing       = 0;
			//m_pAttackProp = NULL;
			//SetAction( GLAT_IDLE );
			TurnAction( GLAT_IDLE );
			return EMRETURN_SETREACTION_FAILED_SKILL;
		}

		GLSUMMON::SETACTIVESKILL ( m_sAttackProp.skill_id, wLevel );
		emSelectReaction = EMREACT_SKILL;
	}

	// Note : 무사히 완료
	//

	m_TargetID = sTargetID;
	m_vTARPOS  = pTARGET->GetPosition();

	switch ( emSelectReaction )
	{
	case EMREACT_ATTACK: // 공격
		{
			m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			m_wRangeTracing       = 0;

			ReSetActState(EM_ACT_ALL);
			//SetActState ( EM_SUMMONACT_ATTACK );
			TurnAction( GLAT_ATTACK );
			++m_dwReactCount; // 공격 횟수 증가
		}
		break;

	case EMREACT_SKILL:  // 스킬
		{
			m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
			m_wRangeTracing       = 0;

			ReSetActState(EM_ACT_ALL);
			//SetActState ( EM_SUMMONACT_SKILL );
			TurnAction( GLAT_SKILL );

			++m_dwReactCount; // 공격 횟수 증가

			// 이벤트 스킬 초기화;
			if ( m_bUsingEventSkill )
			{
				m_bUsingEventSkill = false;
				//sc::writeLogInfo( "End Event Skill Mode." );
			}
		}
		break;
	}

	return EMRETURN_SETREACTION_SUCCESS;
}

void GLSummonField::DetectReactionableTarget( STARGETID& sOutTargetID )
{
	sOutTargetID = STARGETID();

	switch ( m_emTYPE )
	{
		case SUMMON_TYPE_BUFF:
			{
				GLLandMan* const pLand( GetLandMan() );
				if ( pLand == NULL )
					break;

				ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
                //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
                if (!pOwner)
                    break;

				// 파티의 멤버를 검색한다
				GLPartyField* const pMyParty = m_pGaeaServer->GetParty(pOwner->GetPartyID());

				if ( pMyParty == NULL )
				{ // 파티 중이 아니라면 주인을 따라간다;
					sOutTargetID.emCrow = pOwner->GetCrow();
					sOutTargetID.GaeaId = pOwner->GetGaeaID();
					break;
				}

				PartyMemberGaeaIDs dwPartyMemberGaeaIDs;
				const DWORD nPartyMember = pMyParty->GetPartyMemberGaeaIDs(dwPartyMemberGaeaIDs);				

				// Note : 주인의 파티 인덱스를 찾는다.
				DWORD dwOwnerIndex = 0;

				for ( dwOwnerIndex = 0; dwOwnerIndex < nPartyMember; ++dwOwnerIndex )
				{
					if ( dwPartyMemberGaeaIDs[dwOwnerIndex] == m_dwOwnerGaeaID )
					{
						break;
					}
				}

				// Note : 실패하면 주인을 타겟
				if ( nPartyMember == 0  || dwOwnerIndex >= nPartyMember )
				{
					sOutTargetID.emCrow = pOwner->GetCrow();
					sOutTargetID.GaeaId   = pOwner->GetGaeaID();
					break;
				}

				// Note : 해당 타겟이 범위 내에있는지 검색 한다.
				//

				STARGETID   sTempTargetID;
				D3DXVECTOR3 vOwnerPosition = pOwner->GetPosition();
				FLOAT       fRange         = (m_fPartyDistance*m_fPartyDistance);
				DWORD       i              = 0;

				for ( i=0; i<nPartyMember; ++i )
				{
					DWORD dwIndex      = (m_dwReactCount + dwOwnerIndex + i ) % nPartyMember;
					DWORD dwTargetGUID = dwPartyMemberGaeaIDs[dwIndex];

					sTempTargetID.emCrow = CROW_PC;
					sTempTargetID.GaeaId   = dwTargetGUID;

					ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, sTempTargetID);

					if ( !pActor )
					{
						continue;
					}

					D3DXVECTOR3 vDistance = pActor->GetPosition() - vOwnerPosition;
					FLOAT       fDistance = D3DXVec3LengthSq( &vDistance );

					if ( fDistance > fRange )
					{
						continue;
					}

					break;
				}

				// Note : 한명도 찾지 못했다면 주인을 따라간다
				if ( i == nPartyMember )
				{
					sOutTargetID.emCrow = pOwner->GetCrow();
					sOutTargetID.GaeaId = pOwner->GetGaeaID();
					break;
				}

				sOutTargetID = sTempTargetID;
				
			}
			break;
	}
}

DWORD GLSummonField::FindOptReactionType ( const STARGETID &sTargetID, bool branvar )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return SCROWDATA::EMMAXATTACK;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
        return SCROWDATA::EMMAXATTACK; 

	ServerActor* pTarget = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if (!pTarget)
        return SCROWDATA::EMMAXATTACK;

	// Note :  해당 타겟이 적인지
	const bool bEnemy = IsReActionable(pTarget, EMIMPACT_SIDE_ENEMY) ? true : false;

	// Note : 타겟이 달리는지
	bool bTarStateRun = (pTarget->IsRunning()==TRUE);

	D3DXVECTOR3 vDX     = m_vPosition - pTarget->GetPosition();
	float       fLength = D3DXVec3Length ( &vDX );

	// 근접이 우선인지 점검.
	bool bprior_near(false);
	if ( g_fMAXNEAR>fLength && !bTarStateRun )	bprior_near = true;

	std::vector<PRIOR_PAIR> vecPRIOR;
	vecPRIOR.clear ();

	DWORD dwAttSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();

	for ( DWORD i=0; i<dwAttSize; ++i )
	{
		const SCROWATTACK &sATTACK = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i];

		if ( !sATTACK.bUsed )
			continue;

		if ( m_fACTIONDELAY[i] > 0.0f )
			continue;

		bool bskill(false);
		bool bnear(false);

		// 스킬일 경우
		if ( sATTACK.skill_id!=SNATIVEID(false) )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sATTACK.skill_id );
			if ( !pSkill )
				continue;

			// 이동속도가 없는 경우 스킬의 사정거리가 타겟과의 거리보다 커야한다;
			if ( GetMoveVelo() <= 0.f )
			{
				D3DXVECTOR3 vLength = pTarget->GetPosition() - GetPosition();
				float fTargetLength = D3DXVec3Length( &vLength );
				if ( pSkill->m_sBASIC.wTARRANGE <= fTargetLength )
					continue;
			}

			//	딜래이 시간, 조건수치를 확인하여 사용 가능한지 점검.
			EMSKILLCHECK emcheck = CHECHSKILL ( DWORD(i) );
			if ( emcheck!=EMSKILL_OK && emcheck!=EMSKILL_NOTSP )
				continue;

			// 해당 스킬의 타겟이 올바른지 점검
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
				case EMIMPACT_SIDE_PARTY:
					if ( bEnemy  )
						continue;
					break;
				case EMIMPACT_SIDE_ANYBODY:
					if ( bEnemy  )
						continue; 
					break;
				case EMIMPACT_SIDE_ENEMY:  
					if ( !bEnemy ) 
						continue; 
					break;
			}

			bskill = true;

			WORD wTARRANGE = GETSKILLRANGE_TAR( *pSkill );

			if ( wTARRANGE < g_fMAXNEAR )
				bnear = true;
			else
				bnear = false;
		}
		// 일반 공격이면 적일 경우만
		else if ( bEnemy )
		{
			if ( sATTACK.emAttRgType == EMATT_SHORT )
				bnear = true;
			else
				bnear = false;
		}
		else
		{
			continue;
		}

		//	우선순위 계산.
		int nprior(0);

		//	우선순위에 랜덤성 수치를 약간 부여. ( 랜덤 선택 )
		if (branvar)
            nprior += int((sc::Random::RANDOM_POS()) * dwAttSize);

		//	스킬의 경우 가산점.
		if (bskill)
            nprior += 11;

		//	현제 근접이 유리한가 장거리가 유리한가에 따라 우선 순위 변경.
		if (bprior_near)
		{
			//	근접 우선.
			if (bnear)
                nprior += 10;
			else
                nprior += 5;
		}
		else
		{
			//	장거리 우선.
			if (bnear)
                nprior += 5;
			else
                nprior += 10;
		}

		vecPRIOR.push_back ( std::make_pair(nprior,i) );
	}

	if ( vecPRIOR.empty() )		return SCROWDATA::EMMAXATTACK;

	std::sort ( vecPRIOR.begin(), vecPRIOR.end(), prior_sort() );

	const PRIOR_PAIR &sPRIOR = *vecPRIOR.begin();

	GASSERT ( sPRIOR.second < (int)dwAttSize );
	if ( sPRIOR.second >= (int)dwAttSize )	return SCROWDATA::EMMAXATTACK;

	return sPRIOR.second;
}

const EMREACTIONABLE GLSummonField::IsReActionable ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide )
{
	GASSERT(pTARGET&&"GLSummonField::IsReActionable()");

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || pTARGET == NULL )
        return EMREACTIONABLE_FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
	//GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return EMREACTIONABLE_FALSE;	
	if (!pTARGET)
		return EMREACTIONABLE_FALSE;

	// 타겟 불가능 하면 타겟을 못한다;
	if ( pTARGET->IsNonTargetable() )
		return EMREACTIONABLE_FALSE;

    // Note : 타겟이 소환수라면 대상 타겟검사를 소환수의 주인으로 바꾼다
    //
    if ( pTARGET->GetCrow() == CROW_SUMMON )
    {
        PGLSUMMONFIELD pSummon = pLand->GetSummon( pTARGET->GetGaeaID() );

        if ( !pSummon )
        {
            return EMREACTIONABLE_FALSE;
        }

        pTARGET = m_pGaeaServer->GetTarget( pLand, STARGETID(CROW_PC, pSummon->m_dwOwnerGaeaID) );

        if( !pTARGET )
        {
            return EMREACTIONABLE_FALSE;
        }
    }
	
	//  [4/12/2016 gbgim]
	// MobNpcEdit에서 세팅하는 일부 옵션은 주인과 상태가 다를 수 있다;
// 	if ( pOwner->GetCrow() == CROW_PC )
// 	{
// 		return static_cast<GLChar*>( pOwner )->IsReActionable(pTARGET, _emTargetSide);
// 	}
	
	if ( FALSE == pTARGET->IsVisibleDetect( this ) )
		return EMREACTIONABLE_FALSE;

	// 팩션 매니저만 따로 주인이있다면 처리해준다;
	if ( pOwner->GetCrow() == CROW_PC )
	{
		const Faction::ManagerField* const _pFactionMan(pLand->GetFactionManager());
		if ( _pFactionMan )
			return static_cast<GLChar*>( pOwner )->IsReActionableFaction(pTARGET, _emTargetSide);
		else
			return static_cast<GLChar*>( pOwner )->IsReActionableNonFaction(pTARGET, _emTargetSide);
	}
	
	return EMREACTIONABLE_TRUE;
}

inline void GLSummonField::INSERT_PC_VIEW ( DWORD dwGUID )
{
	if ( m_vecPC_VIEW.size() < m_dwPC_AMOUNT+16 )
		m_vecPC_VIEW.resize(m_dwPC_AMOUNT+16);

	m_vecPC_VIEW[m_dwPC_AMOUNT++] = dwGUID;
}

inline void GLSummonField::INSERT_CROW_VIEW ( DWORD dwGUID )
{
	if ( m_vecCROW_VIEW.size() < m_dwCROW_AMOUNT+16 )
		m_vecCROW_VIEW.resize(m_dwCROW_AMOUNT+16);

	m_vecCROW_VIEW[m_dwCROW_AMOUNT++] = dwGUID;
}

inline void GLSummonField::INSERT_SUMMON_VIEW ( DWORD dwGUID )
{
	if ( m_vecSUMMON_VIEW.size() < m_dwSUMMON_AMOUNT+16 )
		m_vecSUMMON_VIEW.resize(m_dwSUMMON_AMOUNT+16);

	m_vecSUMMON_VIEW[m_dwSUMMON_AMOUNT++] = dwGUID;
}

HRESULT GLSummonField::UpdateViewAround ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	int nX, nZ;

	m_dwPC_AMOUNT     = 0;
	m_dwCROW_AMOUNT   = 0;
	m_dwSUMMON_AMOUNT = 0;
	
	// 해당 타겟은 무조건 가져온다.
	//
	if ( m_TargetID.GaeaId != EMTARGET_NULL )
	{
		switch ( m_TargetID.emCrow )
		{
			case CROW_PC:
				{
					GLChar* pChar = m_pGaeaServer->GetChar ( m_TargetID.GaeaId );

					if ( pChar )
					{
						INSERT_PC_VIEW ( m_TargetID.GaeaId );
					}
				}
				break;

			case CROW_MOB:
				{
					GLCrow* pCrow = pLand->GetCrow( m_TargetID.GaeaId );

					if ( pCrow )
					{
						INSERT_CROW_VIEW ( m_TargetID.GaeaId );
					}
				}
				break;

			case CROW_SUMMON:
				{
					GLSummonField* pSummon = pLand->GetSummon( m_TargetID.GaeaId );

					if ( pSummon )
					{
						INSERT_SUMMON_VIEW ( m_TargetID.GaeaId );
					}
				}
				break;
		}
	}

	// 현재 시야에 들어오는 CELL 모두 탐색.
	//
	nX = int ( m_vPosition.x );
	nZ = int ( m_vPosition.z );
	LANDQUADNODE* pQuadHead = NULL;
	const int _maxViewRange(GETMAXVIEWRANGE());
	BOUDRECT bRect(nX+_maxViewRange,nZ+_maxViewRange,nX-_maxViewRange,nZ-_maxViewRange);
	pLand->GetLandTree()->FindNodes ( bRect, pLand->GetLandTree()->GetRootNode(), &pQuadHead );

	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode ) continue;

		if ( pQuadNode->pData->m_PCList.m_dwAmount!=0 )
		{
			// Char
			//
			GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				GLChar* pPChar = pCharNode->Data;
				if( !pPChar ) continue;

				// 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
				if ( !pPChar )													
					continue;

				if ( !m_pGaeaServer->GetChar ( pPChar->GetGaeaID() ) )	
					continue;

				nX = int(pPChar->GetPosition().x); nZ = int(pPChar->GetPosition().z);

				// Note : 해당 타겟과 중복 방지
				if ( (m_TargetID.emCrow != pPChar->GetCrow() || m_TargetID.GaeaId != pPChar->GetGaeaID()) && bRect.IsWithIn(nX,nZ) )
				{
					INSERT_PC_VIEW ( pPChar->GetGaeaID() );
				}
			}
		}

		if ( pQuadNode->pData->m_CROWList.m_dwAmount!=0 )
		{
			// Crow
			//
			GLCROWNODE *pCrowNode = pQuadNode->pData->m_CROWList.m_pHead;
			for ( ; pCrowNode; pCrowNode = pCrowNode->pNext )
			{
				GLCrow* pCrow = pCrowNode->Data;
				if( !pCrow ) continue;

				nX = int(pCrow->GetPosition().x); nZ = int(pCrow->GetPosition().z);

				if ( (m_TargetID.emCrow != pCrow->GetCrow() || m_TargetID.GaeaId != pCrow->GetGaeaID()) && bRect.IsWithIn(nX,nZ) )
				{
					INSERT_CROW_VIEW ( pCrow->GetGaeaID() );
				}
			}
		}

		if ( pQuadNode->pData->m_SummonList.m_dwAmount!=0 )
		{
			// Summon
			//
			GLSUMMONNODE *pSummonNode = pQuadNode->pData->m_SummonList.m_pHead;
			for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
			{
				GLSummonField* pSummon = pSummonNode->Data;
				if( !pSummon || pSummon->m_dwGaeaID == m_dwGaeaID ) continue;

				nX = int(pSummon->m_vPosition.x); nZ = int(pSummon->m_vPosition.z);

				if ( (m_TargetID.emCrow != pSummon->GetCrow() || m_TargetID.GaeaId != pSummon->m_dwGaeaID) && bRect.IsWithIn(nX,nZ) )
				{
					INSERT_SUMMON_VIEW ( pSummon->m_dwGaeaID );
				}
			}
		}
	}

	return S_OK;
}

HRESULT GLSummonField::SendMsgViewAround(NET_MSG_GENERIC* pMsg)
{
	DWORD dwGAEAID = 0;
	GLChar* pPChar = NULL;

	DWORD wSize = (DWORD)m_vecPC_VIEW.size();

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<wSize; ++i )
	{
		//	Memo :	m_vecPC_VIEW[i] is GaeaID
		pPChar = m_pGaeaServer->GetChar ( m_vecPC_VIEW[i] );
		if ( pPChar )
		{
			m_pGaeaServer->SENDTOCLIENT(pPChar->ClientSlot(), pMsg);
		}
	}

	return S_OK;
}
void GLSummonField::SendToClient(NET_MSG_GENERIC* pMsg){
	GLSummonField::SendMsgViewAround(pMsg);
}
void GLSummonField::SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg)
{
	if (m_pGaeaServer)
		m_pGaeaServer->SENDTOCLIENT(dwClientSlot, pMsg);
}

void GLSummonField::DisableSkillFact()
{	
	for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
	{
		if (i >= CROW_SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill )
            continue;

		RESETSKEFF( i );
	}
}

void GLSummonField::Order_Die()
{
	const EMACTIONTYPE& emAction = GetAction();
	switch ( emAction )
	{
	case GLAT_IDLE:
		{
			// 생성중일때만 등록
			if ( IsActState(EM_ACT_CREATE) ) 
			{
				SSCHEDULEDACTION sSchedule;
				sSchedule.emAction = EMSCHEDULETYPE_DIE;
				TurnScheduledAction ( sSchedule );
			}
			else
			{
				m_fRemainedTime = 0.0f;
				m_dwNowHP       = 0;
				TurnAction(GLAT_FALLING);
			}
		}
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		{
			SSCHEDULEDACTION sSchedule;
			sSchedule.emAction = EMSCHEDULETYPE_DIE;
			TurnScheduledAction ( sSchedule );
		}
		break;

	default:
		{
			m_fRemainedTime = 0.0f;
			m_dwNowHP       = 0;
			TurnAction(GLAT_FALLING);
		}
	};
}

float GLSummonField::GetMoveVelo()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0.f;

    bool bRun = false;
	float fWalkVelo = 0.0f;
    float fRunVelo = 0.0f;
	fWalkVelo = m_pSummonCrowData->m_sAction.m_fWalkVelo;

	if ( fWalkVelo <= 0.f )
		return 0.f;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	// 주인의 속도를 따라가는지 여부
	if (pOwner && m_bOwnerSpeed)
    {
        bRun = TRUE;
		fWalkVelo = fRunVelo = pOwner->GetMoveVelo();
    }
	else
    {
        bRun = IsActState(EM_ACT_RUN);
		fWalkVelo = m_pSummonCrowData->m_sAction.m_fWalkVelo;
        fRunVelo  = m_pSummonCrowData->m_sAction.m_fRunVelo;

		if ( pOwner )
		{
			fWalkVelo += ( pOwner->GetMoveVelo() * m_fOwnerMovePer );
			fRunVelo += ( pOwner->GetMoveVelo() * m_fOwnerMovePer );
		}
    }

	if ( m_bSpeedUp )
		return AISystem::GLFSMContainer::Instance()->GetSpeedUpValue();	// 고정으로 설정하였다;
	else
	{
		return GLOGICEX::GetMoveVelocity(
			bRun,
			fRunVelo,
			fWalkVelo,
			false,
			GLSUMMON::GETMOVEVELO(),
			0.0f,
            m_fMoveVeloLimit_Min,
            m_fMoveVeloLimit_Max);
	}
}

const bool GLSummonField::CheckHit(const STARGETID &cTargetID)
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	ServerActor* const pTargetActor =
		m_pGaeaServer->GetTarget( pLand, cTargetID );
	if ( pTargetActor == NULL )
		return false;
	
	const EMBRIGHT emBright = pTargetActor->GetBright();
    const EM_BRIGHT_FB bFB = GLOGICEX::GLSPACEGAP(GETBRIGHT(), emBright, pLand->GETBRIGHT());
	const int nAVOID = pTargetActor->GetAvoid();

	const int nHitRate = GLOGICEX::GLHITRATE(GETHIT(), nAVOID, bFB);

    return (nHitRate > (sc::Random::RANDOM_POS()*100));
}

DWORD GLSummonField::GETCLIENTID()
{ 
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
	//GLChar* pChar = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (pOwner)
        return pOwner->GETCLIENTID();
    else 
        return UINT_MAX; 
}

WORD GLSummonField::GetSchool() const
{
	GLLandMan* pLand = m_pGaeaServer->GetLand(GetCurrentMap());

	if ( pLand == NULL )
		return 0;

	{
		if ( m_sOwnerID.actorType == CROW_PC )
		{
			GLChar* pSummonOwner =
				static_cast< GLChar* >(
				m_pGaeaServer->GetActor( pLand, m_sOwnerID ) );				

			if (pSummonOwner)
			{
				return pSummonOwner->GetSchool();
			}
		}
	}

    return 0;
}

SCONFTING* GLSummonField::GETCONFTING()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return NULL;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pChar = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (pOwner)
        return pOwner->GETCONFTING();
    else
        return NULL;
}

void GLSummonField::DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLEVEL  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );

	ServerActor* pActorCaster = pLand->GetActor(sCasterID);

	STARID				sTARIDS[EMTARGET_NET];
	WORD				wTARNUM;

	pLand->m_SkillTargetSystem.SelectSkillTarget( pActorCaster, this, sSkillID, wLEVEL, GetPositionActor(), sTARIDS, wTARNUM);


	if ( sCasterID.emCrow == CROW_PC )
	{
		GLChar* pChar = pLand->GetChar(sCasterID.GaeaId);
		if ( pChar == NULL )
			return;

		STARGETID sTargetID;
		sTargetID.emCrow = GetCrow();
		sTargetID.GaeaId = GetGaeaID();
		sTargetID.vPos = GetPosition();

		pChar->ApplySkillDealy( SCHARSKILL( sSkillID, wLEVEL), sTargetID, wTARNUM, sTARIDS, 1 );
	}

	if ( sCasterID.emCrow == CROW_MOB )
	{
		GLCrow* pCrow = pLand->GetCrow(sCasterID.GaeaId);
		if ( pCrow == NULL )
			return;
		pCrow->ApplySkill(TRUE, sSkillID, wLEVEL, wTARNUM, sTARIDS);
	}

	if ( sCasterID.emCrow == CROW_SUMMON )
	{
		GLSummonField* pSummon = pLand->GetSummon(sCasterID.GaeaId);
		if ( pSummon == NULL )
			return;
		pSummon->ApplySkill(TRUE, sSkillID, wLEVEL, wTARNUM, sTARIDS);
	}

	if ( sSkillID.IsValidNativeID() )
	{
		GLMSG::SNET_SUMMON_SKILL NetMsg(m_dwGaeaID
										, m_TargetID, m_dwAType
										, m_bUsingEventSkill
										, sSkillID
										, wLEVEL
										, m_vTARPOS);

		for ( WORD i=0; i<wTARNUM; ++i )	
			NetMsg.ADDTARGET ( sTARIDS[i] );

		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
	}

}
BOOL GLSummonField::IsSkillLoopEnd()
{
	int iSize = m_sHITARRAY.size();

	if( iSize - 1 <= 0 )
		return TRUE;

	return FALSE;
}

BOOL GLSummonField::IsSkillEnd( SNATIVEID sSkill )
{
	int iSize = m_sHITARRAY.size();

	if( iSize - 1 <= 0 )
		return TRUE;

	return FALSE;
}

void GLSummonField::SelfSkillStart( SNATIVEID sSkillID, WORD wLevel )
{	
	if( sSkillID == NATIVEID_NULL() )
		return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 1;	

	STARGETID sTargetID;
	sTargetID.emCrow = GetCrow();
	sTargetID.GaeaId = GetGaeaID();
	sTargetID.vPos = GetPosition();


	sTARIDS[0] = STARID( CROW_PC, GetGaeaID());
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );

		if( pSkill == NULL ) return;

		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
			return;		

		SkillProc( FALSE, sSkillID, wLevel, wTARNUM, sTARIDS );
	}
}

void GLSummonField::SkillProc( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !m_sAttackProp.bUsed )
		return;

	SANIATTACK &sAniAttack = m_sAttackProp.sAniAttack;

	//	스킬 정보 가져옴.
	SNATIVEID SkillID  = m_idACTIVESKILL;
	WORD     wSkillLevel = m_wACTIVESKILL_LVL;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SkillID );
	if ( !pSkill )
		return;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wSkillLevel];

	// Note : 날씨 정보 가져옴
	DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );


	int nEXP_NUM = 0;
	int nALL_LVL = 0;
	int nALL_VARHP = 0, nALL_FULLHP = 0;
	int nALL_VARMP = 0, nALL_FULLMP = 0;
	int nALL_VARSP = 0, nALL_FULLSP = 0;

	WORD wMAX_TAR = 0;
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
		wMAX_TAR = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1;
	else if ( pSkill->m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
		wMAX_TAR = sSKILL_DATA.wTARNUM * ( sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1 );
	else
		wMAX_TAR = sSKILL_DATA.wTARNUM;

	if ( m_wTARNUM > wMAX_TAR )
		m_wTARNUM = wMAX_TAR;

	//	Note : 스킬이 적용되는 타겟 검사.
	//
	STARGETID sTARID;
	for ( WORD i=0; i<m_wTARNUM; ++i )
	{
		const STARID &_sTARID = m_sTARIDS[i];

		sTARID.emCrow = _sTARID.GETCROW();
		sTARID.GaeaId   = _sTARID.GETID();
		ServerActor* pTargetActor = m_pGaeaServer->GetTarget ( pLand, sTARID );
		if ( pTargetActor == NULL )
			continue;

		/*======== 반격 적용 효과 결과 뽑아냄 =========================================================================================*/

		CSkillCAInterface cSkillCA;

		STARGETID sCId   = STARGETID(CROW_MOB,m_dwGaeaID);;
		STARGETID sCTaId = sTARID;

		bool bCaDamage = false;

		switch( sTARID.emCrow )
		{
		case CROW_PC :
			{
				GLChar* const pTarget = dynamic_cast<GLChar*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarget = dynamic_cast<GLCrow*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		}
		/*===============================================================================================================================*/

		if ( pTargetActor->GetNowHP() != 0 )
		{
			//	Note : 물리 데미지 발생.
			//
			DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
			int nVAR_HP(0), nVAR_MP(0), nVAR_SP(0);

			//	Note : 저항치. (주인의 속성을 가져온다)
			short nRESIST = pTargetActor->GETRESIST().GetElement( STATE_TO_ELEMENT(m_sOwnerBlow.emTYPE) );
			if ( nRESIST > 99 )
				nRESIST = 99;

			//	SKILL 기본 적용.
			//
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_HP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					//					bCRITICAL = CALCDAMAGE ( nVAR_HP, bShock, sTARID, m_pLandMan, pSkill, wSKILL_LVL, GLPeriod::GetInstance().GetWeather(), sAniAttack.m_wDivCount );
					dwDamageFlag = CALCDAMAGE ( nVAR_HP, m_dwGaeaID, sTARID, pLand, pSkill, wSkillLevel, dwWeather, sAniAttack.m_wDivCount );
					nVAR_HP = - nVAR_HP;
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_HP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_MP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_MP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_MP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_SP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_SP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_SP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;
			};

			//	Note : SP 부족시에는 변화 값을 반으로 조정.
			//
			if ( bLowSP )
			{
				nVAR_HP /= 2;
				nVAR_MP /= 2;
				nVAR_SP /= 2;
			}

			// Note : 블로킹 검사
			//			
			if ( CalcBlocking(pTargetActor, nVAR_HP, pSkill) == true )
			{
				BlockProc(sTARID, SkillID, wSkillLevel);
				continue;
			}
			else
			{
				//	Note : 스킬 특수 기능.
				//
				int nGATHER_HP(0), nGATHER_MP(0), nGATHER_SP(0);

				for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
				{
					const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSkillLevel];

					switch ( pSkill->m_sAPPLY.vecSPEC[k].emSPEC )
					{
					case EMSPECA_PUSHPULL:
						if (!pTargetActor->IsValidBody())
							break;
						if (sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR2)
							break;	//	Note : 발생확율 적용.
						dwDamageFlag |= DAMAGE_TYPE_SHOCK;
						break;
					case EMSPECA_HP_GATHER:
						nGATHER_HP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_MP_GATHER:
						nGATHER_MP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_SP_GATHER:
						nGATHER_SP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;
					};
				}

				if( bCaDamage ) 
				{
					dwDamageFlag |= DAMAGE_TYPE_CA;
				}

				//	Note : 스킬 기본 영향.
				//
				SSKILLACT sSKILLACT;
				//sSKILLACT.sID      = STARGETID(CROW_SUMMON,GetGaeaID());
				//sSKILLACT.sID_TAR  = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
				sSKILLACT.sID      = sCId;
				sSKILLACT.sID_TAR  = sCTaId;
				sSKILLACT.fDELAY   = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
				sSKILLACT.emAPPLY  = pSkill->m_sBASIC.emAPPLY;
				sSKILLACT.dwDamageFlag = dwDamageFlag;

				//	Note : 체력 변화 저장.
				//
				sSKILLACT.nVAR_HP = nVAR_HP;
				sSKILLACT.nVAR_MP = nVAR_MP;
				sSKILLACT.nVAR_SP = nVAR_SP;

				//	Note : 체력 흡수 저장.
				//
				sSKILLACT.nGATHER_HP = nGATHER_HP;
				sSKILLACT.nGATHER_MP = nGATHER_MP;
				sSKILLACT.nGATHER_SP = nGATHER_SP;

				if ( sSKILLACT.VALID() )
				{
					pLand->RegSkillAct ( sSKILLACT );
				}
			}
		}

		//	Note : 스킬 보조 영향.
		//
		SSKILLACTEX sSKILLACTEX;
		SSKILLACTEX sCASKILLACTEX;
		sSKILLACTEX.sID = STARGETID(GetCrow(),GetGaeaID());
		sSKILLACTEX.sID_TAR = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
		sSKILLACTEX.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;

		sSKILLACTEX.idSKILL = SkillID;
		sSKILLACTEX.wSKILL_LVL = wSkillLevel;

		sCASKILLACTEX = sSKILLACTEX;

		//	Note : 상태 이상 유발.
		//         무기의 종속되는 스킬이라면 주인의 무기 속성을 가져온다. 

		ITEM::SSATE_BLOW sBLOW;
		sBLOW.emTYPE = EMBLOW_NONE;

		if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
		{
			sBLOW = m_sOwnerBlow;
		}
		else
		{
			const SKILL::SSTATE_BLOW &sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSkillLevel];

			sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
			sBLOW.fRATE  = sSKILL_BLOW.fRATE;
			sBLOW.fLIFE  = pSkill->m_sAPPLY.sDATA_LVL[wSkillLevel].fLIFE;
			sBLOW.fVAR1  = sSKILL_BLOW.fVAR1;
			sBLOW.fVAR2  = sSKILL_BLOW.fVAR2;
		}

		if ( sBLOW.emTYPE!=EMBLOW_NONE )
		{
			//	Note : 발생 확율 계산.
			//
			short nBLOWRESIST = pTargetActor->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) );
			if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

			//			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
			DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, dwWeather, pLand->IsWeatherActive() );

			BOOL bBLOW(FALSE);
			if ( !(pTargetActor->GETHOLDBLOW()&STATE_TO_DISORDER(sBLOW.emTYPE)) )
			{
				bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBLOW.fRATE * fPOWER, GETLEVEL(), pTargetActor->GetLevel(), nBLOWRESIST );
			}

			if ( bBLOW )
			{
				//	Note : 상태이상 발생.
				//	
				SSTATEBLOW sSTATEBLOW;
				float fLIFE = sBLOW.fLIFE * fPOWER;
				fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				sSTATEBLOW.emBLOW = sBLOW.emTYPE;
				sSTATEBLOW.fAGE = fLIFE;
				sSTATEBLOW.fSTATE_VAR1 = sBLOW.fVAR1;
				sSTATEBLOW.fSTATE_VAR2 = sBLOW.fVAR2;
				if ( pTargetActor->STATEBLOW( sSTATEBLOW ) )
				{
					//	Note : 상태 이상 발생 저장.
					//
					sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
				}
			}
		}

		sCASKILLACTEX.sSTATEBLOW = sSKILLACTEX.sSTATEBLOW;
		//ApplySkillSpecial( pSkill,  sSKILLACTEX,  pTargetActor,  sTARID, wSkillLevel);
		ApplySkillSpecialCA( pSkill,  sSKILLACTEX,  sCASKILLACTEX, pTargetActor,  sTARID, wSkillLevel);

		switch (pSkill->m_sAPPLY.emBASIC_TYPE)
		{
		case SKILL::EMFOR_CURE:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.dwCUREFLAG;
			break;
		};

		if ( sSKILLACTEX.VALID() )
		{
			//pLand->RegSkillActEx ( sSKILLACTEX );
			pLand->RegSkillActExII( sSKILLACTEX, sCASKILLACTEX );
		}
	}

	//	Note : 경험치 획득 산출.
	//
	int nSUM_EXP = 0;
	if ( nEXP_NUM!=0 )
	{
		//	Note : 회복에 기여했을 때만 경험치 발생.	( 공격시는 제외 )
		//
		if ( nALL_VARHP>0 && nALL_VARMP>0 && nALL_VARSP>0 ) // V560 A part of conditional expression is always false: nALL_VARHP > 0.
		{
			nALL_LVL /= nEXP_NUM;

			nALL_VARHP = abs(nALL_VARHP) / nEXP_NUM;
			nALL_VARMP = abs(nALL_VARMP) / nEXP_NUM;
			nALL_VARSP = abs(nALL_VARSP) / nEXP_NUM;

			nALL_FULLHP /= nEXP_NUM;
			nALL_FULLMP /= nEXP_NUM;
			nALL_FULLSP /= nEXP_NUM;

			//	실제 변화량이 있을 때만 경험치를 산출.
			if ( nALL_VARHP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARHP,nALL_FULLHP);
			if ( nALL_VARMP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARMP,nALL_FULLMP);
			if ( nALL_VARSP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARSP,nALL_FULLSP);
		}
		//	Note : 보조 스킬 사용시 경험치.
		//
		else
		{
			nSUM_EXP = (pSkill->m_sBASIC.dwGRADE*(wSkillLevel+1)*100) / GETLEVEL();
			if (nSUM_EXP < 1)
				nSUM_EXP = 1;
			if (nSUM_EXP > 5)
				nSUM_EXP = 5;
		}
	}
}

void GLSummonField::ApplySkill( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS )
{
	SkillProc( bLowSP, skill_id, wSKILL_LVL, wTARNUM, sTARIDS  );
}

BOOL GLSummonField::IsOtherWay( PGLSKILL pSkill, WORD wLv )
{
	GLLandMan* const pLand(GetLandMan());

	const ServerActor *pActor = GetOwner();

	if( pActor == NULL || pSkill == NULL || pLand == NULL ) return FALSE;

	if( pActor->GetCrow() != CROW_PC ) return FALSE;

	SUMMON_TYPE eType = (m_emTYPE == SUMMON_TYPE_WAY_1) ? SUMMON_TYPE_WAY_2 : SUMMON_TYPE_WAY_1;

	DWORD dwCount = 0;//pActor->GetChildSummonTypeCount( eType );

	for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
	{
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wLv];

		switch ( pSkill->m_sAPPLY.vecSPEC[k].emSPEC )
		{
		case EMSPECA_ENTRANCE_EXIT:
			{
				SNATIVEID sWayID = SNATIVEID( pSkill->m_sAPPLY.vecSPEC[k].sSPEC->fVAR1, pSkill->m_sAPPLY.vecSPEC[k].sSPEC->fVAR2 );

				CROWIDLIST_CITER _iter = pActor->m_listCrowID.begin();

				DWORD dwID = pActor->GetGaeaID();

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				while( _iter != pActor->m_listCrowID.end() )
				{
					Faction::GLActorID sDestID = *_iter;

					switch( sDestID.actorType )
					{
					case CROW_SUMMON :
						{
							PGLSUMMONFIELD pSummonDst = pLand->GetSummon(sDestID.actorID_Num);

							if( pSummonDst && pSummonDst->m_sSummonID == sWayID )
							{
								if( pSummonDst->GetOwner() && pSummonDst->GetOwner()->GetGaeaID() == dwID )
								{
									const SCROWATTACK* sAttackProp = pSummonDst->GetCrowAttack();

									PGLSKILL pDstSkill = sAttackProp ? GLSkillMan::GetInstance().GetData ( sAttackProp->skill_id ) : NULL;

									if( pDstSkill ) {
										DWORD dwSrcRange = (DWORD)(GETBODYRADIUS() + pSkill->m_sAPPLY.sDATA_LVL[wLv].wAPPLYRANGE + 2);
										DWORD dwDstRange = (DWORD)(pSummonDst->GETBODYRADIUS() + pDstSkill->m_sAPPLY.sDATA_LVL[wLv].wAPPLYRANGE + 2);

										DWORD dwTotalRange = dwSrcRange + dwDstRange;

										float fDist = D3DXVec3Length( &D3DXVECTOR3(pSummonDst->GetPosition() - GetPosition()) );

										if( fDist > dwTotalRange )  return TRUE;

										return FALSE;
									}									
								}					
							}
						}			
						break;
					}

					++_iter;
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			}
			break;
		};
	}

	return (dwCount > 0 ) ? TRUE : FALSE;
}

void GLSummonField::UpdateSkillEff( float fTime, float fElapsedTime )
{
	/// 도발 ////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) {
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_TargetID ) m_TargetID = sID;
	}
	/// 피해발동 ////////////////////////////////////////////////
	if( RF_ACTIVE( m_EffSkillVarSet ).IsOn() )
	{
		WORD  wLv		= 0;
		DWORD dwSkillId = 0;
		STARGETID	sID;

		if( RF_ACTIVE( m_EffSkillVarSet ).Invoke( wLv, dwSkillId, sID ) )
		{
			InvokeActionSkillStart( sID, SNATIVEID(dwSkillId), wLv );
		}
	}
}

BOOL GLSummonField::IsViewInActor( const STARGETID& sID )
{
	GLLandMan* const pLand(GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	if( sID.ISNULL() ) return FALSE;

	for( DWORD i = 0; sID.emCrow == CROW_PC && i < m_dwPC_AMOUNT && i < m_vecPC_VIEW.size(); ++i )
	{
		DWORD		dwGAEAID = m_vecPC_VIEW[i];
		GLChar*		pChar	 = m_pGaeaServer->GetChar( dwGAEAID );
		if ( !pChar )
			continue;

		if( pChar->IsSafeTime()	|| pChar->IsSafeZone()	|| !pChar->IsValidBody() || !pChar->IsVisibleDetect( this ) )	{
			continue;
		}

		if( sID.GaeaId == dwGAEAID ) return TRUE;
	} 

	for( DWORD i = 0; sID.emCrow == CROW_MOB &&  i < m_vecCROW_VIEW.size(); i++ )
	{
		DWORD		dwGAEAID = m_vecCROW_VIEW[i];
		GLCrow*		pMob = pLand->GetCrow( m_vecCROW_VIEW[i] );
		if( !pMob )
			continue;

		if( !pMob->IsValidBody() )
			continue;

		if( sID.GaeaId == dwGAEAID ) return TRUE;
	}


	// Note : 캐릭터를 발견 못한경우 소환수를 찾는다.
	for( DWORD i = 0; i < sID.emCrow == CROW_SUMMON && m_dwSUMMON_AMOUNT && i < m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField* pSummon = pLand->GetSummon(dwGAEAID);

		if (!pSummon)
			continue;

		if (!pSummon->IsValidBody())
			continue;

		if ( pSummon->IsNonTargetable() )
			continue;

		if ( !pSummon->IsVisibleDetect( this ) )
			continue;

		if( sID.GaeaId == dwGAEAID ) return TRUE;

	}

	return FALSE;
}

void GLSummonField::InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pTarget = pLand->GetActor(sTargetID);

	if( pTarget == NULL || !sSkillID.IsValidNativeID() ) return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 0;

	pLand->m_SkillTargetSystem.SelectSkillTarget( this, pTarget, sSkillID, wLevel, pTarget->GetPositionActor(), sTARIDS, wTARNUM);

	STARGETID sTarID = STARGETID( sTargetID.emCrow, sTargetID.GaeaId, pTarget->GetPosition() );

	ApplySkill( TRUE, sSkillID, wLevel, wTARNUM, sTARIDS );

	switch( sTarID.emCrow )
	{
	case CROW_PC :
		{
			GLChar* pChar = dynamic_cast<GLChar*>( pTarget );

			if( pChar ) {
				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION NetMsgDealy;
				NetMsgDealy.emCrow		= sTarID.emCrow;
				NetMsgDealy.dwID		= sTarID.GaeaId;
				NetMsgDealy.skill_id	= sSkillID;
				NetMsgDealy.wLEVEL		= wLevel;
				NetMsgDealy.vTARPOS		= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgDealy.ADDTARGET ( sTARIDS[i] );

				pChar->SendToClient(&NetMsgDealy );

				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD NetMsgBRD;
				NetMsgBRD.emCrow	= sTarID.emCrow;
				NetMsgBRD.dwID		= sTarID.GaeaId;
				NetMsgBRD.skill_id	= sSkillID;
				NetMsgBRD.wLEVEL	= wLevel;
				NetMsgBRD.vTARPOS	= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgBRD.ADDTARGET ( sTARIDS[i] );

				pChar->SendMsgViewAround(&NetMsgBRD);
			}
		}
		break;
	case CROW_MOB :
		{
			GLCrow* pMob = dynamic_cast<GLCrow*>( pTarget );

			if( pMob ) {

				GLMSG::SNETCROW_SKILL_DEALY_ACTION NetMsg(sTarID.GaeaId);

				NetMsg.dwAType   = pMob->m_dwAType;
				NetMsg.skill_id  = sSkillID;
				NetMsg.skill_lev = wLevel;
				NetMsg.vTARPOS   = pMob->GetTarPos();
				NetMsg.vPos      = sTarID.vPos;

				for (WORD i=0; i<wTARNUM; ++i)
					NetMsg.ADDTARGET(sTARIDS[i]);
				pMob->SendMsgViewAround(&NetMsg);
			}
		}
		break;
	case CROW_SUMMON :
		{
			GLSummonField* pSummon = dynamic_cast<GLSummonField*>( pTarget );

			if( pSummon ) {

				GLMSG::SNET_SUMMON_SKILL NetMsg( sTarID.GaeaId
					, pSummon->GetTargetID(), pSummon->m_dwAType
					, pSummon->m_bUsingEventSkill
					, sSkillID
					, wLevel
					, pSummon->GetTarPos() );

				for ( WORD i=0; i<wTARNUM; ++i )	
					NetMsg.ADDTARGET ( sTARIDS[i] );

				pSummon->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		break;
	}
}