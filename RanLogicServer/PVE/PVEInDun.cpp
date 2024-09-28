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
		���������� ������ ������ ��쿡�� �ð��� �ʱ�ȭ �� �ش�.
		���� ���������� �������� �ʱ�ȭ �ϸ� �δ��� ������ �����ڰ� 
		������ �δ��� �����ؼ� ������ �ȴ�.

	 */
	if (bSucceed)
	{
		m_lifetime = 0.f;
	}

	/**
		���������� ������ ����� ��쿡��
		��� ��Ƽ������ �δ� �ͼ��� ��� ���� �� �ش�.
		���������� ���� ��� �����ʹ� �δ��� ��� �ͼ� �ȴ�.

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

	// �ʱ�ȭ�� ���¿����� ������ �� �ִ�.
	if (IsIdle() == false)
	{
		return false;
	}

	// �δ� ��Ƽ �ʱ�ȭ
	if (m_party.Init(pPartyAgent, m_server, m_id) == false)
	{
		return false;
	}

	// ���� ���� üũ
    if (IsEntrance() == false)
    {
		EntranceAck(GLMSG::ENTRANCE_ACK_NOT_ENTRANCE, m_id.dwCharDbNum);
		m_party.Reset();
        return false;
    }

	// ��� ��Ƽ���鿡�� ���� Ȯ�� ��û
	EntranceAck(GLMSG::ENTRANCE_ACK_NONE);

	// ���� Ȯ�� ���·� ����
    SetState(INDUNSTATE_CONFIRM);

    return true;
}

bool CPVEInDun::Entrance(DWORD dwCharDbNum)
{
	if (m_server == 0)
	{
		return false;
	}

	// ���� �� ���¿����� ���� �� �� �ִ�.
	if (IsRunning() == false)
	{
		return false;
	}

	// ���� ���� üũ
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
	// ��Ƽ������ üũ
    if (m_party.IsMaster(m_id.dwCharDbNum) == false)
    {
        return false;
    }

    // ��� �ο� üũ
    if (m_mapinfo.person > static_cast<int>(m_party.GetCount()))
    {
        return false;
    }

	// �δ� ���� ���� üũ
	if (m_mapinfo.levelinfo.find(m_id.nLevel) == m_mapinfo.levelinfo.end())
	{
		return false;
	}

	WORD wMinLevel = WORD(m_mapinfo.levelinfo[m_id.nLevel].minlevel);
	WORD wMaxLevel = WORD(m_mapinfo.levelinfo[m_id.nLevel].maxlevel);

	// ��� ���� ���� üũ
    if (m_party.IsLevel(wMinLevel, wMaxLevel) == false)
    {
        return false;
    }

	// ��� ����� �Ҽ� �δ��� ����� �Ѵ�.
	if (m_party.IsIndun())
	{
		return false;
	}

	// ��� ����� CTF �� �������� �ʾƾ� �Ѵ�.
	if (m_party.IsCTF())
	{
		return false;
	}

    // ��� ����� ��Ī�ý��� �������� �������� �ʾƾ� �Ѵ�.;
    if ( GLUseFeatures::GetInstance().IsUsingTournament() )
    {
        if (m_party.IsJoinMatchingContents())
        {
            return false;
        }
    }

    // ��� ����� ��� ���� Ƚ�� üũ
    if (m_party.IsCount(m_mapinfo.dailylimit, m_mapinfo.owntime) == false)
    {
        return false;
    }

	// ��� ������ ����
	if (m_party.ChkMember() == false)
	{
		return false;
	}

    return true;
}

bool CPVEInDun::IsEntrance(DWORD dwCharDbNum)
{
	// ��Ƽ�� �����ؾ� �Ѵ�.
	if (m_server == 0 || m_party.GetPartyID().isValidParty() == false)
	{
		return false;
	}

	// ���� ������ ��� ���� üũ
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

	// ���� ��Ƽ�� �ƴϸ� ���� �� �� ����.
	if (partyID.isValidParty() == false || partyID != m_party.GetPartyID())
	{
		return false;
	}

	// CTF �� �������� �ʾƾ� �Ѵ�.
	if (m_party.IsCTF(dwCharDbNum))
	{
		return false;
	}

    // ��Ī�ý��� �������� �������� �ʾƾ� �Ѵ�.;
    if ( GLUseFeatures::GetInstance().IsUsingTournament() )
    {
        if ( m_party.IsJoinMatchingContents(dwCharDbNum) )
        {
            return false;
        }
    }

	// �̹� ���� �ʿ� �ִٸ� ������ �� ����.(Ȥ�ö� �� ���� ó��)
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
		// �ӽ� �Ҵ� Ÿ�̸� ID
		static int DESTROY_PARTY_TIMER_ID = 65535;

		/*
			�δ� ���� ��
			��Ƽ �ػ��� �Ǹ� ���� �� ó�� �Ѵ�.

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
			���� Ȯ�� ���̿� ���°� ���� �� �� �����Ƿ�
			���� ���� �� �ٽ� Ȯ�� �Ѵ�.

		 */
		if (IsEntrance() == false)
		{
			EntranceAck(GLMSG::ENTRANCE_ACK_NOT_ENTRANCE, m_id.dwCharDbNum);			
			Init();
			return;
		}

		// �δ� �����ڿ� ���
		m_party.Register();

		// �δ��� ���� �Ѵ�.
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
		�δ��� �ʱ�ȭ ���¶��
		��Ƽ�常 �δ� �ͼ� ���� ��ų �� �ִ�.
		�δ��� �ʱ�ȭ ���´� ���� �δ��� �̹� �Ҹ�� ���� �̴�.

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
		�δ��� ���� ���̶��
		��Ƽ��� ��Ƽ�� ��� �δ� �ͼ��� ���� ��ų �� �ִ�.
		���� �δ� �ۿ��� ���� �ϴ�.

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

	// �������� �ʿ����� ������ �� �� ����.
	if ( m_indunid == pChar->GetCurrentMap().getGaeaMapID().Id() )
	{
		sc::writeLogError( "[ PVE Log ] [ Failed ClearOfBelonging, Current Map is The PVE Map. ]" );

		return false;
	}

	// ��� �̿��߸� ���� �� �� �ִ�.
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
			���� �δ��� ���� ���� ���
			���� �Ϸ�� ������ 1�ʴ� �ϷḦ Ȯ�� �ϸ鼭 ��� �Ѵ�.

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
		�־��� ��� ���� ��� �ð��� ����
		�δ��� �ʱ�ȭ �� �� �ֵ���
		���� ������ ���·� ���� ���� �ش�.

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
		������Ʈ �δ� ����

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

    //GUID ����
    // �������� ���� ���� �ʱ�ȭ �Ѵ�.
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
		�ʵ� �δ� ���� ��û

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
			 ���������� ���� �� 
			 �δ� ���� �� ó���� �Դٸ� 
			 �ش� �δ��� ���� �� �ش�.

		 */
		DeleteInstanceMap(dwGaeaMapID);

		return false;
	}

	/**
		���� ���� ��û�� �δ��� �ٸ� �δ��� �´ٸ�
		������ ���е� ��û �� ���� �ִ�.
		�̷� ��� �����̶�� ������ ���� ������
		���ж�� �츮�� ���� �� ��� �Ѵ�.

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
		�δ��� �Ѹ��̶� ������ �Ǹ� �δ� �Ϸ� ���°� �ȴ�.
		�Ѹ��̶� ���� ���� ���ϸ� ���� ��� �ð��� ���� �ʱ�ȭ �ȴ�.
		�׷��� ���� ���и� ���� �Ѵ�.

	 */
    if (bResult && IsCreating())
    {
		SetState(INDUNSTATE_CREATED);
	}

	/**
		�Դ��� ���� �����Ƿ�
		���������� ���� �ϵ��� �˷� �ش�.

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
        �絵�� ������ �ٽ� �δ� ���� �� �Դ� ��Ų��.

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
        // ��� ���и� ���� Ÿ�̸� �ߵ�
        m_timers.InsertCallback(to, 10.f, boost::bind(std::mem_fun(&CPVEInDun::Timerover), this));
    }

    if (to == INDUNSTATE_CREATING)
    {
        // ���� ���и� ���� Ÿ�̸� �ߵ�
        m_timers.InsertCallback(to, 30.f, boost::bind(std::mem_fun(&CPVEInDun::FailedCreate), this));

		// ��Ƽ �ݹ� ���
		SetPartyCallback();
    }

	if (to == INDUNSTATE_NONE)
	{
		// ��Ƽ �ݹ� ����
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
