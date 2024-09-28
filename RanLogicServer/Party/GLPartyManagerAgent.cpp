#include "../pch.h"

#include "../Character/GLCharAG.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Database/DbAction/DbActionLog.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLPartyManagerAgent.h"

#include "../../SigmaCore/String/StringFormat.h"

#include "../../SigmaCore/DebugInclude.h"


GLPartyManagerAgent::GLPartyManagerAgent(GLAgentServer* const pServer, const DWORD dwMaxClient)
: m_pAgentServer(pServer)
, m_nMaxClient((WORD)dwMaxClient)
{
    for( WORD _i = m_nMaxClient - 1; _i; --_i )
	{
        m_FreePartyID.push_back( _i );
		m_FreeExpeditionID.push_back(_i);
	}
}

GLPartyManagerAgent::~GLPartyManagerAgent(void)
{
    for (PARTY_MAP_ITER iter=m_Party.begin(); iter!=m_Party.end(); ++iter)
    {        
        m_PartyPool.destroy(iter->second); //SAFE_DELETE(iter->second); // delete GLPARTY
    }
    m_Party.clear();
}

const bool GLPartyManagerAgent::removePartyMember(const GLPartyID& PartyID, const DWORD dwGaeaID, BYTE eType)
{
	if ( GLPartyID::INVALID == PartyID.GetPartyID() )
	{
		// ��Ƽ�� ���� ĳ���͵� ������ ������ ��
		// SendErrorMsg(PartyID, dwGaeaID, EMPARTY_ERROR_NOPARTY, "removePartyMember()"); �� ���� 
		// �����޽��� ��µǴ� �κ� ����
		return false;
	}
	if ( dwGaeaID == GAEAID_NULL )
		return false;

	if ( PartyID.isValidParty() == false )
		return true;

	GLPartyAgent* pParty = GetParty(PartyID);
	if ( pParty == NULL )
	{
		SendErrorMsg(PartyID, dwGaeaID, EMPARTY_ERROR_NOPARTY, "removePartyMember()");
		return false;
	}

	if ( pParty->isPartyMember(dwGaeaID) == false )
		return false;
	
	GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);

	const bool bExpedition = (pExpedition == NULL) ? false : true;
	const DWORD nCompare = bExpedition ? 1 : 2;	

	GLPartyID rePartyID = PartyID; // �������� ��� �ڸ� �̵����� ���� ��Ƽ ID ������ ���� �� ����;
	if ( bExpedition == true )
	{
		DWORD nMember = 0;
		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			GLPartyAgent* const pExpeditionParty = GetParty(pExpedition->GetPartyID(_i - 1));
			if ( pExpeditionParty == NULL )
			{
				SendErrorMsg(PartyID, dwGaeaID, EMPARTY_ERROR_NOEXPEDITION, "removePartyMember");
				DeleteExpedition(PartyID);
				return false;
			}

			nMember += pExpeditionParty->getNPartyMember();
		}

		if ( nMember < 3 )
		{
			DeleteExpedition(PartyID);
			return true;
		}

		if ( pExpedition->isMaster(dwGaeaID) == true )
		{
			const DWORD dwNewMasterGaeaID = pParty->changeMaster(m_pAgentServer);
			if ( dwNewMasterGaeaID == GAEAID_NULL )
			{
				bool bComplete = false;
				for ( DWORD _i = 1; _i < MAX_PARTYGROUP; ++_i )
				{
					const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i);
					GLPartyAgent* const pExpeditionParty = GetParty(ExpeditionPartyID);
					if ( pExpeditionParty == NULL )
					{
						DeleteExpedition(PartyID);
						return false;
					}

					const DWORD nMember = pExpeditionParty->getNPartyMember();
					if ( nMember == 0 )
						continue;

					const DWORD dwRenewMasterGaeaID = pExpeditionParty->getMaster();

					const GLSwapSlotInformation CurrentMaster(dwGaeaID, MASTER_PARTY);
					const GLSwapSlotInformation NewMaster(dwRenewMasterGaeaID, _i);
					if ( SwapSlot(PartyID, CurrentMaster, NewMaster) == true )
					{
						rePartyID = ExpeditionPartyID;
						bComplete = true;
					}
					break;
				}

				if ( bComplete == false )
				{
					DeleteExpedition(PartyID);
					return true;
				}				
			}
			else
			{
				pExpedition->SetMasterGaeaID(dwNewMasterGaeaID, MASTER_PARTY);

				GLMSG::SNET_PARTY_AUTHORITY NetMsgAuth(PartyID, dwNewMasterGaeaID, dwGaeaID);			
				m_pAgentServer->SENDTOALLCHANNEL(&NetMsgAuth);
			}
		}
		else if ( pExpedition->isSubMaster(dwGaeaID) == true )
		{
			const DWORD dwNewMasterGaeaID = pParty->changeMaster(m_pAgentServer);			
			if ( dwNewMasterGaeaID != GAEAID_NULL )
			{
				const DWORD dwPartyIndex = pExpedition->GetPartySlotIndex(PartyID);

				const GLSwapSlotInformation CurrentMaster(dwGaeaID, dwPartyIndex);
				const GLSwapSlotInformation NewMaster(dwNewMasterGaeaID, dwPartyIndex);
				if ( SwapSlot(PartyID, CurrentMaster, NewMaster) == false )
				{
					DeleteExpedition(PartyID);
					return true;
				}

				pExpedition->SetMasterGaeaID(dwNewMasterGaeaID, dwPartyIndex);
			}
		}
	}
	else
	{
		if ( pParty->isMaster(dwGaeaID) == true )
		{ 
			const DWORD dwNewMasterGaeaID = pParty->changeMaster(m_pAgentServer);
			if ( dwNewMasterGaeaID != GAEAID_NULL && pParty->getNPartyMember() > 2 )
			{ // ������ ����� ��Ƽ���̰� ���� ��Ƽ���� 3�� �̻��̸� ��Ƽ�� ��ü;
				GLMSG::SNET_PARTY_AUTHORITY NetMsgAuth(PartyID, dwNewMasterGaeaID, dwGaeaID);			
				m_pAgentServer->SENDTOALLCHANNEL(&NetMsgAuth);
			}
			else
			{ // �ƴϸ� ��Ƽ ����;
				DeleteParty(PartyID);
				return true;
			}
		}
	}

	GLCharAG* const pChar = m_pAgentServer->GetCharByGaeaID(dwGaeaID);
	if ( pChar != NULL )
	{
		pChar->ResetPartyID();	
		pChar->ResetPartyOption();

		m_pAgentServer->QuitPartyMap( pChar );

        // ������ �ڵ带 �������� �Ű�����
        // �ٺ����� ������ �����ؾ��Ѵ�.

        GLMSG::SNET_PARTY_DEL NetMsgDel(rePartyID, dwGaeaID, pChar->ChaDbNum(), pChar->GetCurrentMap().getGaeaMapID(), static_cast<EMPARTY_ACTION_TYPE>(eType));
        SendMsgToMember(rePartyID, &NetMsgDel);	
        m_pAgentServer->SENDTOALLCHANNEL(&NetMsgDel);
	}


	GLPartyAgent* const pReParty = GetParty(rePartyID);
	if ( pReParty != NULL )
		pReParty->removePartyMember(m_pAgentServer, dwGaeaID);
	_refreshExpeditionMaster(rePartyID);	

	return true;
}

const bool GLPartyManagerAgent::DeleteConfrontMember(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	GLPartyAgent* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	pParty->removeConfrontMember(dwGaeaID);
	return true;
}

const bool GLPartyManagerAgent::removePreMember(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	GLPartyAgent* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	const bool bResult = pParty->removePreMember(dwGaeaID);
	return bResult;
}

const DWORD GLPartyManagerAgent::AddPartyMember(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	GLPartyAgent* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return DEFSLOT_NULL;
	
	if ( pParty->isFull() == TRUE )
		return DEFSLOT_NULL;

	const DWORD nSlotIndex = pParty->addPartyMember(m_pAgentServer, dwGaeaID);
	return nSlotIndex;
}

void GLPartyManagerAgent::AddGameDbJob(db::DbActionPtr spJob)
{
    m_pAgentServer->AddGameDbJob(spJob);
}

void GLPartyManagerAgent::AddGameAdoJob(db::DbActionPtr spJob)
{
    m_pAgentServer->AddGameAdoJob(spJob);
}


const EMPARTY_ERROR_TYPE GLPartyManagerAgent::GetPartyMemberGaeaIDs(const GLPartyID& PartyID, GLPartyMemberGaeaIDs& MemberGaeaIDs)
{
	const bool bExpedition = PartyID.isValidExpedition();
	const DWORD nCount = bExpedition ? MAX_PARTYGROUP : 1;

	GLPartyID PartyIDs[MAX_PARTYGROUP];
	if ( bExpedition == true )
	{
		GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return EMPARTY_ERROR_NOEXPEDITION;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
			PartyIDs[_i - 1] = pExpedition->GetPartyID(_i - 1);
	}
	else
		PartyIDs[0] = PartyID;

	DWORD nMemberCount = 0;
	for ( DWORD _i = nCount; _i; --_i )
	{
		GLPartyAgent* pParty = GetParty(PartyIDs[_i - 1]);
		if ( pParty == NULL )
		{
			if ( bExpedition == true )
			{
				DeleteExpedition(PartyID);
				return EMPARTY_ERROR_NOEXPEDITION;
			}
			return EMPARTY_ERROR_NOPARTY;
		}
		
		for ( GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin(); iter != pParty->m_Member.end(); iter++ )
			MemberGaeaIDs.dwGaeaIDs[nMemberCount++] = *iter;
	}
	MemberGaeaIDs.nMember = nMemberCount;
	
	return EMPARTY_ERROR_OK;
}
GLPartyAgent* const GLPartyManagerAgent::GetParty(const GLPartyID& PartyID) const
{
    PARTY_MAP_CITER iter = m_Party.find( PartyID.GetPartyID() );
    if (iter != m_Party.end())
        return iter->second;

	return NULL;
}

const GLExpeditionAgent* const GLPartyManagerAgent::GetConstExpedition(const GLPartyID& PartyID) const
{
	EXPEDITION_MAP_CITER iter = m_Expedition.find(PartyID.GetExpeditionID());
	if ( iter != m_Expedition.end() )
		return iter->second;

	return NULL;
}
const GLPartyAgent* const GLPartyManagerAgent::GetConstParty(const GLPartyID& PartyID) const
{
	PARTY_MAP_CITER iter = m_Party.find(PartyID.GetPartyID());
	if (iter != m_Party.end())
		return iter->second;

	return NULL;
}

GLExpeditionAgent* const GLPartyManagerAgent::GetExpedition(const GLPartyID& PartyID) const
{
	EXPEDITION_MAP_CITER iter = m_Expedition.find(PartyID.GetExpeditionID() );
	if (iter != m_Expedition.end())
		return iter->second;

	return NULL;
}

// GaeaID�� ��ȿ�ϴٰ� ������.
GLPartyAgent* GLPartyManagerAgent::NewParty(const DWORD dwGaeaID)
{
	GLCharAG* pChar = m_pAgentServer->GetChar(dwGaeaID);
	if ( !pChar )
        return NULL;

	if ( m_FreePartyID.empty() )
        return NULL;
    
	const WORD wPartyID = static_cast<WORD>(*m_FreePartyID.begin());	
	m_FreePartyID.erase(m_FreePartyID.begin());

	PARTY_MAP_ITER iter = m_Party.find(wPartyID);
	if( iter != m_Party.end() )
		return NULL;

	GLPartyAgent* pParty = m_PartyPool.construct();
	if ( NULL == pParty )
	{
		m_PartyPool.set_next_size( 32 );
		pParty = m_PartyPool.construct();
		if ( NULL == pParty )
		{
			return NULL;
		}
	}
    m_Party.insert(PARTY_MAP_VALUE( wPartyID, pParty));

    pChar->SetPartyID(wPartyID);
    pParty->setMaster(wPartyID, dwGaeaID);

	MatchingCore::getinstance()->PartyMemberChaged(dwGaeaID,pParty->GetPartyID().GetPartyID());
    return pParty;
}

const bool GLPartyManagerAgent::DeleteParty(const GLPartyID& PartyID)
{
	const DWORD dwPartyID = PartyID.GetPartyID();
	
    PARTY_MAP_ITER iter = m_Party.find( dwPartyID );
    if( iter == m_Party.end() )
        return false;

    GLPartyAgent* pParty = iter->second;
	
	//////////////////////////////////////////////////////////////////////////
	// �̴ϸ� ��Ƽ ����â�� ������ �ڽ��� �Խù��� ������ ����;
	const DWORD dwMasterGaeaID = pParty->getMaster(); 
	if( dwMasterGaeaID != GAEAID_NULL)
	{
		const GLCharAG* const pCharAG = m_pAgentServer->GetChar(dwMasterGaeaID);
		if ( pCharAG != NULL )
			m_pAgentServer->MapDeletePartyInfoHelper(pCharAG->m_szName);
	}
	//////////////////////////////////////////////////////////////////////////



	GLMSG::SNET_PARTY_DISSOLVE	NetMsg( PartyID );

	// �ʵ� ������ ��Ƽ ��ü�� �˸�;
	m_pAgentServer->SENDTOALLCHANNEL(&NetMsg);
	SendMsgToMember(PartyID, &NetMsg);
	// ������ ����ʰ� ����ȭ ������ ���� Ŭ���̾�Ʈ ������ ���� ��Ƽ Ż�� �ȵǴ� ��찡 �־�,
	// Agent->Field->Client ���� Agent->Field/Client�� �����Ѵ�;

	// ��� ���̾��ٸ�, ��� ���� ����; 
	if ( pParty->isConfronting() == true )
	{
		GLPartyAgent* pConfrontTargetParty = GetParty(pParty->m_ConfrontTargetPartyID);
		if ( pConfrontTargetParty != NULL )
			pConfrontTargetParty->resetConfront(m_pAgentServer);
		pParty->resetConfront(m_pAgentServer);

		GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld(PartyID, pParty->m_ConfrontTargetPartyID, EMCONFRONT_END_FAIL, EMCONFRONT_END_FAIL);
		m_pAgentServer->SENDTOALLCHANNEL(&NetMsgFld);
	}

	for ( GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin(); iter != pParty->m_Member.end(); ++iter )
	{
		const DWORD dwGaeaID(*iter);
		GLCharAG* const pChar = m_pAgentServer->GetChar(dwGaeaID);
		if ( pChar == NULL )
			continue;

		MatchingCore::getinstance()->PartyMemberRemoved(dwGaeaID,pParty->GetPartyID().GetPartyID());
		
		pChar->ResetPartyID();
		pChar->ResetPartyOption();

		m_pAgentServer->QuitPartyMap( pChar );
	}

	MatchingCore::getinstance()->PartyDestroyed(pParty->GetPartyID().GetPartyID());

    pParty->RunDestroyCallback(pParty);
    pParty->CallbackClear();

    m_FreePartyID.push_back( dwPartyID );

    // ��Ƽ ������ ����;
    m_PartyPool.destroy( pParty ); //SAFE_DELETE(pParty); // delete GLPARTY
    m_Party.erase( iter );

	return true;
}

const bool GLPartyManagerAgent::DeleteParty(void)
{
	for ( PARTY_MAP_ITER iterParty = m_Party.begin(); iterParty != m_Party.end(); ++iterParty )
    {
        iterParty->second->RunDestroyCallback(iterParty->second);
		m_PartyPool.destroy(iterParty->second); // SAFE_DELETE(iter->second); // delete GLPARTY
    }

	m_Party.clear();

	for ( EXPEDITION_MAP_ITER iterExpedition = m_Expedition.begin(); iterExpedition != m_Expedition.end(); ++iterExpedition )
		m_ExpeditionPool.destroy(iterExpedition->second); // SAFE_DELETE(iter->second); // delete GLPARTY
	m_Expedition.clear();

	return true;
}

const bool SendMsgToOnlyParty(GLAgentServer* const pServer, GLPartyManagerAgent* const pPartyManager, const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
	GLPartyAgent* pParty = pPartyManager->GetParty(PartyID);
	if ( pParty == NULL )
		return false;
	
	for ( GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin(); iter != pParty->m_Member.end(); ++iter )
	{
		GLCharAG* const pChar = pServer->GetChar(*iter);
		if ( pChar == NULL )
			continue;

		// ��Ƽ ���������� �޽��� ����;
		pServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
	}

	return true;
}

const bool SendMsgToOnlyParty(GLAgentServer* const pServer, GLPartyManagerAgent* const pPartyManager, const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GLPartyAgent* pParty = pPartyManager->GetParty(PartyID);
    if ( pParty == NULL )
        return false;

    for ( GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin(); iter != pParty->m_Member.end(); ++iter )
    {
        GLCharAG* const pChar = pServer->GetChar(*iter);
        if ( pChar == NULL )
            continue;

        // ��Ƽ ���������� �޽��� ����;
        pServer->SENDTOCLIENT(pChar->ClientSlot(), MsgType, PackBuffer, UseCrc);
    }

    return true;
}

const bool GLPartyManagerAgent::SendMsgToMember(const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
	if ( PartyID.isValidExpedition() == true )
	{
		GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
			if ( SendMsgToOnlyParty(m_pAgentServer, this, ExpeditionPartyID, nmg) == false )
				return false;
		}
	}
	else
		return SendMsgToOnlyParty(m_pAgentServer, this, PartyID, nmg);

	return true;
}

const bool GLPartyManagerAgent::SendMsgToMember(const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    if ( PartyID.isValidExpedition() == true )
    {
        GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
        if ( pExpedition == NULL )
            return false;

        for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
        {
            const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
            if ( SendMsgToOnlyParty(m_pAgentServer, this, ExpeditionPartyID, MsgType, PackBuffer, UseCrc) == false )
                return false;
        }
    }
    else
        return SendMsgToOnlyParty(m_pAgentServer, this, PartyID, MsgType, PackBuffer, UseCrc);

    return true;
}

const bool GLPartyManagerAgent::SendMsgToMember(DWORD dwGaeaID, const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
    GLPartyAgent*	pParty = GetParty( PartyID );
    if ( pParty == NULL )
        return false;

    GLPartyAgent::MEMBER_ITER iter     = pParty->m_Member.begin();
    GLPartyAgent::MEMBER_ITER iter_end = pParty->m_Member.end();

    for ( ; iter!=iter_end; ++iter)
    {
        GLCharAG* pChar = m_pAgentServer->GetChar((*iter));
        if ( pChar == NULL || pChar->GaeaId() != dwGaeaID )
            continue;

        // ��Ƽ ���������� �޽��� ����.
        m_pAgentServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
    }

    return true;
}

const bool GLPartyManagerAgent::SendErrorMsg(const GLPartyID& PartyID, const DWORD dwGaeaID, const EMPARTY_ERROR_TYPE emErrorType, const char* cErrorMsg) const
{
	GLCharAG* const pChar = m_pAgentServer->GetCharByGaeaID(dwGaeaID);
	if ( pChar == NULL )
		return false;

	GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetMsg(PartyID);
	NetMsg.emPartyErrorCode = emErrorType;
	m_pAgentServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsg);	

	// RanLogic/Party/GLPartyDefine.h�� ����;
//#if defined(NDEBUG)
#ifndef NDEBUG
	switch ( emErrorType )
	{
	case EMPARTY_ERROR_NOPARTY:
	case EMPARTY_ERROR_NOEXPEDITION:
	case EMPARTY_ERROR_NOMEMBER:
		{
			CString strErrorMsg;
			if ( cErrorMsg == NULL )
				strErrorMsg.Format("[Party] ErrCode : %d, E/PartyID : %d/%d, GaeaID : %d", emErrorType, PartyID.GetExpeditionID(), PartyID.GetPartyID(), dwGaeaID);
			else
				strErrorMsg.Format("[Party] ErrCode : %d, E/PartyID : %d/%d, GaeaID : %d, %s.", emErrorType, PartyID.GetExpeditionID(), PartyID.GetPartyID(), dwGaeaID, cErrorMsg);
			sc::writeLogError(strErrorMsg.GetString());
		}
		break;
	}
#endif
	return true;
}

const bool GLPartyManagerAgent::ConfrontFB(NET_MSG_GENERIC *nmg)
{
	GLMSG::SNETPC_REQ_CONFRONT_FB* pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_FB *) nmg;

	GLPartyAgent*	pPartyA = GetParty(pNetMsg->dwID);
	if ( pPartyA == NULL )
        return false;

    if ( pPartyA->m_ConfrontTargetPartyID.isValidParty() == false )
        return false;

	GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, GAEAID_NULL, pNetMsg->emFB, 0.0f);

	SendMsgToMember ( pPartyA->m_PartyID, &NetMsgFB );
	pPartyA->resetConfront(m_pAgentServer);

	GLPartyAgent* pPartyB = GetParty(pPartyA->m_ConfrontTargetPartyID);
	if ( pPartyB == NULL )
        return false;

	SendMsgToMember(pPartyB->m_PartyID, &NetMsgFB );
	pPartyB->resetConfront(m_pAgentServer);

	return true;
}

GLExpeditionAgent* GLPartyManagerAgent::NewExpedition(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	if ( GLUseFeatures::GetInstance().IsUsingExpeditionWindowRenewal() == false )
		return NULL;

	if ( m_FreeExpeditionID.empty() )
		return NULL;

	if (m_FreePartyID.size() < (MAX_PARTYGROUP + 1) )
		return NULL;	

	const DWORD dwExpeditionID = *m_FreeExpeditionID.begin();	
	m_FreeExpeditionID.erase(m_FreeExpeditionID.begin());
	

	GLPartyAgent* pParty = NULL;	
	GLPartyID ExpeditionPartyID[MAX_PARTYGROUP];
	if ( PartyID.isValidParty() == true )
	{
		pParty = GetParty(PartyID);
		if ( pParty == NULL )
			return NULL;

		if ( pParty->isMaster(dwGaeaID) == false )
			return NULL;

		ExpeditionPartyID[0] = GLPartyID(PartyID.GetPartyID(), dwExpeditionID);		
	}
	else
	{
		pParty = m_PartyPool.construct();
		if ( NULL == pParty )
		{
			m_PartyPool.set_next_size( 32 );
			pParty = m_PartyPool.construct();
			if ( NULL == pParty )
			{
				return NULL;
			}
		}
		const DWORD dwPartyID = *m_FreePartyID.begin();
		ExpeditionPartyID[0] = GLPartyID(dwPartyID, dwExpeditionID);
		m_FreePartyID.erase(m_FreePartyID.begin());
		
		pParty->setMaster(ExpeditionPartyID[0], dwGaeaID);

		m_Party.insert( PARTY_MAP_VALUE( dwPartyID, pParty ) );
	}
	pParty->m_PartyID = ExpeditionPartyID[0];

	{ // ������ ��Ƽ������ ��ƼID�� ������ID + ��ƼID�� ����;
		GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin();
		GLPartyAgent::MEMBER_ITER iter_end = pParty->m_Member.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLCharAG* pCharMem = m_pAgentServer->GetChar(*iter);
			if ( pCharMem == NULL )
				continue;

			pCharMem->SetPartyID(ExpeditionPartyID[0]);
		}
	}

	for ( DWORD _i = 1; _i < MAX_PARTYGROUP; ++_i )
	{
		GLPartyAgent* pNewParty = m_PartyPool.construct();
		if ( NULL == pNewParty )
		{
			m_PartyPool.set_next_size( 32 );
			pNewParty = m_PartyPool.construct();
			if ( NULL == pNewParty )
			{
				return NULL;
			}
		}
		const DWORD dwNewPartyID = *m_FreePartyID.begin();
		ExpeditionPartyID[_i] = GLPartyID(dwNewPartyID, dwExpeditionID);
		m_FreePartyID.erase(m_FreePartyID.begin());

		pNewParty->m_PartyID = ExpeditionPartyID[_i];
		m_Party.insert(PARTY_MAP_VALUE(dwNewPartyID, pNewParty));		
	}

	GLExpeditionAgent* pExpedition = m_ExpeditionPool.construct();		
	if ( NULL == pExpedition )
	{
		m_ExpeditionPool.set_next_size( 32 );
		pExpedition = m_ExpeditionPool.construct();
		if ( NULL == pExpedition )
		{
			return NULL;
		}
	}
	pExpedition->SetExpeditionID(dwExpeditionID);
	pExpedition->SetPartyID(ExpeditionPartyID);
	pExpedition->SetMasterGaeaID(dwGaeaID, MASTER_PARTY);
	m_Expedition.insert(EXPEDITION_MAP_VALUE(dwExpeditionID, pExpedition));

	return pExpedition;
}

const bool GLPartyManagerAgent::DeleteExpedition(const GLPartyID& PartyID)
{
	const DWORD dwExpeditionID = PartyID.GetExpeditionID();
	EXPEDITION_MAP_ITER iter = m_Expedition.find(dwExpeditionID);
	if ( iter == m_Expedition.end() )
		return false;

	GLExpeditionAgent* pExpedition = iter->second;
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		const GLPartyID& PartyID = pExpedition->GetPartyID(_i - 1);
		DeleteParty(PartyID);
	}

	m_FreeExpeditionID.push_back(dwExpeditionID);
	m_ExpeditionPool.destroy(pExpedition);
	m_Expedition.erase( iter );
	return true;
}

const bool GLPartyManagerAgent::SwapSlot(const GLPartyID& PartyID, const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo)
{
	const DWORD dwFromGaeaID = SwapSlotFrom.dwGaeaID;
	const DWORD dwToGaeaID = SwapSlotTo.dwGaeaID;
	if ( dwFromGaeaID == dwToGaeaID )
		return true;

	if ( dwFromGaeaID == GAEAID_NULL && dwToGaeaID == GAEAID_NULL )
		return false; // �� �� GAEAID_NULL�̸� �ȵ�;

	if ( (SwapSlotFrom.nPartyIndex >= MAX_PARTYGROUP) || (SwapSlotTo.nPartyIndex >= MAX_PARTYGROUP) )
		return false;

	GLCharAG* const pFromChar = dwFromGaeaID == GAEAID_NULL ? NULL : m_pAgentServer->GetCharByGaeaID(dwFromGaeaID);
	GLCharAG* const pToChar = dwToGaeaID == GAEAID_NULL ? NULL : m_pAgentServer->GetCharByGaeaID(dwToGaeaID);
	if ( dwFromGaeaID != GAEAID_NULL && pFromChar == NULL )
		return false;
	if ( dwToGaeaID != GAEAID_NULL && pToChar == NULL )
		return false;
	// GaeaID �� �� �ϳ��� GAEAID_NULL�� �� ����;	

	DWORD dwMasterGaeaID = GAEAID_NULL;
	if ( PartyID.isValidExpedition() == true )
	{
		GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		const GLPartyID& PartyIDMaster = pExpedition->GetPartyID(MASTER_PARTY);
		const GLPartyID& PartyIDFrom = pExpedition->GetPartyID(SwapSlotFrom.nPartyIndex);
		const GLPartyID& PartyIDTo = pExpedition->GetPartyID(SwapSlotTo.nPartyIndex);
		GLPartyAgent* const pPartyMaster = GetParty(PartyIDMaster);
		GLPartyAgent* const pPartyFrom = GetParty(PartyIDFrom);
		GLPartyAgent* const pPartyTo = GetParty(PartyIDTo);
		if ( (pPartyFrom == NULL) || (pPartyTo == NULL) || (pPartyMaster == NULL) )
			return false;

		GLPartyAgent::MEMBER_ITER iterFrom = find(pPartyFrom->m_Member.begin(), pPartyFrom->m_Member.end(), dwFromGaeaID);
		GLPartyAgent::MEMBER_ITER iterTo = find(pPartyTo->m_Member.begin(), pPartyTo->m_Member.end(), dwToGaeaID);

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == pPartyFrom->m_Member.end() )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == pPartyTo->m_Member.end() )
			return false;		

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			if ( pPartyTo->m_Member.size() < MAXPARTY )
			{
				pPartyTo->m_Member.push_back(dwFromGaeaID);
				pPartyFrom->m_Member.erase(iterFrom);
			}
			else
				return false;
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			if ( pPartyFrom->m_Member.size() < MAXPARTY )
			{
				pPartyFrom->m_Member.push_back(dwToGaeaID);
				pPartyTo->m_Member.erase(iterTo);
			}
			else
				return false;			
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			*iterFrom = dwToGaeaID;
			*iterTo = dwFromGaeaID;
		}

		dwMasterGaeaID = pPartyMaster->getMaster();

		if ( pFromChar != NULL )
			pFromChar->SetPartyID(PartyIDTo);
		if ( pToChar != NULL )
			pToChar->SetPartyID(PartyIDFrom);

		_refreshExpeditionMaster(PartyID);		
	}
	else if ( PartyID.isValidParty() == true )
	{
		GLPartyAgent* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
			return false;

		GLPartyAgent::MEMBER_ITER iterEnd = pParty->m_Member.end();
		GLPartyAgent::MEMBER_ITER iterFrom = find(pParty->m_Member.begin(), iterEnd, dwFromGaeaID);
		GLPartyAgent::MEMBER_ITER iterTo = find(pParty->m_Member.begin(), iterEnd, dwToGaeaID);

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == iterEnd )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == iterEnd )
			return false;

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			pParty->m_Member.erase(iterFrom);
			pParty->m_Member.push_back(dwFromGaeaID);
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			pParty->m_Member.erase(iterTo);
			pParty->m_Member.push_back(dwToGaeaID);
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			*iterFrom = dwToGaeaID;
			*iterTo = dwFromGaeaID;
		}

		dwMasterGaeaID = pParty->getMaster();
	}	

	GLMSG::SNET_EXPEDITION_SWAP_SLOT NetMsg(PartyID, dwMasterGaeaID, SwapSlotFrom, SwapSlotTo);
	m_pAgentServer->SENDTOALLCHANNEL(&NetMsg);
	
	return true;
}

const bool GLPartyManagerAgent::_refreshExpeditionMaster(const GLPartyID& PartyID)
{
	GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
	if ( pExpedition == NULL )		
		return false;

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
		const GLPartyAgent* const pParty = GetParty(ExpeditionPartyID);
		if ( pParty == NULL )
			return false;

		const DWORD dwMasterGaeaID = pParty->getMaster();
		pExpedition->SetMasterGaeaID(dwMasterGaeaID, _i - 1);
	}

	return true;
}

HRESULT GLPartyManagerAgent::FrameMoveValidExpedition(const float fTime, const float fElapsedTime, GLExpeditionAgent* const pExpedition)
{
	DWORD nMember = 0;
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		const GLPartyID& PartyID = pExpedition->GetPartyID(_i - 1);
		GLPartyAgent* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
		{ // ���� NULL�̾ �ȵ�;
			nMember = 0;
#ifndef NDEBUG
			CString strErrorMsg;
			strErrorMsg.Format(
				"[Party] FrameMove E/PartyID : %d/%d - Expedition Party NULL", 
				PartyID.GetExpeditionID(), PartyID.GetPartyID());
			sc::writeLogError(strErrorMsg.GetString());
#endif
			break;
		}			

		nMember += pParty->getNPartyMember();
	}

	if ( nMember < 2 )
	{ // ������ �������� ������ ID �� �ʿ� �ϹǷ� �����뿡 ���� ��Ƽ �� �ƹ� ID�� ȹ��;
		const GLPartyID& PartyID = pExpedition->GetPartyID(0); 		
		DeleteExpedition(PartyID);

#ifndef NDEBUG
		CString strErrorMsg;
		strErrorMsg.Format(
			"[Party] FrameMove E/PartyID : %d/%d - Expedition NMember under 1", 
			PartyID.GetExpeditionID(), PartyID.GetPartyID());
		sc::writeLogError(strErrorMsg.GetString());
#endif
	}

	return S_OK;
}
HRESULT GLPartyManagerAgent::FrameMoveValidParty(const float fTime, const float fElapsedTime, GLPartyAgent* const pParty)
{
	const GLPartyID& PartyID = pParty->GetPartyID();
	const DWORD dwOldMasterGaeaID = pParty->getMaster();
	const bool bExpedition = PartyID.isValidExpedition();

	GLCharAG* const pMasterChar = m_pAgentServer->GetChar(pParty->getMaster());

	// ���� �ڵ忡�� ó�� -> �˸�(Ŭ���̾�Ʈ)�� �Ǿ� ���� �ʾ�,
	// ó��(����) -> �˸�(Ŭ���̾�Ʈ)���� ����;
	
	if ( pMasterChar == NULL && (bExpedition == false) )
	{ // ��Ƽ/������(��)���� ��ȿ���� ���� ���;
		if ( pParty->isConfronting() == true )
		{ // ��� ���̸� ��� ���� �츮 �� ��� �ʱ�ȭ ��;
			pParty->resetConfront(m_pAgentServer);

			GLPartyAgent* const pConftParty = GetParty(pParty->m_ConfrontTargetPartyID);
			if ( pConftParty != NULL )
				pConftParty->resetConfront(m_pAgentServer);

			GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld(PartyID, pParty->m_ConfrontTargetPartyID, EMCONFRONT_END_FAIL, EMCONFRONT_END_FAIL);
			m_pAgentServer->SENDTOALLCHANNEL(&NetMsgFld);
		}
		
		if ( pParty->getNPartyMember () > 1 )
			removePartyMember(PartyID, dwOldMasterGaeaID);
		else // �����밡 �ƴ� ��� ���� ��Ƽ���� 1�� ���϶�� ��Ƽ ��ü;
			DeleteParty(PartyID);
		
#ifndef NDEBUG
		CString strErrorMsg;
		strErrorMsg.Format(
			"[Party] FrameMove E/PartyID : %d/%d - Master NULL", 
			PartyID.GetExpeditionID(), PartyID.GetPartyID());
		sc::writeLogError(strErrorMsg.GetString());
#endif
		return S_OK;
	}
	
	DWORD dwMemberIndexForLog = 0;
	for ( GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin(); iter!=pParty->m_Member.end(); )
	{
		const DWORD dwMemberGaeaID = (*iter);

		GLCharAG* const pMember = m_pAgentServer->GetChar(dwMemberGaeaID);
		if ( pMember == NULL )
		{ // ��ȿ���� ���� ����� ���;
			removePartyMember(PartyID, dwMemberGaeaID);

#ifndef NDEBUG
			CString strErrorMsg;
			strErrorMsg.Format(
				"[Party] FrameMove E/PartyID : %d/%d - Member(%d) NULL", 
				PartyID.GetExpeditionID(), PartyID.GetPartyID(), dwMemberIndexForLog);
			sc::writeLogError(strErrorMsg.GetString());
#endif
		}
		else
			iter++;		
		++dwMemberIndexForLog;
	}

	//	Note : ��Ƽ �常 ���� ��Ƽ�� ��� �ڵ� �Ҹ�;
	//
	if ( (pParty->getNPartyMember() < 2) && (bExpedition == false) )
	{
		DeleteParty(PartyID);

#ifndef NDEBUG
		CString strErrorMsg;
		strErrorMsg.Format(
			"[Party] FrameMove E/PartyID : %d/%d - NMember under 2", 
			PartyID.GetExpeditionID(), PartyID.GetPartyID());
		sc::writeLogError(strErrorMsg.GetString());
#endif
	}

	//	Note : ����� �϶� ����� ����Ǵ��� �˻�;
	//
	if ( pParty->isConfronting() == true )
	{
		pParty->m_fConfrontTimer += fElapsedTime;

		//	Note : ��ÿ� ���Ͽ��� ���;
		//
		bool bCONFT_END(false);
		bool bCONFT_SCHOOL(pParty->m_ConfrontOption.bSCHOOL);
		EMCONFRONT_END emEND_A(EMCONFRONT_END_PLOSS);
		EMCONFRONT_END emEND_B(EMCONFRONT_END_PWIN);

		WORD wSCHOOL_A = pParty->m_ConfrontOption.wMY_SCHOOL;
		WORD wSCHOOL_B = pParty->m_ConfrontOption.wTAR_SCHOOL;
		WORD wSCHOOL_WIN(wSCHOOL_B);

		if ( pParty->isLoseConfront() )
		{
			GLPartyAgent *pConftParty = GetParty(pParty->m_ConfrontTargetPartyID);

			if ( !pConftParty )
			{
				emEND_A = EMCONFRONT_END_FAIL;
				emEND_B = EMCONFRONT_END_FAIL;
			}
			else if ( pConftParty->isLoseConfront() )
			{
				emEND_A = EMCONFRONT_END_PLOSS;
				emEND_B = EMCONFRONT_END_PLOSS;
			}

			//	Note : ��Ƽ����� ������� �˸� to (�ʵ�);
			//
			GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld( pParty->m_PartyID, pParty->m_ConfrontTargetPartyID, emEND_A, emEND_B );
			m_pAgentServer->SENDTOALLCHANNEL(&NetMsgFld);

			bCONFT_END = true;
			pParty->resetConfront(m_pAgentServer);

			if ( pConftParty != NULL )
				pConftParty->resetConfront(m_pAgentServer);
		}
		else if ( pParty->m_fConfrontTimer > GLCONST_CHAR::fCONFRONT_TIME )
		{		
			//	Note : ��Ƽ����� ������� �˸� to (�ʵ�);
			//
			GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld( pParty->m_PartyID, pParty->m_ConfrontTargetPartyID, EMCONFRONT_END_PTIME, EMCONFRONT_END_PTIME );
			m_pAgentServer->SENDTOALLCHANNEL(&NetMsgFld);

			bCONFT_END = true;
			pParty->resetConfront(m_pAgentServer);

			GLPartyAgent *pConftParty = GetParty(pParty->m_ConfrontTargetPartyID);
			if ( pConftParty != NULL )
				pConftParty->resetConfront(m_pAgentServer);
		}

		//	Note : �б��� ��Ƽ ����� ���;
		//
		if ( bCONFT_END && bCONFT_SCHOOL )
		{
			const MapID& sMapID(pParty->GetMASTER_MAP(m_pAgentServer));
			const char *szMAP_NAME = m_pAgentServer->GetMapName ( sMapID.getBaseMapID() );

			std::string strText;
			if ( emEND_B==EMCONFRONT_END_PWIN )
			{
				strText = sc::string::format(
					ID2SERVERTEXT("EMCONFRONT_END_PWIN"), 
					szMAP_NAME,
					GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_A),
					GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_B),
					GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_WIN));

				//	Note : db �� �б��� ��Ƽ ��� ��� ����;
				//
				int nServerGroup = m_pAgentServer->GetMsgServer()->GetServerGroup();
				int nServerNum = m_pAgentServer->GetMsgServer()->GetServerNum();
				AddGameAdoJob(
					db::DbActionPtr(
					new db::LogPartyMatch(nServerGroup,nServerNum,wSCHOOL_B,wSCHOOL_A)));                    
			}
			else
			{
				strText = sc::string::format(
					ID2SERVERTEXT("EMCONFRONT_END_PDRAWN"),
					szMAP_NAME,
					GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_A),
					GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_B));
			}

			GLMSG::SNET_SERVER_GENERALCHAT SendData;
			if (SendData.SETTEXT(strText))
			{
				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, SendData);
				m_pAgentServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
			}
		}
	}

	return S_OK;
}
HRESULT GLPartyManagerAgent::FrameMove(const float fTime, const float fElapsedTime)
{	
	// ��Ƽ ������ �׷��� �Ʒ� ������ ���� ���� ����������;
	// ����/Ŭ���̾�Ʈ ���� �ְ�޴� �޼����� �� ó�� �� �� �ִ°�;
	// �̿� ���������� �Ź� ������ ���鼭 ��ȿ�� �˻縦 �ϴ� �� �ٶ������� ����;		
	for( EXPEDITION_MAP_ITER iterExpedition = m_Expedition.begin(); iterExpedition != m_Expedition.end(); iterExpedition++ )
	{
		GLExpeditionAgent*	pExpedition = iterExpedition->second;
		if ( pExpedition == NULL )
			continue;

		FrameMoveValidExpedition(fTime, fElapsedTime, pExpedition);	
	}

	for( PARTY_MAP_ITER iterParty = m_Party.begin(); iterParty != m_Party.end(); iterParty++ )
	{
		GLPartyAgent*	pParty = iterParty->second;
		if ( pParty == NULL )
			continue;

		FrameMoveValidParty(fTime, fElapsedTime, pParty);		
	}

	return S_OK;
}
