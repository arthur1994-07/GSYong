#pragma once

#include <string>
#include <vector>
#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../Party/GLPartyDefine.h"

namespace party
{

struct NET_PARTY_INFO_REQ_CA
{
    GLPartyID PartyID;

    MSGPACK_DEFINE(PartyID);

    NET_PARTY_INFO_REQ_CA(const GLPartyID& _PartyID)
        : PartyID(_PartyID)
    {
        // NET_MSG_PARTY_INFO_REQ_CA
    }

    NET_PARTY_INFO_REQ_CA()
    {
        // NET_MSG_PARTY_INFO_REQ_CA
    }
};

struct PARTY_MEMBER_INFO
{
    DWORD m_ChaDbNum;
    DWORD m_ChaClass; // EMCHARCLASS
    WORD  m_ChaLevel;
    std::string m_ChaName;

	MSGPACK_DEFINE(m_ChaDbNum, m_ChaClass, m_ChaLevel, m_ChaName);

    PARTY_MEMBER_INFO()
        : m_ChaDbNum(0)
        , m_ChaClass(0)
        , m_ChaLevel(0)
    {
    }

    PARTY_MEMBER_INFO(DWORD ChaDbNum, DWORD ChaClass, WORD ChaLevel, const std::string& ChaName)
        : m_ChaDbNum(ChaDbNum)
        , m_ChaClass(ChaClass)
        , m_ChaLevel(ChaLevel)
        , m_ChaName(ChaName)
    {
    }
};

struct NET_PARTY_INFO_REQ_AC
{
	GLPartyID PartyID;

	std::vector<PARTY_MEMBER_INFO> m_Member;

	MSGPACK_DEFINE(PartyID, m_Member);

	NET_PARTY_INFO_REQ_AC()
	{
		// NET_MSG_PARTY_INFO_REQ_AC
	}

	NET_PARTY_INFO_REQ_AC(const GLPartyID& _PartyID)
		: PartyID(_PartyID)
    {
        // NET_MSG_PARTY_INFO_REQ_AC
    }

    void AddMember(DWORD ChaDbNum, DWORD ChaClass, WORD ChaLevel, const std::string& ChaName)
    {
        m_Member.push_back(PARTY_MEMBER_INFO(ChaDbNum, ChaClass, ChaLevel, ChaName));
    }
};

} // namespace party