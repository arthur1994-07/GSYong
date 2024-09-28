#include "../pch.h"

#include "./GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameActivity.h"
#include "../../RanLogic/GLUseFeatures.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL GLGaeaServer::MsgActivityDoneItemTake( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		sc::writeLogError(std::string("MsgActivityDoneItemTake() - Wrong use of Activity while it is OFF!"));
		return FALSE;
	}

	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgActivityDoneItemTake( nmg );
	return TRUE;	
}


BOOL GLGaeaServer::MsgActivityChangeBadge( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		sc::writeLogError(std::string("MsgActivityChangeBadge() - Wrong use of Activity while it is OFF!"));
		return FALSE;
	}

	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgActivityChangeBadge( nmg );
	return TRUE;	
}


//
//DbAction for ActivityByOrder has been completed and the DbAction sent a activity-complete message.
//  Field(DbAction) to Field
//
BOOL GLGaeaServer::MsgActivityCompletedFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		sc::writeLogError(std::string("MsgActivityCompletedFF() - Wrong use of Activity while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_ACTIVITY_COMPLETED_FF *msg = (GLMSG::SNET_ACTIVITY_COMPLETED_FF *)nmg;
		
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgActivityCompletedFF( nmg );
	return TRUE;
}


//
//DbAction for ActivityLoad has been completed and the DbAction sent a ActivityLoad-completed message.
//  Field(DbAction) to Field
//
//DF
BOOL GLGaeaServer::MsgActivityLoadCompletedFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		sc::writeLogError(std::string("MsgActivityLoadCompletedFF() - Wrong use of Activity while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_ACTIVITY_LOAD_COMPLETED_FF *pmsg = (GLMSG::SNET_ACTIVITY_LOAD_COMPLETED_FF *)nmg;
	
	GLChar* pChar = GetCharByDbNum( pmsg->nChaNum );
	
	if ( !pChar )
	{
		sc::writeLogError(std::string("MsgActivityLoadCompletedFF() - The character does not exist!"));
		return FALSE;
	}

	pChar->MsgActivityLoadCompletedFF( nmg );

	pChar->SendToClient( nmg );

	return TRUE;
	
}

