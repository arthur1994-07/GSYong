#ifndef GLPartyNotifier_H
#define GLPartyNotifier_H

#include <boost/function.hpp>
#include <list>
#include "FunctionChecker.h"

/**
	��Ƽ�� ���� ������ ���� �� ��ϵ� �ݹ��� ȣ�� �Ѵ�.

    ��Ƽ�� �پ��� �������� ����� �� �ִ�.
    ������� ���� ������ ��Ƽ ������ �Ѵٴ��� ���.
    �׷��� ������ �������� ��Ƽ�� ���� �Ǹ� �׿� �´� ó���� �ؾ� �� ���� �ִ�.

    �̷��� ��Ƽ�� ��� �ڵ���� �м��� ������ ���� �ڵ带 �ֱ�� ������� ������
    ���� ã�� �ִ��� �ڵ尡 �������� �� ���̴�.

    �׷��� ��Ƽ�� ���� �Ǹ� �����ؼ� �ݹ� ���·� �˰� �;��ϴ� �������� �뺸�� ���ش�.
    �׷��� �̹� ��Ƽ�� ���� ����� ������ ���� �����Ƿ� �� ����� ����ȭ �Ƿ��� �ð��� �ɸ� ���̴�.
    �׶����� �� �ٵ�´ٸ� ��Ƽ�� ����� �پ��� �������� ���������� Ȱ��Ǹ��� ���� �Ѵ�.

 */
template<class Caller>
class GLPartyNotifier
{
public:
        typedef boost::function<void (Caller*)> GLPartyCallback;
        typedef std::list<GLPartyCallback> GLPartyCbContainer;

        /**
            ��� �ݹ��� ���� �Ѵ�.
            
         */
        void                CallbackClear()
        {
            m_changemaster.clear();
            m_destroy.clear();
            m_add.clear();
            m_remove.clear();
        }

        /**
        	��Ƽ�� ���� �ݹ��� ��� �Ѵ�.
            ��Ƽ�� ����� ȣ�� �ȴ�.

         */
        void                InsertChangeMasterCallback(GLPartyCallback fun) { Insert(fun, m_changemaster); }
        template<class T>
        void                RemoveChangeMasterCallback(T fun) { Remove(fun, m_changemaster); }
        void                RunChangeMasterCallback(Caller* pCaller) { Run(pCaller, m_changemaster); }

        /**
        	���� �ݹ��� ��� �Ѵ�.
            ��Ƽ�� ������ ȣ�� �ȴ�.

         */
        void                InsertDestroyCallback(GLPartyCallback fun) { Insert(fun, m_destroy); }
        template<class T>
        void                RemoveDestroyCallback(T fun) { Remove(fun, m_destroy); }
        void                RunDestroyCallback(Caller* pCaller) { Run(pCaller, m_destroy); }

        /**
        	��Ƽ�� �߰� �ݹ��� ��� �Ѵ�.
            ��Ƽ���� �߰��� �� ȣ�� �ȴ�.

         */
        void                InsertAddMemberCallback(GLPartyCallback fun) { Insert(fun, m_add); }
        template<class T>
        void                RemoveAddMemberCallback(T fun) { Remove(fun, m_add); }
        void                RunAddMemberCallback(Caller* pCaller) { Run(pCaller, m_add); }

        /**
            ��Ƽ�� ���� �ݹ��� ��� �Ѵ�.
            ��Ƽ���� ���ŵ� �� ȣ�� �ȴ�.

         */
        void                InsertRemoveMemberCallback(GLPartyCallback fun) { Insert(fun, m_remove); }
        template<class T>
        void                RemoveRemoveMemberCallback(T fun) { Remove(fun, m_remove); }
        void                RunRemoveMemberCallback(Caller* pCaller) { Run(pCaller, m_remove); }

protected:
        /**
            ���� �Լ�
            ���� �ڵ� �ּ�ȭ
        	
         */
        template<class T>
        void                Remove(T fun, GLPartyCbContainer& cont)
        {
            GLPartyCbContainer::iterator it = FindFunction<GLPartyCallback>(fun, cont.begin(), cont.end());

            if (it != cont.end())
            {
                cont.erase(it);
            }
        }

        void                Insert(GLPartyCallback fun, GLPartyCbContainer& cont)
        {
            cont.push_back(fun);
        }

        void                Run(Caller* pCaller, GLPartyCbContainer& cont)
        {
            // Run �� �����̳� ����(����)��
            // �Ͼ �� �����Ƿ� ��� ����
            GLPartyCbContainer copy = cont;

            for (GLPartyCbContainer::iterator it = copy.begin(); it != copy.end(); ++it)
            {
                (*it)(pCaller);
            }
        }

        GLPartyCbContainer  m_changemaster;
        GLPartyCbContainer  m_destroy;
        GLPartyCbContainer  m_add;
        GLPartyCbContainer  m_remove;
};

#endif