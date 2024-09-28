#ifndef CServerTriggerFinger_H
#define CServerTriggerFinger_H

#include "../../RanLogic/TriggerSystem/TriggerFinger.h"
#include "ServerMapinfoMessage.h"

class GLLandMan;
class GLGaeaServer;

namespace TriggerSystem
{

/**
	서버 데이터에 기반한 트리거 작동기    

 */
class CServerTriggerFinger
	: public CTriggerFinger
{
public:
        CServerTriggerFinger();

        /**
            트리거 인스턴스가 외부와 통신을 위한 매개를 설정 한다.
            서버 코드에서 GLLandMan 가 된다.

         */
        void                            SetLandMan(GLLandMan* pLandMan) { m_pLandMan = pLandMan; }
        GLLandMan*                      GetLandMan() { return m_pLandMan; }

        void                            SetGaeaServer(GLGaeaServer* pGaeaServer) { m_pGaeaServer = pGaeaServer; }
        GLGaeaServer*                   GetGaeaServer() { return m_pGaeaServer; }

        /**
            현재 실행 중인 트리거들의 맵 정보를 설정 한다.
            이는 트리거를 통해 받을 수 있다.

         */
        const SServerTriggerMapInfo&    GetMapInfo() { return m_mapinfo; }

        /**
        	진행 할 스테이지를 설정 한다.

         */
        void                            SetStage(int stage) { m_stage = stage; }
        int                             GetStage() { return m_stage; }

        /**
        	실행할 레벨을 설정 한다.
            레벨은 난이도를 표현하기 위해 고안 되었다.
            숫자가 높을 수록 난이도가 쎄진다.

         */
        void                            SetLevel(int nLevel) { m_level = nLevel; }
        int                             GetLevel() { return m_level; }

        /**
        	트리거를 발동 시킨다.
            위에서 설정한(Set으로) 데이터를 바탕으로 발동 시킨다.
            
         */
        void                            Start();

        void                            SetGuid( const std::string& strGuid ){ m_strGuid = strGuid; }
        std::string                     GetGuid(){ return m_strGuid; }

protected:
        GLLandMan*                      m_pLandMan; //@ 서버 지형 관리자
        GLGaeaServer*                   m_pGaeaServer;
        SServerTriggerMapInfo           m_mapinfo;  //@ 맵 정보를 가진다.
        int                             m_stage;    //@ 맵 스테이지 정보
        int                             m_level;    //@ 레벨

        std::string                     m_strGuid;
};

} // end namespace

#endif