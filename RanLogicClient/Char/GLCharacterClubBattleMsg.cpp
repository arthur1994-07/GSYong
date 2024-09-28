#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtils.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../Club/GLClubClient.h"
#include "./GLCharacter.h"


#include "../../SigmaCore/DebugInclude.h"
/*
void GLCharacter::MsgProcessClubBattle( NET_MSG_GENERIC* nmg )
{
	switch (nmg->nType)
    {	
	//case NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB:
    //    MsgClubBattleReqFB(nmg);
	//	break;
	
	//case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK:
    //    MsgClubBattleReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT:
    //    MsgClubBattleBegin(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2:
    //    MsgClubBattleBegin2(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT:
    //    MsgClubBattleOver(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB:
    //    MsgClubBattleArmisticeReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK:
    //    MsgClubBattleArmisticeReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB:
    //    MsgClubBattleSubmissionReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE:
    //    MsgClubBattleKillUpdate(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE:
    //    MsgClubBattlePointUpdate(nmg);
	//	break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;
	}
}
*/
/*
void GLCharacter::MsgProcessAllianceBattle( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	//case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB:
    //    MsgClubAllianceBattleReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK:
    //    MsgClubAllianceBattleReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB:
    //    MsgClubAllianceBattleArmisticeReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK:
    //    MsgClubAllianceBattleArmisticeReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB:
    //    MsgClubAllianceBattleSubmissionReqFB(nmg);
	//	break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;
	}
}
*/