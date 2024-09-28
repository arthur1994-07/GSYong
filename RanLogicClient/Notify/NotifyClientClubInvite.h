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
    DWORD MasterID;
    std::string ClubName;
    std::string MasterName;

public:
    DWORD GetMasterID() { return MasterID; }
    const std::string& GetClubName() const { return ClubName; }
    const std::string& GetMasterName() const { return MasterName; }
};
