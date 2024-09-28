#ifndef CServerTriggerFinger_H
#define CServerTriggerFinger_H

#include "../../RanLogic/TriggerSystem/TriggerFinger.h"
#include "ServerMapinfoMessage.h"

class GLLandMan;
class GLGaeaServer;

namespace TriggerSystem
{

/**
	���� �����Ϳ� ����� Ʈ���� �۵���    

 */
class CServerTriggerFinger
	: public CTriggerFinger
{
public:
        CServerTriggerFinger();

        /**
            Ʈ���� �ν��Ͻ��� �ܺο� ����� ���� �Ű��� ���� �Ѵ�.
            ���� �ڵ忡�� GLLandMan �� �ȴ�.

         */
        void                            SetLandMan(GLLandMan* pLandMan) { m_pLandMan = pLandMan; }
        GLLandMan*                      GetLandMan() { return m_pLandMan; }

        void                            SetGaeaServer(GLGaeaServer* pGaeaServer) { m_pGaeaServer = pGaeaServer; }
        GLGaeaServer*                   GetGaeaServer() { return m_pGaeaServer; }

        /**
            ���� ���� ���� Ʈ���ŵ��� �� ������ ���� �Ѵ�.
            �̴� Ʈ���Ÿ� ���� ���� �� �ִ�.

         */
        const SServerTriggerMapInfo&    GetMapInfo() { return m_mapinfo; }

        /**
        	���� �� ���������� ���� �Ѵ�.

         */
        void                            SetStage(int stage) { m_stage = stage; }
        int                             GetStage() { return m_stage; }

        /**
        	������ ������ ���� �Ѵ�.
            ������ ���̵��� ǥ���ϱ� ���� ��� �Ǿ���.
            ���ڰ� ���� ���� ���̵��� ������.

         */
        void                            SetLevel(int nLevel) { m_level = nLevel; }
        int                             GetLevel() { return m_level; }

        /**
        	Ʈ���Ÿ� �ߵ� ��Ų��.
            ������ ������(Set����) �����͸� �������� �ߵ� ��Ų��.
            
         */
        void                            Start();

        void                            SetGuid( const std::string& strGuid ){ m_strGuid = strGuid; }
        std::string                     GetGuid(){ return m_strGuid; }

protected:
        GLLandMan*                      m_pLandMan; //@ ���� ���� ������
        GLGaeaServer*                   m_pGaeaServer;
        SServerTriggerMapInfo           m_mapinfo;  //@ �� ������ ������.
        int                             m_stage;    //@ �� �������� ����
        int                             m_level;    //@ ����

        std::string                     m_strGuid;
};

} // end namespace

#endif