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
    GLPartyID		m_PartyID;
	DWORD			m_dwHashKey;
    DWORD			m_dwMasterID;
	DWORD			m_dwTimeKey;
    SPartyOption	m_sOption;
    std::string		m_strMasterName;
};
