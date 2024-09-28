#include "../pch.h"
#include "../Club/GLClubMan.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgReqGetStorageSpecificItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqGetStorageSpecificItem( nmg );
	return TRUE;    
}


BOOL GLGaeaServer::MsgReqGetStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqGetStorage( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageDrug(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageDrug( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageSkill( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageToHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenExStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenExStorage( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg){
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenToStorage( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg){
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageToStorage( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageSaveMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->MsgReqStorageSaveMoney(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqStorageDrawMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->MsgReqStorageDrawMoney(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqStorageSplit(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageSplit( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqStorageCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageCard( nmg );
    return TRUE;    
}