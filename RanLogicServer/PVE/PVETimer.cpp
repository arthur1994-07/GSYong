#include "pch.h"
#include "PVETimer.h"

void CCbTimer::InsertCallback(TimerID id, TimeType timer, TimerCallback fun)
{
    if (timer <= 0)
    {
        return;
    }

    m_cRepository[id].curtime = timer;
    m_cRepository[id].fun = fun;
}

void CCbTimer::RemoveCallback(TimerID id)
{
    TimerContainerIt it = m_cRepository.find(id);

    if (it != m_cRepository.end())
    {
        m_cRepository.erase(it);
    }
}

void CCbTimer::FrameMove(TimeType elapsetime)
{
    for (TimerContainerIt it = m_cRepository.begin(); it != m_cRepository.end(); )
    {
        it->second.curtime -= elapsetime;

        if (it->second.curtime <= 0)
        {
            TimerInfo temp = it->second;
            m_cRepository.erase(it++);
            temp.fun();
        }
        else
        {
            ++it;
        }
    }
}
