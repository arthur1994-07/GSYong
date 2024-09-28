#include "pch.h"
#include "PVEInDunParty.h"
#include "PVESystem.h"
#include "PVEInDun.h"
#include "../AgentServer/GLAgentServer.h"

using namespace PVESystem;

CPVEInDunParty::CPVEInDunParty()
{
    Reset();
}

bool CPVEInDunParty::Init(GLPartyAgent* pPartyAgent, GLAgentServer* pServer, const SPVEInDunID& id)
{
    if (pPartyAgent == 0 || pServer == 0)
    {
        return false;
    }

    Reset();

    m_server = pServer;
    m_indunID = id;
    m_partyID = pPartyAgent->GetPartyID();
	m_member = CollectMember();

	return true;
}

void CPVEInDunParty::Register()
{
	for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
	{
		PVESystem::InsertInDunMember(*it, m_indunID);
	}

	m_register = m_member;
}

void CPVEInDunParty::UnRegister(bool bOwner)
{
	for (GLPartyAgent::MEMBER_CITER it = m_register.begin(); it != m_register.end(); )
	{
		DWORD dwCharDbNum = *it;

		if (m_indunID.dwCharDbNum == dwCharDbNum && bOwner == false)
		{
			++it;
			continue;
		}

		GLMSG::NET_PVE_REMOVE_INDUN_AC NetMsg;
		NetMsg.sInDunID = m_indunID;
		SendPartyMember(dwCharDbNum, NetMsg, NET_MSG_PVE_REMOVE_INDUN_AC);

		PVESystem::RemoveInDunMember(dwCharDbNum);
		it = m_register.erase(it);
	}
}

void CPVEInDunParty::Reset()
{
    m_member.clear();
    m_pass.clear();
    m_partyID.Reset();
	m_results.clear();	
	m_server = 0;
}

void CPVEInDunParty::SetDB(MAP_TABLE& tbl)
{
    tbl.dwMapID = m_indunID.dwMapID;
    tbl.nLevel = m_indunID.nLevel;
    PVESystem::SetMapTable(tbl);
}

bool CPVEInDunParty::GetDB(const DWORD dwCharDbNum, MAP_TABLE& tbl)
{
    tbl.dwCharDbNum = dwCharDbNum;
    tbl.dwMapID = m_indunID.dwMapID;
    tbl.nLevel = m_indunID.nLevel;
    return PVESystem::GetMapTable(tbl);
}

void CPVEInDunParty::ProcessDB(const int nOwnTime, float& fLifeTime)
{
	MAP_TABLE tbl;

	// ó�� ���� ����
	bool bCreate = true;
	// �δ� �ͼ� ����
	bool bClearOfBelonging = false;	

	// ���� ����ð��� ���� �Ѵٸ�
	// �̹� ������ �δ� �̴�
	if (fLifeTime > 0.f)
	{
		bCreate = false;
		bClearOfBelonging = true;
	}

	// �ͼ��� �ƴѵ� ������ ������ ������
	// �ð��� ���� �� �� �����Ƿ�
	// DB �� ���� �ͼ� ������ ���ɴ�.
	if (bClearOfBelonging == false)
	{
		if (GetDB(m_indunID.dwCharDbNum, tbl))
		{
			fLifeTime = float(PVESystem::DiffTime(tbl.tGenTime, nOwnTime));

			if (fLifeTime > 0.f)
			{
				bClearOfBelonging = true;
			}
		}
	}

	for (GLPartyAgent::MEMBER_CITER it = m_pass.begin(); it != m_pass.end(); ++it)
	{
		if (GetDB(*it, tbl))
		{
			if (bClearOfBelonging && tbl.dwCharDbNum == m_indunID.dwCharDbNum)
			{
				// �ͼ� �� �δ��� �δ� �����ʹ�
				// ī��Ʈ�� ���� �ʴ´�.
				continue;
			}

			if (bCreate && bClearOfBelonging == false && tbl.dwCharDbNum == m_indunID.dwCharDbNum)
			{
				// ���� ���� �̰�
				// �ͼӵ� �δ��� �ƴ϶��
				// ���� �δ� ���� ������ �Է� �Ѵ�.
				PVESystem::SetCurTime(tbl);
				fLifeTime = float(PVESystem::DiffTime(tbl.tGenTime, nOwnTime));
				++tbl.nGenCount;
			}

			if (bCreate)
			{
				// ó������ �����ÿ���
				// ��� ������� ī���� �Ѵ�.
				++tbl.nInCount;
				SetDB(tbl);
			}
		}
	}
}

const DWORD CPVEInDunParty::GetCount() const
{
	if (m_server == 0)
	{
		return false;
	}

	const GLPartyAgent* const pPartyAgent = m_server->GetParty(m_partyID);

	if (pPartyAgent == 0)
	{
		return false;
	}

	return pPartyAgent->getNPartyMember();
}

bool CPVEInDunParty::IsCount(int nCount, int nOwntime)
{
    MAP_TABLE tbl;
    tbl.dwMapID = m_indunID.dwMapID;
    tbl.nLevel = m_indunID.nLevel;

    for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
    {
        tbl.dwCharDbNum = *it;

        if (PVESystem::GetMapTable(tbl))
        {
            if (tbl.dwCharDbNum == m_indunID.dwCharDbNum)
            {
                // �������� ��� ���� �δ��� Ȱ��ȭ ���¶��
                // ī��Ʈ üũ�� ���� �ʴ´�.
                if (float(PVESystem::DiffTime(tbl.tGenTime, nOwntime)) > 0.f)
                {
                    continue;
                }
            }

            if (tbl.nInCount >= nCount)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool CPVEInDunParty::IsIndun() const 
{
	for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
	{
		CPVEInDun* pIndun = PVESystem::FindInstantDungeon(*it);

		if (pIndun)
		{
			if (m_indunID != pIndun->GetID())
			{
				return true;
			}
		}
	}

	return false;
}

bool CPVEInDunParty::IsCTF() const 
{
	if (m_server == 0)
	{
		return false;
	}

	for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
	{
		if (IsCTF(*it))
		{
			return true;
		}
	}

	return false;
}

bool CPVEInDunParty::IsCTF(DWORD dwCharDbNum) const 
{
	if (m_server)
	{
		if (m_server->IsRegisterCTFMember(dwCharDbNum))
		{
			return true;
		}

		GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

		if (pChar)
		{
			if (pChar->IsCtfPlayer())
			{
				return true;
			}
		}
	}

	return false;
}

bool CPVEInDunParty::IsJoinMatchingContents() const 
{
    for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
    {
        if (IsJoinMatchingContents(*it))
        {
            return true;
        }
    }

    return false;
}

bool CPVEInDunParty::IsJoinMatchingContents(DWORD dwCharDbNum) const 
{
    return MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin(dwCharDbNum);
}

bool CPVEInDunParty::IsLevel(const WORD wMin, const WORD wMax) const 
{
	if (m_server == 0)
	{
		return false;
	}

	for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
	{
		GLCharAG* pChar = m_server->GetCharByDbNum(*it);

		if (pChar == 0)
		{
			return false;
		}

		const WORD wTempLevel = pChar->GetChaLevel();

		if (wTempLevel < wMin)
		{
			return false;
		}

		if (wTempLevel > wMax)
		{
			return false;
		}
	}

	return true;
}

bool CPVEInDunParty::IsMaster(const DWORD dwCharDbNum) const
{
	if (m_server == 0)
	{
		return false;
	}

	GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

	if (pChar == 0)
	{
		return false;
	}

	const GLPartyAgent* const pPartyAgent = m_server->GetParty(m_partyID);

	if (pPartyAgent == 0)
	{
		return false;
	}

	return pPartyAgent->getMaster() == pChar->GaeaId();
}

bool CPVEInDunParty::ChkMember() const
{
	if (m_server == 0)
	{
		return false;
	}

	const GLPartyAgent* const pPartyAgent = m_server->GetParty(m_partyID);

	if (pPartyAgent == 0)
	{
		return false;
	}

	if (pPartyAgent->getNPartyMember() != m_member.size())
	{
		return false;
	}

	for (GLPartyAgent::MEMBER_CITER it = m_member.begin(); it != m_member.end(); ++it)
	{
		GLCharAG* pChar = m_server->GetCharByDbNum(*it);

		if (pChar == 0)
		{
			return false;
		}

		if (pPartyAgent->isPartyMember(pChar->GaeaId()) == false)
		{
			return false;
		}
	}

	return true;
}

void CPVEInDunParty::RemoveMember(DWORD dwCharDbNum)
{
	m_member.erase(std::find(m_member.begin(), m_member.end(), dwCharDbNum));
	m_pass.erase(std::find(m_pass.begin(), m_pass.end(), dwCharDbNum));
	m_register.erase(std::find(m_register.begin(), m_register.end(), dwCharDbNum));
	PVESystem::RemoveInDunMember(dwCharDbNum);
}

bool CPVEInDunParty::IsPassMember(DWORD member)
{
	return std::find(m_pass.begin(), m_pass.end(), member) != m_pass.end();
}

bool CPVEInDunParty::PassMember(DWORD member)
{
    GLPartyAgent::MEMBER_ITER it = std::find(m_member.begin(), m_member.end(), member);

    if (it == m_member.end())
    {
        // ��� �ȵ� ���
        return false;
    }

    it = std::find(m_pass.begin(), m_pass.end(), member);

    if (it != m_pass.end())
    {
        // �̹� ��ϵ� ���
        return false;
    }

    m_pass.push_back(member);

    if (m_pass.size() == m_member.size())
    {
        return true;
    }

    return false;
}

void CPVEInDunParty::MoveParty(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID)
{
    if (m_server == 0 || pIndun == 0)
    {
        return;
    }

    bool bReward = true;
    SNATIVEID baseMapID(WORD(pIndun->GetID().dwMapID), 0);

    for (GLPartyAgent::MEMBER_ITER it = m_pass.begin(); it != m_pass.end(); ++it)
    {
        GLCharAG* pChar = m_server->GetCharByDbNum(*it);

        if (pChar == 0)
        {
			continue;
		}

		DWORD dwPreMapID = pIndun->GetPreEntranceMapID();
		DWORD dwCurMapID = pChar->GetCurrentMap().getGaeaMapID().dwID;
// 
// 		/**
// 			�̵� �� �ʿ� ���� �ʴٸ� ���� ��Ƽ�� �ƴϰų�
// 			��Ƽ����� �δ��� ���� ���� ����� �ƴϹǷ� �̵� ��Ű�� �ʴ´�.
// 
// 		 */
        
        // �������� �Ϲ� �ʵ��� ����
        if( dwPreMapID != 0 )
            if(dwPreMapID != dwCurMapID)    // ������ �ۿ� �ִ� �������...
                continue;
		

		DWORD dwNextFieldSvr = m_server->GetFieldServer(baseMapID);
		DWORD dwCurFiledSvr = pChar->GetCurrentFieldServer();

		/**
			1���� ���� ������ ���� �Ѵ�.
			���������� �Ϸ� �Ǿ� �������� �̵� �� ���� ������
			���� ���������� ���� �Ѵ�.

		 */
        if (bReward)
        {
            GLMSG::NET_PVE_STAGE_RESULT_FA_OR_AF NetMsg;
            NetMsg.sInDunID = pIndun->GetID();
            NetMsg.sNativeInDunID.dwID = pIndun->GetEntranceMapID();
            NetMsg.results = m_results;
            msgpack::sbuffer buf;
            msgpack::pack(buf, NetMsg);
            m_server->SENDTOFIELD(pChar->GetChannel(), dwNextFieldSvr, NET_MSG_PVE_STAGE_RESULT_FA_OR_AF, buf);
            bReward = false;
        }

		/**
			ĳ���͸� �� ���̷� �̵� ��Ų��.

		 */
		if (dwNextFieldSvr == dwCurFiledSvr)
		{
			m_server->ChangeMap(pChar->GaeaId(), dwMapID, dwGateID, m_indunID);
		}
		else
		{
			m_server->ChangeFieldSvr(dwNextFieldSvr, pChar->GaeaId(), baseMapID.Id(), dwMapID, dwGateID);
		}
    }
}

void CPVEInDunParty::Move(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID, DWORD dwCharDbNum)
{
	if (m_server == 0)
	{
		return;
	}

	//SNATIVEID sBaseMapID(SNATIVEID(dwMapID).wMainID, 0);
    SNATIVEID baseMapID(WORD(pIndun->GetID().dwMapID), 0);

	GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

	if (pChar == 0)
	{
		return;
	}

	/**
		�̹� ���� �ʿ� �ִ� �ο��� �ٽ� �鿩 ���� �� ����.
		Ȥ�� �� ���� ó��

	 */
	if (pChar->GetCurrentMap().getGaeaMapID().Id() == dwMapID)
	{
		return;
	}



	DWORD dwNextFieldSvr = m_server->GetFieldServer(baseMapID);
	DWORD dwCurFiledSvr = pChar->GetCurrentFieldServer();

	/**
		ĳ���͸� �� ������ �̵� ��Ų��.

	 */
	{
		m_server->ChangeFieldSvr(dwNextFieldSvr, pChar->GaeaId(), baseMapID.Id(), dwMapID, dwGateID);
	}
}

void CPVEInDunParty::RestoreAll(int nWhy)
{
    for (GLPartyAgent::MEMBER_ITER it = m_pass.begin(); it != m_pass.end(); ++it)
    {
		Restore(nWhy, *it);
    }
}

void CPVEInDunParty::Restore(int nWhy, DWORD dwCharDbNum)
{
    if (m_server == 0)
    {
        return;
    }

    GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

    if (pChar == 0)
    {
        return;
    }

    GLMSG::NET_PVE_RETURN_REQ_AF NetMsg;
    NetMsg.dwCharDbNum = dwCharDbNum;
    NetMsg.nWhy = nWhy;
    msgpack::sbuffer buf;
    msgpack::pack(buf, NetMsg);
    m_server->SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_PVE_RETURN_REQ_AF, buf);    
}

void CPVEInDunParty::SavePositionAll()
{
    for (GLPartyAgent::MEMBER_ITER it = m_pass.begin(); it != m_pass.end(); ++it)
    {
		SavePosition(*it);
    }
}

void CPVEInDunParty::SavePosition(DWORD dwCharDbNum)
{
	if (m_server == 0)
	{
		return;
	}

	GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

	if (pChar == 0)
	{
		return;
	}

	GLMSG::NET_PVE_SAVEPOS_REQ_AF NetMsg;
	NetMsg.dwCharDbNum = dwCharDbNum;
	msgpack::sbuffer buf;
	msgpack::pack(buf, NetMsg);
	m_server->SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_PVE_SAVEPOS_REQ_AF, buf);
}

void CPVEInDunParty::SetStageResult(const StageResult& results)
{
    m_results = results;
}

PartyMember CPVEInDunParty::CollectMember()
{
	PartyMember member;

	if (m_server == 0)
	{
		return member;		
	}

	const GLPartyAgent* const pPartyAgent = m_server->GetParty(m_partyID);

	if (pPartyAgent == 0)
	{
		return member;
	}

	const PartyMember& real = pPartyAgent->GetMember();

	for (GLPartyAgent::MEMBER_CITER it = real.begin(); it != real.end(); ++it)
	{
		GLCharAG* pChar = m_server->GetChar(*it);

		if (pChar)
		{			
			member.push_back(pChar->ChaDbNum());
		}
	}

	return member;
}

void CPVEInDunParty::SendParty(NET_MSG_GENERIC* nmg)
{
	if (m_server)
	{
		m_server->SENDTOPARTY(m_partyID, nmg);
	}
}

