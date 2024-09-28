#pragma once

#include "NotifyClientBase.h"

class NotifyClientClubInvite : public INotifyClientBase
{
public:
    NotifyClientClubInvite(
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date );
    virtual ~NotifyClientClubInvite();

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
    DWORD MasterID;
    std::string ClubName;
    std::string MasterName;

public:
    DWORD GetMasterID() { return MasterID; }
    const std::string& GetClubName() const { return ClubName; }
    const std::string& GetMasterName() const { return MasterName; }
};
