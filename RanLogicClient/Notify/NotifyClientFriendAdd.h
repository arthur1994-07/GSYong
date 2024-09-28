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
    //! �˸� NET_MSG_GENERIC ��Ŷ
    virtual bool        Parsing( NET_MSG_GENERIC* nmg );
    virtual bool        Parsing( const json_spirit::mObject& Object );

public:
    //! �˸� ����
    virtual std::string Description() const; 

    //! �޽��� ���� �޽���
    virtual std::string GetMessageBoxText() const;

public:
    virtual void        SendAccept();       //! ��û ����
    virtual void        SendRefused();      //! ��û ����

private:
    std::string         m_strFriendName;
};
