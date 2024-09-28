#include "../pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "./GLCrowClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCrowClient::MsgProcess(NET_MSG_GENERIC* pMsg)
{
    switch (pMsg->Type())
    {
    case NET_MSG_CROW_MOVETO:
        MsgCrowMoveTo(pMsg);
        break;

    case NET_MSG_CROW_ATTACK:
        MsgCrowAttack(pMsg);
        break;

    case NET_MSG_CROW_SKILL:
        MsgCrowSkill(pMsg);
        break;

	case NET_MSG_CROW_SKILL_DEALY_ACTION:
		MsgCrowSkillDealyAction(pMsg);
		break;

	case NET_MSG_CROW_END_SKILL:
		MsgCrowEndSkill(pMsg);
		break;

    case NET_MSG_CROW_UPDATE_STATE:
        MsgCrowUpdateState(pMsg);
        break;

    case NET_MSG_GCTRL_CROW_ATTACK_BLOCKING:
        MsgCrowAttackBlocking(pMsg);
        break;

    case NET_MSG_CROW_AVOID:
        MsgCrowAvoid(pMsg);
        break;

    case NET_MSG_CROW_DAMAGE:
        MsgCrowDamage(pMsg);
        break;

    case NET_MSG_GCTRL_ACTION_BRD:
        MsgActionBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLFACT_BRD:
        MsgSkillFactBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLHOLD_BRD:
        MsgSkillHoldBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
        MsgSkillHoldRsBrd(pMsg);
        break;

    case NET_MSG_GCTRL_STATEBLOW_BRD:
        MsgStateBlowBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
        MsgCureStateBlowBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PUSHPULL_BRD:
        MsgPushPullBrd(pMsg);
        break;

    default:
        CDebugSet::ToListView ( "GLCrowClient::MsgProcess unknown message type %d/%d", pMsg->nType, pMsg->nType-NET_MSG_GCTRL );
        break;
    };
}
