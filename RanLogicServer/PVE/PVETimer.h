#ifndef CCbTimer_H
#define CCbTimer_H

#include <map>
#include <boost/function.hpp>

/**
    시간이 경과 한 후 콜백을 호출 하는 타이머를 정의 한다.
    이를 통해 임의의 시간 동안 응답이 없을 경우 무언가를 수행하는 일을
    손쉽게 할 수 있다. 예를들어 10초 동안 응답이 없으면 포기하게 만드는 로직등.

 */
class CCbTimer
{
public:
        typedef int TimerID;
        typedef float TimeType;
        typedef boost::function<void (void)> TimerCallback;

        /**
            타이머가 완료되면 호출된 콜백을 등록 한다.
            ID는 타이머를 구분하는 구분자 이다.
            이 구분자를 통해 타이머가 관리 된다.
            그러므로 동일한 ID를 사용하지 않도록 해야 한다.
            timer 는 타이머 시간으로 이 시간이 완료되면 호출 된다.

         */
        void                InsertCallback(TimerID id, TimeType timer, TimerCallback fun);

        /**
            타이머를 삭제 한다.
            타이머 시간이 완료가 되서 콜백 호출 후 삭제 되거나
            타이머가 필요 없어 졌을 시 삭제 하면 된다.

         */
        void                RemoveCallback(TimerID id);

        /**
            타이머의 시간을 경과 한다.
            타임이 완료되면 콜백을 호출하고 타이머를 제거 한다.

         */
        void                FrameMove(TimeType elapsetime);

protected:
        /**
            내부에서 타이머를
            관리 하기위한 자료 구조
            경과 시간과 콜백을 가진다.

         */
        struct TimerInfo
        {
            TimeType        curtime;
            TimerCallback   fun;
        };

        typedef std::map<TimerID, TimerInfo> TimerContainer;
        typedef TimerContainer::iterator TimerContainerIt;

        TimerContainer      m_cRepository;
};

#endif