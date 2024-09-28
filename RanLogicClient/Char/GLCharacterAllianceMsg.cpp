#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../Club/GLClubClient.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
/*
void GLCharacter::MsgProcessAlliance( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	//case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB:
    //    MsgClubAllianceReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB:
    //    MsgClubAllianceDelFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB:
    //    MsgClubAllianceSecFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB:
    //    MsgClubAllianceDisFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK:
    //    MsgClubAllianceReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT:
    //    MsgClubAllianceAdd(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT:
    //    MsgClubAllianceDel(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT:
    //    MsgClubAllianceDis(nmg);
	//	break;	

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;
	};
}
*/