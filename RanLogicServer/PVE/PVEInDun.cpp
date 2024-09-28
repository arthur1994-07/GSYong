#include "pch.h"

#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "PVEInDun.h"
#include "PVESystem.h"
#include "../AgentServer/GLAgentServer.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../SigmaCore/String/StringUtil.h"

using namespace PVESystem;

CPVEInDun::CPVEInDun()
:	m_state(INDUNSTATE_NONE)
,	m_dwChannelNum(0)
{
    Init(true);
}

CPVEInDun::~CPVEInDun()
{
	Init(true);
}

void CPVEInDun::Init(bool bSucceed)
{
    SetState(INDUNSTATE_NONE);

	m_dwChannelNum = 0;
	m_preid = 0;
	m_indunid = 0;
	m_gateid = 0;
    m_stage = 1;
	m_server = 0;
	m_RealIndunid = 0;;

	/**
		정상적으로 게임이 끝났을 경우에만 시간을 초기화 해 준다.
		만약 정상적이지 않은데로 초기화 하면 인던을 소유한 소유자가 
		무한히 인던을 소유해서 문제가 된다.

	 */
	if (bSucceed)
	{
		m_lifetime = 0.f;
	}

	/**
		정상적으로 게임이 종료된 경우에만
		모든 파티원들의 인던 귀속을 모두 해제 해 준다.
		정상적이지 않을 경우 마스터는 인던에 계속 귀속 된다.

	 */
	m_party.UnRegister(bSucceed);

    m_party.Reset();
}

bool CPVEInDun::Entrance(GLPartyAgent* pPartyAgent)
{
    if (m_server == 0 || pPartyAgent == 0)
    {
        return false;
    }

	// 초기화된 상태에서만 입장할 수 있다.
	if (IsIdle() == false)
	{
		return false;
	}

	// 인던 파티 초기화
	if (m_party.Init(pPartyAgent, m_server, m_id) == false)
	{
		return false;
	}

	// 진입 가능 체크
    if (IsEntrance() == false)
    {
		EntranceAck(GLMSG::ENTRANCE_ACK_NOT_ENTRANCE, m_id.dwCharDbNum);
		m_party.Reset();
        return false;
    }

	// 모든 파티원들에게 입장 확인 요청
	EntranceAck(GLMSG::ENTRANCE_ACK_NONE);

	// 입장 확인 상태로 변경
    SetState(INDUNSTATE_CONFIRM);

    return true;
}

bool CPVEInDun::Entrance(DWORD dwCharDbNum)
{
	if (m_server == 0)
	{
		return false;
	}

	// 생성 된 상태에서만 입장 할 수 있다.
	if (IsRunning() == false)
	{
		return false;
	}

	// 진입 가능 체크
	if (IsEntrance(dwCharDbNum) == false)
	{
		EntranceAck(GLMSG::ENTRANCE_ACK_NOT_ENTRANCE, dwCharDbNum);
		return false;
	}

	m_party.SavePosition(dwCharDbNum);
	m_party.Move(this, m_indunid, m_gateid, dwCharDbNum);

	return true;
}

bool CPVEInDun::IsEntrance()
{
	// 파티장인지 체크
    if (m_party.IsMaster(m_id.dwCharDbNum) == false)
    {
        return false;
    }

    // 허용 인원 체크
    if (m_mapinfo.person > static_cast<int>(m_party.GetCount()))
    {
        return false;
    }

	// 인던 레벨 존재 체크
	if (m_mapinfo.levelinfo.find(m_id.nLevel) == m_mapinfo.levelinfo.end())
	{
		return false;
	}

	WORD wMinLevel = WORD(m_mapinfo.levelinfo[m_id.nLevel].minlevel);
	WORD wMaxLevel = WORD(m_mapinfo.levelinfo[m_id.nLevel].maxlevel);

	// 허용 유저 레벨 체크
    if (m_party.IsLevel(wMinLevel, wMaxLevel) == false)
    {
        return false;
    }

	// 모든 멤버가 소속 인던이 없어야 한다.
	if (m_party.IsIndun())
	{
		return false;
	}

	// 모든 멤버는 CTF 에 관여되지 않아야 한다.
	if (m_party.IsCTF())
	{
		return false;
	}

    // 모든 멤버는 매칭시스템 컨텐츠와 관여되지 않아야 한다.;
    if ( GLUseFeatures::GetInstance().IsUsingTournament() )
    {
        if (m_party.IsJoinMatchingContents())
        {
            return false;
        }
    }

    // 모든 멤버의 허용 일일 횟수 체크
    if (m_party.IsCount(m_mapinfo.dailylimit, m_mapinfo.owntime) == false)
    {
        return false;
    }

	// 멤버 변경을 감지
	if (m_party.ChkMember() == false)
	{
		return false;
	}

    return true;
}

bool CPVEInDun::IsEntrance(DWORD dwCharDbNum)
{
	// 파티가 존재해야 한다.
	if (m_server == 0 || m_party.GetPartyID().isValidParty() == false)
	{
		return false;
	}

	// 입장 가능한 멤버 인지 체크
	if (m_party.IsPassMember(dwCharDbNum) == false)
	{
		return false;
	}

	GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

	if (pChar == 0)
	{
		return false;
	}

	const GLPartyID& partyID = pChar->GetPartyID();

	// 같은 파티가 아니면 입장 할 수 없다.
	if (partyID.isValidParty() == false || partyID != m_party.GetPartyID())
	{
		return false;
	}

	// CTF 에 관여되지 않아야 한다.
	if (m_party.IsCTF(dwCharDbNum))
	{
		return false;
	}

    // 매칭시스템 컨텐츠와 관여되지 않아야 한다.;
    if ( GLUseFeatures::GetInstance().IsUsingTournament() )
    {
        if ( m_party.IsJoinMatchingContents(dwCharDbNum) )
        {
            return false;
        }
    }

	// 이미 같은 맵에 있다면 입장할 수 없다.(혹시라도 모를 예외 처리)
	if ( m_indunid == pChar->GetCurrentMap().getGaeaMapID().Id() )
	{
		return false;
	}

	return true;
}

void CPVEInDun::DestroyParty(GLPartyAgent* pPartyAgent)
{
	if (IsCreating())
	{
		// 임시 할당 타이머 ID
		static int DESTROY_PARTY_TIMER_ID = 65535;

		/*
			인던 생성 중
			파티 해산이 되면 생성 후 처리 한다.

		 */
		m_timers.InsertCallback(DESTROY_PARTY_TIMER_ID, 
			1.f, boost::bind(std::mem_fun(&CPVEInDun::DestroyParty), this, pPartyAgent));
		return;
	}

	m_party.RestoreAll(GLMSG::STAGE_STATE_GAMEROVER_FIREPARTY);	
}

void CPVEInDun::SetPartyCallback()
{
	if (m_server)
	{
		GLPartyAgent* pPartyAgent = const_cast<GLPartyAgent*>( m_server->GetParty(m_party.GetPartyID()) );

		if (pPartyAgent)
		{
			 pPartyAgent->InsertDestroyCallback(boost::bind(&CPVEInDun::DestroyParty, this, _1));	
		}
	}
}

void CPVEInDun::RemovePartyCallback()
{
    if (m_server)
    {
        GLPartyAgent* pPartyAgent = const_cast<GLPartyAgent*>( m_server->GetParty(m_party.GetPartyID()) );

        if (pPartyAgent)
        {
			pPartyAgent->RemoveDestroyCallback(boost::bind(&CPVEInDun::DestroyParty, this, _1));			
        }
    }
}

void CPVEInDun::PassMember(DWORD member)
{
    if (IsConfirming() == false)
    {
        return;
    }

    EntranceConfirmAck(GLMSG::ENTRANCE_ACK_CONFIRM_OK, member);

    if (m_party.PassMember(member))
    {
		/**
			입장 확인 사이에 상태가 변경 될 수 있으므로
			최종 입장 전 다시 확인 한다.

		 */
		if (IsEntrance() == false)
		{
			EntranceAck(GLMSG::ENTRANCE_ACK_NOT_ENTRANCE, m_id.dwCharDbNum);			
			Init();
			return;
		}

		// 인던 관리자에 등록
		m_party.Register();

		// 인던을 생성 한다.
        if (CreateInstanceMap(m_id.dwMapID, m_mapinfo.gateid, m_stage) == false)
		{
			m_indunid = 0;
			FailedCreate();
		}
    }
}

void PVESystem::CPVEInDun::Impossible( DWORD member )
{
    if (IsConfirming() == false)
    {
        return;
    }

    EntranceConfirmAck(GLMSG::ENTRANCE_ACK_CONFIRM_IMPOSSIBLE, member);
    Init();
}

void CPVEInDun::Cancel(DWORD member)
{
    if (IsConfirming() == false)
    {
        return;
    }

	EntranceConfirmAck(GLMSG::ENTRANCE_ACK_CONFIRM_CANCEL, member);
    Init();
}

void CPVEInDun::Timerover()
{
    if (IsConfirming() == false)
    {
        return;
    }

	EntranceConfirmAck(GLMSG::ENTRANCE_ACK_CONFIRM_TIMEOVER);
    Init();
}

void CPVEInDun::FailedCreate()
{
	if (m_server == 0)
	{
		return;
	}

	if (IsCreating() == false)
	{
		return;
	}

	if (m_indunid != 0)
	{
		DeleteInstanceMap(m_indunid);
	}

	GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsg;
	NetMsg.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE;
	m_party.SendParty(&NetMsg);

	m_party.RestoreAll(GLMSG::STAGE_STATE_GAMEROVER_FAILED);
	Init();
}

bool CPVEInDun::ClearOfBelonging(DWORD member)
{
	/**
		인던이 초기화 상태라면
		파티장만 인던 귀속 해제 시킬 수 있다.
		인던이 초기화 상태는 실제 인던은 이미 소멸된 상태 이다.

	 */
	if (IsIdle())
	{
		if (m_id.dwCharDbNum != member)
		{
			sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Requester is not a Party Master. ]" );
			return false;
		}

		if (ClearGenTime() == false)
		{
			sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Failed Clear GenTime. ]" );
			return false;
		}

		Init(true);
		return true;
	}

	/**
		인던이 진행 중이라야
		파티장과 파티원 모두 인던 귀속을 해제 시킬 수 있다.
		물론 인던 밖에서 가능 하다.

	 */
	if (IsRunning() == false)
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Don't run. ]" );

		return false;
	}

	if (m_server == 0)
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Server is NULL. ]" );

		return false;
	}

	GLCharAG* pChar = m_server->GetCharByDbNum(member);

	if (pChar == 0)
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Requester is NULL. ]" );

		return false;
	}

	// 진행중인 맵에서는 해제를 할 수 없다.
	if ( m_indunid == pChar->GetCurrentMap().getGaeaMapID().Id() )
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Current Map is The PVE Map. ]" );

		return false;
	}

	// 멤버 이여야만 해제 할 수 있다.
	if (m_party.IsPassMember(member) == false)
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Requester is not a Party Member. ]" );

		return false;
	}

	m_party.RemoveMember(member);

	return true;
}

void CPVEInDun::Terminate()
{
	if (IsCreating())
	{
		/**
			현재 인던이 생성 중일 경우
			생성 완료될 때까지 1초당 완료를 확인 하면서 대기 한다.

		 */
		m_lifetime = 1.f;
		return;
	}

    m_party.RestoreAll(GLMSG::STAGE_STATE_GAMEROVER_TIMEOVER);
    Init(true);
}

bool CPVEInDun::CreateInstanceMap(DWORD dwBaseMapID, DWORD dwGateID, int nStage)
{
	/**
		최악의 경우 생성 대기 시간에 의해
		인던이 초기화 될 수 있도록
		먼저 생성중 상태로 변경 시켜 준다.

	 */
	SetState(INDUNSTATE_CREATING);

    if (m_server == 0)
    {
        return false;
    }

    GLCharAG* pChar = m_server->GetCharByDbNum(m_id.dwCharDbNum);
	if ( pChar )
	{
		m_dwChannelNum = pChar->GetChannel();
	}

	/**
		에이전트 인던 생성

	 */
	DWORD dwFieldSvr = m_server->GetFieldServer(SNATIVEID(dwBaseMapID));
    const DWORD dwGaeaMapID = m_server->CreatePVEInstantMap(
		dwBaseMapID,
		/*pChar->GaeaId()*/0,
		m_party.GetPartyID(),
		/*pChar->GetChannel()*/m_dwChannelNum,
		dwFieldSvr );
    if ( dwGaeaMapID == GAEAID_NULL )
    {
        return false;
    }
	m_RealIndunid = dwBaseMapID;

	m_preid = m_indunid;

    //GUID 세팅
    // 이전맵이 없을 때만 초기화 한다.
    std::string strGuid;
    if( m_preid == 0)
    {
        sc::MGUID Guid;
        sc::SeqUniqueGUID::Instance()->getGUID( Guid );
        strGuid = sc::string::uuidToString(Guid);
        sc::string::upperString(strGuid);
        m_Guid = Guid;
    }
    else
    {
        strGuid = sc::string::uuidToString(m_Guid);
        sc::string::upperString(strGuid);
    }

    GLMSG::NET_PVE_CREATE_INSTANT_MAP_REQ_AF NetMsg;
	NetMsg.sPartyID = m_party.GetPartyID();
    NetMsg.sInDunID = m_id;
    NetMsg.dwBaseMapID = dwBaseMapID;
    NetMsg.dwInstanceMapID = m_indunid = dwGaeaMapID;
    NetMsg.nStage = nStage;
    NetMsg.strGuid = strGuid;
    m_gateid = dwGateID;
    m_stage = nStage;
    msgpack::sbuffer buf;
    msgpack::pack(buf, NetMsg);    

    /**
		필드 인던 생성 요청

	 */
    m_server->SENDTOFIELD(
		/*pChar->GetChannel()*/m_dwChannelNum,
		dwFieldSvr,
		NET_MSG_PVE_CREATE_INSTANT_MAP_REQ_AF,
		buf);



	return true;
}

void CPVEInDun::DeleteInstanceMap(DWORD dwGaeaMapID)
{
	if (m_server)
	{
		m_server->DeleteInstantMap(dwGaeaMapID);
	}
}

bool CPVEInDun::PostCreateInstanceMap(DWORD dwGaeaMapID, bool bResult)
{
	if (IsCreating() == false)
	{
		/**
			 정상적이지 않을 떄 
			 인던 생성 후 처리가 왔다면 
			 해당 인던을 삭제 해 준다.

		 */
		DeleteInstanceMap(dwGaeaMapID);

		return false;
	}

	/**
		만약 생성 요청한 인던과 다른 인던이 온다면
		이전에 실패된 요청 일 수도 있다.
		이럴 경우 성공이라면 저절로 삭제 되지만
		실패라면 우리가 삭제 해 줘야 한다.

	 */
	if (dwGaeaMapID != m_indunid)
	{
		if (bResult == false)
		{
			DeleteInstanceMap(dwGaeaMapID);
		}

		return false;
	}

    if (bResult)
    {
		ProcessDB();
        MoveParty(m_indunid, m_gateid, m_stage);

        return true;
    }
    else
    {
		FailedCreate();

        return false;
    }
}

void CPVEInDun::MoveParty(DWORD dwMapID, DWORD dwGateID, int nStage)
{
    if (nStage == 1)
    {
        m_party.SavePositionAll();
    }

    m_party.MoveParty(this, dwMapID, dwGateID);
    m_stage = nStage;
}

void CPVEInDun::PostEntranceInstanceMap(DWORD dwCharDbNum, bool bResult)
{
	/**
		인던에 한명이라도 접속이 되면 인던 완료 상태가 된다.
		한명이라도 접속 하지 못하면 생성 대기 시간에 의해 초기화 된다.
		그래서 접속 실패면 무시 한다.

	 */
    if (bResult && IsCreating())
    {
		SetState(INDUNSTATE_CREATED);
	}

	/**
		입던에 성공 했으므로
		스테이지를 시작 하도록 알려 준다.

	 */
	if (bResult && IsRunning())
	{
		GLMSG::NET_PVE_START_STAGE_FC StartMsg;
		StartMsg.dwCharDbNum = dwCharDbNum;
		StartMsg.nStage = m_stage;
		StartMsg.sInDunID = m_id;
		m_party.SendPartyMember(dwCharDbNum, StartMsg, NET_MSG_PVE_START_STAGE_FC);
	}
}

void CPVEInDun::Retry()
{
    if (IsRunning() == false)
    {
        return;
    }

    /**
        재도전 성공시 다시 인던 생성 후 입던 시킨다.

     */
    if (CreateInstanceMap(m_RealIndunid, m_gateid, m_stage) == false)
	{
		m_indunid = 0;
		FailedCreate();
	}
}

void CPVEInDun::FrameMove(float fElpaseTime)
{
    m_timers.FrameMove(fElpaseTime);

	if (m_lifetime > 0.f)
	{
		m_lifetime -= fElpaseTime;

		if (m_lifetime <= 0.f)
		{
			m_lifetime = 0.f;
			Terminate();
		}
	}
}

void CPVEInDun::SetState(const EIndunState state)
{
    if (state != m_state)
    {
        ChangeState(m_state, state);
    }

    m_state = state;
}

void CPVEInDun::ChangeState(const EIndunState from, const EIndunState to)
{
    if (to == INDUNSTATE_CONFIRM)
    {
        // 대기 실패를 위한 타이머 발동
        m_timers.InsertCallback(to, 10.f, boost::bind(std::mem_fun(&CPVEInDun::Timerover), this));
    }

    if (to == INDUNSTATE_CREATING)
    {
        // 생성 실패를 위한 타이머 발동
        m_timers.InsertCallback(to, 30.f, boost::bind(std::mem_fun(&CPVEInDun::FailedCreate), this));

		// 파티 콜백 등록
		SetPartyCallback();
    }

	if (to == INDUNSTATE_NONE)
	{
		// 파티 콜백 제거
		RemovePartyCallback();
	}

    m_timers.RemoveCallback(from);
}

void CPVEInDun::SetStageResult(const StageResult& results)
{
    m_party.SetStageResult(results);
}

void CPVEInDun::RestoreAll(int nWhy)
{
	if (IsRunning() == false)
	{
		return;
	}

    m_party.RestoreAll(nWhy);
    Init(true);
	ClearGenTime();
}

void CPVEInDun::ProcessDB()
{
	m_party.ProcessDB(m_mapinfo.owntime, m_lifetime);
}

bool CPVEInDun::ClearGenTime()
{
	MAP_TABLE tbl;

	if (m_party.GetDB(m_id.dwCharDbNum, tbl))
	{
		float fLifeTime = float(PVESystem::DiffTime(tbl.tGenTime, m_mapinfo.owntime));

		if (fLifeTime > 0.f)
		{
			tbl.tGenTime = 0;
			m_party.SetDB(tbl);
			return true;
		}
	}

	return false;
}

void CPVEInDun::EntranceAck(GLMSG::ENTRANCE_ACK eAck, DWORD dwCharDbNum)
{
	GLMSG::NET_PVE_ENTRANCE_ACK_AC NetMsg;
	NetMsg.sInDunID = m_id;
	NetMsg.byResult = eAck;

	if (dwCharDbNum == 0)
	{
		m_party.SendParty(NetMsg, NET_MSG_PVE_ENTRANCE_ACK_AC);
	}
	else
	{
		m_party.SendPartyMember(dwCharDbNum, NetMsg, NET_MSG_PVE_ENTRANCE_ACK_AC);
	}
}

void CPVEInDun::EntranceConfirmAck(GLMSG::ENTRANCE_CONFIRM_ACK eAck, DWORD dwChaNum)
{
	GLMSG::NET_PVE_ENTRANCE_CONFIRM_ACK_AC NetMsg;
	NetMsg.byResult = eAck;
    NetMsg.dwChaDbNum = dwChaNum;
	m_party.SendParty(NetMsg, NET_MSG_PVE_ENTRANCE_CONFIRM_ACK_AC);
}
