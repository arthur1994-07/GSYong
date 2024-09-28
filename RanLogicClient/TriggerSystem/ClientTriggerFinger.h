#ifndef CClientTriggerFinger_H
#define CClientTriggerFinger_H

#include "../../RanLogic/TriggerSystem/TriggerFinger.h"

class GLGaeaClient;

namespace TriggerSystem
{

/**
	클라이언트 데이터에 기반한 트리거 작동기	
	
 */
class CClientTriggerFinger
	: public CTriggerFinger
{
public:
        CClientTriggerFinger();
        ~CClientTriggerFinger();

        /**
        	트리거 인스턴스가 외부와 통신을 위한 매개를 설정 한다.
            클라이언트 코드에서 GLGaeaClient 가 된다.

         */
        void            SetGaeaClient(GLGaeaClient* pGaeaClient) { m_pGaeaClient = pGaeaClient; }
        GLGaeaClient*   GetGaeaClient() { return m_pGaeaClient; }

        /**
        	진행 할 스테이지를 설정 한다.

         */
        void            SetStage(int stage) { m_stage = stage; }
        int             GetStage() { return m_stage; }

        /**
        	실행할 레벨을 설정 한다.
            레벨은 난이도를 표현하기 위해 고안 되었다.
            숫자가 높을 수록 난이도가 쎄진다.

         */
        void            SetLevel(int nLevel) { m_level = nLevel; }
        int             GetLevel() { return m_level; }

        /**
            트리거를 발동 시킨다.
            위에서 설정한(Set으로) 데이터를 바탕으로 발동 시킨다.

         */
        void            Start();

protected:
        GLGaeaClient*   m_pGaeaClient;  //@ 클라이언트 전체 로직 관리자
        int             m_stage;        //@ 맵 스테이지 정보
        int             m_level;        //@ 레벨
};

} // end namespace

#endif