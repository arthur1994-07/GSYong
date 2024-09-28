#include "../pch.h"
#include "../Club/GLClubMan.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL GLGaeaServer::MsgReqLearnSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqLearnSkill( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSkillUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkillUp( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqRnSkillUp( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqRnSkillUp( nmg );
    return TRUE;   
}

BOOL GLGaeaServer::MsgReqSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkill( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSkillHoldReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkillHoldReset( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSkillCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkillCancel( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSkillQSetActive(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;

    GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE* pPacket = (GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE*) nmg;    
    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        unsigned int Crc32 = sc::string::getCrc32(pPacket, sizeof(GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE)-sizeof(unsigned int));
        if (pPacket->Crc32 == Crc32)
        {
            pChar->MsgReqSkillQSetActive(pPacket->wSLOT);
            return TRUE;
        }
        else
        {
            sc::writeLogError(
                sc::string::format(
                    "SNETPC_REQ_SKILLQUICK_ACTIVE ChaDbNum %1% Crc %2%/%3%",
                    pChar->CharDbNum(),
                    pPacket->Crc32,
                    Crc32));
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqSkillQSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkillQSet( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSkillQReSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSkillQReSet( nmg );
    return TRUE;    
}