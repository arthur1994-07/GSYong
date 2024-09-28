#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/weak_ptr.hpp>
#include <map>

#include "../../SigmaCore/String/MinGuid.h"

#include "../../RanLogic/Notify/NotifyDefine.h"

class GLGaeaClient;
class INotifyClientBase;

class NotifyClientMan
{
private:
    enum
    {
        DATA_OVERLAP_TIME = 10,
    };
public:
    typedef std::map<sc::MGUID, std::tr1::shared_ptr<INotifyClientBase> >  NOTIFY_DATA;
    typedef NOTIFY_DATA::iterator                                       NOTIFY_DATA_ITER;
    typedef NOTIFY_DATA::const_iterator                                 NOTIFY_DATA_CITER;
    typedef NOTIFY_DATA::value_type                                     NOTIFY_DATA_VALUE;

public:
    NotifyClientMan(GLGaeaClient* pGaeaClient);
    virtual ~NotifyClientMan();

protected:
    GLGaeaClient*       m_pGaeaClient;
    NOTIFY_DATA         m_Data;
    
public:
    void                Clear();
    bool                Add(std::tr1::shared_ptr<INotifyClientBase> spData);
    bool                Del(sc::MGUID Guid);
    const NOTIFY_DATA&  GetData() const { return m_Data; }
    const std::tr1::shared_ptr<INotifyClientBase> GetData( sc::MGUID Guid ) const;
    const std::tr1::shared_ptr<INotifyClientBase> GetLatelyData() const;

private:
    sc::MGUID           m_LatelyGuid;     //최근에 도착한 메세지

};
