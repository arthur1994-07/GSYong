#ifndef CCbTimer_H
#define CCbTimer_H

#include <map>
#include <boost/function.hpp>

/**
    �ð��� ��� �� �� �ݹ��� ȣ�� �ϴ� Ÿ�̸Ӹ� ���� �Ѵ�.
    �̸� ���� ������ �ð� ���� ������ ���� ��� ���𰡸� �����ϴ� ����
    �ս��� �� �� �ִ�. ������� 10�� ���� ������ ������ �����ϰ� ����� ������.

 */
class CCbTimer
{
public:
        typedef int TimerID;
        typedef float TimeType;
        typedef boost::function<void (void)> TimerCallback;

        /**
            Ÿ�̸Ӱ� �Ϸ�Ǹ� ȣ��� �ݹ��� ��� �Ѵ�.
            ID�� Ÿ�̸Ӹ� �����ϴ� ������ �̴�.
            �� �����ڸ� ���� Ÿ�̸Ӱ� ���� �ȴ�.
            �׷��Ƿ� ������ ID�� ������� �ʵ��� �ؾ� �Ѵ�.
            timer �� Ÿ�̸� �ð����� �� �ð��� �Ϸ�Ǹ� ȣ�� �ȴ�.

         */
        void                InsertCallback(TimerID id, TimeType timer, TimerCallback fun);

        /**
            Ÿ�̸Ӹ� ���� �Ѵ�.
            Ÿ�̸� �ð��� �Ϸᰡ �Ǽ� �ݹ� ȣ�� �� ���� �ǰų�
            Ÿ�̸Ӱ� �ʿ� ���� ���� �� ���� �ϸ� �ȴ�.

         */
        void                RemoveCallback(TimerID id);

        /**
            Ÿ�̸��� �ð��� ��� �Ѵ�.
            Ÿ���� �Ϸ�Ǹ� �ݹ��� ȣ���ϰ� Ÿ�̸Ӹ� ���� �Ѵ�.

         */
        void                FrameMove(TimeType elapsetime);

protected:
        /**
            ���ο��� Ÿ�̸Ӹ�
            ���� �ϱ����� �ڷ� ����
            ��� �ð��� �ݹ��� ������.

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