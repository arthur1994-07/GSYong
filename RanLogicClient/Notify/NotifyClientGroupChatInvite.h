#pragma once

#include "NotifyClientBase.h"


class NotifyClientGroupChatInvite : public INotifyClientBase
{
public:
    NotifyClientGroupChatInvite(
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date );
    virtual ~NotifyClientGroupChatInvite();

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
    DWORD		 m_dwKey;	// == ChaNumOwner
	std::string  m_strChaNameOwner;	//set by Server
    DWORD		 m_dwChaNumWhom;	// who will be invited?	-- set by Client
};
