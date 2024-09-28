#ifndef GLPartyNotifier_H
#define GLPartyNotifier_H

#include <boost/function.hpp>
#include <list>
#include "FunctionChecker.h"

/**
	파티의 변경 사항을 감지 해 등록된 콜백을 호출 한다.

    파티는 다양한 컨텐츠와 연계될 수 있다.
    예를들어 던전 입장을 파티 단위로 한다던가 등등.
    그런데 임의의 컨텐츠는 파티가 변경 되면 그에 맞는 처리를 해야 할 때도 있다.

    이럴때 파티의 모든 코드들을 분석해 적당한 곳에 코드를 넣기란 어려움이 따르고
    설사 찾아 넣더라도 코드가 지저분해 질 것이다.

    그래서 파티가 변경 되면 감지해서 콜백 형태로 알고 싶어하는 컨텐츠에 통보를 해준다.
    그런데 이미 파티가 많은 기능을 가지고 꼬여 있으므로 이 기능이 안정화 되려면 시간이 걸릴 것이다.
    그때까지 잘 다듬는다면 파티와 연계된 다양한 컨텐츠에 안정적으로 활용되리라 생각 한다.

 */
template<class Caller>
class GLPartyNotifier
{
public:
        typedef boost::function<void (Caller*)> GLPartyCallback;
        typedef std::list<GLPartyCallback> GLPartyCbContainer;

        /**
            모든 콜백을 제거 한다.
            
         */
        void                CallbackClear()
        {
            m_changemaster.clear();
            m_destroy.clear();
            m_add.clear();
            m_remove.clear();
        }

        /**
        	파티장 변경 콜백을 등록 한다.
            파티장 변경시 호출 된다.

         */
        void                InsertChangeMasterCallback(GLPartyCallback fun) { Insert(fun, m_changemaster); }
        template<class T>
        void                RemoveChangeMasterCallback(T fun) { Remove(fun, m_changemaster); }
        void                RunChangeMasterCallback(Caller* pCaller) { Run(pCaller, m_changemaster); }

        /**
        	삭제 콜백을 등록 한다.
            파티가 삭제시 호출 된다.

         */
        void                InsertDestroyCallback(GLPartyCallback fun) { Insert(fun, m_destroy); }
        template<class T>
        void                RemoveDestroyCallback(T fun) { Remove(fun, m_destroy); }
        void                RunDestroyCallback(Caller* pCaller) { Run(pCaller, m_destroy); }

        /**
        	파티원 추가 콜백을 등록 한다.
            파티원이 추가될 때 호출 된다.

         */
        void                InsertAddMemberCallback(GLPartyCallback fun) { Insert(fun, m_add); }
        template<class T>
        void                RemoveAddMemberCallback(T fun) { Remove(fun, m_add); }
        void                RunAddMemberCallback(Caller* pCaller) { Run(pCaller, m_add); }

        /**
            파티원 제거 콜백을 등록 한다.
            파티원이 제거될 때 호출 된다.

         */
        void                InsertRemoveMemberCallback(GLPartyCallback fun) { Insert(fun, m_remove); }
        template<class T>
        void                RemoveRemoveMemberCallback(T fun) { Remove(fun, m_remove); }
        void                RunRemoveMemberCallback(Caller* pCaller) { Run(pCaller, m_remove); }

protected:
        /**
            공용 함수
            동일 코드 최소화
        	
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
            // Run 중 컨테이너 변경(삭제)이
            // 일어날 수 있으므로 백업 실행
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