#include "../pch.h"
#include "../Club/GLClubMan.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL GLGaeaServer::MsgReqNpcQuestStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqNpcQuestStart( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqNpcQuestStartWithoutNPC(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;

	if(pChar->UserLevel() < USER_GM3)
		return FALSE;

    pChar->MsgReqNpcQuestStartWithoutNPC( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqNpcQuestTalk(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqNpcQuestTalk( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqQuestGiveUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqQuestGiveUp( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqQuestReadReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqQuestReadReset( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqQuestComplete(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqQuestComplete( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqRnattendanceQuestStart( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqRnattendanceQuestStart( nmg );
	return TRUE;  
}
