#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../FieldServer/GLGaeaServer.h"

#include "./GLSummonField.h"
#include "../../MfcExLib/RanFilter.h"

#include "../../SigmaCore/DebugInclude.h"



void GLSummonField::MsgProcess(NET_MSG_GENERIC* nmg)
{
	sc::writeLogWarn(sc::string::format("[WARNING]GLSummonField::MsgProcess Illigal Message(%d)", nmg->nType));

	//	Memo : 아래와 같이 디자인 된 코드를 우선 위와 같이 고쳤다.
	/*switch ( nmg->nType )
	{

	default:
		m_pGLGaeaServer->GetConsoleMsg()->Write (LOG_CONSOLE,
			"[WARNING]GLSummonField::MsgProcess Illigal Message(%d)",
			nmg->nType);
		break;
	};*/
}

void GLSummonField::ReqNetMsg_Drop(GLMSG::SNET_SUMMON_DROP_SUMMON& Msg)
{
	//GLMSG::SNET_SUMMON_DROP_SUMMON NetMsg;
	//NetMsg = GLMSG::SNET_SUMMON_DROP_SUMMON();

	Msg.dwGUID = m_dwGaeaID;

	SDROPSUMMON &Data = Msg.Data;

	Data.m_emTYPE         = m_emTYPE;
	Data.m_dwGUID         = m_dwGaeaID;
	Data.m_sSummonID      = m_sSummonID;
	Data.m_sOwnerID		= m_sOwnerID;
	Data.m_dwOwner        = m_dwOwnerGaeaID;
	Data.m_sMapID         = GetCurrentMap().getBaseMapID();	
	Data.m_dwCellID       = m_dwCeID;
	Data.m_fRemainedTime  = m_fRemainedTime;
	Data.m_vPos           = m_vPosition;	
	Data.m_vDir           = m_vDirect;		 
	Data.m_fSpeed		  = m_fCurSpeed;
	Data.m_dwActionFlag   = GetActState();
	Data.m_emAction       = GetAction();

	Data.m_wAniSub        = m_wAniSub;	

	Data.m_dwNowHP        = m_dwNowHP;
	Data.m_wNowMP         = m_wNowMP;

	Data.m_sTarget        = m_TargetID;

	Data.m_fOwnerIncHP		   = m_fOwnerIncHP;
	Data.m_wOwnerAttack        = m_wOwnerAttack;
	Data.m_wOwnerDefense       = m_wOwnerDefense;
	Data.m_wOwnerSkillLevel    = m_wOwnerSkillLevel;
	Data.m_cOwnerWeaponsGrade  = m_cOwnerWeaponsGrade;
	Data.m_sOwnerBlow          = m_sOwnerBlow;

	if ( Data.m_emAction == GLAT_MOVE )
	{
		if ( m_actorMove.PathIsActive() )
		{
			Data.m_sTarget.vPos = m_actorMove.GetTargetPosition();

			// 서버 클라이언트간 몹의 위치 동기화를 위해
			Data.vStartPos		= m_vStartPos;
			Data.sNextWaypoint  = m_actorMove.GetNextWayPoint ();
		}
		else
		{
			Data.m_emAction = GLAT_IDLE;
		}
	}

	// Note : 소환게이트는 없다고 한다 UINT_MAX
	Data.m_EventNodeSummonGateID = (UINT_MAX);

	// Note : 소환수의 제스쳐와 시간을 입력한다.
	Data.m_emGestureState = m_emGestureState;
	Data.m_fIdleTime      = m_fIdleTime;

	// Note : 스킬 FACT를 입력한다.
	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
        Data.m_sSKILLFACT[i].Assign ( m_sSKILLFACT[i], i );
	for ( int i=0; i<EMBLOW_MULTI; ++i )
        Data.m_sSTATEBLOWS[i] = m_sSTATEBLOWS[i];

	//return NetMsg;
}

