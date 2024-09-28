#include "pch.h"
#include "./NotifyMan.h"

#include "../../SigmaCore/DebugInclude.h"

namespace gamelogic
{

    NotifyMan::NotifyMan()
    {
    }
    
    NotifyMan::~NotifyMan()
    {
    }

    void NotifyMan::Clear()
    {
        m_Data.clear();
    }

    NotifyMan& NotifyMan::operator = (const NotifyMan& rhs)
    {
        if (this == &rhs)
            return *this;

        m_Data = rhs.m_Data;

        return *this;
    }

    bool NotifyMan::Add(std::tr1::shared_ptr<NotifyBase> spData)
    {
        if (!spData)
            return false;
        
        NOTIFY_DATA_ITER iter = m_Data.find(spData->GetGuid());
        if (iter != m_Data.end())
            return false;
        
        m_Data.insert(NOTIFY_DATA_VALUE(spData->GetGuid(), spData));
        return true;
    }

    bool NotifyMan::Del(sc::MGUID Guid)
    {
        NOTIFY_DATA_ITER iter = m_Data.find(Guid);
        if (iter != m_Data.end())
        {
            m_Data.erase(iter);
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace gamelogic
