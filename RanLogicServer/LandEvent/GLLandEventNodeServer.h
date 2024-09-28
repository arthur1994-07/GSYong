
#pragma once

#include "../../EngineLib/G-Logic/GLQuadTree.h"
#include "../../RanLogic/LandEvent/GLLandEventNode.h"

//-------------------------------------------------------------------//
class GLGaeaServer;
class GLChar;

namespace LANDEVENTNODE_PROCESS_FUNCTOR
{
    class CEVENTREMOVE
    {
    public :
        CEVENTREMOVE(GLGaeaServer* pGaeaServer, DWORD dwGUID);

    public :
        void operator() (GLGaeaServer* pGaeaServer, GLChar* pChar);

    public :
        GLGaeaServer*    m_pGaeaServer;
        DWORD            m_dwGUID;
    };
};

//-------------------------------------------------------------------//

class GLGaeaServer;
class GLLandNode;
class GLLandMan;
typedef SQUADNODE<GLLandNode> LANDQUADNODE;

class CEVENT_NODE_SERVER : public CEVENT_NODE
{

public :
    GLGaeaServer*                       m_pGaeaServer;
    GLLandMan*                          m_pLandMan;
    LANDQUADNODE*	                    m_pQuadNode; // ���� Ʈ�� ���.
    sc::SGLNODE<CEVENT_NODE_SERVER*>*	m_pCellList; // �� ����Ʈ ���. 

public :
    CEVENT_NODE_SERVER()
        : m_pGaeaServer(NULL)
        , m_pLandMan(NULL)
        , m_pQuadNode(NULL)
        , m_pCellList(NULL)
       {
       }

       virtual ~CEVENT_NODE_SERVER()
       {
       }

public :
    virtual void ReqNetMsg_Drop(GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE& Msg) = 0;

public :
    // �ֺ� �������� ���μ��� ó��
    void ProcessViewAroundMsg(NET_MSG_GENERIC* pMsg, EMCROW emCrow, DWORD ObjectId) const;

    // Note : �ֺ� Char���� ��ϰ� ���ÿ� ReqNetMsg_Drop �޼����� ������.
    void RegisterViewAroundPC(GLGaeaServer* pGLGaeaServer);

public :
    virtual EMEVENTNODETYPE GetType() const override = 0;

};
typedef CEVENT_NODE_SERVER*                 PLANDEVENTNODESERVER;
typedef sc::CGLLIST<PLANDEVENTNODESERVER>	LANDEVENTNODESERVERLIST;
typedef sc::SGLNODE<PLANDEVENTNODESERVER>	LANDEVENTNODESERVERNODE;

//-------------------------------------------------------------------//

class CEVENT_NODE_SERVER_SUMMONGATE : public CEVENT_NODE_SERVER
{

public :

    SNATIVEID        m_sSkillID;            // �ش� ��ų ����
    WORD             m_wLevel;              // �ش� ��ų ����
    SNATIVEID        m_sSummonNativeID;     // �ش� ��ȯ���� Mob ID
    FLOAT            m_fSummonRemainedTime; // �ش� ��ȯ���� ���� �ð�
    DWORD            m_dwSummonMax;         // �ش� ��ȯ���� �ִ� ����
	DWORD			m_dwType;				// Ÿ��;
	bool			m_bEventSkill;			// �̺�Ʈ ��ų ���� ����;

    float		     m_fOwnerIncHP;		    // ��������  HP ȸ����
    WORD		     m_wOwnerAttack;        // ��������  ���ݷ�
    WORD		     m_wOwnerDefense;       // ��������  ����
    BYTE             m_cOwnerWeaponsGrade;  // �������� ���� ��ȭ
    ITEM::SSATE_BLOW m_sOwnerBlow;          // �������� ���� �Ӽ�
    WORD             m_wOwnerSchool;        // �������� �п�

    STARGETID        m_sEvent;              // �̺�Ʈ ����
    STARGETID        m_sTarget;             // Ÿ�� ����

	SKILL::GLSKILL_ATT	 m_emNeedItemAtt;		// �ش� ��ȯ�� ��ȯ�� �ʿ� �������� ����

public :
    CEVENT_NODE_SERVER_SUMMONGATE()
        : m_wLevel(0)
        , m_fSummonRemainedTime(0.0f)
        , m_dwSummonMax(0)
		, m_dwType(0)
		, m_bEventSkill(false)
        , m_fOwnerIncHP(0.0f)
        , m_wOwnerAttack(0)
        , m_wOwnerDefense(0)
        , m_cOwnerWeaponsGrade(0)
        , m_sOwnerBlow()
        , m_wOwnerSchool(0)
		, m_emNeedItemAtt( SKILL::SKILLATT_NOTHING )
       {
       }

protected :
    virtual void Restore();
    virtual void Clean();

protected :
    virtual void FrameStart();
    virtual void FrameEvent();
    virtual void FrameEnd();

public :
    virtual void ReqNetMsg_Drop(GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE& Msg) override;

public :
    virtual EMEVENTNODETYPE GetType() const override { return EMEVENTNODETYPE_SUMMONGATE; }

};

//-------------------------------------------------------------------//