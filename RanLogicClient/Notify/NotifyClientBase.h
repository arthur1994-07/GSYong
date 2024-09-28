#pragma once

#include <boost/tr1/memory.hpp>

#include "../../SigmaCore/String/MinGuid.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../RanLogic/Notify/NotifyDefine.h"

struct NET_MSG_GENERIC;
class GLGaeaClient;

class INotifyClientBase
{
public:
    enum OPEN_MESSAGE_TYPE
    {
        TYPE_OK,
        TYPE_OKCANCEL,
        TYPE_YESNO,
        TYPE_INPUT,
    };
private:
    typedef gamelogic::notify::NOTIFY_TYPE NOTIFT_TYPE;
public:
    INotifyClientBase( 
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date, 
        const NOTIFT_TYPE Type );
    virtual ~INotifyClientBase();

protected:
    GLGaeaClient*       m_pGaeaClient;
    sc::MGUID           m_Guid;     //! 알림 고유번호
    __time64_t          m_Date;     //! 등록 시간
    NOTIFT_TYPE         m_Type;     //! 알림 타입
    OPEN_MESSAGE_TYPE   m_MessageBoxType; //! 메시지박스 타입

public:    
    //! 알림 NET_MSG_GENERIC 패킷
    virtual bool        Parsing( NET_MSG_GENERIC* nmg ) = 0;
    virtual bool        Parsing( const json_spirit::mObject& Object ){ return false; }

    //! 알림 고유번호
    sc::MGUID           GetGuid() const { return m_Guid; }

    //! 알림 시간
    __time64_t          GetDate() const { return m_Date; }
    virtual std::string GetDateStr() const;

    //! 알림 분류
    NOTIFT_TYPE         GetType() const { return m_Type; }
    virtual std::string GetTypeStr() const;

    //! 알림 설명
    virtual std::string Description() const = 0; 

    //! 메시지 박스 메시지
    virtual std::string GetMessageBoxText() const = 0;

    //! UI에서 오픈될 메시지 박스 타입
    virtual int         GetMessageBoxType() { return m_MessageBoxType; }


public:
    virtual void        SendAccept() = 0;   //! 요청 수락
    virtual void        SendRefused() = 0;  //! 요청 거절
    
};