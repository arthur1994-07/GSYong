
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
    LANDQUADNODE*	                    m_pQuadNode; // 쿼드 트리 노드.
    sc::SGLNODE<CEVENT_NODE_SERVER*>*	m_pCellList; // 셀 리스트 노드. 

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
    // 주변 유저에게 프로세스 처리
    void ProcessViewAroundMsg(NET_MSG_GENERIC* pMsg, EMCROW emCrow, DWORD ObjectId) const;

    // Note : 주변 Char에게 등록과 동시에 ReqNetMsg_Drop 메세지를 보낸다.
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

    SNATIVEID        m_sSkillID;            // 해당 스킬 정보
    WORD             m_wLevel;              // 해당 스킬 레벨
    SNATIVEID        m_sSummonNativeID;     // 해당 소환수의 Mob ID
    FLOAT            m_fSummonRemainedTime; // 해당 소환수의 유지 시간
    DWORD            m_dwSummonMax;         // 해당 소환수의 최대 갯수
	DWORD			m_dwType;				// 타입;
	bool			m_bEventSkill;			// 이벤트 스킬 가능 여부;

    float		     m_fOwnerIncHP;		    // 시전자의  HP 회복율
    WORD		     m_wOwnerAttack;        // 시전자의  공격력
    WORD		     m_wOwnerDefense;       // 시전자의  방어력
    BYTE             m_cOwnerWeaponsGrade;  // 시전자의 무기 강화
    ITEM::SSATE_BLOW m_sOwnerBlow;          // 시전자의 무기 속성
    WORD             m_wOwnerSchool;        // 시전자의 학원

    STARGETID        m_sEvent;              // 이벤트 정보
    STARGETID        m_sTarget;             // 타겟 정보

	SKILL::GLSKILL_ATT	 m_emNeedItemAtt;		// 해당 소환수 소환시 필요 장착무기 종류

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