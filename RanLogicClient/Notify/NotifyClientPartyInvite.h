#pragma once

#include "NotifyClientBase.h"
#include "../../RanLogic/Party/GLPartyDefine.h"

class NotifyClientPartyInvite : public INotifyClientBase
{
public:
    NotifyClientPartyInvite(
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date );
    virtual ~NotifyClientPartyInvite();

protected:
    //! �˸� NET_MSG_GENERIC ��Ŷ
    virtual bool        Parsing( NET_MSG_GENERIC* nmg );

public:
    //! �˸� ����
    virtual std::string Description() const; 

    //! �޽��� ���� �޽���
    virtual std::string GetMessageBoxText() const;

public:
    virtual void        SendAccept();       //! ��û ����
    virtual void        SendRefused();      //! ��û ����

private:
    GLPartyID		m_PartyID;
	DWORD			m_dwHashKey;
    DWORD			m_dwMasterID;
	DWORD			m_dwTimeKey;
    SPartyOption	m_sOption;
    std::string		m_strMasterName;
};
