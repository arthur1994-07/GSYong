#include "../pch.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::DropOutAllSummon(DWORD dwGaeaID)
{
    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

	GLLandMan* const pLand(pChar->GetLandMan());
	if ( !pLand )
		return FALSE;

   /* SUMMONID_ITER SummonIter      = pChar->m_dwSummonGUIDList.end();
    SUMMONID_ITER SummonIterBegin = pChar->m_dwSummonGUIDList.begin();
    while (!pChar->m_dwSummonGUIDList.empty())
    {
        --SummonIter;
        if (pLand)
            pLand->DropOutSummon((*SummonIter));
        SummonIter = pChar->m_dwSummonGUIDList.end();
    }*/

	pChar->RemoveAllChildCrow();

    return TRUE;
}

BOOL GLGaeaServer::DropOutAllSummon(GLChar* pChar)
{
    if (!pChar)
        return FALSE;

	GLLandMan* const pLand(pChar->GetLandMan());
	if ( !pLand )
		return FALSE;

   /* SUMMONID_ITER SummonIter      = pChar->m_dwSummonGUIDList.end();
    SUMMONID_ITER SummonIterBegin = pChar->m_dwSummonGUIDList.begin();
    while (!pChar->m_dwSummonGUIDList.empty())
    {
        --SummonIter;
        if (pLand)
            pLand->DropOutSummon((*SummonIter));
        SummonIter = pChar->m_dwSummonGUIDList.end();
    }*/

	pChar->RemoveAllChildCrow();
    return TRUE;
}

BOOL GLGaeaServer::MsgReqSummonProc ( DWORD dwClientID
									, DWORD dwGaeaID
									, NET_MSG_GENERIC* nmg ) 
{
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
	{
		sc::writeLogError( "GLGaeaServer::MsgReqSummonProc - pChar is NULL." );
		return FALSE;
	}

	GLMSG::SNETPC_REQ_USE_SUMMON* pMsg =
		static_cast< GLMSG::SNETPC_REQ_USE_SUMMON* >( nmg );
	if ( pMsg )
	{
		// 소환 Summon;
		pChar->SummonCrow( pMsg->sSummonID, static_cast<DWORD>( pMsg->emTYPE ) );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgReqSummonEventSkillProc ( DWORD dwClientID
											  , DWORD dwGaeaID
											  , NET_MSG_GENERIC* nmg )
								
{
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
	{
		sc::writeLogError( "GLGaeaServer::MsgReqSummonEventSkillProc - pChar is NULL." );
		return FALSE;
	}

	GLMSG::SNETPC_REQ_USE_SUMMON_EVENT_SKILL_CF* pMsg =
		static_cast< GLMSG::SNETPC_REQ_USE_SUMMON_EVENT_SKILL_CF* >( nmg );
	if ( pMsg )
	{
		// 소환 Summon;
		pChar->UseSummonEventSkill( pMsg->iIdx, SNATIVEID( false ) );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgHireBasicSummonProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
	{
		sc::writeLogError( "GLGaeaServer::MsgHireBasicSummonProc - pChar is NULL." );
		return FALSE;
	}

	GLMSG::SNETPC_REQ_HIRE_BASICSUMMON_CF* pMsg =
		static_cast< GLMSG::SNETPC_REQ_HIRE_BASICSUMMON_CF* >( nmg );
	if ( pMsg )
	{
		// 소환수 고용;
		pChar->HireBasicSummon( pMsg->wMainID, pMsg->wSubID );
	}

	return TRUE;
}