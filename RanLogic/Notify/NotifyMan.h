#pragma once

#include <map>
#include <boost/tr1/memory.hpp>
#include "./Notify.h"

namespace gamelogic
{
    typedef std::map<sc::MGUID, std::tr1::shared_ptr<NotifyBase> > NOTIFY_DATA;
    typedef NOTIFY_DATA::iterator                               NOTIFY_DATA_ITER;
    typedef NOTIFY_DATA::const_iterator                         NOTIFY_DATA_CITER;
    typedef NOTIFY_DATA::value_type                             NOTIFY_DATA_VALUE;

    class NotifyBase;

    class NotifyMan
    {
    public:
        NotifyMan();
        virtual ~NotifyMan();
        NotifyMan& operator = (const NotifyMan& rhs);

    protected:
        NOTIFY_DATA m_Data;

    public:
        void Clear();
        bool Add(std::tr1::shared_ptr<NotifyBase> spData);
        bool Del(sc::MGUID Guid);
        const NOTIFY_DATA& GetData() const { return m_Data; }
    };

} // namespace gamelogic
