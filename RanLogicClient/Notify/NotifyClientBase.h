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
    sc::MGUID           m_Guid;     //! �˸� ������ȣ
    __time64_t          m_Date;     //! ��� �ð�
    NOTIFT_TYPE         m_Type;     //! �˸� Ÿ��
    OPEN_MESSAGE_TYPE   m_MessageBoxType; //! �޽����ڽ� Ÿ��

public:    
    //! �˸� NET_MSG_GENERIC ��Ŷ
    virtual bool        Parsing( NET_MSG_GENERIC* nmg ) = 0;
    virtual bool        Parsing( const json_spirit::mObject& Object ){ return false; }

    //! �˸� ������ȣ
    sc::MGUID           GetGuid() const { return m_Guid; }

    //! �˸� �ð�
    __time64_t          GetDate() const { return m_Date; }
    virtual std::string GetDateStr() const;

    //! �˸� �з�
    NOTIFT_TYPE         GetType() const { return m_Type; }
    virtual std::string GetTypeStr() const;

    //! �˸� ����
    virtual std::string Description() const = 0; 

    //! �޽��� �ڽ� �޽���
    virtual std::string GetMessageBoxText() const = 0;

    //! UI���� ���µ� �޽��� �ڽ� Ÿ��
    virtual int         GetMessageBoxType() { return m_MessageBoxType; }


public:
    virtual void        SendAccept() = 0;   //! ��û ����
    virtual void        SendRefused() = 0;  //! ��û ����
    
};