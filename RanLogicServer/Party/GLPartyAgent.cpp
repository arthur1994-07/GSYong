#include "../pch.h"
#include "./GLPartyAgent.h"

#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../AgentServer/GLAgentServer.h"

#include "../MatchSystem/MatchingManager.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace MatchingSystem;

GLExpeditionAgent::GLExpeditionAgent(void)
{
	m_PreMember.reserve(MAXPARTY);
	m_PreMember.clear();
}
GLExpeditionAgent::~GLExpeditionAgent(void)
{
}
const bool GLExpeditionAgent::isPreMember(const DWORD dwGaeaID) const
{
	MEMBER_CITER iter = find ( m_PreMember.begin(), m_PreMember.end(), dwGaeaID );
	if ( iter != m_PreMember.end() )
		return true;

	return false;
}

const bool GLExpeditionAgent::addPreMember(const DWORD dwGaeaID)
{
	m_PreMember.push_back(dwGaeaID);    
	return true;
}
const bool GLExpeditionAgent::deletePreMember(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = find ( m_PreMember.begin(), m_PreMember.end(), dwGaeaID );
	if ( iter == m_PreMember.end() )
		return false;

	m_PreMember.erase(iter);
	return true;
}



GLPartyAgent::GLPartyAgent(void)
{
	m_Member.reserve(MAXPARTY);
	m_StartConfrontMember.reserve( MAXPARTY );
	m_CurrentConfrontMember.reserve( MAXPARTY );
	reset();
}

void GLPartyAgent::reset(void)
{
	m_Member.clear();
	m_PreMember.clear();

	m_fConfrontTimer = 0.0f;
	m_StartConfrontMember.clear();
	m_CurrentConfrontMember.clear();

	m_ConfrontTargetPartyID.Reset();
	m_PartyID.Reset();
}

const GLPartyID& GLPartyAgent::GetPartyID(void) const
{
	return m_PartyID;
}

const DWORD GLPartyAgent::getMaster(void) const 
{ 
	if ( m_Member.size() > 0 )
		return m_Member[MASTER_PARTY];

	return GAEAID_NULL;
}

const GLPartyAgent::MEMBER& GLPartyAgent::GetMember(void) const
{
	return m_Member;
}

const bool GLPartyAgent::isValidParty(void) const
{
	return ( m_Member.size() > 0 );
}

void GLPartyAgent::setMaster(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	m_PartyID = PartyID;
	if( m_Member.size() == 0 )
	{
		m_Member.push_back( dwGaeaID );
	}
	else
	{
		m_Member.push_back(m_Member[MASTER_PARTY]); // change master's pos
		m_Member[MASTER_PARTY] = dwGaeaID;
	}
}


// 마스터가 GAEAID_NULL인 경우 상당히 위험함;
const bool GLPartyAgent::changeMaster(GLAgentServer* const pServer, const DWORD dwNewMasterGaeaID)
{
	if ( pServer == 0 )
		return false;

	for( DWORD _i = 0; _i < m_Member.size(); ++_i )
	{
		if( m_Member[_i] != dwNewMasterGaeaID )
			continue;

		const DWORD dwOldMasterGaeaID(m_Member[MASTER_PARTY]);
		m_Member[MASTER_PARTY] = dwNewMasterGaeaID;
		m_Member[_i] = dwOldMasterGaeaID;

//        RunChangeMasterCallback(this);

		MatchingCore::getinstance()->PartyMasterChaged(dwOldMasterGaeaID,m_Member[MASTER_PARTY],GetPartyID().GetPartyID());	

		return true;
	} //for;

	return false;
}

const DWORD GLPartyAgent::changeMaster(GLAgentServer* const pServer)
{
	if ( pServer == 0 )
		return false;

	if ( m_Member[MASTER_PARTY] == GAEAID_NULL )
		return GAEAID_NULL;

	for ( DWORD _i = 1; _i < m_Member.size(); ++_i )
	{
		if ( m_Member[_i] == GAEAID_NULL )
			continue;

		const DWORD dwOldMaster = m_Member[MASTER_PARTY];
		m_Member[MASTER_PARTY] = m_Member[_i];
		m_Member[_i] = dwOldMaster;

        RunChangeMasterCallback(this);

		MatchingCore::getinstance()->PartyMasterChaged(dwOldMaster,m_Member[MASTER_PARTY],GetPartyID().GetPartyID());	
		
		return m_Member[MASTER_PARTY];
	}


	return GAEAID_NULL;
}

const bool GLPartyAgent::isMaster(const DWORD dwGaeaID) const
{
	if ( m_Member.size() == 0 )
		return FALSE;

	return ( m_Member[MASTER_PARTY] == dwGaeaID );
}

const bool GLPartyAgent::isFull(void) const
{
	return (m_Member.size() == MAXPARTY);
}

const bool GLPartyAgent::isPartyMember(const DWORD dwGaeaID) const
{
	MEMBER_CITER iter = find ( m_Member.begin(), m_Member.end(), dwGaeaID );
	if ( iter != m_Member.end() )
		return true;

	return false;
}

const DWORD GLPartyAgent::getNPartyMember(void) const
{
	return (DWORD) m_Member.size();
}

const DWORD GLPartyAgent::addPartyMember(GLAgentServer* const pServer, const DWORD dwGaeaID)
{
	if ( isFull() == true )
		return DEFSLOT_NULL;

	if ( isPartyMember(dwGaeaID) == true )
		return DEFSLOT_NULL;

	//  게시판에 등록된게 있으면 제거 하자
	
	GLCharAG* const pChar = pServer->GetChar(dwGaeaID);
	if ( pChar == NULL )
		return DEFSLOT_NULL;

	pServer->MapDeletePartyInfoHelper(pChar->m_szName);
	//////////////////////////////////////////////////////////////////////////
	
	m_Member.push_back(dwGaeaID);

    RunAddMemberCallback(this);

	MatchingCore::getinstance()->PartyMemberChaged(dwGaeaID,GetPartyID().GetPartyID());

	return (m_Member.size() - 1);
}

const bool GLPartyAgent::removePartyMember(GLAgentServer* const pServer, const DWORD dwGaeaID)
{

	

	MEMBER_ITER iter = find( m_Member.begin(), m_Member.end(), dwGaeaID );
	if( iter == m_Member.end() )
		return false;

	m_Member.erase(iter);

	//	Note : 대련 참가자에서 삭제;
	//
	removeConfrontMember(dwGaeaID);

	MatchingCore::getinstance()->PartyMemberRemoved(dwGaeaID,GetPartyID().GetPartyID());

	return true;
}

const bool GLPartyAgent::isPreMember(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = find ( m_PreMember.begin(), m_PreMember.end(), dwGaeaID );
	if ( iter!=m_PreMember.end() )
		return true;

	return false;
}

const DWORD GLPartyAgent::addPreMember ( DWORD dwGaeaID )
{
	m_PreMember.push_back(dwGaeaID);
	return m_PreMember.size() - 1;
}

const bool GLPartyAgent::removePreMember(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = find(m_PreMember.begin(), m_PreMember.end(), dwGaeaID);
	if ( iter == m_PreMember.end() )
		return false;

	m_PreMember.erase(iter);

	return true;
}

void GLPartyAgent::addConfrontMember(const DWORD dwGaeaID)
{
	if ( isPartyMember(dwGaeaID) == false )
		return;

	m_CurrentConfrontMember.push_back (dwGaeaID);

	return;
}

void GLPartyAgent::removeConfrontMember(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = find ( m_CurrentConfrontMember.begin(), m_CurrentConfrontMember.end(), dwGaeaID );
	if ( iter!=m_CurrentConfrontMember.end() )
		m_CurrentConfrontMember.erase(iter);
}

void GLPartyAgent::refreshConfrontMember(void)
{
	m_StartConfrontMember = m_CurrentConfrontMember;
}

const bool GLPartyAgent::isConfronting(void) const
{
	return m_ConfrontTargetPartyID.isValidParty();
}

const bool GLPartyAgent::isLoseConfront(void) const
{
	return m_CurrentConfrontMember.empty();
}

const DWORD GLPartyAgent::getNConfrontMember(void) const
{
	return (DWORD)m_CurrentConfrontMember.size();
}

void GLPartyAgent::resetConfront(GLAgentServer* pServer)
{
	m_fConfrontTimer = 0.0f;

	m_StartConfrontMember.clear();
	m_CurrentConfrontMember.clear();

	m_ConfrontOption.RESET();
	m_ConfrontTargetPartyID.Reset();


	for( DWORD i = 0; i < m_Member.size(); i++ )
	{
		GLCharAG* pChar = pServer->GetChar( m_Member[i] );
		if (pChar)
            pChar->m_sCONFTING.RESET();
	} //for
}

WORD GLPartyAgent::GetMASTER_SCHOOL(GLAgentServer* pServer)
{
	GLCharAG* pMaster = pServer->GetChar( m_Member[MASTER_PARTY] );
	if (!pMaster)
        return 0;
	return pMaster->m_wSchool;
}

const MapID GLPartyAgent::GetMASTER_MAP(GLAgentServer* pServer)
{
	GLCharAG* pMaster = pServer->GetChar( m_Member[MASTER_PARTY] );
	if (!pMaster)
        return MapID();
    else
	    return pMaster->GetCurrentMap();
}

bool GLPartyAgent::IsSAME_SCHOOL(GLAgentServer* pServer)
{
	GLCharAG* pMaster = pServer->GetChar( m_Member[MASTER_PARTY] );
	if (!pMaster)
        return false;

	GLPartyAgent::MEMBER_ITER iter = m_Member.begin();
	GLPartyAgent::MEMBER_ITER iter_end = m_Member.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const DWORD dwGaeaID = (*iter);
		GLCharAG* pChar = pServer->GetChar(dwGaeaID);
		if (pChar)
		{
			if (pChar->m_wSchool != pMaster->m_wSchool)
                return false;
		}
	}
	return true;
}

const bool GLPartyAgent::IsLevelPartyMember(GLAgentServer* const pServer, const WORD wMin, const WORD wMax)
{
	if (pServer == 0)
	{
		return false;
	}

    for( DWORD _i = 0; _i < m_Member.size(); ++_i )
    {
        GLCharAG* pMaster = pServer->GetChar( m_Member[_i] );

        if (pMaster == 0)
        {
			return false;
		}

        const WORD wTempLevel = pMaster->GetChaLevel();

        if (wTempLevel < wMin)
        {
            return false;
        }

		if (wTempLevel > wMax)
		{
			return false;
		}
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

// GLPARTY_GROUP::GLPARTY_GROUP()
// {
// 	Reset();
// } //GLPARTY_GROUP::GLPARTY_GROUP
// 
// void GLPARTY_GROUP::Reset()
// {
// 	wId = INVALID_ID;
// 	for( int i = 0; i < MAX_PARTYGROUP; i++ )
// 	{
// 		wPartyIds[i] = INVALID_ID;
// 	} //for
// } //GLPARTY_GROUP::Reset
// 
// bool GLPARTY_GROUP::IsEmpty()
// { 
// 	for( WORD i = 0; i < MAX_PARTYGROUP; i++ )
// 	{
// 		if( wPartyIds[i] != INVALID_ID )
// 			return false;
// 	} //for
// 	return true;
// } //GLPARTY_GROUP::IsEmpty
// 
// bool GLPARTY_GROUP::AddParty( GLPARTY* pParty )
// {
// 	for( BYTE i = 0; i < MAX_PARTYGROUP; i++ )
// 	{
// 		if( wPartyIds[i] == INVALID_ID )
// 		{
// 			pParty->m_sOPTION.nGroupIndex = i;
// 			wPartyIds[pParty->m_sOPTION.nGroupIndex] = pParty->m_id.wParty;
// 			return true;
// 		} //if
// 	} //for
// 
// 	return false;
// } //GLPARTY_GROUP::AddParty
// 
// void GLPARTY_GROUP::DeleteParty( GLPARTY* pParty )
// {
// 	GASSERT( pParty->m_sOPTION.nGroupIndex < MAX_PARTYGROUP );
// 	if( wPartyIds[pParty->m_sOPTION.nGroupIndex] != pParty->m_id.wParty )
// 		return;
// 
// 	wPartyIds[pParty->m_sOPTION.nGroupIndex] = INVALID_ID;
// } //GLPARTY_GROUP::DeleteParty
