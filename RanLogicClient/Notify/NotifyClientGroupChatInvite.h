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
    //! 알림 NET_MSG_GENERIC 패킷
    virtual bool        Parsing( NET_MSG_GENERIC* nmg );

public:
    //! 알림 설명
    virtual std::string Description() const; 

    //! 메시지 막스 메시지
    virtual std::string GetMessageBoxText() const;

public:
    virtual void        SendAccept();       //! 요청 수락
    virtual void        SendRefused();      //! 요청 거절

private:
    DWORD		 m_dwKey;	// == ChaNumOwner
	std::string  m_strChaNameOwner;	//set by Server
    DWORD		 m_dwChaNumWhom;	// who will be invited?	-- set by Client
};
