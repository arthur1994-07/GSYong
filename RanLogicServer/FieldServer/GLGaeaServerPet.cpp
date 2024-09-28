#include "../pch.h"
#include "../Club/GLClubMan.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL GLGaeaServer::MsgPetGoto( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetGoto ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetStop( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// PetStop에서 서버가 죽는 현상이 발생하여 임시로 막아둔다;
	return TRUE;

    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetStop ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetUpdateMoveState( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetUpdateMoveState ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetChangeActiveSkill( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetChangeActiveSkill ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetRename( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetRename ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::PetRenameFeedBack( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->PetRenameFeedBack ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetChangeColor( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetChangeColor ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetChangeStyle( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetChangeStyle ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetAccInvenExSlot( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetAccInvenExSlot( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetAccInvenToSlot( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetAccInvenToSlot( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetAccSlotToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetAccSlotToInven( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetLearnSkill( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetLearnSkill ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetSkillSlotExpansion( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetSkillSlotExpansion ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetFunnyReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetFunnyReq ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetRemoveSlotItem( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetRemoveSlotItem ( nmg );
        }
    }
    return TRUE;
}

BOOL GLGaeaServer::MsgPetSkinPackItem( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLChar* pOwner = GetChar ( dwGaeaID );
    if ( pOwner )
    {
        PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
        if ( pPet )
        {
            pPet->MsgPetSkinPackItem ( nmg );
        }
    }
    return TRUE;
}


BOOL GLGaeaServer::MsgGiveFood(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgGiveFood( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgGetPetFullFromDB (DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgGetPetFullFromDB( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPetReviveInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPetReviveInfo( nmg );
    return TRUE;    
}

GLPetField* GLGaeaServer::GetPET(DWORD dwGUID)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_Pet2);
    PET_FIELD_MAP_ITER iter = m_PetFieldMap.find(dwGUID);
    if (iter != m_PetFieldMap.end())
        return iter->second;
    else
        return NULL;
}

void GLGaeaServer::RegistPet(DWORD GaeaId, GLPetField* pPet)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_Pet2);
    PET_FIELD_MAP_ITER iter = m_PetFieldMap.find(GaeaId);
    if (iter != m_PetFieldMap.end())
    {
        //sc::writeLogWarn(sc::string::format("RegistPet found same id pet(%1%).", GaeaId));
        m_PetFieldMap.erase(iter);
    }
    m_PetFieldMap.insert(PET_FIELD_MAP_VALUE(GaeaId, pPet));
}

void GLGaeaServer::UnRegistPet(DWORD GaeaId)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_Pet2);
    PET_FIELD_MAP_ITER iter = m_PetFieldMap.find(GaeaId);
    if (iter != m_PetFieldMap.end())
        m_PetFieldMap.erase(iter);
}