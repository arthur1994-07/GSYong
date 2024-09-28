#ifndef CPVEInDunParty_H
#define CPVEInDunParty_H

#include <map>
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../RanLogicServer/Party/GLPartyAgent.h"
#include "../TriggerSystem/ServerMapInfoMessage.h"
#include "../TriggerSystem/ServerRewardInstance.h"

typedef GLPartyAgent::MEMBER PartyMember;
typedef PartyMember::iterator PartyMemberIt;

typedef std::map<DWORD, MAP_TABLE> PVEDBContainer;
typedef PVEDBContainer::iterator PVEDBContainerIt;
typedef PVEDBContainer::const_iterator PVEDBContainerCIt;

typedef GLMSG::StageResult StageResult;

namespace PVESystem
{

class CPVEInDun;

/**
	PVE �ͼ� �δ��� ���� ��
    ��Ƽ ������� ���� �Ѵ�.

 */
class CPVEInDunParty
{
public:
    CPVEInDunParty();

    /**
        ��� �������� ���� �Ѵ�.

     */
    bool            Init(GLPartyAgent* pPartyAgent, GLAgentServer* pServer, const SPVEInDunID& id);

	/**
		PVE �ý��ۿ� ��Ƽ������ ��� �Ѵ�.
		�̷��� ��� �� �ο��� �δ��� ������ �ο��� �ȴ�.

	 */
	void			Register();

	/**
		PVE �ý��ۿ� ��Ƽ������ ���� �Ѵ�.
		���� �� �ο����� �ٽ� �δ��� ������ �� ������ ���� �Ѵ�.

		\param bOwner �δ� ���� ��� ���� ����
	 */
	void			UnRegister(bool bOwner);	

    /**
        ���� �ڷḦ �ʱ�ȭ �Ѵ�.

     */
    void            Reset();

    /**
        ������� DB ������ ����, ���� �Ѵ�.
        �̰��� �����ϸ� ���ϰ� �ɸ��Ƿ�
        ��� �Ϲ����� �˻簡 ���� �� ���������� ȣ�� �ؾ� �Ѵ�.

     */    
    void            SetDB(MAP_TABLE& tbl);
    bool            GetDB(const DWORD dwCharDbNum, MAP_TABLE& tbl);
	void			ProcessDB(const int nOwnTime, float& fLifeTime);

    /**
        ��Ƽ ���� ID �� ��� �Ѵ�.
        �̸� ���� �δ��� ���� �Ѵ�.

     */
    const GLPartyID GetPartyID() const { return m_partyID; }

	/**
		���� ��Ƽ ������� ���� �Ѵ�.

	 */
	const DWORD 	GetCount() const;

    /**
        ��� ������� ���� ī��Ʈ�� �����ϴ��� üũ �Ѵ�.        

     */
    bool            IsCount(int nCount, int nOwntime);

	/**
		�Ҽ� �δ��� �ִ� ����� �ִ��� üũ �Ѵ�.

	 */
	bool			IsIndun() const;

	/**
		������� CTF �� ��û���̰ų� ���������� Ȯ�� �Ѵ�.

	 */
	bool			IsCTF() const;
	bool			IsCTF(DWORD dwCharDbNum) const;

    /**
		������� ��Ī�ý��� ������ �� ��û���̰ų� ���������� Ȯ�� �Ѵ�.

	 */
	bool			IsJoinMatchingContents() const;
	bool			IsJoinMatchingContents(DWORD dwCharDbNum) const;

	/**
		��� ������� ���� �������� ���ԵǴ��� ���ɴ�.

	 */
	bool			IsLevel(const WORD wMin, const WORD wMax) const;

	/**
		���� ��Ƽ�� ��Ƽ������ üũ �Ѵ�.

	 */
	bool			IsMaster(const DWORD dwCharDbNum) const;

	/**
		��û�� ����� ���� ����� �������� Ȯ�� �Ѵ�.
		��û �� ���� Ȯ�� ���̿� ����� ���� �� �� �ִ�.

	 */
	bool			ChkMember() const;

	/**
		����� ���� �Ѵ�.

	 */
	void			RemoveMember(DWORD dwCharDbNum);

    /**
    	���� Ȯ�� ��Ƽ ����� ó�� �Ѵ�.
        ��� ��Ƽ���� Ȯ���� �ؾ����� �Դ��� �ȴ�.

     */
	bool			IsPassMember(DWORD member);
    bool            PassMember(DWORD member);

    /**
    	��Ƽ������ �ԷµǴ� ���� ����Ʈ�� ��� �̵� ��Ų��.

     */
    void            MoveParty(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID);
	void            Move(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID, DWORD dwCharDbNum);

    /**
        ��� ������� �δ� �� ��ġ�� �ǵ�����.
        �̰��� �δ� �ͼӽð��� �����ų� ���� ���⸦ �� ��� ��� �ȴ�.

        nWhy ���ϴ�� ����
     */
    void            RestoreAll(int nWhy);    

    /**
        ��� ��Ƽ���� ���� ��ġ�� ���� �Ѵ�.
        �̰��� �δ��� �������� ���� �ȴ�.

     */
    void            SavePositionAll();
	void            SavePosition(DWORD dwCharDbNum);

    /**
        �������� ����� ���� �Ѵ�.
        �� ���������� Ŭ���� �ϸ� ���� �Ǿ� ����
        PVE �� ����Ǳ� ������ �����Ǿ� ���� ������ ó�� �Ѵ�.

     */
    void            SetStageResult(const StageResult& results);
	
    /**
        ��Ƽ���� �޽����� ������ ���� ���� �Լ����� ���� �Ѵ�.

     */
    template<class T>
    void            SendParty(const T& NetMsg, const EMNET_MSG& eType)
    {
        if (m_server)
        {
            msgpack::sbuffer buf;
            msgpack::pack(buf, NetMsg);
            m_server->SENDTOPARTY(m_partyID, eType, buf);
        }
    }

	void            SendParty(NET_MSG_GENERIC* nmg);

	template<class T>
	void            SendPartyMember(DWORD dwCharDbNum, const T& NetMsg, const EMNET_MSG& eType)
	{
		if (m_server)
		{
			typename GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

			if (pChar)
			{
				msgpack::sbuffer buf;
				msgpack::pack(buf, NetMsg);
				m_server->SENDTOCLIENT(pChar->ClientSlot(), eType, buf);
			}
		}
	}

	template<class T>
	void            SendFieldPartyMember(DWORD dwCharDbNum, const T& NetMsg, const EMNET_MSG& eType)
	{
		if (m_server)
		{
			typename GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

			if (pChar)
			{
				msgpack::sbuffer buf;
				msgpack::pack(buf, NetMsg);
				m_server->SENDTOFIELD(pChar->GetChannel(), pChar->GetCurrentFieldServer(), eType, buf);
			}
		}
	}

protected:
	/**
		��Ƽ������ ���� �Ѵ�.

	 */
	PartyMember		CollectMember();

	/**
		��Ƽ���� ��ȯ ��Ų��.

	 */
	void            Restore(int nWhy, DWORD dwCharDbNum);

    GLAgentServer*  m_server;   //@ ����� ������Ʈ ����

    GLPartyID       m_partyID;  //@ ��Ƽ ���� ID
    SPVEInDunID     m_indunID;  //@ �δ� ���� ID

    PartyMember     m_member;   //@ ��û �� �����
    PartyMember     m_pass;     //@ ��û Ȯ�� �� �����
	PartyMember     m_register; //@ PVE �δ��� ��ϵ� ���

    StageResult     m_results;   //@ ����� ���� ����
};

} // end namespace

#endif