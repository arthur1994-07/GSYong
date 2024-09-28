#pragma once

#include "NotifyClientBase.h"

class NotifyClientFriendAdd : public INotifyClientBase
{
public:
    NotifyClientFriendAdd(
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date );
    virtual ~NotifyClientFriendAdd();

protected:
    //! 알림 NET_MSG_GENERIC 패킷
    virtual bool        Parsing( NET_MSG_GENERIC* nmg );
    virtual bool        Parsing( const json_spirit::mObject& Object );

public:
    //! 알림 설명
    virtual std::string Description() const; 

    //! 메시지 막스 메시지
    virtual std::string GetMessageBoxText() const;

public:
    virtual void        SendAccept();       //! 요청 수락
    virtual void        SendRefused();      //! 요청 거절

private:
    std::string         m_strFriendName;
};
